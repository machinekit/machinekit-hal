/********************************************************************
* Description:  rt-preempt.c
*
*               This file, 'rt-preempt.c', implements the unique
*               functions for the RT_PREEMPT thread system.
*
* Copyright (C) 2012, 2013 Michael Büsch <m AT bues DOT CH>,
*                          John Morris <john AT zultron DOT com>,
*                          Michael Haberler <license AT mah DOT priv DOT at>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
********************************************************************/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "rtapi_flavor.h"
#include "rt-preempt.h"
#include "rtapi.h"
#include "rtapi_common.h"
#include <libcgroup.h>

#include "config.h"
#include <sched.h>		// sched_get_priority_*()
#include <pthread.h>		/* pthread_* */

/***********************************************************************
*                           TASK FUNCTIONS                             *
************************************************************************/

#include <unistd.h>		// getpid(), syscall()
#include <time.h>               // clock_nanosleep()
#include <sys/resource.h>	// rusage, getrusage(), RUSAGE_SELF

#ifdef RTAPI
#include <stdlib.h>		// malloc(), sizeof(), free()
#include <string.h>		// memset()
#include <syscall.h>            // syscall(SYS_gettid);
#include <sys/prctl.h>          // prctl(PR_SET_NAME)
#include <fcntl.h>

// if this exists, and contents is '1', it's RT_PREEMPT
#define PREEMPT_RT_SYSFS "/sys/kernel/realtime"

// Access the rtpreempt_stats_t thread status object
#define FTS(ts) ((rtpreempt_stats_t *)&(ts->flavor))
// Access the rtpreempt_exception_t thread exception detail object
#define FED(detail) ((rtpreempt_exception_t)detail.flavor)

/* Lock for task_array and module_array allocations */
static pthread_key_t task_key;
static pthread_once_t task_key_once = PTHREAD_ONCE_INIT;

int posix_task_self_hook(void);

// Static file descriptor for the Power Management Quality of Service interface (PM QOS)
static int pm_qos_fd = -1;


typedef struct {
    int deleted;
    int destroyed;
    struct timespec next_time;

    /* The realtime thread. */
    pthread_t thread;
    pthread_barrier_t thread_init_barrier;
    void *stackaddr;
    pid_t tid;       // as returned by gettid(2)

    /* Statistics */
    unsigned long minfault_base;
    unsigned long majfault_base;
    unsigned int failures;
} extra_task_data_t;

extra_task_data_t extra_task_data[RTAPI_MAX_TASKS + 1];

int have_cg;  // true when libcgroup initialized successfully
#endif  /* RTAPI */


#ifdef RTAPI
int posix_module_init_hook(void)
{
    int ret;

    // Initialize libcgroup
    have_cg = !(ret = cgroup_init());
    if (have_cg)
        rtapi_print_msg(RTAPI_MSG_INFO, "libcgroup initialization succeded\n");
    else
        rtapi_print_msg(RTAPI_MSG_INFO, "libcgroup initialization failed: (%d) %s\n",
                        ret, cgroup_strerror(ret));
    return ret;
}

int rt_preempt_module_init_hook(void)
{
    int retval = -1;
    __s32 target_qos = 0;

    retval = posix_module_init_hook();
    if(retval){
        rtapi_print_msg(RTAPI_MSG_ERR, "RT PREEMPT: POSIX module init failed with error %d", retval);
        goto exit;
    }

    pm_qos_fd = open("/dev/cpu_dma_latency", O_RDWR);

    if (pm_qos_fd < 0) 
    {  
    rtapi_print_msg(RTAPI_MSG_ERR, "RT PREEMPT: Could not open the Power Management Quality of Service interface file. Error: %s", strerror(errno));  
    retval = -errno;
    }

    write(pm_qos_fd, &target_qos, sizeof(target_qos));

    rtapi_print_msg(RTAPI_MSG_INFO, "RT PREEMPT: PM_QOS activated");

    exit:
    return retval;
}
#else
int posix_module_init_hook(void) { return 0; }
int rt_preempt_module_init_hook(void){ return posix_module_init_hook(); }
#endif

#ifdef RTAPI
void rt_preempt_module_exit_hook(void)
{
    if (pm_qos_fd < 0) 
    {  
        rtapi_print_msg(RTAPI_MSG_ERR, "RT PREEMPT: The Power Management Quality of Service interface file is not open!");  
        return;
    }

    if(close(pm_qos_fd))
    {
        rtapi_print_msg(RTAPI_MSG_ERR, "RT PREEMPT: The Power Management Quality of Service interface file returned error on close. Error: %s", strerror(errno));
    }

    rtapi_print_msg(RTAPI_MSG_INFO, "RT PREEMPT: PM_QOS deactivated");
}
#else
void rt_preempt_module_exit_hook(void){}
#endif

#ifdef RTAPI

static inline int task_id(task_data *task) {
    return (int)(task - task_array);
}

/***********************************************************************
*                           RT thread statistics update                *
************************************************************************/
int posix_task_update_stats_hook(void)
{
    int task_id = posix_task_self_hook();

    // paranoia
    if ((task_id < 0) || (task_id > RTAPI_MAX_TASKS)) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"rtapi_task_update_stats_hook: BUG -"
			" task_id out of range: %d\n",
			task_id);
	return -ENOENT;
    }

    struct rusage ru;

    if (getrusage(RUSAGE_THREAD, &ru)) {
	rtapi_print_msg(RTAPI_MSG_ERR,"getrusage(): %d - %s\n",
			errno, strerror(-errno));
	return errno;
    }

    rtapi_threadstatus_t *ts = &global_data->thread_status[task_id];

    FTS(ts)->utime_usec = ru.ru_utime.tv_usec;
    FTS(ts)->utime_sec  = ru.ru_utime.tv_sec;

    FTS(ts)->stime_usec = ru.ru_stime.tv_usec;
    FTS(ts)->stime_sec  = ru.ru_stime.tv_sec;

    FTS(ts)->ru_minflt = ru.ru_minflt;
    FTS(ts)->ru_majflt = ru.ru_majflt;
    FTS(ts)->ru_nsignals = ru.ru_nsignals;
    FTS(ts)->ru_nivcsw = ru.ru_nivcsw;
    FTS(ts)->ru_nivcsw = ru.ru_nivcsw;

    ts->num_updates++;

    return task_id;
}

static void _rtapi_advance_time(struct timespec *tv, unsigned long ns,
			       unsigned long s) {
    ns += tv->tv_nsec;
    while (ns > 1000000000) {
	s++;
	ns -= 1000000000;
    }
    tv->tv_nsec = ns;
    tv->tv_sec += s;
}

static void rtapi_key_alloc() {
    pthread_key_create(&task_key, NULL);
}

static void rtapi_set_task(task_data *t) {
    pthread_once(&task_key_once, rtapi_key_alloc);
    pthread_setspecific(task_key, (void *)t);

    // set this thread's name so it can be identified in ps -L/top
    // to see the thread in top, run 'top -H' (threads mode)
    // to see the thread in ps:
    // ps H -C rtapi:0 -o 'pid tid cmd comm'
    if (prctl(PR_SET_NAME, t->name, 0, 0, 0) < 0) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"rtapi_set_task: prctl(PR_SETNAME,%s) failed: %s\n",
			t->name,strerror(errno));
    }
    // pthread_setname_np() attempts the same thing as
    // prctl(PR_SET_NAME) in a more portable way, but is
    // only available from glibc 2.12 onwards
    // pthread_t self = pthread_self();
    // pthread_setname_np(self, t->name);
}

static task_data *rtapi_this_task() {
    pthread_once(&task_key_once, rtapi_key_alloc);
    return (task_data *)pthread_getspecific(task_key);
}

int posix_task_new_hook(task_data *task, int task_id) {
    void *stackaddr;

    stackaddr = malloc(task->stacksize);
    if (!stackaddr) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"Failed to allocate realtime thread stack\n");
	return -ENOMEM;
    }
    memset(stackaddr, 0, task->stacksize);
    extra_task_data[task_id].stackaddr = stackaddr;
    extra_task_data[task_id].destroyed = 0;
    return task_id;
}

int posix_task_delete_hook(task_data *task, int task_id) {
    int err_cancel, err_join;
    void *returncode;

    /* Signal thread termination and wait for the thread to exit. */
    if (!extra_task_data[task_id].deleted) {
        rtapi_print_msg(RTAPI_MSG_DBG, "Deleting RT thread '%s'\n", task->name);
	extra_task_data[task_id].deleted = 1;

	// pthread_cancel() will get the thread out of any blocking system
	// calls listed under 'Cancellation points' in man 7 pthreads
	// read(), poll() being important ones
	err_cancel = pthread_cancel(extra_task_data[task_id].thread);
	if (err_cancel)
	    rtapi_print_msg(RTAPI_MSG_ERR,
			    "pthread_cancel() on RT thread '%s': %d %s\n",
			    task->name, err_cancel, strerror(err_cancel));
	err_join = pthread_join(extra_task_data[task_id].thread, &returncode);
	if (err_join)
	    rtapi_print_msg(RTAPI_MSG_ERR,
			    "pthread_join() on RT thread '%s': %d %s\n",
			    task->name, err_join, strerror(err_join));
    }
    /* Free the thread stack. */
    free(extra_task_data[task_id].stackaddr);
    extra_task_data[task_id].stackaddr = NULL;

    return err_cancel || err_join;
}

static int realtime_set_affinity(task_data *task) {
    cpu_set_t set;
    int err, cpu_nr, use_cpu = -1;

    pthread_getaffinity_np(extra_task_data[task_id(task)].thread,
			   sizeof(set), &set);
    if (task->cpu > -1) { // CPU set explicitly
	if (!CPU_ISSET(task->cpu, &set)) {
	    rtapi_print_msg(RTAPI_MSG_ERR,
			    "RTAPI: ERROR: realtime_set_affinity(%s): "
			    "CPU %d not available\n",
			    task->name, task->cpu);
	    return -EINVAL;
	}
	use_cpu = task->cpu;
    } else {
	// select last CPU as default
	for (cpu_nr = CPU_SETSIZE - 1; cpu_nr >= 0; cpu_nr--) {
	    if (CPU_ISSET(cpu_nr, &set)) {
		use_cpu = cpu_nr;
		break;
	    }
	}
	if (use_cpu < 0) {
	    rtapi_print_msg(RTAPI_MSG_ERR,
			    "Unable to get ID of the last CPU\n");
	    return -EINVAL;
	}
	rtapi_print_msg(RTAPI_MSG_DBG, "task %s: using default CPU %d\n",
			task->name, use_cpu);
    }
    CPU_ZERO(&set);
    CPU_SET(use_cpu, &set);

    err = pthread_setaffinity_np(extra_task_data[task_id(task)].thread,
				 sizeof(set), &set);
    if (err) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"%d %s: Failed to set CPU affinity to CPU %d (%s)\n",
			task_id(task), task->name, use_cpu, strerror(errno));
	return -EINVAL;
    }
    rtapi_print_msg(RTAPI_MSG_DBG,
		    "realtime_set_affinity(): task %s assigned to CPU %d\n",
		    task->name, use_cpu);
    return 0;
}

extern rtapi_exception_handler_t rt_exception_handler;

static int realtime_set_priority(task_data *task) {
    struct sched_param schedp;

    memset(&schedp, 0, sizeof(schedp));
    schedp.sched_priority = task->prio;
    if (sched_setscheduler(0, SCHED_FIFO, &schedp)) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"Unable to set FIFO scheduling policy: %s",
			strerror(errno));
	return 1;
    }

    return 0;
}

static void *realtime_thread(void *arg) {
    task_data *task = arg;
    int ret;
    const char *const cgcontrollers[2] = {"cpuset", NULL};

    rtapi_set_task(task);


    if (task->period < period)
	task->period = period;
    task->ratio = task->period / period;

    extra_task_data[task_id(task)].tid = (pid_t) syscall(SYS_gettid);

    rtapi_print_msg(RTAPI_MSG_INFO,
		    "RTAPI: task '%s' at %p"
		    " period = %d ratio=%d id=%d TID=%d, %s scheduling\n",
		    task->name,
		    task, task->period, task->ratio,
		    task_id(task), extra_task_data[task_id(task)].tid,
		    (task->flags & TF_NONRT) ? "non-RT" : "RT");

    if ((ret = realtime_set_affinity(task))) {
        rtapi_print_msg(
            RTAPI_MSG_ERR, "Task '%s' realtime_set_affinity() failed %d\n",
            task->name, ret);
	goto error;
    }

    // cgroup cpuset
    if (task->cgname && task->cgname[0]) {
        if (!have_cg) {
            rtapi_print_msg(
                RTAPI_MSG_ERR, "Task '%s' requested cgroup '%s', "
                "but cgroups failed to initialize\n",
                task->name, task->cgname);
            goto error;
        }

        if ((ret = cgroup_change_cgroup_path(
                 task->cgname,
                 extra_task_data[task_id(task)].tid,
                 cgcontrollers))) {
	    rtapi_print_msg(RTAPI_MSG_ERR,
                            "Error changing cpuset of task '%s' to '%s': %s\n",
                            task->name, task->cgname, cgroup_strerror(ret));
            goto error;
        }
        rtapi_print_msg(RTAPI_MSG_DBG,
                        "Moved task '%s' to cpuset '%s'",
                        task->name, task->cgname);
    }
    if (!(task->flags & TF_NONRT)) {
	if ((ret = realtime_set_priority(task))) {
            if (!(flavor_descriptor->flags && FLAVOR_IS_RT)) {
                // This requires privs - tell user how to obtain them
                rtapi_print_msg(
                    RTAPI_MSG_ERR,
                    "to get non-preemptive scheduling with POSIX threads,");
                rtapi_print_msg(
                    RTAPI_MSG_ERR,
                    "you need to run "
                    "'sudo setcap cap_sys_nice=pe libexec/rtapi_app_posix'");
                rtapi_print_msg(
                    RTAPI_MSG_ERR,
                    "you might have to install setcap "
                    "(e.g.'sudo apt-get install libcap2-bin') to do this.");
            } else {
                rtapi_print_msg(
                    RTAPI_MSG_ERR, "Task %s realtime_set_priority() failed %d",
                    task->name, ret);
                goto error;
            }
        }
    }

    /* We're done initializing. Open the barrier. */
    pthread_barrier_wait(&extra_task_data[task_id(task)].thread_init_barrier);

    clock_gettime(CLOCK_MONOTONIC, &extra_task_data[task_id(task)].next_time);
    _rtapi_advance_time(&extra_task_data[task_id(task)].next_time,
		       task->period + task->pll_correction, 0);

    posix_task_update_stats_hook(); // inital stats update

    /* The task should not pagefault at all. So record initial counts now.
     * Note that currently we _do_ receive a few pagefaults in the
     * taskcode init. This is noncritical and probably not worth
     * fixing. */
    {
	struct rusage ru;

	if (getrusage(RUSAGE_THREAD, &ru)) {
	    rtapi_print_msg(RTAPI_MSG_ERR,"getrusage(): %d - %s\n",
			    errno, strerror(-errno));
	} else {
	    rtapi_threadstatus_t *ts =
		&global_data->thread_status[task_id(task)];
	    FTS(ts)->startup_ru_nivcsw = ru.ru_nivcsw;
	    FTS(ts)->startup_ru_minflt = ru.ru_minflt;
	    FTS(ts)->startup_ru_majflt = ru.ru_majflt;
	}
    }

    /* call the task function with the task argument */
    task->taskcode(task->arg);

    rtapi_print_msg
	(RTAPI_MSG_ERR,
	 "ERROR: reached end of realtime thread for task %d\n",
	 task_id(task));
    extra_task_data[task_id(task)].deleted = 1;

    return NULL;
 error:
    /* Signal that we're dead and open the barrier. */
    rtapi_print_msg(RTAPI_MSG_ERR,"Deleting task %s\n", task->name);
    extra_task_data[task_id(task)].deleted = 1;
    pthread_barrier_wait(&extra_task_data[task_id(task)].thread_init_barrier);
    return NULL;
}

int posix_task_start_hook(task_data *task, int task_id) {
    pthread_attr_t attr;
    int retval;

#define TRY_OR_ERR(func_call, func_name)                                \
    do {                                                                \
        if ((retval = func_call)) {                                     \
            rtapi_print_msg(RTAPI_MSG_ERR, "Thread %s %s() failed %d\n", \
                            task->name, func_name, retval);             \
            return -retval;                                             \
        }                                                               \
    } while (0)

    extra_task_data[task_id].deleted = 0;

    TRY_OR_ERR(pthread_barrier_init(
                   &extra_task_data[task_id].thread_init_barrier, NULL, 2),
               "pthread_barrier_init");

    TRY_OR_ERR(pthread_attr_init(&attr), "pthread_attr_init");

    TRY_OR_ERR(pthread_attr_setstack(
                   &attr, extra_task_data[task_id].stackaddr, task->stacksize),
               "pthread_attr_setstack");

    rtapi_print_msg(RTAPI_MSG_DBG,
		    "About to pthread_create task %d\n", task_id);
    retval = pthread_create(&extra_task_data[task_id].thread,
                            &attr, realtime_thread, (void *)task);

    rtapi_print_msg(RTAPI_MSG_DBG,
                    "Created task %s (%d)\n", task->name, task_id);
    TRY_OR_ERR(pthread_attr_destroy(&attr), "pthread_attr_destroy");
    if (retval) {
        rtapi_print_msg(RTAPI_MSG_ERR, "Thread %s pthread_create() failed; "
                        "cleaning up\n", task->name);
	TRY_OR_ERR(pthread_barrier_destroy
                   (&extra_task_data[task_id].thread_init_barrier),
               "pthread_barrier_destroy");
	return -retval;
    }
    /* Wait for the thread to do basic initialization. */
    pthread_barrier_wait(&extra_task_data[task_id].thread_init_barrier);
    TRY_OR_ERR(pthread_barrier_destroy(
                   &extra_task_data[task_id].thread_init_barrier),
               "pthread_barrier_destroy");
    if (extra_task_data[task_id].deleted) {
	/* The thread died in the init phase. */
	rtapi_print_msg(RTAPI_MSG_ERR,
			"Realtime thread initialization failed\n");
	return -ENOMEM;
    }
    rtapi_print_msg(RTAPI_MSG_DBG,
                    "Task %s (%d) finished basic init\n", task->name, task_id);

    return 0;
}

int posix_task_stop_hook(task_data *task, int task_id) {
    extra_task_data[task_id].destroyed = 1;
    return 0;
}

int posix_wait_hook(const int flags) {
    struct timespec ts;
    task_data *task = rtapi_this_task();

    if (extra_task_data[task_id(task)].deleted)
	pthread_exit(0);

    if (flags & TF_NOWAIT)
	return 0;

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
		    &extra_task_data[task_id(task)].next_time, NULL);
    _rtapi_advance_time(&extra_task_data[task_id(task)].next_time,
		       task->period + task->pll_correction, 0);
    clock_gettime(CLOCK_MONOTONIC, &ts);
    if (ts.tv_sec > extra_task_data[task_id(task)].next_time.tv_sec
	|| (ts.tv_sec == extra_task_data[task_id(task)].next_time.tv_sec
	    && ts.tv_nsec > extra_task_data[task_id(task)].next_time.tv_nsec)) {

	// timing went wrong:

	// update stats counters in thread status
	posix_task_update_stats_hook();

	rtapi_threadstatus_t *ts =
	    &global_data->thread_status[task_id(task)];

	FTS(ts)->wait_errors++;

        if (!(flavor_descriptor->flags && FLAVOR_IS_RT)) {
            rtapi_exception_detail_t detail = {0};
            detail.task_id = task_id(task);

            if (rt_exception_handler)
                rt_exception_handler(RTP_DEADLINE_MISSED, &detail, ts);
        }
    }
    return 0;
}

void posix_task_delay_hook(long int nsec)
{
    struct timespec t;

    t.tv_nsec = nsec;
    t.tv_sec = 0;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &t, NULL);
}


int posix_task_self_hook(void) {
    int n;

    /* ask OS for pointer to its data for the current pthread */
    pthread_t self = pthread_self();

    /* find matching entry in task array */
    n = 1;
    while (n <= RTAPI_MAX_TASKS) {
	if (extra_task_data[n].thread == self) {
	    /* found a match */
	    return n;
	}
	n++;
    }
    return -EINVAL;
}

long long posix_task_pll_get_reference_hook(void) {
    int task_id = posix_task_self_hook();
    if (task_id < 0) return 0;
    return extra_task_data[task_id].next_time.tv_sec * 1000000000LL
        + extra_task_data[task_id].next_time.tv_nsec;
}

int posix_task_pll_set_correction_hook(long value) {
    int task_id = posix_task_self_hook();
    task_data *task = &task_array[task_id];
    if (task <= 0) return -EINVAL;
    if (value > task->pll_correction_limit)
        value = task->pll_correction_limit;
    if (value < -(task->pll_correction_limit))
        value = -(task->pll_correction_limit);
    task->pll_correction = value;
    /* rtapi_print_msg(RTAPI_MSG_DBG, */
    /*     	    "Task %d pll correction set to %ld\n", */
    /*                 task_id, value); */
    return 0;
}

int kernel_is_rtpreempt()
{
    FILE *fd;
    int retval = 0;

    if ((fd = fopen(PREEMPT_RT_SYSFS,"r")) != NULL) {
	int flag;
	retval = ((fscanf(fd, "%d", &flag) == 1) && (flag));
	fclose(fd);
    }
    return retval;
}

int posix_can_run_flavor()
{
    return 1;
}

int rtpreempt_can_run_flavor()
{
    return kernel_is_rtpreempt();
}


void rtpreempt_print_thread_stats(int task_id)
{
    rtapi_threadstatus_t *ts =
	&global_data->thread_status[task_id];

    rtapi_print("    wait_errors=%d\t",
                FTS(ts)->wait_errors);
    rtapi_print("usercpu=%lduS\t",
                FTS(ts)->utime_sec * 1000000 +
                FTS(ts)->utime_usec);
    rtapi_print("syscpu=%lduS\t",
                FTS(ts)->stime_sec * 1000000 +
                FTS(ts)->stime_usec);
    rtapi_print("nsigs=%ld\n",
                FTS(ts)->ru_nsignals);
    rtapi_print("    ivcsw=%ld\t",
                FTS(ts)->ru_nivcsw -
                FTS(ts)->startup_ru_nivcsw);
    rtapi_print("    minflt=%ld\t",
                FTS(ts)->ru_minflt -
                FTS(ts)->startup_ru_minflt);
    rtapi_print("    majflt=%ld\n",
                FTS(ts)->ru_majflt -
                FTS(ts)->startup_ru_majflt);
    rtapi_print("\n");
}


void rtpreempt_exception_handler_hook(int type,
                                      rtapi_exception_detail_t *detail,
                                      int level)
{
    rtapi_threadstatus_t *ts = &global_data->thread_status[detail->task_id];
    switch ((rtpreempt_exception_id_t)type) {
        // Timing violations
	case RTP_DEADLINE_MISSED:
	     rtapi_print_msg(level,
			    "%d: Unexpected realtime delay on RT thread %d ",
			     type, detail->task_id);
	    rtpreempt_print_thread_stats(detail->task_id);
	    break;

	default:
	    rtapi_print_msg(level,
			    "%d: unspecified exception detail=%p ts=%p",
			    type, detail, ts);

    }
}

flavor_descriptor_t flavor_rt_prempt_descriptor = {
    .name = "rt-preempt",
    .flavor_id = RTAPI_FLAVOR_RT_PREEMPT_ID,
    .flags = FLAVOR_DOES_IO + FLAVOR_IS_RT,
    .can_run_flavor = rtpreempt_can_run_flavor,
    .exception_handler_hook = rtpreempt_exception_handler_hook,
    .module_init_hook = rt_preempt_module_init_hook,
    .module_exit_hook = rt_preempt_module_exit_hook,
    .task_update_stats_hook = NULL,
    .task_print_thread_stats_hook = rtpreempt_print_thread_stats,
    .task_new_hook = posix_task_new_hook,
    .task_delete_hook = posix_task_delete_hook,
    .task_start_hook = posix_task_start_hook,
    .task_stop_hook = posix_task_stop_hook,
    .task_pause_hook = NULL,
    .task_wait_hook = posix_wait_hook,
    .task_resume_hook = NULL,
    .task_delay_hook = posix_task_delay_hook,
    .get_time_hook = NULL,
    .get_clocks_hook = NULL,
    .task_self_hook = posix_task_self_hook,
    .task_pll_get_reference_hook = posix_task_pll_get_reference_hook,
    .task_pll_set_correction_hook = posix_task_pll_set_correction_hook
};

flavor_descriptor_t flavor_posix_descriptor = {
    .name = "posix",
    .flavor_id = RTAPI_FLAVOR_POSIX_ID,
    .flags = 0,
    .can_run_flavor = posix_can_run_flavor,
    .exception_handler_hook = NULL,
    .module_init_hook = posix_module_init_hook,
    .module_exit_hook = NULL,
    .task_update_stats_hook = NULL,
    .task_print_thread_stats_hook = rtpreempt_print_thread_stats,
    .task_new_hook = posix_task_new_hook,
    .task_delete_hook = posix_task_delete_hook,
    .task_start_hook = posix_task_start_hook,
    .task_stop_hook = posix_task_stop_hook,
    .task_pause_hook = NULL,
    .task_wait_hook = posix_wait_hook,
    .task_resume_hook = NULL,
    .task_delay_hook = posix_task_delay_hook,
    .get_time_hook = NULL,
    .get_clocks_hook = NULL,
    .task_self_hook = posix_task_self_hook,
    .task_pll_get_reference_hook = posix_task_pll_get_reference_hook,
    .task_pll_set_correction_hook = posix_task_pll_set_correction_hook
};

#endif /* RTAPI */

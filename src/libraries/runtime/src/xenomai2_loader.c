/********************************************************************
* Description:  xenomai2_loader.c
*
* Alongside `xenomai2.c`, implement the unique functions for the Xenomai 2 userland
* thread system.  The Xenomai 2 DLL .so libraries will exit the program if
* `/dev/rtheap` doesn't exist, so those libraries can't be linked directly
* against `rtapi.so` and still work in environments without Xenomai 2. The
* `xenomai2.c` file checks to see whether Xenomai 2 is available, and if so loads
* this code, built as a plugin linked against Xenomai 2 libraries.
*
* Copyright (C) 2012 - 2013 John Morris <john AT zultron DOT com>
*                           Michael Haberler <license AT mah DOT priv DOT at>
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
********************************************************************/

#include "xenomai2.h"
#include "runtime/config.h"

#include <native/task.h>                // RT_TASK, rt_task_*()
#include <native/timer.h>               // rt_timer_*()
#include <stdlib.h>                     // abort()

#ifdef USE_SIGXCPU
#include <signal.h>			// sigaction/SIGXCPU handling
#endif

// Access the xenomai2_stats_t thread status object
#define FTS(ts) ((xenomai2_stats_t *)&(ts->flavor))
// Access the xenomai2_exception_t thread exception detail object
#define FTED(detail) ((xenomai2_exception_t *)&(detail.flavor))

/*  RTAPI task functions  */
RT_TASK ostask_array[RTAPI_MAX_TASKS + 1];

// this is needed due to the weirdness of the rt_task_self return value -
// it does _not_ match the address of the RT_TASK structure it was
// created with
RT_TASK *ostask_self[RTAPI_MAX_TASKS + 1];

int xenomai2_task_self_hook(void);


/***********************************************************************
* RT thread statistics update
*/
int xenomai2_task_update_stats_hook(void)
{
    int task_id = xenomai2_task_self_hook();

    // paranoia
    if ((task_id < 0) || (task_id > RTAPI_MAX_TASKS)) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"rtapi_task_update_stats_hook: BUG -"
			" task_id out of range: %d\n",
			task_id);
	return -ENOENT;
    }

    RT_TASK_INFO rtinfo;
    int retval = rt_task_inquire(ostask_self[task_id], &rtinfo);
    if (retval) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"rt_task_inquire() failed: %d %s\n",
			retval, strerror(-retval));
	return -ESRCH;
    }

    rtapi_threadstatus_t *ts = &global_data->thread_status[task_id];

    FTS(ts)->modeswitches = rtinfo.modeswitches;
    FTS(ts)->ctxswitches = rtinfo.ctxswitches;
    FTS(ts)->pagefaults = rtinfo.pagefaults;
    FTS(ts)->exectime = rtinfo.exectime;
    FTS(ts)->status = rtinfo.status;

    ts->num_updates++;

    return task_id;
}

/***********************************************************************
* Xenomai 2 Domain switching handling
*
* if an RT thread does something silly, like a system call
* (e.g. write(2) caused by a printf()), the Xenomai 2 scheduler will
* switch this thread from RT to Linux scheduling, and post the SIGXCPU
* signal.
*
* This is typically a sign of a coding error, and pretty bad - it
* should cause an estop.
*
* Update the Xenomai 2 thread statistics, and funnel through exception
* handler mechanism.
*
* The important value in thread status is 'modeswitches', which should
* remain zero.
*
* Update: this does not work reliably and is not strictly necessary
* since thread stats will report domain switches anyway without a
* signal handler. Better stay with synchronous reporting.
*/

extern rtapi_exception_handler_t rt_exception_handler;

#ifdef USE_SIGXCPU
// trampoline to current handler
static void signal_handler(int sig, siginfo_t *si, void *uctx)
{
    int task_id = xenomai2_task_update_stats_hook();
    if (task_id > -1) {
	rtapi_threadstatus_t *ts = &global_data->thread_status[task_id];

	rtapi_exception_detail_t detail = {0};
	detail.task_id = task_id;

	if (rt_exception_handler)
	    rt_exception_handler(XU_SIGXCPU, &detail, ts);
    } else {
	rtapi_print_msg(RTAPI_MSG_ERR, "BUG: SIGXCPU - cant identify task\n");
	if (rt_exception_handler)
	    rt_exception_handler(XU_SIGXCPU_BUG, NULL, NULL);
    }
}
#endif

/***********************************************************************
* rtapi_task.c
*/

int xenomai2_task_delete_hook(task_data *task, int task_id) {
    int retval = 0;

    if ((retval = rt_task_delete( &ostask_array[task_id] )) < 0) {
	rtapi_print_msg(RTAPI_MSG_ERR,"ERROR: rt_task_delete(%d) failed: %d %s\n",
			task_id, retval, strerror(-retval));
	return retval;
    }
    // actually wait for the thread to exit
    if ((retval = rt_task_join( &ostask_array[task_id] )) < 0)
	rtapi_print_msg(RTAPI_MSG_ERR,"ERROR: rt_task_join(%d) failed: %d %s\n",
			task_id, retval, strerror(-retval));
    return retval;
}

void _rtapi_task_wrapper(void * task_id_hack) {
    int ret;
    int task_id = (int)(long) task_id_hack; // ugly, but I ain't gonna fix it
    task_data *task = &task_array[task_id];

    /* use the argument to point to the task data */
    if (task->period < period) task->period = period;
    task->ratio = task->period / period;
    rtapi_print_msg(RTAPI_MSG_DBG,
		    "rtapi_task_wrapper: task %p '%s' period=%d "
		    "prio=%d ratio=%d\n",
		    task, task->name, task->ratio * period,
		    task->prio, task->ratio);

    ostask_self[task_id]  = rt_task_self();

    // starting the thread with the TF_NOWAIT flag implies it is not periodic
    // https://github.com/machinekit/machinekit/issues/237#issuecomment-126590880
    // NB this assumes rtapi_wait() is NOT called on this thread any more
    // see thread_task() where this is handled for now

    if (!(task->flags & TF_NOWAIT)) {
	if ((ret = rt_task_set_periodic(NULL, TM_NOW, task->ratio * period)) < 0) {
	    rtapi_print_msg(RTAPI_MSG_ERR,
			    "ERROR: rt_task_set_periodic(%d,%s) failed %d %s\n",
			    task_id, task->name, ret, strerror(-ret));
	    // really nothing one can realistically do here,
	    // so just enable forensics
	    abort();
	}
    }
#ifdef USE_SIGXCPU
    // required to enable delivery of the SIGXCPU signal
    rt_task_set_mode(0, T_WARNSW, NULL);
#endif

    xenomai2_task_update_stats_hook(); // initial recording

#ifdef TRIGGER_SIGXCPU_ONCE
    // enable this for testing only:
    // this should cause a domain switch due to the write()
    // system call and hence a single SIGXCPU posted,
    // causing an XU_SIGXCPU exception
    // verifies rtapi_task_update_status_hook() works properly
    // and thread_status counters are updated
    printf("--- once in task_wrapper\n");
#endif

    /* call the task function with the task argument */
    (task->taskcode) (task->arg);

    /* if the task ever returns, we record that fact */
    task->state = ENDED;
    rtapi_print_msg(RTAPI_MSG_ERR,
		    "ERROR: reached end of wrapper for task %d '%s'\n",
		    task_id, task->name);
}


int xenomai2_task_start_hook(task_data *task, int task_id) {
    int which_cpu = 0;
    int uses_fpu = 0;
    int retval;

    // seems to work for me
    // not sure T_CPU(n) is possible - see:
    // http://www.xenomai.org/pipermail/xenomai-help/2010-09/msg00081.html

    if (task->cpu > -1)  // explicitly set by threads, motmod
	which_cpu = T_CPU(task->cpu);

    // http://www.xenomai.org/documentation/trunk/html/api/group__task.html#ga03387550693c21d0223f739570ccd992
    // Passing T_FPU|T_CPU(1) in the mode parameter thus creates a
    // task with FPU support enabled and which will be affine to CPU #1
    // the task will start out dormant; execution begins with rt_task_start()

    // since this is a usermode RT task, it will be FP anyway
    if (task->uses_fp)
	uses_fpu = T_FPU;

    // optionally start as relaxed thread - meaning defacto a standard Linux thread
    // without RT features
    // see https://xenomai.org/pipermail/xenomai/2015-July/034745.html and
    // https://github.com/machinekit/machinekit/issues/237#issuecomment-126590880

    int prio = (task->flags & TF_NONRT) ? 0 :task->prio;

    if ((retval = rt_task_create (&ostask_array[task_id], task->name,
				  task->stacksize, prio,
				  uses_fpu | which_cpu | T_JOINABLE)
	 ) != 0) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"rt_task_create failed: %d %s\n",
			retval, strerror(-retval));
	return -ENOMEM;
    }

    if ((retval = rt_task_start( &ostask_array[task_id],
				 _rtapi_task_wrapper, (void *)(long)task_id))) {
	rtapi_print_msg(RTAPI_MSG_INFO,
			"rt_task_start failed: %d %s\n",
			retval, strerror(-retval));
	return -ENOMEM;
    }
    return 0;
}

int xenomai2_task_stop_hook(task_data *task, int task_id) {
    int retval;

    if ((retval = rt_task_delete( &ostask_array[task_id] )) < 0) {
	rtapi_print_msg(RTAPI_MSG_ERR,"rt_task_delete() failed: %d %s\n",
			retval, strerror(-retval));
	return retval;
    }

    return 0;
}

int xenomai2_task_pause_hook(task_data *task, int task_id) {
    return rt_task_suspend( &ostask_array[task_id] );
}

int xenomai2_task_resume_hook(task_data *task, int task_id) {
    return rt_task_resume( &ostask_array[task_id] );
}

int xenomai2_wait_hook(const int flags) {

    if (flags & TF_NOWAIT)
	return 0;

    unsigned long overruns = 0;
    int result =  rt_task_wait_period(&overruns);

    if (result) {
	// something went wrong:

	// update stats counters in thread status
	int task_id = xenomai2_task_update_stats_hook();


	// paranoid, but you never know; this index off and
	// things will go haywire really fast
	if ((task_id < 0) || (task_id > RTAPI_MAX_TASKS)) {
	    rtapi_print_msg(RTAPI_MSG_ERR,
			    "rt_task_wait_hook: BUG - task_id out of range: %d\n",
			    task_id);
	    // maybe should call a BUG exception here
	    return 0;
	}

	// inquire, fill in
	// exception descriptor, and call exception handler

	rtapi_exception_detail_t detail = {0};
	rtapi_threadstatus_t *ts = &global_data->thread_status[task_id];
	xenomai2_exception_id_t type;

	// exception descriptor
	detail.task_id = task_id;
	detail.error_code = result;

	switch (result) {

	case -ETIMEDOUT:
	    // release point was missed

	    FTED(detail)->overruns = overruns;

	    // update thread status in global_data
	    FTS(ts)->wait_errors++;
	    FTS(ts)->total_overruns += overruns;
	    type = XU_ETIMEDOUT;
	    break;

	case -EWOULDBLOCK:
	    // returned if rt_task_set_periodic() has not previously
	    // been called for the calling task. This is clearly
	    // a Xenomai 2 API usage error.
	    ts->api_errors++;
	    type = XU_EWOULDBLOCK;
	    break;

	case -EINTR:
	    // returned if rt_task_unblock() has been called for
	    // the waiting task before the next periodic release
	    // point has been reached. In this case, the overrun
	    // counter is reset too.
	    // a Xenomai 2 API usage error.
	    ts->api_errors++;
	    type = XU_EINTR;
	    break;

	case -EPERM:
	    // returned if this service was called from a
	    // context which cannot sleep (e.g. interrupt,
	    // non-realtime or scheduler locked).
	    // a Xenomai 2 API usage error.
	    ts->api_errors++;
	    type = XU_EPERM;
	    break;

	default:
	    // the above should handle all possible returns
	    // as per manual, so at least leave a scent
	    // (or what Jeff calls a 'canary value')
	    ts->other_errors++;
	    type = XU_UNDOCUMENTED;
	}
	if (rt_exception_handler)
	    rt_exception_handler(type, &detail, ts);
    }  // else: ok - no overruns;
    return 0;
}

int xenomai2_task_self_hook(void) {
    RT_TASK *ptr;
    int n;

    /* ask OS for pointer to its data for the current task */
    ptr = rt_task_self();

    if (ptr == NULL) {
	/* called from outside a task? */
	return -EINVAL;
    }
    /* find matching entry in task array */
    n = 1;
    while (n <= RTAPI_MAX_TASKS) {
	if (ostask_self[n] == ptr) {
	    /* found a match */
	    return n;
	}
	n++;
    }
    return -EINVAL;
}


/***********************************************************************
* rtapi_time.c
*/

void xenomai2_task_delay_hook(long int nsec)
{
    long long int release = rt_timer_read() + nsec;
    while (rt_timer_read() < release);
}

long long int xenomai2_get_time_hook(void) {
    /* The value returned will represent a count of jiffies if the
       native skin is bound to a periodic time base (see
       CONFIG_XENO_OPT_NATIVE_PERIOD), or nanoseconds otherwise.  */
    return rt_timer_read();
}

/* This returns a result in clocks instead of nS, and needs to be used
   with care around CPUs that change the clock speed to save power and
   other disgusting, non-realtime oriented behavior.  But at least it
   doesn't take a week every time you call it.
*/
long long int xenomai2_get_clocks_hook(void) {
    return rt_timer_read();
}


void xenomai2_print_thread_stats(int task_id)
{
    rtapi_threadstatus_t *ts =
	&global_data->thread_status[task_id];

    // generic statistics counters
    rtapi_print("    updates=%d\t", ts->num_updates);
    if (ts->num_updates) {
	rtapi_print("api_err=%d\t", ts->api_errors);
	rtapi_print("other_err=%d\n", ts->api_errors);
    }

    rtapi_print("    wait_errors=%d\t", FTS(ts)->wait_errors);
    rtapi_print("overruns=%d\t", FTS(ts)->total_overruns);
    rtapi_print("modeswitches=%d\t", FTS(ts)->modeswitches);
    rtapi_print("contextswitches=%d\n", FTS(ts)->ctxswitches);
    rtapi_print("    pagefaults=%d\t", FTS(ts)->pagefaults);
    rtapi_print("exectime=%llduS\t", FTS(ts)->exectime/1000);
    rtapi_print("status=0x%x\n", FTS(ts)->status);
    rtapi_print("\n");
}


void xenomai2_exception_handler_hook(int type,
                                    rtapi_exception_detail_t *detail,
                                    int level)
{
    rtapi_threadstatus_t *ts = &global_data->thread_status[detail->task_id];
    switch ((xenomai2_exception_id_t)type) {
        // Timing violations
	case XU_ETIMEDOUT:
            rtapi_print_msg(level,
			    "%d: Unexpected realtime delay on RT thread %d ",
                            type, detail->task_id);
            xenomai2_print_thread_stats(detail->task_id);
	    break;
	    // Xenomai 2 User errors
	case XU_SIGXCPU:	// Xenomai 2 Domain switch
	    rtapi_print_msg(level,
			    "%d: Xenomai 2 Domain switch for thread %d",
			    type, detail->task_id);
	    xenomai2_print_thread_stats(detail->task_id);
	    break;
	case XU_EWOULDBLOCK:
	    rtapi_print_msg(level,
			    "API usage bug: rt_task_set_periodic() not called: "
			    "thread %d - errno %d",
			    detail->task_id,
			    detail->error_code);
	    break;

	case XU_EINTR:
	    rtapi_print_msg(level,
			    "API usage bug: rt_task_unblock() called before"
			    " release point: thread %d -errno %d",
			    detail->task_id,
			    detail->error_code);
	    break;

	case XU_EPERM:
	    rtapi_print_msg(level,
			    "API usage bug: cannot call service from current"
			    " context: thread %d - errno %d",
			    detail->task_id,
			    detail->error_code);
	    break;

	case XU_UNDOCUMENTED:
	    rtapi_print_msg(level,
			    "%d: unspecified Xenomai 2 error: thread %d - errno %d",
			    type,
			    detail->task_id,
			    detail->error_code);
	    break;

	default:
	    rtapi_print_msg(level,
			    "%d: unspecified exception detail=%p ts=%p",
			    type, detail, ts);
   }
}

void xenomai2_descriptor_updater(flavor_descriptor_ptr fd)
{
    // Update flavor descriptor with functions from this plugin
    fd->task_update_stats_hook = xenomai2_task_update_stats_hook;
    fd->exception_handler_hook = xenomai2_exception_handler_hook;
    fd->task_print_thread_stats_hook = xenomai2_print_thread_stats;
    fd->task_delete_hook = xenomai2_task_delete_hook;
    fd->task_start_hook = xenomai2_task_start_hook;
    fd->task_stop_hook = xenomai2_task_stop_hook;
    fd->task_pause_hook = xenomai2_task_pause_hook;
    fd->task_wait_hook = xenomai2_wait_hook;
    fd->task_resume_hook = xenomai2_task_resume_hook;
    fd->task_delay_hook = xenomai2_task_delay_hook;
    fd->get_time_hook = xenomai2_get_time_hook;
    fd->get_clocks_hook = xenomai2_get_clocks_hook;
    fd->task_self_hook = xenomai2_task_self_hook;
}

// Not really needed except to keep the build system from barfing
EXPORT_SYMBOL(xenomai2_descriptor_updater);

/********************************************************************
* Description:  rtapi_task.c
*               This file, 'rtapi_task.c', implements the task-
*               related functions for realtime modules.  See rtapi.h
*               for more info.
*
*		The functions here can be customized by defining
*		'hook' functions in a separate source file for the
*		thread system, and define a macro indicating that the
*		definition exists.  The functions below that accept
*		hooks are preceded by a prototype for the hook
*		function.
*
*     Copyright 2006-2013 Various Authors
*
*     This program is free software; you can redistribute it and/or modify
*     it under the terms of the GNU General Public License as published by
*     the Free Software Foundation; either version 2 of the License, or
*     (at your option) any later version.
*
*     This program is distributed in the hope that it will be useful,
*     but WITHOUT ANY WARRANTY; without even the implied warranty of
*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*     GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public License
*     along with this program; if not, write to the Free Software
*     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
********************************************************************/

#include "config.h"		// build configuration
#include "rtapi.h"		// these functions
#include "rtapi_common.h"	// RTAPI macros and decls
#include "rtapi_flavor.h"       // flavor_*

/*
  These functions are completely different between each userland
  thread system, so these are defined in rtapi_module.c for kernel
  threads systems and $THREADS.c for the userland thread systems

  int rtapi_init(const char *modname)
  int rtapi_exit(int id)
*/


/* priority functions */

int rtapi_prio_highest(void) {
    return PRIO_HIGHEST;
}

int rtapi_prio_lowest(void) {
    return PRIO_LOWEST;
}

int rtapi_prio_next_higher(int prio) {
    /* next higher priority for arg */
    prio++;

    /* return a valid priority for out of range arg */
    if (prio > rtapi_prio_highest())
	return rtapi_prio_highest();
    if (prio < rtapi_prio_lowest())
	return rtapi_prio_lowest();

    return prio;
}

int rtapi_prio_next_lower(int prio) {
    /* next lower priority for arg */
    prio--;

    /* return a valid priority for out of range arg */
    if (prio > rtapi_prio_highest())
	return rtapi_prio_highest();
    if (prio < rtapi_prio_lowest())
	return rtapi_prio_lowest();

    return prio;
}


#ifdef RTAPI  /* below functions not available to user programs */

#define IS_TASK_ID_VALID(task_id)                   \
    if (task_id <= 0 || task_id >= RTAPI_MAX_TASKS) \
    {                                               \
        return -EINVAL;                             \
    }

/* task setup and teardown functions */
int rtapi_task_new(const rtapi_task_args_t *args) {
    int task_id;
    int __attribute__((__unused__)) retval = 0;
    task_data *task;

    /* get the mutex */
    rtapi_mutex_get(&(rtapi_data->mutex));

    /* find an empty entry in the task array */
    task_id = 1; // tasks start at one!
    // go through task_array until an empty task slot is found
    while ((task_id < RTAPI_MAX_TASKS) &&
	   (task_array[task_id].magic == TASK_MAGIC))
	task_id++;
    // if task_array is full, release lock and return error
    if (task_id == RTAPI_MAX_TASKS) {
	rtapi_mutex_give(&(rtapi_data->mutex));
	return -ENOMEM;
    }
    task = &(task_array[task_id]);

    // if requested priority is invalid, release lock and return error

    if (args->prio < rtapi_prio_lowest() ||
	args->prio > rtapi_prio_highest()) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"New task  %d  '%s:%d': invalid priority %d "
			"(highest=%d lowest=%d)\n",
			task_id, args->name, rtapi_instance, args->prio,
			rtapi_prio_highest(),
			rtapi_prio_lowest());
	rtapi_mutex_give(&(rtapi_data->mutex));
	return -EINVAL;
    }

    /* Allow RT threads to use nowait. Required for external timing.
    if ((args->flags & (TF_NOWAIT|TF_NONRT)) == TF_NOWAIT) {
	rtapi_print_msg(RTAPI_MSG_ERR,"task '%s' : nowait flag invalid for RT thread\n",
			args->name);
	rtapi_mutex_give(&(rtapi_data->mutex));
	return -EINVAL;
    }
    */

    // task slot found; reserve it and release lock
    rtapi_print_msg(
        RTAPI_MSG_DBG,
        "Creating new task %d  '%s:%d': "
        "req prio %d (highest=%d lowest=%d) stack=%lu fp=%d flags=%d "
        "cgname=%s\n",
        task_id, args->name, rtapi_instance, args->prio,
        rtapi_prio_highest(),
        rtapi_prio_lowest(),
        args->stacksize, args->uses_fp, args->flags, args->cgname);
    task->magic = TASK_MAGIC;

    /* fill out task structure */
    task->owner = args->owner;
    task->arg = args->arg;
    task->stacksize = (args->stacksize < MIN_STACKSIZE) ? MIN_STACKSIZE : args->stacksize;
    task->taskcode = args->taskcode;
    task->prio = args->prio;
    task->flags = args->flags;
    task->uses_fp = args->uses_fp;
    task->cpu = args->cpu_id > -1 ? args->cpu_id : rtapi_data->rt_cpu;
    strncpy(task->cgname, args->cgname, RTAPI_LINELEN);

    rtapi_print_msg(RTAPI_MSG_DBG, "Task CPU:  %d\n", task->cpu);

    rtapi_snprintf(task->name, sizeof(task->name),
	     "%s:%d", args->name, rtapi_instance);
    task->name[sizeof(task->name) - 1] = '\0';

    /* userland threads: flavor_task_new_hook() should perform any
       thread system-specific tasks, and return task_id or an error code back to
       the caller (how do we know the diff between an error and a
       task_id???).  */
    task->state = USERLAND;	// userland threads don't track this

    retval = flavor_task_new_hook(NULL, task, task_id);
    if (retval == -ENOSYS) // Unimplemented
        retval = task_id;

    rtapi_data->task_count++;

    rtapi_mutex_give(&(rtapi_data->mutex));

    /* announce the birth of a brand new baby task */
    rtapi_print_msg(RTAPI_MSG_DBG,
	"RTAPI: task %02d installed by module %02d, priority %d, code: %p\n",
	task_id, task->owner, task->prio, args->taskcode);

    return task_id;
}


int rtapi_task_delete(int task_id) {
    task_data *task;
    int retval = 0;

    IS_TASK_ID_VALID(task_id)

    task = &(task_array[task_id]);
    /* validate task handle */
    if (task->magic != TASK_MAGIC)	// nothing to delete
	return -EINVAL;

    if (task->state != DELETE_LOCKED)	// we don't already hold mutex
	rtapi_mutex_get(&(rtapi_data->mutex));

    flavor_task_delete_hook(NULL, task,task_id);

    if (task->state != DELETE_LOCKED)	// we don't already hold mutex
	rtapi_mutex_give(&(rtapi_data->mutex));
    task->state = EMPTY;
    task->magic = 0;

    rtapi_print_msg(RTAPI_MSG_DBG, "rt_task_delete %d \"%s\"\n", task_id,
		    task->name );

    return retval;
}


/* all threads systems must define this hook */
int rtapi_task_start(int task_id, unsigned long int period_nsec) {
    task_data *task;

    IS_TASK_ID_VALID(task_id)

    task = &task_array[task_id];

    /* validate task handle */
    if (task->magic != TASK_MAGIC)
	return -EINVAL;

    if (period_nsec < period) period_nsec = period;
    task->period = period_nsec;
    task->ratio = period_nsec / period;

    // limit PLL correction values to +/-1% of cycle time
    task->pll_correction_limit = period_nsec / 100;
    task->pll_correction = 0;

    rtapi_print_msg(RTAPI_MSG_DBG,
		    "rtapi_task_start:  starting task %d '%s'\n",
		    task_id, task->name);
    rtapi_print_msg(RTAPI_MSG_DBG, "RTAPI: period_nsec: %ld\n", period_nsec);

    return flavor_task_start_hook(NULL, task,task_id);
}

int rtapi_task_stop(int task_id) {
    task_data *task;

    IS_TASK_ID_VALID(task_id)

    task = &task_array[task_id];

    /* validate task handle */
    if (task->magic != TASK_MAGIC)
	return -EINVAL;

    flavor_task_stop_hook(NULL, task,task_id);

    return 0;
}

int rtapi_task_pause(int task_id) {
    task_data *task;

    IS_TASK_ID_VALID(task_id)

    task = &task_array[task_id];

    /* validate task handle */
    if (task->magic != TASK_MAGIC)
	return -EINVAL;

    return flavor_task_pause_hook(NULL, task, task_id);
}

int rtapi_wait(const int flag) {
    return flavor_task_wait_hook(NULL, flag);
}

int rtapi_task_resume(int task_id) {
    task_data *task;

    IS_TASK_ID_VALID(task_id)

    task = &task_array[task_id];

    /* validate task handle */
    if (task->magic != TASK_MAGIC)
	return -EINVAL;

    return flavor_task_resume_hook(NULL, task, task_id);
}


int rtapi_task_self(void) {
    return flavor_task_self_hook(NULL);
}

long long rtapi_task_pll_get_reference(void) {
    return flavor_task_pll_get_reference_hook(NULL);
}

int rtapi_task_pll_set_correction(long value) {
    return flavor_task_pll_set_correction_hook(NULL, value);
}


#endif  /* RTAPI */


#ifdef RTAPI
EXPORT_SYMBOL(rtapi_prio_highest);
EXPORT_SYMBOL(rtapi_prio_lowest);
EXPORT_SYMBOL(rtapi_prio_next_higher);
EXPORT_SYMBOL(rtapi_prio_next_lower);
EXPORT_SYMBOL(rtapi_task_new);
EXPORT_SYMBOL(rtapi_task_delete);
EXPORT_SYMBOL(rtapi_task_start);
EXPORT_SYMBOL(rtapi_task_stop);
EXPORT_SYMBOL(rtapi_task_pause);
EXPORT_SYMBOL(rtapi_wait);
EXPORT_SYMBOL(rtapi_task_resume);
EXPORT_SYMBOL(rtapi_task_self);
EXPORT_SYMBOL(rtapi_task_pll_get_reference);
EXPORT_SYMBOL(rtapi_task_pll_set_correction);
#endif

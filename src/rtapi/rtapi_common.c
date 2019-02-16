//    Copyright 2006-2013 Various Authors
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifdef ULAPI
#include <stdlib.h>
#endif

#include "rtapi_flavor.h"
#include "config.h"
#include "rtapi.h"
#include "rtapi_common.h"
#include "rtapi_compat.h"
#include "ring.h"

/* these pointers are initialized at startup to point
   to resource data in the master data structure above
   all access to the data structure should uses these
   pointers, they take into account the mapping of
   shared memory into either kernel or user space.
   (the RTAPI kernel module and each ULAPI user process
   has its own set of these vars, initialized to match
   that process's memory mapping.)
*/

// in the userland threads scenario, there is no point in having this
// in shared memory, so keep it here
static rtapi_data_t local_rtapi_data;
rtapi_data_t *rtapi_data = &local_rtapi_data;
task_data *task_array =  local_rtapi_data.task_array;
shmem_data *shmem_array = local_rtapi_data.shmem_array;
module_data *module_array = local_rtapi_data.module_array;

// RTAPI:
// global_data is exported by rtapi_module.c (kthreads)
// or rtapi_main.c (uthreads)
// ULAPI: exported in ulapi_autoload.c
extern global_data_t *global_data;

int shmdrv_loaded;  // set in rtapi_app_main, and ulapi_main
long page_size;     // set in rtapi_app_main

void rtapi_autorelease_mutex(void *variable)
{
    if (rtapi_data != NULL)
	rtapi_mutex_give(&(rtapi_data->mutex));
    else
	// programming error
	rtapi_print_msg(RTAPI_MSG_ERR,
			"rtapi_autorelease_mutex: rtapi_data == NULL!\n");
}

// in the RTAPI scenario,
// global_data is exported by instance.ko and referenced
// by rtapi.ko and hal_lib.ko

// in ULAPI, we have only hal_lib which calls 'down'
// onto ulapi.so to init, so in this case global_data
// is exported by hal_lib and referenced by ulapi.so


/* global init code */

void init_rtapi_data(rtapi_data_t * data)
{
    int n, m;

    /* has the block already been initialized? */
    if (data->magic == RTAPI_MAGIC) {
	/* yes, nothing to do */
	return;
    }
    /* no, we need to init it, grab mutex unconditionally */
    rtapi_mutex_try(&(data->mutex));
    /* set magic number so nobody else init's the block */
    data->magic = RTAPI_MAGIC;
    /* set version code and flavor ID so other modules can check it */
    data->serial = RTAPI_SERIAL;
    data->ring_mutex = 0;
    /* and get busy */
    data->rt_module_count = 0;
    data->ul_module_count = 0;
    data->task_count = 0;
    data->shmem_count = 0;
    data->timer_running = 0;
    data->timer_period = 0;
    /* init the arrays */
    for (n = 0; n <= RTAPI_MAX_MODULES; n++) {
	data->module_array[n].state = EMPTY;
	data->module_array[n].name[0] = '\0';
    }
    for (n = 0; n <= RTAPI_MAX_TASKS; n++) {
	data->task_array[n].state = EMPTY;
	data->task_array[n].prio = 0;
	data->task_array[n].owner = 0;
	data->task_array[n].taskcode = NULL;
	data->task_array[n].cpu = -1;   // use default
    }
    for (n = 0; n <= RTAPI_MAX_SHMEMS; n++) {
	data->shmem_array[n].key = 0;
	data->shmem_array[n].rtusers = 0;
	data->shmem_array[n].ulusers = 0;
	data->shmem_array[n].size = 0;
	for (m = 0; m < RTAPI_BITMAP_SIZE(RTAPI_MAX_SHMEMS +1); m++) {
	    data->shmem_array[n].bitmap[m] = 0;
	}
    }

    /* done, release the mutex */
    rtapi_mutex_give(&(data->mutex));
    return;
}

/***********************************************************************
*                    INIT AND EXIT FUNCTIONS                           *
************************************************************************/

int rtapi_init(const char *modname) {
    return rtapi_next_handle();
}

int rtapi_exit(int module_id) {
    /* do nothing for ULAPI */
    return 0;
}



/***********************************************************************
*                           RT Thread statistics collection            *
*
* Thread statistics are recorded in the global_data_t thread_status.
* array. Values therein are updated when:
*
* - an exception happens, and it is safe to do so
* - by an explicit call to rtapi_task_update_stats() from an RT thread
*
* This avoids the overhead of permanently updating thread status, while
* giving the user the option to track thread status from a HAL component
* thread function if so desired.
*
* Updating thread status is necessarily a flavor-dependent
* operation and hence goes through a hook.
*
* Inspecting thread status (e.g. in halcmd) needs to evaluate
* the thread status information based on the current flavor.
************************************************************************/

#ifdef RTAPI
int rtapi_task_update_stats(void)
{
    if (flavor_descriptor->task_update_stats_hook)
        return flavor_descriptor->task_update_stats_hook();
    else
        return -ENOSYS;  // not implemented in this flavor
}
#endif
/***********************************************************************
*                           RT exception handling                      *
*
*  all exceptions are funneled through a common exception handler
*  the default exception handler is defined in rtapi_exception.c but
*  may be redefined by a user-defined handler.
*
*  NB: the exception handler executes in a restricted context like
*  an in-kernel trap, or a signal handler. Limit processing in the
*  handler to an absolute minimum, and watch the stack size.
************************************************************************/

#ifdef RTAPI
// not available in ULAPI
extern rtapi_exception_handler_t rt_exception_handler;

// may override default exception handler
// returns the current handler so it might be restored
rtapi_exception_handler_t rtapi_set_exception(rtapi_exception_handler_t h)
{
    rtapi_exception_handler_t previous = rt_exception_handler;
    rt_exception_handler = h;
    return previous;
}
#endif

// defined and initialized in rtapi_module.c (kthreads), rtapi_main.c (userthreads)
extern ringbuffer_t rtapi_message_buffer;   // error ring access strcuture

int  rtapi_next_handle(void)
{
    return rtapi_add_and_fetch(1, &global_data->next_handle);
}

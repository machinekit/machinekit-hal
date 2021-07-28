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

#include <stdlib.h>             // strtol

#include "runtime/rtapi_flavor.h"
#include "runtime/config.h"
#include "runtime/rtapi.h"
#include "runtime/rtapi_common.h"
#include "runtime/rtapi_compat.h"
#include "runtime/shmdrv.h"  /* common shm driver API */
#include "runtime/ring.h"

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

int rtapi_instance;                             // instance id, visible throughout RTAPI

global_data_t *global_data = NULL;              // visible to all RTAPI modules
struct rtapi_heap *global_heap = NULL;

extern ringbuffer_t rtapi_message_buffer;   // error ring access strcuture

#ifdef ULAPI
// use 'ULAPI_DEBUG=<level> <hal binary/Python>' to trace ulapi loading
static int ulapi_debug = RTAPI_MSG_NONE;
#endif

#ifdef RTAPI
#define LOGTAG "ULAPI"
#else
#define LOGTAG "RTAPI"
#endif

int shmdrv_loaded;  // set in rtapi_app_main FIXME
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
*                    MODULE INIT FUNCTIONS                             *
************************************************************************/

int rtapi_module_init()
{
    int retval;
    int size  = 0;

#ifdef ULAPI
    // Set the rtapi_instance global for HAL library instances;
    // rtapi_instance set in rtapi_app.cc for RTAPI instances
    char *instance = getenv("MK_INSTANCE");
    if (instance != NULL)
	rtapi_instance = atoi(instance);

    char *debug_env = getenv("ULAPI_DEBUG");
    if (debug_env)
	ulapi_debug = atoi(debug_env);
    rtapi_set_msg_level(ulapi_debug);

    // Set up the ulapi flavor
    flavor_install(flavor_byname("ulapi"));
#endif

    int globalkey = OS_KEY(GLOBAL_KEY, rtapi_instance);

    shm_common_init();

    // tag messages originating from RT proper
    rtapi_set_logtag(LOGTAG);

    // flavor
    rtapi_print_msg(RTAPI_MSG_DBG,"%s:%d  %s %s init\n",
                    LOGTAG,
		    rtapi_instance,
		    flavor_name(NULL),
		    GIT_BUILD_SHA);

    // attach to global segment which rtapi_msgd owns and already
    // has set up:
    retval = shm_common_new(globalkey, &size,
			    rtapi_instance, (void **) &global_data, 0);

    if (retval ==  -ENOENT) {
	// the global_data segment does not exist.
	rtapi_print_msg(RTAPI_MSG_ERR,
			"%s:%d ERROR: global segment 0x%x does not exist"
			" (rtapi_msgd not started?)\n",
			LOGTAG, rtapi_instance, globalkey);
	return -EBUSY;
    }
    if (retval < 0) {
	 rtapi_print_msg(RTAPI_MSG_ERR,
			 "%s:%d ERROR: shm_common_new() failed key=0x%x %s\n",
                         LOGTAG, rtapi_instance, globalkey, strerror(-retval));
	 return retval;
    }
    if (size < sizeof(global_data_t)) {
	 rtapi_print_msg(RTAPI_MSG_ERR,
			 "%s:%d ERROR: unexpected global shm size:"
			 " expected: >%zu actual:%d\n",
			 LOGTAG, rtapi_instance, sizeof(global_data_t), size);
	 return -EINVAL;
    }

    // good to use global_data from here on

    // this heap is inited in rtapi_msgd.cc
    // make it accessible in RTAPI
    global_heap = &global_data->heap;

    // make the message ringbuffer accessible
    ringbuffer_init(shm_ptr(global_data, global_data->rtapi_messages_ptr),
		    &rtapi_message_buffer);
    rtapi_message_buffer.header->refcount++; // rtapi is 'attached'

    // flavor
    init_rtapi_data(rtapi_data);

    retval = flavor_module_init_hook(NULL);

    return retval;
}

int rtapi_app_main()
{
    return rtapi_module_init();
}

void rtapi_app_exit(void)
{
    flavor_module_exit_hook(NULL);

    rtapi_message_buffer.header->refcount--;

    rtapi_print_msg(RTAPI_MSG_DBG,"%s:%d exit\n", LOGTAG, rtapi_instance);

    rtapi_data = NULL;
}

/***********************************************************************
*                    INIT AND EXIT FUNCTIONS                           *
************************************************************************/

int rtapi_init(const char *modname) {
#ifdef ULAPI
    int res;

    // Load ULAPI if global_data hasn't been set up yet
    if (global_data == NULL && (res = rtapi_module_init())) {
        rtapi_print_msg(RTAPI_MSG_ERR,
                        "FATAL:  Failed to initialize module '%s'\n", modname);
        return res;
    }
        rtapi_print_msg(RTAPI_MSG_DBG,
                        "Module '%s' finished ULAPI init\n", modname);
#endif

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
    return flavor_task_update_stats_hook(NULL);
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

// defined and initialized in rtapi_main.c (userthreads)
//extern ringbuffer_t rtapi_message_buffer;   // error ring access strcuture

int  rtapi_next_handle(void)
{
    return rtapi_add_and_fetch(1, &global_data->next_handle);
}

long int simple_strtol(const char *nptr, char **endptr, int base) {
    return strtol(nptr, endptr, base);
}


#ifdef RTAPI
EXPORT_SYMBOL(rtapi_instance);
EXPORT_SYMBOL(global_data);
EXPORT_SYMBOL(global_heap);
EXPORT_SYMBOL(rtapi_autorelease_mutex);
EXPORT_SYMBOL(init_rtapi_data);
EXPORT_SYMBOL(rtapi_module_init);
EXPORT_SYMBOL(rtapi_app_main);
EXPORT_SYMBOL(rtapi_app_exit);
EXPORT_SYMBOL(rtapi_init);
EXPORT_SYMBOL(rtapi_exit);
EXPORT_SYMBOL(rtapi_task_update_stats);
EXPORT_SYMBOL(rtapi_set_exception);
EXPORT_SYMBOL(rtapi_next_handle);
EXPORT_SYMBOL(simple_strtol);
#endif

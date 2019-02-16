/********************************************************************
 * Copyright (C) 2012, 2013 Michael Haberler <license AT mah DOT priv DOT at>
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


#include <stdio.h>		// snprintf
#include <dlfcn.h>              // for dlopen/dlsym ulapi-$THREADSTYLE.so
#include <assert.h>
#include <limits.h>             // PATH_MAX
#include <stdlib.h>		// exit()

#include "rtapi_flavor.h"       // flavor_descriptor
#include "rtapi.h"		// RTAPI realtime OS API
#include "ring.h"		// RTAPI realtime OS API
#include "shmdrv.h" // common shared memory API

#ifdef ULAPI

// exported symbols
// since this is normal userland linking, not RT loading, no need to
// EXPORT_SYMBOL() any of those

global_data_t *global_data;
struct rtapi_heap *global_heap;
extern ringbuffer_t rtapi_message_buffer;   // error ring access strcuture
int rtapi_instance;

// use 'ULAPI_DEBUG=<level> <hal binary/Python>' to trace ulapi loading
static int ulapi_debug = RTAPI_MSG_NONE;

static int ulapi_load();

int _ulapi_init(const char *modname) {
    if (ulapi_load() < 0) {
	return -ENOSYS;
    }
    rtapi_print_msg(RTAPI_MSG_DBG,
		    "_ulapi_init(): ulapi %s loaded\n",
		    flavor_descriptor->name);

    // switch logging level to what was set in global via msgd:
    rtapi_set_msg_level(global_data->user_msg_level);

    // and return what was intended to start with
    return rtapi_init(modname);
}

static int ulapi_load()
{
    int retval;
    char *instance = getenv("MK_INSTANCE");
    char *debug_env = getenv("ULAPI_DEBUG");
    int size = 0;
    int globalkey;

    // set the rtapi_instance global for this hal library instance
    if (instance != NULL)
	rtapi_instance = atoi(instance);

    if (debug_env)
	ulapi_debug = atoi(debug_env);

    rtapi_set_msg_level(ulapi_debug);

    // tag message origin field
    rtapi_set_logtag("ulapi");

    // first thing is to attach the global segment, based on
    // the RTAPI instance id.

    // Also, it's the prerequisite for common error message
    // handling through the message ringbuffer; unless then
    // error messages will go to stderr.

    // init the common shared memory driver APU
    shm_common_init();

    globalkey = OS_KEY(GLOBAL_KEY, rtapi_instance);
    retval = shm_common_new(globalkey, &size,
			    rtapi_instance, (void **) &global_data, 0);

    if (retval == -ENOENT) {
	// the global_data segment does not exist. Happens if the realtime
	// script was not started
	rtapi_print_msg(RTAPI_MSG_ERR,
			"ULAPI:%d ERROR: realtime not started\n",
			rtapi_instance);
	return retval;
    }

    if (retval < 0) {
	// some other error attaching global
	rtapi_print_msg(RTAPI_MSG_ERR,
			"ULAPI:%d ERROR: shm_common_new() failed key=0x%x %s\n",
			rtapi_instance, globalkey, strerror(-retval));
	return retval;
    }

    if (size < sizeof(global_data_t)) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"ULAPI:%d ERROR: global segment size mismatch,"
			" expected: %zd, actual:%d\n",
		 rtapi_instance, sizeof(global_data_t), size);
	return -EINVAL;
    }

    if (global_data->magic != GLOBAL_READY) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"ULAPI:%d ERROR: global segment invalid magic:"
			" expected: 0x%x, actual: 0x%x\n",
			rtapi_instance, GLOBAL_READY,
			global_data->magic);
	return -EINVAL;
    }

    // global data set up ok

    // make the message ringbuffer accessible
    ringbuffer_init(shm_ptr(global_data, global_data->rtapi_messages_ptr),
		    &rtapi_message_buffer);
    
    // this heap is inited in rtapi_msgd.cc
    // make it accessible in HAL
    global_heap = &global_data->heap;

    // declare victory
    return 0;
}

//  ULAPI cleanup. Call the exit handler and unload ulapi-<flavor>.so.
void ulapi_cleanup(void)
{
    // call the ulapi exit handler
    // detach the rtapi shm segment as needed
    // (some flavors do not employ an rtapi shm segment)
    ulapi_exit(rtapi_instance);
    // NB: we do not detach the global segment
}

#endif // ULAPI

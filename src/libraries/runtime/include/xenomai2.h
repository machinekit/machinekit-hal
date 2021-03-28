/********************************************************************
* Description:  xenomai2.h
*               This file defines the differences specific to the
*               the Xenomai 2 user land thread system
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

#include "rtapi_flavor.h"

typedef enum {
    XU_EXCEPTION_NONE=0,

    XU_SIGXCPU,       // RT task switched to secondary domain
    XU_SIGXCPU_BUG,   // same, but failed to identify RT thread
    XU_ETIMEDOUT,     // release point was missed
    XU_EWOULDBLOCK,   // rt_task_wait_period() without previous rt_task_set_periodic()
    XU_EINTR,         // rt_task_unblock() called before release point
    XU_EPERM,         // cannot rt_task_wait_period() from this context
    XU_UNDOCUMENTED,  // unknown error code

    XU_EXCEPTION_LAST,

} xenomai2_exception_id_t;

typedef struct {
    // passed by ref from rt_task_wait_period()
    unsigned long overruns;
} xenomai2_exception_t;
// Check the exception struct size
ASSERT_SIZE_WITHIN(xenomai2_exception_t, MAX_FLAVOR_EXCEPTION_SIZE);

typedef struct {
    // as reported by rt_task_inquire()
    // filled in by rtapi_thread_updatestats(task_id) RTAPI call (TBD)
    int modeswitches;
    int ctxswitches;
    int pagefaults;
    long long exectime;    // Execution time in primary mode in nanoseconds.
    unsigned status;       // T_BLOCKED etc.

    // errors returned by rt_task_wait_period():
    // set by -ETIMEDOUT:
    int wait_errors; 	// total times the release point was missed
    int total_overruns;	// running count of the above
    // the -EWOULDBLOCK and -EINTR returns are API violations
    // and increment api_errors

    // all others increment other_errors
} xenomai2_stats_t;
// Check the stats struct size
ASSERT_SIZE_WITHIN(xenomai2_stats_t, MAX_FLAVOR_THREADSTATUS_SIZE);

extern flavor_descriptor_t flavor_xenomai2_descriptor;
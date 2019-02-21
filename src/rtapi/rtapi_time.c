/********************************************************************
* Description:  rtapi_time.c
*               This file, 'rtapi_time.c', implements the timing-
*               related functions for realtime modules.  See rtapi.h
*               for more info.
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
#include "rtapi_common.h"	// these functions
#include "rtapi_flavor.h"       // flavor_*

#include <time.h>		// clock_getres(), clock_gettime()


// find a useable time stamp counter
#ifdef MSR_H_USABLE
#include <asm/msr.h>
#endif

long int max_delay = DEFAULT_MAX_DELAY;

#ifdef RTAPI  /* hide most functions from ULAPI */

int period = 0;

// Actual number of counts of the periodic timer
unsigned long timer_counts;


long int rtapi_clock_set_period(long int nsecs) {
    struct timespec res = { 0, 0};

    if (nsecs == 0)
	return period;
    if (period != 0) {
	rtapi_print_msg(RTAPI_MSG_ERR, "attempt to set period twice\n");
	return -EINVAL;
    }

    if (flavor_feature(NULL, FLAVOR_TIME_NO_CLOCK_MONOTONIC))
        period = nsecs;
    else {
        clock_getres(CLOCK_MONOTONIC, &res);
        period = (nsecs / res.tv_nsec) * res.tv_nsec;
        if (period < 1)
            period = res.tv_nsec;

        rtapi_print_msg(RTAPI_MSG_DBG,
                        "rtapi_clock_set_period (res=%ld) -> %d\n", res.tv_nsec,
                        period);
    }

    return period;
}

void rtapi_delay(long int nsec)
{
    if (nsec > max_delay) {
	nsec = max_delay;
    }
    flavor_task_delay_hook(NULL, nsec);
}


long int rtapi_delay_max(void)
{
    return max_delay;
}

#endif /* RTAPI */

/* The following functions are common to both RTAPI and ULAPI */

long long int rtapi_get_time(void) {
    long long int res;

    res = flavor_get_time_hook(NULL);
    if (res == -ENOSYS) { // Unimplemented
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        res = ts.tv_sec * 1000000000LL + ts.tv_nsec;
    }
    return res;
}

long long int rtapi_get_clocks(void) {
    long long int res;
    res = flavor_get_clocks_hook(NULL);
    if (res == -ENOSYS) { // Unimplemented

#     ifdef MSR_H_USABLE
        /* This returns a result in clocks instead of nS, and needs to be
           used with care around CPUs that change the clock speed to save
           power and other disgusting, non-realtime oriented behavior.
           But at least it doesn't take a week every time you call it.  */
        rdtscll(res);
#     elif defined(__i386__) || defined(__x86_64__)
        __asm__ __volatile__("rdtsc" : "=A" (res));
#     else
        // Needed for e.g. ARM
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        res = ts.tv_sec * 1000000000LL + ts.tv_nsec;
#     endif
    }
    return res;
}

#ifdef RTAPI
EXPORT_SYMBOL(rtapi_clock_set_period);
EXPORT_SYMBOL(rtapi_delay);
EXPORT_SYMBOL(rtapi_delay_max);
EXPORT_SYMBOL(rtapi_get_time);
EXPORT_SYMBOL(rtapi_get_clocks);
#endif

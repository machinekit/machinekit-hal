/********************************************************************
* Copyright (C) 2012 - 2013 Michael Haberler <license AT mah DOT priv DOT at>
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


/***********************************************************************
*                           RT exception handling                      *
************************************************************************/

#include "config.h"
#include "rtapi.h"
#include "rtapi_exception.h"
#include "rtapi_flavor.h"

#ifdef RTAPI

#define MAX_RT_ERRORS 10

static int rtapi_default_rt_exception_handler(int type,
					      rtapi_exception_detail_t *detail,
					      rtapi_threadstatus_t *ts);

// exported symbol within RTAPI
rtapi_exception_handler_t rt_exception_handler
 = rtapi_default_rt_exception_handler;

// The RTAPI default exception handler -
// factored out as separate file to ease rolling your own in a component
//
// This function is not visible in ULAPI.

// this default handler just writes to the log but does not cause any action
// (eg wiggling an estop pin)
//
// for a more intelligent way to handle RT faults, see
// hal/components/rtmon.comp which uses rtapi_set_exception(handler)
// to override this default handler during module lifetime

static int rtapi_default_rt_exception_handler(int type,
					      rtapi_exception_detail_t *detail,
					      rtapi_threadstatus_t *ts)
{
    static int error_printed = 0;
    int level = (error_printed == 0) ? RTAPI_MSG_ERR : RTAPI_MSG_WARN;
    int res;

    if (error_printed < MAX_RT_ERRORS) {
	error_printed++;

        res = flavor_exception_handler_hook(NULL, type, detail, level);
        if (res == -ENOSYS) // Unimplemented
	    rtapi_print_msg(level,
			    "%d: unspecified exception detail=%p ts=%p",
			    type, detail, ts);

	if (error_printed ==  MAX_RT_ERRORS)
	    rtapi_print_msg(RTAPI_MSG_WARN,
			    "RTAPI: (further messages will be suppressed)\n");
    }
    return 0;
}


#endif

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

#ifndef _RTAPI_EXCEPTION_H
#define _RTAPI_EXCEPTION_H

#define  MAX_FLAVOR_THREADSTATUS_SIZE 256
#define  MAX_FLAVOR_EXCEPTION_SIZE 128

typedef void * exc_register_t;  // questionable

// ---- the common thread status descriptor -------

typedef struct  {
    // number of updates (calls to rtapi_task_update_stats()
    int num_updates;

    // eventually it would be nice to have a timestamp of the last
    // update; this should be done as soon as we have an actual
    // invocation time stamp in thread functions

    // generic
    int api_errors;      // hint at programming error
    int other_errors;    // unclassified error returns - peruse log for details

    // flavor-specific
    char flavor[MAX_FLAVOR_THREADSTATUS_SIZE];
} rtapi_threadstatus_t;


// ---- the common thread exception descriptor -------

typedef struct {
    int task_id;            // which RTAPI thread caused this
    int error_code;         // as reported by the failing API or system call

    // flavor-specific
    char flavor[MAX_FLAVOR_EXCEPTION_SIZE];
} rtapi_exception_detail_t;

// Exception handler signature

// NB: the exception handler very likely executes in a severely
// restricted environment (e.g. kernel trap, signal handler) - take
// care to do only the absolutely minimal processing in the handler
// and avoid operations which use large local variables

// 'type' is guaranteed to be set
// both of detail and threadstatus might be passed as NULL
typedef int (*rtapi_exception_handler_t) (int type,
					  // which determines the interpretation of
					  // the rtapi_exception_detail_t
					  rtapi_exception_detail_t *,
					  rtapi_threadstatus_t *);

#endif // _RTAPI_EXCEPTION_H

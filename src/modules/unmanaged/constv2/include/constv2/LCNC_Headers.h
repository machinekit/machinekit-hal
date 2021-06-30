#ifndef _LCNC_HEADERS_H_
#define _LCNC_HEADERS_H_

#define _CORRECT_ISO_CPP_STRING_H_PROTO

// Std C / C++
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>   /* Standard types */
#include <signal.h>
#include <math.h>
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <ctype.h>
#include <values.h>             // DBL_MAX, maybe
#include <limits.h>             // DBL_MAX, maybe
#include <stdarg.h>
#include <sys/stat.h>           // struct stat, stat()

#include <iostream>

#include <fnmatch.h>

// /usr/include/hal
#include <rtapi.h>
#ifdef RTAPI
#include "rtapi_app.h"
#endif
#include "rtapi_string.h"
#include <hal/hal.h>		
#include <hal/hal_priv.h>	// need to get this from source and add it
#include <hal/hal_accessor.h>
#include "hal/hal_internal.h"

#include <mk-inifile.hh>	// INIFILE
//#include <rcs_print.hh>
//#include <timer.hh>

// Can't rely on bool working otherwise
#undef TRUE
#define TRUE (1)
#undef FALSE
#define FALSE (0)
#undef true
#define true (1)
#undef false
#define false (0)

#endif


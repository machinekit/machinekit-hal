/********************************************************************
 *  Description: string.c
 *  Provides functions for running linux kernel modules in user space,
 *  emulating part of
 *  #include <linux/string.h>
 *
 *  Author(s): Charles Steinkuehler
 *  License: GNU LGPL Version 2.1 or (at your option) any later version.
 *
 *  Last change:
 *  2012-Dec-17 Charles Steinkuehler
 *              Initial version
 ********************************************************************/

/********************************************************************
 *  This file is part of Machinekit RTAPI / HAL
 *
 *  Copyright (C) 2012  Charles Steinkuehler
 *                      <charles AT steinkuehler DOT net>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  THE AUTHORS OF THIS LIBRARY ACCEPT ABSOLUTELY NO LIABILITY FOR
 *  ANY HARM OR LOSS RESULTING FROM ITS USE.  IT IS _EXTREMELY_ UNWISE
 *  TO RELY ON SOFTWARE ALONE FOR SAFETY.  Any machinery capable of
 *  harming persons must have provisions for completely removing power
 *  from all motors, etc, before persons enter any danger area.  All
 *  machinery must be designed to comply with local and national safety
 *  codes, and the authors of this software can not, and do not, take
 *  any responsibility for such compliance.
 *
 *  This code is part of the Machinekit HAL project.  For more
 *  information, go to https://github.com/machinekit.
 *******************************************************************/

#include "user_pci/config_module.h"
#include RTAPI_INC_STRING_H
#include <wordexp.h>    // wordexp (substitute for argv_split)

/*  We need to save the wordexp_t returned by wordexp so we can free it later.
    For now, cheat and use a static global, which means we can only process
    one set of argv at a time.  If this causes problems, we can create a list
    of dynamically allocated wordexp_t values and match up the one to free by
    looking at the we_wordv pointer.  Or a code change to the driver could
    let the calling code track the wordexp_t for us.
 */
static wordexp_t result;

char **argv_split(gfp_t gfp, const char *str, int *argcp)
{

    switch (wordexp (str, &result, 0)) {
        /* Successful */
    case 0:
	*argcp = result.we_wordc;
	return result.we_wordv;
	break;

        // If the error was WRDE_NOSPACE,
        // then perhaps part of the result was allocated.
    case WRDE_NOSPACE:
	wordfree (&result);
	/* Fall-through */

        /* Some other error.  */
    default:
	return NULL;

    }
}

void argv_free(char **argv)
{
    wordfree(&result);
}

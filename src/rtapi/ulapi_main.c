/********************************************************************
* Description:  ulapi_main.c
*
*               purpose:
*               - attach to the rtapi data segment within the ULAPI
*                 symbol namespace
*               - pass in the global_data shm pointer
*               - set the rtapi_instance variable
*
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

#if defined(ULAPI)

#include "rtapi_flavor.h"       // flavor_descriptor
#include "config.h"
#include "rtapi.h"		/* RTAPI realtime OS API */
#include "rtapi_global.h"       /* global_data_t */
#include "rtapi_common.h"
#include "rtapi_compat.h"
#include "shmdrv.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>          //shm_open
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int ulapi_main(int instance, int flavor, global_data_t *global)
{
    rtapi_instance = instance; // from here on global within ulapi.so

    // shm_common_init(); // common shared memory API needs this

    // the HAL library constructor already has the global
    // shm segment attached, so no need to do it again here
    // since we're not using the rtapi_app_init()/rtapi_app_exit()
    // calling conventions might as well pass it it

    // this sets global_data for use within ulapi.so which
    // has a disjoint symbol namespace from hal_lib
    global_data = global;

    /* rtapi_print_msg(RTAPI_MSG_DBG,"ULAPI:%d %s %s init\n", */
    /* 		    rtapi_instance, */
    /* 		    flavor_descriptor->name, */
    /* 		    GIT_VERSION); */


    return 0;
}

int ulapi_exit(int instance)
{
    /* rtapi_print_msg(RTAPI_MSG_DBG, "ULAPI:%d %s exit\n", */
    /* 		    instance, */
    /* 		    GIT_VERSION); */

    return 0;
}


#endif // ULAPI

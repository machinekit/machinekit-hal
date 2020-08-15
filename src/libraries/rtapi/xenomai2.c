/********************************************************************
* Description:  xenomai2.c
*               This file, 'xenomai2.c', implements the unique
*               functions for the Xenomai 2 userland thread system.
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

#include <sys/stat.h>                   // stat()
#include <unistd.h>                     // getgroups()
#include <stdlib.h>                     // calloc(), free(), exit()
#include <dlfcn.h>                      // dlopen(), dlsym(), dlerror()

#include "xenomai2.h"
#include <nucleus/heap.h>               // XNHEAP_DEV_NAME
#define PROC_IPIPE_XENOMAI2 "/proc/ipipe/Xenomai"
#define XENO2_GID_SYSFS "/sys/module/xeno_nucleus/parameters/xenomai_gid"

#ifdef USE_SIGXCPU
#include <signal.h>                     /* sigaction/SIGXCPU handling */
#endif

/***********************************************************************
* rtapi_main.c
*/

// Forward-declare the plugin loader
int load_xenomai2_plugin(void);

int xenomai2_module_init_hook(void)
{
    if (! load_xenomai2_plugin())
        return -1;


#ifdef USE_SIGXCPU
    struct sigaction sig_act;

    sigemptyset( &sig_act.sa_mask );
    sig_act.sa_sigaction = signal_handler;
    sig_act.sa_flags = SA_SIGINFO;

    // SIGXCPU delivery must be enabled within the thread by
    // rt_task_set_mode(0, T_WARNSW, NULL);
    // see _rtapi_task_wrapper()
    if (sigaction(SIGXCPU, &sig_act, (struct sigaction *) NULL))
	rtapi_print_msg(RTAPI_MSG_ERR,
			"rtapi_module_init_hook(sigaction): %d %s\n",
			errno, strerror(errno));
#endif
    return 0;
}

void xenomai2_module_exit_hook(void)
{
#ifdef USE_SIGXCPU
    struct sigaction sig_act;

    // ignore SIGXCPU
    sigemptyset( &sig_act.sa_mask );
    sig_act.sa_handler = SIG_IGN;
    if (sigaction(SIGXCPU, &sig_act, (struct sigaction *) NULL))
	rtapi_print_msg(RTAPI_MSG_ERR,
			"rtapi_module_exit_hook(sigaction): %d %s\n",
			errno, strerror(errno));
#endif
}



int kernel_is_xenomai2()
{
    struct stat sb;

    return ((stat(XNHEAP_DEV_NAME, &sb) == 0) &&
	    (stat(PROC_IPIPE_XENOMAI2, &sb) == 0) &&
	    (stat(XENO2_GID_SYSFS, &sb) == 0));
}

int xenomai2_flavor_check(void);

int xenomai2_can_run_flavor()
{
    if (! kernel_is_xenomai2())
        return 0;

    if (! xenomai2_flavor_check())
        return 0;

    return 1;
}

int xenomai2_gid()
{
    FILE *fd;
    int gid = -1;

    if ((fd = fopen(XENO2_GID_SYSFS,"r")) != NULL) {
	if (fscanf(fd, "%d", &gid) != 1) {
	    fclose(fd);
	    return -EBADF; // garbage in sysfs device
	} else {
	    fclose(fd);
	    return gid;
	}
    }
    return -ENOENT; // sysfs device cant be opened
}

int user_in_xenomai2_group()
{
    int numgroups, i;
    gid_t *grouplist;
    int gid = xenomai2_gid();

    if (gid < 0)
	return gid;

    numgroups = getgroups(0,NULL);
    grouplist = (gid_t *) calloc( numgroups, sizeof(gid_t));
    if (grouplist == NULL)
	return -ENOMEM;
    if (getgroups( numgroups, grouplist) > 0) {
	for (i = 0; i < numgroups; i++) {
	    if (grouplist[i] == (unsigned) gid) {
		free(grouplist);
		return 1;
	    }
	}
    } else {
	free(grouplist);
	return errno;
    }
    return 0;
}

int xenomai2_flavor_check(void) {
    // catch installation error: user not in xenomai group
    int retval = user_in_xenomai2_group();

    switch (retval) {
	case 1:  // yes
	    break;
	case 0:
	    fprintf(stderr, "this user is not member of group xenomai\n");
	    fprintf(stderr, "please 'sudo adduser <username>  xenomai',"
		    " logout and login again\n");
	    exit(EXIT_FAILURE);

	default:
	    fprintf(stderr, "cannot determine if this user "
		    "is a member of group xenomai: %s\n",
		    strerror(-retval));
	    exit(EXIT_FAILURE);
    }
    return retval;
}



// The Xenomai 2 flavor descriptor data before loading the plugin
flavor_descriptor_t flavor_xenomai2_descriptor = {
    .name = "xenomai2",
    .flavor_id = RTAPI_FLAVOR_XENOMAI2_ID,
    .flags = FLAVOR_DOES_IO + FLAVOR_IS_RT + FLAVOR_TIME_NO_CLOCK_MONOTONIC,
    .can_run_flavor = xenomai2_can_run_flavor,
    .module_init_hook = xenomai2_module_init_hook,
    .module_exit_hook = xenomai2_module_exit_hook,
    .task_pll_get_reference_hook = NULL,
    .task_pll_set_correction_hook = NULL,
    .task_new_hook = NULL,
    // The following are set during module init when Xenomai 2 libs are loaded
    .task_update_stats_hook = NULL,
    .exception_handler_hook = NULL,
    .task_print_thread_stats_hook = NULL,
    .task_delete_hook = NULL,
    .task_start_hook = NULL,
    .task_stop_hook = NULL,
    .task_pause_hook = NULL,
    .task_wait_hook = NULL,
    .task_resume_hook = NULL,
    .task_delay_hook = NULL,
    .get_time_hook = NULL,
    .get_clocks_hook = NULL,
    .task_self_hook = NULL,
};

int load_xenomai2_plugin(void)
{
    // Load the xenomai2_loader.so module that does the real work
    void *mod_handle = dlopen("xenomai2_loader.so", RTLD_GLOBAL |RTLD_NOW);
    if (!mod_handle) {
        rtapi_print_msg(RTAPI_MSG_ERR,
                        "Unable to load xenomai2_stub.so:  %s\n",
                        dlerror());
        return -1;
    }
    // Fill out missing parts of the flavor descriptor
    plugin_flavor_descriptor_updater_t xenomai2_descriptor_updater =
        dlsym(mod_handle, "xenomai2_descriptor_updater");
    xenomai2_descriptor_updater(&flavor_xenomai2_descriptor);
    return 0;
}

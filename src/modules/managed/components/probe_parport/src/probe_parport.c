/********************************************************************
* Description:  probe_parport.c
*               This file, 'probe_parport.c', is a HAL component that
*               does "PNP" probing for the standard PC parallel port.
*               If your parallel port does not work with hal, it may if you
*               load this driver before you load hal_parport, e.g.:
*                   loadrt probe_parport
*                   loadrt hal_parport cfg=378
*
*               This driver is particularly likely to be helpful if a message
*               similar to this one is logged in 'dmesg' when you 'sudo
*               modprobe -i parport_pc':
*                   parport: PnPBIOS parport detected.
*
* Author: Jeff Epler
* License: GPL Version 2
*
* Copyright (c) 2006 All rights reserved.
*
* Last change:
********************************************************************/

/* Low-level parallel-port routines for 8255-based PC-style hardware.
 *
 * Authors: Phil Blundell <philb@gnu.org>
 *          Tim Waugh <tim@cyberelk.demon.co.uk>
 *	    Jose Renau <renau@acm.org>
 *          David Campbell <campbell@torque.net>
 *          Andrea Arcangeli
 *
 * based on work by Grant Guenther <grant@torque.net> and Phil Blundell.
 *
 * Cleaned up include files - Russell King <linux@arm.uk.linux.org>
 * DMA support - Bert De Jonghe <bert@sophis.be>
 * Many ECP bugs fixed.  Fred Barnes & Jamie Lokier, 1999
 * More PCI support now conditional on CONFIG_PCI, 03/2001, Paul G.
 * Various hacks, Fred Barnes, 04/2001
 * Updated probing logic - Adam Belay <ambx1@neo.rr.com>
 */

#include "config.h"
#include "hal/hal.h"
#include "rtapi.h"
#include "rtapi_app.h"

static int comp_id;

int rtapi_app_main(void) {
    comp_id = hal_init("probe_parport");
    if (comp_id < 0) {
        rtapi_print_msg(RTAPI_MSG_ERR, "PROBE_PARPORT: ERROR: hal_init() failed\n");
        return -1;
    }
    hal_ready(comp_id);

    return 0;
}

void rtapi_app_exit(void) {
    hal_exit(comp_id);
}

/********************************************************************
 *  Description: device.c
 *  Provides functions for running linux kernel modules in user space,
 *  emulating part of
 *  #include <linux/device.h>
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
 ********************************************************************/

#include "user_pci/config_module.h"
#include RTAPI_INC_DEVICE_H

extern int dev_set_name(struct device *dev, const char *name) {
    strncpy(dev->name, name, HAL_NAME_LEN);
    dev->name[HAL_NAME_LEN - 1] = '\0';
    return 0;
}

extern int device_register(struct device *dev) {
    return 0;
}

extern void device_unregister(struct device *dev) {
    return;
}

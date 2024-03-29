
//
// This is a userspace HAL driver for the ShuttleXpress device by Contour
// Design.
//
// Copyright 2011 Sebastian Kuzminsky <seb@highlab.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//


#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/types.h>
#include <linux/hidraw.h>

#include "hal/hal.h"



#ifndef HIDIOCGRAWNAME
#define HIDIOCGRAWNAME(len)     _IOC(_IOC_READ, 'H', 0x04, len)
#endif

#define Max(a, b)  ((a) > (b) ? (a) : (b))




// USB Vendor and Product IDs
#define VENDOR_ID  0x0b33  // Contour Design
#define PRODUCT_ID 0x0020  // ShuttleXpress


// each packet from the ShuttleXpress is this many bytes
#define PACKET_LEN 5


// the module name, and prefix for all HAL pins
char *modname = "shuttlexpress";


int hal_comp_id;




// each ShuttleXpress presents this interface to HAL
struct shuttlexpress_hal {
    hal_bit_t *button_0;
    hal_bit_t *button_0_not;
    hal_bit_t *button_1;
    hal_bit_t *button_1_not;
    hal_bit_t *button_2;
    hal_bit_t *button_2_not;
    hal_bit_t *button_3;
    hal_bit_t *button_3_not;
    hal_bit_t *button_4;
    hal_bit_t *button_4_not;
    hal_s32_t *counts;        // accumulated counts from the jog wheel
    hal_float_t *spring_wheel_f;  // current position of the springy outer wheel, as a float from -1 to +1 inclusive
    hal_s32_t *spring_wheel_s32;  // current position of the springy outer wheel, as a s32 from -7 to +7 inclusive
};


struct shuttlexpress {
    int fd;
    char *device_file;
    struct shuttlexpress_hal *hal;
    int read_first_event;
    int prev_count;
};


// this will become an array of all the ShuttleXpress devices we're using
struct shuttlexpress **shuttlexpress = NULL;
int num_devices = 0;




static void exit_handler(int sig) {
    printf("%s: exiting\n", modname);
    exit(0);
}


static void call_hal_exit(void) {
    hal_exit(hal_comp_id);
}




int read_update(struct shuttlexpress *s) {
    int r;
    int8_t packet[PACKET_LEN];

    r = read(s->fd, packet, PACKET_LEN);
    if (r < 0) {
        fprintf(stderr, "%s: error reading %s: %s\n", modname, s->device_file, strerror(errno));
        return -1;
    } else if (r == 0) {
        fprintf(stderr, "%s: EOF on %s\n", modname, s->device_file);
        return -1;
    }

    *s->hal->button_0 = packet[3] & 0x10;
    *s->hal->button_0_not = !*s->hal->button_0;
    *s->hal->button_1 = packet[3] & 0x20;
    *s->hal->button_1_not = !*s->hal->button_1;
    *s->hal->button_2 = packet[3] & 0x40;
    *s->hal->button_2_not = !*s->hal->button_2;
    *s->hal->button_3 = packet[3] & 0x80;
    *s->hal->button_3_not = !*s->hal->button_3;
    *s->hal->button_4 = packet[4] & 0x01;
    *s->hal->button_4_not = !*s->hal->button_4;

    {
        int curr_count = packet[1];

        if (s->read_first_event == 0) {
            *s->hal->counts = 0;
            s->prev_count = curr_count;
            s->read_first_event = 1;
        } else {
            int diff_count = curr_count - s->prev_count;
            if (diff_count > 128) diff_count -= 256;
            if (diff_count < -128) diff_count += 256;
            *s->hal->counts += diff_count;
            s->prev_count = curr_count;
        }
    }

    *s->hal->spring_wheel_s32 = packet[0];
    *s->hal->spring_wheel_f = packet[0] / 7.0;

    return 0;
}


struct shuttlexpress *check_for_shuttlexpress(char *dev_filename) {
    struct shuttlexpress *s;
    struct hidraw_devinfo devinfo;
    char name[100];
    int r;

    printf("%s: checking %s\n", modname, dev_filename);

    s = (struct shuttlexpress *)calloc(1, sizeof(struct shuttlexpress));
    if (s == NULL) {
        fprintf(stderr, "%s: out of memory!\n", modname);
        return NULL;
    }

    s->device_file = dev_filename;

    s->fd = open(s->device_file, O_RDONLY);
    if (s->fd < 0) {
        fprintf(stderr, "%s: error opening %s: %s\n", modname, s->device_file, strerror(errno));
        if (errno == EACCES) {
            fprintf(stderr, "%s: make sure you have read permission on %s, read the shuttlexpress(1) manpage for more info\n", modname, s->device_file);
        }
        goto fail0;
    }


    r = ioctl(s->fd, HIDIOCGRAWINFO, &devinfo);
    if (r < 0) {
        fprintf(stderr, "%s: error with ioctl HIDIOCGRAWINFO on %s: %s\n", modname, s->device_file, strerror(errno));
        goto fail1;
    }

    if (devinfo.vendor != VENDOR_ID) {
        fprintf(stderr, "%s: dev %s has unexpected Vendor ID 0x%04x (expected Contour Design, 0x%04x)\n", modname, s->device_file, devinfo.vendor, VENDOR_ID);
        goto fail1;
    }

    if (devinfo.product != PRODUCT_ID) {
        fprintf(stderr, "%s: dev %s has unexpected Product ID 0x%04x (expected ShuttleXpress, 0x%04x)\n", modname, s->device_file, devinfo.product, PRODUCT_ID);
        goto fail1;
    }

    r = ioctl(s->fd, HIDIOCGRAWNAME(99), name);
    if (r < 0) {
        fprintf(stderr, "%s: error with ioctl HIDIOCGRAWNAME on %s: %s\n", modname, s->device_file, strerror(errno));
        goto fail1;
    }
    printf("%s: found %s on %s\n", modname, name, s->device_file);


    s->hal = (struct shuttlexpress_hal *)hal_malloc(sizeof(struct shuttlexpress_hal));
    if (s->hal == NULL) {
        fprintf(stderr, "%s: ERROR: unable to allocate HAL shared memory\n", modname);
        goto fail1;
    }

    r = hal_pin_bit_newf(HAL_OUT, &(s->hal->button_0), hal_comp_id, "%s.%d.button-0", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_bit_newf(HAL_OUT, &(s->hal->button_0_not), hal_comp_id, "%s.%d.button-0-not", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_bit_newf(HAL_OUT, &(s->hal->button_1), hal_comp_id, "%s.%d.button-1", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_bit_newf(HAL_OUT, &(s->hal->button_1_not), hal_comp_id, "%s.%d.button-1-not", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_bit_newf(HAL_OUT, &(s->hal->button_2), hal_comp_id, "%s.%d.button-2", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_bit_newf(HAL_OUT, &(s->hal->button_2_not), hal_comp_id, "%s.%d.button-2-not", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_bit_newf(HAL_OUT, &(s->hal->button_3), hal_comp_id, "%s.%d.button-3", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_bit_newf(HAL_OUT, &(s->hal->button_3_not), hal_comp_id, "%s.%d.button-3-not", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_bit_newf(HAL_OUT, &(s->hal->button_4), hal_comp_id, "%s.%d.button-4", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_bit_newf(HAL_OUT, &(s->hal->button_4_not), hal_comp_id, "%s.%d.button-4-not", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_s32_newf(HAL_OUT, &(s->hal->counts), hal_comp_id, "%s.%d.counts", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_float_newf(HAL_OUT, &(s->hal->spring_wheel_f), hal_comp_id, "%s.%d.spring-wheel-f", modname, num_devices);
    if (r != 0) goto fail1;

    r = hal_pin_s32_newf(HAL_OUT, &(s->hal->spring_wheel_s32), hal_comp_id, "%s.%d.spring-wheel-s32", modname, num_devices);
    if (r != 0) goto fail1;

    *s->hal->button_0 = 0;
    *s->hal->button_0_not = 1;
    *s->hal->button_1 = 0;
    *s->hal->button_1_not = 1;
    *s->hal->button_2 = 0;
    *s->hal->button_2_not = 1;
    *s->hal->button_3 = 0;
    *s->hal->button_3_not = 1;
    *s->hal->button_4 = 0;
    *s->hal->button_4_not = 1;
    *s->hal->counts = 0;
    *s->hal->spring_wheel_f = 0.0;
    *s->hal->spring_wheel_s32 = 0;

    return s;


fail1:
    close(s->fd);

fail0:
    free(s);
    return NULL;
}




int main(int argc, char *argv[]) {
    int i;
    glob_t glob_buffer;

    char **names;
    int num_names;


    hal_comp_id = hal_init(modname);
    if (hal_comp_id < 1) {
        fprintf(stderr, "%s: ERROR: hal_init failed\n", modname);
        exit(1);
    }

    signal(SIGINT, exit_handler);
    signal(SIGTERM, exit_handler);
    atexit(call_hal_exit);


    // get the list of device filenames to check for ShuttleXpress devices
    if (argc > 1) {
        // list of devices provided on the command line
        names = &argv[1];
        num_names = argc - 1;
    } else {
        // probe for /dev/hidraw*
        int r;

        r = glob("/dev/hidraw*", 0, NULL, &glob_buffer);
        if (r == GLOB_NOMATCH) {
            fprintf(stderr, "%s: no /dev/hidraw* found, is device plugged in?\n", modname);
            exit(1);
        } else if (r != 0) {
            fprintf(stderr, "%s: error with glob!\n", modname);
            exit(1);
        }
        names = glob_buffer.gl_pathv;
        num_names = glob_buffer.gl_pathc;

        // the pathnames we got from glob(3) are used in the shuttlexpress array, so we intentionally dont call globfree(3)
    }


    // probe for ShuttleXpress devices on all those device file names
    for (i = 0; i < num_names; i ++) {
        struct shuttlexpress *s;
        s = check_for_shuttlexpress(names[i]);
        if (s == NULL) continue;

        num_devices ++;
        shuttlexpress = (struct shuttlexpress **)realloc(shuttlexpress, (num_devices * sizeof(struct shuttlexpress *)));
        if (shuttlexpress == NULL) {
            fprintf(stderr, "%s: out of memory!\n", modname);
            exit(1);
        }
        shuttlexpress[num_devices - 1] = s;
    }


    if (num_devices == 0) {
        fprintf(stderr, "%s: no devices found\n", modname);
        exit(1);
    }


    hal_ready(hal_comp_id);


    // select on all the hidraw devices, process events from the active ones
    while (1) {
        fd_set readers;
        int max_fd;
        int i;
        int r;

        FD_ZERO(&readers);
        max_fd = -1;

        for (i = 0; i < num_devices; i ++) {
            FD_SET(shuttlexpress[i]->fd, &readers);
            max_fd = Max(max_fd, shuttlexpress[i]->fd);
        }

        r = select(max_fd + 1, &readers, NULL, NULL, NULL);
        if (r < 0) {
            if ((errno == EAGAIN) || (errno == EINTR)) continue;
            fprintf(stderr, "%s: error with select!\n", modname);
            exit(1);
        }

        for (i = 0; i < num_devices; i ++) {
            if (FD_ISSET(shuttlexpress[i]->fd, &readers)) {
                r = read_update(shuttlexpress[i]);
                if (r < 0) {
                    exit(1);
                }
            }
        }
    }

    exit(0);
}

//    Copyright (C) 2009 Jeff Epler
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

#ifndef HAL_PARPORT_COMMON_H
#define HAL_PARPORT_COMMON_H

#include <linux/parport.h>

typedef struct hal_parport_t
{
    unsigned short base;
    unsigned short base_hi;
    struct pardevice *linux_dev;
    void *region;
    void *region_hi;
    int dev_fd;
} hal_parport_t;


#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/ppdev.h>

#if defined(USE_PORTABLE_PARPORT_IO)
static unsigned int hal_parport_error_count;
#else
#include <sys/io.h>
#endif

static int
hal_parport_get(int comp_id, hal_parport_t *port,
		unsigned short base, unsigned short base_hi, unsigned int modes)
{
	FILE *fd;
	char devname[64] = { 0, };
	char devpath[96] = { 0, };

	memset(port, 0, sizeof(*port));
	port->dev_fd = -1;
	port->base = base;
	port->base_hi = base_hi;

	if (base < 0x20) {
		/* base is the parallel port number. */
		snprintf(devname, sizeof(devname), "parport%u", base);
		goto found_dev;
	} else {
		char *buf = NULL;
		size_t bufsize = 0;

		/* base is the I/O port base. */
		fd = fopen("/proc/ioports", "r");
		if (!fd) {
			rtapi_print_msg(RTAPI_MSG_ERR, "Failed to open /proc/ioports: %s\n",
					strerror(errno));
			return -ENODEV;
		}
		while (getline(&buf, &bufsize, fd) > 0) {
			char *b = buf;
			unsigned int start, end;
			int count;

			while (b[0] == ' ' || b[0] == '\t') /* Strip leading whitespace */
				b++;
			count = sscanf(b, "%x-%x : %63s", &start, &end, devname);
			if (count != 3)
				continue;
			if (strncmp(devname, "parport", 7) != 0)
				continue;
			if (start == base) {
				fclose(fd);
				free(buf);
				goto found_dev;
			}
		}
		fclose(fd);
		free(buf);
	}
	rtapi_print_msg(RTAPI_MSG_ERR,
			"Did not find parallel port with base address 0x%03X\n",
			base);
	return -ENODEV;
found_dev:
	snprintf(devpath, sizeof(devpath), "/dev/%s", devname);

#if defined(USE_PORTABLE_PARPORT_IO)
	rtapi_print_msg(RTAPI_MSG_INFO, "Using parallel port %s (base 0x%03X) with ioctl I/O\n",
			devpath, base);
#else
	rtapi_print_msg(RTAPI_MSG_INFO, "Using parallel port %s (base 0x%03X) with direct inb/outb I/O\n",
			devpath, base);
#endif
	port->dev_fd = open(devpath, O_RDWR);
	if (port->dev_fd < 0) {
		rtapi_print_msg(RTAPI_MSG_ERR, "Failed to open parallel port %s: %s\n",
				devpath, strerror(errno));
		return -ENODEV;
	}
	if (ioctl(port->dev_fd, PPEXCL)) {
		rtapi_print_msg(RTAPI_MSG_ERR,
				"Failed to get exclusive access to parallel port %s\n",
				devpath);
		close(port->dev_fd);
		return -ENODEV;
	}
	if (ioctl(port->dev_fd, PPCLAIM)) {
		rtapi_print_msg(RTAPI_MSG_ERR,
				"Failed to claim parallel port %s\n",
				devpath);
		close(port->dev_fd);
		return -ENODEV;
	}
	return 0;
}

static void hal_parport_release(hal_parport_t *port)
{
	if (port->dev_fd < 0)
		return;
	ioctl(port->dev_fd, PPRELEASE);
	close(port->dev_fd);
}

#if defined(USE_PORTABLE_PARPORT_IO)
static inline unsigned char hal_parport_read_status(hal_parport_t *port)
{
	unsigned char x;

	if (ioctl(port->dev_fd, PPRSTATUS, &x)) {
		if (hal_parport_error_count < 10) {
			hal_parport_error_count++;
			rtapi_print_msg(RTAPI_MSG_ERR, "Failed to read parport status.\n");
			return 0;
		}
	}

	return x;
}

static inline unsigned char hal_parport_read_data(hal_parport_t *port)
{
	unsigned char x;

	if (ioctl(port->dev_fd, PPRDATA, &x)) {
		if (hal_parport_error_count < 10) {
			hal_parport_error_count++;
			rtapi_print_msg(RTAPI_MSG_ERR, "Failed to read parport data.\n");
			return 0;
		}
	}

	return x;
}

static inline void hal_parport_write_data(hal_parport_t *port, unsigned char x)
{
	if (ioctl(port->dev_fd, PPWDATA, &x)) {
		if (hal_parport_error_count < 10) {
			hal_parport_error_count++;
			rtapi_print_msg(RTAPI_MSG_ERR, "Failed to write parport data.\n");
			return;
		}
	}
}

static inline unsigned char hal_parport_read_control(hal_parport_t *port)
{
	unsigned char x;

	if (ioctl(port->dev_fd, PPRCONTROL, &x)) {
		if (hal_parport_error_count < 10) {
			hal_parport_error_count++;
			rtapi_print_msg(RTAPI_MSG_ERR, "Failed to read parport control.\n");
			return 0;
		}
	}

	return x;
}

static inline void hal_parport_write_control(hal_parport_t *port, unsigned char x)
{
	if (ioctl(port->dev_fd, PPWCONTROL, &x)) {
		if (hal_parport_error_count < 10) {
			hal_parport_error_count++;
			rtapi_print_msg(RTAPI_MSG_ERR, "Failed to write parport control.\n");
			return;
		}
	}
}

static inline void hal_parport_set_datadir(hal_parport_t *port, int enable_inputs)
{
	enable_inputs = !!enable_inputs;
	if (ioctl(port->dev_fd, PPDATADIR, &enable_inputs)) {
		if (hal_parport_error_count < 10) {
			hal_parport_error_count++;
			rtapi_print_msg(RTAPI_MSG_ERR, "Failed to set parport data direction.\n");
			return;
		}
	}
}
#endif // defined(USE_PORTABLE_PARPORT_IO)



#endif

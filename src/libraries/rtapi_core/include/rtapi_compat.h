/********************************************************************

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

#ifndef RTAPI_COMPAT_H
#define RTAPI_COMPAT_H

#include "rtapi_bitops.h"
#include <limits.h> // provides PATH_MAX

// these functions must work with or without rtapi.h included
#if !defined(SUPPORT_BEGIN_DECLS)
#if defined(__cplusplus)
#define SUPPORT_BEGIN_DECLS extern "C" {
#define SUPPORT_END_DECLS }
#else
#define SUPPORT_BEGIN_DECLS
#define SUPPORT_END_DECLS
#endif
#endif

SUPPORT_BEGIN_DECLS

extern long int simple_strtol(const char *nptr, char **endptr, int base);

// whatever is written is printf-style
int rtapi_fs_write(const char *path, const char *format, ...);

// read a string from a sysfs entry.
// strip trailing newline.
// returns length of string read (>= 0)
// or <0: -errno from open or read.
// filename is printf-style
int rtapi_fs_read(char *buf, const size_t maxlen, const char *name, ...);


/*
 * Look up a parameter value in rtapi.ini [global] section.  Returns 0 if
 * successful, 1 otherwise.  Maximum n-1 bytes of the value and a
 * trailing \0 is copied into *result.
 *
 * Beware:  this function calls exit(-1) if rtapi.ini cannot be
 * successfully opened!
 */

extern int get_rtapi_config(char *result, const char *param, int n);

// inspection of Elf objects (.so, .ko):
// retrieve raw data of Elf section section_name.
// returned in *dest on success.
// caller must free().
// returns size, or < 0 on failure.
int get_elf_section(const char *const fname, const char *section_name, void **dest);

// split the null-delimited strings in an .rtapi_caps Elf section into an argv.
// caller must free.
const char **get_caps(const char *const fname);

// given a path to an elf binary, and a capability name, return its value
// or NULL if not present.
// caller must free().
const char *get_cap(const char *const fname, const char *cap);


// given a module name, return the integer capability mask of tags.
int rtapi_get_tags(const char *mod_name);


SUPPORT_END_DECLS

#endif // RTAPI_COMPAT_H

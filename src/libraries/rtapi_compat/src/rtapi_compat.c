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

// miscellaneous functions, mostly used during startup in
// a user process; neither RTAPI nor ULAPI and universally
// available to user processes

#include "config.h"
#include "rtapi.h"
#include "rtapi_compat.h"
#include "mk-inifile.h"         /* iniFind() */

#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <limits.h>		/* PATH_MAX */
#include <stdlib.h>		/* exit() */
#include <string.h>		/* exit() */
#include <errno.h>              // errno
#include <grp.h>                // getgroups
#include <spawn.h>              // posix_spawn
#include <sys/wait.h>           // wait_pid

#include <elf.h>                // get_rpath()
#include <link.h>

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE // Get GNU-specific strerror_r() behavior
#endif

static FILE *rtapi_inifile = NULL;

void check_rtapi_config_open()
{
    /* Open rtapi.ini if needed.  Private function used by
       get_rtapi_config(). */
    char config_file[PATH_MAX];
    char *fname = getenv("RTAPI_INI");

    if (rtapi_inifile == NULL) {
	/* it's the first -i (ignore repeats) */
	/* there is a following arg, and it's not an option */
	snprintf(config_file, PATH_MAX,
		 "%s/rtapi.ini", HAL_SYSTEM_CONFIG_DIR);
	rtapi_inifile = fopen((fname ? fname : config_file), "r");
	if (rtapi_inifile == NULL) {
	    fprintf(stderr,
		    "Could not open ini file '%s'\n",
		    (fname ? fname : config_file));
	    exit(-1);
	}
	/* make sure file is closed on exec() */
	fcntl(fileno(rtapi_inifile), F_SETFD, FD_CLOEXEC);
    }
}

char *get_rtapi_param(const char *param)
{
    char *val;

    // Open rtapi_inifile if it hasn't been already
    check_rtapi_config_open();
    val = (char *) iniFind(rtapi_inifile, param, "global");

    return val;
}

int get_rtapi_config(char *result, const char *param, int n)
{
    /* Read a parameter value from rtapi.ini.  Copy max n-1 bytes into result
       buffer.  */
    char *val;

    val = get_rtapi_param(param);

    // Return if nothing found
    if (val==NULL) {
	result[0] = 0;
	return -1;
    }

    // Otherwise copy result into buffer (see 'WTF' comment in inifile.cc)
    strncpy(result, val, n-1);
    return 0;
}

// whatever is written is printf-style
int rtapi_fs_write(const char *path, const char *format, ...)
{
    va_list args;
    int fd;
    int retval = 0;
    char buffer[4096];

    if ((fd = open(path,O_WRONLY)) > -1) {
	int len;
	va_start(args, format);
	len = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	retval = write(fd, buffer, len);
	close(fd);
	return retval;
    } else
	return -ENOENT;
}

// filename is printf-style
int rtapi_fs_read(char *buf, const size_t maxlen, const char *name, ...)
{
    char fname[4096];
    va_list args;

    va_start(args, name);
    size_t len = vsnprintf(fname, sizeof(fname), name, args);
    va_end(args);

    if (len < 1)
    return -EINVAL; // name too short

    int fd, rc;
    if ((fd = open(fname, O_RDONLY)) >= 0) {
    rc = read(fd, buf, maxlen);
    close(fd);
    if (rc < 0)
        return -errno;
    char *s = strchr(buf, '\n');
    if (s) *s = '\0';
    return strlen(buf);
    } else {
    return -errno;
    }
}

int get_elf_section(const char *const fname, const char *section_name, void **dest)
{
    int size = -1, i;
    struct stat st;
    char errmsg[200];

    if (stat(fname, &st) != 0) {
        rtapi_print_msg(
            RTAPI_MSG_ERR,
            "get_elf_section(%s, %s) stat:  %s",
            fname, section_name, strerror_r(errno, errmsg, 200));
	return -1;
    }
    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
        rtapi_print_msg(
            RTAPI_MSG_ERR,
            "get_elf_section(%s, %s) open:  %s",
            fname, section_name, strerror_r(errno, errmsg, 200));
	return fd;
    }
    char *p = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (p == NULL) {
        rtapi_print_msg(
            RTAPI_MSG_ERR,
            "get_elf_section(%s, %s) mmap:  %s",
            fname, section_name, strerror_r(errno, errmsg, 200));
        close(fd);
	return -1;
    }

    switch (p[EI_CLASS]) 	{
    case ELFCLASS32:
	{
	    Elf32_Ehdr *ehdr = (Elf32_Ehdr*)p;
	    Elf32_Shdr *shdr = (Elf32_Shdr *)(p + ehdr->e_shoff);
	    int shnum = ehdr->e_shnum;

	    Elf32_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
	    const char *const sh_strtab_p = p + sh_strtab->sh_offset;
	    for (i = 0; i < shnum; ++i) {
		if (strcmp(sh_strtab_p + shdr[i].sh_name, section_name) == 0) {
		    size  = shdr[i].sh_size;
		    if (!size)
			continue;
		    if (dest) {
			*dest = malloc(size);
			if (*dest == NULL) {
                            rtapi_print_msg(
                                RTAPI_MSG_ERR,
                                "get_elf_section(%s, %s) malloc:  %s",
                                fname, section_name,
                                strerror_r(errno, errmsg, 200));
			    size = -1;
			    break;
			}
			memcpy(*dest, p + shdr[i].sh_offset, size);
			break;
		    }
		}
	    }
	}
	break;

    case ELFCLASS64:
	{
	    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)p;
	    Elf64_Shdr *shdr = (Elf64_Shdr *)(p + ehdr->e_shoff);
	    int shnum = ehdr->e_shnum;

	    Elf64_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
	    const char *const sh_strtab_p = p + sh_strtab->sh_offset;
	    for (i = 0; i < shnum; ++i) {
		if (strcmp(sh_strtab_p + shdr[i].sh_name, section_name) == 0) {
		    size  = shdr[i].sh_size;
		    if (!size)
			continue;
		    if (dest) {
			*dest = malloc(size);
			if (*dest == NULL) {
                            rtapi_print_msg(
                                RTAPI_MSG_ERR,
                                "get_elf_section(%s, %s) malloc:  %s",
                                fname, section_name,
                                strerror_r(errno, errmsg, 200));
			    size = -1;
			    break;
			}
			memcpy(*dest, p + shdr[i].sh_offset, size);
			break;
		    }
		}
	    }
	}
	break;
    default:
	fprintf(stderr, "%s: Unknown ELF class %d\n", fname, p[EI_CLASS]);
    }
    munmap(p, st.st_size);
    close(fd);
    return size;
}

const char **get_caps(const char *const fname)
{
    void  *dest;
    int n = 0;
    char *s;
    char errmsg[200];

    int csize = get_elf_section(fname, RTAPI_TAGS, &dest);
    if (csize < 0)
	return 0;

    for (s = dest; s < ((char *)dest + csize); s += strlen(s) + 1)
	n++;

    const char **rv = malloc(sizeof(char*) * (n+1));
    if (rv == NULL) {
        rtapi_print_msg(
            RTAPI_MSG_ERR, "get_caps(%s) malloc:  %s",
            fname, strerror_r(errno, errmsg, 200));
	return NULL;
    }
    n = 0;
    for (s = dest;
	 s < ((char *)dest+csize);
	 s += strlen(s)+1)
	rv[n++] = s;

    rv[n] = NULL;
    return rv;
}

const char *get_cap(const char *const fname, const char *cap)
{
    if ((cap == NULL) || (fname == NULL))
	return NULL;

    const char **cv = get_caps(fname);
    if (cv == NULL)
	return NULL;

    const char **p = cv;
    size_t len = strlen(cap);

    while (p && *p && strlen(*p)) {
	if (strncasecmp(*p, cap, len) == 0) {
	    const char *result = strdup(*p + len + 1); // skip over '='
	    free(cv);
	    return result;
	}
    }
    free(cv);
    return NULL;
}

int rtapi_get_tags(const char *mod_name)
{
    char modpath[PATH_MAX];
    int result = 0, n = 0;
    char *cp1 = "";
    char errmsg[200];

    if (get_rtapi_config(modpath,"RTLIB_DIR",PATH_MAX) != 0) {
        rtapi_print_msg(
            RTAPI_MSG_ERR, "rtapi_compat.c:  Can't get RTLIB_DIR:  %s",
            strerror_r(errno, errmsg, 200));
        return -1;
    }
    // TODO: This is a terrible way how to do this, rework it to use the set of loaded modules
    //       and dlinfo querying! (Like the rtapi_app_module.hh is doing.)
    strcat(modpath,"/mod");
    strcat(modpath,mod_name);
    strcat(modpath, ".so");

    const char **caps = get_caps(modpath);
    char **p = (char **)caps;
    while (p && *p && strlen(*p)) {
	cp1 = *p++;
	if (strncmp(cp1,"HAL=", 4) == 0) {
	    n = strtol(&cp1[4], NULL, 10);
	    result |=  n ;
	}
    }
    free(caps);
    return result;
}

// those are ok to use from userland RT modules:
#if defined(RTAPI)
EXPORT_SYMBOL(rtapi_fs_read);
EXPORT_SYMBOL(rtapi_fs_write);
#endif

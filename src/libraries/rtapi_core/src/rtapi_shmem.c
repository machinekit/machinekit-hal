/********************************************************************
* Description:  rtapi_shmem.c
*
*               This file, 'rtapi_shmem.c', implements the shared
*               memory-related functions for realtime modules.  See
*               rtapi.h for more info.
*
*     Copyright 2006-2013 Various Authors
*
*     This program is free software; you can redistribute it and/or modify
*     it under the terms of the GNU General Public License as published by
*     the Free Software Foundation; either version 2 of the License, or
*     (at your option) any later version.
*
*     This program is distributed in the hope that it will be useful,
*     but WITHOUT ANY WARRANTY; without even the implied warranty of
*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*     GNU General Public License for more details.
*
*     You should have received a copy of the GNU General Public License
*     along with this program; if not, write to the Free Software
*     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
********************************************************************/

#include "config.h"		// build configuration
#include "rtapi.h"		// these functions
#include "rtapi_common.h"
#include "shmdrv.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>          //shm_open
#include <stdlib.h>		/* rand_r() */
#include <unistd.h>		/* getuid(), getgid(), sysconf(),
				   ssize_t, _SC_PAGESIZE */
#include <string.h>             // memset()

#define SHM_PERMISSIONS	0666

/***********************************************************************
*                           USERLAND THREADS                           *
************************************************************************/

int rtapi_shmem_new_inst(int userkey, int instance, int module_id, unsigned long int size) {
    shmem_data *shmem;
    int i, ret, actual_size;
    int is_new = 0;
    int key = OS_KEY(userkey, instance);
    static int page_size;

    if (!page_size)
	page_size = sysconf(_SC_PAGESIZE);


    rtapi_mutex_get(&(rtapi_data->mutex));
    for (i = 1 ; i < RTAPI_MAX_SHMEMS; i++) {
	if (shmem_array[i].magic == SHMEM_MAGIC && shmem_array[i].key == key) {
	    shmem_array[i].count ++;
	    rtapi_mutex_give(&(rtapi_data->mutex));
	    return i;
	}
	if (shmem_array[i].magic != SHMEM_MAGIC)
	    break;
    }
    if (i == RTAPI_MAX_SHMEMS) {
	rtapi_mutex_give(&(rtapi_data->mutex));
	rtapi_print_msg(RTAPI_MSG_ERR,
			"rtapi_shmem_new failed due to RTAPI_MAX_SHMEMS\n");
	return -ENOMEM;
    }
    shmem = &shmem_array[i];

    // redefine size == 0 to mean 'attach only, dont create'
    actual_size = size;
    ret = shm_common_new(key, &actual_size, instance, &shmem->mem, size > 0);
    if (ret > 0)
	is_new = 1;
    if (ret < 0) {
	 rtapi_mutex_give(&(rtapi_data->mutex));
	 rtapi_print_msg(RTAPI_MSG_ERR,
			 "shm_common_new:%d failed key=0x%x size=%ld\n",
			 instance, key, size);
	 return ret;
    }
    // a non-zero size was given but it didn match what we found:
    if (size && (actual_size != size)) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"rtapi_shmem_new:%d 0x8.8%x: requested size %ld"
			" and actual size %d dont match\n",
			instance, key, size, actual_size);
    }
    /* Touch each page by either zeroing the whole mem (if it's a new
       SHM region), or by reading from it. */
    if (is_new) {
	memset(shmem->mem, 0, size);
    } else {
	unsigned int i;

	for (i = 0; i < size; i += page_size) {
	    unsigned int x = *(volatile unsigned int *)
		((unsigned char *)shmem->mem + i);
	    /* Use rand_r to clobber the read so GCC won't optimize it
	       out. */
	    rand_r(&x);
	}
    }

    /* label as a valid shmem structure */
    shmem->magic = SHMEM_MAGIC;
    /* fill in the other fields */
    shmem->size = actual_size;
    shmem->key = key;
    shmem->count = 1;
    shmem->instance = instance;

    rtapi_mutex_give(&(rtapi_data->mutex));

    /* return handle to the caller */
    return i;
}

int rtapi_shmem_getptr_inst(int handle, int instance, void **ptr, unsigned long *size) {
    shmem_data *shmem;
    if (handle < 1 || handle >= RTAPI_MAX_SHMEMS)
	return -EINVAL;

    shmem = &shmem_array[handle];

    /* validate shmem handle */
    if (shmem->magic != SHMEM_MAGIC)
	return -ENOENT;

    /* pass memory address back to caller */
    *ptr = shmem->mem;
    if (size)
	*size = shmem->size;
    return 0;
}

int rtapi_shmem_delete_inst(int handle, int instance, int module_id) {
    shmem_data *shmem;
    int retval = 0;

    if(handle < 1 || handle >= RTAPI_MAX_SHMEMS)
	return -EINVAL;

    rtapi_mutex_get(&(rtapi_data->mutex));
    shmem = &shmem_array[handle];

    /* validate shmem handle */
    if (shmem->magic != SHMEM_MAGIC) {
	rtapi_mutex_give(&(rtapi_data->mutex));
	return -EINVAL;
    }

    shmem->count --;
    if(shmem->count) {
	rtapi_mutex_give(&(rtapi_data->mutex));
	rtapi_print_msg(RTAPI_MSG_DBG,
			"rtapi_shmem_delete: handle=%d module=%d key=0x%x:  "
			"%d remaining users\n",
			handle, module_id, shmem->key, shmem->count);
	return 0;
    }

    retval = shm_common_detach(shmem->size, shmem->mem);
    if (retval < 0) {
	rtapi_print_msg(RTAPI_MSG_ERR,
			"RTAPI:%d ERROR: munmap(0x%8.8x) failed: %s\n",
			instance, shmem->key, strerror(-retval));
    }

    // XXX: probably shmem->mem should be set to NULL here to avoid
    // references to already unmapped segments (and find them early)

    /* free the shmem structure */
    shmem->magic = 0;
    rtapi_mutex_give(&(rtapi_data->mutex));
    return retval;
}

int rtapi_shmem_exists(int userkey) {
    return shm_common_exists(userkey);
}


// implement rtapi_shmem_* calls  in terms of rtapi_shmem_*_inst()

int rtapi_shmem_new(int userkey, int module_id, unsigned long int size) {
    return rtapi_shmem_new_inst(userkey, rtapi_instance, module_id, size);
}

int rtapi_shmem_getptr(int handle, void **ptr) {
    unsigned long size;
    return rtapi_shmem_getptr_inst(handle, rtapi_instance, ptr, &size);
}

int rtapi_shmem_getsize(int handle, unsigned long int *size) {
    void *ptr;
    return rtapi_shmem_getptr_inst(handle, rtapi_instance, &ptr, size);
}

int rtapi_shmem_delete(int handle, int module_id) {
    return rtapi_shmem_delete_inst(handle, rtapi_instance, module_id);
}

#ifdef RTAPI
EXPORT_SYMBOL(rtapi_shmem_new_inst);
EXPORT_SYMBOL(rtapi_shmem_getptr_inst);
EXPORT_SYMBOL(rtapi_shmem_delete_inst);
EXPORT_SYMBOL(rtapi_shmem_exists);
EXPORT_SYMBOL(rtapi_shmem_new);
EXPORT_SYMBOL(rtapi_shmem_getptr);
EXPORT_SYMBOL(rtapi_shmem_getsize);
EXPORT_SYMBOL(rtapi_shmem_delete);
#endif

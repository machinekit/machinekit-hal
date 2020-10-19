#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "rtapi.h"
#include "rtapi_compat.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define HW_REGS_SPAN ( 65536 )
#define MAX_ADDR 1020

#define MAXUIOIDS  100
#define MAXNAMELEN 256

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

static void show_usage(void)
{
    printf("mksocmemio: Utility to read or write hm2socfpga memory locatons\n");
    printf("Note: the mksocfpga uio0 driver needs to be active\n ");
    printf("To input Hexadecimal Address and data values, preceed the number with 0x:\n");
    printf("Usage options:\n");
    printf(" -h For this help message.\n");
    printf(" -r For reading an address: [-r <address>]\n");
    printf(" -w For writing an address: [-w <address> <value>]\n");
}


int main ( int argc, char *argv[] )
{
    void *virtual_base;
    void *h2p_lw_axi_mem_addr=NULL;
    int fd;
    u_int32_t index, inval;
    char *uio_dev;
    
    // Open /dev/uio0
    char buf[MAXNAMELEN];
    int uio_id;
    for (uio_id = 0; uio_id < MAXUIOIDS; uio_id++) {
        if (rtapi_fs_read(buf, MAXNAMELEN, "/sys/class/uio/uio%d/name", uio_id) < 0)
            continue;
        if (strncmp("hm2-socfpga0", buf, strlen("hm2-socfpga0")) == 0)
            break;
    }
    if (uio_id >= MAXUIOIDS) {
        printf(" didn't find a valid hm2-socfpga0 uio device\n");
        return -1;
    }
    snprintf(buf, sizeof(buf), "/dev/uio%d", uio_id);
    uio_dev = strdup(buf);
    printf("located hm2-socfpga0 on %s\n", uio_dev);

    if ( ( fd = open ( uio_dev, ( O_RDWR | O_SYNC ) ) ) == -1 ) {
        printf ( "    ERROR: could not open %s ...\n", uio_dev);
        return ( 1 );
    }
    
    // get virtual addr that maps to physical
    virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, 0);
    
    if ( virtual_base == MAP_FAILED ) {
        printf ( "    ERROR: mmap() failed...\n" );
        
        close ( fd );
        return ( 1 );
    }
    // Get the base address that maps to the device
    //    assign pointer
    h2p_lw_axi_mem_addr=virtual_base;
    if (argc > 2) {
        if(argv[2][0] == '0' && argv[2][1] == 'x') {
            printf("Hex address value input found\n");
            index = (u_int32_t) strtoul(argv[2], NULL, 16);
        }
        else {
            printf("Assuming decimal address value input\n");
            index = (u_int32_t) strtoul(argv[2], NULL, 10);
        }
        u_int32_t value = *((u_int32_t *)(h2p_lw_axi_mem_addr + index));
    
        switch (argv[1][1]) {
            case 'r':
                printf("Read: ");
                printf("Address   0x%08X\t%u\n \tvalue = 0x%08X\t%u \n", index, index, value, value);
                break;
            case 'w':
                if (argc == 4) {
                    printf("Write: ");
//                    printf("Address   0x%08X\t %u will be set to\nvalue   0x%08X \t %u \n", index, index, value, value);
                    if(argv[3][0] == '0' && argv[3][1] == 'x') {
                        inval = (u_int32_t) strtoul(argv[3], NULL, 16);
                    }
                    else {
                        inval = (u_int32_t) strtoul(argv[3], NULL, 10);
                    }
                    *((u_int32_t *)(h2p_lw_axi_mem_addr + index)) = inval;
                    printf("Wrote:  0x%08X\t %u\tto Address --> 0x%08X\t%u \n", inval, inval, index, index);                    
                } else {
                    printf("Value missing use: mksocmemio -h to show valid options and argunemts\n");
                }
                break;
            default:
                printf("Wrong option: %s\n use: mksocmemio -h to show valid options and argunemts\n", argv[1]);
                break;
        }

    }
    else if (argc == 2) {
        switch (argv[1][1]){
            case 'h':
                show_usage();
                break;
            default:
                printf("Wrong option %s\n use: mksocmemio -h to show valid options and argunemts\n", argv[1]);
                break;
        }
    }
    close ( fd );
    return (0);
}

/*    This is a component of Machinekit
 *    Copyright 2013,2014 Michael Geszkiewicz <micges@wp.pl>,
 *    Jeff Epler <jepler@unpythonic.net>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"
#include "config_module.h"
#include RTAPI_INC_SLAB_H
#include RTAPI_INC_CTYPE_H
#include RTAPI_INC_STRING_H

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <spawn.h>
 #include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "rtapi.h"
#include "rtapi_app.h"
#include "rtapi_string.h"

#include "hal.h"

#include "hostmot2-lowlevel.h"
#include "hostmot2.h"
#include "hm2_eth.h"

struct kvlist {
    struct list_head list;
    char key[16];
    int value;
};

static struct list_head board_num;
static struct list_head ifnames;

static int *kvlist_lookup(struct list_head *head, const char *name) {
    struct list_head *ptr;
    list_for_each(ptr, head) {
        struct kvlist *ent = list_entry(ptr, struct kvlist, list);
        if(strncmp(name, ent->key, sizeof(ent->key)) == 0) return &ent->value;
    }
    struct kvlist *ent = kzalloc(sizeof(struct kvlist), GFP_KERNEL);
    strncpy(ent->key, name, sizeof(ent->key));
    list_add(&ent->list, head);
    return &ent->value;
}

static void kvlist_free(struct list_head *head) {
    struct list_head *orig_head = head;
    for(head = head->next; head != orig_head;) {
        struct list_head *ptr = head;
        head = head->next;
        struct kvlist *ent = list_entry(ptr, struct kvlist, list);
        list_del(ptr);
        kfree(ent);
    }
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Geszkiewicz");
MODULE_DESCRIPTION("Driver for HostMot2 on the 7i80 Anything I/O board from Mesa Electronics");
#ifdef MODULE_SUPPORTED_DEVICE
MODULE_SUPPORTED_DEVICE("Mesa-AnythingIO-7i80");
#endif

static char *board_ip[MAX_ETH_BOARDS];
RTAPI_MP_ARRAY_STRING(board_ip, MAX_ETH_BOARDS, "ip address of ethernet board(s)");

static char *config[MAX_ETH_BOARDS];
RTAPI_MP_ARRAY_STRING(config, MAX_ETH_BOARDS, "config string for the AnyIO boards (see hostmot2(9) manpage)")

int debug = 0;
RTAPI_MP_INT(debug, "Developer/debug use only!  Enable debug logging.");

static int boards_count = 0;

int comm_active = 0;

static int comp_id;

#define UDP_PORT 27181
#define SEND_TIMEOUT_US 10
#define RECV_TIMEOUT_US 100        // 100000 nanoseconds, which was the minimum recv timeout before anyway
#define READ_PCK_DELAY_NS 10000

static hm2_eth_t boards[MAX_ETH_BOARDS];

/// ethernet io functions

static int eth_socket_send(int sockfd, const void *buffer, int len, int flags);
static int eth_socket_recv(int sockfd, void *buffer, int len, int flags);

#define IPTABLES "/sbin/iptables"
#define CHAIN "hm2-eth-rules-output"

static int shell(char *command) {
    char *const argv[] = {"sh", "-c", command, NULL};
    pid_t pid;
    int res = posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
    if (res < 0) perror("posix_spawn");
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    else if (WIFSTOPPED(status)) return WTERMSIG(status)+128;
    else return status;
}

static int eshellf(char *fmt, ...) {
    char commandbuf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(commandbuf, sizeof(commandbuf), fmt, ap);
    va_end(ap);

    int res = shell(commandbuf);
    if (res == EXIT_SUCCESS) return 0;

    LL_PRINT("ERROR: Failed to execute '%s'\n", commandbuf);
    return -EINVAL;
}

static bool chain_exists() {
    int result =
        shell(IPTABLES" -n -L "CHAIN" > /dev/null 2>&1");
    return result == EXIT_SUCCESS;
}

static int iptables_state = -1;
static bool use_iptables() {
    if (iptables_state == -1) {
        if (geteuid() != 0) return (iptables_state = 0);
        if (!chain_exists()) {
            int res = shell("/sbin/iptables -N " CHAIN);
            if (res != EXIT_SUCCESS) {
                LL_PRINT("ERROR: Failed to create iptables chain "CHAIN);
                return (iptables_state = 0);
            }
        }
        // now add a jump to our chain at the start of the OUTPUT chain if it isn't in the chain already
        int res = shell("/sbin/iptables -C OUTPUT -j " CHAIN " || /sbin/iptables -I OUTPUT 1 -j " CHAIN);
        if (res != EXIT_SUCCESS) {
            LL_PRINT("ERROR: Failed to insert rule in OUTPUT chain");
            return (iptables_state = 0);
        }
        return (iptables_state = 1);
    }
    return iptables_state;
}

static void clear_iptables() {
    shell(IPTABLES" -F "CHAIN" > /dev/null 2>&1");
}

static char* inet_ntoa_buf(struct in_addr in, char *buf, size_t n) {
    const char *addr = inet_ntoa(in);
    rtapi_snprintf(buf, n, "%s", addr);
    return buf;
}

static char* fetch_ifname(int sockfd, char *buf, size_t n) {
    struct sockaddr_in srcaddr;
    struct ifaddrs *ifa, *it;

    socklen_t addrlen = sizeof(srcaddr);
    int res = getsockname(sockfd, &srcaddr, &addrlen);
    if (res < 0) return NULL;

    if (getifaddrs(&ifa) < 0) {
        perror("getifaddrs");
        return NULL;
    }

    for (it=ifa; it; it=it->ifa_next) {
        struct sockaddr_in *ifaddr = (struct sockaddr_in*)it->ifa_addr;
        if (ifaddr == NULL) continue;
        if (ifaddr->sin_family != srcaddr.sin_family) continue;
        if (ifaddr->sin_addr.s_addr != srcaddr.sin_addr.s_addr) continue;
        rtapi_snprintf(buf, n, "%s", it->ifa_name);
        freeifaddrs(ifa);
        return buf;
    }

    freeifaddrs(ifa);
    return NULL;
}

static char *vseprintf(char *buf, char *ebuf, const char *fmt, va_list ap) {
    int result = rtapi_vsnprintf(buf, ebuf-buf, fmt, ap);
    if (result < 0) return ebuf;
    else if (buf + result > ebuf) return ebuf;
    else return buf + result;
}

static char *seprintf(char *buf, char *ebuf, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char *result = vseprintf(buf, ebuf, fmt, ap);
    va_end(ap);
    return result;
}

static int install_iptables_rule(const char *fmt, ...) {
    char commandbuf[1024], *ptr = commandbuf,
        *ebuf = commandbuf + sizeof(commandbuf);
    ptr = seprintf(ptr, ebuf, IPTABLES" -A "CHAIN" ");
    va_list ap;
    va_start(ap, fmt);
    ptr = vseprintf(ptr, ebuf, fmt, ap);
    va_end(ap);

    if (ptr == ebuf)
    {
        LL_PRINT("ERROR: commandbuf too small\n");
        return -ENOSPC;
    }

    int res = shell(commandbuf);
    if (res == EXIT_SUCCESS) return 0;

    LL_PRINT("ERROR: Failed to execute '%s'\n", commandbuf);
    return -EINVAL;
}

static int install_iptables_board(int sockfd) {
    struct sockaddr_in srcaddr, dstaddr;
    char srchost[16], dsthost[16]; // enough for 255.255.255.255\0

    socklen_t addrlen = sizeof(srcaddr);
    int res = getsockname(sockfd, &srcaddr, &addrlen);
    if (res < 0) return -errno;

    addrlen = sizeof(dstaddr);
    res = getpeername(sockfd, &dstaddr, &addrlen);
    if (res < 0) return -errno;

    res = install_iptables_rule(
        "-p udp -m udp -d %s --dport %d -s %s --sport %d -j ACCEPT",
        inet_ntoa_buf(dstaddr.sin_addr, dsthost, sizeof(dsthost)),
        ntohs(dstaddr.sin_port),
        inet_ntoa_buf(srcaddr.sin_addr, srchost, sizeof(srchost)),
        ntohs(srcaddr.sin_port));
    return res;
}

static int install_iptables_perinterface(const char *ifbuf) {
    // without this rule, 'ping' spews a lot of messages like
    //    From 192.168.1.1 icmp_seq=5 Packet filtered
    // many times for each ping packet sent.  With this rule,
    // ping prints 'ping: sendmsg: Operation not permitted' once
    // per second.
    int res = install_iptables_rule(
        "-o %s -p icmp -j DROP",
        ifbuf);
    if (res < 0) return res;

    res = install_iptables_rule(
        "-o %s -j REJECT --reject-with icmp-admin-prohibited",
        ifbuf);
    if (res < 0) return res;

    res = eshellf("/sbin/sysctl -q net.ipv6.conf.%s.disable_ipv6=1", ifbuf);
    if (res < 0) return res;

    return 0;
}

static int fetch_hwaddr(const char *board_ip, int sockfd, unsigned char buf[6]) {
    lbp16_cmd_addr packet;
    unsigned char response[6];
    LBP16_INIT_PACKET4(packet, 0x4983, 0x0002);
    int res = eth_socket_send(sockfd, &packet, sizeof(packet), 0);
    if (res < 0) return -errno;

    int i=0;
    do {
        res = eth_socket_recv(sockfd, &response, sizeof(response), 0);
    } while (++i < 10 && res < 0 && errno == EAGAIN);
    if (res < 0) return -errno;

    // eeprom order is backwards from arp AF_LOCAL order
    for (i=0; i<6; i++) buf[i] = response[5-i];

    LL_PRINT("%s: Hardware address: %02x:%02x:%02x:%02x:%02x:%02x\n",
        board_ip, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

    return 0;
}

// return value indicates if still under the error limit ceiling or not
// if returns false, the persistent io_error flag has been set and requires
// operator intervention)
static bool record_soft_error(hm2_eth_t *board) {
    if (!board->hal) {
        LL_PRINT("Ignoring soft error during init\n");
        return true;   // still early in hm2_eth_probe
    }

    // From what I discern, the llio driver here can tolerate some limited packet loss (or timeouts on reads).
    // The soft reset flag triggers later write's to do a full force write to refresh the FPGA in case
    // its out of sync I guess.
    board->llio.needs_soft_reset = 1;
    *board->hal->packet_error = 1;

    int32_t increment = *board->hal->packet_error_increment;

    // packet_error_increment can be set by other code so sanity check it a little
    if (increment < 1) increment = 1;
    board->comm_error_counter += increment;

    // Bound the error counter to sane values (because the error increment and decrement amounts are
    // exposed in hal and could be tweaked by some other code)
    if (board->comm_error_counter < 0 || board->comm_error_counter > *board->hal->packet_error_limit)
        board->comm_error_counter = *board->hal->packet_error_limit;

    *board->hal->packet_error_level = board->comm_error_counter;

    if (board->comm_error_counter == *board->hal->packet_error_limit) {
        // had enough packet errors to trip the persistent io_error state that requires user intervention
        *board->hal->packet_error_exceeded = 1;
        *board->llio.io_error = 1;
        LL_PRINT("Recording soft error that tripped hard io_error. iter=%d\n", board->read_cnt);
        return false;
    } else {
        // counted an error, but it hasn't reached the critical level yet.
        //
        // note the original code didn't reset io_error here, it only set it to true if limit was hit
        // io_error indicates a "permanent" error that needs user intervention to fix
        *board->hal->packet_error_exceeded = 0;
        LL_PRINT("Recording soft error. iter=%d\n", board->read_cnt);
        return true;
    }
}

static void decrement_soft_error(hm2_eth_t *board) {
    if (!board->hal) return; // still early in hm2_eth_probe

    // packet_error_decrement can be set by other code so sanity check it a little
    int32_t decrement = *board->hal->packet_error_decrement;
    if (decrement < 1) decrement = 1;

    board->comm_error_counter -= decrement;
    if (board->comm_error_counter < 0) board->comm_error_counter = 0;
    *board->hal->packet_error_level = board->comm_error_counter;
    *board->hal->packet_error = 0;
    *board->hal->packet_error_exceeded = 0;
}

static int init_board(hm2_eth_t *board, const char *board_ip) {
    int ret;

    board->sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (board->sockfd < 0) {
        LL_PRINT("ERROR: can't open socket: %s\n", strerror(errno));
        return -errno;
    }
    board->server_addr.sin_family = AF_INET;
    board->server_addr.sin_port = htons(LBP16_UDP_PORT);
    board->server_addr.sin_addr.s_addr = inet_addr(board_ip);

    board->local_addr.sin_family      = AF_INET;
    board->local_addr.sin_addr.s_addr = INADDR_ANY;

    ret = connect(board->sockfd, (struct sockaddr *) &board->server_addr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        LL_PRINT("ERROR: can't connect: %s\n", strerror(errno));
        return -errno;
    }

    if (!use_iptables()) {
        LL_PRINT(\
"WARNING: Unable to restrict other access to the hm2-eth device.\n"
"This means that other software using the same network interface can violate\n"
"realtime guarantees.  See hm2_eth(9) for more information.\n");
    }

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = RECV_TIMEOUT_US;

    ret = setsockopt(board->sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
    if (ret < 0) {
        LL_PRINT("ERROR: can't set socket option: %s\n", strerror(errno));
        return -errno;
    }

    timeout.tv_usec = SEND_TIMEOUT_US;
    setsockopt(board->sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    if (ret < 0) {
        LL_PRINT("ERROR: can't set socket option: %s\n", strerror(errno));
        return -errno;
    }

    // Manually create an ARP entry over to the mesa board so the IP stack doesn't need to
    // figure that out.

    memset(&board->req, 0, sizeof(board->req));
    struct sockaddr_in *sin;

    sin = (struct sockaddr_in *) &board->req.arp_pa;
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = inet_addr(board_ip);

    board->req.arp_ha.sa_family = AF_LOCAL;
    board->req.arp_flags = ATF_PERM | ATF_COM;
    ret = fetch_hwaddr( board_ip, board->sockfd, (void*)&board->req.arp_ha.sa_data );
    if (ret < 0) {
        LL_PRINT("ERROR: %s: Could not retrieve mac address\n", board_ip);
        return ret;
    }

    ret = ioctl(board->sockfd, SIOCSARP, &board->req);
    if (ret < 0) {
        LL_PRINT("ERROR: ioctl SIOCSARP failed: %s\n", strerror(errno));
        board->req.arp_flags &= ~ATF_PERM;
        return -errno;
    }

    // Setup firewall rules

    if (use_iptables())
    {
        ret = install_iptables_board(board->sockfd);
        if (ret < 0) return ret;
    }

    board->write_packet_ptr = board->write_packet;
    board->read_packet_ptr = board->read_packet;

    return 0;
}

static int close_board(hm2_eth_t *board) {
    int ret = 0;
    if (board->sockfd != -1) {
        if (use_iptables()) clear_iptables();

        if (board->req.arp_flags & ATF_PERM) {
            ret = ioctl(board->sockfd, SIOCDARP, &board->req);
            if (ret < 0) perror("ioctl SIOCDARP");
        }

        ret = shutdown(board->sockfd, SHUT_RDWR);
        if (ret != 0)
            LL_PRINT("ERROR: can't shutdown socket: %s\n", strerror(errno));

        ret = close(board->sockfd);
        if (ret != 0)
            LL_PRINT("ERROR: can't close socket: %s\n", strerror(errno));

        board->sockfd = -1;
    }
    return ret;
}

static int eth_socket_send(int sockfd, const void *buffer, int len, int flags) {
    return send(sockfd, buffer, len, flags);
}

static int eth_socket_recv(int sockfd, void *buffer, int len, int flags) {
    return recv(sockfd, buffer, len, flags);
}

static int eth_socket_recv_loop(int sockfd, void *buffer, int len, int flags, long timeout_ns) {
    // Seems like this should be using rtapi_get_time() and not rtapi_get_clocks()
    // since the timeout is specified in nanos.
    // Changed it.
    long long end = rtapi_get_time() + timeout_ns;
    int result;
    do {
        result = eth_socket_recv(sockfd, buffer, len, flags);
    } while (result < 0 && rtapi_get_time() < end);
    return result;
}

/// hm2_eth io functions
/// Documented to return 0 on failure.  !0 on success.

static int hm2_eth_read(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    hm2_eth_t *board = this->private;
    int send, recv, attempts = 0;
    u8 tmp_buffer[size + 4];
    long long t1, t2, before_recv_time;
    long int deltat;
    long long read_start_time = rtapi_get_time();

    if (comm_active == 0) return 1;    // Fake success
    if (size == 0) return 1;           // Fake success

    board->read_cnt++;

    // Are we running in a realtime task context?
    if (rtapi_task_self() >= 0) {
        static bool printed = false;
        if (!printed) {
            LL_PRINT("ERROR: used llio->read in realtime task (addr=0x%04x)\n", addr);
            LL_PRINT("This causes additional network packets which hurts performance\n");
            printed = true;
        }
    }

    lbp16_cmd_addr read_packet;

    LBP16_INIT_PACKET4(read_packet, CMD_READ_HOSTMOT2_ADDR32_INCR(size/4), addr & 0xFFFF);

    send = eth_socket_send(board->sockfd, (void*) &read_packet, sizeof(read_packet), 0);
    if (send < 0) {
        LL_PRINT("ERROR: sending packet: %s\n", strerror(errno));
        if (record_soft_error(board))
            return -EAGAIN;
        else
            return 0;   // Don't bother trying to receive the answer when the request wasn't sent successfully...
    }

    LL_PRINT_IF(debug, "read(%d) : PACKET SENT [CMD:%02X%02X | ADDR: %02X%02X | SIZE: %d]\n", board->read_cnt, read_packet.cmd_hi, read_packet.cmd_lo,
      read_packet.addr_lo, read_packet.addr_hi, size);
    t1 = rtapi_get_time();
    before_recv_time = t1;
    do {
        errno = 0;

        // This will block for up to RECV_TIMEOUT_US - which is 100,000 nanoseconds.
        // The hard coded timeout deadline below is 200,000,000 nanoseconds or 200 milliseconds.
        // An immense amount of time, not sure why that was picked...
        recv = eth_socket_recv(board->sockfd, (void*) &tmp_buffer, size, 0);

        t2 = rtapi_get_time();

        // Capture the max time we are ever stuck inside recv calls
        deltat = (long int)(t2 - before_recv_time);
        before_recv_time = t2;
        if ((board->hal) && (*board->hal->packet_recv_tmax < deltat))
            *board->hal->packet_recv_tmax = deltat;

        attempts++;
    } while ((recv < 0) && ((t2 - t1) < 200*1000*1000));

    // Capture the max recv attempts
    if ((board->hal) && (*board->hal->packet_recv_attempts < attempts))
        *board->hal->packet_recv_attempts = attempts;

    if (recv == 4) {
        LL_PRINT_IF(debug, "read(%d) : PACKET RECV [DATA: %08X | SIZE: %d | TRIES: %d | TIME: %llu]\n", board->read_cnt, *tmp_buffer, recv, attempts, t2 - t1);
    } else {
        LL_PRINT_IF(debug, "read(%d) : PACKET RECV [SIZE: %d | TRIES: %d | TIME: %llu]\n", board->read_cnt, recv, attempts, t2 - t1);
    }

    if (recv < 0) {
        if (record_soft_error(board))
            return -EAGAIN;
        else
            return 0;
    }

    memcpy(buffer, tmp_buffer, size);
    decrement_soft_error(board);

    // Capture the max time we ever spend inside hm2_eth_read()
    deltat = (long int) (rtapi_get_time() - read_start_time);
    if ((board->hal) && (*board->hal->packet_read_tmax < deltat))
        *board->hal->packet_read_tmax = deltat;

    return 1;  // success
}

// Returns 0 on failure, !0 on success
static int hm2_eth_send_queued_reads(hm2_lowlevel_io_t *this) {
    hm2_eth_t *board = this->private;
    int send;

    // Check for possible read packet buffer overflow
    if ((board->read_packet_ptr + sizeof(lbp16_cmd_addr)*3 + sizeof(uint32_t)) > board->read_packet + sizeof(board->read_packet)) {
        LL_PRINT("ERROR: read buffer overflow error in hm2_eth_send_queued_reads\n");
        return 0;
    } else if ((board->read_entry_count + 2) >= MAX_ETH_READS) {
        LL_PRINT("ERROR: reached max queued reads already of %d in hm2_eth_send_queued_reads\n", MAX_ETH_READS);
        return 0;
    } else {
        // read (low 16 bits of) last write number from space 4 address 0010
        LBP16_INIT_PACKET4(*(lbp16_cmd_addr*)(board->read_packet_ptr), CMD_READ_COMM_CTRL_ADDR16(1), 0x8);
        board->read_packet_ptr += sizeof(lbp16_cmd_addr);
        board->read_entries[board->read_entry_count].buffer = &board->rxudpcount;
        board->read_entries[board->read_entry_count].size = 2;
        board->read_entries[board->read_entry_count].received_data_offset = board->total_read_buffer_size;
        board->read_entry_count++;
        board->total_read_buffer_size += 2;

        board->read_cnt++;
        // write then read back space 4 scratch register at 0010 to verify we got the right receive packet
        LBP16_INIT_PACKET4(*(lbp16_cmd_addr*)(board->read_packet_ptr), CMD_WRITE_TIMER_ADDR16_INCR(2), 0x10);
        board->read_packet_ptr += sizeof(lbp16_cmd_addr);
        *(uint32_t*)board->read_packet_ptr = board->read_cnt;
        board->read_packet_ptr += sizeof(uint32_t);

        LBP16_INIT_PACKET4(*(lbp16_cmd_addr*)(board->read_packet_ptr), CMD_READ_TIMER_ADDR16_INCR(4), 0x10);
        board->read_packet_ptr += sizeof(lbp16_cmd_addr);
        board->read_entries[board->read_entry_count].buffer = &board->confirm_read_cnt;
        board->read_entries[board->read_entry_count].size = 8;
        board->read_entries[board->read_entry_count].received_data_offset = board->total_read_buffer_size;
        board->read_entry_count++;
        board->total_read_buffer_size += 8;

        send = eth_socket_send(board->sockfd, (void*) &board->read_packet, board->read_packet_ptr - board->read_packet, 0);
        if (send < 0) {
            LL_PRINT("ERROR: sending packet: %s\n", strerror(errno));
            if (record_soft_error(board))
                return -EAGAIN;
            else
                return 0;
        }
        if (debug) {
            int size = board->read_packet_ptr - board->read_packet;
            LL_PRINT("read(%d) : PACKET SENT [SIZE: %d]\n", board->read_cnt, size);
        }
        return 1;
    }
}


// Returns 0 on failure, !0 on success
static int hm2_eth_receive_queued_reads(hm2_lowlevel_io_t *this) {
    hm2_eth_t *board = this->private;
    int recv, attempts = 0;
    u8 tmp_buffer[board->total_read_buffer_size];
    long long t1, t2, before_recv_time;
    long int deltat;
    t1 = rtapi_get_time();

    // an error occurred in the past but the user has reset the io_error
    // pin (or they did something else like fiddle with the error limit
    // during a run, in which case we don't care if we reset the counter
    // or not)
    if(board->hal && board->comm_error_counter == *board->hal->packet_error_limit && !*board->llio.io_error) {
        board->comm_error_counter = 0;
    }

    // So llio.period is documented to be the period (in ns) of the last read-request invocation
    // unsigned long period;
    // It is really the period argument passed into the .read function on the servo thread.
    // At least I think - the docs for adding functions to hal threads.
    // The servo thread period is 1,000,000 nanoseconds or 1 millisecond.

    // Units on all these are nanoseconds.
    long read_timeout = board->hal ? *board->hal->read_timeout : 800000;
    if (read_timeout <= 0)
        read_timeout = 80;

    // If read_timeout is less than 100, it represents a percentage of the servo period so use it to scale the value
    if (read_timeout < 100)//less than 100 is interpreted as a percentage of the thread period.
        read_timeout = (long)((read_timeout * (unsigned long long)board->llio.period) / 100ull);

    // But with a minimum of 0.1 milliseconds.
    if (read_timeout < 100000)
        read_timeout = 100000;

    LL_PRINT_IF(debug, "read timeout %li\n", read_timeout);

    if (!board->hal) this->read_time = t1;
    unsigned long long read_deadline = this->read_time + read_timeout;
    before_recv_time = t1;
    do {
do_recv_packet:
        // This will block for up to RECV_TIMEOUT_US which is 100,000 nanoseconds. This was the floor
        // of the timeout above anyway.  And we pulled out the rtapi_delay thing which was really just
        // a busy wait since it had no discernable benefit.
        errno = 0;
        recv = eth_socket_recv(board->sockfd, (void*) &tmp_buffer, board->total_read_buffer_size, 0);
        t2 = rtapi_get_time();

        // Capture the max time we are ever stuck inside recv calls
        deltat = (long int)(t2 - before_recv_time);
        before_recv_time = t2;
        if ((board->hal) && (*board->hal->packet_recv_tmax < deltat))
            *board->hal->packet_recv_tmax = deltat;

        attempts++;
    } while (recv != board->total_read_buffer_size && t2 < read_deadline);

    // Capture the max recv attempts
    if ((board->hal) && (*board->hal->packet_recv_attempts < attempts))
        *board->hal->packet_recv_attempts = attempts;

    if (recv != board->total_read_buffer_size) {
        // We must have timed out
        board->read_packet_ptr = board->read_packet;
        board->read_entry_count = 0;
        board->total_read_buffer_size = 0;
        if (record_soft_error(board))
            return -EAGAIN;
        else
            return 0;
    }

    LL_PRINT_IF(debug, "enqueue_read(%d) : PACKET RECV [SIZE: %d | TRIES: %d | TIME: %llu]\n", board->read_cnt, recv, attempts, t2 - t1);

    // Now that we have the data, copy it from the local stack buffer to the final destinations that we tracked using
    // the array of hm2_read_entry_t structs.
    int ii;
    for (ii = 0; ii < board->read_entry_count; ii++) {
        memcpy(board->read_entries[ii].buffer, &tmp_buffer[board->read_entries[ii].received_data_offset], board->read_entries[ii].size);
    }

    // This appears to be a "do-over" because the UDP packet "sequence" number wasn't what we wanted
    // and there is more time left to read.
    if (board->confirm_read_cnt != board->read_cnt && t2 < read_deadline) {
        before_recv_time = rtapi_get_time();
        if (board->hal)
            *board->hal->packet_wrong_seq += 1;
        goto do_recv_packet;
    }

    board->read_packet_ptr = board->read_packet;
    board->read_entry_count = 0;
    board->total_read_buffer_size = 0;

    int result = 1;

    if (board->hal) {
        // (this means that one in 2^32 lost writes will not be diagnosed,
        // each time board->write_cnt overflows)
        if (board->write_cnt && board->write_cnt != board->confirm_write_cnt) {
            LL_PRINT("write_cnt: %x   confirm_write_cnt: %x\n", board->write_cnt, board->confirm_write_cnt)
            if (record_soft_error(board))
                result = -EAGAIN;
            else
                result = 0;
        } else {
            decrement_soft_error(board);
        }
    }

    // Capture the max time we ever spend inside hm2_eth_receive_queued_reads()
    deltat = (long int) (rtapi_get_time() - t1);
    if ((board->hal) && (*board->hal->packet_read_tmax < deltat))
        *board->hal->packet_read_tmax = deltat;

    return result;
}

// Returns 0 on failure, !0 on success
static int hm2_eth_enqueue_read(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    hm2_eth_t *board = this->private;
    if (comm_active == 0) return 1;     // Fake success
    if (size == 0) return 1;            // Fake success

    // Check for possible read packet buffer overflow
    if ((board->read_packet_ptr + sizeof(lbp16_cmd_addr)) > board->read_packet + sizeof(board->read_packet)) {
        LL_PRINT("ERROR: read buffer overflow error in hm2_eth_enqueue_read\n");
        return 0;
    } else if (board->read_entry_count >= MAX_ETH_READS) {
        LL_PRINT("ERROR: reached max queued reads already of %d in hm2_eth_enqueue_read\n", MAX_ETH_READS);
        return 0;
    } else {
        LBP16_INIT_PACKET4(*(lbp16_cmd_addr*)board->read_packet_ptr, CMD_READ_HOSTMOT2_ADDR32_INCR(size/4), addr);
        board->read_packet_ptr += sizeof(lbp16_cmd_addr);

        board->read_entries[board->read_entry_count].buffer = buffer;
        board->read_entries[board->read_entry_count].size = size;
        board->read_entries[board->read_entry_count].received_data_offset = board->total_read_buffer_size;
        board->read_entry_count++;
        board->total_read_buffer_size += size;
        return 1;
    }
}

// Forward prototype
static int hm2_eth_enqueue_write(hm2_lowlevel_io_t *this, u32 addr, const void *buffer, int size);

/// Documented to return 0 on failure.  !0 on success.
static int hm2_eth_write(hm2_lowlevel_io_t *this, u32 addr, const void *buffer, int size) {
    // If we running in a realtime task context than treat this as a queued write instead.
    if (rtapi_task_self() >= 0)
        return hm2_eth_enqueue_write(this, addr, buffer, size);

    int send;
    static struct {
        lbp16_cmd_addr wr_packet;
        u8 tmp_buffer[127*8];
    } packet;

    if (comm_active == 0) return 1;
    if (size == 0) return 1;
    hm2_eth_t *board = this->private;
    board->write_cnt++;

    memcpy(packet.tmp_buffer, buffer, size);
    LBP16_INIT_PACKET4(packet.wr_packet, CMD_WRITE_HOSTMOT2_ADDR32_INCR(size/4), addr & 0xFFFF);

    send = eth_socket_send(board->sockfd, (void*) &packet, sizeof(lbp16_cmd_addr) + size, 0);
    if (send < 0) {
        LL_PRINT("ERROR: sending packet: %s\n", strerror(errno));
        record_soft_error(board);
        return 0;
    }

    LL_PRINT_IF(debug, "write(%d): PACKET SENT [CMD:%02X%02X | ADDR: %02X%02X | SIZE: %d]\n", board->write_cnt, packet.wr_packet.cmd_hi, packet.wr_packet.cmd_lo,
      packet.wr_packet.addr_lo, packet.wr_packet.addr_hi, size);

    return 1;  // success
}


// Returns 0 on failure, !0 on success
static int hm2_eth_send_queued_writes(hm2_lowlevel_io_t *this) {
    int send;
    long long t0, t1;
    hm2_eth_t *board = this->private;

    // Check for possible write packet buffer overflow
    if ((board->write_packet_ptr + sizeof(lbp16_cmd_addr) + 4) > board->write_packet + sizeof(board->write_packet)) {
        LL_PRINT("ERROR: write buffer overflow error in hm2_eth_send_queued_writes\n");
        return 0;
    } else {
        board->write_cnt++;
        lbp16_cmd_addr *packet = (lbp16_cmd_addr *) board->write_packet_ptr;
        LBP16_INIT_PACKET4(*packet, CMD_WRITE_TIMER_ADDR16_INCR(2), 0x14);
        board->write_packet_ptr += sizeof(*packet);
        memcpy(board->write_packet_ptr, &board->write_cnt, 4);
        board->write_packet_ptr += 4;
        board->write_packet_size += (sizeof(*packet) + 4);

        t0 = rtapi_get_time();
        send = eth_socket_send(board->sockfd, (void*) &board->write_packet, board->write_packet_size, 0);
        if (send < 0) {
            LL_PRINT("ERROR: sending packet: %s\n", strerror(errno));
            record_soft_error(board);
            return 0;
        }
        t1 = rtapi_get_time();
        LL_PRINT_IF(debug, "enqueue_write(%d) : PACKET SEND [SIZE: %d | TIME: %llu]\n", board->write_cnt, send, t1 - t0);
        board->write_packet_ptr = board->write_packet;
        board->write_packet_size = 0;
        return 1;
    }
}

/// Documented to return 0 on failure.  !0 on success.
static int hm2_eth_enqueue_write(hm2_lowlevel_io_t *this, u32 addr, const void *buffer, int size) {
    hm2_eth_t *board = this->private;
    if (comm_active == 0) return 1;     // Fake success
    if (size == 0) return 1;            // Fake success

    // Check for possible write packet buffer overflow
    if ((board->write_packet_ptr + sizeof(lbp16_cmd_addr) + size) > board->write_packet + sizeof(board->write_packet)) {
        LL_PRINT("ERROR: write buffer overflow error in hm2_eth_enqueue_write\n");
        return 0;
    } else {
        lbp16_cmd_addr *packet = (lbp16_cmd_addr *) board->write_packet_ptr;
        LBP16_INIT_PACKET4_PTR(packet, CMD_WRITE_HOSTMOT2_ADDR32_INCR(size/4), addr);
        board->write_packet_ptr += sizeof(*packet);
        memcpy(board->write_packet_ptr, buffer, size);
        board->write_packet_ptr += size;
        board->write_packet_size += (sizeof(*packet) + size);
        return 1;
    }
}

static int hm2_eth_io_reset(hm2_lowlevel_io_t *this) {
    LL_PRINT("hm2_eth_io_reset called!\n");

    // Figure out which board we should reset
    int ii;
    for (ii = 0; ii < MAX_ETH_BOARDS; ii++) {
        if (this == &(boards[ii].llio)) {
            // Bingo.
            hm2_eth_t* pBoard = &boards[ii];

            LL_PRINT("hm2_eth_io_reset: closing board!\n");
            int ev = close_board(pBoard);
            if (ev == 0) {
                LL_PRINT("hm2_eth_io_reset: init board!\n");
                ev = init_board(pBoard, board_ip[ii]);
            }

            // set io_error if we couldn't successfully reset the board io
            if (ev != 0) {
                LL_PRINT("hm2_eth_io_reset: failed so setting io_error = 1\n");
                *this->io_error = 1;
            }

            return 0;
        }
    }
    return 0;
}

static int llio_idx(const char *llio_name) {
    int *idx = kvlist_lookup(&board_num, llio_name);
    return (*idx)++;
}

static int hm2_eth_probe(hm2_eth_t *board) {
    lbp16_cmd_addr read_packet;

    int ret, send, recv;
    char board_name[16] = {0, };
    char llio_name[16] = {0, };

    LBP16_INIT_PACKET4(read_packet, CMD_READ_BOARD_INFO_ADDR16_INCR(16/2), 0);
    send = eth_socket_send(board->sockfd, (void*) &read_packet, sizeof(read_packet), 0);
    if (send < 0) {
        LL_PRINT("ERROR: sending packet: %s\n", strerror(errno));
        return -errno;
    }
    recv = eth_socket_recv_loop(board->sockfd, (void*) &board_name, 16, 0,
                200 * 1000 * 1000);
    if (recv < 0) {
        LL_PRINT("ERROR: receiving packet: %s\n", strerror(errno));
        return -errno;
    }

    board = &boards[boards_count];
    board->llio.private = board;
    board->llio.split_read = true;

    if (strncmp(board_name, "7I80DB-16", 9) == 0) {
        strncpy(llio_name, board_name, 4);
        llio_name[1] = tolower(llio_name[1]);

        board->llio.num_ioport_connectors = 4;
        board->llio.pins_per_connector = 17;
        board->llio.ioport_connector_name[0] = "J2";
        board->llio.ioport_connector_name[1] = "J3";
        board->llio.ioport_connector_name[2] = "J4";
        board->llio.ioport_connector_name[3] = "J5";
        board->llio.fpga_part_number = "XC6SLX16";
        board->llio.num_leds = 4;
    } else if (strncmp(board_name, "7I80DB-25", 9) == 0) {
        strncpy(llio_name, board_name, 4);
        llio_name[1] = tolower(llio_name[1]);

        board->llio.num_ioport_connectors = 4;
        board->llio.pins_per_connector = 17;
        board->llio.ioport_connector_name[0] = "J2";
        board->llio.ioport_connector_name[1] = "J3";
        board->llio.ioport_connector_name[2] = "J4";
        board->llio.ioport_connector_name[3] = "J5";
        board->llio.fpga_part_number = "XC6SLX25";
        board->llio.num_leds = 4;
    } else if (strncmp(board_name, "7I80HD-16", 9) == 0) {
        strncpy(llio_name, board_name, 4);
        llio_name[1] = tolower(llio_name[1]);

        board->llio.num_ioport_connectors = 3;
        board->llio.pins_per_connector = 24;
        board->llio.ioport_connector_name[0] = "P1";
        board->llio.ioport_connector_name[1] = "P2";
        board->llio.ioport_connector_name[2] = "P3";
        board->llio.fpga_part_number = "XC6SLX16";
        board->llio.num_leds = 4;
    } else if (strncmp(board_name, "7I80HD-25", 9) == 0) {
        strncpy(llio_name, board_name, 4);
        llio_name[1] = tolower(llio_name[1]);

        board->llio.num_ioport_connectors = 3;
        board->llio.pins_per_connector = 24;
        board->llio.ioport_connector_name[0] = "P1";
        board->llio.ioport_connector_name[1] = "P2";
        board->llio.ioport_connector_name[2] = "P3";
        board->llio.fpga_part_number = "XC6SLX25";
        board->llio.num_leds = 4;
    } else if (strncmp(board_name, "7I76E-16", 8) == 0) {
        strncpy(llio_name, board_name, 5);
        llio_name[1] = tolower(llio_name[1]);
        llio_name[4] = tolower(llio_name[4]);

        board->llio.num_ioport_connectors = 3;
        board->llio.pins_per_connector = 17;
        board->llio.ioport_connector_name[0] = "P1";
        board->llio.ioport_connector_name[1] = "P2";
        board->llio.ioport_connector_name[2] = "P3";
        board->llio.fpga_part_number = "XC6SLX16";
        board->llio.num_leds = 4;
    } else if (strncmp(board_name, "7I92", 4) == 0) {
        strncpy(llio_name, board_name, 4);
        llio_name[1] = tolower(llio_name[1]);

        board->llio.num_ioport_connectors = 2;
        board->llio.pins_per_connector = 17;
        board->llio.ioport_connector_name[0] = "P2";
        board->llio.ioport_connector_name[1] = "P1";
        board->llio.fpga_part_number = "XC6SLX9";
        board->llio.num_leds = 4;
    } else if (strncmp(board_name, "ECM1", 4) == 0) {
        strncpy(llio_name, board_name, 4);

        board->llio.num_ioport_connectors = 5;
        board->llio.pins_per_connector = 13;
        board->llio.ioport_connector_name[0] = "P2";
        board->llio.ioport_connector_name[1] = "P1";
        board->llio.ioport_connector_name[2] = "P3";
        board->llio.ioport_connector_name[3] = "P4";
        board->llio.ioport_connector_name[4] = "P5";
        board->llio.fpga_part_number = "XC6SLX9";
        board->llio.num_leds = 0;
    } else {
        LL_PRINT("Unrecognized ethernet board found: %.16s -- port names will be wrong\n", board_name);
        strncpy(llio_name, board_name, 4);
        llio_name[1] = tolower(llio_name[1]);

        // this is a layering violation.  it would be nice if special values
        // (such as 0 or -1) could be passed here and the layer which can
        // legitimately read idroms would read the values and store them, but
        // that wasn't trivial to do.
        u32 read_data;
        hm2_eth_read(&board->llio, HM2_ADDR_IDROM_OFFSET, &read_data, 4);
        unsigned int idrom_address = read_data & 0xffff;
        hm2_idrom_t idrom;
        memset(&idrom, 0, sizeof(idrom));
        hm2_eth_read(&board->llio, idrom_address, &idrom, sizeof(idrom));

        board->llio.num_ioport_connectors = idrom.io_ports;
        board->llio.pins_per_connector = idrom.port_width;
        int i;
        for (i=0; i<board->llio.num_ioport_connectors; i++)
            board->llio.ioport_connector_name[i] = "??";
        board->llio.fpga_part_number = "??";
        board->llio.num_leds = 0;
    }

    LL_PRINT("discovered %.*s\n", 16, board_name);

    rtapi_snprintf(board->llio.name, sizeof(board->llio.name), "hm2_%.*s.%d", (int)strlen(llio_name), llio_name, llio_idx(llio_name));

    board->llio.comp_id = comp_id;

    board->llio.io_reset = hm2_eth_io_reset;
    board->llio.read = hm2_eth_read;
    board->llio.write = hm2_eth_write;
    board->llio.queue_read = hm2_eth_enqueue_read;
    board->llio.send_queued_reads = hm2_eth_send_queued_reads;
    board->llio.receive_queued_reads = hm2_eth_receive_queued_reads;
    board->llio.queue_write = hm2_eth_enqueue_write;
    board->llio.send_queued_writes = hm2_eth_send_queued_writes;

    ret = hm2_register(&board->llio, config[boards_count]);
    if (ret != 0) {
        rtapi_print("board fails HM2 registration\n");
        return ret;
    }
    boards_count++;

    return 0;
}

static int hm2_eth_items(hm2_eth_t *board) {
    int r;

    board->hal = (hm2_eth_global_t *)hal_malloc(sizeof(hm2_eth_global_t));
    if (board->hal == NULL) {
        LL_ERR("out of memory!\n");
        return -ENOMEM;
    }

    if ((r = hal_pin_s32_newf(HAL_IO,
            &(board->hal->read_timeout),
            board->llio.comp_id,
            "%s.packet-read-timeout",
            board->llio.name)) < 0)
        return r;
    *board->hal->read_timeout = 80;

    if ((r = hal_pin_s32_newf(HAL_IO,
            &(board->hal->packet_error_limit),
            board->llio.comp_id,
            "%s.packet-error-limit",
            board->llio.name)) < 0)
        return r;
    *board->hal->packet_error_limit = 10;

    if ((r = hal_pin_s32_newf(HAL_IO,
            &(board->hal->packet_error_increment),
            board->llio.comp_id,
            "%s.packet-error-increment",
            board->llio.name)) < 0)
        return r;
    *board->hal->packet_error_increment = 2;

    if ((r = hal_pin_s32_newf(HAL_OUT,
            &(board->hal->packet_error_decrement),
            board->llio.comp_id,
            "%s.packet-error-decrement",
            board->llio.name)) < 0)
        return r;
    *board->hal->packet_error_decrement = 1;

    if ((r = hal_pin_bit_newf(HAL_OUT,
            &(board->hal->packet_error),
            board->llio.comp_id,
            "%s.packet-error",
            board->llio.name)) < 0)
        return r;
    *board->hal->packet_error = 0;

    if ((r = hal_pin_s32_newf(HAL_OUT,
            &(board->hal->packet_error_level),
            board->llio.comp_id,
            "%s.packet-error-level",
            board->llio.name)) < 0)
        return r;
    *board->hal->packet_error_level = 0;

    if ((r = hal_pin_bit_newf(HAL_OUT,
            &(board->hal->packet_error_exceeded),
            board->llio.comp_id,
            "%s.packet-error-exceeded",
            board->llio.name)) < 0)
        return r;
    *board->hal->packet_error_exceeded = 0;

    if ((r = hal_pin_s32_newf(HAL_OUT,
            &(board->hal->packet_read_tmax),
            board->llio.comp_id,
            "%s.packet-read-tmax",
            board->llio.name)) < 0)
        return r;
    *board->hal->packet_read_tmax = 0;

    if ((r = hal_pin_s32_newf(HAL_OUT,
            &(board->hal->packet_recv_tmax),
            board->llio.comp_id,
            "%s.packet-recv-tmax",
            board->llio.name)) < 0)
        return r;
    *board->hal->packet_recv_tmax = 0;

    if ((r = hal_pin_s32_newf(HAL_OUT,
            &(board->hal->packet_wrong_seq),
            board->llio.comp_id,
            "%s.packet-wrong-seq",
            board->llio.name)) < 0)
        return r;
    *board->hal->packet_wrong_seq = 0;

    if ((r = hal_pin_s32_newf(HAL_OUT,
            &(board->hal->packet_recv_attempts),
            board->llio.comp_id,
            "%s.packet-recv-attempts",
            board->llio.name)) < 0)
        return r;
    *board->hal->packet_recv_attempts = 0;

    return 0;
}

int rtapi_app_main(void) {
    INIT_LIST_HEAD(&ifnames);
    INIT_LIST_HEAD(&board_num);

    int ret, i;

    LL_PRINT("loading Mesa AnyIO HostMot2 ethernet driver version " HM2_ETH_VERSION "\n");

    ret = hal_init(HM2_LLIO_NAME);
    if (ret < 0)
        goto error0;
    comp_id = ret;

    if (use_iptables()) clear_iptables();

    for (i = 0, ret = 0; ret == 0 && i<MAX_ETH_BOARDS && board_ip[i] && *board_ip[i]; i++) {
        ret = init_board(&boards[i], board_ip[i]);
        if (ret < 0) board_ip[i] = 0;
    }

    if (ret < 0)
        goto error1;

    int num_boards = i;
    comm_active = 1;

    for (i = 0; i<num_boards; i++)
    {
        ret = hm2_eth_probe(&boards[i]);

        if (ret < 0)
            goto error1;

        ret = hm2_eth_items(&boards[i]);

        if (ret < 0)
            goto error1;
    }

    for (i = 0; i<num_boards; i++) {
        char ifbuf[64]; // more than enough for eth0
        char *ifptr = fetch_ifname(boards[i].sockfd, ifbuf, sizeof(ifbuf));
        if (!ifptr) {
            LL_PRINT("failed to retrieve interface name for board");
            continue;
        }
        boards[i].read_cnt = boards[i].write_cnt = 0;
        int *added = kvlist_lookup(&ifnames, ifptr);
        if (*added) continue;
        install_iptables_perinterface(ifptr);
        *added = 1;
    }

    hal_ready(comp_id);

    return 0;

error1:
    for (i = 0; i<MAX_ETH_BOARDS && board_ip[i] && board_ip[i][0]; i++) {
        close_board(&boards[i]);
        // Dump final stats
        LL_PRINT("HostMot2 ethernet board %s.packet-read-tmax = %d\n", boards[i].llio.name, *boards[i].hal->packet_read_tmax);
        LL_PRINT("HostMot2 ethernet board %s.packet-recv-tmax = %d\n", boards[i].llio.name, *boards[i].hal->packet_recv_tmax);
        LL_PRINT("HostMot2 ethernet board %s.packet-wrong-seq = %d\n", boards[i].llio.name, *boards[i].hal->packet_wrong_seq);
        LL_PRINT("HostMot2 ethernet board %s.packet-recv-attempts = %d\n", boards[i].llio.name, *boards[i].hal->packet_recv_attempts);
    }

    if (use_iptables()) clear_iptables();

error0:
    kvlist_free(&board_num);
    kvlist_free(&ifnames);
    hal_exit(comp_id);
    return ret;
}

void rtapi_app_exit(void) {
    int i;
    comm_active = 0;
    for (i = 0; i<MAX_ETH_BOARDS && board_ip[i] && board_ip[i][0]; i++)
        close_board(&boards[i]);

    if (use_iptables()) clear_iptables();

    kvlist_free(&board_num);
    kvlist_free(&ifnames);

    hal_exit(comp_id);
    LL_PRINT("HostMot2 ethernet driver unloaded\n");
}

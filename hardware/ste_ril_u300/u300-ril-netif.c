/* ST-Ericsson U300 RIL
**
** Copyright (C) ST-Ericsson AB 2008-2010
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
**  Author: Sjur Brendeland <sjur.brandeland@stericsson.com>
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if_arp.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <poll.h>

#include "u300-ril.h"
#include "u300-ril-netif.h"

#define LOG_TAG "RILV"
#include <utils/Log.h>

#define NLMSG_TAIL(nmsg) \
    ((struct rtattr *) (((uint8_t *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))

#define MAX_PAD_SIZE 1024
#define MAX_BUF_SIZE 4096

struct iplink_req {
    struct nlmsghdr n;
    struct ifinfomsg i;
    char pad[MAX_PAD_SIZE];
    int ifindex;
    char ifname[MAX_IFNAME_LEN];
};

struct ipaddr_req {
    struct nlmsghdr n;
    struct ifaddrmsg ifa;
    char pad[MAX_PAD_SIZE];
};

struct gen_req
{
    struct nlmsghdr n;
    struct rtgenmsg g;
};

static __u32 ipconfig_seqnr = 1;

/*
 * Static function declarations
 */
static bool get_ifname(struct ifinfomsg *msg, int bytes,
                       const char **ifname);
static void handle_rtnl_response(struct iplink_req *req, unsigned short type,
                                 int index, unsigned flags, unsigned change,
                                 struct ifinfomsg *msg, int bytes);
static int send_iplink_req(int sk, struct iplink_req *req);
static int parse_rtnl_message(uint8_t *buf, size_t len, struct iplink_req *req);
static int netlink_get_response(int sk, struct iplink_req *req);
static void add_attribute(struct nlmsghdr *n, int maxlen, int type,
                          const void *data, int datalen);
static int rtnl_init(int groups);
static int rtnl_transmit_wilddump(int sk, int family, int msgtype);
static int rtnl_receive_ipaddr_first_match(int sk, int ifindex, int family,
                                           int msgtype, int rta_type,
                                           void* addr, int* prefix_len);
static ssize_t rtnl_get_ip_address_flushb(int sk, int ifindex, int family,
                                          char *flushb, size_t flushe);
static int rtnl_transmit_and_wait_ack(int sk, struct nlmsghdr *n);
static int rtnl_set_ip_addr(char *ifname, char *addr_str, int family,
                            int addr_type, int flags, int prefix, int scope);


static bool get_ifname(struct ifinfomsg *msg, int bytes,
                       const char **ifname)
{
    struct rtattr *attr;

    if (ifname == NULL)
        return false;

    for (attr = IFLA_RTA(msg); RTA_OK(attr, bytes);
         attr = RTA_NEXT(attr, bytes)) {
        if (attr->rta_type == IFLA_IFNAME) {
            *ifname = RTA_DATA(attr);
            return true;
        }
    }

    return false;
}

static void handle_rtnl_response(struct iplink_req *req, unsigned short type,
                                 int index, unsigned flags, unsigned change,
                                 struct ifinfomsg *msg, int bytes)
{
    const char *ifname = NULL;

    get_ifname(msg, bytes, &ifname);
    req->ifindex = index;
    strncpy(req->ifname, ifname, sizeof(req->ifname));
    req->ifname[sizeof(req->ifname)-1] = '\0';
}

/**
 * Returns -1 and sets errno on errors.
 */
static int send_iplink_req(int sk, struct iplink_req *req)
{
    struct sockaddr_nl addr;

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;

    return sendto(sk, req, req->n.nlmsg_len, 0,
                  (struct sockaddr *) &addr, sizeof(addr));
}

/**
 * Returns 0 on receiving an ACK message;
 * Negative on error;
 * Positive otherwise.
 */
static int parse_rtnl_message(uint8_t *buf, size_t len, struct iplink_req *req)
{
    struct ifinfomsg *msg;

    while (len > 0) {
        struct nlmsghdr *hdr = (struct nlmsghdr *)buf;
        struct nlmsgerr *err;

        if (!NLMSG_OK(hdr, len))
            return -EBADMSG;

        if (hdr->nlmsg_type == NLMSG_ERROR) {
            err = NLMSG_DATA(hdr);
            if (err->error)
                ALOGE("%s(): RTNL failed: seq:%d, error %d(%s)\n", __func__,
                      hdr->nlmsg_seq, err->error, strerror(-err->error));

            return err->error;
        } else if (hdr->nlmsg_type == RTM_NEWLINK ||
                   hdr->nlmsg_type == RTM_DELLINK) {
            msg = (struct ifinfomsg *) NLMSG_DATA(hdr);
            handle_rtnl_response(req, msg->ifi_type,
                                 msg->ifi_index, msg->ifi_flags,
                                 msg->ifi_change, msg,
                                 IFA_PAYLOAD(hdr));
        }

        len -= hdr->nlmsg_len;
        buf += hdr->nlmsg_len;
    }

    return 1;
}

/**
 * Returns 0 on success; On failure, errno is set and a negative value returned.
 */
static int netlink_get_response(int sk, struct iplink_req *req)
{
    unsigned char *buf;
    int ret;

    buf = malloc(MAX_BUF_SIZE);
    assert(buf != NULL);

    /*
     * Loops until an ACK message is received (i.e. parse_rtnl_message
     * returns 0) or an error occurs.
     */
    do {
        ret = read(sk, buf, MAX_BUF_SIZE);
        if (ret < 0) {
            if (errno == EINTR) {
                ret = 1;
                continue;
            }
            else
                break;
        }

        /*
         * EOF is treated as error. This may happen when no process
         * has the pipe open for writing or the other end closed
         * the socket orderly.
         */
        if (ret == 0) {
            ALOGW("EOF received.\n");
            errno = EIO;
            ret = -1;
            break;
        }

        ret = parse_rtnl_message(buf, ret, req);
        if (ret < 0)
            errno = -ret;
    } while (ret > 0);

    free(buf);
    return ret;
}

static void add_attribute(struct nlmsghdr *n, int maxlen, int type,
                          const void *data, int datalen)
{
    int len = RTA_LENGTH(datalen);
    struct rtattr *rta;

    if ((int)(NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len)) > maxlen) {
        ALOGE("%s(): attribute too large for message. nlmsg_len:%d, len:%d, \
             maxlen:%d\n", __func__, n->nlmsg_len, len, maxlen);
        assert(false && "attribute too large for message.");
    }

    rta = NLMSG_TAIL(n);
    rta->rta_type = type;
    rta->rta_len = len;
    memcpy(RTA_DATA(rta), data, datalen);

    n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len);
    return;
}

/**
 * Returns netlink socket on success; On failure, errno is set and -1 returned.
 */
static int rtnl_init(int groups)
{
    struct sockaddr_nl addr;
    int sk, ret;

    sk = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

    /* -1 is returned if failed to create socket. */
    if (sk < 0)
        goto error;

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = groups;

    ret = bind(sk, (struct sockaddr *) &addr, sizeof(addr));

    /* -1 is returned if failed to bind a name to a socket. */
    if (ret < 0) {
        /* errno may be clobbered by a successful close. */
        int old_errno = errno;
        close(sk);
        errno = old_errno;
        goto error;
    }

    goto exit;

error:
    sk = -1;

exit:
    return sk;
}

/**
 * rtnl_transmit_wilddump
 *
 * Transfer RTNL request of type <msgtype> on family <family>. This function
 * will not wait for answer message(s).
 * Returns >= 0 on success and -1 on failure
 */
static int rtnl_transmit_wilddump(int sk, int family, int msgtype)
{
    struct gen_req req;

    memset(&req, 0, sizeof(req));
    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
    req.n.nlmsg_flags = NLM_F_ROOT|NLM_F_MATCH|NLM_F_REQUEST;
    req.n.nlmsg_type = msgtype;
    req.n.nlmsg_seq = ipconfig_seqnr++;
    req.g.rtgen_family = family;

    return send(sk, (void*)&req, sizeof(req), 0);
}

/**
 * rtnl_transmit_buf_with_check
 *
 * Transfer RTNL message in <buf> and check that eventual reply message(s)
 * is not error.
 * Returns 0 on success and -1 on failure
 */
int rtnl_transmit_buf_with_check(int sk, char *buf, size_t len)
{
    struct nlmsghdr *h;
    int status;
    char *recvbuf;
    int res = -1;

    recvbuf = malloc(MAX_BUF_SIZE);
    assert(recvbuf != NULL);
    memset(recvbuf, 0, MAX_BUF_SIZE);

    status = send(sk, buf, len, 0);
    if (status < 0)
        goto exit;

    status = recv(sk, recvbuf, sizeof(MAX_BUF_SIZE), MSG_DONTWAIT|MSG_PEEK);
    if (status < 0) {
        if (errno == EAGAIN)
            goto finally;
        ALOGE("%s(): recv() failed on netlink", __func__);
        goto exit;
    }

    for (h = (struct nlmsghdr *)recvbuf; NLMSG_OK(h, (size_t)status);
         h = NLMSG_NEXT(h, status)) {
        if (h->nlmsg_type == NLMSG_ERROR) {
            struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(h);
            ALOGE("%s(): Error reply: %s",__func__, strerror(-err->error));
            goto exit;
        }
    }

finally:
    res = 0;
exit:
    free(recvbuf);
    return res;
}

/**
 * rtnl_receive_ipaddr_first_match
 *
 * Read RTNL messages of type <msgtype> with attribute <rta_type> and get
 * first non-link local IP address of <family>.
 * The IP address and corresponding prefix length is returned in out
 * parameters <addr> and <prefix_len>.
 * Function rtnl_transmit_wilddump() with proper RTNL request message
 * should be called prior to this function!
 * Returns 0 on success and -1 on failure.
 */
static int rtnl_receive_ipaddr_first_match(int sk, int ifindex, int family,
                                           int msgtype, int rta_type,
                                           void* addr, int* prefix_len)
{
    char *buf;
    int res = -1;
    int len;
    struct nlmsghdr *h;
    struct sockaddr_nl nladdr;
    struct iovec iov;
    struct msghdr msg = {
        .msg_name = &nladdr,
        .msg_namelen = sizeof(nladdr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };

    buf = malloc(MAX_BUF_SIZE);
    assert(buf != NULL);
    memset(buf, 0, MAX_BUF_SIZE);
    iov.iov_base = buf;

    while (1) {
        iov.iov_len = MAX_BUF_SIZE;
        h = (struct nlmsghdr*)buf;

        len = recvmsg(sk, &msg, 0);
        if (len < 0) {
            if (errno == EINTR || errno == EAGAIN)
                continue;

            ALOGE("%s(): recvmsg() failed on netlink", __func__);
            goto exit;
        }
        if (len == 0) {
            ALOGE("%s(): EOF on netlink", __func__);
            goto exit;
        }
        while (NLMSG_OK(h, (size_t)len)){
            if (h->nlmsg_type == NLMSG_DONE) {
                ALOGI("%s(): No address found",__func__);
                goto exit;
            }
            if (h->nlmsg_type == NLMSG_ERROR) {
                struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(h);
                ALOGE("%s(): Error reply: %s", __func__, strerror(-err->error));
                goto exit;
            }
            if (h->nlmsg_type == msgtype) {
                struct ifaddrmsg *ifa = (struct ifaddrmsg *) NLMSG_DATA(h);
                int rtl = IFA_PAYLOAD(h);
                struct rtattr *rth = IFA_RTA(ifa);

                /* Loop through all attributes,
                 * But ignore messages with wrong index, family and scope
                 * (Ignore Link-local addresses) */
                while (rtl &&
                       (ifa->ifa_index == (unsigned)ifindex) &&
                       ((family == AF_UNSPEC) || (ifa->ifa_family == family)) &&
                       (ifa->ifa_scope != RT_SCOPE_LINK) &&
                       RTA_OK(rth, rtl)) {
                    if (rth->rta_type == rta_type) {
                        if (prefix_len != NULL)
                            *prefix_len = ifa->ifa_prefixlen;
                        if (addr != NULL)
                            memcpy(addr, RTA_DATA(rth), RTA_PAYLOAD(rth));
                        goto finally;
                    }
                    rth = RTA_NEXT(rth, rtl);
                }
            }
            h = NLMSG_NEXT(h, len);
        }
    }

finally:
    res = 0;
exit:
    free(buf);
    return res;
}

/**
 * rtnl_get_ip_address_flushb()
 *
 * Create a "flush" buffer of RTNL delete address requests to remove all
 * IP addresses of <family> in an interface.
 * The function reads RTNL messages of type RTM_NEWADDR and converts them into
 * delete request messages that are added to the flush buffer.
 * Function rtnl_transmit_wilddump() with proper RTNL request message
 * should be called prior to this function!
 * Returns bytes written in "flush" buffer on success (possibly 0)
 *         -1 on failure.
 */
static ssize_t rtnl_get_ip_address_flushb(int sk, int ifindex, int family,
                                          char *flushb, size_t flushe)
{
    char *buf;
    int res = -1;
    size_t flushp = 0;
    int len;
    struct nlmsghdr *h;
    struct sockaddr_nl nladdr;
    struct iovec iov;
    struct msghdr msg = {
        .msg_name = &nladdr,
        .msg_namelen = sizeof(nladdr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };

    buf = malloc(MAX_BUF_SIZE);
    assert(buf != NULL);
    memset(buf, 0, MAX_BUF_SIZE);
    iov.iov_base = buf;

    while (1) {
        iov.iov_len = MAX_BUF_SIZE;
        h = (struct nlmsghdr*)buf;

        len = recvmsg(sk, &msg, 0);
        if (len < 0) {
            if (errno == EINTR || errno == EAGAIN)
                continue;

            ALOGE("%s(): recvmsg() failed on netlink", __func__);
            goto exit;
        }
        if (len == 0) {
            ALOGE("%s(): EOF on netlink", __func__);
            goto exit;
        }
        while (NLMSG_OK(h, (size_t)len)){
            if (h->nlmsg_type == NLMSG_DONE) {
                res = flushp;
                goto exit;
            }
            if (h->nlmsg_type == NLMSG_ERROR) {
                struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(h);
                ALOGE("%s(): Error reply: %s",__func__, strerror(-err->error));
                goto exit;
            }
            if (h->nlmsg_type == RTM_NEWADDR)
            {
                struct ifaddrmsg *ifa = (struct ifaddrmsg *) NLMSG_DATA(h);
                struct nlmsghdr *fn;

                /*
                 * Make sure that ifindex and family matches
                 * There must also be room for the flush command in the flush buffer
                 */
                if ((ifa->ifa_index == (unsigned)ifindex) &&
                    ((family == AF_UNSPEC) || (ifa->ifa_family == family)) &&
                    (NLMSG_ALIGN(flushp) + h->nlmsg_len <= flushe)) {

                    /* Convert message into delete request */
                    fn = (struct nlmsghdr*)(flushb + NLMSG_ALIGN(flushp));
                    memcpy(fn, h, h->nlmsg_len);
                    fn->nlmsg_type = RTM_DELADDR;
                    fn->nlmsg_flags = NLM_F_REQUEST;
                    fn->nlmsg_seq = ipconfig_seqnr++;
                    flushp = (((char*)fn) + h->nlmsg_len) - flushb;
                }
            }

            h = NLMSG_NEXT(h, len);
        }
    }

exit:
    free(buf);
    return res;
}

/**
 * rtnl_transmit_and_wait_ack
 *
 * Transmit RTNL message (requiring ACK) and wait for ACK message.
 * Returns 0 on success and -1 on failure.
 */
static int rtnl_transmit_and_wait_ack(int sk, struct nlmsghdr *n)
{
    char *buf;
    int res = 0;
    int len;
    struct nlmsghdr *h;
    struct sockaddr_nl nladdr;
    struct iovec iov = {
        .iov_base = (void*) n,
        .iov_len = n->nlmsg_len
    };
    struct msghdr msg = {
        .msg_name = &nladdr,
        .msg_namelen = sizeof(nladdr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };

    buf = malloc(MAX_BUF_SIZE);
    assert(buf != NULL);
    memset(buf, 0, MAX_BUF_SIZE);

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;

    n->nlmsg_seq = ipconfig_seqnr++;
    n->nlmsg_flags |= NLM_F_ACK;  /* Force ACK */

    len = sendmsg(sk, &msg, 0);
    if (len < 0) {
        ALOGE("%s(): sendmsg() failed on netlink", __func__);
        goto error;
    }

    iov.iov_base = buf;

    while (1) {
        iov.iov_len = MAX_BUF_SIZE;
        h = (struct nlmsghdr*)buf;

        len = recvmsg(sk, &msg, 0);
        if (len < 0) {
            if (errno == EINTR || errno == EAGAIN)
                continue;

            ALOGE("%s(): recvmsg() failed on netlink", __func__);
            goto error;
        }
        if (len == 0) {
            ALOGE("%s(): EOF on netlink", __func__);
            goto error;
        }
        while (NLMSG_OK(h, (size_t)len)){
            if (h->nlmsg_type == NLMSG_ERROR) {
                struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(h);

                if ((h->nlmsg_len - sizeof(*h)) < sizeof(struct nlmsgerr)) {
                    ALOGE("%s(): Malformed message, nlmsg_len:%d, len:%d",
                         __func__, n->nlmsg_len, len);
                    goto error;
                }

                /* ACK is an ERROR message with error set to 0! */
                if (err->error == 0)
                    goto exit;

                ALOGE("%s(): Error reply: %s",__func__, strerror(-err->error));
                goto error;
            }

            ALOGE("%s(): Unexpected reply: %d", __func__, h->nlmsg_type);

            h = NLMSG_NEXT(h, len);
        }

        if (len) {
            ALOGE("%s(): Remnant of size %d",  __func__, len);
            goto error;
        }
    }

error:
    res = -1;
exit:
    free(buf);
    return res;
}

int rtnl_if_enable(char *ifname)
{
    int sk;
    struct iplink_req req;
    int ifindex;
    int res = -1;

    ifindex = if_nametoindex(ifname);
    if (!ifindex)
        goto exit;

    memset(&req, 0, sizeof(req));

    sk = rtnl_init(0);
    if (sk < 0)
        goto exit;

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    req.n.nlmsg_flags = NLM_F_REQUEST;
    req.n.nlmsg_type = RTM_NEWLINK;
    req.i.ifi_family = AF_UNSPEC;
    req.i.ifi_index = ifindex;
    req.i.ifi_change |= IFF_UP;
    req.i.ifi_flags |= IFF_UP;

    if (rtnl_transmit_and_wait_ack(sk, &req.n) < 0)
        goto finally;

    res = 0;

finally:
    close(sk);
exit:
    return res;
}

int rtnl_if_disable(char *ifname)
{
    int sk;
    struct iplink_req req;
    int ifindex;
    int res = -1;

    ifindex = if_nametoindex(ifname);
    if (!ifindex)
        goto exit;

    memset(&req, 0, sizeof(req));

    sk = rtnl_init(0);
    if (sk < 0)
        goto exit;

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    req.n.nlmsg_flags = NLM_F_REQUEST;
    req.n.nlmsg_type = RTM_NEWLINK;
    req.i.ifi_family = AF_UNSPEC;
    req.i.ifi_index = ifindex;
    req.i.ifi_change |= IFF_UP;
    req.i.ifi_flags &= ~IFF_UP;

    if (rtnl_transmit_and_wait_ack(sk, &req.n) < 0)
        goto finally;

    res = 0;

finally:
    close(sk);
exit:
    return res;
}

int rtnl_set_mtu(char *ifname, int mtu)
{
    int sk;
    struct iplink_req req;
    int ifindex;
    int res = -1;

    ifindex = if_nametoindex(ifname);
    if (!ifindex)
        goto exit;

    memset(&req, 0, sizeof(req));

    sk = rtnl_init(0);
    if (sk < 0)
        goto exit;

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    req.n.nlmsg_flags = NLM_F_REQUEST;
    req.n.nlmsg_type = RTM_NEWLINK;
    req.i.ifi_family = AF_UNSPEC;
    req.i.ifi_index = ifindex;

    add_attribute(&req.n, sizeof(req), IFLA_MTU, &mtu, sizeof(mtu));

    if (rtnl_transmit_and_wait_ack(sk, &req.n) < 0)
        goto finally;

    res = 0;

finally:
    close(sk);
exit:
    return res;
}

static int rtnl_set_ip_addr(char *ifname, char *addr_str, int family,
                            int addr_type, int flags, int prefix, int scope)
{
    int sk;
    struct ipaddr_req req;
    int ifindex;
    union {
        struct in6_addr in6_addr;
        struct in_addr  in_addr;
    } addr;
    size_t addr_size;
    int res = -1;

    ifindex = if_nametoindex(ifname);
    if (!ifindex)
        goto exit;

    memset(&req, 0, sizeof(req));

    sk = rtnl_init(0);
    if (sk < 0)
        goto exit;

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    req.n.nlmsg_flags = NLM_F_REQUEST;
    req.n.nlmsg_type = RTM_NEWADDR;

    req.ifa.ifa_family = family;
    req.ifa.ifa_prefixlen = prefix;
    req.ifa.ifa_flags = flags;
    req.ifa.ifa_scope = scope;
    req.ifa.ifa_index = ifindex;

    if (inet_pton(family, addr_str, &addr) <= 0)
        goto finally;

    addr_size = (family==AF_INET)?sizeof(addr.in_addr):sizeof(addr.in6_addr);
    add_attribute(&req.n, sizeof(req), addr_type,
                  &addr, addr_size);

    if (rtnl_transmit_and_wait_ack(sk, &req.n) < 0)
        goto finally;

    res = 0;

finally:
    close(sk);
exit:
    return res;
}

int rtnl_set_ipv4_addr(char *ifname, char *addr_str, int prefix)
{
    return rtnl_set_ip_addr(ifname, addr_str, AF_INET, IFA_LOCAL,
                            0, prefix, RT_SCOPE_UNIVERSE);
}

int rtnl_set_ipv6_link_addr(char *ifname, char *addr_str)
{
    return rtnl_set_ip_addr(ifname, addr_str, AF_INET6, IFA_LOCAL,
                            IFA_F_NODAD, 64, RT_SCOPE_LINK);
}

int rtnl_get_first_ip_address(char *ifname, int family,
                              int* prefix_len, void *ip_addr)
{
    int sk;
    int res = -1;
    int ifindex;

    ifindex = if_nametoindex(ifname);
    if (!ifindex)
        goto exit;

    sk = rtnl_init(0);
    if (sk < 0)
        goto exit;

    if (rtnl_transmit_wilddump(sk, family, RTM_GETADDR) < 0)
        goto finally;

    if (rtnl_receive_ipaddr_first_match(sk, ifindex, family,
                                        RTM_NEWADDR, IFA_ADDRESS,
                                        ip_addr, prefix_len) < 0)
        goto finally;

    res = 0;

finally:
    close(sk);
exit:
    return res;
}

int rtnl_flush_ip_addresses(char *ifname, int family)
{
    int sk;
    int res = -1;
    int ifindex;
    char *flushb;
    int flushp;

    ifindex = if_nametoindex(ifname);
    if (!ifindex)
        goto exit;

    sk = rtnl_init(0);
    if (sk < 0)
        goto exit;

    flushb = malloc(MAX_BUF_SIZE);
    assert(flushb != NULL);
    memset(flushb, 0, MAX_BUF_SIZE);

    /* Request all adresses */
    if (rtnl_transmit_wilddump(sk, family, RTM_GETADDR) < 0)
        goto finally;

    /* Read them out and cleate a flush command buffer */
    flushp = rtnl_get_ip_address_flushb(sk, ifindex, family,
                                        flushb, MAX_BUF_SIZE);
    if (flushp < 0)
        goto finally;

    /* Did we receive any flush commands? */
    if (flushp > 0) {

        /* Send flush command */
        if (rtnl_transmit_buf_with_check(sk, flushb, flushp) < 0)
            goto finally;
    }
    res = 0;

finally:
    free(flushb);
    close(sk);
exit:
    return res;
}

#define PMSG_RTA(pmsg) ((struct rtattr*)(((char*)(pmsg)) + \
                                         NLMSG_ALIGN(sizeof(struct prefixmsg))))
#define PMSG_PAYLOAD(n) NLMSG_PAYLOAD(n, sizeof(struct prefixmsg))

int rtnl_wait_for_ipv6_prefix(char *ifname, int* prefix_len, void *prefix_addr)
{
    int sk;
    int res = -1;
    int len;
    char *buf;
    struct nlmsghdr *nlh;
    int ifindex;
    struct pollfd fds;

    ifindex = if_nametoindex(ifname);
    if (!ifindex)
        goto exit;

    sk = rtnl_init(RTMGRP_IPV6_PREFIX);
    if (sk < 0)
        goto exit;

    buf = malloc(MAX_BUF_SIZE);
    assert(buf != NULL);
    memset(buf, 0, MAX_BUF_SIZE);

    nlh = (struct nlmsghdr *)buf;
    fds.fd = sk;
    fds.events = POLLIN;

    /*
     * Wait for RTM_NEWPREFIX event informing us that SAA has created
     * a public address
     */
    while (poll(&fds, 1, 3 * 60 * 1000) > 0) {
        len = recv(sk, nlh, MAX_BUF_SIZE, 0);
        if (len <= 0)
            break;

        while ((NLMSG_OK(nlh, (size_t)len)) && (nlh->nlmsg_type != NLMSG_DONE)) {
            if (nlh->nlmsg_type == RTM_NEWPREFIX) {
                struct prefixmsg *pmsg = (struct prefixmsg *) NLMSG_DATA(nlh);
                int rtl = PMSG_PAYLOAD(nlh);
                struct rtattr *rth = PMSG_RTA(pmsg);

                while (rtl &&
                       (pmsg->prefix_ifindex == ifindex) &&
                       (pmsg->prefix_family == AF_INET6) &&
                       RTA_OK(rth, rtl)) {
                    if ((rth->rta_type == PREFIX_ADDRESS) &&
                        (rth->rta_len >= RTA_LENGTH(sizeof(struct in6_addr)))) {
                        if (prefix_len != NULL)
                            *prefix_len = pmsg->prefix_len;
                        if (prefix_addr != NULL)
                            memcpy(prefix_addr, RTA_DATA(rth),
                                   sizeof(struct in6_addr));
                        res = 0;
                        goto finally;
                    }
                    rth = RTA_NEXT(rth, rtl);
                }
            }
            nlh = NLMSG_NEXT(nlh, len);
        }
    }

finally:
    free(buf);
    close(sk);
exit:
    return res;
}

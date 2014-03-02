
/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

/* define __USE_GNU only for module test */
#ifdef ENABLE_MODULE_TEST
#define __USE_GNU
#endif /* ENABLE_MODULE_TEST */
#include <sys/socket.h>
#ifdef ENABLE_MODULE_TEST
#undef __USE_GNU
#endif /* ENABLE_MODULE_TEST */

#include <sys/un.h>
#include <linux/netlink.h>

#include "cn_log.h"
#include "cn_data_types.h"
#include "message_handler.h"
#include "fdmon.h"
#include "cn_battery_internal.h"
#include "cn_macros.h"

typedef struct {
    int fd;
    int wd_status;
    int wd_capacity;
} cn_battery_client_t;

struct uevent {
    const char *action;
    const char *path;
};

static cn_battery_client_t *battery_client_p = NULL;
static void parse_event(const char *msg, struct uevent *uevent)
{
    uevent->action = "";
    uevent->path = "";

    while(*msg) {
        if(!strncmp(msg, "ACTION=", 7)) {
            msg += 7;
            uevent->action = msg;
        } else if(!strncmp(msg, "DEVPATH=", 8)) {
            msg += 8;
            uevent->path = msg;
        }
            /* advance to after the next \0 */
        while(*msg++)
            ;
    }

    CN_LOG_D("event { '%s', '%s' }\n",
                    uevent->action, uevent->path);

}


#define UEVENT_MSG_LEN  1024
int cn_battery_client_select_callback(const int fd, const void *data_p)
{
    cn_battery_client_t *battery_client_p = NULL;
    struct uevent uevent;
    char msg[UEVENT_MSG_LEN+2];
    char cred_msg[CMSG_SPACE(sizeof(struct ucred))];
    struct iovec iov = {msg, sizeof(msg)};
    struct sockaddr_nl snl;
    struct msghdr hdr = {&snl, sizeof(snl), &iov, 1, cred_msg, sizeof(cred_msg), 0};
    ssize_t n = 0;

    PARAMETER_NOT_USED(fd);

    battery_client_p = (cn_battery_client_t *) data_p;

    if (!battery_client_p) {
        CN_LOG_E("battery_client_p is NULL!");
        goto exit;
    }

    n = recvmsg(battery_client_p->fd, &hdr, 0);

    if(n >= UEVENT_MSG_LEN) {
        CN_LOG_W("Message too large -- discard");
        goto exit;
    }

    if (n <= 0) {
        CN_LOG_E("Error reading messsage, error = %d", n);
        goto exit;
    }

    if ((snl.nl_groups != 1) || (snl.nl_pid != 0)) {
        CN_LOG_W("Ignoring non-kernel netlink multicast message");
        goto exit;
    }

    struct cmsghdr * cmsg = CMSG_FIRSTHDR(&hdr);
    if (cmsg == NULL || cmsg->cmsg_type != SCM_CREDENTIALS) {
        CN_LOG_W("No sender credentials received, ignore message");
        goto exit;
    }

    struct ucred * cred = (struct ucred *)CMSG_DATA(cmsg);
    if (cred->uid != 0) {
        CN_LOG_W("Message from non-root user, ignore");
        goto exit;
    }

    parse_event(msg, &uevent);

    if (strncmp(CN_BATTERY_STATUS_FILE, uevent.path, strlen(CN_BATTERY_STATUS_FILE)) == 0 ||
        strncmp(CN_BATTERY_CAPACITY_FILE, uevent.path, strlen(CN_BATTERY_CAPACITY_FILE)) == 0  ||
        strncmp(CN_BATTERY_FOLDER_STATUS, uevent.path, strlen(CN_BATTERY_FOLDER_STATUS)) == 0  ||
        strncmp(CN_BATTERY_FOLDER_CAPACITY, uevent.path, strlen(CN_BATTERY_FOLDER_CAPACITY)) == 0
        ) {
        cn_battery_modified();
    }

exit:
    return TRUE;
}

cn_bool_t cn_battery_client_open_session()
{
    int result = 0;
    int sz = 64*1024;
    struct sockaddr_nl addr;
    int on = 1;

    battery_client_p = calloc(1, sizeof(cn_battery_client_t));

    if (!battery_client_p) {
        CN_LOG_E("calloc failed for battery_client_p!");
        goto error;
    }

    battery_client_p->fd = -1;
    battery_client_p->fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);

    if (battery_client_p->fd < 0) {
        CN_LOG_E("inotify_init failed!");
        goto error;
    }
    setsockopt( battery_client_p->fd, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz));
    setsockopt( battery_client_p->fd, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on));

    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();
    addr.nl_groups = 0xffffffff;

    if(bind(battery_client_p->fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        CN_LOG_E("bind failed");
        goto error;
    }

    result = fdmon_add(battery_client_p->fd, (void *)battery_client_p, cn_battery_client_select_callback, NULL);

    if (result < 0) {
        CN_LOG_E("fdmon_add failed!");
        goto error;
    }

    return TRUE;

error:
    cn_battery_client_close_session();
    return FALSE;
}

cn_void_t cn_battery_client_close_session()
{
    int status = 0;

    if (battery_client_p) {
        status = fdmon_del(battery_client_p->fd);

        if (status < 0) {
            CN_LOG_W("fdmon_del failed!");
        }

        close(battery_client_p->fd);
        free(battery_client_p);
        battery_client_p = NULL;
    }
}


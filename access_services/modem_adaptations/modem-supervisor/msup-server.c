/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <errno.h>
#include <stdlib.h>

#include "dgram-sock.h"
#include "msup-server.h"

#define BINNAME "MSUP"

#ifdef HAVE_ANDROID_OS
#define LOG_TAG BINNAME
#include <cutils/log.h>
#define OPENLOG(facility) ((void)0)
#else
#include <syslog.h>
#define OPENLOG(facility) openlog(BINNAME, LOG_PID | LOG_CONS, facility)
#define LOG(priority, format, ...) syslog(priority, format, ##__VA_ARGS__)
#define ALOGV(format, ...)   LOG(LOG_INFO, format, ##__VA_ARGS__)
#define ALOGD(format, ...)   LOG(LOG_DEBUG, format, ##__VA_ARGS__)
#define ALOGI(format, ...)   LOG(LOG_INFO, format, ##__VA_ARGS__)
#define ALOGW(format, ...)   LOG(LOG_WARNING, format, ##__VA_ARGS__)
#define ALOGE(format, ...)   LOG(LOG_ERR, format, ##__VA_ARGS__)
#endif

const char *msup_server_socket_name         = MSUP_SERVER_SOCKET_NAME;
const char *msup_notification_with_flags    = MSUP_NOTIFICATION_WITH_FLAGS;

void *msup_server_open(int *fd_p)
{
    void *dgram_p = dgram_sock_init(DGRAM_SOCK_SERVER, MSUP_SERVER_SOCKET_NAME);
    if (dgram_p == NULL) {
        ALOGE("%s: dgram_sock_init() failed!\n", __func__);
        return NULL;
    }

    *fd_p = dgram_sock_get_fd(dgram_p);

    return dgram_p;
}


int msup_server_receive(void *p, msup_flags_t *flags_p)
{
    char msg[80];
    int msg_len;
    char *str_p;

    msg_len = dgram_sock_recv(p, msg, sizeof(msg) - 1);

    if (msg_len < 1) {
        ALOGE("%s: dgram_sock_recv() returns %d!\n", __func__, msg_len);
        return -1;
    }

    msg[msg_len] = '\0';
    str_p = strstr(msg, msup_notification_with_flags);
    if (str_p == NULL) {
        ALOGE("%s: Invalid message received: \"%s\"\n", __func__, msg);
        return -1;
    }

    errno = 0;
    *flags_p = (msup_flags_t)strtol(str_p + strlen(msup_notification_with_flags), NULL, 0);
    if (errno != 0) {
        ALOGE("%s: strtol() %d: %s\n", __func__, errno, strerror(errno));
        return -1;
    }

    return 0;
}


void msup_server_close(void *p)
{
    dgram_sock_destroy(p);
}

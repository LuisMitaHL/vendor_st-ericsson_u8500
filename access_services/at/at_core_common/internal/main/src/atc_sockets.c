/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include "atc_log.h"


int atc_socket_create(const char *path)
{
    int fd;
    struct sockaddr_un addr;
    int i;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path));

    fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);

    if (fd < 0) {
        int errnum = errno;
        ATC_LOG_E("socket creation (%s) failed, errno=%d, \"%s\"!", path, errnum, strerror(errnum));
        return -1;
    }

    unlink(addr.sun_path); /* Remove old socket */

    i = bind(fd, (struct sockaddr *) &addr, sizeof(addr));

    if (i < 0) {
        int errnum = errno;
        ATC_LOG_E("bind to socket (%s) failed, errno=%d, \"%s\"!", path, errnum, strerror(errnum));
        close(fd);
        return -1;
    }

    i = listen(fd, 3); /* Allow 3 pending connections */

    if (i < 0) {
        int errnum = errno;
        ATC_LOG_E("listen to socket (%s) failed, errno=%d, \"%s\"!", path, errnum, strerror(errnum));
        close(fd);
        return -1;
    }

    i = fcntl(fd, F_SETFL, O_NONBLOCK);

    if (i < 0) {
        int errnum = errno;
        ATC_LOG_E("set to non-blocking of socket (%s) failed, errno=%d, \"%s\"!", path, errnum, strerror(errnum));
        close(fd);
        return -1;
    }

    return fd;
}


int atc_socket_accept(int fd)
{
    int connection_fd;
    int i;

    connection_fd = accept(fd, NULL, NULL);

    if (connection_fd < 0) {
        int errnum = errno;
        ATC_LOG_E("accept socket failed, errno=%d, \"%s\"!", errnum, strerror(errnum));
        return -1;
    }

    i = fcntl(connection_fd, F_SETFL, O_NONBLOCK);

    if (i < 0) {
        int errnum = errno;
        ATC_LOG_E("set to non-blocking of socket with fd %d failed, errno=%d, \"%s\"!",
                connection_fd, errnum, strerror(errnum));
        return -1;
    }

    return connection_fd;
}

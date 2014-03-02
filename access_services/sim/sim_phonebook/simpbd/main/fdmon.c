/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 *  File descriptor monitor module.
 *
 *  Author: Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include "log.h"
#include "fdmon.h"

/**
  * @brief Specifies the properties associated with a monitored file descriptor.
  *
  */
struct element_s {
    struct element_s *next;     /* Pointer to next element in list */
    int               fd;       /* File descriptor */
    void             *context;  /* Callback context */
    fdmon_callback_t *cb_event; /* Event callback */
    fdmon_callback_t *cb_remove;/* Remove callback */
};
typedef struct element_s element_t;

/* Local data */
static int read_pipe, write_pipe;
static struct element_s *list_anchor = NULL;

/* Local function prototypes */

/* Module initialization */
int fdmon_init(void)
{
    int pipefd[2];
    int errnum, flags;

    /* Create a pipe that can be used to wake us up when we are waiting
     * for an event to occur on any of the monitored file descriptors.
     * See the fdmon_stopwait() function.
     */
    if (pipe(pipefd) < 0) {
        SIMPB_LOG_E("pipe() failed, errno=%d", errno);
        goto error_return;
    }

    read_pipe = pipefd[0];
    flags = fcntl(read_pipe, F_GETFD, 0);

    if (flags < 0 || fcntl(read_pipe, F_SETFD, flags | FD_CLOEXEC) < 0) {
        errnum = errno;
        SIMPB_LOG_E("fcntl error(%d)=\"%s\"", errnum, strerror(errnum));
        goto cleanup_return;
    }

    write_pipe = pipefd[1];
    flags = fcntl(write_pipe, F_GETFD, FD_CLOEXEC);

    if (flags < 0 || fcntl(write_pipe, F_SETFD, flags | FD_CLOEXEC) < 0) {
        errnum = errno;
        SIMPB_LOG_E("fcntl error(%d)=\"%s\"", errnum, strerror(errnum));
        goto cleanup_return;
    }

    flags = fcntl(write_pipe, F_GETFL, 0);

    if (flags < 0 || fcntl(write_pipe, F_SETFL, flags | O_NONBLOCK) < 0) {
        errnum = errno;
        SIMPB_LOG_E("fcntl error(%d)=\"%s\"", errnum, strerror(errnum));
        goto cleanup_return;
    }

    fdmon_add(read_pipe, NULL, NULL, NULL);
    return 0;

cleanup_return:
    close(pipefd[0]);
    close(pipefd[1]);
error_return:

    return -1;
}

/* Module shutdown */
int fdmon_shutdown(void)
{
    /* Empty list */
    while (list_anchor != NULL) {
        fdmon_del(list_anchor->fd);
    }

    /* Close pipes */
    if (read_pipe) {
        close(read_pipe);
    }

    if (write_pipe) {
        close(write_pipe);
    }

    return 0;
}

/* Wait for event */
int fdmon_waitevent(long timeout)
{
    element_t *element, *next_element;
    struct timeval tv, *tvp;
    fd_set readfds;
    int errnum, max_fd, res;

    /* Set up timeout structure */
    if (timeout < 0) {
        tvp = NULL;
    } else {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        tvp = &tv;
    }

    while (1) {
        /* Walk list and populate fd_set */
        max_fd = 0;
        FD_ZERO(&readfds);

        for (element = list_anchor; element != NULL; element = element->next) {
            FD_SET(element->fd, &readfds);

            if (element->fd > max_fd) {
                max_fd = element->fd;
            }
        }

        /* Perform the select */
        res = select(max_fd + 1, &readfds, NULL, NULL, tvp);

        if (res > 0) {
            /* Walk list and test each fd, process as appropriate */
            for (element = list_anchor; element != NULL; element = next_element) {
                next_element = element->next;

                if (FD_ISSET(element->fd, &readfds)) {
                    if (element->cb_event) {
                        if (element->cb_event(element->fd, element->context) < 0) {
                            /* Fatal error on descriptor, remove it */
                            fdmon_del(element->fd);
                        }
                    }
                }

            }

            break;
        } else if (res < 0) {
            /* Error handling */
            if (errno == EINTR) {
                break;
            } else if (errno == EBADF) {
                SIMPB_LOG_E("select returns EBADF");

                /* Try to recover by identifying and purging any and all invalid fd's */
                for (element = list_anchor; element != NULL; element = next_element) {
                    next_element = element->next;

                    if (fcntl(element->fd, F_GETFL, 0) < 0) {
                        if (errno == EBADF) {
                            SIMPB_LOG_E("fd=%d is invalid, removing", element->fd);
                            fdmon_del(element->fd);
                        }
                    }
                }

                break;
            } else {
                /* Treat all other errors as fatal */
                errnum = errno;
                SIMPB_LOG_E("errno(%d)=\"%s\"", errnum, strerror(errnum));
                goto error_return;
            }
        } else {
            // This should only happen if there's a timeout
            break;
        }
    }

    return 0;
error_return:
    return -1;
}

/* Stop the wait */
int fdmon_stopwait(void)
{
    char str[] = "stop";

    if (write_pipe) {
        if (write(write_pipe, str, sizeof(str)) == sizeof(str)) {
            return 0;
        }
    } else {
        SIMPB_LOG_E("no pipe for writing!");
    }

    return -1;
}

/* List maintenance functions */
int fdmon_add(const int fd, const void *context, fdmon_callback_t *cb_event, fdmon_callback_t *cb_remove)
{
    element_t *element;
    element_t *new_element;

    /* Test that the file/pipe/socket descriptor is not bogus */
    if (0 > fd || FD_SETSIZE <= fd) {
        SIMPB_LOG_E("out-of-range descriptor fd=%d, rejected!", fd);
        goto error_return;
    }

    if (fcntl(fd, F_GETFL, 0) < 0 && errno == EBADF) {
        SIMPB_LOG_E("invalid descriptor fd=%d, rejected!", fd);
        goto error_return;
    }

    /* Find end of list */
    element = list_anchor;

    if (element) {
        while (element->next != NULL) {
            element = element->next;
        }
    }

    /* Allocate new element and chain it in */
    new_element = malloc(sizeof(element_t));

    if (new_element) {
        new_element->next = NULL;
        new_element->fd = fd;
        new_element->context = (void *)context;
        new_element->cb_event = cb_event;
        new_element->cb_remove = cb_remove;

        if (element) {
            element->next = new_element;
        } else {
            list_anchor = new_element;
        }

        SIMPB_LOG_D("registering fd=%d, new_element=%p", fd, new_element);
        return 0;
    }

error_return:
    return -1;
}

int fdmon_del(int fd)
{
    element_t *element;
    element_t *prev_element = NULL;

    SIMPB_LOG_D("unregistering fd=%d", fd);

    /* Find entry */
    for (element = list_anchor; element != NULL; element = element->next) {
        if (element->fd == fd) {
            /* Unlink this entry, and release the memory used by it */
            if (prev_element) {
                prev_element->next = element->next;
            } else {
                list_anchor = NULL;
            }

            /* Make the remove callback to clean up */
            if (element->cb_remove) {
                element->cb_remove(element->fd, element->context);
            }

            /* Release the memory, and then we are done */
            free(element);
            break;
        }

        prev_element = element;
    }

    return -1;
}

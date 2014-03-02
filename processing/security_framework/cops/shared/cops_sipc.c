/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_sipc.h>
#include <cops_fd.h>
#include <cops_error.h>
#include <stdlib.h>

#define SIPC_MAX_MSG_SIZE (4096)

static bool cops_sipc_write(int fd, const void *vptr, size_t n);

ssize_t cops_sipc_recv(int fd, cops_sipc_message_t **msg)
{
    ssize_t res;
    uint8_t *m = NULL;

    m = malloc(SIPC_MAX_MSG_SIZE);

    if (m == NULL) {
        return -1;
    }

    res = cops_read(fd, m, SIPC_MAX_MSG_SIZE);

    if (-1 == res || res < ((cops_sipc_message_t *)m)->length ||
        0 == ((cops_sipc_message_t *)m)->length) {
        free(m);
        return -1;
    }

    *msg = (cops_sipc_message_t *)m;
    return res;
}


cops_return_code_t cops_sipc_send(int fd, cops_sipc_message_t *msg)
{
    if (!cops_sipc_write(fd, msg, msg->length)) {
        return COPS_RC_IPC_ERROR;
    }

    return COPS_RC_OK;
}

static bool cops_sipc_write(int fd, const void *vptr, size_t n)
{
    size_t    nleft;
    ssize_t   nwritten;
    const char *ptr;

    ptr = vptr;                 /* can't do pointer arithmetic on void* */
    nleft = n;

    while (nleft > 0) {
wagain:
        nwritten = cops_write(fd, ptr, nleft);

        if (nwritten == -1) {
            int e = cops_errno();
            /* TODO: EINTR is not correct for PSock */
            if (e == 4/*EINTR*/) {
                goto wagain;
            }

            return false;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return true;
}


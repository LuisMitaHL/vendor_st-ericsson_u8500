/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*----------------------------------------------------------------------------------*/
/* Name: sim_stub.c                                                                 */
/* SIM stub implementation file for SIM Library                                     */
/* version:         0.1                                                             */
/*----------------------------------------------------------------------------------*/


#include <stdio.h>
#include "sim.h"
#include "sim_stub.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#define LOG_TAG "SIM"

#define SIM_LOG_D(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] DBG:" format, ## __VA_ARGS__); fflush(stdout);})
#define SIM_LOG_E(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] ERR:" format, ## __VA_ARGS__); fflush(stdout);})
#define SIM_LOG_I(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] INF:" format, ## __VA_ARGS__); fflush(stdout);})
#define SIM_LOG_V(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] VBE:" format, ## __VA_ARGS__); fflush(stdout);})
#define SIM_LOG_W(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] WRG:" format, ## __VA_ARGS__); fflush(stdout);})


/*------------------------------------------------------------------------------
 * Public function declarations
 *------------------------------------------------------------------------------
 */
void *sim_util_readsocket(
    int fd,
    size_t nobytes,
    ssize_t *bufsize_p)
{
    int bytes_read = 0;
    char *buf_p = NULL;

    if (0 > fd) {
        SIM_LOG_E("%s: Invalid fd %d!", __func__, fd);
        return NULL;
    }

    if (1 > nobytes) {
        SIM_LOG_E("%s: Invalid nobytes %d!", __func__, nobytes);
        return NULL;
    }

    if (NULL == bufsize_p) {
        SIM_LOG_E("%s: Invalid bufsize_p %p!", __func__, bufsize_p);
        return NULL;
    }

    buf_p = malloc(nobytes);
    if (!buf_p) {
        SIM_LOG_E("sim_util_readsocket - Failed to read from socket, Memory Alloc Error");
        return (NULL);
    }

    memset(buf_p, 0, nobytes);
    bytes_read = read(fd, buf_p, nobytes);

    if (0 > bytes_read) {
        SIM_LOG_E("%s: FAILED to read %d bytes from socket %d, error %d(\"%s\")",
                __func__, nobytes, fd, errno, strerror(errno));
        close(fd);
    } else if (0 < bytes_read) {
        SIM_LOG_D("%s: Read %d bytes from socket %d",
                __func__, bytes_read, fd);
    } else {
        bytes_read = -1;
    }

    *bufsize_p = bytes_read;

    return (buf_p);
}

int sim_util_writesocket(
    int fd,
    const char *socketpath_p,
    const char *buf_p,
    const size_t bufsize)
{
    int nobytes = 0;

    if (0 < bufsize) {
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        addr.sun_path[0] = 0;       // abstract namespace socket
        strncpy(&addr.sun_path[1], socketpath_p, sizeof(addr.sun_path) - 1);

        nobytes = sendto(fd, buf_p, bufsize, 0, (struct sockaddr *)&addr, sizeof(addr));

        if (0 > nobytes) {
            SIM_LOG_E("%s: FAILED to send %d bytes on socket %d(\"%s\"), error %d(\"%s\")",
                    __func__, bufsize, fd, socketpath_p, errno, strerror(errno));
            close(fd);
        } else if (0 < nobytes) {
            SIM_LOG_D("%s: Sent %d bytes on socket %d(\"%s\")",
                    __func__, nobytes, fd, socketpath_p);
        } else {
            nobytes = -1;
        }
    } else {
        SIM_LOG_E("%s: ERROR invalid argument buffersize=%d to send on socket %d(\"%s\")!!",
                __func__, bufsize, fd, socketpath_p);
    }

    return (nobytes);
}

char *sim_util_enc(
    char *dst,
    const void *vsrc,
    size_t n)
{
    const char *src = (const char *) vsrc;

    while (n) {
        *dst = *src;
        ++dst;
        ++src;
        --n;
    }

    return (dst);
}


const char *sim_util_dec(
    const char *src,
    void *vdst,
    size_t n,
    const char *smax)
{
    char *dst = (char *) vdst;

    while (src && dst && n) {
        if (src < smax) {
            *dst = *src;
            ++dst;
            ++src;
            --n;
        } else {
            return 0;
        }
    }

    return (src);
}

void *sim_util_encode_generic(
    uint16_t cmd,
    const char *payload_p,
    size_t payloadsize,
    uintptr_t client_tag,
    size_t * bufsize)
{
    char *encbuf = NULL;
    char *p = NULL;
    uint16_t len;
    size_t bsize;

    SIM_LOG_I("sim_util_encode_generic");

    // check payload/payloadsize consistency
    // NULL,0 is good, as is not-null,not-zero.
    if ((payload_p == NULL && payloadsize != 0) || (payload_p != NULL && payloadsize == 0))
        return NULL;

    if (payloadsize + sizeof(client_tag) + sizeof(cmd) > 0x0ffff)       /* too large! */
        return NULL;

    len = payloadsize + sizeof(client_tag) + sizeof(cmd);
    bsize = len + sizeof(len);

    encbuf = malloc(bsize);
    if (!encbuf)
        return NULL;

    p = encbuf;
    p = sim_util_enc(p, &len, sizeof(len));
    p = sim_util_enc(p, &cmd, sizeof(cmd));
    p = sim_util_enc(p, &client_tag, sizeof(client_tag));
    p = sim_util_enc(p, payload_p, payloadsize);

    if (bsize != (size_t) (p - encbuf)) {
        SIM_LOG_E("sim_util_encode_generic: assert failure: %s", strerror(errno));
    }

    *bufsize = bsize;

    return (encbuf);
}

void *sim_util_decode_generic(
    const char *buf_p,
    const ssize_t bufsize,
    uint16_t * len_p,
    uint16_t * cmd_p,
    uintptr_t * client_tag_p,
    size_t * payloadsize_p)
{
    char *payload_p = NULL;

    SIM_LOG_I("sim_util_decode_generic");

    if (buf_p != NULL && bufsize > 0) {
        const char *p = buf_p;
        const char *p_max = buf_p + bufsize;

        p = sim_util_dec(p, len_p, sizeof(*len_p), p_max);
        p = sim_util_dec(p, cmd_p, sizeof(*cmd_p), p_max);
        p = sim_util_dec(p, client_tag_p, sizeof(*client_tag_p), p_max);

        *payloadsize_p = *len_p;
        *payloadsize_p -= sizeof(*cmd_p) + sizeof(*client_tag_p);

        payload_p = malloc(*payloadsize_p);

        if (payload_p != NULL) {
            int payloadsize_set = *payloadsize_p;
            memset(payload_p, 0, payloadsize_set);
            p = sim_util_dec(p, payload_p, sizeof(payload_p) + *payloadsize_p, p_max);
        } else {
            SIM_LOG_E("sim_util_decode_generic: - Error creating payload");
            free(payload_p);
        }
    }

    return (payload_p);
}

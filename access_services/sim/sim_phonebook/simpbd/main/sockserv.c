/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 *  UNIX domain socket server.
 *
 *  Author: Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/sockios.h>

#include "log.h"
#include "sockserv.h"
#include "fdmon.h"

/* Local data */
#define SOCKSERV_MAX_SERVERS 20         /* Max server instances */
#define SOCKSERV_MAX_CLIENTS 100        /* Max clients per server instance */

#define MAKECONTEXT(instance, client) (sockserv_context_t)((instance << 16) | ((short)client & 0xFFFF))
#define GETINSTANCE(context) (int)((int)context >> 16)
#define GETCLIENT(context) (int)((short)((int)context & 0xFFFF))

typedef struct {
    int                   sd;           /* Connected socket descriptor */
    void                 *context_p;    /* Sockserv client context */
    uint8_t              *buf;
    int                   size;
    int                   used;
} sockserv_client_t;

typedef struct {
    int                   sd;           /* Listening socket descriptor */
    int                   client_max;
    int                   clients_used;
    sockserv_client_t   **clients;
    sockserv_callback_t  *cb_func;
} sockserv_instance_t;

static int instance_max;
static int instances_used;
static sockserv_instance_t **instances = NULL;

/* Local function prototypes */
static int sockserv_alloc_instance(void);
static sockserv_instance_t *sockserv_get_instance(int instance_no);
static int sockserv_free_instance(int instance_no);

static int sockserv_alloc_client(int instance_no);
static sockserv_client_t *sockserv_get_client(int instance_no, int client_no);
static int sockserv_free_client(int instance_no, int client_no);

static int sockserv_cb_accept(const int sd, const void *context);
static int sockserv_cb_recv(const int sd, const void *context);
static int sockserv_cb_close(const int sd, const void *context);


/* Module initialization */
int sockserv_init(const int max_servers)
{
    /* Parameter verification */
    if (max_servers < 1 || max_servers > SOCKSERV_MAX_SERVERS) {
        SIMPB_LOG_E("max_servers=%d is out of range (1-%d)!", max_servers, SOCKSERV_MAX_SERVERS);
        goto error_return;
    }

    /* Allocate memory to hold pointers to all instances */
    instances = calloc(max_servers, sizeof(sockserv_instance_t *));

    if (instances) {
        instance_max = max_servers;
        instances_used = 0;
        return 0;
    }

error_return:
    return -1;
}


/* Create an instance of the socket server */
int sockserv_create(const char *name, const int max_clients, sockserv_callback_t *cb_func)
{
    struct sockaddr_un uaddr;
    int errnum, flags, instance_no, sd;
    sockserv_instance_t *instance_p;
    sockserv_context_t instance_context;

    /* Parameter verification */
    if (!name || strlen(name) < 1) {
        SIMPB_LOG_E("no socket name supplied!");
        goto error_return;
    }

    if (max_clients < 1 || max_clients > SOCKSERV_MAX_CLIENTS) {
        SIMPB_LOG_E("max_clients=%d is out of range (1-%d)!", max_clients, SOCKSERV_MAX_CLIENTS);
        goto error_return;
    }

    if (!cb_func) {
        SIMPB_LOG_E("no callback function supplied!!");
        goto error_return;
    }

    /* Allocate and initialise instance structure */
    instance_no = sockserv_alloc_instance();
    instance_p = sockserv_get_instance(instance_no);

    if (instance_p) {
        instance_p->clients_used = 0;
        instance_p->client_max = max_clients;
        instance_p->cb_func = cb_func;
    } else {
        SIMPB_LOG_E("unable to allocate new instance=%d!", instance_no);
        goto error_return;
    }

    /* Create a UNIX domain server socket */
    sd = socket(PF_UNIX, SOCK_STREAM, 0);
    instance_p->sd = sd;

    if (sd < 0) {
        errnum = errno;
        SIMPB_LOG_E("socket() returns error(%d)=\"%s\"", errnum, strerror(errnum));
        goto cleanup_return;
    }

    /* Bind the socket */
    uaddr.sun_family = AF_UNIX;
    strncpy(uaddr.sun_path, name, sizeof(uaddr.sun_path) - 1);
    unlink(uaddr.sun_path);

    if (bind(sd, (struct sockaddr *)&uaddr, sizeof(uaddr)) < 0) {
        errnum = errno;
        SIMPB_LOG_E("bind() socket \"%s\", returns error(%d)=\"%s\"", name, errnum, strerror(errnum));
        goto cleanup_return;
    }

    /* Begin listening to connections */
    if (listen(sd, max_clients) < 0) {
        errnum = errno;
        SIMPB_LOG_E("listen() socket \"%s\", returns error(%d)=\"%s\"", name, errnum, strerror(errnum));
        goto cleanup_return;
    }

    /* Set socket to non-blocking */
    flags = fcntl(sd, F_GETFL, 0);

    if (flags < 0 || fcntl(sd, F_SETFL, flags | O_NONBLOCK) < 0) {
        errnum = errno;
        SIMPB_LOG_E("fcntl() socket \"%s\", returns error(%d)=\"%s\"", name, errnum, strerror(errnum));
        goto cleanup_return;
    }

    /* Let fdmon monitor events and set up a callback to handle connections. */
    instance_context = MAKECONTEXT(instance_no, -1);

    if (fdmon_add(sd, (void *)instance_context, &sockserv_cb_accept, NULL) < 0) {
        goto cleanup_return;
    }

    return instance_no;

cleanup_return:

    if (sd >= 0) {
        close(sd);
    }

    sockserv_free_instance(instance_no);
error_return:
    return -1;
}


/* Connection to socket requested */
int sockserv_cb_accept(const int sd, const void *context)
{
    int instance_no = GETINSTANCE(context);
    sockserv_instance_t *instance_p;
    sockserv_client_t *client_p;
    sockserv_context_t client_context;
    socklen_t slen;
    int csd, client_no, errnum, flags;

    /* Check instance */
    instance_p = sockserv_get_instance(instance_no);

    if (!instance_p) {
        SIMPB_LOG_E("invalid context %p!", context);
        goto error_return;
    }

    /* Ask upper layer if it is OK to accept the connection */
    if (instance_p->cb_func(instance_no, -1, (void *)NULL,
                            SOCKSERV_REASON_CONNECT_REQUEST, NULL, 0) < 0) {
        SIMPB_LOG_V("instance=%d, client connect request rejected.", instance_no);
        /* Reject connection */
        goto error_return;
    }

    /* Allocate and initialise client structure */
    client_no = sockserv_alloc_client(instance_no);
    client_p = sockserv_get_client(instance_no, client_no);

    if (!client_p) {
        SIMPB_LOG_E("unable to allocate new client for instance=%d!", instance_no);
        goto error_return;
    }

    /* Accept connection */
    slen = sizeof(struct sockaddr_un);
    csd = accept(sd, NULL, &slen);
    client_p->sd = csd;

    if (csd < 0) {
        errnum = errno;
        SIMPB_LOG_E("accept() returns error(%d)=\"%s\"", errnum, strerror(errnum));
        goto cleanup_return;
    }

    /* Set socket to non-blocking */
    flags = fcntl(csd, F_GETFL, 0);

    if (flags < 0 || fcntl(csd, F_SETFL, flags | O_NONBLOCK) < 0) {
        errnum = errno;
        SIMPB_LOG_E("fcntl() returns error(%d)=\"%s\"", errnum, strerror(errnum));
        goto cleanup_return;
    }

    /* Let fdmon monitor events and set up a callback */
    client_context = MAKECONTEXT(instance_no, client_no);

    if (fdmon_add(csd, (void *)client_context, &sockserv_cb_recv, &sockserv_cb_close) < 0) {
        SIMPB_LOG_E("fdmon_add failed!");
        goto cleanup_return;
    }

    /* Notify upper layer that a connection has been accepted */
    instance_p->cb_func(instance_no, client_no, client_p->context_p,
                        SOCKSERV_REASON_CONNECTED, NULL, 0);

    /* All done */
    return client_no;

cleanup_return:
    sockserv_close(instance_no, client_no);
error_return:
    return -1;
}


/* Set client context for socket */
int sockserv_set_context(const int instance_no, const int client_no, void *context_p)
{
    sockserv_client_t *client_p = sockserv_get_client(instance_no, client_no);

    if (client_p) {
        client_p->context_p = context_p;
        return 0;
    }

    return -1;
}


/* Recv from socket, this is a callback function called by fdmon_waitevent() */
int sockserv_cb_recv(const int sd, const void *context)
{
    int instance_no = GETINSTANCE(context);
    int client_no = GETCLIENT(context);
    sockserv_instance_t *instance_p;
    sockserv_client_t *client_p;
    int errnum, unread;
    ssize_t rc;

    /* Check client */
    instance_p = sockserv_get_instance(instance_no);

    if (!instance_p) {
        SIMPB_LOG_E("invalid context %p!", context);
        goto error_return;
    }

    /* Check client */
    client_p = sockserv_get_client(instance_no, client_no);

    if (!client_p) {
        SIMPB_LOG_E("invalid context %p!", context);
        goto error_return;
    }

    /* Find out how many bytes are queued to be read on the socket */
    if (ioctl(sd, SIOCINQ, &unread) < 0) {
        /* Error handling */
        errnum = errno;
        SIMPB_LOG_E("ioctl(%d) returns error(%d)=\"%s\".", sd, errnum, strerror(errnum));
        goto error_return;
    }

    /* Always try to read at least 1 byte */
    unread = (unread > 0) ? unread : 1;

    /* Make sure we have enough buffer to hold it all */
    if ((client_p->used + unread) > client_p->size) {
        uint8_t *buf;
        buf = realloc(client_p->buf, (client_p->used + unread));

        if (buf) {
            client_p->buf = buf;
            client_p->size = client_p->used + unread;
        } else {
            SIMPB_LOG_E("buffer (re-)allocation failure!");
            goto error_return;
        }
    }

    do {
        rc = recv(sd, client_p->buf + client_p->used, unread, 0);

        /* recv() Successful handling */
        if (rc > 0) {
            unread -= rc;
            client_p->used += rc;
            /* Make call to decode and process message(s).
             * The decoder MUST process all complete messages even if
             * more than one message is present in the buffer. This is
             * because the callback is made only once per indication
             * as there is no way to determine the number of complete
             * messages in the buffer, if any.
             */
            rc = instance_p->cb_func(instance_no, client_no,
                                     client_p->context_p, SOCKSERV_REASON_RECEIVE,
                                     client_p->buf, client_p->used);

            /* Remove decoded data from buffer */
            if (rc > 0) {
                /* Partial decoding, remove decoded part */
                if (rc < client_p->used) {
                    memmove(client_p->buf, (client_p->buf + rc),
                            (client_p->used - rc));
                    client_p->used -= rc;
                } else {
                    /* All of it was decoded */
                    client_p->used = 0;
                }

                return 0;
            } else if (rc < 0) {
                /* Unrecoverable error occurred, get out */
                SIMPB_LOG_E("Unrecoverable error occurred, aborting");
                break;
            } else {
                /* Incomplete message, wait for more */
                SIMPB_LOG_D("Orderly shutdown by peer");
                return 0;
            }
        }
        /* recv() Error handling */
        else if (rc < 0) {
            if (errno == EINTR) {
                /* Interrupted syscall, try again */
                continue;
            } else {
                /* Socket related error, get out */
                errnum = errno;
                SIMPB_LOG_E("recv(%d) returns error(%d)=\"%s\".", sd, errnum, strerror(errnum));
                break;
            }
        } else {                /* recv() rc == 0 */
            /* Orderly shutdown by peer, shut down this end */
            break;
        }
    } while (unread > 0);

error_return:
    /* Note that that the callee fdmon_waitevent() will stop monitoring the
     * descriptor when the callback returns a negative value. The callee
     * will also implicitly make a callback to sockserv_close() so that
     * cleanup and housekeeping can be done. */
    return -1;
}


/* Send to socket */
int sockserv_send(const int instance_no, const int client_no, const void *buf, const size_t length)
{
    sockserv_client_t *client_p = sockserv_get_client(instance_no, client_no);
    int errnum, to_send;
    ssize_t rc;

    if (!buf) {
        SIMPB_LOG_E("No buffer");
        goto error_return;
    }

    if (length < 1) {
        SIMPB_LOG_E("Buffer length < 1");
        goto error_return;
    }

    if (!client_p) {
        SIMPB_LOG_E("client_p is NULL!");
        goto error_return;
    }

    to_send = length;

    do {
        rc = send(client_p->sd, buf, to_send, 0);

        if (rc > 0) {
            to_send -= rc;

            if (to_send < 1) {
                /* All has been sent: return success */
                return 0;
            }
        } else if (rc < 0) {
            /* Interrupted system-call: retry the operation */
            if (errno == EINTR) {
                continue;
            }
            /* Socket related error: drop out & return failure */
            else {
                errnum = errno;
                SIMPB_LOG_E("send(%d) returns error(%d)=\"%s\".",
                            client_p->sd, errnum, strerror(errnum));
                break;
            }
        } else {
            /* Nothing was sent, socket closed?: drop out & return failure */
            SIMPB_LOG_E("send(%d) socket was closed.",
                        client_p->sd);
            break;
        }
    } while (to_send > 0);

error_return:
    /* Note that that the ultimate callee fdmon_waitevent will stop monitoring the
     * descriptor if the callback returns a negative value */
    return -1;
}


/* Close a connected socket */
int sockserv_close(const int instance_no, const int client_no)
{
    sockserv_instance_t *instance_p;
    sockserv_client_t *client_p;

    /* Check instance */
    instance_p = sockserv_get_instance(instance_no);

    if (!instance_p) {
        SIMPB_LOG_E("invalid instance_no=%d!", instance_no);
        goto error_return;
    }

    /* Check client */
    client_p = sockserv_get_client(instance_no, client_no);

    if (!client_p) {
        SIMPB_LOG_E("invalid client_no=%d!", client_no);
        goto error_return;
    }

    /* Close the socket and release any context associated with it.
     * Note that the call is most likely coming from fdmon_del
     * so there is no need to make a call to stop monitoring.
     */
    close(client_p->sd);

    if (client_p) {
        instance_p->cb_func(instance_no, client_no, client_p->context_p,
                            SOCKSERV_REASON_DISCONNECTED, NULL, 0);
        sockserv_free_client(instance_no, client_no);
    }

    return 0;

error_return:
    return -1;
}


/* Close a connected socket - callback version */
int sockserv_cb_close(const int sd, const void *context)
{
    (void)sd;
    int instance_no = GETINSTANCE(context);
    int client_no = GETCLIENT(context);

    return sockserv_close(instance_no, client_no);
}


/* Destroy an instance of the socket server */
int sockserv_destroy(const int instance_no)
{
    sockserv_instance_t *instance_p = sockserv_get_instance(instance_no);
    int client_no;

    /* Close the server listen socket and release any context(s)
     * associated with it.
     */
    if (instance_p) {
        fdmon_del(instance_p->sd);
        close(instance_p->sd);

        for (client_no = 0; client_no < instance_p->client_max; client_no++) {
            if (instance_p->clients[client_no]) {
                sockserv_close(instance_no, client_no);
            }
        }

        if (instance_p->clients) {
            free(instance_p->clients);
            instance_p->clients = NULL;
        }

        sockserv_free_instance(instance_no);
    }

    return 0;
}


/* Module shutdown */
int sockserv_shutdown()
{
    int instance_no;

    /* Enumerate server service instances and call sockserv_destroy() for each */
    for (instance_no = 0; instance_no < instance_max; instance_no++) {
        if (instances[instance_no]) {
            sockserv_destroy(instance_no);
        }
    }

    free(instances);

    return 0;
}


/* Instance utility functions */
int sockserv_alloc_instance()
{
    sockserv_instance_t *instance_p;
    int instance_no;

    /* Find first available slot */
    for (instance_no = 0; instance_no < instance_max; instance_no++) {
        if (!instances[instance_no]) {
            instance_p = calloc(1, sizeof(sockserv_instance_t));

            if (instance_p) {
                instances[instance_no] = instance_p;
                instances_used++;
                return instance_no;
            } else {
                break;
            }
        }
    }

    return -1;
}


sockserv_instance_t *sockserv_get_instance(int instance_no)
{
    if (instance_no >= 0 && instance_no < instance_max) {
        return instances[instance_no];
    }

    return (sockserv_instance_t *)NULL;
}


int sockserv_free_instance(int instance_no)
{
    sockserv_instance_t *instance_p;

    if (instance_no < 0 || instance_no >= instance_max) {
        return -1;
    }

    instance_p = instances[instance_no];

    if (instance_p) {
        if (instance_p->clients_used > 0) {
            SIMPB_LOG_E("cannot free instance=%d (%p), clients connected", instance_no, instance_p);
            return -1;
        }

        if (instance_p->clients) {
            free(instance_p->clients);
        }

        instances[instance_no] = NULL;
        instances_used--;
        free(instance_p);
        return 0;
    }

    return -1;
}


/* Client utility functions */
int sockserv_alloc_client(int instance_no)
{
    sockserv_instance_t *instance_p;
    sockserv_client_t *client_p;
    int client_no;

    instance_p = sockserv_get_instance(instance_no);

    if (!instance_p) {
        return -1;
    }

    /* Allocate client structure pointer array */
    if (!instance_p->clients) {
        instance_p->clients = calloc(instance_p->client_max, sizeof(sockserv_client_t *));

        if (!instance_p->clients) {
            SIMPB_LOG_E("unable to allocate client array for instance=%d!", instance_no);
            return -1;
        }
    }

    /* Find first available slot */
    for (client_no = 0; client_no < instance_p->client_max; client_no++) {
        if (!instance_p->clients[client_no]) {
            client_p = calloc(1, sizeof(sockserv_client_t));

            if (client_p) {
                instance_p->clients[client_no] = client_p;
                instance_p->clients_used++;
                return client_no;
            } else {
                break;
            }
        }
    }

    return -1;
}


sockserv_client_t *sockserv_get_client(int instance_no, int client_no)
{
    sockserv_instance_t *instance_p = sockserv_get_instance(instance_no);

    if (instance_p) {
        if (client_no >= 0 && client_no < instance_p->client_max) {
            return instance_p->clients[client_no];
        }
    }

    return (sockserv_client_t *)NULL;
}


int sockserv_free_client(int instance_no, int client_no)
{
    sockserv_instance_t *instance_p;
    sockserv_client_t *client_p;

    instance_p = sockserv_get_instance(instance_no);

    if (!instance_p) {
        return -1;
    }

    if (!instance_p->clients) {
        return -1;
    }

    if (client_no < 0 || client_no >= instance_p->client_max) {
        return -1;
    }

    client_p = instance_p->clients[client_no];

    if (client_p) {
        instance_p->clients[client_no] = NULL;
        instance_p->clients_used--;

        if (client_p->buf) {
            free(client_p->buf);
        }

        free(client_p);
        return 0;
    }

    return -1;
}

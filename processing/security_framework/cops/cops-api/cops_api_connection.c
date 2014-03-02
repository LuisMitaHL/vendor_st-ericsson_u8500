/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <fcntl.h>
#include <cops_api_internal.h>
#include <cops_socket.h>
#include <cops_fd.h>
#include <cops_error.h>
#include <cops_log.h>

static int cops_api_connect(const char *path, int *fdp);

static int cops_api_fd_set_nonblocking(int fd);

const char *cops_api_socket_path = STR(COPS_SOCKET_PATH);

int cops_api_connect_request(cops_context_id_t *ctx, uint32_t *client_id)
{
    int       e;
    int       fd;
    uint32_t  clid = 0;
    ssize_t   res;

    e = cops_api_connect(cops_api_socket_path, &fd);

    if (e != 0) {
        return e;
    }

    if (cops_api_block_until_ready(fd)) {
        return -ETIMEDOUT;
    }

wagain:
    res = cops_write(fd, &clid, sizeof(clid));

    if (res != sizeof(clid)) {
        if (res == -1) {
            e = cops_errno();
            /* TODO: EINTR is not correct for PSock */
            if (e == EINTR) {
                goto wagain;
            }

            COPS_LOG(LOG_ERROR, "cops_write(clid): %s\n", cops_strerror(e));
            return e;
        } else {
            COPS_LOG(LOG_ERROR, "cops_write(clid): %d\n", (int)res);
            return -ENOSPC;
        }
    }

    if (cops_api_block_until_ready(fd)) {
        return -ETIMEDOUT;
    }

ragain:
    res = cops_read(fd, &clid, sizeof(clid));

    if (res != sizeof(clid)) {
        if (res == -1) {
            e = cops_errno();
            /* TODO: EINTR is not correct for PSock */
            if (e == EINTR) {
                goto ragain;
            }

            COPS_LOG(LOG_ERROR, "cops_read(clid): %s\n", cops_strerror(e));
            return e;
        } else {
            /* We don't accept partial client_id:s */
            COPS_LOG(LOG_ERROR, "cops_read(clid): %d\n", (int)res);
        }

        return -ENOSPC;
    }

    *client_id = clid;
    ctx->request_fd = fd;
    return 0;
}

int cops_api_connect_async(cops_context_id_t *ctx, uint32_t client_id)
{
    uint8_t   i;
    int       e;
    int       fd;
    uint32_t  event_ids[4];
    size_t    n = 0;
    ssize_t   res;

    e = cops_api_connect(cops_api_socket_path, &fd);

    if (e != 0) {
        return e;
    }

    ctx->async_fd = fd;

    if (cops_api_block_until_ready(fd)) {
        return -ETIMEDOUT;
    }

wagain1:
    res = cops_write(fd, &client_id, sizeof(client_id));

    if (res != sizeof(client_id)) {
        if (res == -1) {
            e = cops_errno();
            /* TODO: EINTR is not correct for PSock */
            if (e == EINTR) {
                goto wagain1;
            }

            COPS_LOG(LOG_ERROR, "cops_write(client_id): %s\n",
                     cops_strerror(e));
            return e;
        } else {
            COPS_LOG(LOG_ERROR, "cops_write(client_id): %d\n", (int)res);
            return -ENOSPC;
        }
    }

    if (ctx->event_cbs.simlock_status_changed_cb != NULL) {
        event_ids[n++] = COPS_API_EVENT_SIMLOCK_STATUS_CHANGED;
    }

    if (ctx->event_cbs.ota_unlock_status_updated_cb != NULL) {
        event_ids[n++] = COPS_API_EVENT_OTA_UNLOCK_STATUS_UPDATED;
    }

    if (ctx->event_cbs.authentication_changed_cb != NULL) {
        event_ids[n++] = COPS_API_EVENT_AUTHENTICATION_CHANGED;
    }


    event_ids[n++] = COPS_API_EVENTS_INVALID;

    for (i = 0; i < n; i++) {
wagain2:
        res = cops_write(fd, &event_ids[i], sizeof(uint32_t));

        if ((size_t)res != sizeof(uint32_t)) {
            if (res == -1) {
                e = cops_errno();
                /* TODO: EINTR is not correct for PSock */
                if (e == EINTR) {
                    goto wagain2;
                }

                COPS_LOG(LOG_ERROR, "cops_write(client_id * %d): %s\n",
                         (int)n, cops_strerror(e));
                return e;
            } else {
                COPS_LOG(LOG_ERROR, "cops_write(client_id * %d): %d\n",
                         (int)n, (int)res);
                return -ENOSPC;
            }
        }
    }

    if (cops_api_block_until_ready(fd)) {
        return -ETIMEDOUT;
    }

    {
        uint8_t   b;
ragain:
        res = cops_read(fd, &b, sizeof(b));

        if (res != sizeof(b)) {
            if (res == -1) {
                e = cops_errno();
                /* TODO: EINTR is not correct for PSock */
                if (e == EINTR) {
                    goto ragain;
                }

                COPS_LOG(LOG_ERROR, "cops_read(b): %s\n", cops_strerror(e));
                return e;
            } else {
                COPS_LOG(LOG_ERROR, "cops_read(b): %d\n", (int)res);
            }

            return -ENOSPC;
        }
    }

    return 0;
}

bool
cops_api_find_client(cops_server_context_id_t *ctx, int fd,
                     cops_client_t **client)
{
    cops_client_t *c;

    for (c = ctx->clients.tqh_first; c != NULL; c = c->link.tqe_next) {
        if (fd == c->request_fd || fd == c->async_fd) {
            *client = c;
            return true;
        }
    }

    return false;
}

void
cops_api_init_client_connection(cops_server_context_id_t *ctx, int fd,
                                cops_client_t *client)
{

    if (fd == -1) {
        COPS_LOG(LOG_ERROR, "fd is -1\n");
        return;
    }

    /*
     * A client connection is initialized in the following step:
     *
     * 1.   Accpet (cops_api_accept) request_fd
     * 2.   Receive client id = 0
     * 2.1A (client id = 0) Send client id (request_fd) on request_fd
     *      continue as 3.
     * 2.1B (client_id = -1) Async connection.
     *      async_fd is assigned request_fd, request_fd is assinged -1
     *      continue as 5.
     * 3.   Accept (cops_api_accept) event_fd (but we treat it as
     *      if it was a request connection as we haven't reveived the
     *      client id yet).
     * 4.   Receive client id = request_fd (a previous registered
     *      request_fd which cops_client_t doesn't have an event_fd
     *      yet).
     * 5.   Reveive event subscriptions.
     * 6.   Reveive end of event subscriptions.
     * 6.1. cops_client_t is completely initialized.
     */

    if (!client->request_inited) {
        uint32_t  client_id;
        ssize_t   res;

        /* 2. */

        if (fd != client->request_fd) {
            COPS_LOG(LOG_ERROR, "Client request connection isn't inited\n");
            cops_api_close_client(ctx, client);
            return;
        }

ragain1:
        res = cops_read(fd, &client_id, sizeof(client_id));

        if (res != sizeof(client_id)) {
            if (res == -1) {
                int e = cops_errno();
                /* TODO: EINTR is not correct for PSock */
                if (e == EINTR) {
                    goto ragain1;
                }

                if (cops_error_is_eagain(e)) {
                    /* There wasn't anything to read after all */
                    return;
                }

                COPS_LOG(LOG_ERROR, "cops_read(client_id): %s\n",
                         cops_strerror(e));
            } else {
                /* We don't accept partial client_id:s */
                COPS_LOG(LOG_ERROR, "cops_read(client_id): %d\n", (int)res);
            }

            cops_api_close_client(ctx, client);
            return;
        }

        if (client_id == 0) {
            /* 2.1A. */

            client->request_inited = true;

            client_id = client->request_fd;
wagain1:
            res = cops_write(fd, &client_id, sizeof(client_id));

            if (res != sizeof(client_id)) {
                if (res == -1) {
                    int e = cops_errno();
                    /* TODO: EINTR is not correct for PSock */
                    if (e == EINTR) {
                        goto wagain1;
                    }

                    COPS_LOG(LOG_ERROR, "cops_write(client_id): %s\n",
                             cops_strerror(e));
                } else {
                    COPS_LOG(LOG_ERROR, "cops_write(client_id): %d\n",
                             (int)res);
                }

                cops_api_close_client(ctx, client);
                return;
            }

            return;
        } else if (client_id == (uint32_t)-1) {
            /* 2.1B. */
            client->request_inited = true;
            client->async_fd = client->request_fd;
            client->request_fd = -1;
            return;
        }

        /* 4. */

        /*
         * It is a event connection, scrap this cops_client_t and
         * put the fd in event_fd of a cops_client_t with
         * request_fd == client_id.
         */
        client->request_fd = -1;        /* avoid having the fd closed below */
        cops_api_close_client(ctx, client);

        if (!cops_api_find_client(ctx, client_id, &client)) {
            /*
             * Didn't find any matching cops_client_t, let's close
             * this fd.
             */
            COPS_LOG(LOG_ERROR, "Couldn't find client_id %d\n", (int)client_id);
            (void)cops_close(fd);
            return;
        }

        if (client->async_fd != -1) {
            /*
             * Found a matching cops_client_t but it already had
             * an event_fd assigned.
             */
            COPS_LOG(LOG_ERROR, "Couldn't find client_id %d\n", (int)client_id);
            (void)cops_close(fd);
            return;
        }

        client->async_fd = fd;
    }

    while (!client->inited) {
        uint32_t  event;
        ssize_t   res;

        /* 5. */

        /* Needed to get same behaviour in OSE and Linux, not sure why!! */
        if (cops_api_block_until_ready(client->async_fd)) {
            COPS_LOG(LOG_ERROR, "cops_api_block_until_ready timed out\n");
            cops_api_close_client(ctx, client);
            return;
        }
ragain2:
        res = cops_read(client->async_fd, &event, sizeof(event));

        if (res != sizeof(event)) {
            if (res == -1) {
                int e = cops_errno();
                /* TODO: EINTR is not correct for PSock */
                if (e == EINTR) {
                    goto ragain2;
                }

                if (cops_error_is_eagain(e)) {
                    /* There wasn't anything to read after all */
                    return;
                }

                COPS_LOG(LOG_ERROR, "cops_read(%d, event): %s\n",
                         client->async_fd, cops_strerror(e));
            } else {
                /* We don't accept partial event:s */
                COPS_LOG(LOG_ERROR, "cops_read(event): %d\n", (int)res);
            }

            cops_api_close_client(ctx, client);
            return;
        }

        if ((cops_api_event_t)event != COPS_API_EVENTS_INVALID) {
            cops_api_event_t *evs = client->events;
            size_t    n;

            for (n = 0; evs != NULL && evs[n] != COPS_API_EVENTS_INVALID; n++)
                ;

            /* make room for stop-marker (COPS_API_EVENTS_INVALID) */
            n++;

            evs = realloc(evs, sizeof(*evs) * (n + 1));

            if (evs == NULL) {
                /*
                 * XXX Should we exit here since it's likely that COPS
                 * doesn't work at all if this would happen?
                 * /ejenwik 100113
                 */
                COPS_LOG(LOG_ERROR, "realloc: %s\n", strerror(errno));
                cops_api_close_client(ctx, client);
                return;
            }

            client->events = evs;

            evs[n-1] = (cops_api_event_t)event;
            evs[n] = COPS_API_EVENTS_INVALID;
        } else {
            uint8_t   b = 0;
            /* 6. */

wagain2:
            res = cops_write(client->async_fd, &b, sizeof(b));

            if (res != sizeof(b)) {
                if (res == -1) {
                    int e = cops_errno();
                    /* TODO: EINTR is not correct for PSock */
                    if (e == EINTR) {
                        goto wagain2;
                    }

                    COPS_LOG(LOG_ERROR, "cops_write(b): %s\n",
                             cops_strerror(e));
                } else {
                    COPS_LOG(LOG_ERROR, "cops_write(b): %d\n", (int)res);
                }

                cops_api_close_client(ctx, client);
                return;
            }

            /* 6.1. */
            /*
             * Have all event subscriptions, the client is now fully
             * connected.
             */
            client->inited = true;

        }
    }
}

int cops_api_listen(cops_server_context_id_t *ctx, const char *path)
{
    int fd;
    size_t plen;

    plen = strlen(path);

    if (plen > COPS_PATH_MAX) {
        /*lint -e557 Suppress unrecognized format %zu which is used in Linux */
        COPS_LOG(LOG_ERROR, "Path \"%s\" is too long %zu "
                 "max is %u\n", path, plen, COPS_PATH_MAX);
        /*lint +e557 */
        return -ENAMETOOLONG;
    }

    fd = cops_socket(true, &ctx->data);

    if (fd == -1) {
        int e = cops_errno();
        COPS_LOG(LOG_ERROR, "cops_socket: %s\n", cops_strerror(e));
        return e;
    }

    if (cops_api_fd_set_nonblocking(fd) == -1) {
        int e = cops_errno();

        COPS_LOG(LOG_ERROR, "cops_api_fd_set_nonblocking: %s\n",
                 cops_strerror(e));
        (void)cops_close(fd);
        return e;
    }

#ifndef COPS_OSE_ENVIRONMENT
    (void)unlink(path);         /* may fail */
#endif

    if (cops_bind(fd, path) == -1) {
        int e = cops_errno();

        /* PSock portmapper can return error without errno being set */
        if (0 == e) {
            e = 1;
        }

        COPS_LOG(LOG_ERROR, "cops_bind(%d, \"%s\"): %s\n", fd,
                 path, cops_strerror(e));
        (void)cops_close(fd);
        return e;
    }

    if (cops_listen(fd, 5) == -1) {
        int e = cops_errno();

        COPS_LOG(LOG_ERROR, "cops_listen(%d, 5): %s\n", fd,
                 cops_strerror(e));
        (void)cops_close(fd);
        return e;
    }

    ctx->data.listen_fd = fd;
    return 0;
}

int cops_api_accept(cops_server_context_id_t *ctx)
{
    while (1) {
        int fd;
        cops_client_t *client;

        fd = cops_accept(ctx->data.listen_fd);

        if (fd == -1) {
            return 0;    /* The wasn't any connection after all */
        }

        client = calloc(1, sizeof(*client));

        if (client == NULL) {
            COPS_LOG(LOG_ERROR, "calloc: %s\n", strerror(errno));
            return -1;
        }

        client->async_fd = -1;

        client->request_fd = fd;
        TAILQ_INSERT_TAIL(&ctx->clients, client, link);
    }
}

void cops_api_close_client(cops_server_context_id_t *ctx, cops_client_t *client)
{
    if (client->request_fd != -1) {
        (void)cops_close(client->request_fd);
    }

    if (client->async_fd != -1) {
        (void)cops_close(client->async_fd);
    }

    if (client->events != NULL) {
        free(client->events);
    }

    TAILQ_REMOVE(&ctx->clients, client, link);

    memset(client, 0, sizeof(*client));
    free(client);
}

cops_return_code_t cops_api_block_until_ready(int fd)
{
    /*
     * PSock is always non blocking and we must handle
     * internal signals before fd is ready
     */
#ifdef COPS_OSE_ENVIRONMENT
    fd_set *rfds = NULL;
    int nfds;
    int res;
    struct timeval timeout = {TIMEOUT_SEC, TIMEOUT_USEC};

    rfds = (fd_set *)malloc(sizeof(fd_set));

    if (rfds == NULL) {
        return COPS_RC_MEMORY_ALLOCATION_ERROR;
    }

    FD_ZERO(rfds);
    nfds = 0;

    if (nfds < fd) {
        nfds = fd;
    }

    FD_SET(fd, rfds);

    res = cops_select(nfds + 1, rfds, NULL, NULL, &timeout);

    if (0 < res) {
        free(rfds);
        return COPS_RC_OK;
    } else  {
        free(rfds);
        return COPS_RC_INTERNAL_IPC_ERROR;
    }
#else
    (void) fd;
    return COPS_RC_OK;
#endif
}

static int cops_api_connect(const char *path, int *fdp)
{
    int fd;
    size_t plen;

    plen = strlen(path);

    if (plen > COPS_PATH_MAX) {
        /*lint -e557 Suppress unrecognized format %zu which is used in Linux */
        COPS_LOG(LOG_ERROR, "Path \"%s\" is too long %zu "
                    "max is %u\n", path, plen, COPS_PATH_MAX);
        /*lint +e557 */
        return -ENAMETOOLONG;
    }

    fd = cops_socket(false, NULL);

    if (fd == -1) {
        int e = cops_errno();
        COPS_LOG(LOG_ERROR, "cops_socket: %s\n", cops_strerror(e));
        return e;
    }

    if (cops_connect(fd, path) == -1) {
        int e = cops_errno();

        /* PSock portmapper can return error without errno being set */
        if (0 == e) {
            e = 1;
        }

        COPS_LOG(LOG_ERROR, "connect(%d, \"%s\"): %s\n", fd,
                 path, cops_strerror(e));
        (void)cops_close(fd);
        return e;
    }

    *fdp = fd;
    return 0;
}

static int cops_api_fd_set_nonblocking(int fd)
{
    int val = cops_fcntl(fd, F_GETFL, 0);

    if (val < 0) {
        return -1;
    }

    val |= O_NONBLOCK;

    if (cops_fcntl(fd, F_SETFL, val) < 0) {
        return -1;
    }

    return 0;
}

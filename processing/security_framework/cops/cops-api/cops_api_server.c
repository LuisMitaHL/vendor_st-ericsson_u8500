/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_api_internal.h>
#include <cops_sipc.h>
#include <cops_fd.h>
#include <cops_shared.h>
#include <cops_log.h>

static void cops_server_status_changed(cops_server_context_id_t *ctx,
                                       cops_sim_status_t *status,
                                       bool *status_changed);

cops_return_code_t
cops_server_context_create(cops_server_context_id_t **ctxpp,
                           const cops_server_callbacks_t *cbs, void *aux)
{
    cops_server_context_id_t *ctx = NULL;
    int       e;

    if (ctxpp == NULL || cbs == NULL) {
        COPS_LOG(LOG_ERROR, "Mandatory arguments NULL\n");
        goto error_return;
    }

    ctx = calloc(1, sizeof(*ctx));

    if (ctx == NULL) {
        COPS_LOG(LOG_ERROR, "cops_server_context_create: calloc: %s\n",
                 strerror(errno));
        goto error_return;
    }

    ctx->data.listen_fd = -1;
    ctx->data.request_fd = -1;
    ctx->data.state = COPS_SOCKET_STATE_UNUSED;
    ctx->data.aux = aux;

    ctx->cbs = *cbs;
    TAILQ_INIT(&ctx->clients);

    e = cops_api_listen(ctx, cops_api_socket_path);

    if (e != 0) {
        goto error_return;
    }

    *ctxpp = ctx;
    return COPS_RC_OK;

error_return:
    cops_server_context_destroy(&ctx);
    return COPS_RC_IPC_ERROR;
}

void cops_server_context_destroy(cops_server_context_id_t **ctxpp)
{
    if (ctxpp == NULL || *ctxpp == NULL) {
        return;
    }

    while ((*ctxpp)->clients.tqh_first != NULL) {
        cops_api_close_client(*ctxpp, (*ctxpp)->clients.tqh_first);
    }

    if ((*ctxpp)->data.listen_fd != -1) {
        (void)cops_close((*ctxpp)->data.listen_fd);
    }

    if ((*ctxpp)->saved_simcard_status != NULL) {
        free((*ctxpp)->saved_simcard_status);
    }

    free(*ctxpp);
    *ctxpp = NULL;
}

void
cops_server_context_get_fdset(cops_server_context_id_t *ctxp,
                              int *nfds, fd_set *readfds)
{
    cops_client_t *clnt;

    if (ctxp == NULL || nfds == NULL || readfds == NULL) {
        COPS_LOG(LOG_ERROR, "Mandatory arguments NULL\n");
        return;
    }

    if (*nfds < ctxp->data.listen_fd) {
        *nfds = ctxp->data.listen_fd;
    }

    FD_SET(ctxp->data.listen_fd, readfds);

    for (clnt = ctxp->clients.tqh_first; clnt != NULL;
            clnt = clnt->link.tqe_next) {
        int       fd;

        if (clnt->request_fd != -1) {
            fd = clnt->request_fd;
        } else {
            fd = clnt->async_fd;
        }

        if (fd < 0 || fd >= FD_SETSIZE) {
            COPS_LOG(LOG_ERROR, "fd (%d) out range\n", fd);
            continue;
        }

        if (*nfds < fd) {
            *nfds = fd;
        }
        /*lint -e661 */
        FD_SET(fd, readfds);
        /*lint +e661 */
    }
}

void
cops_server_context_handle_requests(cops_server_context_id_t *ctxp,
                                    int nfds, fd_set *readfds)
{
    cops_return_code_t ret_code;
    cops_client_t *clnt, *clnt_next;

    (void) nfds;

    if (FD_ISSET(ctxp->data.listen_fd, readfds)) {
        (void)cops_api_accept(ctxp);
    }

    for (clnt = ctxp->clients.tqh_first; clnt != NULL; clnt = clnt_next) {
        int fd;
        cops_sipc_message_t *msg = NULL;

        /*
         * Save "next" here as cops_api_init_client_connection()
         * below may invalidate clnt.
         */
        clnt_next = clnt->link.tqe_next;

        if (clnt->request_fd != -1) {
            fd = clnt->request_fd;
        } else {
            fd = clnt->async_fd;
        }

        if (fd < 0 || fd >= FD_SETSIZE) {
            COPS_LOG(LOG_ERROR, "fd (%d) out range, closing client\n", fd);
            cops_api_close_client(ctxp, clnt);
            continue;
        }

        /*lint -e661 */
        if (!FD_ISSET(fd, readfds)) {
            continue;
        }
        /*lint +e661 */

        if (!clnt->inited) {
            cops_api_init_client_connection(ctxp, fd, clnt);
            continue;
        }

        /* TODO: Handle multiple messages in receive */
        if (-1 == cops_sipc_recv(fd, &msg)) {
            COPS_LOG(LOG_ERROR,
                     "cops_sipc_recv failed, closing client with fd %d\n", fd);
            cops_api_close_client(ctxp, clnt);
            continue;
        }

        COPS_LOG(LOG_INFO,
                 "Received SIPC from fd = %u. Type = 0x%x, length = %u\n",
                 fd, msg->msg_type, msg->length);

        ret_code = ctxp->cbs.msg_handle_complete(ctxp->data.aux, msg, fd);

        /* TODO: Investigate close_client for OSE */
        if (COPS_RC_OK != ret_code) {
            COPS_LOG(LOG_ERROR,
                     "handle_message failed with error code:0x%x, "
                     "closing client with fd:%d\n", ret_code, fd);
            cops_api_close_client(ctxp, clnt);
        }

        cops_sipc_free_message(&msg);
    }
}

void cops_server_send_event(cops_server_context_id_t *ctxp,
                            cops_sipc_message_t *msg)
{
    cops_client_t *client = ctxp->clients.tqh_first;
    cops_client_t *tmp_client = NULL;
    size_t offset = 0;
    cops_api_event_t event = COPS_API_EVENTS_INVALID;
    bool dispatch = false;

    if (COPS_RC_OK != cops_tapp_sipc_get_uint8(msg, &offset,
                                               (uint8_t *)&event)) {
        COPS_LOG(LOG_ERROR, "Failed to extract event from message\n");
        return;
    }

    /* Depending on event check if it should be dispatched */
    if (COPS_API_EVENT_SIMLOCK_STATUS_CHANGED == event) {
        cops_sim_status_t status;

        if (COPS_RC_OK != cops_tapp_sipc_get_opaque(msg, &offset, &status,
                                                   sizeof(cops_sim_status_t))) {
            COPS_LOG(LOG_ERROR, "Failed to extract status from message\n");
            return;
        }

        if (COPS_SIMLOCK_CARD_STATUS_NOT_CHECKED != status.card_status) {
            cops_server_status_changed(ctxp, &status, &dispatch);
        }
    } else if (COPS_API_EVENT_OTA_UNLOCK_STATUS_UPDATED == event ||
               COPS_API_EVENT_AUTHENTICATION_CHANGED == event) {
        dispatch = true;
    }

    while (dispatch && client != NULL) {
        cops_api_event_t *events = client->events;

        if (events != 0) {
            int n;

            for (n = 0;
                 events != NULL && events[n] != COPS_API_EVENTS_INVALID; n++) {
                if (event == events[n]) {
                    int fd;

                    if (client->async_fd != 0) {
                        fd = client->async_fd;
                    } else {
                        fd = client->request_fd;
                    }

                    if (cops_sipc_send(fd, msg) != COPS_RC_OK) {
                        COPS_LOG(LOG_ERROR, "Failed to send event message to "
                                 "client with fd %d, closing client\n", fd);

                        /* Jump to next client and try to send to this. */
                        tmp_client = client->link.tqe_next;

                        /* Sending to client failed, remove it. */
                        cops_api_close_client(ctxp, client);
                        client = NULL;
                        break;
                    }
                }
            }
        }

        if (NULL != client) {
            client = client->link.tqe_next;
        } else if (NULL != tmp_client) {
            client = tmp_client;
            tmp_client = NULL;
        }
    }
}

static void cops_server_status_changed(cops_server_context_id_t *ctx,
                                       cops_sim_status_t *status,
                                       bool *status_changed)
{
    *status_changed = false;

    if (NULL == ctx->saved_simcard_status) {
        ctx->saved_simcard_status = malloc(sizeof(cops_sim_status_t));

        if (NULL == ctx->saved_simcard_status) {
            COPS_LOG(LOG_ERROR, "Failed to allocate memory\n");
        }

        /* if there is no saved previous status then the event should be sent */
        *status_changed = true;
    } else {
        if ((ctx->saved_simcard_status->card_status != status->card_status) ||
            (ctx->saved_simcard_status->failed_lt != status->failed_lt)) {
            *status_changed = true;
        }
    }

    if (NULL != ctx->saved_simcard_status) {
        ctx->saved_simcard_status->card_status = status->card_status;
        ctx->saved_simcard_status->failed_lt = status->failed_lt;
    }
}

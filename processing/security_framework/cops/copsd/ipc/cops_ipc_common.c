/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_ipc_common.h>
#include <cops_common.h>
#include <cops_sipc.h>
#include <cops_fd.h>
#include <cops_error.h>
#include <cops_msg_handler.h>
#include <cops_router.h>
#include <stdlib.h>

cops_return_code_t cops_ipc_context_create(struct cops_ipc_context **ctx,
                                           bool server, void *aux)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int fd = -1;

    if (ctx == NULL) {
        COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "ctx == NULL\n");
    }

    /* *ctx must be zeroed as context_detroy otherwise will fail
       if ipc_open fails */
    *ctx = calloc(1, sizeof(struct cops_ipc_context));

    COPS_CHK_ALLOC(*ctx);

    (*ctx)->server = server;
    (*ctx)->data.listen_fd = -1;
    (*ctx)->data.request_fd = -1;
    (*ctx)->data.state = COPS_SOCKET_STATE_WAITING;
    (*ctx)->data.aux = aux;
    (*ctx)->internal_ctx = NULL;

    COPS_CHK_RC(cops_ipc_open(*ctx, &fd));

    COPS_LOG(LOG_INFO, "Got fd (%d) for %s.\n", fd,
             server ? STR(SIPC_SERVER) : STR(SIPC_CLIENT));

    if (server) {
        (*ctx)->data.listen_fd = fd;
    } else {
        (*ctx)->data.request_fd = fd;
    }

function_exit:
    if (ret_code != COPS_RC_OK && ctx != NULL && *ctx != NULL) {
        free(*ctx);
        *ctx = NULL;
    }

    return ret_code;
}

cops_return_code_t cops_ipc_context_start(struct cops_ipc_context *ctx)
{
    return cops_ipc_start(ctx);
}

void cops_ipc_context_destroy(struct cops_ipc_context **ctxpp)
{
    if (ctxpp == NULL || *ctxpp == NULL) {
        return;
    }

    /* Destroy internal context */
    if (NULL != (*ctxpp)->internal_ctx &&
        NULL != (*ctxpp)->internal_cbs.destroy) {
        (*ctxpp)->internal_cbs.destroy(*ctxpp);
    }

    if ((*ctxpp)->data.listen_fd != -1 &&
        cops_close((*ctxpp)->data.listen_fd) != 0) {
        COPS_LOG(LOG_ERROR, "Failed to close fd %u, Error: %s\n",
                 (*ctxpp)->data.listen_fd,  cops_strerror(cops_errno()));
    }

    if ((*ctxpp)->data.request_fd != -1 &&
        cops_close((*ctxpp)->data.request_fd) != 0) {
        COPS_LOG(LOG_ERROR, "Failed to close fd %u, Error: %s\n",
                 (*ctxpp)->data.request_fd,  cops_strerror(cops_errno()));
    }

    free(*ctxpp);
    *ctxpp = NULL;
}

void cops_ipc_context_get_fdset(struct cops_ipc_context *ctxp, int *nfds,
                                fd_set *readfds)
{
    /* Get fd_set for internal context */
    if (NULL != ctxp->internal_ctx && NULL != ctxp->internal_cbs.get_fdset) {
        ctxp->internal_cbs.get_fdset(ctxp, nfds, readfds);
    }

    /* If internal context exists only add fds if connection is ready */
    if (NULL == ctxp->internal_ctx ||
        COPS_SOCKET_STATE_READY == ctxp->data.state) {

        if (ctxp->data.listen_fd != -1) {
            FD_SET(ctxp->data.listen_fd, readfds);

            if (ctxp->data.listen_fd > *nfds) {
                *nfds = ctxp->data.listen_fd;
            }
        }

        if (ctxp->data.request_fd != -1) {
            FD_SET(ctxp->data.request_fd, readfds);

            if (ctxp->data.request_fd > *nfds) {
                *nfds = ctxp->data.request_fd;
            }
        }
    }
}

/* Messages that are received over IPC are handled here */
void cops_ipc_context_handle_signals(struct cops_ipc_context *ctxp,
                                     struct cops_state *state,
                                     int nfds, fd_set *readfds)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int fd = ctxp->data.request_fd;
    cops_sipc_message_t *in_msg = NULL;
    cops_sipc_message_t *out_msg = NULL;

    /* Handle signals for internal context */
    if (NULL != ctxp->internal_ctx &&
        NULL != ctxp->internal_cbs.handle_signals) {
        ctxp->internal_cbs.handle_signals(ctxp, nfds, readfds);
    }

    /* If internal context exists only handle signals if connection is ready */
    if (NULL == ctxp->internal_ctx ||
        COPS_SOCKET_STATE_READY == ctxp->data.state) {

        if (fd < 0 || fd > nfds || !FD_ISSET(fd, readfds)) {
            return;
        }

        /* TODO: Handle multiple messages in receive */
        if (-1 == cops_sipc_recv(fd, &in_msg)) {
            COPS_SET_RC(COPS_RC_INTERNAL_ERROR, "cops_sipc_recv failed\n");
        }

        COPS_LOG(LOG_INFO, "Received SIPC from fd = %u. Type = 0x%x, "
                 "length = %u\n", fd, in_msg->msg_type, in_msg->length);

        /* Perform the invoke and other handling of the message */
        ret_code = cops_msg_handle(state, in_msg, &out_msg);

        if (COPS_RC_OK != ret_code) {
            COPS_CHK(NULL != out_msg, ret_code);
            COPS_LOG(LOG_WARNING, "cops_msg_handle completed with error:0x%x\n",
                     ret_code);
        }
        /* Send the message to the correct receiver */
        COPS_CHK_RC(cops_router_handle_msg(state, fd, in_msg, out_msg));
        /* Trigger event handling if applicable to current message */
        (void)cops_msg_trigger_event_handling(state, out_msg);
        /* Dispatch event if applicable to current message */
        (void)cops_msg_dispatch_event(state, out_msg);
    }
function_exit:
    if (out_msg != in_msg) {
        cops_sipc_free_message(&out_msg);
    }
    cops_sipc_free_message(&in_msg);
}

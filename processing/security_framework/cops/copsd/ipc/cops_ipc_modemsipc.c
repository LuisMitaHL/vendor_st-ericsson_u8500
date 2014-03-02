/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include <cops_ipc_common.h>
#include <cops_common.h>
#include <fcntl.h>

#ifdef COPS_SILENT_REBOOT_SUPPORT
#include <shm_netlnk.h>

struct cops_shmnetlnk_context {
    int reset_ind;
    int fd;
};

static void cops_ipc_destroy(struct cops_ipc_context *ctx);
static void cops_ipc_get_fdset(struct cops_ipc_context *ctx,
                               int *nfds, fd_set *readfds);
static void cops_ipc_handle_signals(struct cops_ipc_context *ctx,
                                    int nfds, fd_set *readfds);
#endif

cops_return_code_t cops_ipc_open(struct cops_ipc_context *ctx, int *fd)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int local_fd = -1;
#ifdef COPS_SILENT_REBOOT_SUPPORT
    int netlnkfd = -1;
    int ret;
    struct cops_shmnetlnk_context *netlnk_ctx = NULL;
#endif

    /* Sanity check */
    if (ctx->server) {
        COPS_SET_RC(COPS_RC_ARGUMENT_ERROR, "Cant open a modem sipc server\n");
    }

    local_fd = open(STR(SIPC_CLIENT), O_RDWR);

    if (-1 == local_fd) {
        COPS_SET_RC(COPS_RC_INTERNAL_IPC_ERROR,
                    "open() failed on: %s\n", STR(SIPC_CLIENT));
    }
    *fd = local_fd;

#ifdef COPS_SILENT_REBOOT_SUPPORT
    /* Open SHRM Netlink socket */
    ret = netlnk_socket_create(&netlnkfd);

    if (ret < 0 || netlnkfd == -1) {
        COPS_SET_RC(COPS_RC_INTERNAL_IPC_ERROR,
                    "netlnk_socket_create() failed\n");
    }

    netlnk_ctx = malloc(sizeof(struct cops_shmnetlnk_context));
    COPS_CHK_ALLOC(netlnk_ctx);

    netlnk_ctx->fd = netlnkfd;
    netlnk_ctx->reset_ind = 0;

    ctx->internal_ctx = netlnk_ctx;
    ctx->internal_cbs.destroy = cops_ipc_destroy;
    ctx->internal_cbs.get_fdset = cops_ipc_get_fdset;
    ctx->internal_cbs.handle_signals = cops_ipc_handle_signals;
#endif
    ctx->data.state = COPS_SOCKET_STATE_READY;

function_exit:
    return ret_code;
}

cops_return_code_t cops_ipc_start(struct cops_ipc_context *ctx)
{
    (void) ctx;

    return COPS_RC_OK;
}

#ifdef COPS_SILENT_REBOOT_SUPPORT
static void cops_ipc_destroy(struct cops_ipc_context *ctx)
{
    struct cops_shmnetlnk_context *netlnk_ctx = ctx->internal_ctx;

    netlnk_socket_close(netlnk_ctx->fd);

    free(ctx->internal_ctx);
    ctx->internal_ctx = NULL;
}

static void cops_ipc_get_fdset(struct cops_ipc_context *ctx,
                               int *nfds, fd_set *readfds)
{
    struct cops_shmnetlnk_context *netlnk_ctx = ctx->internal_ctx;

    FD_SET(netlnk_ctx->fd, readfds);

    if (netlnk_ctx->fd > *nfds) {
        *nfds = netlnk_ctx->fd;
    }
}

static void cops_ipc_handle_signals(struct cops_ipc_context *ctx,
                                    int nfds, fd_set *readfds)
{
    int msg;
    struct cops_shmnetlnk_context *netlnk_ctx = ctx->internal_ctx;

    if (FD_ISSET(netlnk_ctx->fd, readfds)) {
        /*  receive netlink message since there's a message waiting */
        netlnk_socket_recv(netlnk_ctx->fd, &msg);

        if (msg == MODEM_RESET_IND) {
            /* Send pause indication to child */
            COPS_LOG(LOG_INFO, "Modem reset indication\n");
            netlnk_ctx->reset_ind = 1;
            ctx->data.state = COPS_SOCKET_STATE_WAITING;
        } else if (netlnk_ctx->reset_ind && msg == MODEM_STATUS_ONLINE) {
            /* send resume indication to child */
            COPS_LOG(LOG_INFO, "Modem status online\n");
            netlnk_ctx->reset_ind = 0;
            ctx->data.state = COPS_SOCKET_STATE_READY;
        }
    }
}
#endif /* COPS_SILENT_REBOOT_SUPPORT */


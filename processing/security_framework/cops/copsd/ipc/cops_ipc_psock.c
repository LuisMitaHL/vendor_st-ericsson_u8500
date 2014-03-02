/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include <cops_ipc_common.h>
#include <cops_common.h>
#include <cops_psock.h>

cops_return_code_t cops_ipc_open(struct cops_ipc_context *ctx, int *fd)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int local_fd = -1;

    local_fd = cops_psock_socket(ctx->server, &ctx->data);

    if (-1 == local_fd) {
        COPS_SET_RC(COPS_RC_IPC_ERROR, "cops_psock_socket() failed\n");
    }

    *fd = local_fd;

function_exit:
    return ret_code;
}

cops_return_code_t cops_ipc_start(struct cops_ipc_context *ctx)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    if (COPS_SOCKET_STATE_WAITING == ctx->data.state) {

        if (ctx->server) {

            if (-1 == cops_psock_bind(ctx->data.listen_fd, STR(SIPC_SERVER))) {
                COPS_SET_RC(COPS_RC_IPC_ERROR, "cops_psock_bind() failed\n");
            }

            if (-1 == cops_psock_listen(ctx->data.listen_fd, 5)) {
                COPS_SET_RC(COPS_RC_IPC_ERROR, "cops_psock_listen() failed\n");
            }
        } else {

            if (-1 == cops_psock_connect(ctx->data.request_fd,
                                         STR(SIPC_CLIENT))) {
                COPS_SET_RC(COPS_RC_IPC_ERROR, "cops_psock_connect() failed\n");
            }
        }
        ctx->data.state = COPS_SOCKET_STATE_INITIATED;
    }

function_exit:
    return ret_code;
}


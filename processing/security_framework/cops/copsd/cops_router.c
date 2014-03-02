/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include <cops_router.h>
#include <cops_msg_handler.h>
#include <cops_sipc.h>
#include <cops_sipc_message.h>
#include <cops_srv.h>
#include <cops_common.h>
#include <cops_ipc_common.h>
#include <queue.h>
#include <stdlib.h>
#ifdef COPS_OSE_ENVIRONMENT
#include <r_os.h>
#include <r_serviceregister.h>
#endif

extern uint8_t run_mode;

struct cops_router_client {
    int fd;
    uint8_t msg;
    TAILQ_ENTRY(cops_router_client) link;
};

/*lint -e659 */
TAILQ_HEAD(cops_router_client_list, cops_router_client);
/*lint +e659 */

static struct cops_router_client_list router_client_list;

static cops_return_code_t cops_router_handle_trigger(struct cops_state *state,
                                                     cops_sipc_message_t *msg,
                                                     int fd);

static cops_return_code_t cops_router_handle_ipc_state(struct cops_state *state,
                                                      cops_sipc_message_t *msg);

static cops_return_code_t cops_router_handle_request(struct cops_state *state,
                                         cops_sipc_message_t *msg_before_invoke,
                                         cops_sipc_message_t *msg_after_invoke,
                                         int fd);

static cops_return_code_t cops_router_handle_response(int fd,
                                                      cops_sipc_message_t *msg);

static cops_return_code_t cops_router_add_client(int fd,
                                                 cops_sipc_message_t *msg);

void cops_router_init(void)
{
    TAILQ_INIT(&router_client_list);
}

cops_return_code_t cops_router_handle_msg(struct cops_state *state, int fd,
                                         cops_sipc_message_t *msg_before_invoke,
                                         cops_sipc_message_t *msg_after_invoke)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    if (COPS_SIPC_TRIGGER_MSG == msg_after_invoke->msg_type) {
        /* Handle trigger message */
        COPS_CHK_RC(cops_router_handle_trigger(state, msg_after_invoke, fd));
    } else if (COPS_SIPC_IPC_STATE_REQ == msg_after_invoke->msg_type ||
               COPS_SIPC_IPC_STATE_RESP == msg_after_invoke->msg_type) {
        /* Handle IPC state */
        COPS_CHK_RC(cops_router_handle_ipc_state(state, msg_after_invoke));
    } else if (COPS_SIPC_IS_REQUEST(msg_after_invoke)) {
        /* Handle request */
        COPS_CHK_RC(cops_router_handle_request(state, msg_before_invoke,
                                               msg_after_invoke, fd));
    } else {
        /*Handle response */
        COPS_CHK_RC(cops_router_handle_response(fd, msg_after_invoke));
    }

function_exit:
    return ret_code;
}

cops_return_code_t cops_router_notify_api_proxy(uint32_t primitive)
{
    cops_return_code_t ret_code = COPS_RC_OK;

#ifdef COPS_OSE_ENVIRONMENT
    PROCESS process = NO_PROCESS;

    if (SR_RESULT_OK != Do_SR_Resolve_Pid("COPS_API_PROXY", &process)) {
        COPS_SET_RC(COPS_RC_INTERNAL_ERROR,
                    "Could not resolve PID for COPS_API_PROXY!\n");
    }

    SEND_SIMPLE_PRIMITIVE(primitive, process);

function_exit:
#else
    (void)primitive;
#endif
    return ret_code;
}

static cops_return_code_t cops_router_handle_trigger(struct cops_state *state,
                                                     cops_sipc_message_t *msg,
                                                     int fd)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    size_t offset = 0;
    uint8_t trigger_msg;

    COPS_CHK_RC(cops_tapp_sipc_get_uint8(msg, &offset, &trigger_msg));

    /*
     * Send message towards Modem only if message is triggered on current CPU
     * or if relaying message from APE
     */
    if (NULL != state->ipc_client_ctx &&
        (NO_FD == fd || COPS_RUN_MODE_RELAY == run_mode)) {
        COPS_CHK_RC(cops_sipc_send(state->ipc_client_ctx->data.request_fd,
                                   msg));
    }

    /* Send message towards APE only if message is triggered on current CPU */
    if (COPS_TRIGGER_AUTHENTICATION_EVENT == trigger_msg &&
        NULL != state->ipc_server_ctx && NO_FD == fd) {
        COPS_CHK_RC(cops_sipc_send(state->ipc_server_ctx->data.request_fd,
                                   msg));
    }

function_exit:
    return ret_code;
}

static cops_return_code_t cops_router_handle_ipc_state(struct cops_state *state,
                                                       cops_sipc_message_t *msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *sipc_msg = NULL;
    size_t offset = 0;
    uint32_t primitive = COPS_IPC_NOT_READY_PRIMITIVE;

    /* We have reached the end of the COPS line */
    if (NULL == state->ipc_server_ctx && state->session_key_ready) {
        primitive = COPS_IPC_READY_PRIMITIVE;
    }

    if (COPS_SIPC_IPC_STATE_REQ == msg->msg_type) {
        /* Send message to next COPS process */
        if (NULL != state->ipc_server_ctx &&
            COPS_SOCKET_STATE_READY == state->ipc_server_ctx->data.state) {
            COPS_LOG(LOG_INFO, "SIPC request sent to fd = %u. Type = 0x%x, "
                     "length = %u\n", state->ipc_server_ctx->data.request_fd,
                     msg->msg_type, msg->length);
            COPS_CHK_RC(cops_sipc_send(state->ipc_server_ctx->data.request_fd,
                                       msg));
        } else {
            /* Create IPC state "response" */
            COPS_CHK_RC(cops_sipc_alloc_message(ret_code, sizeof(uint32_t),
                                                COPS_SIPC_IPC_STATE_RESP,
                                                COPS_SENDER_UNSECURE,
                                                &sipc_msg));
            COPS_CHK_RC(cops_tapp_sipc_set_uint32(sipc_msg, &offset,
                                                  primitive));
            COPS_CHK_RC(cops_router_handle_ipc_state(state, sipc_msg));
        }
    } else {
        /* Extract IPC state from message */
        COPS_CHK_RC(cops_tapp_sipc_get_uint32(msg, &offset, &primitive));

        /* Notify API proxy */
        COPS_CHK_RC(cops_router_notify_api_proxy(primitive));

        /* Send message to prevoius COPS process if applicable */
        if (NULL != state->ipc_client_ctx &&
            COPS_SOCKET_STATE_READY == state->ipc_client_ctx->data.state) {
            COPS_LOG(LOG_INFO, "SIPC response sent to fd = %u. Type = 0x%x, "
                     "length = %u\n", state->ipc_client_ctx->data.request_fd,
                     msg->msg_type, msg->length);
            COPS_CHK_RC(cops_sipc_send(state->ipc_client_ctx->data.request_fd,
                                       msg));
        }
    }

function_exit:
    cops_sipc_free_message(&sipc_msg);
    return ret_code;
}

static cops_return_code_t cops_router_handle_request(struct cops_state *state,
                                         cops_sipc_message_t *msg_before_invoke,
                                         cops_sipc_message_t *msg_after_invoke,
                                         int fd)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    int ipc_fd;
    bool dummy_client = false;
    cops_sipc_message_t *msg_to_add = NULL;

    /* See if and what message should be added to client list */
    if (COPS_SIPC_DERIVE_SESSION_KEY_MREQ == msg_before_invoke->msg_type &&
        (COPS_SIPC_GET_LONG_TERM_SHARED_KEY_MREQ ==
                                                   msg_after_invoke->msg_type ||
        COPS_SIPC_GET_SESSION_KEY_MODEM_RAND_MREQ ==
                                                  msg_after_invoke->msg_type)) {
        msg_to_add = msg_before_invoke;
        dummy_client = true;
    } else if (COPS_SIPC_TRIGGER_MSG == msg_before_invoke->msg_type) {
        msg_to_add = msg_after_invoke;
        dummy_client = true;
    } else if (msg_before_invoke->msg_type == msg_after_invoke->msg_type) {
        msg_to_add = msg_after_invoke;
        dummy_client = false;
    }

    /* Add message to client list */
    if (NULL != msg_to_add) {
        COPS_CHK_RC(cops_router_add_client(dummy_client ? NO_FD : fd,
                                           msg_to_add));
    }

    /* The following messages are sent towards Modem, the rest to DCPU/APE */
    if (COPS_SIPC_READ_SIM_DATA_HREQ == msg_after_invoke->msg_type ||
        COPS_SIPC_GET_LONG_TERM_SHARED_KEY_MREQ == msg_after_invoke->msg_type ||
        COPS_SIPC_GET_SESSION_KEY_MODEM_RAND_MREQ ==
                                                   msg_after_invoke->msg_type) {
        ipc_fd = state->ipc_client_ctx->data.request_fd;
    } else {
        ipc_fd = state->ipc_server_ctx->data.request_fd;
    }

    COPS_LOG(LOG_INFO, "SIPC request sent to fd = %u. Type = 0x%x, "
             "length = %u\n", ipc_fd, msg_after_invoke->msg_type,
             msg_after_invoke->length);

    COPS_CHK_RC(cops_sipc_send(ipc_fd, msg_after_invoke));

function_exit:
    return ret_code;
}

static cops_return_code_t cops_router_handle_response(int fd,
                                                      cops_sipc_message_t *msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    struct cops_router_client *router_client, *router_client_next;
    int return_fd = fd;

    /*
     * Check if there is an entry in the router client list that matches.
     * If so return the message to that fd(can be IPC).
     * Otherwise return the message to the fd that the message came from.
     */
    for (router_client = router_client_list.tqh_first;
         router_client != NULL; router_client = router_client_next) {

        router_client_next = router_client->link.tqe_next;

        if (router_client->msg == msg->msg_type) {
            return_fd = router_client->fd;
            break;
        }
    }

    /* Remove client if applicable */
    if (NULL != router_client) {
        COPS_LOG(LOG_INFO, "Removed client from router, fd = %d, "
                 "msg_type = 0x%x\n", router_client->fd, router_client->msg);
        TAILQ_REMOVE(&router_client_list, router_client, link);
        free(router_client);
    }

    /* NO_FD as return_fd means dummy_client */
    if (NO_FD != return_fd) {
        COPS_LOG(LOG_INFO, "SIPC response sent to fd = %u. Type = 0x%x, "
                 "length = %u\n", return_fd, msg->msg_type, msg->length);

        COPS_CHK_RC(cops_sipc_send(return_fd, msg));
    }

function_exit:
    return ret_code;
}

static cops_return_code_t cops_router_add_client(int fd,
                                                 cops_sipc_message_t *msg)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    struct cops_router_client *router_client = NULL;

    COPS_CHK_ASSERTION(COPS_SIPC_IS_REQUEST(msg));

    router_client = calloc(1, sizeof(*router_client));
    COPS_CHK_ALLOC(router_client);

    router_client->msg = msg->msg_type + 1;
    router_client->fd = fd;

    TAILQ_INSERT_TAIL(&router_client_list, router_client, link);

    COPS_LOG(LOG_INFO, "Added client to router, fd = %d, msg_type = 0x%x\n",
             router_client->fd, router_client->msg);
function_exit:
    return ret_code;
}


/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#include <cops_srv.h>
#include <cops_msg_handler.h>
#include <cops_router.h>
#include <cops_common.h>

static cops_return_code_t cops_srv_msg_handle_complete(void *aux,
                                                       cops_sipc_message_t *msg,
                                                       int fd);

static cops_return_code_t cops_srv_msg_handle(void *aux,
                                              cops_sipc_message_t *in_msg,
                                              cops_sipc_message_t **out_msg);

const cops_server_callbacks_t cops_srv_common_cbs_minimal = {
    .msg_handle_complete = cops_srv_msg_handle_complete,
    .msg_handle = cops_srv_msg_handle
};

/* Messages that are received over the API server are handled here */
static cops_return_code_t cops_srv_msg_handle_complete(void *aux,
                                                       cops_sipc_message_t *msg,
                                                       int fd)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *out_msg = NULL;
    struct cops_state *state = aux;

    /* Perform the invoke and other handling of the message */
    ret_code = cops_msg_handle(state, msg, &out_msg);
#ifndef COPS_IN_LOADERS
    if (COPS_RC_OK != ret_code) {
        COPS_CHK(NULL != out_msg, ret_code);
        COPS_LOG(LOG_WARNING, "cops_msg_handle completed with error:0x%x\n",
                 ret_code);
    }
    /* Send the message to the correct receiver */
    COPS_CHK_RC(cops_router_handle_msg(state, fd, msg, out_msg));
    /* Trigger event handling if applicable to current message */
    (void)cops_msg_trigger_event_handling(state, out_msg);
    /* Dispatch event if applicable to current message */
    (void)cops_msg_dispatch_event(state, out_msg);
function_exit:
#endif
    if (out_msg != msg) {
        cops_sipc_free_message(&out_msg);
    }
    return ret_code;
}

static cops_return_code_t cops_srv_msg_handle(void *aux,
                                              cops_sipc_message_t *in_msg,
                                              cops_sipc_message_t **out_msg)
{
    struct cops_state *state = aux;

    return cops_msg_handle(state, in_msg, out_msg);
}

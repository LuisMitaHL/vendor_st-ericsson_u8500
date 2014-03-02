/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#ifndef COPS_MSG_HANDLER_H
#define COPS_MSG_HANDLER_H

#include <cops_state.h>
#include <cops_sipc_message.h>

cops_return_code_t cops_msg_handle(struct cops_state *state,
                                   cops_sipc_message_t *in_msg,
                                   cops_sipc_message_t **out_msg);
#ifndef COPS_IN_LOADERS
cops_return_code_t cops_msg_trigger_event_handling(struct cops_state *state,
                                                   cops_sipc_message_t *msg);

cops_return_code_t cops_msg_dispatch_event(struct cops_state *state,
                                           cops_sipc_message_t *msg);
#endif

#endif

/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#ifndef COPS_ROUTER_H
#define COPS_ROUTER_H

#include <cops_state.h>
#include <cops_sipc_message.h>

#define NO_FD -1

void cops_router_init(void);

cops_return_code_t cops_router_notify_api_proxy(uint32_t primitive);

cops_return_code_t cops_router_handle_msg(struct cops_state *state, int fd,
                                         cops_sipc_message_t *msg_before_invoke,
                                         cops_sipc_message_t *msg_after_invoke);

#endif /* COPS_ROUTER_H */

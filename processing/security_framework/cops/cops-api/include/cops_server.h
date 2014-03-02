/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_SERVER_H
#define COPS_SERVER_H
#include <cops.h>
#include <cops_sipc_message.h>

#ifndef COPS_IN_LOADERS
#include <sys/select.h>
#endif
/**
 * @brief Represents a communication channel with a COPS client
 *
 * All communication with a COPS client is done through the COPS
 * server context.
 */
typedef struct cops_server_context_id cops_server_context_id_t;

typedef struct cops_server_callbacks {

    cops_return_code_t (*msg_handle_complete)(void *aux,
                                              cops_sipc_message_t *msg,
                                              int fd);

    cops_return_code_t (*msg_handle)(void *aux, cops_sipc_message_t *in_msg,
                                     cops_sipc_message_t **out_msg);

} cops_server_callbacks_t;

cops_return_code_t cops_server_context_create(cops_server_context_id_t **ctxpp,
                                             const cops_server_callbacks_t *cbs,
                                             void *aux);

void  cops_server_context_destroy(cops_server_context_id_t **ctxpp);
#ifndef COPS_IN_LOADERS
void  cops_server_context_get_fdset(cops_server_context_id_t *ctxp,
                                    int *nfds, fd_set *readfds);

void  cops_server_context_handle_requests(cops_server_context_id_t *ctxp,
                                          int nfds, fd_set *readfds);
#endif

#endif                          /*COPS_SERVER_H */

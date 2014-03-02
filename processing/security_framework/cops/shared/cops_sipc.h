/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_SIPC_H
#define COPS_SIPC_H
/* Due to OPA file name collision we need this */
#ifdef COPS_USE_AUTO_GEN_INCLUDE
#include <cops_types_auto_gen.h>
#else
#include <cops_types.h>
#endif

#include <cops_sipc_message.h>
#include <sys/types.h>

ssize_t cops_sipc_recv(int fd, cops_sipc_message_t **msg);

cops_return_code_t cops_sipc_send(int fd, cops_sipc_message_t *msg);

#endif /* COPS_SIPC_H */

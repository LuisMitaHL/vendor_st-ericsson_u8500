/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#ifndef COPS_API_PROXY_H
#define COPS_API_PROXY_H

#include <t_os.h>
#include <cops_api_internal.h>

void cops_api_proxy_startup(cops_context_id_t **context);

int cops_api_proxy_handle_signal(union SIGNAL *signal,
                                 cops_context_id_t *context);

#endif /*COPS_API_PROXY_H*/

/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_TEST_H
#define COPS_TEST_H
#include <cops.h>

extern const char *cops_api_socket_path;        /* socket path to connect to */

/*
 * *out should be freed with free() if != NULL when cops_sipc_mx()
 * has returned.
 */
cops_return_code_t cops_modem_sipc_mx(cops_context_id_t *ctxp,
                                      const uint8_t *in, size_t inlen,
                                      uint8_t **out, size_t *outlen);

#endif                          /*COPS_TEST_H */

/*************************************************************************
 * Copyright ST-Ericsson 2012
 ************************************************************************/
#ifndef COPS_STATE_H
#define COPS_STATE_H

#include <cops_shared.h>
#include <cops_data.h>
#include <cops_server.h>

struct cops_state {
    cops_tapp_io_t *tapp_io;
    cops_data_t data;
    cops_data_t perm_auth_state_data;
#ifndef COPS_IN_LOADERS
    cops_server_context_id_t *server_ctx;
    struct cops_ipc_context *ipc_client_ctx;
    struct cops_ipc_context *ipc_server_ctx;
    bool session_key_ready;
#endif
#ifndef COPS_TAPP_EMUL
#ifndef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
    TEEC_Session teec_sess;
#endif
#endif
};

#endif /* COPS_STATE_H */

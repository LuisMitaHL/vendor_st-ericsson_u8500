/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/

#ifndef COPS_IPC_COMMON_H
#define COPS_IPC_COMMON_H

#include <cops_state.h>
#include <cops_socket.h>

/**
 * PSock defines a client server model where the Server always resides
 * on the modem side (and client on the APE side in bridge case) or on the
 * data CPU (COPS_Relay).
 *
 * In the configuraiton of U9540 (or U9500) the setup looks as following:
 * (S) means server and (C) means client.
 *
 * COPS_Modem                COPS_RELAY                COPS_Daemon
 * ----------                ----------                -----------
 * PSock server: cops_modem  PSock server: cops_relay  PSock server: n/a
 * PSock client: n/a         PSock client: cops_modem  PSock client: cops_relay
 *
 *  +----------------+       +----------------+
 *  | cops_modem (S) | <---> | cops_modem (C) |
 *  +----------------+       +----------------+
 *                           +----------------+        +----------------+
 *                           | cops_relay (S) | <----> | cops_relay (C) |
 *                           +----------------+        +----------------+
 */

struct cops_ipc_context;

struct cops_ipc_internal_callbacks {
    void (*destroy)(struct cops_ipc_context *ctx);
    void (*get_fdset)(struct cops_ipc_context *ctx, int *nfds, fd_set *readfds);
    void (*handle_signals)(struct cops_ipc_context *ctx,
                           int nfds, fd_set *readfds);
};

struct cops_ipc_context {
    bool server;
    struct cops_socket_data data;
    void *internal_ctx;
    struct cops_ipc_internal_callbacks internal_cbs;
};

cops_return_code_t cops_ipc_context_create(struct cops_ipc_context **ctx,
                                           bool server, void *aux);
cops_return_code_t cops_ipc_context_start(struct cops_ipc_context *ctx);
void cops_ipc_context_destroy(struct cops_ipc_context **ctxpp);
void cops_ipc_context_get_fdset(struct cops_ipc_context *ctxp,
                                int *nfds, fd_set *readfds);
void cops_ipc_context_handle_signals(struct cops_ipc_context *ctxp,
                                     struct cops_state *state, int nfds,
                                     fd_set *readfds);

cops_return_code_t cops_ipc_open(struct cops_ipc_context *ctx, int *fd);
cops_return_code_t cops_ipc_start(struct cops_ipc_context *ctx);

#endif /* COPS_IPC_COMMON_H */

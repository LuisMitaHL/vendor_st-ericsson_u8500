/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_API_INTERNAL_H
#define COPS_API_INTERNAL_H

#include <queue.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <cops_test.h>
#include <cops_server.h>
#include <cops_socket.h>
#include <cops_sipc_message.h>

/*
 * The following macro is used to remove const cast-away warnings
 * from gcc -Wcast-qual; it should be used with caution because it
 * can hide valid errors; in particular most valid uses are in
 * situations where the API requires it, not to cast away string
 * constants.
 */
#define COPS_API_UNCONST(a) ((void *)(intptr_t)(const void *)(a))

#ifndef SIZE_MAX
#define SIZE_MAX 10000000
#endif

#define XSTR(x) #x
#define STR(x) XSTR(x)

#define TIMEOUT_SEC 5
#define TIMEOUT_USEC 0

#define SIGNED_HEADER_MAGIC 0x53484452

struct cops_context_id {
    int       request_fd;
    int       async_fd;
    cops_event_callbacks_t event_cbs;
    void     *event_aux;
    cops_async_callbacks_t async_cbs;
    void     *async_aux;
};

typedef enum cops_api_event {
    COPS_API_EVENT_SIMLOCK_STATUS_CHANGED = 0,
    COPS_API_EVENT_OTA_UNLOCK_STATUS_UPDATED,
    COPS_API_EVENT_AUTHENTICATION_CHANGED,
    /*... */
    COPS_API_EVENTS_INVALID = 0xFF
} cops_api_event_t;

typedef struct cops_client {
    int       request_fd;
    bool      request_inited;
    int       async_fd;
    cops_api_event_t *events;
    bool      inited;
    TAILQ_ENTRY(cops_client) link;
} cops_client_t;

typedef TAILQ_HEAD(cops_client_head, cops_client)
cops_client_head_t;

struct cops_server_context_id {
    struct cops_socket_data data;
    cops_server_callbacks_t cbs;
    cops_client_head_t clients;
    cops_sim_status_t *saved_simcard_status;
};

int cops_api_connect_request(cops_context_id_t *ctx, uint32_t *client_id);

int cops_api_connect_async(cops_context_id_t *ctx, uint32_t client_id);

int cops_api_listen(cops_server_context_id_t *ctx, const char *path);

int cops_api_accept(cops_server_context_id_t *ctx);

bool cops_api_find_client(cops_server_context_id_t *ctx, int fd,
                          cops_client_t **client);

/* Note the client pointer may not be valid after this call */
void cops_api_init_client_connection(cops_server_context_id_t *ctx, int fd,
                                     cops_client_t *client);

void cops_api_close_client(cops_server_context_id_t *ctx,
                           cops_client_t *client);

cops_return_code_t cops_api_block_until_ready(int fd);

void cops_server_send_event(cops_server_context_id_t *ctxp,
                            cops_sipc_message_t *msg);

#endif                          /*COPS_API_INTERNAL_H */

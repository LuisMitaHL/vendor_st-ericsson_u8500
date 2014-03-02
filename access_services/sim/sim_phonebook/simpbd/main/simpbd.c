/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Implementation of request handlers in simpbd
 *
 * Author:  Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 *          Dag Gullberg <dag.xd.gullberg@stericsson.com>
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "sockserv.h"

#include "simpbd_p.h"
#include "simpbd.h"
#include "simpbd_record.h"
#include "simpbd_socket.h"
#include "util_security.h"

// -----------------------------------------------------------------------------
// Server data
static int socket_server = -1;

static int max_clients;
static int clients_used;
static simpbd_client_t **clients;

// -----------------------------------------------------------------------------
// Local function prototypes
static int simpbd_socket_callback(const int instance, const int client, const void *context_p,
                                  const sockserv_reason_t reason, const uint8_t *buf_p, const size_t size);

static void simpbd_handle_request(simpb_message_t *msg_p, simpbd_client_t *client_p);
static int simpbd_alloc_client(void);
static simpbd_client_t *simpbd_get_client(int client_id);
static int simpbd_free_client(int client_id);


// -----------------------------------------------------------------------------

int simpbd_get_num_servers()
{
    SIMPB_LOG_D("entered");
    return SIMPBD_NUM_SOCKET_SERVERS;
}


// -----------------------------------------------------------------------------

int simpbd_init()
{
    SIMPB_LOG_D("entered");
    /* Allocate and initialise client array */
    clients_used = 0;
    max_clients = SIMPBD_SOCKET_MAX_CLIENTS;
    clients = calloc(max_clients, sizeof(simpbd_client_t *));

    if (!clients) {
        SIMPB_LOG_E("Unable to allocate %d client instances!", max_clients);
        goto error_exit;
    }

    /* Initialise SIMPB daemon */
    simpbd_record_init();
    /*
        simpbd_p = ;

        if (!simpbd_p) {
            SIMPB_LOG_E("Error on SIMPB daemon initialisation!");
            goto error_exit;
        }
    */
    /* Create a request/response socket server instance */
    socket_server = sockserv_create(simpbd_socket_name,
                                    SIMPBD_SOCKET_MAX_CLIENTS,
                                    &simpbd_socket_callback);

    if (socket_server < 0) {
        SIMPB_LOG_E("Unable to create a socket server instance!");
        goto error_exit;
    }

    util_continue_as_non_privileged_user();

    return 0;

error_exit:
    simpbd_shutdown();

    return -1;
}


// -----------------------------------------------------------------------------

int simpbd_socket_callback(const int ss_instance, const int ss_client, const void *context_p,
                           const sockserv_reason_t reason, const uint8_t *buf_p, const size_t buf_size)
{
    int result;
    SIMPB_LOG_D("reason=%d, instance=%d, client=%d, context_p=%p",
                reason, ss_instance, ss_client, context_p);

    switch (reason) {
    case SOCKSERV_REASON_CONNECT_REQUEST:
        /* Allow connection only if there is room for it */
        result = (clients_used < max_clients) ? 0 : -1;
        break;

    case SOCKSERV_REASON_CONNECTED:
        /* Allocate and initialise client context */
    {
        int client_id = simpbd_alloc_client();
        simpbd_client_t *client_p = simpbd_get_client(client_id);

        if (client_p) {
            client_p->type = SIMPBD_CLIENT_ALL;
            client_p->instance = ss_instance;
            client_p->client = ss_client;
            sockserv_set_context(ss_instance, ss_client, (void *)client_id);
            result = 0;
        } else {
            result = -1;
        }
    }
    break;

    case SOCKSERV_REASON_RECEIVE: {
        int client_id = (int)context_p;
        simpbd_client_t *client_p = simpbd_get_client(client_id);

        if (client_p) {
            size_t msg_size;
            size_t decoded = 0;
            simpb_message_t *msg_p;

            /* Need at least the header to know if the message is complete */
            while (buf_p && SIMPB_MESSAGE_HEADER_SIZE <= (buf_size - decoded)) {

                msg_p = (simpb_message_t *)(buf_p + decoded);
                msg_size = SIMPB_MESSAGE_HEADER_SIZE + msg_p->payload_size;

                /* Check that we have all of the message before processing it further */
                if (msg_size <= (buf_size - decoded)) {
                    simpbd_handle_request(msg_p, client_p);
                    decoded += msg_size;
                } else {
                    /* Not a complete message, must wait for the rest */
                    break;
                }
            }

            /* Return number of processed bytes */
            result = (int)decoded;

        } else {
            SIMPB_LOG_E("message from unknown! instance=%d, client=%d, context_p=%p",
                        ss_instance, ss_client, context_p);
            result = -1;  /* Unknown client !? */
        }
    }
    break;

    case SOCKSERV_REASON_DISCONNECTED: {
        int client_id = (int)context_p;
        result = simpbd_free_client(client_id);
    }
    break;

    default:
        SIMPB_LOG_E("Unhandled reason=%d, instance=%d, client=%d, context_p=%p!",
                    reason, ss_instance, ss_client, context_p);
        result = -1;
        break;
    }

    return result;
}


// -----------------------------------------------------------------------------

void simpbd_handle_request(simpb_message_t *msg_p, simpbd_client_t *client_p)
{
    ste_simpb_result_t result;
    SIMPB_LOG_D("entered");

    result = simpbd_execute(client_p, msg_p->request_id, msg_p->payload, (void *)msg_p->client_tag);

    return;
}


// -----------------------------------------------------------------------------

int simpbd_handle_event(ste_simpb_cause_t cause, void *data_p, size_t data_size)
{
    simpb_message_t *msg_p = NULL;
    SIMPB_LOG_D("entered");

    if (data_size > 0) {
        if (!data_p) {
            SIMPB_LOG_E("Data pointer NULL while size > 0!");
            goto error;
        }
    } else {
        if (data_p) {
            SIMPB_LOG_E("Data pointer buffer not NULL while size = 0!");
            goto error;
        }
    }

    /* Allocate and assemble message */
    msg_p = (simpb_message_t *)calloc(1, data_size + SIMPB_MESSAGE_HEADER_SIZE);

    if (!msg_p) {
        SIMPB_LOG_E("Unable to allocate memory for message!");
        goto error;
    }

    msg_p->cause        = cause;
    msg_p->client_tag   = 0;
    msg_p->request_id   = 0;
    msg_p->result       = STE_SIMPB_SUCCESS;
    msg_p->payload_size = data_size;

    if (data_size > 0) {
        memcpy(msg_p->payload, data_p, data_size);
    }

    simpbd_send_event(msg_p, data_size + SIMPB_MESSAGE_HEADER_SIZE);

    free(msg_p);

    return 0;

error:
    return -1;
}


// -----------------------------------------------------------------------------

int simpbd_send_event(const simpb_message_t *msg_p, const size_t size)
{
    simpbd_client_t *client_p;
    int events_sent;
    int client_id;
    SIMPB_LOG_D("entered");

    /* Parameter check */
    if (!msg_p) {
        goto error_return;
    }

    if (size < 1) {
        goto error_return;
    }

    /* Find event clients */
    events_sent = 0;

    for (client_id = 0; client_id < max_clients; client_id++) {
        client_p = simpbd_get_client(client_id);

        if (client_p && (client_p->type & SIMPBD_CLIENT_EVENT)) {
            if (sockserv_send(client_p->instance, client_p->client, (void *)msg_p, size) >= 0) {
                events_sent++;
            }
        }
    }

    return events_sent;

error_return:
    return -1;
}


// -----------------------------------------------------------------------------

int simpbd_handle_response(simpbd_client_t *client_p, void *client_tag_p, simpbd_request_id_t request_id, ste_simpb_result_t result, void *data_p, size_t data_size)
{
    simpb_message_t *msg_p = NULL;
    int res;
    SIMPB_LOG_D("entered");

    if (data_size > 0) {
        if (!data_p) {
            SIMPB_LOG_E("Data pointer NULL while size > 0!");
            goto error;
        }
    } else {
        if (data_p) {
            SIMPB_LOG_E("Data pointer buffer not NULL while size = 0!");
            goto error;
        }
    }

    /* Allocate and assemble message */
    msg_p = (simpb_message_t *)calloc(1, data_size + SIMPB_MESSAGE_HEADER_SIZE);

    if (!msg_p) {
        SIMPB_LOG_E("Unable to allocate memory for message!");
        goto error;
    }

    msg_p->cause        = STE_SIMPB_CAUSE_REQUEST_RESPONSE;
    msg_p->client_tag   = (uintptr_t)client_tag_p;
    msg_p->request_id   = request_id;
    msg_p->result       = result;
    msg_p->payload_size = data_size;

    if (data_size > 0) {
        memcpy(msg_p->payload, data_p, data_size);
    }

    res = simpbd_send_response(client_p, msg_p, data_size + SIMPB_MESSAGE_HEADER_SIZE);

    if (0 > res) {
        SIMPB_LOG_E("Failed sending response!");
    }

    free(msg_p);

    return 0;

error:
    return -1;
}


// -----------------------------------------------------------------------------

int simpbd_send_response(const simpbd_client_t *client_p, const simpb_message_t *msg_p, const size_t size)
{
    SIMPB_LOG_D("entered");

    /* Parameter check */
    if (!client_p) {
        SIMPB_LOG_E("client_p is NULL!");
        goto error_return;
    }

    if (!(client_p->type & SIMPBD_CLIENT_REQUEST)) {
        SIMPB_LOG_E("client_p->type (%d) & SIMPBD_CLIENT_REQUEST!", client_p->type);
        goto error_return;
    }

    if (!msg_p) {
        SIMPB_LOG_E("msg_p is NULL!");
        goto error_return;
    }

    if (1 > size) {
        SIMPB_LOG_E("size is zero or less!");
        goto error_return;
    }

    return(sockserv_send(client_p->instance, client_p->client, (void *)msg_p, size));

error_return:
    return -1;
}


// -----------------------------------------------------------------------------

int simpbd_close_client(const int client_id)
{
    simpbd_client_t *client_p = simpbd_get_client(client_id);
    int result;
    SIMPB_LOG_D("client_id=%d", client_id);

    if (client_p) {
        result = sockserv_close(client_p->instance, client_p->client);
    } else {
        result = -1;
    }

    return result;
}


// -----------------------------------------------------------------------------

int simpbd_shutdown()
{
    int client_id;

    SIMPB_LOG_D("entered");

    /* Shut down socket server instance */
    if (socket_server >= 0) {

        for (client_id = 0; client_id < max_clients; client_id++) {
            simpbd_close_client(client_id);
        }

        /* Destroy a socket server instance */
        sockserv_destroy(socket_server);
        socket_server = -1;
    }

    /* Close SIMPBD daemon */
    /* Free all requests */
    simpbd_record_free_all();

    /* Release client array */
    if (clients_used == 0) {
        free(clients);
    }

    return 0;
}


/* Client utility functions */
int simpbd_alloc_client()
{
    simpbd_client_t *client_p;
    int client_id;

    /* Find first available slot */
    for (client_id = 0; client_id < max_clients; client_id++) {
        if (!clients[client_id]) {
            client_p = calloc(1, sizeof(simpbd_client_t));

            if (client_p) {
                clients[client_id] = client_p;
                clients_used++;
                SIMPB_LOG_D("client_id=%d", client_id);
                return client_id;
            } else {
                break;
            }
        }
    }

    SIMPB_LOG_E("Cannot allocate client!");
    return -1;
}


simpbd_client_t *simpbd_get_client(int client_id)
{
    if (client_id >= 0 && client_id < max_clients) {
        return clients[client_id];
    }

    return (simpbd_client_t *)NULL;
}


int simpbd_free_client(int client_id)
{
    simpbd_client_t *client_p;
    SIMPB_LOG_D("client_id=%d", client_id);

    if (client_id < 0 || client_id >= max_clients) {
        return -1;
    }

    client_p = clients[client_id];

    if (client_p) {
        clients[client_id] = NULL;
        clients_used--;
        free(client_p);
        return 0;
    }

    return -1;
}

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Functionality for serving Call and Network service
 * services to connecting clients.
 */

#include <stddef.h>
#include <stdlib.h>

#include "cn_log.h"
#include "cn_macros.h"
#include "cn_client.h"
#include "cn_utils.h"
#include "sockserv.h"
#include "cnserver.h"
#include "str_convert.h"

#include "cn_internal_handlers.h"
#include "message_handler.h"
#include "modem_client.h"
#include "sim_client.h"
#include "plmn_main.h"

#include "cn_battery_internal.h"
#include "cn_event_trigger_level.h"

#include "util_security.h"

/* Local data */
static int request_server = -1;
static int event_server   = -1;

typedef enum {
    CNSERVER_UNKNOWN,
    CNSERVER_REQUEST,
    CNSERVER_EVENT,
} cnserver_client_type_t;

typedef struct {
    cnserver_client_type_t type;        /* Client type */
    int                   instance;
    int                   client;
} cnserver_client_t;

static int max_clients  = -1;
static int clients_used = -1;
static cnserver_client_t **clients = NULL; /* array of pointers */
static modem_client_t *modem_client_p = NULL;

static cnserver_client_disconnected_cb_t cnserver_client_disconnected_cb = NULL;

/* Local function prototypes */
static int cnserver_callback(const int instance, const int client, const void *context_p,
                             const sockserv_reason_t reason, const cn_uint8_t *buf_p, const size_t size);

static int cnserver_alloc_client(void);
static cnserver_client_t *cnserver_get_client(int client_id);
static int cnserver_free_client(int client_id);

void cnserver_register_client_disconnected_cb(cnserver_client_disconnected_cb_t cb)
{
    cnserver_client_disconnected_cb = cb;
}

int cnserver_get_num_servers()
{
    return CN_NUM_SOCKET_SERVERS;
}

int cnserver_init()
{
    /* Allocate and initialise client array */
    clients_used = 0;
    max_clients = CN_REQUEST_SOCKET_MAX_CLIENTS + CN_EVENT_SOCKET_MAX_CLIENTS;
    clients = calloc(max_clients, sizeof(cnserver_client_t *));

    if (!clients) {
        CN_LOG_E("Unable to allocate %d client instances!", max_clients);
        goto error_exit;
    }

    cn_log_init();

    /* Initialize modem library */
    modem_client_p = modem_client_open_session();

    if (!modem_client_p) {
        CN_LOG_E("Error on open modem session!");
        goto error_exit;
    }

    if (!cn_battery_client_open_session()) {
        CN_LOG_E("Unable to create a battery instance!");
        goto error_exit;
    }

    /* Initialize string converter */
    if (!str_convert_init()) {
        CN_LOG_E("str_convert_init failed!");
        goto error_exit;
    }

    /* Initialize PLMN operator list handler */
    if (plmn_init() < 0) {
        CN_LOG_E("plmn_init failed!");
        goto error_exit;
    }

    /* Initialize SIM client */
    if (!sim_client_init()) {
        CN_LOG_E("sim_client_init failed!");
        goto error_exit;
    }

    /* Initialize client disconnect callback and ETL table */
    cnserver_register_client_disconnected_cb(etl_client_disconnected_cb);
    etl_init();

    /* Create a request/response socket server instance */
    request_server = sockserv_create(cn_request_socket_name,
                                     CN_REQUEST_SOCKET_MAX_CLIENTS, &cnserver_callback);

    if (request_server < 0) {
        CN_LOG_E("Unable to create a request/response socket server instance!");
        goto error_exit;
    }

    /* Create an event socket server instance */
    event_server = sockserv_create(cn_event_socket_name,
                                   CN_EVENT_SOCKET_MAX_CLIENTS, &cnserver_callback);

    if (event_server < 0) {
        CN_LOG_E("Unable to create an event socket server instance!");
        goto error_exit;
    }

    /* Write default non-volatile modem data
     *
     * This functionality is handled here since CNS controls the configuration
     * of the modem.
     *
     * This will among other things include configuration of the following:
     *   - Setting preferred SMS route to CS domain (as opposed to PS domain).
     */
    #ifndef ENABLE_MODULE_TEST
    set_default_non_volatile_modem_data();
    #endif /* ENABLE_MODULE_TEST */

    /* The current process privilege is only required for server initialization.
     * Throw away privilege and become a normal user (but the current group
     * privilege stays as it is).
     */
    util_continue_as_non_privileged_user();

    return 0;

error_exit:
    cnserver_shutdown();

    return -1;
}


int cnserver_callback(const int ss_instance, const int ss_client, const void *context_p,
                      const sockserv_reason_t reason, const cn_uint8_t *buf_p, const size_t buf_size)
{
    int result;

    switch (reason) {
    case SOCKSERV_REASON_CONNECT_REQUEST:
        /* Allow connection only if there is room for it */
        result = (clients_used < max_clients) ? 0 : -1;
        break;

    case SOCKSERV_REASON_CONNECTED:
        /* Allocate and initialise client context */
    {
        int client_id = cnserver_alloc_client();
        cnserver_client_t *client_p = cnserver_get_client(client_id);

        if (client_p) {
            if (ss_instance == request_server) {
                client_p->type = CNSERVER_REQUEST;
            } else if (ss_instance == event_server) {
                client_p->type = CNSERVER_EVENT;
            } else {
                client_p->type = CNSERVER_UNKNOWN;
                CN_LOG_E("connection by unknown! instance=%d, client=%d, context_p=%p",
                         ss_instance, ss_client, context_p);
            }

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
        cnserver_client_t *client_p = cnserver_get_client(client_id);

        if (client_p) {
            size_t msg_size;
            size_t decoded = 0;
            cn_message_t *msg_p;

            /* Need at least the header to know if the message is complete */
            while (buf_p && CN_MESSAGE_HEADER_SIZE <= (buf_size - decoded)) {

                msg_p = (cn_message_t *)(buf_p + decoded);
                msg_size = CN_MESSAGE_HEADER_SIZE + msg_p->payload_size;

                /* Check that we have all of the message before processing it further */
                if (msg_size <= (buf_size - decoded)) {

                    handle_request(msg_p, client_id);

                    decoded += msg_size;
                } else {
                    /* Not a complete message, must wait for the rest */
                    break;
                }
            }

            /* Return number of processed bytes */
            result = (int)decoded;

        } else {
            CN_LOG_E("message from unknown! instance=%d, client=%d, context_p=%p",
                     ss_instance, ss_client, context_p);
            result = -1;  /* Unknown client !? */
        }
    }
    break;

    case SOCKSERV_REASON_DISCONNECTED: {
        int client_id = (int)context_p;
        result = cnserver_free_client(client_id);

        if (cnserver_client_disconnected_cb) {
            cnserver_client_disconnected_cb(client_id);
        }
    }
    break;

    default:
        CN_LOG_E("Unhandled reason=%d, instance=%d, client=%d, context_p=%p!",
                 reason, ss_instance, ss_client, context_p);
        result = -1;
        break;
    }

    return result;
}


int cnserver_send_event(const cn_message_t *msg_p, const size_t size)
{
    cnserver_client_t *client_p;
    int events_sent;
    int client_id;

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
        client_p = cnserver_get_client(client_id);

        if (client_p && client_p->type == CNSERVER_EVENT) {
            if (sockserv_send(client_p->instance, client_p->client, (void *)msg_p, size) >= 0) {
                events_sent++;
            }
        }
    }

    return events_sent;

error_return:
    return -1;
}


int cnserver_send_response(const int client_id, const cn_message_t *msg_p, const size_t size)
{
    cnserver_client_t *client_p = cnserver_get_client(client_id);

    /* Parameter check */
    if (!client_p) {
        goto error_return;
    }

    if (client_p->type != CNSERVER_REQUEST) {
        goto error_return;
    }

    if (!msg_p) {
        goto error_return;
    }

    if (size < 1) {
        goto error_return;
    }

    return(sockserv_send(client_p->instance, client_p->client, (void *)msg_p, size));

error_return:
    return -1;
}


int cnserver_close_client(const int client_id)
{
    cnserver_client_t *client_p = cnserver_get_client(client_id);
    int result;


    if (client_p) {
        result = sockserv_close(client_p->instance, client_p->client);
    } else {
        result = -1;
    }

    return result;
}


int cnserver_shutdown()
{
    /* Shut down request/response socket server instance */
    if (request_server >= 0) {
        /* TODO: Loop through and close all open sockets */
        /* cnsocket_close_client(context); */
        /* Destroy a socket server instance */
        sockserv_destroy(request_server);
        request_server = -1;
    }

    /* Shut down event socket server instance */
    if (event_server >= 0) {
        /* TODO: Loop through and close all open sockets */
        /* cnsocket_close_client(context); */
        /* Destroy a socket server instance */
        sockserv_destroy(event_server);
        event_server = -1;
    }

    /* Shutdown PLMN operator list handler */
    if (plmn_shutdown() < 0) {
        CN_LOG_E("plmn_shutdown failed!");
    }

    /* Shutdown SIM client */
    if (!sim_client_shutdown()) {
        CN_LOG_E("sim_client_shutdown failed!");
    }

    /* Shutdown string converter */
    str_convert_shutdown();

    cn_battery_client_close_session();

    /* Close session with modem */
    modem_client_close_session(modem_client_p);
    modem_client_p = NULL;

    cn_util_clear_latest_network_search();
    cn_log_close();

    /* Free all requests */
    request_record_free_all();

    /* Release client array */
    if (clients_used == 0) {
        free(clients);
    }

    return 0;
}


/* Client utility functions */
int cnserver_alloc_client()
{
    cnserver_client_t *client_p;
    int client_id;
    int result = -1;

    /* Find first available slot */
    for (client_id = 0; client_id < max_clients; client_id++) {
        if (!clients[client_id]) {
            client_p = calloc(1, sizeof(cnserver_client_t));

            if (client_p) {
                clients[client_id] = client_p;
                clients_used++;
                result = client_id;
                break;
            } else {
                break;
            }
        }
    }

    return result;
}


cnserver_client_t *cnserver_get_client(int client_id)
{
    cnserver_client_t *result = NULL;

    if (client_id >= 0 && client_id < max_clients) {
        result = clients[client_id];
    }

    return result;
}


int cnserver_free_client(int client_id)
{
    cnserver_client_t *client_p;
    int result = -1;


    if (client_id >= 0 && client_id < max_clients) {

        client_p = clients[client_id];

        if (client_p) {
            clients[client_id] = NULL;
            clients_used--;
            free(client_p);
            result = 0;
        }
    }

    return result;
}


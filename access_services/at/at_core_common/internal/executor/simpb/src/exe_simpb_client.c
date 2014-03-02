/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "atc_log.h"
#include "atc_selector.h"
#include "exe_simpb_client.h"
#include "exe_internal.h"
#include "exe_start.h"

#include "simpb.h"

struct simpb_client_s {
    int fd_simpb;
    ste_simpb_t *ste_simpb_p;
    ste_simpb_status_t ste_simpb_status;
    ste_simpb_sim_state_t sim_state;
};

simpb_file_ids_t simpb_adn_files;


/********************************************************************
 * Private methods
 ********************************************************************/
/* CALLBACKS */
/*
 * Handle response for simpb
 */
static void simpb_client_handle_response(ste_simpb_cause_t cause, simpbd_request_id_t request_id, void *data_p, ste_simpb_result_t error_code, uintptr_t client_tag)
{
    exe_request_record_t *request_record_p;
    exe_request_t request;

    request_record_p = request_record_get_record_from_client_tag((int)client_tag);

    if (NULL != request_record_p) {
        request = request_record_get_request(request_record_p);

        if (NULL != request) {
            request_record_set_response_data(request_record_p, (void *)data_p);
            request_record_set_resultcode(request_record_p, (int)error_code);
            request(request_record_p);
        } else {
            ATC_LOG_I("Client tag data corrupt. Unknown response. Consuming it.");
            /* Unsupported and unexpected signal; consume it. */
        }
    } else {
        ATC_LOG_I("Client tag data corrupt. Unknown response. Consuming it.");
        /* Unsupported and unexpected signal; consume it. */
    }
}


/*
 * Handle response to start-up request
 */
static void simpb_client_startup_response_handler(simpb_client_t           *simpb_client_p,
        ste_simpb_result_t        result,
        ste_simpb_cb_sim_state_t *sim_state_p)
{

    /* Update the SIMPB service status */
    if (STE_SIMPB_SUCCESS == result) {
        exe_start_set_status(EXE_START_SIMPB, EXE_START_CONNECTED_READY);

        if (STE_SIMPB_SIM_STATE_READY == sim_state_p->value) {
            /* The SIM daemon is ready to serve client request */
            result = ste_simpb_cache_pbr(simpb_client_p->ste_simpb_p, 0);

            if (STE_SIMPB_SUCCESS != result) {
                ATC_LOG_E("ste_simpb_cache_pbr, result %d", result);
                goto error;
            }
        }
    } else {
        simpb_client_close_session(simpb_client_p);
    }

error:
    return;
}


/*
 * Handle response for selector as response from action from simpb
 */
static bool simpb_client_selector_callback(int fd, void *data_p)
{
    int status = 0;
    simpb_client_t *simpb_client_p = (simpb_client_t *)data_p;

    if (NULL != data_p) {
        status = ste_simbp_receive(simpb_client_p->ste_simpb_p);

        if (0 != status) { /* Read of data from simpb failed.  */
            ATC_LOG_E("********************************************************");
            ATC_LOG_E("***** at_core lost contact with simpb daemon       *****");
            ATC_LOG_E("********************************************************");
            simpb_client_close_session(simpb_client_p);
        }
    } else {
        ATC_LOG_E("ERROR: Selector callback without client data!");
    }

    return true; /* Returning false will remove the fd from the selector. */
}


/*
 * Handle event/response after libsimpb read out data from simpbd.
 * This is the call-back conforming to the CBIF of simpb.
 */
void simpb_client_callback(ste_simpb_cause_t    cause,
                           uintptr_t            client_tag,
                           simpbd_request_id_t  request_id,
                           void                *data_p,
                           ste_simpb_result_t   result,
                           void                *client_context)
{
    simpb_client_t *simpb_client_p = (simpb_client_t *)client_context;

    if (NULL == simpb_client_p) {
        ATC_LOG_E(" client_context is NULL!");
        goto error;
    }

    if (0 == client_tag) {
        switch (cause) {
        case STE_SIMPB_CAUSE_UNSOLICITED_STATUS: {
            ste_simpb_cb_status_data_t *status_p = (ste_simpb_cb_status_data_t *)data_p;

            if (NULL != status_p) {
                simpb_client_p->ste_simpb_status = status_p->status;
            } else {
                ATC_LOG_E("STE_SIMPB_CAUSE_UNSOLICITED_STATUS, data_p == NULL!");
            }

            break;
        }

        case STE_SIMPB_CAUSE_UNSOLICITED_SIM_STATE: {
            ste_simpb_cb_sim_state_t *sim_state_p = (ste_simpb_cb_sim_state_t *)data_p;

            if (NULL != sim_state_p) {
                simpb_client_p->sim_state = sim_state_p->value;

                if (STE_SIMPB_SIM_STATE_READY == sim_state_p->value) {
                    ATC_LOG_I("STE_SIMPB_SIM_STATE_READY received from SIMPB.");
                    simpb_client_startup_response_handler(simpb_client_p, result, (ste_simpb_cb_sim_state_t *)data_p);
                }
            } else {
                ATC_LOG_E("STE_SIMPB_CAUSE_UNSOLICITED_SIM_STATE, data_p == NULL!");
            }

            break;
        }

        case STE_SIMPB_CAUSE_UNSOLICITED_SIMPB_FILE_IDS: {
            if (NULL != data_p) {
                memcpy(&simpb_adn_files, data_p, sizeof(simpb_file_ids_t));
                ATC_LOG_I("SIMPB ADN file info received");
                ATC_LOG_I("No OF ADN files = %d", simpb_adn_files.no_of_pb_files);
                ATC_LOG_I("ADN1 = %d, ADN2 = %d", simpb_adn_files.pb_file_ids[0], simpb_adn_files.pb_file_ids[1]);

            } else {
                ATC_LOG_E("STE_SIMPB_CAUSE_UNSOLICITED_SIMPB_FILE_IDS, data_p == NULL!");
            }

            break;

        }

        case STE_SIMPB_CAUSE_REQUEST_RESPONSE:

            if (STE_SIMPB_REQUEST_ID_STARTUP == request_id) {
                simpb_client_startup_response_handler(simpb_client_p, result, (ste_simpb_cb_sim_state_t *)data_p);
            }

            break;

        default:
            ATC_LOG_I("Received unsupported event = %d", cause);
            break;
        }
    } else {
        /* Ordinary signal with valid client tag */
        simpb_client_handle_response(cause, request_id, data_p, result, client_tag);
    }

error:
    return;
}


/* PUBLIC FUNCTIONS */
/**
 * Starts a session towards the service
 * returns simclient on success, NULL on error
 */
void *simpb_client_open_session()
{
    simpb_client_t *simpb_client_p = malloc(sizeof(*simpb_client_p));
    ste_simpb_result_t result;

    if (NULL == simpb_client_p) {
        goto error;
    }

    /* Init sim_client  */
    simpb_client_p->fd_simpb = -1;
    simpb_client_p->ste_simpb_p = NULL;
    simpb_client_p->ste_simpb_status = STE_SIMPB_STATUS_DISCONNECTED;

    result = ste_simpb_connect(&simpb_client_p->ste_simpb_p, &simpb_client_p->fd_simpb,
                               &simpb_client_p->ste_simpb_status, simpb_client_callback,
                               simpb_client_p);

    if (STE_SIMPB_SUCCESS != result) {
        ATC_LOG_E("ste_simpb_connect, result %d, status %d", result, simpb_client_p->ste_simpb_status);
        goto error;
    }

    /* Set service status */
    exe_start_set_status(EXE_START_SIMPB, EXE_START_CONNECTED_READY);

    /* Register selector callbacks */
    selector_register_callback_for_fd(simpb_client_p->fd_simpb, simpb_client_selector_callback, (void *)simpb_client_p);

    /* Ask simpb to start working against sim */
    result = ste_simpb_startup(simpb_client_p->ste_simpb_p, 0);

    if (STE_SIMPB_SUCCESS != result) {
        ATC_LOG_E("ste_simpb_startup, result %d", result);
        goto error;
    }

    ATC_LOG_I("Done, fd = %d", simpb_client_p->fd_simpb);
    return (void *)simpb_client_p;

error:
    /* Failed to initialize all connections, close down. */
    simpb_client_close_session(simpb_client_p);
    return NULL;
}


/**
 * Do service specific clean up when closing session.
 */
void simpb_client_close_session(simpb_client_t *simpb_client_p)
{
    if (NULL != simpb_client_p) {
        ste_simpb_disconnect(simpb_client_p->ste_simpb_p);

        selector_deregister_callback_for_fd(simpb_client_p->fd_simpb);
        free(simpb_client_p);
        simpb_client_p = NULL;
        exe_start_set_status(EXE_START_SIMPB, EXE_START_NOT_CONNECTED);
    }

}


/**
 * Get the simpb handle from exe client struct
 */
ste_simpb_t *simpb_client_get_handle(simpb_client_t *simpb_client_p)
{
    if (NULL != simpb_client_p) {
        return simpb_client_p->ste_simpb_p;
    } else {
        return NULL;
    }
}

bool simpb_client_is_adn_file(uint16_t file_id)
{
    int i = 0;
    bool res = FALSE;

    for (i = 0; i < simpb_adn_files.no_of_pb_files; i++) {
        if (simpb_adn_files.pb_file_ids[i] == file_id) {
            res = TRUE;
            break;
        }
    }

    return res;
}


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Implementation of request record functions.
 *
 * Author:  Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *          Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 */

#include <stdlib.h>

#include "log.h"

#include "simpb.h"
#include "simpbd_common.h"
#include "simpbd_p.h"
#include "simpbd_handlers.h"
#include "simpbd.h"
#include "simpbd_record.h"

/* Request table. Global variable.*/

static simpb_request_table_t simpb_request_table[] = {
    {STE_SIMPB_REQUEST_ID_UNKNOWN, NULL},
    {STE_SIMPB_REQUEST_ID_READ, simpbd_handler_read},
    {STE_SIMPB_REQUEST_ID_UPDATE, simpbd_handler_update},
    {STE_SIMPB_REQUEST_ID_STATUS_GET, simpbd_handler_status_get},
    {STE_SIMPB_REQUEST_ID_FILE_INFORMATION_GET, simpbd_handler_file_information_get},
    {STE_SIMPB_REQUEST_ID_PHONEBOOK_SELECT, simpbd_handler_phonebook_select},
    {STE_SIMPB_REQUEST_ID_SELECTED_PHONEBOOK_GET, simpbd_handler_selected_phonebook_get},
    {STE_SIMPB_REQUEST_ID_SUPPORTED_PHONEBOOKS_GET, simpbd_handler_supported_phonebooks_get},

    {STE_SIMPB_REQUEST_ID_STARTUP, simpbd_handler_startup},
    {STE_SIMPB_REQUEST_ID_SHUTDOWN, simpbd_handler_shutdown},
    {STE_SIMPB_REQUEST_ID_CACHE_PBR, simpbd_handler_cache_pbr},
    {STE_SIMPB_REQUEST_ID_SYNC_EF_PBC, NULL},
    {STE_SIMPB_REQUEST_ID_CACHE_UPDATE_UID, simpbd_handler_adn_uid_update},
    /* Internal request handler IDs */
    {STE_SIMPB_INTERNAL_REQUEST_ID_SYNC_EF_PBC, simpbd_handler_sync_ef_pbc},
    {STE_SIMPB_INTERNAL_REQUEST_ID_CACHE_ADN_INFO, simpbd_handler_cache_adn_info},
    {STE_SIMPB_INTERNAL_REQUEST_ID_CACHE_UPDATE_UID, simpbd_handler_adn_uid_update},
    {STE_SIMPB_INTERNAL_REQUEST_ID_UPDATE_CC, NULL},
    {STE_SIMPB_INTERNAL_REQUEST_ID_UPDATE_PSC, simpbd_handler_update_psc},
    {STE_SIMPB_INTERNAL_REQUEST_ID_PBR_CACHE_REFRESH, simpbd_handler_cache_pbr},
};


#define STE_SIMPB_SIM_TAG_OFFSET    1
#define STE_SIMPB_MAX_SIM_TAG       8  /* MUST!!! be a power of 2 */
static simpbd_record_t *s_record_array[STE_SIMPB_MAX_SIM_TAG];


/* Functions */

/* Record and request related functions */
simpbd_request_t simpbd_get_request(uint8_t request_id)
{
    int i;

    for (i = STE_SIMPB_REQUEST_ID_UNKNOWN; i < STE_SIMPB_INTERNAL_REQUEST_ID_END_OF_LIST; i++) {
        if (request_id == simpb_request_table[i].request_id) {
            return simpb_request_table[i].request;
        }
    }

    return NULL;
}


/* SIM request record functions */
void simpbd_record_init()
{
    int i;

    for (i = 0; i < STE_SIMPB_MAX_SIM_TAG; i++) {
        s_record_array[i] = NULL;
    }
}


/**
 * simpbd_record_create()
 *
 * Allocate a request record from a static array. Each request is marked as free
 * when available for use. Each request has a unique request_id that can be used
 * to quickly retrieve the request record from the array.
 *
 * Allocation is made in a round-robin fashion so that all array entries are
 * used evenly.
 */
simpbd_record_t *simpbd_record_create(simpbd_request_t  request,
                                      simpbd_client_t  *simpbd_client_p,
                                      uint8_t           request_id,
                                      void             *request_data_p,
                                      void             *caller_data_p)
{
    static int start_i = STE_SIMPB_MAX_SIM_TAG - 1;
    simpbd_record_t *record_p = NULL;
    int i;

    for (i = (start_i + 1) % STE_SIMPB_MAX_SIM_TAG;
            i != start_i;
            i = (i + 1) % STE_SIMPB_MAX_SIM_TAG) {
        if (NULL == s_record_array[i]) {
            record_p = calloc(1, sizeof(simpbd_record_t));

            if (!record_p) {
                SIMPB_LOG_E("calloc failed for record_p!");
                goto error;
            }

            record_p->sim_tag = i;
            record_p->request_state = SIMPBD_REQUEST_STATE_FREE;
            s_record_array[i] = record_p;
        }

        if (NULL != s_record_array[i]) {
            record_p = s_record_array[i];

            if (SIMPBD_REQUEST_STATE_FREE == record_p->request_state) {
                start_i = i;
                break;
            }
        }

        record_p = NULL;
    }

    if (!record_p) {
        SIMPB_LOG_E("Request table exhausted!");
        goto error;
    }

    /* The request_id below is used as a handle that is passed to the modem.
     * By adding the array size (STE_SIMPB_MAX_SIM_TAG) to it each time the
     * request is used, it is easy to detect responses to stale requests.
     * At the same time it remains easy to locate the response by using
     * the remainder after a division by the array size.
     *
     * To ensure there is no problem when the request_id wraps around, the
     * array size must always be a power of 2.
     */
    do {
        record_p->sim_tag += STE_SIMPB_MAX_SIM_TAG;
    } while (0 == record_p->sim_tag);

    record_p->request = request;
    record_p->simpbd_client_p = simpbd_client_p;
    record_p->simpbd_request_id = request_id;
    record_p->request_data_p = request_data_p;
    record_p->caller_data_p = caller_data_p;
    record_p->response_data_p = NULL;
    record_p->private_data_p = NULL;
    record_p->request_state = SIMPBD_REQUEST_STATE_REQUEST;
    record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_INITIAL_VALUE;

    SIMPB_LOG_D("allocated request record %d (%p)", record_p->sim_tag, record_p);

    return record_p;

error:
    return NULL;
}


/**
 * simpbd_record_get_sim_tag()
 *
 * Use the request_id as a handle for SIM to reference the request
 */
uintptr_t simpbd_record_get_sim_tag(const simpbd_record_t *record_p)
{
    return record_p->sim_tag;
}


/**
 * simpbd_record_from_sim_tag()
 *
 * Retrieve pointer to request record based on the request_id
 * passed by SIM
 */
simpbd_record_t *simpbd_record_from_sim_tag(const uintptr_t sim_tag)
{
    simpbd_record_t *record_p = NULL;

    record_p = s_record_array[sim_tag % STE_SIMPB_MAX_SIM_TAG];

    if (NULL == record_p) {
        SIMPB_LOG_E("Invalid request record %d", sim_tag);
        goto error;
    }

    if (SIMPBD_REQUEST_STATE_FREE == record_p->request_state) {
        SIMPB_LOG_E("Invalid state for request record %d (%p)", sim_tag, record_p);
        goto error;
    }

    if (sim_tag != record_p->sim_tag) {
        SIMPB_LOG_E("Stale request record %d, current sim_tag is %d", sim_tag, record_p->sim_tag);
        goto error;
    }

    return record_p;

error:
    return NULL;
}


void simpbd_record_free(simpbd_record_t *record_p)
{
    if (SIMPBD_REQUEST_STATE_FREE == record_p->request_state) {
        SIMPB_LOG_E("Invalid state for request record %d (%p)", record_p->sim_tag, record_p);
        goto error;
    }

    record_p->request_state = SIMPBD_REQUEST_STATE_FREE;
    SIMPB_LOG_D("freeing request record %d (%p)", record_p->sim_tag, record_p);

error:
    return;
}


void simpbd_record_free_all()
{
    simpbd_record_t *record_p = NULL;
    int i;

    for (i = 0; i < STE_SIMPB_MAX_SIM_TAG; i++) {
        record_p = s_record_array[i];
        s_record_array[i] = NULL;

        if (record_p) {
            free(record_p);
        }
    }
}


simpbd_record_t *simpbd_record_get_first_non_free()
{
    simpbd_record_t *record_p = NULL;
    int i;

    for (i = 0; i < STE_SIMPB_MAX_SIM_TAG; i++) {
        record_p = s_record_array[i];

        if (record_p && record_p->request_state != SIMPBD_REQUEST_STATE_FREE) {
            return record_p;
        }
    }

    return NULL; /* A free recod_p may be != NULL hence this hard coded return */
}


ste_simpb_result_t simpbd_execute(simpbd_client_t     *simpbd_client_p,
                                  uint8_t              request_id,
                                  void                *request_data_p,
                                  void                *caller_data_p)
{
    simpbd_request_t request = simpbd_get_request(request_id);
    simpbd_record_t *simpbd_record_p = NULL;
    ste_simpb_result_t result = STE_SIMPB_FAILURE;
    SIMPB_LOG_D("simpbd_client_p=%p, request_id=%d, caller_data_p=%p", simpbd_client_p, request_id, caller_data_p);

    if (NULL == request) {
        result = STE_SIMPB_NOT_SUPPORTED;
        SIMPB_LOG_E("No request handler for request_id=%d!", request_id);
        goto exit;
    }

    simpbd_record_p = simpbd_record_create(request, simpbd_client_p, request_id, request_data_p, caller_data_p);

    if (NULL != simpbd_record_p) {
        if (request_id >= STE_SIMPB_REQUEST_ID_END_OF_LIST) {
            simpbd_record_p->internal_handling = 1;
            SIMPB_LOG_D("Internal request executing");
        } else {
            simpbd_record_p->internal_handling = 0;
        }

        result = request(simpbd_record_p);

        if (STE_SIMPB_PENDING != result) {
            /* The request terminated, free the record. */
            simpbd_record_free(simpbd_record_p);
        }
    } else {
        SIMPB_LOG_E("Unable to create record!");
    }

exit:

    return result;
}

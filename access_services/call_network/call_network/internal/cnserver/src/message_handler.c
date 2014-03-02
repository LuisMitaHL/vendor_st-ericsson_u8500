/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

#include "cn_client.h"
#include "cn_log.h"
#include "cn_macros.h"

#include "cnsocket.h"
#include "cnserver.h"
#include "message_handler.h"
#include "request_handling.h"
#include "cn_pdc.h"
#include "cn_rat_control.h"

#define MAX_NUMBER_CLIENT_TAG 64 /* MUST!!! be a power of 2 */

static request_record_t *s_record_array[MAX_NUMBER_CLIENT_TAG] = {NULL}; /* the rest will be NULL as well according to ISO C */

/**
 * request_record_create()
 *
 * Allocate a request record from a static array. Each request is marked as free
 * when available for use. Each request has a unique request_id that can be used
 * to quickly retrieve the request record from the array.
 *
 * Allocation is made in a round-robin fashion so that all array entries are
 * used evenly.
 */
request_record_t *request_record_create(cn_message_type_t message_type, const cn_client_tag_t client_tag, const int client_id)
{
    static unsigned int start_id = MAX_NUMBER_CLIENT_TAG - 1;
    request_record_t *record_p = NULL;
    unsigned int i, id, end_id;

    /*
     * Make a single pass over the array, starting at the last allocated position + 1.
     * Never use record_id's in the range; 0 - (MAX_NUMBER_CLIENT_TAG - 1).
     * So, when the id counter wraps around, add MAX_NUMBER_CLIENT_TAG to it.
     */
    for (id = start_id + 1, end_id = start_id + MAX_NUMBER_CLIENT_TAG;
            id != end_id;
            id++) {

        /* Calculate array number */
        i = id % MAX_NUMBER_CLIENT_TAG;

        /* array position never used before, allocate memory for record */
        if (NULL == s_record_array[i]) {
            record_p = calloc(1, sizeof(request_record_t));

            if (!record_p) {
                CN_LOG_E("calloc failed for record_p!");
                goto error;
            }

            record_p->state = REQUEST_STATE_FREE;
            s_record_array[i] = record_p;
        }

        /* array position is allocated, reuse if it is free */
        if (NULL != s_record_array[i]) {
            record_p = s_record_array[i];

            if (REQUEST_STATE_FREE == record_p->state) {
                /* Set the current request_id for the record, but
                 * if it has wrapped around, adjust it first. */
                if (MAX_NUMBER_CLIENT_TAG > id) {
                    id += MAX_NUMBER_CLIENT_TAG;
                }

                /* The request_id below is used as a handle that is passed to the modem.
                 * By using a sequential request_id that is always incremented,
                 * it becomes easy to detect responses to stale requests.
                 * At the same time it remains easy to locate the response by dividing
                 * the request_id by the array size and using the remainder as the
                 * array index.
                 *
                 * To ensure there is no problem when the request_id wraps around, the
                 * array size must always be a power of 2.
                 */
                record_p->request_id = start_id = id;
                break;
            }
        }

        record_p = NULL;
    }

    if (!record_p) {
        CN_LOG_E("Request table exhausted!");
        print_request_table();
        goto error;
    }

    record_p->message_type = message_type;
    record_p->client_tag = client_tag;
    record_p->client_id = client_id;
    record_p->request_handler_p = NULL;
    record_p->request_data_p = NULL;
    record_p->ss_request_info_p = NULL;
    record_p->pdc_data_p = NULL;
    record_p->state = REQUEST_STATE_REQUEST;
    record_p->response_data_p = NULL;

    CN_LOG_D("allocated request record %d[%d] (%p)", record_p->request_id, i, record_p);

    return record_p;

error:
    return NULL;
}


/**
 * request_record_get_modem_tag()
 *
 * Use the request_id as a handle for the modem to reference the request
 */
void *request_record_get_modem_tag(const request_record_t *record_p)
{
    return (void *)record_p->request_id;
}


/**
 * request_record_from_modem_tag()
 *
 * Retrieve pointer to request record based on the request_id
 * passed by the modem
 */
request_record_t *request_record_from_modem_tag(void *modem_tag)
{
    request_record_t *record_p;
    unsigned int request_id;


    request_id = (unsigned int)modem_tag;
    record_p = s_record_array[request_id % MAX_NUMBER_CLIENT_TAG];

    if (NULL == record_p) {
        CN_LOG_E("Invalid request record %d", request_id);
        goto error;
    }

    if (REQUEST_STATE_FREE == record_p->state) {
        CN_LOG_E("Invalid state for request record %d (%p)", request_id, record_p);
        goto error;
    }

    if (request_id != record_p->request_id) {
        CN_LOG_E("Stale request record %d, current value is %d", request_id, record_p->request_id);
        goto error;
    }

    return record_p;

error:
    return NULL;
}


void request_record_free(request_record_t *record_p)
{

    if (REQUEST_STATE_FREE == record_p->state) {
        CN_LOG_E("Invalid state for request record %d (%p)", record_p->request_id, record_p);
        goto error;
    }

    record_p->state = REQUEST_STATE_FREE;
    CN_LOG_D("freeing request record %d (%p)", record_p->request_id, record_p);

error:
    return;
}


void request_record_free_all()
{
    request_record_t *record_p;
    int i;


    for (i = 0; i < MAX_NUMBER_CLIENT_TAG; i++) {
        record_p = s_record_array[i];
        s_record_array[i] = NULL;

        if (record_p) {
            free(record_p);
        }
    }

}

void print_request_table()
{
    int i = 0;
    char *msg_string_p = NULL;
    char *state_strings[] = {"Free", "Pending"};
    char *state_string_p = NULL;
    char error_message[30];


    memset(&error_message, 0, sizeof(error_message));

    CN_LOG_E("");
    CN_LOG_E("Request Table:");
    CN_LOG_E("+-------------+------------+---------+------------------------------------------------------------------+");
    CN_LOG_E("| Array Index | Request Id | State   | Description                                                      |");
    CN_LOG_E("+-------------+------------+---------+------------------------------------------------------------------+");

    for (i = 0; i < MAX_NUMBER_CLIENT_TAG; i++) {
        if (s_record_array[i]) {
            state_string_p = (REQUEST_STATE_FREE == s_record_array[i]->state) ? state_strings[0] : state_strings[1];
            msg_string_p   = (REQUEST_STATE_FREE == s_record_array[i]->state) ? "" : lookup_message_table(s_record_array[i]->message_type);

            if (!msg_string_p) {
                sprintf((char *)&error_message, "UNKNOWN REQUEST! (id=%d)", s_record_array[i]->message_type);
                msg_string_p = (char *)&error_message;
            }

            CN_LOG_E("| %-11d | %-10d | %-7s | %-64s |", i, s_record_array[i]->request_id, state_string_p, msg_string_p);
        } else {
            CN_LOG_E("| %-11d | %-10s | %-7s | %-64s |", i, "", state_strings[0], "");
        }
    } /* end of for loop */

    CN_LOG_E("+-------------+------------+---------+------------------------------------------------------------------+");
    CN_LOG_E("");

}

int send_response(int client_id, cn_message_type_t type, cn_error_code_t result_code, cn_client_tag_t client_tag, cn_uint32_t payload_size, void *payload_p)
{
    cn_message_t *message_p = NULL;
    int msg_size = 0;
    int result = -1;


    CN_LOG_D("client_id: %d type: %d result_code: %d payload_size: %d",
             client_id, type, result_code, payload_size);

    msg_size = sizeof(*message_p) + payload_size;
    message_p = calloc(1, msg_size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = (cn_uint32_t)type;
    message_p->client_tag   = client_tag;
    message_p->error_code   = result_code;
    message_p->payload_size = payload_size;

    if (payload_size && payload_p) {
        memcpy(message_p->payload, payload_p, payload_size);
    }

    result = cnserver_send_response(client_id, message_p, msg_size);

    free(message_p);

    return result;

error:
    return -1;
}


int send_event(cn_message_type_t type, cn_error_code_t result_code, cn_uint32_t payload_size, void *payload_p)
{
    cn_message_t *message_p = NULL;
    int msg_size = 0;
    int result = -1;


    CN_LOG_D("type: %d result_code: %d payload_size: %d", type, result_code, payload_size);

    msg_size = sizeof(*message_p) + payload_size;
    message_p = calloc(1, msg_size);

    if (!message_p) {
        CN_LOG_E("calloc failed for message_p");
        goto error;
    }

    message_p->type         = (cn_uint32_t)type;
    message_p->client_tag   = 0;
    message_p->error_code   = result_code;
    message_p->payload_size = payload_size;

    if (payload_size && payload_p) {
        memcpy(message_p->payload, payload_p, payload_size);
    }

    result = cnserver_send_event(message_p, msg_size);

    free(message_p);

    return result;

error:
    return -1;
}


/*
 * The purpose of this function is to extract messages from a request socket
 * and delegate the handling to a modem specific handler. This is only a
 * relay function, all parameter checks should be placed in the handler
 * functions. The only check here is the validity of the request messages
 * themselves.
 */
void handle_request(const cn_message_t *msg_p, const int client_id)
{
    request_record_t *record_p = NULL;
    request_status_t status = REQUEST_STATUS_ERROR;

    CN_LOG_D("type:%d, client_tag:%d", msg_p->type, msg_p->client_tag);

    record_p = request_record_create(msg_p->type, msg_p->client_tag, client_id);

    if (!record_p) {
        CN_LOG_E("ERROR, not possible to create request record!");
        send_response(client_id, CN_RESPONSE_UNKNOWN, CN_FAILURE, msg_p->client_tag, 0, NULL);
        return;
    }

    switch (msg_p->type) {
    case CN_REQUEST_RF_ON:
        record_p->request_handler_p = handle_request_rf_on;
        status = handle_request_rf_on(NULL, record_p);
        break;
    case CN_REQUEST_RF_OFF:
        record_p->request_handler_p = handle_request_rf_off;
        status = handle_request_rf_off(NULL, record_p);
        break;
    case CN_REQUEST_RF_STATUS:
        record_p->request_handler_p = handle_request_rf_status;
        status = handle_request_rf_status(NULL, record_p);
        break;
    case CN_REQUEST_REGISTRATION_CONTROL:
        record_p->request_handler_p = handle_request_registration_control;
        status = handle_request_registration_control((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_RESET_MODEM:
        record_p->request_handler_p = handle_request_modem_reset;
        status = handle_request_modem_reset(NULL, record_p);
        break;
    case CN_REQUEST_RESET_MODEM_WITH_DUMP:
        record_p->request_handler_p = handle_request_modem_reset_with_dump;
        status = handle_request_modem_reset_with_dump((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SLEEP_TEST_MODE:
        record_p->request_handler_p = handle_request_sleep_test_mode;
        status = handle_request_sleep_test_mode((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_PREFERRED_NETWORK_TYPE:
        record_p->request_handler_p = cn_rat_control_handle_request;
        status = cn_rat_control_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_GET_PREFERRED_NETWORK_TYPE:
        record_p->request_handler_p = handle_request_get_preferred_network_type;
        status = handle_request_get_preferred_network_type(NULL, record_p);
        break;
    case CN_REQUEST_REGISTRATION_STATE_NORMAL:
        record_p->request_handler_p = handle_request_registration_state_normal;
        status = handle_request_registration_state_normal(NULL, record_p);
        break;
    case CN_REQUEST_REGISTRATION_STATE_GPRS:
        record_p->request_handler_p = handle_request_registration_state_gprs;
        status = handle_request_registration_state_gprs(NULL, record_p);
        break;
    case CN_REQUEST_CELL_INFO:
        record_p->request_handler_p = handle_request_cell_info;
        status = handle_request_cell_info(NULL, record_p);
        break;
    case CN_REQUEST_AUTOMATIC_NETWORK_REGISTRATION:
        record_p->request_handler_p = handle_request_automatic_network_registration;
        status = handle_request_automatic_network_registration((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_MANUAL_NETWORK_REGISTRATION:
        record_p->request_handler_p = handle_request_manual_network_registration;
        status = handle_request_manual_network_registration((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK:
        record_p->request_handler_p = handle_request_manual_network_registration_with_automatic_fallback;
        status = handle_request_manual_network_registration_with_automatic_fallback((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_NETWORK_DEREGISTER:
        record_p->request_handler_p = handle_request_network_deregister;
        status = handle_request_network_deregister((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_NET_QUERY_MODE:
        record_p->request_handler_p = handle_request_net_query_mode;
        status = handle_request_net_query_mode(NULL, record_p);
        break;
    case CN_REQUEST_MANUAL_NETWORK_SEARCH:
        record_p->request_handler_p = handle_request_manual_network_search;
        status = handle_request_manual_network_search(NULL, record_p);
        break;
    case CN_REQUEST_INTERRUPT_NETWORK_SEARCH:
        record_p->request_handler_p = handle_request_interrupt_network_search;
        status = handle_request_interrupt_network_search(NULL, record_p);
        break;
    case CN_REQUEST_CURRENT_CALL_LIST:
        record_p->request_handler_p = handle_request_current_call_list;
        status = handle_request_current_call_list(NULL, record_p);
        break;
    case CN_REQUEST_DIAL:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_HANGUP:
        record_p->request_handler_p = handle_request_hangup;
        status = handle_request_hangup((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SWAP_CALLS:
        record_p->request_handler_p = handle_request_swap_calls;
        status = handle_request_swap_calls((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_HOLD_CALL:
        record_p->request_handler_p = handle_request_hold_call;
        status = handle_request_hold_call((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_RESUME_CALL:
        record_p->request_handler_p = handle_request_resume_call;
        status = handle_request_resume_call((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_CONFERENCE_CALL:
        record_p->request_handler_p = handle_request_conference_call;
        status = handle_request_conference_call((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_CONFERENCE_CALL_SPLIT:
        record_p->request_handler_p = handle_request_conference_call_split;
        status = handle_request_conference_call_split((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_EXPLICIT_CALL_TRANSFER:
        record_p->request_handler_p = handle_request_explicit_call_transfer;
        status = handle_request_explicit_call_transfer((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_ANSWER_CALL:
        record_p->request_handler_p = handle_request_answer_call;
        status = handle_request_answer_call((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_USSD:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_USSD_ABORT:
        record_p->request_handler_p = handle_request_ussd_abort;
        status = handle_request_ussd_abort(NULL, record_p);
        break;
    case CN_REQUEST_RSSI_VALUE:
        record_p->request_handler_p = handle_request_rssi_value;
        status = handle_request_rssi_value(NULL, record_p);
        break;
    case CN_REQUEST_CLIP_STATUS:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_CNAP_STATUS:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_COLR_STATUS:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_CLIR_STATUS:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_CLIR:
        record_p->request_handler_p = handle_request_set_clir;
        status = handle_request_set_clir((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_QUERY_CALL_FORWARD:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_CALL_FORWARD:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_MODEM_PROPERTY:
        record_p->request_handler_p = handle_request_set_modem_property;
        status = handle_request_set_modem_property((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_GET_MODEM_PROPERTY:
        record_p->request_handler_p = handle_request_get_modem_property;
        status = handle_request_get_modem_property((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_CSSN:
        record_p->request_handler_p = handle_request_set_cssn;
        status = handle_request_set_cssn((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_DTMF_SEND:
        record_p->request_handler_p = handle_request_dtmf_send;
        status = handle_request_dtmf_send((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_DTMF_START:
        record_p->request_handler_p = handle_request_dtmf_start;
        status = handle_request_dtmf_start((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_DTMF_STOP:
        record_p->request_handler_p = handle_request_dtmf_stop;
        status = handle_request_dtmf_stop(NULL, record_p);
        break;
    case CN_REQUEST_SET_CALL_WAITING:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;

    case CN_REQUEST_GET_CALL_WAITING:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_CHANGE_BARRING_PASSWORD:
        record_p->request_handler_p = handle_request_change_barring_password;
        status = handle_request_change_barring_password((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_QUERY_CALL_BARRING:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_CALL_BARRING:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_PRODUCT_PROFILE_FLAG:
        record_p->request_handler_p = handle_request_set_product_profile_flag;
        status = handle_request_set_product_profile_flag((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_L1_PARAMETER:
        record_p->request_handler_p = handle_request_set_l1_parameter;
        status = handle_request_set_l1_parameter((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_USER_ACTIVITY_STATUS:
        record_p->request_handler_p = handle_request_set_user_activity_status;
        status = handle_request_set_user_activity_status((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_NEIGHBOUR_CELLS_REPORTING:
        record_p->request_handler_p = handle_request_set_neighbour_cells_reporting;
        status = handle_request_set_neighbour_cells_reporting((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_GET_NEIGHBOUR_CELLS_REPORTING:
        record_p->request_handler_p = handle_request_get_neighbour_cells_reporting;
        status = handle_request_get_neighbour_cells_reporting((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_GET_NEIGHBOUR_CELLS_COMPLETE_INFO:
        record_p->request_handler_p = handle_request_get_neighbour_cells_basic_extd_info;
        status = handle_request_get_neighbour_cells_basic_extd_info((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_GET_NEIGHBOUR_CELLS_EXTD_INFO:
        record_p->request_handler_p = handle_request_get_neighbour_cells_extd_info;
        status = handle_request_get_neighbour_cells_extd_info((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_EVENT_REPORTING:
        record_p->request_handler_p = handle_request_set_event_reporting;
        status = handle_request_set_event_reporting((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_GET_RAB_STATUS:
        record_p->request_handler_p = handle_request_get_rab_status;
        status = handle_request_get_rab_status(NULL, record_p);
        break;
    case CN_REQUEST_BASEBAND_VERSION:
        record_p->request_handler_p = handle_request_baseband_version;
        status = handle_request_baseband_version(NULL, record_p);
        break;
    case CN_REQUEST_GET_PP_FLAGS:
        record_p->request_handler_p = handle_request_get_pp_flags;
        status = handle_request_get_pp_flags((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_MODIFY_EMERGENCY_NUMBER_LIST:
        record_p->request_handler_p = handle_request_modify_emergency_number_list;
        status = handle_request_modify_emergency_number_list((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_GET_EMERGENCY_NUMBER_LIST:
        record_p->request_handler_p = handle_request_get_emergency_number_list;
        status = handle_request_get_emergency_number_list((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_SIGNAL_INFO_CONFIG:
        record_p->request_handler_p = handle_request_set_signal_info_config;
        status = handle_request_set_signal_info_config((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_GET_SIGNAL_INFO_CONFIG:
        record_p->request_handler_p = handle_request_get_signal_info_config;
        status = handle_request_get_signal_info_config((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_SIGNAL_INFO_REPORTING:
        record_p->request_handler_p = handle_request_set_signal_info_reporting;
        status = handle_request_set_signal_info_reporting((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_GET_SIGNAL_INFO_REPORTING:
        record_p->request_handler_p = handle_request_get_signal_info_reporting;
        status = handle_request_get_signal_info_reporting((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_TIMING_ADVANCE:
        record_p->request_handler_p = handle_request_get_timing_advance;
        status = handle_request_get_timing_advance(NULL, record_p);
        break;
    case CN_REQUEST_NMR_INFO:
        record_p->request_handler_p = handle_request_get_nmr_info;
        status = handle_request_get_nmr_info((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_HSXPA_MODE:
        record_p->request_handler_p = handle_request_set_hsxpa_mode;
        status = handle_request_set_hsxpa_mode((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_GET_HSXPA_MODE:
        record_p->request_handler_p = handle_request_get_hsxpa_mode;
        status = handle_request_get_hsxpa_mode((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SS:
        record_p->request_handler_p = cn_pdc_handle_request;
        status = cn_pdc_handle_request((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_REG_STATUS_EVENT_CONFIG:
        record_p->request_handler_p = handle_request_reg_status_event_config;
        status = handle_request_reg_status_event_config((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_RAT_NAME:
        record_p->request_handler_p = handle_request_rat_name;
        status = handle_request_rat_name(NULL, record_p);
        break;
    case CN_REQUEST_MODEM_POWER_OFF:
        record_p->request_handler_p = handle_request_modem_power_off;
        status = handle_request_modem_power_off(NULL, record_p);
        break;
    case CN_REQUEST_SEND_TX_BACK_OFF_EVENT:
        record_p->request_handler_p = handle_request_send_tx_back_off_event;
        status = handle_request_send_tx_back_off_event((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_DEFAULT_NVMD:
        record_p->request_handler_p = handle_request_set_default_nvmd;
        status = handle_request_set_default_nvmd((void *)msg_p->payload, record_p);
        break;
    case CN_REQUEST_SET_EMPAGE:
        record_p->request_handler_p = handle_request_set_empage;
        status = handle_request_set_empage((void *)msg_p->payload, record_p);
        break;
    default:
        CN_LOG_W("unknown request message! (msg type:%d)", msg_p->type);
        send_response(record_p->client_id, CN_RESPONSE_UNKNOWN, CN_FAILURE, record_p->client_tag, 0, NULL);
        break;
    }

    /* Cleanup for the cases when the record structure no longer is needed. The scenarios are
     * in case of error and for synchronous request handlers. The record is normally freed in the
     * modem client handler after invoking the response handler.
     */
    if (REQUEST_STATUS_PENDING != status) {
        request_record_free(record_p);
    }

}



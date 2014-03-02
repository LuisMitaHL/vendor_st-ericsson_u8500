/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <string.h>

#include "exe_internal.h"
#include "exe_pscc_client.h"
#include "exe_pscc_iadb.h"
#include "exe_pscc_requests_data_call_list.h"
#include "exe_pscc_requests_deactivate_data_call.h"
#include "exe_pscc_requests.h"
#include "exe_pscc_requests_modify_data_call.h"
#include "exe_pscc_requests_nap.h"
#include "exe_pscc_requests_dun.h"
#include "exe_pscc_requests_send_data_uplink_call.h"
#include "exe_pscc_requests_setup_data_call.h"
#include "exe_pscc_util.h"
#include "exe_request_record_p.h"

#include "pscc_msg.h"


#define DEFAULT_PDP_TYPE (pscc_pdp_type_ipv4)
#define DEFAULT_D_COMP (0)
#define DEFAULT_H_COMP (0)

extern bool add_cgpaddr_cid_to_list(int cid, exe_cgpaddr_cid_t **exe_cgpaddr_cid_pp);
extern void free_cgpaddr_cid_list(exe_cgpaddr_cid_t *exe_cgpaddr_cid_p);

/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static exe_pscc_result_t exe_pscc_setup_data_call(exe_request_record_t *record_p);
static exe_pscc_result_t exe_pscc_deactivate_data_call(exe_request_record_t *record_p);
static exe_pscc_result_t exe_pscc_create_sterc(exe_request_record_t *record_p);
static exe_request_result_t exe_pscc_set_request_result(exe_pscc_result_t result);
static exe_pscc_result_t exe_pscc_start_pscc_interaction(exe_request_record_t *record_p,
        int cid,
        bool activate,
        void *channel_id_p,
        void *conn_id_p);
static bool exe_pscc_get_connection_parameters(exe_request_record_t *record_p, int cid);
static bool set_default_req_qos_parameters(int cid);
static bool set_default_min_qos_parameters(int cid);
static bool set_default_qos_2G_parameters(int cid);
static bool set_default_pdp_context_parameters(int cid);
static bool add_cid_to_cgcmod_set_list(exe_cgcmod_set_request_t *exe_cgcmod_set_request_p, int cid);
static bool add_cid_to_cgact_set_list(exe_cgact_set_request_t *exe_cgact_set_request_p, int cid);
static void free_cgact_set_list(exe_cgact_set_request_t *cgact_p);
static void free_cgcmod_set_list(exe_cgcmod_set_request_t *cgcmod_p);
static bool psccclient_add_qos_item_to_request_list(exe_request_record_t *record_p, exe_pdp_qos_t *qos_p);
static bool psccclient_add_qos_2G_item_to_request_list(exe_request_record_t *record_p, exe_pdp_qos_2G_t *qos_p);
/********************************************************************
 * Private methods
 ********************************************************************
 */

/*
 * returns true if current index in connection id vector is equal to (or greater than) the length of the vector.
 */
bool psccclient_increment_and_get_current_data_list_index(exe_request_record_t *record_p, int *current_index)
{
    bool result = false;
    psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)request_record_get_client_data(record_p);

    if (NULL == client_data_p) {
        ATC_LOG_E("client_data is NULL. Failed.");
        goto exit;
    }

    client_data_p->connection_ids_current_index++;
    *current_index = client_data_p->connection_ids_current_index;

    if (client_data_p->connection_ids_current_index >= client_data_p->connection_ids_length) {
        result = true;
    }

exit:
    return result;
}


/**
 * set_default_req_qos_parameters
 *
 *  Description:
 *  Sets default required qos parameters into iadb
 */
static bool set_default_req_qos_parameters(int cid)
{
    bool    iadb_results   = true;
    int     default_int;
    char   *default_char;

    ATC_LOG_I("Default qos for cid =%d", cid);

    default_int = EXE_QOS3G_PROFILE_TRAFFIC_CLASS_REQ;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_traffic_class, &default_int);

    default_int = EXE_QOS3G_PROFILE_MAX_BITRATE_UL;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_max_bitrate_ul, &default_int);

    default_int = EXE_QOS3G_PROFILE_MAX_BITRATE_DL;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_max_bitrate_dl, &default_int);

    default_int = EXE_QOS3G_PROFILE_GUARANTEED_BITRATE_UL;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_guaranteed_bitrate_ul, &default_int);

    default_int = EXE_QOS3G_PROFILE_GUARANTEED_BITRATE_DL;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_guaranteed_bitrate_dl, &default_int);

    default_int = EXE_QOS3G_PROFILE_DELIVERY_ORDER_REQ;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_delivery_order, &default_int);

    default_int = EXE_QOS3G_PROFILE_SDU_SIZE;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_max_sdu_size, &default_int);

    default_int = EXE_QOS3G_PROFILE_DELIVERY_ERR_SDU_REQ;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_delivery_err_sdu, &default_int);

    default_int = EXE_QOS3G_PROFILE_TRANSFER_DELAY;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_transfer_delay, &default_int);

    default_int = EXE_QOS3G_PROFILE_TRAFFIC_HANDLING_PRIORITY;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_traffic_handling_priority, &default_int);

    default_char = EXE_QOS3G_PROFILE_SDU_ERROR_RATIO;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_sdu_error_ratio_p, &default_char);

    default_char = EXE_QOS3G_PROFILE_RESIDUAL_BER;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_residual_ber_p, &default_char);

    default_int = EXE_QOS3G_PROFILE_SOURCE_STATISTICS_DESCRIPTOR;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_source_statistics_descriptor, &default_int);

    default_int = EXE_QOS3G_PROFILE_SIGNALLING_INDICATION;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_signalling_indication, &default_int);

    return iadb_results;
}

/**
 *  set_default_qos_2G_parameters
 *
 *  Description:
 *  Sets default minimum qos parameters into iadb for the rel97/2G parameters for min and req.
 */
static bool set_default_qos_2G_parameters(int cid)
{
    bool    iadb_results   = true;
    int     default_value;

    ATC_LOG_I("Default 2G qos for cid =%d", cid);

    default_value = EXE_QOS2G_PROFILE_DEFAULT;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_precedence, &default_value);
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_delay, &default_value);
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_reliability, &default_value);
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_peak, &default_value);
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_mean, &default_value);

    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_precedence, &default_value);
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_delay, &default_value);
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_reliability, &default_value);
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_peak, &default_value);
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_req_mean, &default_value);

    return iadb_results;
}

/**
 * set_default_min_qos_parameters
 *
 *  Description:
 *  Sets default minimum qos parameters into iadb
 */
static bool set_default_min_qos_parameters(int cid)
{
    bool    iadb_results   = true;
    int     default_int;
    char   *default_char;

    default_int = EXE_QOS3G_PROFILE_TRAFFIC_CLASS;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_traffic_class, &default_int);

    default_int = EXE_QOS3G_PROFILE_MAX_BITRATE_UL;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_max_bitrate_ul, &default_int);

    default_int = EXE_QOS3G_PROFILE_MAX_BITRATE_DL;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_max_bitrate_dl, &default_int);

    default_int = EXE_QOS3G_PROFILE_GUARANTEED_BITRATE_UL;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_guaranteed_bitrate_ul, &default_int);

    default_int = EXE_QOS3G_PROFILE_GUARANTEED_BITRATE_DL;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_guaranteed_bitrate_dl, &default_int);

    default_int = EXE_QOS3G_PROFILE_DELIVERY_ORDER;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_delivery_order, &default_int);

    default_int = EXE_QOS3G_PROFILE_SDU_SIZE;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_max_sdu_size, &default_int);

    default_int = EXE_QOS3G_PROFILE_DELIVERY_ERR_SDU;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_delivery_err_sdu, &default_int);

    default_int = EXE_QOS3G_PROFILE_TRANSFER_DELAY;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_transfer_delay, &default_int);

    default_int = EXE_QOS3G_PROFILE_TRAFFIC_HANDLING_PRIORITY;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_traffic_handling_priority, &default_int);

    default_char = EXE_QOS3G_PROFILE_SDU_ERROR_RATIO;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_sdu_error_ratio_p, &default_char);

    default_char = EXE_QOS3G_PROFILE_RESIDUAL_BER;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_residual_ber_p, &default_char);

    default_int = EXE_QOS3G_PROFILE_SOURCE_STATISTICS_DESCRIPTOR;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_source_statistics_descriptor, &default_int);

    default_int = EXE_QOS3G_PROFILE_SIGNALLING_INDICATION;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_min_signalling_indication, &default_int);

    return iadb_results;
}


/**
 * set_default_pdp_context_parameters
 *
 *  Description:
 *  Sets default required parameters related to +CGDCONT into iadb
 */
static bool set_default_pdp_context_parameters(int cid)
{
    bool   iadb_results   = true;
    int    default_int;
    char  *default_char;

    default_int = DEFAULT_PDP_TYPE;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_pdp_type, &default_int);
    default_char = DEFAULT_APN;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_apn_p, &default_char);
    default_char = DEFAULT_PDP_ADDR;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_pdp_address_p, &default_char);
    default_char = DEFAULT_PDP_ADDR;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_ipv6_pdp_address_p, &default_char);
    default_int = DEFAULT_D_COMP;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_d_comp, &default_int);
    default_int = DEFAULT_H_COMP;
    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_h_comp, &default_int);

    return iadb_results;
}


static bool add_cid_to_cgact_set_list(exe_cgact_set_request_t *exe_cgact_set_request_p, int cid)
{
    exe_cgact_set_request_cid_t *new_cid_p = NULL;

    if (NULL == exe_cgact_set_request_p) {
        return false;
    }

    new_cid_p = (exe_cgact_set_request_cid_t *)calloc(1, sizeof(exe_cgact_set_request_cid_t));

    if (NULL == new_cid_p) {
        return false;
    }

    new_cid_p->cid = cid;

    if (NULL == exe_cgact_set_request_p->cid_p) {
        exe_cgact_set_request_p->cid_p = new_cid_p;
    } else {
        exe_cgact_set_request_cid_t *tmp_cid_p = exe_cgact_set_request_p->cid_p;

        while (NULL != tmp_cid_p->next_p) {
            tmp_cid_p = tmp_cid_p->next_p;
        }

        tmp_cid_p->next_p = new_cid_p;
    }

    return true;
}

static bool add_cid_to_cgcmod_set_list(exe_cgcmod_set_request_t *exe_cgcmod_set_request_p, int cid)
{
    exe_cgcmod_set_request_cid_t *new_cid_p = NULL;

    if (NULL == exe_cgcmod_set_request_p) {
        return false;
    }

    new_cid_p = (exe_cgcmod_set_request_cid_t *) calloc(1, sizeof(exe_cgcmod_set_request_cid_t));

    if (NULL == new_cid_p) {
        return false;
    }

    new_cid_p->cid = cid;

    if (NULL == exe_cgcmod_set_request_p->cid_p) {
        exe_cgcmod_set_request_p->cid_p = new_cid_p;
    } else {
        exe_cgcmod_set_request_cid_t *tmp_cid_p = exe_cgcmod_set_request_p->cid_p;

        while (NULL != tmp_cid_p->next_p) {
            tmp_cid_p = tmp_cid_p->next_p;
        }

        tmp_cid_p->next_p = new_cid_p;
    }

    return true;
}

static void free_cgact_set_list(exe_cgact_set_request_t *cgact_p)
{
    if (NULL != cgact_p) {
        exe_cgact_set_request_cid_t *next_p = NULL;
        exe_cgact_set_request_cid_t *current_p = cgact_p->cid_p;

        while (NULL != current_p) {
            next_p = current_p->next_p;
            free((void *)current_p);
            current_p = next_p;
        }

        free(cgact_p);
    }
}

static void free_cgcmod_set_list(exe_cgcmod_set_request_t *cgcmod_p)
{
    if (NULL != cgcmod_p) {
        exe_cgcmod_set_request_cid_t *next_p    = NULL;
        exe_cgcmod_set_request_cid_t *current_p = cgcmod_p->cid_p;

        while (NULL != current_p) {
            next_p = current_p->next_p;
            free((void *) current_p);
            current_p = next_p;
        }

        free(cgcmod_p);
    }
}


static exe_pscc_result_t exe_pscc_create_sterc(exe_request_record_t *record_p)
{
    exe_pscc_connection_response_t *response_data_p = NULL;
    psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;
    exe_pscc_result_t result = EXE_PSCC_FAILURE;
    int cid = client_data_p->cid;
    int zero = 0;


    /* get required connection parameters, set routing rules and activate nap connection */
    /* Reset client tag in iadb for the handled cid */
    psccclient_iadb_set(cid, psccclient_iadb_field_at_ct, &zero);

    response_data_p = (exe_pscc_connection_response_t *)record_p->response_data_p;

    if (NULL == response_data_p) {
        ATC_LOG_E("response_data_p is NULL.");
        goto error;
    }

    /* [2] Activate nap connection */
    result = pscc_request_nap_send_sterc_create(record_p, cid, response_data_p->dns_server_p, response_data_p->netdev_name_p);

    if (EXE_PSCC_SUCCESS != result) {
        ATC_LOG_E("pscc_request_nap_send_sterc_create result is not success.");
        goto error;
    }

    free_pscc_connection_response(response_data_p);
    record_p->response_data_p = NULL;
    record_p->state = EXE_STATE_STERC_CREATE;

    return result;

error:
    return result;
}


static exe_request_result_t exe_pscc_set_request_result(exe_pscc_result_t result)
{
    exe_request_result_t request_result = EXE_SUCCESS;

    if (EXE_PSCC_FAILURE == result || EXE_PSCC_FAILURE_DESTROY == result) {
        request_result = EXE_FAILURE;
    }

    return request_result;
}


static exe_pscc_result_t exe_pscc_start_pscc_interaction(exe_request_record_t *record_p, int cid, bool activate,
        void *channel_id_p, void *conn_id_p)
{
    bool iadb_results = true;
    exe_pscc_result_t result = EXE_PSCC_FAILURE;

    if (NULL != channel_id_p) {
        ATC_LOG_I("channel_id value set, ignored");
        iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_channel_id, channel_id_p);
    }

    if (NULL != conn_id_p) {
        iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_conn_id, conn_id_p);
    }

    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_at_ct, (void *)&record_p);

    /*Note that channel_id is from spec and part of the IAdb entry, the desired value of the issuer of the AT command.
     * Conn_id is something that is used by the executer to handle PSCC. Note that they do not necessarily have to be
     * the same after PSCC create, although they are so currently. If channel_id is same as an already existing conn_id
     * PSCC will fail the create request*/

    if (!iadb_results) {
        ATC_LOG_E("Failed to set some of the parameters. Failed.");
        result = EXE_PSCC_FAILURE;
    }

    if (activate) {
        result = pscc_request_setup_data_call_scenario_0_1_step_1_send_pscc_create_request(record_p);
    } else {
        result = pscc_request_deactivate_data_call_scenario_0_step_1_send_pscc_disconnect_request(record_p);
    }

    return result;
}


static bool exe_pscc_get_connection_parameters(exe_request_record_t *record_p, int cid)
{
    int set_flags = 0;
    exe_request_result_t result = EXE_FAILURE;

    /* Check if cid is registered and fetch the connection id*/
    set_flags = psccclient_iadb_get_set_flags_from_cid(cid);

    if (0 == set_flags) {
        ATC_LOG_E("Incorrect cid");
        goto error;
    }

    if (!psccclient_iadb_set(cid, psccclient_iadb_field_at_ct, (void *)&record_p)) {
        ATC_LOG_E("failed to set at_ct in iadb.");
        goto error;
    }

    result = pscc_request_setup_data_call_send_pscc_get(record_p);

    if (result == EXE_PSCC_SUCCESS) {
        return true;
    }

error:
    return false;
}

/**
 *  Handles set up of a data call.
 *  Shall be called from the request being executed with the current record.
 *  Does not initiate the setup as this requires the record to be in the state
 *  EXE_REQUEST and this is not always true.
 */
static exe_pscc_result_t exe_pscc_setup_data_call(exe_request_record_t *record_p)
{
    exe_pscc_result_t result = EXE_PSCC_FAILURE;
    pscc_msg_t *request_p = NULL;
    psccclient_record_data_t *client_data_p   = (psccclient_record_data_t *)record_p->client_data_p;

    if (NULL != client_data_p->pscc_event_p && pscc_event_disconnected == client_data_p->pscc_event_p->id
            && EXE_STATE_SETUP_DATA_CALL_WAIT_RECONNECT != record_p->state) {
        /* In case disconnected event arrives, clean up */
        goto disconnected;
    }

    switch (record_p->state) {
    case EXE_STATE_SETUP_DATA_CALL_CREATE:
        ATC_LOG_I("EXE_STATE_SETUP_DATA_CALL_CREATE");
        result = pscc_request_setup_data_call_scenario_0_1_step_2_3_receive_pscc_create_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_SETUP_DATA_CALL_SET;
        break;

    case EXE_STATE_SETUP_DATA_CALL_SET:
        ATC_LOG_I("EXE_STATE_SETUP_DATA_CALL_SET");
        result = pscc_request_setup_data_call_scenario_0_1_step_4_5_receive_pscc_set_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_SETUP_DATA_CALL_CONNECT;
        break;

    case EXE_STATE_SETUP_DATA_CALL_CONNECT:
        ATC_LOG_I("EXE_STATE_SETUP_DATA_CALL_CONNECT");

        result = pscc_request_setup_data_call_scenario_0_1_step_6_receive_pscc_connect_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_SETUP_DATA_CALL_CONNECTING;
        break;

    case EXE_STATE_SETUP_DATA_CALL_CONNECTING:
        ATC_LOG_I("EXE_STATE_SETUP_DATA_CALL_CONNECTING");

        result = EXE_PSCC_SUCCESS;
        record_p->state = EXE_STATE_SETUP_DATA_CALL_CONNECTED;
        break;

    case EXE_STATE_SETUP_DATA_CALL_CONNECTED: {
        ATC_LOG_I("EXE_STATE_SETUP_DATA_CALL_CONNECTED");

        result = pscc_request_setup_data_call_scenario_0_step_8_9_receive_pscc_connected_event(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_SETUP_DATA_CALL_GET;
        break;
    }
    case EXE_STATE_SETUP_DATA_CALL_GET:
        ATC_LOG_I("EXE_STATE_SETUP_DATA_CALL_GET");

        /* Handle the response to the get request. */
        result = pscc_request_setup_data_call_scenario_0_step_10_11_12_receive_pscc_get_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);

        /* Generate service reporting for connected PS account (see AT+CR) */
        exe_event(EXE_UNSOLICITED_CR, NULL);

        result = EXE_PSCC_SUCCESS_TERMINATE;
        break;

    case EXE_STATE_SETUP_DATA_CALL_DISCONNECTED:
disconnected:
        ATC_LOG_I("EXE_STATE_SETUP_DATA_CALL_DISCONNECTED");
        result = pscc_request_setup_data_call_scenario_1_step_10_11_receive_pscc_get_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_SETUP_DATA_CALL_DESTROY;
        break;

    case EXE_STATE_SETUP_DATA_CALL_DESTROY:
        ATC_LOG_I("EXE_STATE_SETUP_DATA_CALL_DESTROY");
        result = pscc_request_setup_data_call_scenario_1_step_12_13_14_receive_pscc_destroy_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        goto error;
    case EXE_STATE_SETUP_DATA_CALL_WAIT_RECONNECT:
        ATC_LOG_I("EXE_STATE_SETUP_DATA_CALL_WAIT_RECONNECT");

        /* Corner case, we were in state disconnecting while trying to activate the PDP context.
         * Wait for destroy response before attempting to reconnect. */
        if (NULL != client_data_p->pscc_event_p && pscc_event_disconnected == client_data_p->pscc_event_p->id) {
            result = pscc_request_setup_data_call_scenario_1_step_10_11_receive_pscc_get_response(record_p);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            record_p->state = EXE_STATE_SETUP_DATA_CALL_RECONNECT;
        }

        break;
    case EXE_STATE_SETUP_DATA_CALL_RECONNECT:
        ATC_LOG_I("EXE_STATE_SETUP_DATA_CALL_RECONNECT");
        result = pscc_request_setup_data_call_scenario_1_step_12_13_14_receive_pscc_destroy_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        exe_eppsd_t *request_data_p = (exe_eppsd_t *)record_p->request_data_p;

        if (NULL != request_data_p && EXE_PSCC_STATE_ON == request_data_p->state) {
            if (EXE_PSCC_SUCCESS != exe_pscc_start_pscc_interaction(record_p,
                    request_data_p->cid,
                    request_data_p->state,
                    NULL, NULL)) {
                result = EXE_PSCC_FAILURE;
                goto error;
            }

            record_p->state = EXE_STATE_SETUP_DATA_CALL_CREATE;
        } else if (NULL != client_data_p->cgact_p && EXE_PSCC_STATE_ON == client_data_p->cgact_p->state) {
            if (EXE_PSCC_SUCCESS != exe_pscc_start_pscc_interaction(record_p,
                    client_data_p->cgact_p->cid_p->cid,
                    client_data_p->cgact_p->state,
                    NULL, NULL)) {
                result = EXE_PSCC_FAILURE;
                goto error;
            }

            record_p->state = EXE_STATE_SETUP_DATA_CALL_CREATE;
        }

        break;

    default:
        ATC_LOG_E("Reached unknown state %d!", record_p->state);
        goto error;
    }

    return result;

error:

    if (EXE_PSCC_FAILURE_DESTROY == result) {

        /* Setup failed, destroy the connection. */
        result = exe_pscc_send_destroy_request(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_SETUP_DATA_CALL_DESTROY;
    }

    return result;
}


/**
 *  Handles deactivation of a data call.
 *  Shall be called from the request being executed with the current record.
 *  Does not initiate the setup as this requires the record to be in the state
 *  EXE_REQUEST and this is not always true.
 */
static exe_pscc_result_t exe_pscc_deactivate_data_call(exe_request_record_t *record_p)
{
    exe_pscc_result_t result = EXE_PSCC_FAILURE;
    pscc_msg_t *request_p = NULL;
    psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

    if (NULL != client_data_p->pscc_event_p && pscc_event_disconnected == client_data_p->pscc_event_p->id) {
        /* In case disconnected event arrives, clean up */
        goto disconnected;
    }

    switch (record_p->state) {
    case EXE_STATE_DEACTIVATE_DATA_CALL_CONNECTING:
        ATC_LOG_I("EXE_STATE_DEACTIVATE_DATA_CALL_CONNECTING");

        result = pscc_request_deactivate_data_call_scenario_0_step_2_receive_disconnect_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTING;
        break;
    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT:
        ATC_LOG_I("EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT");

        result = pscc_request_deactivate_data_call_scenario_0_step_2_receive_disconnect_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTING;
        break;

    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTING:
        /* No action need to be done, just update the state. */
        ATC_LOG_I("EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTING");

        result = EXE_PSCC_SUCCESS;
        record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTED;
        break;

    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTED:
disconnected:
        ATC_LOG_I("EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTED");
        result = pscc_request_deactivate_data_call_scenario_0_step_3_4_receive_disconnected_event(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DESTROY;
        break;

    case EXE_STATE_DEACTIVATE_DATA_CALL_DESTROY:
        ATC_LOG_I("EXE_STATE_DEACTIVATE_DATA_CALL_DESTROY");
        result = pscc_request_deactivate_data_call_scenario_0_step_5_6_7_receive_destroy_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        result = EXE_PSCC_SUCCESS_TERMINATE;
        break;

    default:
        ATC_LOG_E("Reached unknown state %d!", record_p->state);
        goto error;
    }

    /* Update last fail cause for deactivation */
    if (NULL != client_data_p->pscc_event_p && pscc_event_disconnected == client_data_p->pscc_event_p->id) {
        mpl_param_element_t *param_element_p  = mpl_param_list_find(pscc_paramid_cause, client_data_p->pscc_event_p->param_list_p);

        if (NULL != param_element_p && NULL != param_element_p->value_p) {
            client_data_p->fault_cause = *((int *)param_element_p->value_p);
            exe_pscc_send_fail_cause_event((int32_t)client_data_p->fault_cause);
        }
    }

    return result;

error:
    return EXE_PSCC_FAILURE;
}

#ifndef EXE_USE_ATC_CUSTOMER_EXTENSIONS
exe_request_result_t pscc_request_send_pcd_event(int cid, int pcd_status)
{
    return EXE_SUCCESS;
}
#endif


/**
 * psccclient_add_qos_2G_item_to_request_list
 *
 *  Description:
 *  Related to +CGQMIN? and +CGQREQ? (read 2G qos data)
 *  Copy qos parameters from record_p to request data. qos_p must not be NULL.
 */
static bool psccclient_add_qos_2G_item_to_request_list(exe_request_record_t *record_p, exe_pdp_qos_2G_t *qos_p)
{
    exe_pdp_qos_2G_response_t *request_data_p = (exe_pdp_qos_2G_response_t *) record_p->request_data_p;
    exe_pdp_qos_2G_response_t *new_item_p = NULL;

    new_item_p = (exe_pdp_qos_2G_response_t *) calloc(1,
                 sizeof(exe_pdp_qos_2G_response_t));

    if (NULL == new_item_p) {
        ATC_LOG_E("new_item_p is NULL. Failed to allocate memory!!!");
        /* If this calloc fails, there is no other memory to deallocate so return is safe. */
        return false;
    }

    new_item_p->pdp_qos.cid = qos_p->cid;
    new_item_p->pdp_qos.precedence = qos_p->precedence;
    new_item_p->pdp_qos.delay = qos_p->delay;
    new_item_p->pdp_qos.reliability = qos_p->reliability;
    new_item_p->pdp_qos.peak = qos_p->peak;
    new_item_p->pdp_qos.mean = qos_p->mean;

    if (NULL == record_p->request_data_p) {
        record_p->request_data_p = new_item_p;
    } else {
        while (NULL != request_data_p->next_p) {
            request_data_p = request_data_p->next_p;
        }

        request_data_p->next_p = new_item_p;
    }

    return true;
}
/**
 *  psccclient_add_qos_item_to_request_list
 *
 *  Description:
 *  Related to +CGEQMIN? and +CGEQREQ? (reads)
 *  Copy qos parameters from record_p to request data. qos_p must not be NULL.
 */
static bool psccclient_add_qos_item_to_request_list(exe_request_record_t *record_p, exe_pdp_qos_t *qos_p)
{
    exe_pdp_qos_response_t *request_data_p = (exe_pdp_qos_response_t *) record_p->request_data_p;
    exe_pdp_qos_response_t *new_item_p = NULL;

    new_item_p = (exe_pdp_qos_response_t *) calloc(1,
                 sizeof(exe_pdp_qos_response_t));

    if (NULL == new_item_p) {
        ATC_LOG_E("new_item_p is NULL. Failed to allocate memory!!!");
        goto error;
    }

    new_item_p->pdp_qos.cid = qos_p->cid;
    new_item_p->pdp_qos.traffic_class = qos_p->traffic_class;
    new_item_p->pdp_qos.max_bitrate_ul = qos_p->max_bitrate_ul;
    new_item_p->pdp_qos.max_bitrate_dl = qos_p->max_bitrate_dl;
    new_item_p->pdp_qos.guaranteed_bitrate_ul = qos_p->guaranteed_bitrate_ul;
    new_item_p->pdp_qos.guaranteed_bitrate_dl = qos_p->guaranteed_bitrate_dl;
    new_item_p->pdp_qos.delivery_order = qos_p->delivery_order;
    new_item_p->pdp_qos.max_sdu_size = qos_p->max_sdu_size;

    if (NULL != qos_p->sdu_error_ratio_p) {
        new_item_p->pdp_qos.sdu_error_ratio_p = (char *) calloc(1, 3 + 1);

        if (NULL == new_item_p->pdp_qos.sdu_error_ratio_p) {
            ATC_LOG_E("new_item_p->sdu_error_ratio_p is NULL. Failed to allocate memory!!!");
            goto error;
        } else {
            strncpy(new_item_p->pdp_qos.sdu_error_ratio_p,
                    qos_p->sdu_error_ratio_p, 3);
        }
    }

    if (NULL != qos_p->residual_ber_p) {
        new_item_p->pdp_qos.residual_ber_p = (char *) calloc(1, 3 + 1);

        if (NULL == new_item_p->pdp_qos.residual_ber_p) {
            ATC_LOG_E("new_item_p->residual_ber_p is NULL. Failed to allocate memory!!!");
            goto error;
        } else {
            strncpy(new_item_p->pdp_qos.residual_ber_p, qos_p->residual_ber_p,
                    3);
        }
    }

    new_item_p->pdp_qos.delivery_err_sdu = qos_p->delivery_err_sdu;
    new_item_p->pdp_qos.transfer_delay = qos_p->transfer_delay;
    new_item_p->pdp_qos.traffic_handling_priority = qos_p->traffic_handling_priority;
    new_item_p->pdp_qos.source_statistics_descriptor = qos_p->source_statistics_descriptor;
    new_item_p->pdp_qos.signalling_indication  = qos_p->signalling_indication;

    if (NULL == record_p->request_data_p) {
        record_p->request_data_p = new_item_p;
    } else {
        while (NULL != request_data_p->next_p) {
            request_data_p = request_data_p->next_p;
        }

        request_data_p->next_p = new_item_p;
    }

    return true;

error:

    if (NULL != new_item_p) {
        if (NULL != new_item_p->pdp_qos.sdu_error_ratio_p) {
            free(new_item_p->pdp_qos.sdu_error_ratio_p);
        }

        if (NULL != new_item_p->pdp_qos.residual_ber_p) {
            free(new_item_p->pdp_qos.residual_ber_p);
        }

        free(new_item_p);
    }

    return false;
}

/********************************************************************
 * Public methods
 ********************************************************************
 */

exe_request_result_t pscc_request_send_cgev_event(
    int                   cid,
    char                 *pdp_addr_p,
    int                   pdp_type,
    exe_cgev_response_type_t response_type)
{
    exe_cgev_response_t *cgev_response_data_p = (exe_cgev_response_t *)calloc(1, sizeof(exe_cgev_response_t));

    if (NULL == cgev_response_data_p) {
        ATC_LOG_E("cgev_response_data_p is NULL. No unsolicited event response possible.");
        goto exit;
    }

    cgev_response_data_p->cid = cid;
    cgev_response_data_p->pdp_addr_p = pdp_addr_p;
    cgev_response_data_p->pdp_type = pdp_type;
    cgev_response_data_p->response_type = response_type;
    ATC_LOG_I("Issueing EXE_UNSOLICITED_CGEV cid=%d response_type=%d",  cgev_response_data_p->cid, cgev_response_data_p->response_type);
    exe_event(EXE_UNSOLICITED_CGEV, cgev_response_data_p);

    free(cgev_response_data_p);
    return EXE_SUCCESS;

exit:
    return EXE_FAILURE;
}


exe_request_result_t request_set_GPRS_state(exe_request_record_t *record_p)
{
    pscc_msg_t *request_p = NULL;

    /* Check if an ongoing execution shall be aborted. */
    if (record_p->abort && EXE_STATE_REQUEST != record_p->state) {
        free(record_p->client_data_p);
        /* TODO Do we need to do some more? */
        return EXE_SUCCESS;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("EXE_STATE_REQUEST");
        exe_cgatt_gprs_state_t gprs_state;

        EXE_CHECK_GOTO_ERROR(NULL != record_p->request_data_p);
        request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);
        EXE_CHECK_GOTO_ERROR(NULL != request_p);

        gprs_state = ((exe_cgatt_request_data_t *)record_p->request_data_p)->state;

        request_p->req_attach_status.id = (EXE_CGATT_ATTACH == gprs_state) ? pscc_attach : pscc_detach;

        EXE_CHECK_GOTO_ERROR(create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_UNDEFINED, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS));
        EXE_CHECK_GOTO_ERROR(psccclient_send_request(record_p, request_p)) ;

        mpl_msg_free((mpl_msg_t *)request_p);
        record_p->state = (EXE_CGATT_ATTACH == gprs_state) ? EXE_STATE_GPRS_ATTACH : EXE_STATE_GPRS_DETACH;

        return EXE_PENDING;
    }

    /* REPONSE PART */
    case EXE_STATE_GPRS_ATTACH: {
        /* Request response */
        ATC_LOG_I("EXE_STATE_GPRS_ATTACH");
        mpl_msg_resp_t *response_p = psccclient_get_pscc_response_p(record_p);
        EXE_CHECK_GOTO_ERROR(NULL != response_p);

        if (pscc_result_failed_operation_not_allowed == response_p->result) {
            /* GPRS already attached, return SUCCESS. */
            goto exit;
        }

        EXE_CHECK_GOTO_ERROR(response_p->result == pscc_result_ok);
        record_p->state = EXE_STATE_GPRS_ATTACHING;
        break;
    }

    case EXE_STATE_GPRS_ATTACHING: {
        /* Resulting event */
        ATC_LOG_I("EXE_STATE_GPRS_ATTACHING");
        psccclient_record_data_t *client_data_p   = (psccclient_record_data_t *)record_p->client_data_p;

        /* Check if the request was successfully executed. */
        EXE_CHECK_GOTO_ERROR(pscc_event_attached == client_data_p->pscc_event_p->id);
        goto exit;
        break;
    }

    case EXE_STATE_GPRS_DETACH: {
        /* Request response */
        ATC_LOG_I("EXE_STATE_GPRS_DETACH");
        mpl_msg_resp_t *response_p = psccclient_get_pscc_response_p(record_p);
        EXE_CHECK_GOTO_ERROR(NULL != response_p);

        if (pscc_result_failed_operation_not_allowed == response_p->result) {
            /* GPRS already detached, return SUCCESS. */
            goto exit;
        }

        EXE_CHECK_GOTO_ERROR(response_p->result == pscc_result_ok);
        record_p->state = EXE_STATE_GPRS_DETACHING;
        break;
    }

    case EXE_STATE_GPRS_DETACHING: {
        /* Resulting event */
        ATC_LOG_I("EXE_STATE_GPRS_DETACHING");
        psccclient_record_data_t *client_data_p   = (psccclient_record_data_t *)record_p->client_data_p;

        /* Check if the request was successfully executed. */
        EXE_CHECK_GOTO_ERROR(pscc_event_detached == client_data_p->pscc_event_p->id);

        /* Update last fail cause for detach */
        mpl_param_element_t *param_element_p  = mpl_param_list_find(pscc_paramid_cause, client_data_p->pscc_event_p->param_list_p);

        if (NULL != param_element_p && NULL != param_element_p->value_p) {
            client_data_p->fault_cause = *((int *)param_element_p->value_p);
            exe_pscc_send_fail_cause_event((int32_t)client_data_p->fault_cause);
        }

        goto exit;
        break;
    }

    default:
        goto error;
    }

    return EXE_SUCCESS;

exit:
    free(record_p->client_data_p);
    exe_request_complete(record_p, EXE_SUCCESS, NULL);
    return EXE_SUCCESS;

error:
    mpl_msg_free((mpl_msg_t *)request_p);
    free(record_p->client_data_p);

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t reguest_get_GPRS_state(exe_request_record_t *record_p)
{
    pscc_msg_t *request_p = NULL;

    /* Check if an ongoing execution shall be aborted. */
    if (record_p->abort && EXE_STATE_REQUEST != record_p->state) {
        free(record_p->client_data_p);
        /* TODO Do we need to do some more? */
        return EXE_SUCCESS;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("EXE_STATE_REQUEST");

        request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);
        EXE_CHECK_GOTO_ERROR(NULL != request_p);
        request_p->req_attach_status.id = pscc_attach_status;

        EXE_CHECK_GOTO_ERROR(create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_UNDEFINED, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS));
        EXE_CHECK_GOTO_ERROR(psccclient_send_request(record_p, request_p)) ;

        mpl_msg_free((mpl_msg_t *)request_p);
        record_p->state = EXE_STATE_GPRS_GET_STATE;

        return EXE_PENDING;
    }

    case EXE_STATE_GPRS_GET_STATE: {
        ATC_LOG_I("EXE_STATE_GPRS_GET_STATE");
        mpl_msg_resp_t *response_p = psccclient_get_pscc_response_p(record_p);
        EXE_CHECK_GOTO_ERROR(NULL != response_p);
        EXE_CHECK_GOTO_ERROR(response_p->result == pscc_result_ok);

        exe_cgatt_response_data_t request_response;
        exe_request_result_t result;
        mpl_param_element_t *param_status_p;
        psccclient_record_data_t *client_data_p;

        param_status_p = mpl_param_list_find(pscc_paramid_attach_status, response_p->param_list_p);
        client_data_p = (psccclient_record_data_t *) request_record_get_client_data(record_p);
        EXE_CHECK_GOTO_ERROR(NULL != client_data_p);

        client_data_p->gprs_status = *((int *) param_status_p->value_p);

        switch ((client_data_p)->gprs_status) {
        case pscc_attach_status_attached:
            request_response.state = 1;
            break;
        case pscc_attach_status_detached:
            request_response.state = 0;
            break;
        default:
            ATC_LOG_E("illegal gprs status");
            goto error;
        }

        free(record_p->client_data_p);

        exe_request_complete(record_p, EXE_SUCCESS, &request_response);

        return EXE_SUCCESS;
    }

    default:
        goto error;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_FAILURE;
}

/**
 * request_get_PDP_required_qos -- AT+CGEQREQ
 *
 *  Description from spec:
 * See request_set_PDP_required_qos
 */

exe_request_result_t request_get_PDP_required_qos(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else

    int cids_length = 0;
    exe_request_result_t result               = EXE_SUCCESS;
    /* Method returns all iadb entries save for those listed in the first parameter - NULL in this case => all */
    int *cids_p = psccclient_iadb_list_cids_without_connection_ids(NULL, 0, &cids_length);

    if (NULL != cids_p) {
        int i = 0;

        for (i = 0; i < cids_length; i++) {
            exe_pdp_qos_t *qos_p = calloc(1, sizeof(exe_pdp_qos_t));

            if (NULL != qos_p) {
                qos_p->cid = cids_p[i];
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_traffic_class, &qos_p->traffic_class);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_max_bitrate_ul, &qos_p->max_bitrate_ul);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_max_bitrate_dl, &qos_p->max_bitrate_dl);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_guaranteed_bitrate_ul, &qos_p->guaranteed_bitrate_ul);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_guaranteed_bitrate_dl, &qos_p->guaranteed_bitrate_dl);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_delivery_order, &qos_p->delivery_order);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_max_sdu_size, &qos_p->max_sdu_size);

                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_sdu_error_ratio_p, &qos_p->sdu_error_ratio_p);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_residual_ber_p, &qos_p->residual_ber_p);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_delivery_err_sdu, &qos_p->delivery_err_sdu);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_transfer_delay, &qos_p->transfer_delay);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_traffic_handling_priority, &qos_p->traffic_handling_priority);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_source_statistics_descriptor, &qos_p->source_statistics_descriptor);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_signalling_indication, &qos_p->signalling_indication);

                if (!psccclient_add_qos_item_to_request_list(record_p, qos_p)) {
                    result = EXE_FAILURE;
                }

                free(qos_p);
            } else {
                ATC_LOG_E("memory allocation error.");
                result = EXE_FAILURE;
            }
        }

        free(cids_p);
    }

    return result;
#endif
}

exe_request_result_t request_set_PDP_negotiated_qos(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    exe_cgpaddr_cid_t *request_data_p = NULL;
    exe_pscc_result_t result;
    pscc_msg_t *request_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST:
        ATC_LOG_I("EXE_STATE_REQUEST");
        request_data_p = (exe_cgpaddr_cid_t *)record_p->request_data_p;
        EXE_CHECK_GOTO_ERROR(NULL != request_data_p);
        psccclient_record_data_t *client_data_p = NULL;

        request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);
        EXE_CHECK_GOTO_ERROR(NULL != request_p);

        request_p->req_list_all_connections.id = pscc_list_all_connections;

        EXE_CHECK_GOTO_ERROR(create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_DATA_LIST_NEG_QOS, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS));
        client_data_p = record_p->client_data_p;

        while (NULL != request_data_p) {
            EXE_CHECK_GOTO_ERROR(add_cgpaddr_cid_to_list(request_data_p->cid, &client_data_p->cgpaddr_p));
            request_data_p = request_data_p->next_p;
        }

        EXE_CHECK_GOTO_ERROR(psccclient_send_request(record_p, request_p)) ;

        mpl_msg_free((mpl_msg_t *)request_p);
        record_p->state = EXE_STATE_LIST_CONNECTIONS_REQUEST;
        return EXE_PENDING;

    case EXE_STATE_LIST_CONNECTIONS_REQUEST:
        ATC_LOG_I("EXE_STATE_LIST_CONNECTIONS_REQUEST");
        result = pscc_data_call_list_step_2_3_receive_pscc_list_all_connections_response(record_p);

        if (EXE_PSCC_SUCCESS_TERMINATE == result) {
            goto exit;
        } else {
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            record_p->state = EXE_STATE_LIST_CONNECTIONS_GET;
        }

        break;

    case EXE_STATE_LIST_CONNECTIONS_GET: {
        ATC_LOG_I("EXE_STATE_LIST_CONNECTIONS_GET");
        int current_index  = 0;
        result = pscc_data_call_list_step_4_5_receive_pscc_get_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);

        /* Have all connections been handled in the response array? */
        if (psccclient_increment_and_get_current_data_list_index(record_p, &current_index)) {
            goto exit;
        } else {
            result = exe_pscc_list_all_connections_get(record_p, current_index);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        }

        break;


    }
    default:
        ATC_LOG_E("unknown state");
        goto error;
        break;
    }

    return exe_pscc_set_request_result(result);

exit: {
        exe_pdp_qos_response_t *response_data_p = (exe_pdp_qos_response_t *)record_p->response_data_p;
        psccclient_record_data_t *client_data_p   = (psccclient_record_data_t *)record_p->client_data_p;

        /* cleanup */
        if (NULL != client_data_p) {
            free_cgpaddr_cid_list(client_data_p->cgpaddr_p);
            free(client_data_p->connection_ids_data_list_p);
            free(record_p->client_data_p);
        }

        exe_request_complete(record_p, exe_pscc_set_request_result(result), response_data_p);

        /* cleanup response specific memory*/
        exe_pdp_qos_response_t *tmp_p;

        while (NULL != response_data_p) {
            tmp_p = response_data_p->next_p;

            free(response_data_p->pdp_qos.sdu_error_ratio_p);
            free(response_data_p->pdp_qos.residual_ber_p);
            free(response_data_p);

            response_data_p = tmp_p;
        }
    }
    return exe_pscc_set_request_result(result);


error:

    if (NULL != record_p->client_data_p) {
        free_cgpaddr_cid_list(((psccclient_record_data_t *)record_p->client_data_p)->cgpaddr_p);
        free(((psccclient_record_data_t *) record_p->client_data_p)->connection_ids_data_list_p);
    }

    free(record_p->client_data_p);

    mpl_msg_free((mpl_msg_t *)request_p);


    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;

#endif
}


/**
 * request_get_PDP_address
 *
 *  Description from spec:
 *  Gets the PDP address for each provided cid in record_p->request_data_p.
 *  If record_p->request_data_p is NULL, PDP address for all defined cids are returned.
 *  Test method lists all defined cids.
 */
exe_request_result_t request_get_PDP_address(exe_request_record_t *record_p)
{
    exe_cgpaddr_cid_t *request_data_p = NULL;
    exe_pscc_result_t result = EXE_PSCC_FAILURE;
    pscc_msg_t *request_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST:
        ATC_LOG_I("EXE_STATE_REQUEST");
        request_data_p = (exe_cgpaddr_cid_t *)record_p->request_data_p;
        psccclient_record_data_t *client_data_p = NULL;

        EXE_CHECK_GOTO_ERROR(create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_DATA_LIST_PDP_ADDRESS, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS));

        client_data_p = record_p->client_data_p;

        /*if request_data_p equals NULL, we should list address for all defined cids */
        while (NULL != request_data_p) {
            EXE_CHECK_GOTO_ERROR(add_cgpaddr_cid_to_list(request_data_p->cid, &client_data_p->cgpaddr_p));
            request_data_p = request_data_p->next_p;
        }

        request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);
        EXE_CHECK_GOTO_ERROR(NULL != request_p);

        request_p->req_list_all_connections.id = pscc_list_all_connections;

        EXE_CHECK_GOTO_ERROR(psccclient_send_request(record_p, request_p)) ;

        mpl_msg_free((mpl_msg_t *)request_p);
        record_p->state = EXE_STATE_LIST_CONNECTIONS_REQUEST;
        return EXE_PENDING;

    case EXE_STATE_LIST_CONNECTIONS_REQUEST:
        ATC_LOG_I("EXE_STATE_LIST_CONNECTIONS_REQUEST");
        result = pscc_data_call_list_step_2_3_receive_pscc_list_all_connections_response(record_p);

        if (EXE_PSCC_SUCCESS_TERMINATE == result) {
            goto exit;
        } else {
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            record_p->state = EXE_STATE_LIST_CONNECTIONS_GET;
        }

        break;

    case EXE_STATE_LIST_CONNECTIONS_GET: {
        ATC_LOG_I("EXE_STATE_LIST_CONNECTIONS_GET");
        int current_index  = 0;
        result = pscc_data_call_list_step_4_5_receive_pscc_get_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);

        /* Have all connections been handled in the response array? */
        if (psccclient_increment_and_get_current_data_list_index(record_p, &current_index)) {
            result = exe_pscc_list_all_connections_add_from_iadb(record_p);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            goto exit;
        } else {
            result = exe_pscc_list_all_connections_get(record_p, current_index);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        }

        break;
    }

    default:
        goto error;
    }

    return EXE_SUCCESS;

exit: {
        psccclient_record_data_t *client_data_p = (psccclient_record_data_t *) record_p->client_data_p;

        if (NULL != client_data_p) {
            free_cgpaddr_cid_list(client_data_p->cgpaddr_p);
            free(client_data_p->connection_ids_data_list_p);
            free(record_p->client_data_p);
        }

        exe_cgpaddr_response_t *response_data_p = (exe_cgpaddr_response_t *) record_p->response_data_p;
        exe_request_complete(record_p, exe_pscc_set_request_result(result), response_data_p);

        /* free response specific data */
        exe_cgpaddr_response_t *tmp_p = NULL;

        while (NULL != response_data_p) {
            tmp_p = response_data_p->next_p;
            free(response_data_p->pdp_address_p);
            free(response_data_p->ipv6_pdp_address_p);
            free(response_data_p);
            response_data_p = tmp_p;
        }

        return exe_pscc_set_request_result(result);
    }

error:

    if (NULL != record_p->client_data_p) {
        free_cgpaddr_cid_list(((psccclient_record_data_t *) record_p->client_data_p)->cgpaddr_p);
        free(((psccclient_record_data_t *) record_p->client_data_p)->connection_ids_data_list_p);
        free(record_p->client_data_p);
    }

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }
    mpl_msg_free((mpl_msg_t *)request_p);

    return EXE_FAILURE;
}

/**
 * request_get_activation_stats -- AT+CGACT?
 *
 * requests activation status for all available connections
 */
exe_request_result_t request_get_activation_stats(exe_request_record_t *record_p)
{
    void *request_data_p = NULL;

#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    exe_pscc_result_t result;
    pscc_msg_t *request_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST:
        ATC_LOG_I("EXE_STATE_REQUEST");
        ATC_LOG_I("-> request");

        request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);
        EXE_CHECK_GOTO_ERROR(NULL != request_p);

        request_p->req_list_all_connections.id = pscc_list_all_connections;

        EXE_CHECK_GOTO_ERROR(create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_DATA_LIST_CGACT, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS));
        EXE_CHECK_GOTO_ERROR(psccclient_send_request(record_p, request_p)) ;

        mpl_msg_free((mpl_msg_t *)request_p);
        record_p->state = EXE_STATE_LIST_CONNECTIONS_REQUEST;
        return EXE_PENDING;

    case EXE_STATE_LIST_CONNECTIONS_REQUEST:
        ATC_LOG_I("EXE_STATE_LIST_CONNECTIONS_REQUEST");
        result = pscc_data_call_list_step_2_3_receive_pscc_list_all_connections_response(record_p);

        if (EXE_PSCC_SUCCESS_TERMINATE == result) {
            goto exit;
        } else {
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            record_p->state = EXE_STATE_LIST_CONNECTIONS_GET;
        }

        break;

    case EXE_STATE_LIST_CONNECTIONS_GET: {
        ATC_LOG_I("EXE_STATE_LIST_CONNECTIONS_GET");
        int current_index  = 0;
        result = pscc_data_call_list_step_4_5_receive_pscc_get_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);

        /* Have all connections been handled in the response array? */
        if (psccclient_increment_and_get_current_data_list_index(record_p, &current_index)) {
            result = exe_pscc_list_all_connections_add_from_iadb(record_p);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            goto exit;
        } else {
            result = exe_pscc_list_all_connections_get(record_p, current_index);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        }

        break;
    }

    default:
        goto error;
    }

    return exe_pscc_set_request_result(result);

exit: {
        psccclient_record_data_t    *client_data_p   = (psccclient_record_data_t *)record_p->client_data_p;
        exe_cgact_read_response_t *response_data_p = (exe_cgact_read_response_t *)record_p->response_data_p;

        ATC_LOG_I("<- response");


        if (NULL != client_data_p) {

            if (NULL != client_data_p->connection_ids_data_list_p) {
                free(client_data_p->connection_ids_data_list_p);
            }

            free(record_p->client_data_p);
        }

        exe_request_complete(record_p, EXE_SUCCESS, record_p->response_data_p);

        while (NULL != response_data_p) {
            exe_cgact_read_response_t *next_p = response_data_p->next_p;
            free(response_data_p);
            response_data_p = next_p;
        }

        return EXE_SUCCESS;
    }

error: {
        /* Clean up. */
        psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

        if (NULL != client_data_p) {
            if (NULL != client_data_p->connection_ids_data_list_p) {
                free(client_data_p->connection_ids_data_list_p);
            }

            free(record_p->client_data_p);
        }

        mpl_msg_free((mpl_msg_t *)request_p);

        /* TODO free response data? */

        if (EXE_STATE_REQUEST != record_p->state) {
            exe_request_complete(record_p, EXE_FAILURE, NULL);
        }

        return EXE_FAILURE;
    }
#endif
}

exe_request_result_t request_set_PDP_minimum_qos_2G(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    bool iadb_results = true;
    exe_pdp_qos_2G_t *request_data_p = NULL;

    ATC_LOG_I("-> request");

    request_data_p = (exe_pdp_qos_2G_t *)record_p->request_data_p;

    if (NULL == request_data_p) {
        ATC_LOG_E("request_data is NULL. Failed.");
        goto error;
    }

    if (0 == request_data_p->cid) {
        ATC_LOG_E("Wrong cid = 0");
        goto error;
    } else {
        if (!psccclient_iadb_get_set_flags_from_cid(request_data_p->cid)) {
            /* creation of account with CGDCONT has not been made for this cid */
            ATC_LOG_E("No account defined for the cid:%d", request_data_p->cid);
            goto error;
        }
    }

    /* Set flag to show that rel97 parameters will be used if a pdp activation is requested. */
    int iadb_use_rel97 = true;
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_use_rel_97_qos,   &(iadb_use_rel97));

    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_precedence, &(request_data_p->precedence));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_delay, &(request_data_p->delay));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_reliability, &(request_data_p->reliability));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_peak, &(request_data_p->peak));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_mean, &(request_data_p->mean));

    if (!iadb_results) {
        ATC_LOG_E("Failed to set some of the parameters in iadb. Failed.");
        goto error;
    }

    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
#endif
}


exe_request_result_t request_get_PDP_minimum_qos_2G(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    int cids_length = 0;
    exe_request_result_t result               = EXE_SUCCESS;

    /* Method returns all iadb entries save for those listed in the first parameter - NULL in this case => all */
    int *cids_p = psccclient_iadb_list_cids_without_connection_ids(NULL, 0, &cids_length);

    if (NULL != cids_p) {
        int i = 0;

        for (i = 0; i < cids_length; i++) {
            exe_pdp_qos_2G_t *qos_p = calloc(1, sizeof(exe_pdp_qos_2G_t));

            if (NULL != qos_p) {
                qos_p->cid = cids_p[i];
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_precedence, &qos_p->precedence);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_delay, &qos_p->delay);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_reliability, &qos_p->reliability);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_peak, &qos_p->peak);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_mean, &qos_p->mean);


                if (!psccclient_add_qos_2G_item_to_request_list(record_p, qos_p)) {
                    result = EXE_FAILURE;
                }

                free(qos_p);
            } else {
                ATC_LOG_E("memory allocation error.");
                result = EXE_FAILURE;
            }
        }

        free(cids_p);
    }

    return result;
#endif
}
exe_request_result_t request_set_PDP_required_qos_2G(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    bool iadb_results = true;
    exe_pdp_qos_2G_t *request_data_p = NULL;

    ATC_LOG_I("-> request");

    request_data_p = (exe_pdp_qos_2G_t *)record_p->request_data_p;

    if (NULL == request_data_p) {
        ATC_LOG_E("request_data is NULL. Failed.");
        goto error;
    }

    if (0 == request_data_p->cid) {
        ATC_LOG_E("Wrong cid = 0");
        goto error;
    } else {
        if (!psccclient_iadb_get_set_flags_from_cid(request_data_p->cid)) {
            /* creation of account with CGDCONT has not been made for this cid */
            ATC_LOG_E("No account defined for the cid:%d", request_data_p->cid);
            goto error;
        }
    }

    /* Set flag to show that rel97 parameters will be used if a pdp activation is requested. */
    int iadb_use_rel97 = true;
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_use_rel_97_qos,   &(iadb_use_rel97));

    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_precedence, &(request_data_p->precedence));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_delay, &(request_data_p->delay));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_reliability, &(request_data_p->reliability));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_peak, &(request_data_p->peak));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_mean, &(request_data_p->mean));

    if (!iadb_results) {
        ATC_LOG_E("Failed to set some of the parameters in iadb. Failed.");
        goto error;
    }

    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
#endif
}

exe_request_result_t request_get_PDP_required_qos_2G(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else

    int cids_length = 0;
    exe_request_result_t result               = EXE_SUCCESS;

    /* Method returns all iadb entries save for those listed in the first parameter - NULL in this case => all */
    int *cids_p = psccclient_iadb_list_cids_without_connection_ids(NULL, 0, &cids_length);

    if (NULL != cids_p) {
        int i = 0;

        for (i = 0; i < cids_length; i++) {
            exe_pdp_qos_2G_t *qos_p = calloc(1, sizeof(exe_pdp_qos_2G_t));

            if (NULL != qos_p) {
                qos_p->cid = cids_p[i];
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_precedence, &qos_p->precedence);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_delay, &qos_p->delay);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_reliability, &qos_p->reliability);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_peak, &qos_p->peak);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_req_mean, &qos_p->mean);

                if (!psccclient_add_qos_2G_item_to_request_list(record_p, qos_p)) {
                    result = EXE_FAILURE;
                }

                free(qos_p);
            } else {
                ATC_LOG_E("memory allocation error.");
                result = EXE_FAILURE;
            }
        }

        free(cids_p);
    }

    return result;
#endif
}

/**
 * request_get_PDP_minimum_qos -- AT+CGEQMIN
 *
 *  Description from spec:
 * See request_set_PDP_minimum_qos
 */

exe_request_result_t request_get_PDP_minimum_qos(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else

    int cids_length = 0;
    exe_request_result_t result               = EXE_SUCCESS;

    /* Method returns all iadb entries save for those listed in the first parameter - NULL in this case => all */
    int *cids_p = psccclient_iadb_list_cids_without_connection_ids(NULL, 0, &cids_length);

    if (NULL != cids_p) {
        int i = 0;

        for (i = 0; i < cids_length; i++) {
            exe_pdp_qos_t *qos_p = calloc(1, sizeof(exe_pdp_qos_t));

            if (NULL != qos_p) {
                qos_p->cid = cids_p[i];
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_traffic_class, &qos_p->traffic_class);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_max_bitrate_ul, &qos_p->max_bitrate_ul);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_max_bitrate_dl, &qos_p->max_bitrate_dl);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_guaranteed_bitrate_ul, &qos_p->guaranteed_bitrate_ul);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_guaranteed_bitrate_dl, &qos_p->guaranteed_bitrate_dl);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_delivery_order, &qos_p->delivery_order);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_max_sdu_size, &qos_p->max_sdu_size);

                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_sdu_error_ratio_p, &qos_p->sdu_error_ratio_p);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_residual_ber_p, &qos_p->residual_ber_p);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_delivery_err_sdu, &qos_p->delivery_err_sdu);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_transfer_delay, &qos_p->transfer_delay);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_traffic_handling_priority, &qos_p->traffic_handling_priority);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_source_statistics_descriptor, &qos_p->source_statistics_descriptor);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_min_signalling_indication, &qos_p->signalling_indication);

                if (!psccclient_add_qos_item_to_request_list(record_p, qos_p)) {
                    result = EXE_FAILURE;
                }

                free(qos_p);
            } else {
                ATC_LOG_E("memory allocation error.");
                result = EXE_FAILURE;
            }
        }

        free(cids_p);
    }


    return result;

#endif
}

/**
 * request_get_PDP_context  --  AT+CGDCONT?
 *
 * get PDP context data for all available connections
 *
 */
exe_request_result_t request_get_PDP_context(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    exe_pscc_result_t result;
    pscc_msg_t *request_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("EXE_STATE_REQUEST");

        request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);
        EXE_CHECK_GOTO_ERROR(NULL != request_p);

        request_p->req_list_all_connections.id = pscc_list_all_connections;

        EXE_CHECK_GOTO_ERROR(create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_DATA_LIST_CGDCONT, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS));
        EXE_CHECK_GOTO_ERROR(psccclient_send_request(record_p, request_p)) ;

        mpl_msg_free((mpl_msg_t *)request_p);
        record_p->state = EXE_STATE_LIST_CONNECTIONS_REQUEST;
        return EXE_PENDING;
    }

    case EXE_STATE_LIST_CONNECTIONS_REQUEST:
        ATC_LOG_I("EXE_STATE_LIST_CONNECTIONS_REQUEST");
        result = pscc_data_call_list_step_2_3_receive_pscc_list_all_connections_response(record_p);

        if (EXE_PSCC_SUCCESS_TERMINATE == result) {
            goto exit;
        } else {
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            record_p->state = EXE_STATE_LIST_CONNECTIONS_GET;
        }

        break;

    case EXE_STATE_LIST_CONNECTIONS_GET: {
        ATC_LOG_I("EXE_STATE_LIST_CONNECTIONS_GET");
        int current_index  = 0;
        result = pscc_data_call_list_step_4_5_receive_pscc_get_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);

        /* Have all connections been handled in the response array? */
        if (psccclient_increment_and_get_current_data_list_index(record_p, &current_index)) {
            result = exe_pscc_list_all_connections_add_from_iadb(record_p);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            goto exit;
        } else {
            result = exe_pscc_list_all_connections_get(record_p, current_index);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        }

        break;
    }

    default:
        goto error;
    }

    return exe_pscc_set_request_result(result);

exit: {
        exe_cgdcont_response_t *response_data_p = (exe_cgdcont_response_t *)record_p->response_data_p;

        ATC_LOG_I("<- response");

        if (NULL == response_data_p) {
            ATC_LOG_I("response_data_p is NULL. No PDP contexts found.");
        }

        if (NULL != record_p->client_data_p) {
            psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

            if (NULL != client_data_p->connection_ids_data_list_p) {
                free(client_data_p->connection_ids_data_list_p);
            }

            free(record_p->client_data_p);
        }

        exe_request_complete(record_p, EXE_SUCCESS, response_data_p);

        exe_cgdcont_response_t *tmp_p;

        while (response_data_p != NULL) {
            tmp_p = response_data_p->next_p;

            if (response_data_p->cgdcont_p != NULL) {
                if (response_data_p->cgdcont_p->apn_p != NULL) {
                    free(response_data_p->cgdcont_p->apn_p);
                }

                if (response_data_p->cgdcont_p->pdp_addr_p != NULL) {
                    free(response_data_p->cgdcont_p->pdp_addr_p);
                }

                if (response_data_p->cgdcont_p->ipv6_pdp_addr_p != NULL) {
                    free(response_data_p->cgdcont_p->ipv6_pdp_addr_p);
                }

                free(response_data_p->cgdcont_p);
            }

            free(response_data_p);
            response_data_p = tmp_p;
        }

        return EXE_SUCCESS;
    }

error: {
        /* Clean up. */
        psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

        mpl_msg_free((mpl_msg_t *)request_p);

        if (NULL != client_data_p) {
            if (NULL != client_data_p->connection_ids_data_list_p) {
                free(client_data_p->connection_ids_data_list_p);
            }

            free(record_p->client_data_p);
        }

        /* TODO free response data? */

        if (EXE_STATE_REQUEST != record_p->state) {
            exe_request_complete(record_p, EXE_FAILURE, NULL);
        }

        return EXE_FAILURE;
    }
#endif
}

/**
 * request_set_PDP_modify  --  AT+CGCMOD
 *
 * This function issues a QoS (quality of service) modification procedure for a PDP context.
 * Note this function will only affect active PDP context(s).
 *
 */
exe_request_result_t request_set_PDP_modify(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    exe_cgcmod_set_request_t     *request_data_p = NULL;
    exe_cgcmod_set_request_cid_t *tmp_p          = NULL;
    psccclient_record_data_t     *client_data_p  = NULL;
    bool                          iadb_results   = true;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("EXE_STATE_REQUEST");
        request_data_p = (exe_cgcmod_set_request_t *) record_p->request_data_p;
        EXE_CHECK_GOTO_ERROR(NULL != request_data_p);

        if (!create_and_init_psccclient_record_data(record_p,
                EXE_PSCC_ACTION_MODIFY_QOS, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS)) {
            ATC_LOG_E("could not create and init pscclient record data!");
            goto error;
        }

        client_data_p = (psccclient_record_data_t *) record_p->client_data_p;
        client_data_p->cgcmod_p = (exe_cgcmod_set_request_t *) calloc(1, sizeof(exe_cgcmod_set_request_t));

        if (NULL == client_data_p->cgcmod_p) {
            ATC_LOG_E("cgcmod_p is NULL");
            goto error;
        }

        tmp_p = request_data_p->cid_p;

        /* add each CID to the set list */
        while (NULL != tmp_p) {
            if (psccclient_iadb_get_set_flags_from_cid(tmp_p->cid)) {
                if (!add_cid_to_cgcmod_set_list(client_data_p->cgcmod_p, tmp_p->cid)) {
                    ATC_LOG_E("add_cid_to_cgcmod_set_list failed.");
                    goto error;
                }
            }

            tmp_p = tmp_p->next_p;
        }

        if (NULL == client_data_p->cgcmod_p->cid_p) {
            /* No valid cids to operate on */
            ATC_LOG_E("no valid CIDS to operate on!");
            goto error;
        }

        /* start with first cid in list */
        iadb_results &= psccclient_iadb_set(client_data_p->cgcmod_p->cid_p->cid,
                                            psccclient_iadb_field_at_ct,
                                            (int *) &record_p);

        if (!iadb_results) {
            ATC_LOG_E("Failed to set client tag parameter. Failed.");
            goto error;
        }

        /* Send modify request. The response will end up in this functions response part */
        if (EXE_PSCC_FAILURE == pscc_request_modify_data_call_scenario_0_1_step_1_send_pscc_modify_request(
                    record_p)) {
            /* failed to do modify procedure->error scenario */
            ATC_LOG_E("Failed to modify!");
            goto error;
        }

        record_p->state = EXE_STATE_MODIFY_DATA_CALL_REQUEST;
        return EXE_PENDING;
    }

    case EXE_STATE_MODIFY_DATA_CALL_REQUEST: {
        ATC_LOG_I("EXE_STATE_MODIFY_DATA_CALL_REQUEST");
        mpl_msg_resp_t       *response_p = psccclient_get_pscc_response_p(record_p);
        /* TODO Can we get a failure event already here? */
        EXE_CHECK_GOTO_ERROR(NULL != response_p);
        EXE_CHECK_GOTO_ERROR(pscc_result_ok == response_p->result);
        record_p->state = EXE_STATE_MODIFY_DATA_CALL_RESPONSE;
        break;
    }

    case EXE_STATE_MODIFY_DATA_CALL_RESPONSE: {
        ATC_LOG_I("EXE_STATE_MODIFY_DATA_CALL_RESPONSE");
        psccclient_record_data_t *client_data_p   = (psccclient_record_data_t *)record_p->client_data_p;

        EXE_CHECK_GOTO_ERROR(NULL != client_data_p);
        EXE_CHECK_GOTO_ERROR(NULL != client_data_p->pscc_event_p);

        if (pscc_event_modify_failed == client_data_p->pscc_event_p->id) {
            goto error;
        } else if (pscc_event_modified == client_data_p->pscc_event_p->id) {
            int                  zero   = 0;
            exe_request_result_t result = EXE_FAILURE;

            if (NULL == client_data_p || NULL == client_data_p->cgcmod_p || NULL == client_data_p->cgcmod_p->cid_p) {
                ATC_LOG_E("client_data_p unexpected NULL.");
                goto error;
            }

            exe_cgcmod_set_request_cid_t *handled_cid_p = client_data_p->cgcmod_p->cid_p;
            exe_cgcmod_set_request_cid_t *next_cid_p = client_data_p->cgcmod_p->cid_p->next_p;

            /* Reset client tag in iadb for the handled cid */
            psccclient_iadb_set(handled_cid_p->cid, psccclient_iadb_field_at_ct, &zero);

            free(handled_cid_p);

            if (NULL == next_cid_p) {
                /* all requested cids handled */
                free(client_data_p->cgcmod_p);
                free(client_data_p);
                goto exit;
            } else {
                /* continue with next cid in list */
                client_data_p->cgcmod_p->cid_p = next_cid_p;
                iadb_results &= psccclient_iadb_set(next_cid_p->cid,
                                                    psccclient_iadb_field_at_ct,
                                                    (void *) &record_p);

                if (!iadb_results) {
                    ATC_LOG_E("Failed to set client tag parameter. Failed.");
                    goto error;
                }

                if (EXE_FAILURE == pscc_request_modify_data_call_scenario_0_1_step_1_send_pscc_modify_request(record_p)) {
                    ATC_LOG_E("Failed to perform modify scenario");
                    goto error;
                }

                record_p->state = EXE_STATE_MODIFY_DATA_CALL_REQUEST;
            }
        }

        break;
    }
    default:
        goto error;
    }

    return EXE_SUCCESS;

exit:
    exe_request_complete(record_p, EXE_SUCCESS, NULL);
    return EXE_SUCCESS;

error:

    if (NULL != client_data_p) {
        free_cgcmod_set_list(client_data_p->cgcmod_p);
        free(client_data_p);
    }

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
#endif
}

/**
 * internet account configuration, write authentication parameters -- AT*EIAAUW
 *
 */
exe_request_result_t request_set_PDP_auth(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        exe_eiaauw_t *request_data_p = NULL;
        bool          iadb_results   = true;
        int           mode           = 0;
        int           set_flags      = 0;

        ATC_LOG_I("EXE_STATE_REQUEST");
        request_data_p = (exe_eiaauw_t *) record_p->request_data_p;
        EXE_CHECK_GOTO_ERROR(NULL != request_data_p);

        /* Check if cid is registered */
        set_flags = psccclient_iadb_get_set_flags_from_cid(request_data_p->cid);

        if (0 == set_flags) {
            ATC_LOG_E("Invalid cid %d received", request_data_p->cid);
            goto error;
        }

        /* TODO: Check whether we should set PAP or CHAP as default auth method
         * or if a new auth method is needed in PSCC for case auth_prot=00000111 */
        switch (request_data_p->auth_prot) {
        case EXE_PSCC_AUTH_NONE:
            mode = pscc_auth_method_none; /* PAP or CHAP is never performed */
            break;
        case EXE_PSCC_AUTH_PAP:
            mode = pscc_auth_method_pap; /*  PAP may be performed */
            break;
        case EXE_PSCC_AUTH_CHAP:
            mode = pscc_auth_method_chap; /* CHAP may be performed */
            break;
        case EXE_PSCC_AUTH_PAP_OR_CHAP:
            mode = pscc_auth_method_pap; /*  PAP or CHAP may be performed, baseband dependant. Default=pap */
            break;
        default:
            ATC_LOG_E("Invalid authentication protocol %d received", request_data_p->auth_prot);
            goto error;
        }

        iadb_results &= psccclient_user_set(request_data_p->cid, psccclient_user_field_bearer_type, &(request_data_p->bearer_type));
        iadb_results &= psccclient_user_set(request_data_p->cid, psccclient_user_field_user_id_p,   &(request_data_p->userid_p));
        iadb_results &= psccclient_user_set(request_data_p->cid, psccclient_user_field_passwd_p,    &(request_data_p->password_p));
        iadb_results &= psccclient_user_set(request_data_p->cid, psccclient_user_field_auth_prot,   &mode);
        iadb_results &= psccclient_user_set(request_data_p->cid, psccclient_user_field_ask4passwd,  &(request_data_p->ask4pwd));

        if (!iadb_results) {
            ATC_LOG_E("Failed to set some of the parameters. Failed.");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_SUCCESS;
        break;
    }
    default:
        goto error;
    }

error:
    return EXE_FAILURE;
#endif
}

/**
 * request_nap_connect
 *
 * setup NAP connection
 *
 * Activate (if not already established) data connection and binds
 * this to the incoming (USB Ethernet) connection in a bridge (NAP) service
 *
 */
exe_request_result_t request_nap_connect(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    exe_pscc_result_t result = EXE_PSCC_FAILURE;

    /* 1. Activate PDP if not already activated
     *    1.1 if conn_id is not assigned to cid
     *        1.1 a) activate PDP context
     *    1.2 if conn_id is assigned to cid
     *        1.2 a) check activation status of conn_id
     *        1.2 b) if disconnected activate PDP context
     * 2. Set routing rules
     * 3. Activate NAP connection (connect USB with PS connection) */

    switch (record_p->state) {
    case EXE_STATE_REQUEST: { /* REQUEST PART */
        ATC_LOG_I("EXE_STATE_REQUEST");
        exe_enap_t *request_data_p = NULL;
        int set_flags;
        int conn_id = 0;
        psccclient_record_data_t *client_data_p;

        request_data_p = (exe_enap_t *)record_p->request_data_p;

        if (NULL == request_data_p) {
            ATC_LOG_E("request_data_p is NULL");
            goto error;
        }

        /* [1] Check if cid is registered */
        set_flags = psccclient_iadb_get_set_flags_from_cid(request_data_p->cid);

        if (0 == set_flags) {
            ATC_LOG_E("incorrect cid.");
            goto error;
        }

        psccclient_iadb_get(request_data_p->cid, psccclient_iadb_field_conn_id, &conn_id);

        if (0 == conn_id) {
            /* [1.1] No connection assigned to cid, activate PDP */
            if (!create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_UNDEFINED, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS)) {
                ATC_LOG_E("create_and_init_psccclient_record_data failed.");
                goto error;
            }

            client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

            if (NULL == client_data_p) {
                ATC_LOG_E("client_data_p is NULL.");
                goto error;
            }

            client_data_p->cid = request_data_p->cid;

            /* Activate PDP context */
            result = exe_pscc_start_pscc_interaction(record_p, request_data_p->cid, EXE_PSCC_STATE_ON, NULL, NULL);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            record_p->state = EXE_STATE_SETUP_DATA_CALL_CREATE;
        } else {
            /* [1.2] Connection assigned to cid, check PDP activation status */
            if (!create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_UNDEFINED, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS)) {
                ATC_LOG_E("create_and_init_psccclient_record_data failed.");
                goto error;
            }

            client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

            if (NULL == client_data_p) {
                ATC_LOG_E("client_data_p is NULL.");
                goto error;
            }

            client_data_p->cid = request_data_p->cid;

            /* request PDP connection parameters */
            if (!exe_pscc_get_connection_parameters(record_p, request_data_p->cid)) {
                ATC_LOG_E("exe_pscc_get_connection_parameters failed.");
                goto error;
            }

            record_p->state = EXE_STATE_QUERY_CONNECTION;
        }

        return EXE_PENDING;
    }
    break;

    /* RESPONSE PART */
    case EXE_STATE_QUERY_CONNECTION: {
        ATC_LOG_I("EXE_STATE_QUERY_CONNECTION");
        result = pscc_request_setup_data_call_send_pscc_get_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);

        /* [1.2 a] Check activation status, if disconnected activate PDP context */
        exe_pscc_connection_response_t *response_data_p = NULL;
        psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;
        int cid = client_data_p->cid;
        int conn_id = 0;
        int zero = 0;

        /* Reset client tag in iadb for the handled cid */
        psccclient_iadb_set(cid, psccclient_iadb_field_at_ct, &zero);

        response_data_p = (exe_pscc_connection_response_t *)record_p->response_data_p;

        if (NULL == response_data_p) {
            ATC_LOG_E("response_data_p is NULL.");
            goto error;
        }

        if (EXE_PSCC_SUCCESS != result) {
            ATC_LOG_E("exe_pscc_get_connection_parameters result is not success.");
            free_pscc_connection_response(response_data_p);
            record_p->response_data_p = NULL;
            goto error;
        }

        if (pscc_connection_status_connected != (pscc_connection_status_t)(response_data_p->connection_status)) {
            /* [1.2 b] Activate PDP context */

            /* free data allocated after get connection status */
            free_pscc_connection_response(response_data_p);
            record_p->response_data_p = NULL;

            if (!create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_UNDEFINED, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS)) {
                ATC_LOG_E("create_and_init_psccclient_record_data failed.");
                goto error;
            }

            client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

            if (NULL == client_data_p) {
                ATC_LOG_E("client_data_p is NULL.");
                goto error;
            }

            client_data_p->cid = cid;

            psccclient_iadb_get(cid, psccclient_iadb_field_conn_id, &conn_id);

            if (0 == conn_id) {
                ATC_LOG_E("conn_id not found in iadb.");
                goto error;
            }

            /* Activate PDP context */
            result = exe_pscc_start_pscc_interaction(record_p, cid, EXE_PSCC_STATE_ON, &conn_id, &conn_id);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            record_p->state = EXE_STATE_SETUP_DATA_CALL_CREATE;
            return EXE_PENDING;
        } else {
            /* [1.2. a] PDP already activated */
            result = exe_pscc_create_sterc(record_p);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            record_p->state = EXE_STATE_STERC_CREATE;
        }

        break;
    }

    case EXE_STATE_SETUP_DATA_CALL_CREATE:
    case EXE_STATE_SETUP_DATA_CALL_SET:
    case EXE_STATE_SETUP_DATA_CALL_CONNECT:
    case EXE_STATE_SETUP_DATA_CALL_CONNECTING:
    case EXE_STATE_SETUP_DATA_CALL_CONNECTED:
    case EXE_STATE_SETUP_DATA_CALL_DISCONNECTED:
    case EXE_STATE_SETUP_DATA_CALL_DESTROY:
    case EXE_STATE_SETUP_DATA_CALL_WAIT_RECONNECT:
    case EXE_STATE_SETUP_DATA_CALL_RECONNECT:
        result = exe_pscc_setup_data_call(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        break;

    case EXE_STATE_SETUP_DATA_CALL_GET: {
        result = exe_pscc_setup_data_call(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS_TERMINATE == result);

        result = exe_pscc_create_sterc(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_STERC_CREATE;
        break;
    }

    case EXE_STATE_STERC_CREATE: {
        ATC_LOG_I("EXE_STATE_STERC_CREATE");
        /* Check of NAP connection is set-up - if so respond back to client */
        int sterc_handle = 0;
        result = pscc_request_nap_send_sterc_create_response(record_p);

        if (EXE_PSCC_FAILURE_DESTROY == result) {
            result = pscc_request_nap_send_sterc_destroy(record_p, sterc_handle);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            record_p->state = EXE_STATE_STERC_DESTROY;
        } else {
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            /* Everything went fine, send response. */
            goto exit;
        }

        break;
    }
    case EXE_STATE_STERC_DESTROY: {
        ATC_LOG_I("EXE_STATE_STERC_DESTROY");
        mpl_msg_resp_t *response_p = NULL;

        response_p = (mpl_msg_resp_t *)psccclient_get_pscc_response_p(record_p);
        EXE_CHECK_GOTO_ERROR(NULL != response_p);

        if (response_p->result != sterc_result_ok) {
            ATC_LOG_E("response failed: %d", response_p->result);
            result = EXE_PSCC_FAILURE;
            goto error;
        }

        break;
    }

    default:
        goto error;
    }

    return EXE_SUCCESS;

exit:
    free_pscc_connection_response((exe_pscc_connection_response_t *)record_p->response_data_p);
    free(record_p->client_data_p);
    exe_request_complete(record_p, EXE_SUCCESS, NULL);
    return EXE_SUCCESS;

error:
    free_pscc_connection_response((exe_pscc_connection_response_t *)record_p->response_data_p);
    free(record_p->client_data_p);

    if (EXE_STATE_REQUEST != record_p->state) {
        if (EXE_PSCC_FAILURE == result) {
            exe_request_complete(record_p, EXE_FAILURE, NULL);
        }
    }

    return EXE_FAILURE;
#endif
}

/**
 * request_nap_disconnect
 *
 * disconnect NAP connection, deactivate PDP context
 *
 */
exe_request_result_t request_nap_disconnect(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    /*
     *  0.  Check the presence of cid and sterc_handle
     *  1.  Deactivate NAP -> disconnect usb with ps connection
     *  2.  Deactivate PDP context */
    exe_pscc_result_t result = EXE_PSCC_FAILURE;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: { /* REQUEST PART */
        ATC_LOG_I("EXE_STATE_REQUEST");
        psccclient_record_data_t *client_data_p = NULL;
        int set_flags;
        int sterc_handle;
        exe_pscc_state_t pscc_state;
        exe_enap_t *request_data_p = (exe_enap_t *)record_p->request_data_p;

        if (NULL == request_data_p) {
            ATC_LOG_E("request_data_p is NULL");
            goto error;
        }

        /* Check if cid is registered */
        set_flags = psccclient_iadb_get_set_flags_from_cid(request_data_p->cid);

        if (0 == set_flags) {
            ATC_LOG_E("No conneciton assigned to cid.");
            goto error;
        }

        /* Fetch sterc_handle if it exists */
        if (!psccclient_iadb_get(request_data_p->cid, psccclient_iadb_field_sterc_handle, &sterc_handle)) {
            ATC_LOG_E("No sterc_handle assigned to cid.");
            goto error;
        }

        if (!create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_UNDEFINED, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS)) {
            ATC_LOG_E("create_and_init_psccclient_record_data failed.");
            goto error;
        }

        client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

        if (NULL == client_data_p) {
            ATC_LOG_E("client_data_p is NULL.");
            goto error;
        }

        client_data_p->cid = request_data_p->cid;

        /* disconnect NAP */
        if (EXE_PSCC_SUCCESS !=  pscc_request_nap_send_sterc_destroy(record_p, sterc_handle)) {
            ATC_LOG_E("pscc_request_nap_send_sterc_destroy failed.");
            return EXE_FAILURE;
        }

        ATC_LOG_I("-> request sterc_destroy");

        record_p->state = EXE_STATE_STERC_DESTROY;
        return EXE_PENDING;
    }

    case EXE_STATE_STERC_DESTROY: {
        ATC_LOG_I("EXE_STATE_STERC_DESTROY");
        mpl_msg_resp_t *response_p = NULL;

        response_p = (mpl_msg_resp_t *)psccclient_get_pscc_response_p(record_p);
        EXE_CHECK_GOTO_ERROR(NULL != response_p);

        if (response_p->result != sterc_result_ok) {
            ATC_LOG_E("response failed: %d", response_p->result);
            goto error;
        }

        exe_pscc_connection_response_t *response_data_p = (exe_pscc_connection_response_t *)record_p->response_data_p;
        psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;
        int cid = client_data_p->cid;
        int conn_id = 0;

        ATC_LOG_I("<- response sterc_destroy");

        (void)psccclient_iadb_remove_sterc_handle(cid);

        psccclient_iadb_get(cid, psccclient_iadb_field_conn_id, &conn_id);

        if (0 == conn_id) {
            ATC_LOG_E("conn_id not found in iadb.");
            goto error;
        }

        /* Deactivate PDP context */
        result = exe_pscc_start_pscc_interaction(record_p, cid, EXE_PSCC_STATE_OFF, NULL, NULL);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT;
        break;
    }
    case EXE_STATE_DEACTIVATE_DATA_CALL_CONNECTING:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTING:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTED:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DESTROY:
        result = exe_pscc_deactivate_data_call(record_p);

        if (EXE_PSCC_SUCCESS_TERMINATE == result) {
            goto exit;
        } else {
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        }

        break;

    default:
        goto error;
    }

    return EXE_SUCCESS;

exit: {
        psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;
        int zero = 0;

        /* Reset client tag in iadb for the handled cid */
        psccclient_iadb_set(client_data_p->cid, psccclient_iadb_field_at_ct, &zero);

        free(client_data_p);
        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:
    free(record_p->client_data_p);

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
#endif
}


/**
 * request_nap_read
 *
 * get nap activation status
 */
exe_request_result_t request_nap_read(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("EXE_STATE_REQUEST");
        int sterc_handle = 0;
        int max_entries = 0;
        int iadb = 0;

        max_entries = psccclient_iadb_get_define_PSCCCLIENT_MAX_IADB_ENTRIES();

        for (iadb = 0; iadb < max_entries; iadb++) {
            /* find all valid cids */
            if (psccclient_iadb_get_set_flags_from_cid(iadb)) {
                if (psccclient_iadb_get(iadb, psccclient_iadb_field_sterc_handle, &sterc_handle)) {
                    /* there should be only one NAP connection */
                    break;
                }
            }
        }

        if (0 == sterc_handle) {
            ATC_LOG_I("No sterc_handle assigned to cid.");
            return EXE_SUCCESS;
        }

        if (!create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_UNDEFINED, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS)) {
            ATC_LOG_E("create_and_init_psccclient_record_data failed.");
            goto error;
        }

        if (EXE_PSCC_SUCCESS != pscc_request_get_nap_status_send_sterc_get(record_p, sterc_handle)) {
            ATC_LOG_E("pscc_request_get_nap_status_send_sterc_get failed.");
            goto error;
        }

        ATC_LOG_I("-> request");
        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("EXE_STATE_RESPONSE");

        pscc_request_get_nap_status_send_sterc_get_response(record_p);

        exe_enap_t *response_data_p = (exe_enap_t *)record_p->response_data_p;

        ATC_LOG_I("<- response");

        free(record_p->client_data_p);
        exe_request_complete(record_p, EXE_SUCCESS, response_data_p);
        free(response_data_p);
        return EXE_SUCCESS;
    }

error:
    free(record_p->client_data_p);

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
#endif
}



/**
 * request_set_PDP_context_state - AT+CGACT
 *
 * Description:
 * Activates or deactivates the PDP context for all cid's in the cid_p of the
 * exe_cgact_set_request_t defined by the record_p->request_data_p.
 * If no cid's are give, all valid connections will be processed.
 * The PDP context is activated or deactivated, depending on the value
 * of the state in the exe_cgact_set_request_t.
 * Activation or deactivation is synchronous, or asynchronous, depending
 * on the value of the async_mode in the exe_cgact_set_request_t.
 * If any PDP context is already in the requested state, the state for that
 * context remains unchanged. If the requested state context cannot be acheived,
 * an ERROR or +CME ERROR response is returned, but the successfully acheived
 * context states are not changed (e.g. deactivated) as a cause of this.
 */
exe_request_result_t request_set_PDP_context_state(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    exe_request_result_t         result = EXE_PSCC_FAILURE;
    exe_cgact_set_request_t     *request_data_p = NULL;
    exe_cgact_set_request_cid_t *tmp_p          = NULL;
    psccclient_record_data_t    *client_data_p  = NULL;
    int                          state;
    exe_pscc_action_t            action = EXE_PSCC_ACTION_UNDEFINED;
    exe_cgact_set_request_t     *new_request_data_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: { /* REQUEST PART */
        ATC_LOG_I("EXE_STATE_REQUEST");
        request_data_p = (exe_cgact_set_request_t *)record_p->request_data_p;
        exe_cgact_set_request_cid_t *cid_p = NULL;
        int conn_id = 0;

        if (NULL == request_data_p) {
            ATC_LOG_E("request_data is NULL. Failed.");
            goto error;
        }

        cid_p = request_data_p->cid_p;

        if (EXE_CGACT_ASYNC_MODE_ASYNCHRONOUS == request_data_p->async_mode) {
            /* Allocate a new request_data_p for driving the asynchronous request,
             * saving the old one in the request */
            /* TODO This should always be done, thus limiting the usage of client_data_p. */
            new_request_data_p = calloc(1, sizeof(exe_cgact_set_request_t));

            if (NULL == new_request_data_p) {
                ATC_LOG_E("Failed to allocate new exe_cgact_set_request_t");
                goto error;
            }

            new_request_data_p->async_mode = request_data_p->async_mode;
            new_request_data_p->state = request_data_p->state;
            record_p->request_data_p = new_request_data_p;
        } else if (EXE_CGACT_ASYNC_MODE_SYNCHRONOUS == request_data_p->async_mode) {
            /* NOTHING */
        } else {
            ATC_LOG_E("request_data_p->async_mode undefined");
            goto error;
        }

        if (!create_and_init_psccclient_record_data(record_p, action, request_data_p->async_mode)) {
            ATC_LOG_E("create_and_init_psccclient_record_data failed.");
            goto error;
        }

        client_data_p = (psccclient_record_data_t *)record_p->client_data_p;
        client_data_p->cgact_p = calloc(1, sizeof(exe_cgact_set_request_t));

        if (NULL == client_data_p->cgact_p) {
            ATC_LOG_E("Failed to allocate client_data_p->cgact_p");
            goto error;
        }

        client_data_p->cgact_p->state = request_data_p->state;
        client_data_p->cgact_p->async_mode = request_data_p->async_mode;
        state = request_data_p->state;

        /*
         * Special case, no cid is given, it means that all valid connections
         * should be changed the given state.
         */
        if (NULL == cid_p) {
            int i = 0;
            int max_entries = psccclient_iadb_get_define_PSCCCLIENT_MAX_IADB_ENTRIES();
            ATC_LOG_I("Special case, no cid is given");

            for (i; i < max_entries; i++) {
                /* find all valid cids */
                if (psccclient_iadb_get_set_flags_from_cid(i)) {
                    int tmp_id;

                    if (EXE_PSCC_STATE_OFF == request_data_p->state &&
                            !psccclient_iadb_get(i, psccclient_iadb_field_conn_id, &tmp_id)) {
                        ATC_LOG_I("cid: %d not added to deactivation list, no connection available", i);
                    } else {
                        if (!add_cid_to_cgact_set_list(client_data_p->cgact_p, i)) {
                            ATC_LOG_E("add_cid_to_cgact_set_list failed.");
                            goto error;
                        }
                    }
                }
            }

            if (NULL == client_data_p->cgact_p->cid_p) {
                free(client_data_p->cgact_p);
                free(client_data_p);
                return (EXE_PSCC_STATE_ON == request_data_p->state ? EXE_FAILURE: EXE_SUCCESS);
            }
        } else {
            tmp_p = cid_p;

            while (NULL != tmp_p) {
                if (psccclient_iadb_get_set_flags_from_cid(tmp_p->cid)) {
                    int tmp_id;

                    if (EXE_PSCC_STATE_OFF == request_data_p->state &&
                            !psccclient_iadb_get(tmp_p->cid, psccclient_iadb_field_conn_id, &tmp_id)) {
                        ATC_LOG_I("cid: %d not added to deactivation list, no connection available", tmp_p->cid);
                    } else {
                        if (!add_cid_to_cgact_set_list(client_data_p->cgact_p, tmp_p->cid)) {
                            ATC_LOG_E("add_cid_to_cgact_set_list failed.");
                            goto error;
                        }
                    }
                } else {
                    ATC_LOG_E("invalid cid: %d", tmp_p->cid);
                    /* creation of account with CGDCONT has not been made for this cid */
                    goto error;
                }

                tmp_p = tmp_p->next_p;
            }
        }

        /* Initial state: If requested state is deactivated while we have no active connections, return success */
        if (NULL == client_data_p->cgact_p->cid_p && EXE_PSCC_STATE_OFF == state) {
            ATC_LOG_I("Initial state.");
            free(client_data_p->cgact_p);
            free(client_data_p);
            return EXE_SUCCESS;
        }

        /* If the connection exists, check its connection status. */
        psccclient_iadb_get(client_data_p->cgact_p->cid_p->cid, psccclient_iadb_field_conn_id, &conn_id);

        if (0 != conn_id) {
            /* request PDP connection parameters */
            if (!exe_pscc_get_connection_parameters(record_p, client_data_p->cgact_p->cid_p->cid)) {
                ATC_LOG_E("exe_pscc_get_connection_parameters failed.");
                goto error;
            }

            record_p->state = EXE_STATE_QUERY_CONNECTION;
        } else {
            /* start with first cid in list */
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == exe_pscc_start_pscc_interaction(record_p,
                                 client_data_p->cgact_p->cid_p->cid,
                                 state, NULL, NULL));

            if (EXE_PSCC_STATE_ON == request_data_p->state) {
                record_p->state = EXE_STATE_SETUP_DATA_CALL_CREATE;
            } else {
                record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT;
            }
        }

        if (EXE_CGACT_ASYNC_MODE_ASYNCHRONOUS == request_data_p->async_mode) {
            /* Should really be pending for a short while more,
             * allowing the server to acknowledge that it can accept an async request
             * Retain the record as it otherwise will be destroyed when we return. */
            request_record_retain(record_p);
            result = EXE_SUCCESS;
        } else {
            result = EXE_PENDING;
        }

        return result;
    }
    case EXE_STATE_QUERY_CONNECTION: {
        int cid = 0;
        int state = 0;
        int conn_id = 0;
        exe_pscc_connection_response_t *response_data_p = NULL;
        pscc_connection_status_t status = pscc_connection_status_disconnected;

        ATC_LOG_I("EXE_STATE_QUERY_CONNECTION");

        client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

        if (NULL == client_data_p || NULL == client_data_p->cgact_p || NULL == client_data_p->cgact_p->cid_p) {
            ATC_LOG_E("unexpected NULL.");
            goto error;
        }

        cid = client_data_p->cgact_p->cid_p->cid;
        state = client_data_p->cgact_p->state;

        if (EXE_PSCC_SUCCESS != pscc_request_setup_data_call_send_pscc_get_response(record_p)) {
            ATC_LOG_E("pscc_request_setup_data_call_send_pscc_get_response failed.");
            free_pscc_connection_response((exe_pscc_connection_response_t *)record_p->response_data_p);
            record_p->response_data_p = NULL;
            goto error;
        }

        response_data_p = (exe_pscc_connection_response_t *)record_p->response_data_p;

        if (NULL == response_data_p) {
            ATC_LOG_E("response_data_p is NULL.");
            goto error;
        }

        status = response_data_p->connection_status;

        /* free data allocated after get connection status */
        free_pscc_connection_response(response_data_p);
        record_p->response_data_p = NULL;

        ATC_LOG_I("Connection status: %d", status);

        if (EXE_PSCC_STATE_ON == state) {
            switch (status) {
            case pscc_connection_status_disconnected:
                record_p->state = EXE_STATE_SETUP_DATA_CALL_CREATE;
                break;
            case pscc_connection_status_connecting:
                record_p->state = EXE_STATE_SETUP_DATA_CALL_CONNECTED;
                return EXE_PENDING;
            case pscc_connection_status_connected:
            case pscc_connection_status_modifying:
                /* Nothing to do. We are already connected i.e. activated. Go to next CID */
                goto next_cid;
            case pscc_connection_status_disconnecting:
                /* Wait for disconnected event before doing connect */
                record_p->state = EXE_STATE_SETUP_DATA_CALL_WAIT_RECONNECT;
                return EXE_PENDING;
            default:
                ATC_LOG_E("Received unknown connection status %d", status);
                goto error;
            }

        } else {
            switch (status) {
            case pscc_connection_status_disconnecting:
                record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTED;
                return EXE_PENDING;
            case pscc_connection_status_disconnected:
                /* Nothing to do. We are already disconnected i.e. deactivated. Go to next CID */
                goto next_cid;
            case pscc_connection_status_connecting:
                /* State connecting may receive disconnect. Continue */
                record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_CONNECTING;
                break;
            case pscc_connection_status_connected:
            case pscc_connection_status_modifying:
                record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT;
                break;
            default:
                ATC_LOG_E("Received unknown connection status %d", status);
                goto error;
            }
        }

        psccclient_iadb_get(cid, psccclient_iadb_field_conn_id, &conn_id);

        if (0 == conn_id) {
            ATC_LOG_E("conn_id not found in iadb.");
            goto error;
        }

        /* Change PDP context state */
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == exe_pscc_start_pscc_interaction(record_p, cid, state, &conn_id, &conn_id));
        return EXE_SUCCESS;
    }
    /* RESPONSE PART */
    case EXE_STATE_SETUP_DATA_CALL_CREATE:
    case EXE_STATE_SETUP_DATA_CALL_SET:
    case EXE_STATE_SETUP_DATA_CALL_CONNECT:
    case EXE_STATE_SETUP_DATA_CALL_CONNECTING:
    case EXE_STATE_SETUP_DATA_CALL_CONNECTED:
    case EXE_STATE_SETUP_DATA_CALL_DISCONNECTED:
    case EXE_STATE_SETUP_DATA_CALL_GET:
    case EXE_STATE_SETUP_DATA_CALL_DESTROY:
    case EXE_STATE_SETUP_DATA_CALL_WAIT_RECONNECT:
    case EXE_STATE_SETUP_DATA_CALL_RECONNECT: {
        exe_pscc_result_t pscc_result = EXE_PSCC_FAILURE;

        pscc_result = exe_pscc_setup_data_call(record_p);

        if (EXE_PSCC_SUCCESS_TERMINATE == pscc_result) {
            /* Data call has been setup for requested cid. */
            goto next_cid;
        } else {
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_result);
        }

        break;
    }
    case EXE_STATE_DEACTIVATE_DATA_CALL_CONNECTING:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTING:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTED:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DESTROY: {
        exe_pscc_result_t pscc_result = EXE_PSCC_FAILURE;

        pscc_result = exe_pscc_deactivate_data_call(record_p);

        if (EXE_PSCC_SUCCESS_TERMINATE == pscc_result) {
            /* Data call has been deactivated for requested cid. */
            goto next_cid;
        } else {
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_result);
        }

        break;
    }

    default:
        goto error;
    }

    return EXE_SUCCESS;

next_cid: {
        int zero = 0;
        int conn_id = 0;

        free_pscc_connection_response(record_p->response_data_p);
        record_p->response_data_p = NULL;
        client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

        if (NULL == client_data_p || NULL == client_data_p->cgact_p || NULL == client_data_p->cgact_p->cid_p) {
            ATC_LOG_E("unexpected NULL.");
            goto error;
        }

        exe_cgact_set_request_cid_t *handled_cid_p = client_data_p->cgact_p->cid_p;
        client_data_p->cgact_p->cid_p = client_data_p->cgact_p->cid_p->next_p;
        state = client_data_p->cgact_p->state;
        /* Reset client tag in iadb for the handled cid */
        psccclient_iadb_set(handled_cid_p->cid, psccclient_iadb_field_at_ct, &zero);

        free(handled_cid_p);

        if (NULL != client_data_p->cgact_p->cid_p) {
            /* Check if this connection already exists and get its connection status */
            psccclient_iadb_get(client_data_p->cgact_p->cid_p->cid, psccclient_iadb_field_conn_id, &conn_id);

            if (0 != conn_id) {
                /* request PDP connection parameters */
                if (!exe_pscc_get_connection_parameters(record_p, client_data_p->cgact_p->cid_p->cid)) {
                    ATC_LOG_E("exe_pscc_get_connection_parameters failed.");
                    goto error;
                }

                record_p->state = EXE_STATE_QUERY_CONNECTION;
                return EXE_SUCCESS;
            } else {
                /* continue with next cid in list */
                EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == exe_pscc_start_pscc_interaction(record_p,
                                     client_data_p->cgact_p->cid_p->cid,
                                     state, NULL, NULL));

                if (EXE_PSCC_STATE_ON == state) {
                    record_p->state = EXE_STATE_SETUP_DATA_CALL_CREATE;
                } else {
                    record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT;
                }
            }
        } else {
            goto exit;
        }

        return EXE_SUCCESS;

    }

exit: {
        exe_cgact_async_mode_t client_data_async_mode;

        client_data_async_mode = client_data_p->async_mode;

        /* All requested cids handled, terminate the request. */
        ATC_LOG_I("in response, all cids handled");

        free(client_data_p->cgact_p);
        client_data_p->cgact_p = NULL;

        free(client_data_p);
        client_data_p = NULL;

        free_pscc_connection_response((exe_pscc_connection_response_t *)record_p->response_data_p);

        if (EXE_CGACT_ASYNC_MODE_ASYNCHRONOUS == client_data_async_mode) {
            free(record_p->request_data_p);
            request_record_destroy(record_p);
        } else {
            exe_request_complete(record_p, EXE_SUCCESS, NULL);
        }

        return EXE_SUCCESS;
    }

error: {
        /* Clean up if required. Note that clean-up shall not be done at this
         * point in non-failure cases, since the data needs to be kept for each
         * cid to until it has been processed.
         */
        exe_cgact_async_mode_t client_data_async_mode = EXE_CGACT_ASYNC_MODE_SYNCHRONOUS;

        client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

        if (NULL != client_data_p) {
            client_data_async_mode = client_data_p->async_mode;

            if (NULL != client_data_p->cgact_p) {
                exe_cgact_set_request_cid_t *tmp_cid_p = client_data_p->cgact_p->cid_p;

                while (NULL != client_data_p->cgact_p->cid_p) {
                    tmp_cid_p = client_data_p->cgact_p->cid_p;
                    client_data_p->cgact_p->cid_p = client_data_p->cgact_p->cid_p->next_p;
                    free(tmp_cid_p);
                }

                free(client_data_p->cgact_p);
            }

            free(client_data_p);
            record_p->client_data_p = NULL;
        }

        free_pscc_connection_response((exe_pscc_connection_response_t *)record_p->response_data_p);
        record_p->response_data_p = NULL;


        if (EXE_STATE_REQUEST != record_p->state) {
            if (EXE_CGACT_ASYNC_MODE_ASYNCHRONOUS == client_data_async_mode) {
                free(record_p->request_data_p);
                request_record_destroy(record_p);
            } else {
                exe_request_complete(record_p, EXE_FAILURE, NULL);
            }
        } else {
            free(new_request_data_p);
        }

        return EXE_FAILURE;
    }
#endif
}


/**
 * request_activate_PDP_context -- AT*EPPSD
 *
 * request_propriatery_activate_PDP_context.
 *
 * returns information in XML like
 * <?xml version="1.0"?>
 * <connection_parameters>
 * <ip_address>10.64.151.14</ip_address>
 * <subnet_mask>255.255.255.255</subnet_mask>
 * <mtu>1500</mtu>
 * <dns_server>10.64.148.4</dns_server>
 * <dns_server>10.64.148.5</dns_server>
 * </connection_parameters>
 *
 *
 *  Description from spec:
 * The execution command causes the ME to establish a primary PS connection between the ME and the
 * network, using a CAIF datagram channel type between the mobile platform and the external device,
 * e.g. an application CPU. This link type allows for raw IP packets to be sent between the two entities.
 * When the connection is successfully established the intermediate result code
 * *EPPSD: <connection_parameters> is returned, followed by the final result code OK. In case it is not
 * possible to establish a connection due to e.g. radio resources the ME returns ERROR or +CME ERROR: <err>.
 * In case an established connection is terminated by the network or by other reasons not initiated by the user,
 * the intermediate result code *EPPSD: <cid>,<state> is returned followed by the final result code OK.
 * The only applicable value for <state> parameter in this case is 0.
 * Note: This command can only be used if the CAIF protocol is implemented.
 */
exe_request_result_t request_activate_PDP_context(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    exe_pscc_result_t         result = EXE_PSCC_FAILURE;
    exe_eppsd_t              *request_data_p = NULL;
    psccclient_record_data_t *client_data_p  = NULL;
    exe_pscc_action_t         action         = EXE_PSCC_ACTION_UNDEFINED;
    exe_eppsd_t              *new_request_data_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: { /* REQUEST PART */
        unsigned long conn_id = 0;

        ATC_LOG_I("EXE_STATE_REQUEST");

        request_data_p = (exe_eppsd_t *)record_p->request_data_p;

        if (NULL == request_data_p) {
            ATC_LOG_E("request_data is NULL. Failed.");
            goto error;
        }

        if (!psccclient_iadb_get_set_flags_from_cid(request_data_p->cid)) {
            /* creation of account with CGDCONT has not been made for this cid */
            ATC_LOG_E("No account defined for the cid:%d", request_data_p->cid);
            goto error;
        }

        new_request_data_p = calloc(1, sizeof(exe_eppsd_t));

        if (NULL == new_request_data_p) {
            ATC_LOG_E("Failed to allocate new exe_eppsd_t");
            goto error;
        }

        new_request_data_p->cid = request_data_p->cid;
        new_request_data_p->connection_id = request_data_p->connection_id;
        new_request_data_p->state = request_data_p->state;
        record_p->request_data_p = new_request_data_p;

        /* connection_id is not used since <channel_id> from AT*EPPSD is ignored */
        ATC_LOG_I("connection id: %d ignored", new_request_data_p->connection_id);

        if (!create_and_init_psccclient_record_data(record_p, action, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS)) {
            ATC_LOG_E("create_and_init_psccclient_record_data failed.");
            goto error;
        }

        /* If the connection already exists, check its connection status. */
        psccclient_iadb_get(request_data_p->cid, psccclient_iadb_field_conn_id, &conn_id);

        if (0 != conn_id) {
            /* request PDP connection parameters */
            if (!exe_pscc_get_connection_parameters(record_p, request_data_p->cid)) {
                ATC_LOG_E("exe_pscc_get_connection_parameters failed.");
                goto error;
            }

            record_p->state = EXE_STATE_QUERY_CONNECTION;
        } else {
            /*
             * Note: This is correct, connection_id should be used for both the 4th and 5th
             * parameter in this function call
             */
            result = exe_pscc_start_pscc_interaction(record_p,
                     request_data_p->cid,
                     request_data_p->state,
                     NULL, NULL);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);

            if (EXE_PSCC_STATE_ON == request_data_p->state) {
                record_p->state = EXE_STATE_SETUP_DATA_CALL_CREATE;
            } else {
                record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT;
            }
        }

        return EXE_PENDING;
    }
    case EXE_STATE_QUERY_CONNECTION: {
        exe_pscc_connection_response_t *response_data_p = NULL;
        int cid = 0;
        int state = 0;
        unsigned long conn_id = 0;
        pscc_connection_status_t status = pscc_connection_status_disconnected;

        ATC_LOG_I("EXE_STATE_QUERY_CONNECTION");

        request_data_p = (exe_eppsd_t *)record_p->request_data_p;
        cid = request_data_p->cid;
        state = request_data_p->state;

        if (EXE_PSCC_SUCCESS != pscc_request_setup_data_call_send_pscc_get_response(record_p)) {
            ATC_LOG_E("pscc_request_setup_data_call_send_pscc_get_response failed.");
            free_pscc_connection_response((exe_pscc_connection_response_t *)record_p->response_data_p);
            record_p->response_data_p = NULL;
            goto error;
        }

        response_data_p = (exe_pscc_connection_response_t *)record_p->response_data_p;

        if (NULL == response_data_p) {
            ATC_LOG_E("response_data_p is NULL.");
            goto error;
        }

        status = response_data_p->connection_status;

        /* free data after get connection status */
        free_pscc_connection_response(response_data_p);
        record_p->response_data_p = NULL;

        ATC_LOG_I("Connection status: %d", status);

        if (EXE_PSCC_STATE_ON == state) {
            switch (status) {
            case pscc_connection_status_disconnected:
                record_p->state = EXE_STATE_SETUP_DATA_CALL_CREATE;
                break;
            case pscc_connection_status_connecting:
                /* Just change state and wait for connected event. */
                record_p->state = EXE_STATE_SETUP_DATA_CALL_CONNECTED;
                return EXE_PENDING;
            case pscc_connection_status_connected:
            case pscc_connection_status_modifying:
                /* We are already connected i.e. activated.
                 * Unlike AT+CGACT, AT*EPPSD does not require the PDP context to keep
                 * the previous state if the requested state is identical.
                 * Hence return ERROR */
                goto error;
            case pscc_connection_status_disconnecting:
                /* Wait for disconnect event+destroy before activating */
                record_p->state = EXE_STATE_SETUP_DATA_CALL_WAIT_RECONNECT;
                return EXE_PENDING;
            default:
                ATC_LOG_E("Received unknown connection status %d", status);
                goto error;
            }

        } else {
            switch (status) {
            case pscc_connection_status_disconnecting:
                /* Just change state and wait for disconnected event. */
                record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTED;
                return EXE_PENDING;
            case pscc_connection_status_disconnected:
                /* We are already disconnected i.e. deactivated.
                 * Unlike AT+CGACT, AT*EPPSD does not require the PDP context to keep
                 * the previous state if the requested state is identical.
                 * Hence return ERROR */
                goto error;
            case pscc_connection_status_connecting:
                /* State connecting may receive disconnect. Continue */
                record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_CONNECTING;
                break;
            case pscc_connection_status_connected:
            case pscc_connection_status_modifying:
                record_p->state = EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT;
                break;
            default:
                ATC_LOG_E("Received unknown connection status %d", status);
                goto error;
            }
        }

        /* Double check if connection exists */
        psccclient_iadb_get(cid, psccclient_iadb_field_conn_id, &conn_id);

        if (0 == conn_id) {
            ATC_LOG_E("conn_id not found in iadb.");
            goto error;
        }

        /* Change PDP context state */
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == exe_pscc_start_pscc_interaction(record_p, cid, state, &conn_id, &conn_id));
        return EXE_SUCCESS;
    }
    /* RESPONSE PART */
    case EXE_STATE_SETUP_DATA_CALL_CREATE:
    case EXE_STATE_SETUP_DATA_CALL_SET:
    case EXE_STATE_SETUP_DATA_CALL_CONNECT:
    case EXE_STATE_SETUP_DATA_CALL_CONNECTING:
    case EXE_STATE_SETUP_DATA_CALL_CONNECTED:
    case EXE_STATE_SETUP_DATA_CALL_DISCONNECTED:
    case EXE_STATE_SETUP_DATA_CALL_GET:
    case EXE_STATE_SETUP_DATA_CALL_DESTROY:
    case EXE_STATE_SETUP_DATA_CALL_WAIT_RECONNECT:
    case EXE_STATE_SETUP_DATA_CALL_RECONNECT:
        result = exe_pscc_setup_data_call(record_p);

        if (EXE_PSCC_SUCCESS_TERMINATE == result) {
            goto exit;
        } else {
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        }

        break;
    case EXE_STATE_DEACTIVATE_DATA_CALL_CONNECTING:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTING:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTED:
    case EXE_STATE_DEACTIVATE_DATA_CALL_DESTROY:
        result = exe_pscc_deactivate_data_call(record_p);

        if (EXE_PSCC_SUCCESS_TERMINATE == result) {
            goto exit;
        } else {
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        }

        break;

    default:
        goto error;
    }

    return EXE_SUCCESS;

exit: {
        /* Clean up the received response */
        int cid = 0;
        int at_ct = 0;

        exe_pscc_connection_response_t *response_data_p = (exe_pscc_connection_response_t *)record_p->response_data_p;
        psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;
        exe_eppsd_t *request_data_p = (exe_eppsd_t *)record_p->request_data_p;

        if (psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
            (void)psccclient_iadb_set(cid, psccclient_iadb_field_at_ct, &at_ct);
        }

        free(request_data_p);
        free(client_data_p);

        if (NULL == response_data_p) {
            exe_request_complete(record_p, EXE_SUCCESS, NULL);
        } else {
            exe_request_complete(record_p, EXE_SUCCESS, response_data_p);
            free_pscc_connection_response(response_data_p);
        }

        return EXE_SUCCESS;
    }

error: {
        exe_pscc_connection_response_t *response_data_p = (exe_pscc_connection_response_t *)record_p->response_data_p;
        psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;
        exe_eppsd_t *request_data_p = (exe_eppsd_t *)record_p->request_data_p;

        if (EXE_STATE_REQUEST != record_p->state) {
            exe_request_complete(record_p, EXE_FAILURE, NULL);
            free_pscc_connection_response(response_data_p);
            free(request_data_p);
        } else {
            free(new_request_data_p);
        }

        free(client_data_p);
        return EXE_FAILURE;
    }
#endif
}


/**
 * request_set_PDP_context  --  AT+CGDCONT=x,y,z,u,v
 *
 * set PDP context
 *
 * Notice that this request only updates data in exe_pscc_iadb.
 * No calls are to be made to pscc layer, as this is handled in the EPPSD request.
 *
 * TODO: Analyze impact of this choice of design on the ATD relaetd command to activate then PDP context.
 *
 * Description CGDCONT from spec:
 * The set command specifies PDP context parameter values for a PDP context identified by the (local)
 * context identification parameter, <cid>. The number of PDP contexts that may be in a defined state at
 * the same time is given by the range returned by the test command.
 * A special form of the set command, +CGDCONT= <cid> causes the values for context number <cid> to
 * become undefined.
 * The read command returns the current settings for each defined context.
 * The test command returns values supported as a compound value. Each PDP type will have the parameter
 * value range returned on a separate line.
 *
 */
exe_request_result_t request_set_PDP_context(exe_request_record_t *record_p)
{

#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    exe_cgdcont_t *request_data_p = NULL;
    bool           iadb_results   = true;

    if (EXE_STATE_REQUEST == record_p->state) { /* REQUEST PART */
        ATC_LOG_I("-> request");

        request_data_p = (exe_cgdcont_t *)record_p->request_data_p;

        if (NULL == request_data_p) {
            ATC_LOG_E("request_data is NULL. Failed.");
            goto error;
        }

        if (request_data_p->cid == 0) {
            ATC_LOG_E("cid=0, not defined. Failed.");
            goto error;
        }

        /* If only CID submitted, the entry must be deleted from IAdb*/
        if (EXE_PDP_TYPE_INVALID == request_data_p->pdp_type) {
            psccclient_iadb_remove_cid(request_data_p->cid);
            record_p->state = EXE_STATE_RESPONSE;
            return EXE_SUCCESS;
        }

        if (!psccclient_iadb_existing_cid(request_data_p->cid)) {
            /* Set default qos type to use (rel99 = 3G) */
            int default_use_rel97 = false;
            iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_use_rel_97_qos,   &(default_use_rel97));

            /* Must also set default values on qos parameters*/
            iadb_results &= set_default_min_qos_parameters(request_data_p->cid);
            iadb_results &= set_default_req_qos_parameters(request_data_p->cid);
            iadb_results &= set_default_qos_2G_parameters(request_data_p->cid);
        }


        int pscc_pdp_type = pscc_pdp_type_ipv4;

        if (EXE_PDP_TYPE_IPV6 == request_data_p->pdp_type) {
            pscc_pdp_type = pscc_pdp_type_ipv6;
        } else if (EXE_PDP_TYPE_IPV4V6 == request_data_p->pdp_type) {
            pscc_pdp_type = pscc_pdp_type_ipv4v6;
        } else if (EXE_PDP_TYPE_TEST == request_data_p->pdp_type) {
            pscc_pdp_type = pscc_pdp_type_test;
        }

        if (NULL == request_data_p->apn_p) {
            ATC_LOG_I("WARNING no apn set!");
        }

        iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_pdp_type,   &(pscc_pdp_type));
        iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_apn_p,      &(request_data_p->apn_p));
        iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_pdp_address_p,      &(request_data_p->pdp_addr_p));
        iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_d_comp,     &(request_data_p->d_comp));
        iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_h_comp,     &(request_data_p->h_comp));

        if (!iadb_results) {
            ATC_LOG_E("Failed to set some of the parameters. Failed.");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_SUCCESS;
    }

error:
    return EXE_FAILURE;
#endif
}


/**
 * request_set_PDP_minimum_qos -- AT+CGEQMIN
 *
 *  Description from spec:
 * This command allows the TE to specify a UMTS Quality of Service Profile that is used when the ME
 * sends an Activate PDP Context Request message to the network.
 * The set command specifies a profile for the context identified by the (local) context identification
 * parameter, <cid>. The specified profile will be stored in the ME and sent to the network only at
 * activation or ME-initiated modification of the related context. Since this is the same parameter that is
 * used in the +CGDCONT and +CGDSCONT commands, the +CGEQREQ command is effectively an extension to these
 * commands. The QoS profile consists of a number of parameters, each of which may be set to a separate value.
 * The read command returns the current settings for each defined context.
 * The test command returns values supported as a compound value. The parameter value ranges for each PDP
 * type are returned on a separate line.
 */

exe_request_result_t request_set_PDP_minimum_qos(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    bool iadb_results = true;
    exe_pdp_qos_t *request_data_p = NULL;

    ATC_LOG_I("-> request");

    request_data_p = (exe_pdp_qos_t *)record_p->request_data_p;

    if (NULL == request_data_p) {
        ATC_LOG_E("request_data is NULL. Failed.");
        goto error;
    }

    if (0 == request_data_p->cid) {
        ATC_LOG_E("Wrong cid = 0");
        goto error;
    } else {
        if (!psccclient_iadb_get_set_flags_from_cid(request_data_p->cid)) {
            /* creation of account with CGDCONT has not been made for this cid */
            ATC_LOG_E("No account defined for the cid:%d", request_data_p->cid);
            goto error;
        }
    }

    /* Set flag to show that rel99 parameters will be used if a pdp activation is requested. */
    int iadb_use_rel97 = false;
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_use_rel_97_qos,   &(iadb_use_rel97));

    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_traffic_class, &(request_data_p->traffic_class));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_max_bitrate_ul, &(request_data_p->max_bitrate_ul));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_max_bitrate_dl, &(request_data_p->max_bitrate_dl));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_guaranteed_bitrate_ul, &(request_data_p->guaranteed_bitrate_ul));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_guaranteed_bitrate_dl, &(request_data_p->guaranteed_bitrate_dl));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_delivery_order, &(request_data_p->delivery_order));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_max_sdu_size, &(request_data_p->max_sdu_size));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_sdu_error_ratio_p, &(request_data_p->sdu_error_ratio_p));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_residual_ber_p, &(request_data_p->residual_ber_p));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_delivery_err_sdu, &(request_data_p->delivery_err_sdu));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_transfer_delay, &(request_data_p->transfer_delay));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_traffic_handling_priority, &(request_data_p->traffic_handling_priority));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_source_statistics_descriptor, &(request_data_p->source_statistics_descriptor));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_min_signalling_indication, &(request_data_p->signalling_indication));

    if (!iadb_results) {
        ATC_LOG_E("Failed to set some of the parameters in iadb. Failed.");
        goto error;
    }

    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
#endif
}


/**
 * request_set_PDP_required_qos -- AT+CGEQREQ
 *
 *  Description from spec:
 * This command allows the TE to specify a UMTS Quality of Service Profile that is used when the ME
 * sends an Activate PDP Context Request message to the network.
 * The set command specifies a profile for the context identified by the (local) context identification
 * parameter, <cid>. The specified profile will be stored in the ME and sent to the network only at
 * activation or ME-initiated modification of the related context. Since this is the same parameter that is
 * used in the +CGDCONT and +CGDSCONT commands, the +CGEQREQ command is effectively an extension to these
 * commands. The QoS profile consists of a number of parameters, each of which may be set to a separate value.
 * The read command returns the current settings for each defined context.
 * The test command returns values supported as a compound value. The parameter value ranges for each PDP
 * type are returned on a separate line.
 */

exe_request_result_t request_set_PDP_required_qos(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    bool iadb_results = true;
    exe_pdp_qos_t *request_data_p = NULL;

    ATC_LOG_I("-> request");

    request_data_p = (exe_pdp_qos_t *)record_p->request_data_p;

    if (NULL == request_data_p) {
        ATC_LOG_E("request_data is NULL. Failed.");
        goto error;
    }

    if (0 == request_data_p->cid) {
        ATC_LOG_E("Wrong cid = 0");
        goto error;
    } else {
        if (!psccclient_iadb_get_set_flags_from_cid(request_data_p->cid)) {
            /* creation of account with CGDCONT has not been made for this cid */
            ATC_LOG_E("No account defined for the cid:%d", request_data_p->cid);
            goto error;
        }
    }

    /* Set flag to show that rel99 parameters will be used if a pdp activation is requested. */
    int iadb_use_rel97 = false;
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_use_rel_97_qos,   &(iadb_use_rel97));

    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_traffic_class, &(request_data_p->traffic_class));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_max_bitrate_ul, &(request_data_p->max_bitrate_ul));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_max_bitrate_dl, &(request_data_p->max_bitrate_dl));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_guaranteed_bitrate_ul, &(request_data_p->guaranteed_bitrate_ul));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_guaranteed_bitrate_dl, &(request_data_p->guaranteed_bitrate_dl));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_delivery_order, &(request_data_p->delivery_order));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_max_sdu_size, &(request_data_p->max_sdu_size));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_sdu_error_ratio_p, &(request_data_p->sdu_error_ratio_p));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_residual_ber_p, &(request_data_p->residual_ber_p));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_delivery_err_sdu, &(request_data_p->delivery_err_sdu));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_transfer_delay, &(request_data_p->transfer_delay));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_traffic_handling_priority, &(request_data_p->traffic_handling_priority));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_source_statistics_descriptor, &(request_data_p->source_statistics_descriptor));
    iadb_results &= psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_req_signalling_indication, &(request_data_p->signalling_indication));

    if (!iadb_results) {
        ATC_LOG_E("Failed to set some of the parameters in iadb. Failed.");
        goto error;
    }

    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
#endif
}

/**
 * request_set_PDP_send_data -- AT*EUPLINK
 *
 *  Description from spec:
    This command send <size> number of bytes uplink on an activated PDP context defined by <cid>.
    The PDP context MUST be activated prior to this call in order to work properly.

 */
exe_request_result_t request_set_PDP_send_data(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else
    exe_request_result_t      result         = EXE_FAILURE;

    switch (record_p->state) { /* REQUEST PART */
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("EXE_STATE_REQUEST");

        if (!pscc_request_send_data_uplink_call_handle_request(record_p)) {
            ATC_LOG_E("pscc_request_send_data_uplink_call_handle_request failed.");
            goto error;
        }

        if (!create_and_init_psccclient_record_data(record_p,
                EXE_PSCC_ACTION_UNDEFINED,
                EXE_CGACT_ASYNC_MODE_SYNCHRONOUS)) {
            ATC_LOG_E("create_and_init_psccclient_record_data failed.");
            goto error;
        }

        if (EXE_PSCC_SUCCESS != pscc_request_send_data_uplink_call_step_1_send_pscc_set_request(record_p)) {
            ATC_LOG_E("pscc_request_setup_data_call_scenario_2_step_1_send_pscc_set_request failed.");
            goto error;
        }

        record_p->state = EXE_STATE_SEND_DATA_UPLINK_SET_SIZE;
        result = EXE_PENDING;
        break;
    }
    case EXE_STATE_SEND_DATA_UPLINK_SET_SIZE: {
        ATC_LOG_I("EXE_STATE_SEND_DATA_UPLINK_SET_SIZE");
        mpl_msg_resp_t *response_p = psccclient_get_pscc_response_p(record_p);
        EXE_CHECK_GOTO_ERROR(NULL != response_p);
        EXE_CHECK_GOTO_ERROR(pscc_result_ok == response_p->result);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_request_send_data_uplink_call_step_2_send_pscc_generate_uplink_data_request(record_p));

        record_p->state = EXE_STATE_SEND_DATA_UPLINK_SEND_DATA;
        result = EXE_SUCCESS;
        break;
    }
    case EXE_STATE_SEND_DATA_UPLINK_SEND_DATA: {
        ATC_LOG_I("EXE_STATE_SEND_DATA_UPLINK_SEND_DATA");
        int cid = 0;
        int zero = 0;
        mpl_msg_resp_t *response_p = psccclient_get_pscc_response_p(record_p);
        EXE_CHECK_GOTO_ERROR(NULL != response_p);
        EXE_CHECK_GOTO_ERROR(pscc_result_ok == response_p->result);

        /* Reset client tag in iadb for the handled cid */
        EXE_CHECK_GOTO_ERROR(psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid));
        EXE_CHECK_GOTO_ERROR(psccclient_iadb_set(cid, psccclient_iadb_field_at_ct, &zero));

        free(record_p->client_data_p);
        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        result = EXE_SUCCESS;
        break;
    }
    default:
        goto error;
    }

    return result;

error:
    free(record_p->client_data_p);

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
#endif
}

/**
 * request_psdun -- ATD*99#
 *
 *  Description from spec:
    This command requests online data state over the L2 protocol.
 */
exe_request_result_t request_psdun(exe_request_record_t *record_p)
{
#ifdef PSCC_REQUESTS_STUBBED
    ATC_LOG_I("STUBBED");
    return EXE_SUCCESS;
#else

    exe_request_result_t      result             = EXE_FAILURE;
    psccclient_record_data_t *client_data_p      = (psccclient_record_data_t *)record_p->client_data_p;

    /* Ignore all events except for disconnected from sterc */
    if (NULL != client_data_p && NULL != client_data_p->pscc_event_p) {
        if (sterc_event_disconnected != client_data_p->pscc_event_p->id) {
            ATC_LOG_I("Ignoring event: %d", client_data_p->pscc_event_p->id);
            return EXE_SUCCESS;
        }
    }

    switch (record_p->state) { /* REQUEST PART */
    case EXE_STATE_REQUEST: {
        exe_atd_psdun_t *request_data_p = NULL;
        int conn_id = 0;

        ATC_LOG_I("EXE_STATE_REQUEST");

        request_data_p = (exe_atd_psdun_t *)record_p->request_data_p;

        if (NULL == request_data_p) {
            ATC_LOG_E("request_data is NULL. Failed.");
            goto error;
        }

        /* Omitted CID case */
        if (-1 == request_data_p->cid) {
            int max_entries = 0;
            int iadb = 0;

            /* Special case */
            /* Go thru list of CID's and use the first one found. */
            max_entries = psccclient_iadb_get_define_PSCCCLIENT_MAX_IADB_ENTRIES();

            for (iadb = 0; iadb < max_entries; iadb++) {
                /* find all valid cids */
                if (psccclient_iadb_get_set_flags_from_cid(iadb)) {
                    /* use the first CID we can find */
                    request_data_p->cid = iadb;
                    break;
                }
            }
            ATC_LOG_I("CID was omitted, using default %d", request_data_p->cid);
        } else {
            if (!psccclient_iadb_get_set_flags_from_cid(request_data_p->cid)) {
                /* creation of account with CGDCONT has not been made for this cid */
                ATC_LOG_E("No account defined for the cid:%d", request_data_p->cid);
                goto error;
            }
        }

        EXE_CHECK_GOTO_ERROR(create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_UNDEFINED, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS));

        client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

        client_data_p->cid = request_data_p->cid;

        psccclient_iadb_set(request_data_p->cid, psccclient_iadb_field_at_ct, (void *)&record_p);

        /* Check if associated connection already exists */
        psccclient_iadb_get(request_data_p->cid, psccclient_iadb_field_conn_id, &conn_id);

        if (0 == conn_id) {
            /* Create a pscc connection */
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_request_dun_step_1_send_pscc_create_request(record_p));
            record_p->state = EXE_STATE_DUN_PSCC_CREATE;
            result = EXE_PENDING;
        } else {
            /* re-use the existing pscc connection */
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_request_dun_step_4_5_send_sterc_create_request(record_p, conn_id));
            record_p->state = EXE_STATE_STERC_CREATE;

            /* Return EXE_SUCCESS to AT parser before
             * going to online data state. Retain record for later use. */
            request_record_retain(record_p);
            result = EXE_SUCCESS;
        }

        break;
    }
    case EXE_STATE_DUN_PSCC_CREATE: {
        ATC_LOG_I("EXE_STATE_DUN_PSCC_CREATE");
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_request_dun_step_2_3_receive_pscc_create_response(record_p));
        record_p->state = EXE_STATE_DUN_PSCC_SET;
        result = EXE_SUCCESS;
        break;
    }
    case EXE_STATE_DUN_PSCC_SET: {
        int conn_id = 0;

        ATC_LOG_I("EXE_STATE_DUN_PSCC_SET");

        if (NULL != client_data_p) {
            psccclient_iadb_get(client_data_p->cid, psccclient_iadb_field_conn_id, &conn_id);

            if (0 == conn_id) {
                ATC_LOG_E("conn_id not found in iadb!");
                goto error;
            }

            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_request_dun_step_4_5_send_sterc_create_request(record_p, conn_id));
            record_p->state = EXE_STATE_STERC_CREATE;
            result = EXE_SUCCESS;

            /* Return EXE_SUCCESS to AT parser before
             * going to online data state. Retain record for later use. */
            request_record_retain(record_p);
            exe_request_complete(record_p, EXE_SUCCESS, NULL);
        }

        break;
    }
    case EXE_STATE_STERC_CREATE: {
        ATC_LOG_I("EXE_STATE_STERC_CREATE");
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_request_dun_step_6_send_sterc_receive_create_response(record_p));
        record_p->state = EXE_STATE_STERC_DESTROY;
        result = EXE_SUCCESS;
        break;
    }
    case EXE_STATE_STERC_DESTROY: {
        int sterc_handle = 0;
        ATC_LOG_I("EXE_STATE_STERC_DESTROY");

        if (NULL != client_data_p && NULL != client_data_p->pscc_event_p) {
            EXE_CHECK_GOTO_ERROR(sterc_event_disconnected == client_data_p->pscc_event_p->id);
            psccclient_iadb_get(client_data_p->cid, psccclient_iadb_field_sterc_handle, &sterc_handle);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_request_dun_step_7_8_receive_sterc_event(record_p, sterc_handle));
            record_p->state = EXE_STATE_DUN_PSCC_DESTROY;
            result = EXE_SUCCESS;
        }

        break;
    }
    case EXE_STATE_DUN_PSCC_DESTROY: {
        ATC_LOG_I("EXE_STATE_DUN_PSCC_DESTROY");
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_request_dun_step_9_10_receive_sterc_destroy_response(record_p));
        record_p->state = EXE_STATE_DUN_PSCC_DESTROYED;
        result = EXE_SUCCESS;
        break;
    }
    case EXE_STATE_DUN_PSCC_DESTROYED: {
        int zero = 0;
        int cid = 0;

        ATC_LOG_I("EXE_STATE_DUN_PSCC_DESTROYED");
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == pscc_request_dun_step_11_receive_pscc_destroy_response(record_p));

        /* Reset client tag in iadb for the handled cid */
        if (psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
            (void)psccclient_iadb_set(cid, psccclient_iadb_field_at_ct, &zero);
        }

        free(client_data_p);
        result = EXE_SUCCESS;
        exe_event(EXE_UNSOLICITED_ATD, NULL);
        request_record_destroy(record_p);
        break;
    }
    default:
        goto error;
    }

    return result;

error: {
        int zero = 0;
        int cid = 0;

        /* Reset client tag in iadb for the handled cid */
        if (psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
            (void)psccclient_iadb_set(cid, psccclient_iadb_field_at_ct, &zero);
        }

        free(client_data_p);

        if (EXE_STATE_REQUEST != record_p->state) {
            exe_request_complete(record_p, EXE_FAILURE, NULL);
            exe_event(EXE_UNSOLICITED_ATD, NULL);
        }
    }

    return EXE_FAILURE;
#endif
}

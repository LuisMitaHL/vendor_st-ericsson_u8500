/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mpl_config.h" /* libmpl */
#include "sterc_msg.h"

#include "atc_log.h"
#include "atc_parser.h"
#include "atc_context.h"

#include "exe_internal.h"
#include "exe_pscc_client.h"
#include "exe_pscc_requests.h"
#include "exe_pscc_iadb.h"
#include "exe_pscc_util.h"
#include "exe_pscc_requests_dun.h"
#include "exe_request_record_p.h"



/****************************************************************************************************************************************************
 * Establish and tear down DUN
 *
 * description:
 * ---------------------
 * 1)  Send    pscc create request
 * 2)  Receive pscc create response
 * 3)  Send    pscc set request (apn, auth_type, username, password)
 * 4)  Receive pscc set response
 * 5)  Send sterc create request
 * 6)  Receive sterc create response
 * 7)  Receive sterc_disconnected event
 * 8)  Send sterc_destroy request
 * 9)  Receive sterc_destroy response
 * 10) Send pscc_destroy request
 * 11) Receive pscc_destroy response
 *
 ****************************************************************************************************************************************************/

/* 1) Send pscc create request */
exe_pscc_result_t pscc_request_dun_step_1_send_pscc_create_request(exe_request_record_t *record_p)
{
    exe_pscc_result_t    result     = EXE_PSCC_FAILURE;
    pscc_msg_t          *request_p  = NULL;
    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        goto exit;
    }

    request_p->req_create.id = pscc_create;

    if (!psccclient_send_request(record_p, request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        goto exit;
    }

    result = EXE_PSCC_SUCCESS;

exit:
    mpl_msg_free((mpl_msg_t *)request_p);
    return result;
}

/* 2) Receive pscc create response
 * 3) Send    pscc set request (apn, auth_type, username, password)
 */
exe_pscc_result_t pscc_request_dun_step_2_3_receive_pscc_create_response(exe_request_record_t *record_p)
{
    exe_pscc_result_t    result          = EXE_PSCC_FAILURE_DESTROY;
    pscc_msg_t          *request_p       = NULL;
    mpl_msg_resp_t      *response_p      = NULL;
    mpl_param_element_t *param_elem_p    = NULL;
    pscc_auth_method_t   auth_type       = pscc_auth_method_none; /* Default value, No Authentication */
    pscc_pdp_type_t      pdp_type        = pscc_pdp_type_ipv4; /* Default value */
    char                *apn_p           = NULL;
    char                *pdp_address_p   = NULL;
    char                *username_p      = NULL;
    char                *password_p      = NULL;
    char               **request_data    = NULL;
    int                  cid             = 0;

    response_p = psccclient_get_pscc_response_p(record_p);

    if (NULL == response_p) {
        ATC_LOG_E("response_p is NULL");
        goto exit;
    }

    /* Connection id  -> cid  This is to set the initial conn_id so we can map between cid<->conn_id<->client_tag*/
    param_elem_p = mpl_param_list_find(pscc_paramid_connid, response_p->param_list_p);

    if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
        if (!psccclient_iadb_set_conn_id_on_at_ct((uintptr_t)record_p, *(int *)param_elem_p->value_p)) {
            ATC_LOG_E("Failed to map the connection id to any account information. CGDCONT run?!");
            goto exit;
        }
    } else {
        ATC_LOG_E("pscc_paramid_connid not found!");
        goto exit;
    }

    if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
        ATC_LOG_E("can't find cid related to at record!");
        goto exit;
    }

    /* Set parameters (apn, pdp_address, auth_method, userid, password) */
    if (!psccclient_iadb_get(cid, psccclient_iadb_field_apn_p, &apn_p)) {
        ATC_LOG_E("apn_p undefined!");
        goto exit;
    }

    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        goto exit;
    }

    request_p->req_set.id = pscc_set;

    if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_apn, apn_p) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_apn)");
        goto exit;
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_pdp_address_p, &pdp_address_p)) {
        if (strlen(pdp_address_p) > 0) {
            if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_static_ip_address, pdp_address_p) < 0) {
                ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_static_ip_address)");
                goto exit;
            }
        }
    }

    psccclient_user_get(cid, psccclient_user_field_auth_prot, (int *)&auth_type);

    if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_auth_method, (int *)&auth_type) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_auth_method) auth_type: AUTH_TYPE_NO_AUTHENTICATION");
        goto exit;
    }

    if (pscc_auth_method_none != auth_type) { /* UID and PWD are irrelevant if no authentication is performed */
        if (!(psccclient_user_get(cid, psccclient_user_field_user_id_p, &username_p) &&
                psccclient_user_get(cid, psccclient_user_field_passwd_p,  &password_p))) {
            ATC_LOG_E("username or passwd not defined! ");
            goto exit;
        }

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_auth_uid, username_p) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_auth_method)");
            goto exit;
        }

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_auth_pwd, password_p) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_auth_method)");
            goto exit;
        }
    }

    /* Read what kind of qos this cid want to use. The iadb uses  variable psccclient_iadb_field_use_rel_97_qos to keep track
     * on which kind of qos that was set last. */
    int use_97_qos = false;

    if (!psccclient_iadb_get(cid, psccclient_iadb_field_use_rel_97_qos, &use_97_qos)) {
        ATC_LOG_E("QoS version preference undefined!");
        goto exit;
    }

    if (true == use_97_qos) {
        if (!add_2G_qos_parameters_to_pscc_message(cid, request_p)) {
            ATC_LOG_E("Error in parameter conversion!");
            goto exit;
        }
    } else {
        if (!add_qos_parameters_to_pscc_message(cid, request_p)) {
            ATC_LOG_E("Error in parameter conversion!");
            goto exit;
        }
    }

    if (!psccclient_iadb_get(cid, psccclient_iadb_field_pdp_type, (int *)&pdp_type)) {
        ATC_LOG_E("Error could not retrieve PDP type!");
        goto exit;
    }

    if (pscc_pdp_type_ipv4 != pdp_type) {
        ATC_LOG_E("Wrong pdp type received. Only pdp type ipv4 is supported");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_pdp_type, (int *)&pdp_type) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_pdp_type)");
        goto exit;
    }

    if (!psccclient_send_request(record_p, request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        result = EXE_PSCC_FAILURE;
        goto exit;
    }

    result = EXE_PSCC_SUCCESS;

exit:
    mpl_msg_free((mpl_msg_t *)request_p);

    return result;
}
/*
 * 4-5) Send sterc_create request
 */
exe_pscc_result_t pscc_request_dun_step_4_5_send_sterc_create_request(exe_request_record_t *record_p, int cid)
{
    mpl_msg_t           *request_p     = NULL;
    sterc_mode_t         mode          = sterc_mode_psdun;
    char                *tty_p         = NULL;
    AT_ParserState_s    *parser_p      = NULL;
    atc_context_t       *at_context_p  = NULL;

    parser_p = (AT_ParserState_s *)request_record_get_caller_data(record_p);

    if (NULL == parser_p) {
        ATC_LOG_E("failed to get parser_p!");
        goto error;
    }

    at_context_p = atc_context_get_by_context_id(parser_p->ContextID);

    if (NULL == at_context_p) {
        ATC_LOG_E("failed to get at_context_p!");
        goto error;
    }

    if (NULL == at_context_p->conn) {
        ATC_LOG_E("failed to get anchor connection!");
        goto error;
    }

    tty_p = at_context_p->conn->device_name;

    request_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        goto error;
    }

    request_p->req.id = sterc_create;

    if (mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_cid, &cid) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (sterc_paramid_cid)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_mode, &mode) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (sterc_paramid_mode)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_tty, tty_p) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (sterc_paramid_tty)");
        goto error;
    }

    /* Send request */
    if (!psccclient_send_request(record_p, (pscc_msg_t *)request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        goto error;
    }

    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_PSCC_SUCCESS;

error:
    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_PSCC_FAILURE;
}

/* 6) Receive sterc_create response */
exe_pscc_result_t pscc_request_dun_step_6_send_sterc_receive_create_response(exe_request_record_t *record_p)
{
    mpl_msg_resp_t           *response_p    = NULL;
    mpl_param_element_t      *param_elem_p  = NULL;
    psccclient_record_data_t *client_data_p = NULL;
    sterc_result_t            sterc_result  = sterc_result_failed_unspecified;
    int                       sterc_handle  = 0;

    /* Check response */
    response_p = (mpl_msg_resp_t *)psccclient_get_pscc_response_p(record_p);
    EXE_CHECK_GOTO_ERROR(NULL != response_p);

    /* Take care of error scenario */
    if (response_p->result != sterc_result_ok) {
        ATC_LOG_E("response failed: %d", response_p->result);
        goto error;
    }

    /* Fetch sterc handler */
    param_elem_p = mpl_param_list_find(sterc_paramid_handle, response_p->param_list_p);

    if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
        sterc_handle = *((int *)param_elem_p->value_p);
    } else {
        ATC_LOG_E("sterc_paramid_handle not found");
        goto error;
    }

    client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

    if (NULL == client_data_p) {
        ATC_LOG_E("client_data_p is NULL.");
        goto error;
    }

    /* Store response parameters */
    psccclient_iadb_set(client_data_p->cid, psccclient_iadb_field_sterc_handle, &sterc_handle);

    return EXE_PSCC_SUCCESS;

error:
    return EXE_PSCC_FAILURE;
}
/*
 * 7)  Receive sterc_event
 * 8)  Send sterc_destroy request
*/
exe_pscc_result_t pscc_request_dun_step_7_8_receive_sterc_event(exe_request_record_t *record_p, int sterc_handle)
{
    mpl_msg_t    *request_p = NULL;

    request_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        goto error;
    }

    /* Add arguments to function: message, handle */
    request_p->req.id = sterc_destroy;

    if (mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_handle, &sterc_handle) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (sterc_paramid_handle)");
        goto error;
    }

    /* Send request */
    if (!psccclient_send_request(record_p, (pscc_msg_t *)request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        goto error;
    }


    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_PSCC_SUCCESS;


error:
    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_PSCC_FAILURE;
}


/*
* 9) Receive sterc_destroy response
* 10) Send    pscc destroy request
*/
exe_pscc_result_t pscc_request_dun_step_9_10_receive_sterc_destroy_response(exe_request_record_t *record_p)
{
    pscc_msg_t               *request_p     = NULL;
    mpl_msg_resp_t           *response_p    = NULL;
    int cid = 0;

    /* Check response */
    response_p = (mpl_msg_resp_t *)psccclient_get_pscc_response_p(record_p);
    EXE_CHECK_GOTO_ERROR(NULL != response_p);

    /* Take care of error scenario */
    if (response_p->result != sterc_result_ok) {
        ATC_LOG_E("response failed: %d", response_p->result);
        goto error;
    }

    if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
        ATC_LOG_E("can't find cid related to at record!");
        goto error;
    }

    (void)psccclient_iadb_remove_sterc_handle(cid);

    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("failed to allocate memory for request_p");
        goto error;
    }

    request_p->req_destroy.id = pscc_destroy;

    if (!psccclient_send_request(record_p, request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        goto error;
    }

    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_PSCC_SUCCESS;

error:
    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_PSCC_FAILURE;
}

/* 11) Receive pscc_destroy response */
exe_pscc_result_t pscc_request_dun_step_11_receive_pscc_destroy_response(exe_request_record_t *record_p)
{
    mpl_msg_resp_t       *response_p        = NULL;
    exe_request_result_t  result      = EXE_FAILURE;
    int cid = 0;

    response_p = psccclient_get_pscc_response_p(record_p);
    EXE_CHECK_GOTO_ERROR(NULL != response_p);

    if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
        ATC_LOG_E("can't find cid related to at record!");
        goto error;
    }

    /* Take care of error scenario with response returning error code */
    if (response_p->result != pscc_result_ok) {
        ATC_LOG_E("response failed:%d", response_p->result);
        goto error;
    }

    (void)psccclient_iadb_remove_conn_id(cid);

    return EXE_PSCC_SUCCESS;
error:
    return EXE_PSCC_FAILURE;
}

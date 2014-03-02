/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
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

#include "exe_internal.h"
#include "exe_pscc_client.h"
#include "exe_pscc_requests.h"
#include "exe_pscc_iadb.h"
#include "exe_pscc_util.h"
#include "exe_pscc_requests_nap.h"
#include "exe_request_record_p.h"


/********************************************************************
 * Private prototypes
 ********************************************************************
 */


/********************************************************************
 * Private methods
 ********************************************************************
 */


/********************************************************************
 * Public methods
 ********************************************************************
 */

/****************************************************************************************************************************************************
 * Establish NAP
 *
 * description:
 * ---------------------
 * 1)  Send sterc_create request
 * 2)  Receive sterc_create response
 *
 ****************************************************************************************************************************************************/
exe_pscc_result_t pscc_request_nap_send_sterc_create(exe_request_record_t *record_p, int cid, char *dns_server_p, char *wan_device_p)
{
    mpl_msg_t           *request_p  = NULL;
    char                *lan_device_p = "usb0";
    sterc_mode_t         mode = sterc_mode_usb_eth_nap;

    request_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        goto error;
    }

    request_p->req.id = sterc_create;

    /* Add arguments to function: wan_device, lan_device, wan_dns */

    if (mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_cid, &cid) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (sterc_paramid_cid)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_lan_device, lan_device_p) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (sterc_paramid_lan_device)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_mode, &mode) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (sterc_paramid_mode)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_wan_device, wan_device_p) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (sterc_paramid_wan_device)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_wan_dns, dns_server_p) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (sterc_paramid_wan_dns)");
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


/* Receive sterc_create response */
exe_pscc_result_t pscc_request_nap_send_sterc_create_response(exe_request_record_t *record_p)
{
    mpl_msg_resp_t           *response_p    = NULL;
    mpl_param_element_t      *param_elem_p  = NULL;
    psccclient_record_data_t *client_data_p = NULL;
    sterc_result_t            sterc_result  = sterc_result_failed_unspecified;
    int                       sterc_handle  = 0;
    int                       cid           = 0;

    if (NULL == record_p) {
        ATC_LOG_E("record_p is NULL!");
        goto error;
    }

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

    cid = client_data_p->cid;

    /* Store response parameters */
    if (!psccclient_iadb_set(cid, psccclient_iadb_field_sterc_handle, &sterc_handle)) {
        ATC_LOG_E("not able to store sterc_handle in iadb");
        goto error_send_destroy; /* TODO This seems to be an at_core internal error in iadb. Why should it fail it all? */
    }

    return EXE_PSCC_SUCCESS;

error:
    return EXE_PSCC_FAILURE;

error_send_destroy:
    return EXE_PSCC_FAILURE_DESTROY;
}

/****************************************************************************************************************************************************
 * Disconnect NAP
 *
 * description:
 * ---------------------
 * 1)  Send sterc_destroy request
 * 2)  Receive sterc_destroy response
 *
 ****************************************************************************************************************************************************/
exe_pscc_result_t pscc_request_nap_send_sterc_destroy(exe_request_record_t *record_p, int sterc_handle)
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



/****************************************************************************************************************************************************
 * Get NAP status
 *
 * description:
 * ---------------------
 * 1)  Send sterc_get request
 * 2)  Receive sterc_get response
 *
 ****************************************************************************************************************************************************/
exe_pscc_result_t pscc_request_get_nap_status_send_sterc_get(exe_request_record_t *record_p, int sterc_handle)
{
    mpl_msg_t    *request_p = NULL;

    request_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        goto error;
    }

    /* Add arguments to function: message, handle */
    request_p->req.id = sterc_get;

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

/*  Receive sterc_get response */
exe_pscc_result_t pscc_request_get_nap_status_send_sterc_get_response(exe_request_record_t *record_p)
{
    mpl_msg_resp_t *response_p = NULL;
    exe_enap_t     *response_data_p = NULL;
    bool            nap_connected = false;

    if (NULL == record_p) {
        ATC_LOG_E("record_p is NULL!");
        goto error;
    }

    /* Check response */
    response_p = (mpl_msg_resp_t *)psccclient_get_pscc_response_p(record_p);
    EXE_CHECK_GOTO_ERROR(NULL != response_p);

    switch (response_p->result) {
    case sterc_result_ok:
        nap_connected = true;
        break;
    case sterc_result_failed_object_not_found:
        nap_connected = false;
        break;
    default:
        ATC_LOG_E("response failed: %d", response_p->result);
        goto error;
    }

    response_data_p = (exe_enap_t *)record_p->response_data_p;

    if (NULL != response_data_p) {
        free(response_data_p);
    }

    response_data_p = (exe_enap_t *)calloc(1, sizeof(exe_enap_t));

    if (NULL == response_data_p) {
        ATC_LOG_E("response_data_p is NULL");
        goto error;
    }

    response_data_p->state = nap_connected;

    record_p->response_data_p = response_data_p;


    return EXE_PSCC_SUCCESS;

error:
    return EXE_PSCC_FAILURE;
}



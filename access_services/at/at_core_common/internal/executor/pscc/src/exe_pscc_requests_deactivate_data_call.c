/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "exe_internal.h"
#include "exe_pscc_client.h"
#include "exe_pscc_requests.h"
#include "exe_request_record.h"
#include "exe_pscc_requests_deactivate_data_call.h"
#include "atc_log.h"
#include "exe_pscc_util.h"
#include "exe_pscc_iadb.h"

#include "mpl_config.h" /* libmpl */



/********************************************************************
 * Public methods
 ********************************************************************
 */

/****************************************************************************************************************************************************
 * RIL_REQUEST_DEACTIVATE_DATA_CALL: Scenario 0 (successful case - the data call is disconnected)
 *
 * Scenario description:
 * ---------------------
 * 1)  Send    pscc disconnect request
 * 2)  Receive pscc disconnect response
 * 3)  Receive pscc disconnected event
 * 4)  Send    pscc destroy request
 * 5)  Receive pscc destroy response
 * ->  not APPLICABLE ANYMORE -> 6)  Send RIL_RESPONSE_DEACTIVATE_DATA_CALL (RIL_E_SUCCESS)
 * ->  not APPLICABLE ANYMORE -> 7)  Trigger RIL_UNSOL_DATA_CALL_LIST_CHANGED (another scenario)
 *
 ****************************************************************************************************************************************************/


/* 1)  Send    pscc disconnect request  (initial call, direct from exe_request function.)
 */
exe_request_result_t pscc_request_deactivate_data_call_scenario_0_step_1_send_pscc_disconnect_request(exe_request_record_t *record_p)
{
    pscc_msg_t           *request_p   = NULL;
    int                   conn_id     = 0;

    if (NULL == record_p) {
        ATC_LOG_E("record_p is NULL! Failed.");
        goto error;
    }

    if (!psccclient_iadb_get_conn_id_on_at_ct((uintptr_t)record_p, &conn_id)) {
        ATC_LOG_E("Failed to get conn_id on client_tag.");
        goto error;
    }

    /* Shutdown PDP context (this starts with a disconnect request) */
    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (request_p == NULL) {
        ATC_LOG_E("failed to allocate memory for request_p");
        goto error;
    }

    request_p->req_disconnect.id = pscc_disconnect;

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

/* 2)  Receive pscc disconnect response
 */
exe_request_result_t pscc_request_deactivate_data_call_scenario_0_step_2_receive_disconnect_response(exe_request_record_t *record_p)
{
    mpl_msg_resp_t       *response_p  = NULL;

    response_p = psccclient_get_pscc_response_p(record_p);
    EXE_CHECK_GOTO_ERROR(NULL != response_p);

    /* Take care of error scenario */
    if (response_p->result != pscc_result_ok) {
        ATC_LOG_E("response failed: %d", response_p->result);
        goto error;
    }

    return EXE_PSCC_SUCCESS;
error:
    return EXE_PSCC_FAILURE;
}


/* 3)  Receive pscc disconnected event
 * 4)  Send    pscc destroy request
 */
exe_request_result_t pscc_request_deactivate_data_call_scenario_0_step_3_4_receive_disconnected_event(exe_request_record_t *record_p)
{
    pscc_msg_t           *request_p   = NULL;

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


/* 5)  Receive pscc destroy response
 * 6)  ->  not APPLICABLE ANYMORE ->Send RIL_RESPONSE_DEACTIVATE_DATA_CALL (RIL_E_SUCCESS)
 * 7)  ->  not APPLICABLE ANYMORE ->Trigger RIL_UNSOL_DATA_CALL_LIST_CHANGED (another scenario)
 */
exe_request_result_t pscc_request_deactivate_data_call_scenario_0_step_5_6_7_receive_destroy_response(exe_request_record_t *record_p)
{
    mpl_msg_resp_t *response_p  = NULL;
    int cid = 0;
    exe_request_result_t  result      = EXE_FAILURE;

    response_p = psccclient_get_pscc_response_p(record_p);
    EXE_CHECK_GOTO_ERROR(NULL != response_p);

    if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
        ATC_LOG_E("can't find cid related to at record!");
        goto error;
    }

    if (!(EXE_SUCCESS == pscc_request_send_pcd_event(cid, EXE_PCD_RESPONSE_STATUS_DETACHED))) {
        ATC_LOG_E("Unable to issue PCD request");
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

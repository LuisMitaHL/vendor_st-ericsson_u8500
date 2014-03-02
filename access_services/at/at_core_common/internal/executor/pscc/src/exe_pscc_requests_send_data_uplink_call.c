/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/sockios.h>

#include "mpl_config.h" /* libmpl */

#include "atc_log.h"

#include "exe_internal.h"
#include "exe_pscc_client.h"
#include "exe_pscc_requests.h"
#include "exe_pscc_iadb.h"
#include "exe_pscc_util.h"
#include "exe_pscc_requests_send_data_uplink_call.h"
#include "exe_request_record_p.h"

/****************************************************************************************************************************************************
 * Scenario: Generate data uplink using the active PDP context.
 *
 * description:
 * ---------------------
 * 1)  Send pscc_set request
 * 2)  Receive pscc_set response
 * 3)  Send pscc_req_generate_uplink_data request
 * 4)  Receive pscc_req_generate_uplink_data response
 *
 ****************************************************************************************************************************************************/

bool pscc_request_send_data_uplink_call_handle_request(exe_request_record_t *record_p)
{
    bool           result         = false;
    exe_euplink_t *request_data_p = NULL;
    bool           iadb_results   = true;
    int            set_flags      = 0;
    int            pdp_type       = 0;

    request_data_p = (exe_euplink_t *)record_p->request_data_p;
    EXE_CHECK_GOTO_ERROR(NULL != request_data_p);

    /* Check if cid is registered */
    set_flags = psccclient_iadb_get_set_flags_from_cid(request_data_p->cid);

    if (0 == set_flags) {
        ATC_LOG_E("No connection assigned to cid!");
        goto error;
    }

    /* sanity check the pdp type, only pscc_pdp_type_test is acceptable */
    iadb_results &= psccclient_iadb_get(request_data_p->cid,
                                        psccclient_iadb_field_pdp_type,
                                        &pdp_type);

    if (pscc_pdp_type_test != pdp_type) {
        ATC_LOG_E("Wrong PDP type used: %d", pdp_type);
        goto error;
    }

    /* set client tag parameter */
    iadb_results &= psccclient_iadb_set(request_data_p->cid,
                                        psccclient_iadb_field_at_ct,
                                        (int *) &record_p);

    if (!iadb_results) {
        ATC_LOG_E("Failed to set client tag parameter. Failed.");
        goto error;
    }

    /* set data uplink size parameter */
    iadb_results &= psccclient_iadb_set(request_data_p->cid,
                                        psccclient_iadb_field_data_uplink_size,
                                        &(request_data_p->size));

    if (!iadb_results) {
        ATC_LOG_E("Failed to set the parameter in iadb. Failed.");
        goto error;
    }

    result = true;

error:
    return result;
}

exe_pscc_result_t pscc_request_send_data_uplink_call_step_1_send_pscc_set_request(exe_request_record_t *record_p)
{
    exe_request_result_t  result    = EXE_PSCC_FAILURE;
    pscc_msg_t           *request_p = NULL;
    int                   value     = 0;
    int                   cid       = 0;
    int                   conn_id   = 0;
    int                   size      = 0;

    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        goto exit;
    }

    /* Set parameter uplink data size */
    if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
        ATC_LOG_E("can't find cid related to at record!");
        goto exit;
    }

    if (!psccclient_iadb_get_conn_id_on_at_ct((uintptr_t)record_p, &conn_id)) {
        ATC_LOG_I("failed to get the just set conn_id!");
    }

    if (!psccclient_iadb_get(cid, psccclient_iadb_field_data_uplink_size, &size)) {
        ATC_LOG_E("data uplink size undefined!");
        goto exit;
    }

    request_p->req_get.id = pscc_set;

    if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_connid, &conn_id) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_connid)");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_uplink_data_size, &size) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_uplink_data_size)");
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

exe_pscc_result_t pscc_request_send_data_uplink_call_step_2_send_pscc_generate_uplink_data_request(exe_request_record_t *record_p)
{
    exe_request_result_t  result    = EXE_PSCC_FAILURE;
    pscc_msg_t           *request_p = NULL;
    int                   conn_id   = 0;

    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        goto exit;
    }

    if (!psccclient_iadb_get_conn_id_on_at_ct((uintptr_t)record_p, &conn_id)) {
        ATC_LOG_I("failed to get the just set conn_id!");
    }

    request_p->req_get.id = pscc_generate_uplink_data;

    if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_connid, &conn_id) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_connid)");
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

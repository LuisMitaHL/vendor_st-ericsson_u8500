/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>
#include "atc_log.h"
#include "exe_internal.h"
#include "mpl_config.h" /* libmpl */
#include "exe_pscc_events.h"
#include "exe_pscc_requests.h"
#include "exe_pscc_requests_data_call_list.h"
#include "exe_pscc_requests_deactivate_data_call.h"
#include "exe_pscc_util.h"
#include "exe_pscc_client.h"
#include "exe_pscc_iadb.h"
#include "exe_request_record_p.h"


/********************************************************************
 * Private prototypes
 ********************************************************************
 */

static exe_pscc_result_t pscc_request_deactivate_data_call_scenario_0(exe_request_record_t *record_p);
static exe_pscc_result_t pscc_update_data_call_fail_cause_scenario_0_step_1_2_receive_disconnected_event(exe_request_record_t *record_p);
static exe_pscc_result_t pscc_update_data_call_fail_cause_scenario_0_step_3_4_receive_destroy_response(exe_request_record_t *record_p);
static bool psccclient_set_cgev_response_data(exe_request_record_t *record_p,
        int                      cid,
        char                    *ip_address_p,
        char                    *ipv6_address_p,
        int                      pdp_type,
        exe_cgev_response_type_t response_type);


/********************************************************************
 * Private methods
 ********************************************************************
 */

/* setup response data for unsolicited event handling */
static bool psccclient_set_cgev_response_data(exe_request_record_t *record_p,
        int                      cid,
        char                    *ip_address_p,
        char                    *ipv6_address_p,
        int                      pdp_type,
        exe_cgev_response_type_t response_type)
{
    exe_cgev_response_t *response_data_p = (exe_cgev_response_t *)record_p->response_data_p;
    bool                 result          = false;

    if (NULL != response_data_p) {
        free(response_data_p);
    }

    response_data_p = (exe_cgev_response_t *)calloc(1, sizeof(exe_cgev_response_t));

    if (NULL == response_data_p) {
        ATC_LOG_E("response_data_p is NULL. Failed to allocate memory!!!");
        goto exit;
    }

    response_data_p->cid           = cid;
    response_data_p->pdp_type      = pdp_type;
    response_data_p->pdp_addr_p    = ip_address_p;
    response_data_p->ipv6_pdp_addr_p = ipv6_address_p;
    response_data_p->response_type = response_type;

    record_p->response_data_p = response_data_p;

    result = true;
exit:
    return result;
}


/****************************************************************************************************************************************************
 * Update data call fail cause Scenario 0
 *
 * This scenario is not directly related to any RIL request or event. But it will update fail cause and result in a RIL_UNSOL_DATA_CALL_LIST_CHANGED
 *
 * Scenario description:
 * ---------------------
 * 1)  Receive pscc disconnected event (no association with request)
 * 2)  Send    pscc destroy request
 * 3)  Receive pscc destroy response
 * 4)  Trigger UNSOL_DATA_CALL_LIST_CHANGED (another scenario)
 *
 ****************************************************************************************************************************************************/

/**
 * 1)  Receive pscc disconnected event (no association with request)
 * 2)  Send    pscc destroy request
 */
static exe_pscc_result_t pscc_update_data_call_fail_cause_scenario_0_step_1_2_receive_disconnected_event(exe_request_record_t *record_p)
{
    exe_pscc_result_t          result           = EXE_PSCC_FAILURE_DESTROY;
    psccclient_record_data_t  *client_data_p    = (psccclient_record_data_t *)record_p->client_data_p;
    pscc_event_disconnected_t *event_p          = NULL;
    mpl_param_element_t       *param_element_p  = NULL;
    int                        value            = 0;
    int                        cid              = 0;
    int                        pdp_type         = pscc_pdp_type_ipv4;
    char                      *own_ip_addr      = NULL;
    char                      *own_ipv6_addr    = NULL;


    if (NULL == client_data_p || NULL == client_data_p->pscc_event_p) {
        ATC_LOG_E("client_data_p or pscc_event_p is NULL.");
        goto exit;
    }

    event_p = client_data_p->pscc_event_p;

    /* Connection id  -> cid*/
    param_element_p = mpl_param_list_find(pscc_paramid_connid, event_p->param_list_p);

    if (NULL != param_element_p && NULL != param_element_p->value_p) {
        (void)psccclient_iadb_get_cid_on_conn_id(*((int *)param_element_p->value_p), &cid);
    }

    param_element_p = mpl_param_list_find(pscc_paramid_cause, event_p->param_list_p);

    if (NULL != param_element_p && NULL != param_element_p->value_p) {
        client_data_p->fault_cause = *((int *)param_element_p->value_p);
        exe_pscc_send_fail_cause_event((int32_t)client_data_p->fault_cause);
    }

    param_element_p = mpl_param_list_find(pscc_paramid_pdp_type, event_p->param_list_p);

    if (NULL != param_element_p && NULL != param_element_p->value_p) {
        pdp_type = *((int *)param_element_p->value_p);
    }

    param_element_p = mpl_param_list_find(pscc_paramid_own_ip_address, event_p->param_list_p);


    if (NULL != param_element_p && NULL != param_element_p->value_p) {
        own_ip_addr = (char *)calloc(1, 1 + strlen((char *)(param_element_p->value_p)));

        if (NULL == own_ip_addr) {
            ATC_LOG_E("alloc of own_ip_addr failed!");
            goto exit;
        }

        strncpy(own_ip_addr, (char *)(param_element_p->value_p), strlen((char *)(param_element_p->value_p)));
    }

    param_element_p = mpl_param_list_find(pscc_paramid_own_ipv6_address, event_p->param_list_p);


    if (NULL != param_element_p && NULL != param_element_p->value_p) {
        own_ipv6_addr = (char *)calloc(1, 1 + strlen((char *)(param_element_p->value_p)));

        if (NULL == own_ipv6_addr) {
            ATC_LOG_E("alloc of own_ipv6_addr failed!");
            free(own_ip_addr);
            goto exit;
        }

        strncpy(own_ipv6_addr, (char *)(param_element_p->value_p), strlen((char *)(param_element_p->value_p)));
    }


    psccclient_set_cgev_response_data(record_p, cid, own_ip_addr, own_ipv6_addr, pdp_type, CGEV_TYPE_NW_DEACT);

    result = EXE_PSCC_SUCCESS;

    if (!(EXE_SUCCESS == pscc_request_send_pcd_event(cid, EXE_PCD_RESPONSE_STATUS_DETACHED))) {
        ATC_LOG_E("Unable to issue PCD request");
    }

exit:
    return result;
}

/* 3)  Receive pscc destroy response
 * 4)  Trigger UNSOL_DATA_CALL_LIST_CHANGED (another scenario)
 */
static exe_pscc_result_t pscc_update_data_call_fail_cause_scenario_0_step_3_4_receive_destroy_response(exe_request_record_t *record_p)
{
    exe_pscc_result_t    result       = EXE_PSCC_FAILURE;
    mpl_msg_resp_t      *response_p   = NULL;
    mpl_param_element_t *param_elem_p = NULL;
    int cid = 0;

    response_p = psccclient_get_pscc_response_p(record_p);

    if (NULL == response_p) {
        ATC_LOG_E("response is NULL");
        goto exit;
    }

    /* Take care of error scenario with response returning error code */
    if ((pscc_result_failed_unspecified == response_p->result) ||
            (pscc_result_failed_parameter_not_found == response_p->result) ||
            (pscc_result_failed_operation_not_allowed == response_p->result)) {
        ATC_LOG_E("response failed:%d", response_p->result);
        /* No need to try to do handle_error_scenario_data_call_fail_cause here as the previous destroy failed. */
        goto exit;
    }

    /* Get cid from connection id */
    param_elem_p = mpl_param_list_find(pscc_paramid_connid, response_p->param_list_p);

    if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
        if (!psccclient_iadb_get_cid_on_conn_id(*((int *)param_elem_p->value_p), &cid)) {
            ATC_LOG_E("psccclient_iadb_get_cid_on_conn_id failed");
            goto exit;
        }
    } else {
        ATC_LOG_E("no connid found");
        goto exit;
    }

    (void)psccclient_iadb_remove_conn_id(cid);
    ATC_LOG_I("Removed cid=%d from iadb", cid);

    result = EXE_PSCC_SUCCESS;

exit:
    return result;
}


exe_request_result_t internal_request_disconnected_failure_cause(exe_request_record_t *record_p)
{
    exe_pscc_result_t result = EXE_PSCC_FAILURE;
    psccclient_record_data_t *client_data_p   = (psccclient_record_data_t *)request_record_get_client_data(record_p);
    exe_cgev_response_t      *response_data_p = (exe_cgev_response_t *)request_record_get_response_data(record_p);
    mpl_param_element_t       *param_element_p  = NULL;
    bool destroy = true;

    switch (record_p->state) {
    case EXE_REQUEST:
        result = pscc_update_data_call_fail_cause_scenario_0_step_1_2_receive_disconnected_event(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);

        param_element_p = mpl_param_list_find(pscc_paramid_reason, client_data_p->pscc_event_p->param_list_p);

        /* If the reason of the disconnect was a reset, we assume that pscc will shut down and dont send any destroy */
        if (NULL != param_element_p && NULL != param_element_p->value_p && *((pscc_reason_t *)param_element_p->value_p) ==
                pscc_reason_pdp_context_nw_deactivated_reset) {
            destroy = false;
        }

        if (destroy) {
            /* Send pscc destroy request */
            result = exe_pscc_send_destroy_request(record_p);
            EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
            record_p->state = EXE_STATE_DISCONNECTED_FAILURE_CAUSE_DESTROY;
        } else {
            /* If we shouldnt destroy then we are finished, also the response data can be taken from the disconnect response */
            response_data_p = (exe_cgev_response_t *)request_record_get_response_data(record_p);
            goto exit;
        }

        break;
    case EXE_STATE_DISCONNECTED_FAILURE_CAUSE_DESTROY:
        result = pscc_update_data_call_fail_cause_scenario_0_step_3_4_receive_destroy_response(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        goto exit;
    }

    return EXE_SUCCESS;

exit:
    /* Channel disconnected, send cgev unsolicited. */
    EXE_CHECK_GOTO_ERROR(NULL != response_data_p);

    if (NULL != response_data_p->pdp_addr_p) {
        if (!(EXE_SUCCESS == pscc_request_send_cgev_event(response_data_p->cid, response_data_p->pdp_addr_p, response_data_p->pdp_type, response_data_p->response_type))) {
            ATC_LOG_E("Unable to issue CGEV request for IPv4 PDP address");
        }
        free(response_data_p->pdp_addr_p);
    }

    if (NULL != response_data_p->ipv6_pdp_addr_p) {
        if (!(EXE_SUCCESS == pscc_request_send_cgev_event(response_data_p->cid, response_data_p->ipv6_pdp_addr_p, response_data_p->pdp_type, response_data_p->response_type))) {
            ATC_LOG_E("Unable to issue CGEV request for IPv6 PDP address");
        }
        free(response_data_p->ipv6_pdp_addr_p);
    }

    free(response_data_p);
    free(client_data_p);
    request_record_destroy(record_p);
    return EXE_SUCCESS;

error:

    if (EXE_PSCC_FAILURE_DESTROY == result) {
        result = exe_pscc_send_destroy_request(record_p);
        EXE_CHECK_GOTO_ERROR(EXE_PSCC_SUCCESS == result);
        record_p->state = EXE_STATE_DISCONNECTED_FAILURE_CAUSE_DESTROY;
    } else {
        if (NULL != response_data_p) {
            free(response_data_p->pdp_addr_p);
        }

        free(response_data_p);
        free(client_data_p);
        request_record_destroy(record_p);
    }

    return EXE_PSCC_FAILURE;
}


/********************************************************************
 * Public methods
 ********************************************************************
 */

bool exe_pscc_event_handler(psccclient_t *psccclient_p, mpl_msg_event_t *event_p, int conn_id)
{
    int record_handle = 0;
    exe_request_record_t *record_p = NULL;
    psccclient_record_data_t *client_data_p = NULL;
    int cid = 0;

    switch (event_p->id) {
    case pscc_event_disconnected:
        record_p = request_record_create(EXE_PSCC_EVENT_DISCONNECTED,
                                         internal_request_disconnected_failure_cause,
                                         NULL, NULL, NULL, &record_handle);
        EXE_CHECK_GOTO_ERROR(NULL != record_p);

        psccclient_iadb_set_at_ct_on_conn_id((uintptr_t)record_p, conn_id);
        EXE_CHECK_GOTO_ERROR(create_and_init_psccclient_record_data(record_p, EXE_PSCC_ACTION_UNSOLICITED, EXE_CGACT_ASYNC_MODE_SYNCHRONOUS));
        client_data_p = record_p->client_data_p;
        client_data_p->psccclient_p = psccclient_p;
        EXE_CHECK_GOTO_ERROR(psccclient_set_pscc_event_p(record_p, event_p));

        internal_request_disconnected_failure_cause(record_p);
        return true;

    case sterc_event_disconnected:
        return false;
    default:
        return false;
    }

error:

    if (NULL != record_p) {
        request_record_destroy(record_p);
        record_p = NULL;
    }

    return false;
}


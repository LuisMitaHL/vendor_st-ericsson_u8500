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
#include "exe_pscc_requests_setup_data_call.h"
#include "exe_request_record_p.h"


/* #define USE_31_BIT_NETMASK_RFC_3021 */

/*******************************************************************
 * Private prototypes
 ********************************************************************
 */
static bool psccclient_set_connection_response_data(exe_request_record_t *record_p,
        int                   pdp_type,
        char                 *netdev_name_p,
        char                 *ip_address_p,
        char                 *ipv6_address_p,
        char                 *gw_address_p,
        char                 *dns_address_1_p,
        char                 *ipv6_dns_address_1_p,
        char                 *dns_address_2_p,
        char                 *ipv6_dns_address_2_p,
        int                   connection_status,
        int                   activation_cause);

/********************************************************************
 * Private methods
 ********************************************************************
 */
static bool psccclient_set_connection_response_data(exe_request_record_t *record_p,
        int                   pdp_type,
        char                 *netdev_name_p,
        char                 *ip_address_p,
        char                 *ipv6_address_p,
        char                 *gw_address_p,
        char                 *dns_address_1_p,
        char                 *ipv6_dns_address_1_p,
        char                 *dns_address_2_p,
        char                 *ipv6_dns_address_2_p,
        int                   connection_status,
        int                   activation_cause)
{
    exe_pscc_connection_response_t *response_data_p = (exe_pscc_connection_response_t *)record_p->response_data_p;
    bool                  result          = false;
    char                 *net_mask_p      = NULL;

    ATC_LOG_I("enter");

    if (NULL != response_data_p) {
        free_pscc_connection_response(response_data_p);
    }

    response_data_p = (exe_pscc_connection_response_t *)calloc(1, sizeof(exe_pscc_connection_response_t));

    if (NULL == response_data_p) {
        ATC_LOG_E("response_data_p is NULL. Failed to allocate memory!!!");
        goto exit;
    }

    net_mask_p = (char *)calloc(1, 16); /*255.255.255.255 == 15 chars*/

    if (NULL == net_mask_p) {
        ATC_LOG_E("failed allocate memory for (netmask)");
        free(response_data_p);
        goto exit;
    }

    switch (pdp_type) {
    /* intentional fall-through*/
    case pscc_pdp_type_test:
    case pscc_pdp_type_ipv4:
    case pscc_pdp_type_ipv6:
    case pscc_pdp_type_ipv4v6:
        break;
    default:
        ATC_LOG_E("received unknown pdp type: %d", pdp_type);
        free(net_mask_p);
        free(response_data_p);
        goto exit;
    }

    if ((pscc_pdp_type_ipv4 == pdp_type) || (NULL != ip_address_p)) {
        strcpy(net_mask_p, "255.255.255.255");
    } else {
        strcpy(net_mask_p, "0");
    }

    response_data_p-> netdev_name_p    = netdev_name_p;
    response_data_p->ip_address_p      = ip_address_p;
    response_data_p->ipv6_address_p      = ipv6_address_p;
    response_data_p->default_gateway_p = gw_address_p;
    response_data_p->dns_server_p      = dns_address_1_p;
    response_data_p->ipv6_dns_server_p   = ipv6_dns_address_1_p;
    response_data_p->dns_server_2_p    = dns_address_2_p;
    response_data_p->ipv6_dns_server_2_p = ipv6_dns_address_2_p;
    /* hard coded here.Should be extracted from the system, but pscc/mal does not support this*/
    response_data_p->subnet_netmask_p  = net_mask_p;
    response_data_p->p_cscf_server_p   = NULL;
    response_data_p->p_cscf_server_p   = NULL;
    response_data_p->mtu               = 0;
    response_data_p->connection_status = connection_status;
    response_data_p->activation_cause = activation_cause;
    record_p->response_data_p = response_data_p;

    result = true;
exit:
    return result;
}



/********************************************************************
 * Public methods
 ********************************************************************
 */


/****************************************************************************************************************************************************
 * REQUEST_SETUP_DATA_CALL: Scenario 0 (successful case - the data call is established)
 *
 * Scenario description:
 * ---------------------
 * 1)  Send    pscc create request
 * 2)  Receive pscc create response
 * 3)  Send    pscc set request (apn, auth_type, username, password)
 * 4)  Receive pscc set response
 * 5)  Send    pscc connect request
 * 6)  Receive pscc connect response
 * 7)  Receive pscc connecting event
 * 8)  Receive pscc connected event
 * 9)  Send    pscc get request (ip, net device name, cid)
 * 10) Receive pscc get response
 * 11) Send RESPONSE_SETUP_DATA_CALL (EXE_SUCCESS)
 * 12) Trigger UNSOL_DATA_CALL_LIST_CHANGED (another scenario)
 *
 * NOTE: Step 1-7 shared with scenario 1
 *
 ****************************************************************************************************************************************************/

/****************************************************************************************************************************************************
 * REQUEST_SETUP_DATA_CALL: Scenario 1 (unsuccessful case - the data call is not established)
 *
 * Scenario description:
 * ---------------------
 * 1)  Send    pscc create request
 * 2)  Receive pscc create response
 * 3)  Send    pscc set request (apn, auth_type, username, password)
 * 4)  Receive pscc set response
 * 5)  Send    pscc connect request
 * 6)  Receive pscc connect response
 * 7)  Receive pscc connecting event
 * 8)  Receive pscc disconnected event
 * 9)  --> NOT APPLICABLE -->Send    pscc get request (cause of disconnection)
 * 10) --> CHANGED        -->Receive pscc get response --> This is now the receiving end of the disconnect event
 * 11) Send    pscc destroy request
 * 12) Receive pscc destroy response
 * 13) Send RESPONSE_SETUP_DATA_CALL (EXE_FAILURE),
 * 14) Trigger UNSOL_DATA_CALL_LIST_CHANGED (another scenario)
 *
 * NOTE: Step 1-7 shared with scenario 0
 *
 *****************************************************************************************************************************************************/


/********************************************************************************************************************************************************************
 * HANDLER FUNCTIONS COMMON FOR BOTH SCENARIOS                                                                                                                      *
 ********************************************************************************************************************************************************************/

/* 1) Send pscc create request */
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_1_step_1_send_pscc_create_request(exe_request_record_t *record_p)
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
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_1_step_2_3_receive_pscc_create_response(exe_request_record_t *record_p)
{
    exe_pscc_result_t    result          = EXE_PSCC_FAILURE_DESTROY;
    pscc_msg_t          *request_p       = NULL;
    mpl_msg_resp_t      *response_p      = NULL;
    mpl_param_element_t *param_elem_p    = NULL;
    pscc_auth_method_t   auth_type       = pscc_auth_method_none; /* Default value, No Authentication */
    int                  value           = 0;
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
            ATC_LOG_E("Failed to map the connection id to any account information. CGDCONT runned?!");
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

    if (!psccclient_iadb_get(cid, psccclient_iadb_field_pdp_type, &value)) {
        ATC_LOG_E("Error could not retrieve PDP type!");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_pdp_type, &value) < 0) {
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


/* 4) Receive pscc set response
 * 5) Send    pscc connect request
 */
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_1_step_4_5_receive_pscc_set_response(exe_request_record_t *record_p)
{
    exe_pscc_result_t    result    = EXE_PSCC_FAILURE_DESTROY;
    pscc_msg_t          *request_p = NULL;
    int cid = -1;

    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        goto exit;
    }

    request_p->req_connect.id = pscc_connect;

    if (!psccclient_send_request(record_p, request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        goto exit;
    }

    if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
        ATC_LOG_E("can't find cid related to at record!");
        goto exit;
    }

    if (!(EXE_SUCCESS == pscc_request_send_pcd_event(cid, EXE_PCD_RESPONSE_STATUS_ATTACHED))) {
        ATC_LOG_E("Unable to issue PCD request");
        goto exit;
    }

    result = EXE_PSCC_SUCCESS;

exit:
    mpl_msg_free((mpl_msg_t *)request_p);
    return result;
}


/* 6) Receive pscc connect response */
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_1_step_6_receive_pscc_connect_response(exe_request_record_t *record_p)
{
    exe_pscc_result_t    result     = EXE_PSCC_SUCCESS;
    mpl_msg_resp_t      *response_p = NULL;

    response_p = psccclient_get_pscc_response_p(record_p);

    if (NULL == response_p) {
        ATC_LOG_E("response_p is NULL");
        goto exit;
    }

    /* Take care of error scenario with response returning error code */
    if (response_p->result != pscc_result_ok) {
        ATC_LOG_E("response failed: %d", response_p->result);
        result = EXE_PSCC_FAILURE_DESTROY;
    }

exit:
    return result;
}


/********************************************************************************************************************************************************************
 * HANDLER FUNCTIONS SPECIFIC FOR SCENARIO 0 (successful establishment of data call)                                                                                *
 ********************************************************************************************************************************************************************/

/* 8) Receive pscc connected event
 * 9) Send    pscc get request (ip, net device name, cid)
 */
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_step_8_9_receive_pscc_connected_event(exe_request_record_t *record_p)
{
    exe_pscc_result_t    result     = EXE_PSCC_FAILURE_DESTROY;
    pscc_msg_t          *request_p  = NULL;
    pscc_event_connected_t *event_p          = NULL;
    mpl_param_element_t   *param_elem_p = NULL;
    psccclient_record_data_t  *client_data_p    = (psccclient_record_data_t *)record_p->client_data_p;
    bool iadb_results = true;
    int cid = 0;
    int activation_cause = 0x00;
    event_p = client_data_p->pscc_event_p;

    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        result = EXE_PSCC_FAILURE;
        goto exit;
    }


    if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
        ATC_LOG_E("can't find cid related to at record!");
        goto exit;
    }

    /* Get activation cause */
    param_elem_p = mpl_param_list_find(pscc_paramid_cause, event_p->param_list_p);

    if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
        activation_cause = *(int *)(param_elem_p->value_p);
    }

    iadb_results &= psccclient_iadb_set(cid, psccclient_iadb_field_activation_cause, &(activation_cause));

    if (!iadb_results) {
        ATC_LOG_E("Failed to set activation cause parameter in iadb. Failed.");
        goto exit;
    }

    request_p->req_get.id = pscc_get;

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_pdp_type, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_pdp_type)");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_netdev_name, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_netdev_name)");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ip_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_own_ip_address)");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ipv6_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_own_ipv6_address)");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_gw_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_gw_address)");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_dns_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_dns_address)");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_ipv6_dns_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_ipv6_dns_address)");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_secondary_dns_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_secondary_dns_address)");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_ipv6_secondary_dns_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_ipv6_secondary_dns_address)");
        goto exit;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_connection_status, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_connection_status)");
        goto exit;
    }

    if (!psccclient_send_request(record_p, request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        goto exit;
    }

    result = EXE_PSCC_SUCCESS;

exit:
    mpl_msg_free((mpl_msg_t *)request_p);
    return result;
}


/* 10) Receive pscc get response
 * ---> Not applicable -> 11) Send RIL_RESPONSE_SETUP_DATA_CALL
 * ---> Not applicable -> 12) Trigger RIL_UNSOL_DATA_CALL_LIST_CHANGED (another scenario)
 */

exe_pscc_result_t pscc_request_setup_data_call_scenario_0_step_10_11_12_receive_pscc_get_response(exe_request_record_t *record_p)
{
    exe_pscc_result_t      result     = EXE_PSCC_FAILURE_DESTROY;
    mpl_msg_resp_t        *response_p        = NULL;
    mpl_param_element_t   *param_element_p   = NULL;
    char                  *netdev_name_p      = NULL;
    char                  *ip_address_p      = NULL;
    char                  *ipv6_address_p    = NULL;
    char                  *gw_address_p      = NULL;
    char                  *dns_address_1_p   = NULL;
    char                  *ipv6_dns_address_1_p   = NULL;
    char                  *dns_address_2_p   = NULL;
    char                  *ipv6_dns_address_2_p   = NULL;
    int                    ip_family         = AF_INET;
    int                    pdp_type          = pscc_pdp_type_ipv4; /* IPv4 default*/
    int                    connection_status = 0;
    mpl_param_element_t   *param_elem_p      = NULL;
    unsigned char          dummy[sizeof(struct in6_addr)];
    psccclient_record_data_t *client_data_p  = (psccclient_record_data_t *)record_p->client_data_p;
    exe_cgact_async_mode_t async_mode        = client_data_p->async_mode;
    int activation_cause = 0x00;
    int cid = 0;

    /*connid, devname, ip_address, gw_address, dns_name1, dnsname2, netmask*/

    response_p = psccclient_get_pscc_response_p(record_p);
    EXE_CHECK_GOTO_ERROR(NULL != response_p);

    /* Get pdp type */
    param_elem_p = mpl_param_list_find(pscc_paramid_pdp_type, response_p->param_list_p);

    if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
        pdp_type = *(int *)(param_elem_p->value_p);
    }


    switch (pdp_type) {
    /* intentional fall-through*/
    case pscc_pdp_type_test:
    case pscc_pdp_type_ipv4:
    case pscc_pdp_type_ipv6:
    case pscc_pdp_type_ipv4v6:
        break;
    default:
        ATC_LOG_E("Received invalid pdp type!");
        goto error;
    }

    /* Get netdev_name_p */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_netdev_name, &netdev_name_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (netdev_name)");
        goto error;
    }

    /* Get own ip address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_own_ip_address, &ip_address_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (ip_address)");
    }

    /* Get own ipv6 address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_own_ipv6_address, &ipv6_address_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (ipv6_address)");
    }

    if ((NULL == ip_address_p) && (NULL == ipv6_address_p)) {
        ATC_LOG_E("Neither ip_address nor ipv6_address found");
        goto error;
    }

    if (NULL != ip_address_p) {
        if ((inet_pton(AF_INET, ip_address_p, dummy)) <= 0) {
            ATC_LOG_E("inet_pton failed! (pscc_paramid_own_ip_address==%s)", ip_address_p);
            goto error;
        }
    }

    if (NULL != ipv6_address_p) {
        if ((inet_pton(AF_INET6, ipv6_address_p, dummy)) <= 0) {
            ATC_LOG_E("inet_pton failed! (pscc_paramid_own_ipv6_address==%s)", ipv6_address_p);
            goto error;
        }
    }


    /* Get gw address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_gw_address, &gw_address_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (pscc_paramid_gw_address)");
        goto error;
    }

    if ((NULL != gw_address_p) && (inet_pton(ip_family, gw_address_p, dummy) <= 0)) {
        ATC_LOG_E("inet_pton failed! (pscc_paramid_gw_address==%s)", gw_address_p);
        goto error;
    }

    /* Get dns address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_dns_address, &dns_address_1_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (dns_address_1)");
    }

    /* Get ipv6 dns address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_ipv6_dns_address, &ipv6_dns_address_1_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (ipv6_dns_address_1)");
    }

    if ((NULL == dns_address_1_p) && (NULL == ipv6_dns_address_1_p)) {
        ATC_LOG_E("Neither dns_address_1_p nor ipv6_dns_address_1_p found");
        goto error;
    }

    if (NULL != dns_address_1_p) {
        if ((inet_pton(AF_INET, dns_address_1_p, dummy)) <= 0) {
            ATC_LOG_E("inet_pton failed! (dns_address_1==%s)", dns_address_1_p);
            goto error;
        }
    }

    if (NULL != ipv6_dns_address_1_p) {
        if ((inet_pton(AF_INET6, ipv6_dns_address_1_p, dummy)) <= 0) {
            ATC_LOG_E("inet_pton failed! (ipv6_dns_address_1_p==%s)", ipv6_dns_address_1_p);
            goto error;
        }
    }

    /* Get dns 2 address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_secondary_dns_address, &dns_address_2_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (dns_address_2)");
    }

    if ((NULL != dns_address_2_p) && (inet_pton(AF_INET, dns_address_2_p, dummy)) <= 0) {
        ATC_LOG_E("inet_pton failed! (dns_address_2==%s)", dns_address_2_p);
    }

    /* Get ipv6 dns 2 address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_ipv6_secondary_dns_address, &ipv6_dns_address_2_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (ipv6_dns_address_2_p)");
    }

    if ((NULL != ipv6_dns_address_2_p) && (inet_pton(AF_INET6, ipv6_dns_address_2_p, dummy)) <= 0) {
        ATC_LOG_E("inet_pton failed! (ipv6_dns_address_2_p==%s)", ipv6_dns_address_2_p);
    }

    /* Get connection status */
    param_elem_p = mpl_param_list_find(pscc_paramid_connection_status, response_p->param_list_p);

    if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
        connection_status = *(int *)(param_elem_p->value_p);
    }

    if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
        ATC_LOG_E("can't find cid related to at record!");
        goto error;
    }
    if (!psccclient_iadb_get(cid, psccclient_iadb_field_activation_cause, &(activation_cause))) {
        ATC_LOG_E("Activation cause undefined!");
        goto error;
    }
    (void)psccclient_set_connection_response_data(record_p, pdp_type, netdev_name_p, ip_address_p, ipv6_address_p,
        gw_address_p, dns_address_1_p, ipv6_dns_address_1_p, dns_address_2_p, ipv6_dns_address_2_p, connection_status, activation_cause);

    /* TODO Move this to the requester. */
    if (EXE_CGACT_ASYNC_MODE_ASYNCHRONOUS == async_mode) {
        int cid;

        if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
            ATC_LOG_E("can't find cid related to at record!");
            goto error;
        }

        /* Send +CGEV Event for IPv4 address */
        if (NULL != ip_address_p) {
            if (!(EXE_SUCCESS == pscc_request_send_cgev_event(cid, ip_address_p, pdp_type, CGEV_TYPE_ME_ACT))) {
                ATC_LOG_E("Unable to issue CGEV request");
                goto error;
            }
        }

        /* Send +CGEV Event for IPv6 address */
        if (NULL != ipv6_address_p) {
            if (!(EXE_SUCCESS == pscc_request_send_cgev_event(cid, ipv6_address_p, pdp_type, CGEV_TYPE_ME_ACT))) {
                ATC_LOG_E("Unable to issue CGEV request");
                goto error;
            }
        }
    }
    result = EXE_PSCC_SUCCESS;
    return result;

error:
    free(netdev_name_p);
    if (NULL != ip_address_p)
        free(ip_address_p);
    if (NULL != ipv6_address_p)
        free(ipv6_address_p);
        free(gw_address_p);
    if (NULL != dns_address_1_p)
        free(dns_address_1_p);
    if (NULL != ipv6_dns_address_1_p)
        free(ipv6_dns_address_1_p);
    if (NULL != dns_address_2_p)
        free(dns_address_2_p);
    if (NULL != ipv6_dns_address_2_p)
        free(ipv6_dns_address_2_p);
    return result;
}

/********************************************************************************************************************************************************************
 * HANDLER FUNCTIONS SPECIFIC FOR SCENARIO 1 (unsuccessful establishment of data call)                                                                                *
 ********************************************************************************************************************************************************************/

/* 10) Receive pscc get response
 * 11) Send    pscc destroy request
 */
exe_pscc_result_t pscc_request_setup_data_call_scenario_1_step_10_11_receive_pscc_get_response(exe_request_record_t *record_p)
{
    exe_pscc_result_t          result           = EXE_PSCC_FAILURE;
    psccclient_record_data_t  *client_data_p    = (psccclient_record_data_t *)record_p->client_data_p;
    mpl_msg_resp_t            *response_p       = NULL;
    pscc_msg_t                *request_p        = NULL;
    pscc_event_disconnected_t *event_p          = NULL;
    mpl_param_element_t       *param_element_p  = NULL;
    error_cause_t              cause            = 0;

    event_p = client_data_p->pscc_event_p;

    param_element_p = mpl_param_list_find(pscc_paramid_cause, event_p->param_list_p);

    if (NULL != param_element_p && NULL != param_element_p->value_p) {
        client_data_p->fault_cause = *((int *)param_element_p->value_p);
        exe_pscc_send_fail_cause_event((int32_t)client_data_p->fault_cause);
    }

    /* Send pscc destroy request */
    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("failed to allocate memory for request_p");
        goto exit;
    }

    request_p->req_destroy.id = pscc_destroy;

    if (!psccclient_send_request(record_p, request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        goto exit;
    }

    result = EXE_PSCC_SUCCESS;

exit:
    mpl_msg_free((mpl_msg_t *)request_p);
    return result;
}

/* 12) Receive pscc destroy response
 * 13) Send RESPONSE_SETUP_DATA_CALL (EXE_FAILURE),
 * 14) Trigger UNSOL_DATA_CALL_LIST_CHANGED (another scenario)
 */
exe_pscc_result_t pscc_request_setup_data_call_scenario_1_step_12_13_14_receive_pscc_destroy_response(exe_request_record_t *record_p)
{
    int cid = 0;
    psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)record_p->client_data_p;
    exe_cgact_async_mode_t async_mode = client_data_p->async_mode;

    if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
        ATC_LOG_E("psccclient_iadb_get_cid_on_at_ct failed");
    }

    /* TODO This function seems unnecessary, just make the following line happen in the request function when terminating. */
    (void)psccclient_iadb_remove_conn_id(cid);

    /* TODO Move this to the requester. */
    if (EXE_CGACT_ASYNC_MODE_SYNCHRONOUS == async_mode) {
        ATC_LOG_I("executing in synchronous mode");

        return EXE_PSCC_FAILURE;
        /* Now the PS connection is completely destroyed. This means that the connection object can be freed. */
    } else if (EXE_CGACT_ASYNC_MODE_ASYNCHRONOUS == async_mode) {
        ATC_LOG_I("executing in asynchronous mode");

        if (psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
            if (!(EXE_SUCCESS == pscc_request_send_pcd_event(cid, EXE_PCD_RESPONSE_STATUS_DETACHED))) {
                ATC_LOG_E("Unable to issue PCD request for cid=%d", cid);
            }
        }

        return EXE_PSCC_FAILURE;
    }

    return EXE_PSCC_SUCCESS; /* the request function went OK, but the overall PS-data call setup failed ...*/
}


/****************************************************************************************************************************************************
 * Get PDP connection status
 *
 * description:
 * ---------------------
 * 1)  Send pscc_get request
 * 2)  Receive pscc_get response
 *
 ****************************************************************************************************************************************************/

/* Send    pscc get request */
exe_pscc_result_t pscc_request_setup_data_call_send_pscc_get(exe_request_record_t *record_p)
{
    pscc_msg_t          *request_p  = NULL;

    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("mpl_req_msg_alloc failed!");
        goto error;
    }

    request_p->req_get.id = pscc_get;

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_pdp_type, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_pdp_type)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_netdev_name, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_netdev_name)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ip_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_own_ip_address)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ipv6_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_own_ipv6_address)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_gw_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_gw_address)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_dns_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_dns_address)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_ipv6_dns_address, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_ipv6_dns_address)");
        goto error;
    }

    if (mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_connection_status, NULL) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_dns_address)");
        goto error;
    }

    if (!psccclient_send_request(record_p, request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        goto error;
    }

    ATC_LOG_I("-> request");

    mpl_msg_free((mpl_msg_t *)request_p);
    return  EXE_PSCC_SUCCESS;

error:
    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_PSCC_FAILURE;
}


/* 10) Receive pscc_get response */
exe_pscc_result_t pscc_request_setup_data_call_send_pscc_get_response(exe_request_record_t *record_p)
{
    exe_request_result_t   result            = EXE_PSCC_FAILURE;
    mpl_msg_resp_t        *response_p        = NULL;
    mpl_param_element_t   *param_element_p   = NULL;
    char                  *netdev_name_p     = NULL;
    char                  *ip_address_p      = NULL;
    char                  *ipv6_address_p    = NULL;
    char                  *gw_address_p      = NULL;
    char                  *dns_address_1_p   = NULL;
    char                  *ipv6_dns_address_1_p   = NULL;
    char                  *dns_address_2_p   = NULL;
    char                  *ipv6_dns_address_2_p   = NULL;
    int                    ip_family         = AF_INET;
    int                    pdp_type          = pscc_pdp_type_ipv4; /* IPv4 default*/
    int                    connection_status = 0;
    mpl_param_element_t   *param_elem_p      = NULL;
    unsigned char          dummy[sizeof(struct in6_addr)];
    int activation_cause = 0x00;
    int cid = 0;

    ATC_LOG_I("<- response");
    /*connid, devname, ip_address, gw_address, dns_name1, dnsname2, netmask*/

    response_p = psccclient_get_pscc_response_p(record_p);
    EXE_CHECK_GOTO_ERROR(NULL != response_p);

    /* Take care of error scenario */
    if (response_p->result != pscc_result_ok) {
        ATC_LOG_E("response failed: %d", response_p->result);
        goto error;
    }

    /* Get pdp type */
    param_elem_p = mpl_param_list_find(pscc_paramid_pdp_type, response_p->param_list_p);

    if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
        pdp_type = *(int *)(param_elem_p->value_p);
    }


    switch (pdp_type) {
    /* intentional fall-through*/
    case pscc_pdp_type_test:
    case pscc_pdp_type_ipv4:
    case pscc_pdp_type_ipv6:
    case pscc_pdp_type_ipv4v6:
        break;
    default:
        ATC_LOG_E("Received invalid pdp type!");
        goto error;
    }

    /* Get netdev name: e.i. gprs0, caif0 */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_netdev_name, &netdev_name_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (netdev_name)");
        goto error;
    }

    /* Get own ip address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_own_ip_address, &ip_address_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (ip_address)");
    }

    /* Get own ipv6 address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_own_ipv6_address, &ipv6_address_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (ipv6_address)");
    }

    if ((NULL == ip_address_p) && (NULL == ipv6_address_p)) {
        ATC_LOG_E("Neither ip_address nor ipv6_address found");
        goto error;
    }

    if (NULL != ip_address_p) {
        if ((inet_pton(AF_INET, ip_address_p, dummy)) <= 0) {
        ATC_LOG_E("inet_pton failed! (pscc_paramid_own_ip_address==%s)", ip_address_p);
        goto error;
    }
    }

    if (NULL != ipv6_address_p) {
        if ((inet_pton(AF_INET6, ipv6_address_p, dummy)) <= 0) {
            ATC_LOG_E("inet_pton failed! (pscc_paramid_own_ipv6_address==%s)", ipv6_address_p);
            goto error;
        }
    }

    /* Get gw address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_gw_address, &gw_address_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (pscc_paramid_gw_address)");
        goto error;
    }

    if ((NULL != gw_address_p) && (inet_pton(ip_family, gw_address_p, dummy) <= 0)) {
        ATC_LOG_E("inet_pton failed! (pscc_paramid_gw_address==%s)", gw_address_p);
        goto error;
    }

    /* Get dns address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_dns_address, &dns_address_1_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (dns_address_1_p)");
    }

    /* Get ipv6 dns address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_ipv6_dns_address, &ipv6_dns_address_1_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (ipv6_dns_address_1_p)");
    }

    if ((NULL == dns_address_1_p) && (NULL == ipv6_dns_address_1_p)) {
        ATC_LOG_E("Neither dns_address_1_p nor ipv6_dns_address_1_p found");
        goto error;
    }

    if (NULL != dns_address_1_p) {
        if ((inet_pton(AF_INET, dns_address_1_p, dummy)) <= 0) {
        ATC_LOG_E("inet_pton failed! (dns_address_1==%s)", dns_address_1_p);
        goto error;
    }
    }

    if (NULL != ipv6_dns_address_1_p) {
        if ((inet_pton(AF_INET6, ipv6_dns_address_1_p, dummy)) <= 0) {
            ATC_LOG_E("inet_pton failed! (ipv6_dns_address_1_p==%s)", ipv6_dns_address_1_p);
            goto error;
        }
    }

    /* Get dns 2 address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_secondary_dns_address, &dns_address_2_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (dns_address_2)");
    }

    if ((NULL != dns_address_2_p) && (inet_pton(AF_INET, dns_address_2_p, dummy)) <= 0) {
        ATC_LOG_E("inet_pton failed! (dns_address_2==%s)", dns_address_2_p);
    }

    /* Get ipv6 dns 2 address */
    if (!get_response_parameter_from_mpl_msg(response_p, pscc_paramid_ipv6_secondary_dns_address, &ipv6_dns_address_2_p)) {
        ATC_LOG_E("mpl_param_list_find failed! (ipv6_dns_address_2_p)");
    }

    if ((NULL != ipv6_dns_address_2_p) && (inet_pton(AF_INET6, ipv6_dns_address_2_p, dummy)) <= 0) {
        ATC_LOG_E("inet_pton failed! (ipv6_dns_address_2_p==%s)", ipv6_dns_address_2_p);
    }

    /* Get connection status */
    param_elem_p = mpl_param_list_find(pscc_paramid_connection_status, response_p->param_list_p);

    if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
        connection_status = *(int *)(param_elem_p->value_p);
    }

    if (!psccclient_iadb_get_cid_on_at_ct((uintptr_t)record_p, &cid)) {
        ATC_LOG_E("can't find cid related to at record!");
        goto error;
    }
    if (!psccclient_iadb_get(cid, psccclient_iadb_field_activation_cause, &(activation_cause))) {
        ATC_LOG_E("Activation cause undefined!");
        goto error;
    }
    (void)psccclient_set_connection_response_data(record_p, pdp_type, netdev_name_p, ip_address_p, ipv6_address_p,
        gw_address_p, dns_address_1_p, ipv6_dns_address_1_p, dns_address_2_p, ipv6_dns_address_2_p, connection_status,activation_cause);

    return  EXE_PSCC_SUCCESS;

error:
    free(netdev_name_p);
    if (NULL != ip_address_p)
        free(ip_address_p);
    if (NULL != ipv6_address_p)
        free(ipv6_address_p);
        free(gw_address_p);
    if (NULL != dns_address_1_p)
        free(dns_address_1_p);
    if (NULL != ipv6_dns_address_1_p)
        free(ipv6_dns_address_1_p);
    if (NULL != dns_address_2_p)
        free(dns_address_2_p);
    if (NULL != ipv6_dns_address_2_p)
        free(ipv6_dns_address_2_p);
    return EXE_PSCC_FAILURE;
}

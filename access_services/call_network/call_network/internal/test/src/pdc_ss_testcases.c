/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "testcases.h"
#include "module_test_main.h"
#include "cn_client.h"
#include "cn_log.h"
#include "message_handler.h"
#include "cn_pdc_internal.h"
#include "sim.h"
#include "mal_utils.h"
#include "mal_call.h"
#include "mal_ss.h"
#include "pdc_test_utils.h"
#include "sim_client.h"

/* MAL callback prototypes, defined in mal_client.c */
extern void call_event_callback(int mal_call_event_id, void *data_p, int mal_error, void *client_tag_p);

/* Global variable defined in module_test_main.c to tell if pdc is enabled or not (default not) */
extern cn_bool_t g_tf_pdc_check_enabled;

/* Global variables defined in mal_stubs.c */
extern void *g_mal_client_tag_p;
extern void *g_mal_request_data_p;
extern char g_ussd_string[6];

/* Global variables defined in sim_stubs.c */
extern sim_service_type_t g_ste_uicc_get_service_availability__sim_service_type;
extern uintptr_t g_sim_client_tag_p;
extern int8_t g_ste_uicc_sim_file_read_fdn__int;
extern int8_t g_ste_cat_call_control__int;

extern uint32_t g_mal_ss_query_forward_status;
extern uint32_t g_mal_ss_query_forward_reason;

tc_result_t pdc_make_ussd_request()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t size = 0;

    /* Enable pdc checking */
    \
    g_tf_pdc_check_enabled = TRUE;
    \

    /* Create and initialize an ussd request struct: */
    /*************************************************/
    cn_ussd_info_t ussd_info;
    memset(&ussd_info, 0, sizeof(cn_ussd_info_t));
    ussd_info.type = 0xFF; /* Not used value in cn_request_ussd call */
    ussd_info.received_type = 0xFF; /* Not used value in cn_request_ussd call */
    ussd_info.length = 4;
    ussd_info.dcs = 42;
    ussd_info.sat_initiated = true;
    ussd_info.me_initiated = false;
    memmove(ussd_info.ussd_string, "USSD", 4);

    /* Prepare struct to be used for retrieving ussd call data */
    /***********************************************************/
    mal_ss_ussd_data *actual_mal_ussd_p = (mal_ss_ussd_data *) calloc(1, sizeof(*actual_mal_ussd_p));
    g_mal_request_data_p = actual_mal_ussd_p;

    /* Send CN_REQUEST_USSD */
    result = cn_request_ussd(g_context_p, &ussd_info, (cn_client_tag_t) 77);

    /* ECC should be skipped as it is no voicecall */
    /* FDN should be skipped as it is sat-initiated */

    /* Wait for select to finish, then check that a request for call control availability has been sent: */
    WAIT_FOR_SELECT_TO_FINISH();
    TC_ASSERT(SIM_SERVICE_TYPE_CALL_CONTROL == g_ste_uicc_get_service_availability__sim_service_type);


    /* Generate callback from ste_uicc_get_service_availability */
    /************************************************************/
    ste_uicc_get_service_availability_response_t ust_response;
    memset(&ust_response, 0, sizeof(ste_uicc_get_service_availability_response_t));
    ust_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    ust_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check that a request to ste_cat_call_control has been sent: */
    TC_ASSERT(1 == g_ste_cat_call_control__int);

    /* Generate callback from ste_cat_call_control, replacing the USSD string */
    /**************************************************************************/
    char *cc_number_p = "USSR";
    uint8_t new_dcs = 41;
    ste_cat_call_control_response_t sat_cc_response;
    memset(&sat_cc_response, 0, sizeof(ste_cat_call_control_response_t));
    sat_cc_response.cc_result = STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS;
    sat_cc_response.cc_info.cc_type = STE_CAT_CC_USSD;
    sat_cc_response.cc_info.cc_data.ussd_p = calloc(1, sizeof(ste_cat_cc_ussd_t));
    sat_cc_response.cc_info.cc_data.ussd_p->ussd_data.dcs = new_dcs;
    sat_cc_response.cc_info.cc_data.ussd_p->ussd_data.dialled_string_p = calloc(1, sizeof(ste_sim_string_t));
    sat_cc_response.cc_info.cc_data.ussd_p->ussd_data.dialled_string_p->text_coding = STE_SIM_ASCII8;
    sat_cc_response.cc_info.cc_data.ussd_p->ussd_data.dialled_string_p->no_of_bytes = strlen(cc_number_p);
    sat_cc_response.cc_info.cc_data.ussd_p->ussd_data.dialled_string_p->str_p = calloc(1, strlen(cc_number_p) + 1);
    memmove(sat_cc_response.cc_info.cc_data.ussd_p->ussd_data.dialled_string_p->str_p, cc_number_p, strlen(cc_number_p));

    sim_client_callback(STE_CAT_CAUSE_SIM_EC_CALL_CONTROL, (uintptr_t)g_sim_client_tag_p, &sat_cc_response, NULL);

    /* Free the memory allocated for the response */
    free(sat_cc_response.cc_info.cc_data.ussd_p->ussd_data.dialled_string_p->str_p);
    free(sat_cc_response.cc_info.cc_data.ussd_p->ussd_data.dialled_string_p);
    free(sat_cc_response.cc_info.cc_data.ussd_p);

    /* Generate callback from MAL, indicating that the USSD command has been performed */
    /***********************************************************************************/
    call_event_callback(MAL_SS_GSM_USSD_SEND_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    /* Receive response and verify validity */
    /****************************************/
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_make_ss_request failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Check USSD parameters used to call SIM */
    TC_ASSERT(strlen(cc_number_p) == actual_mal_ussd_p->length);
    TC_ASSERT(new_dcs == actual_mal_ussd_p->dcs);
    TC_ASSERT(memcmp(cc_number_p, g_ussd_string, strlen(cc_number_p)) == 0);

    /* Check USSD call response */
    TC_ASSERT(CN_RESPONSE_USSD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 77 == msg_p->client_tag);

exit:
    free(msg_p);
    free(actual_mal_ussd_p);
    g_tf_pdc_check_enabled = FALSE; /* Reset pdc check enablement */
    return tc_result;
}

tc_result_t pdc_make_ss_request()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t size = 0;

    /* Enable pdc checking */
    \
    g_tf_pdc_check_enabled = TRUE;
    \

    /* Make struct containing data to the call: */
    cn_call_forward_info_t call_forward_info;
    memset(&call_forward_info, 0, sizeof(cn_call_forward_info_t));

    /* Call cn_request_query_call_forward, starting the sequence */
    /***********************************************/
    result = cn_request_query_call_forward(g_context_p, &call_forward_info, (cn_client_tag_t) 45);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_make_ss_request failed! Response = %d", result);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* FDN availability will be requested first (ECC skipped as it is no voice call.
     * Wait for select to finish, then check variables: */
    WAIT_FOR_SELECT_TO_FINISH();
    TC_ASSERT(SIM_SERVICE_TYPE_FDN == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability */
    /************************************************************/
    ste_uicc_get_service_availability_response_t ust_response;
    memset(&ust_response, 0, sizeof(ste_uicc_get_service_availability_response_t));
    ust_response.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
    ust_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check that a request for call control availability has been sent: */
    TC_ASSERT(SIM_SERVICE_TYPE_CALL_CONTROL == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability */
    /************************************************************/
    /* NB! The same response struct is used as when FDN requested information from UST file. */
    ust_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check that a request to ste_cat_call_control has been sent: */
    TC_ASSERT(1 == g_ste_cat_call_control__int);

    /* Generate callback from ste_cat_call_control, replacing the USSD string */
    /**************************************************************************/
    cn_uint8_t new_ton = STE_SIM_TON_UNKNOWN;
    cn_uint8_t new_npi = STE_SIM_NPI_UNKNOWN;

    char *cc_number_p = "*#21**0*8#";

    ste_cat_call_control_response_t sat_cc_response;
    memset(&sat_cc_response, 0, sizeof(ste_cat_call_control_response_t));
    sat_cc_response.cc_result = STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS;
    sat_cc_response.cc_info.cc_type = STE_CAT_CC_SS;
    sat_cc_response.cc_info.cc_data.ss_p = calloc(1, sizeof(ste_cat_cc_ss_t));
    sat_cc_response.cc_info.cc_data.ss_p->address.ton = new_ton;
    sat_cc_response.cc_info.cc_data.ss_p->address.npi = new_npi;

    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p = calloc(1, sizeof(ste_sim_string_t));
    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_coding = STE_SIM_ASCII8;
    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->no_of_characters = strlen(cc_number_p);
    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_p = calloc(1, strlen(cc_number_p) + 1);
    memmove(sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_p, cc_number_p, strlen(cc_number_p));

    sim_client_callback(STE_CAT_CAUSE_SIM_EC_CALL_CONTROL, (uintptr_t)g_sim_client_tag_p, &sat_cc_response, NULL);

    /* Free the memory allocated for the response */
    free(sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_p);
    free(sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p);
    free(sat_cc_response.cc_info.cc_data.ss_p);

    /* Check that a request to ste_cat_call_control has been sent: */
    TC_ASSERT(g_mal_ss_query_forward_status == 2); /* 2 = reason interrogation, i.e. query */

    /* Generate callback from mal_ss_query_call_forward ()*/
    /**************************************************/
    mal_ss_service_response service_response;
    memset(&service_response, 0, sizeof(mal_ss_service_response));
    mal_ss_call_fwd_info call_fwd_response;
    memset(&call_fwd_response, 0, sizeof(mal_ss_call_fwd_info));
    service_response.call_forward_info = &call_fwd_response;

    call_event_callback(CN_RESPONSE_QUERY_CALL_FORWARD, &service_response, MAL_SUCCESS, g_mal_client_tag_p);

    /* Receive response and verify validity */
    /****************************************/
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_make_ss_request failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

exit:
    free(msg_p);
    g_tf_pdc_check_enabled = FALSE; /* Reset pdc check enablement */
    return tc_result;
}



tc_result_t pdc_make_ss_request_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t size = 0;

    /* Enable pdc checking */
    \
    g_tf_pdc_check_enabled = TRUE;
    \

    /* Make struct containing data to the call: */
    cn_call_forward_info_t call_forward_info;
    memset(&call_forward_info, 0, sizeof(cn_call_forward_info_t));

    /* Call cn_request_query_call_forward, starting the sequence */
    /***********************************************/
    result = cn_request_query_call_forward(g_context_p, &call_forward_info, (cn_client_tag_t) 45);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_make_ss_request failed! Response = %d", result);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* FDN availability will be requested first (ECC skipped as it is no voice call.
     * Wait for select to finish, then check variables: */
    WAIT_FOR_SELECT_TO_FINISH();
    TC_ASSERT(SIM_SERVICE_TYPE_FDN == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability */
    /************************************************************/
    ste_uicc_get_service_availability_response_t ust_response;
    memset(&ust_response, 0, sizeof(ste_uicc_get_service_availability_response_t));
    ust_response.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
    ust_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check that a request for call control availability has been sent: */
    TC_ASSERT(SIM_SERVICE_TYPE_CALL_CONTROL == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability */
    /************************************************************/
    /* NB! The same response struct is used as when FDN requested information from UST file. */
    ust_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check that a request to ste_cat_call_control has been sent: */
    TC_ASSERT(1 == g_ste_cat_call_control__int);

    /* Generate callback from ste_cat_call_control, replacing the USSD string */
    /**************************************************************************/
    cn_uint8_t new_ton = STE_SIM_TON_UNKNOWN;
    cn_uint8_t new_npi = STE_SIM_NPI_UNKNOWN;

    char *cc_number_p = "*#21**0*8#";

    ste_cat_call_control_response_t sat_cc_response;
    memset(&sat_cc_response, 0, sizeof(ste_cat_call_control_response_t));
    sat_cc_response.cc_result = STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS;
    sat_cc_response.cc_info.cc_type = STE_CAT_CC_SS;
    sat_cc_response.cc_info.cc_data.ss_p = calloc(1, sizeof(ste_cat_cc_ss_t));
    sat_cc_response.cc_info.cc_data.ss_p->address.ton = new_ton;
    sat_cc_response.cc_info.cc_data.ss_p->address.npi = new_npi;

    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p = calloc(1, sizeof(ste_sim_string_t));
    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_coding = STE_SIM_ASCII8;
    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->no_of_characters = strlen(cc_number_p);
    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_p = calloc(1, strlen(cc_number_p) + 1);
    memmove(sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_p, cc_number_p, strlen(cc_number_p));

    sim_client_callback(STE_CAT_CAUSE_SIM_EC_CALL_CONTROL, (uintptr_t)g_sim_client_tag_p, &sat_cc_response, NULL);

    /* Free the memory allocated for the response */
    free(sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_p);
    free(sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p);
    free(sat_cc_response.cc_info.cc_data.ss_p);

    /* Check that a request to ste_cat_call_control has been sent: */
    //TC_ASSERT(g_mal_ss_query_forward_status == 2); /* 2 = reason interrogation, i.e. query */

    /* Generate callback from mal_ss_query_call_forward ()*/
    /**************************************************/
    mal_ss_service_response service_response;
    memset(&service_response, 0, sizeof(mal_ss_service_response));
    mal_ss_call_fwd_info call_fwd_response;
    memset(&call_fwd_response, 0, sizeof(mal_ss_call_fwd_info));
    service_response.call_forward_info = &call_fwd_response;


    call_event_callback(CN_RESPONSE_QUERY_CALL_FORWARD, &service_response, MAL_FAIL, g_mal_client_tag_p);

    /* Receive response and verify validity */
    /****************************************/
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_make_ss_request failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

exit:
    free(msg_p);
    g_tf_pdc_check_enabled = FALSE; /* Reset pdc check enablement */
    return tc_result;
}


tc_result_t pdc_make_ss_request_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t size = 0;

    /* Enable pdc checking */
    \
    g_tf_pdc_check_enabled = TRUE;
    \

    /* Make struct containing data to the call: */
    cn_call_forward_info_t call_forward_info;
    memset(&call_forward_info, 0, sizeof(cn_call_forward_info_t));

    /* Call cn_request_query_call_forward, starting the sequence */
    /***********************************************/
    result = cn_request_query_call_forward(g_context_p, &call_forward_info, (cn_client_tag_t) 45);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_make_ss_request failed! Response = %d", result);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* FDN availability will be requested first (ECC skipped as it is no voice call.
     * Wait for select to finish, then check variables: */
    WAIT_FOR_SELECT_TO_FINISH();
    TC_ASSERT(SIM_SERVICE_TYPE_FDN == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability */
    /************************************************************/
    ste_uicc_get_service_availability_response_t ust_response;
    memset(&ust_response, 0, sizeof(ste_uicc_get_service_availability_response_t));
    ust_response.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
    ust_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check that a request for call control availability has been sent: */
    TC_ASSERT(SIM_SERVICE_TYPE_CALL_CONTROL == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability */
    /************************************************************/
    /* NB! The same response struct is used as when FDN requested information from UST file. */
    ust_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check that a request to ste_cat_call_control has been sent: */
    TC_ASSERT(1 == g_ste_cat_call_control__int);

    /* Generate callback from ste_cat_call_control, replacing the USSD string */
    /**************************************************************************/
    cn_uint8_t new_ton = STE_SIM_TON_UNKNOWN;
    cn_uint8_t new_npi = STE_SIM_NPI_UNKNOWN;

    char *cc_number_p = "*#21**0*8#";

    ste_cat_call_control_response_t sat_cc_response;
    memset(&sat_cc_response, 0, sizeof(ste_cat_call_control_response_t));
    sat_cc_response.cc_result = STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS;
    sat_cc_response.cc_info.cc_type = STE_CAT_CC_SS;
    sat_cc_response.cc_info.cc_data.ss_p = calloc(1, sizeof(ste_cat_cc_ss_t));
    sat_cc_response.cc_info.cc_data.ss_p->address.ton = new_ton;
    sat_cc_response.cc_info.cc_data.ss_p->address.npi = new_npi;

    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p = calloc(1, sizeof(ste_sim_string_t));
    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_coding = STE_SIM_ASCII8;
    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->no_of_characters = strlen(cc_number_p);
    sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_p = calloc(1, strlen(cc_number_p) + 1);
    memmove(sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_p, cc_number_p, strlen(cc_number_p));

    sim_client_callback(STE_CAT_CAUSE_SIM_EC_CALL_CONTROL, (uintptr_t)g_sim_client_tag_p, &sat_cc_response, NULL);

    /* Free the memory allocated for the response */
    free(sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p->text_p);
    free(sat_cc_response.cc_info.cc_data.ss_p->address.dialled_string_p);
    free(sat_cc_response.cc_info.cc_data.ss_p);

    /* Check that a request to ste_cat_call_control has been sent: */
    //TC_ASSERT(g_mal_ss_query_forward_status == 2); /* 2 = reason interrogation, i.e. query */

    /* Generate callback from mal_ss_query_call_forward ()*/
    /**************************************************/
    mal_ss_service_response service_response;
    memset(&service_response, 0, sizeof(mal_ss_service_response));
    mal_ss_call_fwd_info call_fwd_response;
    memset(&call_fwd_response, 0, sizeof(mal_ss_call_fwd_info));
    service_response.call_forward_info = &call_fwd_response;


    call_event_callback(CN_RESPONSE_QUERY_CALL_FORWARD, &service_response, MAL_NOT_SUPPORTED, g_mal_client_tag_p);

    /* Receive response and verify validity */
    /****************************************/
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_make_ss_request failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

exit:
    free(msg_p);
    g_tf_pdc_check_enabled = FALSE; /* Reset pdc check enablement */
    return tc_result;
}


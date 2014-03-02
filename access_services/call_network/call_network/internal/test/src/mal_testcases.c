/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
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
#include "plmn_main.h"
#include "cn_internal_handlers.h"
#include "str_convert.h"
#include "cn_event_trigger_level.h"
#include "main.h"
#include "mal_call.h"
#include "mal_gss.h"
#include "mal_mce.h"
#include "mal_net.h"
#include "mal_ss.h"
#include "mal_nvd.h"
#include "mal_rf.h"
#include "mal_utils.h"
#include "mal_ftd.h"
#include "mal_mis.h"
#include "mal_mte.h"
#include "sim_client.h"
#include "modem_client.h"


#include "cn_pdc_ecc_list_handling.h"

/* MAL callback prototypes, defined in mal_client.c */
extern void call_event_callback(int mal_call_event_id, void *data_p, int mal_error, void *client_tag_p);
extern void gss_event_callback(int mal_gss_event_id, void *data_p, mal_gss_error_type mal_error, void *client_tag_p);
extern void mce_event_callback(int mal_event_id, void *data_p, mal_mce_error_type error_code, void *client_tag_p);
extern void net_event_callback(int mal_event_id, void *data_p, mal_net_error_type error_code, void *client_tag_p);
extern void nvd_event_callback(int nvd_event_id, void *data_p, mal_net_error_type error_code, void *client_tag_p);
extern void ss_event_callback(int mal_ss_event_id, void *data_p, int error_code, void *client_tag_p);
extern void ftd_event_callback(int mal_event_id, void *data_p, mal_ftd_error_type mal_error, void *client_tag_p);
extern void mis_event_callback(int mal_event_id, void *data_p, mal_mis_error_type error_code, void *modem_tag_p);
extern void rf_event_callback(int mal_event_id, void *data_p, mal_rf_error_type error_code, void *modem_tag);

/* Global variables defined in mal_stubs.c */
extern int32_t g_mal_net_set_mode__net_mode;
extern int32_t g_mal_return_value;
extern int8_t g_mal_call_request_hangup__call_id;
extern mal_call_dtmf_info g_mal_call_request_dtmf_send__dtmf_info;
extern mal_gss_network_type g_mal_gss_set_preferred_network_type__type;
extern mal_net_cs_request_type g_mal_net_control_cs__req_type;
extern mal_net_nmr_rat_type g_mal_rat_type;
extern mal_net_utran_nmr_type g_mal_nmr_type;
extern mal_net_nw_access_conf_data g_mal_net_nw_access_conf_data;
extern mal_ss_serv_class_info g_mal_ss_serv_class_info;
extern mal_ss_service_op g_mal_ss_service_op;
extern uint32_t g_mal_string_length;
extern uint8_t *g_mal_net_manual_register__mnc_mcc_p;
extern uint8_t g_mal_no_of_calls;
extern void *g_mal_call_list_p;
extern void *g_mal_client_tag_p;
extern void *g_mal_request_data_p;
extern char g_ussd_string[4];
extern uint32_t  g_mal_ss_forward_status;
extern uint32_t  g_mal_ss_forward_reason;
extern uint32_t  g_mal_ss_forward_time;
extern cn_bool_t g_tf_pdc_check_enabled;
extern uint32_t g_mal_ss_query_forward_status;
extern uint32_t g_mal_ss_query_forward_reason;
extern uint32_t g_mal_ss_query_serv_class_info;
extern mal_net_reg_status_set_mode g_mal_net_reg_status_set_mode;
extern char g_mal_nvd_set_default_called;
extern mal_net_ind_type g_mal_net_set_ind_state_type;
extern mal_net_ind_state g_mal_net_set_ind_state_state;
extern mal_rf_tx_back_off_event g_mal_rf_send_tx_back_off_event;
extern cn_registration_info_t s_reg_info;

/* Global variables defined in sim_stubs.c */
extern uintptr_t g_sim_client_tag_p;

/* String constants */
#define MCC_MNC_STRING "405803"
#define SHORT_OPERATOR_NAME_STRING "Op name 23"
#define LONG_OPERATOR_NAME_STRING "This is a maximum (60 characters) length operator name ....."
#define CHARSET_GSM_DEFAULT_123 1
#define MCC_MNC_STRING_1 "123456"

/* Test data vectors used for FTD tests */
ftd_table_entry_t MAL_FTD_TEST_table1[] = {
    {0, 0, 0, 0, "CDR-PER-151::Uplink Channel Mode "                , "ULMode1"},
    {0, 0, 0, 0, "CDR-PER-153::Downlink Channel Mode"               , "DLMode1"},
    {0, 0, 0, 0, "CDR-PER-155::Active Code Set0 (ACS0)"             , "ACS1"},
    {0, 0, 0, 0, "CDR-PER-161::C/I"                                 , "123.01"},
    {0, 0, 0, 0, "CDR-PER-157::DTX UL ON/OFF"                       , "On1"},
    {0, 0, 0, 0, "CDR-PER-163::RxQual_SUB (Reported when DTX is ON)", "tripp"}
};

ftd_table_entry_t MAL_FTD_TEST_table2[] = {
    {0, 0, 0, 0, "CDR-PER-151::Uplink Channel Mode"                 , "ULMode2"},
    {0, 0, 0, 0, "CDR-PER-153::Downlink Channel Mode"               , "DLMode2"},
    {0, 0, 0, 0, "CDR-PER-155::Active Code Set0 (ACS0)"             , "ACS2"},
    {0, 0, 0, 0, "CDR-PER-161::C/I"                                 , "123.02"},
    {0, 0, 0, 0, "CDR-PER-157::DTX UL ON/OFF"                       , "On2"},
    {0, 0, 0, 0, "CDR-PER-163::RxQual_SUB (Reported when DTX is ON)", "trapp"}
};

ftd_table_entry_t MAL_FTD_TEST_table3[] = {
    {0, 0, 0, 0, "CDR-PER-151::Uplink Channel Mode"                 , "ULMode3"},
    {0, 0, 0, 0, "CDR-PER-153::Downlink Channel Mode"               , "DLMode3"},
    {0, 0, 0, 0, "CDR-PER-155::Active Code Set0 (ACS0)"             , "ACS3"},
    {0, 0, 0, 0, "CDR-PER-161::C/I"                                 , "123.03"},
    {0, 0, 0, 0, "CDR-PER-157::DTX UL ON/OFF"                       , "On3"},
    {0, 0, 0, 0, "CDR-PER-163::RxQual_SUB (Reported when DTX is ON)", "trull"}
};

ftd_table_entry_t MAL_FTD_TEST_table4[] = {
    {0, 0, 0, 0, "Page2::Parameter1", "ABC"},
    {0, 0, 0, 0, "Page2::Parameter1", "true"}
};

ftd_table_entry_t MAL_FTD_TEST_table5[] = {
    {0, 0, 0, 0, "Page2::Parameter1", "DEF"},
    {0, 0, 0, 0, "Page2::Parameter1", "false"}
};

ftd_table_entry_t MAL_FTD_TEST_table6[] = {
    {0, 0, 0, 0, "CDR-PER-151::Uplink Channel Mode"                 , "ULMode4"},
    {0, 0, 0, 0, "CDR-PER-153::Downlink Channel Mode"               , "DLMode4"},
    {0, 0, 0, 0, "CDR-PER-155::Active Code Set0 (ACS0)"             , FTD_MEASUREMENT_NOT_SUPPORTED},
    {0, 0, 0, 0, "CDR-PER-161::C/I"                                 , FTD_MEASUREMENT_NOT_AVAILABLE},
    {0, 0, 0, 0, "CDR-PER-157::DTX UL ON/OFF"                       , FTD_MEASUREMENT_REQ_FAILED},
    {0, 0, 0, 0, "CDR-PER-163::RxQual_SUB (Reported when DTX is ON)", FTD_MEASUREMENT_REQ_TIMEOUT}
};

view_details_t MAL_FTD_MEASUREMENTREPORTS[] = {
    { 6, MAL_FTD_TEST_table1 },
    { 6, MAL_FTD_TEST_table2 },
    { 6, MAL_FTD_TEST_table3 },
    { 2, MAL_FTD_TEST_table4 },
    { 2, MAL_FTD_TEST_table5 },
    { 6, MAL_FTD_TEST_table6 }
};

#define NO_OF_MAL_FTD_MEASUREMENTREPORTS (int32_t)(sizeof(MAL_FTD_MEASUREMENTREPORTS) / sizeof(view_details_t))

static int32_t mal_ftd_event_index = 0;
static bool    mal_ftd_isOneshot   = false;

/* =================
 * REQUEST TESTCASES
 * =================
 */
tc_result_t cn_request_rf_on_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_mce_status mce_status;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RF_ON */
    result = cn_request_rf_on(g_context_p, (cn_client_tag_t) 12345);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rf_on failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mce_status = MAL_MCE_OK;
    mce_event_callback(MAL_MCE_RF_STATE_RESP, &mce_status, MAL_MCE_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RF_ON */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RF_ON == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(12345 == msg_p->client_tag);
    TC_ASSERT(CN_MODEM_STATUS_OK == *((cn_modem_status_t *)msg_p->payload));

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_rf_on_negative_1() /* Negative scenario: MAL failure code in callback, not response */
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_mce_status mce_status;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RF_ON */
    result = cn_request_rf_on(g_context_p, (cn_client_tag_t) 1234);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rf_on failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mce_status = MAL_MCE_OK;
    mce_event_callback(MAL_MCE_RF_STATE_RESP, &mce_status, MAL_MCE_REQ_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RF_ON */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RF_ON == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(1234 == msg_p->client_tag);
    TC_ASSERT(CN_MODEM_STATUS_OK == *((cn_modem_status_t *)msg_p->payload));

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_rf_on_negative_2() /* Negative scenario: MAL failure code in response data, not in callback */
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_mce_status mce_status;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RF_ON */
    result = cn_request_rf_on(g_context_p, (cn_client_tag_t) 123);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rf_on failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mce_status = MAL_MCE_NOT_ALLOWED;
    mce_event_callback(MAL_MCE_RF_STATE_RESP, &mce_status, MAL_MCE_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RF_ON */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RF_ON == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(123 == msg_p->client_tag);
    TC_ASSERT(CN_MODEM_STATUS_NOT_ALLOWED == *((cn_modem_status_t *)msg_p->payload));

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_set_modem_sleep_on_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint8_t *mal_modem_sleep_status_p = (uint8_t *) calloc(1, 1);

    if (NULL == mal_modem_sleep_status_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    g_mal_request_data_p = mal_modem_sleep_status_p;

    /* Send CN_REQUEST_SET_MODEM_SLEEP */
    result = cn_request_sleep_test_mode(g_context_p, CN_SLEEP_TEST_MODE_ON, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_sleep_test_mode failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_MODEM_TEST_SLEEP_TEST_MODE_SET_RESP, NULL, MAL_MTE_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_SLEEP */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SLEEP_TEST_MODE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_modem_sleep_status_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_set_modem_sleep_on_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint8_t *mal_modem_sleep_status_p = (uint8_t *) calloc(1, 1);

    if (NULL == mal_modem_sleep_status_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    g_mal_request_data_p = mal_modem_sleep_status_p;

    /* Send CN_REQUEST_SET_MODEM_SLEEP */
    result = cn_request_sleep_test_mode(g_context_p, CN_SLEEP_TEST_MODE_ON, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_sleep_test_mode failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_MODEM_TEST_SLEEP_TEST_MODE_SET_RESP, NULL, MAL_MTE_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_SLEEP */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SLEEP_TEST_MODE == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_modem_sleep_status_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_set_modem_sleep_off_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint8_t *mal_modem_sleep_status_p = (uint8_t *) calloc(1, 1);

    if (NULL == mal_modem_sleep_status_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    g_mal_request_data_p = mal_modem_sleep_status_p;

    /* Send CN_REQUEST_SET_MODEM_SLEEP */
    result = cn_request_sleep_test_mode(g_context_p, CN_SLEEP_TEST_MODE_OFF, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_sleep_test_mode failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_MODEM_TEST_SLEEP_TEST_MODE_SET_RESP, NULL, MAL_MTE_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_SLEEP */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SLEEP_TEST_MODE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_modem_sleep_status_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_set_modem_sleep_off_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint8_t *mal_modem_sleep_status_p = (uint8_t *) calloc(1, 1);

    if (NULL == mal_modem_sleep_status_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    g_mal_request_data_p = mal_modem_sleep_status_p;

    /* Send CN_REQUEST_SET_MODEM_SLEEP */
    result = cn_request_sleep_test_mode(g_context_p, CN_SLEEP_TEST_MODE_OFF, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_sleep_test_mode failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_MODEM_TEST_SLEEP_TEST_MODE_SET_RESP, NULL, MAL_MTE_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_SLEEP */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SLEEP_TEST_MODE == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_modem_sleep_status_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_rf_off_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_mce_status mce_status;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RF_OFF */
    result = cn_request_rf_off(g_context_p, (cn_client_tag_t) 768);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rf_off failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mce_status = MAL_MCE_OK;
    mce_event_callback(MAL_MCE_RF_STATE_RESP, &mce_status, MAL_MCE_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RF_OFF */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RF_OFF == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(768 == msg_p->client_tag);
    TC_ASSERT(CN_MODEM_STATUS_OK == *((cn_modem_status_t *)msg_p->payload));

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_rf_off_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RF_OFF */
    result = cn_request_rf_off(g_context_p, (cn_client_tag_t) 44);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rf_off failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mce_event_callback(MAL_MCE_RF_STATE_RESP, NULL, MAL_MCE_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RF_OFF */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RF_OFF == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(44 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_rf_status_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_mce_rf_status rf_status;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RF_STATUS */
    result = cn_request_rf_status(g_context_p, (cn_client_tag_t) 769);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rf_status failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    rf_status = MAL_MCE_RF_ON;
    mce_event_callback(MAL_MCE_RF_STATE_QUERY_RESP, &rf_status, MAL_MCE_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RF_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RF_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(769 == msg_p->client_tag);
    TC_ASSERT(CN_RF_ON == *((cn_rf_status_t *)msg_p->payload));

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_rf_status_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RF_STATUS */
    result = cn_request_rf_status(g_context_p, (cn_client_tag_t) 43);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rf_status failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mce_event_callback(MAL_MCE_RF_STATE_QUERY_RESP, NULL, MAL_MCE_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RF_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RF_STATUS == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(43 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_registration_control_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_network_access_config_data_t cn_config_data = { CN_NETWORK_ACCESS_CONFIG_ENABLE, CN_NETWORK_ACCESS_CONFIG_ENABLE };

    g_mal_net_nw_access_conf_data.reg_conf = MAL_NET_CONF_DISABLE;
    g_mal_net_nw_access_conf_data.roam_conf = MAL_NET_CONF_DISABLE;

    /* Send CN_REQUEST_REGISTRATION_CONTROL */
    result = cn_request_registration_control(g_context_p, &cn_config_data, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_control failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(cn_config_data.registration_conf == g_mal_net_nw_access_conf_data.reg_conf);
    TC_ASSERT(cn_config_data.roaming_conf == g_mal_net_nw_access_conf_data.roam_conf);

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_SET_ACCESS_CONF_RESP, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_REGISTRATION_CONTROL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_REGISTRATION_CONTROL == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_registration_control_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_network_access_config_data_t cn_config_data = { CN_NETWORK_ACCESS_CONFIG_ENABLE, CN_NETWORK_ACCESS_CONFIG_DISABLE };

    g_mal_return_value = MAL_NET_GENERAL_ERROR;

    /* Send CN_REQUEST_REGISTRATION_CONTROL */
    result = cn_request_registration_control(g_context_p, &cn_config_data, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_control failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_SET_ACCESS_CONF_RESP, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_REGISTRATION_CONTROL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_REGISTRATION_CONTROL == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);

    /* Restore */
    g_mal_return_value = MAL_NET_SUCCESS;

    return tc_result;
}


tc_result_t cn_request_reset_modem_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RESET_MODEM */
    result = cn_request_reset_modem(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_modem failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* mal_mce_reset -> synchronous request */

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RESET_MODEM */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RESET_MODEM == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_reset_modem_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    g_mal_return_value = MAL_NET_GENERAL_ERROR;

    /* Send CN_REQUEST_RESET_MODEM */
    result = cn_request_reset_modem(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_reset_modem failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* mal_mce_reset -> synchronous request (stub returns g_mal_return_value) */

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RESET_MODEM */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RESET_MODEM == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);

    /* Restore */
    g_mal_return_value = MAL_NET_SUCCESS;

    return tc_result;
}


tc_result_t cn_request_reset_modem_with_dump_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_cpu_type_t cn_cpu = CN_CPU_ALL;


    /* Send CN_REQUEST_RESET_MODEM_WITH_DUMP */
    result = cn_request_reset_modem_with_dump(g_context_p, cn_cpu, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_modem failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* mal_mce_reset -> synchronous request */

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RESET_MODEM_WITH_DUMP */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RESET_MODEM_WITH_DUMP == msg_p->type);
    TC_ASSERT(0 == msg_p->payload_size);
    TC_ASSERT(99 == msg_p->client_tag);
    //TC_ASSERT(CN_SUCCESS == msg_p->error_code);

exit:
    free(msg_p);
    return tc_result;
}



tc_result_t cn_request_set_preferred_network_type_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_SET_PREFERRED_NETWORK_TYPE */
    result = cn_request_set_preferred_network_type(g_context_p, CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE, (cn_client_tag_t) 760);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_preferred_network_type failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate callback from ste_uicc_sim_read_preferred_RAT_setting */
    ste_uicc_sim_read_preferred_RAT_setting_response_t  rat_result;
    rat_result.RAT = STE_UICC_SIM_RAT_SETTING_NO_PREFERENCES;
    rat_result.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING, (uintptr_t)g_sim_client_tag_p, &rat_result, NULL);

    /* MAL parameter check in the stub */
    TC_ASSERT(MAL_GSS_GSM_WCDMA_AUTO_MODE == g_mal_gss_set_preferred_network_type__type);

    /* Generate fake MAL response */
    gss_event_callback(MAL_GSS_SET_PREFERRED_NETWORK_TYPE_RESP, NULL, MAL_GSS_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(760 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_set_preferred_network_type_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_SET_PREFERRED_NETWORK_TYPE */
    result = cn_request_set_preferred_network_type(g_context_p, CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE, (cn_client_tag_t) 31);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_preferred_network_type failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate callback from ste_uicc_sim_read_preferred_RAT_setting */
    ste_uicc_sim_read_preferred_RAT_setting_response_t  rat_result;
    rat_result.RAT = STE_UICC_SIM_RAT_SETTING_NO_PREFERENCES;
    rat_result.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING, (uintptr_t)g_sim_client_tag_p, &rat_result, NULL);

    /* MAL parameter check in the stub */
    TC_ASSERT(MAL_GSS_GSM_WCDMA_AUTO_MODE == g_mal_gss_set_preferred_network_type__type);

    /* Generate fake MAL response */
    gss_event_callback(MAL_GSS_SET_PREFERRED_NETWORK_TYPE_RESP, NULL, MAL_GSS_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(31 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_set_preferred_network_type_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_SET_PREFERRED_NETWORK_TYPE */
    result = cn_request_set_preferred_network_type(g_context_p, CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE, (cn_client_tag_t) 67);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_preferred_network_type failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate callback from ste_uicc_sim_read_preferred_RAT_setting */
    ste_uicc_sim_read_preferred_RAT_setting_response_t  rat_result;
    rat_result.RAT = STE_UICC_SIM_RAT_SETTING_2G_ONLY;
    rat_result.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING, (uintptr_t)g_sim_client_tag_p, &rat_result, NULL);

    /* MAL parameter check in the stub */
    TC_ASSERT(MAL_GSS_GSM_WCDMA_AUTO_MODE != g_mal_gss_set_preferred_network_type__type);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE == msg_p->type);
    TC_ASSERT(CN_REQUEST_SIM_RAT_REJECTED == msg_p->error_code);
    TC_ASSERT(67 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_get_preferred_network_type_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_gss_network_type mal_network_type;

    /* Send CN_REQUEST_GET_PREFERRED_NETWORK_TYPE */
    result = cn_request_get_preferred_network_type(g_context_p, (cn_client_tag_t) 330);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_preferred_network_type failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mal_network_type = MAL_GSS_WCDMA_ONLY;
    gss_event_callback(MAL_GSS_GET_PREFERRED_NETWORK_TYPE_RESP, &mal_network_type, MAL_GSS_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(330 == msg_p->client_tag);
    TC_ASSERT(CN_NETWORK_TYPE_WCDMA_ONLY == *((cn_network_type_t *)msg_p->payload));

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_get_preferred_network_type_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_GET_PREFERRED_NETWORK_TYPE */
    result = cn_request_get_preferred_network_type(g_context_p, (cn_client_tag_t) 12);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_preferred_network_type failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    gss_event_callback(MAL_GSS_GET_PREFERRED_NETWORK_TYPE_RESP, NULL, MAL_GSS_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(12 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_colr_status_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t colr = 1; /* COLR provisioned */
    mal_ss_service_response response;

    /* Send request to cn */
    result = cn_request_colr_status(g_context_p, (cn_client_tag_t) 330);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_colr_status failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    memset(&response, 0, sizeof(response));
    response.response[0] = 1;
    gss_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_GSS_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_COLR_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_COLR_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(330 == msg_p->client_tag);
    TC_ASSERT(colr == *((cn_colr_status_t *)msg_p->payload));

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_colr_status_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    result = cn_request_colr_status(g_context_p, (cn_client_tag_t) 12);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_colr_status failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    gss_event_callback(CN_RESPONSE_COLR_STATUS, NULL, MAL_GSS_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_COLR_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_COLR_STATUS == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(12 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_registration_state_normal_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_registration_info_t *registration_info_p = NULL;
    mal_net_registration_info mal_registration_info;

    /* Send CN_REQUEST_REGISTRATION_STATE_NORMAL */
    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Load operator name into PLMN lookup table */
    plmn_add_nitz_name(MCC_MNC_STRING,
                       LONG_OPERATOR_NAME_STRING, sizeof(LONG_OPERATOR_NAME_STRING) - 1, /*spare*/ 0, CHARSET_UTF_8,
                       SHORT_OPERATOR_NAME_STRING, sizeof(SHORT_OPERATOR_NAME_STRING) - 1, /*spare*/ 0, CHARSET_UTF_8);

    /* Generate fake MAL response */
    mal_registration_info.reg_status          = MAL_NET_NOT_REG_SEARCHING_OP;
    mal_registration_info.rat                 = MAL_NET_RAT_UMTS;
    mal_registration_info.cid                 = 3457;
    mal_registration_info.lac                 = 124;
    mal_registration_info.mcc_mnc_string      = (uint8_t *) MCC_MNC_STRING;
    mal_registration_info.gprs_attached       = 1; /* PS network attach */
    mal_registration_info.cs_attached         = 1; /* CS network attach */
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_REGISTRATION_STATE_NORMAL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_REGISTRATION_STATE_NORMAL == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(113 == msg_p->client_tag);

    registration_info_p = (cn_registration_info_t *)msg_p->payload;
    TC_ASSERT(CN_NOT_REG_SEARCHING_OP == registration_info_p->reg_status);
    TC_ASSERT(1 == registration_info_p->gprs_attached);
    TC_ASSERT(CN_RAT_TYPE_UMTS == registration_info_p->rat);
    TC_ASSERT(124 == registration_info_p->lac);
    TC_ASSERT(3457 == registration_info_p->cid);
    TC_ASSERT(strcmp(registration_info_p->mcc_mnc, MCC_MNC_STRING) == 0);
    TC_ASSERT(strcmp(registration_info_p->long_operator_name, LONG_OPERATOR_NAME_STRING) == 0);
    TC_ASSERT(strcmp(registration_info_p->short_operator_name, SHORT_OPERATOR_NAME_STRING) == 0);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_registration_state_normal_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_REGISTRATION_STATE_NORMAL */
    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 8);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, NULL, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_REGISTRATION_STATE_NORMAL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_REGISTRATION_STATE_NORMAL == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(8 == msg_p->client_tag);
exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_registration_state_gprs_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_registration_info_t *registration_info_p = NULL;
    mal_net_registration_info mal_registration_info;

    /* Send CN_REQUEST_REGISTRATION_STATE_GPRS */
    result = cn_request_registration_state_gprs(g_context_p, (cn_client_tag_t) 904);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_gprs failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mal_registration_info.reg_status          = MAL_NET_REGISTERED_TO_HOME_NW;
    mal_registration_info.rat                 = MAL_NET_RAT_HSDPA;
    mal_registration_info.cid                 = 754;
    mal_registration_info.lac                 = 2;
    mal_registration_info.mcc_mnc_string      = (uint8_t *) MCC_MNC_STRING;
    mal_registration_info.gprs_attached       = 0; /* PS network attach */
    mal_registration_info.cs_attached         = 1; /* CS network attach */
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_REGISTRATION_STATE_GPRS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_REGISTRATION_STATE_GPRS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(904 == msg_p->client_tag);

    registration_info_p = (cn_registration_info_t *)msg_p->payload;
    TC_ASSERT(CN_REGISTERED_TO_HOME_NW == registration_info_p->reg_status);
    TC_ASSERT(0 == registration_info_p->gprs_attached);
    TC_ASSERT(CN_RAT_TYPE_HSDPA == registration_info_p->rat);
    TC_ASSERT(2 == registration_info_p->lac);
    TC_ASSERT(754 == registration_info_p->cid);
    TC_ASSERT(strcmp(registration_info_p->mcc_mnc, MCC_MNC_STRING) == 0);
    TC_ASSERT(strcmp(registration_info_p->long_operator_name, LONG_OPERATOR_NAME_STRING) == 0);
    TC_ASSERT(strcmp(registration_info_p->short_operator_name, SHORT_OPERATOR_NAME_STRING) == 0);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_registration_state_gprs_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_REGISTRATION_STATE_GPRS */
    result = cn_request_registration_state_gprs(g_context_p, (cn_client_tag_t) 924);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_gprs failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, NULL, MAL_NET_INSUFFICIENT_MEMORY, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_REGISTRATION_STATE_GPRS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_REGISTRATION_STATE_GPRS == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(924 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_cell_info_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_cell_info_t *cell_info_p = NULL;
    mal_net_cell_info mal_cell_info;

    /* Send CN_REQUEST_CELL_INFO */
    result = cn_request_cell_info(g_context_p, (cn_client_tag_t) 914);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_cell_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mal_cell_info.bands_available = 0x0000000F;
    mal_cell_info.country_code = 46;
    mal_cell_info.current_ac = 3;
    mal_cell_info.current_cell_id = 0x01020304;
    mal_cell_info.gsm_frame_nbr = 0x04030201;
    mal_cell_info.mcc_mnc_string = (uint8_t *) MCC_MNC_STRING;
    mal_cell_info.network_code = 5;
    mal_cell_info.network_type = MAL_NET_GSM_PREFERRED_PLMN;
    mal_cell_info.rat = MAL_NET_CELL_RAT_GSM;
    mal_cell_info.service_status = MAL_NET_LIMITED_SERVICE;

    net_event_callback(MAL_NET_CELL_INFO_GET_RESP, &mal_cell_info, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CELL_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CELL_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(914 == msg_p->client_tag);

    cell_info_p = (cn_cell_info_t *)msg_p->payload;
    TC_ASSERT(0x0000000F == cell_info_p->bands_available);
    TC_ASSERT(46 == cell_info_p->country_code);
    TC_ASSERT(3 == cell_info_p->current_ac);
    TC_ASSERT(0x01020304 == cell_info_p->current_cell_id);
    TC_ASSERT(0x04030201 == cell_info_p->gsm_frame_nbr);
    TC_ASSERT(strcmp(cell_info_p->mcc_mnc, MCC_MNC_STRING) == 0);
    TC_ASSERT(5 == cell_info_p->network_code);
    TC_ASSERT(MAL_NET_GSM_PREFERRED_PLMN == cell_info_p->network_type);
    TC_ASSERT(MAL_NET_CELL_RAT_GSM == cell_info_p->rat);
    TC_ASSERT(MAL_NET_LIMITED_SERVICE == cell_info_p->service_status);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_cell_info_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_CELL_INFO */
    result = cn_request_cell_info(g_context_p, (cn_client_tag_t) 927);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_cell_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_CELL_INFO_GET_RESP, NULL, MAL_NET_INSUFFICIENT_MEMORY, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CELL_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CELL_INFO == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(927 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_get_neighbour_cells_complete_info_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;
    cn_neighbour_cells_info_t *cn_neighbour_cells_info_p = NULL;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_GET_NEIGHBOUR_CELLS_COMPLETE_INFO */
    result = cn_request_get_neighbour_cells_complete_info(g_context_p, CN_NEIGHBOUR_RAT_TYPE_2G, (cn_client_tag_t) 987);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_neighbour_cells_complete_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    mal_neighbour_cells_info.rat_type = MAL_NET_NEIGHBOUR_RAT_2G;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid = 0x1A0F;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance = 39;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn = 106;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic = 31;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev = -71;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.neigh_info_type = 1;

    //CN_NET_NEIGHBOUR_CELLS_BASIC_EXT
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours = 3;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].cid = 0x2A13;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].rxlev = -84;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].cid = 0x1A10;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].rxlev = -79;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].cid = 0x1A11;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].rxlev = -93;

    //CN_NET_NEIGHBOUR_CELLS_EXT
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours = 3;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].arfcn = 105;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].bsic = 17;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].rxlev = -84;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].arfcn = 96;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].bsic = 15;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].rxlev = -79;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].arfcn = 62;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].bsic = 22;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].rxlev = -93;

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, &mal_neighbour_cells_info, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(987 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);

    cn_neighbour_cells_info_p = (cn_neighbour_cells_info_t *)msg_p->payload;
    TC_ASSERT(MAL_NET_NEIGHBOUR_RAT_2G == cn_neighbour_cells_info_p->rat_type);
    TC_ASSERT(0x0194 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc);
    TC_ASSERT(0x002D == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc);
    TC_ASSERT(0x0591 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac);
    TC_ASSERT(0x1A0F == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid);
    TC_ASSERT(39 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance);
    TC_ASSERT(106 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn);
    TC_ASSERT(31 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic);
    TC_ASSERT(-71 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev);
    TC_ASSERT(1 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.neigh_info_type);

    //CN_NET_NEIGHBOUR_CELLS_BASIC_EXT
    TC_ASSERT(3 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours);

    TC_ASSERT(0x0194 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].mcc);
    TC_ASSERT(0x002D == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].mnc);
    TC_ASSERT(0x0591 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].lac);
    TC_ASSERT(0x2A13 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].cid);
    TC_ASSERT(-84 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].rxlev);

    TC_ASSERT(0x0194 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].mcc);
    TC_ASSERT(0x002D == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].mnc);
    TC_ASSERT(0x0591 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].lac);
    TC_ASSERT(0x1A10 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].cid);
    TC_ASSERT(-79 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].rxlev);

    TC_ASSERT(0x0194 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].mcc);
    TC_ASSERT(0x002D == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].mnc);
    TC_ASSERT(0x0591 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].lac);
    TC_ASSERT(0x1A11 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].cid);
    TC_ASSERT(-93 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].rxlev);

    //CN_NET_NEIGHBOUR_CELLS_EXT
    TC_ASSERT(3 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours);

    TC_ASSERT(105 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].arfcn);
    TC_ASSERT(17 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].bsic);
    TC_ASSERT(-84 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].rxlev);

    TC_ASSERT(96 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].arfcn);
    TC_ASSERT(15 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].bsic);
    TC_ASSERT(-79 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].rxlev);

    TC_ASSERT(62 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].arfcn);
    TC_ASSERT(22 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].bsic);
    TC_ASSERT(-93 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].rxlev);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_get_neighbour_cells_complete_info_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;

    /* Send CN_REQUEST_GET_NEIGHBOUR_CELLS_COMPLETE_INFO */
    result = cn_request_get_neighbour_cells_complete_info(g_context_p, CN_NEIGHBOUR_RAT_TYPE_2G, (cn_client_tag_t) 988);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_neighbour_cells_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    memset(&mal_neighbour_cells_info, 0x0, sizeof(mal_net_neighbour_cells_info));

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, &mal_neighbour_cells_info, MAL_NET_INSUFFICIENT_MEMORY, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_neighbour_cells_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(988 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_get_neighbour_cells_complete_info_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;

    /* Send CN_REQUEST_GET_NEIGHBOUR_CELLS_COMPLETE_INFO */
    result = cn_request_get_neighbour_cells_complete_info(g_context_p, CN_NEIGHBOUR_RAT_TYPE_2G, (cn_client_tag_t) 988);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_neighbour_cells_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_neighbour_cells_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);//NULL data
    TC_ASSERT(988 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_neighbour_cells_info_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;
    cn_neighbour_cells_info_t *cn_neighbour_cells_info_p = NULL;
    cn_message_t *msg_p = NULL;

    mal_neighbour_cells_info.rat_type = MAL_NET_NEIGHBOUR_RAT_2G;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid = 0x1A0F;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance = 39;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn = 106;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic = 31;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev = -71;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.neigh_info_type = 1;

    //CN_NET_NEIGHBOUR_CELLS_BASIC_EXT
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours = 3;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].cid = 0x2A13;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].rxlev = -84;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].cid = 0x1A10;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].rxlev = -79;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].cid = 0x1A11;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].rxlev = -93;

    //CN_NET_NEIGHBOUR_CELLS_EXT
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours = 3;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].arfcn = 105;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].bsic = 17;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].rxlev = -84;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].arfcn = 96;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].bsic = 15;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].rxlev = -79;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].arfcn = 62;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].bsic = 22;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].rxlev = -93;

    /* Generate fake MAL unsolicited event */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, &mal_neighbour_cells_info, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive CN_EVENT_NEIGHBOUR_CELLS_INFO */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_neighbour_cells_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_NEIGHBOUR_CELLS_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);

    cn_neighbour_cells_info_p = (cn_neighbour_cells_info_t *)msg_p->payload;
    TC_ASSERT(MAL_NET_NEIGHBOUR_RAT_2G == cn_neighbour_cells_info_p->rat_type);
    TC_ASSERT(0x0194 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc);
    TC_ASSERT(0x002D == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc);
    TC_ASSERT(0x0591 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac);
    TC_ASSERT(0x1A0F == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid);
    TC_ASSERT(39 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance);
    TC_ASSERT(106 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn);
    TC_ASSERT(31 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic);
    TC_ASSERT(-71 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev);
    TC_ASSERT(1 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.neigh_info_type);

    //CN_NET_NEIGHBOUR_CELLS_BASIC_EXT
    TC_ASSERT(3 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours);

    TC_ASSERT(0x0194 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].mcc);
    TC_ASSERT(0x002D == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].mnc);
    TC_ASSERT(0x0591 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].lac);
    TC_ASSERT(0x2A13 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].cid);
    TC_ASSERT(-84 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[0].rxlev);

    TC_ASSERT(0x0194 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].mcc);
    TC_ASSERT(0x002D == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].mnc);
    TC_ASSERT(0x0591 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].lac);
    TC_ASSERT(0x1A10 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].cid);
    TC_ASSERT(-79 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[1].rxlev);

    TC_ASSERT(0x0194 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].mcc);
    TC_ASSERT(0x002D == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].mnc);
    TC_ASSERT(0x0591 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].lac);
    TC_ASSERT(0x1A11 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].cid);
    TC_ASSERT(-93 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.gsm_nmr_info[2].rxlev);

    //CN_NET_NEIGHBOUR_CELLS_EXT
    TC_ASSERT(3 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours);

    TC_ASSERT(105 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].arfcn);
    TC_ASSERT(17 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].bsic);
    TC_ASSERT(-84 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].rxlev);

    TC_ASSERT(96 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].arfcn);
    TC_ASSERT(15 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].bsic);
    TC_ASSERT(-79 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].rxlev);

    TC_ASSERT(62 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].arfcn);
    TC_ASSERT(22 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].bsic);
    TC_ASSERT(-93 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].rxlev);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_get_neighbour_cells_extd_info_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;
    cn_neighbour_cells_info_t *cn_neighbour_cells_info_p = NULL;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_GET_NEIGHBOUR_CELLS_EXTD_INFO */
    result = cn_request_get_neighbour_cells_extd_info(g_context_p, CN_NEIGHBOUR_RAT_TYPE_3G, (cn_client_tag_t) 990);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_neighbour_cells_extd_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    mal_neighbour_cells_info.rat_type = MAL_NET_NEIGHBOUR_RAT_3G;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_serving_cell_info.current_ucid = 0x0137834C;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_serving_cell_info.current_mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_serving_cell_info.current_mnc = 0x0047;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_serving_cell_info.current_psc = 121;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_serving_cell_info.current_dl_uarfcn = 10782;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_serving_cell_info.current_utra_carrier_rssi = 28;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_ecno  = 32;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_rscp = 35;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_pathloss = 50;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.num_of_dl_uarfcn = 1;

    //NET_UTRAN_NEIGH_LIST_SEQ
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].dl_uarfcn = 10782;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].utra_carrier_rssi = 28;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].num_of_cells = 3;

    //NET_UTRAN_CELL_LIST_SEQ
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].ucid = 0x0137834D;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].psc = 167;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].cpich_ecno = 16;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].cpich_rscp = 24;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].cpich_pathloss = 48;

    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].ucid = 0x0137832C;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].psc = 37;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].cpich_ecno = 11;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].cpich_rscp = 27;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].cpich_pathloss = 52;

    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[2].ucid = 0x01378336;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[2].psc = 138;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[2].cpich_ecno = 30;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[2].cpich_rscp = 23;
    mal_neighbour_cells_info.cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[2].cpich_pathloss = 49;

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, &mal_neighbour_cells_info, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(990 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);

    cn_neighbour_cells_info_p = (cn_neighbour_cells_info_t *)msg_p->payload;
    TC_ASSERT(MAL_NET_NEIGHBOUR_RAT_3G == cn_neighbour_cells_info_p->rat_type);
    TC_ASSERT(0x0137834C == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_ucid);
    TC_ASSERT(0x0194 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_mcc);
    TC_ASSERT(0x0047 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_mnc);
    TC_ASSERT(121 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_psc);
    TC_ASSERT(10782 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_dl_uarfcn);
    TC_ASSERT(28 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_utra_carrier_rssi);
    TC_ASSERT(32 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_ecno);
    TC_ASSERT(35 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_rscp);
    TC_ASSERT(50 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_serving_cell_info.current_cpich_pathloss);
    TC_ASSERT(1 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.num_of_dl_uarfcn);


    TC_ASSERT(10782 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].dl_uarfcn);
    TC_ASSERT(28 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].utra_carrier_rssi);
    TC_ASSERT(3 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].num_of_cells);

    TC_ASSERT(0x0137834D == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].ucid);
    TC_ASSERT(167 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].psc);
    TC_ASSERT(16 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].cpich_ecno);
    TC_ASSERT(24 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].cpich_rscp);
    TC_ASSERT(48 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[0].cpich_pathloss);

    TC_ASSERT(0x0137832C == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].ucid);
    TC_ASSERT(37 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].psc);
    TC_ASSERT(11 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].cpich_ecno);
    TC_ASSERT(27 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].cpich_rscp);
    TC_ASSERT(52 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[1].cpich_pathloss);

    TC_ASSERT(0x01378336 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[2].ucid);
    TC_ASSERT(138 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[2].psc);
    TC_ASSERT(30 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[2].cpich_ecno);
    TC_ASSERT(23 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[2].cpich_rscp);
    TC_ASSERT(49 == cn_neighbour_cells_info_p->cells_info.umts_cells_info.umts_info_extd.umts_uarfcn_info[0].umts_cell_detailed_info[2].cpich_pathloss);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_get_neighbour_cells_extd_info_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;
    cn_rat_type_t rat = s_reg_info.rat;
    s_reg_info.rat = CN_RAT_TYPE_GSM;

    /* Send CN_REQUEST_GET_NEIGHBOUR_CELLS_REPORTING */
    result = cn_request_get_neighbour_cells_extd_info(g_context_p, CN_NEIGHBOUR_RAT_TYPE_2G, (cn_client_tag_t) 991);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_neighbour_cells_extd_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    s_reg_info.rat = rat;
    memset(&mal_neighbour_cells_info, 0x0, sizeof(mal_net_neighbour_cells_info));

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, &mal_neighbour_cells_info, MAL_NET_INSUFFICIENT_MEMORY, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_neighbour_cells_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code); //MAL_NET_INSUFFICIENT_MEMORY
    TC_ASSERT(991 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_get_neighbour_cells_extd_info_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;
    cn_neighbour_cells_info_t *cn_neighbour_cells_info_p = NULL;
    cn_message_t *msg_p = NULL;
    cn_rat_type_t rat = s_reg_info.rat;
    s_reg_info.rat = CN_RAT_TYPE_GSM;

    /* Send CN_REQUEST_GET_NEIGHBOUR_CELLS_COMPLETE_INFO */
    result = cn_request_get_neighbour_cells_extd_info(g_context_p, CN_NEIGHBOUR_RAT_TYPE_2G, (cn_client_tag_t) 987);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_neighbour_cells_extd_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    s_reg_info.rat = rat;
    mal_neighbour_cells_info.rat_type = MAL_NET_NEIGHBOUR_RAT_2G;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid = 0x1A0F;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance = 39;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn = 106;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic = 31;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev = -71;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.neigh_info_type = 0;

    //CN_NET_NEIGHBOUR_CELLS_EXT
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours = 3;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].arfcn = 105;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].bsic = 17;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].rxlev = -84;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].arfcn = 96;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].bsic = 15;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].rxlev = -79;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].arfcn = 62;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].bsic = 22;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].rxlev = -93;

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, &mal_neighbour_cells_info, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(987 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);

    cn_neighbour_cells_info_p = (cn_neighbour_cells_info_t *)msg_p->payload;
    TC_ASSERT(MAL_NET_NEIGHBOUR_RAT_2G == cn_neighbour_cells_info_p->rat_type);
    TC_ASSERT(0x0194 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc);
    TC_ASSERT(0x002D == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc);
    TC_ASSERT(0x0591 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac);
    TC_ASSERT(0x1A0F == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid);
    TC_ASSERT(39 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance);
    TC_ASSERT(106 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn);
    TC_ASSERT(31 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic);
    TC_ASSERT(-71 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev);
    TC_ASSERT(0 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.neigh_info_type);

    //CN_NET_NEIGHBOUR_CELLS_BASIC_EXT
    TC_ASSERT(0 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_basic.num_of_neighbours);

    //CN_NET_NEIGHBOUR_CELLS_EXT
    TC_ASSERT(3 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours);

    TC_ASSERT(105 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].arfcn);
    TC_ASSERT(17 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].bsic);
    TC_ASSERT(-84 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].rxlev);

    TC_ASSERT(96 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].arfcn);
    TC_ASSERT(15 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].bsic);
    TC_ASSERT(-79 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].rxlev);

    TC_ASSERT(62 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].arfcn);
    TC_ASSERT(22 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].bsic);
    TC_ASSERT(-93 == cn_neighbour_cells_info_p->cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[2].rxlev);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_get_neighbour_cells_extd_info_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;

    /* Send CN_REQUEST_GET_NEIGHBOUR_CELLS_REPORTING */
    result = cn_request_get_neighbour_cells_extd_info(g_context_p, CN_NEIGHBOUR_RAT_TYPE_3G, (cn_client_tag_t) 993);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_neighbour_cells_extd_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    mal_neighbour_cells_info.rat_type = CN_NEIGHBOUR_RAT_TYPE_3G;

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, &mal_neighbour_cells_info, MAL_NET_INTERRUPTED, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_neighbour_cells_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO == msg_p->type);
    TC_ASSERT(CN_REQUEST_INTERRUPTED == msg_p->error_code); //MAL_NET_INTERRUPTED
    TC_ASSERT(993 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_get_neighbour_cells_extd_info_negative_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;

    /* Send CN_REQUEST_GET_NEIGHBOUR_CELLS_REPORTING */
    result = cn_request_get_neighbour_cells_extd_info(g_context_p, CN_NEIGHBOUR_RAT_TYPE_3G, (cn_client_tag_t) 993);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_neighbour_cells_extd_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_neighbour_cells_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code); //NULL mal data
    TC_ASSERT(993 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_get_neighbour_cells_extd_info_negative_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;
    cn_rat_type_t rat = s_reg_info.rat;
    s_reg_info.rat = CN_RAT_TYPE_GSM;

    /* Send CN_REQUEST_GET_NEIGHBOUR_CELLS_REPORTING */
    result = cn_request_get_neighbour_cells_extd_info(g_context_p, CN_NEIGHBOUR_RAT_TYPE_3G, (cn_client_tag_t) 991);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_neighbour_cells_extd_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    s_reg_info.rat = rat;
    memset(&mal_neighbour_cells_info, 0x0, sizeof(mal_net_neighbour_cells_info));

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, &mal_neighbour_cells_info, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_neighbour_cells_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code); //RAT Mismatch
    TC_ASSERT(991 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_automatic_network_registration_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_AUTOMATIC_NETWORK_REGISTRATION */
    result = cn_request_automatic_network_registration(g_context_p, (cn_client_tag_t) 777);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_automatic_network_registration failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_SET_RESP, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(777 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_automatic_network_registration_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_AUTOMATIC_NETWORK_REGISTRATION */
    result = cn_request_automatic_network_registration(g_context_p, (cn_client_tag_t) 12);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_automatic_network_registration failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_SET_RESP, NULL, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(12 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


/*
 * NOTE: The manual network registration tests below are run 3 times in a
 * row to test registration using all three operator formats;
 *      long operator name
 *      short operator name
 *      numeric
 *
 * In addition these tests are run twice. One set before the manual network
 * search test has been run, and a second set after. The reason for this is
 * that a prior network search may influence the results of manual network
 * registration in cases where the long or short name is used to reference
 * an operator.
 */
tc_result_t cn_request_manual_network_registration_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_registration_data mal_net_reg_data;
    cn_network_registration_req_data_t reg_info;

    static int operator_format = CN_OPERATOR_FORMAT_LONG;
    char *long_name_operator_p = LONG_OPERATOR_NAME_STRING;
    char *short_name_operator_p = SHORT_OPERATOR_NAME_STRING;
    char *numeric_operator_p;

    /* First set should always result in MCC_MNC_STRING.
     * Second set, after network search, should always result
     * in MCC_MNC_STRING_1.
     */
    if (operator_format <= CN_OPERATOR_FORMAT_NUMERIC) {
        numeric_operator_p = MCC_MNC_STRING;
    } else {
        numeric_operator_p = MCC_MNC_STRING_1;
    }

    /* Set operator format conditions */
    switch (operator_format % (CN_OPERATOR_FORMAT_NUMERIC + 1)) {
    case CN_OPERATOR_FORMAT_LONG:
        reg_info.format = CN_OPERATOR_FORMAT_LONG;
        strncpy(reg_info.operator, long_name_operator_p, sizeof(reg_info.operator));
        break;
    case CN_OPERATOR_FORMAT_SHORT:
        reg_info.format = CN_OPERATOR_FORMAT_SHORT;
        strncpy(reg_info.operator, short_name_operator_p, sizeof(reg_info.operator));
        break;
    case CN_OPERATOR_FORMAT_NUMERIC:
        reg_info.format = CN_OPERATOR_FORMAT_NUMERIC;
        strncpy(reg_info.operator, numeric_operator_p, sizeof(reg_info.operator));
        break;
    default:
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    reg_info.act = 2;

    /* Send CN_REQUEST_MANUAL_NETWORK_REGISTRATION */
    result = cn_request_manual_network_registration(g_context_p, &reg_info, (cn_client_tag_t) 727);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_manual_network_registration failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter check in the stub */
    if (!g_mal_net_manual_register__mnc_mcc_p) {
        CN_LOG_E("MCC+MNC in MAL stub not set!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(strcmp((const char *)g_mal_net_manual_register__mnc_mcc_p, numeric_operator_p) == 0);

    /* Generate fake MAL response */
    mal_net_reg_data.registration_status = 134;
    mal_net_reg_data.selection_mode      = 135;
    mal_net_reg_data.GSM_operator_code_1 = 136;
    mal_net_reg_data.GSM_operator_code_2 = 137;
    mal_net_reg_data.GSM_operator_code_3 = 138;
    mal_net_reg_data.fd_available_in_cell = 139;
    mal_net_reg_data.gsm_network_type    = 140;
    mal_net_reg_data.GPRS_support        = 141;
    mal_net_reg_data.GPRS_network_mode   = 142;
    mal_net_reg_data.CS_services         = 143;
    mal_net_reg_data.GPRS_services       = 144;
    mal_net_reg_data.EGPRS_support       = 145;
    mal_net_reg_data.DTM_support         = 146;
    mal_net_reg_data.current_RAC         = 147;
    mal_net_reg_data.HSDPA_available     = 148;
    mal_net_reg_data.HSUPA_available     = 149;
    mal_net_reg_data.camped_in_HPLMN     = 150;
    mal_net_reg_data.RAT_name            = 151;
    mal_net_reg_data.reject_code         = 152;
    mal_net_reg_data.current_LAC         = 34001;
    mal_net_reg_data.GSM_current_cell_id = 342456;
    net_event_callback(MAL_NET_SET_RESP, &mal_net_reg_data, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NETWORK_REGISTRATION */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MANUAL_NETWORK_REGISTRATION == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(727 == msg_p->client_tag);

exit:
    free(msg_p);

    /* Prepare for next test. Cycle through all three operator formats. */
    operator_format++;

    return tc_result;
}


tc_result_t cn_request_manual_network_registration_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_network_registration_req_data_t reg_info;

    strncpy(reg_info.operator, MCC_MNC_STRING, sizeof(reg_info.operator));
    reg_info.format = CN_OPERATOR_FORMAT_NUMERIC;
    reg_info.act = 2;

    /* Send CN_REQUEST_MANUAL_NETWORK_REGISTRATION */
    result = cn_request_manual_network_registration(g_context_p, &reg_info, (cn_client_tag_t) 728);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_manual_network_registration failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter check in the stub */
    TC_ASSERT(strcmp((const char *)g_mal_net_manual_register__mnc_mcc_p, MCC_MNC_STRING) == 0);

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_SET_RESP, NULL, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NETWORK_REGISTRATION */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MANUAL_NETWORK_REGISTRATION == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(728 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


/*
 * NOTE: The manual network registration with automatic fallback tests
 * below are run 3 times in a row to test registration using all three
 * operator formats;
 *      long operator name
 *      short operator name
 *      numeric
 *
 * In addition these tests are run twice. One set before the manual network
 * search test has been run, and a second set after. The reason for this is
 * that a prior network search may influence the results of manual network
 * registration in cases where the long or short name is used to reference
 * an operator.
 */tc_result_t cn_request_manual_network_registration_with_automatic_fallback_positive_1() /* scenario: manual registration succeeded */
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_registration_data mal_net_reg_data;
    cn_network_registration_req_data_t reg_info;

    static int operator_format = CN_OPERATOR_FORMAT_LONG;
    char *long_name_operator_p = LONG_OPERATOR_NAME_STRING;
    char *short_name_operator_p = SHORT_OPERATOR_NAME_STRING;
    char *numeric_operator_p;

    /* First set should always result in MCC_MNC_STRING.
     * Second set, after network search, should always result
     * in MCC_MNC_STRING_1.
     */
    if (operator_format <= CN_OPERATOR_FORMAT_NUMERIC) {
        numeric_operator_p = MCC_MNC_STRING;
    } else {
        numeric_operator_p = MCC_MNC_STRING_1;
    }

    /* Set operator format conditions */
    switch (operator_format % (CN_OPERATOR_FORMAT_NUMERIC + 1)) {
    case CN_OPERATOR_FORMAT_LONG:
        reg_info.format = CN_OPERATOR_FORMAT_LONG;
        strncpy(reg_info.operator, long_name_operator_p, sizeof(reg_info.operator));
        break;
    case CN_OPERATOR_FORMAT_SHORT:
        reg_info.format = CN_OPERATOR_FORMAT_SHORT;
        strncpy(reg_info.operator, short_name_operator_p, sizeof(reg_info.operator));
        break;
    case CN_OPERATOR_FORMAT_NUMERIC:
        reg_info.format = CN_OPERATOR_FORMAT_NUMERIC;
        strncpy(reg_info.operator, numeric_operator_p, sizeof(reg_info.operator));
        break;
    default:
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    reg_info.act = 2;

    /* Send CN_REQUEST_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK */
    result = cn_request_manual_network_registration_with_automatic_fallback(g_context_p, &reg_info, (cn_client_tag_t) 728);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_manual_network_registration_with_automatic_fallback failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter check in the stub */
    if (!g_mal_net_manual_register__mnc_mcc_p) {
        CN_LOG_E("MCC+MNC in MAL stub not set!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(strcmp((const char *)g_mal_net_manual_register__mnc_mcc_p, numeric_operator_p) == 0);

    /* Generate fake MAL response */
    mal_net_reg_data.registration_status = 134;
    mal_net_reg_data.selection_mode      = 135;
    mal_net_reg_data.GSM_operator_code_1 = 136;
    mal_net_reg_data.GSM_operator_code_2 = 137;
    mal_net_reg_data.GSM_operator_code_3 = 138;
    mal_net_reg_data.fd_available_in_cell   = 139;
    mal_net_reg_data.gsm_network_type    = 140;
    mal_net_reg_data.GPRS_support        = 141;
    mal_net_reg_data.GPRS_network_mode   = 142;
    mal_net_reg_data.CS_services         = 143;
    mal_net_reg_data.GPRS_services       = 144;
    mal_net_reg_data.EGPRS_support       = 145;
    mal_net_reg_data.DTM_support         = 146;
    mal_net_reg_data.current_RAC         = 147;
    mal_net_reg_data.HSDPA_available     = 148;
    mal_net_reg_data.HSUPA_available     = 149;
    mal_net_reg_data.camped_in_HPLMN     = 150;
    mal_net_reg_data.RAT_name            = 151;
    mal_net_reg_data.reject_code         = 152;
    mal_net_reg_data.current_LAC         = 34001;
    mal_net_reg_data.GSM_current_cell_id = 342456;
    net_event_callback(MAL_NET_SET_RESP, &mal_net_reg_data, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NETWORK_REGISTRATION */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(728 == msg_p->client_tag);

exit:
    free(msg_p);

    /* Prepare for next test. Cycle through all three operator formats. */
    operator_format++;

    return tc_result;
}


tc_result_t cn_request_manual_network_registration_with_automatic_fallback_negative_1() /* scenario: manual registration fails in the first mal response, but automatic fallback succeeds */
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_network_registration_req_data_t reg_info;

    strncpy(reg_info.operator, MCC_MNC_STRING, sizeof(reg_info.operator));
    reg_info.format = CN_OPERATOR_FORMAT_NUMERIC;
    reg_info.act = 2;

    /* Send CN_REQUEST_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK */
    result = cn_request_manual_network_registration_with_automatic_fallback(g_context_p, &reg_info, (cn_client_tag_t) 992);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_manual_network_registration_with_automatic_fallback failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter check in the stub */
    TC_ASSERT(strcmp((const char *)g_mal_net_manual_register__mnc_mcc_p, MCC_MNC_STRING) == 0);

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_SET_RESP, NULL, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_NETWORK_SELECT_MODE_SET_RESP, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NETWORK_REGISTRATION */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK == msg_p->type);
    TC_ASSERT(CN_SUCCESS_AUTOMATIC_FALLBACK == msg_p->error_code);
    TC_ASSERT(0 == msg_p->payload_size);
    TC_ASSERT(992 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_manual_network_registration_with_automatic_fallback_negative_2() /* scenario: manual registration fails in the first mal response, but automatic fallback fails in second mal response */
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_network_registration_req_data_t reg_info;

    strncpy(reg_info.operator, MCC_MNC_STRING, sizeof(reg_info.operator));
    reg_info.format = CN_OPERATOR_FORMAT_NUMERIC;
    reg_info.act = 2;

    /* Send CN_REQUEST_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK */
    result = cn_request_manual_network_registration_with_automatic_fallback(g_context_p, &reg_info, (cn_client_tag_t) 937);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_manual_network_registration_with_automatic_fallback failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter check in the stub */
    TC_ASSERT(strcmp((const char *)g_mal_net_manual_register__mnc_mcc_p, MCC_MNC_STRING) == 0);

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_SET_RESP, NULL, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_NETWORK_SELECT_MODE_SET_RESP, NULL, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NETWORK_REGISTRATION */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(0 == msg_p->payload_size);
    TC_ASSERT(937 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_network_deregister_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_NETWORK_DEREGISTER */
    result = cn_request_network_deregister(g_context_p, (cn_client_tag_t) 764);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_network_deregister failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_CS_CONTROL_RESP, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NETWORK_DEREGISTER */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_NETWORK_DEREGISTER == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(764 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_network_deregister_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_NETWORK_DEREGISTER */
    result = cn_request_network_deregister(g_context_p, (cn_client_tag_t) 32);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_network_deregister failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_CS_CONTROL_RESP, NULL, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NETWORK_DEREGISTER */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_NETWORK_DEREGISTER == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(32 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_net_query_mode_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_registration_info_t *cn_info_p = NULL;
    mal_net_registration_info mal_info;
    char *num_p = "12345";
    char long_op[] = "long_op";
    char short_op[] = "short_op";

    /* Setup fake MAL response contents */
    mal_info.reg_status = 1;
    mal_info.rat = MAL_NET_RAT_UMTS;
    mal_info.selection_mode = MAL_NET_SELECT_MODE_AUTOMATIC;
    mal_info.cid = 1;
    mal_info.lac = 0xAAAA;
    mal_info.mcc_mnc_string = (uint8_t *)num_p;
    mal_info.gprs_attached = 1;
    mal_info.cs_attached = 1;
    /* Send CN_REQUEST_NET_QUERY_MODE */
    result = cn_request_net_query_mode(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_operator_code failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Load operator name into PLMN lookup table */
    plmn_add_nitz_name(num_p,
                       long_op, sizeof(long_op) - 1, /*spare*/ 0, CHARSET_UTF_8,
                       short_op, sizeof(short_op) - 1, /*spare*/ 0, CHARSET_UTF_8);

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_info,  MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    TC_ASSERT(CN_RESPONSE_NET_QUERY_MODE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);
    cn_info_p = (cn_registration_info_t *) msg_p->payload;
    TC_ASSERT(1 == cn_info_p->reg_status);
    TC_ASSERT(1 == cn_info_p->gprs_attached);
    TC_ASSERT(CN_RAT_TYPE_UMTS == cn_info_p->rat);
    TC_ASSERT(1 == cn_info_p->cid);
    TC_ASSERT(0xAAAA == cn_info_p->lac);
    TC_ASSERT(0 == strcmp(num_p, cn_info_p->mcc_mnc));
    TC_ASSERT(0 == strcmp(long_op, cn_info_p->long_operator_name));
    TC_ASSERT(0 == strcmp(short_op, cn_info_p->short_operator_name));

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_net_query_mode_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_registration_info mal_info;

    /* Send CN_REQUEST_NET_QUERY_MODE */
    result = cn_request_net_query_mode(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_manual_network_search failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_info, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NET_QUERY_MODE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    TC_ASSERT(CN_RESPONSE_NET_QUERY_MODE == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_manual_network_search_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    char *numeric_p = MCC_MNC_STRING;
    char *long_name_p = LONG_OPERATOR_NAME_STRING;
    char *short_name_p = SHORT_OPERATOR_NAME_STRING;
    mal_net_manual_search_data mal_list;
    mal_net_network_node mal_nodes[3];
    cn_uint32_t i;
    cn_manual_network_search_data_t *cn_data_p = NULL;
    unsigned long mcc_mnc;
    uint16_t mcc, mnc;

    /* Extract MCC and MNC from numeric operator string */
    mcc_mnc = strtoul(numeric_p, NULL, 16);

    if (0xFFFFF < mcc_mnc) {
        mcc = (uint16_t)(mcc_mnc >> 12);
        mnc = (uint16_t)(mcc_mnc & 0x00FFF);
    } else {
        mcc = (uint16_t)(mcc_mnc >> 8);
        mnc = (uint16_t)(mcc_mnc & 0x00FF);
    }

    /* Setup fake MAL response contents */
    memset(&mal_nodes[0], 0x00, sizeof(mal_nodes));
    mal_list.num_of_networks = 3;
    mal_list.head = &mal_nodes[0];
    mal_list.tail = &mal_nodes[2];

    for (i = 0; i < mal_list.num_of_networks; i++) {
        mal_nodes[i].index = 1;
        mal_nodes[i].network_status = 1;
        mal_nodes[i].band_info = 1;
        mal_nodes[i].network_type = 1;
        mal_nodes[i].umts_available = 0;
        mal_nodes[i].current_lac = 0xAAAA;
        mal_nodes[i].country_code = mcc;
        mal_nodes[i].network_code = mnc;
        (void) strncpy((char *) mal_nodes[i].mcc_mnc, numeric_p, sizeof(mal_nodes[0].mcc_mnc) - 1);

        if (i + 1 < mal_list.num_of_networks) {
            mal_nodes[i].next = &mal_nodes[i + 1];
        } else {
            mal_nodes[i].next = NULL;
        }

    }

    /* Send CN_REQUEST_MANUAL_NETWORK_SEARCH */
    result = cn_request_manual_network_search(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_manual_network_search failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Load same operator name, but twice using different MCC+MNC, into PLMN lookup table */
    plmn_add_nitz_name(MCC_MNC_STRING,
                       long_name_p, strlen(long_name_p), /*spare*/ 0, CHARSET_UTF_8,
                       short_name_p, strlen(short_name_p), /*spare*/ 0, CHARSET_UTF_8);
    plmn_add_nitz_name(MCC_MNC_STRING_1,
                       long_name_p, strlen(long_name_p), /*spare*/ 0, CHARSET_UTF_8,
                       short_name_p, strlen(short_name_p), /*spare*/ 0, CHARSET_UTF_8);

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_MODEM_AVAILABLE_GET_RESP, &mal_list,  MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    TC_ASSERT(CN_RESPONSE_MANUAL_NETWORK_SEARCH == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

    cn_data_p = (cn_manual_network_search_data_t *) msg_p->payload;

    TC_ASSERT(3 == cn_data_p->num_of_networks);

    for (i = 0; i < cn_data_p->num_of_networks; i++) {
        TC_ASSERT(1 == cn_data_p->cn_network_info[i].index);
        TC_ASSERT(1 == cn_data_p->cn_network_info[i].network_status);
        TC_ASSERT(1 == cn_data_p->cn_network_info[i].band_info);
        TC_ASSERT(1 == cn_data_p->cn_network_info[i].network_type);
        TC_ASSERT(0xAAAA == cn_data_p->cn_network_info[i].current_lac);
        TC_ASSERT(mcc == cn_data_p->cn_network_info[i].country_code);
        TC_ASSERT(mnc == cn_data_p->cn_network_info[i].network_code);
        TC_ASSERT(0 == strcmp(long_name_p, cn_data_p->cn_network_info[i].long_op_name));
        TC_ASSERT(0 == strcmp(short_name_p, cn_data_p->cn_network_info[i].short_op_name));
        TC_ASSERT(0 == strcmp(numeric_p, cn_data_p->cn_network_info[i].mcc_mnc));
    }

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_manual_network_search_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_manual_search_data mal_list;

    /* Send CN_REQUEST_MANUAL_NETWORK_SEARCH */
    result = cn_request_manual_network_search(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_manual_network_search failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_MODEM_AVAILABLE_GET_RESP, &mal_list,  MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    TC_ASSERT(CN_RESPONSE_MANUAL_NETWORK_SEARCH == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_interrupt_network_search_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_INTERRUPT_NETWORK_SEARCH */
    result = cn_request_interrupt_network_search(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_interrupt_network_search failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_AVAILABLE_CANCEL_RESP, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_INTERRUPT_NETWORK_SEARCH */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_INTERRUPT_NETWORK_SEARCH == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_interrupt_network_search_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_INTERRUPT_NETWORK_SEARCH */
    result = cn_request_interrupt_network_search(g_context_p, (cn_client_tag_t) 3334);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_interrupt_network_search failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_AVAILABLE_CANCEL_RESP, NULL, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_INTERRUPT_NETWORK_SEARCH */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_INTERRUPT_NETWORK_SEARCH == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(3334 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_current_call_list_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_call_context mal_call_list[3];
    char *number_p = "0123456789";
    char *connected_nr_p = "55";
    char *sub_addr_p = "sub_addr_p";
    uint16_t name_str[] = { 'r', 'e', 'm', 'o', 't', 'e', '_', 'p', 'a', 'r', 't', 'y' };
    connected_line line = { 1, 1, connected_nr_p, 1, sub_addr_p };
    exit_cause cause = { CALL_CAUSE_TYPE_SERVER, 1, 0, NULL };
    cn_call_list_t *cn_call_list_p = NULL;
    cn_uint32_t i = 0;

    memset(&mal_call_list, 0, sizeof(mal_call_list));

    /* Setup fake MAL response contents */
    g_mal_no_of_calls = 3;
    g_mal_call_list_p = &mal_call_list;

    for (i = 0; i < g_mal_no_of_calls; i++) {
        mal_call_list[i].callState = CALL_ACTIVE;
        mal_call_list[i].callId = 1;
        mal_call_list[i].addrType = 145;
        mal_call_list[i].message_type = 1;
        mal_call_list[i].message_direction = 1;
        mal_call_list[i].transaction_id = 11;
        mal_call_list[i].isMpty = 1;
        mal_call_list[i].isMT = 1;
        mal_call_list[i].als = 1;
        mal_call_list[i].call_mode = MAL_CALL_MODE_SPEECH;
        mal_call_list[i].isVoicePrivacy = 1;
        mal_call_list[i].number = number_p;
        mal_call_list[i].numberPresentation = CN_USER_SCREENED_PASSED | CN_PRESENTATION_RESTRICTED;
        mal_call_list[i].call_cli_cause = 4;
        mal_call_list[i].name_length = sizeof(name_str) / sizeof(uint16_t);
        mal_call_list[i].name = name_str;
        mal_call_list[i].namePresentation = CN_USER_SCREENED_FAILED | CN_PRESENTATION_UNAVAILABLE;
        mal_call_list[i].connectedLine = line;
        mal_call_list[i].call_cause = cause;
    }

    /* Send CN_REQUEST_CURRENT_CALL_LIST */
    result = cn_request_current_call_list(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_current_call_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Synchronous MAL call made by CN */

    /* No MAL parameter to check in the stub */

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_INTERRUPT_NETWORK_SEARCH */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CURRENT_CALL_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

    cn_call_list_p = (cn_call_list_t *) msg_p->payload;

    TC_ASSERT(g_mal_no_of_calls == cn_call_list_p->nr_of_calls);

    for (i = 0; i < cn_call_list_p->nr_of_calls; i++) {
        TC_ASSERT(CN_CALL_STATE_ACTIVE == cn_call_list_p->call_context[i].call_state);
        TC_ASSERT(1 == cn_call_list_p->call_context[i].call_id);
        TC_ASSERT(145 == cn_call_list_p->call_context[i].address_type);
        TC_ASSERT(1 == cn_call_list_p->call_context[i].message_type);
        TC_ASSERT(1 == cn_call_list_p->call_context[i].message_direction);
        TC_ASSERT(11 == cn_call_list_p->call_context[i].transaction_id);
        TC_ASSERT(1 == cn_call_list_p->call_context[i].is_multiparty);
        TC_ASSERT(1 == cn_call_list_p->call_context[i].is_MT);
        TC_ASSERT(1 == cn_call_list_p->call_context[i].als);
        TC_ASSERT(CN_CALL_MODE_SPEECH == cn_call_list_p->call_context[i].mode);
        TC_ASSERT(1 == cn_call_list_p->call_context[i].is_voice_privacy);
        TC_ASSERT(4 == cn_call_list_p->call_context[i].cause_no_cli);
        TC_ASSERT(CN_CALL_CAUSE_TYPE_SENDER_SERVER == cn_call_list_p->call_context[i].call_cause.cause_type_sender);
        TC_ASSERT(1 == cn_call_list_p->call_context[i].call_cause.cause);
        TC_ASSERT((CN_USER_SCREENED_PASSED | CN_PRESENTATION_RESTRICTED) == cn_call_list_p->call_context[i].number_presentation);
        TC_ASSERT(0 == strcmp(number_p, cn_call_list_p->call_context[i].number));
        TC_ASSERT((CN_USER_SCREENED_FAILED | CN_PRESENTATION_UNAVAILABLE) == cn_call_list_p->call_context[i].name_presentation);
        TC_ASSERT(0 == memcmp(name_str, cn_call_list_p->call_context[i].name, sizeof(name_str)));
        TC_ASSERT(sizeof(name_str) / sizeof(uint16_t) == cn_call_list_p->call_context[i].name_char_length);
    }

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_current_call_list_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint16_t name_str[] = { 'r', 'e', 'm', 'o', 't', 'e', '_', 'p', 'a', 'r', 't', 'y' };
    mal_call_context mal_call_list[1];

    memset(&mal_call_list, 0, sizeof(mal_call_list));

    /* Set MAL stub parameters (negative result) */
    g_mal_call_list_p = &mal_call_list;
    g_mal_no_of_calls = 1;
    g_mal_return_value = MAL_NET_GENERAL_ERROR;

    mal_call_list[0].number = "0123456789";
    mal_call_list[0].name = name_str;
    mal_call_list[0].name_length = sizeof(name_str) / sizeof(uint16_t);

    /* Send CN_REQUEST_CURRENT_CALL_LIST */
    result = cn_request_current_call_list(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_manual_network_search failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Synchronous MAL call made by CN */

    /* No MAL parameter to check in the stub */

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_INTERRUPT_NETWORK_SEARCH */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_current_call_list_negative_1 failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CURRENT_CALL_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_return_value = 0; /* Reset MAL result */

    return tc_result;
}

/*
 * Test a successfull dial WITHOUT DTMF string
 */
#define BC_DATA "artuifgsuioestyuhts"
#define BC_LENGTH 20
tc_result_t cn_request_dial_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_call_dailup mal_dial;
    char *number_p = "0123456789";
    char *dialed_p = number_p;
    uint16_t name_str[] = { 't', 'e', 's', 't' };
    cn_dial_t cn_dial;
    cn_message_t *msg_p = NULL;
    cn_message_t *evt_p = NULL;
    mal_call_context cc;
    cn_response_dial_t *response_dial_p = NULL;

    char *dtmf_string_p = malloc(CN_MAX_STRING_BUFF);
    size_t dtmf_string_len;

    if (!dtmf_string_p) {
        CN_LOG_E("Failure on line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    memset(dtmf_string_p, '*', CN_MAX_STRING_BUFF - 1);
    dtmf_string_p[CN_MAX_STRING_BUFF - 1] = 0;
    dtmf_string_len = strlen(dtmf_string_p);
    g_mal_call_request_dtmf_send__dtmf_info.dtmf_string = dtmf_string_p;
    g_mal_call_request_dtmf_send__dtmf_info.length = dtmf_string_len;

    memset(&cc, 0, sizeof(cc));
    cc.callId    = 12;
    cc.callState = CALL_DIALING;
    cc.number    = number_p;
    cc.name      = name_str;
    cc.name_length = sizeof(name_str) / sizeof(uint16_t);

    memset(&mal_dial, 0, sizeof(mal_dial));
    memset(&cn_dial, 0, sizeof(cn_dial));
    (void) strncpy(cn_dial.phone_number, number_p, CN_MAX_STRING_BUFF);
    cn_dial.call_type = CN_CALL_TYPE_VOICE_CALL;
    cn_dial.sat_initiated = FALSE;
    cn_dial.clir = 1;
    cn_dial.bc_length = 20;
    memmove(cn_dial.bc_data, BC_DATA, BC_LENGTH);

    /* MAL parameter to check in the stub */
    g_mal_string_length = CN_MAX_STRING_BUFF;
    mal_dial.phone_number = (char *) calloc(1, CN_MAX_STRING_BUFF);
    mal_dial.bc_data = calloc(1, 248);
    g_mal_request_data_p = &mal_dial;


    /**********************************************************************
     * Dial & response
     **********************************************************************/

    /* Send CN_REQUEST_DIAL */
    result = cn_request_dial(g_context_p, &cn_dial, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* check stub values */
    TC_ASSERT(20 == mal_dial.bc_length);
    TC_ASSERT(memcmp(mal_dial.bc_data, BC_DATA, mal_dial.bc_length) == 0);
    CN_LOG_D("mal_dial.bc_data: \"%s\"", mal_dial.bc_data);
    TC_ASSERT(0 == strcmp(dialed_p, mal_dial.phone_number));
    TC_ASSERT(cn_dial.clir == mal_dial.clir);

    /* Generate fake MAL response */
    call_event_callback(MAL_CALL_MODEM_CREATE_RESP, &cc, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DIAL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_DIAL == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);
    response_dial_p = (cn_response_dial_t *) msg_p->payload;
    TC_ASSERT(CN_SERVICE_TYPE_AFTER_SAT_CC_VOICE_CALL == response_dial_p->service_type);
    TC_ASSERT(12 == response_dial_p->data.call_id);

    /**********************************************************************
    * Call state change: CALL_DIALING (new call)
    **********************************************************************/

    /* Generate fake MAL call state change event */
    call_event_callback(MAL_CALL_STATE_CHANGED, &cc, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &evt_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!evt_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    free(evt_p);
    evt_p = NULL;


    /**********************************************************************
     * Call state change: CALL_ACTIVE
     **********************************************************************/
    cc.callState = CALL_ACTIVE;

    /* Generate fake MAL call state change event */
    call_event_callback(MAL_CALL_STATE_CHANGED, &cc, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &evt_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!evt_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    free(evt_p);
    evt_p = NULL;


    /**********************************************************************
     * Call state change: CALL_IDLE
     **********************************************************************/

    cc.callState                        = CALL_IDLE;
    cc.call_cause.cause_type_sender     = CN_CALL_CAUSE_TYPE_SENDER_CLIENT;
    cc.call_cause.cause                 = 3;
    cc.call_cause.detailed_cause_length = strlen(__func__);
    cc.call_cause.detailed_cause        = (uint8_t *) __func__;

    /* Generate fake MAL call state change event */
    call_event_callback(MAL_CALL_STATE_CHANGED, &cc, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &evt_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!evt_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    free(evt_p);
    evt_p = NULL;


    /**********************************************************************
     * Evaluate DTMF result, make sure global variable hasn't been touched
     * since no DTMF should have been sent
     **********************************************************************/
    TC_ASSERT(g_mal_call_request_dtmf_send__dtmf_info.length         == dtmf_string_len);
    TC_ASSERT(g_mal_call_request_dtmf_send__dtmf_info.dtmf_string    == dtmf_string_p);
    TC_ASSERT(g_mal_call_request_dtmf_send__dtmf_info.dtmf_string[0] == 42);

exit:
    free(msg_p);
    free(evt_p);
    free(mal_dial.phone_number);
    free(mal_dial.bc_data);
    free(dtmf_string_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */
    g_mal_call_request_dtmf_send__dtmf_info.dtmf_string = NULL;

    return tc_result;
}

/* Succeeding emergency call.
 * For this test-case we rely on the ECC check updating the call type.
 */
tc_result_t cn_request_dial_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_call_dailup mal_dial;
    char *number_p = "112";
    cn_dial_t cn_dial;
    cn_message_t *msg_p = NULL;

    g_tf_pdc_check_enabled = TRUE;

    memset(&mal_dial, 0, sizeof(mal_dial));
    memset(&cn_dial, 0, sizeof(cn_dial));
    (void) strncpy(cn_dial.phone_number, number_p, CN_MAX_STRING_BUFF);
    cn_dial.clir = 1;
    cn_dial.call_type = CN_CALL_TYPE_EMERGENCY_CALL;

    /* setup stub variables */
    g_mal_string_length = strlen(number_p) + 1;
    mal_dial.phone_number = (char *) calloc(1, g_mal_string_length);
    g_mal_request_data_p = &mal_dial;

    /* Send CN_REQUEST_DIAL */
    result = cn_request_dial(g_context_p, &cn_dial, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dial_positive_1 failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check MAL parameters in the stub */
    TC_ASSERT(0 == strcmp(cn_dial.phone_number, mal_dial.phone_number));
    TC_ASSERT(cn_dial.clir == mal_dial.clir);
    TC_ASSERT(MAL_CALL_TYPE_EMERGENCY == mal_dial.call_type);

    /* Generate fake MAL response */
    call_event_callback(MAL_CALL_MODEM_CREATE_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DIAL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dial_positive_1 failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_DIAL == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_dial.phone_number);
    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

/*
 * Test a dial WITH DTMF, but DTMF SHOULD NOT BE SENT since call goes to idle (never becomes active).
 */
tc_result_t cn_request_dial_positive_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_call_dailup mal_dial;
    char *number_p = "0123456789w12345p67890";
    char *dialed_p = "0123456789";
    uint16_t name_str[] = { 't', 'e', 's', 't' };
    cn_dial_t cn_dial; // = { { '\0' }, 0, 1 };
    cn_message_t *msg_p = NULL;
    cn_message_t *evt_p = NULL;
    mal_call_context cc;
    memset(&mal_dial, 0, sizeof(mal_call_dailup));
    memset(&cn_dial, 0, sizeof(cn_dial_t));

    char *dtmf_string_p = malloc(CN_MAX_STRING_BUFF);
    size_t dtmf_string_len;

    if (!dtmf_string_p) {
        CN_LOG_E("Failure on line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    memset(dtmf_string_p, '*', CN_MAX_STRING_BUFF - 1);
    dtmf_string_p[CN_MAX_STRING_BUFF - 1] = 0;
    dtmf_string_len = strlen(dtmf_string_p);
    g_mal_call_request_dtmf_send__dtmf_info.dtmf_string = dtmf_string_p;
    g_mal_call_request_dtmf_send__dtmf_info.length = dtmf_string_len;

    memset(&cc, 0, sizeof(cc));
    cc.callId    = 1;
    cc.callState = CALL_DIALING;
    cc.number    = number_p;
    cc.name      = name_str;
    cc.name_length = sizeof(name_str) / sizeof(uint16_t);

    (void) strncpy(cn_dial.phone_number, number_p, CN_MAX_STRING_BUFF);
    cn_dial.clir = 1;

    /* MAL parameter to check in the stub */
    g_mal_string_length = CN_MAX_STRING_BUFF;
    mal_dial.phone_number = (char *) calloc(1, CN_MAX_STRING_BUFF);
    g_mal_request_data_p = &mal_dial;


    /**********************************************************************
     * Dial & response
     **********************************************************************/

    /* Send CN_REQUEST_DIAL */
    result = cn_request_dial(g_context_p, &cn_dial, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response */
    call_event_callback(MAL_CALL_MODEM_CREATE_RESP, &cc, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DIAL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_DIAL == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

    TC_ASSERT(0 == strcmp(dialed_p, mal_dial.phone_number));
    TC_ASSERT(cn_dial.clir == mal_dial.clir);


    /**********************************************************************
     * Call state change: CALL_DIALING (new call)
     **********************************************************************/

    /* Generate fake MAL call state change event */
    call_event_callback(MAL_CALL_STATE_CHANGED, &cc, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &evt_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!evt_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    free(evt_p);
    evt_p = NULL;


    /**********************************************************************
     * Call state change: CALL_ACTIVE, another call (call id = 2) goes active!
     **********************************************************************/
    cc.callId = 2;
    cc.callState = CALL_ACTIVE;

    /* Generate fake MAL call state change event */
    call_event_callback(MAL_CALL_STATE_CHANGED, &cc, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &evt_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!evt_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    free(evt_p);
    evt_p = NULL;


    /**********************************************************************
     * Call state change: CALL_IDLE, another call (call id = 2) goes idle!
     **********************************************************************/
    cc.callState                        = CALL_IDLE;
    cc.call_cause.cause_type_sender     = CN_CALL_CAUSE_TYPE_SENDER_CLIENT;
    cc.call_cause.cause                 = 3;
    cc.call_cause.detailed_cause_length = strlen(__func__);
    cc.call_cause.detailed_cause        = (uint8_t *) __func__;

    /* Generate fake MAL call state change event */
    call_event_callback(MAL_CALL_STATE_CHANGED, &cc, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &evt_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!evt_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    free(evt_p);
    evt_p = NULL;


    /**********************************************************************
     * Call state change: CALL_IDLE, our call goes idle
     **********************************************************************/
    cc.callId                           = 1;
    cc.callState                        = CALL_IDLE;
    cc.call_cause.cause_type_sender     = CN_CALL_CAUSE_TYPE_SENDER_CLIENT;
    cc.call_cause.cause                 = 3;
    cc.call_cause.detailed_cause_length = strlen(__func__);
    cc.call_cause.detailed_cause        = (uint8_t *) __func__;

    /* Generate fake MAL call state change event */
    call_event_callback(MAL_CALL_STATE_CHANGED, &cc, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &evt_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!evt_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    free(evt_p);
    evt_p = NULL;


    /**********************************************************************
     * Evaluate DTMF result, make sure global variable hasn't been touched
     * since no DTMF should have been sent
     **********************************************************************/
    TC_ASSERT(g_mal_call_request_dtmf_send__dtmf_info.length         == dtmf_string_len);
    TC_ASSERT(g_mal_call_request_dtmf_send__dtmf_info.dtmf_string    == dtmf_string_p);
    TC_ASSERT(g_mal_call_request_dtmf_send__dtmf_info.dtmf_string[0] == 42);

exit:
    free(msg_p);
    free(evt_p);
    free(dtmf_string_p);
    free(mal_dial.phone_number);

    g_mal_request_data_p = NULL; /* Reset MAL result */
    g_mal_call_request_dtmf_send__dtmf_info.dtmf_string = NULL;

    return tc_result;
}

/*
 * Test a dial WITH DTMF and DTMF SHOULD BE SENT since call goes to active.
 */
tc_result_t cn_request_dial_positive_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_call_dailup mal_dial;
    char *number_p = "0123456789w12345p67890";
    char *dialed_p = "0123456789";
    char *dtmf_p   =           "p12345p67890"; // Note, expected DTMF begins with p, not w
    size_t dtmf_len = strlen(dtmf_p);
    uint16_t name_str[] = { 't', 'e', 's', 't' };
    cn_dial_t cn_dial; // = { { '\0' }, 0, 1 };
    cn_message_t *msg_p = NULL;
    cn_message_t *evt_p = NULL;
    mal_call_context cc;
    memset(&mal_dial, 0, sizeof(mal_call_dailup));
    memset(&cn_dial, 0, sizeof(cn_dial_t));
    memset(&cc, 0, sizeof(cc));
    cc.callId    = 1;
    cc.callState = CALL_DIALING;
    cc.number    = number_p;
    cc.name      = name_str;
    cc.name_length = sizeof(name_str) / sizeof(uint16_t);

    (void) strncpy(cn_dial.phone_number, number_p, CN_MAX_STRING_BUFF);
    cn_dial.clir = 1;

    /* MAL parameter to check in the stub */
    g_mal_string_length = CN_MAX_STRING_BUFF;
    mal_dial.phone_number = (char *) calloc(1, CN_MAX_STRING_BUFF);
    g_mal_request_data_p = &mal_dial;


    /**********************************************************************
     * Dial & response
     **********************************************************************/

    /* Send CN_REQUEST_DIAL */
    result = cn_request_dial(g_context_p, &cn_dial, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response */
    call_event_callback(MAL_CALL_MODEM_CREATE_RESP, &cc, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DIAL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_DIAL == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

    TC_ASSERT(0 == strcmp(dialed_p, mal_dial.phone_number));
    TC_ASSERT(cn_dial.clir == mal_dial.clir);


    /**********************************************************************
     * Call state change: CALL_DIALING (new call)
     **********************************************************************/

    /* Generate fake MAL call state change event */
    call_event_callback(MAL_CALL_STATE_CHANGED, &cc, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &evt_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!evt_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    free(evt_p);
    evt_p = NULL;


    /**********************************************************************
     * Call state change: CALL_ACTIVE
     * At this point we expect CNS to send the DTMF sequence.
     **********************************************************************/
    cc.callState = CALL_ACTIVE;
    cc.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_client_tag_p = (void *) 0xDEADBEEF; // Make sure we can detect when DTMF has been sent

    /* Generate fake MAL call state change event */
    call_event_callback(MAL_CALL_STATE_CHANGED, &cc, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &evt_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!evt_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    free(evt_p);
    evt_p = NULL;

    /* Make sure DTMF has been sent */
    TC_ASSERT(g_mal_client_tag_p != (void *) 0xDEADBEEF);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_DTMF_SEND_RESP, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    /**********************************************************************
     * Call state change: CALL_IDLE
     **********************************************************************/

    cc.callState                        = CALL_IDLE;
    cc.call_cause.cause_type_sender     = CN_CALL_CAUSE_TYPE_SENDER_CLIENT;
    cc.call_cause.cause                 = 3;
    cc.call_cause.detailed_cause_length = strlen(__func__);
    cc.call_cause.detailed_cause        = (uint8_t *) __func__;

    /* Generate fake MAL call state change event */
    call_event_callback(MAL_CALL_STATE_CHANGED, &cc, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &evt_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!evt_p) {
        CN_LOG_E("failure, line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    free(evt_p);
    evt_p = NULL;


    /**********************************************************************
     * Evaluate DTMF result
     **********************************************************************/
    TC_ASSERT(g_mal_call_request_dtmf_send__dtmf_info.string_type == UTF8_STRING);
    TC_ASSERT(g_mal_call_request_dtmf_send__dtmf_info.length == dtmf_len);
    TC_ASSERT(0 == memcmp(g_mal_call_request_dtmf_send__dtmf_info.dtmf_string, dtmf_p, dtmf_len));

exit:
    free(msg_p);
    free(evt_p);
    free(mal_dial.phone_number);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

/* failing dial response, MAL error */
tc_result_t cn_request_dial_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_call_dailup mal_dial;
    char *number_p = "0123456789";
    cn_dial_t cn_dial;
    cn_message_t *msg_p = NULL;

    memset(&mal_dial, 0, sizeof(mal_dial));
    memset(&cn_dial, 0, sizeof(cn_dial));
    (void) strncpy(cn_dial.phone_number, number_p, CN_MAX_STRING_BUFF);
    cn_dial.clir = 1;
    cn_dial.call_type = CN_CALL_TYPE_VOICE_CALL;

    /* setup stub variables */
    g_mal_string_length = strlen(number_p) + 1;
    mal_dial.phone_number = (char *) calloc(1, g_mal_string_length);
    g_mal_request_data_p = &mal_dial;

    /* Send CN_REQUEST_DIAL */
    result = cn_request_dial(g_context_p, &cn_dial, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("%s failed!", __func__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check MAL parameters in the stub */
    TC_ASSERT(0 == strcmp(cn_dial.phone_number, mal_dial.phone_number));
    TC_ASSERT(cn_dial.clir == mal_dial.clir);
    TC_ASSERT(MAL_CALL_TYPE_VOICE == mal_dial.call_type);

    /* Generate fake MAL response */
    call_event_callback(MAL_CALL_MODEM_CREATE_RESP, NULL, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DIAL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("%s failed!", __func__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_DIAL == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_dial.phone_number);
    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

/* asynchronous mal_call_request_dialup fails */
tc_result_t cn_request_dial_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_call_dailup mal_dial;
    char *number_p = "0123456789";
    cn_dial_t cn_dial;
    cn_message_t *msg_p = NULL;

    memset(&mal_dial, 0, sizeof(mal_dial));
    memset(&cn_dial, 0, sizeof(cn_dial));
    (void) strncpy(cn_dial.phone_number, number_p, CN_MAX_STRING_BUFF);
    cn_dial.clir = 1;
    cn_dial.call_type = CN_CALL_TYPE_VOICE_CALL;

    /* setup stub variables */
    g_mal_string_length = strlen(number_p) + 1;
    mal_dial.phone_number = (char *) calloc(1, g_mal_string_length);
    g_mal_request_data_p = &mal_dial;
    g_mal_return_value = MAL_NET_GENERAL_ERROR;

    /* Send CN_REQUEST_DIAL */
    result = cn_request_dial(g_context_p, &cn_dial, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dial_positive_1 failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check MAL parameters in the stub */
    TC_ASSERT(0 == strcmp(cn_dial.phone_number, mal_dial.phone_number));
    TC_ASSERT(cn_dial.clir == mal_dial.clir);
    TC_ASSERT(MAL_CALL_TYPE_VOICE == mal_dial.call_type);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DIAL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dial_negative failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_DIAL == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT(99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_dial.phone_number);
    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_hangup_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_HANGUP */
    result = cn_request_hangup(g_context_p, CN_CALL_STATE_FILTER_NONE, 2 /* call-id */, 88);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_hangup failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */
    TC_ASSERT(2 == g_mal_call_request_hangup__call_id);

    /* Generate fake MAL response */
    call_event_callback(MAL_CALL_MODEM_RELEASE_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_HANGUP */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_HANGUP == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(88 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_hangup_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_HANGUP */
    result = cn_request_hangup(g_context_p, CN_CALL_STATE_FILTER_NONE, 3 /* call-id */, 88);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_hangup failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */
    TC_ASSERT(3 == g_mal_call_request_hangup__call_id);

    /* Generate fake MAL response */
    call_event_callback(MAL_CALL_MODEM_RELEASE_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_HANGUP */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_HANGUP == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(88 == msg_p->client_tag);
    TC_ASSERT(0 == msg_p->payload_size);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_hangup_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    /* CAUSE_TYPE_SENDER = NETWORK, NETWORK_CAUSE = TEMPORARY_FAILURE */
    cn_uint32_t exit_cause_array[2] = {3, 0x29};

    g_mal_client_tag_p = NULL;

    /* Send CN_REQUEST_HANGUP */
    result = cn_request_hangup(g_context_p, CN_CALL_STATE_FILTER_NONE, 4 /* call-id */, 83);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_hangup failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */
    TC_ASSERT(4 == g_mal_call_request_hangup__call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_RELEASE_RESP, (void *)exit_cause_array, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_HANGUP */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_HANGUP == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(83 == msg_p->client_tag);
    TC_ASSERT(44 == msg_p->payload_size);
    TC_ASSERT(3 == *(cn_uint32_t *) msg_p->payload);
    TC_ASSERT(0x29 == *(cn_uint32_t *)(msg_p->payload + 4));
exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_hangup_negative_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    /* CAUSE_TYPE_SENDER = NETWORK, NETWORK_CAUSE = TEMPORARY_FAILURE */
    cn_uint32_t exit_cause_array[2] = {3, 0x29};

    g_mal_client_tag_p = NULL;

    /* Send CN_REQUEST_HANGUP */
    result = cn_request_hangup(g_context_p, CN_CALL_STATE_FILTER_INCLUSIVE, 5 /* call-id */, 83);

    if (CN_SUCCESS != result) {
        //("cn_request_hangup failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_RELEASE_RESP, (void *)exit_cause_array, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_HANGUP */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        //("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_HANGUP == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(83 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_swap_calls_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_SWAP_CALLS */
    result = cn_request_swap_calls(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_swap_calls failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SWAP_CALLS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SWAP_CALLS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_swap_calls_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_SWAP_CALLS */
    result = cn_request_swap_calls(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_swap_calls failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SWAP_CALLS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SWAP_CALLS == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_hold_call_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_HOLD_CALL */
    result = cn_request_hold_call(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_hold_call failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_HOLD_CALL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_HOLD_CALL == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_hold_call_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_HOLD_CALL */
    result = cn_request_hold_call(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_hold_call failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_HOLD_CALL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_HOLD_CALL == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_resume_call_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_RESUME_CALL */
    result = cn_request_resume_call(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_resume_call failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RESUME_CALL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RESUME_CALL == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_resume_call_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_RESUME_CALL */
    result = cn_request_resume_call(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_resume_call failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RESUME_CALL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RESUME_CALL == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_conference_call_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_CONFERENCE_CALL */
    result = cn_request_conference_call(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_conference_call failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CONFERENCE_CALL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CONFERENCE_CALL == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_conference_call_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_CONFERENCE_CALL */
    result = cn_request_conference_call(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_conference_call failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CONFERENCE_CALL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CONFERENCE_CALL == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_conference_call_split_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_CONFERENCE_CALL_SPLIT */
    result = cn_request_conference_call_split(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_conference_call_split failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CONFERENCE_CALL_SPLIT */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CONFERENCE_CALL_SPLIT == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_conference_call_split_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_CONFERENCE_CALL_SPLIT */
    result = cn_request_conference_call_split(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_conference_call_split failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CONFERENCE_CALL_SPLIT */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CONFERENCE_CALL_SPLIT == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_explicit_call_transfer_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_EXPLICIT_CALL_TRANSFER */
    result = cn_request_explicit_call_transfer(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_explicit_call_transfer failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_EXPLICIT_CALL_TRANSFER */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_EXPLICIT_CALL_TRANSFER == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_explicit_call_transfer_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_EXPLICIT_CALL_TRANSFER */
    result = cn_request_explicit_call_transfer(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_explicit_call_transfer failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_EXPLICIT_CALL_TRANSFER */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_EXPLICIT_CALL_TRANSFER == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_answer_call_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_ANSWER_CALL */
    result = cn_request_answer_call(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_answer_call failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_ANSWER_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_ANSWER_CALL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_ANSWER_CALL == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_answer_call_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    int32_t mal_call_id = 1;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Setup MAL parameter to check in the stub */
    g_mal_request_data_p = &mal_call_id;

    /* Send CN_REQUEST_ANSWER_CALL */
    result = cn_request_answer_call(g_context_p, (cn_uint8_t) 1, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_answer_call failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameter to check in the stub */
    TC_ASSERT((int32_t) 1 == mal_call_id);

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_CONTROL_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_ANSWER_CALL */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_ANSWER_CALL == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_ussd_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_ss_ussd_data *actual_mal_ussd_p = NULL;
    mal_ss_ussd_data expected_mal_ussd;
    cn_ussd_info_t ussd_info;
    cn_response_ussd_t *ussd_rsp = NULL;
    char ussd_str[] = {'U', 'S', 'S', 'D'};

    expected_mal_ussd.length = 4;
    expected_mal_ussd.dcs = 42;
    expected_mal_ussd.ussd_str = (uint8_t *) "USSD";

    memset(&ussd_info, 0, sizeof(ussd_info));
    ussd_info.type = 0xFF; /* Not used value in cn_request_ussd call */
    ussd_info.received_type = 0xFF; /* Not used value in cn_request_ussd call */
    ussd_info.length = 4;
    ussd_info.dcs = 42;
    memcpy(ussd_info.ussd_string, ussd_str, sizeof(ussd_str));

    actual_mal_ussd_p = (mal_ss_ussd_data *) calloc(1, sizeof(*actual_mal_ussd_p));

    if (!actual_mal_ussd_p) {
        CN_LOG_E("cn_request_ussd_positive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    memset(actual_mal_ussd_p, 0, sizeof(*actual_mal_ussd_p));
    g_mal_request_data_p = actual_mal_ussd_p;

    /* Send CN_REQUEST_USSD */
    result = cn_request_ussd(g_context_p, &ussd_info, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_ussd_positive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_GSM_USSD_SEND_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_USSD */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_USSD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
    ussd_rsp = (cn_response_ussd_t *) msg_p->payload;

    TC_ASSERT(ussd_rsp->ss_error.cn_ss_error_code_type == CN_SS_ERROR_CODE_TYPE_NONE);

    TC_ASSERT(expected_mal_ussd.length == actual_mal_ussd_p->length);
    TC_ASSERT(expected_mal_ussd.dcs == actual_mal_ussd_p->dcs);
    TC_ASSERT(memcmp(expected_mal_ussd.ussd_str, g_ussd_string, expected_mal_ussd.length) == 0);

exit:
    free(msg_p);
    free(actual_mal_ussd_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */
    g_mal_string_length = 0;

    return tc_result;
}


tc_result_t cn_request_ussd_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    char *ussd_p = "ussd";
    mal_ss_ussd_data *mal_ussd_p = (mal_ss_ussd_data *) calloc(1, sizeof(mal_ss_ussd_data));

    if (NULL == mal_ussd_p) {
        CN_LOG_E("allocation of mal_ussd_p failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    mal_ussd_p->ussd_str = (uint8_t *) calloc(1, strlen(ussd_p) + 1);

    if (NULL == mal_ussd_p->ussd_str) {
        CN_LOG_E("allocation of mal_ussd_p->ussd_str failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    g_mal_request_data_p = mal_ussd_p;

    /* Send CN_REQUEST_USSD */
    result = cn_request_ussd(g_context_p, (cn_ussd_info_t *)ussd_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_ussd_positive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_GSM_USSD_SEND_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_USSD */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_USSD == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_ussd_p->ussd_str);
    free(mal_ussd_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */
    g_mal_string_length = 0;

    return tc_result;
}

tc_result_t cn_request_ussd_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_ss_ussd_data *actual_mal_ussd_p = NULL;
    mal_ss_ussd_data expected_mal_ussd;
    cn_ussd_info_t ussd_info;
    mal_ss_error_type_value mal_ss_error;
    cn_response_ussd_t *ussd_rsp = NULL;
    mal_ss_service_response ss_service_response;

    char ussd_str[] = {'U', 'S', 'S', 'D'};

    ss_service_response.error_info = &mal_ss_error;

    expected_mal_ussd.length = 4;
    expected_mal_ussd.dcs = 42;
    expected_mal_ussd.ussd_str = (uint8_t *) "USSD";

    memset(&ussd_info, 0, sizeof(ussd_info));
    ussd_info.type = 0xFF; /* Not used value in cn_request_ussd call */
    ussd_info.received_type = 0xFF; /* Not used value in cn_request_ussd call */
    ussd_info.length = 4;
    ussd_info.dcs = 42;
    memcpy(ussd_info.ussd_string, ussd_str, sizeof(ussd_str));

    actual_mal_ussd_p = (mal_ss_ussd_data *) calloc(1, sizeof(*actual_mal_ussd_p));

    if (!actual_mal_ussd_p) {
        CN_LOG_E("cn_request_ussd_positive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    memset(actual_mal_ussd_p, 0, sizeof(*actual_mal_ussd_p));
    g_mal_request_data_p = actual_mal_ussd_p;

    /* Use g_ussd_string global for the actual string */
    memset(g_ussd_string, 0, sizeof(g_ussd_string));

    /* Send CN_REQUEST_USSD */

    result = cn_request_ussd(g_context_p, &ussd_info, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_ussd_positive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response */
    mal_ss_error.error_type = SS_ERROR_TYPE_MISC;
    mal_ss_error.error_value = MAL_SS_GSM_DATA_ERROR; /* Fake data error */

    call_event_callback(MAL_SS_SERVICE_FAILED_RESP, (void *) &ss_service_response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_USSD */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_USSD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

    ussd_rsp = (cn_response_ussd_t *) msg_p->payload;

    CN_LOG_E("cn_message_receive error_code_type: %d !", ussd_rsp->ss_error.cn_ss_error_code_type);
    TC_ASSERT(ussd_rsp->ss_error.cn_ss_error_code_type == CN_SS_ERROR_CODE_TYPE_CN);
    TC_ASSERT(ussd_rsp->ss_error.cn_ss_error_value.cn_ss_error_code == CN_SS_ERROR_CODE_GSM_DATA_ERROR);

exit:
    free(msg_p);
    free(actual_mal_ussd_p);
    g_mal_request_data_p = NULL; /* Reset MAL result */
    g_mal_string_length = 0;

    return tc_result;
}


tc_result_t cn_request_ussd_abort_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_USSD_ABORT */
    result = cn_request_ussd_abort(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_ussd_abort failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_GSM_USSD_SEND_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_USSD_ABORT */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_USSD_ABORT == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_ussd_abort_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_USSD_ABORT */
    result = cn_request_ussd_abort(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_ussd_abort failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_GSM_USSD_SEND_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_USSD_ABORT */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_USSD_ABORT == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_rssi_value_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    char mal_rssi = 99;
    cn_rssi_t *cn_rssi_p = NULL;

    /* Send CN_REQUEST_RSSI_VALUE */
    result = cn_request_rssi_value(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rssi_value failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_NET_RSSI_GET_RESP, &mal_rssi, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RSSI_VALUE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RSSI_VALUE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
    cn_rssi_p = (cn_rssi_t *) msg_p->payload;
    TC_ASSERT(99 == *cn_rssi_p);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_rssi_value_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    char mal_rssi = 99;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RSSI_VALUE */
    result = cn_request_rssi_value(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_ussd_abort failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_NET_RSSI_GET_RESP, &mal_rssi, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RSSI_VALUE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RSSI_VALUE == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_clip_status_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_clip_status_t *cn_clip_status_p = NULL;
    mal_ss_service_response response;

    /* Send CN_REQUEST_CLIP_STATUS */
    result = cn_request_clip_status(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rssi_value failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    response.response[0] = 1;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CLIP_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CLIP_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

    cn_clip_status_p = (cn_clip_status_t *) msg_p->payload;

    TC_ASSERT(CN_CLIP_STATUS_PROVISIONED == cn_clip_status_p[0]);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_clip_status_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_ss_service_response response;
    /* Send CN_REQUEST_CLIP_STATUS */
    result = cn_request_clip_status(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_ussd_abort failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    memset(&response, 0, sizeof(mal_ss_service_response));
    response.response[0] = 1;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CLIP_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CLIP_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_clir_status_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    int32_t mal_data = 2;
    mal_ss_service_response mal_ss_data;
    cn_message_t *msg_p = NULL;
    cn_clir_t *cn_clir_p = NULL;

    /* Send CN_REQUEST_CLIR_STATUS */
    result =  cn_request_clir_status(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rssi_value failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &mal_data, MAL_NET_SUCCESS, g_mal_client_tag_p);

    /* Generate fake MAL response with an exit cause */
    mal_ss_data.response[0] = CLIR_PROVISIONED_PERMANENT;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &mal_ss_data, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CLIR_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CLIR_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

    cn_clir_p = (cn_clir_t *) msg_p->payload;

    TC_ASSERT(CN_CLIR_SETTING_SUPPRESSION == cn_clir_p->setting);
    TC_ASSERT(CN_CLIP_STATUS_PROVISIONED == cn_clir_p->status);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_clir_status_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    int32_t mal_data[2] = { 2 /* SUPPRESSION */, CLIR_PROVISIONED_PERMANENT };
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_CLIR_STATUS */
    result =  cn_request_clir_status(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rssi_value failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &mal_data, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CLIR_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CLIR_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_set_clir_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint8_t *mal_clir_status_p = (uint8_t *) calloc(1, 1);

    if (NULL == mal_clir_status_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    g_mal_request_data_p = mal_clir_status_p;

    /* Send CN_REQUEST_SET_CLIR */
    result = cn_request_set_clir(g_context_p, CN_CLIR_SETTING_SUPPRESSION, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_clir failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CLIR */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_CLIR == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
    TC_ASSERT(*mal_clir_status_p == (uint8_t) 2 /* SUPPRESSION */);

exit:
    free(msg_p);
    free(mal_clir_status_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_set_clir_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint8_t *mal_clir_status_p = (uint8_t *) calloc(1, 1);

    if (NULL == mal_clir_status_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    g_mal_request_data_p = mal_clir_status_p;

    /* Send CN_REQUEST_SET_CLIR */
    result = cn_request_set_clir(g_context_p, CN_CLIR_SETTING_SUPPRESSION, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_clir failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CLIR */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_CLIR == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_clir_status_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_set_signal_info_config_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_signal_info_config_t signal_info_config;

    struct mal_request_data_t {
        mal_net_rssi_conf_data net_set;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_net_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_net_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    signal_info_config.num_of_segments = 1;
    signal_info_config.signal_level_type = 2;
    signal_info_config.hysteresis_down = 0;
    signal_info_config.hysteresis_up = 0;
    signal_info_config.border[2] = 0;
    signal_info_config.ccsq_mode = 0;


    g_mal_request_data_p = mal_request_net_p;

    result = cn_request_set_signal_info_config(g_context_p, &signal_info_config, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_signal_info_config failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    call_event_callback(MAL_NET_RSSI_IND, NULL, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_SIGNAL_INFO_CONFIG == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);
    free(mal_request_net_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;


}




tc_result_t cn_request_get_signal_info_config_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_signal_info_config_t signal_info_config;

    struct mal_request_data_t {
        mal_net_rssi_conf_data net_set;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_net_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_net_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    signal_info_config.num_of_segments = 1;
    signal_info_config.signal_level_type = 2;
    signal_info_config.hysteresis_down = 0;
    signal_info_config.hysteresis_up = 0;
    signal_info_config.border[2] = 0;
    signal_info_config.ccsq_mode = 0;


    g_mal_request_data_p = mal_request_net_p;

    result = cn_request_get_signal_info_config(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_signal_info_config failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    call_event_callback(MAL_NET_RSSI_IND, NULL, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_SIGNAL_INFO_CONFIG == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);
    free(mal_request_net_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;


}



tc_result_t cn_request_set_signal_info_reporting_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_rssi_mode_t signal_info_t;

    struct mal_request_data_t {
        mal_net_signal_info net_info;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_net_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_net_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    signal_info_t = CN_RSSI_EVENT_REPORTING_DISABLE;


    g_mal_request_data_p = mal_request_net_p;

    result = cn_request_set_signal_info_reporting(g_context_p, signal_info_t, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_signal_info_config failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    call_event_callback(MAL_NET_RSSI_IND, NULL, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_SIGNAL_INFO_REPORTING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);
    free(mal_request_net_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;


}



tc_result_t cn_request_get_signal_info_reporting_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_rssi_mode_t signal_info_t;

    struct mal_request_data_t {
        mal_net_signal_info net_info;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_net_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_net_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    signal_info_t = CN_RSSI_EVENT_REPORTING_DISABLE;


    g_mal_request_data_p = mal_request_net_p;

    result = cn_request_get_signal_info_reporting(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_signal_info_config failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    call_event_callback(MAL_NET_RSSI_IND, NULL, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_SIGNAL_INFO_REPORTING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);
    free(mal_request_net_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;


}

tc_result_t cn_request_set_hsxpa_mode_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_hsxpa_mode_t hsxpa_info_t;

    struct mal_request_data_t {
        mal_gss_hsxpa_mode hsxpa_info;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_hsxpa_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_hsxpa_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    hsxpa_info_t = CN_HSXPA_DISABLE;


    g_mal_request_data_p = mal_request_hsxpa_p;

    result = cn_request_set_hsxpa_mode(g_context_p, hsxpa_info_t, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_hsxpa_mode failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    call_event_callback(MAL_GSS_HSXPA_USER_SETTING_WRITE_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_HSXPA_MODE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);
    free(mal_request_hsxpa_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;


}


tc_result_t cn_request_get_hsxpa_mode_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    /*
    cn_hsxpa_mode_t hsxpa_info_t;

    struct mal_request_data_t{
           mal_gss_hsxpa_mode hsxpa_info;
           uint8_t n;
       };
    struct mal_request_data_t *mal_request_hsxpa_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_hsxpa_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    hsxpa_info_t=CN_HSXPA_DISABLE;
    */


    //g_mal_request_data_p = mal_request_hsxpa_p;

    result = cn_request_get_hsxpa_mode(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_hsxpa_mode failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    call_event_callback(MAL_GSS_SET_USER_ACTIVITY_INFO_IND, NULL, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_HSXPA_MODE == msg_p->type);
    //TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);
    // free(mal_request_hsxpa_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;


}

tc_result_t cn_request_set_event_reporting_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_event_reporting_type_t  event_reporting_t;

    struct mal_request_data_t {
        mal_net_registration_data net_reg_data;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_net_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_net_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    event_reporting_t = CN_EVENT_REPORTING_TYPE_RAT_NAME;

    g_mal_request_data_p = mal_request_net_p;

    result = cn_request_set_event_reporting(g_context_p, event_reporting_t, 0, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_event_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    call_event_callback(MAL_NET_SELECT_TIME_IND, NULL, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    //TC_ASSERT(CN_EVENT_REPORTING_TYPE_RAT_NAME == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);
    free(mal_request_net_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;

}


tc_result_t cn_request_get_event_reporting_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_event_reporting_type_t  event_reporting_t;

    struct mal_request_data_t {
        mal_net_registration_data net_reg_data;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_net_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_net_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    event_reporting_t = CN_EVENT_REPORTING_TYPE_RAT_NAME;

    g_mal_request_data_p = mal_request_net_p;

    result = cn_request_get_event_reporting(g_context_p, event_reporting_t, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_event_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    call_event_callback(MAL_NET_SELECT_TIME_IND, NULL, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    //TC_ASSERT(CN_REQUEST_GET_EVENT_REPORTING == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);
    free(mal_request_net_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;

}


tc_result_t cn_request_rab_status_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    struct mal_request_data_t {
        mal_net_registration_data net_reg_data;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_net_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_net_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }


    g_mal_request_data_p = mal_request_net_p;

    result = cn_request_rab_status(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_event_reporting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    call_event_callback(MAL_NET_RADIO_INFO_IND, NULL, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    //TC_ASSERT(CN_REQUEST_GET_RAB_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);
    free(mal_request_net_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;


}

tc_result_t cn_request_get_neighbour_cells_reporting_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;
    cn_neighbour_cells_info_t *cn_neighbour_cells_info_p = NULL;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_GET_NEIGHBOUR_CELLS_REPORTING */
    result = cn_request_get_neighbour_cells_reporting(g_context_p, CN_NEIGHBOUR_RAT_TYPE_2G, (cn_client_tag_t) 987);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_neighbour_cells_complete_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    mal_neighbour_cells_info.rat_type = MAL_NET_NEIGHBOUR_RAT_2G;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid = 0x1A0F;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance = 39;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn = 106;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic = 31;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev = -71;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.neigh_info_type = 0;

    //CN_NET_NEIGHBOUR_CELLS_EXT
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours = 3;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].arfcn = 105;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].bsic = 17;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].rxlev = -84;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].arfcn = 96;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].bsic = 15;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].rxlev = -79;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].arfcn = 62;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].bsic = 22;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].rxlev = -93;

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, &mal_neighbour_cells_info, MAL_NET_SUCCESS, g_mal_client_tag_p);


    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(987 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_neighbour_cells_info_p = (cn_neighbour_cells_info_t *)msg_p->payload;

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_set_neighbour_cells_reporting_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    mal_net_neighbour_cells_info mal_neighbour_cells_info;
    cn_neighbour_cells_info_t *cn_neighbour_cells_info_p = NULL;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_SET_NEIGHBOUR_CELLS_REPORTING */
    result = cn_request_set_neighbour_cells_reporting(g_context_p, TRUE, CN_NEIGHBOUR_RAT_TYPE_2G, (cn_client_tag_t) 987);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_neighbour_cells_complete_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    mal_neighbour_cells_info.rat_type = MAL_NET_NEIGHBOUR_RAT_2G;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_mcc = 0x0194;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_mnc = 0x002D;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_lac = 0x0591;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_cid = 0x1A0F;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_timing_advance = 39;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_arfcn = 106;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_bsic = 31;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_serving_cell_info.current_rxlev = -71;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.neigh_info_type = 0;

    //CN_NET_NEIGHBOUR_CELLS_EXT
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.num_of_neighbours = 3;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].arfcn = 105;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].bsic = 17;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[0].rxlev = -84;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].arfcn = 96;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].bsic = 15;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].rxlev = -79;

    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].arfcn = 62;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].bsic = 22;
    mal_neighbour_cells_info.cells_info.gsm_cells_info.gsm_neigh_info_extd.gsm_nmr_info_extd[1].rxlev = -93;

    /* Generate fake MAL response */
    net_event_callback(MAL_NET_UNSOL_NEIGHBOUR_CELLS_INFO, &mal_neighbour_cells_info, MAL_NET_SUCCESS, g_mal_client_tag_p);


    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(987 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_neighbour_cells_info_p = (cn_neighbour_cells_info_t *)msg_p->payload;

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_set_modem_property_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_modem_property_t cn_modem_property;
    struct mal_request_data_t {
        mal_call_modem_property property;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_data_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_modem_property.type = CN_MODEM_PROPERTY_TYPE_CLIR;
    cn_modem_property.value.clir.clir_setting = CN_CLIR_SETTING_SUPPRESSION;
    cn_modem_property.value.clir.service_status = 99;

    g_mal_request_data_p = mal_request_data_p;

    /* Send CN_REQUEST_SET_MODEM_PROPERTY */
    result = cn_request_set_modem_property(g_context_p, cn_modem_property, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_modem_property failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_PROPERTY_SET_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_MODEM_PROPERTY == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_set_modem_property_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_modem_property_t cn_modem_property;
    struct mal_request_data_t {
        mal_call_modem_property property;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_data_p) {
        //("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_modem_property.type = CN_MODEM_PROPERTY_TYPE_CTM;
    cn_modem_property.value.ctm = CN_MODEM_PROP_VALUE_CTM_SUPPORTED;

    g_mal_request_data_p = mal_request_data_p;

    /* Send CN_REQUEST_SET_MODEM_PROPERTY */
    result = cn_request_set_modem_property(g_context_p, cn_modem_property, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        //("cn_request_set_modem_property failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_PROPERTY_SET_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        //("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_MODEM_PROPERTY == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_set_modem_property_positive_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_modem_property_t cn_modem_property;
    struct mal_request_data_t {
        mal_call_modem_property property;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_data_p) {
        //("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_modem_property.type = CN_MODEM_PROPERTY_TYPE_ALS;
    cn_modem_property.value.als.current_line = CN_ALS_LINE_NORMAL;
    cn_modem_property.value.als.supported_lines = CN_SUPPORTED_ALS_LINE1;

    g_mal_request_data_p = mal_request_data_p;

    /* Send CN_REQUEST_SET_MODEM_PROPERTY */
    result = cn_request_set_modem_property(g_context_p, cn_modem_property, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        //("cn_request_set_modem_property failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_PROPERTY_SET_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        //("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_MODEM_PROPERTY == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_set_modem_property_positive_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_modem_property_t cn_modem_property;
    struct mal_request_data_t {
        mal_call_modem_property property;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_data_p) {
        //("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_modem_property.type = CN_MODEM_PROPERTY_TYPE_UNKNOWN;

    g_mal_request_data_p = mal_request_data_p;

    /* Send CN_REQUEST_SET_MODEM_PROPERTY */
    result = cn_request_set_modem_property(g_context_p, cn_modem_property, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        //("cn_request_set_modem_property failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_PROPERTY_SET_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        //("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_MODEM_PROPERTY == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_set_modem_property_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_call_modem_property *mal_modem_property_p = (mal_call_modem_property *) calloc(1, sizeof(mal_call_modem_property));
    cn_modem_property_t cn_modem_property;

    if (NULL == mal_modem_property_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_modem_property.type = CN_MODEM_PROPERTY_TYPE_CLIR;
    cn_modem_property.value.clir.clir_setting = CN_CLIR_SETTING_SUPPRESSION;
    cn_modem_property.value.clir.service_status = 99;

    g_mal_request_data_p = mal_modem_property_p;

    /* Send CN_REQUEST_SET_MODEM_PROPERTY */
    result = cn_request_set_modem_property(g_context_p, cn_modem_property, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_modem_property failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_PROPERTY_SET_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_MODEM_PROPERTY == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_modem_property_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_get_modem_property_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    //cn_modem_property_t cn_modem_property;
    struct mal_request_data_t {
        mal_call_modem_property property;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_data_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Send CN_REQUEST_GET_MODEM_PROPERTY */
    printf("**********enter cn_request_get_modem_property function\n");

    result = cn_request_get_modem_property(g_context_p, CN_MODEM_PROPERTY_TYPE_CTM, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_modem_property failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    printf("**********enter WAIT_FOR_SELECT_TO_FINISH function\n");

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    printf("**********enter call_event_callback function\n");
    call_event_callback(MAL_CALL_MODEM_PROPERTY_GET_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);
    printf("**********enter cn_message_receive function\n");

    /* Receive CN_RESPONSE_GET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_MODEM_PROPERTY == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_get_modem_property_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    //cn_modem_property_t cn_modem_property;
    struct mal_request_data_t {
        mal_call_modem_property property;
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_data_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Send CN_REQUEST_GET_MODEM_PROPERTY */
    printf("**********enter cn_request_get_modem_property function\n");

    result = cn_request_get_modem_property(g_context_p, CN_MODEM_PROPERTY_TYPE_UNKNOWN, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_modem_property failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    printf("**********enter WAIT_FOR_SELECT_TO_FINISH function\n");

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    printf("**********enter call_event_callback function\n");
    call_event_callback(MAL_CALL_MODEM_PROPERTY_GET_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);
    printf("**********enter cn_message_receive function\n");

    /* Receive CN_RESPONSE_GET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_MODEM_PROPERTY == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_dtmf_send_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char *dtmf_string_p = "2,d,A";
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint16_t string_length = strlen(dtmf_string_p);
    cn_uint32_t size = 0;
    const cn_uint16_t dtmf_duration_time = 123;
    const cn_uint16_t dtmf_pause_time = 456;
    mal_call_context mal_call_list;

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ACTIVE;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    /* Send test string with ASCII type */
    result = cn_request_dtmf_send(g_context_p, dtmf_string_p,
                                  string_length,
                                  CN_DTMF_STRING_TYPE_ASCII,
                                  dtmf_duration_time,
                                  dtmf_pause_time,
                                  (cn_client_tag_t) 98);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL,
                        MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_SEND */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Verify status and message */
    TC_ASSERT(CN_RESPONSE_DTMF_SEND == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 98 == msg_p->client_tag);
    TC_ASSERT(0 == size);

    /* Verify DTMF data */
    TC_ASSERT(string_length == g_mal_call_request_dtmf_send__dtmf_info.length);
    TC_ASSERT(UTF8_STRING == g_mal_call_request_dtmf_send__dtmf_info.string_type);
    TC_ASSERT(0 == memcmp(dtmf_string_p, g_mal_call_request_dtmf_send__dtmf_info.dtmf_string, string_length));
    TC_ASSERT(dtmf_duration_time == g_mal_call_request_dtmf_send__dtmf_info.dtmf_duration_time);
    TC_ASSERT(dtmf_pause_time == g_mal_call_request_dtmf_send__dtmf_info.dtmf_pause_time);

exit:
    free(msg_p);

    return tc_result;
}

tc_result_t cn_request_dtmf_send_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint16_t string_length;
    cn_uint32_t size = 0;
    const cn_uint16_t dtmf_duration_time = 123;
    const cn_uint16_t dtmf_pause_time = 456;
    mal_call_context mal_call_list;

    char *dtmf_string_p = malloc(CN_MAX_STRING_SIZE + 1);

    if (!dtmf_string_p) {
        CN_LOG_E("Failure on line %d", __LINE__);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    memset(dtmf_string_p, 'a', CN_MAX_STRING_SIZE);
    dtmf_string_p[CN_MAX_STRING_SIZE] = '\0';
    string_length = strlen(dtmf_string_p);

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ACTIVE;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_MULTIMEDIA;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    /* Send test string with ASCII type */
    result = cn_request_dtmf_send(g_context_p, dtmf_string_p,
                                  string_length,
                                  CN_DTMF_STRING_TYPE_ASCII,
                                  dtmf_duration_time,
                                  dtmf_pause_time,
                                  (cn_client_tag_t) 98);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL,
                        MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_SEND */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Verify status and message */
    TC_ASSERT(CN_RESPONSE_DTMF_SEND == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 98 == msg_p->client_tag);
    TC_ASSERT(0 == size);

    /* Verify DTMF data */
    CN_LOG_E("string_length=%d, dtmf_info.length=%d", string_length, g_mal_call_request_dtmf_send__dtmf_info.length);
    TC_ASSERT(string_length == g_mal_call_request_dtmf_send__dtmf_info.length);
    TC_ASSERT(UTF8_STRING == g_mal_call_request_dtmf_send__dtmf_info.string_type);
    TC_ASSERT(0 == memcmp(dtmf_string_p, g_mal_call_request_dtmf_send__dtmf_info.dtmf_string, string_length));
    TC_ASSERT(dtmf_duration_time == g_mal_call_request_dtmf_send__dtmf_info.dtmf_duration_time);
    TC_ASSERT(dtmf_pause_time == g_mal_call_request_dtmf_send__dtmf_info.dtmf_pause_time);

exit:
    free(msg_p);
    free(dtmf_string_p);

    return tc_result;
}

/*
 * Note that MAL, when sending DTMF, the indicated length is in digits, not bytes.
 * e.g. The DTMF string "0123456789" is 10 digits, but the test case sets the
 * length to the length of the buffer, which in this case would be 5 bytes.
 */
tc_result_t cn_request_dtmf_send_positive_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char dtmf_string[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0x00};
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint16_t string_length = sizeof(dtmf_string) - 1;
    cn_uint32_t size = 0;
    const cn_uint16_t dtmf_duration_time = 123;
    const cn_uint16_t dtmf_pause_time = 456;
    mal_call_context mal_call_list;

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ALERTING;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    /* Send test string with ASCII type */
    result = cn_request_dtmf_send(g_context_p, dtmf_string,
                                  string_length * 2,
                                  CN_DTMF_STRING_TYPE_BCD,
                                  dtmf_duration_time,
                                  dtmf_pause_time,
                                  (cn_client_tag_t) 98);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL,
                        MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_SEND */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Verify status and message */
    TC_ASSERT(CN_RESPONSE_DTMF_SEND == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 98 == msg_p->client_tag);
    TC_ASSERT(0 == size);

    /* Verify DTMF data */
    TC_ASSERT(string_length * 2 == g_mal_call_request_dtmf_send__dtmf_info.length);
    TC_ASSERT(BCD_STRING == g_mal_call_request_dtmf_send__dtmf_info.string_type);
    TC_ASSERT(0 == memcmp(dtmf_string, g_mal_call_request_dtmf_send__dtmf_info.dtmf_string, string_length));
    TC_ASSERT(dtmf_duration_time == g_mal_call_request_dtmf_send__dtmf_info.dtmf_duration_time);
    TC_ASSERT(dtmf_pause_time == g_mal_call_request_dtmf_send__dtmf_info.dtmf_pause_time);

exit:
    free(msg_p);

    return tc_result;
}

/*
 * note that MAL, when sending DTMF, the indicated length is in digits, not bytes.
 * e.g. The DTMF string "0123456789" is 10 digits, but the test case sets the
 * length to the length of the buffer.
 */
tc_result_t cn_request_dtmf_send_positive_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t size = 0;
    const cn_uint16_t dtmf_duration_time = 123;
    const cn_uint16_t dtmf_pause_time = 456;
    mal_call_context mal_call_list;

    size_t string_length = CN_MAX_STRING_SIZE / 2 - 1;
    char *dtmf_string_p = malloc(string_length + 1);

    if (NULL == dtmf_string_p) {
        CN_LOG_E("Failed to allocate memory for test string!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    memset(dtmf_string_p, 0x55, string_length);
    dtmf_string_p[string_length] = '\0';

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ACTIVE;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    /* Send test string with ASCII type */
    result = cn_request_dtmf_send(g_context_p, dtmf_string_p,
                                  string_length * 2, /* Digits, not bytes */
                                  CN_DTMF_STRING_TYPE_BCD,
                                  dtmf_duration_time,
                                  dtmf_pause_time,
                                  (cn_client_tag_t) 98);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL,
                        MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_SEND */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Verify status and message */
    TC_ASSERT(CN_RESPONSE_DTMF_SEND == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 98 == msg_p->client_tag);
    TC_ASSERT(0 == size);

    /* Verify DTMF data */
    TC_ASSERT(string_length * 2 == g_mal_call_request_dtmf_send__dtmf_info.length);
    TC_ASSERT(BCD_STRING == g_mal_call_request_dtmf_send__dtmf_info.string_type);
    TC_ASSERT(0 == memcmp(dtmf_string_p, g_mal_call_request_dtmf_send__dtmf_info.dtmf_string, string_length));
    TC_ASSERT(dtmf_duration_time == g_mal_call_request_dtmf_send__dtmf_info.dtmf_duration_time);
    TC_ASSERT(dtmf_pause_time == g_mal_call_request_dtmf_send__dtmf_info.dtmf_pause_time);

exit:
    free(msg_p);
    free(dtmf_string_p);

    return tc_result;
}

tc_result_t cn_request_dtmf_send_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char *dtmf_string_p = "2,d,A";
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint16_t string_length = strlen(dtmf_string_p);
    cn_uint32_t size = 0;
    mal_call_context mal_call_list;

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ACTIVE;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    /* Send test string with ASCII type */
    result = cn_request_dtmf_send(g_context_p, dtmf_string_p,
                                  string_length,
                                  CN_DTMF_STRING_TYPE_ASCII,
                                  0,
                                  0,
                                  (cn_client_tag_t) 98);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Verify status and message */
    TC_ASSERT(CN_RESPONSE_DTMF_SEND == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 98 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}

tc_result_t cn_request_dtmf_send_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char *dtmf_string_p = "2,d,A";
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint16_t string_length = strlen(dtmf_string_p);
    cn_uint32_t size = 0;

    /* Simulate no calls which should fail the prerequisites check for the DTMF handling */
    g_mal_no_of_calls = 0;

    /* Send test string with ASCII type */
    result = cn_request_dtmf_send(g_context_p, dtmf_string_p,
                                  string_length,
                                  CN_DTMF_STRING_TYPE_ASCII,
                                  0,
                                  0,
                                  (cn_client_tag_t) 98);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_SEND */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Verify status and message */
    TC_ASSERT(CN_RESPONSE_DTMF_SEND == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 98 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_dtmf_send_negative_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char *dtmf_string_p = "2,d,A";
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint16_t string_length = strlen(dtmf_string_p);
    cn_uint32_t size = 0;
    mal_call_context mal_call_list;

    /* Simulate no _active_ calls which should fail the prerequisites check for the DTMF handling */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_IDLE;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    /* Send test string with ASCII type */
    result = cn_request_dtmf_send(g_context_p, dtmf_string_p,
                                  string_length,
                                  CN_DTMF_STRING_TYPE_ASCII,
                                  0,
                                  0,
                                  (cn_client_tag_t) 98);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_SEND */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Verify status and message */
    TC_ASSERT(CN_RESPONSE_DTMF_SEND == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 98 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_dtmf_send_negative_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char *dtmf_string_p = "2,d,A";
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint16_t string_length = strlen(dtmf_string_p);
    mal_call_context mal_call_list;

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ACTIVE;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    /* Send test string with incorrect string type */
    result = cn_request_dtmf_send(g_context_p, dtmf_string_p,
                                  string_length,
                                  0xbeef,
                                  0,
                                  0,
                                  (cn_client_tag_t) 98);

    if (CN_SUCCESS == result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_dtmf_send_negative_5()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    mal_call_context mal_call_list;

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ALERTING;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    /* Send test string with incorrect string type */
    result = cn_request_dtmf_send(g_context_p, NULL,
                                  0,
                                  CN_DTMF_STRING_TYPE_ASCII,
                                  0,
                                  0,
                                  (cn_client_tag_t) 98);

    if (CN_SUCCESS == result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

exit:
    free(msg_p);

    return tc_result;
}


/*
 * Note that MAL, when sending DTMF, the indicated length is in digits, not bytes.
 * e.g. The DTMF string "0123456789" is 10 digits, but the test case sets the
 * length to the length of the buffer, which in this case would be 5 bytes.
 */
tc_result_t cn_request_dtmf_send_negative_6()
{
    tc_result_t tc_result = TC_RESULT_OK;
    char dtmf_string[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89};
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint16_t string_length = sizeof(dtmf_string) - 1;
    cn_uint32_t size = 0;
    const cn_uint16_t dtmf_duration_time = 123;
    const cn_uint16_t dtmf_pause_time = 456;
    mal_call_context mal_call_list;

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ALERTING;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    /* Send test string with ASCII type */
    result = cn_request_dtmf_send(g_context_p, dtmf_string,
                                  string_length * 2,
                                  CN_DTMF_STRING_TYPE_BCD,
                                  dtmf_duration_time,
                                  dtmf_pause_time,
                                  (cn_client_tag_t) 98);

    if (CN_SUCCESS != result) {
        //("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_SEND */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        //("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Verify status and message */
    TC_ASSERT(CN_RESPONSE_DTMF_SEND == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 98 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_cssn_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_cssn_setting_t cn_cssn_setting;
    struct mal_request_data_t {
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_data_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    //cn_cssn_setting=CN_SUPPL_SVC_NOTIFY_ENABLE;

    g_mal_request_data_p = mal_request_data_p;

    /* Send CN_REQUEST_GET_MODEM_PROPERTY */
    result = cn_request_set_cssn(g_context_p, CN_SUPPL_SVC_NOTIFY_ENABLE, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_modem_property failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_PROPERTY_SET_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_CSSN == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    // TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_set_default_nvmd_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    // cn_modem_property_t cn_modem_property;
    struct mal_request_data_t {
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_data_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    g_mal_request_data_p = mal_request_data_p;

    /* Send CN_REQUEST_SET_MODEM_PROPERTY */
    result = cn_request_set_default_nvmd(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_modem_property failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_PROPERTY_SET_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    // TC_ASSERT(CN_REQUEST_SET_DEFAULT_NVMD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_set_signal_info_config_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_signal_info_config_t cn_signal_info_config;
    struct mal_request_data_t {
        uint8_t n;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(struct mal_request_data_t));

    if (NULL == mal_request_data_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_signal_info_config.num_of_segments = 1;
    cn_signal_info_config.signal_level_type = 1;
    cn_signal_info_config.hysteresis_up = 0;
    cn_signal_info_config.hysteresis_down = 0;
    cn_signal_info_config.ccsq_mode = 0;
    cn_signal_info_config.border[0] = 1;
    g_mal_request_data_p = mal_request_data_p;

    /* Send CN_REQUEST_SET_MODEM_PROPERTY */
    result = cn_request_set_signal_info_config(g_context_p, &cn_signal_info_config, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_modem_property failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_PROPERTY_SET_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_MODEM_PROPERTY */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    // TC_ASSERT(CN_REQUEST_SET_DEFAULT_NVMD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}
tc_result_t cn_request_dtmf_start_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t size = 0;
    mal_call_context mal_call_list;
    struct mal_request_data_t {
        char character;
        int32_t call_id;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(mal_request_data_p));

    if (NULL == mal_request_data_p) {
        CN_LOG_E("Failed to allocate memory for test string!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ACTIVE;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    g_mal_request_data_p = mal_request_data_p;

    /* Request CN_REQUEST_DTMF_START */
    result = cn_request_dtmf_start(g_context_p, 'A', (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_CALL_MODEM_DTMF_SEND_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_START */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
    TC_ASSERT('A' == mal_request_data_p->character);
    TC_ASSERT(1 == mal_request_data_p->call_id);

exit:
    free(msg_p);
    free(mal_request_data_p);

    return tc_result;
}


tc_result_t cn_request_dtmf_start_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t size = 0;
    mal_call_context mal_call_list;
    struct mal_request_data_t {
        char character;
        int32_t call_id;
    };
    struct mal_request_data_t *mal_request_data_p = (struct mal_request_data_t *) calloc(1, sizeof(mal_request_data_p));

    if (NULL == mal_request_data_p) {
        CN_LOG_E("Failed to allocate memory for test string!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ACTIVE;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    g_mal_request_data_p = mal_request_data_p;

    /* Request CN_REQUEST_DTMF_START */
    result = cn_request_dtmf_start(g_context_p, 'A', (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_START */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    return tc_result;
}


tc_result_t cn_request_dtmf_stop_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t size = 0;
    mal_call_context mal_call_list;
    int32_t *mal_request_data_p = (int32_t *) calloc(1, sizeof(int32_t));

    if (NULL == mal_request_data_p) {
        CN_LOG_E("Failed to allocate memory for test string!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ACTIVE;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    g_mal_request_data_p = mal_request_data_p;

    /* Request CN_REQUEST_DTMF_STOP */
    result = cn_request_dtmf_stop(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_START */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
    TC_ASSERT(*mal_request_data_p == mal_call_list.callId);

exit:
    free(msg_p);
    free(mal_request_data_p);

    return tc_result;
}


tc_result_t cn_request_dtmf_stop_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t size = 0;
    mal_call_context mal_call_list;
    int32_t *mal_request_data_p = (int32_t *) calloc(1, sizeof(int32_t));

    if (NULL == mal_request_data_p) {
        CN_LOG_E("Failed to allocate memory for test string!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Simulate an active call which is needed for DTMF to be sent */
    memset(&mal_call_list, 0, sizeof(mal_call_list));
    mal_call_list.callState = CALL_ACTIVE;
    mal_call_list.callId = 1;
    mal_call_list.call_mode = MAL_CALL_MODE_SPEECH;
    g_mal_no_of_calls = 1;
    g_mal_call_list_p = &mal_call_list;

    g_mal_request_data_p = mal_request_data_p;

    /* Request CN_REQUEST_DTMF_STOP */
    result = cn_request_dtmf_stop(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_dtmf_send failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_DTMF_START */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_request_data_p);

    return tc_result;
}


tc_result_t cn_request_set_call_waiting_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_call_waiting_t cn_call_waiting;
    int32_t *mal_service_class_p = (int32_t *) calloc(1, sizeof(int32_t) * 2);
    mal_ss_service_response response;

    if (NULL == mal_service_class_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_call_waiting.setting = CN_CALL_WAITING_SERVICE_ENABLED;
    cn_call_waiting.service_class = CN_SERVICE_CLASS_GSM_VOICE;

    g_mal_request_data_p = mal_service_class_p;

    /* Send CN_REQUEST_SET_CALL_WAITING */
    result = cn_request_set_call_waiting(g_context_p, &cn_call_waiting, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_call_waiting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    memset(&response, 0, sizeof(mal_ss_service_response));

    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_WAITING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_CALL_WAITING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
    TC_ASSERT(mal_service_class_p[0] == (int32_t) cn_call_waiting.setting);
    TC_ASSERT(mal_service_class_p[1] == (int32_t) cn_call_waiting.service_class);

exit:
    free(msg_p);
    free(mal_service_class_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_set_call_waiting_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_call_waiting_t cn_call_waiting;
    int32_t *mal_service_class_p = (int32_t *) calloc(1, sizeof(int32_t) * 2);

    if (NULL == mal_service_class_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_call_waiting.setting = CN_CALL_WAITING_SERVICE_ENABLED;
    cn_call_waiting.service_class = 1;

    g_mal_request_data_p = mal_service_class_p;

    /* Send CN_REQUEST_SET_CALL_WAITING */
    result = cn_request_set_call_waiting(g_context_p, &cn_call_waiting, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_call_waiting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_WAITING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_CALL_WAITING == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_service_class_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_get_call_waiting_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t cn_service_class;
    cn_call_waiting_t *cn_call_waiting_p = NULL;
    int32_t *mal_service_class_p = (int32_t *) calloc(1, sizeof(int32_t));
    mal_ss_service_response response;

    typedef struct {
        uint8_t                           response[2];        /**< SS Service response information, refer event callback function for details. */
        mal_ss_call_fwd_info             *call_forward_info ;  /**< Call Forward Infomation structure See \ref mal_ss_call_fwd_info */
        mal_ss_gsm_additional_info       *additional_info;    /**< RETURN RESULT information as part of SS Service response  See \ref mal_ss_gsm_additional_info */
        mal_ss_error_type_value          *error_info;         /**< SS failure error type and value See \ref mal_ss_error_type_value */

    } mal_ss_service_response;

    if (NULL == mal_service_class_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_service_class = CN_SERVICE_CLASS_GSM_VOICE;

    g_mal_request_data_p = mal_service_class_p;

    /* Send CN_REQUEST_GET_CALL_WAITING */
    result = cn_request_get_call_waiting(g_context_p, cn_service_class, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_call_waiting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    memset(&response, 0, sizeof(mal_ss_service_response));
    response.response[0] = 1; /* call waiting enabled */
    response.response[1] = GSM_VOICE;

    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_CALL_WAITING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_CALL_WAITING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
    TC_ASSERT(*mal_service_class_p == (int32_t) cn_service_class);

    cn_call_waiting_p = (cn_call_waiting_t *) msg_p->payload;

    TC_ASSERT(response.response[0] == (int32_t) cn_call_waiting_p->service_class);
    TC_ASSERT(response.response[1] == (int32_t) cn_call_waiting_p->setting);

exit:
    free(msg_p);
    free(mal_service_class_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}


tc_result_t cn_request_get_call_waiting_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t cn_service_class;
    mal_ss_service_response response;
    int32_t *mal_service_class_p = (int32_t *) calloc(1, sizeof(int32_t));

    if (NULL == mal_service_class_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_service_class = 1;

    g_mal_request_data_p = mal_service_class_p;

    /* Send CN_REQUEST_GET_CALL_WAITING */
    result = cn_request_get_call_waiting(g_context_p, cn_service_class, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_call_waiting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    memset(&response, 0, sizeof(mal_ss_service_response));
    response.response[0] = 1;
    response.response[1] = 1;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_CALL_WAITING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_CALL_WAITING == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);
    free(mal_service_class_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_get_timing_advance(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_timing_advance_info_t *cn_info = NULL;
    mal_gss_timing_advance_info mal_info;


    /* Send CN_REQUEST_GET_TIMING_ADVANCE */
    result = cn_get_timing_advance_value(g_context_p, (cn_client_tag_t) 2057);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_timing_advance failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    mal_info.value = 5;
    mal_info.validity = MAL_GSS_TIMING_ADVANCE_VALID;

    call_event_callback(CN_RESPONSE_TIMING_ADVANCE, &mal_info, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_TIMING_ADVANCE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_TIMING_ADVANCE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2057 == msg_p->client_tag);
    TC_ASSERT(0 != msg_p->payload_size);
    cn_info = (cn_timing_advance_info_t *)msg_p->payload;

    TC_ASSERT(5 == cn_info->value);
    TC_ASSERT(CN_GSS_TIMING_ADVANCE_VALID == cn_info->validity);

exit:
    free(msg_p);
    return tc_result;
}
tc_result_t cn_request_get_timing_advance_fail(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_gss_timing_advance_info mal_info;


    /* Send CN_REQUEST_GET_TIMING_ADVANCE */
    result = cn_get_timing_advance_value(g_context_p, (cn_client_tag_t) 2057);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_timing_advance failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    mal_info.value = 5;
    mal_info.validity = MAL_GSS_TIMING_ADVANCE_VALID;

    call_event_callback(CN_RESPONSE_TIMING_ADVANCE, &mal_info, MAL_GSS_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_TIMING_ADVANCE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_TIMING_ADVANCE == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(2057 == msg_p->client_tag);
    TC_ASSERT(0 == msg_p->payload_size);

exit:
    free(msg_p);
    return tc_result;
}

#define NMR_GERAN_MEASUREMENTS_RESULT "1234567890123456"
#define NMR_GERAN_ARFCN "AABBCC"
tc_result_t cn_request_nmr_geran_info()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_nmr_info response_data;
    cn_nmr_info_t *cn_response_data_p = NULL;

    g_mal_rat_type = MAL_NET_NMR_RAT_GERAN;
    g_mal_nmr_type = MAL_NET_SIM_INTER_FREQ_NMR;

    result = cn_request_nmr_info(g_context_p, CN_NMR_RAT_TYPE_GERAN, CN_NMR_UTRAN_TYPE_INTER_FREQ_NMR, 92);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_nmr_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    response_data.rat = MAL_NET_NMR_RAT_GERAN;
    response_data.geran_info.num_of_channels = 3;
    response_data.geran_info.measurement_results = (cn_uint8_t *)NMR_GERAN_MEASUREMENTS_RESULT;
    response_data.geran_info.ARFCN_list = (cn_uint16_t *)NMR_GERAN_ARFCN;

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_NET_NMR_INFO_RESP, &response_data, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NMR_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_NMR_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(92 == msg_p->client_tag);
    TC_ASSERT(sizeof(cn_nmr_info_t) == msg_p->payload_size);
    cn_response_data_p = (cn_nmr_info_t *)msg_p->payload;
    TC_ASSERT(CN_NMR_RAT_TYPE_GERAN == cn_response_data_p->rat);
    TC_ASSERT(3 == cn_response_data_p->nmr_info.geran_info.num_of_channels);
    TC_ASSERT(0 == memcmp(cn_response_data_p->nmr_info.geran_info.measurement_results, NMR_GERAN_MEASUREMENTS_RESULT, strlen(NMR_GERAN_MEASUREMENTS_RESULT)));
    TC_ASSERT(0 == memcmp(cn_response_data_p->nmr_info.geran_info.ARFCN_list, NMR_GERAN_ARFCN, strlen(NMR_GERAN_ARFCN)));

exit:
    free(msg_p);
    return tc_result;
}

#define NMR_UTRAN_MEASUREMENTS_REPORT "ABC"
tc_result_t cn_request_nmr_utran_info()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_nmr_info response_data;
    cn_nmr_info_t *cn_response_data_p = NULL;

    g_mal_rat_type = MAL_NET_NMR_RAT_UTRAN;
    g_mal_nmr_type = MAL_NET_SIM_INTER_FREQ_NMR;

    /* Send CN_REQUEST_HANGUP */
    result = cn_request_nmr_info(g_context_p, CN_NMR_RAT_TYPE_UTRAN, CN_NMR_UTRAN_TYPE_INTER_FREQ_NMR, 91);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_nmr_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    response_data.rat = MAL_NET_NMR_RAT_UTRAN;
    response_data.utran_info.mm_report_len = 3;
    response_data.utran_info.measurements_report = (cn_uint8_t *) NMR_UTRAN_MEASUREMENTS_REPORT;

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_NET_NMR_INFO_RESP, &response_data, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NMR_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_NMR_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(91 == msg_p->client_tag);
    TC_ASSERT(sizeof(cn_nmr_info_t) == msg_p->payload_size);
    cn_response_data_p = (cn_nmr_info_t *)msg_p->payload;
    TC_ASSERT(CN_NMR_RAT_TYPE_UTRAN == cn_response_data_p->rat);
    TC_ASSERT(3 == cn_response_data_p->nmr_info.utran_info.mm_report_len);
    TC_ASSERT(0 == memcmp(cn_response_data_p->nmr_info.utran_info.measurements_report, NMR_UTRAN_MEASUREMENTS_REPORT, strlen(NMR_UTRAN_MEASUREMENTS_REPORT)));

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_nmr_utran_info_fail()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_nmr_info response_data;

    /* Send CN_REQUEST_HANGUP */
    result = cn_request_nmr_info(g_context_p, CN_NMR_RAT_TYPE_UTRAN, CN_NMR_UTRAN_TYPE_INTER_FREQ_NMR, 91);

    g_mal_rat_type = MAL_NET_NMR_RAT_UTRAN;
    g_mal_nmr_type = MAL_NET_SIM_INTER_FREQ_NMR;

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_nmr_info failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    response_data.rat = MAL_NET_NMR_RAT_UTRAN;
    response_data.utran_info.mm_report_len = 3;
    response_data.utran_info.measurements_report = (cn_uint8_t *) NMR_UTRAN_MEASUREMENTS_REPORT;

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_NET_NMR_INFO_RESP, &response_data, MAL_NET_INVALID_DATA, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_NMR_INFO */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_NMR_INFO == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(91 == msg_p->client_tag);
    TC_ASSERT(0 == msg_p->payload_size);

exit:
    free(msg_p);
    return tc_result;
}

/* =================
 * EVENT TESTCASES
 * =================
 */
tc_result_t cn_event_radio_status_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_mce_rf_status mal_rf_info = MAL_MCE_RF_TRANSITION;
    cn_rf_status_t *cn_info_p = NULL;

    mce_event_callback(MAL_MCE_RF_STATE_IND, &mal_rf_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive CN_EVENT_RADIO_STATUS */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_RADIO_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_info_p = (cn_rf_status_t *) msg_p->payload;
    TC_ASSERT(CN_RF_TRANSITION == *cn_info_p);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_modem_registration_status_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_registration_info mal_registration_info;
    cn_registration_info_t *cn_registration_info_p = NULL;

    memset(&mal_registration_info, 0, sizeof(mal_net_registration_info));

    mal_registration_info.reg_status = (uint32_t) 1;
    mal_registration_info.rat = MAL_NET_RAT_UMTS;
    mal_registration_info.selection_mode = MAL_NET_SELECT_MODE_AUTOMATIC;
    mal_registration_info.cid = (uint32_t) 44;
    mal_registration_info.lac = (uint16_t) 55;
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    mal_registration_info.gprs_attached = 1;

    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *)&mal_registration_info, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_NET_MODEM_REG_STATUS_IND */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_MODEM_REGISTRATION_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_registration_info_p = (cn_registration_info_t *) msg_p->payload;
    TC_ASSERT(mal_registration_info.reg_status == cn_registration_info_p->reg_status);
    TC_ASSERT(mal_registration_info.rat == cn_registration_info_p->rat);
    TC_ASSERT(CN_NETWORK_SEARCH_MODE_AUTOMATIC == cn_registration_info_p->search_mode);
    TC_ASSERT(mal_registration_info.cid == cn_registration_info_p->cid);
    TC_ASSERT(mal_registration_info.lac == cn_registration_info_p->lac);
    TC_ASSERT(0 == strcmp((char *) mal_registration_info.mcc_mnc_string, (char *) cn_registration_info_p->mcc_mnc));

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_modem_registration_status_negative_1()
{
    return TC_RESULT_NOT_IMPLEMENTED;
}


tc_result_t cn_event_time_info_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_net_time_info mal_time_info;
    cn_time_info_t *time_info_p = NULL;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Generate fake MAL event */
    mal_time_info.year = 10;
    mal_time_info.month = 9;
    mal_time_info.day = 8;
    mal_time_info.hour = 7;
    mal_time_info.minute = 55;
    mal_time_info.second = 2;
    mal_time_info.time_zone_sign = 0;
    mal_time_info.time_zone_value = 1;
    mal_time_info.day_light_saving_time = 64;
    net_event_callback(MAL_NET_TIME_IND, &mal_time_info, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive CN_EVENT_TIME_INFO */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_TIME_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    time_info_p = (cn_time_info_t *)msg_p->payload;
    TC_ASSERT(10 == time_info_p->year);
    TC_ASSERT(9  == time_info_p->month);
    TC_ASSERT(8  == time_info_p->day);
    TC_ASSERT(7  == time_info_p->hour);
    TC_ASSERT(55 == time_info_p->minute);
    TC_ASSERT(2  == time_info_p->second);
    TC_ASSERT(0  == time_info_p->time_zone_sign);
    TC_ASSERT(1  == time_info_p->time_zone_value);
    TC_ASSERT(64 == time_info_p->day_light_saving_time);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_name_info_positive_gsm7()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_net_name_info mal_name_info;
    cn_network_name_info_t *name_info_p = NULL;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint8_t mcc_mnc[] = "34567";
    uint8_t spn_gsm7[81] = "";
    uint8_t mnn_gsm7[41] = "";
    size_t length;
    int res;

    /* Generate fake MAL event */
    memset(&mal_name_info, 0, sizeof(mal_name_info));
    mal_name_info.mcc_mnc_string = mcc_mnc;

    length = sizeof(spn_gsm7) - 1;
    res = str_convert_string(CHARSET_UTF_8,
                             sizeof(LONG_OPERATOR_NAME_STRING) - 1, (uint8_t *)LONG_OPERATOR_NAME_STRING, /*spare*/ 0,
                             CHARSET_GSM_DEFAULT_7_BIT, &length, spn_gsm7);
    mal_name_info.full_operator_name.text_p = (char *)spn_gsm7;
    mal_name_info.full_operator_name.length = (uint8_t)length;
    mal_name_info.full_operator_name.add_ci = (uint8_t)0;
    mal_name_info.full_operator_name.dcs = 0x00; /* GSM-7 */
    mal_name_info.full_operator_name.spare = (uint8_t)0;

    length = sizeof(mnn_gsm7) - 1;
    res = str_convert_string(CHARSET_UTF_8,
                             sizeof(SHORT_OPERATOR_NAME_STRING) - 1, (uint8_t *)SHORT_OPERATOR_NAME_STRING, /*spare*/ 0,
                             CHARSET_GSM_DEFAULT_7_BIT, &length, mnn_gsm7);
    mal_name_info.short_operator_name.text_p = (char *)mnn_gsm7;
    mal_name_info.short_operator_name.length = (uint8_t)length;
    mal_name_info.short_operator_name.add_ci = (uint8_t)0;
    mal_name_info.short_operator_name.dcs = 0x00; /* GSM-7 */
    mal_name_info.short_operator_name.spare = (uint8_t)0;

    net_event_callback(MAL_NET_NITZ_NAME_IND, &mal_name_info, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive CN_EVENT_NETWORK_INFO */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_NETWORK_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    name_info_p = (cn_network_name_info_t *)msg_p->payload;

    TC_ASSERT(strcmp(name_info_p->mcc_mnc, (char *)mcc_mnc) == 0);
    TC_ASSERT(memcmp(name_info_p->name[CN_NETWORK_NAME_LONG].text, spn_gsm7, name_info_p->name[CN_NETWORK_NAME_LONG].length) == 0);
    TC_ASSERT(memcmp(name_info_p->name[CN_NETWORK_NAME_SHORT].text, mnn_gsm7, name_info_p->name[CN_NETWORK_NAME_SHORT].length) == 0);

exit:
    free(msg_p);
    return tc_result;

}

tc_result_t cn_event_name_info_positive_ucs2()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_net_name_info mal_name_info;
    cn_network_name_info_t *name_info_p = NULL;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint8_t mcc_mnc[] = "45678";
    uint8_t spn_ucs2[121] = "";
    uint8_t mnn_ucs2[41] = "";
    size_t length;
    int res;

    /* Generate fake MAL event */
    mal_name_info.mcc_mnc_string      = mcc_mnc;

    length = sizeof(spn_ucs2) - 1;
    res = str_convert_string(CHARSET_UTF_8,
                             sizeof(LONG_OPERATOR_NAME_STRING) - 1, (uint8_t *)LONG_OPERATOR_NAME_STRING, /*spare*/ 0,
                             CHARSET_UCS_2, &length, spn_ucs2);
    mal_name_info.full_operator_name.text_p = (char *)spn_ucs2;
    mal_name_info.full_operator_name.length = (uint8_t)length;
    mal_name_info.full_operator_name.add_ci = (uint8_t)0;
    mal_name_info.full_operator_name.dcs = 0x01; /* UCS-2 */
    mal_name_info.full_operator_name.spare = (uint8_t)0;

    length = sizeof(mnn_ucs2) - 1;
    res = str_convert_string(CHARSET_UTF_8,
                             sizeof(SHORT_OPERATOR_NAME_STRING) - 1, (uint8_t *)SHORT_OPERATOR_NAME_STRING, /*spare*/ 0,
                             CHARSET_UCS_2, &length, mnn_ucs2);
    mal_name_info.short_operator_name.text_p = (char *)mnn_ucs2;
    mal_name_info.short_operator_name.length = (uint8_t)length;
    mal_name_info.short_operator_name.add_ci = (uint8_t)0;
    mal_name_info.short_operator_name.dcs = 0x01; /* UCS-2 */
    mal_name_info.short_operator_name.spare = (uint8_t)0;

    net_event_callback(MAL_NET_NITZ_NAME_IND, &mal_name_info, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive CN_EVENT_NETWORK_INFO */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_NETWORK_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    name_info_p = (cn_network_name_info_t *)msg_p->payload;

    TC_ASSERT(strcmp(name_info_p->mcc_mnc, (char *)mcc_mnc) == 0);
    TC_ASSERT(memcmp(name_info_p->name[CN_NETWORK_NAME_LONG].text, spn_ucs2, name_info_p->name[CN_NETWORK_NAME_LONG].length) == 0);
    TC_ASSERT(memcmp(name_info_p->name[CN_NETWORK_NAME_SHORT].text, mnn_ucs2, name_info_p->name[CN_NETWORK_NAME_SHORT].length) == 0);

exit:
    free(msg_p);
    return tc_result;

}

tc_result_t cn_event_net_modem_detailed_fail_cause_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_net_detailed_fail_cause_t *cn_net_detailed_fail_cause_p = NULL;

    uint8_t reject_code = 42;         /* Reject code, if error is reported
                                        shall be propagated as-is */

    net_event_callback(MAL_NET_MODEM_DETAILED_FAIL_CAUSE, &reject_code, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_NET_MODEM_DETAILED_FAIL_CAUSE */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_NET_DETAILED_FAIL_CAUSE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_net_detailed_fail_cause_p = (cn_net_detailed_fail_cause_t *)msg_p->payload;

    TC_ASSERT(CN_NET_DETAILED_FAIL_CAUSE_CLASS_NET == cn_net_detailed_fail_cause_p->class);
    TC_ASSERT(42 == cn_net_detailed_fail_cause_p->cause);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_ss_detailed_fail_cause_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_net_detailed_fail_cause_t *cn_net_detailed_fail_cause_p = NULL;
    mal_ss_error_type_value ss_error_info;

    ss_error_info.error_type = SS_ERROR_TYPE_GSM;
    ss_error_info.error_value = 42; /* This shall be propagated as-is */
    ss_event_callback(MAL_SS_DETAILED_FAIL_CAUSE, (void *)&ss_error_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_DETAILED_FAIL_CAUSE */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_NET_DETAILED_FAIL_CAUSE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_net_detailed_fail_cause_p = (cn_net_detailed_fail_cause_t *)msg_p->payload;

    TC_ASSERT(CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS == cn_net_detailed_fail_cause_p->class);
    TC_ASSERT(42 == cn_net_detailed_fail_cause_p->cause);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_ss_detailed_fail_cause_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_net_detailed_fail_cause_t *cn_net_detailed_fail_cause_p = NULL;
    mal_ss_error_type_value ss_error_info;

    ss_error_info.error_type = SS_ERROR_TYPE_GSM;
    ss_error_info.error_value = 42; /* This shall be propagated as-is */
    ss_event_callback(MAL_SS_DETAILED_FAIL_CAUSE, (void *)&ss_error_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_DETAILED_FAIL_CAUSE */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_NET_DETAILED_FAIL_CAUSE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_net_detailed_fail_cause_p = (cn_net_detailed_fail_cause_t *)msg_p->payload;

    TC_ASSERT(CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS == cn_net_detailed_fail_cause_p->class);
    TC_ASSERT(42 == cn_net_detailed_fail_cause_p->cause);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_ss_detailed_fail_cause_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_ss_error_type_value ss_error_info;

    ss_error_info.error_type = SS_ERROR_TYPE_GSM_MSG; /* This shall be not be propagated */
    ss_error_info.error_value = 42;
    ss_event_callback(MAL_SS_DETAILED_FAIL_CAUSE, (void *)&ss_error_info, MAL_SUCCESS, NULL);

    usleep(EVENT_NOT_SENT_THRESHOLD);

    /* Shall not receive MAL_SS_DETAILED_FAIL_CAUSE */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_FAILURE != result) {
        CN_LOG_E("cn_message_receive failed (did not consume error as expected)!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(NULL == msg_p);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_ss_status_info_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_status_info_t *ss_status_info_p = NULL;
    mal_ss_ussd_status_ind_info status_ind_info;

    status_ind_info.ussd_status_ind_type = MAL_SS_STATUS_REQUEST_USSD_BUSY;
    status_ind_info.error_info = NULL;

    status_ind_info.error_info = calloc(1, sizeof(*status_ind_info.error_info));

    if (status_ind_info.error_info) {
        status_ind_info.error_info->error_type = SS_ERROR_TYPE_GSM_MSG;
        status_ind_info.error_info->error_value =  MAL_SS_GSM_MSG_INCORRECT;
    }

    status_ind_info.ussd_session_id = (void *) 1;
    status_ind_info.is_ussd_ue_terminated = 1; /* Not exposed by the CNS interfaces */
    ss_event_callback(MAL_SS_STATUS_IND, (void *)&status_ind_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_STATUS_IND MAL_SS_GSM_STATUS_REQUEST_USSD_STOP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_SS_STATUS_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    ss_status_info_p = (cn_ss_status_info_t *)msg_p->payload;

    TC_ASSERT(ss_status_info_p->type == CN_SS_STATUS_INFO_TYPE_USSD_BUSY);
    TC_ASSERT(1 == ss_status_info_p->session_id);
    TC_ASSERT(CN_SS_ERROR_CODE_TYPE_CN == ss_status_info_p->ss_error.cn_ss_error_code_type);
    TC_ASSERT(CN_SS_ERROR_CODE_GSM_MSG_INCORRECT == ss_status_info_p->ss_error.cn_ss_error_value.cn_ss_error_code);

exit:
    free(msg_p);
    free(status_ind_info.error_info);
    return tc_result;
}


tc_result_t cn_event_ss_status_ussd_stop()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_status_info_t *ss_status_info_p = NULL;
    mal_ss_ussd_status_ind_info status_ind_info;

    status_ind_info.ussd_status_ind_type =  MAL_SS_STATUS_REQUEST_USSD_STOP;
    status_ind_info.error_info = NULL;

    status_ind_info.error_info = calloc(1, sizeof(*status_ind_info.error_info));

    if (status_ind_info.error_info) {
        status_ind_info.error_info->error_type = SS_ERROR_TYPE_GSM_MSG;
        status_ind_info.error_info->error_value =  MAL_SS_GSM_MSG_INCORRECT;
    }

    status_ind_info.ussd_session_id = (void *) 1;
    status_ind_info.is_ussd_ue_terminated = 1; /* Not exposed by the CNS interfaces */
    ss_event_callback(MAL_SS_STATUS_IND, (void *)&status_ind_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_STATUS_IND MAL_SS_GSM_STATUS_REQUEST_USSD_STOP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

#if 0
    TC_ASSERT(CN_EVENT_SS_STATUS_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    ss_status_info_p = (cn_ss_status_info_t *)msg_p->payload;

    TC_ASSERT(ss_status_info_p->type == MAL_SS_STATUS_REQUEST_USSD_STOP);
    TC_ASSERT(1 == ss_status_info_p->session_id);
    TC_ASSERT(CN_SS_ERROR_CODE_TYPE_CN == ss_status_info_p->ss_error.cn_ss_error_code_type);
    TC_ASSERT(CN_SS_ERROR_CODE_GSM_MSG_INCORRECT == ss_status_info_p->ss_error.cn_ss_error_value.cn_ss_error_code);
#endif 0
exit:
    free(msg_p);
    free(status_ind_info.error_info);
    return tc_result;
}



tc_result_t cn_event_ss_status_ussd_failed()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_status_info_t *ss_status_info_p = NULL;
    mal_ss_ussd_status_ind_info status_ind_info;

    status_ind_info.ussd_status_ind_type =  MAL_SS_STATUS_REQUEST_USSD_FAILED;
    status_ind_info.error_info = NULL;

    status_ind_info.error_info = calloc(1, sizeof(*status_ind_info.error_info));

    if (status_ind_info.error_info) {
        status_ind_info.error_info->error_type = SS_ERROR_TYPE_GSM_MSG;
        status_ind_info.error_info->error_value =  MAL_SS_GSM_MSG_INCORRECT;
    }

    status_ind_info.ussd_session_id = (void *) 1;
    status_ind_info.is_ussd_ue_terminated = 1; /* Not exposed by the CNS interfaces */
    ss_event_callback(MAL_SS_STATUS_IND, (void *)&status_ind_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_STATUS_IND MAL_SS_GSM_STATUS_REQUEST_USSD_STOP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

#if 0
    TC_ASSERT(CN_EVENT_SS_STATUS_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    ss_status_info_p = (cn_ss_status_info_t *)msg_p->payload;

    TC_ASSERT(ss_status_info_p->type == MAL_SS_STATUS_REQUEST_USSD_FAILED);
    TC_ASSERT(1 == ss_status_info_p->session_id);
    TC_ASSERT(CN_SS_ERROR_CODE_TYPE_CN == ss_status_info_p->ss_error.cn_ss_error_code_type);
    TC_ASSERT(CN_SS_ERROR_CODE_GSM_MSG_INCORRECT == ss_status_info_p->ss_error.cn_ss_error_value.cn_ss_error_code);
#endif 0
exit:
    free(msg_p);
    free(status_ind_info.error_info);
    return tc_result;
}


tc_result_t cn_event_ussd_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ussd_info_t *ussd_info_p = NULL;
    mal_ss_ussd_info ussd_info;

    ussd_info.ussd_type_code = 42;
    ussd_info.ussd_received_type = MAL_SS_GSM_USSD_END;
    ussd_info.ussd_data.length = strlen("Message");
    ussd_info.ussd_data.dcs = 3;
    ussd_info.ussd_data.ussd_str = (uint8_t *) "Message"; /* Not zero-terminated in live setting, doesn't matter for the module tests */
    ussd_info.ussd_session_id = (void *) 1;
    ussd_info.is_ussd_ue_terminated = 1;
    ss_event_callback(MAL_SS_ON_USSD, (void *)&ussd_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_STATUS_IND MAL_SS_GSM_STATUS_REQUEST_USSD_STOP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_USSD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    ussd_info_p = (cn_ussd_info_t *)msg_p->payload;

    TC_ASSERT(ussd_info_p->type == 42);
    TC_ASSERT(ussd_info_p->received_type == CN_USSD_RECEIVED_TYPE_GSM_USSD_END);
    TC_ASSERT(ussd_info_p->length == strlen("Message")); /* Do not check the NULL terminator since it's not included. */
    TC_ASSERT(ussd_info_p->dcs == 3);
    TC_ASSERT(ussd_info_p->session_id == 1);
    TC_ASSERT(ussd_info_p->me_initiated == 1);
    TC_ASSERT(memcmp(ussd_info_p->ussd_string, "Message", strlen("Message")) == 0);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_gsm_ussd_req()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ussd_info_t *ussd_info_p = NULL;
    mal_ss_ussd_info ussd_info;

    ussd_info.ussd_type_code = 42;
    ussd_info.ussd_received_type = MAL_SS_GSM_USSD_REQUEST;
    ussd_info.ussd_data.length = strlen("Message");
    ussd_info.ussd_data.dcs = 3;
    ussd_info.ussd_data.ussd_str = (uint8_t *) "Message"; /* Not zero-terminated in live setting, doesn't matter for the module tests */
    ussd_info.ussd_session_id = (void *) 1;
    ussd_info.is_ussd_ue_terminated = 1;
    ss_event_callback(MAL_SS_ON_USSD, (void *)&ussd_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_STATUS_IND MAL_SS_GSM_STATUS_REQUEST_USSD_STOP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_USSD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    ussd_info_p = (cn_ussd_info_t *)msg_p->payload;

    TC_ASSERT(ussd_info_p->type == 42);
    TC_ASSERT(ussd_info_p->received_type == CN_USSD_RECEIVED_TYPE_GSM_USSD_REQUEST);
    TC_ASSERT(ussd_info_p->length == strlen("Message")); /* Do not check the NULL terminator since it's not included. */
    TC_ASSERT(ussd_info_p->dcs == 3);
    TC_ASSERT(ussd_info_p->session_id == 1);
    TC_ASSERT(ussd_info_p->me_initiated == 1);
    TC_ASSERT(memcmp(ussd_info_p->ussd_string, "Message", strlen("Message")) == 0);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_gsm_ussd_notify()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ussd_info_t *ussd_info_p = NULL;
    mal_ss_ussd_info ussd_info;

    ussd_info.ussd_type_code = 42;
    ussd_info.ussd_received_type = MAL_SS_GSM_USSD_NOTIFY;
    ussd_info.ussd_data.length = strlen("Message");
    ussd_info.ussd_data.dcs = 3;
    ussd_info.ussd_data.ussd_str = (uint8_t *) "Message"; /* Not zero-terminated in live setting, doesn't matter for the module tests */
    ussd_info.ussd_session_id = (void *) 1;
    ussd_info.is_ussd_ue_terminated = 1;
    ss_event_callback(MAL_SS_ON_USSD, (void *)&ussd_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_STATUS_IND MAL_SS_GSM_STATUS_REQUEST_USSD_STOP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

#if 0
    TC_ASSERT(CN_EVENT_USSD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    ussd_info_p = (cn_ussd_info_t *)msg_p->payload;

    TC_ASSERT(ussd_info_p->type == 42);
    TC_ASSERT(ussd_info_p->received_type == MAL_SS_GSM_USSD_NOTIFY);
    TC_ASSERT(ussd_info_p->length == strlen("Message")); /* Do not check the NULL terminator since it's not included. */
    TC_ASSERT(ussd_info_p->dcs == 3);
    TC_ASSERT(ussd_info_p->session_id == 1);
    TC_ASSERT(ussd_info_p->me_initiated == 1);
    TC_ASSERT(memcmp(ussd_info_p->ussd_string, "Message", strlen("Message")) == 0);
#endif 0
exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_gsm_ussd_command()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ussd_info_t *ussd_info_p = NULL;
    mal_ss_ussd_info ussd_info;

    ussd_info.ussd_type_code = 42;
    ussd_info.ussd_received_type = MAL_SS_GSM_USSD_COMMAND;
    ussd_info.ussd_data.length = strlen("Message");
    ussd_info.ussd_data.dcs = 3;
    ussd_info.ussd_data.ussd_str = (uint8_t *) "Message"; /* Not zero-terminated in live setting, doesn't matter for the module tests */
    ussd_info.ussd_session_id = (void *) 1;
    ussd_info.is_ussd_ue_terminated = 1;
    ss_event_callback(MAL_SS_ON_USSD, (void *)&ussd_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_STATUS_IND MAL_SS_GSM_STATUS_REQUEST_USSD_STOP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

#if 0
    TC_ASSERT(CN_EVENT_USSD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    ussd_info_p = (cn_ussd_info_t *)msg_p->payload;

    TC_ASSERT(ussd_info_p->type == 42);
    TC_ASSERT(ussd_info_p->received_type == MAL_SS_GSM_USSD_COMMAND);
    TC_ASSERT(ussd_info_p->length == strlen("Message")); /* Do not check the NULL terminator since it's not included. */
    TC_ASSERT(ussd_info_p->dcs == 3);
    TC_ASSERT(ussd_info_p->session_id == 1);
    TC_ASSERT(ussd_info_p->me_initiated == 1);
    TC_ASSERT(memcmp(ussd_info_p->ussd_string, "Message", strlen("Message")) == 0);
#endif 0
exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_ring_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    char *number_p = "12345";
    uint16_t name_str[] = { 'n', 'a', 'm', 'e' };
    cn_message_t *msg_p = NULL;
    mal_call_context mal_call_context = {
        .callState = CALL_INCOMING,
        .callId = (int32_t) 1,
        .addrType = (int32_t) 1,
        .message_type = (uint8_t) 2,
        .message_direction = (uint8_t) 0,
        .transaction_id = (uint8_t) 0,
        .isMpty = (char) 1,
        .isMT = (char) 0,
        .als = (char) 1,
        .call_mode = MAL_CALL_MODE_SPEECH,
        .isVoicePrivacy = (char) 1,
        .numberPresentation = CN_USER_SCREENED_PASSED | CN_PRESENTATION_RESTRICTED,
        .number = number_p,
        .call_cli_cause = (char) 4,
        .namePresentation = CN_USER_SCREENED_PASSED | CN_PRESENTATION_RESTRICTED,
        .name_length = sizeof(name_str) / sizeof(uint16_t),
        .name = name_str,
        .connectedLine = { 0, 0, NULL, 0, NULL },
        .call_cause = { 0, 0, 0, NULL }
    };
    cn_call_context_t *call_context_p = NULL;

    call_event_callback(MAL_CALL_RING, (void *) &mal_call_context, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_CALL_RING */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_RING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    call_context_p = (cn_call_context_t *) msg_p->payload;

    TC_ASSERT(mal_call_context.callState == call_context_p->call_state);
    TC_ASSERT(mal_call_context.callId == call_context_p->call_id);
    TC_ASSERT(mal_call_context.addrType == call_context_p->address_type);
    TC_ASSERT(mal_call_context.message_type == call_context_p->message_type);
    TC_ASSERT(mal_call_context.message_direction == call_context_p->message_direction);
    TC_ASSERT(mal_call_context.transaction_id == call_context_p->transaction_id);
    TC_ASSERT(mal_call_context.isMpty == call_context_p->is_multiparty);
    TC_ASSERT(mal_call_context.isMT == call_context_p->is_MT);
    TC_ASSERT(mal_call_context.call_mode == MAL_CALL_MODE_SPEECH);
    TC_ASSERT(CN_CALL_MODE_SPEECH == call_context_p->mode);
    TC_ASSERT(mal_call_context.isVoicePrivacy == call_context_p->is_voice_privacy);
    TC_ASSERT(0 == strcmp(mal_call_context.number, call_context_p->number));
    TC_ASSERT((CN_USER_SCREENED_PASSED | CN_PRESENTATION_RESTRICTED) == call_context_p->number_presentation);
    TC_ASSERT((cn_cause_no_cli_t) mal_call_context.call_cli_cause == call_context_p->cause_no_cli);
    TC_ASSERT(0 == memcmp(mal_call_context.name, call_context_p->name, sizeof(name_str)));
    TC_ASSERT(mal_call_context.name_length == call_context_p->name_char_length);
    TC_ASSERT((CN_USER_SCREENED_PASSED | CN_PRESENTATION_RESTRICTED) == call_context_p->name_presentation);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_call_cnap_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    char *number_p = "12345";
    uint16_t name_str[] = { 'n', 'a', 'm', 'e' };
    char *cause_p = "cause";
    cn_message_t *msg_p = NULL;
    mal_call_context mal_call_context = {
        .callState = CALL_INCOMING,
        .callId = (int32_t) 1,
        .addrType = (int32_t) 1,
        .message_type = (uint8_t) 2,
        .message_direction = (uint8_t) 0,
        .transaction_id = (uint8_t) 0,
        .isMpty = (char) 1,
        .isMT = (char) 0,
        .als = (char) 1,
        .call_mode = MAL_CALL_MODE_SPEECH,
        .isVoicePrivacy = (char) 1,
        .number = number_p,
        .numberPresentation = CN_USER_NOT_SCREENED | CN_PRESENTATION_ALLOWED,
        .call_cli_cause = (char) 4,
        .name_length = sizeof(name_str) / sizeof(uint16_t),
        .name = name_str,
        .namePresentation = CN_SCREEN_NETWORK_PROVIDED | CN_PRESENTATION_UNAVAILABLE,
        .connectedLine = { 0, 0, NULL, 0, NULL },
        .call_cause = {
            CALL_CAUSE_TYPE_CLIENT,
            (uint8_t) MAL_CALL_MODEM_CAUSE_RELEASE_BY_USER,
            (uint8_t) strlen(cause_p) + 1,
            (uint8_t *) cause_p
        }
    };
    cn_call_context_t *call_context_p = NULL;

    call_event_callback(MAL_CALL_CNAP, (void *) &mal_call_context, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_CALL_CNAP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_RING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    call_context_p = (cn_call_context_t *) msg_p->payload;

    TC_ASSERT(mal_call_context.callState == call_context_p->call_state);
    TC_ASSERT(mal_call_context.callId == call_context_p->call_id);
    TC_ASSERT(mal_call_context.addrType == call_context_p->address_type);
    TC_ASSERT(mal_call_context.message_type == call_context_p->message_type);
    TC_ASSERT(mal_call_context.message_direction == call_context_p->message_direction);
    TC_ASSERT(mal_call_context.transaction_id == call_context_p->transaction_id);
    TC_ASSERT(mal_call_context.isMpty == call_context_p->is_multiparty);
    TC_ASSERT(mal_call_context.isMT == call_context_p->is_MT);
    TC_ASSERT(mal_call_context.call_mode == MAL_CALL_MODE_SPEECH);
    TC_ASSERT(CN_CALL_MODE_SPEECH == call_context_p->mode);
    TC_ASSERT(mal_call_context.isVoicePrivacy == call_context_p->is_voice_privacy);
    TC_ASSERT(mal_call_context.numberPresentation == call_context_p->number_presentation);
    TC_ASSERT(0 == strcmp(mal_call_context.number, call_context_p->number));
    TC_ASSERT((cn_cause_no_cli_t) mal_call_context.call_cli_cause == call_context_p->cause_no_cli);
    TC_ASSERT(0 == memcmp(mal_call_context.name, call_context_p->name, sizeof(name_str)));
    TC_ASSERT(mal_call_context.name_length == call_context_p->name_char_length);
    TC_ASSERT(mal_call_context.namePresentation == call_context_p->name_presentation);
    TC_ASSERT(CN_CALL_CAUSE_TYPE_SENDER_CLIENT == call_context_p->call_cause.cause_type_sender);
    TC_ASSERT(CN_CALL_CLIENT_CAUSE_RELEASE_BY_USER == call_context_p->call_cause.cause);
    TC_ASSERT(0 == strcmp(cause_p, (char *) call_context_p->call_cause.detailed_cause));
    TC_ASSERT(strlen(cause_p) + 1 == call_context_p->call_cause.detailed_cause_length);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_call_state_changed_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    mal_call_context *mal_context_p = calloc(1, sizeof(mal_call_context));
    cn_call_context_t *cn_context_p = NULL;

    if (!mal_context_p) {
        CN_LOG_E("calloc failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* CAUSE_TYPE_SENDER  = CLIENT, CLIENT_CAUSE = RELEASE_BY_USER */
    mal_context_p->callState = CALL_IDLE;
    mal_context_p->call_cause.cause_type_sender = CN_CALL_CAUSE_TYPE_SENDER_CLIENT;
    mal_context_p->call_cause.cause = 3;
    mal_context_p->call_cause.detailed_cause_length = strlen("detailed_cause test");
    mal_context_p->call_cause.detailed_cause = calloc(mal_context_p->call_cause.detailed_cause_length, sizeof(cn_uint8_t));

    if (!mal_context_p->call_cause.detailed_cause) {
        CN_LOG_E("calloc failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    memcpy(mal_context_p->call_cause.detailed_cause, "cn_event_call_state_changed_positive_1", mal_context_p->call_cause.detailed_cause_length * sizeof(cn_uint8_t));

    call_event_callback(MAL_CALL_STATE_CHANGED, mal_context_p, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("msg_p was NULL");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_context_p = (cn_call_context_t *) msg_p->payload;

    TC_ASSERT(sizeof(* cn_context_p) == msg_p->payload_size);
    TC_ASSERT(NULL != msg_p->payload);
    TC_ASSERT(CN_EVENT_CALL_STATE_CHANGED == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    /* NOTE - This CLIENT_CAUSE remaps to NETWORK_CAUSE = NORMAL in util_call_cause() */
    TC_ASSERT(CN_CALL_CAUSE_TYPE_SENDER_CLIENT == cn_context_p->call_cause.cause_type_sender);
    TC_ASSERT(3 == cn_context_p->call_cause.cause);
    TC_ASSERT(mal_context_p->call_cause.detailed_cause_length == cn_context_p->call_cause.detailed_cause_length);
    TC_ASSERT(0 == memcmp(mal_context_p->call_cause.detailed_cause, cn_context_p->call_cause.detailed_cause, cn_context_p->call_cause.detailed_cause_length * sizeof(cn_uint8_t)));

exit:

    if (mal_context_p) {
        if (mal_context_p->call_cause.detailed_cause) {
            free(mal_context_p->call_cause.detailed_cause);
        }

        free(mal_context_p);
    }

    if (msg_p) {
        free(msg_p);
    }

    return tc_result;
}

tc_result_t cn_event_call_state_changed_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    mal_call_context *mal_context_p = calloc(1, sizeof(mal_call_context));
    cn_call_context_t *cn_context_p = NULL;

    if (!mal_context_p) {
        CN_LOG_E("calloc failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* CAUSE_TYPE_SENDER = SERVER, SERVER_CAUSE = CHANNEL_LOSS */
    mal_context_p->callState = CALL_IDLE;
    mal_context_p->call_cause.cause_type_sender = 2;
    mal_context_p->call_cause.cause = 0x13;
    call_event_callback(MAL_CALL_STATE_CHANGED, mal_context_p, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("msg_p was NULL");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_context_p = (cn_call_context_t *) msg_p->payload;


    TC_ASSERT(sizeof(* cn_context_p) == msg_p->payload_size);
    TC_ASSERT(NULL != msg_p->payload);
    TC_ASSERT(CN_EVENT_CALL_STATE_CHANGED == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    /* NOTE - This SERVER call passes thru unchanged but logs a debug message */
    TC_ASSERT(2 == cn_context_p->call_cause.cause_type_sender);
    TC_ASSERT(0x13 == cn_context_p->call_cause.cause);

exit:

    if (mal_context_p) {
        free(mal_context_p);
    }

    if (msg_p) {
        free(msg_p);
    }

    return tc_result;
}

tc_result_t cn_event_call_state_changed_positive_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_call_context *mal_context_p = calloc(1, sizeof(mal_call_context));
    cn_call_context_t *cn_context_p = NULL;

    if (!mal_context_p) {
        CN_LOG_E("calloc failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* unknown CN_CALL_CAUSE_TYPE, random data */
    mal_context_p->callState = CALL_IDLE;
    mal_context_p->call_cause.cause_type_sender = 4;
    mal_context_p->call_cause.cause = 77;
    call_event_callback(MAL_CALL_STATE_CHANGED, mal_context_p, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("msg_p was NULL");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    cn_context_p = (cn_call_context_t *) msg_p->payload;


    TC_ASSERT(sizeof(* cn_context_p) == msg_p->payload_size);
    TC_ASSERT(NULL != msg_p->payload);
    TC_ASSERT(CN_EVENT_CALL_STATE_CHANGED == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    /* NOTE - This unknown sender call passes thru as 0,0 (no error) and logs an error message */
    TC_ASSERT(0 == cn_context_p->call_cause.cause_type_sender);
    TC_ASSERT(0 == cn_context_p->call_cause.cause);

exit:

    if (mal_context_p) {
        free(mal_context_p);
    }

    if (msg_p) {
        free(msg_p);
    }

    return tc_result;
}

tc_result_t cn_request_cnap_status_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    int mal_data = 1; /* PROVISIONED */

    /* Send CN_REQUEST_CNAP_STATUS */
    result = cn_request_cnap_status(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_cnap_status failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &mal_data, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CNAP_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CNAP_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
    TC_ASSERT(CN_CNAP_STATUS_PROVISIONED == *((cn_cnap_status_t *) msg_p->payload));

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_cnap_status_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    int mal_data = 1; /* PROVISIONED */

    /* Send CN_REQUEST_CNAP_STATUS */
    result = cn_request_cnap_status(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_cnap_status failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &mal_data, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_CNAP_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_CNAP_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);

exit:
    free(msg_p);

    return tc_result;
}


tc_result_t cn_request_set_call_barring_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_call_barring_t call_barring;

    call_barring.service_class = CN_SERVICE_CLASS_GSM_DEFAULT;
    call_barring.setting = CN_CALL_BARRING_ACTIVATION;

    result = cn_request_set_call_barring(g_context_p, "AO", &call_barring, "asdf", (cn_client_tag_t) 751);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_call_barring failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check MAL parameters in the stub */
    TC_ASSERT(GSM_DEFAULT == g_mal_ss_serv_class_info);
    TC_ASSERT(MAL_SS_ACTIVATION == g_mal_ss_service_op);

    /* Generate fake MAL response */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_CALL_BARRING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_set_call_barring_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_call_barring_t call_barring;

    call_barring.service_class = CN_SERVICE_CLASS_GSM_DEFAULT;
    call_barring.setting = CN_CALL_BARRING_ACTIVATION;

    result = cn_request_set_call_barring(g_context_p, "AO", &call_barring, "asdf", (cn_client_tag_t) 751);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_call_barring failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check MAL parameters in the stub */
    TC_ASSERT(GSM_DEFAULT == g_mal_ss_serv_class_info);
    TC_ASSERT(MAL_SS_ACTIVATION == g_mal_ss_service_op);

    /* Generate fake MAL response */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_CALL_BARRING == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_set_call_forward_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_call_forward_info_t call_forward_info;

    memset(&call_forward_info, 0, sizeof(cn_call_forward_info_t));

    call_forward_info.reason = 3;
    call_forward_info.status = 3;
    strcpy(call_forward_info.number, "423890");
    call_forward_info.time_seconds = 10;
    call_forward_info.service_class = CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS;

    result = cn_request_set_call_forward(g_context_p, &call_forward_info, (cn_client_tag_t) 751);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_call_barring failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check MAL parameters in the stub */
    TC_ASSERT(GSM_VOICE_FACSIMILE_SMS == g_mal_ss_serv_class_info);

    /* Generate fake MAL response */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_CALL_FORWARD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_set_call_forward_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_call_forward_info_t call_forward_info;

    memset(&call_forward_info, 0, sizeof(cn_call_forward_info_t));

    call_forward_info.service_class = CN_SERVICE_CLASS_GSM_DEFAULT;

    result = cn_request_set_call_forward(g_context_p, &call_forward_info, (cn_client_tag_t) 751);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_call_barring failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check MAL parameters in the stub */
    TC_ASSERT(GSM_DEFAULT == g_mal_ss_serv_class_info);

    /* Generate fake MAL response */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, NULL, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_CALL_FORWARD == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ss_command_forward_register()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_command_t ss_command;
    mal_ss_service_response response;
    mal_ss_gsm_additional_info additional_info;

    memset(&ss_command, 0, sizeof(cn_ss_command_t));

    strcpy((char *) ss_command.mmi_string, "*67*+4646101010*10#");
    ss_command.mmi_string_length = strlen("*67*+4646101010*10#");

    result = cn_request_ss(g_context_p, ss_command, (cn_client_tag_t) 751);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS == g_mal_ss_serv_class_info);
    TC_ASSERT(3 == g_mal_ss_forward_status);
    TC_ASSERT(1 == g_mal_ss_forward_reason);

    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    additional_info.return_result_length = strlen("TESTDATA");
    additional_info.return_result = (uint8_t *) "TESTDATA";
    response.additional_info = &additional_info;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ss_command_forward_erasure()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_command_t ss_command;
    mal_ss_service_response response;
    mal_ss_gsm_additional_info additional_info;

    memset(&ss_command, 0, sizeof(cn_ss_command_t));

    strcpy((char *) ss_command.mmi_string, "##67*+4646101010*0#");
    ss_command.mmi_string_length = strlen("##67*+4646101010*0#");
    ss_command.additional_results = TRUE;

    result = cn_request_ss(g_context_p, ss_command, (cn_client_tag_t) 751);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(CN_SERVICE_CLASS_GSM_DEFAULT_MMI == g_mal_ss_serv_class_info);
    TC_ASSERT(4 == g_mal_ss_forward_status);
    TC_ASSERT(1 == g_mal_ss_forward_reason);
    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    additional_info.return_result_length = strlen("TESTDATA");
    additional_info.return_result = (uint8_t *) "TESTDATA";
    response.additional_info = &additional_info;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ss_command_forward_register_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_command_t ss_command;
    mal_ss_service_response response;
    mal_ss_gsm_additional_info additional_info;

    memset(&ss_command, 0, sizeof(cn_ss_command_t));

    strcpy((char *) ss_command.mmi_string, "**67*+4646101010*0#");
    ss_command.mmi_string_length = strlen("**67*+4646101010*0#");

    result = cn_request_ss(g_context_p, ss_command, (cn_client_tag_t) 751);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(CN_SERVICE_CLASS_GSM_DEFAULT_MMI == g_mal_ss_serv_class_info);
    TC_ASSERT(3 == g_mal_ss_forward_status);
    TC_ASSERT(1 == g_mal_ss_forward_reason);

    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    additional_info.return_result_length = strlen("TESTDATA");
    additional_info.return_result = (uint8_t *) "TESTDATA";
    response.additional_info = &additional_info;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ss_command_forward_register_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_command_t ss_command;
    mal_ss_service_response response;
    mal_ss_gsm_additional_info additional_info;

    memset(&ss_command, 0, sizeof(cn_ss_command_t));

    strcpy((char *) ss_command.mmi_string, "**61*+4646101010*0*15#");
    ss_command.mmi_string_length = strlen("**61*+4646101010*0*15#");

    result = cn_request_ss(g_context_p, ss_command, (cn_client_tag_t) 751);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(CN_SERVICE_CLASS_GSM_DEFAULT_MMI == g_mal_ss_serv_class_info);
    TC_ASSERT(3 == g_mal_ss_forward_status);
    TC_ASSERT(2 == g_mal_ss_forward_reason);
    TC_ASSERT(15 == g_mal_ss_forward_time);

    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    additional_info.return_result_length = strlen("TESTDATA");
    additional_info.return_result = (uint8_t *) "TESTDATA";
    response.additional_info = &additional_info;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ss_command_forward_activation()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_command_t ss_command;
    mal_ss_service_response response;
    mal_ss_gsm_additional_info additional_info;

    memset(&ss_command, 0, sizeof(cn_ss_command_t));

    strcpy((char *) ss_command.mmi_string, "*67*#");
    ss_command.mmi_string_length = strlen("*67*#");

    result = cn_request_ss(g_context_p, ss_command, (cn_client_tag_t) 751);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(CN_SERVICE_CLASS_GSM_DEFAULT_MMI == g_mal_ss_serv_class_info);
    TC_ASSERT(1 == g_mal_ss_forward_status);
    TC_ASSERT(1 == g_mal_ss_forward_reason);

    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    additional_info.return_result_length = strlen("TESTDATA");
    additional_info.return_result = (uint8_t *) "TESTDATA";
    response.additional_info = &additional_info;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ss_command_forward_deactivation()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_command_t ss_command;
    mal_ss_service_response response;
    mal_ss_gsm_additional_info additional_info;

    memset(&ss_command, 0, sizeof(cn_ss_command_t));

    strcpy((char *) ss_command.mmi_string, "#67*+4646101010*0#");
    ss_command.mmi_string_length = strlen("#67*+4646101010*0#");

    result = cn_request_ss(g_context_p, ss_command, (cn_client_tag_t) 751);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(CN_SERVICE_CLASS_GSM_DEFAULT_MMI == g_mal_ss_serv_class_info);
    TC_ASSERT(0 == g_mal_ss_forward_status);
    TC_ASSERT(1 == g_mal_ss_forward_reason);

    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    additional_info.return_result_length = strlen("TESTDATA");
    additional_info.return_result = (uint8_t *) "TESTDATA";
    response.additional_info = &additional_info;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ss_command_forward_error_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_command_t ss_command;
    mal_ss_service_response response;
    mal_ss_error_type_value error;

    memset(&ss_command, 0, sizeof(cn_ss_command_t));

    strcpy((char *) ss_command.mmi_string, "##67*+4646101010*0#");
    ss_command.mmi_string_length = strlen("##67*+4646101010*0#");
    ss_command.additional_results = TRUE;

    result = cn_request_ss(g_context_p, ss_command, (cn_client_tag_t) 751);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(CN_SERVICE_CLASS_GSM_DEFAULT_MMI == g_mal_ss_serv_class_info);
    TC_ASSERT(4 == g_mal_ss_forward_status);
    TC_ASSERT(1 == g_mal_ss_forward_reason);
    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    error.error_value = 12;
    error.error_type = SS_ERROR_TYPE_GSM;
    response.additional_info = NULL;
    response.error_info = &error;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ss_command_forward_query_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_command_t ss_command;
    mal_ss_service_response response;
    mal_ss_gsm_additional_info additional_info;

    memset(&ss_command, 0, sizeof(cn_ss_command_t));

    strcpy((char *) ss_command.mmi_string, "*#67**0#");
    ss_command.mmi_string_length = strlen("*#67**0#");
    ss_command.additional_results = TRUE;

    result = cn_request_ss(g_context_p, ss_command, (cn_client_tag_t) 751);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(CN_SERVICE_CLASS_GSM_DEFAULT_MMI == g_mal_ss_query_serv_class_info);
    TC_ASSERT(2 == g_mal_ss_query_forward_status);
    TC_ASSERT(1 == g_mal_ss_query_forward_reason);
    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    additional_info.return_result_length = strlen("TESTDATA");
    additional_info.return_result = (uint8_t *) "TESTDATA";
    response.additional_info = &additional_info;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ss_command_forward_query_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_command_t ss_command;
    mal_ss_service_response response;
    mal_ss_gsm_additional_info additional_info;

    memset(&ss_command, 0, sizeof(cn_ss_command_t));

    strcpy((char *) ss_command.mmi_string, "*#61**0*15#");
    ss_command.mmi_string_length = strlen("*#61**0*15#");
    ss_command.additional_results = TRUE;

    result = cn_request_ss(g_context_p, ss_command, (cn_client_tag_t) 751);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(CN_SERVICE_CLASS_GSM_DEFAULT_MMI == g_mal_ss_query_serv_class_info);
    TC_ASSERT(2 == g_mal_ss_query_forward_status);
    TC_ASSERT(2 == g_mal_ss_query_forward_reason);
    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    additional_info.return_result_length = strlen("TESTDATA");
    additional_info.return_result = (uint8_t *) "TESTDATA";
    response.additional_info = &additional_info;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ss_command_barring_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ss_command_t ss_command;
    mal_ss_service_response response;
    mal_ss_gsm_additional_info additional_info;

    memset(&ss_command, 0, sizeof(cn_ss_command_t));

    strcpy((char *) ss_command.mmi_string, "*330*a*0#");
    ss_command.mmi_string_length = strlen("*330*a*0#");
    ss_command.additional_results = TRUE;

    result = cn_request_ss(g_context_p, ss_command, (cn_client_tag_t) 751);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(CN_SERVICE_CLASS_GSM_DEFAULT_MMI == g_mal_ss_serv_class_info);
    /* Generate fake MAL response */
    memset(&response, 0, sizeof(mal_ss_service_response));
    additional_info.return_result_length = strlen("TESTDATA");
    additional_info.return_result = (uint8_t *) "TESTDATA";
    response.additional_info = &additional_info;
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);
    TC_ASSERT(CN_CALL_BARRING_ACTIVATION == g_mal_ss_service_op);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_cell_info_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_cell_info_t *cn_cell_info_p = NULL;
    mal_net_cell_info mal_cell_info;

    memset(&mal_cell_info, 0, sizeof(mal_cell_info));
    mal_cell_info.gsm_frame_nbr      = 10;
    mal_cell_info.bands_available    = 1;
    mal_cell_info.current_cell_id    = 2737;
    mal_cell_info.current_ac         = 321;
    mal_cell_info.country_code       = 123;
    mal_cell_info.network_code       = 19;
    mal_cell_info.mcc_mnc_string     = "12";
    /* We assume these types to be identical. */
    mal_cell_info.rat                = MAL_NET_CELL_RAT_WCDMA;
    mal_cell_info.network_type       = MAL_NET_GSM_HOME_PLMN;
    mal_cell_info.service_status     = MAL_NET_LIMITED_SERVICE;

    net_event_callback(MAL_NET_CELL_INFO_IND, &mal_cell_info, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_NET_MODEM_DETAILED_FAIL_CAUSE */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_CELL_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_cell_info_p = (cn_cell_info_t *)msg_p->payload;

    TC_ASSERT(10 == cn_cell_info_p->gsm_frame_nbr);
    TC_ASSERT(1 == cn_cell_info_p->bands_available);
    TC_ASSERT(2737 == cn_cell_info_p->current_cell_id);
    TC_ASSERT(321 == cn_cell_info_p->current_ac);
    TC_ASSERT(123 == cn_cell_info_p->country_code);
    TC_ASSERT(19 == cn_cell_info_p->network_code);
    TC_ASSERT(CN_CELL_RAT_WCDMA == cn_cell_info_p->rat);
    TC_ASSERT(CN_NET_GSM_HOME_PLMN == cn_cell_info_p->network_type);
    TC_ASSERT(CN_NW_LIMITED_SERVICE == cn_cell_info_p->service_status);
    TC_ASSERT(mal_cell_info.mcc_mnc_string[0] == cn_cell_info_p->mcc_mnc[0]);
    TC_ASSERT(mal_cell_info.mcc_mnc_string[1] == cn_cell_info_p->mcc_mnc[1]);
    TC_ASSERT(mal_cell_info.mcc_mnc_string[2] == cn_cell_info_p->mcc_mnc[2]);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_generate_local_comfort_tones_positive_1() /* start local comfort tone generation */
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_call_alert_tone mal_tone_generation = MAL_CALL_START_ALERT_TONE;
    cn_comfort_tone_generation_t *tone_generation_p = NULL;

    call_event_callback(MAL_CALL_GEN_ALERTING_TONE, &mal_tone_generation, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("msg_p was NULL");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(sizeof(cn_comfort_tone_generation_t) == msg_p->payload_size);
    TC_ASSERT(NULL != msg_p->payload);
    TC_ASSERT(CN_EVENT_GENERATE_LOCAL_COMFORT_TONES == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);

    tone_generation_p = (cn_comfort_tone_generation_t *)msg_p->payload;
    TC_ASSERT(*tone_generation_p == CN_COMFORT_TONE_GENERATION_START);

exit:

    if (msg_p) {
        free(msg_p);
    }

    return tc_result;
}

tc_result_t cn_event_generate_local_comfort_tones_positive_2() /* stop local comfort tone generation */
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_call_alert_tone mal_tone_generation = MAL_CALL_STOP_ALERT_TONE;
    cn_comfort_tone_generation_t *tone_generation_p = NULL;

    call_event_callback(MAL_CALL_GEN_ALERTING_TONE, &mal_tone_generation, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive context with message */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("msg_p was NULL");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(sizeof(cn_comfort_tone_generation_t) == msg_p->payload_size);
    TC_ASSERT(NULL != msg_p->payload);
    TC_ASSERT(CN_EVENT_GENERATE_LOCAL_COMFORT_TONES == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);

    tone_generation_p = (cn_comfort_tone_generation_t *)msg_p->payload;
    TC_ASSERT(*tone_generation_p == CN_COMFORT_TONE_GENERATION_STOP);

exit:

    if (msg_p) {
        free(msg_p);
    }

    return tc_result;
}

tc_result_t cn_event_radio_info_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_rab_status_t *cn_rab_status_p = NULL;
    mal_net_rab_status_type mal_rab_status = MAL_NET_BOTH_HSDPA_HSUPA;

    net_event_callback(MAL_NET_RADIO_INFO_IND, &mal_rab_status, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("msg_p was NULL");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_RAB_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);

    cn_rab_status_p = (cn_rab_status_t *)msg_p->payload;
    TC_ASSERT(CN_RAB_STATUS_BOTH_HSDPA_HSUPA == *cn_rab_status_p);

exit:
    free(msg_p);
    return tc_result;
}

/* request_reg_status_event_config from client, positive case: CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC */
tc_result_t cn_request_reg_status_event_config_1(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_reg_status_trigger_level_t trigger_level = CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC;

    result = cn_request_reg_status_event_config(g_context_p, trigger_level, (cn_client_tag_t)998);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(MAL_NET_SELECT_MODEM_REG_STATUS_IND == g_mal_net_set_ind_state_type);
    TC_ASSERT(MAL_NET_IND_ENABLE == g_mal_net_set_ind_state_state);
    TC_ASSERT((MAL_NET_MSG_SEND_MODE_REG_STATUS_CHANGE | MAL_NET_MSG_SEND_MODE_CELL_ID_OR_LAC_CHANGE)
              == g_mal_net_reg_status_set_mode);

    net_event_callback(MAL_NET_CONFIGURE_RESP, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_REG_STATUS_EVENT_CONFIG */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_REG_STATUS_EVENT_CONFIG == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(998 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

/* request_reg_status_event_config from client, positive case: CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM */
tc_result_t cn_request_reg_status_event_config_2(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_reg_status_trigger_level_t trigger_level = CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM;

    result = cn_request_reg_status_event_config(g_context_p, trigger_level, (cn_client_tag_t)998);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(MAL_NET_SELECT_MODEM_REG_STATUS_IND == g_mal_net_set_ind_state_type);
    TC_ASSERT(MAL_NET_IND_DISABLE == g_mal_net_set_ind_state_state);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_REG_STATUS_EVENT_CONFIG */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_REG_STATUS_EVENT_CONFIG == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(998 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

/* request_reg_status_event_config from client, negative case: MAL error */
tc_result_t cn_request_reg_status_event_config_3(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_reg_status_trigger_level_t trigger_level = CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC;

    result = cn_request_reg_status_event_config(g_context_p, trigger_level, (cn_client_tag_t)998);

    WAIT_FOR_SELECT_TO_FINISH();

    /* MAL parameters to check in the stub */
    TC_ASSERT(MAL_NET_SELECT_MODEM_REG_STATUS_IND == g_mal_net_set_ind_state_type);
    TC_ASSERT(MAL_NET_IND_ENABLE == g_mal_net_set_ind_state_state);
    TC_ASSERT((MAL_NET_MSG_SEND_MODE_REG_STATUS_CHANGE | MAL_NET_MSG_SEND_MODE_CELL_ID_OR_LAC_CHANGE)
              == g_mal_net_reg_status_set_mode);

    net_event_callback(MAL_NET_CONFIGURE_RESP, NULL, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_REG_STATUS_EVENT_CONFIG */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_REG_STATUS_EVENT_CONFIG == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(998 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

/* request_reg_status_event_config from client, negative case: trigger_level out of range */
tc_result_t cn_request_reg_status_event_config_4(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_reg_status_trigger_level_t trigger_level = 42;

    result = cn_request_reg_status_event_config(g_context_p, trigger_level, (cn_client_tag_t)998);

    WAIT_FOR_SELECT_TO_FINISH();

    /* no parameters to check in the stub */

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_REG_STATUS_EVENT_CONFIG */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_REG_STATUS_EVENT_CONFIG == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(998 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


/* request_reg_status_event_config from client disconnect callback, positive case
 * The real code involving the cnserver callback that is imitated here is tested
 * when the test framework is disconnecting it's client to CNS right before shutdown.
 * to script this is out of scope of the current test framework.
 */
tc_result_t cn_request_reg_status_event_config_5()
{
    int trigger_level = 0;
    cn_error_code_t result = 0;
    tc_result_t tc_result = TC_RESULT_OK;

    CN_LOG_D("populate ETL table with entries:");
    etl_clear_table(ETL_TABLE_REG_STATUS);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 0, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 1, CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 2, CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 3, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);

    CN_LOG_D("check that the current trigger level is CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC");
    trigger_level =  etl_determine_reg_status_trigger_level();
    TC_ASSERT(CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC == trigger_level);

    CN_LOG_D("invoke callback telling CNS that a client disconnect has occured (remove CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC):");
    etl_client_disconnected_cb(2);

    /* MAL parameters to check in the stub */
    TC_ASSERT(MAL_NET_SELECT_MODEM_REG_STATUS_IND == g_mal_net_set_ind_state_type);
    TC_ASSERT(MAL_NET_IND_ENABLE == g_mal_net_set_ind_state_state);
    TC_ASSERT(MAL_NET_MSG_SEND_MODE_REG_STATUS_CHANGE == g_mal_net_reg_status_set_mode);

    net_event_callback(MAL_NET_CONFIGURE_RESP, NULL, MAL_NET_SUCCESS, g_mal_client_tag_p);

exit:
    return tc_result;
}


/* request_reg_status_event_config from client disconnect callback, negative case */
tc_result_t cn_request_reg_status_event_config_6()
{
    CN_LOG_D("");
    CN_LOG_D("clearing ETL table with entries:");
    etl_clear_table(ETL_TABLE_REG_STATUS);

    CN_LOG_D("");
    CN_LOG_D("invoke callback telling CNS that a client disconnect has occured (the client does not exist):");
    etl_client_disconnected_cb(23);

    etl_clear_table(ETL_TABLE_REG_STATUS);

    return TC_RESULT_OK; /* visual inspection */
}


tc_result_t cn_request_rat_name_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_rat_name mal_rat_name = MAL_NET_RAT_NAME_UMTS;
    cn_rat_name_t *rat_name_p = NULL;

    result = cn_request_rat_name(g_context_p, (cn_client_tag_t)997);

    WAIT_FOR_SELECT_TO_FINISH();

    /* No parameter to check in the stub */

    net_event_callback(MAL_NET_RAT_RESP, &mal_rat_name, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RAT_NAME */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    rat_name_p = (cn_rat_name_t *)msg_p->payload;

    TC_ASSERT(CN_RESPONSE_RAT_NAME == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(997 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    TC_ASSERT(CN_RAT_NAME_UMTS == *rat_name_p);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_rat_name_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_rat_name mal_rat_name = MAL_NET_RAT_NAME_GSM;
    cn_rat_name_t *rat_name_p = NULL;

    result = cn_request_rat_name(g_context_p, (cn_client_tag_t)997);

    WAIT_FOR_SELECT_TO_FINISH();

    /* No parameter to check in the stub */

    net_event_callback(MAL_NET_RAT_RESP, &mal_rat_name, MAL_NET_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RAT_NAME */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    rat_name_p = (cn_rat_name_t *)msg_p->payload;

    TC_ASSERT(CN_RESPONSE_RAT_NAME == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(997 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    TC_ASSERT(CN_RAT_NAME_GSM == *rat_name_p);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_rat_name_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_rat_name mal_rat_name = MAL_NET_RAT_NAME_UMTS;

    result = cn_request_rat_name(g_context_p, (cn_client_tag_t)997);

    WAIT_FOR_SELECT_TO_FINISH();

    /* No parameter to check in the stub */

    net_event_callback(MAL_NET_RAT_RESP, &mal_rat_name, MAL_NET_GENERAL_ERROR, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RAT_NAME */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RAT_NAME == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(997 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_rat_name_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_rat_name mal_rat_name = MAL_NET_RAT_NAME_UMTS;
    cn_rat_name_t *rat_name_p = NULL;

    net_event_callback(MAL_NET_RAT_IND, &mal_rat_name, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("msg_p was NULL");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_RAT_NAME == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);

    rat_name_p = (cn_rat_name_t *)msg_p->payload;
    TC_ASSERT(CN_RAT_NAME_UMTS == *rat_name_p);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_rat_name_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_net_rat_name mal_rat_name = MAL_NET_RAT_NAME_GSM;
    cn_rat_name_t *rat_name_p = NULL;

    net_event_callback(MAL_NET_RAT_IND, &mal_rat_name, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    if (!msg_p) {
        CN_LOG_E("msg_p was NULL");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_RAT_NAME == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);

    rat_name_p = (cn_rat_name_t *)msg_p->payload;
    TC_ASSERT(CN_RAT_NAME_GSM == *rat_name_p);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_modem_power_off_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    result = cn_request_modem_power_off(g_context_p, (cn_client_tag_t)937);

    WAIT_FOR_SELECT_TO_FINISH();

    /* No parameter to check in the stub */

    net_event_callback(MAL_MCE_POWER_OFF_RESP, NULL, MAL_MCE_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_MODEM_POWER_OFF */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODEM_POWER_OFF == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(937 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_modem_power_off_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    result = cn_request_modem_power_off(g_context_p, (cn_client_tag_t)932);

    WAIT_FOR_SELECT_TO_FINISH();

    /* No parameter to check in the stub */

    net_event_callback(MAL_MCE_POWER_OFF_RESP, NULL, MAL_MCE_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_MODEM_POWER_OFF */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODEM_POWER_OFF == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(932 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_send_tx_back_off_event_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_tx_back_off_event_t event = CN_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_ACTIVE;

    result = cn_request_send_tx_back_off_event(g_context_p, event, (cn_client_tag_t)888);

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check parameter in the stub */
    TC_ASSERT(MAL_RF_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_ACTIVE == g_mal_rf_send_tx_back_off_event);

    net_event_callback(MAL_RF_SEND_TX_BACK_OFF_EVENT_RESP, NULL, MAL_RF_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SEND_TX_BACK_OFF_EVENT */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SEND_TX_BACK_OFF_EVENT == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(888 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_send_tx_back_off_event_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_tx_back_off_event_t event = CN_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_NEAR;

    result = cn_request_send_tx_back_off_event(g_context_p, event, (cn_client_tag_t)889);

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check parameter in the stub */
    TC_ASSERT(MAL_RF_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_NEAR == g_mal_rf_send_tx_back_off_event);

    net_event_callback(MAL_RF_SEND_TX_BACK_OFF_EVENT_RESP, NULL, MAL_RF_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SEND_TX_BACK_OFF_EVENT */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SEND_TX_BACK_OFF_EVENT == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(889 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_send_tx_back_off_event_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_tx_back_off_event_t event = CN_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_NEAR;

    result = cn_request_send_tx_back_off_event(g_context_p, event, (cn_client_tag_t)889);

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check parameter in the stub */
    TC_ASSERT(MAL_RF_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_NEAR == g_mal_rf_send_tx_back_off_event);

    net_event_callback(MAL_RF_SEND_TX_BACK_OFF_EVENT_RESP, NULL, MAL_RF_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SEND_TX_BACK_OFF_EVENT */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SEND_TX_BACK_OFF_EVENT == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(889 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

static mal_call_emergency_number_list *cn_test_get_mal_emergency_number_list(cn_uint8_t entries);
static cn_emergency_number_list_internal_t *cn_test_get_cn_emergency_number_list(cn_uint8_t entries);
static tc_result_t cn_enl_populate_emergency_number_list(cn_emergency_number_list_internal_t *list_p);
static tc_result_t cn_enl_validate_emergency_number_list(char *expected_p[]);

#define CN_GET_MAL_EMERGENCY_NUMBER_LIST(list_p, entries) \
    do { \
        list_p = cn_test_get_mal_emergency_number_list(entries); \
        if (NULL == list_p) { \
            CN_LOG_E("could not get mal emergency number list"); \
            SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED); \
        } \
    } while (0);

#define CN_GET_CN_EMERGENCY_NUMBER_LIST(list_p, entries) \
    do { \
        list_p = cn_test_get_cn_emergency_number_list(entries); \
        if (NULL == list_p) { \
            CN_LOG_E("could not get cn emergency number list"); \
            SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED); \
        } \
    } while (0);

#define CN_STORE_EMERGENCY_NUMBER_LIST(list_p) \
    do { \
        if (TC_RESULT_OK != cn_enl_populate_emergency_number_list(list_p)) { \
            CN_LOG_E("could not populate list"); \
            SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED); \
        } \
     } while (0);

#define CN_FREE_AND_SET_TO_NULL(list_p) \
    do { \
        free(list_p); \
        list_p = NULL; \
    } while (0);

#define CN_VALIDATE_EMERGENCY_LIST(expected_list, expected_result) \
    do { \
        if (expected_result != cn_enl_validate_emergency_number_list(expected_list)) { \
            CN_LOG_E("emergency number list contains unexpected values"); \
            SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED); \
        } \
    } while (0);

static tc_result_t cn_enl_validate_emergency_number_list(char *expected_p[])
{
    FILE *file_p = NULL;
    uint8_t i, num_of_emergency_numbers = 0;
    char buffer[255];
    tc_result_t result = TC_RESULT_FAILED;

    if (NULL == expected_p) {
        CN_LOG_I("received null parameters");
        goto error;
    }

    file_p = fopen("/tmp/enl", "r");

    if (NULL == file_p) {
        CN_LOG_I("couldn't open:/tmp/enl");
        goto error;
    }

    /* compare number of emergency numbers */
    while (NULL != fgets(buffer, sizeof(buffer), file_p)) {
        num_of_emergency_numbers++;
    }

    for (i = 0; expected_p[i]; i++);

    if (num_of_emergency_numbers != i) {
        CN_LOG_E("mismatch detected: list contains %d entries, expected %d entries", num_of_emergency_numbers, i);
        goto error;
    }

    (void) fseek(file_p, 0L, SEEK_SET);

    /* compare emergency number entries */
    for (i = 0; NULL != fgets(buffer, sizeof(buffer), file_p); i++) {
        if (0 != strncmp(buffer, expected_p[i], 255)) {
            CN_LOG_E("mismatch detected: buffer:%s, expected:%s", buffer, expected_p[i]);
            goto error;
        }
    }

    result = TC_RESULT_OK;

error:

    if (NULL != file_p) {
        fclose(file_p);
    }

    return result;
}

static tc_result_t cn_enl_populate_emergency_number_list(cn_emergency_number_list_internal_t *list_p)
{
    FILE *file_p = NULL;
    uint8_t i;
    tc_result_t result = TC_RESULT_FAILED;

    if (NULL == list_p) {
        CN_LOG_I("received null parameters");
        goto error;
    }

    file_p = fopen("/tmp/enl", "w");

    if (NULL == file_p) {
        CN_LOG_I("couldn't open:/tmp/enl");
        goto error;
    }

    for (i = 0; i < list_p->num_of_emergency_numbers; i++) {
        char buffer[CN_ENL_BUFFER_SIZE] = { '\0' };

        cn_sint16_t n = snprintf(buffer, CN_ENL_BUFFER_SIZE,
                                 "%s;%d;%s;%s;%d\n",
                                 list_p->emergency_number[i].emergency_number,
                                 list_p->emergency_number[i].service_type,
                                 list_p->emergency_number[i].mcc,
                                 list_p->emergency_number[i].mcc_range,
                                 list_p->emergency_number[i].origin);

        if (n < 0) {
            CN_LOG_E("couldn't write config into buffer");
            goto error;
        } else if (n >= CN_ENL_BUFFER_SIZE) {
            CN_LOG_E("truncation occured");
            goto error;
        } else {
            if (1 != fwrite(buffer, n, 1, file_p)) {
                CN_LOG_E("fwrite failed");
            }
        }
    }

    result = TC_RESULT_OK;

error:

    if (NULL != file_p) {
        fclose(file_p);
    }

    return result;
}

static mal_call_emergency_number_list *cn_test_get_mal_emergency_number_list(cn_uint8_t entries)
{
    mal_call_emergency_number_list *mal_list_p = NULL;
    uint16_t n = 0;
    uint8_t i = 0;

    if (0 < entries) {
        n = sizeof(mal_call_emergency_number_list) + (entries - 1) * sizeof(mal_call_emergency_number_entry);
    } else {
        n = sizeof(mal_call_emergency_number_list); /* always one entry in list */
    }

    mal_list_p = (mal_call_emergency_number_list *) calloc(1, n);

    if (NULL != mal_list_p) {
        mal_list_p->num_of_numbers = entries;

        for (i = 0; i < mal_list_p->num_of_numbers; i++) {
            mal_list_p->emergency_numbers[i].num_len = strlen("112");
            (void) strcpy((char *) mal_list_p->emergency_numbers[i].emergency_num, "112");
            mal_list_p->emergency_numbers[i].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK;
            mal_list_p->emergency_numbers[i].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_NONE;
        }
    }

    return mal_list_p;
}

static cn_emergency_number_list_internal_t *cn_test_get_cn_emergency_number_list(cn_uint8_t entries)
{
    cn_emergency_number_list_internal_t *cn_list_p = NULL;
    uint16_t n = 0;
    uint8_t i = 0;

    if (0 < entries) {
        n = sizeof(cn_emergency_number_list_internal_t) + (entries - 1) * sizeof(cn_emergency_number_list_internal_t);
    } else {
        n = sizeof(cn_emergency_number_list_internal_t); /* always one entry in list */
    }

    cn_list_p = (cn_emergency_number_list_internal_t *) calloc(1, n);

    if (NULL != cn_list_p) {
        cn_list_p->num_of_emergency_numbers = entries;

        for (i = 0; i < cn_list_p->num_of_emergency_numbers; i++) {
            (void) strcpy((char *) cn_list_p->emergency_number[i].emergency_number, "112");
            cn_list_p->emergency_number[i].service_type = CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED;
            (void) strcpy((char *) cn_list_p->emergency_number[i].mcc, "-");
            (void) strcpy((char *) cn_list_p->emergency_number[i].mcc_range, "-");
            cn_list_p->emergency_number[i].origin = CN_EMERGENCY_NUMBER_ORIGIN_NETWORK;
        }
    }

    return cn_list_p;
}

tc_result_t cn_event_enl_emergency_number_ind_empty_list(void)
{
    mal_net_registration_info mal_registration_info = { 0, MAL_NET_RAT_UNKNOWN, MAL_NET_SELECT_MODE_UNKNOWN, 0, 0, (uint8_t *) MCC_MNC_STRING, 0, 0 };
    mal_call_emergency_number_list *mal_list_p = NULL;
    tc_result_t tc_result = TC_RESULT_OK;

    char *expected_net_unspecified[] = { "112;0;405;-;4\n",
                                         NULL
                                       };

    char *expected_net_mountin_rescue[] = { "112;16;405;-;4\n",
                                            NULL
                                          };

    char *expected_net_n_services[] = { "112;0;405;-;4\n",
                                        "112;1;405;-;4\n",
                                        "112;2;405;-;4\n",
                                        "112;4;405;-;4\n",
                                        "112;8;405;-;4\n",
                                        "112;16;405;-;4\n",
                                        NULL
                                      };

    char *expected_net_first_two_duplicates[] = { "112;1;405;-;4\n",
            "112;2;405;-;4\n",
            "112;4;405;-;4\n",
            "112;8;405;-;4\n",
            "112;16;405;-;4\n",
            NULL
                                                };

    char *expected_net_last_two_duplicates[] = { "112;0;405;-;4\n",
            "112;1;405;-;4\n",
            "112;2;405;-;4\n",
            "112;4;405;-;4\n",
            "112;8;405;-;4\n",
            NULL
                                               };

    char *expected_net_some_duplicates[] = { "112;1;405;-;4\n",
                                           "112;4;405;-;4\n",
                                           "112;8;405;-;4\n",
                                           NULL
                                           };

    char *expected_net_all_duplicates[] = { "112;1;405;-;4\n",
                                            NULL
                                          };

    /* 1 number with service category unspecified. */
    unlink("/tmp/enl");
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 1);
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_unspecified, TC_RESULT_OK);

    /* 1 number with service category mountain rangers. */
    unlink("/tmp/enl");
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MOUNTAIN_RESCUE;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_mountin_rescue, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* n numbers with different service categories. */
    unlink("/tmp/enl");
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 6);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_NONE;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_AMBULANCE;
    mal_list_p->emergency_numbers[3].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[4].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    mal_list_p->emergency_numbers[5].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MOUNTAIN_RESCUE;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_n_services, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* n numbers, two first numbers are duplicates. */
    unlink("/tmp/enl");
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 6);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_AMBULANCE;
    mal_list_p->emergency_numbers[3].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[4].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    mal_list_p->emergency_numbers[5].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MOUNTAIN_RESCUE;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_first_two_duplicates, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* n numbers, two last numbers are duplicates. */
    unlink("/tmp/enl");
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 6);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_NONE;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_AMBULANCE;
    mal_list_p->emergency_numbers[3].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[4].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    mal_list_p->emergency_numbers[5].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_last_two_duplicates, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* n numbers, some of the numbers are duplicates. */
    unlink("/tmp/enl");
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 6);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[3].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[4].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    mal_list_p->emergency_numbers[5].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_some_duplicates, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* n numbers, all are duplicates. */
    unlink("/tmp/enl");
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 6);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[3].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[4].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[5].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_all_duplicates, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

exit:

    unlink("/tmp/enl");
    free(mal_list_p);

    return tc_result;
}

tc_result_t cn_event_enl_emergency_number_ind_populated_list(void)
{
    mal_net_registration_info mal_registration_info = { 0, MAL_NET_RAT_UNKNOWN, MAL_NET_SELECT_MODE_UNKNOWN, 0, 0, (uint8_t *) MCC_MNC_STRING, 0, 0 };
    tc_result_t tc_result = TC_RESULT_OK;
    cn_emergency_number_list_internal_t *cn_list_p = NULL;
    mal_call_emergency_number_list *mal_list_p = NULL;

    char *expected_net_unspecified[] = { "112;0;405;-;4\n",
                                         NULL
                                       };
    char *expected_net_unspecified_and_client[] = { "112;0;405;-;16\n",
            "112;0;405;-;4\n",
            NULL
                                                  };
    char *expected_net_fire_brigade_and_clients[] = { "112;0;405;-;16\n",
            "112;2;405;-;16\n",
            "112;8;405;-;16\n",
            "112;4;405;-;4\n",
            NULL
                                                    };
    char *expected_mixed_and_clients[] = { "112;0;405;-;16\n",
                                           "112;2;405;-;16\n",
                                           "112;8;405;-;16\n",
                                           "112;2;405;-;4\n",
                                           "112;16;405;-;4\n",
                                           NULL
                                         };
    char *expected_mixed_first_two_duplicates[] = { "112;0;405;-;16\n",
            "112;2;405;-;16\n",
            "112;8;405;-;16\n",
            "112;4;405;-;4\n",
            "112;0;405;-;4\n",
            NULL
                                                  };
    char *expected_mixed_last_two_duplicates[] = { "112;0;405;-;16\n",
            "112;2;405;-;16\n",
            "112;8;405;-;16\n",
            "112;4;405;-;4\n",
            "112;8;405;-;4\n",
            NULL
                                                 };
    char *expected_mixed_some_duplicates[] = { "112;0;405;-;16\n",
            "112;2;405;-;16\n",
            "112;8;405;-;16\n",
            "112;1;405;-;4\n",
            "112;4;405;-;4\n",
            "112;8;405;-;4\n",
            NULL
                                             };
    char *expected_mixed_all_duplicates[] = { "112;0;405;-;16\n",
                                            "112;2;405;-;16\n",
                                            "112;8;405;-;16\n",
                                            "112;1;405;-;4\n",
                                            NULL
                                            };

    /* 1 number with service category unspecified into list with non-client entry. */
    unlink("/tmp/enl");
    CN_GET_CN_EMERGENCY_NUMBER_LIST(cn_list_p, 1);
    cn_list_p->emergency_number[0].origin = CN_EMERGENCY_NUMBER_ORIGIN_MEMORY;
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    CN_FREE_AND_SET_TO_NULL(cn_list_p);

    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 1);
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_unspecified, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* 1 number with service category unspecified into list with client entry. */
    unlink("/tmp/enl");
    CN_GET_CN_EMERGENCY_NUMBER_LIST(cn_list_p, 1);
    cn_list_p->emergency_number[0].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "405");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    CN_FREE_AND_SET_TO_NULL(cn_list_p);

    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 1);
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_unspecified_and_client, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* 1 number with service category fire brigade into n list (mixed client and non-client). */
    unlink("/tmp/enl");
    CN_GET_CN_EMERGENCY_NUMBER_LIST(cn_list_p, 6);
    cn_list_p->emergency_number[0].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    cn_list_p->emergency_number[0].service_type = CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED;
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "405");
    cn_list_p->emergency_number[1].origin = CN_EMERGENCY_NUMBER_ORIGIN_SIM;
    cn_list_p->emergency_number[1].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[1].mcc, "405");
    cn_list_p->emergency_number[2].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    cn_list_p->emergency_number[2].service_type = CN_EMERGENCY_CALL_SERVICE_AMBULANCE;
    (void) strcpy(cn_list_p->emergency_number[2].mcc, "405");
    cn_list_p->emergency_number[3].origin = CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED;
    cn_list_p->emergency_number[3].service_type = CN_EMERGENCY_CALL_SERVICE_FIRE_BRIGADE;
    (void) strcpy(cn_list_p->emergency_number[3].mcc, "405");
    cn_list_p->emergency_number[4].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    cn_list_p->emergency_number[4].service_type = CN_EMERGENCY_CALL_SERVICE_MARINE_GUARD;
    (void) strcpy(cn_list_p->emergency_number[4].mcc, "405");
    cn_list_p->emergency_number[5].origin = CN_EMERGENCY_NUMBER_ORIGIN_MEMORY;
    cn_list_p->emergency_number[5].service_type = CN_EMERGENCY_CALL_SERVICE_MOUNTAIN_RESCUE;
    (void) strcpy(cn_list_p->emergency_number[5].mcc, "405");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 1);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[0].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_fire_brigade_and_clients, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* n number with mixed service categories into n list (mixed client and non-client). */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 6);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_NONE;
    mal_list_p->emergency_numbers[0].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_HARD_CODED;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[1].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_SIM;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_AMBULANCE;
    mal_list_p->emergency_numbers[2].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK;
    mal_list_p->emergency_numbers[3].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[3].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_PERMANENT_MEM;
    mal_list_p->emergency_numbers[4].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    mal_list_p->emergency_numbers[4].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_SIM;
    mal_list_p->emergency_numbers[5].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MOUNTAIN_RESCUE;
    mal_list_p->emergency_numbers[5].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_mixed_and_clients, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* n numbers, two first numbers are duplicates. */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 3);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_mixed_first_two_duplicates, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* n numbers, two last numbers are duplicates. */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 3);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_mixed_last_two_duplicates, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* n numbers, some of the numbers are duplicates. */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 6);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[3].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[4].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    mal_list_p->emergency_numbers[5].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_mixed_some_duplicates, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);

    /* n numbers, all are duplicates. */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 6);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[3].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[4].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[5].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_mixed_all_duplicates, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(mal_list_p);
    CN_FREE_AND_SET_TO_NULL(cn_list_p);

exit:
    unlink("/tmp/enl");
    free(cn_list_p);
    free(mal_list_p);
    return tc_result;
}

tc_result_t cn_event_enl_registration_status_ind_empty_list(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    mal_net_registration_info mal_registration_info = { (uint32_t) 1, MAL_NET_RAT_UMTS,
                              MAL_NET_SELECT_MODE_AUTOMATIC,
                              (uint32_t) 44, (uint16_t) 55,
                              NULL, 1, 0
                                                      };
    char *expected_empty_list[] = { NULL };

    memset(&mal_registration_info, 0, sizeof(mal_net_registration_info));

    /* 1. no mcc when there was no mcc set */
    unlink("/tmp/enl");
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *)&mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_FAILED);

    /* 2. new mcc when there was no mcc set */
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *)&mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_FAILED);

    /* 3. new mcc, but its the same as previously */
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *)&mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_FAILED);

    /* 4. new mcc, but its not the same as previously */
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *)&mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_FAILED);

    /* 5. mcc lost */
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *)&mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_FAILED);

exit:
    unlink("/tmp/enl");
    return tc_result;
}

tc_result_t cn_event_enl_registration_status_ind_populated_list(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    mal_net_registration_info mal_registration_info = { (uint32_t) 1, MAL_NET_RAT_UMTS,
                              MAL_NET_SELECT_MODE_AUTOMATIC,
                              (uint32_t) 44, (uint16_t) 55,
                              NULL, 1, 0
                                                      };
    cn_emergency_number_list_internal_t *cn_list_p = NULL;
    char *expected_empty_list[] = { NULL };
    char *expected_net_number[] = { "112;0;240;-;4\n",
                                    NULL
                                  };
    char *expected_client_number[] = { "112;0;405;-;16\n",
                                       NULL
                                     };
    char *expected_client_numbers[] = { "112;0;405;-;16\n",
                                        "112;1;405;-;16\n",
                                        NULL
                                      };
    char *expected_client_and_network_numbers_1[] = { "112;0;240;-;4\n",
            "112;0;405;-;16\n",
            "112;1;405;-;16\n",
            NULL
                                                    };
    char *expected_client_and_network_numbers_2[] = { "112;0;405;-;1\n",
            "112;0;405;-;2\n",
            "112;0;240;-;4\n",
            "112;0;405;-;8\n",
            "112;0;405;-;16\n",
            "112;0;405;-;32\n",
            "112;1;405;-;1\n",
            "112;1;405;-;2\n",
            "112;1;240;-;4\n",
            "112;1;405;-;16\n",
            NULL
                                                    };
    char *expected_client_and_network_numbers_3[] = { "112;0;405;-;16\n",
            "112;1;250;-;4\n",
            "112;1;405;-;16\n",
            NULL
                                                    };
    /*
     * NETWORK: verifies the emergency number list with a single network number entry
     * is correctly updated when the mcc changes. The emergency numbers received from
     * the network is to be updated when the mcc changes.
     */
    unlink("/tmp/enl");
    CN_GET_CN_EMERGENCY_NUMBER_LIST(cn_list_p, 1);
    cn_list_p->emergency_number[0].origin = CN_EMERGENCY_NUMBER_ORIGIN_NETWORK;
    cn_list_p->emergency_number[0].service_type = CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED;
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "405");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    /* 1. no mcc when there was no mcc set */
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_OK);

    /* 2. new mcc when there was no mcc set */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* network numbers with different mcc is not allowed */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_OK);

    /* 3. same mcc as previously set */
    unlink("/tmp/enl");
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "240");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_net_number, TC_RESULT_OK);

    /* 4. new mcc different from previously set */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* network numbers with different mcc are not allowed */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_OK);

    /* 5. lost mcc */
    unlink("/tmp/enl");
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "250");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* network number entry should be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_OK);

    /*
     * CLIENT: verifies the emergency number list with a single client defined number entry
     * is never updated when the mcc changes. It's the client's responsibility to update
     * the client defined emergency numbers whenever the mcc changes.
     *
     */
    unlink("/tmp/enl");
    cn_list_p->emergency_number[0].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    cn_list_p->emergency_number[0].service_type = CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED;
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "405");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    /* 1. no mcc when there was no mcc set */
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* client numbers should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_number, TC_RESULT_OK);

    /* 2. new mcc when there was no mcc set */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* client numbers should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_number, TC_RESULT_OK);

    /* 3. same mcc as previously set */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* client numbers should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_number, TC_RESULT_OK);

    /* 4. new mcc different from previously set */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* client numbers should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_number, TC_RESULT_OK);

    /* 5. lost mcc */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* client numbers should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_number, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(cn_list_p);

    /*
     * CLIENT-NETWORK-OTHERS: verifies the emergency number list with multiple emergency number
     * entries with different origins are correctly updated when the mcc changes. The only allowed
     * origins are client and network. Network emergency number entries are updated when the
     * mcc changes, but client defined emergency number entries are not.
     *
     */
    unlink("/tmp/enl");
    CN_GET_CN_EMERGENCY_NUMBER_LIST(cn_list_p, 10);
    cn_list_p->emergency_number[0].origin = CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED;
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "405");
    cn_list_p->emergency_number[1].origin = CN_EMERGENCY_NUMBER_ORIGIN_SIM;
    (void) strcpy(cn_list_p->emergency_number[1].mcc, "405");
    cn_list_p->emergency_number[2].origin = CN_EMERGENCY_NUMBER_ORIGIN_NETWORK;
    (void) strcpy(cn_list_p->emergency_number[2].mcc, "405");
    cn_list_p->emergency_number[3].origin = CN_EMERGENCY_NUMBER_ORIGIN_MEMORY;
    (void) strcpy(cn_list_p->emergency_number[3].mcc, "405");
    cn_list_p->emergency_number[4].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    (void) strcpy(cn_list_p->emergency_number[4].mcc, "405");
    cn_list_p->emergency_number[5].origin = CN_EMERGENCY_NUMBER_ORIGIN_UNSPECIFIED;
    (void) strcpy(cn_list_p->emergency_number[5].mcc, "405");
    cn_list_p->emergency_number[6].origin = CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED;
    cn_list_p->emergency_number[6].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[6].mcc, "405");
    cn_list_p->emergency_number[7].origin = CN_EMERGENCY_NUMBER_ORIGIN_SIM;
    cn_list_p->emergency_number[7].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[7].mcc, "405");
    cn_list_p->emergency_number[8].origin = CN_EMERGENCY_NUMBER_ORIGIN_NETWORK;
    cn_list_p->emergency_number[8].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[8].mcc, "405");
    cn_list_p->emergency_number[9].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    cn_list_p->emergency_number[9].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[9].mcc, "405");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    /* 1. no mcc when there was no mcc set */
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* client numbers should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_numbers, TC_RESULT_OK);

    /* 2. new mcc when there was no mcc set */
    unlink("/tmp/enl");
    (void) strcpy(cn_list_p->emergency_number[2].mcc, "240");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* network numbers with same mcc and client numbers should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_and_network_numbers_1, TC_RESULT_OK);

    /* 3. same mcc as previously set */
    unlink("/tmp/enl");
    (void) strcpy(cn_list_p->emergency_number[8].mcc, "240");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* list is only updated when mcc changes */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_and_network_numbers_2, TC_RESULT_OK);

    /* 4. new mcc different from previously set */
    unlink("/tmp/enl");
    (void) strcpy(cn_list_p->emergency_number[8].mcc, "250");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* network numbers with same mcc and client numbers should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_and_network_numbers_3, TC_RESULT_OK);

    /* 5. lost mcc */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* client numbers should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_numbers, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(cn_list_p);

exit:

    unlink("/tmp/enl");
    free(cn_list_p);

    return tc_result;
}

tc_result_t cn_event_enl_registration_status_response_empty_list(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result;
    mal_net_registration_info mal_registration_info = { MAL_NET_NOT_REG_SEARCHING_OP, MAL_NET_RAT_UMTS,
                              MAL_NET_SELECT_MODE_AUTOMATIC,
                              (uint32_t) 44, (uint16_t) 55,
                              NULL, 1, 0
                                                      };
    cn_message_t *msg_p = NULL;
    uint32_t size = 0;

    char *expected_empty_list[] = { NULL };

    /* 1. no mcc when there was no mcc set */
    unlink("/tmp/enl");
    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* should not be possible to open file! */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_FAILED);

    /* 2. new mcc when there was no mcc set */
    unlink("/tmp/enl");
    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* should not be possible to open file! */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_FAILED);

    /* 3. new mcc, but its the same as previously */
    unlink("/tmp/enl");
    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* should not be possible to open file! */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_FAILED);

    /* 4. new mcc, but its not the same as previously */
    unlink("/tmp/enl");
    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* should not be possible to open file! */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_FAILED);

    /* 5. mcc lost */
    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* should not be possible to open file! */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_FAILED);

exit:
    unlink("/tmp/enl");
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_enl_registration_status_response_populated_list(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result;
    mal_net_registration_info mal_registration_info = { MAL_NET_NOT_REG_SEARCHING_OP, MAL_NET_RAT_UMTS,
                              MAL_NET_SELECT_MODE_AUTOMATIC,
                              (uint32_t) 44, (uint16_t) 55,
                              NULL, 1, 0
                                                      };
    cn_message_t *msg_p = NULL;
    uint32_t size = 0;
    cn_emergency_number_list_internal_t *cn_list_p = NULL;
    char *expected_empty_list[] = { NULL };
    char *expected_network[] = { "112;0;250;-;4\n",
                                 NULL
                               };
    char *expected_client[] = { "112;0;405;-;16\n",
                                NULL
                              };
    char *expected_client_numbers[] = { "112;0;405;-;16\n",
                                        "112;1;405;-;16\n",
                                        NULL
                                      };
    char *expected_client_and_network_numbers_1[] = { "112;0;240;-;4\n",
            "112;0;405;-;16\n",
            "112;1;405;-;16\n",
            NULL
                                                    };
    char *expected_client_and_network_numbers_2[] = { "112;0;405;-;1\n",
            "112;0;405;-;2\n",
            "112;0;240;-;4\n",
            "112;0;405;-;8\n",
            "112;0;405;-;16\n",
            "112;0;405;-;32\n",
            "112;1;405;-;1\n",
            "112;1;405;-;2\n",
            "112;1;240;-;4\n",
            "112;1;405;-;16\n",
            NULL
                                                    };
    char *expected_client_and_network_numbers_3[] = { "112;0;405;-;16\n",
            "112;1;250;-;4\n",
            "112;1;405;-;16\n",
            NULL
                                                    };
    /*
     * NETWORK: verifies the emergency number list with a single network number entry
     * is correctly updated when the mcc changes. The emergency numbers received from
     * the network is to be updated when the mcc changes.
     */
    unlink("/tmp/enl");
    CN_GET_CN_EMERGENCY_NUMBER_LIST(cn_list_p, 1);
    cn_list_p->emergency_number[0].origin = CN_EMERGENCY_NUMBER_ORIGIN_NETWORK;
    cn_list_p->emergency_number[0].service_type = CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED;
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "405");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    /* 1. no mcc when there was no mcc set */
    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_OK);

    /* 2. new mcc when there was no mcc set */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* network numbers with different mcc is not allowed */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_OK);

    /* 3. new mcc, but its the same as previously */
    unlink("/tmp/enl");
    strcpy(cn_list_p->emergency_number[0].mcc, "250");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* mcc is the same as previously, nothing is updated */
    CN_VALIDATE_EMERGENCY_LIST(expected_network, TC_RESULT_OK);

    /* 4. new mcc, but its not the same as previously */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* mcc is the same as previously, nothing is updated */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_OK);

    /* 5. lost mcc */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* network number entry should be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_empty_list, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(cn_list_p);

    /*
     * CLIENT: verifies the emergency number list with a single client defined number entry
     * is never updated when the mcc changes. It's the client's responsibility to update
     * the client defined emergency numbers whenever the mcc changes.
     *
     */
    /* 1. no mcc when there was no mcc set */
    unlink("/tmp/enl");
    CN_GET_CN_EMERGENCY_NUMBER_LIST(cn_list_p, 1);
    cn_list_p->emergency_number[0].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    cn_list_p->emergency_number[0].service_type = CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED;
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "405");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* client number entry should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client, TC_RESULT_OK);

    /* 2. new mcc when there was no mcc set */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* client number entry should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client, TC_RESULT_OK);

    /* 3. new mcc, but its the same as previously */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* mcc is the same as previously, nothing is updated */
    CN_VALIDATE_EMERGENCY_LIST(expected_client, TC_RESULT_OK);

    /* 4. new mcc, but its not the same as previously */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* mcc is not the same as previously, client number entry should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client, TC_RESULT_OK);

    /* 5. lost mcc */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* client number entry should never be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client, TC_RESULT_OK);
    CN_FREE_AND_SET_TO_NULL(cn_list_p);

    /*
     * CLIENT-NETWORK-OTHERS: verifies the emergency number list with multiple emergency number
     * entries with different origins are correctly updated when the mcc changes. The only allowed
     * origins are client and network. Network emergency number entries are updated when the
     * mcc changes, but client defined emergency number entries are not.
     *
     */
    unlink("/tmp/enl");
    CN_GET_CN_EMERGENCY_NUMBER_LIST(cn_list_p, 10);
    cn_list_p->emergency_number[0].origin = CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED;
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "405");
    cn_list_p->emergency_number[1].origin = CN_EMERGENCY_NUMBER_ORIGIN_SIM;
    (void) strcpy(cn_list_p->emergency_number[1].mcc, "405");
    cn_list_p->emergency_number[2].origin = CN_EMERGENCY_NUMBER_ORIGIN_NETWORK;
    (void) strcpy(cn_list_p->emergency_number[2].mcc, "405");
    cn_list_p->emergency_number[3].origin = CN_EMERGENCY_NUMBER_ORIGIN_MEMORY;
    (void) strcpy(cn_list_p->emergency_number[3].mcc, "405");
    cn_list_p->emergency_number[4].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    (void) strcpy(cn_list_p->emergency_number[4].mcc, "405");
    cn_list_p->emergency_number[5].origin = CN_EMERGENCY_NUMBER_ORIGIN_UNSPECIFIED;
    (void) strcpy(cn_list_p->emergency_number[5].mcc, "405");
    cn_list_p->emergency_number[6].origin = CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED;
    cn_list_p->emergency_number[6].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[6].mcc, "405");
    cn_list_p->emergency_number[7].origin = CN_EMERGENCY_NUMBER_ORIGIN_SIM;
    cn_list_p->emergency_number[7].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[7].mcc, "405");
    cn_list_p->emergency_number[8].origin = CN_EMERGENCY_NUMBER_ORIGIN_NETWORK;
    cn_list_p->emergency_number[8].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[8].mcc, "405");
    cn_list_p->emergency_number[9].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    cn_list_p->emergency_number[9].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[9].mcc, "405");

    /* 1. no mcc when there was no mcc set */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* only client number entries should be kept */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_numbers, TC_RESULT_OK);

    /* 2. new mcc when there was no mcc set */
    unlink("/tmp/enl");
    (void) strcpy(cn_list_p->emergency_number[2].mcc, "240");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* network numbers with different mcc is not allowed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_and_network_numbers_1, TC_RESULT_OK);

    /* 3. new mcc, but its the same as previously */
    unlink("/tmp/enl");
    (void) strcpy(cn_list_p->emergency_number[2].mcc, "240");
    (void) strcpy(cn_list_p->emergency_number[8].mcc, "240");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "24099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* mcc is the same as previously, nothing is updated */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_and_network_numbers_2, TC_RESULT_OK);

    /* 4. mcc is not the same as previously, network emergency number entries with different mcc should be removed */
    unlink("/tmp/enl");
    (void) strcpy(cn_list_p->emergency_number[8].mcc, "250");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "25099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    /* mcc is the same as previously, nothing is updated */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_and_network_numbers_3, TC_RESULT_OK);

    /* 5. lost mcc */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);

    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);

    /* network number entries should be removed */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_numbers, TC_RESULT_OK);

exit:
    unlink("/tmp/enl");
    free(msg_p);
    free(cn_list_p);
    return tc_result;
}

tc_result_t cn_event_enl_mixed_ind_responses_and_indications_populated_list(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result;
    mal_net_registration_info mal_registration_info = { MAL_NET_NOT_REG_SEARCHING_OP, MAL_NET_RAT_UMTS,
                              MAL_NET_SELECT_MODE_AUTOMATIC,
                              (uint32_t) 44, (uint16_t) 55,
                              NULL, 1, 0
                                                      };
    cn_message_t *msg_p = NULL;
    uint32_t size = 0;
    cn_emergency_number_list_internal_t *cn_list_p = NULL;
    mal_call_emergency_number_list *mal_list_p = NULL;
    char *expected_client_numbers_1[] = { "112;0;405;-;16\n",
                                          "112;1;405;-;16\n",
                                          NULL
                                        };
    char *expected_client_numbers_2[] = { "112;0;405;-;16\n",
                                          "112;1;260;-;4\n",
                                          "112;1;405;-;16\n",
                                          NULL
                                        };
    char *expected_networks_and_clients_1[] = { "112;0;405;-;16\n",
            "112;1;405;-;16\n",
            "112;2;260;-;4\n",
            "112;16;260;-;4\n",
            NULL
                                              };
    char *expected_networks_and_clients_2[] = { "112;0;405;-;16\n",
            "112;1;405;-;16\n",
            "112;1;270;-;4\n",
            "112;8;270;-;4\n",
            NULL
                                              };

    CN_GET_CN_EMERGENCY_NUMBER_LIST(cn_list_p, 10);
    cn_list_p->emergency_number[0].origin = CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED;
    (void) strcpy(cn_list_p->emergency_number[0].mcc, "405");
    cn_list_p->emergency_number[1].origin = CN_EMERGENCY_NUMBER_ORIGIN_SIM;
    (void) strcpy(cn_list_p->emergency_number[1].mcc, "405");
    cn_list_p->emergency_number[2].origin = CN_EMERGENCY_NUMBER_ORIGIN_NETWORK;
    (void) strcpy(cn_list_p->emergency_number[2].mcc, "405");
    cn_list_p->emergency_number[3].origin = CN_EMERGENCY_NUMBER_ORIGIN_MEMORY;
    (void) strcpy(cn_list_p->emergency_number[3].mcc, "405");
    cn_list_p->emergency_number[4].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    (void) strcpy(cn_list_p->emergency_number[4].mcc, "405");
    cn_list_p->emergency_number[5].origin = CN_EMERGENCY_NUMBER_ORIGIN_UNSPECIFIED;
    (void) strcpy(cn_list_p->emergency_number[5].mcc, "405");
    cn_list_p->emergency_number[6].origin = CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED;
    cn_list_p->emergency_number[6].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[6].mcc, "405");
    cn_list_p->emergency_number[7].origin = CN_EMERGENCY_NUMBER_ORIGIN_SIM;
    cn_list_p->emergency_number[7].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[7].mcc, "405");
    cn_list_p->emergency_number[8].origin = CN_EMERGENCY_NUMBER_ORIGIN_NETWORK;
    cn_list_p->emergency_number[8].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[8].mcc, "260");
    cn_list_p->emergency_number[9].origin = CN_EMERGENCY_NUMBER_ORIGIN_CLIENT;
    cn_list_p->emergency_number[9].service_type = CN_EMERGENCY_CALL_SERVICE_POLICE;
    (void) strcpy(cn_list_p->emergency_number[9].mcc, "405");

    /* 1. there is no mcc */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    /* only client numbers allowed when there is no mcc */
    CN_VALIDATE_EMERGENCY_LIST(expected_client_numbers_1, TC_RESULT_OK);

    /* 2. new mcc received from registration ind */
    unlink("/tmp/enl");
    CN_STORE_EMERGENCY_NUMBER_LIST(cn_list_p);
    mal_registration_info.mcc_mnc_string = (uint8_t *) "26099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_client_numbers_2, TC_RESULT_OK);

    /* 3. emergency number indication received */
    CN_GET_MAL_EMERGENCY_NUMBER_LIST(mal_list_p, 6);
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_NONE;
    mal_list_p->emergency_numbers[0].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_HARD_CODED;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[1].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_SIM;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_AMBULANCE;
    mal_list_p->emergency_numbers[2].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK;
    mal_list_p->emergency_numbers[3].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[3].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_PERMANENT_MEM;
    mal_list_p->emergency_numbers[4].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    mal_list_p->emergency_numbers[4].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_SIM;
    mal_list_p->emergency_numbers[5].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MOUNTAIN_RESCUE;
    mal_list_p->emergency_numbers[5].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_networks_and_clients_1, TC_RESULT_OK);

    /* 4. unchanged mcc received from registration response */
    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "26099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_networks_and_clients_1, TC_RESULT_OK);

    /* 5. unchanged mcc received from registration indication */
    mal_registration_info.mcc_mnc_string = (uint8_t *) "26099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_networks_and_clients_1, TC_RESULT_OK);

    /* 6. new emergency number indication received (new mcc) */
    mal_registration_info.mcc_mnc_string = (uint8_t *) "27099";
    mal_list_p->emergency_numbers[0].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_NONE;
    mal_list_p->emergency_numbers[0].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_HARD_CODED;
    mal_list_p->emergency_numbers[1].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_POLICE;
    mal_list_p->emergency_numbers[1].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK;
    mal_list_p->emergency_numbers[2].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_AMBULANCE;
    mal_list_p->emergency_numbers[2].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_SIM;
    mal_list_p->emergency_numbers[3].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE;
    mal_list_p->emergency_numbers[3].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_PERMANENT_MEM;
    mal_list_p->emergency_numbers[4].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MARINE_GUARD;
    mal_list_p->emergency_numbers[4].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_NETWORK;
    mal_list_p->emergency_numbers[5].srvc_cat = MAL_CALL_MODEM_SERVICE_CAT_MOUNTAIN_RESCUE;
    mal_list_p->emergency_numbers[5].emer_num_origin = MAL_CALL_MODEM_EMERG_NBR_ORIG_HARD_CODED;
    call_event_callback(MAL_CALL_MODEM_EMERG_NBR_IND, mal_list_p, MAL_SUCCESS, 0);
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_networks_and_clients_2, TC_RESULT_OK);

    /* 7. mcc received from registration indication */
    mal_registration_info.mcc_mnc_string = (uint8_t *) "27099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_networks_and_clients_2, TC_RESULT_OK);

    /* 8. mcc received from registration response */
    result = cn_request_registration_state_normal(g_context_p, (cn_client_tag_t) 113);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_registration_state_normal failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mal_registration_info.mcc_mnc_string = (uint8_t *) "27099";
    net_event_callback(MAL_NET_MODEM_REG_STATUS_GET_RESP, &mal_registration_info, MAL_NET_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    CN_FREE_AND_SET_TO_NULL(msg_p);
    CN_VALIDATE_EMERGENCY_LIST(expected_networks_and_clients_2, TC_RESULT_OK);

    /* 9. mcc lost */
    mal_registration_info.mcc_mnc_string = NULL;
    net_event_callback(MAL_NET_MODEM_REG_STATUS_IND, (void *) &mal_registration_info, MAL_NET_SUCCESS, NULL);
    CN_VALIDATE_EMERGENCY_LIST(expected_client_numbers_1, TC_RESULT_OK);

exit:
    unlink("/tmp/enl");
    free(msg_p);
    free(cn_list_p);
    free(mal_list_p);
    return tc_result;
}

/* =============================
 * SIM RAT CONTROL TESTCASES
 * =============================
 */

tc_result_t rat_test_indication()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Generate STE_CAT_CAUSE_RAT_SETTING_UPDATED event */
    sim_client_callback(STE_CAT_CAUSE_RAT_SETTING_UPDATED, (uintptr_t)g_sim_client_tag_p, NULL, NULL);

    /* Generate callback from ste_uicc_sim_read_preferred_RAT_setting */
    ste_uicc_sim_read_preferred_RAT_setting_response_t  rat_result;
    rat_result.RAT = STE_UICC_SIM_RAT_SETTING_3G_ONLY;
    rat_result.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING, (uintptr_t)g_sim_client_tag_p, &rat_result, NULL);

    /* MAL parameter check in the stub */
    TC_ASSERT(MAL_GSS_WCDMA_ONLY == g_mal_gss_set_preferred_network_type__type);

    /* Generate fake MAL response */
    gss_event_callback(MAL_GSS_SET_PREFERRED_NETWORK_TYPE_RESP, NULL, MAL_GSS_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);

exit:
    free(msg_p);
    return tc_result;
}

/* The CNS implementation has a SIM RAT state associated with the SIM_STATE_CHANGED event. Depending on the state, an additional
 * request of type set_preferred_network_type will be sent. This will happen only on the first SIM_STATE_CHANGED event. To make sure
 * that this won't break other test cases, this test case is executed as the first of all in the test case table.
 */
tc_result_t rat_test_sim_state_changed()
{
    tc_result_t tc_result = TC_RESULT_OK;

    /* Generate SIM_STATE_PUK_NEEDED (which will trigger update_rat_from_sim) */
    ste_uicc_sim_state_changed_t uicc_sim_state_changed = { .state = SIM_STATE_PUK_NEEDED };
    sim_client_callback(STE_UICC_CAUSE_SIM_STATE_CHANGED, (uintptr_t)NULL, &uicc_sim_state_changed, NULL);

    /* Generate callback from ste_uicc_sim_read_preferred_RAT_setting */
    ste_uicc_sim_read_preferred_RAT_setting_response_t  rat_result;
    rat_result.RAT = STE_UICC_SIM_RAT_SETTING_3G_ONLY;
    rat_result.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING, (uintptr_t)g_sim_client_tag_p, &rat_result, NULL);

    /* Generate fake MAL response */
    gss_event_callback(MAL_GSS_SET_PREFERRED_NETWORK_TYPE_RESP, NULL, MAL_GSS_SUCCESS, g_mal_client_tag_p);

    return tc_result;
}

/* FT - Field Test specific tests */
tc_result_t cn_request_ftd_common(cn_client_tag_t ct, cn_empage_t *empage, cn_error_code_t cn_error)
{
    tc_result_t         tc_result  = TC_RESULT_OK;
    cn_error_code_t     result     = 0;
    cn_uint32_t         size       = 0;
    cn_message_t       *msg_p      = NULL;

    /* Send CN_REQUEST_ */
    result = cn_request_set_empage(g_context_p, empage , ct);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_empage failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_EMPAGE */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_EMPAGE == msg_p->type);
    TC_ASSERT(cn_error               == msg_p->error_code);
    TC_ASSERT(ct                     == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_ftd_oneshot(void)
{
    cn_client_tag_t ct     = 500;
    cn_empage_t     empage = { 0, 1 , 10 };

    mal_ftd_isOneshot = true; /* We expect an measurement URC later */

    return (cn_request_ftd_common(ct, &empage, CN_SUCCESS));
}

tc_result_t cn_request_ftd_start_periodic1(void)
{
    cn_client_tag_t ct     = 501;
    cn_empage_t     empage = { 1, 1 , 10 };

    mal_ftd_isOneshot = false; /* We do not expect an measurement URC later */

    return (cn_request_ftd_common(ct, &empage, CN_SUCCESS));
}

tc_result_t cn_request_ftd_start_periodic2(void)
{
    cn_client_tag_t ct     = 502;
    cn_empage_t     empage = { 1, 2 , 10 };

    mal_ftd_isOneshot = false; /* We do not expect an measurement URC later */

    return (cn_request_ftd_common(ct, &empage, CN_SUCCESS));
}

tc_result_t cn_request_ftd_stop_periodic(void)
{
    cn_client_tag_t ct     = 503;
    cn_empage_t     empage = { 2, 1 , 10 };  /* Page is not applicable , upper layer would ensure 1 is set */

    return (cn_request_ftd_common(ct, &empage, CN_SUCCESS));
}

tc_result_t cn_request_ftd_reset(void)
{
    cn_client_tag_t ct     = 504;
    cn_empage_t     empage = { 3, 1 , 10 }; /* Page is not applicable , upper layer would ensure 1 is set */

    return (cn_request_ftd_common(ct, &empage, CN_SUCCESS));
}

tc_result_t cn_request_ftd_start_periodic_not_finish()
{
    cn_client_tag_t ct     = 505;
    cn_empage_t     empage = { 1, 1 , 10 };

    mal_ftd_isOneshot = false; /* We do not expect an URC later */

    return (cn_request_ftd_common(ct, &empage, CN_SUCCESS));
}

tc_result_t cn_request_ftd_stop_periodic_err()
{
    cn_client_tag_t     ct         = 1500;
    cn_empage_t         empage     = { 2, 1 , 10 };  /* Page is not applicable , upper layer would ensure 1 is set */

    return (cn_request_ftd_common(ct, &empage, CN_FAILURE));
}

tc_result_t cn_request_ftd_stop_periodic_not_finish()
{
    cn_client_tag_t ct     = 506;
    cn_empage_t     empage = { 2, 1 , 10 };          /* Page is not applicable , upper layer would ensure 1 is set */

    return (cn_request_ftd_common(ct, &empage, CN_SUCCESS));
}

tc_result_t cn_request_ftd_start_periodic_err()
{
    cn_client_tag_t     ct         = 1501;
    cn_empage_t         empage     = { 1, 3 , 10 };

    return (cn_request_ftd_common(ct, &empage, CN_FAILURE));
}
/* NOTE - A table decides in a sequence which values to simulate */
tc_result_t cn_event_ftd_issue_measurements()
{
    tc_result_t     tc_result   = TC_RESULT_OK;
    cn_error_code_t result      = 0;
    cn_uint32_t     size        = 0;
    cn_message_t   *msg_p       = NULL;
    view_details_t   *mal_info_p  = NULL;

    mal_info_p = &MAL_FTD_MEASUREMENTREPORTS[mal_ftd_event_index];

    /* Generate fake MAL event , use prepared table */
    ftd_event_callback(MAL_FTD_MEASUREMENT_IND, mal_info_p, MAL_FTD_SUCCESS, NULL);

    /* In case ONE SHOT is active we should expect an URC */
    if (mal_ftd_isOneshot) {
        WAIT_FOR_MESSAGE(g_event_fd);

        /* Receive CN_EVENT_EMPAGE_INFO */
        result = cn_message_receive(g_event_fd, &size, &msg_p);

        if (CN_SUCCESS != result) {
            CN_LOG_E("cn_message_receive failed!");
            SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
        }

        TC_ASSERT(CN_EVENT_EMPAGE_INFO == msg_p->type);
        TC_ASSERT(CN_SUCCESS           == msg_p->error_code);
        TC_ASSERT(0                    == msg_p->client_tag);
        TC_ASSERT(NULL                 != msg_p->payload);
    }

exit:

    /* Step up index to use into prepared table with values for next caller */
    if (++mal_ftd_event_index >= NO_OF_MAL_FTD_MEASUREMENTREPORTS) {
        mal_ftd_event_index = 0;
    }

    free(msg_p);
    return tc_result;
}

/* NOTE - A table decides in a sequence which values to simulate */
tc_result_t cn_event_ftd_issue_error_measurements()
{
    tc_result_t     tc_result   = TC_RESULT_OK;
    cn_error_code_t result      = 0;
    cn_uint32_t     size        = 0;
    cn_message_t   *msg_p       = NULL;
    view_details_t   *mal_info_p  = NULL;

    mal_info_p = &MAL_FTD_MEASUREMENTREPORTS[mal_ftd_event_index];

    /* Generate fake MAL event , use prepared table */
    ftd_event_callback(MAL_FTD_ERROR_IND, mal_info_p, MAL_FTD_FAIL, NULL);

    /* In case ONE SHOT is active we should expect an URC */
    if (mal_ftd_isOneshot) {
        WAIT_FOR_MESSAGE(g_event_fd);

        /* Receive CN_EVENT_EMPAGE_INFO */
        result = cn_message_receive(g_event_fd, &size, &msg_p);

        if (CN_FAILURE != result) {
            CN_LOG_E("cn_message_receive failed!");
            SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_OK);
        }

        TC_ASSERT(CN_EVENT_EMPAGE_INFO == msg_p->type);
        TC_ASSERT(CN_SUCCESS           == msg_p->error_code);
        TC_ASSERT(0                    == msg_p->client_tag);
        TC_ASSERT(NULL                 != msg_p->payload);
    }

exit:

    /* Step up index to use into prepared table with values for next caller */
    if (++mal_ftd_event_index >= NO_OF_MAL_FTD_MEASUREMENTREPORTS) {
        mal_ftd_event_index = 0;
    }

    free(msg_p);
    return tc_result;
}



tc_result_t cn_event_ftd_issue_empage_common(int32_t message_id, mal_ftd_error_type mal_error, cn_error_code_t cn_error)
{
    tc_result_t     tc_result   = TC_RESULT_OK;
    cn_error_code_t result      = 0;
    cn_uint32_t     size        = 0;
    cn_message_t   *msg_p       = NULL;

    /* Generate fake MAL event , use prepared table */
    ftd_event_callback(message_id, NULL, mal_error, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive CN_EVENT_EMPAGE_INFO */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_EMPAGE_INFO == msg_p->type);
    TC_ASSERT(cn_error             == msg_p->error_code);
    TC_ASSERT(0                    == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_ftd_issue_empage_activate_failure(void)
{
    return (cn_event_ftd_issue_empage_common(MAL_FTD_ACT_IND, MAL_FTD_FAIL, CN_SUCCESS));
}

tc_result_t cn_event_ftd_issue_empage_deactivate_success(void)
{
    return (cn_event_ftd_issue_empage_common(MAL_FTD_DEACT_IND, MAL_FTD_SUCCESS, CN_SUCCESS));
}

tc_result_t cn_event_ftd_issue_empage_deactivate_failure(void)
{
    return (cn_event_ftd_issue_empage_common(MAL_FTD_DEACT_IND, MAL_FTD_FAIL, CN_SUCCESS));
}

tc_result_t set_default_non_volatile_modem_data_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    int i = 0;

    /* cleanup */
    (void)unlink(NVD_INDICATOR_PATH);

    CN_LOG_D("********** FIRST STARTUP AFTER FLASH (invoke MAL) *************");
    set_default_non_volatile_modem_data(); /* does not involve select() */

    TC_ASSERT(TRUE == g_mal_nvd_set_default_called);
    g_mal_nvd_set_default_called = FALSE;

    nvd_event_callback(MAL_NVD_SET_DEFAULT_RESP, NULL, MAL_NVD_SUCCESS, g_mal_client_tag_p);

    for (i = 0; i < 9; i++) {
        CN_LOG_D("********* STARTUP #%d (DO NOTHING) *************", i + 2);
        set_default_non_volatile_modem_data(); /* does not involve select() */
        TC_ASSERT(FALSE == g_mal_nvd_set_default_called);
    }

exit:
    return tc_result;
}

tc_result_t cn_event_ring_waiting_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    char *number_p = "12345";
    uint16_t name_str[] = { 'n', 'a', 'm', 'e' };
    cn_message_t *msg_p = NULL;
    mal_call_context mal_call_context = {
        .callState = CALL_WAITING,
        .callId = (int32_t) 1,
        .addrType = (int32_t) 1,
        .message_type = (uint8_t) 2,
        .message_direction = (uint8_t) 0,
        .transaction_id = (uint8_t) 0,
        .isMpty = (char) 1,
        .isMT = (char) 0,
        .als = (char) 1,
        .call_mode = MAL_CALL_MODE_SPEECH,
        .isVoicePrivacy = (char) 1,
        .numberPresentation = CN_USER_SCREENED_PASSED | CN_PRESENTATION_RESTRICTED,
        .number = number_p,
        .call_cli_cause = (char) 2,
        .namePresentation = CN_USER_SCREENED_PASSED | CN_PRESENTATION_RESTRICTED,
        .name_length = sizeof(name_str) / sizeof(uint16_t),
        .name = name_str,
        .connectedLine = { 0, 0, NULL, 0, NULL },
        .call_cause = { 0, 0, 0, NULL }
    };
    cn_call_context_t *call_context_p = NULL;

    call_event_callback(MAL_CALL_RING_WAITING, (void *) &mal_call_context, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_CALL_RING */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_RING_WAITING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    call_context_p = (cn_call_context_t *) msg_p->payload;

    TC_ASSERT(mal_call_context.callState == call_context_p->call_state);
    TC_ASSERT(mal_call_context.callId == call_context_p->call_id);
    TC_ASSERT(mal_call_context.addrType == call_context_p->address_type);
    TC_ASSERT(mal_call_context.message_type == call_context_p->message_type);
    TC_ASSERT(mal_call_context.message_direction == call_context_p->message_direction);
    TC_ASSERT(mal_call_context.transaction_id == call_context_p->transaction_id);
    TC_ASSERT(mal_call_context.isMpty == call_context_p->is_multiparty);
    TC_ASSERT(mal_call_context.isMT == call_context_p->is_MT);
    TC_ASSERT(mal_call_context.call_mode == MAL_CALL_MODE_SPEECH);
    TC_ASSERT(CN_CALL_MODE_SPEECH == call_context_p->mode);
    TC_ASSERT(mal_call_context.isVoicePrivacy == call_context_p->is_voice_privacy);
    TC_ASSERT(0 == strcmp(mal_call_context.number, call_context_p->number));
    TC_ASSERT((CN_USER_SCREENED_PASSED | CN_PRESENTATION_RESTRICTED) == call_context_p->number_presentation);
    TC_ASSERT((cn_cause_no_cli_t) mal_call_context.call_cli_cause == call_context_p->cause_no_cli);
    TC_ASSERT(0 == memcmp(mal_call_context.name, call_context_p->name, sizeof(name_str)));
    TC_ASSERT(mal_call_context.name_length == call_context_p->name_char_length);
    TC_ASSERT((CN_USER_SCREENED_PASSED | CN_PRESENTATION_RESTRICTED) == call_context_p->name_presentation);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_request_rf_on_negative_3() /* Negative scenario: MAL failure code in response data and in callback */
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_mce_status mce_status;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RF_ON */
    result = cn_request_rf_on(g_context_p, (cn_client_tag_t) 0);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rf_on failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mce_status = MAL_MCE_OK;
    mce_event_callback(MAL_MCE_RF_STATE_RESP, &mce_status, MAL_MCE_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RF_ON */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_RF_ON == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(CN_MODEM_STATUS_OK == *((cn_modem_status_t *)msg_p->payload));

exit:
    free(msg_p);
    return tc_result;
}


//#if 0
tc_result_t cn_event_supp_svc_notifn_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    char *number_p = NULL;


    mal_supp_svc_notification mal_supp_svc_notification_p = {

        .code = (int32_t)1,
        .index = (int32_t)1,
        .notificationType = (int32_t)1,
        .number = number_p,
        .type = (int32_t)0
    };

    cn_supp_svc_notification_t *cn_supp_svc_notif_p = NULL;

    call_event_callback(MAL_CALL_SUPP_SVC_NOTIFICATION, (void *) &mal_supp_svc_notification_p, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_CALL_SUPP_SVC_NOTIFICATION == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_supp_svc_notif_p = (cn_supp_svc_notification_t *)msg_p->payload;

    TC_ASSERT(mal_supp_svc_notification_p.code == cn_supp_svc_notif_p->code);
    TC_ASSERT(mal_supp_svc_notification_p.index == cn_supp_svc_notif_p->index);
    TC_ASSERT(mal_supp_svc_notification_p.notificationType == cn_supp_svc_notif_p->notification_type);
    //TC_ASSERT(mal_supp_svc_notification_p.number == cn_supp_svc_notif_p->number);
    TC_ASSERT(mal_supp_svc_notification_p.type == cn_supp_svc_notif_p->address_type);

exit:
    free(msg_p);
    return tc_result;
}
//#endif
tc_result_t cn_event_supp_svc_notifn_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    char *number_p = "1";


    mal_supp_svc_notification mal_supp_svc_notification_p = {

        .code = (int32_t)1,
        .index = (int32_t)1,
        .notificationType = (int32_t)1,
        .number = number_p,
        .type = (int32_t)0
    };

    cn_supp_svc_notification_t *cn_supp_svc_notif_p = NULL;

    call_event_callback(MAL_CALL_SUPP_SVC_NOTIFICATION, (void *) &mal_supp_svc_notification_p, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        //("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_CALL_SUPP_SVC_NOTIFICATION == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_supp_svc_notif_p = (cn_supp_svc_notification_t *)msg_p->payload;

    TC_ASSERT(mal_supp_svc_notification_p.code == cn_supp_svc_notif_p->code);
    TC_ASSERT(mal_supp_svc_notification_p.index == cn_supp_svc_notif_p->index);
    TC_ASSERT(mal_supp_svc_notification_p.notificationType == cn_supp_svc_notif_p->notification_type);
    TC_ASSERT(mal_supp_svc_notification_p.number[0] == cn_supp_svc_notif_p->number[0]);
    TC_ASSERT(mal_supp_svc_notification_p.number[1] == cn_supp_svc_notif_p->number[1]);
    TC_ASSERT(mal_supp_svc_notification_p.type == cn_supp_svc_notif_p->address_type);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_signal_info_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_signal_info_t *signal_info_p = NULL;


    mal_net_signal_info mal_signal_info_p = {

        .ber = (uint8_t)6,
        .border[0] = (uint8_t)113,
        .border[1] = (uint8_t)103,
        .border[2] = (uint8_t)93,
        .border[3] = (uint8_t)83,
        .rssi_dbm = (uint8_t)1,
        .no_of_signal_bars = (uint8_t)1,
        .num_of_segments = 3,
        .ecno = (uint8_t)10,
        .rat = (uint8_t)MAL_NET_CELL_RAT_WCDMA

    };
    memset(&mal_signal_info_p, 0, sizeof(mal_signal_info_p));

    net_event_callback(MAL_NET_RSSI_IND, &mal_signal_info_p, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_SIGNAL_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    signal_info_p = (cn_signal_info_t *)msg_p->payload;
    TC_ASSERT(mal_signal_info_p.ber == signal_info_p->ber);
    TC_ASSERT(mal_signal_info_p.border[0] == signal_info_p->border[0]);
    TC_ASSERT(mal_signal_info_p.border[1] == signal_info_p->border[1]);
    TC_ASSERT(mal_signal_info_p.border[2] == signal_info_p->border[2]);
    TC_ASSERT(mal_signal_info_p.border[3] == signal_info_p->border[3]);
    TC_ASSERT(mal_signal_info_p.no_of_signal_bars == signal_info_p->no_of_signal_bars);
    TC_ASSERT(mal_signal_info_p.num_of_segments == signal_info_p->num_of_segments);
    TC_ASSERT(mal_signal_info_p.ecno == signal_info_p->ecno);
    //TC_ASSERT(CN_CELL_RAT_WCDMA == signal_info_p->rat);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_ussd_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ussd_info_t *ussd_info_p = NULL;
    mal_ss_ussd_info ussd_info;

    ussd_info.ussd_type_code = 1;
    ussd_info.ussd_received_type = MAL_SS_GSM_USSD_REQUEST;
    ussd_info.ussd_data.length = strlen("Message");
    ussd_info.ussd_data.dcs = 3;
    ussd_info.ussd_data.ussd_str = (uint8_t *) "Message"; /* Not zero-terminated in live setting, doesn't matter for the module tests */
    ussd_info.ussd_session_id = (void *) 1; /* MT-Null and MO-ptr to client tag */
    ussd_info.is_ussd_ue_terminated = 1;  /* Indicates whether ussd is terminated by UE-1/Nw-0 */
    ss_event_callback(MAL_SS_ON_USSD, (void *)&ussd_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_STATUS_IND MAL_SS_GSM_STATUS_REQUEST_USSD_STOP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_USSD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    ussd_info_p = (cn_ussd_info_t *)msg_p->payload;

    TC_ASSERT(ussd_info_p->type == 1);
    TC_ASSERT(ussd_info_p->received_type == CN_USSD_RECEIVED_TYPE_GSM_USSD_REQUEST);
    TC_ASSERT(ussd_info_p->length == strlen("Message")); /* Do not check the NULL terminator since it's not included. */
    TC_ASSERT(ussd_info_p->dcs == 3);
    TC_ASSERT(ussd_info_p->session_id == 1);
    TC_ASSERT(ussd_info_p->me_initiated == 1);
    TC_ASSERT(memcmp(ussd_info_p->ussd_string, "Message", strlen("Message")) == 0);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_event_ussd_positive_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ussd_info_t *ussd_info_p = NULL;
    mal_ss_ussd_info ussd_info;

    ussd_info.ussd_type_code = 0;
    ussd_info.ussd_received_type = MAL_SS_GSM_USSD_NOTIFY;
    ussd_info.ussd_data.length = strlen("Message");
    ussd_info.ussd_data.dcs = 3;
    ussd_info.ussd_data.ussd_str = (uint8_t *) "Message"; /* Not zero-terminated in live setting, doesn't matter for the module tests */
    ussd_info.ussd_session_id = (void *) 1; /* MT-Null and MO-ptr to client tag */
    ussd_info.is_ussd_ue_terminated = 1;  /* Indicates whether ussd is terminated by UE-1/Nw-0 */
    ss_event_callback(MAL_SS_ON_USSD, (void *)&ussd_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_STATUS_IND MAL_SS_GSM_STATUS_REQUEST_USSD_STOP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_USSD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    ussd_info_p = (cn_ussd_info_t *)msg_p->payload;

    TC_ASSERT(ussd_info_p->type == 0);
    TC_ASSERT(ussd_info_p->received_type == CN_USSD_RECEIVED_TYPE_GSM_USSD_NOTIFY);
    TC_ASSERT(ussd_info_p->length == strlen("Message")); /* Do not check the NULL terminator since it's not included. */
    TC_ASSERT(ussd_info_p->dcs == 3);
    TC_ASSERT(ussd_info_p->session_id == 1);
    TC_ASSERT(ussd_info_p->me_initiated == 1);
    TC_ASSERT(memcmp(ussd_info_p->ussd_string, "Message", strlen("Message")) == 0);

exit:
    free(msg_p);
    return tc_result;
}

tc_result_t cn_event_ussd_positive_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_ussd_info_t *ussd_info_p = NULL;
    mal_ss_ussd_info ussd_info;

    ussd_info.ussd_type_code = 5;
    ussd_info.ussd_received_type = MAL_SS_GSM_USSD_COMMAND;
    ussd_info.ussd_data.length = strlen("Message");
    ussd_info.ussd_data.dcs = 3;
    ussd_info.ussd_data.ussd_str = (uint8_t *) "Message"; /* Not zero-terminated in live setting, doesn't matter for the module tests */
    ussd_info.ussd_session_id = (void *) 1; /* MT-Null and MO-ptr to client tag */
    ussd_info.is_ussd_ue_terminated = 1;  /* Indicates whether ussd is terminated by UE-1/Nw-0 */
    ss_event_callback(MAL_SS_ON_USSD, (void *)&ussd_info, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_STATUS_IND MAL_SS_GSM_STATUS_REQUEST_USSD_STOP */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_USSD == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    ussd_info_p = (cn_ussd_info_t *)msg_p->payload;

    TC_ASSERT(ussd_info_p->type == 5);
    TC_ASSERT(ussd_info_p->received_type == CN_USSD_RECEIVED_TYPE_GSM_USSD_COMMAND);
    TC_ASSERT(ussd_info_p->length == strlen("Message")); /* Do not check the NULL terminator since it's not included. */
    TC_ASSERT(ussd_info_p->dcs == 3);
    TC_ASSERT(ussd_info_p->session_id == 1);
    TC_ASSERT(ussd_info_p->me_initiated == 1);
    TC_ASSERT(memcmp(ussd_info_p->ussd_string, "Message", strlen("Message")) == 0);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_baseband_version_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    request_record_t request_record;

    request_record.message_type = 0x40;
    request_record.response_error_code = MAL_SUCCESS;
    request_record.state = REQUEST_STATE_FREE;

    result = cn_request_baseband_version(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_signal_info_config failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mis_event_callback(CN_RESPONSE_BASEBAND_VERSION, &request_record, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_BASEBAND_VERSION == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;

}

tc_result_t cn_request_baseband_version_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    request_record_t request_record;

    request_record.message_type = 0x40;
    request_record.response_error_code = MAL_SUCCESS;
    request_record.state = REQUEST_STATE_FREE;

    result = cn_request_baseband_version(g_context_p, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        //("cn_request_set_signal_info_config failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mis_event_callback(CN_RESPONSE_BASEBAND_VERSION, &request_record, MAL_MIS_FAIL, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        //("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_BASEBAND_VERSION == msg_p->type);
    TC_ASSERT(CN_SUCCESS != msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;

}


tc_result_t cn_request_set_product_profile_flag_positive()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    request_record_t request_record;

    request_record.message_type = 0x37;
    request_record.response_error_code = MAL_SUCCESS;
    request_record.state = 1;

    cn_uint32_t flag_id = 130;
    cn_uint32_t flag_value = 5;

    result = cn_request_set_product_profile_flag(g_context_p, flag_id, flag_value, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_product_profile_flag failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mis_event_callback(CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG, &request_record, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(0x337 == msg_p->type);//CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG-0x337
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;

}


#if 1

tc_result_t cn_request_set_product_profile_flag_negative()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    request_record_t request_record;

    request_record.message_type = 0x37;
    request_record.response_error_code = MAL_SUCCESS;
    request_record.state = 1;

    cn_uint32_t flag_id = 130;
    cn_uint32_t flag_value = 5;

    result = cn_request_set_product_profile_flag(g_context_p, flag_id, flag_value, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_product_profile_flag failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mis_event_callback(CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG, &request_record, MAL_FAIL, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(0x337 == msg_p->type);//CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG-0x337
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;

}
#endif

tc_result_t cn_request_get_pp_flag_positive()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    request_record_t request_record;

    request_record.message_type = 0x41;
    request_record.response_error_code = MAL_SUCCESS;
    request_record.state = 1;

    cn_uint32_t flag_id = 130;
    int flag = 1;

    result = cn_request_get_pp_flags(g_context_p, flag, flag_id, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        //("cn_request_get_pp_flags failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mis_event_callback(CN_RESPONSE_GET_PP_FLAGS, &request_record, MAL_SUCCESS, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        //("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_PP_FLAGS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;

}


tc_result_t cn_request_get_pp_flag_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    request_record_t request_record;

    request_record.message_type = 0x41;
    request_record.response_error_code = MAL_SUCCESS;
    request_record.state = 1;

    cn_uint32_t flag_id = 130;
    int flag = 1;

    result = cn_request_get_pp_flags(g_context_p, flag, flag_id, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        //("cn_request_get_pp_flags failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();
    mis_event_callback(CN_RESPONSE_GET_PP_FLAGS, &request_record, MAL_MIS_FAIL, g_mal_client_tag_p);
    WAIT_FOR_MESSAGE(g_request_fd);

    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        //("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_PP_FLAGS == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
exit:
    free(msg_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;

}


tc_result_t cn_request_query_call_barring_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    int mal_data = 1;
    cn_call_barring_t call_barring = {

        .service_class = CN_SERVICE_CLASS_GSM_DEFAULT,
        .setting = CN_CALL_BARRING_ACTIVATION
    };
    result = cn_request_query_call_barring(g_context_p, "AO", &call_barring, (cn_client_tag_t) 751);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_call_barring failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check MAL parameters in the stub */
    //    TC_ASSERT(GSM_DEFAULT == g_mal_ss_serv_class_info);
    //  TC_ASSERT(MAL_SS_ACTIVATION == g_mal_ss_service_op);

    /* Generate fake MAL response */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &mal_data, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_QUERY_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_QUERY_CALL_BARRING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}


tc_result_t cn_request_query_call_barring_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    int mal_data = 1;
    cn_call_barring_t call_barring = {

        .service_class = CN_SERVICE_CLASS_GSM_DEFAULT,
        .setting = CN_CALL_BARRING_ACTIVATION
    };
    result = cn_request_query_call_barring(g_context_p, "AO", &call_barring, (cn_client_tag_t) 751);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_call_barring failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check MAL parameters in the stub */
    //  TC_ASSERT(GSM_DEFAULT == g_mal_ss_serv_class_info);
    //  TC_ASSERT(MAL_SS_ACTIVATION == g_mal_ss_service_op);

    /* Generate fake MAL response */
    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &mal_data, MAL_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_QUERY_CALL_BARRING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_QUERY_CALL_BARRING == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);
    TC_ASSERT(751 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}




tc_result_t cn_request_set_l1_parameter_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    result = cn_request_set_l1_parameter(g_context_p, 1, 1, (cn_client_tag_t) 12);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_user_activity_status_positive_1 failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    gss_event_callback(MAL_GSS_SET_USER_ACTIVITY_INFO_IND, NULL, MAL_GSS_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_USER_ACTIVITY_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_L1_PARAMETER == msg_p->type);
    //      TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(12 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;


}
tc_result_t cn_request_set_user_activity_status_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    mal_gss_battery_info bat_info;

    cn_user_status_t status = 1;
    /* Send cn_request_set_user_activity_status */
    result = cn_request_set_user_activity_status(g_context_p, status, (cn_client_tag_t) 12);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_set_user_activity_status_positive_1 failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    bat_info = 2;
    gss_event_callback(MAL_GSS_SET_USER_ACTIVITY_INFO_IND, &bat_info, MAL_GSS_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_SET_USER_ACTIVITY_STATUS */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_SET_USER_ACTIVITY_STATUS == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(12 == msg_p->client_tag);

exit:
    free(msg_p);
    return tc_result;
}

/* =================
 * BRANCH TESTCASES
 * =================
 */
tc_result_t cn_request_rf_on_negative_cllient_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_mce_status mce_status;
    cn_uint32_t size = NULL;
    cn_message_t *msg_p = NULL;

    /* Send CN_REQUEST_RF_ON */
    result = cn_request_rf_on(NULL, (cn_client_tag_t) 12345);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_rf_on failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* No MAL parameter to check in the stub */

    /* Generate fake MAL response */
    mce_status = MAL_MCE_OK;
    mce_event_callback(MAL_MCE_RF_STATE_RESP, &mce_status, MAL_MCE_FAIL, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_RF_ON */
    result = cn_message_receive(g_request_fd, NULL, NULL);
exit:
    return tc_result;
}

tc_result_t cn_event_name_info_positive_convert_postive()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_net_name_info mal_name_info;
    cn_network_name_info_t *name_info_p = NULL;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint8_t mcc_mnc[] = "34567";
    uint8_t spn_gsm7[81] = "";
    uint8_t mnn_gsm7[41] = "";
    size_t length;
    int res;
    int n_cl = 0;

    /* Generate fake MAL event */
    memset(&mal_name_info, 0, sizeof(mal_name_info));
    mal_name_info.mcc_mnc_string = mcc_mnc;

    length = sizeof(spn_gsm7) - 1;
    res = str_convert_string(CHARSET_UTF_16,
                             sizeof(LONG_OPERATOR_NAME_STRING) - 1, (uint8_t *)LONG_OPERATOR_NAME_STRING, /*spare*/ 0,
                             CHARSET_GSM_DEFAULT_7_BIT, &length, spn_gsm7);
    mal_name_info.full_operator_name.text_p = (char *)spn_gsm7;
    mal_name_info.full_operator_name.length = (uint8_t)length;
    mal_name_info.full_operator_name.add_ci = (uint8_t)0;
    mal_name_info.full_operator_name.dcs = 0x00; /* GSM-7 */
    mal_name_info.full_operator_name.spare = (uint8_t)0;
#if 0
    length = sizeof(mnn_gsm7) - 1;
    res = str_convert_string(CHARSET_UTF_8,
                             sizeof(SHORT_OPERATOR_NAME_STRING) - 1, (uint8_t *)SHORT_OPERATOR_NAME_STRING, /*spare*/ 0,
                             CHARSET_GSM_DEFAULT_7_BIT, &length, mnn_gsm7);
    mal_name_info.short_operator_name.text_p = (char *)mnn_gsm7;
    mal_name_info.short_operator_name.length = (uint8_t)length;
    mal_name_info.short_operator_name.add_ci = (uint8_t)0;
    mal_name_info.short_operator_name.dcs = 0x00; /* GSM-7 */
    mal_name_info.short_operator_name.spare = (uint8_t)0;
#endif 0
    net_event_callback(MAL_NET_NITZ_NAME_IND, &mal_name_info, MAL_NET_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive CN_EVENT_NETWORK_INFO */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_NETWORK_INFO == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    name_info_p = (cn_network_name_info_t *)msg_p->payload;

    TC_ASSERT(strcmp(name_info_p->mcc_mnc, (char *)mcc_mnc) == 0);
    TC_ASSERT(memcmp(name_info_p->name[CN_NETWORK_NAME_LONG].text, spn_gsm7, name_info_p->name[CN_NETWORK_NAME_LONG].length) == 0);
    TC_ASSERT(memcmp(name_info_p->name[CN_NETWORK_NAME_SHORT].text, mnn_gsm7, name_info_p->name[CN_NETWORK_NAME_SHORT].length) == 0);
    n_cl = cnserver_close_client(12);

exit:
    free(msg_p);
    return tc_result;


}



tc_result_t cn_event_ss_detailed_gsm_cause_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_net_detailed_fail_cause_t *cn_net_detailed_fail_cause_p = NULL;
    mal_ss_error_type_value mal_ss_error_type_test;
    mal_ss_error_type_test.error_value = 0x16;
    mal_ss_error_type_test.error_type = SS_ERROR_TYPE_MISC;
    mal_ss_error_type_test.error_value = MAL_SS_SERVICE_REQUEST_RELEASED; /* This shall be propagated as-is */
    ss_event_callback(MAL_SS_DETAILED_FAIL_CAUSE, (void *)&mal_ss_error_type_test, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_DETAILED_FAIL_CAUSE */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_NET_DETAILED_FAIL_CAUSE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_net_detailed_fail_cause_p = (cn_net_detailed_fail_cause_t *)msg_p->payload;

    TC_ASSERT(CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS == cn_net_detailed_fail_cause_p->class);
    TC_ASSERT(MAL_SS_SERVICE_REQUEST_RELEASED == cn_net_detailed_fail_cause_p->cause);

exit:
    free(msg_p);
    return tc_result;
}
tc_result_t cn_event_ss_detailed_gsm_cause_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_net_detailed_fail_cause_t *cn_net_detailed_fail_cause_p = NULL;
    mal_ss_error_type_value mal_ss_error_type_test;
    mal_ss_error_type_test.error_value = MAL_SS_SERVICE_UNKNOWN_ERROR;
    mal_ss_error_type_test.error_type = SS_ERROR_TYPE_MISC;
    mal_ss_error_type_test.error_value = MAL_SS_SERVICE_UNKNOWN_ERROR; /* This shall be propagated as-is */
    ss_event_callback(MAL_SS_DETAILED_FAIL_CAUSE, (void *)&mal_ss_error_type_test, MAL_SUCCESS, NULL);

    WAIT_FOR_MESSAGE(g_event_fd);

    /* Receive MAL_SS_DETAILED_FAIL_CAUSE */
    result = cn_message_receive(g_event_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_EVENT_NET_DETAILED_FAIL_CAUSE == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(0 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_net_detailed_fail_cause_p = (cn_net_detailed_fail_cause_t *)msg_p->payload;

    TC_ASSERT(CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS == cn_net_detailed_fail_cause_p->class);
    TC_ASSERT(MAL_SS_SERVICE_UNKNOWN_ERROR == cn_net_detailed_fail_cause_p->cause);

exit:
    free(msg_p);
    return tc_result;
}
tc_result_t cn_request_get_call_pas_positive_1()
{
    int *data = 2;
    empage_timer_expired(&data);
    rf_event_callback(1, &data, 0, &data);

    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_uint32_t cn_service_class;
    cn_request_change_barring_password_t *cn_change_barring_password = NULL;
    int32_t *mal_service_class_p = (int32_t *) calloc(1, sizeof(int32_t));
    mal_ss_service_response response;

    typedef struct {
        uint8_t                     ssOperation;
        uint8_t                     serviceCode;        /* Values from the constant table SS_BASIC_SERVICE_CODES */
        uint8_t                     ssCode[2];
        uint8_t                     sbCount;
        mal_ss_call_forward_info   *call_fwd_info;
        uint8_t                    *old_pword;
        uint8_t                    *new_pword;
    } ss_service_request_data;

    ss_service_request_data ss_password;
    ss_password.old_pword = "1234";
    ss_password.new_pword = "4321";
    char b = "bala";


    if (NULL == mal_service_class_p) {
        CN_LOG_E("allocation failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    g_mal_request_data_p = mal_service_class_p;

    /* Send CN_REQUEST_GET_CALL_WAITING */
    result = cn_request_change_barring_password(g_context_p, &b, &(ss_password.old_pword), &(ss_password.new_pword), (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_call_waiting failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Generate fake MAL response with an exit cause */
    memset(&response, 0, sizeof(mal_ss_service_response));

    call_event_callback(MAL_SS_SERVICE_COMPLETED_RESP, &response, MAL_SUCCESS, g_mal_client_tag_p);

    WAIT_FOR_MESSAGE(g_request_fd);

    /* Receive CN_RESPONSE_GET_CALL_WAITING */
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    //  TC_ASSERT(CN_RESPONSE_GET_CALL_WAITING == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT((cn_client_tag_t) 99 == msg_p->client_tag);
    //  TC_ASSERT(*mal_service_class_p == (int32_t) cn_service_class);

    //cn_call_waiting_p = (cn_call_waiting_t *) msg_p->payload;

    //TC_ASSERT(response.response[0] == (int32_t) cn_call_waiting_p->service_class);
    //  TC_ASSERT(response.response[1] == (int32_t) cn_call_waiting_p->setting);

exit:
    free(msg_p);
    free(mal_service_class_p);

    g_mal_request_data_p = NULL; /* Reset MAL result */

    return tc_result;
}

tc_result_t cn_request_negative_1()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    mal_net_name_info mal_name_info;
    cn_network_name_info_t *name_info_p = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    uint8_t mcc_mnc[] = "34567";
    uint8_t spn_gsm7[81] = "";
    uint8_t mnn_gsm7[41] = "";
    size_t length;
    int *data = 1;
    cn_log_select_callback(-1, &data);
    etl_find_entry_in_table(ETL_TABLE_UNKNOWN, 1);
    int *context = NULL, *context_fs = 3;
    cn_client_get_event_fd(&context, &context_fs);
    cn_request_rf_status(&context, 1);
    cn_request_rf_off(&context, 1);
    cn_request_rf_on(&context, 1);
    cn_network_access_config_data_t cn_config_data = { CN_NETWORK_ACCESS_CONFIG_ENABLE, CN_NETWORK_ACCESS_CONFIG_ENABLE };
    cn_request_registration_control(&context, &cn_config_data, 1);
    cn_request_reset_modem(&context, 1);
    cn_request_reset_modem_with_dump(&context, CN_CPU_ALL, 1);
    cn_request_set_preferred_network_type(&context, CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE, 1);
    cn_request_get_preferred_network_type(&context, 1);
    cn_request_registration_state_normal(&context, 1);
    cn_request_registration_state_gprs(&context, 1);
    cn_request_cell_info(&context, 1);
    cn_request_automatic_network_registration(&context, 1);
    cn_network_registration_req_data_t reg_info;
    strncpy(reg_info.operator, MCC_MNC_STRING, sizeof(reg_info.operator));
    reg_info.format = CN_OPERATOR_FORMAT_NUMERIC;
    reg_info.act = 2;
    cn_request_manual_network_registration(&context, &cn_config_data, 1);
    cn_request_manual_network_registration_with_automatic_fallback(&context, &cn_config_data, 1);
    cn_request_network_deregister(&context, 1);
    cn_request_net_query_mode(&context, 1);
    cn_request_manual_network_search(&context, 1);
    cn_request_interrupt_network_search(&context, 1);
    cn_request_current_call_list(&context, 1);
    cn_dial_t cn_dial;
    char *number_p = "0123456789";
    memset(&cn_dial, 0, sizeof(cn_dial));
    (void) strncpy(cn_dial.phone_number, number_p, CN_MAX_STRING_BUFF);
    cn_dial.clir = 1;
    cn_dial.call_type = CN_CALL_TYPE_VOICE_CALL;
    cn_request_dial(&context, &cn_dial, 1);
    cn_request_hangup(&context, CN_CALL_STATE_FILTER_NONE, 3, 1);
    cn_request_swap_calls(&context, (cn_uint8_t) 1, 1);
    cn_request_hold_call(&context, (cn_uint8_t) 1, 1);
    cn_request_resume_call(&context, (cn_uint8_t) 1, 1);
    cn_request_conference_call(&context, (cn_uint8_t) 1, 1);
    cn_request_conference_call_split(&context, (cn_uint8_t) 1, 1);
    cn_request_explicit_call_transfer(&context, (cn_uint8_t) 1, 1);
    cn_request_answer_call(&context, (cn_uint8_t) 1, 1);
    cn_request_ussd(&context, &cn_config_data, 1);
    cn_request_ussd_abort(&context, 1);
    cn_request_set_signal_info_config(&context, &cn_config_data, 1);
    cn_request_get_signal_info_config(&context, 1);
    cn_request_set_signal_info_reporting(&context, CN_RSSI_EVENT_REPORTING_DISABLE, 1);
    cn_request_get_signal_info_reporting(&context, 1);
    cn_request_rssi_value(&context, 1);
    cn_request_clip_status(&context, 1);
    cn_request_cnap_status(&context, 1);
    cn_request_colr_status(&context, 1);
    cn_request_clir_status(&context, 1);
    cn_request_set_clir(&context, 0, 1);
    cn_request_set_call_forward(&context, &cn_config_data, 1);
    cn_request_set_hsxpa_mode(&context, 1, 1);
    cn_request_get_hsxpa_mode(&context, 1);
    cn_modem_property_t cn_modem_property;
    cn_modem_property.type = CN_MODEM_PROPERTY_TYPE_CLIR;
    cn_modem_property.value.clir.clir_setting = CN_CLIR_SETTING_SUPPRESSION;
    cn_modem_property.value.clir.service_status = 99;
    cn_request_set_modem_property(&context, cn_modem_property, 1);
    cn_request_get_modem_property(&context, 0, 1);
    cn_request_set_cssn(&context, 0, 1);
    //cn_request_dtmf_send(&context, NULL,0,4,0,0,1);
    cn_request_dtmf_start(&context, 'A', 1);
    cn_request_dtmf_stop(&context, 1);
    cn_request_set_call_waiting(&context, NULL, 1);
    cn_request_get_call_waiting(&context, 64, 1);
    cn_request_query_call_barring(&context, "AO", 64, 1);
    cn_request_set_product_profile_flag(&context, 1, 64, 1);
    cn_request_set_l1_parameter(&context, 1, 1, 1);
    cn_request_set_user_activity_status(&context, 0, 1);
    cn_request_set_neighbour_cells_reporting(&context, TRUE, CN_NEIGHBOUR_RAT_TYPE_2G, 1);
    cn_request_get_neighbour_cells_reporting(&context, CN_NEIGHBOUR_RAT_TYPE_2G, 1);
    cn_request_get_neighbour_cells_complete_info(&context, CN_NEIGHBOUR_RAT_TYPE_2G, 1);
    cn_request_set_event_reporting(&context, CN_NEIGHBOUR_RAT_TYPE_2G, TRUE, 1);
    cn_request_get_event_reporting(&context, CN_NEIGHBOUR_RAT_TYPE_2G, 1);
    cn_request_set_event_reporting(&context, CN_NEIGHBOUR_RAT_TYPE_2G, TRUE, 1);
    cn_request_rab_status(&context, 1);
    cn_request_baseband_version(&context, 1);
    cn_request_modify_emergency_number_list(&context, 0, &context_fs, 1);
    cn_request_get_emergency_number_list(&context, 1);
    cn_request_nmr_info(&context, CN_NMR_RAT_TYPE_UTRAN, CN_NMR_UTRAN_TYPE_INTER_FREQ_NMR, 1);
    cn_get_timing_advance_value(&context, 1);
    cn_ss_command_t ss_command;
    memset(&ss_command, 0, sizeof(cn_ss_command_t));
    strcpy((char *) ss_command.mmi_string, "*330*a*0#");
    ss_command.mmi_string_length = strlen("*330*a*0#");
    ss_command.additional_results = TRUE;
    cn_request_ss(&context, ss_command, 1);
    cn_request_reg_status_event_config(&context, 0, 1);
    cn_request_rat_name(&context, 1);
    cn_request_modem_power_off(&context, 1);
    cn_request_send_tx_back_off_event(&context, 0, 1);
    cn_request_set_default_nvmd(&context, 1);
    typedef struct {
        uint8_t                     ssOperation;
        uint8_t                     serviceCode;        /* Values from the constant table SS_BASIC_SERVICE_CODES */
        uint8_t                     ssCode[2];
        uint8_t                     sbCount;
        mal_ss_call_forward_info   *call_fwd_info;
        uint8_t                    *old_pword;
        uint8_t                    *new_pword;
    } ss_service_request_data;

    ss_service_request_data ss_password;
    ss_password.old_pword = "1234";
    ss_password.new_pword = "4321";
    char b = "bala";


    cn_request_change_barring_password(&context, &b, &(ss_password.old_pword), &(ss_password.new_pword), 1);
    etl_print_table(0);
    etl_read_entry_from_table(0, 1);
    etl_write_entry_to_table(0, 1, 1, 1);
    etl_get_max_trigger_level_in_table(0);
    etl_find_first_empty_row(0);
    etl_clear_table_entry(0, 1);
    etl_update_trigger_level_table(0, 1, -1);
    cn_set_log_level(-1);
    handle_request_modify_emergency_number_list(&context, &context_fs);
    cn_message_send(1, 0, 0);
    cn_get_message_queue_size(1, 0);
    cn_clear_message_queue(-1);
    cn_message_receive(1, 0, NULL);
    cn_client_init(0);
    cn_client_get_request_fd(&context, 0);
    cn_client_get_event_fd(&context, 0);
    cn_call_forward_info_t call_forward_info;
    call_forward_info.reason = 3;
    call_forward_info.status = 3;
    strcpy(call_forward_info.number, "423890");
    call_forward_info.time_seconds = 10;
    call_forward_info.service_class = CN_SERVICE_CLASS_GSM_VOICE_FACSIMILE_SMS;
    cn_request_query_call_forward(&context, &call_forward_info, 1);
    cn_request_dtmf_send(&context, 0, 0, CN_DTMF_STRING_TYPE_ASCII, 0, 0, 1);
    cn_request_set_call_waiting(&context, &context_fs, 1);
    cn_request_set_call_barring(&context, "AO", &context_fs, "asdf", 1);
    cn_request_set_empage(&context, 0, 1);
    call_event_callback(MAL_CALL_MODEM_DTMF_STOP_RESP, &context, MAL_SUCCESS, 0);
    mis_event_callback(1, &context, 0, 0);

    //mal_net_init(&context);
    //initialize_pdc_data_in_request_record(&context,&context_fs);
    char *convere = 0xC;
    convert_bcd_ascii(&convere, 4);
    char *convera = 0xA;
    convert_bcd_ascii(&convera, 4);
    char *converb = 0xB;
    convert_bcd_ascii(&converb, 4);
    char *converc = 0xD;
    convert_bcd_ascii(&converc, 4);
    char *converd = 0xF;
    convert_bcd_ascii(&converd, 4);

    char *converea = 0x0C;
    convert_bcd_ascii(&converea, 1);
    char *converaa = 0x0A;
    convert_bcd_ascii(&converaa, 1);
    char *converab = 0x0B;
    convert_bcd_ascii(&converab, 1);
    char *converca = 0x0D;
    convert_bcd_ascii(&converca, 1);
    char *converda = 0x0F;
    convert_bcd_ascii(&converda, 1);
    convert_call_forward_status_to_procedure_type(CN_SS_PROCEDURE_TYPE_INTERROGATION);
    convert_call_forward_status_to_procedure_type(CN_SS_PROCEDURE_TYPE_ERASURE);
    convert_call_forward_status_to_procedure_type(CN_SS_PROCEDURE_TYPE_UNKNOWN);
    cn_util_convert_cn_service_class_to_ss_class(15);
    cn_util_convert_cn_service_class_to_ss_class(2);
    cn_util_convert_cn_service_class_to_ss_class(4);
    cn_util_convert_cn_service_class_to_ss_class(5);
    cn_util_convert_cn_service_class_to_ss_class(8);
    cn_util_convert_cn_service_class_to_ss_class(12);
    cn_util_convert_cn_service_class_to_ss_class(16);
    cn_util_convert_cn_service_class_to_ss_class(32);
    cn_util_convert_cn_service_class_to_ss_class(64);
    cn_util_convert_cn_service_class_to_ss_class(128);
    cn_util_convert_cn_service_class_to_ss_class(160);
    convert_reason_to_cn_cf_ss_type(1);
    convert_reason_to_cn_cf_ss_type(5);
    convert_reason_to_cn_cf_ss_type(12);
    char *barringa = "OI";
    convert_facility_to_cn_call_barring_ss_type(barringa);
    char *barringb = "OX";
    convert_facility_to_cn_call_barring_ss_type(barringb);
    char *barringc = "AI";
    convert_facility_to_cn_call_barring_ss_type(barringc);
    char *barringd = "IR";
    convert_facility_to_cn_call_barring_ss_type(barringd);
    char *barringe = "AB";
    convert_facility_to_cn_call_barring_ss_type(barringe);
    char *barringf = "AG";
    convert_facility_to_cn_call_barring_ss_type(barringf);
    char *barringg = "AC";
    convert_facility_to_cn_call_barring_ss_type(barringg);
    convert_facility_to_cn_call_barring_ss_type(&context);

}


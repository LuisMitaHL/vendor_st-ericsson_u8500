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
#include "pdc_test_utils.h"
#include "sim_client.h"

/* MAL callback prototypes, defined in mal_client.c */
extern void call_event_callback(int mal_call_event_id, void *data_p, int mal_error, void *client_tag_p);

/* SIM callback prototypes, defined in sim_client.c */
cn_void_t sim_client_callback(int cause, uintptr_t client_tag, void *data_p, void *user_data_p);

/* Global variables defined in mal_stubs.c */
extern uint32_t g_mal_string_length;
extern void *g_mal_request_data_p;
extern void *g_mal_client_tag_p;

/* Global variables defined in sim_stubs.c */
extern uintptr_t g_sim_client_tag_p;
extern int8_t g_ste_uicc_sim_file_read_ecc__int;
extern int8_t g_ste_uicc_sim_file_read_fdn__int;
extern int8_t g_ste_cat_call_control__int;
extern sim_service_type_t g_ste_uicc_get_service_availability__sim_service_type;
extern sim_service_type_t g_ste_uicc_get_service_table__sim_service_type_t;
extern uicc_request_status_t g_ste_uicc_sim_file_read_ecc__uicc_request_status_t;

/* Global variable defined in module_test_main.c to tell if pdc is enabled or not (default not) */
extern cn_bool_t g_tf_pdc_check_enabled;

/* I am very sorry to make such a horrid macro, but it is the easiest way to
 * keep definitions and initializations of common variables in one place. It
 * will reduce the amount of work needed to maintain the code.*/
#define VAR_DEFINE_AND_INIT tc_result_t tc_result = TC_RESULT_OK; \
cn_error_code_t result = 0; \
cn_message_t *msg_p = NULL; \
cn_uint32_t size = 0; \
ste_uicc_sim_ecc_response_t *sim_ecc_response_p = NULL; \
\
/* Enable pdc checking */ \
g_tf_pdc_check_enabled = TRUE; \
\
/* Make the dial-struct for input: */ \
/***********************************/ \
char *number_p = "0123456789"; \
cn_dial_t cn_dial; \
mal_call_dailup mal_dial; \
memset(&mal_dial, 0, sizeof(mal_dial)); \
memset(&cn_dial, 0, sizeof(cn_dial)); \
(void) strncpy(cn_dial.phone_number, number_p, CN_MAX_STRING_BUFF); \
cn_dial.clir = 1; \
cn_dial.call_type = CN_CALL_TYPE_VOICE_CALL; \
\
/* setup stub variables required to read back info from mal_call_request_dialup stub */ \
g_mal_string_length = strlen(number_p) + 1; \
mal_dial.phone_number = (char *) calloc(1, g_mal_string_length); \
g_mal_request_data_p = &mal_dial;


/* Test case 0:
 *
 * Make a normal phone call. SIM server returns failure when trying to read the ECC list, causing a calling error.
 */
tc_result_t pdc_voice_call_failure_1()
{
    VAR_DEFINE_AND_INIT
    (void) sim_ecc_response_p;

    /* Send CN_REQUEST_DIAL, starting the sequence */
    /***********************************************/
    result = cn_request_dial(g_context_p, &cn_dial, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_voice_call_accept_without_modification_1 failed! Response = %d", result);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    TC_ASSERT(1 == g_ste_uicc_sim_file_read_ecc__int);

    /* Generate erroneous callback from ste_uicc_sim_file_read_ecc */
    /*****************************************************/
    sim_client_callback(STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC, (uintptr_t)g_sim_client_tag_p, NULL, NULL);


    /* Receive response and verify validity */
    /****************************************/
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_voice_call_accept_without_modification_1 failed!(got result %d when expecting %d)", result, CN_SUCCESS);
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Check return message */
    TC_ASSERT(CN_RESPONSE_DIAL == msg_p->type);
    TC_ASSERT(CN_FAILURE == msg_p->error_code);

exit:
    free(msg_p);
    free(mal_dial.phone_number);
    g_mal_request_data_p = NULL; /* Reset MAL result */
    g_tf_pdc_check_enabled = FALSE; /* Reset pdc check enablement */
    return tc_result;
}


/*
 * Test case 1:
 *
 * Make a normal phone call. SAT does not modify it, no ECC match, FDN not available, just accept and make the call.
 *
 * */
tc_result_t pdc_voice_call_accept_without_modification_1()
{
    VAR_DEFINE_AND_INIT

    /* Send CN_REQUEST_DIAL, starting the sequence */
    /***********************************************/
    result = cn_request_dial(g_context_p, &cn_dial, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_voice_call_accept_without_modification_1 failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    TC_ASSERT(1 == g_ste_uicc_sim_file_read_ecc__int);

    /* Generate fake SIM response with ECC data: */
    char *sim_ecc_numbers[] = {"111", "222", "333", "444", "888"};
    sim_ecc_response_p = cn_pdc_make_sim_ecc_response(sim_ecc_numbers, 5);

    /* Generate callback from ste_uicc_sim_file_read_ecc */
    /*****************************************************/
    sim_client_callback(STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC, (uintptr_t)g_sim_client_tag_p, sim_ecc_response_p, NULL);
    free_sim_ecc_response(sim_ecc_response_p);

    TC_ASSERT(SIM_SERVICE_TYPE_FDN == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability */
    /************************************************************/
    ste_uicc_get_service_availability_response_t ust_response;
    ust_response.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
    ust_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check that a request for call control availability has been sent: */
    TC_ASSERT(SIM_SERVICE_TYPE_CALL_CONTROL == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate a callback for the SAT CC response: */
    /************************************************/
    /* (The request is the same as when the FDN availability was checked) */
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check MAL parameters in the stub */
    TC_ASSERT(0 == strcmp(cn_dial.phone_number, mal_dial.phone_number));
    TC_ASSERT(cn_dial.clir == mal_dial.clir);
    TC_ASSERT(MAL_CALL_TYPE_VOICE == mal_dial.call_type);

    /* Generate a callback for the MAL response */
    /********************************************/
    call_event_callback(MAL_CALL_MODEM_CREATE_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    /* Receive response and verify validity */
    /****************************************/
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_voice_call_accept_without_modification_1 failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

exit:
    free(msg_p);
    free(mal_dial.phone_number);
    g_mal_request_data_p = NULL; /* Reset MAL result */
    g_tf_pdc_check_enabled = FALSE; /* Reset pdc check enablement */
    return tc_result;
}

/* Call 0123456789, which FDN (enabled and containing a match) and ECC will
 * accept with no modification. Have SAT change it to 911, which
 * ECC-pass-2 will detect and make an emergency call. */
tc_result_t pdc_voice_call_accept_with_modification_1()
{
    VAR_DEFINE_AND_INIT

    /* Send CN_REQUEST_DIAL, starting the sequence */
    /***********************************************/
    result = cn_request_dial(g_context_p, &cn_dial, (cn_client_tag_t) 99);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_voice_call_accept_without_modification_1 failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    TC_ASSERT(1 == g_ste_uicc_sim_file_read_ecc__int);

    /* Generate fake SIM response with ECC data: */
    sim_ecc_response_p = cn_pdc_make_sim_ecc_response();

    /* Generate callback from ste_uicc_sim_file_read_ecc */
    /*****************************************************/
    sim_client_callback(STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC, (uintptr_t)g_sim_client_tag_p, sim_ecc_response_p, NULL);
    free_sim_ecc_response(sim_ecc_response_p);

    /* Check that a request for FDN availability has been sent: */
    TC_ASSERT(SIM_SERVICE_TYPE_FDN == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability */
    /************************************************************/
    ste_uicc_get_service_availability_response_t ust_response;
    ust_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    ust_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check that a request for FDN enabled status has been sent: */
    TC_ASSERT(SIM_SERVICE_TYPE_FDN == g_ste_uicc_get_service_table__sim_service_type_t);

    /* Generate callback from ste_uicc_get_service_table */
    /*****************************************************/
    ste_uicc_get_service_table_response_t est_response;
    est_response.service_status = STE_UICC_SERVICE_STATUS_ENABLED;
    est_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE, (uintptr_t)g_sim_client_tag_p, &est_response, NULL);

    /* Check that a request to read FDN data has been sent: */
    TC_ASSERT(1 == g_ste_uicc_sim_file_read_fdn__int);

    /* Generate callback from ste_uicc_sim_file_read_fdn */
    /*****************************************************/
    char *numbers_p[] = {"123", "1", "123", "0123456789", "123"};
    ste_uicc_sim_fdn_response_t *fdn_response = cn_pdc_make_sim_fdn_response(numbers_p, 5, STE_UICC_STATUS_CODE_OK);
    sim_client_callback(STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN, (uintptr_t)g_sim_client_tag_p, fdn_response, NULL);
    free_EST_response(&fdn_response);

    /* Check that a request to read SAT data has been sent: */
    TC_ASSERT(SIM_SERVICE_TYPE_CALL_CONTROL == g_ste_uicc_get_service_availability__sim_service_type);

    /* Generate callback from ste_uicc_get_service_availability */
    /************************************************************/
    /* NB! The same response struct is used as when FDN requested information from UST file. */
    sim_client_callback(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY, (uintptr_t)g_sim_client_tag_p, &ust_response, NULL);

    /* Check that a request to ste_cat_call_control has been sent: */
    TC_ASSERT(1 == g_ste_cat_call_control__int);
    /* TODO: Perform additional tests on that call control was called properly */

    /* Generate callback from ste_cat_call_control */
    /***********************************************/
    char *cc_number_p = "112";
    ste_cat_call_control_response_t *sat_cc_response_p = cn_pdc_make_sim_sat_cc_response(cc_number_p, 3);
    sim_client_callback(STE_CAT_CAUSE_SIM_EC_CALL_CONTROL, (uintptr_t)g_sim_client_tag_p, sat_cc_response_p, NULL);
    free_sim_sat_response(&sat_cc_response_p);

    /* Check that a request to MAL has been sent: */
    TC_ASSERT(0 == strcmp(cc_number_p, mal_dial.phone_number));
    TC_ASSERT(cn_dial.clir == mal_dial.clir);
    TC_ASSERT(MAL_CALL_TYPE_EMERGENCY == mal_dial.call_type);

    /* Generate a callback for the MAL response */
    /********************************************/
    call_event_callback(MAL_CALL_MODEM_CREATE_RESP, NULL, MAL_SUCCESS, g_mal_client_tag_p);

    /* Receive response and verify validity */
    /****************************************/
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("pdc_voice_call_accept_without_modification_1 failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

exit:
    free(msg_p);
    free(mal_dial.phone_number);
    g_mal_request_data_p = NULL; /* Reset MAL result */
    g_tf_pdc_check_enabled = FALSE; /* Reset pdc check enablement */
    return tc_result;

}


/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cn_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cn_pdc_ecc_list_handling.h"
#include "testcases.h"
#include "cn_pdc_internal.h"
#include "pdc_test_utils.h"

/*
 * Local support method prototypes:
 */
static cn_pdc_t *cn_pdc_setup_fdn_test_environment(char *calling_number);
static ste_uicc_get_service_availability_response_t *cn_pdc_make_sim_ust_response(ste_uicc_service_availability_t availability, ste_uicc_status_code_t status);
static ste_uicc_get_service_table_response_t *cn_pdc_make_sim_est_response(ste_uicc_service_status_t service_status, ste_uicc_status_code_t status);

/* Test of number comparison methods for FDN. Positive cases (cases where numbers match)
 *
 * Normal international
 * Normal non-international
 * Partial
 * All non-number characters (*, #, p and w)
 * Wildcards
 * */
tc_result_t pdc_number_comparison_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    int i = 0;
    int number_of_comparisons = 7;
    cn_bool_t status = false;
    char *numbers_p[] = {"+4646101010", "4646101010", "046102030", "*43#123p123w12", "123123123", "*21*+1234#", "*21*+4425993004780#"};
    char reference_TON[] = {0x01, 0x00, 0x00, 0x02, 0x03, 0x01, 0x01};
    char *reference_numbers_p[] = {"4646101010", "4646101010", "046", "*43#123C123C12", "123DDD123", "*21*1234#", "*21*4425993004780#"};
    int reference_length_p[] = {10, 10, 3, 14, 9, 9, 18};

    for (i = 0; i < number_of_comparisons; i++) {
        status = cn_pdc_fdn_equal_numbers(numbers_p[i], reference_numbers_p[i], reference_length_p[i], reference_TON[i]);
        TC_ASSERT(TRUE == status);
    }

exit:
    return tc_result;
}

/* Test of number comparison methods for FDN. Negative cases (cases where numbers do not match)
 *
 * TON in FDN, no '+' in compared number
 * TON match, but not enough digits in phone number
 * Phone number does not match pause character
 * Not enough digits in phone number
 * Plain wrong number
 * */
tc_result_t pdc_number_comparison_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    int i = 0;

    int number_of_comparisons = 5;
    cn_bool_t status = false;
    char *numbers_p[] = {"004646101010", "+4646101010", "123#", "12345", "12312313"};
    char reference_TON[] = {0x01, 0x01, 0x00, 0x02, 0x03};
    char *reference_numbers_p[] = {"004646101010", "4646101010D", "123C", "123456", "1231233"};
    int reference_length_p[] = {12, 11, 4, 6, 7};

    for (i = 0; i < number_of_comparisons; i++) {
        status = cn_pdc_fdn_equal_numbers(numbers_p[i], reference_numbers_p[i], reference_length_p[i], reference_TON[i]);
        TC_ASSERT(FALSE == status);
    }

exit:
    return tc_result;
}

/* Test that NULL as input data aborts execution gracefully */
tc_result_t pdc_fdn_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_fdn_test_environment("+46462111326");
    free(cn_pdc_p->pdc_input_p);
    cn_pdc_p->pdc_input_p = NULL;
    TC_ASSERT(CN_ECC_RESULT_ERROR == cn_pdc_fdn_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that a SIM failure will result in FDN accepted (as it cannot be ascertained that FDN is active) */
tc_result_t pdc_fdn_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_fdn_test_environment("+46462111326");
    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_ust_response(STE_UICC_SERVICE_AVAILABLE, STE_UICC_STATUS_CODE_UNKNOWN);
    TC_ASSERT(CN_FDN_RESULT_ACCEPT == cn_pdc_fdn_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that call is accepted if EF_EST response is that EF_FDN is not enabled */
tc_result_t pdc_fdn_negative_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_fdn_test_environment("+46462111326");

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_ust_response(STE_UICC_SERVICE_AVAILABLE, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    free(cn_pdc_p->record_p->response_data_p);
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_est_response(STE_UICC_SERVICE_STATUS_DISABLED, STE_UICC_STATUS_CODE_SHUTTING_DOWN);

    TC_ASSERT(CN_FDN_RESULT_ACCEPT == cn_pdc_fdn_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that call is accepted when read to EF_FDN response does not return valid response */
tc_result_t pdc_fdn_negative_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_fdn_test_environment("222");

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_ust_response(STE_UICC_SERVICE_AVAILABLE, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    free(cn_pdc_p->record_p->response_data_p);
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_est_response(STE_UICC_SERVICE_STATUS_ENABLED, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    free(cn_pdc_p->record_p->response_data_p);
    char *sim_fdn_numbers[] = {"1", "22", "333", "4444", "555555"};
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_fdn_response(sim_fdn_numbers, 5, STE_UICC_STATUS_CODE_FAIL);

    TC_ASSERT(CN_FDN_RESULT_ACCEPT == cn_pdc_fdn_check(cn_pdc_p));

    free_EST_response((ste_uicc_sim_fdn_response_t **)&(cn_pdc_p->record_p->response_data_p));
exit:

    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that call is accepted if EF_UST response is that EF_FDN is not supported */
tc_result_t pdc_fdn_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_fdn_test_environment("+46462111326");

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_ust_response(STE_UICC_SERVICE_NOT_AVAILABLE, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_ACCEPT == cn_pdc_fdn_check(cn_pdc_p));

exit:
    //free_EST_response((ste_uicc_sim_fdn_response_t**)&(cn_pdc_p->record_p->response_data_p));
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}


/* Test that call is accepted if EF_EST response is that EF_FDN is not enabled */
tc_result_t pdc_fdn_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_fdn_test_environment("+46462111326");

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_ust_response(STE_UICC_SERVICE_AVAILABLE, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    free(cn_pdc_p->record_p->response_data_p);
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_est_response(STE_UICC_SERVICE_STATUS_DISABLED, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_ACCEPT == cn_pdc_fdn_check(cn_pdc_p));

exit:
    //free_EST_response((ste_uicc_sim_fdn_response_t**)&(cn_pdc_p->record_p->response_data_p));
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that call is rejected when EF_FDN response does not contain matching number */
tc_result_t pdc_fdn_positive_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_fdn_test_environment("222");

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_ust_response(STE_UICC_SERVICE_AVAILABLE, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    free(cn_pdc_p->record_p->response_data_p);
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_est_response(STE_UICC_SERVICE_STATUS_ENABLED, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    free(cn_pdc_p->record_p->response_data_p);
    char *sim_fdn_numbers[] = {"1", "2222", "333", "4444", "555555"};
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_fdn_response(sim_fdn_numbers, 5, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_REJECT == cn_pdc_fdn_check(cn_pdc_p));

    free_EST_response((ste_uicc_sim_fdn_response_t **)&(cn_pdc_p->record_p->response_data_p));
exit:

    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}



/* Test that call is accepted if EF_FDN response contains matching number */
tc_result_t pdc_fdn_positive_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_fdn_test_environment("46462111326");

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_ust_response(STE_UICC_SERVICE_AVAILABLE, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    free(cn_pdc_p->record_p->response_data_p);
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_est_response(STE_UICC_SERVICE_STATUS_ENABLED, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_PENDING == cn_pdc_fdn_check(cn_pdc_p));
    free(cn_pdc_p->record_p->response_data_p);
    char *sim_fdn_numbers[] = {"111", "222", "46462111326", "444", "888"};
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_fdn_response(sim_fdn_numbers, 5, STE_UICC_STATUS_CODE_OK);

    TC_ASSERT(CN_FDN_RESULT_ACCEPT == cn_pdc_fdn_check(cn_pdc_p));

    free_EST_response((ste_uicc_sim_fdn_response_t **)&(cn_pdc_p->record_p->response_data_p));
exit:

    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

tc_result_t pdc_fdn_negative_24()
{
    tc_result_t tc_result = TC_RESULT_OK;
    return tc_result;
}
/* Test that call is rejected if EF_FDN response does not contain matching number */


/**************************************************************************************
 *
 * Static support methods
 *
 *************************************************************************************/
cn_pdc_t *cn_pdc_setup_fdn_test_environment(char *calling_number)
{
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_test_environment();
    cn_pdc_p->state = CN_PDC_STATE_FDN;
    cn_pdc_p->pdc_fdn_state = CN_PDC_FDN_STATE_INITIAL;
    strcpy(cn_pdc_p->pdc_input_p->service.voice_call.phone_number, calling_number);

    return cn_pdc_p;
}

ste_uicc_get_service_availability_response_t *cn_pdc_make_sim_ust_response(ste_uicc_service_availability_t availability, ste_uicc_status_code_t status)
{
    ste_uicc_get_service_availability_response_t *response_p = (ste_uicc_get_service_availability_response_t *) calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    response_p->service_availability = availability;
    response_p->uicc_status_code = status;

    return response_p;
}

ste_uicc_get_service_table_response_t *cn_pdc_make_sim_est_response(ste_uicc_service_status_t service_status, ste_uicc_status_code_t status)
{
    ste_uicc_get_service_table_response_t *response_p = (ste_uicc_get_service_table_response_t *) calloc(1, sizeof(ste_uicc_get_service_table_response_t));
    response_p->service_status = service_status;
    response_p->uicc_status_code = status;

    return response_p;
}


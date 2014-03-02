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

/* Global variables defined in sim_stubs.c */
extern int g_ste_uicc_sim_file_read_ecc__int;

/*
 * Local support method prototypes:
 */
static cn_pdc_t *cn_pdc_setup_ecc_test_environment(char *calling_number);

static void cn_pdc_add_sim_ecc_number_to_file(char *number_p);


/* Test that wrong service type aborts execution gracefully */
tc_result_t pdc_ecc_negative_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("112");
    cn_pdc_p->pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
    TC_ASSERT(CN_ECC_RESULT_CHECK_NOT_RELEVANT == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that call with wrong/modified state exits gracefully */
tc_result_t pdc_ecc_negative_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("777");
    cn_pdc_p->pdc_ecc_state = CN_PDC_ECC_STATE_WAIT_FOR_SIM_LIST;

    TC_ASSERT(CN_ECC_RESULT_ERROR == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that NULL as input data aborts execution gracefully */
tc_result_t pdc_ecc_negative_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("112");
    free(cn_pdc_p->pdc_input_p);
    cn_pdc_p->pdc_input_p = NULL;
    TC_ASSERT(CN_ECC_RESULT_ERROR == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that 112 is identified by default ECC as emergency number */
tc_result_t pdc_ecc_default_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("112");

    TC_ASSERT(CN_ECC_RESULT_EMERGENCY_CALL == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that 911 is identified by default ECC as emergency number */
tc_result_t pdc_ecc_default_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("911");

    TC_ASSERT(CN_ECC_RESULT_EMERGENCY_CALL == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that a number matched by the list from SIM is identified correctly.*/
tc_result_t pdc_ecc_simlist_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("444");
    TC_ASSERT(CN_ECC_RESULT_PENDING == cn_pdc_ecc_check(cn_pdc_p));
    TC_ASSERT(1 == g_ste_uicc_sim_file_read_ecc__int);

    char *sim_ecc_numbers[] = {"111", "222", "333", "444", "888"};
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_ecc_response(sim_ecc_numbers, 5);

    TC_ASSERT(CN_ECC_RESULT_EMERGENCY_CALL == cn_pdc_ecc_check(cn_pdc_p));

exit:
    free(((ste_uicc_sim_ecc_response_t *)(cn_pdc_p->record_p->response_data_p))->ecc_number_p);
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Test that the default SIM-list can be used to identify a number when SIM is not present */
tc_result_t pdc_ecc_simlist_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("118"); /* 118 is part of the predefined no-sim list */
    TC_ASSERT(CN_ECC_RESULT_PENDING == cn_pdc_ecc_check(cn_pdc_p));

    cn_pdc_p->record_p->response_error_code = STE_UICC_CAUSE_NOT_READY;
    cn_pdc_p->record_p->response_data_p = NULL;

    TC_ASSERT(CN_ECC_RESULT_EMERGENCY_CALL == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Verify that a number can be correctly identified from file (the emergency number list file) */
tc_result_t pdc_ecc_filelist_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("555");

    cn_pdc_add_sim_ecc_number_to_file("545");
    cn_pdc_add_sim_ecc_number_to_file("555");
    cn_pdc_add_sim_ecc_number_to_file("777");

    TC_ASSERT(CN_ECC_RESULT_EMERGENCY_CALL == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Verify that a number will be classified correctly as NO EMERGENCY in a situation where
 * all ECC number sources contain reasonable values. */
tc_result_t pdc_ecc_all_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("555");

    cn_pdc_add_sim_ecc_number_to_file("545");
    cn_pdc_add_sim_ecc_number_to_file("5555");

    TC_ASSERT(CN_ECC_RESULT_PENDING == cn_pdc_ecc_check(cn_pdc_p));

    char *sim_ecc_numbers[] = {"111", "222", "333", "444", "888"};
    cn_pdc_p->record_p->response_data_p = cn_pdc_make_sim_ecc_response(sim_ecc_numbers, 5);

    TC_ASSERT(CN_ECC_RESULT_NO_EMERGENCY_CALL == cn_pdc_ecc_check(cn_pdc_p));

exit:
    free(((ste_uicc_sim_ecc_response_t *)(cn_pdc_p->record_p->response_data_p))->ecc_number_p);
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Verify that a number will be classified correctly as NO EMERGENCY in a situation where
 * only the default ECC number source contain reasonable values. */
tc_result_t pdc_ecc_all_positive_2()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("555");

    /* Don't create a file with numbers. */

    TC_ASSERT(CN_ECC_RESULT_PENDING == cn_pdc_ecc_check(cn_pdc_p));

    /* Use default SIM-list response. */
    cn_pdc_p->record_p->response_error_code = STE_UICC_CAUSE_NOT_READY;
    cn_pdc_p->record_p->response_data_p = NULL;

    TC_ASSERT(CN_ECC_RESULT_NO_EMERGENCY_CALL == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}
tc_result_t cn_pdc_set_emergency_number_filepath_postive_1()
{

    tc_result_t tc_result = TC_RESULT_OK;
    char filepath_p = 1;
    cn_pdc_set_emergency_number_filepath(&filepath_p);

exit:
    return tc_result;
}

tc_result_t util_continue_as_privileged_user_postive_1()
{

    tc_result_t tc_result = TC_RESULT_OK;
    util_continue_as_privileged_user(1);

exit:
    return tc_result;
}
tc_result_t pdc_ecc_all_positive_3(void)
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_util_clear_emergency_number_list();
}

/* Call ecc checker with the state set to CN_PDC_ECC_STATE_POST_SAT_CC emergency number 112 */
tc_result_t pdc_ecc_postsat_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("112");
    cn_pdc_p->pdc_ecc_state = CN_PDC_ECC_STATE_POST_SAT_CC;

    TC_ASSERT(CN_ECC_RESULT_EMERGENCY_CALL == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Call ecc checker with the state set to CN_PDC_ECC_STATE_POST_SAT_CC emergency number 911 */
tc_result_t pdc_ecc_postsat_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("911");
    cn_pdc_p->pdc_ecc_state = CN_PDC_ECC_STATE_POST_SAT_CC;

    TC_ASSERT(CN_ECC_RESULT_EMERGENCY_CALL == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/* Call ecc checker with the state set to CN_PDC_ECC_STATE_POST_SAT_CC and a non-emergency number */
tc_result_t pdc_ecc_postsat_positive_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_ecc_test_environment("111");
    cn_pdc_p->pdc_ecc_state = CN_PDC_ECC_STATE_POST_SAT_CC;
    TC_ASSERT(CN_ECC_RESULT_NO_EMERGENCY_CALL == cn_pdc_ecc_check(cn_pdc_p));

exit:
    cleanup_test_environment(cn_pdc_p);
    return tc_result;
}

/**************************************************************************************
 *
 * Static support methods
 *
 *************************************************************************************/
cn_pdc_t *cn_pdc_setup_ecc_test_environment(char *calling_number)
{
    cn_pdc_t *cn_pdc_p = cn_pdc_setup_test_environment();
    cn_pdc_p->state = CN_PDC_STATE_ECC;
    cn_pdc_p->pdc_ecc_state = CN_PDC_ECC_STATE_INITIAL;
    strcpy(cn_pdc_p->pdc_input_p->service.voice_call.phone_number, calling_number);

    return cn_pdc_p;
}


void cn_pdc_add_sim_ecc_number_to_file(char *number_p)
{
    cn_emergency_number_list_internal_t list = { 1, {{
                CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED,
                CN_EMERGENCY_NUMBER_ORIGIN_SIM,
                {'\0'},
                {'\0'},
                {'\0'}
            }
        }
    };

    (void) memmove(list.emergency_number[0].emergency_number, number_p, CN_EMERGENCY_NUMBER_STRING_LENGTH);
    list.emergency_number[0].emergency_number[CN_EMERGENCY_NUMBER_STRING_LENGTH] = '\0';

    if (!cn_pdc_util_add_emergency_numbers_to_list(&list)) {
        CN_LOG_I("could not add sim ecc emergency number");
    }
}

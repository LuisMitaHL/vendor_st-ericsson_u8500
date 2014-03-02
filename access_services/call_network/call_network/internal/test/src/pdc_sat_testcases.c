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
#include "pdc_test_utils.h"


tc_result_t pdc_sat_voice_1()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;
    ste_cat_call_control_response_t *sat_response_p;
    ste_uicc_get_service_availability_response_t *sat_check_response;

    pdc_data_p = cn_pdc_setup_test_environment();

    pdc_data_p->pdc_input_p->sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service_type = CN_SERVICE_TYPE_VOICE_CALL;
    pdc_data_p->pdc_input_p->service.voice_call.clir = 0;
    memmove(pdc_data_p->pdc_input_p->service.voice_call.phone_number, "+461010110", strlen("+461010110"));
    pdc_data_p->pdc_input_p->service.voice_call.sat_initiated = FALSE;
    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);

    sat_check_response = calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    pdc_data_p->record_p->response_data_p = sat_check_response;
    sat_check_response->service_availability = STE_UICC_SERVICE_AVAILABLE;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);
    free(pdc_data_p->record_p->response_data_p);

    /* create response */
    sat_response_p = calloc(1, sizeof(ste_cat_call_control_response_t));
    pdc_data_p->record_p->response_data_p = sat_response_p;
    sat_response_p->cc_result = STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_DONE == pdc_response.result);
    TC_ASSERT(STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS == pdc_response.cc_result);

exit:
    cleanup_test_environment(pdc_data_p);
    return tc_result;
}

tc_result_t pdc_sat_ss_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;
    ste_cat_call_control_response_t *sat_response_p;
    ste_uicc_get_service_availability_response_t *sat_check_response;

    pdc_data_p = cn_pdc_setup_test_environment();

    pdc_data_p->pdc_input_p->sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
    pdc_data_p->pdc_input_p->service.ss.sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service.ss.ton_npi_used = TRUE;
    pdc_data_p->pdc_input_p->service.ss.type_of_number = CN_SS_TON_INTERNATIONAL;
    pdc_data_p->pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NATIONAL;
    memmove(pdc_data_p->pdc_input_p->service.ss.mmi_string, "*41*43#", strlen("*41*43#"));

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);

    sat_check_response = calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    pdc_data_p->record_p->response_data_p = sat_check_response;
    sat_check_response->service_availability = STE_UICC_SERVICE_AVAILABLE;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);
    free(pdc_data_p->record_p->response_data_p);

    /* create response */
    sat_response_p = calloc(1, sizeof(ste_cat_call_control_response_t));
    pdc_data_p->record_p->response_data_p = sat_response_p;
    sat_response_p->cc_result = STE_CAT_CC_ALLOWED_NO_MODIFICATION;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_DONE == pdc_response.result);
    TC_ASSERT(STE_CAT_CC_ALLOWED_NO_MODIFICATION == pdc_response.cc_result);

exit:
    cleanup_test_environment(pdc_data_p);
    return tc_result;
}

tc_result_t pdc_sat_ss_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;
    ste_cat_call_control_response_t *sat_response_p;
    ste_uicc_get_service_availability_response_t *sat_check_response;

    pdc_data_p = cn_pdc_setup_test_environment();

    pdc_data_p->pdc_input_p->sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
    pdc_data_p->pdc_input_p->service.ss.sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service.ss.ton_npi_used = TRUE;
    pdc_data_p->pdc_input_p->service.ss.type_of_number = CN_SS_TON_INTERNATIONAL;
    pdc_data_p->pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NATIONAL;
    memmove(pdc_data_p->pdc_input_p->service.ss.mmi_string, "*41*43#", strlen("*41*43#"));

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);

    sat_check_response = calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    pdc_data_p->record_p->response_data_p = sat_check_response;
    sat_check_response->service_availability = STE_UICC_SERVICE_AVAILABLE;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);
    free(pdc_data_p->record_p->response_data_p);

    /* create response */
    sat_response_p = calloc(1, sizeof(ste_cat_call_control_response_t));
    pdc_data_p->record_p->response_data_p = sat_response_p;
    sat_response_p->cc_result = STE_CAT_CC_NOT_ALLOWED_TEMP_PROBLEM;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_DONE == pdc_response.result);
    TC_ASSERT(STE_CAT_CC_NOT_ALLOWED_TEMP_PROBLEM == pdc_response.cc_result);

exit:
    cleanup_test_environment(pdc_data_p);
    return tc_result;
}

tc_result_t pdc_sat_ussd_1()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;
    ste_cat_call_control_response_t *sat_response_p;
    ste_uicc_get_service_availability_response_t *sat_check_response;

    pdc_data_p = cn_pdc_setup_test_environment();

    pdc_data_p->pdc_input_p->sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service_type = CN_SERVICE_TYPE_USSD;
    pdc_data_p->pdc_input_p->service.ussd.dcs = 0;
    pdc_data_p->pdc_input_p->service.ussd.length = strlen("*41*43#");
    memmove(pdc_data_p->pdc_input_p->service.ussd.ussd_string, "*41*43#", strlen("*41*43#"));

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);


    sat_check_response = calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    pdc_data_p->record_p->response_data_p = sat_check_response;
    sat_check_response->service_availability = STE_UICC_SERVICE_AVAILABLE;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);
    free(pdc_data_p->record_p->response_data_p);

    /* create response */
    sat_response_p = calloc(1, sizeof(ste_cat_call_control_response_t));
    pdc_data_p->record_p->response_data_p = sat_response_p;
    sat_response_p->cc_result = STE_CAT_CC_NOT_ALLOWED;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_DONE == pdc_response.result);
    TC_ASSERT(STE_CAT_CC_NOT_ALLOWED == pdc_response.cc_result);

exit:
    cleanup_test_environment(pdc_data_p);
    return tc_result;
}

tc_result_t pdc_sat_no_app_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;
    ste_uicc_get_service_availability_response_t *sat_check_response;

    pdc_data_p = cn_pdc_setup_test_environment();

    pdc_data_p->pdc_input_p->sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
    pdc_data_p->pdc_input_p->service.ss.sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service.ss.ton_npi_used = TRUE;
    pdc_data_p->pdc_input_p->service.ss.type_of_number = CN_SS_TON_INTERNATIONAL;
    pdc_data_p->pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NATIONAL;
    memmove(pdc_data_p->pdc_input_p->service.ss.mmi_string, "*41*43#", strlen("*41*43#"));

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);

    /* create response */
    sat_check_response = calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    pdc_data_p->record_p->response_data_p = sat_check_response;
    sat_check_response->service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_DONE == pdc_response.result);
    TC_ASSERT(STE_CAT_CC_ALLOWED_NO_MODIFICATION == pdc_response.cc_result);

exit:
    cleanup_test_environment(pdc_data_p);
    return tc_result;
}

tc_result_t pdc_sat_negative_1()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;
    ste_cat_call_control_response_t *sat_response_p;
    ste_uicc_get_service_availability_response_t *sat_check_response;

    pdc_data_p = cn_pdc_setup_test_environment();

    pdc_data_p->pdc_input_p->sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service_type = CN_SERVICE_TYPE_USSD;
    pdc_data_p->pdc_input_p->service.ussd.dcs = 0;
    pdc_data_p->pdc_input_p->service.ussd.length = strlen("*41*43#");
    memmove(pdc_data_p->pdc_input_p->service.ussd.ussd_string, "*41*43#", strlen("*41*43#"));

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);

    sat_check_response = calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    pdc_data_p->record_p->response_data_p = sat_check_response;
    sat_check_response->service_availability = STE_UICC_SERVICE_AVAILABLE;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);
    free(pdc_data_p->record_p->response_data_p);

    /* create response */
    sat_response_p = NULL;
    pdc_data_p->record_p->response_data_p = sat_response_p;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_ERROR == pdc_response.result);

exit:
    cleanup_test_environment(pdc_data_p);
    return tc_result;
}

tc_result_t pdc_sat_negative_2()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;

    pdc_data_p = NULL;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_ERROR == pdc_response.result);

exit:
    return tc_result;
}

tc_result_t pdc_sat_negative_3()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;
    ste_uicc_get_service_availability_response_t *sat_check_response;

    pdc_data_p = cn_pdc_setup_test_environment();

    pdc_data_p->pdc_input_p->sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service_type = 10;
    pdc_data_p->pdc_input_p->service.ussd.dcs = 0;
    pdc_data_p->pdc_input_p->service.ussd.length = strlen("*41*43#");
    memmove(pdc_data_p->pdc_input_p->service.ussd.ussd_string, "*41*43#", strlen("*41*43#"));
    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);

    sat_check_response = calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    pdc_data_p->record_p->response_data_p = sat_check_response;
    sat_check_response->service_availability = STE_UICC_SERVICE_AVAILABLE;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;
    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_ERROR == pdc_response.result);

exit:
    cleanup_test_environment(pdc_data_p);
    return tc_result;
}

tc_result_t pdc_sat_negative_4()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;
    ste_uicc_get_service_availability_response_t *sat_check_response;

    pdc_data_p = cn_pdc_setup_test_environment();

    pdc_data_p->pdc_input_p->sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service_type = CN_SERVICE_TYPE_UNKNOWN;
    pdc_data_p->pdc_input_p->service.ussd.dcs = 0;
    pdc_data_p->pdc_input_p->service.ussd.length = strlen("*41*43#");
    memmove(pdc_data_p->pdc_input_p->service.ussd.ussd_string, "*41*43#", strlen("*41*43#"));

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);


    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);

    sat_check_response = calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    pdc_data_p->record_p->response_data_p = sat_check_response;
    sat_check_response->service_availability = STE_UICC_SERVICE_AVAILABLE;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_ERROR == pdc_response.result);

exit:
    cleanup_test_environment(pdc_data_p);
    return tc_result;
}

tc_result_t pdc_sat_negative_5()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;
    ste_cat_call_control_response_t *sat_response_p;
    ste_uicc_get_service_availability_response_t *sat_check_response;

    pdc_data_p = cn_pdc_setup_test_environment();

    pdc_data_p->pdc_input_p->sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service_type = CN_SERVICE_TYPE_VOICE_CALL;
    pdc_data_p->pdc_input_p->service.voice_call.clir = 2;
    memmove(pdc_data_p->pdc_input_p->service.voice_call.phone_number, "+461010110", strlen("+461010110"));
    pdc_data_p->pdc_input_p->service.voice_call.sat_initiated = FALSE;
    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);

    sat_check_response = calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    pdc_data_p->record_p->response_data_p = sat_check_response;
    sat_check_response->service_availability = STE_UICC_SERVICE_AVAILABLE;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);
    free(pdc_data_p->record_p->response_data_p);

    /* create response */
    sat_response_p = calloc(1, sizeof(ste_cat_call_control_response_t));
    pdc_data_p->record_p->response_data_p = sat_response_p;
    sat_response_p->cc_result = STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_UNKOWN;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_ERROR == pdc_response.result);

exit:
    cleanup_test_environment(pdc_data_p);
    return tc_result;

}
tc_result_t pdc_sat_negative_6()
{

    tc_result_t tc_result = TC_RESULT_OK;
    cn_pdc_t *pdc_data_p = NULL;
    cn_sat_cc_result_struct_t pdc_response;
    ste_cat_call_control_response_t *sat_response_p;
    ste_uicc_get_service_availability_response_t *sat_check_response;

    pdc_data_p = cn_pdc_setup_test_environment();
    pdc_data_p->pdc_fdn_state = 0;

    pdc_data_p->pdc_input_p->sat_initiated = FALSE;
    pdc_data_p->pdc_input_p->service_type = CN_SERVICE_TYPE_USSD;
    pdc_data_p->pdc_input_p->service.ussd.dcs = 0;
    pdc_data_p->pdc_input_p->service.ussd.length = strlen("*41*43#");
    memmove(pdc_data_p->pdc_input_p->service.ussd.ussd_string, "*41*43#", strlen("*41*43#"));

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);
    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);

    sat_check_response = calloc(1, sizeof(ste_uicc_get_service_availability_response_t));
    pdc_data_p->record_p->response_data_p = sat_check_response;
    sat_check_response->service_availability = STE_UICC_SERVICE_AVAILABLE;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_PENDING == pdc_response.result);
    free(pdc_data_p->record_p->response_data_p);

    /* create response */
    sat_response_p = calloc(1, sizeof(ste_cat_call_control_response_t));
    pdc_data_p->record_p->response_data_p = sat_response_p;
    sat_response_p->cc_result = 20;
    pdc_data_p->sat_state = CN_PDC_SAT_STATE_RESPONSE;

    cn_pdc_sat_cc(pdc_data_p, &pdc_response);

    TC_ASSERT(CN_SAT_CC_RESULT_DONE == pdc_response.result);

exit:
    cleanup_test_environment(pdc_data_p);
    return tc_result;
}


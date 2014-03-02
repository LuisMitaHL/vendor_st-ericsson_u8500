/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* Set up a set of structs for testing the ECC cases.
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "cn_log.h"
#include "cn_pdc_internal.h"

/***********************************************/
/* Utilities for basic (method-based) tests:   */
/***********************************************/
cn_pdc_t *cn_pdc_setup_test_environment()
{
    /* Allocate data structs */
    cn_pdc_t *cn_pdc_p = (cn_pdc_t *) calloc(1, sizeof(cn_pdc_t));
    cn_pdc_p->record_p = (request_record_t *) calloc(1, sizeof(request_record_t));
    cn_pdc_p->pdc_input_p = (cn_pdc_input_t *) calloc(1, sizeof(cn_pdc_input_t));

    cn_pdc_p->pdc_input_p->service_type = CN_SERVICE_TYPE_VOICE_CALL;

    /* Remove left-over emergency-number list */
    unlink("/tmp/enl");

    return cn_pdc_p;
}

void cleanup_test_environment(cn_pdc_t *cn_pdc_p)
{
    free(cn_pdc_p->pdc_input_p);
    free(cn_pdc_p->record_p->response_data_p);
    free(cn_pdc_p->record_p);
    free(cn_pdc_p);

    /* Remove left-over emergency-number list */
    unlink("/tmp/enl");
}

/***************************************************/
/* Utilities for use-case (interface-based) tests: */
/***************************************************/
ste_uicc_sim_ecc_response_t *cn_pdc_make_sim_ecc_response(char *sim_ecc_numbers[], int number_of_ecc_numbers)
{
    int i = 0;
    ste_uicc_sim_ecc_response_t *response_p = (ste_uicc_sim_ecc_response_t *) calloc(1, sizeof(ste_uicc_sim_ecc_response_t));
    response_p->ecc_number_p = calloc(number_of_ecc_numbers, sizeof(ste_uicc_sim_ecc_number_t));

    for (i = 0; i < number_of_ecc_numbers; i++) {
        strcpy(response_p->ecc_number_p[i].number, sim_ecc_numbers[i]);
    }

    response_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
    response_p->number_of_records = number_of_ecc_numbers;

    return response_p;
}

void free_sim_ecc_response(ste_uicc_sim_ecc_response_t *response_p)
{
    free(response_p->ecc_number_p);
    free(response_p);
}

ste_cat_call_control_response_t *cn_pdc_make_sim_sat_cc_response(char *number_p, int length)
{
    ste_cat_call_control_response_t *return_p = (ste_cat_call_control_response_t *)calloc(1, sizeof(ste_cat_call_control_response_t));
    return_p->cc_result = STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS;

    return_p->cc_info.cc_type = STE_CAT_CC_CALL_SETUP;
    return_p->cc_info.cc_data.call_setup_p = (ste_cat_cc_call_setup_t *)calloc(1, sizeof(ste_cat_cc_call_setup_t));

    return_p->cc_info.cc_data.call_setup_p->address.ton = STE_SIM_TON_UNKNOWN;
    return_p->cc_info.cc_data.call_setup_p->address.npi = STE_SIM_NPI_UNKNOWN;
    return_p->cc_info.cc_data.call_setup_p->address.dialled_string_p = calloc(1, sizeof(ste_sim_text_t));
    return_p->cc_info.cc_data.call_setup_p->address.dialled_string_p->no_of_characters = length;
    return_p->cc_info.cc_data.call_setup_p->address.dialled_string_p->text_coding = STE_SIM_ASCII8;
    return_p->cc_info.cc_data.call_setup_p->address.dialled_string_p->text_p = calloc(1, length);
    memmove(return_p->cc_info.cc_data.call_setup_p->address.dialled_string_p->text_p, number_p, length);

    return return_p;
}

void free_sim_sat_response(ste_cat_call_control_response_t **p_pp)
{
    ste_cat_call_control_response_t *p_p = *p_pp;

    free(p_p->cc_info.cc_data.call_setup_p->address.dialled_string_p->text_p);
    free(p_p->cc_info.cc_data.call_setup_p->address.dialled_string_p);
    free(p_p->cc_info.cc_data.call_setup_p);
    free(p_p);
    *p_pp = NULL;
}

ste_uicc_sim_fdn_response_t *cn_pdc_make_sim_fdn_response(char *sim_fdn_numbers[], int number_of_fdn_numbers, ste_uicc_status_code_t status)
{

    int i = 0;
    char *tmp_text_p = NULL;

    ste_uicc_sim_fdn_response_t *response_p = (ste_uicc_sim_fdn_response_t *) calloc(1, sizeof(ste_uicc_sim_fdn_response_t));
    response_p->number_of_records = number_of_fdn_numbers;
    response_p->fdn_record_p = (ste_uicc_sim_fdn_record_t *) calloc(number_of_fdn_numbers, sizeof(ste_uicc_sim_fdn_record_t));

    for (i = 0; i < number_of_fdn_numbers; i++) {
        tmp_text_p = (char *) calloc(1, strlen(sim_fdn_numbers[i])); /* NB: Not allocating bytes for null termination is intentional! */
        strncpy(tmp_text_p, sim_fdn_numbers[i], strlen(sim_fdn_numbers[i]));
        response_p->fdn_record_p[i].dial_string.text_p = (void *)tmp_text_p;
        response_p->fdn_record_p[i].dial_string.no_of_characters = strlen(sim_fdn_numbers[i]);
        response_p->fdn_record_p[i].dial_string.text_coding = STE_SIM_ASCII8;
    }

    response_p->uicc_status_code = status;
    return response_p;
}

void free_EST_response(ste_uicc_sim_fdn_response_t **response_pp)
{
    if (NULL != response_pp && NULL != *response_pp) {
        ste_uicc_sim_fdn_response_t *response_p = *response_pp;
        CN_LOG_I("number_of_records is %d", response_p->number_of_records);
        uint32_t i = 0;

        for (i = 0; i < response_p->number_of_records; i++) {
            CN_LOG_I("Free:ing element %d", i);
            free(response_p->fdn_record_p[i].dial_string.text_p);
        }

        free(response_p->fdn_record_p);
        free(response_p);
        *response_pp = NULL;
    } else {
        printf("This is not working out!\n");
    }
}

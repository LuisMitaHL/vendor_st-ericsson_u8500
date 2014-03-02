/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "sim.h"

#include "cn_pdc_internal.h"
#include "cn_macros.h"
#include "sim_client.h"

static void cn_pdc_extract_response(cn_sat_cc_result_struct_t* result_struct_p, ste_cat_call_control_response_t* response_p);

void cn_pdc_sat_cc(cn_pdc_t* pdc_p, cn_sat_cc_result_struct_t* result_struct_p)
{
    result_struct_p->result = CN_SAT_CC_RESULT_ERROR;
    ste_sim_t *sim_p = NULL;
    ste_cat_call_control_t cc_config;
    ste_cat_cc_call_setup_t call_setup; /* Data for voice call.*/
    ste_cat_cc_ss_t ss_p; /* Data for SS.*/
    ste_cat_cc_ussd_t ussd_p; /* Data for USSD.*/
    cn_dial_t       *voice_call_in_p;
    cn_ss_command_t *ss_in_p;
    cn_ussd_info_t  *ussd_in_p;
    ste_sim_string_t ussd_string;
    ste_sim_text_t phone_number_text;
    int result = 0;


    if (!pdc_p) {
        CN_LOG_E("pdc_p is NULL!");
        goto error;
    }

    if( CN_PDC_SAT_STATE_INITIAL == pdc_p->sat_state) {
        sim_p = sim_client_get_handle();
        if (!sim_p) {
            CN_LOG_E("sim_client_get_handle failed!");
            goto error;
        }

        CN_LOG_D("ste_uicc_get_service_availability -> request");
        result = ste_uicc_get_service_availability ( sim_p,
                                                     (uintptr_t)request_record_get_modem_tag(pdc_p->record_p),
                                                     SIM_SERVICE_TYPE_CALL_CONTROL);
        if (UICC_REQUEST_STATUS_OK != result) {
            CN_LOG_E("ste_cat_call_control failed! (%d)", result);
            goto error;
        }

        result_struct_p->result = CN_SAT_CC_RESULT_PENDING;
        pdc_p->sat_state = CN_PDC_SAT_STATE_QUERY_RESPONSE;

    } else if ( CN_PDC_SAT_STATE_QUERY_RESPONSE == pdc_p->sat_state) {

        ste_uicc_get_service_availability_response_t *response_p;
        if( NULL == pdc_p->record_p->response_data_p) {
             CN_LOG_E("invalid response data");
             goto error;
         }
        CN_LOG_D("ste_uicc_get_service_availability <- response");
         response_p = (ste_uicc_get_service_availability_response_t *)pdc_p->record_p->response_data_p;

         if (STE_UICC_STATUS_CODE_OK != response_p->uicc_status_code) {
             CN_LOG_E("Call to ste_uicc_get_service_availability returned with code: %d", response_p->uicc_status_code);
             result_struct_p->result = CN_SAT_CC_RESULT_ERROR;
             return;
         }

         if( STE_UICC_SERVICE_AVAILABLE != response_p->service_availability) {
             CN_LOG_D("No sat control active");
             result_struct_p->cc_result = STE_CAT_CC_ALLOWED_NO_MODIFICATION;
             result_struct_p->result = CN_SAT_CC_RESULT_DONE;
             return;
         }

        sim_p = sim_client_get_handle();
        if (!sim_p) {
            CN_LOG_E("sim_client_get_handle failed!");
            goto error;
        }

        switch(pdc_p->pdc_input_p->service_type) {
        case CN_SERVICE_TYPE_UNKNOWN:
            CN_LOG_E("Unknown service type");
            goto error;
            break;
        case CN_SERVICE_TYPE_VOICE_CALL:
            cc_config.cc_type = STE_CAT_CC_CALL_SETUP;
            CN_LOG_D("cc_config.cc_type: %d", cc_config.cc_type);
            cc_config.cc_data.call_setup_p = &call_setup;
            voice_call_in_p = &(pdc_p->pdc_input_p->service.voice_call);


            phone_number_text.text_coding = STE_SIM_ASCII8; /* STE_SIM_UTF8 is not supported by SIM implementation */
            CN_LOG_D("text_coding: %d", STE_SIM_ASCII8);
            phone_number_text.text_p = ('+' == voice_call_in_p->phone_number[0]) ? &voice_call_in_p->phone_number[1] : voice_call_in_p->phone_number;
            CN_LOG_D("text_p: %s",phone_number_text.text_p  );
            phone_number_text.no_of_characters = strlen(phone_number_text.text_p);
            CN_LOG_D("phone_number_text.no_of_characters: %d",phone_number_text.no_of_characters);
            call_setup.address.ton = ('+' == voice_call_in_p->phone_number[0]) ? STE_SIM_TON_INTERNATIONAL : STE_SIM_TON_UNKNOWN;
            call_setup.address.npi = STE_SIM_NPI_ISDN;
            call_setup.address.dialled_string_p = &phone_number_text;
            call_setup.sub_address.len = 0;
            call_setup.sub_address.sub_address_p = NULL;
            call_setup.ccp_1.len = 0;
            call_setup.ccp_1.ccp_p = NULL;
            call_setup.ccp_2.len = 0;
            call_setup.ccp_2.ccp_p = NULL;
            call_setup.bc_repeat_ind = 0;
            break;
        case CN_SERVICE_TYPE_SS:
            cc_config.cc_type = STE_CAT_CC_SS;
            cc_config.cc_data.ss_p = &ss_p;
            ss_in_p = &(pdc_p->pdc_input_p->service.ss);

            phone_number_text.text_coding = STE_SIM_ASCII8; /* STE_SIM_UTF8 is not supported by SIM implementation */
            phone_number_text.text_p = (void*) ss_in_p->mmi_string;
            phone_number_text.no_of_characters = strlen(phone_number_text.text_p);
            ss_p.address.ton = ss_in_p->type_of_number;
            ss_p.address.npi = ss_in_p->numbering_plan_id;
            ss_p.address.dialled_string_p = &phone_number_text;
            break;
        case CN_SERVICE_TYPE_USSD:
            cc_config.cc_type = STE_CAT_CC_USSD;
            cc_config.cc_data.ussd_p = &ussd_p;
            ussd_in_p = &(pdc_p->pdc_input_p->service.ussd);

            ussd_string.text_coding = STE_SIM_ASCII8; /* STE_SIM_UTF8 is not supported by SIM implementation */
            ussd_string.str_p = (void*) ussd_in_p->ussd_string;
            ussd_string.no_of_bytes = ussd_in_p->length;
            ussd_p.ussd_data.dcs = ussd_in_p->dcs;
            ussd_p.ussd_data.dialled_string_p = &ussd_string;
            break;
        default:
            CN_LOG_E("Unknown service type, default case");
            goto error;
            break;
        }
        CN_LOG_D("ste_cat_call_control -> request");
        result = ste_cat_call_control(sim_p, (uintptr_t)request_record_get_modem_tag(pdc_p->record_p), &cc_config);
        if (result < 0) {
            CN_LOG_E("ste_cat_call_control failed! (%d)", result);
            goto error;
        }

        result_struct_p->result = CN_SAT_CC_RESULT_PENDING;
        pdc_p->sat_state = CN_PDC_SAT_STATE_RESPONSE;
    } else if ( CN_PDC_SAT_STATE_RESPONSE == pdc_p->sat_state) {
        ste_cat_call_control_response_t *response_p = NULL;
        CN_LOG_D("ste_cat_call_control <- response");

        if( NULL == pdc_p->record_p->response_data_p) {
            CN_LOG_E("invalid response data");
            goto error;
        }
        response_p = (ste_cat_call_control_response_t*) pdc_p->record_p->response_data_p;

        switch (response_p->cc_result) {
            case STE_CAT_CC_ALLOWED_NO_MODIFICATION:
                CN_LOG_D("STE_CAT_CC_ALLOWED_NO_MODIFICATION");
                break;
            case STE_CAT_CC_NOT_ALLOWED:
                CN_LOG_D("STE_CAT_CC_NOT_ALLOWED");
                break;
            case STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS:
                CN_LOG_D("STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS");
                CN_LOG_D("response_p->user_indication.text_coding: %d", response_p->user_indication.text_coding);
                CN_LOG_D("response_p->user_indication.no_of_bytes: %d", response_p->user_indication.no_of_bytes);
                CN_LOG_D("response_p->user_indication.no_of_bytes: %p, \"%s\"", response_p->user_indication.str_p, response_p->user_indication.str_p);
                break;
            default:
                CN_LOG_E("unknown result code: %d", response_p->cc_result);
                break;
        }

        cn_pdc_extract_response(result_struct_p, response_p);
        result_struct_p->result = CN_SAT_CC_RESULT_DONE;
    } else {
        CN_LOG_E("Unknown record state, state = %d", pdc_p->sat_state);
        goto error;
    }

error:
    return;
}

void cn_pdc_extract_response(cn_sat_cc_result_struct_t* result_struct_p, ste_cat_call_control_response_t* response_p)
{
    NULL_CHECK_AND_GOTO_EXIT(result_struct_p, "result_struct_p");
    NULL_CHECK_AND_GOTO_EXIT(response_p, "response_p");

    /* Copy generic parameters: */
    result_struct_p->cc_result = response_p->cc_result;

    if (STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS == result_struct_p->cc_result) {
    /* Only extract data from the response struct if the response is ALLOWED_WITH_MODIFICATIONS,
     * as there is no way to ensure that all the data is relevant for other cases. */
        result_struct_p->cc_type = response_p->cc_info.cc_type;

        /* Copy cc-type specific parameters: */
        switch (result_struct_p->cc_type) {
        case STE_CAT_CC_CALL_SETUP:
            result_struct_p->ton = response_p->cc_info.cc_data.call_setup_p->address.ton;
            result_struct_p->text_coding = response_p->cc_info.cc_data.call_setup_p->address.dialled_string_p->text_coding;
            result_struct_p->no_of_characters = response_p->cc_info.cc_data.call_setup_p->address.dialled_string_p->no_of_characters;
            memmove(result_struct_p->text_p, response_p->cc_info.cc_data.call_setup_p->address.dialled_string_p->text_p, result_struct_p->no_of_characters);
            break;
        case STE_CAT_CC_SS:
            result_struct_p->no_of_characters = response_p->cc_info.cc_data.ss_p->address.dialled_string_p->no_of_characters;
            result_struct_p->text_coding = response_p->cc_info.cc_data.ss_p->address.dialled_string_p->text_coding;
            result_struct_p->ton = response_p->cc_info.cc_data.ss_p->address.ton;
            result_struct_p->npi = response_p->cc_info.cc_data.ss_p->address.npi;
            memmove(result_struct_p->text_p, response_p->cc_info.cc_data.ss_p->address.dialled_string_p->text_p, result_struct_p->no_of_characters);
            break;
        case STE_CAT_CC_USSD:
            result_struct_p->no_of_characters = response_p->cc_info.cc_data.ussd_p->ussd_data.dialled_string_p->no_of_bytes;
            result_struct_p->text_coding = response_p->cc_info.cc_data.ussd_p->ussd_data.dialled_string_p->text_coding;
            result_struct_p->dcs = response_p->cc_info.cc_data.ussd_p->ussd_data.dcs;
            memmove(result_struct_p->text_p, response_p->cc_info.cc_data.ussd_p->ussd_data.dialled_string_p->str_p, result_struct_p->no_of_characters);
            break;
        case STE_CAT_CC_TYPE_UNKNOWN:
        default:
            CN_LOG_E("Unknown cc_type = %d", result_struct_p->cc_type);
            break;
        }
    }

exit:
    return;
}


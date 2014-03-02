/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cn_pdc_internal.h"
#include "cn_ss_command_handling.h"
#include "cn_macros.h"

cn_pdc_input_t* cn_pdc_translate_message_to_pdc_input(cn_message_type_t message_type, void *data_p)
{
    cn_pdc_input_t* pdc_input_p = NULL;

    pdc_input_p = calloc(1, sizeof(cn_pdc_input_t));
    if (!pdc_input_p) {
        CN_LOG_E("calloc failed for pdc_input_p!");
        goto error;
    }

    switch (message_type) {
        case CN_REQUEST_DIAL: {
            cn_request_dial_t *request_data_p = (cn_request_dial_t *) data_p;

            NULL_CHECK_AND_GOTO_ERROR(data_p, "data_p");

            pdc_input_p->service_type = CN_SERVICE_TYPE_VOICE_CALL;
            pdc_input_p->sat_initiated = request_data_p->dial.sat_initiated;
            pdc_input_p->service.voice_call = request_data_p->dial;
            break;
        }
        case CN_REQUEST_SS:
            NULL_CHECK_AND_GOTO_ERROR(data_p, "data_p");

            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            memmove(&pdc_input_p->service.ss, data_p, sizeof(cn_ss_command_t));
            pdc_input_p->sat_initiated = pdc_input_p->service.ss.sat_initiated;
            break;
        case CN_REQUEST_CLIP_STATUS: {
            char *ss_p = NULL;
            cn_ss_string_compounds_t ss_data;

            memset(&ss_data, 0, sizeof(ss_data));
            ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_INTERROGATION;
            ss_data.ss_type = CN_SS_TYPE_CALLING_LINE_IDENTIFICATION_PRESENTATION;

            ss_p = encode_ss_string(&ss_data);
            if (!ss_p) {
                CN_LOG_E("encode_ss_string failed!");
                goto error;
            }
 
            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            memmove(pdc_input_p->service.ss.mmi_string, ss_p, strlen(ss_p));
            pdc_input_p->sat_initiated = FALSE;
            pdc_input_p->service.ss.type_of_number = CN_SS_TON_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.additional_results = FALSE;
            free(ss_p);
            break;
        }
        case CN_REQUEST_CNAP_STATUS: {
            char *ss_p = NULL;
            cn_ss_string_compounds_t ss_data;

            memset(&ss_data, 0, sizeof(ss_data));
            ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_INTERROGATION;
            ss_data.ss_type = CN_SS_TYPE_CALLING_NAME_PRESENTATION;

            ss_p = encode_ss_string(&ss_data);
            if (!ss_p) {
                CN_LOG_E("encode_ss_string failed!");
                goto error;
            }

            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            memmove(pdc_input_p->service.ss.mmi_string, ss_p, strlen(ss_p));
            pdc_input_p->sat_initiated = FALSE;
            pdc_input_p->service.ss.type_of_number = CN_SS_TON_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.additional_results = FALSE;
            free(ss_p);
            break;
        }
        case CN_REQUEST_COLR_STATUS: {
            char *ss_p = NULL;
            cn_ss_string_compounds_t ss_data;

            memset(&ss_data, 0, sizeof(ss_data));
            ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_INTERROGATION;           
            ss_data.ss_type = CN_SS_TYPE_CONNECTED_LINE_IDENTIFICATION_RESTRICTION;

            ss_p = encode_ss_string(&ss_data);
            if (!ss_p) {
                CN_LOG_E("encode_ss_string failed!");
                goto error;
            }

            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            memmove(pdc_input_p->service.ss.mmi_string, ss_p, sizeof(char)*strlen(ss_p));
            pdc_input_p->sat_initiated = FALSE;
            pdc_input_p->service.ss.type_of_number = CN_SS_TON_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.additional_results = FALSE;
            free(ss_p);
            break;
        }
        case CN_REQUEST_CLIR_STATUS: {
            char *ss_p = NULL;
            cn_ss_string_compounds_t ss_data;

            memset(&ss_data, 0, sizeof(ss_data));
            ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_INTERROGATION;
            ss_data.ss_type = CN_SS_TYPE_CALLING_LINE_IDENTIFICATION_RESTRICTION;

            ss_p = encode_ss_string(&ss_data);
            if (!ss_p) {
                CN_LOG_E("encode_ss_string failed!");
                goto error;
            }

            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            memmove(pdc_input_p->service.ss.mmi_string, ss_p, strlen(ss_p));
            pdc_input_p->sat_initiated = FALSE;
            pdc_input_p->service.ss.type_of_number = CN_SS_TON_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.additional_results = FALSE;
            free(ss_p);
            break;
        }
        case CN_REQUEST_SET_CLIR:
            CN_LOG_D("CN_REQUEST_SET_CLIR not handled in pre-dial checks!");
            goto error;
            break;
        case CN_REQUEST_QUERY_CALL_FORWARD: {
            char *ss_p = NULL;
            cn_ss_string_compounds_t ss_data;
            cn_request_call_forward_info_t *forward_data_p = (cn_request_call_forward_info_t *) data_p;

            NULL_CHECK_AND_GOTO_ERROR(data_p, "data_p");

            memset(&ss_data, 0, sizeof(ss_data));
            if (0 != cn_util_convert_cn_service_class_to_ss_class(forward_data_p->call_forward_info.service_class)) {
                sprintf(ss_data.supplementary_info_b, "%d", cn_util_convert_cn_service_class_to_ss_class(forward_data_p->call_forward_info.service_class));
            }
            memmove(ss_data.supplementary_info_a, forward_data_p->call_forward_info.number, SI_ENTRY_MAX_SIZE);
            ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_INTERROGATION; /* For QUERY, status should always be 2 */
            ss_data.ss_type = convert_reason_to_cn_cf_ss_type(forward_data_p->call_forward_info.reason);

            ss_p = encode_ss_string(&ss_data);
            if (!ss_p) {
                CN_LOG_E("encode_ss_string failed!");
                goto error;
            }

            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            memmove(pdc_input_p->service.ss.mmi_string, ss_p, strlen(ss_p));
            pdc_input_p->sat_initiated = FALSE;
            pdc_input_p->service.ss.type_of_number = CN_SS_TON_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.additional_results = FALSE;
            free(ss_p);
            break;
        }
        case CN_REQUEST_SET_CALL_FORWARD: {
            char *ss_p = NULL;
            int no_reply_condition_timer = 0;
            cn_ss_string_compounds_t ss_data;
            memset(&ss_data, 0, sizeof(ss_data));
            cn_request_call_forward_info_t *forward_data_p = (cn_request_call_forward_info_t *) data_p;

            NULL_CHECK_AND_GOTO_ERROR(data_p, "data_p");

            memmove(ss_data.supplementary_info_a, forward_data_p->call_forward_info.number, SI_ENTRY_MAX_SIZE);
            CN_LOG_D("ss_data.supplementary_info_a = \"%s\"\n", ss_data.supplementary_info_a);

            if (0 != cn_util_convert_cn_service_class_to_ss_class(forward_data_p->call_forward_info.service_class)) {
                sprintf(ss_data.supplementary_info_b, "%d", cn_util_convert_cn_service_class_to_ss_class(forward_data_p->call_forward_info.service_class));
            }
            CN_LOG_D("ss_data.supplementary_info_b = \"%s\"\n", ss_data.supplementary_info_b);

            /* Include no reply condition timer setting in SIC according to Table B.1 in 3GPP TS 22.030
             * The SS encoder must specify SIC according to the condition below. */
            no_reply_condition_timer = forward_data_p->call_forward_info.time_seconds;
            if ((CN_CALL_FORWARDING_REASON_NO_REPLY == forward_data_p->call_forward_info.reason ||
                  CN_CALL_FORWARDING_REASON_ALL_CALL_FORWARDING == forward_data_p->call_forward_info.reason ||
                  CN_CALL_FORWARDING_REASON_ALL_CONDITIONAL_CALL_FORWARDING == forward_data_p->call_forward_info.reason
                ) && (no_reply_condition_timer >= 5 && no_reply_condition_timer <= 30)) {
                sprintf(ss_data.supplementary_info_c, "%d", no_reply_condition_timer);
            } else {
                CN_LOG_D("skipping no_reply_condition_timer setting (reason=%d and no_reply_condition_timer=%d)",
                         forward_data_p->call_forward_info.reason,
                         no_reply_condition_timer);
            }
            CN_LOG_D("ss_data.supplementary_info_c = \"%s\"\n", ss_data.supplementary_info_c);

            ss_data.procedure_type = convert_call_forward_status_to_procedure_type(forward_data_p->call_forward_info.status);
            ss_data.ss_type = convert_reason_to_cn_cf_ss_type(forward_data_p->call_forward_info.reason);

            ss_p = encode_ss_string(&ss_data);
            if (!ss_p) {
                CN_LOG_E("encode_ss_string failed!");
                goto error;
            }

            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            memmove(pdc_input_p->service.ss.mmi_string, ss_p, strlen(ss_p));
            pdc_input_p->sat_initiated = FALSE;
            pdc_input_p->service.ss.additional_results = FALSE;

            if (CN_SS_PROCEDURE_TYPE_REGISTRATION == ss_data.procedure_type) {
            pdc_input_p->service.ss.numbering_plan_id = (forward_data_p->call_forward_info.toa & 0x0F);
            pdc_input_p->service.ss.type_of_number = cn_util_convert_ss_toa_to_cn_ss_toa(forward_data_p->call_forward_info.toa);
            } else {
                pdc_input_p->service.ss.type_of_number = CN_SS_TON_NOT_A_DIALLING_NUMBER;
                pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NOT_A_DIALLING_NUMBER;
            }
            free(ss_p);
            break;
        }
       case CN_REQUEST_SET_CALL_WAITING: {
           char *ss_p = NULL;
           cn_ss_string_compounds_t ss_data;
           memset(&ss_data, 0, sizeof(ss_data));
           cn_request_set_call_waiting_t *waiting_data_p = (cn_request_set_call_waiting_t *) data_p;

           NULL_CHECK_AND_GOTO_ERROR(data_p, "data_p"); 

           CN_LOG_D("waiting_data_p->call_waiting.service_class: %d", waiting_data_p->call_waiting.service_class);
           if (0 != cn_util_convert_cn_service_class_to_ss_class(waiting_data_p->call_waiting.service_class)) {
               sprintf(ss_data.supplementary_info_a, "%d", cn_util_convert_cn_service_class_to_ss_class(waiting_data_p->call_waiting.service_class));
           }
           CN_LOG_D("ss_data.supplementary_info_a: \"%s\"",ss_data.supplementary_info_a);
           if(CN_CALL_WAITING_SERVICE_ENABLED  == waiting_data_p->call_waiting.setting) {
               ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
           } else {
               ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_DEACTIVATION;
           }
           ss_data.ss_type = CN_SS_TYPE_CALL_WAITING;

           ss_p = encode_ss_string(&ss_data);
           if (!ss_p) {
               CN_LOG_E("encode_ss_string failed!");
               goto error;
           }

           pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
           memmove(pdc_input_p->service.ss.mmi_string, ss_p, strlen(ss_p));
           pdc_input_p->sat_initiated = FALSE;
           pdc_input_p->service.ss.type_of_number = CN_SS_TON_NOT_A_DIALLING_NUMBER;
           pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NOT_A_DIALLING_NUMBER;
           pdc_input_p->service.ss.additional_results = FALSE;
           free(ss_p);
           break;
       }
       case CN_REQUEST_GET_CALL_WAITING: {
            char *ss_p = NULL;
            cn_ss_string_compounds_t ss_data;
            memset(&ss_data, 0, sizeof(ss_data));
            cn_request_get_call_waiting_t *waiting_data_p = (cn_request_get_call_waiting_t *) data_p;

            NULL_CHECK_AND_GOTO_ERROR(data_p, "data_p");

            ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_INTERROGATION;
            ss_data.ss_type = CN_SS_TYPE_CALL_WAITING;
            if (0 != cn_util_convert_cn_service_class_to_ss_class(waiting_data_p->service_class)) {
                sprintf(ss_data.supplementary_info_a, "%d", cn_util_convert_cn_service_class_to_ss_class(waiting_data_p->service_class));
            }

            ss_p = encode_ss_string(&ss_data);
            if (!ss_p) {
               CN_LOG_E("encode_ss_string failed!");
               goto error;
            }

            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            memmove(pdc_input_p->service.ss.mmi_string, ss_p, strlen(ss_p));
            pdc_input_p->sat_initiated = FALSE;
            pdc_input_p->service.ss.type_of_number = CN_SS_TON_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.additional_results = FALSE;
            free(ss_p);
            break;
        }
        case CN_REQUEST_QUERY_CALL_BARRING: {
            char *ss_p = NULL;
            cn_ss_string_compounds_t ss_data;
            memset(&ss_data, 0, sizeof(ss_data));
            cn_request_query_call_barring_t *barring_data_p = (cn_request_query_call_barring_t *) data_p;

            NULL_CHECK_AND_GOTO_ERROR(data_p, "data_p");

            if (0 != cn_util_convert_cn_service_class_to_ss_class(barring_data_p->service_class)) {
                sprintf(ss_data.supplementary_info_b, "%d", cn_util_convert_cn_service_class_to_ss_class(barring_data_p->service_class));
            }
            ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_INTERROGATION;
            ss_data.ss_type = convert_facility_to_cn_call_barring_ss_type(barring_data_p->facility);

            ss_p = encode_ss_string(&ss_data);
            if (!ss_p) {
               CN_LOG_E("encode_ss_string failed!");
               goto error;
            }

            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            memmove(pdc_input_p->service.ss.mmi_string, ss_p, strlen(ss_p));
            pdc_input_p->sat_initiated = FALSE;
            pdc_input_p->service.ss.type_of_number = CN_SS_TON_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.additional_results = FALSE;
            free(ss_p);
            break;
        }
        case CN_REQUEST_SET_CALL_BARRING: {
            char *ss_p = NULL;
            cn_ss_string_compounds_t ss_data;
            memset(&ss_data, 0, sizeof(ss_data));
            cn_request_set_call_barring_t *barring_data_p = (cn_request_set_call_barring_t *) data_p;

            NULL_CHECK_AND_GOTO_ERROR(data_p, "data_p");

            memset(&ss_data,0,sizeof(cn_ss_string_compounds_t));
            if (0 != cn_util_convert_cn_service_class_to_ss_class(barring_data_p->call_barring.service_class)) {
                sprintf(ss_data.supplementary_info_b, "%d", cn_util_convert_cn_service_class_to_ss_class(barring_data_p->call_barring.service_class));
            }
            if( CN_CALL_BARRING_ACTIVATION == barring_data_p->call_barring.setting) {
                ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_REGISTRATION;
            } else {
                ss_data.procedure_type = CN_SS_PROCEDURE_TYPE_ERASURE;
            }
            CN_LOG_D("facility code = %s",barring_data_p->facility);
            ss_data.ss_type = convert_facility_to_cn_call_barring_ss_type(barring_data_p->facility);
            memmove(ss_data.supplementary_info_a,barring_data_p->passwd,strlen(barring_data_p->passwd));

            ss_p = encode_ss_string(&ss_data);
            if (!ss_p) {
               CN_LOG_E("encode_ss_string failed!");
               goto error;
            }

            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            memmove(pdc_input_p->service.ss.mmi_string, ss_p, strlen(ss_p));
            pdc_input_p->sat_initiated = FALSE;
            pdc_input_p->service.ss.type_of_number = CN_SS_TON_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.numbering_plan_id = CN_SS_NPI_NOT_A_DIALLING_NUMBER;
            pdc_input_p->service.ss.additional_results = FALSE;
            pdc_input_p->service_type = CN_SERVICE_TYPE_SS;
            free(ss_p);
            break;
        }
        case CN_REQUEST_USSD:
            NULL_CHECK_AND_GOTO_ERROR(data_p, "data_p");

            pdc_input_p->service_type = CN_SERVICE_TYPE_USSD;
            memmove(&pdc_input_p->service.ussd, data_p, sizeof(cn_ussd_info_t));
            pdc_input_p->sat_initiated = pdc_input_p->service.ussd.sat_initiated;
            CN_LOG_D("pdc_input_p->sat_initiated = %d",pdc_input_p->sat_initiated);
            break;
        default:
            CN_LOG_E("unknown request message!");
            pdc_input_p->service_type = CN_SERVICE_TYPE_UNKNOWN;
            break;
    }

    return pdc_input_p;

error:
    free(pdc_input_p);

    return NULL;
}



/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cn_log.h"
#include "cn_macros.h"
#include "cn_pdc_internal.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sim.h>

typedef struct {
    cn_message_type_t request_type;
    cn_message_type_t response_type;
} request_response_mapping_table_entry_t;

request_response_mapping_table_entry_t request_response_mapping_table[] = {
    {CN_REQUEST_DIAL,               CN_RESPONSE_DIAL},
    {CN_REQUEST_USSD,               CN_RESPONSE_USSD},
    {CN_REQUEST_CLIP_STATUS,        CN_RESPONSE_CLIP_STATUS},
    {CN_REQUEST_CNAP_STATUS,        CN_RESPONSE_CNAP_STATUS},
    {CN_REQUEST_COLR_STATUS,        CN_RESPONSE_COLR_STATUS},
    {CN_REQUEST_CLIR_STATUS,        CN_RESPONSE_CLIR_STATUS},
    {CN_REQUEST_QUERY_CALL_FORWARD, CN_RESPONSE_QUERY_CALL_FORWARD},
    {CN_REQUEST_SET_CALL_FORWARD,   CN_RESPONSE_SET_CALL_FORWARD},
    {CN_REQUEST_SET_CALL_WAITING,   CN_RESPONSE_SET_CALL_WAITING},
    {CN_REQUEST_GET_CALL_WAITING,   CN_RESPONSE_GET_CALL_WAITING},
    {CN_REQUEST_QUERY_CALL_BARRING, CN_RESPONSE_QUERY_CALL_BARRING},
    {CN_REQUEST_SET_CALL_BARRING,   CN_RESPONSE_SET_CALL_BARRING},
    {CN_REQUEST_SS,                 CN_RESPONSE_SS},
    {CN_REQUEST_UNKNOWN,            CN_REQUEST_UNKNOWN}, /* termination entry */
};

void initialize_pdc_data_in_request_record(void *data_p, request_record_t *record_p)
{
    NULL_CHECK_AND_GOTO_EXIT(record_p, "record_p");

    record_p->pdc_data_p = calloc(1, sizeof(cn_pdc_t));

    if (!record_p->pdc_data_p) {
        CN_LOG_E("calloc failed for record_p->pdc_data_p!");
        goto exit;
    }

    record_p->pdc_data_p->state = CN_PDC_STATE_ECC;

    record_p->pdc_data_p->pdc_input_p = cn_pdc_translate_message_to_pdc_input(record_p->message_type, data_p);

    if (!record_p->pdc_data_p->pdc_input_p) {
        CN_LOG_E("translate_message_to_pdc_input failed!");
        goto exit;
    }

    record_p->pdc_data_p->pdc_output_p = calloc(1, sizeof(cn_pdc_output_t));

    if (!record_p->pdc_data_p->pdc_output_p) {
        CN_LOG_E("calloc failed for record_p->pdc_data_p->pdc_output_p!");
        goto exit;
    }

    record_p->pdc_data_p->record_p = record_p; /* the request record is needed inside each PDC block that */
                                               /* doesn't (and shouldn't) have the record_p as input for  */
                                               /* semantic reasons.                                       */
exit:
    return;
}

void copy_input_data_to_output_data(cn_pdc_t *pdc_data_p)
{
    cn_pdc_input_t  *pdc_input_p  = NULL;
    cn_pdc_output_t *pdc_output_p = NULL;

    NULL_CHECK_AND_GOTO_EXIT(pdc_data_p, "pdc_data_p");
    NULL_CHECK_AND_GOTO_EXIT(pdc_data_p->pdc_input_p,  "pdc_data_p->pdc_input_p");
    NULL_CHECK_AND_GOTO_EXIT(pdc_data_p->pdc_output_p, "pdc_data_p->pdc_output_p");

    pdc_input_p  = pdc_data_p->pdc_input_p;
    pdc_output_p = pdc_data_p->pdc_output_p;

    pdc_output_p->service_type = pdc_input_p->service_type;
    pdc_output_p->emergency_call = false;

    switch (pdc_output_p->service_type) {
    case CN_SERVICE_TYPE_VOICE_CALL:
        pdc_output_p->service.voice_call = pdc_input_p->service.voice_call;
        break;
    case CN_SERVICE_TYPE_SS:
        pdc_output_p->service.ss = pdc_input_p->service.ss;
        break;
    case CN_SERVICE_TYPE_USSD:
        pdc_output_p->service.ussd = pdc_input_p->service.ussd;
        break;
    default:
        CN_LOG_E("pdc_output_p->service_type is unknown! (%d)", pdc_output_p->service_type);
        break;
    }

exit:
    return;
}


void translate_cat_cc_result_to_new_input_data_for_ecc_check(cn_sat_cc_result_struct_t *response_p, cn_pdc_t *pdc_data_p)
{
    cn_pdc_input_t  *pdc_input_p  = NULL;

    NULL_CHECK_AND_GOTO_EXIT(response_p, "response_p");
    NULL_CHECK_AND_GOTO_EXIT(pdc_data_p, "pdc_data_p");
    NULL_CHECK_AND_GOTO_EXIT(pdc_data_p->pdc_input_p,  "pdc_data_p->pdc_input_p");

    pdc_input_p  = pdc_data_p->pdc_input_p;

    switch (response_p->cc_type) {
    case STE_CAT_CC_CALL_SETUP:
        pdc_input_p->service_type = CN_SERVICE_TYPE_VOICE_CALL;
        pdc_input_p->service.voice_call.call_type     = CN_CALL_TYPE_VOICE_CALL; /* this will change if ECC is positive */
        /* use clir settings from user since it is not available from SAT */
        pdc_input_p->service.voice_call.sat_initiated = false; /* regardless if this is true of not, the field is no longer relevant */

        CN_LOG_D("response_p->ton: %d", response_p->ton);
        CN_LOG_D("response_p->npi: %d", response_p->npi);
        CN_LOG_D("response_p->text_coding: %d", response_p->text_coding);
        CN_LOG_D("response_p->no_of_characters: %d", response_p->no_of_characters);

        CN_LOG_D("pdc_input_p->service.voice_call.phone_number before update: \"%s\"", pdc_input_p->service.voice_call.phone_number);

        memset(pdc_input_p->service.voice_call.phone_number, 0, sizeof(pdc_input_p->service.voice_call.phone_number));

        if (STE_SIM_TON_INTERNATIONAL == response_p->ton) {
            pdc_input_p->service.voice_call.phone_number[0] = '+';
        }

        if (STE_SIM_BCD == response_p->text_coding) {
            int len = response_p->no_of_characters / 2;

            if (response_p->no_of_characters % 2) {
                len++;
            }

            char *tmp_p = convert_bcd_ascii(response_p->text_p, len);
            NULL_CHECK_AND_GOTO_EXIT(tmp_p, "tmp_p");

            (void)strncat(pdc_input_p->service.voice_call.phone_number,
                          tmp_p, response_p->no_of_characters * sizeof(char));

            free(tmp_p);
        } else if (STE_SIM_ASCII8 == response_p->text_coding) {
            (void)strncat(pdc_input_p->service.voice_call.phone_number,
                          response_p->text_p, response_p->no_of_characters * sizeof(char));
        } else {
            CN_LOG_E("Unknown call coding scheme (%d)", response_p->text_coding);
            goto exit;
        }

        CN_LOG_D("pdc_input_p->service.voice_call.phone_number after update: \"%s\"", pdc_input_p->service.voice_call.phone_number);
        break;
    case STE_CAT_CC_SS:
        pdc_input_p->service_type                  = CN_SERVICE_TYPE_SS;
        pdc_input_p->service.ss.mmi_string_length  = response_p->no_of_characters;
        pdc_input_p->service.ss.sat_initiated      = false; /* regardless if this is true of not, the field is no longer relevant */
        pdc_input_p->service.ss.ton_npi_used       = true;
        pdc_input_p->service.ss.type_of_number     = response_p->ton;
        pdc_input_p->service.ss.numbering_plan_id  = response_p->npi;

        CN_LOG_D("response_p->ton: %d", response_p->ton);
        CN_LOG_D("response_p->npi: %d", response_p->npi);
        CN_LOG_D("response_p->no_of_characters: %d", response_p->no_of_characters);
        CN_LOG_D("response_p->text_coding: %d", response_p->text_coding);

        memset(pdc_input_p->service.ss.mmi_string, 0, sizeof(pdc_input_p->service.ss.mmi_string));

        if (STE_SIM_BCD == response_p->text_coding) {
            int len = response_p->no_of_characters / 2;

            if (response_p->no_of_characters % 2) {
                len++;
            }

            char *tmp_p = convert_bcd_ascii(response_p->text_p, len);
            NULL_CHECK_AND_GOTO_EXIT(tmp_p, "tmp_p");
            memmove(pdc_input_p->service.ss.mmi_string,
                    tmp_p,
                    response_p->no_of_characters * sizeof(char));
            free(tmp_p);
        } else if (STE_SIM_ASCII8 == response_p->text_coding) {
            memmove(pdc_input_p->service.ss.mmi_string,
                    response_p->text_p,
                    response_p->no_of_characters * sizeof(char));
        } else {
            CN_LOG_E("Unknown SS coding scheme (%d)", response_p->text_coding);
            goto exit;
        }

        break;
    case STE_CAT_CC_USSD:
        pdc_input_p->service_type               = CN_SERVICE_TYPE_USSD;
        pdc_input_p->service.ussd.type          = CN_USSD_TYPE_USSD_REQUEST;
        pdc_input_p->service.ussd.received_type = CN_USSD_RECEIVED_TYPE_GSM_USSD_REQUEST;
        pdc_input_p->service.ussd.length        = response_p->no_of_characters;
        pdc_input_p->service.ussd.dcs           = response_p->dcs;
        pdc_input_p->service.ussd.session_id    = 0;
        pdc_input_p->service.ussd.sat_initiated = false;
        pdc_input_p->service.ussd.me_initiated  = 0;

        CN_LOG_D("response_p->no_of_characters: %d", response_p->no_of_characters);

        memmove(pdc_input_p->service.ussd.ussd_string, response_p->text_p,
                response_p->no_of_characters);
        break;
    default:
        CN_LOG_E("unknown service type, response_p->cc_type: %d", response_p->cc_type);
        break;
    }

exit:
    return;
}

char *convert_bcd_ascii(char *bcd_p, int len)
{
    char *ascii_p = calloc(1, len * 2 + 1);
    char *tmp_p = ascii_p;
    int i = 0;

    if (NULL == ascii_p) {
        goto exit;
    }

    for (i = 0; i < len; i++) {
        int a = ((bcd_p[i] &  0xF0) >> 4);
        int b = bcd_p[i] &  0x0F;

        switch (b) {
        case 0xA:
            sprintf(tmp_p, "%s", "*");
            break;
        case 0xB:
            sprintf(tmp_p, "%s", "#");
            break;
        case 0xC:
            sprintf(tmp_p, "%s", "p");
            break;
        case 0xD:
            break;
        case 0xF:
            break;
        default:
            sprintf(tmp_p, "%d", b);
            break;
        }

        switch (a) {
        case 0xA:
            sprintf(tmp_p + 1, "%s", "*");
            break;
        case 0xB:
            sprintf(tmp_p + 1, "%s", "#");
            break;
        case 0xC:
            sprintf(tmp_p + 1, "%s", "p");
            break;
        case 0xD:
            break;
        case 0xF:
            break;
        default:
            sprintf(tmp_p + 1, "%d", a);
        }

        tmp_p = tmp_p + 2;
    }

exit:
    return ascii_p;
}

cn_message_type_t get_response_message_type_from_request(cn_message_type_t type)
{
    cn_message_type_t msg_type = CN_RESPONSE_UNKNOWN;
    cn_uint32_t i = 0;

    while (request_response_mapping_table[i].request_type != CN_REQUEST_UNKNOWN) {
        if (request_response_mapping_table[i].request_type == type) {
            msg_type = request_response_mapping_table[i].response_type;
            break;
        }

        i++;
    }

    return msg_type;
}

cn_ss_procedure_type_t convert_call_forward_status_to_procedure_type(cn_uint32_t status)
{
    cn_ss_procedure_type_t procedure;

    switch (status) {
    case 0:
        procedure = CN_SS_PROCEDURE_TYPE_DEACTIVATION;
        break;
    case 1:
        procedure = CN_SS_PROCEDURE_TYPE_ACTIVATION;
        break;
    case 2:
        procedure = CN_SS_PROCEDURE_TYPE_INTERROGATION;
        break;
    case 3:
        procedure = CN_SS_PROCEDURE_TYPE_REGISTRATION;
        break;
    case 4:
        procedure = CN_SS_PROCEDURE_TYPE_ERASURE;
        break;
    default:
        procedure = CN_SS_PROCEDURE_TYPE_UNKNOWN;
        break;
    }

    return procedure;
}


cn_sint32_t cn_util_convert_cn_service_class_to_ss_class(cn_sint32_t cn_service_class)
{
    cn_sint32_t ss_class = 0;

    switch (cn_service_class) {
    case 15:
        ss_class = 0;
        break;
    case 7:
        ss_class = 1;
        break;
    case 1:
        ss_class = 11;
        break;
    case 2:
        ss_class = 20;
        break;
    case 4:
        ss_class = 13;
        break;
    case 5:
        ss_class = 19;
        break;
    case 8:
        ss_class = 16;
        break;
    case 12:
        ss_class = 12;
        break;
    case 13:
        ss_class = 10;
        break;
    case 16:
        ss_class = 24;
        break;
    case 32:
        ss_class = 25;
        break;
    case 64: /* Included in 3GPP TS 02.30, not in 3GPP TS 22.030 */
        ss_class = 26;
        break;
    case 80:
        ss_class = 22;
        break;
    case 128: /* Included in 3GPP TS 02.30, not in 3GPP TS 22.030 */
        ss_class = 27;
        break;
    case 160:
        ss_class = 21;
        break;
    default:
        ss_class = 0;
        break;
    }

    CN_LOG_D("cn_service_class: %d --> ss_class: %d", cn_service_class, ss_class);

    return ss_class;
}

cn_ss_type_t convert_reason_to_cn_cf_ss_type(cn_uint32_t reason)
{
    cn_ss_type_t ss_type = CN_SS_TYPE_UNKNOWN;


    switch (reason) {
    case 0:
        ss_type = CN_SS_TYPE_CALL_FORWARDING_UNCONDITIONAL;
        break;
    case 1:
        ss_type = CN_SS_TYPE_CALL_FORWARDING_BUSY;
        break;
    case 2:
        ss_type = CN_SS_TYPE_CALL_FORWARDING_NO_REPLY;
        break;
    case 3:
        ss_type = CN_SS_TYPE_CALL_FORWARDING_NOT_REACHABLE;
        break;
    case 4:
        ss_type = CN_SS_TYPE_CALL_FORWARDING_ALL;
        break;
    case 5:
        ss_type = CN_SS_TYPE_CALL_FORWARDING_ALL_CONDITIONAL;
        break;
    default:
        ss_type = CN_SS_TYPE_UNKNOWN;
        break;
    }

    return ss_type;
}

cn_ss_type_t convert_facility_to_cn_call_barring_ss_type(char *facility_p)
{
    cn_ss_type_t ss_type;

    CN_LOG_D("facility code = %s", facility_p);

    if (strcmp(facility_p, "AO") == 0) {
        ss_type = CN_SS_TYPE_CALL_BARRING_BAOC;
    } else if (strcmp(facility_p, "OI") == 0) {
        ss_type = CN_SS_TYPE_CALL_BARRING_BAOIC;
    } else if (strcmp(facility_p, "OX") == 0) {
        ss_type = CN_SS_TYPE_CALL_BARRING_BAOIC_EXCL_HOME;
    } else if (strcmp(facility_p, "AI") == 0) {
        ss_type = CN_SS_TYPE_CALL_BARRING_BAIC;
    } else if (strcmp(facility_p, "IR") == 0) {
        ss_type = CN_SS_TYPE_CALL_BARRING_BAIC_ROAMING;
    } else if (strcmp(facility_p, "AB") == 0) {
        ss_type = CN_SS_TYPE_CALL_BARRING_ALL;
    } else if (strcmp(facility_p, "AG") == 0) {
        ss_type = CN_SS_TYPE_CALL_BARRING_ALL_OUTGOING;
    } else if (strcmp(facility_p, "AC") == 0) {
        ss_type = CN_SS_TYPE_CALL_BARRING_ALL_INCOMING;
    } else {
        ss_type = CN_SS_TYPE_UNKNOWN;
        CN_LOG_E("Recvd Invalid/Unsupported Barring string code %s \n", facility_p);
    }


    return ss_type;
}

cn_ss_ton_t cn_util_convert_ss_toa_to_cn_ss_toa(int type)
{
    /* See TS 24.008 10.5.4.7 and definition of cn_ss_ton_t */
    return ((type & 0x70) >> 4);
}


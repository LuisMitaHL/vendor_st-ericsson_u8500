/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <unistd.h>
#include "cn_pdc_internal.h"
#include "cn_macros.h"
#include "request_handling.h"
#include "sim_client.h"


request_status_t cn_pdc_handle_request(void *data_p, request_record_t *record_p)
{
    cn_pdc_input_t  *pdc_input_p = NULL;
    cn_pdc_output_t *pdc_output_p = NULL;
    request_status_t request_status = REQUEST_STATUS_ERROR;
    cn_error_code_t  err = CN_FAILURE;

    NULL_CHECK_AND_GOTO_ERROR(record_p, "record_p");

    if (!record_p->pdc_data_p) {

#ifdef REMOVE_PLUS_IN_DIAL_STRING
        /*For some networks dial string starting with '*+' should trigger one of their services.
                 CALL_MODEM_CREATE_REQ with '+' in the string is rejected by the modem.
                 Hence, '+' character is removed from the string before dial request is sent.*/
        if(record_p->message_type == CN_REQUEST_DIAL){
            cn_request_dial_t *request_data_p = (cn_request_dial_t *) data_p;
            NULL_CHECK_AND_GOTO_ERROR(request_data_p, "request_data_p");
            CN_LOG_D("Received dial string : %s", request_data_p->dial.phone_number);

            if ((request_data_p->dial.phone_number[0] == '*') && (request_data_p->dial.phone_number[1] == '+')){
                int number_len = strlen (request_data_p->dial.phone_number);
                memcpy(request_data_p->dial.phone_number+1,request_data_p->dial.phone_number+2, number_len);
                CN_LOG_D("Modified dial string : %s", request_data_p->dial.phone_number);
            }
        }
#endif
        initialize_pdc_data_in_request_record(data_p, record_p); /* CN_PDC_STATE_ECC */
    }


#ifdef ENABLE_MODULE_TEST
    extern cn_bool_t g_tf_pdc_check_enabled;

    if (!g_tf_pdc_check_enabled) {
        CN_LOG_D("skipping pre-dial checks");
        copy_input_data_to_output_data(record_p->pdc_data_p);
        CN_LOG_D("goto pre_dial_check_passed");
        goto pre_dial_check_passed;
    }

#endif

    /******************** PRE-DIAL CHECKING START *************/
    pdc_input_p  = record_p->pdc_data_p->pdc_input_p;
    pdc_output_p = record_p->pdc_data_p->pdc_output_p;

    switch (record_p->pdc_data_p->state) {
    case CN_PDC_STATE_ECC: { /* ECC check */
        cn_ecc_result_t  ecc_result = CN_ECC_RESULT_UNKNOWN;

        CN_LOG_D("CN_PDC_STATE_ECC");

        ecc_result = cn_pdc_ecc_check(record_p->pdc_data_p);

        switch (ecc_result) {
        case CN_ECC_RESULT_PENDING:
            CN_LOG_D("CN_EEC_RESULT_PENDING");
            goto pending;
        case CN_ECC_RESULT_EMERGENCY_CALL:
            CN_LOG_D("CN_ECC_RESULT_EMERGENCY_CALL");
            pdc_output_p->service_type = CN_SERVICE_TYPE_VOICE_CALL;
            pdc_output_p->emergency_call = true;
            pdc_output_p->service.voice_call = pdc_input_p->service.voice_call;
            pdc_output_p->service.voice_call.call_type = CN_CALL_TYPE_EMERGENCY_CALL;
            goto pre_dial_check_passed;
        case CN_ECC_RESULT_NO_EMERGENCY_CALL:
        case CN_ECC_RESULT_CHECK_NOT_RELEVANT: /* not a voice call */
            CN_LOG_D("CN_ECC_RESULT_NO_EMERGENCY_CALL || CN_ECC_RESULT_CHECK_NOT_RELEVANT");
            record_p->pdc_data_p->state = CN_PDC_STATE_FDN;
            goto cn_pdc_state_fdn;
        case CN_ECC_RESULT_ERROR:
        default:
            CN_LOG_E("cn_pdc_ecc_check failed, aborting PDC!");
            goto error;
        }

        break;
    }
    case CN_PDC_STATE_FDN: /* FDN check */
cn_pdc_state_fdn: {
            cn_fdn_result_t  fdn_result   = CN_FDN_RESULT_UNKNOWN;

            CN_LOG_D("CN_PDC_STATE_FDN");

            fdn_result = cn_pdc_fdn_check(record_p->pdc_data_p);

            switch (fdn_result) {
            case CN_FDN_RESULT_PENDING:
                CN_LOG_D("CN_FDN_RESULT_PENDING");
                goto pending;
            case CN_FDN_RESULT_REJECT:
                CN_LOG_D("CN_FDN_RESULT_REJECT");
                CN_LOG_E("service rejected by FDN checker!");
                err = CN_REQUEST_FDN_BLOCKED;
                goto error;
            case CN_FDN_RESULT_ACCEPT:
            case CN_FDN_RESULT_CHECK_NOT_RELEVANT: /* SAT initiated, not applicable SS or FDN not enabled */
                CN_LOG_D("CN_FDN_RESULT_ACCEPT || CN_FDN_RESULT_CHECK_NOT_RELEVANT");
                record_p->pdc_data_p->state = CN_PDC_STATE_SAT_CC;
                goto cn_pdc_state_sat_cc;
            case CN_FDN_RESULT_ERROR:
            default:
                CN_LOG_E("service rejected by FDN checker!");
                goto error;
            }

            break;
        }
    case CN_PDC_STATE_SAT_CC: /* SAT CC */
cn_pdc_state_sat_cc: {
            cn_sat_cc_result_struct_t sat_cc_result_struct;
            memset(&sat_cc_result_struct, 0, sizeof(cn_sat_cc_result_struct_t));

            CN_LOG_D("CN_PDC_STATE_SAT_CC");

            cn_pdc_sat_cc(record_p->pdc_data_p, &sat_cc_result_struct);

            switch (sat_cc_result_struct.result) {
            case CN_SAT_CC_RESULT_PENDING:
                CN_LOG_D("CN_SAT_CC_RESULT_PENDING");
                goto pending;
            case CN_SAT_CC_RESULT_DONE:
                CN_LOG_D("CN_SAT_CC_RESULT_DONE");

                if (STE_CAT_CC_NOT_ALLOWED == sat_cc_result_struct.cc_result) {
                    CN_LOG_D("STE_CAT_CC_NOT_ALLOWED");
                    err = CN_REQUEST_CC_REJECTED;
                    goto error;
                } else if (STE_CAT_CC_NOT_ALLOWED_TEMP_PROBLEM == sat_cc_result_struct.cc_result) {
                    CN_LOG_D("STE_CAT_CC_NOT_ALLOWED_TEMP");
                    err = CN_REQUEST_MODEM_NOT_READY;
                    goto error;
                } else if (STE_CAT_CC_ALLOWED_NO_MODIFICATION == sat_cc_result_struct.cc_result) {
                    CN_LOG_D("STE_CAT_CC_ALLOWED_NO_MODIFICATION");
                    copy_input_data_to_output_data(record_p->pdc_data_p);
                    goto pre_dial_check_passed;
                } else if (STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS == sat_cc_result_struct.cc_result) {
                    CN_LOG_D("STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS");

                    if (sat_cc_result_struct.cc_type != pdc_input_p->service_type) {
                        CN_LOG_D("Service is changed");

                        switch (sat_cc_result_struct.cc_type) {
                        case STE_CAT_CC_CALL_SETUP:
                            err = CN_REQUEST_CC_SERVICE_MODIFIED_CALL;
                            break;
                        case STE_CAT_CC_SS:
                            err = CN_REQUEST_CC_SERVICE_MODIFIED_SS;
                            break;
                        case STE_CAT_CC_USSD:
                            err = CN_REQUEST_CC_SERVICE_MODIFIED_USSD;
                            break;
                        case STE_CAT_CC_TYPE_UNKNOWN:
                            /* Fall trough*/
                        default:
                            err = CN_FAILURE;
                            goto error;
                        }

                        /* For non-SAT initiated requests we only want the first response to be sent to the client
                         * (for modification of service request) as this may otherwise cause unexpected behavior.
                         * This is done by setting the client_id in the record structure to -1. Then the cns lookup the
                         * client and will discard the response.
                         *
                         * For SAT initiated request we only want the second response to be sent to the client
                         * therefore we have this if statement. This response will only be sent when CN USSD
                         * request is done. Will use the client tag from the dial request.
                         *
                         * First response means response on the dial request and the second response means
                         * on the service request (USSD).
                         */
                        if (!pdc_input_p->sat_initiated) {
                            send_response(record_p->client_id, get_response_message_type_from_request(record_p->message_type), err, record_p->client_tag, 0, NULL);
                            record_p->client_id = -1;
                        }

                        /* We will continue so reset error variable */
                        err = CN_FAILURE;

                    }

                    translate_cat_cc_result_to_new_input_data_for_ecc_check(&sat_cc_result_struct, record_p->pdc_data_p);
                    record_p->pdc_data_p->pdc_ecc_state = CN_PDC_ECC_STATE_POST_SAT_CC;
                    record_p->pdc_data_p->state = CN_PDC_STATE_ECC_AFTER_SAT_CC;
                    goto cn_pdc_state_ecc_after_sat_cc;
                } else {
                    CN_LOG_E("unknown SAT CC result, cc_result: %d", sat_cc_result_struct.cc_result);
                    goto error;
                }

            case CN_SAT_CC_RESULT_ERROR:
            default:
                CN_LOG_E("service rejected by SAT CC!");
                goto error;
            }

            break;
        }
    case CN_PDC_STATE_ECC_AFTER_SAT_CC: /* ECC after SAT CC */
cn_pdc_state_ecc_after_sat_cc: {
            cn_ecc_result_t  ecc_result = CN_ECC_RESULT_UNKNOWN;

            CN_LOG_D("CN_PDC_STATE_ECC_AFTER_SAT_CC");

            ecc_result = cn_pdc_ecc_check(record_p->pdc_data_p);

            switch (ecc_result) {
            case CN_ECC_RESULT_EMERGENCY_CALL:
                CN_LOG_D("CN_ECC_RESULT_EMERGENCY_CALL");
                pdc_output_p->service_type = CN_SERVICE_TYPE_VOICE_CALL;
                pdc_output_p->emergency_call = true;
                pdc_output_p->service.voice_call = pdc_input_p->service.voice_call;
                pdc_output_p->service.voice_call.call_type = CN_CALL_TYPE_EMERGENCY_CALL;
                goto pre_dial_check_passed;
            case CN_ECC_RESULT_NO_EMERGENCY_CALL:
            case CN_ECC_RESULT_CHECK_NOT_RELEVANT: /* not a voice call */
                CN_LOG_D("CN_ECC_RESULT_NO_EMERGENCY_CALL || CN_ECC_RESULT_CHECK_NOT_RELEVANT");
                copy_input_data_to_output_data(record_p->pdc_data_p);
                goto pre_dial_check_passed;
            case CN_ECC_RESULT_ERROR:
            case CN_ECC_RESULT_PENDING: /* after SAT there is no case where pending should be allowed */
            default:
                CN_LOG_E("cn_pdc_ecc_check failed!");
                goto error;
            }

            break;
        }
    default:
        CN_LOG_E("unknown PDC state!");
        goto error;
    }

    /******************** PRE-DIAL CHECKING END *************/
pre_dial_check_passed:
    CN_LOG_D("pre_dial_check_passed:");

    /* Delegate execution of the service to respective request handler */

    pdc_output_p = record_p->pdc_data_p->pdc_output_p;

    switch (pdc_output_p->service_type) {
    case CN_SERVICE_TYPE_VOICE_CALL: {
        cn_request_dial_t dial_request;
        dial_request.dial = pdc_output_p->service.voice_call;
        record_p->request_handler_p = handle_request_dial;

        request_status = handle_request_dial(&dial_request, record_p);

        if (REQUEST_STATUS_PENDING != request_status) {
            CN_LOG_D("freeing record");
            request_record_free(record_p);
        }

        break;
    }
    case CN_SERVICE_TYPE_SS: {
        cn_request_ss_command_t ss_request;

        ss_request.ss_command = pdc_output_p->service.ss;
        record_p->request_handler_p = handle_request_ss_command;
        request_status = handle_request_ss_command(&ss_request, record_p);

        if (REQUEST_STATUS_PENDING != request_status) {
            CN_LOG_D("freeing record");
            request_record_free(record_p);
        }

        break;
    }
    case CN_SERVICE_TYPE_USSD:  {
        cn_request_ussd_t ussd_request;

        ussd_request.ussd_info = pdc_output_p->service.ussd;
        record_p->request_handler_p = handle_request_ussd;
        request_status = handle_request_ussd(&ussd_request, record_p);

        if (REQUEST_STATUS_PENDING != request_status) {
            CN_LOG_D("freeing record");
            request_record_free(record_p);
        }

        break;
    }
    default:
        CN_LOG_E("unknown service type, cannot execute service!");
        goto error;
    }

    if (record_p->pdc_data_p) {
        if (record_p->pdc_data_p->pdc_input_p)  {
            free(record_p->pdc_data_p->pdc_input_p);
        }

        if (record_p->pdc_data_p->pdc_output_p) {
            free(record_p->pdc_data_p->pdc_output_p);
        }

        free(record_p->pdc_data_p);
    }

    return request_status;

pending:
    return REQUEST_STATUS_PENDING;

error:
    send_response(record_p->client_id, get_response_message_type_from_request(record_p->message_type), err, record_p->client_tag, 0, NULL);

    if (record_p->pdc_data_p) {
        free(record_p->pdc_data_p);
    }

    if (pdc_input_p) {
        free(pdc_input_p);
    }

    if (pdc_output_p) {
        free(pdc_output_p);
    }

    return REQUEST_STATUS_ERROR;
}

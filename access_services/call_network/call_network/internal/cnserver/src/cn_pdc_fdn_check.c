/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "cn_pdc_ecc_list_handling.h"

#include "cn_macros.h"
#include "cn_pdc_internal.h"
#include "sim_client.h"
#include "str_convert.h"

cn_fdn_result_t cn_pdc_fdn_check(cn_pdc_t *pdc_p)
{
    ste_sim_t *sim_p = NULL;
    uicc_request_status_t result = 0;
    unsigned int i = 0;
    unsigned int length = 0;

    ste_uicc_get_service_availability_response_t *ust_resp_p = NULL;
    ste_uicc_get_service_table_response_t *est_resp_p = NULL;
    ste_uicc_sim_fdn_response_t *fdn_resp_p = NULL;

    if (!pdc_p || !(pdc_p->pdc_input_p)) {
        CN_LOG_E("pdc_input_p is NULL!");
        goto error;
    }

    CN_LOG_D("Enter, state = %d", pdc_p->pdc_fdn_state);

    if (pdc_p->pdc_input_p->sat_initiated) {
        CN_LOG_D("String i SAT initiated. FDN check not relevant.");
        return CN_FDN_RESULT_CHECK_NOT_RELEVANT;
    }

    sim_p = sim_client_get_handle();

    if (!sim_p) {
        CN_LOG_E("sim_client_get_handle failed!");
        goto error;
    }

    switch (pdc_p->pdc_fdn_state) {
    case CN_PDC_FDN_STATE_INITIAL:
        /* Call SIM to find out if FDN is supported in the sim. */
        result = ste_uicc_get_service_availability(sim_p, (uintptr_t) request_record_get_modem_tag(pdc_p->record_p), SIM_SERVICE_TYPE_FDN);

        if ((UICC_REQUEST_STATUS_PENDING != result) && (UICC_REQUEST_STATUS_OK
                != result)) {
            CN_LOG_E("SIM call ste_uicc_sim_file_read_ecc failed!");
            goto sim_error;
        }

        pdc_p->pdc_fdn_state = CN_PDC_FDN_STATE_WAIT_FOR_UST_RESPONSE;
        return CN_FDN_RESULT_PENDING;
        break;
    case CN_PDC_FDN_STATE_WAIT_FOR_UST_RESPONSE:

        if (!(pdc_p->record_p) || !(pdc_p->record_p->response_data_p)) {
            CN_LOG_E("SIM response error");
            goto sim_error;
        }

        ust_resp_p = (ste_uicc_get_service_availability_response_t *)(pdc_p->record_p->response_data_p);

        if (STE_UICC_STATUS_CODE_OK != ust_resp_p->uicc_status_code) {
            CN_LOG_E("Call to ste_uicc_get_service_availability returned with code: %d", ust_resp_p->uicc_status_code);
            goto sim_error;
        } else if (STE_UICC_SERVICE_AVAILABLE != ust_resp_p->service_availability) {
            /* If response is that FDN check is not supported, return CN_FDN_RESULT_ACCEPT */
            CN_LOG_W("FDN check is not supported, CN_FDN_RESULT_ACCEPT returned");
            return CN_FDN_RESULT_ACCEPT;
        } else {
            /* Call was ok and FDN was supported. Call read of EST file to see if it is enabled. */
            CN_LOG_D("UST response was OK. Request service table.");
            result = ste_uicc_get_service_table(sim_p,
                                                (uintptr_t) request_record_get_modem_tag(pdc_p->record_p),
                                                SIM_SERVICE_TYPE_FDN);

            if ((UICC_REQUEST_STATUS_PENDING != result) && (UICC_REQUEST_STATUS_OK
                    != result)) {
                CN_LOG_E("SIM call ste_uicc_get_service_table failed!");
                goto sim_error;
            }

            pdc_p->pdc_fdn_state = CN_PDC_FDN_STATE_WAIT_FOR_EST_RESPONSE;
            return CN_FDN_RESULT_PENDING;
            break;
        }

        break;
    case CN_PDC_FDN_STATE_WAIT_FOR_EST_RESPONSE:

        if (!(pdc_p->record_p) || !(pdc_p->record_p->response_data_p)) {
            CN_LOG_E("SIM response error");
            goto sim_error;
        }

        est_resp_p = (ste_uicc_get_service_table_response_t *)(pdc_p->record_p->response_data_p);

        if (STE_UICC_STATUS_CODE_OK != est_resp_p->uicc_status_code) {
            CN_LOG_E("Call to ste_uicc_get_service_table returned with code: %d", est_resp_p->uicc_status_code);
            goto sim_error;
        } else if (STE_UICC_SERVICE_STATUS_ENABLED != est_resp_p->service_status) {
            /* If response is that FDN check is not enabled, return CN_FDN_RESULT_ACCEPT */
            CN_LOG_W("FDN check is not enabled, CN_FDN_RESULT_ACCEPT returned");
            return CN_FDN_RESULT_ACCEPT;
        } else {
            /* Call was ok and FDN was enabled. Call read of FDN file to get the white-list to compare with. */
            result = ste_uicc_sim_file_read_fdn(sim_p,
                                                (uintptr_t) request_record_get_modem_tag(pdc_p->record_p));

            if ((UICC_REQUEST_STATUS_PENDING != result) && (UICC_REQUEST_STATUS_OK
                    != result)) {
                CN_LOG_E("SIM call ste_uicc_sim_file_read_fdn failed!");
                goto sim_error;;
            }

            pdc_p->pdc_fdn_state = CN_PDC_FDN_STATE_WAIT_FOR_FDN_LIST_RESPONSE;
            return CN_FDN_RESULT_PENDING;
            break;
        }

        break;
    case CN_PDC_FDN_STATE_WAIT_FOR_FDN_LIST_RESPONSE:

        /* Compare to what the FDN response was. Take appropriate actions. */
        if (!(pdc_p->record_p) || !(pdc_p->record_p->response_data_p)) {
            CN_LOG_E("SIM response error");
            goto sim_error;
        }

        fdn_resp_p = (ste_uicc_sim_fdn_response_t *)(pdc_p->record_p->response_data_p);

        if (STE_UICC_STATUS_CODE_OK != fdn_resp_p->uicc_status_code) {
            /* Call failed. */
            CN_LOG_E("SIM response to ste_uicc_sim_file_read_fdn was %d" , fdn_resp_p->uicc_status_code);
            goto sim_error;
        } else {
            /* Call successful. Perform comparison operations:*/
            CN_LOG_D("Retrieved %d FDN records from SIM.", fdn_resp_p->number_of_records);

            /* Get number that should be checked, location is different depending on type of operation */
            char number[CN_MAX_STRING_BUFF+1];
            memset(number, 0, sizeof(number));

            switch (pdc_p->pdc_input_p->service_type) {
            case CN_SERVICE_TYPE_VOICE_CALL:
                memmove(number, pdc_p->pdc_input_p->service.voice_call.phone_number, strlen(pdc_p->pdc_input_p->service.voice_call.phone_number));
                break;
            case CN_SERVICE_TYPE_SS:
                /* cn_ss_command_t allows non-terminated strings to be defined. In that case         *
                 *  mmi_string_length is different from zero. This is taken into consideration below */
                length = (0 != pdc_p->pdc_input_p->service.ss.mmi_string_length) ?
                         pdc_p->pdc_input_p->service.ss.mmi_string_length : strlen((const char *)pdc_p->pdc_input_p->service.ss.mmi_string);
                memmove(number, pdc_p->pdc_input_p->service.ss.mmi_string, length * sizeof(cn_uint8_t));
                break;
            case CN_SERVICE_TYPE_USSD:
                CN_LOG_D("DCS of dialled string:%d", pdc_p->pdc_input_p->service.ussd.dcs);
                CN_LOG_D("Dialled string: %s", pdc_p->pdc_input_p->service.ussd.ussd_string);
                size_t to_size = sizeof(number);

                if (pdc_p->pdc_input_p->service.ussd.dcs < 16 || !(pdc_p->pdc_input_p->service.ussd.dcs & 0x0c)) {
                    if (str_convert_string(CHARSET_GSM_DEFAULT_7_BIT, pdc_p->pdc_input_p->service.ussd.length, pdc_p->pdc_input_p->service.ussd.ussd_string, 0,
                                           CHARSET_US_ASCII, &to_size, (uint8_t *) number) < 0) {
                        CN_LOG_E("unable to convert GSM7 bit string to US ASCII string.");
                        goto error;
                    }
                } else {
                    memmove(number, pdc_p->pdc_input_p->service.ussd.ussd_string, pdc_p->pdc_input_p->service.ussd.length);
                }

                break;
            default:
                CN_LOG_E("Unknown type of service, unable to determine number string.");
                goto error;
            }

            for (i = 0; i < fdn_resp_p->number_of_records; i++) {
                ste_uicc_sim_fdn_record_t *number_to_test = &(fdn_resp_p->fdn_record_p[i]);

                /* Logging and debug functionality: */
                if (STE_SIM_ASCII8 == number_to_test->dial_string.text_coding) {
                    char fdn_entry[CN_MAX_STRING_BUFF+1]; /* for printout */
                    memset(fdn_entry, 0, sizeof(fdn_entry));
                    memmove(fdn_entry, number_to_test->dial_string.text_p, number_to_test->dial_string.no_of_characters * sizeof(char));

                    CN_LOG_D("Number from SIM to perform FDN check on:");
                    CN_LOG_D("dial_string contains: text_coding=%d, no_of_characters=%d and text_p=%s",
                             number_to_test->dial_string.text_coding,
                             number_to_test->dial_string.no_of_characters,
                             (char *)fdn_entry);
                } else {
                    CN_LOG_W("unknown text coding=%d!", number_to_test->dial_string.text_coding);
                }

                /* End of logging and debug functionality. */

                if (cn_pdc_fdn_equal_numbers(number, (char *)(number_to_test->dial_string.text_p),
                                             number_to_test->dial_string.no_of_characters, number_to_test->ton)) {
                    return CN_FDN_RESULT_ACCEPT;
                }
            }

            return CN_FDN_RESULT_REJECT;
        }

        break;
    case CN_PDC_FDN_STATE_ERROR:
    default:
        goto error;
        break;
    }

sim_error:
    /* Design: A call is accepted until proven otherwise, i.e. if at any time SIM does not give
     * the correct responses regarding service available and activated in UST/EST the call will
     * be accepted. */
    return CN_FDN_RESULT_ACCEPT;

error:
    /* TODO: At least for debugging, since there might be so many issues with sim and how the responses look,
     * call should be accepted at this level in order to pass on through the tests. */
    return CN_FDN_RESULT_ERROR;
}

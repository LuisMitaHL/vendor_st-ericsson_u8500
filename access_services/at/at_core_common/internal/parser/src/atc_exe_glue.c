/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <atc_exe_glue.h>
#include <atc_hand_net.h>
#include <atc_handlers.h>
#include <atc_log.h>
#include <atc_parser.h>
#include <exe.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


static exe_t *exe_handle = NULL;

static void store_exe_handle(exe_t *exe_p)
{
    exe_handle = exe_p;
}

exe_t *atc_get_exe_handle(void)
{
    return exe_handle;
}

void atc_exe_final_response_callback(void *client_data_p, exe_request_result_t result, void *response_p)
{
    AT_ParserState_s *parser = (AT_ParserState_s *) client_data_p;
    struct AT_ResponseMessage_s response_message;
    response_message.data_p = response_p;
    response_message.len = 0; /* Only used in transparent mode */
    response_message.exe_result_code = result;
    (void)Parser_ParseCommandLine(parser, (AT_ResponseMessage_t)&response_message);
}

/*
 * at_ex_event_callback()
 *
 * Filters so that at least one channel is listening for this response and sends the
 * call back to the correct handle in that case.
 *
 */

void atc_exe_event_callback(exe_event_t event, void *response_p)
{

    switch (event) {
    case EXE_UNSOLICITED_ESIMSR: {
        handle_incoming_ESIMSR_event((exe_esimsr_sim_state_t *) response_p);
        break;
    }
    case EXE_EVENT_CALL_RESPONSE: {
        handle_incoming_call_response_event((exe_call_response_t *)response_p);
        break;
    }
    case EXE_UNSOLICITED_CGEV: {
        handle_incoming_CGEV_event((exe_cgev_response_t *) response_p);
        break;
    }
    case EXE_UNSOLICITED_ECAV: {
        handle_incoming_ECAV_event((exe_ecav_response_t *) response_p);
        break;
    }
    case EXE_UNSOLICITED_CLIP: {
        handle_incoming_CLIP_event((exe_clip_response_t *) response_p);
        break;
    }
    case EXE_UNSOLICITED_CNAP: {
        handle_incoming_CNAP_event((exe_cnap_response_t *) response_p);
        break;
    }
    case EXE_UNSOLICITED_CCWA: {
        handle_incoming_CCWA_event((exe_ccwa_response_t *) response_p);
        break;
    }
    case EXE_UNSOLICITED_CSSI: {
        handle_incoming_CSSI_event((exe_cssi_response_t *) response_p);
        break;
    }
    case EXE_UNSOLICITED_CSSU: {
        handle_incoming_CSSU_event((exe_cssu_response_t *) response_p);
        break;
    }
    case EXE_UNSOLICITED_CREG: {
        handle_incoming_creg_event(response_p);
        break;
    }
    case EXE_UNSOLICITED_EREG:
        handle_incoming_ereg_event(response_p);
        break;
    case EXE_UNSOLICITED_CGREG: {
        handle_incoming_cgreg_event(response_p);
        break;
    }
    case EXE_UNSOLICITED_CMT: {
        handle_incoming_cmt_event((exe_sms_mt_t *)response_p);
        break;
    }
    case EXE_UNSOLICITED_CMTI: {
        handle_incoming_cmti_event((exe_sms_mt_t *)response_p);
        break;
    }
    case EXE_UNSOLICITED_CBM: {
        handle_incoming_cbm_event((exe_cbm_t *)response_p);
        break;
    }
    case EXE_UNSOLICITED_CDS: {
        handle_incoming_cds_event((exe_sms_status_report_t *)response_p);
        break;
    }
    case EXE_UNSOLICITED_CDSI: {
        handle_incoming_cdsi_event((exe_sms_status_report_t *)response_p);
        break;
    }
    case EXE_UNSOLICITED_RING:
        handle_incoming_RING_event((exe_call_type_t *)response_p);
        break;

    case EXE_UNSOLICITED_ETZV:
        handle_incoming_ETZV_event((exe_etzv_response_t *)response_p);
        break;

    case EXE_UNSOLICITED_ECME:
        handle_incoming_ecme_event((exe_ecme_response_t *)response_p);
        break;

    case EXE_UNSOLICITED_ECRAT:
        handle_incoming_ecrat_event((exe_ecrat_ind_t *)response_p);
        break;

    case EXE_UNSOLICITED_ENNIR:
        handle_incoming_ennir_event((exe_ennir_ind_t *)response_p);
        break;

    case EXE_UNSOLICITED_ELAT:
        handle_incoming_elat_event((exe_elat_response_t *)response_p);
        break;

    case EXE_UNSOLICITED_SAT_EVENT_NOTIFY:
        handle_incoming_stkn_event((exe_apdu_data_t *)response_p);
        break;

    case EXE_UNSOLICITED_SAT_PROACTIVE_COMMAND:
        handle_incoming_stki_event((exe_apdu_data_t *)response_p);
        break;

    case EXE_UNSOLICITED_SAT_SESSION_COMPLETED:
        handle_incoming_stkend_event();
        break;

    case EXE_UNSOLICITED_ERFSTATE:
        handle_incoming_erfstate_event((exe_erfstate_response_t *)response_p);
        break;

    case EXE_UNSOLICITED_ESIMRF:
        handle_incoming_esimrf_event((exe_esimrf_unsol_t *)response_p);
        break;

    case EXE_UNSOLICITED_EPEV:
        handle_incoming_epev_event();
        break;

    case EXE_UNSOLICITED_CUSD:
        handle_incoming_CUSD_event((exe_cusd_response_t *)response_p);
        break;

    case EXE_UNSOLICITED_CUUS1I:
        handle_incoming_CUUS1I_event((exe_cuus1_response_t *)response_p);
        break;

    case EXE_UNSOLICITED_CUUS1U:
        handle_incoming_CUUS1U_event((exe_cuus1_response_t *)response_p);
        break;

    case EXE_UNSOLICITED_RSSI:
        handle_incoming_RSSI_event(response_p);
        break;

    case EXE_UNSOLICITED_CR:
        (void)handle_incoming_CR_event();
        break;

    case EXE_UNSOLICITED_CIEV_SIGNAL:
        (void)handle_incoming_ciev_signal_event(response_p);
        break;

    case EXE_UNSOLICITED_CIEV_STORAGE:
        (void)handle_incoming_ciev_storage_event(response_p);
        break;

    case EXE_UNSOLICITED_ATD:
        (void)handle_incoming_ATD_event();
        break;
    case EXE_UNSOLICITED_EEMPAGE:
        handle_incoming_EEMPAGE_event((exe_empage_urc_t *)response_p);
        break;
    default:
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS

        if (!atc_exe_event_customer_extension_callback(event, response_p)) {
            ATC_LOG_E("NOT IMPLEMENTED YET: Event in atc_exe_event_callback()");
        }

#else
        ATC_LOG_E("NOT IMPLEMENTED YET: Event in atc_exe_event_callback()");
#endif
        break;
    }/* switch */
}

void atc_setup_exe_glue(exe_t *exe_p)
{
    store_exe_handle(exe_p);
    exe_register_callback_final_response(exe_p, atc_exe_final_response_callback);
    exe_register_callback_event(exe_p, atc_exe_event_callback);
}


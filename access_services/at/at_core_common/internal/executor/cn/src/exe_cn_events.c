/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "atc_log.h"

#include "exe.h"
#include "exe_internal.h"
#include "exe_cn_events.h"
#include "exe_cn_utils.h"
#include "exe_last_fail_cause.h"

#include "cn_data_types.h"

#ifdef EXE_USE_AUDIO_SERVICE
#include "ste_adm_client.h"
#endif

/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static bool event_radio_status(cn_rf_status_t *rf_status_p);
static bool event_modem_registration_status(cn_registration_info_t *reg_info_p);
static bool event_time_info(cn_time_info_t *time_info_p);
static bool event_ring(cn_call_context_t *call_context_p);
static bool event_ring_waiting(cn_call_context_t *call_context_p);
static bool event_call_state_changed(cn_call_context_t *call_context_p);
static bool event_net_detailed_fail_cause(cn_net_detailed_fail_cause_t *net_detailed_fail_cause_p);
static bool event_call_supp_svc_notification(cn_supp_svc_notification_t *cn_supp_svc_notification_p);
static bool event_ussd(cn_ussd_info_t *ussd_info_p);
static bool event_call_cnap(cn_call_context_t *call_context_p);
static exe_call_status_t exe_cn_convert_call_status(cn_call_state_t mal_call_state);
static bool event_generate_local_comfort_tones(cn_comfort_tone_generation_t *tone_generation_p);
static bool event_signal_info(cn_signal_info_t *signalinfo_p);
static bool event_rat_name(cn_rat_name_t *rat_name_p);
static bool event_cell_info(cn_cell_info_t *cell_info_p);
static bool event_empage_info(cn_empage_info_t *empage_info_p);
static bool event_nitz_name(cn_network_name_info_t *network_name_p);


bool exe_handle_event(cn_message_type_t type, void *data_p)
{
    bool result;

    switch (type) {
    case CN_EVENT_RADIO_STATUS:
        result = event_radio_status((cn_rf_status_t *)data_p);
        break;

    case CN_EVENT_MODEM_REGISTRATION_STATUS:
        result = event_modem_registration_status((cn_registration_info_t *)data_p);
        break;

    case CN_EVENT_TIME_INFO:
        result = event_time_info((cn_time_info_t *)data_p);
        break;

    case CN_EVENT_RING:
        result = event_ring((cn_call_context_t *)data_p);
        break;

    case CN_EVENT_RING_WAITING:
        result = event_ring_waiting((cn_call_context_t *)data_p);
        break;

    case CN_EVENT_CALL_STATE_CHANGED:
        result = event_call_state_changed((cn_call_context_t *)data_p);
        break;

    case CN_EVENT_NET_DETAILED_FAIL_CAUSE:
        result = event_net_detailed_fail_cause((cn_net_detailed_fail_cause_t *)data_p);
        break;

    case CN_EVENT_CALL_SUPP_SVC_NOTIFICATION:
        result = event_call_supp_svc_notification((cn_supp_svc_notification_t *)data_p);
        break;

    case CN_EVENT_USSD:
        result = event_ussd((cn_ussd_info_t *)data_p);
        break;

    case CN_EVENT_CALL_CNAP:
        result = event_call_cnap((cn_call_context_t *)data_p);
        break;

    case CN_EVENT_GENERATE_LOCAL_COMFORT_TONES:
        result = event_generate_local_comfort_tones((cn_comfort_tone_generation_t *)data_p);
        break;

    case CN_EVENT_SIGNAL_INFO:
        result = event_signal_info((cn_signal_info_t *)data_p);
        break;

    case CN_EVENT_RAT_NAME:
        result = event_rat_name((cn_rat_name_t *)data_p);
        break;
    case CN_EVENT_EMPAGE_INFO:
        result = event_empage_info((cn_empage_info_t *) data_p);
        break;
    case CN_EVENT_CELL_INFO:
        result = event_cell_info((cn_cell_info_t *)data_p);
        break;
    case CN_EVENT_NETWORK_INFO:
        result = event_nitz_name((cn_network_name_info_t *)data_p);
        break;
    default:
        result = false;
        break;
    } /* switch(type) */

    return result;
}


bool event_radio_status(cn_rf_status_t *rf_status_p)
{
    exe_erfstate_response_t erfstate_response;

    if (rf_status_p) {
        ATC_LOG_I("%s: CN_EVENT_RADIO_STATUS: rf_status: %d ",
                  __FUNCTION__, *rf_status_p);
        erfstate_response.radio_state = *rf_status_p;
        exe_event(EXE_UNSOLICITED_ERFSTATE, (void *)&erfstate_response);
    } else {
        erfstate_response.radio_state = CN_RF_OFF;
        ATC_LOG_I("%s: CN_EVENT_RADIO_STATUS: rf_status == NULL!",
                  __FUNCTION__);
        exe_event(EXE_UNSOLICITED_ERFSTATE, (void *)&erfstate_response);
    }

    return true;
}


bool event_modem_registration_status(cn_registration_info_t *reg_info_p)
{
    exe_cgreg_reg_info_t cgreg = {EXE_NET_REG_STAT_UNKNOWN, 0, 0, 0};
    exe_creg_reg_info_t creg_reg_info;
    exe_ereg_reg_info_t ereg_reg_info;
    exe_net_reg_stat_t reg_stat = EXE_NET_REG_STAT_NOT_REGISTERED;


    if (!reg_info_p) {
        ATC_LOG_E("CN_EVENT_MODEM_REGISTRATION_STATUS: reg_info_p == NULL!");
        goto error;
    }

    if (!map_registration_state_from_cn_to_at(reg_info_p->reg_status,
            &reg_stat)) {
        ATC_LOG_E("registration state mapping failed");
        goto cgreg_response;
    }

    creg_reg_info.stat = reg_stat;
    creg_reg_info.lac = reg_info_p->lac;
    creg_reg_info.cid = reg_info_p->cid;
    creg_reg_info.cs_attached = reg_info_p->cs_attached;

    ereg_reg_info.stat = reg_stat;
    ereg_reg_info.lac = reg_info_p->lac;
    ereg_reg_info.cid = reg_info_p->cid;
    ereg_reg_info.cs_attached = reg_info_p->cs_attached;

    if (EXE_NET_REG_STAT_REGISTERED_HOME == reg_stat ||
            EXE_NET_REG_STAT_REGISTERED_ROAMING == reg_stat) {

        if (!(map_rat_to_basic_act(reg_info_p->rat, &ereg_reg_info.act) && map_rat_to_basic_act(reg_info_p->rat, &creg_reg_info.act))) {
            ATC_LOG_E("radio technology mapping for ereg and creg failed");
            goto cgreg_response;
        }

    }

    exe_event(EXE_UNSOLICITED_CREG, &creg_reg_info);
    exe_event(EXE_UNSOLICITED_EREG, &ereg_reg_info);

cgreg_response:

    if (!map_gprs_registration_state_from_cn_to_at(reg_info_p->reg_status,
            reg_info_p->gprs_attached, &cgreg.stat)) {
        ATC_LOG_E("registration state mapping for cgreg failed");
        goto error;
    }

    if (EXE_NET_REG_STAT_REGISTERED_HOME == cgreg.stat ||
            EXE_NET_REG_STAT_REGISTERED_ROAMING == cgreg.stat) {
        cgreg.lac = reg_info_p->lac;
        cgreg.ci = reg_info_p->cid;

        if (!map_rat_to_act(reg_info_p->rat, &cgreg.act)) {
            ATC_LOG_E("radio technology mapping for cgreg failed");
            goto error;
        }
    }

    exe_event(EXE_UNSOLICITED_CGREG, &cgreg);

    return true;

error:
    return false;
}


bool event_time_info(cn_time_info_t *time_info_p)
{
    exe_etzv_response_t etzv;

    if (time_info_p) {
        etzv.year                   = time_info_p->year;
        etzv.month                  = time_info_p->month;
        etzv.day                    = time_info_p->day;
        etzv.hour                   = time_info_p->hour;
        etzv.minute                 = time_info_p->minute;
        etzv.second                 = time_info_p->second;
        etzv.time_zone_sign         = time_info_p->time_zone_sign;
        etzv.time_zone_value        = time_info_p->time_zone_value;
        etzv.day_light_saving_time  = time_info_p->day_light_saving_time;
        exe_event(EXE_UNSOLICITED_ETZV, &etzv);
    } else {
        ATC_LOG_E("%s: CN_EVENT_TIME_INFO: time_info_p == NULL!",
                  __FUNCTION__);
    }

    return true;
}


bool event_ring(cn_call_context_t *call_context_p)
{
    exe_ecav_response_t ecav;
    exe_clip_response_t clip;

    if (call_context_p) {
        exe_call_type_t call_type;

        switch (call_context_p->mode) {
        case CN_CALL_MODE_EMERGENCY:
            call_type = CALL_SPEECH;  /* Voice */
            break;
        case CN_CALL_MODE_SPEECH:
            call_type = CALL_SPEECH;  /* Voice */
            break;
        case CN_CALL_MODE_ALS_2:
            call_type = CALL_ALS;     /* Voice2 = als */
            break;
        case CN_CALL_MODE_MULTIMEDIA:
            call_type = CALL_SPEECH;  /* Voice */
            break;
        default:
            call_type = CALL_EMPTY;
            ATC_LOG_E("Call_type (0x%02X) -> Not supported", call_context_p->mode);
            break;
        }

        if (0 == call_context_p->number_of_rings || call_context_p->updated) {
            /* +CLIP */
            clip.number_p = call_context_p->number;

            if (call_context_p->address_type > 127 && call_context_p->address_type < 256) {
                clip.type = call_context_p->address_type;
            } else {
                clip.type = EXE_TYPE_OF_NUMBER_UNKNOWN;
            }

            exe_event(EXE_UNSOLICITED_CLIP, &clip);

            /* +CNAP */
            event_call_cnap(call_context_p);

            /* +CUUS1U / +CUUS1I */
            if (call_context_p->user_to_user_len > 0) {
                exe_cuus1_response_t cuus1;

                cuus1.buff = call_context_p->user_to_user;
                cuus1.len = call_context_p->user_to_user_len;
                cuus1.type = call_context_p->user_to_user_type;

                if (call_context_p->is_MT) {
                    exe_event(EXE_UNSOLICITED_CUUS1U, &cuus1);
                } else {
                    exe_event(EXE_UNSOLICITED_CUUS1I, &cuus1);
                }
            }
        }

        /* RING */
        exe_event(EXE_UNSOLICITED_RING, &call_type);

        /* *ECAV */
        if (0 < call_context_p->number_of_rings && call_context_p->updated) {
            /* Do not send an "*ECAV:" event unless the call-context has been updated.
             * Normally the "event_call_state_changed()" handler below emits the "*ECAV:"
             * event whenever the call-state for a line changes.
             */
            memset(&ecav, 0, sizeof(ecav));
            ecav.ccid = call_context_p->call_id;
            ecav.call_status = exe_cn_convert_call_status(call_context_p->call_state);
            ecav.call_type = call_type;
            ecav.subscriber_number_p = call_context_p->number;
            ecav.subscriber_number_addr_type = call_context_p->address_type;

            exe_event(EXE_UNSOLICITED_ECAV, &ecav);
        }

    } else {
        ATC_LOG_E("call_context_p == NULL!");
    }

    return true;
}


bool event_ring_waiting(cn_call_context_t *call_context_p)
{
    exe_ccwa_response_t ccwa;

    if (call_context_p) {
        /* *CCWA */
        memset(&ccwa, 0x00, sizeof(ccwa));
        ccwa.number_p = call_context_p->number;

        if (call_context_p->address_type > 127 && call_context_p->address_type < 256) {
            ccwa.type = call_context_p->address_type;
        } else {
            ccwa.type = EXE_TYPE_OF_NUMBER_UNKNOWN;
        }

        /* MAL does not provide the class parameter, so for now
         * it's hardcoded to 1 (Voice)*/
        ccwa.class = 1;

        exe_event(EXE_UNSOLICITED_CCWA, &ccwa);

        /* +CNAP */
        event_call_cnap(call_context_p);

        /* +CUUS1U / +CUUS1I */
        if (call_context_p->user_to_user_len > 0) {
            exe_cuus1_response_t cuus1;

            cuus1.buff = call_context_p->user_to_user;
            cuus1.len = call_context_p->user_to_user_len;
            cuus1.type = call_context_p->user_to_user_type;

            if (call_context_p->is_MT) {
                exe_event(EXE_UNSOLICITED_CUUS1U, &cuus1);
            } else {
                exe_event(EXE_UNSOLICITED_CUUS1I, &cuus1);
            }
        }

        /* *ECAV */
        /* Note that the "*ECAV:" event is issued by the "event_call_state_changed()"
         * handler below. This handler is invoked every time the call-state for a line
         * changes.
         */

    } else {
        ATC_LOG_E("CN_EVENT_RING_WAITING: call_context_p == NULL!");
    }

    return true;
}

exe_call_exit_response_t exe_cn_map_exit_cause_to_call_response(cn_exit_cause_t cn_exit_cause)
{
    exe_call_exit_response_t exit_response = EXE_CALL_RESPONSE_NONE;

    /* For this mapping we need not to look at CN_CALL_CAUSE_TYPE_SENDER_CLIENT. */
    if (CN_CALL_CAUSE_TYPE_SENDER_NETWORK == cn_exit_cause.cause_type_sender) {
        switch (cn_exit_cause.cause) {
        case CN_CALL_NETWORK_CAUSE_USER_BUSY:
            exit_response = EXE_CALL_RESPONSE_BUSY;
            break;

        case CN_CALL_NETWORK_CAUSE_ALERT_NO_ANSWER:
            exit_response = EXE_CALL_RESPONSE_NO_ANSWER;
            break;

        default:
            exit_response = EXE_CALL_RESPONSE_NO_CARRIER;
            break;
        }
    } else if (CN_CALL_CAUSE_TYPE_SENDER_SERVER == cn_exit_cause.cause_type_sender) {
        switch (cn_exit_cause.cause) {
        case CN_CALL_SERVER_CAUSE_CS_INACTIVE:
            exit_response = EXE_CALL_RESPONSE_NO_DIALTONE;
            break;

        default:
            exit_response = EXE_CALL_RESPONSE_NO_CARRIER;
            break;
        }
    } else {
        exit_response = EXE_CALL_RESPONSE_NONE;
    }

    return exit_response;

}

static exe_call_status_t exe_cn_convert_call_status(cn_call_state_t mal_call_state)
{
    exe_call_status_t exe_call_status = ECAM_IDLE;

    switch (mal_call_state) {
    case CN_CALL_STATE_ACTIVE:
        exe_call_status = ECAM_ACTIVE;
        break;
    case CN_CALL_STATE_HOLDING:
        exe_call_status = ECAM_HOLD;
        break;
    case CN_CALL_STATE_DIALING:
        exe_call_status = ECAM_CALLING;
        break;
    case CN_CALL_STATE_ALERTING:
        exe_call_status = ECAM_CONNECTING;
        break;
    case CN_CALL_STATE_INCOMING:
        exe_call_status = ECAM_ALERTING;
        break;
    case CN_CALL_STATE_WAITING:
        exe_call_status = ECAM_WAITING;
        break;
    case CN_CALL_STATE_IDLE:
    default:
        exe_call_status = ECAM_IDLE;
        break;
    }

    return exe_call_status;
}


bool event_call_state_changed(cn_call_context_t *call_context_p)
{
    exe_ecav_response_t ecav;

    if (NULL != call_context_p) {
        /* Handle sending of *ECAV */
        exe_call_type_t call_type = CALL_EMPTY;

        if ( call_context_p->call_state == CN_CALL_STATE_MT_SETUP ) {
            ATC_LOG_I("%s:Event Dropped -> Only for SIM to Send ENVELOPE for MT Call Setup",
                       __FUNCTION__);
            return true;
        }

        switch (call_context_p->mode) {
        case CN_CALL_MODE_EMERGENCY:
            call_type = CALL_SPEECH;  /* Voice */
            break;
        case CN_CALL_MODE_SPEECH:
            call_type = CALL_SPEECH;  /* Voice */
            break;
        case CN_CALL_MODE_ALS_2:
            call_type = CALL_ALS;     /* Voice2 = als */
            break;
        case CN_CALL_MODE_MULTIMEDIA:
            call_type = CALL_SPEECH;  /* Voice */
            break;
        default:
            call_type = CALL_EMPTY;
            ATC_LOG_E("Call_type (0x%02X) -> Not supported", call_context_p->mode);
            break;
        }

        memset(&ecav, 0, sizeof(ecav));
        ecav.ccid        = call_context_p->call_id;
        ecav.call_status = exe_cn_convert_call_status(call_context_p->call_state);
        ecav.call_type   = call_type;

        /* In the following cases ecav shall report subscriber number and type */
        if (ecav.call_status == ECAM_WAITING ||
                ecav.call_status == ECAM_CALLING ||
                ecav.call_status == ECAM_ALERTING) {
            ecav.subscriber_number_p         = call_context_p->number;
            ecav.subscriber_number_addr_type = call_context_p->address_type;
        }

        /* Set exit cause for the exit classes of interest. */
        if (CN_CALL_CAUSE_TYPE_SENDER_NETWORK == call_context_p->call_cause.cause_type_sender ||
                CN_CALL_CAUSE_TYPE_SENDER_CLIENT == call_context_p->call_cause.cause_type_sender) {
            ecav.exit_cause = exe_cn_get_exit_cause(&call_context_p->call_cause);

            /* To send a special ecav busy unsolicited event */
            if (ecav.exit_cause == CN_CALL_NETWORK_CAUSE_USER_BUSY) {
                exe_ecav_response_t busy_ecav;
                memset(&busy_ecav, 0, sizeof(ecav));
                busy_ecav.ccid = ecav.ccid;
                busy_ecav.call_status = ECAM_BUSY;
                busy_ecav.call_type = ecav.call_type;
                exe_event(EXE_UNSOLICITED_ECAV, &busy_ecav);
            }
        }

        exe_event(EXE_UNSOLICITED_ECAV, &ecav);

        /* Send CSSU with code2 = 5 - "call on hold has been released"
         * This is a special case due to code2 = 5 is not an SS notification */
        if (call_context_p->prev_call_state == CN_CALL_STATE_HOLDING &&
                call_context_p->call_state == CN_CALL_STATE_IDLE &&
                call_context_p->message_type == 3 &&
                ((call_context_p->message_direction == 2 &&
                  call_context_p->call_cause.cause_type_sender == CN_CALL_CAUSE_TYPE_SENDER_NETWORK) ||
                 (call_context_p->message_direction == 1 &&
                  call_context_p->call_cause.cause_type_sender == CN_CALL_CAUSE_TYPE_SENDER_CLIENT))) {

            cn_supp_svc_notification_t cn_supp_svc_notification_p;
            cn_supp_svc_notification_p.address_type = call_context_p->address_type;
            cn_supp_svc_notification_p.notification_type = CN_NOTIFICATION_TYPE_MT_UNSOLICITED;
            cn_supp_svc_notification_p.code = 5;
            strncpy(cn_supp_svc_notification_p.number, call_context_p->number, CN_MAX_STRING_BUFF);
            event_call_supp_svc_notification(&cn_supp_svc_notification_p);
        }

        /* Handle registration of last fail cause */
        if (CN_CALL_STATE_IDLE == call_context_p->call_state) {
            /* Extended Error Processing, this is an unsolicited disconnection */
            exe_cn_register_fail_cause(&call_context_p->call_cause, __FUNCTION__);
        }

        /* Handle sending of Call response */
        if (CN_CALL_STATE_IDLE == call_context_p->prev_call_state ||
                CN_CALL_STATE_DIALING == call_context_p->prev_call_state ||
                CN_CALL_STATE_ALERTING == call_context_p->prev_call_state) {

            exe_call_response_t call_response;

            memset(&call_response, 0, sizeof(exe_call_response_t));
            call_response.call_state = (exe_call_state_t)call_context_p->call_state;
            call_response.exit_response = exe_cn_map_exit_cause_to_call_response(call_context_p->call_cause);

            exe_event(EXE_EVENT_CALL_RESPONSE, &call_response);
        }
    } else {
        ATC_LOG_E("call_context_p == NULL!");
    }

    return true;
}


bool event_net_detailed_fail_cause(cn_net_detailed_fail_cause_t *net_detailed_fail_cause_p)
{
    if (NULL != net_detailed_fail_cause_p) {
        exe_last_fail_cause_t fail_response;

        switch (net_detailed_fail_cause_p->class) {
        case CN_NET_DETAILED_FAIL_CAUSE_CLASS_PSCC:
            fail_response.class = EXE_LAST_FAIL_CAUSE_CLASS_NET_PSCC;
            break;

        case CN_NET_DETAILED_FAIL_CAUSE_CLASS_NET:
            fail_response.class = EXE_LAST_FAIL_CAUSE_CLASS_NET_GSM;
            break;

        case CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS:
            fail_response.class = EXE_LAST_FAIL_CAUSE_CLASS_NET_SS;
            break;

        default:
            ATC_LOG_E("%s: Non supported fail cause class for CN net.", __FUNCTION__);
            return false;
        }

        fail_response.cause = (int32_t)net_detailed_fail_cause_p->cause;
        /* Register fail data in executor */
        exe_set_last_fail_cause(exe_get_exe(), &fail_response);
    } else {
        ATC_LOG_E("%s: NET_DETAILED_FAIL_CAUSE: net_detailed_fail_cause_p == NULL!", __FUNCTION__);
    }

    return true;
}

bool event_call_supp_svc_notification(cn_supp_svc_notification_t *cn_supp_svc_notification_p)
{
    if (cn_supp_svc_notification_p) {
        switch (cn_supp_svc_notification_p->notification_type) {
        case CN_NOTIFICATION_TYPE_MO_INTERMEDIATE: {
            /* MO Call */
            exe_cssi_response_t cssi;
            memset(&cssi, 0x00, sizeof(exe_cssi_response_t));
            cssi.code1 = cn_supp_svc_notification_p->code;
            cssi.index = cn_supp_svc_notification_p->index;
            exe_event(EXE_UNSOLICITED_CSSI, &cssi);
            break;
        }

        case CN_NOTIFICATION_TYPE_MT_UNSOLICITED: {
            /* MT Call */
            exe_cssu_response_t cssu;
            memset(&cssu, 0x00, sizeof(exe_cssu_response_t));
            cssu.code2 = cn_supp_svc_notification_p->code;
            cssu.index = cn_supp_svc_notification_p->index;
            cssu.address_type = cn_supp_svc_notification_p->address_type;
            size_t number_len = strnlen(cn_supp_svc_notification_p->number, CN_MAX_STRING_BUFF);
            cssu.number = malloc(number_len + 1);

            if (!cssu.number) {
                ATC_LOG_E("%s: CN_NOTIFICATION_TYPE_MT_UNSOLICITED: error allocating memory!",
                          __FUNCTION__);
                break;
            }

            strncpy(cssu.number, cn_supp_svc_notification_p->number, number_len + 1);
            cssu.number[number_len] = '\0';

            exe_event(EXE_UNSOLICITED_CSSU, &cssu);
            break;
        }

        default:
            ATC_LOG_E("%s: CN_EVENT_CALL_SUPP_SVC_NOTIFICATION: invalid notification_type (%d)!",
                      __FUNCTION__, cn_supp_svc_notification_p->notification_type);
            break;
        }
    } else {
        ATC_LOG_E("%s: CN_EVENT_CALL_SUPP_SVC_NOTIFICATION: cn_supp_svc_notification_p == NULL!",
                  __FUNCTION__);
    }

    return true;
}


bool event_ussd(cn_ussd_info_t *ussd_info_p)
{
    exe_cusd_response_t cusd;

    if (NULL == ussd_info_p) {
        ATC_LOG_E("%s: CN_EVENT_USSD: ussd_info_p == NULL!", __FUNCTION__);
    } else {
        cusd.type           = ussd_info_p->type;
        cusd.received_type  = ussd_info_p->received_type;
        cusd.length         = ussd_info_p->length;
        cusd.dcs            = ussd_info_p->dcs;
        cusd.ussd_str_p     = ussd_info_p->ussd_string;
        cusd.me_initiated   = ussd_info_p->me_initiated;
        cusd.suppress_ussd_on_end  = ussd_info_p->suppress_ussd_on_end;
        exe_event(EXE_UNSOLICITED_CUSD, &cusd);
    }

    return true;
}


bool event_call_cnap(cn_call_context_t *call_context_p)
{
    exe_cnap_response_t cnap;

    if (call_context_p) {
        /* +CNAP */
        cnap.name_p = call_context_p->name;
        cnap.name_char_length = call_context_p->name_char_length;

        if (!map_presentation_to_cni_validity(call_context_p->name_presentation,
                                              &cnap.cni_validity)) {
            ATC_LOG_E("Failed to map 'name_presentation' (0x%X) to 'CNI validity'!",
                      call_context_p->name_presentation);
        }

        exe_event(EXE_UNSOLICITED_CNAP, &cnap);
    } else {
        ATC_LOG_E("call_context_p == NULL!");
    }

    return true;
}


static bool event_generate_local_comfort_tones(cn_comfort_tone_generation_t *tone_generation_p)
{
    exe_elat_response_t elat;

    if (tone_generation_p) {
        elat = (exe_elat_response_t) * tone_generation_p;
        /* We need to mute/unmute RX audio because some networks send both RX audio
         * and this signal to generate local tones at the same time */
        if (elat == 1) {
#ifdef EXE_USE_AUDIO_SERVICE
            int res = ste_adm_client_set_cscall_downstream_mute(1);

            if (res != 0) {
                ATC_LOG_E("ste_adm_client_set_cscall_downstream_mute(1) failed res = %d", res);
            }
#else
            ATC_LOG_I("ste_adm_client_set_cscall_downstream_mute() not called. Audio in At disabled");
#endif
        } else {
#ifdef EXE_USE_AUDIO_SERVICE
            int res = ste_adm_client_set_cscall_downstream_mute(0);

            if (res != 0) {
                ATC_LOG_E("ste_adm_client_set_cscall_downstream_mute(0) failed res = %d", res);
            }
#else
            ATC_LOG_I("ste_adm_client_set_cscall_downstream_mute() not called. Audio in At disabled");
#endif

        }
        exe_event(EXE_UNSOLICITED_ELAT, &elat);

    } else {
        ATC_LOG_E("%s: CN_EVENT_GENERATE_LOCAL_COMFORT_TONES: tone_generation_p == NULL!",
                  __FUNCTION__);
    }

    return true;
}


bool event_signal_info(cn_signal_info_t *signal_info_p)
{
    exe_cmer_ciev_data_t ciev_data;

    if (NULL == signal_info_p) {
        ATC_LOG_E("%s, signal_info_p == NULL!", __func__);
        goto error;
    }

    ciev_data.rssi_dbm           = signal_info_p->rssi_dbm;
    exe_event(EXE_UNSOLICITED_CIEV_SIGNAL, &ciev_data);

    return true;
error:
    return false;
}


bool event_rat_name(cn_rat_name_t *rat_name_p)
{
    exe_ecrat_ind_t ecrat;

    if (!rat_name_p) {
        ATC_LOG_E("rat_name_p is NULL!");
        goto exit;
    }

    ecrat.rat = (exe_rat_name_t) * rat_name_p;
    exe_event(EXE_UNSOLICITED_ECRAT, &ecrat);

exit:
    return true;
}

bool event_nitz_name(cn_network_name_info_t *name_info_p)
{
    exe_ennir_ind_t ennir;

    if (!name_info_p) {
        ATC_LOG_E("nitz_name_p is NULL!");
        goto exit;
    }

    ennir.name_char_short_length = name_info_p->name[CN_NETWORK_NAME_SHORT].length;
    ennir.name_char_long_length  = name_info_p->name[CN_NETWORK_NAME_LONG].length;
    ennir.short_dcs = name_info_p->name[CN_NETWORK_NAME_SHORT].dcs;
    ennir.long_dcs = name_info_p->name[CN_NETWORK_NAME_LONG].dcs;

    memcpy(ennir.mcc_mnc, name_info_p->mcc_mnc, EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH + 1);

    if (ennir.name_char_long_length) {
        memcpy(ennir.text_long, name_info_p->name[CN_NETWORK_NAME_LONG].text, EXE_COPS_OPERATOR_MAX_LONG_LEN + 1);
    }

    if (ennir.name_char_short_length) {
        memcpy(ennir.text_short, name_info_p->name[CN_NETWORK_NAME_SHORT].text, EXE_COPS_OPERATOR_MAX_SHORT_LEN + 1);
    }

    exe_event(EXE_UNSOLICITED_ENNIR, &ennir);

exit:
    return true;
}



bool event_cell_info(cn_cell_info_t *cell_info_p)
{
    if (!cell_info_p) {
        ATC_LOG_E("cell_info_p is NULL!");
        goto error;
    }

    if (CN_NW_LIMITED_SERVICE == cell_info_p->service_status ||
            CN_NW_LIMITED_SERVICE_WAITING == cell_info_p->service_status) {
        /* Trigger an unsolicited CREG event in case we are in limited service.
         * The <stat> value is not of big importance since RIL will request
         * OPERATOR, REGISTRATION_STATE, GPRS_REGISTRATION_STATE in order
         * to retrieve the correct registration status values to android. */
        exe_creg_reg_info_t reg_info = { EXE_NET_REG_STAT_NOT_REGISTERED_SEARCH_EMERGENCY_CALLS_ALLOWED, 0, 0, EXE_BASIC_ACT_UNKNOWN, 0};
        exe_event(EXE_UNSOLICITED_CREG, &reg_info);
    }

    return true;

error:
    return false;
}

bool event_empage_info(cn_empage_info_t *empage_info_p)
{
    exe_empage_urc_t empage;

    if (!empage_info_p) {
        ATC_LOG_E("%s: empage_info_p is NULL", __FUNCTION__);
        goto error;
    }

    empage.page  = empage_info_p->page;
    empage.seqnr = empage_info_p->seqnr;

    exe_event(EXE_UNSOLICITED_EEMPAGE, &empage);
    return true;
error:
    return false;
}

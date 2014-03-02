/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

/* TODO: MFL headers */

#include "cn_client.h"
#include "cn_log.h"

#include "cnsocket.h"
#include "cnserver.h"

#include "cn_log.h"
#include "request_handling.h"
#include "cn_mfl_assertions.h" /* do NOT remove this inclusion! */


request_status_t handle_request_rf_on(void *data_p, request_record_t *record_p) /* TODO: Request_RadioCtrl_RadioOn */
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_RF_ON, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_rf_off(void *data_p, request_record_t *record_p) /* TODO: Request_RadioCtrl_RadioOff */
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_RF_OFF, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_rf_status(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_RF_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_registration_control(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_CONTROL, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_reset_modem(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_RESET_MODEM, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_preferred_network_type(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_get_preferred_network_type(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_registration_state_normal(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_registration_state_gprs(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_GPRS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_cell_info(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_CELL_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_automatic_network_registration(void *data_p, request_record_t *record_p) /* Request_Roaming_PLMN_SelectionMode_Set (ROAMING_PLMN_SELECTION_MODE_AUTOMATIC) */
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_manual_network_registration(void *data_p, request_record_t *record_p) /* Request_Roaming_PLMN_SelectionMode_Set (ROAMING_PLMN_SELECTION_MODE_MANUAL, Request_Roaming_Network_Set_V3 */
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_network_deregister(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_NETWORK_DEREGISTER, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_net_query_mode(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_NET_QUERY_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_manual_network_search(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_SEARCH, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_interrupt_network_search(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_INTERRUPT_NETWORK_SEARCH, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_current_call_list(void *data_p, request_record_t *record_p) /* TODO: Request_CSS_AllCallsCurrentStatus_Get */
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_CURRENT_CALL_LIST, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_dial(void *data_p, request_record_t *record_p) /* TODO: Request_CSS_MO_Call_Start */
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_DIAL, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_hangup(void *data_p, request_record_t *record_p) /* TODO: Request_CSS_Call_End */
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_HANGUP, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_swap_calls(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SWAP_CALLS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_hold_call(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_HOLD_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}
request_status_t handle_request_resume_call(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_RESUME_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}
request_status_t handle_request_conference_call(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_CONFERENCE_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}
request_status_t handle_request_conference_call_split(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_CONFERENCE_CALL_SPLIT, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}
request_status_t handle_request_explicit_call_transfer(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_EXPLICIT_CALL_TRANSFER, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}
request_status_t handle_request_answer_call(void *data_p, request_record_t *record_p) /* TODO: Request_CSS_MT_Call_Answer */
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_ANSWER_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}
request_status_t handle_request_ussd(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_USSD, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_ussd_abort(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_USSD_ABORT, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_rssi_value(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_RSSI_VALUE, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_clip_status(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_CLIP_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_clir_status(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_CLIR_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_cnap_status(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_CNAP_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_colr_status(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_COLR_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_set_clir(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_CLIR, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_set_modem_property(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_set_cssn(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_CSSN, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_dtmf_send(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_DTMF_SEND, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_dtmf_start(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_DTMF_START, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_dtmf_stop(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_DTMF_STOP, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_set_call_waiting(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_CALL_WAITING, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}


request_status_t handle_request_get_call_waiting(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_CALL_WAITING, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_manual_network_registration_with_automatic_fallback(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_product_profile_flag(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_l1_parameter(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_L1_PARAMETER, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_user_activity_status(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_USER_ACTIVITY_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_neighbour_cells_reporting(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_neighbour_cells_reporting(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_neighbour_cells_basic_extd_info(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_neighbour_cells_extd_info(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_timing_advance(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_TIMING_ADVANCE, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_nmr_info(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_NMR_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_query_call_forward(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_FORWARD, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_call_forward(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_CALL_FORWARD, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_modem_property(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_change_barring_password(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_CHANGE_BARRING_PASSWORD, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_query_call_barring(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_BARRING, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_call_barring(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_CALL_BARRING, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_event_reporting(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_EVENT_REPORTING, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_rab_status(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_RAB_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_baseband_version(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_BASEBAND_VERSION, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_pp_flags(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_PP_FLAGS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_modify_emergency_number_list(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_emergency_number_list(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_signal_info_config(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_SIGNAL_INFO_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_signal_info_config(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_SIGNAL_INFO_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_signal_info_reporting(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_SIGNAL_INFO_REPORTING, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_signal_info_reporting(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_SIGNAL_INFO_REPORTING, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_set_hsxpa_mode(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_HSXPA_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_get_hsxpa_mode(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_GET_HSXPA_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

request_status_t handle_request_ss_command(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SS, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}



request_status_t handle_request_set_empage(void *data_p, request_record_t *record_p)
{
    (void)data_p;
    CN_LOG_E("Not implemented in MFL backend!");
    send_response(record_p->client_id, CN_RESPONSE_SET_EMPAGE, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
}

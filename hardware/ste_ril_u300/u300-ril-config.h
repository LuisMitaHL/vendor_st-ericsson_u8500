/* ST-Ericsson U300 RIL
**
** Copyright (C) ST-Ericsson AB 2008-2010
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Based on reference-ril by The Android Open Source Project.
**
** Heavily modified for ST-Ericsson U300 modems.
** Author: Christian Bejram <christian.bejram@stericsson.com>
** Author: Henrik Persson <henrik.persson@stericsson.com>
*/

#ifndef _U300_RIL_CONFIG_H
#define _U300_RIL_CONFIG_H 1

#include <telephony/ril.h>

/*
 * Requests that will go on the priority queue instead of the normal queue.
 *
 * If only one queue is configured, the request will be put on the normal
 * queue and sent as a normal request.
 */
static int prioRequests[] = {
    RIL_REQUEST_RADIO_POWER,
    RIL_REQUEST_SCREEN_STATE,
    RIL_REQUEST_OPERATOR,
    RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE,
    RIL_REQUEST_VOICE_REGISTRATION_STATE,
    RIL_REQUEST_DATA_REGISTRATION_STATE,
    RIL_REQUEST_SET_LOCATION_UPDATES,
    RIL_REQUEST_DIAL,
    RIL_REQUEST_HANGUP,
    RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND,
    RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND,
    RIL_REQUEST_GET_CURRENT_CALLS,
    RIL_REQUEST_ANSWER,
    RIL_REQUEST_LAST_CALL_FAIL_CAUSE,
    RIL_REQUEST_UDUB,
    RIL_REQUEST_SET_MUTE,
    RIL_REQUEST_GET_MUTE,
    RIL_REQUEST_CONFERENCE,
    RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE,
    RIL_REQUEST_SIGNAL_STRENGTH,
    RIL_REQUEST_DTMF,
    RIL_REQUEST_DTMF_START,
    RIL_REQUEST_DTMF_STOP,
    RIL_REQUEST_SMS_ACKNOWLEDGE, /* Acknowledge on same channel as incoming SMS is received */
    RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING, /* STK shall not be unduly delayed */
    RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE,
    RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND,
    RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM,
    RIL_REQUEST_GET_SIM_STATUS, /* Prioritized to ensure reliable SIM state transition detection */
    RIL_REQUEST_ENTER_SIM_PIN,
    RIL_REQUEST_ENTER_SIM_PUK,
    RIL_REQUEST_ENTER_SIM_PIN2,
    RIL_REQUEST_ENTER_SIM_PUK2,
    RIL_REQUEST_CHANGE_SIM_PIN,
    RIL_REQUEST_CHANGE_SIM_PIN2,
    RIL_REQUEST_QUERY_FACILITY_LOCK,
    RIL_REQUEST_SET_FACILITY_LOCK,
    RIL_REQUEST_CHANGE_BARRING_PASSWORD,
    RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION,
    RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION,
    RIL_REQUEST_OEM_HOOK_RAW,
    RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION
};
#endif

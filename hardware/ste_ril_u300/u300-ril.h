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
*/

#ifndef U300_RIL_H
#define U300_RIL_H 1

#include "telephony/ril.h"

#ifdef RIL_VERSION
#if RIL_VERSION > 6
#undef RIL_VERSION
#define RIL_VERSION 6
#endif
#endif

#define RIL_VERSION_STRING  "ST-Ericsson u300-ril R2B"

RIL_RadioState currentState();
void setRadioState(RIL_RadioState newState);
void getScreenStateLock();
int getScreenState();
void releaseScreenStateLock();

#define MAX_IFNAME_LEN 16
extern char ril_iface[MAX_IFNAME_LEN];

const struct RIL_Env *s_rilenv;

/**
 * Called by OEM to send a
 * RIL_UNSOL_OEM_HOOK_RAW response
 */
void sendUnsolOemHookRaw(uint8_t *data, size_t datalen);

#define RIL_onRequestComplete(t, e, response, responselen) s_rilenv->OnRequestComplete(t, e, response, responselen)
#define RIL_onUnsolicitedResponse(a, b, c) s_rilenv->OnUnsolicitedResponse(a, b, c)

/* Backward support for RIL interface < v6 */
#if RIL_VERSION < 6
#define RIL_DataCallFailCause RIL_LastDataCallActivateFailCause
#define RIL_SignalStrength_v6 RIL_SignalStrength
#define RIL_CardStatus_v6 RIL_CardStatus
#define RIL_SIM_IO_v6 RIL_SIM_IO
#define RIL_REQUEST_VOICE_REGISTRATION_STATE RIL_REQUEST_REGISTRATION_STATE
#define RIL_REQUEST_DATA_REGISTRATION_STATE RIL_REQUEST_GPRS_REGISTRATION_STATE
#define RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED

typedef int RIL_RadioTechnology;

#define RADIO_TECH_UNKNOWN 0
#define RADIO_TECH_GPRS 1
#define RADIO_TECH_EDGE 2
#define RADIO_TECH_UMTS 3
#define RADIO_TECH_HSDPA 9
#define RADIO_TECH_HSUPA 10
#define RADIO_TECH_HSPA 11

typedef int RIL_PreferredNetworkType;

#define PREF_NET_TYPE_GSM_WCDMA 0
#define PREF_NET_TYPE_GSM_ONLY 1
#define PREF_NET_TYPE_WCDMA 2
#else
#define RIL_Data_Call_Response RIL_Data_Call_Response_v6
#endif

void enqueueRILEvent(int isPrio, void (*callback) (void *param),
                     void *param, const struct timeval *relativeTime);

#define RIL_EVENT_QUEUE_NORMAL 0
#define RIL_EVENT_QUEUE_PRIO 1
#define RIL_EVENT_QUEUE_ALL 2

#define SS_DEFAULT_CLASS 15

typedef struct {
    int id;
    char *name;
    int cni_validity;   /* 0=Allowed, 1=Restricted, 2=Not available */
} RIL_CachedCNAPInfo;


/* 3GPP TS 24.008 10.5.4.25 says max is 35 or 131 bytes */
#define RIL_UUS_DATA_MAX_SIZE 131
#define RIL_UUS_DATA_MAX_BUF (RIL_UUS_DATA_MAX_SIZE + 1)
#define RIL_UUS_ELEMENT_IDENTIFIER 0x7E

typedef struct {
    int id;
    RIL_UUS_Info uusInfo;
    char data[RIL_UUS_DATA_MAX_BUF];
} RIL_CachedUUSInfo;


#endif

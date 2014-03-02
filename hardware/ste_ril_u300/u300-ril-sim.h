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

#ifndef U300_RIL_SIM_H
#define U300_RIL_SIM_H 1

/*
 * The following SIM_Status list consist of indexes to combine the result
 * string of 3GPP AT command AT+CPIN? (ST-Ericsson version) with RIL API
 * "RIL_AppStatus" structure. To fill this structure the SIM_Status value is
 * matched to an entry in the static app_status_array[] found in u300-ril-sim.c.
 */
typedef enum {
    SIM_ABSENT = 0,                      /* SIM card is not inserted */
    SIM_NOT_READY = 1,                   /* SIM card is not ready */
    SIM_READY = 2,                       /* meaning radio state RADIO_STATE_SIM_READY */
    SIM_PIN = 3,                         /* SIM PIN code lock */
    SIM_PUK = 4,                         /* SIM PUK code lock */
    SIM_NETWORK_PERSO = 5,               /* Network Personalization lock */
    SIM_PIN2 = 6,                        /* SIM PIN2 lock */
    SIM_PUK2 = 7,                        /* SIM PUK2 lock */
    SIM_NETWORK_SUBSET_PERSO = 8,        /* Network Subset Personalization lock */
    SIM_SERVICE_PROVIDER_PERSO = 9,      /* Service Provider Personalization lock */
    SIM_CORPORATE_PERSO = 10,            /* Corporate Personalization lock */
    SIM_PERSO = 11,                      /* SIM/USIM Personalization lock */
    SIM_STERICSSON_LOCK = 12,            /* ST-Ericsson Extended SIM lock */
    SIM_BLOCKED = 13,                    /* SIM card is blocked for the user */
    SIM_PERM_BLOCKED = 14,               /* SIM card is permanently blocked for the user */
    SIM_NETWORK_PERSO_PUK = 15,          /* Network Personalization PUK lock */
    SIM_NETWORK_SUBSET_PERSO_PUK = 16,   /* Network Subset Personalization PUK lock */
    SIM_SERVICE_PROVIDER_PERSO_PUK = 17, /* Service Provider Personalization PUK lock */
    SIM_CORPORATE_PERSO_PUK = 18,        /* Corporate Personalization PUK lock */
    SIM_SIM_PERSO_PUK = 19,              /* SIM Personalization PUK (unused) */
    SIM_PUK2_PERM_BLOCKED = 20,          /* PUK2 is permanently blocked */
    SIM_ERROR = 21
} SIM_Status;

#define PROP_EMERGENCY_LIST_RO ("ro.ril.ecclist")
#define PROP_EMERGENCY_LIST_RW ("ril.ecclist")

void onSimStateChanged(const char *s);

void requestGetSimStatus(void *data, size_t datalen, RIL_Token t);
void requestSIM_IO(void *data, size_t datalen, RIL_Token t);
void requestEnterSimPin(void *data, size_t datalen, RIL_Token t, int request);
void requestChangePassword(char *facility, void *data, size_t datalen,
                           RIL_Token t, int request);
void requestSetFacilityLock(void *data, size_t datalen, RIL_Token t);
void requestQueryFacilityLock(void *data, size_t datalen, RIL_Token t);
void requestISIMAuthentication(void *data, size_t datalen, RIL_Token t);

void pollSIMState(void *param);
void setupECCList(int check_attached_network);
bool isSimAbsent();
#endif

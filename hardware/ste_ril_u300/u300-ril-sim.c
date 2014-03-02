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

#include <cutils/properties.h>
#include <telephony/ril.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "atchannel.h"
#include "at_tok.h"
#include "fcp_parser.h"
#include "u300-ril.h"
#include "u300-ril-sim.h"
#include "u300-ril-network.h"
#include "misc.h"

#define LOG_TAG "RILV"
#include <utils/Log.h>

/*
 * The following list contains values for the structure "RIL_AppStatus" to be
 * sent to Android on a given SIM state. It is indexed by the SIM_Status enum in
 * u300-ril-sim.h.
 */
static const RIL_AppStatus app_status_array[] = {
    /*
     * RIL_AppType,  RIL_AppState,
     * RIL_PersoSubstate,
     * Aid pointer, App Label pointer, PIN1 replaced,
     * RIL_PinState (PIN1),
     * RIL_PinState (PIN2)
     */
    /* SIM_ABSENT = 0 */
    {
        RIL_APPTYPE_UNKNOWN, RIL_APPSTATE_UNKNOWN,
        RIL_PERSOSUBSTATE_UNKNOWN,
        NULL, NULL, 0,
        RIL_PINSTATE_UNKNOWN,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_NOT_READY = 1 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_DETECTED,
        RIL_PERSOSUBSTATE_UNKNOWN,
        NULL, NULL, 0,
        RIL_PINSTATE_UNKNOWN,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_READY = 2 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_READY,
        RIL_PERSOSUBSTATE_READY,
        NULL, NULL, 0,
        RIL_PINSTATE_UNKNOWN,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_PIN = 3 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_PIN,
        RIL_PERSOSUBSTATE_UNKNOWN,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_PUK = 4 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_PUK,
        RIL_PERSOSUBSTATE_UNKNOWN,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_BLOCKED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_NETWORK_PERSO = 5 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO,
        RIL_PERSOSUBSTATE_SIM_NETWORK,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_PIN2 = 6 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_READY,
        RIL_PERSOSUBSTATE_UNKNOWN,
        NULL, NULL, 0,
        RIL_PINSTATE_UNKNOWN,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED
    },
    /* SIM_PUK2 = 7 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_READY,
        RIL_PERSOSUBSTATE_UNKNOWN,
        NULL, NULL, 0,
        RIL_PINSTATE_UNKNOWN,
        RIL_PINSTATE_ENABLED_BLOCKED
    },
    /* SIM_NETWORK_SUBSET_PERSO = 8 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO,
        RIL_PERSOSUBSTATE_SIM_NETWORK,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_SERVICE_PROVIDER_PERSO = 9 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO,
        RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_CORPORATE_PERSO = 10 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO,
        RIL_PERSOSUBSTATE_SIM_CORPORATE,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_PERSO = 11 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO,
        RIL_PERSOSUBSTATE_SIM_SIM,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_STERICSSON_LOCK = 12 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO,
        RIL_PERSOSUBSTATE_UNKNOWN,    /* Ril.h have no define for ST-Ericsson lock substate... */
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_BLOCKED = 13 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_UNKNOWN,
        RIL_PERSOSUBSTATE_UNKNOWN,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_BLOCKED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_PERM_BLOCKED = 14 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_UNKNOWN,
        RIL_PERSOSUBSTATE_UNKNOWN,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_PERM_BLOCKED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_NETWORK_PERSO_PUK = 15 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO,
        RIL_PERSOSUBSTATE_SIM_NETWORK_PUK,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_NETWORK_SUBSET_PERSO_PUK = 16 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO,
        RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET_PUK,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_SERVICE_PROVIDER_PERSO_PUK = 17 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO,
        RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER_PUK,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    },
    /* SIM_CORPORATE_PERSO_PUK = 18 */
    {
        RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO,
        RIL_PERSOSUBSTATE_SIM_CORPORATE_PUK,
        NULL, NULL, 0,
        RIL_PINSTATE_ENABLED_NOT_VERIFIED,
        RIL_PINSTATE_UNKNOWN
    }
};

typedef enum {
    UICC_TYPE_UNKNOWN,
    UICC_TYPE_SIM,
    UICC_TYPE_USIM,
} UICC_Type;

static const struct timeval TIMEVAL_SIMPOLL = { 1, 0 };
static const struct timeval TIMEVAL_SIMRESET = { 60, 0 };

/* All files listed under ADF_USIM in 3GPP TS 31.102 */
static const int ef_usim_files[] = {
    0x6F05, 0x6F06, 0x6F07, 0x6F08, 0x6F09,
    0x6F2C, 0x6F31, 0x6F32, 0x6F37, 0x6F38,
    0x6F39, 0x6F3B, 0x6F3C, 0x6F3E, 0x6F3F,
    0x6F40, 0x6F41, 0x6F42, 0x6F43, 0x6F45,
    0x6F46, 0x6F47, 0x6F48, 0x6F49, 0x6F4B,
    0x6F4C, 0x6F4D, 0x6F4E, 0x6F4F, 0x6F50,
    0x6F55, 0x6F56, 0x6F57, 0x6F58, 0x6F5B,
    0x6F5C, 0x6F60, 0x6F61, 0x6F62, 0x6F73,
    0x6F78, 0x6F7B, 0x6F7E, 0x6F80, 0x6F81,
    0x6F82, 0x6F83, 0x6FAD, 0x6FB1, 0x6FB2,
    0x6FB3, 0x6FB4, 0x6FB5, 0x6FB6, 0x6FB7,
    0x6FC3, 0x6FC4, 0x6FC5, 0x6FC6, 0x6FC7,
    0x6FC8, 0x6FC9, 0x6FCA, 0x6FCB, 0x6FCC,
    0x6FCD, 0x6FCE, 0x6FCF, 0x6FD0, 0x6FD1,
    0x6FD2, 0x6FD3, 0x6FD4, 0x6FD5, 0x6FD6,
    0x6FD7, 0x6FD8, 0x6FD9, 0x6FDA, 0x6FDB,
};

/* Returns true if SIM is absent */
bool isSimAbsent()
{
    ATResponse *atresponse = NULL;
    int err;
    AT_CME_Error cme_error_code;
    bool simAbsent = true;

    err = at_send_command_singleline("AT+CPIN?", "+CPIN:", &atresponse);

    if (err < 0 || atresponse == NULL) {
        ALOGE("%s(): failed to get SIM status", __func__);
        goto exit;
    }

    if (atresponse->success == 1)
        simAbsent = false;
    else if ((cme_error_code = at_get_cme_error(atresponse)))
        if (cme_error_code != CME_SIM_NOT_INSERTED)
            simAbsent = false;

    at_response_free(atresponse);

exit:
    return simAbsent;
}

static void resetSim(void *param)
{
    ATResponse *atresponse = NULL;
    int err, state;
    char *line;

    err =
        at_send_command_singleline("AT*ESIMSR?", "*ESIMSR:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &state);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &state);
    if (err < 0)
        goto error;

    if (state == 7) {
        at_send_command("AT*ESIMR", NULL);

        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, resetSim, NULL,
                        &TIMEVAL_SIMRESET);
    } else {
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED,
                                  NULL, 0);
        pollSIMState(NULL);
    }

  finally:
    at_response_free(atresponse);
    return;

  error:
    goto finally;
}

void onSimStateChanged(const char *s)
{
    int err, state;
    char *tok;
    char *line = tok = strdup(s);

    RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, NULL,
                              0);

    /* Also check sim state, that will trigger radio state to sim absent. */
    enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, pollSIMState, (void *) 1, NULL);

    /*
     * Now, find out if we went to poweroff-state. If so, enqueue some loop
     * to try to reset the SIM for a minute or so to try to recover.
     */
    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &state);
    if (err < 0)
        goto error;

    if (state == 7)
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, resetSim, NULL, NULL);

  finally:
    free(tok);
    return;

  error:
    ALOGE("ERROR in onSimStateChanged!");
    goto finally;
}

/**
 * Get the number of retries left for pin functions
 */
static int getNumRetries (int request) {
    ATResponse *atresponse = NULL;
    int err = 0;
    char *cmd = NULL;
    char *line;
    int num_retries = -1;

#ifndef USE_U8500_RIL
    asprintf(&cmd, "AT*EPIN?");
    err = at_send_command_singleline(cmd, "*EPIN:", &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    switch (request) {
    case RIL_REQUEST_ENTER_SIM_PIN:
    case RIL_REQUEST_CHANGE_SIM_PIN:
        sscanf(line, "*EPIN: %d", &num_retries);
        break;
    case RIL_REQUEST_ENTER_SIM_PUK:
        sscanf(line, "*EPIN: %*d,%d", &num_retries);
        break;
    case RIL_REQUEST_ENTER_SIM_PIN2:
    case RIL_REQUEST_CHANGE_SIM_PIN2:
        sscanf(line, "*EPIN: %*d,%*d,%d", &num_retries);
        break;
    case RIL_REQUEST_ENTER_SIM_PUK2:
        sscanf(line, "*EPIN: %*d,%*d,%*d,%d", &num_retries);
        break;
    default:
        num_retries = -1;
        break;
    }
#else
    switch (request) {
    case RIL_REQUEST_ENTER_SIM_PIN:
    case RIL_REQUEST_CHANGE_SIM_PIN:
        asprintf(&cmd, "AT*EPINR=1");
        break;
    case RIL_REQUEST_ENTER_SIM_PUK:
        asprintf(&cmd, "AT*EPINR=3");
        break;
    case RIL_REQUEST_ENTER_SIM_PIN2:
    case RIL_REQUEST_CHANGE_SIM_PIN2:
        asprintf(&cmd, "AT*EPINR=2");
        break;
    case RIL_REQUEST_ENTER_SIM_PUK2:
        asprintf(&cmd, "AT*EPINR=4");
        break;
    default:
        return -1;
        break;
    }

    err = at_send_command_singleline(cmd, "*EPINR:", &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    sscanf(line, "*EPINR: %d", &num_retries);

#endif

finally:
    at_response_free(atresponse);
    return num_retries;
error:
    goto finally;
}

/* Function that based on number of retries, checks the PIN2 state */
static RIL_PinState getPin2State()
{
    int nrOfRetries = 0;
    RIL_PinState state = RIL_PINSTATE_UNKNOWN;

    nrOfRetries = getNumRetries(RIL_REQUEST_ENTER_SIM_PIN2); // Get number of retries for PIN2

    if (nrOfRetries < 0) {
        ALOGE("%s(): failed to get nrOfRetries for PIN2", __func__);
        goto exit;
    }

    if (nrOfRetries == 0) {
        nrOfRetries = getNumRetries(RIL_REQUEST_ENTER_SIM_PUK2);

        if (nrOfRetries < 0) {
            ALOGE("%s(): failed to get nrOfRetries for PUK2", __func__);
            goto exit;
        }

        if (nrOfRetries == 0) {
            state = RIL_PINSTATE_ENABLED_PERM_BLOCKED;
        } else {
            state = RIL_PINSTATE_ENABLED_BLOCKED;
        }
    }

exit:
    return state;
}

/*
 * returns 1 if PIN2 is required
 */
static int check_sw_for_pin2lock(RIL_SIM_IO_Response *sr) {
    int locked = 0;

    /* 0x6982 = Security status not satisfied, iso/iec 7816-4
     * 0x9804 = Access conditions not fulfilled, gsm 11.11
     */
    if ((sr->sw1 == 0x69 && sr->sw2 == 0x82) ||
       (sr->sw1 == 0x98 && sr->sw2 == 0x04)) {
        locked = 1;
    }
    return locked;
}

/** Returns one of SIM_*. Returns SIM_NOT_READY on error. */
SIM_Status getSIMStatus()
{
    ATResponse *atresponse = NULL;
    SIM_Status ret = SIM_ABSENT;
    char *cpinLine = NULL;
    char *cpinResult = NULL;


    if (at_send_command_singleline("AT+CPIN?", "+CPIN:", &atresponse) != 0) {
        ret = SIM_NOT_READY;
        goto exit;
    }

    /*
     * We need to handle SIM events even if radio is off due to SIM hotswap
     * without changing the radio state.
     */
    if (atresponse->success != 0 && (currentState() == RADIO_STATE_OFF ||
        currentState() == RADIO_STATE_UNAVAILABLE)) {
        ret = SIM_NOT_READY;
        goto exit;
    }

    if (atresponse->success == 0) {
            switch (at_get_cme_error(atresponse)) {
            case CME_SIM_NOT_INSERTED:
                ret = SIM_ABSENT;
                break;
            case CME_SIM_PIN_REQUIRED:
                ret = SIM_PIN;
                break;
            case CME_SIM_PUK_REQUIRED:
                ret = SIM_PUK;
                break;
            case CME_SIM_PIN2_REQUIRED:
                ret = SIM_PIN2;
                break;
            case CME_SIM_PUK2_REQUIRED:
                ret = SIM_PUK2;
                break;
            case CME_NETWORK_PERSONALIZATION_PIN_REQUIRED:
                ret = SIM_NETWORK_PERSO;
                break;
            case CME_NETWORK_PERSONALIZATION_PUK_REQUIRED:
                ret = SIM_NETWORK_PERSO_PUK;
                break;
            case CME_NETWORK_SUBSET_PERSONALIZATION_PIN_REQUIRED:
                ret = SIM_NETWORK_SUBSET_PERSO;
                break;
            case CME_NETWORK_SUBSET_PERSONALIZATION_PUK_REQUIRED:
                ret = SIM_NETWORK_SUBSET_PERSO_PUK;
                break;
            case CME_SERVICE_PROVIDER_PERSONALIZATION_PIN_REQUIRED:
                ret = SIM_SERVICE_PROVIDER_PERSO;
                break;
            case CME_SERVICE_PROVIDER_PERSONALIZATION_PUK_REQUIRED:
                ret = SIM_SERVICE_PROVIDER_PERSO_PUK;
                break;
            case CME_CORPORATE_PERSONALIZATION_PIN_REQUIRED:
                ret = SIM_CORPORATE_PERSO;
                break;
            case CME_CORPORATE_PERSONALIZATION_PUK_REQUIRED:
                ret = SIM_CORPORATE_PERSO_PUK;
                break;
            case CME_SIM_WRONG:
                ret = SIM_PERM_BLOCKED;
                break;
            default:
                ret = SIM_NOT_READY;
                break;
            }
        goto exit;
    }

    /* CPIN? has succeeded, now look at the result. */
    cpinLine = atresponse->p_intermediates->line;

    if (at_tok_start(&cpinLine) < 0) {
        ret = SIM_NOT_READY;
        goto exit;
    }

    if (at_tok_nextstr(&cpinLine, &cpinResult) < 0) {
        ret = SIM_NOT_READY;
        goto exit;
    }

    if (0 == strcmp(cpinResult, "READY")) {
        ret = SIM_READY;
    } else if (0 == strcmp(cpinResult, "SIM PIN")) {
        ret = SIM_PIN;
    } else if (0 == strcmp(cpinResult, "SIM PUK")) {
        ret = SIM_PUK;
    } else if (0 == strcmp(cpinResult, "SIM PIN2")) {
        ret = SIM_PIN2;
    } else if (0 == strcmp(cpinResult, "SIM PUK2")) {
        ret = SIM_PUK2;
    } else if (0 == strcmp(cpinResult, "PH-NET PIN")) {
        ret = SIM_NETWORK_PERSO;
    } else if (0 == strcmp(cpinResult, "PH-NETSUB PIN")) {
        ret = SIM_NETWORK_SUBSET_PERSO;
    } else if (0 == strcmp(cpinResult, "PH-SP PIN")) {
        ret = SIM_SERVICE_PROVIDER_PERSO;
    } else if (0 == strcmp(cpinResult, "PH-CORP PIN")) {
        ret = SIM_CORPORATE_PERSO;
    } else if (0 == strcmp(cpinResult, "PH-SIMLOCK PIN")) {
        ret = SIM_PERSO;
    } else if (0 == strcmp(cpinResult, "PH-ESL PIN")) {
        ret = SIM_STERICSSON_LOCK;
    } else if (0 == strcmp(cpinResult, "BLOCKED")) {
        int numRetries = getNumRetries(RIL_REQUEST_ENTER_SIM_PUK);
        if (numRetries == -1 || numRetries == 0)
            ret = SIM_PERM_BLOCKED;
        else
            ret = SIM_PUK2_PERM_BLOCKED;
    } else if (0 == strcmp(cpinResult, "PH-SIM PIN")) {
        /*
         * Should not happen since lock must first be set from the phone.
         * Setting this lock is not supported by Android.
         */
        ret = SIM_BLOCKED;
    } else {
        /* Unknown locks should not exist. Defaulting to "sim absent" */
        ret = SIM_ABSENT;
    }

exit:
    at_response_free(atresponse);
    return ret;
}

/**
 * Fetch information about UICC card type (SIM/USIM)
 *
 * \return UICC_Type: type of UICC card.
 */
static UICC_Type getUICCType()
{
    ATResponse *atresponse = NULL;
    static UICC_Type UiccType = UICC_TYPE_UNKNOWN;
    int err;

    if (currentState() == RADIO_STATE_OFF ||
        currentState() == RADIO_STATE_UNAVAILABLE) {
        return UICC_TYPE_UNKNOWN;
    }

    if (UiccType == UICC_TYPE_UNKNOWN) {
        err = at_send_command_singleline("AT+CUAD", "+CUAD:", &atresponse);
        if (err == 0 && atresponse->success) {
            /* USIM */
            UiccType = UICC_TYPE_USIM;
            ALOGI("Detected card type USIM - stored");
        } else if (err == 0 && !atresponse->success) {
            /* Command failed - unknown card */
            UiccType = UICC_TYPE_UNKNOWN;
            ALOGE("getUICCType(): Failed to detect card type - Retry at next request");
        } else {
            /* Legacy SIM */
            /* TODO: CUAD only responds OK if SIM is inserted.
             *       This is an inccorect AT response...
             */
            UiccType = UICC_TYPE_SIM;
            ALOGI("Detected card type Legacy SIM - stored");
        }
        at_response_free(atresponse);
    }

    return UiccType;
}

/**
 * Get the current card status.
 *
 * @return: On success returns RIL_E_SUCCESS.
 */
static int getCardStatus(RIL_CardStatus_v6 *p_card_status)
{
    SIM_Status sim_status;

    /* Initialize base card status. */
    p_card_status->card_state = RIL_CARDSTATE_ABSENT;
    p_card_status->universal_pin_state = RIL_PINSTATE_UNKNOWN;
    p_card_status->gsm_umts_subscription_app_index = RIL_CARD_MAX_APPS;
    p_card_status->cdma_subscription_app_index = RIL_CARD_MAX_APPS;
#if RIL_VERSION >= 6
    p_card_status->ims_subscription_app_index = RIL_CARD_MAX_APPS;
#endif

    p_card_status->num_applications = 0;

    /* Initialize application status. */
    int i;
    for (i = 0; i < RIL_CARD_MAX_APPS; i++)
        p_card_status->applications[i] = app_status_array[SIM_ABSENT];

    sim_status = getSIMStatus();
    ALOGI("[Card type discovery]: SIM status = %d", sim_status);

    if (sim_status == SIM_ABSENT)
        p_card_status->card_state = RIL_CARDSTATE_ABSENT;
    else if (sim_status == SIM_ERROR)
        p_card_status->card_state = RIL_CARDSTATE_ERROR;
    else {
        p_card_status->card_state = RIL_CARDSTATE_PRESENT;

        /* Only support one app, gsm/wcdma. */
        p_card_status->num_applications = 1;
        p_card_status->gsm_umts_subscription_app_index = 0;

        /* Get the correct app status. */
        p_card_status->applications[0] = app_status_array[sim_status];

        if (p_card_status->applications[0].perso_substate == RIL_PERSOSUBSTATE_READY) {
            p_card_status->applications[0].pin2 = getPin2State(); // Get PIN2 state based on number of retries
        }

        /* Get the correct app type */
        if (getUICCType() == UICC_TYPE_SIM)
            ALOGI("[Card type discovery]: Legacy SIM");
        else { /* defaulting to USIM */
            ALOGI("[Card type discovery]: USIM");
            p_card_status->applications[0].app_type = RIL_APPTYPE_USIM;
        }
    }

    return RIL_E_SUCCESS;
}

/**
 * Free the card status returned by getCardStatus.
 */
static void freeCardStatus(RIL_CardStatus_v6 * p_card_status)
{
    free(p_card_status);
}

/**
 * SIM ready means any commands that access the SIM will work, including:
 *  AT+CPIN, AT+CSMS, AT+CNMI, AT+CRSM
 *  (all SMS-related commands).
 */
void pollSIMState(void *param)
{
    if (((int) param) != 1 &&
        currentState() != RADIO_STATE_SIM_NOT_READY &&
        currentState() != RADIO_STATE_SIM_LOCKED_OR_ABSENT)
        /* No longer valid to poll. */
        return;

    switch (getSIMStatus()) {
    case SIM_NOT_READY:
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, pollSIMState, NULL,
                        &TIMEVAL_SIMPOLL);
        return;

    case SIM_PIN2:
    case SIM_PUK2:
    case SIM_READY:
        setRadioState(RADIO_STATE_SIM_READY);
        return;
    case SIM_ABSENT:
        /* Only change radio state if radio is not off */
        if (currentState() != RADIO_STATE_OFF) {
            setRadioState(RADIO_STATE_SIM_LOCKED_OR_ABSENT);
        } else {
            /* Trigger a GET_SIM_STATE */
            RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED,
                                      NULL, 0);
        }
        return;
    case SIM_PIN:
    case SIM_PUK:
    case SIM_NETWORK_PERSO:
    case SIM_NETWORK_SUBSET_PERSO:
    case SIM_SERVICE_PROVIDER_PERSO:
    case SIM_CORPORATE_PERSO:
    case SIM_PERSO:
    case SIM_STERICSSON_LOCK:
    case SIM_BLOCKED:
    default:
        setRadioState(RADIO_STATE_SIM_LOCKED_OR_ABSENT);
        return;
    }
}

/**
 * RIL_REQUEST_GET_SIM_STATUS
 *
 * Requests status of the SIM interface and the SIM card.
 *
 * Valid errors:
 *  Must never fail.
 */
void requestGetSimStatus(void *data, size_t datalen, RIL_Token t)
{
    RIL_CardStatus_v6 *card_status = NULL;

    card_status = malloc(sizeof(*card_status));
    assert(card_status != NULL);

    (void)getCardStatus(card_status);

    RIL_onRequestComplete(t, RIL_E_SUCCESS, (char *) card_status,
                          sizeof(*card_status));

    free(card_status);

    return;
}

static int simIOGetLogicalChannel()
{
    ATResponse *atresponse = NULL;
    static int g_lc = 0;
    char *cmd = NULL;
    int err = 0;

    if (g_lc == 0) {
        struct tlv tlvApp, tlvAppId;
        char *line;
        char *resp;

        err = at_send_command_singleline("AT+CUAD", "+CUAD:", &atresponse);
        if (err < 0)
            goto error;
        if (atresponse->success == 0) {
            err = -1;
            goto error;
        }

        line = atresponse->p_intermediates->line;
        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextstr(&line, &resp);
        if (err < 0)
            goto error;

        err = parseTlv(resp, &resp[strlen(resp)], &tlvApp);
        if (err < 0)
            goto error;
        if (tlvApp.tag != 0x61) { /* Application */
            err = -1;
            goto error;
        }

        err = parseTlv(tlvApp.data, tlvApp.end, &tlvAppId);
        if (err < 0)
            goto error;
        if (tlvAppId.tag != 0x4F) { /* Application ID */
            err = -1;
            goto error;
        }

        asprintf(&cmd, "AT+CCHO=\"%.*s\"",
            tlvAppId.end - tlvAppId.data, tlvAppId.data);
        if (cmd == NULL) {
            err = -1;
            goto error;
        }

        at_response_free(atresponse);
        err = at_send_command_singleline(cmd, "+CCHO:", &atresponse);
        if (err < 0)
            goto error;

        if (atresponse->success == 0) {
            err = -1;
            goto error;
        }

        line = atresponse->p_intermediates->line;
        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &g_lc);
        if (err < 0)
            goto error;
    }

finally:
    at_response_free(atresponse);
    free(cmd);
    return g_lc;

error:
    goto finally;
}

static int simIOSelectFile(unsigned short fileid)
{
    int err = 0;
    char *cmd = NULL;
    unsigned short lc = simIOGetLogicalChannel();
    ATResponse *atresponse = NULL;
    char *line = NULL;
    char *resp = NULL;
    int resplen;

    if (lc == 0) {
        err = -1;
        goto error;
    }

    asprintf(&cmd, "AT+CGLA=%d,14,\"00A4000C02%.4X\"",
        lc, fileid);
    if (cmd == NULL) {
        err = -1;
        goto error;
    }

    err = at_send_command_singleline(cmd, "+CGLA:", &atresponse);
    if (err < 0)
        goto error;
    if (atresponse->success == 0) {
        err = -1;
        goto error;
    }

    line = atresponse->p_intermediates->line;
    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &resplen);
    if (err < 0)
        goto error;

    err = at_tok_nextstr(&line, &resp);
    if (err < 0)
        goto error;

    /* Std resp code: "9000" */
    if (resplen != 4 || strcmp(resp, "9000") != 0) {
        err = -1;
        goto error;
    }

finally:
    at_response_free(atresponse);
    free(cmd);
    return err;

error:
    goto finally;
}

static int simIOSelectPath(const char *path, unsigned short fileid)
{
    int err = 0;
    size_t path_len = 0;
    size_t pos;
    static char cashed_path[4 * 10 + 1] = {0};
    static unsigned short cashed_fileid = 0;

    if (path == NULL) {
        path = "3F00";
    }
    path_len = strlen(path);

    if (path_len & 3) {
        err = -1;
        goto error;
    }

    if ((fileid != cashed_fileid) || (strcmp(path, cashed_path) != 0)) {
        for (pos = 0; pos < path_len; pos += 4) {
            unsigned val;
            if (sscanf(&path[pos], "%4X", &val) != 1) {
                err = -1;
                goto error;
            }
            err = simIOSelectFile(val);
            if (err < 0)
                goto error;
        }
        err = simIOSelectFile(fileid);
    }
    if (path_len < sizeof(cashed_path)) {
        strcpy(cashed_path, path);
        cashed_fileid = fileid;
    } else {
        cashed_path[0] = 0;
        cashed_fileid = 0;
    }

finally:
    return err;

error:
    goto finally;
}

int sendSimIOCmdUICC(const RIL_SIM_IO_v6 *ioargs, ATResponse **atresponse, RIL_SIM_IO_Response *sr)
{
    int err = 0;
    int resplen;
    char *line = NULL, *resp = NULL;
    char *cmd = NULL, *data = NULL;
    unsigned short lc = simIOGetLogicalChannel();
    unsigned char sw1[1], sw2[1];

    if (lc == 0) {
        err = -1;
        goto error;
    }

    memset(sr, 0, sizeof(*sr));

    switch (ioargs->command) {
        case 0xC0: /* Get response */
            /* Convert Get response to Select. */
            asprintf(&data, "00A4000402%.4X00",
                ioargs->fileid);
            break;

        case 0xB0: /* Read binary */
        case 0xB2: /* Read record */
            asprintf(&data, "00%.2X%.2X%.2X%.2X",
                (unsigned char)ioargs->command,
                (unsigned char)ioargs->p1,
                (unsigned char)ioargs->p2,
                (unsigned char)ioargs->p3);
            break;

        case 0xD6: /* Update binary */
        case 0xDC: /* Update record */
            if (!ioargs->data) {
                err = -1;
                goto error;
            }
            asprintf(&data, "00%.2X%.2X%.2X%.2X%s",
                (unsigned char)ioargs->command,
                (unsigned char)ioargs->p1,
                (unsigned char)ioargs->p2,
                (unsigned char)ioargs->p3,
                ioargs->data);
            break;

        default:
            err = -1;
            goto error;
    }
    if (data == NULL) {
        err = -1;
        goto error;
    }

    asprintf(&cmd, "AT+CGLA=%d,%d,\"%s\"", lc, strlen(data), data);
    if (cmd == NULL) {
        err = -1;
        goto error;
    }

    err = simIOSelectPath(ioargs->path, ioargs->fileid);
    if (err < 0)
        goto error;

    err = at_send_command_singleline(cmd, "+CGLA:", atresponse);
    if (err < 0)
        goto error;

    if ((*atresponse)->success == 0) {
        err = -1;
        goto error;
    }

    line = (*atresponse)->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &resplen);
    if (err < 0)
        goto error;

    err = at_tok_nextstr(&line, &resp);
    if (err < 0)
        goto error;

    if ((resplen < 4) || ((size_t)resplen != strlen(resp))) {
        err = -1;
        goto error;
    }

    err = stringToBinary(&resp[resplen - 4], 2, (unsigned char *)&sw1);
    if (err < 0)
        goto error;

    err = stringToBinary(&resp[resplen - 2], 2, (unsigned char *)&sw2);
    if (err < 0)
        goto error;

    sr->sw1 = sw1[1];
    sr->sw2 = sw2[1];
    resp[resplen - 4] = 0;
    sr->simResponse = resp;

finally:
    free(cmd);
    free(data);
    return err;

error:
    goto finally;

}

int sendSimIOCmdICC(const RIL_SIM_IO_v6 *ioargs, ATResponse **atresponse, RIL_SIM_IO_Response *sr)
{
    int err = 0;
    char *cmd = NULL;
    char *fmt = NULL;
    char *arg6 = NULL;
    char *arg7 = NULL;
    char *line = NULL;

    arg6 = ioargs->data;
    arg7 = ioargs->path;

    if (arg7 && arg6) {
        fmt = "AT+CRSM=%d,%d,%d,%d,%d,\"%s\",\"%s\"";
    } else if (arg7) {
        fmt = "AT+CRSM=%d,%d,%d,%d,%d,,\"%s\"";
        arg6 = arg7;
    } else if (arg6) {
        fmt = "AT+CRSM=%d,%d,%d,%d,%d,\"%s\"";
    } else {
        fmt = "AT+CRSM=%d,%d,%d,%d,%d";
    }

    asprintf(&cmd, fmt,
             ioargs->command, ioargs->fileid,
             ioargs->p1, ioargs->p2, ioargs->p3,
             arg6, arg7);

    if (cmd == NULL) {
        err = -1;
        goto error;
    }

    err = at_send_command_singleline(cmd, "+CRSM:", atresponse);
    if (err < 0)
        goto error;

    if ((*atresponse)->success == 0) {
        err = -1;
        goto error;
    }

    line = (*atresponse)->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &(sr->sw1));
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &(sr->sw2));
    if (err < 0)
        goto error;

    if (at_tok_hasmore(&line)) {
        err = at_tok_nextstr(&line, &(sr->simResponse));
        if (err < 0)
            goto error;
    }

finally:
    free(cmd);
    return err;

error:
    goto finally;
}

static int sendSimIOCmd(const RIL_SIM_IO_v6 *ioargs, ATResponse **atresponse, RIL_SIM_IO_Response *sr)
{
    int err = 0;

    if (sr == NULL)
        return -1;

    err = sendSimIOCmdICC(ioargs, atresponse, sr);
    (void)check_sw_for_pin2lock(sr);

    return err;
}

static int convertSimIoFcp(RIL_SIM_IO_Response *sr, char **cvt)
{
    int err = 0;
    size_t fcplen;
    struct ts_51011_921_resp resp;
    void *cvt_buf = NULL;

    if (!sr->simResponse || !cvt) {
        err = -1;
        goto error;
    }

    fcplen = strlen(sr->simResponse);
    if ((fcplen == 0) || (fcplen & 1)) {
        err = -1;
        goto error;
    }

    err = fcp_to_ts_51011(sr->simResponse, fcplen, &resp);
    if (err < 0)
        goto error;

    cvt_buf = malloc(sizeof(resp) * 2 + 1);
    if (!cvt_buf) {
        err = -1;
        goto error;
    }

    err = binaryToString((unsigned char*)(&resp),
                   sizeof(resp), cvt_buf);
    if (err < 0)
        goto error;

    /* cvt_buf ownership is moved to the caller */
    *cvt = cvt_buf;
    cvt_buf = NULL;

finally:
    return err;

error:
    free(cvt_buf);
    goto finally;
}

/**
 * verifySimPIN2() - Enter PIN2 for verification
 *
 * Returns:  0 on success
 *          -1 on generic ECEXPIN failure
 *          -2 on PIN2 required
 *          -3 on PUK2 required
 *          -4 on password incorrect
 *          -5 on other unknown ECEXPIN response
 */
static int verifySimPin2(char *pin2) {
    ATResponse *atresponse = NULL;
    int ret = -1;  /* generic failure */
    AT_CME_Error cme_error_code = -1;
    char *cmd = NULL;

    asprintf(&cmd, "AT*ECEXPIN=1,,\"SIM PIN2\",\"%s\"", pin2);
    ret = at_send_command(cmd, &atresponse);

    if (ret < 0) {
        /* This is to ensure that GENERIC_FAILURE is sent to JAVA RIL
         * and not the return code from send command failure, because
         * it is possible that the return value may fall in any of the
         * pin errors that get mapped from the AT response below. */
        ret = -1;
        goto exit;
    }

    if (atresponse->success == 0) {
        if (at_get_cme_error_new(atresponse, &cme_error_code)) {
            switch (cme_error_code) {
            case CME_SIM_PIN2_REQUIRED:
                ret = -2;
                break;
            case CME_SIM_PUK2_REQUIRED:
                ret = -3;
                break;
            case CME_INCORRECT_PASSWORD:
                ret = -4;
                break;
            default:
                ret = -5;
                break;
            }
        }
        goto exit;
    }

exit:
    free(cmd);
    at_response_free(atresponse);
    return ret;
}

/**
 * RIL_REQUEST_SIM_IO
 *
 * Request SIM I/O operation.
 * This is similar to the TS 27.007 "restricted SIM" operation
 * where it assumes all of the EF selection will be done by the
 * callee.
 */
void requestSIM_IO(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    RIL_SIM_IO_Response sr;
    RIL_SIM_IO_v6 ioargsDup;
    int cvt_done = 0;
    int rilErrorCode;
    AT_CME_Error cme_error_code = -1;
    bool pathReplaced = false;

    /*
     * Android telephony framework does not support USIM cards properly,
     * and RIL needs to change the file path of all files listed under the
     * ADF_USIM directory in TS 31.102.
     */
    memcpy(&ioargsDup, data, sizeof(RIL_SIM_IO_v6));
    if (UICC_TYPE_SIM != getUICCType()) {
        unsigned int i;
        unsigned int count = sizeof(ef_usim_files) / sizeof(int);

        for (i = 0; i < count; i++) {
            if (ef_usim_files[i] == ioargsDup.fileid) {
                asprintf(&ioargsDup.path, "3F007FFF");
                pathReplaced = true;
                break;
            }
        }
    }

    do {
        /* Reset values for file access */
        rilErrorCode = RIL_E_GENERIC_FAILURE;
        memset(&sr, 0, sizeof(sr));
        sr.simResponse = NULL;
        at_response_free(atresponse);
        atresponse = NULL;

        /* Verify SIM PIN 2 if present */
        if (ioargsDup.pin2 != NULL ) {
          int pinRes = verifySimPin2(ioargsDup.pin2);
          if (pinRes == -1) { /* Unknown error entering PIN2 */
              ALOGD("%s(): Failed executing enter PIN2 for SIM IO, "
                   "unknown error", __func__);
              rilErrorCode = RIL_E_GENERIC_FAILURE;
              break;
          } else if (pinRes == -2) { /* Entered PIN2 state */
              ALOGD("%s(): Failed entering PIN2 for SIM IO, "
                  "probably incorrect PIN2", __func__);
              rilErrorCode = RIL_E_SIM_PIN2;
              break;
          } else if (pinRes == -3) { /* Entered PUK2 state */
              ALOGD("%s(): Failed entering PIN2 for SIM IO, "
                  "probably incorrect PIN2 leading to PUK2 state", __func__);
              rilErrorCode = RIL_E_SIM_PUK2;
              break;
          } else if (pinRes == -4) { /* Password incorrect */
              ALOGD("%s(): Failed entering PIN2 for SIM IO, "
                  "incorrect password", __func__);
              rilErrorCode = RIL_E_PASSWORD_INCORRECT;
              break;
          } else if ( pinRes == -5 ) { /* unknown error */;
              ALOGD("%s(): Failed entering PIN2 for SIM IO, "
                   "unknown error", __func__);
              rilErrorCode = RIL_E_GENERIC_FAILURE;
              break;
          } else { /* PIN2 verified successfully */
              /* Continue processing the IO command */
          }
        }

        /* Requesting SIM IO */
        if (sendSimIOCmd(&ioargsDup, &atresponse, &sr) < 0) {
            break;
        }
        /* If success break early and finish */
        if (atresponse->success > 0 && sr.sw1 == 0x90 && sr.sw2 == 0x00) {
            rilErrorCode = RIL_E_SUCCESS;
            break;
        }

        /*
         * If file operation failed it might be that PIN2 or PUK2 is required
         * for file access. This is detected and PIN2/PUK2 error is reported
         */
        /* AT Command Error Check */

        if (atresponse->success == 0 &&
            at_get_cme_error_new(atresponse, &cme_error_code)) {
            if (cme_error_code == CME_SIM_PIN2_REQUIRED)
                rilErrorCode = RIL_E_SIM_PIN2;
            else if (cme_error_code == CME_SIM_PUK2_REQUIRED)
                rilErrorCode = RIL_E_SIM_PUK2;
        }
        /* Sw1, Sw2 Error Check (0x6982 = Security status not satisfied, iso/iec 7816-4)
         *                      (0x9804 = Access conditions not fulfilled, gsm 11.11) */
        else if (check_sw_for_pin2lock(&sr)) {
            sleep(1); /* The SIM takes some time to actually do the state change */
            SIM_Status simState = getSIMStatus();
            if (simState == SIM_PIN2)
                rilErrorCode = RIL_E_SIM_PIN2;
            else if (simState == SIM_PUK2)
                rilErrorCode = RIL_E_SIM_PUK2;
        }
    } while (0);

    /* Finally send response to Android */
    if (rilErrorCode == RIL_E_SUCCESS) {
        /*
         * In case the command is GET_RESPONSE and cardtype is 3G SIM
         * conversion to 2G FCP is required
         */
        if (ioargsDup.command == 0xC0 && getUICCType() != UICC_TYPE_SIM) {
            if (convertSimIoFcp(&sr, &sr.simResponse) < 0) {
                RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
                goto exit;
            }
            cvt_done = 1; /* sr.simResponse needs to be freed */
        }

        RIL_onRequestComplete(t, RIL_E_SUCCESS, &sr, sizeof(sr));
    } else
        RIL_onRequestComplete(t, rilErrorCode, NULL, 0);

exit:
    at_response_free(atresponse);
    if (cvt_done)
        free(sr.simResponse);
    /* Free duplicated variables */
    if (pathReplaced)
        free(ioargsDup.path);
}

/**
 * Enter SIM PIN, might be PIN, PIN2, PUK, PUK2, etc.
 *
 * Data can hold pointers to one or two strings, depending on what we
 * want to enter. (PUK requires new PIN, etc.).
 *
 */
void requestEnterSimPin(void *data, size_t datalen, RIL_Token t, int request)
{
    ATResponse *atresponse = NULL;
    int err;
    int cme_err;
    char *cmd = NULL;
    const char **strings = (const char **) data;
    int num_retries = -1;
    char *code;
    int nr_strings;
    switch (request) {
        case RIL_REQUEST_ENTER_SIM_PIN2:
            code = "SIM PIN2";
            break;
        case RIL_REQUEST_ENTER_SIM_PUK2:
            code = "SIM PUK2";
            break;
        case RIL_REQUEST_ENTER_SIM_PIN:
            code = "SIM PIN";
            break;
        case RIL_REQUEST_ENTER_SIM_PUK:
            code = "SIM PUK";
            break;
        default:
            ALOGE("requestEnterSimPin(): Invalid RIL request!");
            goto error;
    }

#if RIL_VERSION >= 6
    nr_strings = ( (datalen - sizeof(char *) == sizeof(char *)) ? 1 : 2);
#else
    nr_strings = ( (datalen == sizeof(char *)) ? 1 : 2);
#endif

    if (nr_strings == 1)
        asprintf(&cmd, "AT*ECEXPIN=1,,\"%s\",\"%s\"", code, strings[0]);
    else
        asprintf(&cmd, "AT*ECEXPIN=1,,\"%s\",\"%s\",\"%s\"", code, strings[0], strings[1]);

    err = at_send_command(cmd, &atresponse);
    free(cmd);

    cme_err = at_get_cme_error(atresponse);

    if (cme_err != CME_SUCCESS && (err < 0 || atresponse->success == 0)) {
        if (cme_err == 16 || cme_err == 17 || cme_err == 18 || cme_err == 11 || cme_err == 12 || cme_err == 262) {
            num_retries = getNumRetries(request);
            if (cme_err == 18 && request == RIL_REQUEST_ENTER_SIM_PIN2) {
                /*
                 * This is not documented in ril.h as a valid error code.
                 * But this is the one that gives a PUK2 dialog.
                 */
                RIL_onRequestComplete(t, RIL_E_SIM_PUK2, &num_retries, sizeof(int *));
            } else {
                RIL_onRequestComplete(t, RIL_E_PASSWORD_INCORRECT, &num_retries, sizeof(int *));
            }
        } else
            goto error;
    } else {
        /*
         * Got OK, return success and wait for *EPEV to trigger poll
         * of SIM state.
         */
        num_retries = getNumRetries(request);
        RIL_onRequestComplete(t, RIL_E_SUCCESS, &num_retries, sizeof(int *));
    }

  finally:
    at_response_free(atresponse);
    return;
  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

void requestChangePassword(char *facility, void *data, size_t datalen,
                           RIL_Token t, int request)
{
    int err = 0;
    char *oldPassword = NULL;
    char *newPassword = NULL;
    char *cmd = NULL;
    ATResponse *atresponse = NULL;
    int num_retries = -1;
#ifdef USE_U8500_RIL
    RIL_Errno errorril = RIL_E_GENERIC_FAILURE;
#endif

    if ( ! (datalen >= 2 * sizeof(char *)) || strlen(facility) != 2) {
        goto error;
    }


    oldPassword = ((char **) data)[0];
    newPassword = ((char **) data)[1];

    asprintf(&cmd, "AT+CPWD=\"%s\",\"%s\",\"%s\"", facility, oldPassword,
             newPassword);

    err = at_send_command(cmd, &atresponse);
    free(cmd);

    num_retries = getNumRetries(request);

    if (err < 0 || atresponse->success == 0) {
#ifdef USE_U8500_RIL
        switch (at_get_cme_error(atresponse)) {
            case 11:
                /* CME ERROR 11: "SIM PIN required" happens when PIN is wrong */
                ALOGE("requestChangePassword(): wrong PIN");
                errorril = RIL_E_PASSWORD_INCORRECT;
                break;
            case 12:
                /* CME ERROR 12: "SIM PUK required" happens when wrong PIN is used 3 times in a row*/
                ALOGE("requestChangePassword() PIN locked, change PIN with PUK");
                num_retries = 0;/* PUK required */
                errorril = RIL_E_PASSWORD_INCORRECT;
                break;
            case 16:
                /*
                 * CME ERROR 16: "Incorrect password" happens when wrong PIN length is used
                 * or if faulty password is given using 2G sim-card
                 */
                ALOGE("requestChangePassword() Wrong PIN ");
                if (RIL_REQUEST_CHANGE_SIM_PIN == request || RIL_REQUEST_CHANGE_SIM_PIN2 == request) {
                    errorril = RIL_E_PASSWORD_INCORRECT;
                }
                break;
            case 17:
                /* CME ERROR 17: "SIM PIN2 required" happens when PIN2 is wrong */
                ALOGE("requestChangePassword() wrong PIN2");
                errorril = RIL_E_PASSWORD_INCORRECT;
                break;

            case 18:
                /* CME ERROR 18: "SIM PUK2 required" happens when wrong PIN2 is used 3 times in a row */
                ALOGE("requestChangePassword() PIN2 locked, change PIN2 with PUK2");
                num_retries = 0;/* PUK2 required */
                /*
                 * This is not documented in ril.h as a valid error code.
                 * But this is the one that gives a PUK2 dialog.
                 */
                errorril = RIL_E_SIM_PUK2;
                break;
            default:
                num_retries = -1; /* some other error */
                goto error;
        }
        RIL_onRequestComplete(t, errorril, &num_retries, sizeof(int *));
        goto finally;

#else

        if (at_get_cme_error(atresponse) == 16) {
            RIL_onRequestComplete(t, RIL_E_PASSWORD_INCORRECT, &num_retries, sizeof(int *));
            goto finally;
        }
        goto error;
#endif
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &num_retries, sizeof(int *));

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE,  NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_FACILITY_LOCK
 *
 * Enable/disable one facility lock.
 */
void requestSetFacilityLock(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;
    char *facility_string = NULL;
    int facility_mode = -1;
    char *facility_mode_str = NULL;
    char *facility_password = NULL;
    char empty_password[] = "";
    int num_retries = -1;
    RIL_Errno errorril = RIL_E_GENERIC_FAILURE;
#ifdef USE_U8500_RIL
    char *facility_class_str = NULL;
    int facility_class = 0;
#else
    char *facility_class = NULL;
#endif

    assert(datalen >= (4 * sizeof(char **)));

    facility_string = ((char **) data)[0];
    facility_mode_str = ((char **) data)[1];
    facility_password = ((char **) data)[2];

    assert(*facility_mode_str == '0' || *facility_mode_str == '1');
    facility_mode = atoi(facility_mode_str);

#ifdef USE_U8500_RIL
    facility_class_str = ((char **) data)[3];
    if (facility_class_str != NULL) {
        facility_class = atoi(facility_class_str);
    }

    if (!facility_password) {
        facility_password = empty_password;
    }

    if (0 == facility_class) {
        facility_class = SS_DEFAULT_CLASS; /* if not set, use default. */
    }

    asprintf(&cmd, "AT+CLCK=\"%s\",%d,\"%s\",%d", facility_string,
            facility_mode, facility_password, facility_class);
#else
    facility_class = ((char **) data)[3];

    asprintf(&cmd, "AT+CLCK=\"%s\",%d,\"%s\",%s", facility_string,
             facility_mode, facility_password, facility_class);
#endif

    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0){
        goto error;
    }
    if (atresponse->success == 0){
        switch (at_get_cme_error(atresponse)) {
            case 11: /* CME ERROR 11: "SIM PIN required" happens when
                PIN is wrong */
                ALOGE("requestSetFacilityLock(): wrong PIN");
                num_retries = getNumRetries(RIL_REQUEST_ENTER_SIM_PIN);
                errorril = RIL_E_PASSWORD_INCORRECT;
                break;
            case 12:/* CME ERROR 12: "SIM PUK required" happens when wrong PIN
                is used 3 times in a row*/
                ALOGE("requestSetFacilityLock() PIN locked,"
                " change PIN with PUK");
                num_retries = 0;/* PUK required */
                errorril = RIL_E_PASSWORD_INCORRECT;
                break;
            case 16: /* CME ERROR 16: "Incorrect password" */
                ALOGE("requestSetFacilityLock(): Incorrect password");
                if (strncmp(facility_string, "SC", 2) == 0) {
                    num_retries = getNumRetries(RIL_REQUEST_ENTER_SIM_PIN);
                } else if  (strncmp(facility_string, "FD", 2) == 0) {
                    num_retries = getNumRetries(RIL_REQUEST_ENTER_SIM_PIN2);
                }
                errorril = RIL_E_PASSWORD_INCORRECT;
                break;
            case 17: /* CME ERROR 17: "SIM PIN2 required" happens when
                PIN2 is wrong */
                ALOGE("requestSetFacilityLock() wrong PIN2");
                num_retries = getNumRetries(RIL_REQUEST_ENTER_SIM_PIN2);
                errorril = RIL_E_PASSWORD_INCORRECT;
                break;

            case 18:/* CME ERROR 18: "SIM PUK2 required" happens when wrong
                PIN2 is used 3 times in a row*/
                ALOGE("requestSetFacilityLock() PIN2 locked, change PIN2"
                "with PUK2");
                num_retries = 0;/*PUK2 required*/
                errorril = RIL_E_SIM_PUK2 ; /* This is not documented in ril.h as a valid error code.
                                               But this is the one that gives a PUK2 dialog.*/
                break;
            default:
                num_retries = -1; /*some other error*/
        }
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &num_retries, sizeof(int *));
    at_response_free(atresponse);
    return;

  error:
    at_response_free(atresponse);
    RIL_onRequestComplete(t, errorril, &num_retries,  sizeof(int *));
}


/**
 * RIL_REQUEST_QUERY_FACILITY_LOCK
 *
 * Query the status of a facility lock state.
 */
void requestQueryFacilityLock(void *data, size_t datalen, RIL_Token t)
{
    int err;
    int response = 0;
    ATResponse *atresponse = NULL;
    char *cmd = NULL;
    char *line = NULL;
    char *facility_string = NULL;
    char *facility_password = NULL;
    char empty_password[] = "";
#ifdef USE_U8500_RIL
    ATLine *cursor = NULL;
    char *facility_class_str = NULL;
    int facility_class = 0;
    int lineno = 0;
#else
    char *facility_class = NULL;
#endif
    assert(datalen >= (3 * sizeof(char **)));

    facility_string = ((char **) data)[0];
    facility_password = ((char **) data)[1];
#ifdef USE_U8500_RIL
    facility_class_str = ((char **) data)[2];

    if (facility_class_str != NULL) {
        facility_class = atoi(facility_class_str);
    }

    if (0 == facility_class) {
        facility_class = SS_DEFAULT_CLASS; /* if not set, use default. */
    }

    if (!facility_password) {
        facility_password = empty_password;
    }

    asprintf(&cmd, "AT+CLCK=\"%s\",2,\"%s\",%d", facility_string,
            facility_password, facility_class);

    err = at_send_command_multiline(cmd, "+CLCK:", &atresponse);
#else
    facility_class = ((char **) data)[2];

    asprintf(&cmd, "AT+CLCK=\"%s\",2,\"%s\",%s", facility_string,
             facility_password, facility_class);
    err = at_send_command_singleline(cmd, "+CLCK:", &atresponse);
#endif
    free(cmd);

    if (err < 0 || atresponse->success == 0) {
        goto error;
    }
#ifdef USE_U8500_RIL
    /* When <mode> =2 and command successful:
           +CLCK: <status>,<class1>[<CR><LF>+CLCK: <status>,<class2>[...]]
       Note however that if call-baring is inactive the response will be simply:
           +CLCK: 0 */

    if ((0 == strcmp(facility_string, "SC")) ||
        (0 == strcmp(facility_string, "FD"))) {
        /* If facility_string = "SC" or "FD" then no class will be returned by AT.
           The response must then be constructed by testing on the status then
           respond with the class Android asked for */
        int status = 0;

        line = atresponse->p_intermediates->line;

        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &status);
        if (err < 0)
            goto error;

        if (status == 1) {
            /* Use the class given by Android */
            response = facility_class;
        }
#ifdef FDN_SERVICE_AVAILABILITY
        else if (status == 2) {
            response = status;
        }
#endif

    } else {

        for (cursor = atresponse->p_intermediates; cursor != NULL;
             cursor = cursor->p_next) {
            int serviceClass = 0;
            int status = 0;
            line = cursor->line;

            err = at_tok_start(&line);
            if (err < 0)
                goto error;

            err = at_tok_nextint(&line, &status);
            if (err < 0)
                goto error;

            err = at_tok_nextint(&line, &serviceClass);
            if (err < 0 && lineno > 0)
                goto error;

            if (status == 1 && serviceClass > 0 && serviceClass <= 128)
                response |= serviceClass;

            lineno++;
        }
    }
#else
    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);

    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &response);

    if (err < 0)
        goto error;
#endif

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));

finally:
    at_response_free(atresponse);
    return;

error:
    if (at_get_cme_error(atresponse) == CME_FDN_BLOCKED) {
        RIL_onRequestComplete(t, RIL_E_FDN_CHECK_FAILURE, NULL, 0);
    } else {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
    goto finally;
}

/**
 * RIL_REQUEST_ISIM_AUTHENTIACATION
 *
 * Request the ISIM application on the UICC to perform AKA
 * challenge/response algorithm for IMS authentication
 *
 * "data" is a const char * containing the challenge string in Base64 format
 * "response" is a const char * containing the response in Base64 format
 */
void requestISIMAuthentication(void *data, size_t datalen, RIL_Token t)
{
    ALOGW("U300::requestISIMAuthentication NOT IMPLEMENTED");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * Check if string array contains the passed string.
 *
 * \param str: string to examine.
 * \param list: NULL-terminated string array to examine.
 * \return non zero if string is found.
 */
static int isStringInList(const char *str, const char **list)
{
    size_t pos;
    for (pos = 0; list[pos]; ++pos) {
        if (!strcmp(str, list[pos]))
            return 1;
    }
    return 0;
}

/**
 * Append a string to the comma-separated list.
 *
 * \param phone_list: Pointer to the comma-separated list.
 * \param str: String to be appended.
 * \note Function allocates memory for the phone_list.
 */
static void phoneListAppend(char **phone_list, const char *str)
{
    if (*phone_list) {
        char *tmp;
        asprintf(&tmp, "%s,%s", *phone_list, str);
        free(*phone_list);
        *phone_list = tmp;
    } else {
        asprintf(phone_list, "%s", str);
    }
}

/**
 * Append a NULL-terminated string array to the comma-separated list.
 *
 * \param phone_list: Pointer to the comma-separated list.
 * \param list: NULL-terminated string array to be appended.
 * \note Function allocates memory for the phone_list.
 */
static void phoneListAppendList(char **phone_list, const char **list)
{
    size_t pos;
    for (pos = 0; list[pos]; ++pos) {
        phoneListAppend(phone_list, list[pos]);
    }
}

/**
 * Convert a Called party BCD number (defined in 3GPP TS 24.008)
 * to an Ascii number.
 *
 * \param bcd: input character
 *
 * \return Ascii value
 */
static char bcdToAscii(const unsigned char bcd)
{
    switch(bcd & 0x0F) {
        case 0x0A:
            return '*';
        case 0x0B:
            return '#';
        case 0x0C:
            return 'a';
        case 0x0D:
            return 'b';
        case 0x0E:
            return 'c';
        case 0x0F:
            return 0;
        default:
            return (bcd + '0');
    }
}

/**
 * Store an ECC list in the r/w ECC list property (ril.ecclist).
 *
 * \param list: ECC list in 3GPP TS 51.011, 10.3.27 format.
 * \param use_japan_extensions: Use Japan-specific ECC numbers.
 * \note Function appends standard ECC numbers to the list provided.
 */
static void storeEccList(const char *list, int use_japan_extensions)
{
    /* Phone number conversion as per 3GPP TS 51.011, 10.3.27 */
    char *buf = NULL;
    size_t pos;
    size_t len = strlen(list);
    static const char *std_ecc[] = {"112", "911", NULL};
    static const char *std_ecc_jpn[] = {"110", "118", "119", NULL};

    ALOGD("[ECC]: ECC list from SIM (length: %d): %s", (int) len, list);

    for (pos = 0; pos + 6 <= len; pos += 6) {
        size_t i;
        char dst[7];

        for (i = 0; i < 6; i += 2) {
            char digit1 = bcdToAscii(char2nib(list[pos + i + 1]));
            char digit2 = bcdToAscii(char2nib(list[pos + i + 0]));

            dst[i + 0] = digit1;
            dst[i + 1] = digit2;
        }
        dst[i] = 0;

        if (dst[0]) {
            if (isStringInList(dst, std_ecc))
                continue;
            if (use_japan_extensions && isStringInList(dst, std_ecc_jpn))
                continue;

            phoneListAppend(&buf, dst);
        }
    }

    if (buf == NULL || strlen(buf) == 0) {
        ALOGI("[ECC]: No valid ECC numbers on SIM, keeping defaults");
        goto exit;
    }

    phoneListAppendList(&buf, std_ecc);
    if (use_japan_extensions)
        phoneListAppendList(&buf, std_ecc_jpn);
    if (buf) {
        ALOGD("[ECC]: ECC phone numbers: %s", buf);
        (void)property_set(PROP_EMERGENCY_LIST_RW, buf);
        free(buf);
    }
exit:
    return;
}

/*
 * Reads the emergency call codes from the EF_ECC file in the SIM
 * card from path "3F007F20" using "READ BINARY" command.
 */
void read2GECCFile(int use_japan_extensions)
{
    int err = 0;
    ATResponse *atresponse = NULL;
    RIL_SIM_IO_v6 ioargs;
    RIL_SIM_IO_Response sr;
    memset(&ioargs, 0, sizeof(ioargs));
    memset(&sr, 0, sizeof(sr));
    sr.simResponse = NULL;

    ioargs.command = 176;   /* READ_BINARY */
    ioargs.fileid  = 0x6FB7; /* EF_ECC */
    ioargs.path = "3F007F20"; /* GSM directory */
    ioargs.data = NULL;
    ioargs.p3 = 15;          /* length */

    err = sendSimIOCmd(&ioargs, &atresponse, &sr);
    if (!err && sr.sw1 == 0x90 && sr.sw2 == 0x00) {
        storeEccList(sr.simResponse, use_japan_extensions);
    } else {
        ALOGI("[ECC]: No valid ECC numbers on SIM, keeping defaults");
    }
    at_response_free(atresponse);
}

/*
 * Reads the emergency call codes from the EF_ECC file in the SIM
 * card from path "7FFF" using "READ RECORD" commands.
 */
bool read3GECCFile(int use_japan_extensions)
{
    int err = 0;
    ATResponse *atresponse = NULL;
    int numRecords = 0;
    int fileSize = 0;
    int recordSize = 0;
    int i = 1;
    char *ecc_list = NULL;

    RIL_SIM_IO_v6 ioargs;
    RIL_SIM_IO_Response sr;
    memset(&ioargs, 0, sizeof(ioargs));
    memset(&sr, 0, sizeof(sr));
    sr.simResponse = NULL;

    ioargs.command = 192;   /* GET RESPONSE */
    ioargs.fileid  = 0x6FB7; /* EF_ECC */
    ioargs.path = "3F007FFF"; /* USIM directory */
    ioargs.data = NULL;
    ioargs.p3 = 15;          /* length */

    err = sendSimIOCmd(&ioargs, &atresponse, &sr);

    if (err || sr.sw1 != 0x90 || sr.sw2 != 0x00) {
        ALOGW("[ECC]: GET RESPONSE command on 3G EFecc file failed, error "
            "%.2X:%.2X.", sr.sw1, sr.sw2);
        at_response_free(atresponse);
        return false;
    }

    /*
     * Convert response from GET_RESPONSE using convertSimIoFcp() to
     * simplify fetching record size and file size using fixed offsets.
     */
    err = convertSimIoFcp(&sr, &sr.simResponse);
    if (err < 0) {
        ALOGW("[ECC]: Conversion of GET RESPONSE data failed.");
        goto error2;
    }

    /* Convert hex string to int's and calculate number of records */
    recordSize = ((char2nib(sr.simResponse[28]) * 16) +
        char2nib(sr.simResponse[29])) & 0xff;
    fileSize = ((((char2nib(sr.simResponse[4]) * 16) +
        char2nib(sr.simResponse[5])) & 0xff) << 8) +
        (((char2nib(sr.simResponse[6]) * 16) +
        char2nib(sr.simResponse[7])) & 0xff);
    numRecords = fileSize / recordSize;

    ALOGI("[ECC]: Number of records in EFecc file: %d", numRecords);

    if (numRecords > 254) {
        goto error1;
    }

    free(sr.simResponse); /* sr.simResponse needs to be freed */
    at_response_free(atresponse);
    atresponse = NULL;

    /*
     * Allocate memory for a list containing the emergency call codes in
     * raw format. Each emergency call code is coded on three bytes.
     */
    ecc_list = malloc((numRecords * 3 * 2) + 1);
    if(!ecc_list) {
        ALOGE("[ECC]: Failed to allocate memory for SIM fetched ECC's");
        goto error1;
    }
    memset(ecc_list, 0, (numRecords * 3 * 2) + 1);

    /*
     * Loop and fetch all the records using READ RECORD command.
     * Linear fixed EF files uses 1-based counting of records.
     */
    for(i = 1; i <= numRecords; i++) {
        char *p;
        p = (ecc_list + ((i - 1) * 6));
        memset(&ioargs, 0, sizeof(ioargs));
        memset(&sr, 0, sizeof(sr));
        sr.simResponse = NULL;

        ioargs.command = 178;   /* READ_RECORD */
        ioargs.fileid  = 0x6FB7; /* EF_ECC */
        ioargs.path = "3F007FFF";
        ioargs.data = NULL;
        ioargs.p1 = i;          /* record number */
        ioargs.p2 = 4;          /* absolute method */
        ioargs.p3 = recordSize; /* length */

        err = sendSimIOCmd(&ioargs, &atresponse, &sr);
        if (!err && sr.sw1 == 0x90 && sr.sw2 == 0x00)
            memcpy(p, sr.simResponse, 6);
        else
            ALOGW("[ECC]: Can't fetch ECC record from 3G USIM card: error"
                "%.2X:%.2X. Continuing.", sr.sw1, sr.sw2);

        at_response_free(atresponse);
        atresponse = NULL;
    }
    goto finally;

error1:
    free(sr.simResponse); /* sr.simResponse needs to be freed */
error2:
    at_response_free(atresponse);

finally:
    if(ecc_list) {
        storeEccList(ecc_list, use_japan_extensions);
        free(ecc_list);
    } else {
        ALOGI("[ECC]: No valid ECC numbers on SIM, keeping defaults");
    }
    return true;
}

/*
 * Setup r/w ECC list property (ril.ecclist) with values from EF_ECC
 * and predefined values.
 *
 * \param check_attached_network: Check attached network for the MCC code
 *                                (Japan extensions)
 */
void setupECCList(int check_attached_network)
{
    int use_japan_extensions = 0;
    int mcc;

    /* Check for Japan expensions. */
    if (check_attached_network && (0 == getAttachedNetworkIdentity(&mcc, NULL))
        && (mcc == 440 || mcc == 441)) {
        ALOGD("[ECC]: Using Japan extensions: detected by %s network.",
            "attached");
        use_japan_extensions = 1;
    } else if ((0 == getHomeNetworkIdentity(&mcc, NULL)) && (mcc == 440 ||
        mcc == 441)) {
        ALOGD("[ECC]: Using Japan extensions: detected by %s network.", "home");
        use_japan_extensions = 1;
    } else {
        ALOGD("[ECC]: Using world rules.");
    }

    /*
     * Fetch emergency call code list from EF_ECC
     * as described in 3GPP TS 51.011, section 10.3.27.
     */
    if (getUICCType() == UICC_TYPE_SIM) {
        ALOGI("[ECC]: 2G SIM card detected, using read binary method.");
        read2GECCFile(use_japan_extensions);
    }
    /*
     * Fetch emergency call code list from EF_ECC
     * as described in 3GPP TS 31.102, section 4.2.21.
     */
    else {
        ALOGI("[ECC]: 3G USIM card detected, using read record method.");
        if(!read3GECCFile(use_japan_extensions)) {
            /*
             * A SIM card that have the EFecc file stored in the 2G SIM path
             * despite having a UICC application running was found during
             * testing. This is the reasoning for having the below fallback
             * solution.
             */
            ALOGI("[ECC]: ECC file does not exist in USIM directory, "
                "try reading from GSM directory.");
            read2GECCFile(use_japan_extensions);
        }
    }
}

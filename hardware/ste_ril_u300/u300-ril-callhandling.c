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

#include <stdio.h>
#include <string.h>
#include <telephony/ril.h>

#include <assert.h>
#include <errno.h>

#include "atchannel.h"
#include "at_tok.h"
#include "u300-ril.h"
#include "misc.h"

#define LOG_TAG "RILV"
#include <utils/Log.h>

typedef enum {
    ECAV_IDLE = 0,
    ECAV_CALLING = 1,
    ECAV_CONNECTING = 2,
    ECAV_ACTIVE = 3,
    ECAV_HOLD = 4,
    ECAV_WAITING = 5,
    ECAV_ALERTING = 6,
    ECAV_BUSY = 7
} ecav_dialstate_t;

typedef enum {
    CLI_VALIDITY_VALID = 0,
    CLI_VALIDITY_WITHHELD = 1,
    CLI_VALIDITY_NA_INTERWORKING = 2,
    CLI_VALIDITY_NA_PAYPHONE = 3,
    CLI_VALIDITY_NA_OTHER = 4,      /* Not used by Android, map to INTERWORKING (2) */
} cli_validity_t;


typedef enum {
    CNI_VALIDITY_ALLOWED = 0,
    CNI_VALIDITY_RESTRICTED = 1,
    CNI_VALIDITY_NOT_AVAILABLE = 2,
} cni_validity_t;


/* Last call fail cause, obtained by *ECAV. */
static int s_lastCallFailCause = CALL_FAIL_ERROR_UNSPECIFIED;

/* Store remote party name for an incoming Calling Name Indication (+CNAP) */
static RIL_CachedCNAPInfo s_incomingCNAPInfo = {-1, NULL, CNI_VALIDITY_NOT_AVAILABLE};

/* Store incoming UUS info (+CUUS1U) during a Call's SETUP phase.
 * If we need to store it for a longer period, then we probably
 * need an array here.
*/
static RIL_CachedUUSInfo s_incomingUusInfo = {0, {0, 0, 0, NULL}, {0}};

static int clccStateToRILState(int state, RIL_CallState * p_state)
{
    switch (state) {
    case 0:
        *p_state = RIL_CALL_ACTIVE;
        return 0;
    case 1:
        *p_state = RIL_CALL_HOLDING;
        return 0;
    case 2:
        *p_state = RIL_CALL_DIALING;
        return 0;
    case 3:
        *p_state = RIL_CALL_ALERTING;
        return 0;
    case 4:
        *p_state = RIL_CALL_INCOMING;
        return 0;
    case 5:
        *p_state = RIL_CALL_WAITING;
        return 0;
    default:
        return -1;
    }
}

/**
 *  Resets the cached Calling Name Presentation (+CNAP) information
 */
static void resetCNAPInfo(RIL_CachedCNAPInfo *cachedInfo)
{
    if (cachedInfo == NULL)
        return;

    cachedInfo->id = -1;
    free(cachedInfo->name);
    cachedInfo->name = NULL;
    cachedInfo->cni_validity = CNI_VALIDITY_NOT_AVAILABLE; /* Default 2=Not available */
}

/**
 * Binds the cached Calling Name Presentation (+CNAP) information to a certain call,
 * depending on its state.
 */
static void bindCNAPInfo(RIL_CachedCNAPInfo *cachedInfo, int cid, int cstate)
{
    if (cachedInfo == NULL)
        return;

    /* ECAV values are from 0-6 and +CLCC values are from 1 so we add one.
     * See 3GPP TS 22.030, subclause 6.5.5.1 */
    cid++;
    if (cachedInfo->id == cid) {
        /*
         * We are already bound to this callid.
         * Check the state to see if we should discard.
         */
        if (cstate == ECAV_WAITING || cstate == ECAV_ALERTING)
            goto finally;  /* Call is still WAITING or ALERTING */

        /* Call is no longer in setup state. Discard cached information */
        resetCNAPInfo(cachedInfo);
        goto finally;
    }

    if (cachedInfo->id != -1)
        goto finally;  /*Already bound*/

    if (cstate == ECAV_WAITING || cstate == ECAV_ALERTING) {
        cachedInfo->id = cid;
    } else {
        ALOGW("Invalid call state: %d for unbound CNAP", cstate);
        resetCNAPInfo(cachedInfo);
    }

  finally:
    return;
}

/**
 *  Finds the call that the cached Calling Name Presentation (+CNAP) information belongs
 *  to and connects them together. If the +CNAP information is no longer valid, it is discarded.
 */
static void addCNAPInfoToCall(RIL_CachedCNAPInfo *cachedInfo, RIL_Call *calls, int count)
{
    int i;
    RIL_Call *foundCall = NULL;

    for (i=0; i < count; i++) {
        if (cachedInfo->id == calls[i].index) {
            foundCall = &calls[i];
            break;
        }
    }

    if (foundCall == NULL) {
        /* Call not found.  Discard cached information. */
        resetCNAPInfo(cachedInfo);
        goto finally;
    }

    foundCall->name = cachedInfo->name;

    /* If name is not available, and +CLCC has returned more precise information, use it */
    if (CNI_VALIDITY_NOT_AVAILABLE == cachedInfo->cni_validity && 2 < foundCall->numberPresentation)
        foundCall->namePresentation = foundCall->numberPresentation;
    else
        foundCall->namePresentation = cachedInfo->cni_validity;

  finally:
    return;
}


/**
 *  Resets the cached UUSInfo
 *
 */
static void resetUUSInfo(RIL_CachedUUSInfo *cachedInfo)
{
    if (cachedInfo == NULL)
        return;

    cachedInfo->id = -1;
    cachedInfo->uusInfo.uusType = RIL_UUS_TYPE1_IMPLICIT;
    cachedInfo->uusInfo.uusDcs = RIL_UUS_DCS_USP;
    cachedInfo->uusInfo.uusLength = 0;
    cachedInfo->uusInfo.uusData = NULL;

}

/**
 *  Packs a RIL_UUS_Info struct into a hex encoded
 *  User-User information element (UUIE) as defined
 *  in 3GPP TS 24.008 10.5.4.25
 *
 */
static int packUUSIE(RIL_UUS_Info *uusInfo, char **packedData)
{
    int status = 0;
    char *unpacked = NULL;
    /* 3 bytes for Identifier, encoding and length.*/
    int payloadLength = 3 + uusInfo->uusLength;

    if (uusInfo->uusLength > RIL_UUS_DATA_MAX_SIZE) {
        ALOGE("UUS data was too long: %d bytes", uusInfo->uusLength);
        status = -EINVAL;
        goto error;
    }

    unpacked = calloc(1, payloadLength + 1);
    *packedData = calloc(1, 2 * payloadLength + 1);

    if (unpacked == NULL) {
        ALOGE("Memory allocation failed for unpacked");
        status = -ENOMEM;
        goto error;
    }

    if (*packedData == NULL) {
        ALOGE("Memory allocation failed packedData");
        status = -ENOMEM;
        goto error;
    }

    /* Identifier + Length(including Dcs)+Dcs+Data */
    snprintf(unpacked, payloadLength + 1, "%c%c%c%s",
            RIL_UUS_ELEMENT_IDENTIFIER, uusInfo->uusLength+1, uusInfo->uusDcs, uusInfo->uusData);

    status = binaryToString((unsigned char*)unpacked, payloadLength, *packedData);
    if (status < 0)
        goto error;

  finally:
    free(unpacked);
    return status;

  error:
    ALOGE("packUUSIE failed with status = %d",status);
    free(*packedData);
    *packedData = NULL;
    goto finally;
}

/**
 * Unpacks a hex encoded User-User information element (UUIE)
 * into a RIL_UUS_Info struct.
 *
 */
static int unpackUUSIE(const char *data, RIL_UUS_Info *uusInfo, char *uusData)
{
    int status = 0;
    char *unpacked = calloc(1, strlen(data));

    status = stringToBinary(data, strlen(data), (unsigned char*)unpacked);
    if (status < 0) {
        ALOGE("stringToBinary failed");
        goto error;
    }

    /*
     * byte 1 contains length
     * byte 2 contains coding scheme
     * byte 3-n contains uus info
     */
    if (unpacked[1] > RIL_UUS_DATA_MAX_SIZE) {
        ALOGE("Incoming UUS data was too long: %d bytes", unpacked[1]);
        status = -EINVAL;
        goto error;
    }

    if (unpacked[2] > RIL_UUS_DCS_IA5c) {
        ALOGE("Incoming UUS contained unsupported DCS: %d", unpacked[2]);
        status = -EINVAL;
        goto error;
    }

    /* Length without Dcs */
    uusInfo->uusLength = (int)unpacked[1]-1;
    uusInfo->uusDcs = (RIL_UUS_DCS)unpacked[2];
    strncpy(uusData, (char *)&unpacked[3], uusInfo->uusLength);
    uusInfo->uusData = uusData;
  finally:
    free(unpacked);
    return status;

  error:
    ALOGE("unpackUUSIE failed with status = %d",status);
    goto finally;
}

/**
 * Binds the cached UUSInfo to a certain call,
 * Depending on its state.
 *
 */
static void bindUUSInfo(RIL_CachedUUSInfo *cachedInfo, int cid, int cstate)
{
    /* ECAV values are from 0-6 and +CLCC values are from 1 so we add one
    * See 3GPP TS 22.030, subclause 6.5.5.1 */
    cid++;
    if (cachedInfo->id == cid) {
        /*
         * We are already bound to this callid.
         * Check the state to see if we should discard.
         */
        if (cstate == ECAV_WAITING || cstate == ECAV_ALERTING)
            goto finally;  /* Call is still WAITING or ALERTING */

        /* Call is no longer in setup state. Discard cached information */
        resetUUSInfo(cachedInfo);
        goto finally;
    }

    if (cachedInfo->id != -1)
        goto finally;  /*Already bound*/

    if (cstate == ECAV_WAITING || cstate == ECAV_ALERTING) {
        cachedInfo->id = cid;
    } else {
        ALOGW("Invalid call state: %d for unbound UUS", cstate);
        resetUUSInfo(cachedInfo);
    }
  finally:
    return;

}


/**
 *  Finds the call that the cached UUSInfo belongs to and connects
 *  them together.  If the UUSInfo is no longer valid, it is discarded
 *
 */
static void addUUSInfoToCall(RIL_CachedUUSInfo *cachedInfo, RIL_Call *calls, int count)
{
    int i;
    RIL_Call *foundCall = NULL;

    for (i=0; i < count; i++) {
        if (cachedInfo->id == calls[i].index) {
            foundCall = &calls[i];
            break;
        }
    }

    if (foundCall == NULL) {
        /* Call not found.  Discard UUS. */
        resetUUSInfo(cachedInfo);
        goto finally;
    }
    foundCall->uusInfo = &cachedInfo->uusInfo;

  finally:
    return;
}


/**
 * Note: Directly modified line and has *p_call point directly into
 * modified line.
 */
static int callFromCLCCLine(char *line, RIL_Call * p_call)
{
    /* +CLCC: 1,0,2,0,0,\"+18005551212\",145,,,1
       index,isMT,state,mode,isMpty(,number,TOA),,,CLI_validity */
    int err;
    int state;
    int mode;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &(p_call->index));
    if (err < 0)
        goto error;

    err = at_tok_nextbool(&line, &(p_call->isMT));
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &state);
    if (err < 0)
        goto error;

    err = clccStateToRILState(state, &(p_call->state));
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &mode);
    if (err < 0)
        goto error;

    p_call->isVoice = (mode == 0);

    err = at_tok_nextbool(&line, &(p_call->isMpty));
    if (err < 0)
        goto error;

    if (at_tok_hasmore(&line)) {
        err = at_tok_nextstr(&line, &(p_call->number));

        /* Tolerate null here */
        if (err >= 0) {
            err = at_tok_nextint(&line, &p_call->toa);
            if (err < 0)
                goto error;
        }
    }

    if (at_tok_hasmore(&line)) {
        int dummy;

        /* alpha and priority not supported and thus empty so ignore them */
        (void) at_tok_nextint(&line, &dummy);
        (void) at_tok_nextint(&line, &dummy);

        err = at_tok_nextint(&line, &(p_call->numberPresentation));
        if (err < 0)
            goto error;

        /* Android does not grok NA codes beyond payphone (3) */
        if (CLI_VALIDITY_NA_PAYPHONE < p_call->numberPresentation)
            p_call->numberPresentation = CLI_VALIDITY_NA_INTERWORKING;
    }

    p_call->namePresentation = CNI_VALIDITY_NOT_AVAILABLE;

    return 0;

  error:
    ALOGE("invalid CLCC line\n");
    return -1;
}

/**
 * AT*ECAV handler function.
 */
void onECAVReceived(const char *s)
{
    char *line;
    char *tok;
    int err;
    int res;
    int cid;

    tok = line = strdup(s);

    err = at_tok_start(&tok);
    if (err < 0)
        goto error;

    /* This is CID */
    err = at_tok_nextint(&tok, &cid);
    if (err < 0)
        goto error;

    /* Read ccstate. */
    err = at_tok_nextint(&tok, &res);
    if (err < 0)
        goto error;

    /* Associate incoming CNAP  data with the call if applicable */
    bindCNAPInfo(&s_incomingCNAPInfo, cid, res);

    /* Associate incoming UUS data with the call if applicable */
    bindUUSInfo(&s_incomingUusInfo, cid, res);

    /* If alerting(6) or waiting(5) send CALL_RING */
    if( res == ECAV_WAITING || res == ECAV_ALERTING) {
            RIL_onUnsolicitedResponse(RIL_UNSOL_CALL_RING,
                                      NULL, 0);
    }
#ifdef USE_U8500_RIL
    /* We need to save last call failcause for all cases where it exists. */
#else
    /* If 0 (IDLE), we want to know why. */
    if (res == ECAV_IDLE) {
#endif
        /* Read call type and process id, we don't care about them though. */
        err = at_tok_nextint(&tok, &res);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&tok, &res);
        if (err < 0)
#ifdef USE_U8500_RIL
            /* If process id doesn't exist */
#else
            goto error;
#endif

        /* Now, read exit cause and save it for later. */
        err = at_tok_nextint(&tok, &s_lastCallFailCause);

#ifdef USE_U8500_RIL
#else
    }
#endif

  finally:
    free(line);

    /* Send the response even if we failed.. */
    RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL,
                              0);

    return;

  error:
    ALOGE("ECAV: Failed to parse %s.", s);
    goto finally;
}

/**
 * *EACE: Ringback tone received
 */
void onAudioCallEventNotify(const char *s)
{
    char *line;
    char *tok;
    int err;
    int res;

    tok = line = strdup(s);

    err = at_tok_start(&tok);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&tok, &res);
    if (err < 0)
        goto error;

  finally:
    free(line);
    /* Send the response even if we failed.. */
    RIL_onUnsolicitedResponse(RIL_UNSOL_RINGBACK_TONE, &res,
                              sizeof(int *));
    return;

  error:
    ALOGE("EACE: Failed to parse %s.", s);
    goto finally;
}

/**
 * +CNAP (Calling Name Presentation) handler function
 */
void onCallingNAmePresentationReceived(const char *s)
{
    char *line;
    char *tok;
    int err;
    int res;
    char *data;

    tok = line = strdup(s);

    resetCNAPInfo(&s_incomingCNAPInfo);

    err = at_tok_start(&tok);
    if (err < 0)
        goto error;

    err = at_tok_nextstr(&tok, &data);
    if (err < 0)
        goto error;

    /* An empty string ("") indicates the name is not available */
    if (0 == strlen(data)) {
        s_incomingCNAPInfo.cni_validity = CNI_VALIDITY_NOT_AVAILABLE;
    } else {
        /* Cache the name, set CNI validity */
        s_incomingCNAPInfo.name = strdup(data);
        s_incomingCNAPInfo.cni_validity = CNI_VALIDITY_ALLOWED;
    }

    if (at_tok_hasmore(&tok)) {
        /* Optional 'CNI validity' */
        err = at_tok_nextint(&tok, &res);
        if (err >= 0) {
            s_incomingCNAPInfo.cni_validity = res;
        }
    }

  finally:
    free(line);
    return;

  error:
    ALOGE("CNAP: Failed to parse \"%s\".", s);
    goto finally;
}

/**
 * +CUUS1U (User to User signalling) handler function
 */
void onUUS1Received(const char *s)
{
    char *line;
    char *tok;
    int err;
    int res;
    char *data;

    tok = line = strdup(s);

    err = at_tok_start(&tok);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&tok, &res);
    if (err < 0)
        goto error;

    if (res != 1) {  /* 1 is SETUP message type*/
        ALOGE("CUUS1U: Unsupported message type received: %d.", res);
        goto error;
    }

    err = at_tok_nextstr(&tok, &data);
    if (err < 0)
        goto error;

    resetUUSInfo(&s_incomingUusInfo);

    err = unpackUUSIE(data, &s_incomingUusInfo.uusInfo, (char *)s_incomingUusInfo.data);
    if (err < 0)
        goto error;

  finally:
    free(line);
    return;

  error:
    ALOGE("CUUS1U: Failed to parse %s.", s);
    goto finally;
}

/**
 * RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND
 *
 * Hang up waiting or held (like AT+CHLD=0)
 */
void requestHangupWaitingOrBackground(void *data, size_t datalen,
                                      RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    /* 3GPP 22.030 6.5.5
       "Releases all held calls or sets User Determined User Busy
       (UDUB) for a waiting call." */
    err = at_send_command("AT+CHLD=0", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND
 *
 * Hang up waiting or held (like AT+CHLD=1)
 */
void requestHangupForegroundResumeBackground(void *data, size_t datalen,
                                             RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    /* 3GPP 22.030 6.5.5
       "Releases all active calls (if any exist) and accepts
       the other (held or waiting) call." */
    err = at_send_command("AT+CHLD=1", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    return;
  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE
 *
 * Switch waiting or holding call and active call (like AT+CHLD=2)
 */
void requestSwitchWaitingOrHoldingAndActive(void *data, size_t datalen,
                                            RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    /* 3GPP 22.030 6.5.5
       "Places all active calls (if any exist) on hold and accepts
       the other (held or waiting) call." */
    err = at_send_command("AT+CHLD=2", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_CONFERENCE
 *
 * Conference holding and active (like AT+CHLD=3)
 */
void requestConference(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    /* 3GPP 22.030 6.5.5
       "Adds a held call to the conversation." */
    err = at_send_command("AT+CHLD=3", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SEPARATE_CONNECTION
 *
 * Separate a party from a multiparty call placing the multiparty call
 * (less the specified party) on hold and leaving the specified party
 * as the only other member of the current (active) call
 *
 * Like AT+CHLD=2x
 *
 * See TS 22.084 1.3.8.2 (iii)
 * TS 22.030 6.5.5 "Entering "2X followed by send"
 * TS 27.007 "AT+CHLD=2x"
 */
void requestSeparateConnection(void *data, size_t datalen, RIL_Token t)
{
    char cmd[12];
    int party = ((int *) data)[0];
    int err;
    ATResponse *atresponse = NULL;

    /* Make sure that party is a single digit. */
    if (party < 1 || party > 9)
        goto error;

    sprintf(cmd, "AT+CHLD=2%d", party);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_EXPLICIT_CALL_TRANSFER
 *
 * Connects the two calls and disconnects the subscriber from both calls.
 */
void requestExplicitCallTransfer(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    /* 3GPP TS 22.091
       Connects the two calls and disconnects the subscriber from both calls. */
    err = at_send_command("AT+CHLD=4", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_UDUB
 *
 * Send UDUB (user determined used busy) to ringing or
 * waiting call answer (RIL_BasicRequest r).
 */
void requestUDUB(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command("ATH", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_MUTE
 *
 * Turn on or off uplink (microphone) mute.
 *
 * Will only be sent while voice call is active.
 * Will always be reset to "disable mute" when a new voice call is initiated.
 */
void requestSetMute(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int mute = ((int *) data)[0];
    int err = 0;
    char *cmd = NULL;

    assert(mute == 0 || mute == 1);

    asprintf(&cmd, "AT+CMUT=%d", mute);
    at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_GET_MUTE
 *
 * Queries the current state of the uplink mute setting.
 */
void requestGetMute(void *data, size_t datalen, RIL_Token t)
{
    char *line = NULL;
    int err = 0;
    int response = 0;
    ATResponse *atresponse = NULL;

    err = at_send_command_singleline("AT+CMUT?", "+CMUT:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &response);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_LAST_CALL_FAIL_CAUSE
 *
 * Requests the failure cause code for the most recently terminated call.
 *
 * See also: RIL_REQUEST_LAST_PDP_FAIL_CAUSE
 */
void requestLastCallFailCause(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_lastCallFailCause,
                          sizeof(int));
}

/**
 * RIL_REQUEST_GET_CURRENT_CALLS
 *
 * Requests current call list.
 */
void requestGetCurrentCalls(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse;
    ATLine *cursor;
    int countCalls;
    int countValidCalls;
    RIL_Call *calls;
    RIL_Call **response;
    int i;

    err = at_send_command_multiline("AT+CLCC", "+CLCC:", &atresponse);

    if (err != 0 || atresponse->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    /* Count the calls. */
    for (countCalls = 0, cursor = atresponse->p_intermediates;
         cursor != NULL; cursor = cursor->p_next)
        countCalls++;

    /* Yes, there's an array of pointers and then an array of structures. */
    response = (RIL_Call **) alloca(countCalls * sizeof(RIL_Call *));
    calls = (RIL_Call *) alloca(countCalls * sizeof(RIL_Call));
    memset(calls, 0, countCalls * sizeof(RIL_Call));

    /* Init the pointer array. */
    for (i = 0; i < countCalls; i++)
        response[i] = &(calls[i]);

    for (countValidCalls = 0, cursor = atresponse->p_intermediates;
         cursor != NULL; cursor = cursor->p_next) {
        err = callFromCLCCLine(cursor->line, calls + countValidCalls);

        if (err != 0)
            continue;

        countValidCalls++;
    }

    addCNAPInfoToCall(&s_incomingCNAPInfo, calls, countValidCalls);

    addUUSInfoToCall(&s_incomingUusInfo, calls, countValidCalls);

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response,
                          countValidCalls * sizeof(RIL_Call *));

    at_response_free(atresponse);
    return;
}

/**
 * RIL_REQUEST_DIAL
 *
 * Initiate voice call.
 */
void requestDial(void *data, size_t datalen, RIL_Token t)
{
    RIL_Dial *dial;
    ATResponse *atresponse = NULL;
    char *cmd;
    const char *clir;
    char *uuscmd = NULL;
    char *uusie = NULL;
    int err;
    int cme_err;

    dial = (RIL_Dial *) data;

    switch (dial->clir) {
    case 1:
        clir = "I";
        break;                  /* Invocation */
    case 2:
        clir = "i";
        break;                  /* Suppression */
    default:
    case 0:
        clir = "";
        break;                  /* Subscription default */
    }

    if (dial->uusInfo != NULL) { /* User-to-User info present */
        switch(dial->uusInfo->uusType) {
        case RIL_UUS_TYPE1_IMPLICIT: /* Only Type 1 implicit is supported*/

            packUUSIE(dial->uusInfo, &uusie);

            /* +CUUS1=
             *  0 = disable events for MO calls
             *  1 = enable events for MT calls
             *  1 = SETUP message type
             *  uusie = The user to user information element.
             */
            asprintf(&uuscmd, "AT+CUUS1=0,1,1,\"%s\"", uusie);

            free(uusie);

            err = at_send_command(uuscmd, &atresponse);
            if (err < 0 || atresponse->success == 0)
                goto error;

            at_response_free(atresponse);
            break;
        default:
            ALOGW("Dial request contained unsupported UUS info of type: %d", dial->uusInfo->uusType);
        }
    }

    asprintf(&cmd, "ATD%s%s;", dial->address, clir);

    err = at_send_command(cmd, &atresponse);

    free(cmd);

    cme_err = at_get_cme_error(atresponse);

    at_response_free(atresponse);
    atresponse = NULL;

    err = at_send_command("AT+CUUS1=0,1,1", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    if (CME_FDN_BLOCKED == cme_err) {
        /* This error code would be read by requestLastCallFailCause
         * when upper layer finds out that we have no call after GET_CURRENT_CALLS.
         */
        s_lastCallFailCause = CALL_FAIL_FDN_BLOCKED;
    }

    /* Success or failure is ignored by the upper layer here,
       it will call GET_CURRENT_CALLS and determine success that way. */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    free(uuscmd);
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_ANSWER
 *
 * Answer incoming call.
 *
 * Will not be called for WAITING calls.
 * RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE will be used in this case
 * instead.
 */
void requestAnswer(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command("ATA", &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    /* Success or failure is ignored by the upper layer here,
       it will call GET_CURRENT_CALLS and determine success that way. */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_HANGUP
 *
 * Hang up a specific line (like AT+CHLD=1x).
 */
void requestHangup(void *data, size_t datalen, RIL_Token t)
{
    int cid;
    char *cmd = NULL;
    ATLine *cursor;
    int err;
    int i;
    int countCalls;
    ATResponse *atresponse = NULL;
    RIL_Call *calls;

    cid = ((int *) data)[0];

    /*
     * Until we get some silver bullet AT-command that will kill whatever
     * call we have, we need to check what state we're in and act accordingly.
     *
     * TODO: Refactor this and merge with the other query to CLCC.
     */
    err = at_send_command_multiline("AT+CLCC", "+CLCC:", &atresponse);

    if (err != 0 || atresponse->success == 0) {
        goto error;
    }

    /* the UUSInfo is not valid anymore */
    if (s_incomingUusInfo.id == cid)
        resetUUSInfo(&s_incomingUusInfo);

    /* Count the calls. */
    for (countCalls = 0, cursor = atresponse->p_intermediates;
         cursor != NULL; cursor = cursor->p_next)
        countCalls++;

    if (countCalls <= 0)
        goto error;

    calls = (RIL_Call *) alloca(countCalls * sizeof(RIL_Call));
    memset(calls, 0, countCalls * sizeof(RIL_Call));

    for (i = 0, cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next) {
        err = callFromCLCCLine(cursor->line, calls + i);

        if (err != 0)
            continue;

        if (calls[i].index == cid)
            break;

        i++;
    }

    at_response_free(atresponse);
    atresponse = NULL;

    /* We didn't find the call. Just drop the request and let android decide. */
    if (calls[i].index != cid)
        goto error;

    if (calls[i].state == RIL_CALL_DIALING ||
        calls[i].state == RIL_CALL_ALERTING)
        asprintf(&cmd, "ATH");
    else
        /* 3GPP 22.030 6.5.5
           "Releases a specific active call X" */
        asprintf(&cmd, "AT+CHLD=1%d", cid);

    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    if (cmd)
        free(cmd);
    at_response_free(atresponse);
    return;

  error:
    /* Success or failure is ignored by the upper layer here,
       it will call GET_CURRENT_CALLS and determine success that way. */
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_DTMF
 *
 * Send a DTMF tone
 *
 * If the implementation is currently playing a tone requested via
 * RIL_REQUEST_DTMF_START, that tone should be cancelled and the new tone
 * should be played instead.
 */
void requestDTMF(void *data, size_t datalen, RIL_Token t)
{
    char c = *((char *) data);
    char *cmd = NULL;
    ATResponse *atresponse = NULL;
    int err = 0;

    /* Set duration to default (manufacturer specific, 70ms in our case). */
    err = at_send_command("AT+VTD=0", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    at_response_free(atresponse);
    atresponse = NULL;
    asprintf(&cmd, "AT+VTS=%c", c);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    if (cmd != NULL)
        free(cmd);
    at_response_free(atresponse);

    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_DTMF_START
 *
 * Start playing a DTMF tone. Continue playing DTMF tone until
 * RIL_REQUEST_DTMF_STOP is received .
 *
 * If a RIL_REQUEST_DTMF_START is received while a tone is currently playing,
 * it should cancel the previous tone and play the new one.
 *
 * See also: RIL_REQUEST_DTMF, RIL_REQUEST_DTMF_STOP.
 */
void requestDTMFStart(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    char c = *((char *) data);
    char *cmd = NULL;
    int err = 0;

    /* Set duration to maximum, 10000000  n/10 ms = 10000s. */
    err = at_send_command("AT+VTD=10000000", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    at_response_free(atresponse);
    atresponse = NULL;

    /* Start the DTMF tone. */
    asprintf(&cmd, "AT+VTS=%c", c);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    if (cmd != NULL)
        free(cmd);

    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_DTMF_STOP
 *
 * Stop playing a currently playing DTMF tone.
 *
 * See also: RIL_REQUEST_DTMF, RIL_REQUEST_DTMF_START.
 */
void requestDTMFStop(void *data, size_t datalen, RIL_Token t)
{
    int err = 0;
    ATResponse *atresponse = NULL;

    err = at_send_command("AT+VTD=0", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}


/**
 * Local comfort tone generation indication
 */
void onLocalComfortToneGenerationIndication(const char *s)
{
    char *line = NULL, *tok = NULL;
    int err = 0, lat = 0;
    int i = 0 ;

    /* get lat (assuming format: *ELAT: <int>) */
    tok = line = strdup(s);
    if (at_tok_start(&tok) < 0) {
        ALOGE("at_tok_start failed!");
        goto error;
    }

    err = at_tok_nextint(&tok, &lat);
    if (err < 0) {
        goto error;
    }

    RIL_onUnsolicitedResponse(RIL_UNSOL_RINGBACK_TONE,
                                  &lat, sizeof(int *));

    free(line);
    return;

error:
    ALOGE("%s: Failed to parse unsolicited command %s\n", __FUNCTION__, s);
    free(line);
}



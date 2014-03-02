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
#include <stdbool.h>
#include <telephony/ril.h>
#include <assert.h>
#include "atchannel.h"
#include "at_tok.h"
#include "misc.h"
#include "u300-ril.h"
#include "u300-ril-sim.h"

#define LOG_TAG "RILV"
#include <utils/Log.h>

#define REPOLL_OPERATOR_SELECTED 30 /* 30 * 2 = 1M = ok? */

#define MAX_NUM_CELLS 8         /* Maximum number of neighborhood cells */

#define NBR_OF_SIMULTANEOUS_PS_CALLS 3 /* Maximum number of PS calls supported */

static const struct timeval TIMEVAL_OPERATOR_SELECT_POLL = { 2, 0 };

static void pollOperatorSelected(void *params);

struct operatorPollParams {
    RIL_Token t;
    int loopcount;
};

typedef enum {
    NETWORK_DOMAIN_UNKNOWN,
    NETWORK_DOMAIN_CS,
    NETWORK_DOMAIN_PS
} network_domain_t;

static int registration_status_cs = 0;
static int registration_status_ps = 0;
static int network_reject_cause   = 0;
static int network_reject_cause_domain = NETWORK_DOMAIN_UNKNOWN;

static bool ok_to_display_operator_name()
{
    bool result = true;

    /*
     * There are requirements that the operator name cannot be displayed
     * in case of unsuccessful registration on the PS domain. It is assumed
     * that the operator name on screen indicates that the services towards
     * the network are available to the user. Also, it should be possible
     * to register against the network with CS only mode. With that in mind
     * the table below has been devised:
     *
     *********************************************************************
     *  CS  PS  Action                                                   *
     *********************************************************************
     *  0   0   do not display operator                                  *
     *  0   1   do not display operator                                  *
     *  1   0   display operator only if no reject cause is given for PS *
     *  1   1   display operator                                         *
     *********************************************************************/

    ALOGI("%s: CS=%d PS=%d domain=%d", __FUNCTION__, registration_status_cs,
         registration_status_ps, network_reject_cause_domain);

    if (!registration_status_cs && !registration_status_ps) {
        result = false;
    } else if (!registration_status_cs && registration_status_ps) {
        result = false;
    } else if ((registration_status_cs && !registration_status_ps) &&
                (NETWORK_DOMAIN_PS == network_reject_cause_domain)) {
        result = false;
    } else if ((registration_status_cs && !registration_status_ps) &&
                (NETWORK_DOMAIN_PS != network_reject_cause_domain)) {
        result = true;
    } else { /* (registration_status_cs && registration_status_ps) */
        result = true;
    }

    ALOGI("%s: result=%d", __FUNCTION__, result);

    return result;
}

/**
 * Poll +COPS? and return a success, or if the loop counter reaches
 * REPOLL_OPERATOR_SELECTED, return generic failure.
 */
static void pollOperatorSelected(void *params)
{
    int err = 0;
    int response = 0;
    char *line = NULL;
    ATResponse *atresponse = NULL;
    struct operatorPollParams *poll_params;
    RIL_Token t;

    assert(params != NULL);

    poll_params = (struct operatorPollParams *) params;
    t = poll_params->t;

    if (poll_params->loopcount >= REPOLL_OPERATOR_SELECTED)
        goto error;

    err = at_send_command_singleline("AT+COPS?", "+COPS:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &response);
    if (err < 0)
        goto error;

    /* If we don't get more than the COPS: {0-4} we are not registered.
       Loop and try again. */
    if (!at_tok_hasmore(&line)) {
        poll_params->loopcount++;
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, pollOperatorSelected,
                        poll_params, &TIMEVAL_OPERATOR_SELECT_POLL);
    } else {
        /* We got operator, throw a success! */
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
        goto finished;
    }

  finally:
    at_response_free(atresponse);
    return;

  finished:
    free(poll_params);
    goto finally;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finished;
}

/**
 * GSM Network Neighbourhood Cell IDs
 */
static void GSMNeighbouringCellIDs(void *data, size_t datalen, RIL_Token t)
{
    int err = 0;
    char *p = NULL;
    int n = 0;
    ATLine *tmp = NULL;
    ATResponse *atresponse = NULL;
    RIL_NeighboringCell *ptr_cells[MAX_NUM_CELLS];

    err = at_send_command_multiline("AT*EGNCI", "*EGNCI:", &atresponse);
    if (err < 0 ||
        atresponse->success == 0 || atresponse->p_intermediates == NULL)
        goto error;

    tmp = atresponse->p_intermediates;
    while (tmp) {
        if (n > MAX_NUM_CELLS)
            goto error;
        p = tmp->line;
        if (*p == '*') {
            char *line = p;
            char *plmn = NULL;
            char *lac = NULL;
            char *cid = NULL;
            int arfcn = 0;
            int bsic = 0;
            int rxlvl = 0;
            int ilac = 0;
            int icid = 0;

            err = at_tok_start(&line);
            if (err < 0)
                goto error;

            /* PLMN */
            err = at_tok_nextstr(&line, &plmn);
            if (err < 0) {
                ALOGE("%s: PLMN missing!\n", __func__);
                goto error;
            }

            /* LAC */
            err = at_tok_nextstr(&line, &lac);
            if (err < 0) {
                ALOGE("%s: LAC missing!\n", __func__);
                goto error;
            }

            /* CellID */
            err = at_tok_nextstr(&line, &cid);
            if (err < 0) {
                ALOGE("%s: CellID missing!\n", __func__);
                goto error;
            }

            /* ARFCN */
            err = at_tok_nextint(&line, &arfcn);
            if (err < 0) {
                /* Accept even if no  value is provided */
            }

            /* BSIC */
            err = at_tok_nextint(&line, &bsic);
            if (err < 0) {
                /* Accept even if no  value is provided */
            }

            /* RxLevel */
            err = at_tok_nextint(&line, &rxlvl);
            if (err < 0) {
                ALOGE("%s: RxLevel missing!\n", __func__);
                goto error;
            }

            /* process data for each cell */
            ptr_cells[n] = alloca(sizeof(RIL_NeighboringCell));
            ptr_cells[n]->rssi = rxlvl;
            ptr_cells[n]->cid = alloca(9 * sizeof(char));
            sscanf(lac,"%x",&ilac);
            sscanf(cid,"%x",&icid);
            sprintf(ptr_cells[n]->cid, "%08x", ((ilac << 16) + icid));
            n++;
        }
        tmp = tmp->p_next;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, ptr_cells,
                          n*sizeof(RIL_NeighboringCell*));

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * WCDMA Network Neighbourhood Cell IDs
 */
static void WCDMANeighbouringCellIDs(void *data, size_t datalen,
                                     RIL_Token t)
{
    int err = 0;
    char *p = NULL;
    int n = 0;
    ATLine *tmp = NULL;
    ATResponse *atresponse = NULL;
    RIL_NeighboringCell *ptr_cells[MAX_NUM_CELLS];

    err = at_send_command_multiline("AT*EWNCI", "*EWNCI:", &atresponse);
    if (err < 0 ||
        atresponse->success == 0 || atresponse->p_intermediates == NULL)
        goto error;

    tmp = atresponse->p_intermediates;
    while (tmp) {
        if (n > MAX_NUM_CELLS)
            goto error;
        p = tmp->line;
        if (*p == '*') {
            char *line = p;
            int uarfcn = 0;
            int psc = 0;
            int rscp = 0;
            int ecno = 0;
            int pathloss = 0;

            err = at_tok_start(&line);
            if (err < 0)
                goto error;

            /* UARFCN */
            err = at_tok_nextint(&line, &uarfcn);
            if (err < 0) {
                /* Accept even if no  value is provided */
            }

            /* PSC */
            err = at_tok_nextint(&line, &psc);
            if (err < 0) {
                ALOGE("%s: PSC missing!\n", __func__);
                goto error;
            }

            /* Optional: RSCP */
            if (at_tok_hasmore(&line)) {
                err = at_tok_nextint(&line, &rscp);
                if (err < 0) {
                    /* Accept even if no  value is provided */
                }
            }

            /* Optional: ECNO */
            if (at_tok_hasmore(&line)) {
                err = at_tok_nextint(&line, &ecno);
                if (err < 0) {
                    /* Accept even if no  value is provided */
                }
            }

            /* Optional: PathLoss */
            if (at_tok_hasmore(&line)) {
                err = at_tok_nextint(&line, &pathloss);
                if (err < 0) {
                    /* Accept even if no  value is provided */
                }
            }

            /* process data for each cell */
            ptr_cells[n] = alloca(sizeof(RIL_NeighboringCell));
            ptr_cells[n]->rssi = rscp;
            ptr_cells[n]->cid = alloca(9 * sizeof(char));
            sprintf(ptr_cells[n]->cid, "%08x", psc);
            n++;
        }
        tmp = tmp->p_next;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, ptr_cells,
                          n*sizeof(RIL_NeighboringCell*));

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * Get network identity (MCC/MNC) of the Home network.
 *
 * \param mcc: [out] MCC value.
 * \param mnc: [out] MNC value.
 * \return Negative value indicates failure.
 */
 int getHomeNetworkIdentity(int *mcc, int *mnc)
{
    char *tok, *mcc_mnc, *end;
    ATResponse *atresponse = NULL;
    int status = 0;

    status = at_send_command_singleline("AT*EHNET=2", "*EHNET", &atresponse);
    if (status < 0)
        goto out;
    if (atresponse->success == 0) {
        status = -1;
        goto out;
    }
    tok = atresponse->p_intermediates->line;

    status = at_tok_start(&tok);
    if (status < 0)
        goto out;

    status = at_tok_nextstr(&tok, &mcc_mnc);
    if (status < 0)
        goto out;

    /* MCC+MNC is either 5 (3+2) or 6 (3+3) */
    if (strlen(mcc_mnc) < 5) {
        status = -1;
        goto out;
    }

    if (mnc) {
        *mnc = strtol(&mcc_mnc[3], &end, 10);
        if (*end != '\0') {
            status =  -1;
            goto out;
        }
    }

    mcc_mnc[3] = '\0';
    if (mcc) {
        *mcc = strtol(&mcc_mnc[0], &end, 10);
        if (*end != '\0') {
            status =  -1;
            goto out;
        }
    }

out:
    at_response_free(atresponse);
    return status;
}

 /**
  * Get network identity (MCC/MNC) of the Attached network.
  *
  * \param mcc: [out] MCC value.
  * \param mnc: [out] MNC value.
  * \return Negative value indicates failure.
  */
 int getAttachedNetworkIdentity(int *mcc, int *mnc)
 {
     char *tok, *mcc_mnc, *end;
     ATResponse *atresponse = NULL;
     int status = 0;

     status = at_send_command_singleline("AT+COPS=3,2;+COPS?", "+COPS:",
                                         &atresponse);
     if (status < 0)
         goto out;
     if (atresponse->success == 0) {
         status = -1;
         goto out;
     }
     tok = atresponse->p_intermediates->line;

     status = at_tok_start(&tok);
     if (status < 0)
         goto out;

     status = at_tok_nextstr(&tok, &mcc_mnc);
     if (status < 0)
         goto out;

     /* MCC+MNC is either 5 (3+2) or 6 (3+3) */
     if (strlen(mcc_mnc) < 5) {
         status = -1;
         goto out;
     }

     if (mnc) {
         *mnc = strtol(&mcc_mnc[3], &end, 10);
         if (*end != '\0') {
             status =  -1;
             goto out;
         }
     }

     mcc_mnc[3] = '\0';
     if (mcc) {
         *mcc = strtol(&mcc_mnc[0], &end, 10);
         if (*end != '\0') {
             status =  -1;
             goto out;
         }
     }

 out:
     at_response_free(atresponse);
     return status;
 }

 /**
  * setupECCListAsyncAdapter: async adapter for enqueueRILEvent()
  */
 static void setupECCListAsyncAdapter(void *param)
 {
     setupECCList(1);
 }

 /**
  * RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED
  *
  * Called when modem has sent one of registration status update unsolicited
  * results. It can be one of:
  *  *EREG:
  *  +CREG:
  *  +CGREG:
  *  *EPSB:
  *  *ENNIR
  */
 void onNetworkStateChanged(const char *s)
 {
     char buf[16], *tok = buf;
     int status = -1;

     strncpy(buf, s, sizeof(buf) - 1);
     buf[sizeof(buf) - 1] = 0;

     (void)at_tok_start(&tok) ;
     (void)at_tok_nextint(&tok, &status);

     /* If roaming to Japan a few extra emergency numbers are required. */
     if (strStartsWith(s, "+CREG:") || strStartsWith(s, "*EREG:")) {
         if (status == 1 || status == 5) /* Registered, home or roaming */
             /* Check for Japan extensions and update ECC list */
             enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL,
                             setupECCListAsyncAdapter, NULL, NULL);

         /* update CS registration status */
         registration_status_cs = (1 == status || 5 == status) ? 1 : 0;
     } else if (strStartsWith(s, "+CGREG:")) {
         /* update PS registration status */
         registration_status_ps = (1 == status || 5 == status) ? 1 : 0;
     }

     /* Always send network state change event */
     RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED,
                               NULL, 0);
 }

/**
 * RIL_UNSOL_NITZ_TIME_RECEIVED
 *
 * Called when radio has received a NITZ time message.
 *
 * IMPLEMENTATION NOTE:
 * Assumes that AT*ETZR=3 is used.
 */
void onNetworkTimeReceived(const char *s)
{
    char *line, *tok, *response, *tz, *time, *timestamp, *dst;
    int err = 0;

    tok = line = strdup(s);
    if (at_tok_start(&tok) < 0) {
        goto error;
    }

    if (at_tok_nextstr(&tok, &tz) != 0) {
        goto error;
    }
    else if (at_tok_nextstr(&tok, &time) != 0) {
        goto error;
    }
    else if (at_tok_nextstr(&tok, &timestamp) != 0) { /* not used by the RIL */
        goto error;
    }
    else if (at_tok_nextstr(&tok, &dst) != 0) {
        goto error;
    }
    else {
        asprintf(&response, "%s%s,%s", time + 2, tz, dst);

        RIL_onUnsolicitedResponse(RIL_UNSOL_NITZ_TIME_RECEIVED,
                                  response, sizeof(char *));

        free(response);
    }

    free(line);
    return;

error:
    ALOGE("%s: Failed to parse unsolicited command %s\n", __FUNCTION__, s);
    free(line);
}


/**
 * Called when a network reject cause is received.
 */
void onNetworkRejectCause(const char *s)
{
    char *line = NULL, *tok = NULL;
    int err = 0, cause = 0;

    /* get cause code and translate it to the Android equivalent (assuming format: *ECME: <int>,<string>) */
    tok = line = strdup(s);

    if (at_tok_start(&tok) < 0) {
        ALOGE("at_tok_start failed!");
        goto error;
    }

    err = at_tok_nextint(&tok, &cause);

    if (err < 0) {
        goto error;
    }

    cause -= 100; /* {102, 103, ...} will be mapped to {2, 3, ...} */

    ALOGI("%s: caching reject cause %d for RIL_REQUEST_VOICE_REGISTRATION_STATE\n", __FUNCTION__, cause);
    network_reject_cause = cause; /* update local cache */

    /* determine which domain that is affected */
    if (registration_status_cs && !registration_status_ps) {
        ALOGI("%s, affected domain: PS", __FUNCTION__);
        network_reject_cause_domain = NETWORK_DOMAIN_PS;
    } else if (!registration_status_cs && registration_status_ps) {
        ALOGI("%s, affected domain: CS", __FUNCTION__);
        network_reject_cause_domain = NETWORK_DOMAIN_CS;
    } else { /* both CS and PS are detached, not enough information to determine domain */
        ALOGI("%s, affected domain: unknown", __FUNCTION__);
        network_reject_cause_domain = NETWORK_DOMAIN_UNKNOWN;
    }

    free(line);
    return;

error:
    ALOGE("%s: Failed to parse unsolicited command %s\n", __FUNCTION__, s);
    free(line);
}


/**
 * RIL_UNSOL_SIGNAL_STRENGTH
 *
 * Radio may report signal strength rather than have it polled.
 *
 * "data" is a const RIL_SignalStrength_v6 *
 */
void unsolSignalStrength(const char *s)
{
    char *line = NULL;
    int err;
    int skip;
    int signalQuality;
    RIL_SignalStrength_v6 response;

    /* Initialize with invalid value -1 */
    memset(&response, -1, sizeof(RIL_SignalStrength_v6));

    line = strdup(s);
    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &skip);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &signalQuality);
    if (err < 0)
        goto error;

    /*
     * RIL API uses the +CSQ 27.007 range 0-31,99 for SignalStrength.
     * See frameworks/base/packages/SystemUI/src/com/android/systemui/statusbar/StatusBarPolicy.java
     * Note that "ASU" corresponds to +CSQ.
     * * * * * * * *  * * * * * * * * * * * * * * * *
     * ASU ranges from 0 to 31 - TS 27.007 Sec 8.5
     * asu = 0 (-113dB or less) is very weak
     * signal, its better to show 0 bars to the user in such cases.
     * asu = 99 is a special case, where the signal strength is unknown.
     *
     * if (asu <= 2 || asu == 99) iconLevel = 0;
     * else if (asu >= 12) iconLevel = 4;
     * else if (asu >= 8)  iconLevel = 3;
     * else if (asu >= 5)  iconLevel = 2;
     * else iconLevel = 1;
     * * * * * * *  * * * * * * * * * * * * * * * * *
     * The +CIEV URC gives a range from 0-5.
     * By using the formula (4*CIEV - 1) we get the following mapping:
     * +CIEV = 0 -> +CSQ =  0 -> iconLevel = 0
     * +CIEV = 1 -> +CSQ =  3 -> iconLevel = 1
     * +CIEV = 2 -> +CSQ =  7 -> iconLevel = 2
     * +CIEV = 3 -> +CSQ = 11 -> iconLevel = 3
     * +CIEV = 4 -> +CSQ = 15 -> iconLevel = 4
     * +CIEV = 5 -> +CSQ = 19 -> iconLevel = 4
     */
    if (signalQuality > 0) {
        signalQuality *= 4;
        signalQuality--;
    }

    /* Assigning values to RIL structure */
    response.GW_SignalStrength.signalStrength = signalQuality;
    response.GW_SignalStrength.bitErrorRate   = 99;  /* undefined */

    RIL_onUnsolicitedResponse(RIL_UNSOL_SIGNAL_STRENGTH,
                              &response, sizeof(RIL_SignalStrength_v6));
    free(line);
    return;

  error:
    free(line);
}

/**
 * RIL_UNSOL_SIM_SMS_STORAGE_FULL
 *
 * SIM SMS storage area is full, cannot receive
 * more messages until memory freed
 */
void unsolSimSmsFull(const char *s)
{
    char *line = NULL;

    int err;
    int skip;
    int response;

    line = strdup(s);
    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &skip);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &response);
    if (err < 0)
        goto error;

    if (response == 1) {
        RIL_onUnsolicitedResponse(RIL_UNSOL_SIM_SMS_STORAGE_FULL, NULL, 0);
    } else {
        ALOGI("Indication SIM have SMS memory locations available again. Ignored");
    }

    free(line);
    return;

  error:
    free(line);
}

/**
 * RIL_REQUEST_SET_BAND_MODE
 *
 * Assign a specified band for RF configuration.
 */
void requestSetBandMode(void *data, size_t datalen, RIL_Token t)
{
    int bandMode = ((int *) data)[0];

    /* Currently only allow automatic. */
    if (bandMode == 0)
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    else
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

/**
 * RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE
 *
 * Query the list of band mode supported by RF.
 *
 * See also: RIL_REQUEST_SET_BAND_MODE
 */
void requestQueryAvailableBandMode(void *data, size_t datalen, RIL_Token t)
{
    int response[2];

    response[0] = 2;
    response[1] = 0;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));
}

/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC
 *
 * Specify that the network should be selected automatically.
 */
void requestSetNetworkSelectionAutomatic(void *data, size_t datalen,
                                         RIL_Token t)
{
    int err = 0;
    struct operatorPollParams *poll_params = NULL;

    err = at_send_command("AT+COPS=0", NULL);
    if (err < 0)
        goto error;

    poll_params = malloc(sizeof(struct operatorPollParams));

    poll_params->loopcount = 0;
    poll_params->t = t;

    enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, pollOperatorSelected,
                    poll_params, &TIMEVAL_OPERATOR_SELECT_POLL);

    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    return;
}

/**
 * RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL
 *
 * Manually select a specified network.
 *
 * The radio baseband/RIL implementation is expected to fall back to
 * automatic selection mode if the manually selected network should go
 * out of range in the future.
 */
void requestSetNetworkSelectionManual(void *data, size_t datalen,
                                      RIL_Token t)
{
    /*
     * AT+COPS=[<mode>[,<format>[,<oper>[,<AcT>]]]]
     *    <mode>   = 1 = Manual (<oper> field shall be present and AcT optionally).
     *    <format> = 2 = Numeric <oper>, the number has structure:
     *                   (country code digit 3)(country code digit 2)(country code digit 1)
     *                   (network code digit 2)(network code digit 1)
     */

    int err = 0;
    char *cmd = NULL;
    ATResponse *atresponse = NULL;
    const char *mccMnc = (const char *) data;

    /* Check inparameter. */
    if (mccMnc == NULL)
        goto error;
    /* Build and send command. */
    asprintf(&cmd, "AT+COPS=1,2,\"%s\"", mccMnc);
    err = at_send_command(cmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
  finally:

    at_response_free(atresponse);

    if (cmd != NULL)
        free(cmd);

    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

    /* If COPS=1 returns error, enter automatic mode.
     * AT+COPS= [<mode>[,<format>[,<oper>[,<AcT>]]]]
     *    <mode> = 0 = Automatic (<oper> field is ignored)
     */

    /* Build and send command. */
    at_send_command("AT+COPS=0", NULL);

    goto finally;
}

/**
 * RIL_REQUEST_QUERY_AVAILABLE_NETWORKS
 *
 * Scans for available networks.
 */
void requestQueryAvailableNetworks(void *data, size_t datalen, RIL_Token t)
{
    /*
     * AT+COPS=?
     *   +COPS: [list of supported (<stat>,long alphanumeric <oper>
     *           ,short alphanumeric <oper>,numeric <oper>[,<AcT>])s]
     *          [,,(list of supported <mode>s),(list of supported <format>s)]
     *
     *   <stat>
     *     0 = unknown
     *     1 = available
     *     2 = current
     *     3 = forbidden
     */

    int err = 0;
    ATResponse *atresponse = NULL;
    const char *statusTable[] =
        { "unknown", "available", "current", "forbidden" };
    char **responseArray = NULL;
    char *p;
    int n = 0;
    int i = 0;
    int j = 0;
    int numStoredNetworks = 0;

    err = at_send_command_multiline("AT+COPS=?", "+COPS:", &atresponse);
    if (err < 0 ||
        atresponse->success == 0 || atresponse->p_intermediates == NULL)
        goto error;

    p = atresponse->p_intermediates->line;
    while (*p != '\0') {
        if (*p == '(')
            n++;
        p++;
    }

    /* Allocate array of strings, blocks of 4 strings. */
    responseArray = alloca(n * 4 * sizeof(char *));

    p = atresponse->p_intermediates->line;

    /* Loop and collect response information into the response array. */
    for (i = 0; i < n; i++) {
        int status = 0;
        char *line = NULL;
        char *s = NULL;
        char *longAlphaNumeric = NULL;
        char *shortAlphaNumeric = NULL;
        char *numeric = NULL;
        char *remaining = NULL;
        bool duplicateFound = false;


        s = line = getFirstElementValue(p, "(", ")", '\"', &remaining);
        p = remaining;

        if (line == NULL) {
            break;
        }
        /* <stat> */
        err = at_tok_nextint(&line, &status);
        if (err < 0)
            goto error;

        /* long alphanumeric <oper> */
        err = at_tok_nextstr(&line, &longAlphaNumeric);
        if (err < 0)
            goto error;

        /* short alphanumeric <oper> */
        err = at_tok_nextstr(&line, &shortAlphaNumeric);
        if (err < 0)
            goto error;

        /* numeric <oper> */
        err = at_tok_nextstr(&line, &numeric);
        if (err < 0)
            goto error;

        /*
          * The response of AT+COPS=? returns GSM networks and WCDMA networks as
          * separate network search hits. The RIL API does not support network
          * type parameter and the RIL must prevent duplicates.
          */
         for (j = numStoredNetworks - 1; j >= 0; j--) {
             if (strcmp(responseArray[j * 4 + 2],
                        numeric) == 0) {
                 ALOGD("%s(): Skipped storing duplicate operator: %s.",
                      __func__, longAlphaNumeric);
                 duplicateFound = true;
                 break;
             }
         }
         if (duplicateFound) {
             free(s);
             s = NULL;
             continue; /* Skip storing this duplicate operator */
         }


        responseArray[numStoredNetworks * 4 + 0] = alloca(strlen(longAlphaNumeric) + 1);
        strcpy(responseArray[numStoredNetworks * 4 + 0], longAlphaNumeric);

        responseArray[numStoredNetworks * 4 + 1] = alloca(strlen(shortAlphaNumeric) + 1);
        strcpy(responseArray[numStoredNetworks * 4 + 1], shortAlphaNumeric);

        responseArray[numStoredNetworks * 4 + 2] = alloca(strlen(numeric) + 1);
        strcpy(responseArray[numStoredNetworks * 4 + 2], numeric);

        free(s);

        /*
         * Check if modem returned an empty string, and fill it with MNC/MMC
         * if that's the case.
         */
        if (responseArray[numStoredNetworks * 4 + 0]
            && strlen(responseArray[numStoredNetworks * 4 + 0]) == 0) {
            responseArray[numStoredNetworks * 4 + 0] =
                alloca(strlen(responseArray[numStoredNetworks * 4 + 2])
                       + 1);
            strcpy(responseArray[numStoredNetworks * 4 + 0], responseArray[numStoredNetworks * 4 + 2]);
        }

        if (responseArray[numStoredNetworks * 4 + 1]
            && strlen(responseArray[numStoredNetworks * 4 + 1]) == 0) {
            responseArray[numStoredNetworks * 4 + 1] =
                alloca(strlen(responseArray[numStoredNetworks * 4 + 2])
                       + 1);
            strcpy(responseArray[numStoredNetworks * 4 + 1], responseArray[numStoredNetworks * 4 + 2]);
        }

        responseArray[numStoredNetworks * 4 + 3] = alloca(strlen(statusTable[status]) + 1);
        sprintf(responseArray[numStoredNetworks * 4 + 3], "%s", statusTable[status]);

        numStoredNetworks++;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseArray,
                          numStoredNetworks * 4 * sizeof(char *));

  finally:
    at_response_free(atresponse);
    return;

  error:
    /* The scenario is manual search ended with no network found. */
    if(at_get_cme_error(atresponse) == CME_NO_NETWORK_SERVICE) {
       RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    } else {
       RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
    goto finally;
}

/**
 * RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE
 *
 * Requests to set the preferred network type for searching and registering
 * (CS/PS domain, RAT, and operation mode).
 */
void requestSetPreferredNetworkType(void *data, size_t datalen,
                                    RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err = 0;
    RIL_PreferredNetworkType rat;
    int arg;
    char *cmd = NULL;
    RIL_Errno errno = RIL_E_GENERIC_FAILURE;

    rat = ((int *) data)[0];

    switch (rat) {
    case PREF_NET_TYPE_GSM_WCDMA:
        arg = 7;
        break;
    case PREF_NET_TYPE_GSM_ONLY:
        arg = 5;
        break;
    case PREF_NET_TYPE_WCDMA:
        arg = 6;
        break;
    default:
        errno = RIL_E_MODE_NOT_SUPPORTED;
        goto error;
    }

    asprintf(&cmd, "AT+CFUN=%d", arg);

    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, errno, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE
 *
 * Query the preferred network type (CS/PS domain, RAT, and operation mode)
 * for searching and registering.
 */
void requestGetPreferredNetworkType(void *data, size_t datalen,
                                    RIL_Token t)
{
    int err = 0;
    RIL_PreferredNetworkType response = 0;
    int cfun;
    char *line;
    ATResponse *atresponse = NULL;

    err = at_send_command_singleline("AT+CFUN?", "+CFUN:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &cfun);
    if (err < 0)
        goto error;

    switch (cfun) {
    case 5:
        response = PREF_NET_TYPE_GSM_ONLY;
        break;
    case 6:
        response = PREF_NET_TYPE_WCDMA;
        break;
    case 7:
        response = PREF_NET_TYPE_GSM_WCDMA;
        break;
    default:
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(int));

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION
 *
 * Requests that network personlization be deactivated.
 */
void requestEnterNetworkDepersonalization(void *data, size_t datalen,
                                          RIL_Token t)
{
    /*
     * AT+CLCK=<fac>,<mode>[,<passwd>[,<class>]]
     *     <fac>    = "PN" = Network Personalization (refer 3GPP TS 22.022)
     *     <mode>   = 0 = Unlock
     *     <passwd> = inparam from upper layer
     */

    int err = 0;
    char *cmd = NULL;
    ATResponse *atresponse = NULL;
    const char *passwd = ((const char **) data)[0];
    RIL_Errno rilerr = RIL_E_GENERIC_FAILURE;
    int num_retries = -1;

    /* Check inparameter. */
    if (passwd == NULL)
        goto error;
    /* Build and send command. */
    asprintf(&cmd, "AT+CLCK=\"PN\",0,\"%s\"", passwd);
    err = at_send_command(cmd, &atresponse);

    free(cmd);

    if (err < 0 || atresponse->success == 0)
        goto error;

    /* TODO: Return number of retries left. */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &num_retries, sizeof(int *));

  finally:
    at_response_free(atresponse);
    return;

  error:
    if (atresponse && at_get_cme_error(atresponse) == 16)
        rilerr = RIL_E_PASSWORD_INCORRECT;

    RIL_onRequestComplete(t, rilerr, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE
 *
 * Query current network selectin mode.
 */
void requestQueryNetworkSelectionMode(void *data, size_t datalen,
                                      RIL_Token t)
{
    int err;
    ATResponse *atresponse = NULL;
    int response = 0;
    char *line;

    err = at_send_command_singleline("AT+COPS?", "+COPS:", &atresponse);

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
    ALOGE("requestQueryNetworkSelectionMode must never return error when radio is on");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SIGNAL_STRENGTH
 *
 * Requests current signal strength and bit error rate.
 *
 * Must succeed if radio is on.
 */
void requestSignalStrength(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    RIL_SignalStrength_v6 signalStrength;
    char *line;

    /* Initialize with invalid value -1 */
    memset(&signalStrength, -1, sizeof(RIL_SignalStrength_v6));

    err = at_send_command_singleline("AT+CSQ", "+CSQ:", &atresponse);

    if (err < 0 || atresponse->success == 0) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        goto error;
    }

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line,
                         &(signalStrength.
                           GW_SignalStrength.signalStrength));
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line,
                         &(signalStrength.GW_SignalStrength.bitErrorRate));
    if (err < 0)
        goto error;

    at_response_free(atresponse);
    atresponse = NULL;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &signalStrength,
                          sizeof(RIL_SignalStrength_v6));

  finally:
    at_response_free(atresponse);
    return;

  error:
    ALOGE("requestSignalStrength must never return an error when radio is on");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_VOICE_REGISTRATION_STATE
 *
 * Request current registration state.
 */
/**
 * RIL_REQUEST_DATA_REGISTRATION_STATE
 *
 * Request current GPRS registration state.
 */
void requestRegistrationState(int request, void *data,
                              size_t datalen, RIL_Token t)
{
    int err = 0;
    int response[4];
    int psc;
    char *responseStr[15];
    ATResponse *atresponse = NULL;
    const char *cmd;
    const char *prefix;
    char *line, *p;
    int commas;
    int skip, tmp;
    int count = 3;
    int acT = 0;

    /* IMPORTANT: Will take screen state lock here. Make sure to always call
       releaseScreenStateLock BEFORE returning! */
    getScreenStateLock();
    if (!getScreenState()) {
#ifdef USE_U8500_RIL
        cmd = "AT+CGREG=2;+CREG=2";
#else
        cmd = "AT+CGREG=2;*EREG=2";
#endif
        at_send_command(cmd, NULL); /* Ignore the response, not VITAL. */
    }

    memset(responseStr, 0, sizeof(responseStr));

    if (request == RIL_REQUEST_VOICE_REGISTRATION_STATE) {
#ifdef USE_U8500_RIL
        cmd = "AT+CREG?";
        prefix = "+CREG:";
#else
        cmd = "AT*EREG?";
        prefix = "*EREG:";
#endif
    } else if (request == RIL_REQUEST_DATA_REGISTRATION_STATE) {
        cmd = "AT+CGREG?";
        prefix = "+CGREG:";
    } else {
        assert(0);
        goto error;
    }

    err = at_send_command_singleline(cmd, prefix, &atresponse);

    if (err < 0 ||
        atresponse->success == 0 || atresponse->p_intermediates == NULL) {
#ifdef USE_U8500_RIL
        if (request == RIL_REQUEST_DATA_REGISTRATION_STATE) {
            switch (at_get_cme_error(atresponse)) {
            case 3:  /* CME_SIM_NOT_INSERTED */
                asprintf(&responseStr[0], "%d", 0); /* state */
                responseStr[1] = NULL;              /* LAC */
                responseStr[2] = NULL;              /* CID */
                asprintf(&responseStr[3], "%d", 0); /* radio technology */
                RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr,
                                      count * sizeof(char *));

                goto finally;
            default:
                goto error;
            }
        }

#endif
        goto error;
    }

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    /*
     * The solicited version of the *EREG/+CGREG response is
     * *EREG/+CGREG: n, stat, [lac, cid [,<AcT>]]
     * and the unsolicited version is
     * *EREG/+CGREG: stat, [lac, cid [,<AcT>]]
     * The <n> parameter is basically "is unsolicited creg on?"
     * which it should always be.
     *
     * Now we should normally get the solicited version here,
     * but the unsolicited version could have snuck in
     * so we have to handle both.
     *
     * Also since the LAC, CID and AcT are only reported when registered,
     * we can have 1, 2, 3, 4 or 5 arguments here.
     *
     */

    /* Count number of commas */
    commas = 0;
    for (p = line; *p != '\0'; p++)
        if (*p == ',')
            commas++;

    switch (commas) {
    case 0:                    /* *EREG/+CGREG: <stat> */
        err = at_tok_nextint(&line, &response[0]);
        if (err < 0)
            goto error;
        response[1] = -1;
        response[2] = -1;
        break;

    case 1:                    /* *EREG/+CGREG: <n>, <stat> */
        err = at_tok_nextint(&line, &skip);
        if (err < 0)
            goto error;
        err = at_tok_nextint(&line, &response[0]);
        if (err < 0)
            goto error;
        response[1] = -1;
        response[2] = -1;
        break;

    case 2:                    /* *EREG/+CGREG: <stat>, <lac>, <cid> */
        err = at_tok_nextint(&line, &response[0]);
        if (err < 0)
            goto error;
        err = at_tok_nexthexint(&line, &response[1]);
        if (err < 0)
            goto error;
        err = at_tok_nexthexint(&line, &response[2]);
        if (err < 0)
            goto error;
        break;

    case 3:                    /* *EREG/+CGREG: <n>, <stat>, <lac>, <cid> */
                               /* *EREG/+CGREG: <stat>, <lac>, <cid>, <AcT> */
        err = at_tok_nextint(&line, &tmp);
        if (err < 0)
            goto error;
        /* We need to check if the second parameter is <lac> */
        if (*(line) == '"') {
            response[0] = tmp; /* <stat> */
            err = at_tok_nexthexint(&line, &response[1]); /* <lac> */
            if (err < 0)
                goto error;
            err = at_tok_nexthexint(&line, &response[2]); /* <cid> */
            if (err < 0)
                goto error;
            err = at_tok_nexthexint(&line, &response[3]); /* <AcT> */
            if (err < 0)
                goto error;
            acT = 1;
        } else {
            err = at_tok_nextint(&line, &response[0]); /* <stat> */
            if (err < 0)
                goto error;
            err = at_tok_nexthexint(&line, &response[1]); /* <lac> */
            if (err < 0)
                goto error;
            err = at_tok_nexthexint(&line, &response[2]); /* <cid> */
            if (err < 0)
                goto error;
        }
        break;

    case 4:                    /* *EREG/+CGREG: <n>, <stat>, <lac>, <cid>, <AcT> */
        err = at_tok_nextint(&line, &skip); /* <n> */
        if (err < 0)
            goto error;
        err = at_tok_nextint(&line, &response[0]); /* <stat> */
        if (err < 0)
            goto error;
        err = at_tok_nexthexint(&line, &response[1]); /* <lac> */
        if (err < 0)
            goto error;
        err = at_tok_nexthexint(&line, &response[2]); /* <cid> */
        if (err < 0)
            goto error;
        err = at_tok_nexthexint(&line, &response[3]); /* <AcT> */
        if (err < 0)
            goto error;
        count = 4;
        acT = 1;
        break;
    default:
        goto error;
    }

    asprintf(&responseStr[0], "%d", response[0]); /* state */

    if (response[1] > 0)
        asprintf(&responseStr[1], "%04x", response[1]); /* LAC */
    else
        responseStr[1] = NULL;

    if (response[2] > 0)
        asprintf(&responseStr[2], "%08x", response[2]); /* CID */
    else
        responseStr[2] = NULL;

    /* Android expects something like this here:
     *
     *    static final int DATA_ACCESS_UNKNOWN = 0;
     *    static final int DATA_ACCESS_GPRS = 1;
     *    static final int DATA_ACCESS_EDGE = 2;
     *    static final int DATA_ACCESS_UMTS = 3;
     *
     * *EREG response:
     *    0 GSM
     *    1 GSM Compact                Not Supported
     *    2 UTRAN
     *
     * +CGREG response:
     *    0 GSM
     *    1 GSM Compact                Not Supported
     *    2 UTRAN
     *    3 GSM w/EGPRS
     *    4 UTRAN w/HSDPA
     *    5 UTRAN w/HSUPA
     *    6 UTRAN w/HSUPA and HSDPA
     */

    RIL_RadioTechnology networkType = RADIO_TECH_UNKNOWN;

    if ((response[1] > 0) && (response[2] > 0)) {
        if (acT) {
            switch (response[3]) {
#ifdef USE_U8500_RIL
            case 0:
                networkType = RADIO_TECH_GPRS;
                break;
#endif
            case 2:
                networkType = RADIO_TECH_UMTS;
                break;
            case 3:
                networkType = RADIO_TECH_EDGE;
                break;
            case 4:
                networkType = RADIO_TECH_HSDPA;
                break;
            case 5:
                networkType = RADIO_TECH_HSUPA;
                break;
            case 6:
                networkType = RADIO_TECH_HSPA;
                break;
            default:
                networkType = RADIO_TECH_UNKNOWN;
                break;
            }
            asprintf(&responseStr[3], "%d", networkType); /* available radio technology */
        }
    }

    if (RIL_REQUEST_VOICE_REGISTRATION_STATE == request) { /* responses defined explicitly to improve readability (memset invoked above) */
        responseStr[4]  = NULL; /* Base Station ID if registered on a CDMA system                                                  */
        responseStr[5]  = NULL; /* Base Station latitude if registered on a CDMA system                                            */
        responseStr[6]  = NULL; /* Base Station longitude if registered on a CDMA system                                           */
        responseStr[7]  = NULL; /* Concurrent services support indicator if registered on a CDMA system                            */
        responseStr[8]  = NULL; /* System ID if registered on a CDMA system                                                        */
        responseStr[9]  = NULL; /* Network ID if registered on a CDMA system                                                       */
        responseStr[10] = NULL; /* TSB-58 Roaming Indicator if registered on a CDMA or EVDO system                                 */
        responseStr[11] = NULL; /* Indicates whether the current system is in the PRL if registered on a CDMA or EVDO system       */
        responseStr[12] = NULL; /* The default Roaming Indicator from the PRL, if registered on a CDMA or EVDO system              */
        responseStr[13] = NULL; /* 3GPP TS 24.008 Annex G reject reason, value unspecified                                         */
        responseStr[14] = NULL; /* Primary Scrambling Code of the current cell as described in TS 25.331                                         */

        /* Clear reject cause cache when the problem no longer remains.
         */
        if (0 != network_reject_cause) { /* a reject cause has been given */
            if ((NETWORK_DOMAIN_PS == network_reject_cause_domain && registration_status_ps)  || /* 1) PS attach failed, but now it's OK */
                (NETWORK_DOMAIN_CS == network_reject_cause_domain && registration_status_cs)  || /* 2) CS attach failed, but now it's OK */
                (NETWORK_DOMAIN_UNKNOWN == network_reject_cause_domain && /* 3) Either PS or CS failed (starting with CS=0, PS=0) and    */
                 (registration_status_cs || registration_status_ps))) {   /*    the attach succeeded                                     */

                ALOGI("%s: clearing network_reject_cause (domain=%d)", __FUNCTION__, network_reject_cause_domain);
                network_reject_cause = 0;
                network_reject_cause_domain = NETWORK_DOMAIN_UNKNOWN;
            }
        }

        /* Specify reject cause in the RIL response if it is available */
        if (0 != network_reject_cause) {
            asprintf(&responseStr[13], "%d", network_reject_cause);
        }

        ALOGI("%s: networkType: %d", __FUNCTION__, networkType);

        /*get Primary Scrambling code if RAT is 3G*/
        if ((networkType == RADIO_TECH_UMTS) || (networkType == RADIO_TECH_HSDPA) ||
            (networkType == RADIO_TECH_HSUPA) || (networkType == RADIO_TECH_HSPA)){
            cmd = "AT*EWSCI";
            err = at_send_command_singleline(cmd, "*EWSCI: ", &atresponse);
            if (err == 0 && atresponse->success == 1){
                line = atresponse->p_intermediates->line;
                err = at_tok_start(&line);
                if (err < 0)
                    goto continue_psc; /*if there is any error, skip PSC reporting*/
                err = at_tok_nextint(&line, &skip);
                if (err < 0)
                    goto continue_psc;
                err = at_tok_nextint(&line, &psc);
                if (err < 0)
                    goto continue_psc;
                asprintf(&responseStr[14], "%x", psc);
            }
        }
continue_psc:
        count = 15;
        registration_status_cs = (1 == response[0] || 5 == response[0]) ? 1 : 0;
    } else if (RIL_REQUEST_DATA_REGISTRATION_STATE == request) {
        if (0 != network_reject_cause) { /* a reject cause has been given */
            if ((NETWORK_DOMAIN_PS == network_reject_cause_domain && registration_status_ps)  || /* 1) PS attach failed, but now it's OK */
                (NETWORK_DOMAIN_UNKNOWN == network_reject_cause_domain && /* 3) Either PS or CS failed (starting with CS=0, PS=0) and    */
                 (registration_status_cs || registration_status_ps))) {   /*    the attach succeeded                                     */

                ALOGI("%s: clearing network_reject_cause (domain=%d)", __FUNCTION__, network_reject_cause_domain);
                network_reject_cause = 0;
                network_reject_cause_domain = NETWORK_DOMAIN_UNKNOWN;
            }
        }
        if (0 != network_reject_cause && NETWORK_DOMAIN_PS == network_reject_cause_domain) {
            asprintf(&responseStr[4], "%d", network_reject_cause);
        }
        asprintf(&responseStr[5], "%d", NBR_OF_SIMULTANEOUS_PS_CALLS);
        registration_status_ps = (1 == response[0] || 5 == response[0]) ? 1 : 0;

    }

    ALOGI("registration state: cs=%d ps=%d", registration_status_cs, registration_status_ps);

    RIL_onRequestComplete(t, RIL_E_SUCCESS, responseStr,
                          count * sizeof(char *));

  finally:
    if (!getScreenState()) {
#ifdef USE_U8500_RIL
        cmd = "AT+CGREG=1;+CREG=1";
#else
        cmd = "AT+CGREG=0;*EREG=0";
#endif
        at_send_command(cmd, NULL);
    }
    releaseScreenStateLock();   /* Important! */

    if (responseStr[0])
        free(responseStr[0]);
    if (responseStr[1])
        free(responseStr[1]);
    if (responseStr[2])
        free(responseStr[2]);
    if (responseStr[3])
        free(responseStr[3]);
    if (responseStr[13])
        free(responseStr[13]);
    if (responseStr[14])
        free(responseStr[14]);

    at_response_free(atresponse);
    return;

  error:
    ALOGE("requestRegistrationState must never return an error when radio is on.");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_OPERATOR
 *
 * Request current operator ONS or EONS.
 */
void requestOperator(void *data, size_t datalen, RIL_Token t)
{
    int err;
    int i;
    int skip;
    ATLine *cursor;
    char *response[3];
    ATResponse *atresponse = NULL;

    memset(response, 0, sizeof(response));

    err = at_send_command_multiline
        ("AT+COPS=3,0;+COPS?;+COPS=3,1;+COPS?;+COPS=3,2;+COPS?", "+COPS:",
         &atresponse);

    /* We expect 3 lines here:
     * +COPS: 0,0,"T - Mobile"
     * +COPS: 0,1,"TMO"
     * +COPS: 0,2,"310170"
     */

    if (err < 0 || atresponse->success == 0)
        goto error;

    for (i = 0, cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next, i++) {
        char *line = cursor->line;

        err = at_tok_start(&line);

        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &skip);

        if (err < 0)
            goto error;

        /* If we're unregistered, we may just get
           a "+COPS: 0" response. */
        if (!at_tok_hasmore(&line)) {
            response[i] = NULL;
            continue;
        }

        err = at_tok_nextint(&line, &skip);

        if (err < 0)
            goto error;

        /* A "+COPS: 0, n" response is also possible. */
        if (!at_tok_hasmore(&line)) {
            response[i] = NULL;
            continue;
        }

        err = at_tok_nextstr(&line, &(response[i]));

        if (err < 0)
            goto error;
    }

    if (i != 3)
        goto error;

    /* Do not show operator if a network reject cause has been specified */
    if (!ok_to_display_operator_name()) {
        response[0] = NULL;
        response[1] = NULL;
        response[2] = NULL;
    } else {
        /*
         * Check if modem returned an empty string, and fill it with MNC/MMC
         * if that's the case.
         */
        if (response[0] && strlen(response[0]) == 0) {
            if (!response[2]) {
                ALOGE("requestOperator: The MNC/MMC string is NULL! (1)");
                at_response_free(atresponse);
                goto error;
            } else {
                response[0] = alloca(strlen(response[2]) + 1);
                strcpy(response[0], response[2]);
            }
        }

        if (response[1] && strlen(response[1]) == 0) {
            if (!response[2]) {
                ALOGE("requestOperator: The MNC/MMC string is NULL! (2)");
                at_response_free(atresponse);
                goto error;
            } else {
                response[1] = alloca(strlen(response[2]) + 1);
                strcpy(response[1], response[2]);
            }
        }
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_LOCATION_UPDATES
 *
 * Enables/disables network state change notifications due to changes in
 * LAC and/or CID (basically, *EREG=2 vs. *EREG=1).
 *
 * Note:  The RIL implementation should default to "updates enabled"
 * when the screen is on and "updates disabled" when the screen is off.
 *
 * See also: RIL_REQUEST_SCREEN_STATE, RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED.
 */
void requestSetLocationUpdates(void *data, size_t datalen, RIL_Token t)
{
    int enable = 0;
    int err = 0;
    char *cmd;
    ATResponse *atresponse = NULL;

    enable = ((int *) data)[0];
    assert(enable == 0 || enable == 1);

    asprintf(&cmd, "AT*EREG=%d", (enable == 0 ? 1 : 2));
    err = at_send_command(cmd, &atresponse);
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
 * RIL_REQUEST_NEIGHBOURINGCELL_IDS
 */
void requestNeighbouringCellIDs(void *data, size_t datalen, RIL_Token t)
{
    int access_technology = -1;
    int dummy = 0;
    char *dummyStr = NULL;
    int err = 0;
    ATResponse *atresponse = NULL;
    char *line = NULL;

    /* Determine GSM or WCDMA */
    err = at_send_command_singleline("AT+COPS?", "+COPS:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &dummy);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &dummy);
    if (err < 0) {
        /* If only "mode" is reported, the modem is not registered */
        ALOGE("%s: Not registered!\n", __func__);
        goto error;
    }

    err = at_tok_nextstr(&line, &dummyStr);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &access_technology);
    if (err < 0) {
        ALOGE("%s: No access-technology (AcT)!\n", __func__);
        goto error;
    }

    switch (access_technology) {
    case 0:  /* GSM */
    case 3:  /* EDGE */
    case 16: /* GPRS_DTM */
    case 19: /* EDGE_DTM */
        GSMNeighbouringCellIDs(data, datalen, t);
        break;

    case 2: /* UTRAN */
    case 4: /* HSDPA */
    case 5: /* HSUPA */
    case 6: /* HSDPA_HSUPA */
        WCDMANeighbouringCellIDs(data, datalen, t);
        break;

    default:
        ALOGE("%s: Unexpected access-technology (AcT): %d\n", __func__, access_technology);
        goto error;
    }

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}


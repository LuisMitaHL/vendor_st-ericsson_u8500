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
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <telephony/ril.h>
#include "atchannel.h"
#include "at_tok.h"
#include "misc.h"
#include "u300-ril.h"

#define LOG_TAG "RILV"
#include <utils/Log.h>

static char s_outstanding_acknowledge = 0;

#define OUTSTANDING_SMS    0
#define OUTSTANDING_STATUS 1

#define SMS_SEND_FDN_CHECK_FAILED 512
#define SMS_SEND_FDN_CHECK_FAILED_ERROR_CODE 6

struct held_pdu {
    char type;
    char *sms_pdu;
    struct held_pdu *next;
};

static pthread_mutex_t s_held_pdus_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct held_pdu *s_held_pdus = NULL;
static StringSet s_cbsMidList = {NULL, NULL, 0, 0, 0, 0};

static struct held_pdu *dequeue_held_pdu()
{
    struct held_pdu *hpdu = NULL;

    if (s_held_pdus != NULL) {
        hpdu = s_held_pdus;
        s_held_pdus = hpdu->next;
        hpdu->next = NULL;
    }

    return hpdu;
}

static void enqueue_held_pdu(char type, const char *sms_pdu)
{
    struct held_pdu *hpdu = malloc(sizeof(*hpdu));

    memset(hpdu, 0, sizeof(*hpdu));
    hpdu->type = type;
    hpdu->sms_pdu = strdup(sms_pdu);

    if (s_held_pdus == NULL)
        s_held_pdus = hpdu;
    else {
        struct held_pdu *p = s_held_pdus;
        while (p->next != NULL)
            p = p->next;

        p->next = hpdu;
    }
}

void onNewSms(const char *sms_pdu)
{
#ifndef SMS_PHASE_2_PLUS
    pthread_mutex_lock(&s_held_pdus_mutex);

    if (s_outstanding_acknowledge) {
        ALOGI("Waiting for ack for previous sms, enqueueing PDU.");
        enqueue_held_pdu(OUTSTANDING_SMS, sms_pdu);
    } else {
        s_outstanding_acknowledge = 1;
#endif
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS,
                                  sms_pdu, strlen(sms_pdu));
#ifndef SMS_PHASE_2_PLUS
    }

    pthread_mutex_unlock(&s_held_pdus_mutex);
#endif
}

void onNewStatusReport(const char *sms_pdu)
{
    char *response = NULL;
#ifndef USE_U8500_RIL
    /* Baseband will not prepend SMSC addr, but Android expects it. */
    asprintf(&response, "%s%s", "00", sms_pdu);

    pthread_mutex_lock(&s_held_pdus_mutex);

    if (s_outstanding_acknowledge) {
        ALOGE("Waiting for previous ack, enqueueing PDU..");
        enqueue_held_pdu(OUTSTANDING_STATUS, response);
    } else {
        s_outstanding_acknowledge = 1;
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT,
                                  response, strlen(response));
#else
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT,
                                  sms_pdu, strlen(sms_pdu));
#endif
#ifndef USE_U8500_RIL
    }

    pthread_mutex_unlock(&s_held_pdus_mutex);
#endif
}

void onNewBroadcastSms(const char *s, const char *pdu)
{
    char *tok = NULL;
    char *line = NULL;
    unsigned char *message = NULL;
    int length = 0;
    int err = 0;
    int i;

    if (NULL == s || NULL == pdu) {
        ALOGE("null parameters received");
        goto error;
    }

    tok = line = strdup(s);

    err = at_tok_start(&tok);
    if (err < 0) {
        ALOGE("parsing start failed");
        goto error;
    }

    err = at_tok_nextint(&tok, &length);
    if (err < 0) {
        ALOGE("parsing length failed");
        goto error;
    }

    if (length <= 0) {
        ALOGE("invalid PDU length:%d", length);
        goto error;
    }

    message = (unsigned char *) calloc(1, length);

    if (NULL == message) {
        ALOGE("allocation failed");
        goto error;
    }

    for (i = 0; i < length; i++) {
        message[i] |= (char2nib(pdu[i * 2]) << 4);
        message[i] |= char2nib(pdu[i * 2 + 1]);
    }

    RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS,
                              message, length);

error:
    free(line);
    return;
}

void onNewSmsOnSIM(const char *s)
{
    char *line;
    char *mem;
    char *tok;
    int err = 0;
    int index = -1;

    tok = line = strdup(s);

    err = at_tok_start(&tok);
    if (err < 0)
        goto error;

    err = at_tok_nextstr(&tok, &mem);
    if (err < 0)
        goto error;

    if (strncmp(mem, "SM", 2) != 0)
        goto error;

    err = at_tok_nextint(&tok, &index);
    if (err < 0)
        goto error;

    RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM,
                              &index, sizeof(int *));

  finally:
    free(line);
    return;

  error:
    ALOGE("Failed to parse +CMTI.");
    goto finally;
}

/**
 * RIL_REQUEST_GSM_BROADCAST_GET_SMS_CONFIG
 */
void requestGSMGetBroadcastSMSConfig(void *data, size_t datalen,
                                     RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err = 0;
    char *line;
    char *tok = NULL;
    RIL_GSM_BroadcastSmsConfigInfo *configInfo = NULL;

    err = at_send_command_singleline("AT+CSCB?", "+CSCB:", &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    configInfo = malloc(sizeof(RIL_GSM_BroadcastSmsConfigInfo));
    memset(configInfo, 0, sizeof(RIL_GSM_BroadcastSmsConfigInfo));

    tok = atresponse->p_intermediates->line;


    err = at_tok_start(&tok);
    if (err < 0)
        goto error;

    err = at_tok_nextbool(&tok, (char *) &(configInfo->selected));
    if (err < 0)
        goto error;

    /* Get the string that yields the service ids.
       We expect the form: "fromServiceId-toServiceId". */
    err = at_tok_nextstr(&tok, &line);
    if (err < 0)
        goto error;

    line = strsep(&tok, "\"");
    if (line == NULL)
        goto error;
    err = at_tok_nextint(&tok, &(configInfo->fromServiceId));
    if (err < 0)
        goto error;

    line = strsep(&tok, "-");
    if (line == NULL)
        goto error;
    /*err = at_tok_nexthexint(&tok, &temp);
       if (err < 0)
       goto error; */

    err = at_tok_nextint(&tok, &(configInfo->toServiceId));
    if (err < 0)
        goto error;

    /* Get the string that yields the coding schemes.
       We expect the form: "fromCodeScheme-toCodeScheme". */
    err = at_tok_nextstr(&tok, &line);
    if (err < 0)
        goto error;

    line = strsep(&line, "\"");

    if (line == NULL)
        goto error;

    err = at_tok_nextint(&tok, &(configInfo->fromCodeScheme));
    if (err < 0)
        goto error;

    line = strsep(&line, "-");
    if (line == NULL)
        goto error;

    err = at_tok_nextint(&tok, &(configInfo->toCodeScheme));
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, configInfo,
                          sizeof(RIL_GSM_BroadcastSmsConfigInfo *));

  finally:
    if (configInfo != NULL)
        free(configInfo);
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_GSM_BROADCAST_SET_SMS_CONFIG
 */
void requestGSMSetBroadcastSMSConfig(void *data, size_t datalen,
                                     RIL_Token t)
{
    ATResponse *atresponse = NULL;
    char *cmd = NULL;
    int i;
    int err = 0;
    int count = 0;
    char *midRange = NULL;
    char *dcsRange = NULL;
    char *midRangesStr;
    char *dcsRangesStr;
    char default_mids[] = "20,147,263,310,457,590,999";
    /*TBD: add 0-255 only once, instead for each msg id, this should be checked*/
    char default_dcs[] = "0-255,0-255,0-255,0-255,0-255,0-255,0-255";
    RIL_GSM_BroadcastSmsConfigInfo **configInfo = NULL;

    if (NULL == data) {
        ALOGE("null parameter received");
        goto error;
    }

    if (0 == datalen || 0 != datalen % sizeof(RIL_GSM_BroadcastSmsConfigInfo *)) {
        ALOGE("incorrect datalength:%d received", datalen);
        goto error;
    }

    configInfo = (RIL_GSM_BroadcastSmsConfigInfo **) data;
    count = datalen / sizeof(RIL_GSM_BroadcastSmsConfigInfo *);
    for (i = 0; i < count; i++) {
        ALOGD("from - %d to - %d, dcs1 - %d dcs2 - %d", configInfo[i]->fromServiceId, configInfo[i]->toServiceId,
             configInfo[i]->fromCodeScheme, configInfo[i]->toCodeScheme);
        asprintf(&midRange, "%d-%d", configInfo[i]->fromServiceId, configInfo[i]->toServiceId);
        assert(midRange != NULL);
        asprintf(&dcsRange, "%d-%d", configInfo[i]->fromCodeScheme, configInfo[i]->toCodeScheme);
        assert(dcsRange != NULL);
        if (!configInfo[i]->selected) {
            (void)stringSetRemove(&s_cbsMidList, midRange);
            ALOGD("Removed MID range: %s", midRange);
        }else {
            (void)stringSetAdd(&s_cbsMidList, midRange, dcsRange);
            ALOGD("Added MID: %s DCS: %s", midRange, dcsRange);
        }
        free(midRange);
        free(dcsRange);
    }

    midRangesStr = stringSetToCommaSeparatedString(&s_cbsMidList);
    dcsRangesStr = dcsStringSetToCommaSeparatedString(&s_cbsMidList);

    ALOGD("CBS already activated, updating configuration (%s,%s)", midRangesStr, dcsRangesStr);
    /*TBD: Add RIL DCS only if it is other than 0-255. Otherwise add 0-255 only once*/
    if (strlen(midRangesStr)){
        asprintf(&cmd, "AT+CSCB=0,\"%s,%s\",\"%s,%s\"", default_mids, midRangesStr, default_dcs, dcsRangesStr);
    }else {
        asprintf(&cmd, "AT+CSCB=0,\"%s\",\"%s\"", default_mids, default_dcs);
    }
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    free(midRangesStr);
    free(dcsRangesStr);

    if (err < 0 || atresponse->success == 0) {
        ALOGE("Failed: Could not set the CB configuration");
        goto error;
    }

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    at_response_free(atresponse);
    return;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION
 */
void requestGSMSMSBroadcastActivation(void *data, size_t datalen,
                                      RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int mode, mt, bm, ds, bfr, skip;
    int activation;
    char *cmd = NULL;
    char *tok;
    int err;

    /* AT+CNMI=[<mode>[,<mt>[,<bm>[,<ds>[,<bfr>]]]]] */
    err = at_send_command_singleline("AT+CNMI?", "+CNMI:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    tok = atresponse->p_intermediates->line;

    err = at_tok_start(&tok);
    if (err < 0)
        goto error;
    err = at_tok_nextint(&tok, &mode);
    if (err < 0)
        goto error;
    err = at_tok_nextint(&tok, &mt);
    if (err < 0)
        goto error;
    err = at_tok_nextint(&tok, &skip);
    if (err < 0)
        goto error;
    err = at_tok_nextint(&tok, &ds);
    if (err < 0)
        goto error;
    err = at_tok_nextint(&tok, &bfr);
    if (err < 0)
        goto error;

    at_response_free(atresponse);

    /* 0 - Activate, 1 - Turn off */
    activation = *((const int *)data);
    if (activation == 0)
        bm = 2;
    else
        bm = 0;

    asprintf(&cmd, "AT+CNMI=%d,%d,%d,%d,%d", mode, mt, bm, ds, bfr);

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
 * RIL_REQUEST_SEND_SMS
 *
 * Sends an SMS message.
 */
void requestSendSMS(void *data, size_t datalen, RIL_Token t)
{
    int err;
    const char *smsc;
    const char *pdu;
    char *line;
    int tpLayerLength;
    char *cmd1, *cmd2;
    RIL_SMS_Response response;
    AT_CMS_Error cms_error;
    RIL_Errno ret = RIL_E_SUCCESS;
    ATResponse *atresponse = NULL;

    smsc = ((const char **) data)[0];
    pdu = ((const char **) data)[1];

    tpLayerLength = strlen(pdu) / 2;

    /* NULL for default SMSC. */
    if (smsc == NULL)
        smsc = "00";

    asprintf(&cmd1, "AT+CMGS=%d", tpLayerLength);
    asprintf(&cmd2, "%s%s", smsc, pdu);

    err = at_send_command_sms(cmd1, cmd2, "+CMGS:", &atresponse);
    free(cmd1);
    free(cmd2);

    if (err != 0 || atresponse->success == 0)
        goto error;

    memset(&response, 0, sizeof(response));

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &response.messageRef);
    if (err < 0)
        goto error;

    /* No support for ackPDU. Do we need it? */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(response));

  finally:
    at_response_free(atresponse);
    return;

  error:

#ifdef USE_U8500_RIL
  if (NULL == atresponse) {
      RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
      goto finally;
  } else {
    if (at_get_cms_error(atresponse, &cms_error)) {
      switch (cms_error) {
      case SMS_SEND_FDN_CHECK_FAILED:
        response.errorCode = SMS_SEND_FDN_CHECK_FAILED_ERROR_CODE;
        RIL_onRequestComplete(t, RIL_E_FDN_CHECK_FAILURE, &response, sizeof(response));
        break;
      default:
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        break;
      }
    }
    goto finally;
  }
#else
    if (at_get_cms_error(atresponse, &cms_error)) {
        switch (cms_error) {
        /* Network not yet fully ready (pending/ongoing) temporary
         * fail resend the sms.
         */
        case CMS_TEMPORARY_FAILURE:
            ret = RIL_E_SMS_SEND_FAIL_RETRY;
            break;
        default:
            ret = RIL_E_GENERIC_FAILURE;
            break;
        }
        RIL_onRequestComplete(t, ret, NULL, 0);
    }
    goto finally;
#endif
}

/**
 * RIL_REQUEST_SEND_SMS_EXPECT_MORE
 *
 * Send an SMS message. Identical to RIL_REQUEST_SEND_SMS,
 * except that more messages are expected to be sent soon. If possible,
 * keep SMS relay protocol link open (eg TS 27.005 AT+CMMS command).
 */
void requestSendSMSExpectMore(void *data, size_t datalen, RIL_Token t)
{
    /* Throw the command on the channel and ignore any errors, since we
       need to send the SMS anyway and subsequent SMSes will be sent anyway. */
    at_send_command("AT+CMMS=1", NULL);

    requestSendSMS(data, datalen, t);
}

/**
 * RIL_REQUEST_SMS_ACKNOWLEDGE
 *
 * Acknowledge successful or failed receipt of SMS previously indicated
 * via RIL_UNSOL_RESPONSE_NEW_SMS .
 */
void requestSMSAcknowledge(void *data, size_t datalen, RIL_Token t)
{
#ifndef USE_U8500_RIL
    struct held_pdu *hpdu;

    pthread_mutex_lock(&s_held_pdus_mutex);

    hpdu = dequeue_held_pdu();

    if (hpdu != NULL) {
        ALOGE("Outstanding requests in queue, dequeueing and sending.");
        int unsolResponse = 0;

        if (hpdu->type == OUTSTANDING_SMS)
            unsolResponse = RIL_UNSOL_RESPONSE_NEW_SMS;
        else
            unsolResponse = RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT;

        RIL_onUnsolicitedResponse(unsolResponse, hpdu->sms_pdu,
                                  strlen(hpdu->sms_pdu));

        free(hpdu->sms_pdu);
        free(hpdu);
    } else
        s_outstanding_acknowledge = 0;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

    pthread_mutex_unlock(&s_held_pdus_mutex);
#else

    int err;
        ATResponse *atresponse = NULL;
        int success = ((int *)data)[0];

        if(!success)
            err = at_send_command("AT+CNMA=2", &atresponse);
        else if(success == 1)
            err = at_send_command("AT+CNMA=1", &atresponse);
        else
        {
            ALOGE("unsupported value for +CNMA");
            goto error;
        }

        if(err < 0 || atresponse->success == 0)
            goto error;

        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

    finally:
        at_response_free(atresponse);
        return;

    error:
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        goto finally;

    /* TODO FIXME Temporary solution - Will be updated when CNMA support is delivered */
#endif
}

/**
 * RIL_REQUEST_WRITE_SMS_TO_SIM
 *
 * Stores a SMS message to SIM memory.
 */
void requestWriteSmsToSim(void *data, size_t datalen, RIL_Token t)
{
    RIL_SMS_WriteArgs *args;
    char *cmd;
    char *pdu;
    char *line;
    int length;
    int index;
    int err;
    AT_CMS_Error cms_error;
    ATResponse *atresponse = NULL;

    args = (RIL_SMS_WriteArgs *) data;

    length = strlen(args->pdu) / 2;
    asprintf(&cmd, "AT+CMGW=%d,%d", length, args->status);
    asprintf(&pdu, "%s%s", (args->smsc ? args->smsc : "00"), args->pdu);

    err = at_send_command_sms(cmd, pdu, "+CMGW:", &atresponse);
    free(cmd);
    free(pdu);

    /*Check if SIM_SMS_Storage already is full, if so send unsolicited
     * event UNSOL_SIM_SMS_STORAGE_FULL. No good error code according
     * to this Storage full exists in ril.h, therefore we send an unsol
     * event here just to provide more information about the error.
     */
    if (at_get_cms_error(atresponse, &cms_error)) {
        if (cms_error == CMS_MEMORY_FULL) {
            RIL_onUnsolicitedResponse(RIL_UNSOL_SIM_SMS_STORAGE_FULL,
                                      NULL, 0);
            goto error;
        }
    }

    if (err < 0 || atresponse->success == 0)
        goto error;

    if (atresponse->p_intermediates->line == NULL)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &index);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, &index, sizeof(int *));

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_DELETE_SMS_ON_SIM
 *
 * Deletes a SMS message from SIM memory.
 */
void requestDeleteSmsOnSim(void *data, size_t datalen, RIL_Token t)
{
    char *cmd;
    ATResponse *atresponse = NULL;
    int err;

    asprintf(&cmd, "AT+CMGD=%d", ((int *) data)[0]);
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    else
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    at_response_free(atresponse);
    return;
}

/**
 * RIL_REQUEST_GET_SMSC_ADDRESS
 */
void requestGetSMSCAddress(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    char *line;
    char *response;

    err = at_send_command_singleline("AT+CSCA?", "+CSCA:", &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextstr(&line, &response);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(char *));

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_SMSC_ADDRESS
 */
void requestSetSMSCAddress(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    char *cmd;
    const char *smsc = (const char *) data;

    asprintf(&cmd, "AT+CSCA=\"%s\"", smsc);
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

void requestSmsStorageFull(void *data, size_t datalen, RIL_Token t)
{
    char *cmd;
    ATResponse *atresponse = NULL;
    int err;
#ifdef USE_U8500_RIL
    int storage = 0;

    if (0 == ((int *) data)[0]) {
        storage = 1; /* Invert */
    } /* else it should remain 0 */

    asprintf(&cmd, "AT*ESMSFULL=%d", storage);
#else
    asprintf(&cmd, "AT*ESMSFULL=%d", ((int *) data)[0]);
#endif
    err = at_send_command(cmd, &atresponse);
    free(cmd);
    if (err < 0 || atresponse->success == 0)
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    else
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    at_response_free(atresponse);
    return;
}

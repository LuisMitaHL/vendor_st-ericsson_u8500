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
#include <assert.h>
#include <telephony/ril.h>
#include "atchannel.h"
#include "at_tok.h"
#include "u300-ril.h"

#define LOG_TAG "RILV"
#include <utils/Log.h>

/* From TS 27.007 7.11 <reason> */
typedef enum {
    CCFC_REASON_UNCONDITIONAL = 0,
    CCFC_REASON_MOBILE_BUSY = 1,
    CCFC_REASON_NO_REPLY = 2,
    CCFC_REASON_NOT_REACHABLE = 3,
    CCFC_REASON_ALL_CALL_FRWDNG = 4,
    CCFC_REASON_ALL_COND_CALL_FRWDNG = 5
} ccfc_reason_t;

/**
 * RIL_REQUEST_QUERY_CLIP
 *
 * Queries the status of the CLIP supplementary service.
 *
 * (for MMI code "*#30#")
 */
void requestQueryClip(void *data, size_t datalen, RIL_Token t)
{
    /* AT+CLIP? */
    char *line = NULL;
    int err = 0;
    int response = 2;
    ATResponse *atresponse = NULL;

    err = at_send_command_singleline("AT+CLIP?", "+CLIP:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    /* Read the first int and ignore it, we just want to know if
       CLIP is provisioned. */
    err = at_tok_nextint(&line, &response);
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
 * RIL_REQUEST_CANCEL_USSD
 *
 * Cancel the current USSD session if one exists.
 */
void requestCancelUSSD(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command("AT+CUSD=2", &atresponse);

    if (err < 0 || atresponse->success == 0)
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    else {
        RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
    }
    at_response_free(atresponse);
    return;
}

/**
 * RIL_REQUEST_SEND_USSD
 *
 * Send a USSD message.
 *
 * See also: RIL_REQUEST_CANCEL_USSD, RIL_UNSOL_ON_USSD.
 */
void requestSendUSSD(void *data, size_t datalen, RIL_Token t)
{
    const char *ussdRequest;
    char *cmd = NULL;
    int err = -1;
    ATResponse *response = NULL;

    ussdRequest = (char *)data;

    asprintf(&cmd, "AT+CUSD=1,\"%s\"", ussdRequest);

    err = at_send_command(cmd, &response);
    free(cmd);

    if (err < 0 || response->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    if (response != NULL)
        at_response_free(response);

    return;

error:
    if (response && at_get_cme_error(response) == CME_FDN_BLOCKED) {
        RIL_onRequestComplete(t, RIL_E_FDN_CHECK_FAILURE, NULL, 0);
    } else {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
    goto finally;
}

/**
 * RIL_UNSOL_SUPP_SVC_NOTIFICATION
 *
 * Reports supplementary service related notification from the network.
 */
void onSuppServiceNotification(const char *s, int type)
{
    RIL_SuppSvcNotification ssnResponse;
    char *line;
    char *tok;
    int err;

    line = tok = strdup(s);

    memset(&ssnResponse, 0, sizeof(ssnResponse));
    ssnResponse.notificationType = type;

    err = at_tok_start(&tok);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&tok, &ssnResponse.code);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&tok, &ssnResponse.index);

    if (ssnResponse.code == 16 ||
        (type == 0 && ssnResponse.code == 4) ||
        (type == 1 && ssnResponse.code == 1)) {
        if (err < 0)
            goto error;
    } else {
        /* Index may be invalid but continue parsing in this case */
        if (err < 0)
            ssnResponse.index = 0;
    }

    /* CSSU has two extra parameters */
    if (ssnResponse.notificationType == 1 && at_tok_hasmore(&tok)) {
        err = at_tok_nextstr(&tok, &ssnResponse.number);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&tok, &ssnResponse.type);
        if (err < 0)
            goto error;
    }

    RIL_onUnsolicitedResponse(RIL_UNSOL_SUPP_SVC_NOTIFICATION,
                              &ssnResponse, sizeof(ssnResponse));

  error:
    free(line);
}

/**
 * RIL_UNSOL_ON_USSD
 *
 * Called when a new USSD message is received.
 */
void onUSSDReceived(const char *s)
{
    char **response;
    char *line;
    int err = -1;
    int i = 0;
    int n = 0;

    line = alloca(strlen(s) + 1);
    strcpy(line, s);
    line[strlen(s)] = 0;

    response = alloca(2 * sizeof(char *));
    response[0] = NULL;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &i);
    if (err < 0)
        goto error;

    if (i < 0 || i > 5)
        goto error;

    response[0] = alloca(2);
    sprintf(response[0], "%d", i);

    n = 1;

    if (i < 2) {
        n = 2;

        err = at_tok_nextstr(&line, &response[1]);
        if (err < 0){
            /* Ignore the <str> parameter if its value is NULL in case of USSD-Notify */
            if(NULL == line && 0 == i){
                n=1;
            } else{
            goto error;
            }
        }
    }

    /* TODO: We ignore the <dcs> parameter, might need this some day. */

    RIL_onUnsolicitedResponse(RIL_UNSOL_ON_USSD, response,
                              n * sizeof(char *));

  error:
    return;
}

/**
 * RIL_REQUEST_GET_CLIR
 *
 * Gets current CLIR status.
 */
void requestGetCLIR(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;
    int response[2];            // <n> and <m>
    char *line;

    err = at_send_command_singleline("AT+CLIR?", "+CLIR:", &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    /* Parse and store <n> as first repsonse parameter. */
    err = at_tok_nextint(&line, &(response[0]));
    if (err < 0)
        goto error;

    /* Parse and store <m> as second response parameter. */
    err = at_tok_nextint(&line, &(response[1]));
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(response));

  finally:
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_CLIR
 */
void requestSetCLIR(void *data, size_t datalen, RIL_Token t)
{
    char *cmd = NULL;
    int err;
    ATResponse *atresponse = NULL;

    asprintf(&cmd, "AT+CLIR=%d", ((int *) data)[0]);

    err = at_send_command(cmd, &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    free(cmd);
    at_response_free(atresponse);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_QUERY_CALL_FORWARD_STATUS
 */
void requestQueryCallForwardStatus(void *data, size_t datalen, RIL_Token t)
{
    char *cmd;
    int err;
    ATResponse *atresponse = NULL;
    ATLine *cursor = NULL;
    const RIL_CallForwardInfo *pCallForwardInfo =
        (RIL_CallForwardInfo *) data;
    int mode = 2;               /* Query status */
    int n;
    int i = 0;
    int serviceClass = pCallForwardInfo->serviceClass;
    RIL_CallForwardInfo *rilResponse;
    RIL_CallForwardInfo **rilResponseArray;
    int skip;
    char *strskip;

    /* AT+CCFC=<reason>,<mode>[,<number>[,<type>[,<class> [,<subaddr> [,<satype> [,<time>]]]]]] */

#ifdef USE_U8500_RIL
    /* Workaround for Android sending serviceClass = 0.
     *
     * Class 0 is not considered a valid value and according to 3GPP 24.080.
     * The default value for <class> is 7, i.e. voice + data + fax.
     */
    if (serviceClass == 0) {
        serviceClass = SS_DEFAULT_CLASS;
    }

    asprintf(&cmd, "AT+CCFC=%d,%d,,,%d", pCallForwardInfo->reason, mode, serviceClass);

#else

    asprintf(&cmd, "AT+CCFC=%d,%d", pCallForwardInfo->reason, mode);

#endif

    err = at_send_command_multiline(cmd, "+CCFC:", &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    n = 0;

    for (cursor = atresponse->p_intermediates; cursor != NULL;
         cursor = cursor->p_next)
        n++;

    rilResponse = alloca(n * sizeof(RIL_CallForwardInfo));
    rilResponseArray = alloca(n * sizeof(RIL_CallForwardInfo *));
    memset(rilResponse, 0, sizeof(RIL_CallForwardInfo) * n);

    for (i = 0; i < n; i++)
        rilResponseArray[i] = &(rilResponse[i]);

    /* When <mode>=2 and command successful:
     * +CCFC: <status>,<class1>[,<number>,<type>
     * [,<subaddr>,<satype>[,<time>]]][
     * <CR><LF>
     * +CCFC: <status>,<class2>[,<number>,<type>
     * [,<subaddr>,<satype>[,<time>]]]
     * [...]]
     */
    for (i = 0, cursor = atresponse->p_intermediates;
         cursor != NULL && i < n; cursor = cursor->p_next, ++i) {
        char *line = NULL;
        line = cursor->line;

        err = at_tok_start(&line);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &rilResponse[i].status);
        if (err < 0)
            goto error;

        err = at_tok_nextint(&line, &rilResponse[i].serviceClass);
        if (err < 0)
            goto error;

        if (at_tok_hasmore(&line)) {
            err = at_tok_nextstr(&line, &rilResponse[i].number);
            if (err < 0)
                goto error;

            err = at_tok_nextint(&line, &rilResponse[i].toa);
            if (err < 0)
                goto error;

            if (at_tok_hasmore(&line)) {
                err = at_tok_nextstr(&line, &strskip); /* skip <subaddr> */
                /* Ignore error, parameter might be missing */

                err = at_tok_nextint(&line, &skip); /* skip <satype> */
                /* Ignore error, parameter might be missing */

                /* reason = "No reply" */
                if (at_tok_hasmore(&line) &&
                    pCallForwardInfo->reason == CCFC_REASON_NO_REPLY) {
                    err = at_tok_nextint(&line,
                                         &rilResponse[i].timeSeconds);
                    if (err < 0)
                        goto error;
                }
            }
        }
        rilResponse[i].reason = pCallForwardInfo->reason;
    }
    RIL_onRequestComplete(t, RIL_E_SUCCESS, rilResponseArray,
                          n * sizeof(RIL_CallForwardInfo *));
finally:
    if (cmd != NULL)
        free(cmd);
    if (atresponse != NULL)
        at_response_free(atresponse);
    return;

error:
    if (atresponse && at_get_cme_error(atresponse) == CME_FDN_BLOCKED) {
        RIL_onRequestComplete(t, RIL_E_FDN_CHECK_FAILURE, NULL, 0);
    } else {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
    goto finally;
}

/**
 * RIL_REQUEST_SET_CALL_FORWARD
 *
 * Configure call forward rule.
 */
void requestSetCallForward(void *data, size_t datalen, RIL_Token t)
{
    char *cmd = NULL;
    int err;
    ATResponse *atresponse = NULL;
    const RIL_CallForwardInfo *callForwardInfo =
        (RIL_CallForwardInfo *) data;
    int serviceClass = callForwardInfo->serviceClass;

    /* AT+CCFC=<reason>,<mode>[,<number>[,<type>[,<class> [,<subaddr>
     * [,<satype> [,<time>]]]]]] Disregard time parameter.
     */

    /* Workaround for Android sending serviceClass = 0 when doing call
     * forwarding registration using USSD string (**21*<phone-number>#).
     *
     * <class> = 0 is not a legal value when doing registration. The
     * network does not return ERROR but it is not possible to do enable (*21#)
     * later.The default value for <class> is 7, i.e. voice + data + fax. If
     * the network is not able to register one of the classes then AT+CCFC
     * will return error, but some of the classes may have been registered.
     * The enable command (AT+CCFC=0,1 or *21#) will also use 7 as default for
     * class.
     */
    if (serviceClass == 0) {
        serviceClass = SS_DEFAULT_CLASS;
    }

    /* Android may send down the phone number even if mode = 0 (disable) or
     * mode = 4 (erasure). This will give ERROR from the network, which means
     * that the dialstring must be disregarded by the RIL.
     */

    /* The phone number must always be included when mode = 3 (registration).
     * If a phone number is supplied and mode = 1 (activation), then the mode
     * must be changed to 3 (registration) or the network will fail the request.
     */

#ifdef USE_U8500_RIL

    if (((callForwardInfo->status == 1) ||
        (callForwardInfo->status == 3)) &&
        (callForwardInfo->number != NULL)) {

        if (2 == callForwardInfo->reason &&
            0 < callForwardInfo->timeSeconds) {
            asprintf(&cmd, "AT+CCFC=%d,%d,\"%s\",%d,%d,,,%d",
                     callForwardInfo->reason,
                     callForwardInfo->status,
                     callForwardInfo->number,
                     callForwardInfo->toa,
                     serviceClass,
                     callForwardInfo->timeSeconds);
        } else {
            asprintf(&cmd, "AT+CCFC=%d,%d,\"%s\",%d,%d",
                     callForwardInfo->reason,
                     callForwardInfo->status,
                     callForwardInfo->number,
                     callForwardInfo->toa,
                     serviceClass);
        }
    } else if ((callForwardInfo->status == 0) ||
               (callForwardInfo->status == 1) ||
               (callForwardInfo->status == 4))
    {
        if (2 == callForwardInfo->reason &&
            0 < callForwardInfo->timeSeconds) {
            asprintf(&cmd, "AT+CCFC=%d,%d,,,%d,,,%d",
                callForwardInfo->reason, callForwardInfo->status, serviceClass, callForwardInfo->timeSeconds);
        } else {
            asprintf(&cmd, "AT+CCFC=%d,%d,,,%d",
                callForwardInfo->reason, callForwardInfo->status, serviceClass);

        }
    } else {
        ALOGE("requestSetCallForward(): unexpected status or missing number");
        goto error;
    }

#else

    if (((callForwardInfo->status == 1) ||
        (callForwardInfo->status == 3)) &&
        (callForwardInfo->number != NULL)) {
        asprintf(&cmd, "AT+CCFC=%d,3,\"%s\",%d,%d",
                 callForwardInfo->reason,
                 callForwardInfo->number,
                 callForwardInfo->toa,
                 serviceClass);
    } else if ((callForwardInfo->status == 0) ||
               (callForwardInfo->status == 1) ||
               (callForwardInfo->status == 4))
    {
        asprintf(&cmd, "AT+CCFC=%d,%d",
                 callForwardInfo->reason, callForwardInfo->status);
    } else {
        ALOGE("requestSetCallForward(): unexpected status or missing number");
        goto error;
    }

#endif

    err = at_send_command(cmd, &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

finally:
    free(cmd);
    at_response_free(atresponse);
    return;

error:
    if (atresponse && at_get_cme_error(atresponse) == CME_FDN_BLOCKED) {
        RIL_onRequestComplete(t, RIL_E_FDN_CHECK_FAILURE, NULL, 0);
    } else {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    }
    goto finally;
}

/**
 * RIL_REQUEST_QUERY_CALL_WAITING
 *
 * Query current call waiting state.
 */
void requestQueryCallWaiting(void *data, size_t datalen, RIL_Token t)
{
    char *cmd = NULL;
    char *line = NULL;
    int response[2] = { 0, 0 };
    int err;
    ATResponse *atresponse = NULL;
    ATLine *cursor = NULL;
    int class = ((int *) data)[0];

#ifdef USE_U8500_RIL
    if (0 == class) {
        class = SS_DEFAULT_CLASS; /* if not set, use default. */
    }
#endif

    /* AT+CCWA=[<n>[,<mode>[,<class>]]]  n=0 (default) mode=2 (query) */
    asprintf(&cmd, "AT+CCWA=0,2,%d", class);

    err = at_send_command_multiline(cmd, "+CCWA:", &atresponse);

    /* When <mode> =2 and command successful:
       +CCWA: <status>,<class1>[<CR><LF>+CCWA: <status>,<class2>[...]]  */

    if (err < 0 || atresponse->success == 0)
        goto error;

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
        if (err < 0)
            goto error;

        if (status == 1 && serviceClass > 0 && serviceClass <= 128)
            response[1] |= serviceClass;
    }

    if (response[1] > 0)
        response[0] = 1;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(int) * 2);

finally:
    free(cmd);
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
 * RIL_REQUEST_SET_CALL_WAITING
 *
 * Configure current call waiting state.
 */
void requestSetCallWaiting(void *data, size_t datalen, RIL_Token t)
{
    char *pCmd = NULL;
    int err;
    ATResponse *atresponse = NULL;
    const int mode = ((int *) data)[0];
    int class = ((int *) data)[1];

#ifdef USE_U8500_RIL
    if (0 == class) {
        class = SS_DEFAULT_CLASS; /* if not set, use default. */
    }
#endif

    /* AT+CCWA=[<n>[,<mode>[,<class>]]]  n=0 (default) */
    asprintf(&pCmd, "AT+CCWA=0,%d,%d", mode, class);

    err = at_send_command(pCmd, &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    at_response_free(atresponse);
    free(pCmd);
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION
 *
 * Enables/disables supplementary service related notifications
 * from the network.
 *
 * Notifications are reported via RIL_UNSOL_SUPP_SVC_NOTIFICATION.
 *
 * See also: RIL_UNSOL_SUPP_SVC_NOTIFICATION.
 */
void requestSetSuppSvcNotification(void *data, size_t datalen, RIL_Token t)
{
    int err;
    int ssn = ((int *) data)[0];
    char *cmd;

    assert(ssn == 0 || ssn == 1);

    asprintf(&cmd, "AT+CSSN=%d,%d", ssn, ssn);

    err = at_send_command(cmd, NULL);
    free(cmd);
    if (err < 0)
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    return;

  error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

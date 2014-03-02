/* ST-Ericsson U300 RIL
 *
 * Copyright (C) ST-Ericsson AB 2008-2010
 * Copyright 2006, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Based on reference-ril by The Android Open Source Project.
 *
 * Heavily modified for ST-Ericsson U300 modems.
 * Author: Christian Bejram <christian.bejram@stericsson.com>
 */

#include <cutils/properties.h>
#include <telephony/ril.h>
#ifdef USE_U8500_RIL
#include <stdbool.h>
#endif
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <alloca.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cutils/sockets.h>
#include <cutils/properties.h>
#include <termios.h>
#ifdef U300_RIL_CAIF
#include <linux/caif/caif_socket.h>
#endif

#include "atchannel.h"
#include "at_tok.h"
#include "misc.h"

#include "u300-ril.h"
#include "u300-ril-callhandling.h"
#include "u300-ril-config.h"
#include "u300-ril-messaging.h"
#include "u300-ril-network.h"
#include "u300-ril-pdp.h"
#include "u300-ril-services.h"
#include "u300-ril-sim.h"
#include "u300-ril-stk.h"
#include "u300-ril-oem.h"
#include "u300-ril-requestdatahandler.h"

#define LOG_TAG "RILV"
#include <utils/Log.h>

#define MAX_AT_RESPONSE 0x1000

#define timespec_cmp(a, b, op)			   \
	((a).tv_sec == (b).tv_sec		 \
	 ? (a).tv_nsec op(b).tv_nsec \
	 : (a).tv_sec op(b).tv_sec)

/*** Declarations ***/
static void onRequest(int request, void *data, size_t datalen,
                      RIL_Token t);
static int onSupports(int requestCode);
static void onCancel(RIL_Token t);
static const char *getVersion(void);
static int isRadioOn();
static void signalCloseQueues(void);
static bool shutdown_is_pending();
extern const char *requestToString(int request);

static void requestSTEHookRaw(uint8_t *data, size_t datalen, RIL_Token t);
static void sendUnsolOemHookRawWithTag(uint8_t *data, size_t datalen, uint8_t oem_tag);
static void requestModemPower(uint8_t *data, size_t datalen, RIL_Token t);

/*** Static Variables ***/
static const RIL_RadioFunctions s_callbacks = {
    RIL_VERSION,
    onRequest,
    currentState,
    onSupports,
    onCancel,
    getVersion
};

char ril_iface[MAX_IFNAME_LEN] = "";

#ifdef NO_AUTO_GPRS_ATTACH

typedef enum {
    U300_CS_STATE_UNKNOWN = 0,
    U300_CS_STATE_ATTACH,
} CS_State_t;

/* CS status  */
static CS_State_t sCSState = U300_CS_STATE_UNKNOWN;
#endif

static RIL_RadioState sState = RADIO_STATE_UNAVAILABLE;

static pthread_mutex_t s_state_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_screen_state_mutex = PTHREAD_MUTEX_INITIALIZER;

static int s_screenState = 1;

static int s_ttyMode = 0;


typedef struct RILRequest {
    int request;
    void *data;
    size_t datalen;
    RIL_Token token;
    struct RILRequest *next;
} RILRequest;

typedef struct RILEvent {
    void (*eventCallback) (void *param);
    void *param;
    struct timespec abstime;
    struct RILEvent *next;
    struct RILEvent *prev;
} RILEvent;

typedef struct RequestQueue {
    pthread_mutex_t queueMutex;
    pthread_cond_t cond;
    RILRequest *requestList;
    RILEvent *eventList;
    char enabled;
    char closed;
} RequestQueue;

static RequestQueue s_requestQueue = {
    .queueMutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .requestList = NULL,
    .eventList = NULL,
    .enabled = 1,
    .closed = 1
};

static RequestQueue s_requestQueuePrio = {
    .queueMutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
    .requestList = NULL,
    .eventList = NULL,
    .enabled = 0,
    .closed = 1
};

static RequestQueue *s_requestQueues[] = {
    &s_requestQueue,
    &s_requestQueuePrio
};

#define OEM_HOOK 0x00
#define STE_HOOK 0xff

typedef enum {
    STE_MODEM_SHUTDOWN = 0x0D1E, /* 1 byte payload, 0 = shutdown, 1 = reboot. */
} STE_Hook_t;

static const struct timeval TIMEVAL_0 = { 0, 0 };

/**
 * Enqueue a RILEvent to the request queue. isPrio specifies in what queue
 * the request will end up.
 *
 * 0 = the "normal" queue, 1 = prio queue and 2 = both. If only one queue
 * is present, then the event will be inserted into that queue.
 */
void enqueueRILEvent(int isPrio, void (*callback) (void *param),
                     void *param, const struct timeval *relativeTime)
{
    struct timeval tv;
    char done = 0;
    RequestQueue *q = NULL;

    RILEvent *e = malloc(sizeof(RILEvent));

    memset(e, 0, sizeof(RILEvent));

    e->eventCallback = callback;
    e->param = param;

    if (relativeTime == NULL) {
        relativeTime = alloca(sizeof(struct timeval));
        memset((struct timeval *) relativeTime, 0, sizeof(struct timeval));
    }

    gettimeofday(&tv, NULL);

    e->abstime.tv_sec = tv.tv_sec + relativeTime->tv_sec;
    e->abstime.tv_nsec = (tv.tv_usec + relativeTime->tv_usec) * 1000;

    if (e->abstime.tv_nsec > 1000000000) {
        e->abstime.tv_sec++;
        e->abstime.tv_nsec -= 1000000000;
    }

    if (!s_requestQueuePrio.enabled ||
        (isPrio == RIL_EVENT_QUEUE_NORMAL
         || isPrio == RIL_EVENT_QUEUE_ALL))
        q = &s_requestQueue;
    else if (isPrio == RIL_EVENT_QUEUE_PRIO)
        q = &s_requestQueuePrio;
    else
        q = &s_requestQueue;  /* Default */

  again:
    pthread_mutex_lock(&q->queueMutex);
    if (q->eventList == NULL)
        q->eventList = e;
    else {
        if (timespec_cmp(q->eventList->abstime, e->abstime, >)) {
            e->next = q->eventList;
            q->eventList->prev = e;
            q->eventList = e;
        } else {
            RILEvent *tmp = q->eventList;
            do {
                if (timespec_cmp(tmp->abstime, e->abstime, >)) {
                    tmp->prev->next = e;
                    e->prev = tmp->prev;
                    tmp->prev = e;
                    e->next = tmp;
                    break;
                } else if (tmp->next == NULL) {
                    tmp->next = e;
                    e->prev = tmp;
                    break;
                }
                tmp = tmp->next;
            } while (tmp);
        }
    }

    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->queueMutex);

    if (s_requestQueuePrio.enabled && isPrio == RIL_EVENT_QUEUE_ALL
        && !done) {
        RILEvent *e2 = malloc(sizeof(RILEvent));
        memcpy(e2, e, sizeof(RILEvent));
        e = e2;
        done = 1;
        q = &s_requestQueuePrio;

        goto again;
    }

    return;
}

/** Query property sys.shutdown.requested to see if a system shutdown is pending */
static bool shutdown_is_pending()
{
    bool result = false;
    char prop_value[PROPERTY_VALUE_MAX];
    int i = 0;

    memset(prop_value, 0, sizeof(prop_value));
    (void)property_get("sys.shutdown.requested", prop_value, NULL);

    /* in case of a pending shutdown, the system property *
     * "sys.shutdown.requested" is defined with reason    *
     * code '0', and '1' for reboot.                */
    result = (prop_value[0] == '0' || prop_value[0] == '1');

    ALOGD("shutdown_is_pending: %d", result);
    return result;
}

void sendUnsolOemHookRaw(uint8_t *data, size_t datalen) {
    sendUnsolOemHookRawWithTag(data, datalen, OEM_HOOK);
}

/** For STE specific responses, set oem_tag to STE_HOOK */
static void sendUnsolOemHookRawWithTag(uint8_t *data, size_t datalen, uint8_t oem_tag) {
    uint8_t *tmpdata;
    if (!data && datalen > 0) {
        ALOGE("%s: Invalid arguments! Not sending UNSOL_OEM_HOOK_RAW", __func__);
        return;
    }

    tmpdata = alloca(datalen + 1);
    tmpdata[0] = oem_tag;
    if (datalen > 0) {
        memcpy(&tmpdata[1], data, datalen);
    }

    RIL_onUnsolicitedResponse(RIL_UNSOL_OEM_HOOK_RAW, tmpdata, datalen + 1);

}

/** Process the STE specific OEM Raw hook */
static void requestSTEHookRaw(uint8_t *data, size_t datalen, RIL_Token t) {
    STE_Hook_t req;

    if (!data || datalen < 2) {
        /* The ID is missing */
        ALOGE("%s: Hook ID missing!", __func__);
        goto error;
    }

    req = (STE_Hook_t)(data[0] << 8 | (data[1] & 0xff));
    datalen -= 2;

    switch(req) {
    case STE_MODEM_SHUTDOWN:
        if (datalen > 0) {
            requestModemPower(&data[2], datalen, t);
        } else {
            ALOGE("%s: Got STE_MODEM_SHUTDOWN Hook with invalid length", __func__);
        }

        break;
    default:
        ALOGE("%s: Unknown STE command: %d", __func__, req);
        goto error;
    }

    goto exit;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
exit:
    return;
}

/** Do post-AT+CFUN=1 initialization. */
static void onRadioPowerOn()
{
    enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, pollSIMState, NULL, NULL);
}

/** Do post- SIM ready initialization. */
static void onSIMReady()
{
    ATResponse *atresponse = NULL;
    int err = 0;

    /* Select message service */
#ifndef SMS_PHASE_2_PLUS
    at_send_command("AT+CSMS=0", NULL);
#else
    at_send_command("AT+CSMS=1", NULL);
#endif

    /* Configure new messages indication
     *  mode = 2 - Buffer unsolicited result code in TA when TA-TE link is
     *             reserved(e.g. in on.line data mode) and flush them to the
     *             TE after reservation. Otherwise forward them directly to
     *             the TE.
     *  mt   = 2 - SMS-DELIVERs (except class 2 messages and messages in the
     *             message waiting indication group (store message)) are
     *             routed directly to TE using unsolicited result code:
     *             +CMT: [<alpha>],<length><CR><LF><pdu> (PDU mode)
     *             Class 2 messages are handled as if <mt> = 1
     *  bm   = 0 - No CBM indications are routed to the TE.
     *  ds   = 1 - SMS-STATUS-REPORTs are routed to the TE using unsolicited
     *             result code: +CDS: <length><CR><LF><pdu> (PDU mode)
     *  dfr  = 0 - TA buffer of unsolicited result codes defined within this
     *             command is flushed to the TE when <mode> 1...3 is entered
     *             (OK response is given before flushing the codes).
     */
#ifdef USE_U8500_RIL
    at_send_command("AT+CNMI=2,2,2,1,0", NULL);
#else
    at_send_command("AT+CNMI=2,2,0,1,0", NULL);
#endif
    /*
     * TODO: Remove line below when proper implementation for retrieving
     * cell broadcast message-id's from SIM is in place.
     */
    at_send_command("AT+CSCB=0,\"20,147,263,310,457,590,999\",\"0-255,0-255,0-255,0-255,0-255,0-255,0-255\"",NULL);

    /* Configure preferred message storage
     *   mem1 = ME, mem2 = ME, mem3 = ME
     */
    at_send_command("AT+CPMS=\"ME\",\"ME\",\"ME\"", NULL);

    /* Configure ST-Ericsson current PS bearer. */
    at_send_command("AT*EPSB=1", NULL);

    /* Subscribe to network registration events.
     *  n = 2 - Enable network registration and location information
     *          unsolicited result code *EREG: <stat>[,<lac>,<ci>]
     */
#ifndef USE_U8500_RIL
    err = at_send_command("AT*EREG=2", &atresponse);
    if (err < 0 || atresponse->success == 0)
        /* Some handsets -- in tethered mode -- don't support CREG=2. */
        at_send_command("AT*EREG=1", NULL);

    at_response_free(atresponse);
#endif

#ifdef USE_U8500_RIL
    /* Don't subscribe to Ericsson network registration events
     *  n = 0 - Disable network registration unsolicited result codes.
     */
    at_send_command("AT*EREG=0", NULL);
#endif

    /* Configure Packet Domain Network Registration Status events
     *    2 = Enable network registration and location information
     *        unsolicited result code
     */
    at_send_command("AT+CGREG=2", NULL);

    /* Subscribe to Call Waiting Notifications.
     *  n = 1 - Enable call waiting notifications
     */
    at_send_command("AT+CCWA=1", NULL);

#ifdef USE_U8500_RIL
    /* Configure mute control.
     *  n 0 - Mute off
     */
    at_send_command("AT+CMUT=0", NULL);
#endif

    /* Subscribe to Supplementary Services Notification
     *  n = 1 - Enable the +CSSI result code presentation status.
     *          Intermediaate result codes. When enabled and a supplementary
     *          service notification is received after a mobile originated
     *          call setup.
     *  m = 1 - Enable the +CSSU result code presentation status.
     *          Unsolicited result code. When a supplementary service
     *          notification is received during a mobile terminated call
     *          setup or during a call, or when a forward check supplementary
     *          service notification is received.
     */
    at_send_command("AT+CSSN=1,1", NULL);

    /* Subscribe to Packet Domain Event Reporting.
     *  mode = 1 - Discard unsolicited result codes when ME-TE link is reserved
     *             (e.g. in on-line data mode); otherwise forward them directly
     *             to the TE.
     *   bfr = 0 - MT buffer of unsolicited result codes defined within this
     *             command is cleared when <mode> 1 is entered.
     */
    at_send_command("AT+CGEREP=1,0", NULL);

    /* Configure Short Message (SMS) Format
     *  mode = 0 - PDU mode.
     */
    at_send_command("AT+CMGF=0", NULL);

    /* Set default SMS routing domain (CS primary, PS secondary) */
    at_send_command("AT+CGSMS=3", NULL);

    /* Subscribe to ST-Ericsson time zone/NITZ reporting.
     *
     */
    at_send_command("AT*ETZR=3", NULL);

    /* Subscribe to ST-Ericsson network reject cause reporting.
     *
     */
    at_send_command("AT*ECME=1", NULL);

    /* Subscribe to ST-Ericsson local tone generation indication.
     *
     */
    at_send_command("AT*ELAT=1", NULL);

#ifndef USE_U8500_RIL

    /* Subscribe to ST-Ericsson Call monitoring events.
     *  onoff = 1 - Call monitoring is on
     */
    at_send_command("AT*ECAM=1", NULL);
#endif

    /* Configure Mobile Equipment Event Reporting.
     *  mode = 3 - Forward unsolicited result codes directly to the TE;
     *             There is no inband technique used to embed result codes
     *             and data when TA is in on-line data mode.
     *  keyp = 0 - No keypad event reporting.
     *  disp = 0 - No display event reporting.
     *  ind  = 1 - Indicator event reporting using result code +CIEV: <ind>.
     */
    at_send_command("AT+CMER=3,0,0,1", NULL);

   /* Subscribe to ST-Ericsson NITZ name reporting.
    *
    */
    at_send_command("AT*ENNIR=1", NULL);
}

#ifdef NO_AUTO_GPRS_ATTACH

void doPSAttach(void *param)
{
    at_send_command("AT+CGATT=1", NULL);
}


void onCSNetStateChanged(const char *s)
{
    int err, state;
    char *tok;
    char *line = tok = strdup(s);
    CS_State_t oldCSState = sCSState;

    /*
     * Find out if we went to CS attach state. If so, send a CGATT=1 to make a PS attach
     */
    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &state);
    if (err < 0)
        goto error;

    if (U300_CS_STATE_UNKNOWN == oldCSState) {

        if ((state == 1) || (state == 5)) {
            sCSState = U300_CS_STATE_ATTACH;
            /* Do a AT+CGATT=1 */
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, doPSAttach, (void *) 1, NULL);
        }
    } else if (U300_CS_STATE_ATTACH == oldCSState) {
        if ((state == 0) || (state == 2) || (state == 3) || (state == 4)) {
            sCSState = U300_CS_STATE_UNKNOWN;
        }
    }
  finally:
    free(tok);
    return;

  error:
    ALOGE("ERROR in onCSNetStateChanged!");
    goto finally;
}
#endif

/**
 * RIL_REQUEST_GET_IMSI
 */
static void requestGetIMSI(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command_numeric("AT+CIMI", &atresponse);

    if (err < 0 || atresponse->success == 0)
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    else {
        RIL_onRequestComplete(t, RIL_E_SUCCESS,
                              atresponse->p_intermediates->line,
                              sizeof(char *));
    }
    at_response_free(atresponse);
    return;
}

/* RIL_REQUEST_DEVICE_IDENTITY
 *
 * Request the device ESN / MEID / IMEI / IMEISV.
 *
 */
static void requestDeviceIdentity(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    char *response[4];
    char *line = NULL;
    int svn;
    int err;

    response[0] = NULL;
    response[1] = NULL;
    response[2] = ""; /* CDMA not supported */
    response[3] = ""; /* CDMA not supported */

    /* IMEI */
    err = at_send_command_numeric("AT+CGSN", &atresponse);

    if (err < 0 || atresponse->success == 0)
        goto error;
    asprintf(&response[0], "%s", atresponse->p_intermediates->line);

    at_response_free(atresponse);
    atresponse = NULL;

    /* IMEISV */
    err = at_send_command_singleline("AT*ESVN?", "*ESVN:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto finally;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto finally;

    err = at_tok_nextint(&line, &svn);
    if (err == 0)
        asprintf(&response[1], "%02d", svn);

finally:
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &response, sizeof(response));
    goto exit;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

exit:
    at_response_free(atresponse);
    if (response[0] != NULL)
        free(response[0]);
    if (response[1] != NULL)
        free(response[1]);
    return;
}

/* Deprecated */
/**
 * RIL_REQUEST_GET_IMEI
 *
 * Get the device IMEI, including check digit.
 */
static void requestGetIMEI(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    int err;

    err = at_send_command_numeric("AT+CGSN", &atresponse);

    if (err < 0 || atresponse->success == 0)
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    else {
        RIL_onRequestComplete(t, RIL_E_SUCCESS,
                              atresponse->p_intermediates->line,
                              sizeof(char *));
    }
    at_response_free(atresponse);
    return;
}

/* Deprecated */
/**
 * RIL_REQUEST_GET_IMEISV
 *
 * Get the device IMEISV, which should be two decimal digits.
 */
static void requestGetIMEISV(void *data, size_t datalen, RIL_Token t)
{
    ATResponse *atresponse = NULL;
    char *response = NULL;
    char *line = NULL;
    int svn;
    int err;

    err = at_send_command_singleline("AT*ESVN?", "*ESVN:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &svn);
    if (err < 0 )
        goto error;

    asprintf(&response, "%02d", svn);
    RIL_onRequestComplete(t, RIL_E_SUCCESS, response, sizeof(char *));
    goto exit;

error:
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

exit:
    at_response_free(atresponse);
    if (response != NULL)
        free(response);
    return;
}

/**
 * RIL_REQUEST_RADIO_POWER
 *
 * Toggle radio on and off (for "airplane" mode).
 * Note: Modem power down during system shutdown/reboot is
 *       handled in requestModemPower
 */
static void requestRadioPower(void *data, size_t datalen, RIL_Token t)
{
    int onOff;
    int err;
    ATResponse *atresponse = NULL;

    assert(datalen >= sizeof(int *));
    onOff = ((int *) data)[0];

    if (onOff == 0) {
        /* in case of pending system shutdown, prepare modem for shutdown
         * Note: This should only be the case if the radio was not powered down
         * by requestModemPower
         */
        if (shutdown_is_pending()) {
            err = at_send_command("AT+CFUN=100", &atresponse);
            if (err < 0 || atresponse->success == 0)
                   goto error;
            setRadioState(RADIO_STATE_OFF);
        } else if (sState != RADIO_STATE_OFF) { /* perform RF off but keep modem alive */
            err = at_send_command("AT+CFUN=4", &atresponse);
            if (err < 0 || atresponse->success == 0)
                goto error;
            setRadioState(RADIO_STATE_OFF);
        }
    } else if (onOff > 0 && sState == RADIO_STATE_OFF) {
        err = at_send_command("AT+CFUN=99", &atresponse);
        if (err < 0 || atresponse->success == 0)
            goto error;
        setRadioState(RADIO_STATE_SIM_NOT_READY);
    } else {
        ALOGE("Erroneous input to requestRadioPower()!");
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
 * STE_MODEM_SHUTDOWN
 *
 * Power down the modem. This request is only called during
 * system shutdown/reboot.
 */
static void requestModemPower(uint8_t *data, size_t datalen, RIL_Token t) {

    int err;
    ATResponse *atresponse = NULL;

    if (!data || datalen < 1) {
        ALOGE("%s Arguments missing for STE_MODEM_SHUTDOWN request", __func__);
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    ALOGI("%s Powering down modem due to system %s",
                        __func__, (data[0] == 1) ? "reboot" : "shutdown");

    /* in case of pending system shutdown, prepare modem for shutdown */
    err = at_send_command("AT+CFUN=100", &atresponse);
    if (err < 0 || atresponse->success == 0) {
     ALOGE("%s Failed to power down modem!", __func__);
    }

    /* Send a successful response regardless */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

    /*
     * We don't change the radio state here, since that only confuses Android
     * since the telephony framework is not aware of the ongoing shutdown.
     * And besides, we will be shutdown momentarily..
     */

    at_response_free(atresponse);
    return;
}

/**
 * Will LOCK THE MUTEX! MAKE SURE TO RELEASE IT!
 */
void getScreenStateLock(void)
{
    /* Just make sure we're not changing anything with regards to screen state. */
    pthread_mutex_lock(&s_screen_state_mutex);
}

int getScreenState(void)
{
    return s_screenState;
}

void releaseScreenStateLock(void)
{
    pthread_mutex_unlock(&s_screen_state_mutex);
}

static void requestScreenState(void *data, size_t datalen, RIL_Token t)
{
    int err, screenState;

    assert(datalen >= sizeof(int *));

    pthread_mutex_lock(&s_screen_state_mutex);
    screenState = s_screenState = ((int *) data)[0];

    if (screenState == 1) {
        /* Screen is on - be sure to enable all unsolicited notifications again. */
#ifdef USE_U8500_RIL
        err = at_send_command("AT+CREG=2", NULL);
#else
        err = at_send_command("AT*EREG=2", NULL);
#endif
        if (err < 0)
            goto error;
        err = at_send_command("AT+CGREG=2", NULL);
        if (err < 0)
            goto error;
#ifndef USE_U8500_RIL
        err = at_send_command("AT+CGEREP=1,0", NULL);
        if (err < 0)
            goto error;
#endif
        err = at_send_command("AT*EPSB=1", NULL);
        if (err < 0)
            goto error;
        err = at_send_command("AT+CMER=3,0,0,1", NULL);
        if (err < 0)
            goto error;

       err = at_send_command("AT*ENNIR=1", NULL);
        if (err < 0) {
            goto error;
        }
    } else if (screenState == 0) {
        /* Screen is off - disable all unsolicited notifications. */
        err = at_send_command("AT*EREG=0", NULL);
        if (err < 0)
            goto error;
#ifdef USE_U8500_RIL
        err = at_send_command("AT+CREG=1", NULL);
        if (err < 0)
            goto error;
        err = at_send_command("AT+CGREG=1", NULL);
        if (err < 0)
            goto error;
#else
        err = at_send_command("AT+CGREG=0", NULL);
        if (err < 0)
            goto error;
#endif
#ifndef USE_U8500_RIL
        err = at_send_command("AT+CGEREP=0,0", NULL);
        if (err < 0)
            goto error;
#endif
        /* FIXME If this is causing unnessecary wakeups, add again.
           err = at_send_command("AT*EPSB=0", NULL);
           if (err < 0)
               goto error; */
        err = at_send_command("AT+CMER=3,0,0,0", NULL);
        if (err < 0)
            goto error;

        err = at_send_command("AT*ENNIR=0", NULL);
        if (err < 0) {
            goto error;
        }
    } else
        /* Not a defined value - error. */
        goto error;

    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);

  finally:
    pthread_mutex_unlock(&s_screen_state_mutex);
    return;

  error:
    ALOGE("ERROR: requestScreenState failed");
    if (t != (RIL_Token) 0)
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

    goto finally;
}

/**
 * RIL_REQUEST_BASEBAND_VERSION
 *
 * Return string value indicating baseband version, eg
 * response from AT+CGMR.
 */
static void requestBasebandVersion(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse = NULL;
    char *line = NULL;
    char *temp_line = NULL;

    //err = at_send_command_singleline("AT+CGMR", "\0", &atresponse);
    err = at_send_command_singleline("AT+RADIOVER?", "+RADIOVER:", &atresponse);

    if (err < 0 ||
        atresponse->success == 0 || atresponse->p_intermediates == NULL)
        goto error;

    temp_line = atresponse->p_intermediates->line;
    
    err = at_tok_start(&temp_line);
    if (err < 0)
        goto error;
    


    err = at_tok_nextstr(&temp_line, &line);
    if (err < 0)
        goto error;

    /* The returned value is used by Android in a system property.          */
    /* The RIL should have no knowledge about this, but since Android       */
    /* system properties only allow values with length < 90 and causes an   */
    /* exception if the length of the returned string is > 90 this needs to */
    /* be checked here.                                                     */
    /* Todo:  Until Android implements limit handling on the string we need */
    /* to have a workaround in the RIL to chop the string.                  */
    if (strlen(line) > 90)
        *(line+90) = '\0';

    RIL_onRequestComplete(t, RIL_E_SUCCESS, line, sizeof(char *));

  finally:
    at_response_free(atresponse);
    return;

  error:
    ALOGE("Error in requestBasebandVersion()");
    RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
    goto finally;
}

/**
 * RIL_REQUEST_SET_TTY_MODE
 *
 * Return response value from baseband
 */
static void requestSetTtyMode(void *data, size_t datalen, RIL_Token t)
{
    int err;
    ATResponse *atresponse = NULL;
    char cmd[12];
    int mode = ((int *) data)[0];

    s_ttyMode = mode;

    sprintf(cmd, "AT*ETTY=%d", (mode ? 1 : 0));
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
 * RIL_REQUEST_QUERY_TTY_MODE
 *
 * Requests the current TTY mode.
 */
static void requestQueryTtyMode(void *data, size_t datalen, RIL_Token t)
{
    RIL_onRequestComplete(t, RIL_E_SUCCESS, &s_ttyMode, sizeof(int));
}

#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
unsigned int getHexByte(const unsigned char *ec)
{
    char hexbyte[3] = {0};
    unsigned int temp;
    hexbyte[0] = ec[0];
    hexbyte[1] = ec[1];
    sscanf(hexbyte,"%x",&temp);

    return temp;

}

static char isEventDnDataAvailable(const unsigned char *ec)
{
    const unsigned char RIL_STK_EVENT_DOWNLOAD_TAG = 0xD6;
    const unsigned char  RIL_STK_EVENT_LIST_TAG = 0x19;
    const unsigned char RIL_STK_EVENT_DATA_AVAILABLE = 0x09;
    const unsigned char  RIL_STK_TWO_BYTES_LENGTH_IND = 0x81;
    const unsigned char  RIL_STK_COMPREHENSION_REQUIRED_MASK = 0x80;
    unsigned int temp;

    if (!ec)
        return 0;

    temp = getHexByte(ec);
    if (temp != RIL_STK_EVENT_DOWNLOAD_TAG)
        return 0;

    ec+=2;
    temp = getHexByte(ec);
    if (temp == RIL_STK_TWO_BYTES_LENGTH_IND){
        ec+=2;
    }

    ec+=2;
    temp = getHexByte(ec);
    if ((temp & ~RIL_STK_COMPREHENSION_REQUIRED_MASK) != RIL_STK_EVENT_LIST_TAG)
        return 0;

    ec+=4;
    temp = getHexByte(ec);
    if (temp == RIL_STK_EVENT_DATA_AVAILABLE)
        return 1;
    else
        return 0;
}
#endif

static char isPrioRequest(int request)
{
    unsigned int i;

    for (i = 0; i < sizeof(prioRequests) / sizeof(int); i++)
        if (request == prioRequests[i])
            return 1;
    return 0;
}

static void processRequest(int request, void *data, size_t datalen,
                           RIL_Token t)
{
    ALOGI("processRequest: %s", requestToString(request));

    /* Ignore all requests except RIL_REQUEST_GET_SIM_STATUS
     * when RADIO_STATE_UNAVAILABLE.
     */
    if (sState == RADIO_STATE_UNAVAILABLE
        && request != RIL_REQUEST_GET_SIM_STATUS) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    /*
     * These commands won't accept RADIO_NOT_AVAILABLE, so we just return
     * GENERIC_FAILURE if we're not in SIM_STATE_READY.
     */
    if (sState != RADIO_STATE_SIM_READY
        && (request == RIL_REQUEST_WRITE_SMS_TO_SIM ||
            request == RIL_REQUEST_DELETE_SMS_ON_SIM ||
            request == RIL_REQUEST_SCREEN_STATE)) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    /* Ignore all non-power requests when RADIO_STATE_OFF
     * and RADIO_STATE_SIM_NOT_READY (except RIL_REQUEST_RADIO_POWER
     * and a few more).
     */
    if ((sState == RADIO_STATE_OFF || sState == RADIO_STATE_SIM_NOT_READY)
        && !(request == RIL_REQUEST_RADIO_POWER ||
             request == RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING ||
             request == RIL_REQUEST_GET_SIM_STATUS ||
             request == RIL_REQUEST_GET_IMEISV ||
             request == RIL_REQUEST_GET_IMEI ||
             request == RIL_REQUEST_DEVICE_IDENTITY ||
             request == RIL_REQUEST_BASEBAND_VERSION ||
             request == RIL_REQUEST_GET_CURRENT_CALLS ||
             request == RIL_REQUEST_OEM_HOOK_RAW ||
             request == RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE ||
             request == RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND ||
             request == RIL_REQUEST_SIM_IO ||
             request == RIL_REQUEST_ENTER_SIM_PIN ||
             request == RIL_REQUEST_ENTER_SIM_PUK ||
             request == RIL_REQUEST_ENTER_SIM_PIN2 ||
             request == RIL_REQUEST_ENTER_SIM_PUK2 ||
             request == RIL_REQUEST_CHANGE_SIM_PIN ||
             request == RIL_REQUEST_CHANGE_SIM_PIN2 ||
             request == RIL_REQUEST_QUERY_FACILITY_LOCK ||
             request == RIL_REQUEST_SET_FACILITY_LOCK ||
             request == RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION ||
             request == RIL_REQUEST_QUERY_TTY_MODE ||
             request == RIL_REQUEST_SET_TTY_MODE)) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    /* Don't allow radio operations when sim is absent or locked, with
     * the exception of those that must succeed for the Android telephony
     * frameworks polling of radio states.
     * DIAL, GET_CURRENT_CALLS and HANGUP are required to handle
     * emergency calls
     */
    if (sState == RADIO_STATE_SIM_LOCKED_OR_ABSENT
        && !(request == RIL_REQUEST_ENTER_SIM_PIN ||
             request == RIL_REQUEST_ENTER_SIM_PUK ||
             request == RIL_REQUEST_ENTER_SIM_PIN2 ||
             request == RIL_REQUEST_ENTER_SIM_PUK2 ||
             request == RIL_REQUEST_CHANGE_SIM_PIN ||
             request == RIL_REQUEST_CHANGE_SIM_PIN2 ||
             request == RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION ||
             request == RIL_REQUEST_GET_SIM_STATUS ||
             request == RIL_REQUEST_RADIO_POWER ||
             request == RIL_REQUEST_GET_IMEISV ||
             request == RIL_REQUEST_GET_IMEI ||
             request == RIL_REQUEST_BASEBAND_VERSION ||
             request == RIL_REQUEST_DIAL ||
             request == RIL_REQUEST_GET_CURRENT_CALLS ||
             request == RIL_REQUEST_HANGUP ||
             request == RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND ||
#ifdef USE_U8500_RIL
             request == RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE ||
             request == RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND ||
#endif
             request == RIL_REQUEST_VOICE_REGISTRATION_STATE || /* must succeed */
             request == RIL_REQUEST_DATA_REGISTRATION_STATE || /* must succeed */
             request == RIL_REQUEST_OPERATOR || /* must succeed */
             request == RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE || /* must succeed */
             request == RIL_REQUEST_QUERY_FACILITY_LOCK ||
             request == RIL_REQUEST_SIM_IO ||
             request == RIL_REQUEST_DTMF_START ||
             request == RIL_REQUEST_DTMF_STOP ||
             request == RIL_REQUEST_DTMF ||
             request == RIL_REQUEST_DATA_CALL_LIST ||
             request == RIL_REQUEST_OEM_HOOK_RAW ||
             request == RIL_REQUEST_QUERY_TTY_MODE ||
             request == RIL_REQUEST_SET_TTY_MODE)) {
        RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        return;
    }

    switch (request) {

        /* Basic Voice Call */
    case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
        requestLastCallFailCause(data, datalen, t);
        break;
    case RIL_REQUEST_GET_CURRENT_CALLS:
        requestGetCurrentCalls(data, datalen, t);
        break;
    case RIL_REQUEST_DIAL:
        requestDial(data, datalen, t);
        break;
    case RIL_REQUEST_HANGUP:
        requestHangup(data, datalen, t);
        break;
    case RIL_REQUEST_ANSWER:
        requestAnswer(data, datalen, t);
        break;

        /* Advanced Voice Call */
    case RIL_REQUEST_GET_CLIR:
        requestGetCLIR(data, datalen, t);
        break;
    case RIL_REQUEST_SET_CLIR:
        requestSetCLIR(data, datalen, t);
        break;
    case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS:
        requestQueryCallForwardStatus(data, datalen, t);
        break;
    case RIL_REQUEST_SET_CALL_FORWARD:
        requestSetCallForward(data, datalen, t);
        break;
    case RIL_REQUEST_QUERY_CALL_WAITING:
        requestQueryCallWaiting(data, datalen, t);
        break;
    case RIL_REQUEST_SET_CALL_WAITING:
        requestSetCallWaiting(data, datalen, t);
        break;
    case RIL_REQUEST_UDUB:
        requestUDUB(data, datalen, t);
        break;
    case RIL_REQUEST_GET_MUTE:
        requestGetMute(data, datalen, t);
        break;
    case RIL_REQUEST_SET_MUTE:
        requestSetMute(data, datalen, t);
        break;
    case RIL_REQUEST_SCREEN_STATE:
        requestScreenState(data, datalen, t);
        /* Trigger a rehash of network values, just to be sure. */
        if (((int *) data)[0] == 1)
            RIL_onUnsolicitedResponse
                (RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0);
        break;
    case RIL_REQUEST_QUERY_CLIP:
        requestQueryClip(data, datalen, t);
        break;
    case RIL_REQUEST_DTMF:
        requestDTMF(data, datalen, t);
        break;
    case RIL_REQUEST_DTMF_START:
        requestDTMFStart(data, datalen, t);
        break;
    case RIL_REQUEST_DTMF_STOP:
        requestDTMFStop(data, datalen, t);
        break;

        /* Multiparty Voice Call */
    case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND:
        requestHangupWaitingOrBackground(data, datalen, t);
        break;
    case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
        requestHangupForegroundResumeBackground(data, datalen, t);
        break;
    case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
        requestSwitchWaitingOrHoldingAndActive(data, datalen, t);
        break;
    case RIL_REQUEST_CONFERENCE:
        requestConference(data, datalen, t);
        break;
    case RIL_REQUEST_SEPARATE_CONNECTION:
        requestSeparateConnection(data, datalen, t);
        break;
    case RIL_REQUEST_EXPLICIT_CALL_TRANSFER:
        requestExplicitCallTransfer(data, datalen, t);
        break;

        /* Data Call Requests */
    case RIL_REQUEST_SETUP_DATA_CALL:
        requestSetupDataCall(data, datalen, t);
        break;
    case RIL_REQUEST_DEACTIVATE_DATA_CALL:
        requestDeactivateDataCall(data, datalen, t);
        break;
    case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE:
        requestLastPDPFailCause(data, datalen, t);
        break;
    case RIL_REQUEST_DATA_CALL_LIST:
        requestPDPContextList(data, datalen, t);
        break;

        /* SMS Requests */
    case RIL_REQUEST_SEND_SMS:
        requestSendSMS(data, datalen, t);
        break;
    case RIL_REQUEST_SEND_SMS_EXPECT_MORE:
        requestSendSMSExpectMore(data, datalen, t);
        break;
    case RIL_REQUEST_WRITE_SMS_TO_SIM:
        requestWriteSmsToSim(data, datalen, t);
        break;
    case RIL_REQUEST_DELETE_SMS_ON_SIM:
        requestDeleteSmsOnSim(data, datalen, t);
        break;
    case RIL_REQUEST_GET_SMSC_ADDRESS:
        requestGetSMSCAddress(data, datalen, t);
        break;
    case RIL_REQUEST_SET_SMSC_ADDRESS:
        requestSetSMSCAddress(data, datalen, t);
        break;
    case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS:
        requestSmsStorageFull(data, datalen, t);
        break;
    case RIL_REQUEST_SMS_ACKNOWLEDGE:
        requestSMSAcknowledge(data, datalen, t);
        break;
    case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG:
        requestGSMGetBroadcastSMSConfig(data, datalen, t);
        break;
    case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG:
        requestGSMSetBroadcastSMSConfig(data, datalen, t);
        break;
    case RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION:
        requestGSMSMSBroadcastActivation(data, datalen, t);
        break;

        /* SIM Handling Requests */
    case RIL_REQUEST_SIM_IO:
        requestSIM_IO(data, datalen, t);
        break;
    case RIL_REQUEST_GET_SIM_STATUS:
        requestGetSimStatus(data, datalen, t);
        break;
    case RIL_REQUEST_ENTER_SIM_PIN:
    case RIL_REQUEST_ENTER_SIM_PUK:
    case RIL_REQUEST_ENTER_SIM_PIN2:
    case RIL_REQUEST_ENTER_SIM_PUK2:
        requestEnterSimPin(data, datalen, t, request);
        break;
    case RIL_REQUEST_CHANGE_SIM_PIN:
        requestChangePassword("SC", data, datalen, t, request);
        break;
    case RIL_REQUEST_CHANGE_SIM_PIN2:
        requestChangePassword("P2", data, datalen, t, request);
        break;
    case RIL_REQUEST_CHANGE_BARRING_PASSWORD:
        requestChangePassword(((char **) data)[0],
                              ((char *) data) + sizeof(char *),
                              datalen - sizeof(char *), t, request);
        break;
    case RIL_REQUEST_QUERY_FACILITY_LOCK:
        requestQueryFacilityLock(data, datalen, t);
        break;
    case RIL_REQUEST_SET_FACILITY_LOCK:
        requestSetFacilityLock(data, datalen, t);
        break;

        /* USSD Requests */
    case RIL_REQUEST_SEND_USSD:
        requestSendUSSD(data, datalen, t);
        break;
    case RIL_REQUEST_CANCEL_USSD:
        requestCancelUSSD(data, datalen, t);
        break;

        /* Network Selection */
    case RIL_REQUEST_SET_BAND_MODE:
        requestSetBandMode(data, datalen, t);
        break;
    case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE:
        requestQueryAvailableBandMode(data, datalen, t);
        break;
    case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION:
        requestEnterNetworkDepersonalization(data, datalen, t);
        break;
    case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE:
        requestQueryNetworkSelectionMode(data, datalen, t);
        break;
    case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC:
        requestSetNetworkSelectionAutomatic(data, datalen, t);
        break;
    case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL:
        requestSetNetworkSelectionManual(data, datalen, t);
        break;
    case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS:
        requestQueryAvailableNetworks(data, datalen, t);
        break;
    case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
        requestSetPreferredNetworkType(data, datalen, t);
        break;
    case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
        requestGetPreferredNetworkType(data, datalen, t);
        break;
    case RIL_REQUEST_VOICE_REGISTRATION_STATE:
    case RIL_REQUEST_DATA_REGISTRATION_STATE:
        requestRegistrationState(request, data, datalen, t);
        break;
    case RIL_REQUEST_SET_LOCATION_UPDATES:
        requestSetLocationUpdates(data, datalen, t);
        break;

        /* OEM */
    case RIL_REQUEST_OEM_HOOK_RAW:
        if (data && datalen > 0) {
            uint8_t* rawdata = (uint8_t*)data;
            uint8_t oem_tag = rawdata[0];
            datalen -= 1;

            if (datalen == 0) {
                rawdata = NULL;
            } else {
                rawdata++;
            }

            if (oem_tag == STE_HOOK) {
                requestSTEHookRaw(rawdata, datalen, t);
            } else {
                requestOEMHookRaw((void*)(rawdata), datalen, t);
            }
        } else {
            /* The request did not contain an OEM tag. */
            RIL_onRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
        }
        break;
    case RIL_REQUEST_OEM_HOOK_STRINGS:
        requestOEMHookStrings(data, datalen, t);
        break;

        /* Misc */
    case RIL_REQUEST_SIGNAL_STRENGTH:
        requestSignalStrength(data, datalen, t);
        break;
    case RIL_REQUEST_OPERATOR:
        requestOperator(data, datalen, t);
        break;
    case RIL_REQUEST_RADIO_POWER:
        requestRadioPower(data, datalen, t);
        break;
    case RIL_REQUEST_GET_IMSI:
        requestGetIMSI(data, datalen, t);
        break;
    case RIL_REQUEST_GET_IMEI: /* Deprecated */
        requestGetIMEI(data, datalen, t);
        break;
    case RIL_REQUEST_GET_IMEISV:   /* Deprecated */
        requestGetIMEISV(data, datalen, t);
        break;
    case RIL_REQUEST_DEVICE_IDENTITY:
        requestDeviceIdentity(data, datalen, t);
        break;
    case RIL_REQUEST_BASEBAND_VERSION:
        requestBasebandVersion(data, datalen, t);
        break;
    case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION:
        requestSetSuppSvcNotification(data, datalen, t);
        break;

        /* SIM Application Toolkit */
    case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
        requestStkSendTerminalResponse(data, datalen, t);
        break;
    case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
        requestStkSendEnvelopeCommand(data, datalen, t);
        break;
    case RIL_REQUEST_STK_GET_PROFILE:
        requestStkGetProfile(data, datalen, t);
        break;
    case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
        requestStkIsRunning(data, datalen, t);
        break;
    case RIL_REQUEST_STK_SET_PROFILE:
        requestStkSetProfile(data, datalen, t);
        break;
    case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM:
        requestStkHandleCallSetupRequestedFromSIM(data, datalen, t);
        break;

        /* Network neighbours */
    case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS:
        requestNeighbouringCellIDs(data, datalen, t);
        break;

        /* TTY mode */
    case RIL_REQUEST_SET_TTY_MODE:
        requestSetTtyMode(data, datalen, t);
        break;
    case RIL_REQUEST_QUERY_TTY_MODE:
        requestQueryTtyMode(data, datalen, t);
        break;
#if RIL_VERSION >= 6
    case RIL_REQUEST_ISIM_AUTHENTICATION:
        requestISIMAuthentication(data, datalen, t);
        break;
#endif
    default:
        ALOGW("FIXME: Unsupported request logged: %s",
             requestToString(request));
        RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
        break;
    }
}

/*** Callback methods from the RIL library to us ***/

/**
 * Call from RIL to us to make a RIL_REQUEST.
 *
 * Must be completed with a call to RIL_onRequestComplete().
 */
static void onRequest(int request, void *data, size_t datalen, RIL_Token t)
{
    RILRequest *r;
    RequestQueue *q = &s_requestQueue;

    if (s_requestQueuePrio.enabled && isPrioRequest(request))
        q = &s_requestQueuePrio;

#ifdef SAT_ENV_NO_INTERLEAVING_SUPPORT
    if (request == RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND && isEventDnDataAvailable((const unsigned char *)data)) {
        ALOGI("onRequest()- received Data download data available ec ...");
        q = &s_requestQueue;
    }
#endif

    pthread_mutex_lock(&q->queueMutex);

    /*
     * If the queue is closed, don't allow any requests to enter it
     * from higher layers, with two exceptions;
     *
     *      RIL_REQUEST_GET_SIM_STATUS
     *          This request must never fail. Allow it to enter the
     *          queue.
     *
     *      RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING
     *          This request is only ever emitted once by the Android
     *          framework. If it is rejected the SIM toolkit will not
     *          be available. Allow it to enter the queue.
     */
    if (q->closed != 0) {
        switch (request) {
        case RIL_REQUEST_GET_SIM_STATUS:
        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
            break;
        default:
            RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
            pthread_mutex_unlock(&q->queueMutex);
            return;
        }
    }

    r = malloc(sizeof(RILRequest));
    memset(r, 0, sizeof(RILRequest));

    /* Formulate a RILRequest and put it in the queue. */
    r->request = request;
    r->data = dupRequestData(request, data, datalen);
    r->datalen = datalen;
    r->token = t;

    /* Queue empty, just throw r on top. */
    if (q->requestList == NULL)
        q->requestList = r;
    else {
        RILRequest *l = q->requestList;
        while (l->next != NULL)
            l = l->next;

        l->next = r;
    }

    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->queueMutex);
}

/**
 * Synchronous call from the RIL to us to return current radio state.
 * RADIO_STATE_UNAVAILABLE should be the initial state.
 */
RIL_RadioState currentState()
{
    return sState;
}

/**
 * Call from RIL to us to find out whether a specific request code
 * is supported by this implementation.
 *
 * Return 1 for "supported" and 0 for "unsupported".
 *
 * Currently just stubbed with the default value of one. This is currently
 * not used by android, and therefore not implemented here. We return
 * RIL_E_REQUEST_NOT_SUPPORTED when we encounter unsupported requests.
 */
static int onSupports(int requestCode)
{
    ALOGI("onSupports() called!");

    return 1;
}

/**
 * onCancel() is currently stubbed, because android doesn't use it and
 * our implementation will depend on how a cancellation is handled in
 * the upper layers.
 */
static void onCancel(RIL_Token t)
{
    ALOGI("onCancel() called!");
}

static const char *getVersion(void)
{
    return RIL_VERSION_STRING;
}

const char * radioStateToString(RIL_RadioState radioState)
{
    const char *state;

    switch (radioState)
    {
    case RADIO_STATE_OFF:
        state = "RADIO_STATE_OFF";
        break;
    case RADIO_STATE_UNAVAILABLE:
        state = "RADIO_STATE_UNAVAILABLE";
        break;
    case RADIO_STATE_SIM_NOT_READY:
        state = "RADIO_STATE_SIM_NOT_READY";
        break;
    case RADIO_STATE_SIM_LOCKED_OR_ABSENT:
        state = "RADIO_STATE_SIM_LOCKED_OR_ABSENT";
        break;
    case RADIO_STATE_SIM_READY:
        state = "RADIO_STATE_SIM_READY";
        break;
    case RADIO_STATE_RUIM_NOT_READY:
        state = "RADIO_STATE_RUIM_NOT_READY";
        break;
    case RADIO_STATE_RUIM_LOCKED_OR_ABSENT:
        state = "RADIO_STATE_RUIM_READY";
        break;
    case RADIO_STATE_NV_NOT_READY:
        state = "RADIO_STATE_NV_NOT_READY";
        break;
    case RADIO_STATE_NV_READY:
        state = "RADIO_STATE_NV_READY";
        break;
    default:
        state = "RADIO_STATE_<> Unknown!";
        break;
    }
    return state;
}

void setRadioState(RIL_RadioState newState)
{
    RIL_RadioState oldState;

    pthread_mutex_lock(&s_state_mutex);

    oldState = sState;

    ALOGI("%s: oldState=%s newState=%s", __func__, radioStateToString(oldState), radioStateToString(newState));

    if (sState != newState) {
        sState = newState;
        if (sState == RADIO_STATE_UNAVAILABLE || sState == RADIO_STATE_OFF) {
            /*
             * Trigger an update of the PDP context list, in case of, i.e.
             * modem restart, RIL may have lingering PDP contexts in the list
             * that are no longer valid.
             */
            enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, onPDPContextListChanged,
                            NULL, NULL);
        }
    }

    pthread_mutex_unlock(&s_state_mutex);

    /* Do these outside of the mutex. */
    if (sState != oldState || sState == RADIO_STATE_SIM_LOCKED_OR_ABSENT) {
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED,
                                  NULL, 0);

        switch (sState) {
        case RADIO_STATE_SIM_NOT_READY:
            enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, onRadioPowerOn, NULL, NULL);
            break;
        case RADIO_STATE_SIM_LOCKED_OR_ABSENT:
            /*
             * Fetch emergency call code list from EF_ECC (always readable).
             * It needs to be read even if PIN is active.
             */
            ALOGI("[ECC] setRadioState RADIO_STATE_SIM_LOCKED_OR_ABSENT");
            setupECCList(0);
            break;
        case RADIO_STATE_SIM_READY:
            enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, onSIMReady, NULL, NULL);
            break;
        default:
            break;
        }
    }
}

/** Returns 1 if on, 0 if off, and -1 on error. */
static int isRadioOn()
{
    ATResponse *atresponse = NULL;
    int err;
    char *line;
    int ret;

    err = at_send_command_singleline("AT+CFUN?", "+CFUN:", &atresponse);
    if (err < 0 || atresponse->success == 0)
        /* Assume radio is off. */
        goto error;

    line = atresponse->p_intermediates->line;

    err = at_tok_start(&line);
    if (err < 0)
        goto error;

    err = at_tok_nextint(&line, &ret);
    if (err < 0)
        goto error;

    switch (ret) {
    case 5:                    /* GSM only */
    case 6:                    /* WCDMA only */
    case 7:                    /* Preferred WCDMA */
        ret = 1;
        break;

    default:
        ret = 0;
    }

    at_response_free(atresponse);

    return ret;

  error:
    at_response_free(atresponse);
    return -1;
}

static char initializeCommon(void)
{
    int err = 0;

#ifndef USE_U8500_RIL
    ALOGI("initializeCommon()");
#endif

    if (at_handshake() < 0) {
        LOG_FATAL("Handshake failed!");
        goto error;
    }

    /* Configure/set
     *   command echo (E), result code suppression (Q), DCE response format (V)
     *
     *  E0 = DCE does not echo characters during command state and online
     *       command state
     *  Q0 = DCE transmits result codes
     *  V1 = Display verbose result codes
     */
    err = at_send_command("ATE0Q0V1", NULL);
    if (err < 0)
        goto error;

    /* Set default character set. */
    err = at_send_command("AT+CSCS=\"UTF-8\"", NULL);
    if (err < 0)
        goto error;

    /* Disable automatic answer. */
    err = at_send_command("ATS0=0", NULL);
    if (err < 0)
        goto error;

    /* Enable +CME ERROR: <err> result code and use numeric <err> values. */
    err = at_send_command("AT+CMEE=1", NULL);
    if (err < 0)
        goto error;

    /* Enable Connected Line Identification Presentation. */
    err = at_send_command("AT+COLP=0", NULL);
    if (err < 0)
        goto error;

    /* Disable Service Reporting. */
    err = at_send_command("AT+CR=0", NULL);
    if (err < 0)
        goto error;

    /* Configure carrier detect signal - 1 = DCD follows the connection. */
    err = at_send_command("AT&C=1", NULL);
    if (err < 0)
        goto error;

    /* Configure DCE response to Data Termnal Ready signal - 0 = ignore. */
    err = at_send_command("AT&D=0", NULL);
    if (err < 0)
        goto error;

    /* Configure Cellular Result Codes - 0 = Disables extended format. */
    err = at_send_command("AT+CRC=0", NULL);
    if (err < 0)
        goto error;

    /* Configure Bearer Service Type and HSCSD Non-Transparent Call
     *  +CBST
     *     7 = 9600 bps V.32
     *     0 = Asynchronous connection
     *     1 = Non-transparent connection element
     *  +CHSN
     *     1 = Wanted air interface user rate is 9,6 kbits/s
     *     1 = Wanted number of receive timeslots is 1
     *     0 = Indicates that the user is not going to change <wAiur> and /or
     *         <wRx> during the next call
     *     4 = Indicates that the accepted channel coding for the next
     *         established non-transparent HSCSD call is 9,6 kbit/s only
     */
    err = at_send_command("AT+CBST=7,0,1", NULL);
    if (err < 0)
        goto error;

    err = at_send_command("AT+CHSN=1,1,0,4", NULL);
    if (err < 0)
        goto error;

    /* Configure Call progress Monitoring
     *    3 = BUSY result code given if called line is busy.
     *        No NO DIALTONE result code is given.
     *        Reports line speed together with CONNECT result code.
     */
    err = at_send_command("ATX3", NULL);
    if (err < 0)
        goto error;

    return 0;
  error:
    return 1;
}

/**
 * Initialize everything that can be configured while we're still in
 * AT+CFUN=0.
 */
#ifdef DUAL_CHANNELS
static char initializeChannel(bool prio_channel_exists)
#else
static char initializeChannel()
#endif
{
    int err;

    ALOGI("initializeChannel()");

#ifdef DUAL_CHANNELS
    if (!prio_channel_exists) {
        ATResponse *atresponse = NULL;
        /* Add the subscriptions done on prio channel here if no prio channel is configured */

        ALOGI("Add prioChannel commands to primary channel");

        /* Subscribe to ST-Ericsson Pin code event.
         *   The command requests the MS to report when the PIN code has been
         *   inserted and accepted.
         *      1 = Request for report on inserted PIN code is activated (on)
         */
        err = at_send_command("AT*EPEE=1", NULL);

        if (err < 0) {
            ALOGE("AT*EPEE=1 failed");
            goto error;
        }

        /* Subscribe to ST-Ericsson SIM State Reporting.
         *   Enable SIM state reporting on the format *ESIMSR: <sim_state>
         */
        err = at_send_command("AT*ESIMSR=1", NULL);

        if (err < 0) {
            ALOGE("AT*ESIMSR=1 failed");
            goto error;
        }

        /* Subscribe to ST-Ericsson Call monitoring events.
         *  onoff = 1 - Call monitoring is on
         */
        err = at_send_command("AT*ECAM=1", NULL);

        if (err < 0) {
            ALOGE("AT*ECAM=1 failed");
            goto error;
        }

        /* Subscribe to ST-Ericsson RF-state events.
         *   Enable RF-state reporting on this channel.
         */
        err = at_send_command("AT*ERFSTATE=1", NULL);

        if (err < 0) {
            ALOGE("AT*ERFSTATE=1 failed");
        /*  goto error; * Ignore error, AT may not support the command yet */
        }

        /*
         * REFRESH Proactive SAT Command Information
         * onoff = 1 Enable REFRESH information reporting.
         * Android does not support handling of REFRESH as a proactive command.
         * *ESIMRF is a proprietary AT command to handle RIL_UNSOL_SIM_REFRESH.
         */
         at_send_command("AT*ESIMRF=1", NULL);

         /* Subscribe to Calling Name Identification Presentation.
          *  +CNAP
          *      0 = disable CNAP events
          *      1 = enable CNAP events
          */
         err = at_send_command("AT+CNAP=1", NULL);

         if (err < 0) {
             ALOGW("AT+CNAP=1 failed");
         }

         /*
          * Configure User to User Signalling
          *  +CUSS1
          *      0 = disable UUS events for MO calls (+CUUS1I events).
          *      1 = enable UUS events for MT calls (+CUUS1U events)
          */
         err = at_send_command("AT+CUUS1=0,1", NULL);
         if (err < 0)
             goto error;

         /*
          * Configure Packet Domain Network Registration Status events
          *   2 - Enable network registration and location information
          *       unsolicited result code +CREG: <stat>[,<lac>,<ci>]
          */
         err = at_send_command("AT+CREG=2", &atresponse);
         /* Some handsets -- in tethered mode -- don't support CREG=2. */
         if (err < 0 || atresponse->success == 0)
             err = at_send_command("AT+CREG=1", NULL);
         at_response_free(atresponse);
         if (err < 0)
             goto error;

    } else {

        /* Unsubscribe from ST-Ericsson RF-state events.
         *   Disable RF-state reporting on this channel.
         */
        err = at_send_command("AT*ERFSTATE=0", NULL);

        if (err < 0) {
            ALOGE("AT*ERFSTATE=0 failed");
        /*  goto error; * Ignore error, AT may not support the command yet */
        }
    }
#endif

#ifndef USE_U8500_RIL
   /* SIM Application Toolkit Configuration
     *  n = 0 - Disable SAT unsolicited result codes
     *  stkPrfl = - SIM application toolkit profile in hexadecimal format
     *              starting with first byte of the profile.
     *              See 3GPP TS 11.14[1] for details.
     */
    /* TODO: Profile is currently empty because stkPrfl is currently overriden
       by the default profile stored in the modem. Investigate if profile should
       be set from host (Android) side. */
    at_send_command("AT*STKC=0,\"000000000000000000\"", NULL);

#endif

    /* Subscribe to Unstructured Supplementary Service Data (USSD) notifications
     *  n = 1 - Enable result code presentation in the TA.
     */
    at_send_command("AT+CUSD=1", NULL);

    if (err < 0) {
        ALOGW("AT+CUSD=1 failed");
    }

#ifdef DUAL_CHANNELS
    if (!prio_channel_exists) {
#endif
    /* Set phone functionality.
     *    4 = Disable the phone's transmit and receive RF circuits.
     */
    err = at_send_command("AT+CFUN=4", NULL);
    if (err < 0)
        goto error;

    /* Assume radio is off on error. */
    if (isRadioOn() > 0)
        setRadioState(RADIO_STATE_SIM_NOT_READY);
#ifdef DUAL_CHANNELS
    }
#endif

#ifndef USE_U8500_RIL
    /* Subscribe to ST-Ericsson Pin code event.
     *   The command requests the MS to report when the PIN code has been
     *   inserted and accepted.
     *      1 = Request for report on inserted PIN code is activated (on)
     */
    err = at_send_command("AT*EPEE=1", NULL);
    if (err < 0)
        goto error;

    /* Subscribe to ST-Ericsson SIM State Reporting.
     *   Enable SIM state reporting on the format *ESIMSR: <sim_state>
     */
    err = at_send_command("AT*ESIMSR=1", NULL);
    if (err < 0)
        goto error;

    /* Subscribe to ST-Ericsson Call monitoring events.
     *  onoff = 1 - Call monitoring is on
     */
    err = at_send_command("AT*ECAM=1", NULL);
    if (err < 0)
        goto error;
#endif

    /*
     * Emergency numbers from 3GPP TS 22.101, chapter 10.1.1.
     * 911 and 112 should always be set in the system property, but if SIM is
     * absent, these numbers also has to be added: 000, 08, 110, 999, 118
     * and 119.
     */
    err = property_set(PROP_EMERGENCY_LIST_RW,
                        "911,112,000,08,110,999,118,119");

    /*
     * We do not go to error in this case. Even though we cannot set emergency
     * numbers it is better to continue and at least be able to call some
     * numbers.
     */
    if (err < 0)
        ALOGE("[ECC] Creating emergency list ril.ecclist"
            " in system properties failed!");
    else
        ALOGD("[ECC] Set initial defaults to system property ril.ecclist");

    /*
     * Older versions of Android does not support ril.ecclist. For legacy
     * reasons ro.ril.ecclist is therefore set up with emergency numbers from
     * 3GPP TS 22.101, chapter 10.1.1.
     */
    err = property_set(PROP_EMERGENCY_LIST_RO,
                        "911,112,000,08,110,999,118,119");

    if (err < 0)
        ALOGE("[ECC] Creating emergency list ro.ril.ecclist in "
            "system properties failed!");
    else
        ALOGD("[ECC] Set initial defaults to system property ro.ril.ecclist");

    if (!isSimAbsent()) {
        err = property_set(PROP_EMERGENCY_LIST_RW, "911,112");

         /*
          * We do not go to error in this case. Even though we cannot set emergency
          * numbers it is better to continue and at least be able to call some
          * numbers.
          */
         if (err < 0)
             ALOGE("[ECC] Creating emergency list ril.ecclist"
                 " in system properties failed!");
         else
             ALOGD("[ECC] Set initial defaults SIM present to system property ril.ecclist");
    }
    else
        ALOGI("[ECC]: SIM is absent, keeping default ECCs");

    return 0;

error:
    return 1;
}

#ifdef DUAL_CHANNELS
/**
 * Initialize everything that can be configured while we're still in
 * AT+CFUN=0.
 */
static char initializePrioChannel()
{
    int err;
    ATResponse *atresponse = NULL;

    ALOGI("initializePrioChannel()");

    /* Subscribe to ST-Ericsson Pin code event.
     *   The command requests the MS to report when the PIN code has been
     *   inserted and accepted.
     *      1 = Request for report on inserted PIN code is activated (on)
     */
    err = at_send_command("AT*EPEE=1", NULL);

    if (err < 0) {
        goto error;
    }

    /* Subscribe to ST-Ericsson SIM State Reporting.
     *   Enable SIM state reporting on the format *ESIMSR: <sim_state>
     */
    err = at_send_command("AT*ESIMSR=1", NULL);

    if (err < 0) {
        goto error;
    }

    /* Subscribe to ST-Ericsson Call monitoring events.
     *  onoff = 1 - Call monitoring is on
     */
    err = at_send_command("AT*ECAM=1", NULL);

    if (err < 0) {
        ALOGE("AT*ECAM=1 failed");
        goto error;
    }

    /* Subscribe to ST-Ericsson RF-state events.
     *   Enable RF-state reporting in the format *ERFSTATE: <rf_state>
     */
    err = at_send_command("AT*ERFSTATE=1", NULL);

    if (err < 0) {
        ALOGE("AT*ERFSTATE=1 failed");
    /*  goto error; * Ignore error, AT may not support the command yet */
    }

    /*
     * REFRESH Proactive SAT Command Information
     * onoff = 1 Enable REFRESH information reporting.
     * Android does not support handling of REFRESH as a proactive command.
     * *ESIMRF is a proprietary AT command to handle RIL_UNSOL_SIM_REFRESH.
     */
     at_send_command("AT*ESIMRF=1", NULL);

     /* Subscribe to Calling Name Identification Presentation.
      *  +CNAP
      *      0 = disable CNAP events
      *      1 = enable CNAP events
      */
     err = at_send_command("AT+CNAP=1", NULL);

     if (err < 0) {
         ALOGW("AT+CNAP=1 failed");
     }

     /* Unsubscribe from Unstructured Supplementary Service Data (USSD) notifications
      *  n = 0 - Disable result code presentation in the TA.
      *
      *  NOTE: Unsubscription is done on the priority channel because USSD sessions
      *        may take time to complete. During this time unsolicited result codes
      *        may become blocked for a longer time than is acceptable. Subscription
      *        to USSD notifications is done only on the non-priority channel.
      */
     at_send_command("AT+CUSD=0", NULL);

     /*
      * Configure User to User Signalling
      *  +CUSS1
      *      0 = disable UUS events for MO calls (+CUUS1I events).
      *      1 = enable UUS events for MT calls (+CUUS1U events)
      */
     err = at_send_command("AT+CUUS1=0,1", NULL);
     if (err < 0)
         goto error;

     /*
      * Set phone functionality.
      *    4 = Disable the phone's transmit and receive RF circuits.
      */
     err = at_send_command("AT+CFUN=4", NULL);
     if (err < 0)
         goto error;

     /*
      * Configure Packet Domain Network Registration Status events
      *    2 - Enable network registration and location information
      *        unsolicited result code +CREG: <stat>[,<lac>,<ci>]
      */
     err = at_send_command("AT+CREG=2", &atresponse);
     /* Some handsets -- in tethered mode -- don't support CREG=2. */
     if (err < 0 || atresponse->success == 0)
         err = at_send_command("AT+CREG=1", NULL);
     at_response_free(atresponse);
     if (err < 0)
         goto error;

     /* Assume radio is off on error. */
     if (isRadioOn() > 0)
         setRadioState(RADIO_STATE_SIM_NOT_READY);

    return 0;

  error:
    return 1;
}
#endif

/**
 * Called by atchannel when an unsolicited line appears.
 * This is called on atchannel's reader thread. AT commands may
 * not be issued here.
 */
static void onUnsolicited(const char *s, const char *sms_pdu)
{
    ALOGI("onUnsolicited: %s", s);

    /* Ignore unsolicited responses until we're initialized.
       This is OK because the RIL library will poll for initial state. */
    if (sState == RADIO_STATE_UNAVAILABLE)
        return;

    if (strStartsWith(s, "*ETZV:")) {
        onNetworkTimeReceived(s);
    } else if (strStartsWith(s, "*ECME")) {
        onNetworkRejectCause(s);
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED,
                                  NULL, 0);
    } else if (strStartsWith(s, "*ELAT")) {
        onLocalComfortToneGenerationIndication(s);
    } else if (strStartsWith(s, "*EPEV"))
        /* Pin event, poll SIM State! */
        enqueueRILEvent(RIL_EVENT_QUEUE_PRIO, pollSIMState, NULL, NULL);
    else if (strStartsWith(s, "*ESIMSR"))
        onSimStateChanged(s);
    else if (strStartsWith(s, "NO CARRIER")
             || strStartsWith(s, "+CCWA")
             || strStartsWith(s, "BUSY"))
        RIL_onUnsolicitedResponse(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
                                  NULL, 0);
    else if (strStartsWith(s, "*EREG:")
#ifdef USE_U8500_RIL
             || strStartsWith(s, "+CREG:")
#endif
             || strStartsWith(s, "+CGREG:")
             || strStartsWith(s, "*EPSB:")
             || strStartsWith(s, "*ENNIR:"))
            onNetworkStateChanged(s);

#ifdef NO_AUTO_GPRS_ATTACH
        if (strStartsWith(s, "+CREG:")) {
            onCSNetStateChanged(s);
        }
#endif
    else if (strStartsWith(s, "+CMT:"))
        onNewSms(sms_pdu);
    else if (strStartsWith(s, "+CBM:"))
        onNewBroadcastSms(s, sms_pdu);
    else if (strStartsWith(s, "+CMTI:"))
        onNewSmsOnSIM(s);
    else if (strStartsWith(s, "+CDS:"))
        onNewStatusReport(sms_pdu);
    else if (strStartsWith(s, "+CGEV:")) {
        /* Really, we can ignore NW CLASS and ME CLASS events here,
           but right now we don't since extranous
           RIL_UNSOL_PDP_CONTEXT_LIST_CHANGED calls are tolerated. */
        enqueueRILEvent(RIL_EVENT_QUEUE_NORMAL, onPDPContextListChanged,
                        NULL, NULL);
    } else if (strStartsWith(s, "+CIEV: 2"))
        unsolSignalStrength(s);
    else if (strStartsWith(s, "+CIEV: 10"))
        unsolSimSmsFull(s);
    else if (strStartsWith(s, "+CNAP:"))
        onCallingNAmePresentationReceived(s);
    else if (strStartsWith(s, "+CSSI:"))
        onSuppServiceNotification(s, 0);
    else if (strStartsWith(s, "+CSSU:"))
        onSuppServiceNotification(s, 1);
    else if (strStartsWith(s, "+CUSD:"))
        onUSSDReceived(s);
    else if (strStartsWith(s, "+CUUS1U:"))
        onUUS1Received(s);
    else if (strStartsWith(s, "*ECAV:"))
        onECAVReceived(s);
    else if (strStartsWith(s, "*STKEND"))
        RIL_onUnsolicitedResponse(RIL_UNSOL_STK_SESSION_END, NULL, 0);
    else if (strStartsWith(s, "*STKI:"))
        onStkProactiveCommand(s);
    else if (strStartsWith(s, "*ESIMRF:"))
        onStkSimRefresh(s);
    else if (strStartsWith(s, "*STKN:"))
        onStkEventNotify(s);
    else if (strStartsWith(s, "*EACE:"))
        onAudioCallEventNotify(s);
#ifdef USE_U8500_RIL
    else if (strStartsWith(s, "*ERFSTATE: 1"))
        onRadioPowerOn();
    else if (strStartsWith(s, "*ERFSTATE: 0")) {
        /* TODO Radio is off - so what to do now? */
    }
#endif
}

static void signalCloseQueues(void)
{
    unsigned int i;

    for (i = 0; i < (sizeof(s_requestQueues) / sizeof(RequestQueue *));
         i++) {
        RequestQueue *q = s_requestQueues[i];
        pthread_mutex_lock(&q->queueMutex);
        q->closed = 1;
        pthread_cond_signal(&q->cond);
        pthread_mutex_unlock(&q->queueMutex);
    }
}

/* Called on command or reader thread. */
static void onATReaderClosed()
{
    ALOGI("AT channel closed");

    setRadioState(RADIO_STATE_UNAVAILABLE);
    signalCloseQueues();
}

/* Called on command thread. */
static void onATTimeout()
{
    ALOGI("AT channel timeout; restarting..\n");
    /* Last resort, throw escape on the line, close the channel
       and hope for the best. */
    at_send_escape();

    setRadioState(RADIO_STATE_UNAVAILABLE);
    signalCloseQueues();

    /* TODO We may cause a radio reset here. */
}

static void usage(char *s)
{
    ALOGE("usage: %s [-c <connection type>] "
         "[-n <number of channels>] "
         "[-p <primary channel>] "
         "[-s <secondary channel>] "
         "[-x <extra argument>] "
         "[-i <network interface>]\n", s);
    exit(-1);
}

struct queueArgs {
    char channels;
    const char *type;
    char *arg;
    char *xarg;
    char index;
};

#ifdef USE_U8500_RIL
#define CHANNEL_INIT_SLEEP 200000
#endif
static void *queueRunner(void *param)
{
    int fd;
    int ret;
    int i = 0;
    struct queueArgs *queueArgs = (struct queueArgs *) param;
    struct RequestQueue *q = NULL;
#ifdef USE_U8500_RIL
    int retry_count = 0;
#endif
    ALOGI("queueRunner starting!");

    for (;;) {
        fd = -1;
        while (fd < 0) {

            if (queueArgs->type == NULL) {
                ALOGE("Unsupported channel type. Bailing out.");
                free(queueArgs);
                return NULL;
            }

            if (!strncmp(queueArgs->type, "CAIF", 4)) {
#ifdef U300_RIL_CAIF
                struct sockaddr_caif addr = {
                    .family = AF_CAIF,
                    .u.at.type = CAIF_ATTYPE_PLAIN
                };

                fd = socket(AF_CAIF, SOCK_SEQPACKET, CAIFPROTO_AT);
                connect(fd, (struct sockaddr *) &addr, sizeof(addr));
#else
                ALOGE("Unsupported channel type CAIF. Bailing out.");
                free(queueArgs);
                return NULL;
#endif
            } else if (!strncmp(queueArgs->type, "UNIX", 4)) {
                struct sockaddr_un addr;
                int len;
                if (queueArgs->arg == NULL) {
                    ALOGE("No path specified for UNIX socket. Bailing out.");
                    free(queueArgs);
                    return NULL;
                }
                bzero((char *) &addr, sizeof(addr));
                addr.sun_family = AF_UNIX;

                strncpy(addr.sun_path, queueArgs->arg,
                        sizeof(addr.sun_path));
                len = strlen(addr.sun_path) + sizeof(addr.sun_family);
#ifdef USE_U8500_RIL
                fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
#else
                fd = socket(AF_UNIX, SOCK_STREAM, 0);
#endif
                if (fd < 0) {
                    ALOGE("failed to create socket error: %s",strerror(errno));
                    return NULL;
                }
                i = connect(fd, (struct sockaddr *) &addr, len);
                if (i < 0) {
                    ALOGE("failed to connect socket error: %s",strerror(errno));
                }
            } else if (!strncmp(queueArgs->type, "IP", 2)) {
                int port;
                if (!queueArgs->arg) {
                    ALOGE("No port specified for IP socket. Bailing out.");
                    free(queueArgs);
                    return NULL;
                }
                port = atoi(queueArgs->arg);
                if (queueArgs->xarg) {
                    char *host = queueArgs->xarg;
                    fd = socket_network_client(host, port, SOCK_STREAM);
                } else
                    fd = socket_loopback_client(port, SOCK_STREAM);
            } else if (!strncmp(queueArgs->type, "TTY", 3)) {
                struct termios ios;
                fd = open(queueArgs->arg, O_RDWR);

                /* Disable echo on serial ports. */
                tcgetattr(fd, &ios);
                cfmakeraw(&ios);
                cfsetospeed(&ios, B115200);
                cfsetispeed(&ios, B115200);
                ios.c_cflag |= CREAD | CLOCAL;
                tcflush(fd, TCIOFLUSH);
                tcsetattr(fd, TCSANOW, &ios);
            } else if (!strncmp(queueArgs->type, "CHAR", 4))
                fd = open(queueArgs->arg, O_RDWR);

            if (fd < 0) {
#ifdef USE_U8500_RIL
                ALOGE("FAILED to open AT channel type:%s %s %s err:%s. "
                     "retrying in 1.", queueArgs->type,
                     queueArgs->arg ? queueArgs->arg : "",
                     queueArgs->xarg ? queueArgs->xarg : "",
                     strerror(errno));
                sleep(1);
#else
                ALOGE("FAILED to open AT channel type:%s %s %s err:%s. "
                     "retrying in 10.", queueArgs->type,
                     queueArgs->arg ? queueArgs->arg : "",
                     queueArgs->xarg ? queueArgs->xarg : "",
                     strerror(errno));
                sleep(10);
#endif
            }
        }

        ret = at_open(fd, onUnsolicited);

        if (ret < 0) {
            ALOGE("AT error %d on at_open\n", ret);
            at_close();
            continue;
        }

        at_set_on_reader_closed(onATReaderClosed);
        at_set_on_timeout(onATTimeout);

        q = &s_requestQueue;

#ifdef DUAL_CHANNELS
        if (initializeCommon()) {
            if (queueArgs->index == 0) {  /* Only need logs on one channel  */
                if (retry_count < 5) {
                    ALOGE("FAILED to initialize channel common! Retrying.");
                } else if (retry_count == 6) {
                    ALOGE("FAILED to initialize channel common! Retrying silently.");
                }
                retry_count++;
            }

            at_close();
            usleep(CHANNEL_INIT_SLEEP);
            continue;
        }

        if (queueArgs->index == 0) {
            q->closed = 0;

            /* Initialization of the normal channel (index=0) depends
             * on if there is a priority channel (index=1) or not.
             */
            if (initializeChannel(queueArgs->channels > 1)) {
                ALOGE("FAILED to initialize channel!");
                at_close();
                usleep(CHANNEL_INIT_SLEEP);
                continue;
            }

            at_make_default_channel();
        } else {
            q = &s_requestQueuePrio;
            q->closed = 0;
        }
        at_set_timeout_msec(0);   /* This modem has no direct control
                                     over the channels so it may not reset on timeout */

        if (queueArgs->channels > 1) {
            if (queueArgs->index == 1) {
                if (initializePrioChannel()) {
                    ALOGE("FAILED to initialize prio channel!");
                    at_close();
                    usleep(CHANNEL_INIT_SLEEP);
                    continue;
                }
                setRadioState(RADIO_STATE_OFF);
            }
        } else {
            setRadioState(RADIO_STATE_OFF);
        }
#else
        if (initializeCommon()) {
            ALOGE("FAILED to initialize channel!");
            at_close();
            continue;
        }

        if (queueArgs->index == 0) {
            q->closed = 0;
            if (initializeChannel()) {
                ALOGE("FAILED to initialize channel!");
                at_close();
                continue;
            }
            at_make_default_channel();
            setRadioState(RADIO_STATE_OFF);
        } else {
            q = &s_requestQueuePrio;
            q->closed = 0;
            at_set_timeout_msec(1000 * 30);
        }
#endif

#if RIL_VERSION >= 6
        {
            int rilVersion = RIL_VERSION;
            RIL_onUnsolicitedResponse(RIL_UNSOL_RIL_CONNECTED, &rilVersion, sizeof(int *));
        }
#endif

        ALOGI("Looping the requestQueue!");
        for (;;) {
            RILRequest *r;
            RILEvent *e;
            struct timeval tv;
            struct timespec ts;

            memset(&ts, 0, sizeof(ts));

            pthread_mutex_lock(&q->queueMutex);

            if (q->closed != 0) {
                ALOGW("AT Channel error, attempting to recover..");
                pthread_mutex_unlock(&q->queueMutex);
                break;
            }

            while (q->closed == 0 && q->requestList == NULL &&
                   q->eventList == NULL) {
                pthread_cond_wait(&q->cond, &q->queueMutex);

            }

            /* eventList is prioritized, smallest abstime first. */
            if (q->closed == 0 && q->requestList == NULL && q->eventList) {
                int err = 0;
                err = pthread_cond_timedwait(&q->cond, &q->queueMutex,
                                             &q->eventList->abstime);
                if (err && err != ETIMEDOUT)
                    ALOGE("timedwait returned unexpected error: %s",
                         strerror(err));
            }

            if (q->closed != 0) {
                pthread_mutex_unlock(&q->queueMutex);
                continue;       /* Catch the closed bit at the top of the loop. */
            }

            e = NULL;
            r = NULL;

            gettimeofday(&tv, NULL);

            /* Saves a macro, uses some stack and clock cycles.
               TODO Might want to change this. */
            ts.tv_sec = tv.tv_sec;
            ts.tv_nsec = tv.tv_usec * 1000;

            if (q->eventList != NULL &&
                timespec_cmp(q->eventList->abstime, ts, <)) {
                e = q->eventList;
                q->eventList = e->next;
            }

            if (q->requestList != NULL) {
                r = q->requestList;
                q->requestList = r->next;
            }

            pthread_mutex_unlock(&q->queueMutex);

            if (e) {
                e->eventCallback(e->param);
                free(e);
            }

            if (r) {
                processRequest(r->request, r->data, r->datalen, r->token);
                freeRequestData(r->request, r->data, r->datalen);
                free(r);
            }
        }

        at_close();

        /*
         * After loss of communication, complete all pending requests.
         * If contact is lost with the radio, for instance when an
         * MSR (modem silent reboot) is performed, the upper layer should
         * not be left waiting for responses to requests while the modem
         * reboots and services are restarted. This could leave
         * applications in an incorrect state for the duration.
         * Instead return an error for the requests.
         */
        int count = 0;
        pthread_mutex_lock(&q->queueMutex);
        RILRequest *r = q->requestList;
        while (r) {
            q->requestList = r->next;
            processRequest(r->request, r->data, r->datalen, r->token);
            freeRequestData(r->request, r->data, r->datalen);
            free(r);
            r = q->requestList;
            count++;
        }
        pthread_mutex_unlock(&q->queueMutex);

        if (count != 0) {
            ALOGW("Completed %d pending requests after close", count);
        }
        ALOGI("Re-opening after close");
    }

    free(queueArgs);
    return NULL;
}

#define RIL_MAX_NR_OF_CHANNELS 2
pthread_t s_tid_queueRunner[RIL_MAX_NR_OF_CHANNELS];

void dummyFunction(void *args)
{
    ALOGE("dummyFunction: %p", args);
}

const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc,
                                   char **argv)
{
    int opt;
    int i;
    int channels = 1;
    char *type = NULL;
    char *args[RIL_MAX_NR_OF_CHANNELS] = { NULL, NULL };
    char *xarg = NULL;
    struct queueArgs *queueArgs[RIL_MAX_NR_OF_CHANNELS] = { NULL, NULL };
    pthread_attr_t attr;

    s_rilenv = env;

    /*Ingore SIGPIPE and avoid crash */
    if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
    ALOGE("RIL: SIG_ERR when ignoring SIGPIPE\n");
    abort();
    }

    while (-1 != (opt = getopt(argc, argv, "c:n:p:s:x:i:"))) {
        switch (opt) {
        case 'c':
            type = optarg;
            ALOGI("Using channel type %s.", type);
            break;

        case 'n':
            channels = atoi(optarg);
            ALOGI("Number of channels %d.", channels);
            break;

        case 'p':
            args[0] = optarg;
            ALOGI("Primary: %s\n", args[0]);
            break;

        case 's':
            args[1] = optarg;
            ALOGI("Secondary: %s\n", args[1]);
            break;

        case 'x':
            xarg = optarg;
            ALOGI("Extra argument %s.", xarg);
            break;

        case 'i':
            strncpy(ril_iface, optarg, strlen(optarg));
            ril_iface[strlen(optarg)] = '\0';
            ALOGI("Using network interface %s as prefix for data channel.",
                    ril_iface);
            break;

        default:
            usage(argv[0]);
            return NULL;
        }
    }

    if (!strcmp(ril_iface, "")) {
        ALOGI("Network interface was not supplied, falling back on gprs0!");
        strcpy(ril_iface, "gprs0");
    }


    for (i = 0; i < channels; i++) {
        if (i >= RIL_MAX_NR_OF_CHANNELS)
            break;

        queueArgs[i] = malloc(sizeof(struct queueArgs));
        memset(queueArgs[i], 0, sizeof(struct queueArgs));

        queueArgs[i]->channels = channels;
        queueArgs[i]->type = type;
        queueArgs[i]->index = i;
        queueArgs[i]->arg = args[i];
        queueArgs[i]->xarg = xarg;

        if (i == 1)
            s_requestQueuePrio.enabled = 1;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        pthread_create(&s_tid_queueRunner[i], &attr, queueRunner,
                       queueArgs[i]);
    }

    return &s_callbacks;
}

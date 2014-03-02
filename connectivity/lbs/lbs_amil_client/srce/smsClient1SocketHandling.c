/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Balaji Natakala
 *  Email : balaji.natakala@stericsson.com
 *****************************************************************************/

#define SMSCLIENT_SESSIONHANDLING_C

#include "amil.h"
#include "smsClientSocketHandling.h"
#include "smsClientSmsHandling.h"


static int v_SmsReqRespSckt = K_SMSCLIENT_UNDEFINED_SOCKET;
static int v_SmsEventSckt   = K_SMSCLIENT_UNDEFINED_SOCKET;

/**
 * @brief Initialise SMS Server Request Control Block Data structure for WAIT_MODE
 *
 * @param ReqCtrlBlk   Request Control Block to be initialised.
 *
 * @return TRUE if success, otherwise FALSE.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
bool smsClient1_1ServerReqCtrlBlkWaitModeInit(SMS_RequestControlBlock_t * const ReqCtrlBlk)
{
    if (v_SmsReqRespSckt == K_SMSCLIENT_UNDEFINED_SOCKET || v_SmsEventSckt == K_SMSCLIENT_UNDEFINED_SOCKET)
        ERR("SMS Sockets not initialised. %d %d\n", v_SmsReqRespSckt, v_SmsEventSckt);

    ReqCtrlBlk->WaitMode  = SMS_WAIT_MODE;
    ReqCtrlBlk->ClientTag = SMS_CLIENT_TAG_NOT_USED;
    ReqCtrlBlk->Socket    = v_SmsReqRespSckt;

    return (bool) (v_SmsReqRespSckt != K_SMSCLIENT_UNDEFINED_SOCKET);
}



/**
 * @brief Creates receive thread used handle data received from SMS and UICC CAT Server.
 */ 
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
bool smsClient1_2Init(int8_t vp_handle)
{
    bool vl_Error = FALSE;

    if (v_SmsReqRespSckt == K_SMSCLIENT_UNDEFINED_SOCKET) {
        // Create session with SMS server
        smsClient1_9CreateSmsSession();
    }

    if (v_SmsReqRespSckt != K_SMSCLIENT_UNDEFINED_SOCKET && v_SmsEventSckt != K_SMSCLIENT_UNDEFINED_SOCKET) 
    {
        Amil1_06RegisterForListen(v_SmsEventSckt, &smsClient1_5HandleSocketSelect, vp_handle);
    } 
    else 
    {
    	vl_Error = TRUE;
        ERR("SMS Socket creation failed %d %d\n", v_SmsReqRespSckt, v_SmsEventSckt);
    }

	return vl_Error;
}


/**
 * @brief Sets initial data needed to specify a timeout with select(). If
 *        called with a non-zero value, before a previous timer has expired,
 *        the new timer value will overwrite the previous value.
 *
 * @param vp_Seconds Duration, in seconds, for timer.  Set to 0 to clear a timer.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
void smsClient1_3SelectTimeoutSet(const long vp_Seconds)
{
    t_smsClient_TimeoutInfo *pl_TimeoutInfo;

    INF("timeout: %lds\n", vp_Seconds);

    pl_TimeoutInfo = smsClient1_7GetTimeoutInfo();

    if (pl_TimeoutInfo != NULL) {
        if (vp_Seconds > 0) {
            // Setting a new timer.
            pl_TimeoutInfo->v_TimerSet        = TRUE;
            pl_TimeoutInfo->v_TimerLengthSecs = vp_Seconds;

            // Store time when timer was set.
            (void) gettimeofday(&pl_TimeoutInfo->v_TimerCreatedNow, NULL);
        } else {
            // Clearing a timer.
            pl_TimeoutInfo->v_TimerSet        = FALSE;
            pl_TimeoutInfo->v_TimerLengthSecs = 0;
        }
    } else
        ERR("pl_TimeoutInfo is NULL\n");
}

#if 0
/**
 * @brief Thread created for the SMS and UICC Receiver. Creates a sessions with
 *        the SMS Server and UICC CAT Server. It then starts infinite receive loop.
 *
 * @param threadid   Thread ID passed in from pthread_create(). It's not currently
 *                   used.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
static void *smsClient1_4SocketHandlerThread(void *threadid)
{
    INF("%s: called\n", __func__);

    if (v_SmsReqRespSckt == K_SMSCLIENT_UNDEFINED_SOCKET) {
        // Create session with SMS server
        smsClient1_9CreateSmsSession();
    }

    if (v_SmsReqRespSckt != K_SMSCLIENT_UNDEFINED_SOCKET && v_SmsEventSckt != K_SMSCLIENT_UNDEFINED_SOCKET) 
    {
        smsClient1_5HandleSocketSelect(v_SmsEventSckt,);  // This call should never return.
    } else {
        ERR("SMS Socket creation failed %d %d\n", v_SmsReqRespSckt, v_SmsEventSckt);
    }

    pthread_exit(NULL);
    INF("%s: done\n", __func__);
    return NULL;
}
#endif

/**
 * @brief Waits for data on SMS and UICC server sockets. When data is detected
 *        it is passed to the appropriate handler functions.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
static bool smsClient1_5HandleSocketSelect(void* pp_FileDes, int32_t vl_SelectReturn, int8_t vp_handle)
{
    bool vl_closed = FALSE;

    if (vl_SelectReturn > 0) 
    {
        // SMS Event pl_Signal detected on socket.
        INF("Data Available on SMS Event socket %d\n", *((int32_t*)pp_FileDes));
        vl_closed = smsClient1_10HandleSmsSocketRead(*((int32_t*)pp_FileDes));
    } 
    else if (vl_SelectReturn == 0) 
    {
        // select has returned without anything on one the sockets being monitored, therefore the
        // timeout must have expired.
        INF("select() timed-out\n");
        
        // Notify sms handling code that the timer has expired.
        smsClient2_08TimerExpired();
    } 
    else 
    {
        // Something badly wrong, select() has returned an error!
        ERR("select() returned %d\n", vl_SelectReturn);
    }

    return vl_closed;
}



/**
 * @brief Returns the highest socket value from the SMS and UICC sockets.
 *
 * @return Highest socket value.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
static int smsClient1_6GetHighestSocketValue(void)
{
    // We don't select on the v_SmsReqRespSckt socket because we only
    // use SMS Server in WAIT_MODE.
    return v_SmsEventSckt;
}


/**
 * @brief Returns a data structure needed to support the timer functionality. If the
 *        data structure has not been created, previously, it is malloc'd and initialised.
 *
 * @return Data structure.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
static t_smsClient_TimeoutInfo *smsClient1_7GetTimeoutInfo(void)
{
    static t_smsClient_TimeoutInfo *pl_TimeoutInfo = NULL;

    INF("pl_TimeoutInfo %08X\n", (uint32_t)pl_TimeoutInfo);

    if (pl_TimeoutInfo == NULL) {
        // Memory has not been allocated previously
        pl_TimeoutInfo = (t_smsClient_TimeoutInfo *) OSA_Malloc(sizeof(t_smsClient_TimeoutInfo));

        if (pl_TimeoutInfo != NULL) {
            pl_TimeoutInfo->v_TimerSet = FALSE;
            pl_TimeoutInfo->v_TimerLengthSecs = 0;
            pl_TimeoutInfo->v_TimerCreatedNow.tv_sec = 0;
            pl_TimeoutInfo->v_TimerCreatedNow.tv_usec = 0;
            pl_TimeoutInfo->v_SelectTimeout.tv_sec = 0;
            pl_TimeoutInfo->v_SelectTimeout.tv_usec = 0;
        } else
            ERR("Malloc failed\n");
    }

    return pl_TimeoutInfo;
}

/**
 * @brief Returns a timeval structure which can be used in a select() call
 *        so that a basic timeout can be supported. If no timeout is
 *        currently active, NULL is returned.
 *
 * @return timeval structure, or NULL, for use with select().
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
static struct timeval *smsClient1_8GetTimeoutForSelect(void)
{
    t_smsClient_TimeoutInfo *pl_TimeoutInfo;
    struct timeval* pl_SelectTimeout = NULL;

    INF("%s : called\n", __func__);

    pl_TimeoutInfo = smsClient1_7GetTimeoutInfo();

    if (pl_TimeoutInfo != NULL && pl_TimeoutInfo->v_TimerSet) {
        struct timeval vl_TimeNow;

        // When v_TimerSet is TRUE a timer has been set. However when select() is called it is possible that
        // data will be detected on a socket before the timer has expired.  This could happen in such a way
        // that the timer should have expired before select() is called again.  Therefore if this happens we
        // should continue to use a timeout with select(), however the timeout value should be 0. This will
        // force an immediate timer expiry from select(); again however there is a possiblity that there will
        // have been data detected on one of the sockets so the timer is not reset...and we go around the
        // loop again with a timeout of 0.  When select() finally returns due to the timeout, v_TimerSet is
        // set to FALSE and the timeout "actions" performed; and then the timer functionality is disabled.

        // We are not using the tv_usec member of struct timeval so set it to 0 and forget about it.
        pl_TimeoutInfo->v_SelectTimeout.tv_usec = 0;

        // Now check if any time has elapsed since smsClient1_select_timeout_set() was called and which
        // should be deducted from the timeout value passed to select().
        if (gettimeofday(&vl_TimeNow, NULL) == 0) {
            if (vl_TimeNow.tv_sec > pl_TimeoutInfo->v_TimerCreatedNow.tv_sec) {
                // Time has elapsed since timeout was first set
                const long vl_TimeDiff = vl_TimeNow.tv_sec - pl_TimeoutInfo->v_TimerCreatedNow.tv_sec;

                if (vl_TimeDiff < pl_TimeoutInfo->v_TimerLengthSecs) {
                    // Time elapsed is less than the initial length of the timer.
                    pl_TimeoutInfo->v_SelectTimeout.tv_sec = pl_TimeoutInfo->v_TimerLengthSecs - vl_TimeDiff;
                } else {
                    // Time elapsed is more than, or equal to, the initial length of the timer. Therefore
                    // set the timeout value to be 0 which will force an immediate expiry when select()
                    // is called.
                    pl_TimeoutInfo->v_SelectTimeout.tv_sec = 0;
                }
            } else {
                // No time has elapsed since timeout was first set.
                pl_TimeoutInfo->v_SelectTimeout.tv_sec = pl_TimeoutInfo->v_TimerLengthSecs;
            }
        } else {
            // Something is wrong as we can't get the current time. Set the timeout value to be 0 which
            // will force an immediate expiry when select() is called.
            pl_TimeoutInfo->v_SelectTimeout.tv_sec = 0;
            ERR("gettimeofday() FAILED\n");
        }

        INF("v_SelectTimeout %ld secs\n", pl_TimeoutInfo->v_SelectTimeout.tv_sec);
        pl_SelectTimeout = &pl_TimeoutInfo->v_SelectTimeout;
    }

    return pl_SelectTimeout;
}


/**
 * @brief Creates the SMS Server session and gets the SMS Server sockets.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
static void smsClient1_9CreateSmsSession(void)
{
    SMS_RequestStatus_t vl_ReqStatus;
    SMS_Error_t         vl_Error;

    INF("%s : called\n", __func__);

    vl_ReqStatus = Request_SMS_CB_SessionCreate(&v_SmsReqRespSckt, &v_SmsEventSckt, &vl_Error);

    if (vl_ReqStatus == SMS_REQUEST_OK && vl_Error == SMS_ERROR_INTERN_NO_ERROR)
    {
        INF("v_SmsReqRespSckt=%d v_SmsEventSckt=%d\n", v_SmsReqRespSckt, v_SmsEventSckt);
        smsClient2_01RegisterApplicationPort();
    }
    else
    {
        ERR("Create SMS Session failed %d %d\n", vl_ReqStatus, vl_Error);
        v_SmsReqRespSckt = K_SMSCLIENT_UNDEFINED_SOCKET;
        v_SmsEventSckt = K_SMSCLIENT_UNDEFINED_SOCKET;
    }
}



/**
 * @brief Handles a read on one of the SMS Server sockets.
 *
 * @param socket SMS Server socket to read from.
 */
#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
static bool smsClient1_10HandleSmsSocketRead(const int socket)
{
    bool vl_closed = FALSE;
    uint32_t vl_Primitive = 0;
    void*    pl_Signal = Util_SMS_SignalReceiveOnSocket(socket, &vl_Primitive, NULL);

    if (pl_Signal != NULL) {
        switch (vl_Primitive) {
        // Events which are specifically handled in AMIL.
        case EVENT_SMS_APPLICATIONPORTMATCHED:
            INF("Rx: EVENT_SMS_APPLICATIONPORTMATCHED\n");
            smsClient2_07HandleSmsEventApplicationPortMatched(pl_Signal);
            break;

        // Events which are not specifically handled in AMIL.
        case EVENT_SMS_SERVERSTATUSCHANGED:
            INF("Rx: EVENT_SMS_SERVERSTATUSCHANGED (Unhandled)\n");
            break;
        case EVENT_SMS_ERROROCCURRED:
            INF("Rx: EVENT_SMS_ERROROCCURRED (Unhandled)\n");
            break;
        case EVENT_SMS_STORAGESTATUSCHANGED:
            INF("Rx: EVENT_SMS_STORAGESTATUSCHANGED (Unhandled)\n");
            break;
        case EVENT_SMS_SHORTMESSAGERECEIVED:
            INF("Rx: EVENT_SMS_SHORTMESSAGERECEIVED (Unhandled)\n");
            break;
        case EVENT_SMS_STATUSREPORTRECEIVED:
            INF("Rx: EVENT_SMS_STATUSREPORTRECEIVED (Unhandled)\n");
            break;

        // Everything else!
        default:
            ERR("Received Unexpected SMS Primitive 0x%08X\n", vl_Primitive);
            break;
        }

        // Free up the SMS signal data.
        Util_SMS_SignalFree(pl_Signal);
    } else {
        // If no signal was received the socket has been closed by the peer,
        // the sms_server. Close and clean-up at this end, and try to reconnect.
        vl_closed = TRUE;
    }

    return vl_closed;
}

#undef  PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
void smsClient1_11DeInit(int8_t vp_handle)
{

    INF("%s : called\n", __func__);

    if (   v_SmsReqRespSckt != K_SMSCLIENT_UNDEFINED_SOCKET
		|| v_SmsEventSckt != K_SMSCLIENT_UNDEFINED_SOCKET) {
        // Deregistering both the FDs even if any one of the FD is defined.

		INF("smsClient1_11DeInit : FD:%d,  Handle : %d\n",v_SmsEventSckt, vp_handle);
        Amil1_07DeRegister(v_SmsEventSckt, vp_handle);

        close(v_SmsReqRespSckt);
        close(v_SmsEventSckt);

        v_SmsReqRespSckt = K_SMSCLIENT_UNDEFINED_SOCKET;
        v_SmsEventSckt =K_SMSCLIENT_UNDEFINED_SOCKET;
        
        INF("%s: done\n", __func__);
    }
}

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

#ifndef SMS_CLIENT_SOCKET_HANDLING_H
#define SMS_CLIENT_SOCKET_HANDLING_H

#ifdef AMIL_LCS_FTR
#include "smsClient.h"
#endif /* AMIL_LCS_FTR */

// SMS Server Header files
#include "r_sms_cb_session.h"



/**
 * @brief Structure used to store information needed to handle a timer. The timer is used when
 *        concatenated SMS messages are received.
 *
 * @param v_TimerSet         TRUE if a timer has been set, but it has not yet been cleared.
 *
 * @param v_TimerLengthSecs Length, in seconds, that the timer was specified for.
 *
 * @param timer_stored_now  Stores time at which the timer was initialised. Used to track
 *                          elapsed time while timer is active.
 *
 * @param v_SelectTimeout    Stores the timeout value which is passed to select(). Note that
 *                          select() may update this value so its value should not be relied
 *                          upon after select() has been called.
 */
typedef struct {
    bool v_TimerSet;
    long v_TimerLengthSecs;
    struct timeval v_TimerCreatedNow;
    struct timeval v_SelectTimeout;
} t_smsClient_TimeoutInfo;

#define K_SMSCLIENT_UNDEFINED_SOCKET                (-1)
#define K_SMSCLIENT_UICC_INIT_THREAD_ID_VALUE       0


static void* smsClient1_4SocketHandlerThread(void* pl_ThreadId);
static bool smsClient1_5HandleSocketSelect(void* pp_FileDes, int32_t vl_SelectReturn, int8_t vp_handle);
static t_smsClient_TimeoutInfo* smsClient1_7GetTimeoutInfo(void);
static struct timeval* smsClient1_8GetTimeoutForSelect(void);
static void smsClient1_9CreateSmsSession(void);
static int  smsClient1_6GetHighestSocketValue(void);
static bool smsClient1_10HandleSmsSocketRead(const int socket);
void smsClient1_11DeInit(int8_t vp_handle);


#endif /*SMS_CLIENT_SOCKET_HANDLING_H*/


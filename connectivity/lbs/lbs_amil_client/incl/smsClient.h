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

#ifndef SMS_CLIENT_COMMON_H
#define SMS_CLIENT_COMMON_H


#include "agpsosa.h"
#include "t_sms_cb_session.h"
#include "sim.h"
#include "gnsSUPL_Api.h"
#include "amil.h"


#include "r_sms.h"              /* SMS server */
#include "agps.h"

#undef  LOG_TAG
#define LOG_TAG "AMIL SMS"

#define INF       ALOGI
#define ERR       ALOGE
#define DBG       ALOGD



/*
 * PMIL internal functions to handle SMS Server interface.
 */
// Utility functions to initialise RequestControlBlock data structure passed to SMS Server APIs.
bool smsClient1_1ServerReqCtrlBlkWaitModeInit(SMS_RequestControlBlock_t * const ReqCtrlBlk);

bool smsClient1_2Init(int8_t vp_handle);

// Utility function to set a timer.  Set to 0 to reset it.
// N.B. only one timer is possible at a time. If function is called, with a non-zero value,
// before a previous timer has expired; the old timer is replaced with the new one.
void smsClient1_3SelectTimeoutSet(const long vp_Seconds);

 
// Functions to unpack and handle asynchronous signals from SMS Server.
void smsClient2_07HandleSmsEventApplicationPortMatched(const void *const RecPrim_p);

// Utility function to notify sms code that the timeout has expired.
void smsClient2_08TimerExpired(void);


#endif /* SMS_CLIENT_COMMON_H */


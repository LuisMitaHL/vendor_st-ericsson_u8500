/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __LCSCLIENT_H__
#define __LCSCLIENT_H__

#ifdef AGPS_FTR
#include "gnsCP_Api.h"
#endif

#ifdef GPS_FREQ_AID_FTR
#include "gnsFA.h"
#endif
#ifdef AGPS_TIME_SYNCH_FTR
#include "gnsFTA_Typedefs.h"
#endif
#include "amil.h"

bool lcsClient1_01Init (int8_t vp_handle);
void lcsClient1_12DeInit (int8_t vp_handle);
bool smsClient1_2Init(int8_t vp_handle);
void smsClient1_11DeInit(int8_t vp_handle);
bool netClient1_1Init(int8_t vp_handle);
void netClient1_12Deinit(int8_t vp_handle);
bool simClient1_1Init(int8_t vp_handle);
void simClient1_6DeInit(int8_t vp_handle);

void Amil1_01Init(void);

#ifdef AGPS_FTR
void __GnsCpCallback (e_gnsCP_MsgType msgtype, uint32_t  length, u_gnsCP_MsgData *data);
#endif

#ifdef GPS_FREQ_AID_FTR
void __GnsFaCallback(e_gnsFA_MsgType msgtype, uint32_t length, u_gnsFA_MsgDataOut *data);
#endif

#ifdef AGPS_TIME_SYNCH_FTR
void __GnsFtaCallback(e_gnsFTA_MsgType msgtype, uint32_t length, u_gnsFTA_MsgDataOut *data);
#endif

#endif /*__LCSCLIENT_H__*/



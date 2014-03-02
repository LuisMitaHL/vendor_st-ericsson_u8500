/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     CSCall_loopback.h
 * \brief    factory method for cscall proxy
 * \author   ST-Ericsson
 */
/*****************************************************************************/

#ifndef _CSCALL_LOOPBACK_H_
#define _CSCALL_LOOPBACK_H_

typedef struct AudioConfigSpeechLoops_t {
  t_uint8 bLoopUplinkDownlink;
  t_uint8 bLoopUplinkDownlinkAlg;
  t_uint8 bLoopDownlinkUplink;
  t_uint8 bLoopDownlinkUplinkAlg;
} AudioConfigSpeechLoops_t;

typedef struct AudioConfigSpeechTimingReq_t {
  t_uint32 nDeliveryTime;
  t_uint32 nModemProcessingTime;
}AudioConfigSpeechTimingReq_t;


#endif//#ifndef _CSCALL_LOOPBACK_H_

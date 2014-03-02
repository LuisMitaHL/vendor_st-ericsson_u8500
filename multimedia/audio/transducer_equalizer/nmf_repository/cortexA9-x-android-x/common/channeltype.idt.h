/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated common/channeltype.idt defined type */
#if !defined(_CHANNELTYPE_IDT)
#define _CHANNELTYPE_IDT


#define MAXCHANNELS_SUPPORT 16

typedef enum t_xyuv_t_channel_type {
  CHANNEL_NONE=0x0,
  CHANNEL_LF=0x1,
  CHANNEL_RF=0x2,
  CHANNEL_CF=0x3,
  CHANNEL_LS=0x4,
  CHANNEL_RS=0x5,
  CHANNEL_LFE=0x6,
  CHANNEL_CS=0x7,
  CHANNEL_LR=0x8,
  CHANNEL_RR=0x9,
  CHANNEL_MAX=0x7FFFFFFF} t_channel_type;

#endif

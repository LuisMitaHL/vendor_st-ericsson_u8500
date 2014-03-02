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

/* Generated stereowidener/nmfil/effect/config.idt defined type */
#if !defined(_STEREOWIDENER_CONFIG_H_)
#define _STEREOWIDENER_CONFIG_H_

#include <host/memorypreset.idt.h>

typedef struct t_xyuv_StereowidenerParams_t {
  t_uint16 iMemoryPreset;
  t_uint16 iSamplingFreq;
  t_uint16 iChannelsIn;
} StereowidenerParams_t;

typedef struct t_xyuv_StereowidenerConfig_t {
  t_uint16 iEnable;
  t_uint16 iOutputMode;
  t_uint16 iHeadphoneMode;
  t_uint16 iInternalExternalSpeaker;
  t_uint16 iStrength;
} StereowidenerConfig_t;

#endif

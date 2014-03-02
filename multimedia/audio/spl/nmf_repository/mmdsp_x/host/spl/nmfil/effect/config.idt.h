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

/* Generated spl/nmfil/effect/config.idt defined type */
#if !defined(_SPL_CONFIG_H_)
#define _SPL_CONFIG_H_


typedef struct t_xyuv_SPLimiterParams_t {
  t_uint16 instance;
  t_uint16 iSamplingFreq;
  t_uint16 iNumberChannel;
  t_uint16 iType;
  t_uint16 iPeakClippingMode;
  t_uint16 iPowerAttackTime_l;
  t_uint16 iPowerAttackTime_h;
  t_uint16 iPowerReleaseTime_l;
  t_uint16 iPowerReleaseTime_h;
  t_uint16 iGainAttackTime_l;
  t_uint16 iGainAttackTime_h;
  t_uint16 iGainReleaseTime_l;
  t_uint16 iGainReleaseTime_h;
  t_uint16 iGainHoldTime_l;
  t_uint16 iGainHoldTime_h;
  t_uint16 iThreshAttackTime_l;
  t_uint16 iThreshAttackTime_h;
  t_uint16 iThreshReleaseTime_l;
  t_uint16 iThreshReleaseTime_h;
  t_uint16 iThreshHoldTime_l;
  t_uint16 iThreshHoldTime_h;
  t_uint16 iBlockSize;
  t_uint16 iLookAheadSize;
} SPLimiterParams_t;

typedef struct t_xyuv_SPLimiterConfig_t {
  t_uint16 iEnable;
  t_uint16 iThreshold;
  t_uint16 iSPL_Mode;
  t_uint16 iSPL_UserGain;
  t_uint16 iPeakL_Gain;
} SPLimiterConfig_t;

#endif

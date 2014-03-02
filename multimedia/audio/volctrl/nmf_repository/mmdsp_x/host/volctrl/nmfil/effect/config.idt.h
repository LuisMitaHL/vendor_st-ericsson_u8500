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

/* Generated volctrl/nmfil/effect/config.idt defined type */
#if !defined(_VOLCTRL_CONFIG_H_)
#define _VOLCTRL_CONFIG_H_


typedef struct t_xyuv_VolctrlConfig_t {
  t_uint16 iDownMix;
  t_uint16 igll;
  t_uint16 iglr;
  t_uint16 igrl;
  t_uint16 igrr;
  t_uint24 ialpha;
  t_uint16 iDBRamp;
} VolctrlConfig_t;

typedef struct t_xyuv_VolctrlRampConfig_t {
  t_uint16 iStartVolume;
  t_uint16 iEndVolume;
  t_uint24 ialpha;
  t_uint16 iDBRamp;
  t_uint16 iChannel;
  t_uint16 iTerminate;
} VolctrlRampConfig_t;

#endif

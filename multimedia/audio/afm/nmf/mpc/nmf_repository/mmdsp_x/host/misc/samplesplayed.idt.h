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

/* Generated misc/samplesplayed.idt defined type */
#if !defined(__SAMPLESPLAYED_H_)
#define __SAMPLESPLAYED_H_


typedef struct t_xyuv_SamplesPlayed_t {
  t_uint32 samplesPlayedLH;
  t_uint32 samplesPlayedLL;
} SamplesPlayed_t;

typedef struct t_xyuv_MixerSamplesPlayed_t {
  SamplesPlayed_t port[9];
} MixerSamplesPlayed_t;

typedef struct t_xyuv_MixerUseSamplesPlayed_t {
  t_uint48 port[9];
} MixerUseSamplesPlayed_t;

#endif

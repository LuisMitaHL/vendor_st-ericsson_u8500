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

/* Generated common/bindings/shmpcmconfig.idt defined type */
#if !defined(_SHM_PCM_CONFIG_H_)
#define _SHM_PCM_CONFIG_H_

#include <host/bool.idt.h>
#include <host/samplefreq.idt.h>
#include <host/common/bindings/swap.idt.h>

typedef struct t_xyuv_ShmPcmConfig_t {
  t_uint16 nb_buffer;
  t_swap_mode swap_bytes;
  t_uint16 bitsPerSampleIn;
  t_uint16 bitsPerSampleOut;
  t_uint16 channelsIn;
  t_uint16 channelsOut;
  t_sample_freq sampleFreq;
  BOOL isSynchronized;
  BOOL hostMpcSyncEnabled;
} ShmPcmConfig_t;

#endif

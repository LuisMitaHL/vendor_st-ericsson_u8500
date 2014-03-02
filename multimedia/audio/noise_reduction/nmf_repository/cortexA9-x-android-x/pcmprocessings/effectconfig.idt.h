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

/* Generated pcmprocessings/effectconfig.idt defined type */
#if !defined(EFFECTCONFIG_IDT)
#define EFFECTCONFIG_IDT

#include <samplefreq.idt.h>
#include <common/channeltype.idt.h>

typedef struct t_xyuv_t_host_effect_format {
  t_sample_freq freq;
  t_uint16 nof_channels;
  t_uint16 nof_bits_per_sample;
  t_channel_type channel_mapping[MAXCHANNELS_SUPPORT];
  t_uint16 headroom;
  t_bool interleaved;
} t_host_effect_format;

typedef struct t_xyuv_t_host_effect_config {
  t_uint32 block_size;
  t_host_effect_format infmt;
  t_host_effect_format outfmt;
} t_host_effect_config;

#endif

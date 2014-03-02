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

/* Generated common/config.idt defined type */
#if !defined(_DMA_CONFIG_H_)
#define _DMA_CONFIG_H_


typedef struct t_xyuv_DmaConfig_t {
  void* buffer;
  t_uint16 buffer_size;
  void* buffer_ring;
  t_uint16 samplerate;
  void* sample_count_buffer;
  t_uint16 sample_count_buffer_size;
  t_uint16 nb_msp_channel_enabled;
} DmaConfig_t;

#endif

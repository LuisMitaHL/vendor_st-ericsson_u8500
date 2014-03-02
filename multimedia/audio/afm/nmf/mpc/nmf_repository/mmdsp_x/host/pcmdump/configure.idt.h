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

/* Generated pcmdump/configure.idt defined type */
#if !defined(__CONFIGURE_H_)
#define __CONFIGURE_H_

#include <host/bool.idt.h>

typedef struct t_xyuv_PcmDumpConfigure_t {
  t_uint8 nmf_port_idx;
  t_uint8 omx_port_idx;
  t_uint8 effect_position_idx;
  BOOL enable;
  void* buffer;
  t_uint24 buffer_size;
} PcmDumpConfigure_t;

#endif

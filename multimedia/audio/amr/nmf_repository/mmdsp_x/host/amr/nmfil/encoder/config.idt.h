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

/* Generated amr/nmfil/encoder/config.idt defined type */
#if !defined(__AMRENC_CONFIG_IDT__)
#define __AMRENC_CONFIG_IDT__

#include <host/memorypreset.idt.h>
#include <host/amr/nmfil/common.idt.h>

typedef struct t_xyuv_AmrEncParam_t {
  t_uint16 bNoHeader;
  t_memory_preset memory_preset;
} AmrEncParam_t;

typedef struct t_xyuv_AmrEncConfig_t {
  t_uint16 nBitRate;
  t_uint16 bDtxEnable;
  AmrPayloadFormat_e ePayloadFormat;
  t_uint16 bEfr_on;
} AmrEncConfig_t;

#endif

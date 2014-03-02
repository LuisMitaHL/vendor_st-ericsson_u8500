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

/* Generated amr/nmfil/decoder/config.idt defined type */
#if !defined(__AMRDEC_CONFIG_IDT__)
#define __AMRDEC_CONFIG_IDT__

#include <host/memorypreset.idt.h>
#include <host/amr/nmfil/common.idt.h>

typedef struct t_xyuv_AmrDecParams_t {
  t_uint16 bNoHeader;
  t_uint16 bErrorConcealment;
  t_memory_preset memory_preset;
} AmrDecParams_t;

typedef struct t_xyuv_AmrDecConfig_t {
  AmrPayloadFormat_e ePayloadFormat;
  t_uint16 bEfr_on;
} AmrDecConfig_t;

#endif

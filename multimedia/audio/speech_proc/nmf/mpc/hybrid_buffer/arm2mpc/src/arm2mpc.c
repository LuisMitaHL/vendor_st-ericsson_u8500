/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   arm2mpc.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <speech_proc/nmf/mpc/hybrid_buffer/arm2mpc.nmf>
#include <speech_proc/nmf/mpc/hybrid_buffer/hybrid_buffer.idt>

#include "inc/archi-wrapper.h"

#include <archi.h>
#define DATA24_DSP_BASE_ADDR        0x10000UL


void METH(emptyThisBuffer)(t_uint32 hybrid_buffer)
{
  Buffer_t * buffer = (Buffer_t *)hybrid_buffer;
  if(hybrid_buffer >= DATA24_DSP_BASE_ADDR) MMDSP_FLUSH_DCACHE();
  outputport.emptyThisBuffer(buffer);
}


void METH(fillThisBuffer)(Buffer_t *buffer)
{
  HybridBuffer_t *hybrid_buffer = (HybridBuffer_t *)buffer;
  arm.fillThisBuffer(hybrid_buffer->arm_addr);
}

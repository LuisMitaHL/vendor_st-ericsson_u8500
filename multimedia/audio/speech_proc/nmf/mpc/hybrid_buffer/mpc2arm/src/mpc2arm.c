/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mpc2arm.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <speech_proc/nmf/mpc/hybrid_buffer/mpc2arm.nmf>
#include <speech_proc/nmf/mpc/hybrid_buffer/hybrid_buffer.idt>


#include <archi.h>
#define DATA24_DSP_BASE_ADDR        0x10000UL
typedef volatile __MMIO struct {
    t_uword mode;
    t_uword control;
    t_uword way;
    t_uword line;
    t_uword command;
    t_uword status;
    t_uword cptr1l;
    t_uword cptr1h;
    t_uword cptr2l;
    t_uword cptr2h;
    t_uword cptr3l;
    t_uword cptr3h;
    t_uword cptrsel;
} t_dcache_regs;

#define MMDSP_FLUSH_DATABUFFER() {*(volatile __MMIO t_uword *)0xF804 = 1;}
#define MMDSP_FLUSH_DCACHE() {t_dcache_regs * pDcache = (t_dcache_regs *)0xEC05; pDcache->command = (t_uint16)0x7U; wnop(); wnop(); MMDSP_FLUSH_DATABUFFER();}




void METH(fillThisBuffer)(t_uint32 hybrid_buffer)
{
  Buffer_t * buffer = (Buffer_t *)hybrid_buffer;
  if(hybrid_buffer >= DATA24_DSP_BASE_ADDR) MMDSP_FLUSH_DCACHE();
  inputport.fillThisBuffer(buffer);
}


void METH(emptyThisBuffer)(Buffer_t *buffer)
{
  HybridBuffer_t *hybrid_buffer = (HybridBuffer_t *)buffer;
  arm.emptyThisBuffer(hybrid_buffer->arm_addr);
}

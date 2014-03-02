/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _CKG_API_H_
#define _CKG_API_H_

/*
 * Includes                                                            
 */
#include <stwdsp.h>             /* for mmdsp intrinsics */
#include "macros.h"            /* for assertion function in debug mode */
#include "t1xhv_types.h"        /* for basic types definitions */
#include "t1xhv_hamac_xbus_regs.h"
#include "mmdsp_api.h"            /* for assertion function in debug mode */


/*
 * Defines                             
 */
#define ALL_BLOCKS 0xFFFFU

/* CKG_CKEN1 */
#define CKG_MTF       0x1
#define CKG_VPP       0x2
#define CKG_DFI       0x4
#define CKG_BDU       0x8
#define CKG_BPU       0x10
#define CKG_IMC       0x20
#define CKG_IPA       0x40
#define CKG_H264VLC   0x80
#define CKG_H264FITQ  0x100
#define CKG_DCT       0x1000
#define CKG_IDCT      0x2000
#define CKG_Q         0x4000
#define CKG_IQ        0x8000U

/* CKG_CKEN2 */
#define CKG_SCN       0x1
#define CKG_RLC       0x2
#define CKG_IACDC     0x4
#define CKG_PA        0x8
#define CKG_REC       0x10
#define CKG_FIFO      0x20
#define CKG_QTAB      0x40
#define CKG_VC1IT     0x80
#define CKG_DMA       0x100
#define CKG_VBUF      0x200
#define CKG_FIFO2     0x400
#define CKG_VLC       0x800
#define CKG_IME       0x1000
#define CKG_HED       0x2000
#define CKG_CUP       0x4000


/*
 * Variables
 */                            
/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get CKG_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_CKG(a) (*(volatile MMIO t_uint16 *)(CKG_BASE+a))
#define HW_CKG_WAIT(a) (*(volatile __XIO t_uint16 *)(CKG_BASE+a))


/*****************************************************************************/
/** \brief 	Write a register from CKG - 16 bits
 *  \param	addr	Address to write (offset) 
 *  \param	value	Value to write						
 */
/*****************************************************************************/
#pragma inline 
static void CKG_SET_REG(t_uint16 addr, t_uint16 value)
{
    HW_CKG(addr) = value;
}

#pragma inline 
static t_uint16 CKG_GET_REG(t_uint16 addr)
{
    return HW_CKG(addr);
}

#pragma inline
static void CKG_ENABLE_CLOCK(t_uint16 enabled_clock1, t_uint16 enabled_clock2)
{
    CKG_SET_REG(CKG_CKEN_L,enabled_clock1);
    CKG_SET_REG(CKG_CKEN_H,enabled_clock2);
}




#define LOW_POWER
#ifdef LOW_POWER

#pragma inline
static void CKG_WAKE_UP_FROM_DEEP_SLEEP(void)
{
  /* DO NOTHING ! */
}


#pragma inline
static void CKG_GO_TO_DEEP_SLEEP(void)
{
  t_uint16 saved_cken1, saved_cken2;

  saved_cken1 = CKG_GET_REG(CKG_CKEN_L);
  saved_cken2 = CKG_GET_REG(CKG_CKEN_H);

  CKG_SET_REG(CKG_CKEN_L, saved_cken1 & (CKG_MTF|CKG_VPP));
  CKG_SET_REG(CKG_CKEN_H, saved_cken2 & CKG_DMA);
}

#ifndef __T1XHV_NMF_ENV
#pragma inline
static void CKG_ENABLE_CLOCKS_VLC_MP4VEC(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BPU|CKG_IMC|CKG_DCT|CKG_IDCT|CKG_Q|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_RLC|CKG_PA|CKG_REC|CKG_FIFO|CKG_FIFO2| CKG_VLC | CKG_IME);
}

#pragma inline
static void CKG_ENABLE_CLOCKS_NOVLC_MP4VEC(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BPU|CKG_IMC|CKG_DCT|CKG_IDCT|CKG_Q|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_RLC|CKG_PA|CKG_REC|CKG_FIFO|CKG_FIFO2|CKG_IME);
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_SH_intra(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO);
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_SH_inter(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO);
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype0_intra(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC);
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype0_inter(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC);
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype1_intra(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC|CKG_QTAB);
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype1_inter(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC|CKG_QTAB);
}


#pragma inline
static void CKG_ENABLE_CLOCKS_MP2VDC(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC|CKG_QTAB);
}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVEC(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BPU|CKG_IMC|CKG_DCT|CKG_Q|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_RLC|CKG_FIFO2|CKG_QTAB|CKG_VLC);
}


/*
#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); 
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_QTAB);
}
*/

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_NORESIZE(void)
{

 t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_PA|CKG_REC|CKG_QTAB);

}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_RESIZE_8(void)
{

t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_FIFO2|CKG_QTAB);

}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_RESIZE(void)
{

  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  tmp = tmp&(CKG_VPP); /* don't touch VPP clock */
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ|CKG_MTF);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_SCN|CKG_FIFO|CKG_QTAB);

}
#endif //#ifndef __T1XHV_NMF_ENV


#pragma inline
static void CKG_ENABLE_CLOCKS_H264VEC(void)
{
  t_uint16 tmp;
  t_uint16 EMU_unit_maskit_prev = EMU_unit_maskit;

  /* start of  critical code section*/
  MMDSP_EMU_MASKIT(INTERRUPT_DISABLE);
      
  tmp = CKG_GET_REG(CKG_CKEN_L);
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_VPP|CKG_DFI|CKG_BPU|CKG_IMC|CKG_MTF|CKG_IPA|CKG_H264VLC|CKG_H264FITQ);

  tmp = CKG_GET_REG(CKG_CKEN_H);
  tmp = tmp&(CKG_HED); /* don't touch HED clock */  
  CKG_SET_REG(CKG_CKEN_H, tmp|CKG_DMA|CKG_PA|CKG_REC|CKG_FIFO|CKG_FIFO2|CKG_SCN|CKG_IME|CKG_VBUF|CKG_CUP);

  /*  end of critical code section */
  /* if we were in a critical code section before this function, we remain critical*/
  MMDSP_EMU_MASKIT(EMU_unit_maskit_prev);
}

#pragma inline
static void CKG_ENABLE_CLOCKS_H264VDC(t_uint16 HED_used)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_VPP|CKG_DFI|CKG_BDU|CKG_IMC|CKG_MTF|CKG_IPA|CKG_H264FITQ|CKG_BPU);
  CKG_SET_REG(CKG_CKEN_H, CKG_DMA|CKG_PA|CKG_REC|CKG_SCN|CKG_CUP|(HED_used?CKG_HED:0));  
}

#pragma inline
static void CKG_ENABLE_CLOCKS_VC1VDC(void)
{
  t_uint16 tmp;
  t_uint16 EMU_unit_maskit_prev = EMU_unit_maskit;
      
  /* start of  critical code section*/
  MMDSP_EMU_MASKIT(INTERRUPT_DISABLE);

  tmp = CKG_GET_REG(CKG_CKEN_L);
  CKG_SET_REG(CKG_CKEN_L, tmp|CKG_VPP|CKG_IQ|CKG_DFI|CKG_BDU|CKG_IMC|CKG_MTF|CKG_BPU);

  tmp = CKG_GET_REG(CKG_CKEN_H);
  tmp = tmp&(CKG_HED); /* don't touch HED clock */
  CKG_SET_REG(CKG_CKEN_H, tmp|CKG_IACDC|CKG_DMA|CKG_PA|CKG_REC|CKG_SCN|CKG_VC1IT|CKG_VBUF|CKG_CUP);    

  /*  end of critical code section */
  /* if we were in a critical code section before this function, we remain critical*/
  MMDSP_EMU_MASKIT(EMU_unit_maskit_prev);
}


#pragma inline
static void CKG_ENABLE_CLOCKS_DPL(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  CKG_SET_REG(CKG_CKEN_L, tmp | CKG_VPP);
}

#pragma inline
static void CKG_DISABLE_CLOCKS_DPL(void)
{
  t_uint16 tmp;
  tmp = CKG_GET_REG(CKG_CKEN_L);
  CKG_SET_REG(CKG_CKEN_L, tmp & ~CKG_VPP);  
}

#ifdef __T1XHV_NMF_ENV
/****************** Start of NMF APIs  *************/
//APIs for enabling the HW blocks
static void CKG_ENABLE_CLOCKS_VPP_NMF(void)
{
	t_uint16 tmp_l, tmp1;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l | CKG_VPP);
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_VLC_MP4VEC_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BPU|CKG_IMC|CKG_DCT|CKG_IDCT|CKG_Q|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_RLC|CKG_PA|CKG_REC|CKG_FIFO|CKG_FIFO2| CKG_VLC | CKG_IME;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_NOVLC_MP4VEC_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BPU|CKG_IMC|CKG_DCT|CKG_IDCT|CKG_Q|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_RLC|CKG_PA|CKG_REC|CKG_FIFO|CKG_FIFO2|CKG_IME;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_SH_intra_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_SH_inter_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype0_intra_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype0_inter_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype1_intra_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC|CKG_QTAB;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype1_inter_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC|CKG_QTAB;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}


#pragma inline
static void CKG_ENABLE_CLOCKS_MP2VDC_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC|CKG_QTAB;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVEC_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BPU|CKG_IMC|CKG_DCT|CKG_Q;
	tmp2 = tmp_h|CKG_SCN|CKG_RLC|CKG_FIFO2|CKG_QTAB|CKG_VLC;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
}


#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_NORESIZE_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_PA|CKG_REC|CKG_QTAB;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_RESIZE_8_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BDU|CKG_IQ;
	tmp2 = tmp_h|CKG_FIFO2|CKG_QTAB;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_RESIZE_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = tmp_l|CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ;
	tmp2 = tmp_h|CKG_SCN|CKG_FIFO|CKG_QTAB;
	
	CKG_SET_REG(CKG_CKEN_L, tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp2);  
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_RM(void)
{
	t_uint16 tmp, tmp1, tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, CKG_MTF);
	CKG_SET_REG(CKG_CKEN_H, CKG_DMA);  
}


//APIs for disabling the HW blocks
#pragma inline
static void CKG_DISABLE_CLOCKS_VPP_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	
	tmp1 = ~CKG_VPP;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l & tmp1);
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_VLC_MP4VEC_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BPU|CKG_IMC|CKG_DCT|CKG_IDCT|CKG_Q|CKG_IQ;
	tmp2 = CKG_SCN|CKG_RLC|CKG_PA|CKG_REC|CKG_FIFO|CKG_FIFO2| CKG_VLC | CKG_IME;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_NOVLC_MP4VEC_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BPU|CKG_IMC|CKG_DCT|CKG_IDCT|CKG_Q|CKG_IQ;
	tmp2 = CKG_SCN|CKG_RLC|CKG_PA|CKG_REC|CKG_FIFO|CKG_FIFO2|CKG_IME;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_SH_intra_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ;
	tmp2 = CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_SH_inter_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ;
	tmp2 = CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_qtype0_intra_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ;
	tmp2 = CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_qtype0_inter_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ;
	tmp2 = CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_qtype1_intra_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ;
	tmp2 = CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC|CKG_QTAB;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_qtype1_inter_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ;
	tmp2 = CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC|CKG_QTAB;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}


#pragma inline
static void CKG_DISABLE_CLOCKS_MP2VDC_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BDU|CKG_IMC|CKG_IDCT|CKG_IQ;
	tmp2 = CKG_SCN|CKG_PA|CKG_REC|CKG_FIFO|CKG_IACDC|CKG_QTAB;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_JPGVEC_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BPU|CKG_IMC|CKG_DCT|CKG_Q;
	tmp2 = CKG_SCN|CKG_RLC|CKG_FIFO2|CKG_QTAB|CKG_VLC;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
}


#pragma inline
static void CKG_DISABLE_CLOCKS_JPGVDC_NORESIZE_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ;
	tmp2 = CKG_SCN|CKG_PA|CKG_REC|CKG_QTAB;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_JPGVDC_RESIZE_8_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BDU|CKG_IQ;
	tmp2 = CKG_FIFO2|CKG_QTAB;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_JPGVDC_RESIZE_NMF(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);
	
	tmp1 = CKG_DFI|CKG_BDU|CKG_IDCT|CKG_IQ;
	tmp2 = CKG_SCN|CKG_FIFO|CKG_QTAB;
	
	tmp1 = ~tmp1;
	tmp2 = ~tmp2;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2);   
	
}


#pragma inline
static void CKG_DISABLE_CLOCKS_RM(void)
{
	t_uint16 tmp_l, tmp_h, tmp1, tmp2;
	
	tmp_l = CKG_GET_REG(CKG_CKEN_L);
	tmp_h = CKG_GET_REG(CKG_CKEN_H);  
	
	tmp1 = ~CKG_MTF;
	tmp2 = ~CKG_DMA;
	
	CKG_SET_REG(CKG_CKEN_L, tmp_l&tmp1);
	CKG_SET_REG(CKG_CKEN_H, tmp_h&tmp2); 
	
}

/****************** End of NMF APIs  *************/
#endif //#ifdef __T1XHV_NMF_ENV

#else  /* NOT LOW POWER, don't use CKG */

#pragma inline
static void CKG_WAKE_UP_FROM_DEEP_SLEEP(void)
{
  CKG_ENABLE_CLOCK(ALL_BLOCKS,ALL_BLOCKS);
}

#pragma inline
static void CKG_GO_TO_DEEP_SLEEP(void)
{
  CKG_ENABLE_CLOCK(CKG_MTF|CKG_VPP,CKG_DMA);
}

#ifndef __T1XHV_NMF_ENV
#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VEC(void)
{
  
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC(void)
{
  
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP2VDC(void)
{
}


#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVEC(void)
{
  
}

/*
#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC(void)
{
  
}
*/
#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_NORESIZE(void)
{

}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_RESIZE_8(void)
{

}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_RESIZE(void)
{

}
#endif //#ifndef __T1XHV_NMF_ENV

#pragma inline
static void CKG_ENABLE_CLOCKS_H264VEC(void)
{
  
}

#pragma inline
static void CKG_ENABLE_CLOCKS_H264VDC(void)
{
  
}

#pragma inline
static void CKG_ENABLE_CLOCKS_VC1VDC(void)
{
  
}


#pragma inline
static void CKG_ENABLE_CLOCKS_DPL(void)
{
  
}


#pragma inline
static void CKG_DISABLE_CLOCKS_DPL(void)
{
  
}

#ifdef __T1XHV_NMF_ENV
/****************** Start of NMF APIs  *************/
//APIs for enabling the HW blocks
static void CKG_ENABLE_CLOCKS_VPP_NMF(void)
{
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_VLC_MP4VEC_NMF(void)
{

}

#pragma inline
static void CKG_ENABLE_CLOCKS_NOVLC_MP4VEC_NMF(void)
{
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_SH_intra_NMF(void)
{
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_SH_inter_NMF(void)
{
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype0_intra_NMF(void)
{
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype0_inter_NMF(void)
{
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype1_intra_NMF(void)
{
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_MP4VDC_qtype1_inter_NMF(void)
{
	
}


#pragma inline
static void CKG_ENABLE_CLOCKS_MP2VDC_NMF(void)
{
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVEC_NMF(void)
{

}


#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_NORESIZE_NMF(void)
{
	
}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_RESIZE_8_NMF(void)
{

}

#pragma inline
static void CKG_ENABLE_CLOCKS_JPGVDC_RESIZE_NMF(void)
{
}

#pragma inline
static void CKG_ENABLE_CLOCKS_RM(void)
{

}


//APIs for disabling the HW blocks
#pragma inline
static void CKG_DISABLE_CLOCKS_VPP_NMF(void)
{

}

#pragma inline
static void CKG_DISABLE_CLOCKS_VLC_MP4VEC_NMF(void)
{
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_NOVLC_MP4VEC_NMF(void)
{
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_SH_intra_NMF(void)
{
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_SH_inter_NMF(void)
{
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_qtype0_intra_NMF(void)
{
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_qtype0_inter_NMF(void)
{
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_qtype1_intra_NMF(void)
{
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_MP4VDC_qtype1_inter_NMF(void)
{
	
}


#pragma inline
static void CKG_DISABLE_CLOCKS_MP2VDC_NMF(void)
{
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_JPGVEC_NMF(void)
{
 
}


#pragma inline
static void CKG_DISABLE_CLOCKS_JPGVDC_NORESIZE_NMF(void)
{
	
}

#pragma inline
static void CKG_DISABLE_CLOCKS_JPGVDC_RESIZE_8_NMF(void)
{

}

#pragma inline
static void CKG_DISABLE_CLOCKS_JPGVDC_RESIZE_NMF(void)
{
}


#pragma inline
static void CKG_DISABLE_CLOCKS_RM(void)
{
}

/****************** End of NMF APIs  *************/
#endif //#ifdef __T1XHV_NMF_ENV

#endif //LOW_POWER



#endif /* _CKG_API_H_ */


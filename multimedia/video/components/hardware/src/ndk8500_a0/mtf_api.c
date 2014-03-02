/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * Includes							       
 */
#include "mtf_api.h"
#ifndef T1XHV_SIA
#include "dma_api.h"
#else
#include "dma_sia_api.h"
#endif
#include "mmdsp_api.h"
#include "macros.h"

/**
 * Defines							       
 */

/**
 * Global Variables							       
 */


/*****************************************************************************/
/**
 * \brief 	reads a table from external memory via MTF
 * \author 	Jean-Marc Volle Cyril Enault Maurizio Colombo
 * \param 	addr_in: 64-bit AHB address where to read from
 *                    MUST be aligned on one 64 bit word 
 * \param 	*pt_dest: pointer to the table in MMDSP
 * \param 	len: size of the table in number of 16-bit words, (for historical
 *              reasons). No need to be aligned to word number modulo 4
 * \param 	incr: external address increment
 *              in number of 64-bits words  
 * \param       swap_cfg configure swaps to perform from 
 *                       a 64 bit 0x12345678abcdefgh word :
 *              MTF_NO_SWAP_BYTE_NO_SWAP_WORD64 : no swap done 0x12345678abcdefgh
 *              MTF_SWAP_BYTE_NO_SWAP_WORD64 : swap only byte  0x34127856cdabghef
 *              MTF_NO_SWAP_BYTE_SWAP_WORD64: swap word        0xghefcdab78563412
 *              MTF_SWAP_BYTE_SWAP_WORD64: swap word+bytes     0xefghabcd56781234
 *         
 * \note    MTF usage is made critical to ensure 2 task do not share the MTF
 *          For the Scheduler, the call is made from an already critical code, 
 *          so the interrupts do not need to be masked. They must not
 *          be un-masked once the write is performed. 
 * \note    To write data for the host (little endian), we must set swap_cfg 
 *          to MTF_SWAP_BYTE_...            
 * \note    This function is compiled only in dsp mode.   
 *    
 **/
/*****************************************************************************/
void mtf_read_table(t_uint32 addr_in,t_uint16 *pt_dest,t_uint16 len, 
                          t_uint16 incr,t_mtf_swap_cfg swap_cfg)
{
    
  /* this version does not handle unaligned addr_in */
  /* Only FIFO 16 is used */


    t_uint16 EMU_unit_maskit_prev; /*  current value of EMU_unit_maskit */
    volatile __XIO t_uint16 *ptr_mtf_r = 0;
    t_uint16 value;
    t_uint16 current_length1 = len;
    t_uint16 current_length2 = (8-(len%8))%8;
    t_uint16 tfl;
    t_uint32 read_value=0;
    t_uint32 read_value_32 = 0;
    t_uint16 pgm_dma_swap = 0;

    /* start of  critical code section*/
    EMU_unit_maskit_prev = EMU_unit_maskit;
    MMDSP_EMU_MASKIT(INTERRUPT_DISABLE);

    /* Check that transfer is completed */
#ifdef T1XHV_SIA
    while((DMA_GET_REG_32(DMA_ENR_L, DMA_ENR_H) & MTF_R) != 0);
#else
    while((DMA_GET_REG_32(DMA_ENR_L, DMA_ENR_H) & MTF_R) != 0);
#endif

	/* configure swap in DMA and MTF */
    switch (swap_cfg)
    {

      case MTF_NO_SWAP_BYTE_NO_SWAP_WORD64 :
		pgm_dma_swap = 0;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16); 
        break;
      case MTF_SWAP_BYTE_NO_SWAP_WORD64 :
		pgm_dma_swap = 0;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16_SWAP); 
        break;
      case MTF_NO_SWAP_BYTE_SWAP_WORD64 :
		pgm_dma_swap = 1;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16); 
        break;
      case MTF_SWAP_BYTE_SWAP_WORD64 :
		pgm_dma_swap = 1;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16_SWAP); 
        break;
      default:
        ASSERT(0);
      /* other not possible */
		break;
     }

#ifdef T1XHV_SIA
    read_value = DMA_GET_REG_32(DMA_BSM_L, DMA_BSM_H);
    DMA_SET_REG_32(DMA_BSM_L, DMA_BSM_H, (read_value & ~(t_uint32)MTF_R)|(pgm_dma_swap ? MTF_R : 0));
#else
    read_value = DMA_GET_REG_32(DMA_BSM_L, DMA_BSM_H);
    DMA_SET_REG_32(DMA_BSM_L, DMA_BSM_H, (read_value & ~(t_uint32)MTF_R)|(pgm_dma_swap ? MTF_R : 0));
#endif

    /* program DMA transfer */
    tfl = (current_length2 + current_length1) >>2; 
	/* Len is 16 bit word, tfl is 64 bit word */

	/* addr in must be aligned on 64 bits */
    DMA_SET_MTF_READ_PARAM((addr_in&(~0x7UL)),incr,tfl);

    /* start DMA transfer */
#ifdef T1XHV_SIA
    DMA_SET_REG_32(DMA_ENR_L, DMA_ENR_H, MTF_R);
    MTF_SET_REG(MTF_R_REG_ENR,0x1);
#else
    DMA_SET_REG_32(DMA_ENR_L, DMA_ENR_H, MTF_R);
    MTF_SET_REG(MTF_R_REG_ENR,0x1);
#endif

    /* get all values from MTF_R Fifo */
    while (current_length1--) {
        value = *ptr_mtf_r;	/* read from fifo */
        *pt_dest++ = value;	/* write to MMDSP table */
    }

    /* Flush the Fifo */
    while (current_length2--) {
        value = *ptr_mtf_r;	/* read from fifo */
    }

	/* MTF_SET_REG(MTF_R_REG_DIR,0x1);*/
    /*DMA_SET_REG(DMA_DIR_L,MTF_R);*/ /* removed according to latest HW spec */

    /*  end of critical code section */
    /* if we were in a critical code section before this function, we remain critical*/
    MMDSP_EMU_MASKIT(EMU_unit_maskit_prev);
}



/*****************************************************************************/
/**
 * \brief 	write a table in external memory via MTF, MTF is "locked" 
 *          during write
 * \author 	jean-marc volle
 * \param 	addr_out : 64-bit pointer to the address where to write to
 * \param 	*pt_src: pointer to the table in MMDSP memory
 * \param 	len: size of the table in 16-bits words (for historical reasons). 
 *              total transfer size must be a X of 4. 32bits words
 * \param 	incr: external address increment
 *              in number of 64 bit words  
 * \param       swap_cfg configure swaps to perform from 
 *                       a 64 bit 0x12345678abcdefgh word :
 *              MTF_NO_SWAP_BYTE_NO_SWAP_WORD64 : no swap done 0x12345678abcdefgh
 *              MTF_SWAP_BYTE_NO_SWAP_WORD64 : swap only byte  0x34127856cdabghef
 *              MTF_NO_SWAP_BYTE_SWAP_WORD64: swap word        0xghefcdab78563412
 *              MTF_SWAP_BYTE_SWAP_WORD64: swap word+bytes     0xefghabcd56781234
 * \note    MTF usage is made critical to ensure 2 task do not share the MTF
 *          For the Scheduler, the call is made from an already critical code,
 *          so the interrupts do not need to be masked. They must not
 *          be un-masked once the write is performed.
 * \note    To write data for the host (little endian), we must set swap_cfg 
 *          to MTF_SWAP_BYTE_...          
 * \note    This function is compiled only in dsp mode.   
 *
 **/
/*****************************************************************************/
void mtf_write_table(t_uint32 addr_out,t_uint16 *pt_src,t_uint16 len, 
                           t_uint16 incr,t_mtf_swap_cfg swap_cfg)

{

    t_uint16 EMU_unit_maskit_prev; /*  current value of EMU_unit_maskit */

    volatile __XIO t_uint16 *ptr_mtf_w = 0;
    t_uint16 value;
    t_uint32 read_value=0;
    t_uint16 pgm_dma_swap = 0;


    ASSERT((len%8) == 0);     /* length must be multiple of 8,16bit word or 4,32bit word */

    /* start of  critical code section*/
    EMU_unit_maskit_prev = EMU_unit_maskit;
    MMDSP_EMU_MASKIT(INTERRUPT_DISABLE);

    /* Check that transfer is completed */
#ifdef T1XHV_SIA
    while((DMA_GET_REG_32(DMA_ENR_L, DMA_ENR_H) & MTF_W) != 0);
#else
    while((DMA_GET_REG_32(DMA_ENR_L, DMA_ENR_H) & MTF_W) != 0);
#endif

    switch (swap_cfg)
    {

      case MTF_NO_SWAP_BYTE_NO_SWAP_WORD64 :
        pgm_dma_swap = 0;
		ptr_mtf_w = (volatile __XIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16);
        break;
      case MTF_SWAP_BYTE_NO_SWAP_WORD64 :
        pgm_dma_swap = 0;
		ptr_mtf_w = (volatile __XIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16_SWAP);
        break;
      case MTF_NO_SWAP_BYTE_SWAP_WORD64 :
        pgm_dma_swap = 1;
		ptr_mtf_w = (volatile __XIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16);
        break;
      case MTF_SWAP_BYTE_SWAP_WORD64 :
        pgm_dma_swap = 1;
		ptr_mtf_w = (volatile __XIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16_SWAP);
        break;

      default :
        ASSERT(0);
		break;
      /* other not possible */
	}

#ifdef T1XHV_SIA
    read_value = DMA_GET_REG_32(DMA_BSM_L, DMA_BSM_H);
    DMA_SET_REG_32(DMA_BSM_L, DMA_BSM_H, (read_value & ~(t_uint32)MTF_W)|(pgm_dma_swap ? MTF_W : 0));
#else
    read_value = DMA_GET_REG_32(DMA_BSM_L, DMA_BSM_H);
    DMA_SET_REG_32(DMA_BSM_L, DMA_BSM_H, (read_value & ~(t_uint32)MTF_W)|(pgm_dma_swap ? MTF_W : 0));
#endif
  
    /* program DMA transfer */
    DMA_SET_MTF_WRITE_PARAM(addr_out,incr,(len>>2));

    /* start DMA transfer */
#ifdef T1XHV_SIA
    DMA_SET_REG_32(DMA_ENR_L, DMA_ENR_H, MTF_W);
#else
    DMA_SET_REG_32(DMA_ENR_L, DMA_ENR_H, MTF_W);
#endif
	MTF_SET_REG(MTF_W_REG_ENR,0x1);

    /* write all values to MTF_W Fifo */
    while (len--) 
    {
          value = *pt_src++;	/* read from MMDSP table */
          *ptr_mtf_w = value;	/* write to fifo */
    }       

	/*MTF_SET_REG(MTF_W_REG_DIR,0x1);*/
    /*DMA_SET_REG(DMA_DIR,MTF_W);*/ /* removed according to latest HW spec */
    
    /*  end of critical code section */
    /**
     * if we were in a critical code section before this function, we 
     * remain critical
     */
    MMDSP_EMU_MASKIT(EMU_unit_maskit_prev);
        
}

/* END of mtf_api.c */

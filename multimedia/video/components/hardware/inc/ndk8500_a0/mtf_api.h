/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _MTF_API_H_
#define _MTF_API_H_

/*
 * Includes						       
 */
#include "macros.h"       /* defines LS_SHORT, MS_SHORT */
#include "t1xhv_types.h" 	/* Include for MMDSP types    */
#ifndef T1XHV_SIA 
#include "dma_api.h"      /* defines increment DMA      */
#else
#include "dma_sia_api.h"
#endif /* SIA */
#include <stwdsp.h>
#include "mmdsp_api.h"

/*
 * Defines							       
 */
/*
 * Types
 */
typedef enum
{
  MTF_NO_SWAP_BYTE_NO_SWAP_WORD64 = 0,   /**< \brief no swap byte, no swap word */
  MTF_NO_SWAP_BYTE_SWAP_WORD64 =1,     /**< \brief no swap byte, swap within the 64 bit word */
  MTF_SWAP_BYTE_NO_SWAP_WORD64 = 2,      /**< \brief swap byte, no swap word */
  MTF_SWAP_BYTE_SWAP_WORD64 = 3        /**< \brief swap byte, swap word */
} t_mtf_swap_cfg;

/*
 * Global Varibales
 */
/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get MTF_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_MTF(a) (*(volatile MMIO t_uint16 *)(MTF_BASE+a))
#define HW_MTF_WAIT(a) (*(volatile __XIO t_uint16 *)(MTF_BASE+a))


/*****************************************************************************/
/** @{ \name MMDSP MTF		 											
 */
/*****************************************************************************/
/*****************************************************************************/
/** \brief 	Read a register from MTF - 16 bits
 *  \param	addr	Address to read (offset) 
 *  \return	Value read					
 */
/*****************************************************************************/
#pragma inline
static t_uint16 MTF_GET_REG(t_uint16 addr)
{
    return (HW_MTF(addr));
}

/*****************************************************************************/
/** \brief 	Read a register from MTF - 32 bits
 *  \param	addr_lsb	Address to read (offset) LSB 
 *  \param	addr_msb	Address to read (offset) MSB 
 *  \return	value	output - Value read					
 */
/*****************************************************************************/
#pragma inline
static t_uint32 MTF_GET_REG_32(addr_lsb,addr_msb,value)
{
  value = HW_MTF(addr_msb); \
    value<<=16; value |= HW_MTF(addr_lsb);
    return ((t_uint32) ((t_uint32) HW_MTF(addr_msb)<<16U) |\
                        (t_uint32) HW_MTF(addr_lsb));
}


/*****************************************************************************/
/** \brief 	Write a register from MTF - 16 bits
 *  \param	addr	Address to write (offset) 
 *  \param	value	Value to write						
 */
/*****************************************************************************/
#pragma inline
static void MTF_SET_REG(t_uint16 addr, t_uint16 value)
{
    HW_MTF(addr)=value;
}


/*****************************************************************************/
/** \brief 	Write a register from MTF - 32 bits
 *  \param	addr_lsb	Address to read (offset) LSB 
 *  \param	addr_msb	Address to read (offset) MSB 
 *  \param	value	Value to write							
 */
/*****************************************************************************/
#pragma inline
static void MTF_SET_REG_32(t_uint16 addr_lsb,t_uint16 addr_msb, t_uint32 value)
{
    HW_MTF(addr_lsb) = LS_SHORT(value);
    HW_MTF(addr_msb) = MS_SHORT(value); 
}

void mtf_read_table(t_uint32 addr_in,
                     t_uint16 *pt_dest,
                     t_uint16 len,
                     t_uint16 incr,
                     t_mtf_swap_cfg swap_cfg);
void mtf_write_table(t_uint32 addr_out,
                     t_uint16 *pt_src,
                     t_uint16 len,
                     t_uint16 incr,
                     t_mtf_swap_cfg swap_cfg);


inline static 
void mtf_read_table_inlined(t_uint32 addr_in,t_uint16 * restrict pt_dest,t_uint16 len, 
                          t_uint16 incr,t_mtf_swap_cfg swap_cfg)
{
    
    t_uint16 EMU_unit_maskit_prev; /*  current value of EMU_unit_maskit */
    volatile __XIO t_uint16 *ptr_mtf_r = 0;
    volatile __MMIO t_uint16 *ptr_mtf_r2 = 0;
    t_uint16 value, value2;
	t_uint32 *pt_dest_long;
    t_uint16 i, ind;
    t_uint16 tfl;
    t_uint16 read_value=0;
    t_uint16 pgm_dma_swap = 0; /* /\* programmed values for mtf and dma swap *\/ */
    t_uint16 current_length1 = len;
    t_uint16 current_length2 = (8-(len%8))%8;

    /* start of  critical code section*/
    EMU_unit_maskit_prev = EMU_unit_maskit;
    MMDSP_EMU_MASKIT(INTERRUPT_DISABLE);

    /* Check that transfer is completed */
    while((DMA_GET_REG(DMA_ENR_L) & MTF_R) != 0) {
            ;
    }

    /* we program the swap mode */
    /* ------------------------ */

    /* the mtf perform swap short only if swap is programmed */
    /* the dma performs swap byte and swap short if swap is programmed */
    /* if we have to have independant swap byte and swap short we must ask the */
    /*mtf to swap short whenever a swap byte is asked  */
 
    switch (swap_cfg)
    {
       case MTF_NO_SWAP_BYTE_NO_SWAP_WORD64 :
		pgm_dma_swap = 0;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16); 
		ptr_mtf_r2 = (volatile __MMIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16); 
        break;
      case MTF_SWAP_BYTE_NO_SWAP_WORD64 :
		pgm_dma_swap = 0;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16_SWAP); 
		ptr_mtf_r2 = (volatile __MMIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16_SWAP); 
        break;
      case MTF_NO_SWAP_BYTE_SWAP_WORD64 :
		pgm_dma_swap = 1;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16); 
		ptr_mtf_r2 = (volatile __MMIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16); 
        break;
      case MTF_SWAP_BYTE_SWAP_WORD64 :
		pgm_dma_swap = 1;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16_SWAP); 
		ptr_mtf_r2 = (volatile __MMIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16_SWAP); 
        break;
      default:
        ASSERT(0);
      /* other not possible */
		break;
     }

    read_value = DMA_GET_REG(DMA_BSM_L);
    DMA_SET_REG(DMA_BSM_L, (read_value & ~(t_uint16)MTF_R)|(pgm_dma_swap ? MTF_R : 0));

    
    /* program DMA transfer */
    tfl = (current_length2 + current_length1) >>2;  /* Len is 16 bit word, tfl is 32 bit word */

    DMA_SET_MTF_READ_PARAM(addr_in,incr,tfl);

    /* start DMA transfer */
    DMA_SET_REG(DMA_ENR_L,MTF_R);
    MTF_SET_REG(MTF_R_REG_ENR,0x1);

    /* get all values from MTF_R Fifo */

	pt_dest_long = (t_uint32 *)pt_dest;

    while (current_length1>2*1023+1) {
		#pragma loop maxitercount(1023)
		for (i=0; i<1023; i++) {
		    t_uint32 value_long;
			value = *ptr_mtf_r;	/* read from fifo */
			value2 = *ptr_mtf_r2;	/* read from fifo */
			value_long = winsert_l(0, value2);
			value_long = winsert_h(value_long, value);
			*pt_dest_long++ = value_long;	/* write to MMDSP table */
		}
		current_length1-=2*1023;
	}
	#pragma loop maxitercount(1023)
	#pragma loop minitercount(1)
	for (ind = 0; ind < (current_length1)/2 ; ind++) {
	    t_uint32 value_long;
		value = *ptr_mtf_r;	/* read from fifo */
		value2 = *ptr_mtf_r2;	/* read from fifo */
		value_long = winsert_l(0, value2);
		value_long = winsert_h(value_long, value);
		*pt_dest_long++ = value_long;	/* write to MMDSP table */
	}
    if (current_length1&1) {
        value = *ptr_mtf_r;	/* read from fifo */
        *(t_uint16 *)pt_dest_long = value;	/* write to MMDSP table */
    }

    /* Flush the Fifo */
    #pragma loop maxitercount(1023)
	while (current_length2--) {
        value = *ptr_mtf_r;	/* read from fifo */
    }

    /*  end of critical code section */
    /* if we were in a critical code section before this function, we remain critical*/
    MMDSP_EMU_MASKIT(EMU_unit_maskit_prev);
}

inline static 
void mtf_read_table_inlined_1o8(t_uint32 addr_in,t_uint16 * restrict pt_dest,t_uint16 line, 
                          t_uint16 incr,t_mtf_swap_cfg swap_cfg)
{
    
    t_uint16 EMU_unit_maskit_prev; /*  current value of EMU_unit_maskit */
    volatile __XIO t_uint16 *ptr_mtf_r = 0;
    volatile __MMIO t_uint16 *ptr_mtf_r2 = 0;
    t_uint16 value, value2;
	t_uint32 *pt_dest_long;
    t_uint16 i, ind;
    t_uint16 tfl;
    t_uint16 read_value=0;
    t_uint16 pgm_dma_swap = 0; /* /\* programmed values for mtf and dma swap *\/ */

    /* start of  critical code section*/
    EMU_unit_maskit_prev = EMU_unit_maskit;
    MMDSP_EMU_MASKIT(INTERRUPT_DISABLE);

    /* Check that transfer is completed */
    while((DMA_GET_REG(DMA_ENR_L) & MTF_R) != 0) {
            ;
    }

    /* we program the swap mode */
    /* ------------------------ */

    /* the mtf perform swap short only if swap is programmed */
    /* the dma performs swap byte and swap short if swap is programmed */
    /* if we have to have independant swap byte and swap short we must ask the */
    /*mtf to swap short whenever a swap byte is asked  */
 
    switch (swap_cfg)
    {
       case MTF_NO_SWAP_BYTE_NO_SWAP_WORD64 :
		pgm_dma_swap = 0;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16); 
		ptr_mtf_r2 = (volatile __MMIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16); 
        break;
      case MTF_SWAP_BYTE_NO_SWAP_WORD64 :
		pgm_dma_swap = 0;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16_SWAP); 
		ptr_mtf_r2 = (volatile __MMIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16_SWAP); 
        break;
      case MTF_NO_SWAP_BYTE_SWAP_WORD64 :
		pgm_dma_swap = 1;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16); 
		ptr_mtf_r2 = (volatile __MMIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16); 
        break;
      case MTF_SWAP_BYTE_SWAP_WORD64 :
		pgm_dma_swap = 1;
		ptr_mtf_r = (volatile __XIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16_SWAP); 
		ptr_mtf_r2 = (volatile __MMIO t_uint16*)(MTF_BASE + MTF_R_FIFO_16_SWAP); 
        break;
      default:
        ASSERT(0);
      /* other not possible */
		break;
     }

    read_value = DMA_GET_REG(DMA_BSM_L);
    DMA_SET_REG(DMA_BSM_L, (read_value & ~(t_uint16)MTF_R)|(pgm_dma_swap ? MTF_R : 0));

    
    /* program DMA transfer */
    tfl = (line); /* Len is 16 bit word, tfl is 32 bit word */

    DMA_SET_MTF_READ_PARAM(addr_in,incr,tfl);

    /* start DMA transfer */
    DMA_SET_REG(DMA_ENR_L,MTF_R);
    MTF_SET_REG(MTF_R_REG_ENR,0x1);

    /* get all values from MTF_R Fifo */

	pt_dest_long = (t_uint32 *)pt_dest;

	#pragma loop maxitercount(1023)
	#pragma loop minitercount(1)
	for (ind = 0; ind < (line) ; ind++) {
		value = *ptr_mtf_r;	/* read from fifo */
		value2 = *ptr_mtf_r2;	/* read from fifo */
		value = *ptr_mtf_r;	/* read from fifo */
		value2 = *ptr_mtf_r2;	/* read from fifo */
		*pt_dest++ = value2;	/* write to MMDSP table */
	}
	
    /*  end of critical code section */
    /* if we were in a critical code section before this function, we remain critical*/
    MMDSP_EMU_MASKIT(EMU_unit_maskit_prev);
}

inline static
void mtf_write_table_inlined(t_uint32 addr_out,t_uint16 *pt_src,t_uint16 len, 
                           t_uint16 incr,t_mtf_swap_cfg swap_cfg)

{

    t_uint16 EMU_unit_maskit_prev; /*  current value of EMU_unit_maskit */

    volatile __XIO t_uint16 *ptr_mtf_w=0;
    volatile __MMIO t_uint16 *ptr_mtf_w2=0;
    t_uint16 value, value2;
    t_uint32 value_long;
	t_uint32 *pt_src_long;
    t_uint16 i, ind;
    t_uint16 read_value=0;
    t_uint16 pgm_dma_swap = 0;


    ASSERT((len%8) == 0);     /* length must be multiple of 8,16bit word or 4,32bit word */

    /* start of  critical code section*/
    EMU_unit_maskit_prev = EMU_unit_maskit;
    MMDSP_EMU_MASKIT(INTERRUPT_DISABLE);

    /* Check that transfer is completed */
    while((DMA_GET_REG(DMA_ENR_L) & MTF_W) != 0) {
	      ;
    }


    switch (swap_cfg)
    {

      case MTF_NO_SWAP_BYTE_NO_SWAP_WORD64 :
        pgm_dma_swap = 0;
		ptr_mtf_w = (volatile __XIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16);
		ptr_mtf_w2 = (volatile __MMIO t_uint16*)(MTF_BASE + MTF_W_FIFO_16); 
        break;
      case MTF_SWAP_BYTE_NO_SWAP_WORD64 :
        pgm_dma_swap = 0;
		ptr_mtf_w = (volatile __XIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16_SWAP);
		ptr_mtf_w2 = (volatile __MMIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16_SWAP);
        break;
      case MTF_NO_SWAP_BYTE_SWAP_WORD64 :
        pgm_dma_swap = 1;
		ptr_mtf_w = (volatile __XIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16);
		ptr_mtf_w2 = (volatile __MMIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16);
        break;
      case MTF_SWAP_BYTE_SWAP_WORD64 :
        pgm_dma_swap = 1;
		ptr_mtf_w = (volatile __XIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16_SWAP);
		ptr_mtf_w2 = (volatile __MMIO t_uint16 *)(MTF_BASE + MTF_W_FIFO_16_SWAP);
        break;

      default :
        ASSERT(0);
		break;
      /* other not possible */
	}

    read_value = DMA_GET_REG(DMA_BSM_L);
    DMA_SET_REG(DMA_BSM_L, (read_value & ~(t_uint16)MTF_W)|(pgm_dma_swap ? MTF_W : 0));
  
    /* program DMA transfer */
    DMA_SET_MTF_WRITE_PARAM(addr_out,incr,(len>>2));

    /* start DMA transfer */
    DMA_SET_REG(DMA_ENR_L,MTF_W);
    MTF_SET_REG(MTF_W_REG_ENR,0x1);

    /* write all values to MTF_W Fifo */
	pt_src_long = (t_uint32 *)pt_src;
	value_long = *pt_src_long++;	/* read from MMDSP table */
	value = wextract_h(value_long);
	value2 = wextract_l(value_long);
	/* We start with xio write to avoid nops */
	while (len>2*1023+1) {
		#pragma loop maxitercount(1023)
		for (i=0; i<1023; i++) {
			*ptr_mtf_w = value;	/* write to fifo */
			*ptr_mtf_w2 = value2;	/* write to fifo */
			value_long = *pt_src_long++;	/* read from MMDSP table */
			value = wextract_h(value_long);
			value2 = wextract_l(value_long);
		}
		len-=2*1023;
	}
	#pragma loop maxitercount(1023)
	for (ind = 0; ind < (len & (1023*2+1))/2 ; ind++) {
		*ptr_mtf_w = value;	/* write to fifo */
		*ptr_mtf_w2 = value2;	/* write to fifo */
		value_long = *pt_src_long++;	/* read from MMDSP table */
		value = wextract_h(value_long);
		value2 = wextract_l(value_long);
	}
	if (len&1) {
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

/** @}*/
#endif /* _MTF_API_H_ */


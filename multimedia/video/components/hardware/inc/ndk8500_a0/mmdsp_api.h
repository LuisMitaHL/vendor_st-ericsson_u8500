/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _MMDSP_API_H_
#define _MMDSP_API_H_

/*****************************************************************************/
/*
 * Includes							       
 */
/*****************************************************************************/

#include <stwdsp.h>	                  /* Standard include for MMDSP  */
#include <archi.h>                    /* for EMU_unit_maskit */
#include "t1xhv_types.h"              /* Type definition                      */
#include "macros.h"
#ifndef T1XHV_SIA
#include "new_regs.h" 
#endif

/*****************************************************************************/
/*
 * Types							       
 */
/*****************************************************************************/

/** @{ \name Mask interrupts values
 *     \warning DO NOT change enum values
 */
typedef enum
{
    INTERRUPT_DISABLE = 1, /**< \brief Interrupts are disable */
    INTERRUPT_ENABLE  = 0  /**< \brief Interrupts are enable  */
} t_mmdsp_interrupt;

/** @} end of  Mask interrupts values*/


/*****************************************************************************/
/*
 * Variables							       
 */
/*****************************************************************************/


#ifdef MMDSP
#define HW_CFG(a) (*(volatile t_uint16 *)(a))

/**
 * Static variable which read replaces a nop, for flexcc2
 */
#ifdef __T1XHV_NMF_ENV
static volatile int nop;
#else
extern volatile int nop;
#endif
#endif



/*****************************************************************************/
/** @{ \name MMDSP Host CFG 											
  */
/*****************************************************************************/
/*****************************************************************************/
/** \brief 	Add NOP operation
 *  \return	none			
 */
/*****************************************************************************/
#pragma inline
static MMDSP_NOP()
{
    nop;
}
/*****************************************************************************/
/** \brief 	Read a register from CFG - 16 bits
 *  \param	addr	Address to read (offset) 
 *  \return	value		output - Value read				
 */
/*****************************************************************************/
/* #define CFG_READ_REG(addr,value) value = hw_host_cfg[addr] */
#pragma inline
static t_uint16 CFG_GET_REG(t_uint16 addr)
{
    
  return HW_CFG(addr);
}

/*****************************************************************************/
/** \brief 	Read a register from CFG - 32 bits
 *  \param	addr_lsb	Address to read (offset) LSB 
 *  \param 	addr_msb	Address to read (offset) MSB 
 *  \return  value	output - Value read					
 */
/*****************************************************************************/
/* #define CFG_READ_REG_32(addr_lsb,addr_msb,value) value = hw_host_cfg[addr_msb]; \ */
/*     value<<=16; value |= hw_host_cfg[addr_lsb]; */

#pragma inline
static t_uint32 CFG_GET_REG_32(t_uint16 addr_lsb, t_uint16 addr_msb)
{
    t_uint32 lsb,msb;
    msb = HW_CFG(addr_msb);
    lsb = HW_CFG(addr_lsb);
    return ((msb << 16) | lsb);
}

/*****************************************************************************/
/** \brief 	Write a register from CFG - 16 bits
 *  \param	addr	Address to write (offset) 
 *  \param	value	Value to write						
 */
/*****************************************************************************/
/* #define CFG_WRITE_REG(addr,value) hw_host_cfg[addr] = value */
#pragma inline 
static void CFG_SET_REG(t_uint16 addr, t_uint16 value)
{
  HW_CFG(addr) = value;
}

/*****************************************************************************/
/** \brief 	Write a register from CFG - 32 bits
 *  \param	addr_lsb	Address to read (offset) LSB 
 *  \param	addr_msb	Address to read (offset) MSB 
 *  \param	value	Value to write						
 */
/*****************************************************************************/
/* #define CFG_WRITE_REG_32(addr_lsb,addr_msb,value) hw_host_cfg[addr_lsb] = wextract_l(value); \ */
/*     hw_host_cfg[addr_msb] = wextract_h(value);  */

#pragma inline
static void CFG_SET_REG_32(t_uint16 addr_lsb, t_uint16 addr_msb, t_uint32 value)
{
  HW_CFG(addr_lsb) = LS_SHORT(value);
  HW_CFG(addr_msb) = MS_SHORT(value);

}

/** @} */



/*****************************************************************************/
/** \brief 	Mask MMDSP interrupts
 *  \param	mask: Mask or unmask interrupts
 */
/*****************************************************************************/
#pragma inline
static void MMDSP_EMU_MASKIT(t_mmdsp_interrupt mask)
{
    /* Mask or unmask interrupts */
	EMU_unit_maskit = mask;

    /* To take in account interrupt change */
	MMDSP_NOP();
	MMDSP_NOP();
  /* After masking an interrupts (either using EMU_unit_maskit or ITREMAP_interf_msk registers), three nops must be inserted (not two as specified before). */
	MMDSP_NOP();
}


/*****************************************************************************/
/** \brief 	Remap MMDSP interrupts 
 *  \brief       this function is specific to 8820 SVA
 *  \brief       in line with 8820 spec  
 */
/*****************************************************************************/
#pragma inline
static void t1xhv_remap_interrupts()
{
  ITREMAP_interf_reg2 = 59;       /*  2) IRQ_CMP0 : Encode\n  */
  ITREMAP_interf_reg3 = 60;       /*  3) IRQ_CMP1 : Decode\n  */ 
  ITREMAP_interf_reg4 = 61;       /*  4) IRQ_CMP2 : NU\n    */
  ITREMAP_interf_reg5 = 62;       /*  5) IRQ_CMP3 : Display VPP\n */
  ITREMAP_interf_reg6 = 54;       /*  6) IRQ_CTL0 : Encode\n  */
  ITREMAP_interf_reg7 = 55;       /*  7) IRQ_CTL1 : Decode\n  */
  ITREMAP_interf_reg8 = 56;       /*  8) IRQ_CTL2 : NU\n    */
  ITREMAP_interf_reg9 = 57;       /*  9) IRQ_CTL3 : Display VPP\n */
  ITREMAP_interf_reg10 = 58;      /*  10) IRQ_CTL4 : NU\n     */

  ITREMAP_interf_reg11 = 39;      /*  11) DMA_IRQ_EOT \n   */
  ITREMAP_interf_reg20 = 42;      /*  20) VPP_SYNC\n          */
  ITREMAP_interf_reg21 = 41;      /*  21) VPP_IRQ_EOT\n       */ 
  ITREMAP_interf_reg26 = 43;      /*  26) MECC_IRQ\n          */
  ITREMAP_interf_reg29 = 37;      /*  29) DISP_SYNC0\n         */

  ITREMAP_interf_reg22 = 45;      /*  22) HED interrupt      */

  /* to memorize ITs */
  ITREMAP_interf_itmemol=0xFFFFU;
  ITREMAP_interf_itmemoh=0xFFFFU;
}


/*****************************************************************************/
/** \brief 	Remap MMDSP interrupts for SIA
 */
/*****************************************************************************/
#pragma inline
static void t1xhv_sia_remap_interrupts()
{
  /* ITREMAP_interf_reg0 = ? */
  /* ITREMAP_interf_reg1 = ? */

    ITREMAP_interf_reg2 = 59;       /*  2) IRQ_CMP0 : N/A */
    ITREMAP_interf_reg3 = 60;       /*  3) IRQ_CMP1 : N/A */ 
    ITREMAP_interf_reg4 = 61;       /*  4) IRQ_CMP2 : N/A */
    ITREMAP_interf_reg5 = 62;       /*  5) IRQ_CMP3 : N/A */
    ITREMAP_interf_reg6 = 54;       /*  6) IRQ_CTL0 : N/A  */
    ITREMAP_interf_reg7 = 55;       /*  7) IRQ_CTL1 : N/A  */
    ITREMAP_interf_reg8 = 56;       /*  8) IRQ_CTL2 : "GRAB" */
    ITREMAP_interf_reg9 = 57;       /*  9) IRQ_CTL3 : N/A */
    ITREMAP_interf_reg10 = 58;      /*  10) IRQ_CTL4 : N/A */
    ITREMAP_interf_reg11 = 46;       /*  11) DMA_IRQ_EOT */

    ITREMAP_interf_reg30 = 40;       /*  30) IRP_INDIRECT_MCU_ACK_IRQ\n  */

  /* to memorize ITs */
  ITREMAP_interf_itmemol=0xFFFFU;
  ITREMAP_interf_itmemoh=0xFFFFU;
}


#endif /* _MMDSP_API_H_ */


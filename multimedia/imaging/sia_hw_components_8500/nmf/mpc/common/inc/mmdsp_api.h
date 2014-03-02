/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* \brief   MMDSP specific macros
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
#include <inc/type.h>                 /* provided by NMF                 */

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


#ifdef __pic__
extern volatile t_uint16 *hw_host_cfg;
#else
extern volatile t_uint16 hw_host_cfg[512];   /**<\brief CFG zone (shared with host) */
#endif


/**
 * Static variable which read replaces a nop, for flexcc2
 */
static volatile int nop;

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
    nop;
    nop;
    nop;
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

    ITREMAP_interf_reg25 = 43;	     /* 25) IRP_SENSOR_MODE_ERR_IRQ */

    ITREMAP_interf_reg29 = 41;       /*  29) IRP_READY_IRQ\n  */

    ITREMAP_interf_reg30 = 40;       /*  30) IRP_INDIRECT_MCU_ACK_IRQ\n  */

  /* to memorize ITs */
  ITREMAP_interf_itmemol=0xFFFFU;
  ITREMAP_interf_itmemoh=0xFFFFU;
}


#endif /* _MMDSP_API_H_ */


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* \brief    All itc inline functions
*/

#ifndef _ITC_API_H
#define _ITC_API_H

/*
 * Include
 */

#include <archi.h>	           /* For mmdsp definition           */
#include <inc/type.h>          /* provided by NMF                */
#include "mmdsp_api.h"         /* For un/mask interrupts         */
#include <stwdsp.h>            /* for MMDSP instrinsics          */

#define ITC_BASE             0xDE00U

/* -------------------------------------------------------------------- */
/*  Interrupt Controller sub-addresses */
/* -------------------------------------------------------------------- */
/* noexp register name              base + offset       size    def value   acces   type    xiordy */
#define CFG_TIM_L            0x0000U
#define CFG_TIM_H            0x0001U
#define CFG_TIC_L            0x0002U
#define CFG_TIC_H            0x0003U

/* noexp register name              base + offset       size    def value   acces   type    xiordy */
#define ITC_CMP0_L           0x0100U
#define ITC_CMP0_H           0x0101U
#define ITC_CMP1_L           0x0102U
#define ITC_CMP1_H           0x0103U
#define ITC_CMP2_L           0x0104U
#define ITC_CMP2_H           0x0105U
#define ITC_CMP3_L           0x0106U
#define ITC_CMP3_H           0x0107U
#define CFG_ISS_L            0x0110U
#define GRB_STS_L            0x0190U
#define GRB_ISS_L            0x0192U

/*
 * Global variable
 */

/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get ITC_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_ITC(a) (*(volatile MMIO t_uint16 *)(ITC_BASE+a))
#define HW_ITC_WAIT(a) (*(volatile __XIO t_uint16 *)(ITC_BASE+a))


/*****************************************************************************/
/** \brief Read a register from ITC - 16 bits
 *  \param reg_name: register to read (offset)
 *  \return Value read
 */
/*****************************************************************************/
#pragma inline
static t_uint16 ITC_GET_REG(t_uint16 reg_name)
{
    return HW_ITC(reg_name);
}

/*****************************************************************************/
/** \brief Read TIMER from ITC - 32 bits
 *  \return timer value
 *
 * Read lsb first to stop and latch timer
 */
/*****************************************************************************/
#pragma inline
static t_uint32 ITC_GET_TIMER_32()
{
   t_uint16 lsb;
   lsb = HW_ITC(CFG_TIC_L);

   return (((t_uint32)HW_ITC(CFG_TIC_H) << 16) | (t_uint32)lsb);
}

/* mode24 version of above function */
#pragma inline
static t_uint32 ITC_GET_TIMER_32_24()
{
    t_uint16 lsb;
    lsb = HW_ITC(CFG_TIC_L);
    return (((t_uint32)HW_ITC(CFG_TIC_H) << 16) | ((t_uint32)lsb&0xFFFFUL));
}

/*****************************************************************************/
/** \brief Write a register from ITC - 16 bits
 *  \param reg_name: Register to write
 *  \param value: value to write
 */
/*****************************************************************************/
#pragma inline
static void ITC_SET_REG(t_uint16 reg_name,
                        t_uint16 value)
{
    HW_ITC(reg_name) = value;
}

/*****************************************************************************/
/** \brief 	Write a register from ITC - 32 bits
 *  \param reg_lsb: register to write LSB
 *  \param reg_msb: register to write MSB
 *  \param value: Value to write (32 bits)
 */
/*****************************************************************************/
#pragma inline
static void ITC_SET_REG_32(t_uint16 reg_lsb,
                           t_uint16 reg_msb,
                           t_uint32 value)
{
    HW_ITC(reg_lsb) = wextract_l(value);
    HW_ITC(reg_msb) = wextract_h(value);
}


/*****************************************************************************/
/** \brief 	Mask specific interrupt
 *  \param	mask : Mask interrupr
 *
 * Mask interrupt for less significant bits
 */
/*****************************************************************************/
#pragma inline
static void MASK_INTERRUPT_L(t_uint16 mask)
{
    t_uint16 mask_cmp;

    mask_cmp = ITREMAP_interf_itmskl;
    ITREMAP_interf_itmskl = mask_cmp | mask;
}


/*****************************************************************************/
/** \brief 	Unmask specific interrupt
 *  \param	mask : Unmask interrupt
 *
 * Unmask interrupt for less significant bits
 */
/*****************************************************************************/
#pragma inline
static void UNMASK_INTERRUPT_L(t_uint16 mask)
{
    t_uint16 mask_cmp;

    mask_cmp = ITREMAP_interf_itmskl;
    ITREMAP_interf_itmskl = mask_cmp & ~(t_uint16)(mask);
}


/*****************************************************************************/
/** \brief 	Mask specific interrupt
 *  \param	mask : Mask interrupt
 *
 * Mask interrupt for most significant bits
 */
/*****************************************************************************/
#pragma inline
static void MASK_INTERRUPT_H(t_uint16 mask)
{
    t_uint16 mask_cmp;

    mask_cmp = ITREMAP_interf_itmskh;
    ITREMAP_interf_itmskh = mask_cmp | mask;

    /* To take in account interrupt change */
    nop;
    nop;
    nop;
}


/*****************************************************************************/
/** \brief 	Unmask specific interrupt
 *  \param	mask : Unmask interrupt
 *
 * Unmask interrupt for most significant bits
 */
/*****************************************************************************/
#pragma inline
static void UNMASK_INTERRUPT_H(t_uint16 mask)
{
    t_uint16 mask_cmp;

    mask_cmp = ITREMAP_interf_itmskh;
    ITREMAP_interf_itmskh = mask_cmp & ~(t_uint16)(mask);
}

#endif /* _ITC_API_H_ */



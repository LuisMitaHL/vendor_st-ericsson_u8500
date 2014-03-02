/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _ITC_API_H
#define _ITC_API_H

/*
 * Include							       
 */

#include <archi.h>	                  /* For mmdsp definition           */
#include "t1xhv_types.h"              /* Type definition                */
#include "t1xhv_hamac_xbus_regs.h"    /* Register definition            */
#include "macros.h"                   /* MS_SHORT() and LS_SHORT(), XIO */
#include "mmdsp_api.h"                /* For un/mask interrupts         */
#include <stwdsp.h>

/* 
 * Global variable
 */

/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get ITC_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
#define HW_ITC(a) (*(volatile MMIO t_uint16 *)(ITC_BASE+a))
#define HW_ITC_WAIT(a) (*(volatile __XIO t_uint16 *)(ITC_BASE+a))



/*****************************************************************************/
/** \brief 	Loop on semaphore register until unlocked
 *  \param	reg_name: register to read (offset of semaphore) 
 *
 * When semaphore is equal to zero, semaphore is free.
 * With this loop semaphore is waited and taken.
 */
/*****************************************************************************/
#pragma inline
static void ITC_TAKE_SEMAPHORE(t_uint16 reg_name)
{
    t_uint16 sem_value = 1;

    while(sem_value)
    {
        sem_value = HW_ITC(reg_name);
    }
}

/*****************************************************************************/
/** \brief 	Unlock semaphore
 *  \param	reg_name: register to read (offset of semaphore) 
 *
 * Unlock semaphore
 */
/*****************************************************************************/
#pragma inline
static void ITC_DROP_SEMAPHORE(t_uint16 reg_name)
{
    HW_ITC(reg_name) = 0x1;
}

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
    HW_ITC(reg_lsb) = LS_SHORT(value); 
    HW_ITC(reg_msb) = MS_SHORT(value);
}


/*****************************************************************************/
/** \brief Generate an interrupt and update status
 *  \param reg_sta: STA register
 *  \param reg_sts: STS register
 *  \param reg_iss: ISS register
 *  \param mask:    Mask on interrupt to write
 */
/*****************************************************************************/
#pragma inline
static void ITC_GEN_INTERRUPT(t_uint16 reg_sta,
                              t_uint16 reg_sts,
                              t_uint16 reg_iss,
                              t_uint16 mask)
{
    t_uint16 status;
    t_uint16 EMU_unit_maskit_prev;

    /* Mask interrupts */
    EMU_unit_maskit_prev = EMU_unit_maskit;
    MMDSP_EMU_MASKIT(INTERRUPT_DISABLE);

    /* Read status */
    status = ITC_GET_REG(reg_sta);

    /* Added interrupt */
    status |= mask;
    ITC_SET_REG(reg_sts, status);
    ITC_SET_REG(reg_iss, mask);
                                  
    /* Unmask interrupts */
    MMDSP_EMU_MASKIT(EMU_unit_maskit_prev);
}


/*****************************************************************************/
/** \brief Generate an interrupt and update status
 *  \param reg_sta: STA register
 *  \param reg_sts: STS register
 *  \param mask:    Mask on interrupt to write
 */
/*****************************************************************************/
#pragma inline
static void ITC_SET_STATUS(t_uint16 reg_sta,
                           t_uint16 reg_sts,
                           t_uint16 mask)
{
    t_uint16 status;
    t_uint16 EMU_unit_maskit_prev;

    /* Mask interrupts */
    EMU_unit_maskit_prev = EMU_unit_maskit;
    MMDSP_EMU_MASKIT(EMU_unit_maskit);

    /* Read status */
    status = ITC_GET_REG(reg_sta);
    
    /* Added status */
    status |= mask;
    ITC_SET_REG(reg_sts, status);

    MMDSP_EMU_MASKIT(EMU_unit_maskit_prev);
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



/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* \brief    This files defines and implements the API for the IRP hardware block
*/

#ifndef _IRP_API_H_
#define _IRP_API_H_


/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include <stwdsp.h>

#include <inc/type.h> /* provided by NMF */
#include "crm_mmdsp.h" /* register definition */
#include "dma_mmdsp.h" /* register definition */
#include "pictor_mmdsp.h" /* register definition */
#include "proj_mmdsp.h" /* register definition */
#include "sia_mmdsp.h" /* register definition */
#include "stbp_mmdsp.h" /* register definition */
#include "mmdsp_api.h"
#include "ipp_api.h"

/* This is new in 8820, HW blocks memory mapping is no more handled
   through MMDSP Tools, now we get IPP_BASE from t1xhv_hamac_xbus_regs.h */
/* Two variants, with and without XIORDY */
/* Offset is 0x1000 because IRP registers in SIA are offset 0xB000, but header file
 already adds an offset of 0xA000 to register addresses. SNAFU. */

/* Offset ISP register is 0x9000 see in STn8500 SIA Hardware architecture specification/MMDSP+ memory mapping */
#define HW_IRP_WAIT(a) (*(volatile __XIO t_uint16 *)(0x9000U+a))

#define IRP_SET_REG(_Reg_, _Value_) HW_IRP_WAIT(_Reg_##_OFFSET) = (t_uint16)_Value_
#define IRP_SET32_REG(_Reg_, _Value_) irp_set32_reg(_Reg_##_OFFSET, _Value_)

#define IRP_GET_REG(_Reg_) (t_uint16) HW_IRP_WAIT(_Reg_##_OFFSET)
#define IRP_GET32_REG(_Reg_) irp_get32_reg(_Reg_##_OFFSET)

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
#define MTF_CMD_PACKET_SIZE 4

#define XP70_TRANSACTION_ID_MASK    0xff00U
#define XP70_OPCODE_WRITE       0x0001U
#define XP70_OPCODE_READ        0x0000U

#define XP70_ANY_ITM0_ACK       0xFFFFFFFFUL
#define XP70_ANY_ITM1_ACK       0xFFFFFFFFUL
#define XP70_ANY_ITM2_ACK       0xFFFFFFFFUL
#define XP70_ANY_ITM3_ACK       0xFFFFFFFFUL

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/**
 * \brief Sensor configuration information, used to configure IPP
*/

/*------------------------------------------------------------------------
 * Prototypes
 *----------------------------------------------------------------------*/

void write_page_element(t_uint16 address, t_uint32 value);
void read_page_element(t_uint16 address);
t_uint32 retrieve_read_page_element(void);

/*------------------------------------------------------------------------
 * variables
 *----------------------------------------------------------------------*/

extern volatile int  G_irp_pelement_is_16bits;
extern volatile t_uint32  G_grb_irp_bs;
extern volatile t_uint32  G_grb_irp_be;
#ifdef _MTF_
extern volatile t_uint32  G_grb_irp_ptr;
#else
extern t_uint16   __XIO *G_grb_irp_ptr;
#endif

extern volatile t_uint16  G_mtf_idx;
extern volatile t_uint16  mtf_cmd_packet[MTF_CMD_PACKET_SIZE];

/*------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief Read irp register
 * \param reg_name: register to read
 * \return Register read
 *
 */
/*****************************************************************************/
#pragma inline
static  void irp_set32_reg (t_uint16 reg_name,
                          t_uint32 reg_value)

{
    HW_IRP_WAIT(reg_name)=wextract_l(reg_value);
    HW_IRP_WAIT(reg_name+1)=wextract_h(reg_value);
}

/****************************************************************************/
/**
 * \brief   Make a 32 bits register with 2 16 bits registers
 * \param   msp 16 bits signed word (MSB)
 * \param   lsp 16 bits signed word (LSB)
 * \return  32 bits unsigned word
 *
 */
/****************************************************************************/
asm long MAKE_LONG(int msp, int lsp)
{
    mv @{msp}, @{}.1
    mv @{lsp}, @{}.0
}

#pragma inline
static   t_uint32 irp_get32_reg (t_uint16 reg_name)
{
    return (t_uint32) MAKE_LONG(HW_IRP_WAIT(reg_name+1),HW_IRP_WAIT(reg_name));
}

/*****************************************************************************/
/**
 * \brief Disable MCU ACK IT
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_DISABLE_ITM0_IT(void)
{
  IRP_SET32_REG(ITM_FW_EVENT0_EN_BCLR, XP70_ANY_ITM0_ACK);
}

/*****************************************************************************/
/**
 * \brief Enable ITM0 interrupts
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_ENABLE_ITM0_IT(void)
{
  IRP_SET32_REG(ITM_FW_EVENT0_EN_BSET, XP70_ANY_ITM0_ACK);
}


/*****************************************************************************/
/**
 * \brief Reset ITM0 status interrupts
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_CLEAR_ITM0_STATUS_IT(t_uint32 clear_item)
{
  IRP_SET32_REG(ITM_FW_EVENT0_STATUS_BCLR, clear_item);
}


static  t_uint32 IRP_GET_ITM0_STATUS_INT()
{
    return (t_uint32) IRP_GET32_REG(ITM_FW_EVENT0_STATUS);

}


static  t_uint32 IRP_GET_ITM0_MASK_SET()
{
    return (t_uint32) IRP_GET32_REG(ITM_FW_EVENT0_EN_STATUS);

}

/*****************************************************************************/
/**
 * \brief Enable ITM1 interrupts
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_ENABLE_ITM1_IT(void)
{
  IRP_SET32_REG(ITM_FW_EVENT1_EN_BSET, XP70_ANY_ITM1_ACK);
}

/*****************************************************************************/
/**
 * \brief Disable ITM1 interrupts
 *  *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_DISABLE_ITM1_IT(void)
{
  IRP_SET32_REG(ITM_FW_EVENT1_EN_BCLR, XP70_ANY_ITM1_ACK);
}


/*****************************************************************************/
/**
 * \brief Reset ITM1 status interrupts
 *
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_CLEAR_ITM1_STATUS_IT(t_uint32 clear_item)
{
  IRP_SET32_REG(ITM_FW_EVENT1_STATUS_BCLR, clear_item);
}


static  t_uint32 IRP_GET_ITM1_STATUS_INT()
{
    return (t_uint32) IRP_GET32_REG(ITM_FW_EVENT1_STATUS);

}


static  t_uint32 IRP_GET_ITM1_MASK_SET()
{
    return (t_uint32) IRP_GET32_REG(ITM_FW_EVENT1_EN_STATUS);

}


static  t_uint32 IRP_GET_ITM2_MASK_SET()
{
    return (t_uint32) IRP_GET32_REG(ITM_FW_EVENT2_EN_STATUS);

}

/*****************************************************************************/
/**
 * \brief Enable ITM2 interrupts
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_ENABLE_ITM2_IT(void)
{
  IRP_SET32_REG(ITM_FW_EVENT2_EN_BSET, XP70_ANY_ITM2_ACK);
}

/*****************************************************************************/
/**
 * \brief Disable ITM2 interrupts
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_DISABLE_ITM2_IT(void)
{
  IRP_SET32_REG(ITM_FW_EVENT2_EN_BCLR, XP70_ANY_ITM2_ACK);
}


/*****************************************************************************/
/**
 * \brief Reset ITM2 status interrupts
 *
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_CLEAR_ITM2_STATUS_IT(t_uint32 clear_item)
{
  IRP_SET32_REG(ITM_FW_EVENT2_STATUS_BCLR, clear_item);
}


static  t_uint32 IRP_GET_ITM2_STATUS_INT()
{
    return (t_uint32) IRP_GET32_REG(ITM_FW_EVENT2_STATUS);

}


/*****************************************************************************/
/**
 * \brief Enable ITM3 interrupts
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_ENABLE_ITM3_IT(void)
{
  IRP_SET32_REG(ITM_FW_EVENT3_EN_BSET, XP70_ANY_ITM3_ACK);
}

/*****************************************************************************/
/**
 * \brief Disable ITM3 interrupts
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_DISABLE_ITM3_IT(void)
{
  IRP_SET32_REG(ITM_FW_EVENT3_EN_BCLR, XP70_ANY_ITM3_ACK);
}


/*****************************************************************************/
/**
 * \brief Reset ITM3 status interrupts
 *
 *
 */
/*****************************************************************************/
#pragma inline
static  void IRP_CLEAR_ITM3_STATUS_IT(t_uint32 clear_item)
{
  IRP_SET32_REG(ITM_FW_EVENT3_STATUS_BCLR, clear_item);
}


static  t_uint32 IRP_GET_ITM3_STATUS_INT()
{
    return (t_uint32) IRP_GET32_REG(ITM_FW_EVENT3_STATUS);

}


static  t_uint32 IRP_GET_ITM3_MASK_SET()
{
    return (t_uint32) IRP_GET32_REG(ITM_FW_EVENT3_EN_STATUS);

}

#endif // _IRP_API_H_
//eof

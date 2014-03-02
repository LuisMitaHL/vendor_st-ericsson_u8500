/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* \brief    This files defines and implements (inlines) the API for the IPP hardware block
*/


#ifndef _IPP_API_H_
#define _IPP_API_H_

/*
 * Include
 */

#include <inc/type.h>  /* provided by NMF */
#include "crm_mmdsp.h" /* register definition */
#include "dma_mmdsp.h" /* register definition */
#include "pictor_mmdsp.h" /* register definition */
#include "proj_mmdsp.h" /* register definition */
#include "sia_mmdsp.h" /* register definition */
#include "stbp_mmdsp.h" /* register definition */
#include "ipp_mmdsp.h" /* register definition */

#include <stwdsp.h>

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
/*IPP_SD_ERROR */
#define IPP_SD_ERROR_CCP_SHIFT_SYNC         (1<<0)
#define IPP_SD_ERROR_CCP_FALSE_SYNC         (1<<1)
#define IPP_SD_ERROR_CCP_CRC_ERROR          (1<<2)
#define IPP_SD_ERROR_OPIPE0_CH0_OVERFLOW    (1<<3)
#define IPP_SD_ERROR_OPIPE0_CH1_OVERFLOW    (1<<4)
#define IPP_SD_ERROR_OPIPE0_CH2_OVERFLOW    (1<<5)
#define IPP_SD_ERROR_OPIPE1_OVERFLOW        (1<<6)
#define IPP_SD_ERROR_OPIPE2_OVERFLOW        (1<<7)
#define IPP_SD_ERROR_OPIPE0_MULT_ERR        (1<<8)
#define IPP_SD_ERROR_OPIPE1_MULT_ERR        (1<<9)
#define IPP_SD_ERROR_SD_CSI2_PACKET_ERROR   (1<<10)
#define IPP_SD_ERROR_SD_CSI2_DPHY_ERROR     (1<<11)

/*IPP_OPIPE03_SEL */
#define IPP_OPIPE03_SEL_PIPE0   0
#define IPP_OPIPE03_SEL_PIPE3   1

/*IPP_SD_PRY0_FORMAT */
#define IPP_SD_PRY0_FORMAT_YUV422_1BUF      0x100 /*< Pipe0 outputs YUV422 interleaved */
#define IPP_SD_PRY0_FORMAT_YUV422_2BUF      0x200 /*< Pipe0 outputs YUV422 semi-planar */
#define IPP_SD_PRY0_FORMAT_YUV420_2BUF      0x600 /*< Pipe0 outputs YUV420 semi-planar */
#define IPP_SD_PRY0_FORMAT_YUV420_3BUF      0x700 /*< Pipe0 outputs YUV420 planar */
#define IPP_SD_PRY0_FORMAT_RGB888_1BUF      0xC00 /*< Pipe0 outputs RGB888 interleaved */
#define IPP_SD_PRY0_FORMAT_RGB101010_1BUF       0xE00 /*< Pipe0 outputs RGB30 interleaved */

/*IPP_SD_PRY1_FORMAT */
#define IPP_SD_PRY1_FORMAT_YUV422_1BUF      0x100 /*< Pipe1 outputs YUV422 interleaved */
#define IPP_SD_PRY1_FORMAT_ARGB4444_1BUF    0x800 /*< Pipe1 outputs ARGB4444 interleaved */
#define IPP_SD_PRY1_FORMAT_ARGB1555_1BUF    0x900 /*< Pipe1 outputs ARGB1555 interleaved */
#define IPP_SD_PRY1_FORMAT_RGB565_1BUF      0xA00 /*< Pipe1 outputs RGB565 interleaved */
#define IPP_SD_PRY1_FORMAT_RGB888_1BUF      0xC00 /*< Pipe1 outputs RGB888 interleaved */
#define IPP_SD_PRY1_FORMAT_ARGB8888_1BUF    0xD00 /*< Pipe1 outputs ARGB8888 interleaved */
#define IPP_SD_PRY1_FORMAT_RGB101010_1BUF   0xE00 /*< Pipe1 outputs RGB30 interleaved */

/*IPP_SD_BMS_FORMAT */
#define IPP_SD_BMS_FORMAT_RAW12_1BUF        0x0 /* set the raw mamory Bayer Store format */
#define IPP_SD_BMS_FORMAT_RAW8_1BUF         0x1 /* set the raw mamory Bayer Store format */

/* Some combined defines to clear up code */
#define IT_GLOBAL

#define IPP_ERR_GRAB_LR_OVERFLOW (IPP_SD_ERROR_OPIPE1_OVERFLOW)
#define IPP_ERR_GRAB_HR_OVERFLOW (IPP_SD_ERROR_OPIPE0_CH0_OVERFLOW |IPP_SD_ERROR_OPIPE0_CH1_OVERFLOW|IPP_SD_ERROR_OPIPE0_CH2_OVERFLOW)
#define IPP_ERR_BML
#define IPP_ERR_BMS IPP_SD_ERROR_OPIPE2_OVERFLOW
#define IPP_ERR_CCP (IPP_SD_ERROR_CCP_SHIFT_SYNC|IPP_SD_ERROR_CCP_FALSE_SYNC|IPP_SD_ERROR_CCP_CRC_ERROR)
#define IPP_ERR_CSI2_DPHY IPP_SD_ERROR_SD_CSI2_DPHY_ERROR
#define IPP_ERR_CSI2_PACKET IPP_SD_ERROR_SD_CSI2_PACKET_ERROR

/*
 * Global Variables
 */

/* This is new in 8500, now we get IPP_BASE from spec */
#define HW_IPP_WAIT(a) (*(volatile __XIO t_uint16 *)(a))

/*
 * Types
 */

/*------------------------------------------------------------------------
 * Exported functions
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief program a IPP register for 16 bits
 * \param reg_name: Register name
 * \param reg_value: Register value on 16 bits
 *
 */
/*****************************************************************************/

#define IPP_SET_REG(_Reg_, _Val_)   IPP_SET_REG_fct(IPP_##_Reg_##_OFFSET, _Val_)
#pragma inline
static  void IPP_SET_REG_fct (t_uint16 reg_name,
                         t_uint16 reg_value)
{
    HW_IPP_WAIT(reg_name) = reg_value;
}


/*****************************************************************************/
/**
 * \brief Read ipp register
 * \param reg_name: register to read
 * \return Register read
 *
 */
/*****************************************************************************/
#define IPP_GET_REG(_Reg_)  IPP_GET_REG_fct(IPP_##_Reg_##_OFFSET)
#pragma inline
static t_uint16 IPP_GET_REG_fct (t_uint16 reg_name)
{
    return (t_uint16) HW_IPP_WAIT(reg_name);
}


/*****************************************************************************/
/**
 * \brief Read ipp 32 bits register
 * \param reg_name_h: register to read, less bits
 * \param reg_name_l: register to read, mean bits
 * \return Register read
 *
 */
/*****************************************************************************/
#pragma inline
static  t_uint32 IPP_GET_REG_32 (t_uint16 reg_name_h,
                            t_uint16 reg_name_l)
{
    return (t_uint32) (((t_uint32)HW_IPP_WAIT(reg_name_h) << 16L)
                       | (t_uint32)HW_IPP_WAIT(reg_name_l));
}


/*****************************************************************************/
/**
 * \brief Read IPP Status and give the error type correspondant
 * \param error_type: Pointer on error type value
 * \return Continue task continue the next subtask (IT_ERR_CONTINUE)
 *         or stop task (IT_ERR)
 *
 */
/*****************************************************************************/
#pragma inline
static  t_uint16 IPP_SD_ERROR (void)
{
    /* Takes IPP Error */
    return IPP_GET_REG(IPP_SD_ERROR_ITS);
}

#endif /* _IPP_API_H_ */


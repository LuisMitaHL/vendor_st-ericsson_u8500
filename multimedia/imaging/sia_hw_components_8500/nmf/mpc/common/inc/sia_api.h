/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 * \file   sia_api.h
 * \brief
 * \author denis HORY
 * \SIA register acces
 *
 */
/*****************************************************************************/

#ifndef _SIA_API_H_
#define _SIA_API_H_
/*
 * Includes
 */
#include <archi.h>             /* For mmdsp definition           */
#include <inc/type.h>          /* provided by NMF                */
#include "sia_mmdsp.h"
#include <stwdsp.h>

/*
 * Defines
 */

#define SIA_CLOCK_ENABLE_DMA (0x01<<0)
#define SIA_CLOCK_ENABLE_STBP (0x01<<1)
#define SIA_CLOCK_ENABLE_IPP (0x01<<2)
#define SIA_CLOCK_ENABLE_VCF (0x01<<3)
#define SIA_CLOCK_ENABLE_PROJ (0x01<<4)


#define VCF_ENABLE 0x01
#define VCF_DISABLE 0x00

#define SIA_PIPE0 0x00
#define SIA_PIPE3 0x01

#define SIA_ROTATION 0x00
#define SIA_ROTATION_CW 0x01
#define SIA_ROTATION_CCW 0x03

#define HW_SIA_WAIT(a) (*(volatile __XIO t_uint16 *)(a))


/*
 * Types
 */
/*****************************************************************************/
/**
 * \brief program a SIA register for 16 bits
 * \param reg_name: Register name
 * \param reg_value: Register value on 16 bits
 *
 */
/*****************************************************************************/
#define SIA_SET_REG(_Reg_, _Val_) SIA_SET_REG_fct(SIA_##_Reg_##_OFFSET, _Val_)
#pragma inline
static  void SIA_SET_REG_fct (t_uint16 reg_name,
                          t_uint16 reg_value)

{
    HW_SIA_WAIT(reg_name) = reg_value;
}

/*****************************************************************************/
/**
 * \brief Read SIA register
 * \param reg_name: register to read
 * \return Register read
 *
 */
/*****************************************************************************/
#define SIA_GET_REG(_Reg_) SIA_GET_REG_fct(SIA_##_Reg_##_OFFSET)
#pragma inline
static   t_uint16 SIA_GET_REG_fct (t_uint16 reg_name)
{
    return (t_uint16) HW_SIA_WAIT(reg_name);
}


#endif /* _SIA_API_H_ */




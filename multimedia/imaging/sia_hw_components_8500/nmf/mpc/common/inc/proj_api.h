/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 * \file   proj_api.h
 * \brief  projection
 * \author dennis HORY
 *
 *  H and V projection commands
 */
/*****************************************************************************/

#ifndef _PROJ_API_H_
#define _PROJ_API_H_
/*
 * Includes
 */
#include <archi.h>             /* For mmdsp definition           */
#include <inc/type.h>          /* provided by NMF                */
#include "proj_mmdsp.h"
#include <stwdsp.h>

/*
 * Defines
 */

#define HW_PROJ_WAIT(a) (*(volatile __XIO t_uint16 *)(a))

/*PROJ_ENR PROJ_DIR Define */
#define PROJ_HCP (1<<0)	/*Enable/Disable H curve generation */
#define PROJ_VCP (1<<1)	/*Enable/Disable V curve generation */
/*PROJ_ITS_XXX PROJ_ITS_ITM  Define */
#define PROJ_HCP_EOT (1<<0)	/*End of HCP curve generation*/
#define PROJ_VCP_EOT (1<<1)	/*End of VCP curve generation */
#define PROJ_HCP_ERR (1<<2)	/*Error HCP curve generation */
#define PROJ_VCP_ERR (1<<3)	/*Error VCP curve generation*/

/*
 * Types
 */
/*****************************************************************************/
/**
 * \brief program a PROJ register for 16 bits
 * \param reg_name: Register name
 * \param reg_value: Register value on 16 bits
 *
 */
/*****************************************************************************/
#define PROJ_SET_REG(_Reg_, _Val_)	PROJ_SET_REG_fct(PROJ_##_Reg_##_OFFSET, _Val_)
#pragma inline
static  void PROJ_SET_REG_fct (t_uint16 reg_name,
                            t_uint16 reg_value)

{
    HW_PROJ_WAIT(reg_name) = reg_value;
}

/*****************************************************************************/
/**
 * \brief Read PROJ register
 * \param reg_name: register to read
 * \return Register read
 *
 */
/*****************************************************************************/
#define PROJ_GET_REG(_Reg_)	PROJ_GET_REG_fct(PROJ_##_Reg_##_OFFSET)
#pragma inline
static   t_uint16 PROJ_GET_REG_fct (t_uint16 reg_name)
{
    return (t_uint16) HW_PROJ_WAIT(reg_name);
}


#endif /* _PROJ_API_H_ */




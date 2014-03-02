/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 * \file   stbp_sia_api.h
 * \brief  STBP API for SIA
 * \author denis HORY
 *
 * STBP SIA
 */
/*****************************************************************************/

#ifndef _STBP_SIA_API_H_
#define _STBP_SIA_API_H_

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "stbp_mmdsp.h"
#include <stwdsp.h>

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/


/* This is new in 8500, STBP and DMA HW blocks memory mapping are diff */
#define HW_STBP_WAIT(a) (*(volatile __XIO t_uint16 *)(a))


/*****************************************************************************/
/** \brief 	Read a register from STBP - 16 bits
 *  \param	addr	Address to read (offset)
 *  \return Read value
 */
/*****************************************************************************/
#define STBP_GET_REG(_Reg_) STBP_GET_REG_fct(STBP_##_Reg_##_OFFSET)
#pragma inline
static t_uint16 STBP_GET_REG_fct(t_uint16 addr)
{
    return HW_STBP_WAIT(addr);
}

/*****************************************************************************/
/** \brief 	Write a register from STBP - 16 bits
 *  \param	addr	Address to write (offset)
 *  \param	value	Value to write
 */
/*****************************************************************************/
#define STBP_SET_REG(_Reg_, _Val_) STBP_SET_REG_fct(STBP_##_Reg_##_OFFSET,_Val_)
#pragma inline
static void STBP_SET_REG_fct(t_uint16 addr, t_uint16 value)
{
   HW_STBP_WAIT(addr) = value;
}


#endif /* _STBP_SIA_API_H_ */


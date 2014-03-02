/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* \brief    This files defines and implements (inlines) the API for the CKG  hardware block
*/

#ifndef _CKG_API_H_
#define _CKG_API_H_

/*
 * Includes
 */
#include <inc/type.h>        /* provided by NMF */
#include "sia_mmdsp.h" /* register definition */
#include <stwdsp.h>             /* for mmdsp intrinsics */


/*
 * Defines
 */
#define ALL_BLOCKS 0xFFFFU

/*
 * Variables
 */
#define HW_CKG_WAIT(a) (*(volatile __XIO t_uint16 *)(a))
#define CKG_SET_REG(_Reg_, _Value_) HW_CKG_WAIT(SIA_##_Reg_##_OFFSET) = (t_uint16)_Value_
#define CKG_GET_REG(_Reg_) CKG_GET_REG_fct(SIA_##_REG_##OFFSET)

#pragma inline
static t_uint16 CKG_GET_REG_fct(t_uint16 addr)
{
    return HW_CKG_WAIT(addr);
}


#endif /* _CKG_API_H_ */


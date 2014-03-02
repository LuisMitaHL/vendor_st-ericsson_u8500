/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file FLADriver_OPInterface.h
 \brief  This file is a part of the FLADriver module release code and provide an
         interface to the module. This file Containes the defines ,
         which makes an interface for the other Modules to Access the
         functionality of the FLADriver Module .
 \ingroup FLADriver
*/
#ifndef FLADRIVER_OPINTERFACE_H_
#   define FLADRIVER_OPINTERFACE_H_

#   include "FLADriver.h"

extern void FLADriver_UpdateStatus ( void ) TO_EXT_DDR_PRGM_MEM;

//extern volatile uint32_t g_u32_LensMoveStartTime;
extern volatile uint32_t g_u32_LensMoveStopTime;

#   define FLADriver_GetLensISMoving()         (g_FLADriver_Status.e_Flag_LensIsMoving)
#   define FLADriver_GetLimitIsExceeded()      (g_FLADriver_Status.e_Flag_LimitsExceeded)
#   define FLADriver_IsStable()                (g_FLADriver_Status.e_Flag_LensIsMoving == Flag_e_FALSE)
#   define FLADriver_GetLowLevelLensPosition() (g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos)
#endif /*FLADRIVER_OPINTERFACE_H_*/


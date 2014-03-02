/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 * \file    Exposure_OPInterface.h
 * \brief  	Header File containing the output interface provided by the Exposure Module.
 * \ingroup ExpCtrl
*/

#ifndef EXPOSURE_OPINTERFACE_H_
#define EXPOSURE_OPINTERFACE_H_

#include "Exposure.h"


/// Increments the Exposure Sync Error Count.
extern void     Exposure_FrameSyncFailed(void)TO_EXT_DDR_PRGM_MEM;


#define Exposure_GetToBeAppliedDigitalGain()              (g_Exposure_ParametersApplied.f_DigitalGain)


#define Exposure_GetMeteringMode()                        (EXPOSURE_DEFAULT_METERING_MODE)

#define Exposure_ResetErrorState()                        (g_Exposure_ErrorStatus.u8_NumberOfConsecutiveDelayedFrames = 0);   \
	                                                         (g_Exposure_ErrorStatus.e_Flag_ForceInputProcUpdation = Flag_e_FALSE)
#define Exposure_GetForceInputProcUpdationStatus()        (g_Exposure_ErrorStatus.e_Flag_ForceInputProcUpdation)

#endif /*EXPOSURE_OPINTERFACE_H_*/


/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef GLACE_PLATFORMSPECIFIC_
#define GLACE_PLATFORMSPECIFIC_

/**
 \file 		Glace_PlatformSpecific.h

 \brief		This file is NOT a part of the module release code.
	 		All inputs needed by the Glace module that are paltform
	 		dependent are met through this file.
	 		It is the responsibility of the integrator to generate
	 		this file at integration time and meet all the platform
	 		specific dependencies.
	 		This file must be present in incldue path of the
	 		integrated project during compilation of code.

 \note		The following sections define the dependencies that must be
 			met by the system integrator. The way these dependencies
 			have been resolved here are just for example. These dependencies
 			must be appropriately resolved based on the platform being used.

 \ingroup 	Glace
*/



/// Specifies the base address of the Glace memories in the STXP70 address space
#define GLACE_MEM_BASE	(0x90000000 + 0x24404 - 0x4000)	// the 0x4000 corresponds to the "base address" of the offset 0x24404 in T1 domain

/// Specifies the bit width of the Glace multiplier
#define Glace_GetMultiplierWidth()	11

/// Specifies the bit width of the Glace right shift
#define Glace_GetShiftWidth()		5

/// Specifies the default value of Glace_Control.u8_HGridSize page element
#define GLACE_DEFAULT_CONTROL_H_GRID_SIZE				72

/// Specifies the default value of Glace_Control.u8_VGridSize page element
#define GLACE_DEFAULT_CONTROL_V_GRID_SIZE				54

/// Specifies the default value of Glace_Control.f_HBlockSizeFraction page element
#define GLACE_DEFAULT_CONTROL_H_BLOCK_SIZE_FRACTION		(1.0)

/// Specifies the default value of Glace_Control.f_VBlockSizeFraction page element
#define GLACE_DEFAULT_CONTROL_V_BLOCK_SIZE_FRACTION		(1.0)

/// Specifies the default value of Glace_Control.f_HROIStartFraction page element
#define GLACE_DEFAULT_CONTROL_H_ROI_START_FRACTION		0.0

/// Specifies the default value of Glace_Control.f_VROIStartFraction page element
#define GLACE_DEFAULT_CONTROL_V_ROI_START_FRACTION		0.0

/// Specifies the default value of Glace_Control.ptrGlace_Statistics page element
#define GLACE_DEFAULT_CONTROL_STATS_ADDRESS				0

/// Specifies the default value of Glace_Control.u8_RedSaturationLevel page element
#define GLACE_DEFAULT_CONTROL_RED_SATURATION_LEVEL		255

/// Specifies the default value of Glace_Control.u8_GreenSaturationLevel page element
#define GLACE_DEFAULT_CONTROL_GREEN_SATURATION_LEVEL	255

/// Specifies the default value of Glace_Control.u8_BlueSaturationLevel page element
#define GLACE_DEFAULT_CONTROL_BLUE_SATURATION_LEVEL		255

/// Specifies the default value of Glace_Control.GlaceOperationMode_e_Control page element
#define GLACE_DEFAULT_CONTROL_OPERATION_MODE			GlaceOperationMode_e_Disable

/// Specifies the default value of Glace_Control.e_GlaceDataSource page element
#define GLACE_DEFAULT_CONTROL_DATA_SOURCE				GlaceDataSource_e_PostLensShading

/// Specifies the default value of Glace control parameter update count
#define GLACE_DEFAULT_CONTROL_PARAM_UPDATE_COUNT	0

/// Specifies the default value of Glace control operation mode control count
#define GLACE_DEFAULT_CONTROL_CONTROL_UPDATE_COUNT	0

/// Specifies the default value of Glace FOV mode
#define GLACE_DEFAULT_FOV_MODE    (StatisticsFov_e_Sensor)


/// Specifies the status of the default operation mode
#define GLACE_DEFAULT_STATUS_OPERATION_MODE				GlaceOperationMode_e_Continuous

/// Specifies the default value of Glace status parameter update count
#define GLACE_DEFAULT_STATUS_PARAM_UPDATE_COUNT			0

/// Specifies the default value of Glace status operation mode control count
#define GLACE_DEFAULT_STATUS_CONTROL_UPDATE_COUNT	0

/// Specifies the default value of Glace enable pending element
#define GLACE_DEFAULT_STATUS_ENABLE_PENDING			Flag_e_FALSE


#endif /*GLACE_PLATFORMSPECIFIC_*/

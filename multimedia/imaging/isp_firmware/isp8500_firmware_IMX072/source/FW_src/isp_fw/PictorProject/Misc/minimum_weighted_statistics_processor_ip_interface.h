/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef MINIMUM_WEIGHTED_STATISTICS_PROCESSOR_IP_INTERFACE_H_
#define MINIMUM_WEIGHTED_STATISTICS_PROCESSOR_IP_INTERFACE_H_

#   include "PictorhwReg.h"
#   include "Zoom_OPInterface.h"
#   include "SystemConfig.h"
#   include "FrameDimension_op_interface.h"

/// Current X and Y size from Sensor
#   define MWWB_GetWOI_X_Size()    (Zoom_GetCurrentFOVX()/FrameDimension_GetCurrentPreScaleFactor())
#   define MWWB_GetWOI_Y_Size()    (g_Zoom_Status.f_FOVY/FrameDimension_GetCurrentPreScaleFactor())

/// Window of interest starting point
#   define MWWB_GetWOI_X_Start()   (SystemConfig_IsPipe0Active()?Get_ISP_CE0_CROP_ISP_CE0_CROP_H_START():Get_ISP_CE1_CROP_ISP_CE1_CROP_H_START())
#   define MWWB_GetWOI_Y_Start()   (SystemConfig_IsPipe0Active()?Get_ISP_CE0_CROP_ISP_CE0_CROP_V_START():Get_ISP_CE1_CROP_ISP_CE1_CROP_V_START())


#endif /*MINIMUM_WEIGHTED_STATISTICS_PROCESSOR_IP_INTERFACE_H_*/

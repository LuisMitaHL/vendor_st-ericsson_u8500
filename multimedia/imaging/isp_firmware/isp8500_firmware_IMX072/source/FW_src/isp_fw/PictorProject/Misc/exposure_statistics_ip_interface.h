/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef EXPOSURE_STATISTICS_IP_INTERFACE_H_
#define EXPOSURE_STATISTICS_IP_INTERFACE_H_

#   include "PictorhwReg.h"
#   include "Zoom_OPInterface.h"
#   include "SystemConfig.h"
#   include "Exposure_OPInterface.h"
#   include "Stream.h"
#   include "Platform.h"
//#   include "awb_statistics_op_interface.h"
#   include "FrameDimension_op_interface.h"
//#   include "SkinToneDetectionManager.h"
#   include "channel_gains_op_interface.h"


#   define ExpStats_8x6_GetWOI_X_Size()    (Zoom_GetCurrentFOVX()/FrameDimension_GetCurrentPreScaleFactor())
#   define ExpStats_8x6_GetWOI_Y_Size()    (g_Zoom_Status.f_FOVY/FrameDimension_GetCurrentPreScaleFactor())

/// Window of interest starting point
#   define ExpStats_8x6_GetWOI_X_Start()   (SystemConfig_IsPipe0Active()?Get_ISP_CE0_CROP_ISP_CE0_CROP_H_START():Get_ISP_CE1_CROP_ISP_CE1_CROP_H_START())
#   define ExpStats_8x6_GetWOI_Y_Start()   (SystemConfig_IsPipe0Active()?Get_ISP_CE0_CROP_ISP_CE0_CROP_V_START():Get_ISP_CE1_CROP_ISP_CE1_CROP_V_START())

#   define ExpStats_8x6_GetMeteringMode()  Exposure_GetMeteringMode()

#	define EXP_8x6_IS_METERING_ON  			(Flag_e_TRUE)
#   define EXP_8x6_IS_SENSOR_STREAMING()    Stream_IsISPInputStreaming()

#define Set_EXP_8x6_STATS_ENABLE(value)						Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ENABLE(value)

#define Set_EXP_8x6_STATS_ZONE_OFFSET_Y(u16_ZoneOffsetY)	Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_OFFSETY_REQ(u16_ZoneOffsetY)
#define Set_EXP_8x6_STATS_ZONE_OFFSET_X(u16_ZoneOffsetX)	Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_OFFSETX_REQ(u16_ZoneOffsetX)
#define Set_EXP_8x6_STATS_ZONE_SIZE_X(u16_ZoneSizeX)		Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_SIZEX_REQ(u16_ZoneSizeX)
#define Set_EXP_8x6_STATS_ZONE_SIZE_Y(u16_ZoneSizeY)		Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_SIZEY_REQ(u16_ZoneSizeY)

#define Set_EXP_8x6_STATS_THRESHOLD_LO(lo)					Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_THRESHOLDLO_REQ(lo)
#define Set_EXP_8x6_STATS_THRESHOLD_HI(hi)					Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_THRESHOLDHI_REQ(hi)

#define Set_EXP_8x6_STATS_ACC_CTRL_REQ(acc_mode,acc_type,acc_color,acc_src,acc_staggered)	Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_CTRL_REQ(acc_mode,acc_type,acc_color,acc_src,acc_staggered)

#define Set_EXP_8x6_STATS_ACC_ZONE_GAINS_1_16(u32_ZoneGains_1_16)	Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_GAINS_1_16_REQ(u32_ZoneGains_1_16)
#define Set_EXP_8x6_STATS_ACC_ZONE_GAINS_17_32(u32_ZoneGains_17_32) Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_GAINS_17_32_REQ(u32_ZoneGains_17_32)
#define Set_EXP_8x6_STATS_ACC_ZONE_GAINS_33_48(u32_ZoneGains_33_48) Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_GAINS_33_48_REQ(u32_ZoneGains_33_48)

#define Get_EXP_8x6_STATS_ACC_TOTAL()						Get_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ACC_TOTAL()
#define Get_EXP_8x6_STATS_ACC_ZONE_SIZE_X()					Get_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_SIZEX_REQ()
#define Get_EXP_8x6_STATS_ACC_ZONE_SIZE_Y()					Get_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_SIZEY_REQ()


#endif /*EXPOSURE_STATISTICS_IP_INTERFACE_H_*/

/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file RgbToYuvCoder_ip_interface.h
 \brief This file is NOT a part of the module release code.
            All inputs needed by the RGB to YUV coder module that can only
            be resolved at the project level (at integration time)
            are met through this file.
            It is the responsibility of the integrator to generate
            this file at integration time and meet all the input
            dependencies.


 \note      The following sections define the dependencies that must be
           met by the system integrator. The way these dependencies
           have been resolved here are just for example. These dependencies
           must be appropriately resolved based on the platform being used.
 \ingroup RgbToYuvCoder
*/
#ifndef _RGBTOYUVCODER_IP_INTERFACE_H_
#   define _RGBTOYUVCODER_IP_INTERFACE_H_

#   include "PictorhwReg.h"
#   include "SystemConfig.h"

/// Returns true(1) if the requested output format is YUV422
#   define RgbToYuvCoder_IsCurrentPipeOutputFormat_YUV422(x)        SystemConfig_IsPipeOutputYUV(x)

/// Returns true(1) if the requested output format is RGB UNPEAKED
#   define RgbToYuvCoder_IsCurrentPipeInputSource_RGB_UNPEAKED(x)   SystemConfig_IsPipeOutputUnPeaked(x)

// H/W register's
//YUVCODER_FORMAT

//YUVCODER_ENABLE
#   define Set_ISP_yuvCoder_ce0_ENABLE(enable, soft_reset) Set_ISP_CE0_CODER_ISP_CE0_CODER_ENABLE(enable, soft_reset)

/// Set the YUV coder PIPE0 Output to RGB PEAKED format
#   define SET_PIPE0_YUV_CODER_FORMAT_TO_RGB_PEAKED()  Set_ISP_CE0_CODER_ISP_CE0_CODER_FORMAT(yuv_format_B_0x0)

/// Set the YUV coder PIPE0 Output to RGB UNPEAKED format
#   define SET_PIPE0_YUV_CODER_FORMAT_TO_RGB_UNPEAKED()    Set_ISP_CE0_CODER_ISP_CE0_CODER_FORMAT(yuv_format_B_0x1)

/// Set the YUV coder PIPE0 Output to YUV422 format
#   define SET_PIPE0_YUV_CODER_FORMAT_TO_YUV422()  Set_ISP_CE0_CODER_ISP_CE0_CODER_FORMAT(yuv_format_B_0x2)

/// YUV_CODER_PIPE0_Y_COF00
#   define SET_PIPE0_YUV_CODER_YCBCR_00(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_Y_COF00(x)

/// YUV_CODER_PIPE0_Y_COF01
#   define SET_PIPE0_YUV_CODER_YCBCR_01(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_Y_COF01(x)

/// YUV_CODER_PIPE0_Y_COF02
#   define SET_PIPE0_YUV_CODER_YCBCR_02(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_Y_COF02(x)

/// YUV_CODER_PIPE0_CB_COF10
#   define SET_PIPE0_YUV_CODER_YCBCR_10(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CB_COF10(x)

/// YUV_CODER_PIPE0_CB_COF11
#   define SET_PIPE0_YUV_CODER_YCBCR_11(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CB_COF11(x)

/// YUV_CODER_PIPE0_CB_COF12
#   define SET_PIPE0_YUV_CODER_YCBCR_12(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CB_COF12(x)

/// YUV_CODER_PIPE0_CR_COF20
#   define SET_PIPE0_YUV_CODER_YCBCR_20(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CR_COF20(x)

/// YUV_CODER_PIPE0_CR_COF21
#   define SET_PIPE0_YUV_CODER_YCBCR_21(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CR_COF21(x)

/// YUV_CODER_PIPE0_CR_COF22
#   define SET_PIPE0_YUV_CODER_YCBCR_22(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CR_COF22(x)

/// YUV_CODER_PIPE0_YFLOOR
#   define SET_PIPE0_YUV_CODER_Y_FLOOR(x)  Set_ISP_CE0_CODER_ISP_CE0_CODER_YFLOOR(x)

/// YUV_CODER_PIPE0_CBFLOOR
#   define SET_PIPE0_YUV_CODER_CB_FLOOR(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CBFLOOR(x)

/// YUV_CODER_PIPE0_CRFLOOR
#   define SET_PIPE0_YUV_CODER_CR_FLOOR(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CRFLOOR(x)

/// YUV_CODER_PIPE0_CH0_MAX_CLIP
#   define SET_PIPE0_YUV_CODER_CH0_MAX_CLIP(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CH0_MAX_CLIP(x)

/// YUV_CODER_PIPE0_CH1_MAX_CLIP
#   define SET_PIPE0_YUV_CODER_CH1_MAX_CLIP(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CH1_MAX_CLIP(x)

/// YUV_CODER_PIPE0_CH2_MAX_CLIP
#   define SET_PIPE0_YUV_CODER_CH2_MAX_CLIP(x) Set_ISP_CE0_CODER_ISP_CE0_CODER_CH2_MAX_CLIP(x)

/************************************************************************************************************************/

///                                             PIPE 1 defines

/************************************************************************************************************************/

//YUVCODER_ENABLE
#   define Set_ISP_yuvCoder_ce1_ENABLE(enable, soft_reset) Set_ISP_CE1_CODER_ISP_CE1_CODER_ENABLE(enable, soft_reset)

/// Set the YUV coder PIPE1 Output to RGB PEAKED format
#   define SET_PIPE1_YUV_CODER_FORMAT_TO_RGB_PEAKED()  Set_ISP_CE1_CODER_ISP_CE1_CODER_FORMAT(yuv_format_B_0x0)

/// Set the YUV coder PIPE1 Output to RGB UNPEAKED format
#   define SET_PIPE1_YUV_CODER_FORMAT_TO_RGB_UNPEAKED()    Set_ISP_CE1_CODER_ISP_CE1_CODER_FORMAT(yuv_format_B_0x1)

/// Set the YUV coder PIPE1 Output to YUV422 format
#   define SET_PIPE1_YUV_CODER_FORMAT_TO_YUV422()  Set_ISP_CE1_CODER_ISP_CE1_CODER_FORMAT(yuv_format_B_0x2)

/// YUV_CODER_PIPE1_Y_COF00
#   define SET_PIPE1_YUV_CODER_YCBCR_00(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_Y_COF00(x)

/// YUV_CODER_PIPE1_Y_COF01
#   define SET_PIPE1_YUV_CODER_YCBCR_01(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_Y_COF01(x)

/// YUV_CODER_PIPE1_Y_COF02
#   define SET_PIPE1_YUV_CODER_YCBCR_02(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_Y_COF02(x)

/// YUV_CODER_PIPE1_CB_COF10
#   define SET_PIPE1_YUV_CODER_YCBCR_10(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CB_COF10(x)

/// YUV_CODER_PIPE1_CB_COF11
#   define SET_PIPE1_YUV_CODER_YCBCR_11(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CB_COF11(x)

/// YUV_CODER_PIPE1_CB_COF12
#   define SET_PIPE1_YUV_CODER_YCBCR_12(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CB_COF12(x)

/// YUV_CODER_PIPE1_CR_COF20
#   define SET_PIPE1_YUV_CODER_YCBCR_20(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CR_COF20(x)

/// YUV_CODER_PIPE1_CR_COF21
#   define SET_PIPE1_YUV_CODER_YCBCR_21(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CR_COF21(x)

/// YUV_CODER_PIPE1_CR_COF22
#   define SET_PIPE1_YUV_CODER_YCBCR_22(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CR_COF22(x)

/// YUV_CODER_PIPE1_YFLOOR
#   define SET_PIPE1_YUV_CODER_Y_FLOOR(x)  Set_ISP_CE1_CODER_ISP_CE1_CODER_YFLOOR(x)

/// YUV_CODER_PIPE1_CBFLOOR
#   define SET_PIPE1_YUV_CODER_CB_FLOOR(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CBFLOOR(x)

/// YUV_CODER_PIPE1_CRFLOOR
#   define SET_PIPE1_YUV_CODER_CR_FLOOR(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CRFLOOR(x)

/// YUV_CODER_PIPE1_CH0_MAX_CLIP
#   define SET_PIPE1_YUV_CODER_CH0_MAX_CLIP(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CH0_MAX_CLIP(x)

/// YUV_CODER_PIPE1_CH1_MAX_CLIP
#   define SET_PIPE1_YUV_CODER_CH1_MAX_CLIP(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CH1_MAX_CLIP(x)

/// YUV_CODER_PIPE1_CH2_MAX_CLIP
#   define SET_PIPE1_YUV_CODER_CH2_MAX_CLIP(x) Set_ISP_CE1_CODER_ISP_CE1_CODER_CH2_MAX_CLIP(x)

/// Y_FLOOR Min and Max Values
#   define CE0_YFLOOR_MIN	-255
#   define CE0_YFLOOR_MAX	255
#   define CE0_CBFLOOR_MIN -255
#   define CE0_CBFLOOR_MAX 255
#   define CE0_CRFLOOR_MIN -255
#   define CE0_CRFLOOR_MAX 255
#   define CE1_YFLOOR_MIN	-255
#   define CE1_YFLOOR_MAX	255
#   define CE1_CBFLOOR_MIN -255
#   define CE1_CBFLOOR_MAX 255
#   define CE1_CRFLOOR_MIN -255
#   define CE1_CRFLOOR_MAX 255


#endif // _RGBTOYUVCODER_IP_INTERFACE_H_


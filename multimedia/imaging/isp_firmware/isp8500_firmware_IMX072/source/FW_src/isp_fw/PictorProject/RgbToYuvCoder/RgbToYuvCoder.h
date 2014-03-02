/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file RgbToYuvCoder.h
 \brief This file is a part of the release code. It contains the data
        structures, macros, enums and function declarations used by the module.
 \ingroup RgbToYuvCoder
*/
#ifndef _RGB_TO_YUV_CODER_H_
#   define _RGB_TO_YUV_CODER_H_

#   include "RgbToYuvCoder_op_interface.h"
#   include "RgbToYuvCoder_ip_interface.h"
#   include "RgbToYuvCoder_PlatformSpecific.h"
#   include "GenericFunctions.h"

#define ENABLE_YUVCODER_TRACES    (0)

/**
 \enum Transform_te
 \brief RGB to YUV transform type. YUV coder block can give both YUV and RGB data.
 \ingroup RgbToYuvCoder
*/
typedef enum
{
    /// Full Range BT Rec601 Y'CbCr output
    /// range for luma = 0-255
    /// range for chroma = 0-255    
    Transform_e_YCbCr_JFIF,

    /// limited range BT Rec601 Y'CbCr output
    /// range for luma = 16-235
    /// range for chroma = 16-240        
    Transform_e_YCbCr_Rec601,

    /// Full Range BT Rec709 Y'CbCr output
    /// range for luma = 0-255
    /// range for chroma = 0-255        
    Transform_e_YCbCr_Rec709_FULL_RANGE, 

    /// limited range BT Rec709 Y'CbCr output 
    /// range for luma = 16-235
    /// range for chroma = 16-240      
    Transform_e_YCbCr_Rec709,

    /// Custom requires manual update of stock matrix and signal range
    /// User should fill g_CE_CustomTransformOutputSignalRange[0/1]
    Transform_e_YCbCr_Custom,

    /// Standard 3x10 bit RGB output
    Transform_e_Rgb_Standard,

    /// Custom requires manual update of Luma and chroma parameters like excursion and MidPointTimes2
    /// User should fill g_CE_CustomTransformOutputSignalRange[0/1]
    Transform_e_Rgb_Custom
} Transform_te;


extern void RgbToYuvCoder_Run (uint8_t u8_PipeNo);

/// Default Luma range, the range is equal to max output from the pipe
#   define F_LUMA_RANGE                    RGBTOYUVCODER_MAX_INPUT_FROM_PIPE

/// Default Chroma range the range is equal to max output from the pipe
#   define F_CHROMA_RANGE                  RGBTOYUVCODER_MAX_INPUT_FROM_PIPE

/// Maximum scaler output
#   define F_MATRIX_SCALER                 (1024.0)


#endif // _RGB_TO_YUV_CODER_H_


/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file Norcos.h
 */
#ifndef NORCOS_H_
#define NORCOS_H_


#include "Platform.h"

typedef struct
{
    /// This P.E maps to register ISP_CEx_NORCOS_OUTPUT_CONTROL
    uint16_t u16_Norcos_Luma_Control;

    /// Enable/disable the Luma component of norcos block.
    uint8_t e_Flag_Norcos_Luma_Enable;

    /// Enable/disable the Chroma component of norcos block.
    uint8_t e_Flag_Norcos_Chroma_Enable;

    /// Enable/disable the YUV422 component of norcos block.
    uint8_t e_Flag_Norcos_YUV422_Enable;

    /// Norcos output control:This P.E maps to ISP_CEx_NORCOS_OUTPUT_CONTROL
    uint8_t u8_Norcos_Output_Control;

    /// This setting gives the minimum number of pixels in the kernel whose value meets the Flat detection grade criteria. Given that we work on a 5*5 pixels kernel, the maximum value is 25 (the central pixel is not concerned).The minimum value is zero. This P.E maps to ISP_CEx_NORCOS_LUMA_FLAT_PARAM
    uint8_t u8_Norcos_Luma_FlatParam;

    /// This setting is the threshold for the difference between the central pixel and each of the pixels in the surrounding kernel. This P.E maps to ISP_CEx_NORCOS_CHROMA_FLAT_DETECT_GRADE
    uint8_t u8_Norcos_Luma_FlatDetectGrade;

    /// Sharp Grade. This P.E maps to ISP_CEx_NORCOS_LUMA_SHARP_GRADE
    uint8_t u8_Norcos_Luma_SharpGrade;

    /// UnSharp Grade. This P.E maps to ISP_CEx_NORCOS_LUMA_UNSHARP_GRADE
    uint8_t u8_Norcos_Luma_UnsharpGrade;

    /// This P.E maps to ISP_CEx_NORCOS_LUMA_IMPULSE_NOISE
    uint8_t u8_Norcos_Luma_ImpulseNoise;

    /// Luma Impulse noise threshold. This P.E maps to ISP_CEx_NORCOS_LUMA_NOISE_LEVELTH
    uint8_t u8_Norcos_Luma_ImpulseNoiseThreshold;

    /// Enable Peaking 5x5 (tx=0%) versus 3x3 (tx= 100% => 128) or mixed them. This P.E maps to ISP_CEx_NORCOS_LUMA_TX
    uint8_t u8_Norcos_Luma_Tx;

    /// Luma input peaking low threshold. This P.E maps to ISP_CEx_NORCOS_LUMA_IPK_LOW
    uint8_t u8_Norcos_Luma_PeakLowThreshold;

    /// Luma input peaking high threshold. This P.E maps to ISP_CEx_NORCOS_LUMA_IPK_HIGH
    uint8_t u8_Norcos_Luma_PeakHighThreshold;

    /// Random noise level. This P.E maps to ISP_CEx_NORCOS_LUMA_NOISE_LEVEL
    uint8_t u8_Norcos_Luma_PseudoNoise;

    /// Control de gain applied over the max Value => Limit the overshoot. This P.E maps to ISP_CEx_NORCOS_LUMA_OSHOOT_GAIN
    uint8_t u8_Norcos_Luma_OverShootGain;

    /// Sets bypass filters and norcos chroma mode This P.E maps to ISP_CEx_NORCOS_CHROMA_CTRL
    uint8_t u8_Norcos_Chroma_Ctrl;

    /// This setting gives the minimum number of pixels in the kernel whose value meets the Flat detection grade criteria. Given that we work on a 7*5 pixels kernel, the maximum value is 35 (the central pixel is not concerned).The minimum value is zero. This P.E maps to ISP_CEx_NORCOS_CHROMA_FLAT_PARAM
    uint8_t u8_Norcos_Chroma_FlatParam;

    /// This setting is the threshold for the difference between the central pixel and each of the pixels in the surrounding kernel. This P.E maps to ISP_CEx_NORCOS_CHROMA_FLAT_DETECT_GRADE
    uint8_t u8_Norcos_Chroma_FlatDetectGrade;

    /// This P.E maps to ISP_CEx_NORCOS_CHROMA_IMPULSE_NOISE
    uint8_t u8_Norcos_Chroma_ImpulseNoise;

    /// This P.E maps to ISP_CEx_NORCOS_CHROMA_NOISE_LEVELTH
    uint8_t u8_Norcos_Chroma_ImpulseThreshold;
} Norcos_Ctrl_ts;

// Control page of Norcos
extern Norcos_Ctrl_ts  g_Norcos_Ctrl[];

// Status page of Norcos
extern Norcos_Ctrl_ts  g_Norcos_Status[];

// Function to commit Norcos parameters
void Norcos_Commit(uint8_t index);

// Function to update status  page elements of Norcos
void Norcos_Update(uint8_t index);

#endif /* NORCOS_H_ */


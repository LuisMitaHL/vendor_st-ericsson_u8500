/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
\file Norcos.c
 */
#include "Norcos.h"
#include "Norcos_platform_specific.h"

#if 1
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_Norcos_NorcosTraces.h"
#endif
#endif



Norcos_Ctrl_ts  g_Norcos_Ctrl[NO_OF_HARDWARE_PIPE_IN_ISP]=
{
    {
        DEFAULT_NORCOS_LUMA_CONTROL,
        DEFAULT_NORCOS_LUMA_ENABLE,
        DEFAULT_NORCOS_CHROMA_ENABLE,
        DEFAULT_NORCOS_YUV422_ENABLE,
        DEFAULT_NORCOS_OUTPUT_CONTROL,
        DEFAULT_NORCOS_LUMA_FLAT_PARAM,
        DEFAULT_NORCOS_LUMA_FLAT_DETECT_GRADE,
        DEFAULT_NORCOS_LUMA_SHARP_GRADE,
        DEFAULT_NORCOS_LUMA_UNSHARP_GRADE,
        DEFAULT_NORCOS_LUMA_IMPULSE_NOISE,
        DEFAULT_NORCOS_LUMA_IMPULSE_NOISE_THRESHOLD,
        DEFAULT_NORCOS_LUMA_TX,
        DEFAULT_NORCOS_LUMA_PEAK_LOW_THRESHOLD,
        DEFAULT_NORCOS_LUMA_PEAK_HIGH_THRESHOLD,
        DEFAULT_NORCOS_LUMA_PSEUDO_NOISE,
        DEFAULT_NORCOS_LUMA_OVER_SHOOT_GAIN,
        DEFAULT_NORCOS_CHROMA_CTRL,
        DEFAULT_NORCOS_CHROMA_FLAT_PARAM,
        DEFAULT_NORCOS_CHROMA_FLAT_DETECT_GRADE,
        DEFAULT_NORCOS_CHROMA_IMPULSE_NOISE,
        DEFAULT_NORCOS_CHROMA_IMPULSE_THRESHOLD,
    },
    {
        DEFAULT_NORCOS_LUMA_CONTROL,
        DEFAULT_NORCOS_LUMA_ENABLE,
        DEFAULT_NORCOS_CHROMA_ENABLE,
        DEFAULT_NORCOS_YUV422_ENABLE,
        DEFAULT_NORCOS_OUTPUT_CONTROL,
        DEFAULT_NORCOS_LUMA_FLAT_PARAM,
        DEFAULT_NORCOS_LUMA_FLAT_DETECT_GRADE,
        DEFAULT_NORCOS_LUMA_SHARP_GRADE,
        DEFAULT_NORCOS_LUMA_UNSHARP_GRADE,
        DEFAULT_NORCOS_LUMA_IMPULSE_NOISE,
        DEFAULT_NORCOS_LUMA_IMPULSE_NOISE_THRESHOLD,
        DEFAULT_NORCOS_LUMA_TX,
        DEFAULT_NORCOS_LUMA_PEAK_LOW_THRESHOLD,
        DEFAULT_NORCOS_LUMA_PEAK_HIGH_THRESHOLD,
        DEFAULT_NORCOS_LUMA_PSEUDO_NOISE,
        DEFAULT_NORCOS_LUMA_OVER_SHOOT_GAIN,
        DEFAULT_NORCOS_CHROMA_CTRL,
        DEFAULT_NORCOS_CHROMA_FLAT_PARAM,
        DEFAULT_NORCOS_CHROMA_FLAT_DETECT_GRADE,
        DEFAULT_NORCOS_CHROMA_IMPULSE_NOISE,
        DEFAULT_NORCOS_CHROMA_IMPULSE_THRESHOLD,
    }
};

Norcos_Ctrl_ts  g_Norcos_Status[NO_OF_HARDWARE_PIPE_IN_ISP]=
{
    {
        DEFAULT_NORCOS_LUMA_CONTROL,
        DEFAULT_NORCOS_LUMA_ENABLE,
        DEFAULT_NORCOS_CHROMA_ENABLE,
        DEFAULT_NORCOS_YUV422_ENABLE,
        DEFAULT_NORCOS_OUTPUT_CONTROL,
        DEFAULT_NORCOS_LUMA_FLAT_PARAM,
        DEFAULT_NORCOS_LUMA_FLAT_DETECT_GRADE,
        DEFAULT_NORCOS_LUMA_SHARP_GRADE,
        DEFAULT_NORCOS_LUMA_UNSHARP_GRADE,
        DEFAULT_NORCOS_LUMA_IMPULSE_NOISE,
        DEFAULT_NORCOS_LUMA_IMPULSE_NOISE_THRESHOLD,
        DEFAULT_NORCOS_LUMA_TX,
        DEFAULT_NORCOS_LUMA_PEAK_LOW_THRESHOLD,
        DEFAULT_NORCOS_LUMA_PEAK_HIGH_THRESHOLD,
        DEFAULT_NORCOS_LUMA_PSEUDO_NOISE,
        DEFAULT_NORCOS_LUMA_OVER_SHOOT_GAIN,
        DEFAULT_NORCOS_CHROMA_CTRL,
        DEFAULT_NORCOS_CHROMA_FLAT_PARAM,
        DEFAULT_NORCOS_CHROMA_FLAT_DETECT_GRADE,
        DEFAULT_NORCOS_CHROMA_IMPULSE_NOISE,
        DEFAULT_NORCOS_CHROMA_IMPULSE_THRESHOLD,
    },
    {
        DEFAULT_NORCOS_LUMA_CONTROL,
        DEFAULT_NORCOS_LUMA_ENABLE,
        DEFAULT_NORCOS_CHROMA_ENABLE,
        DEFAULT_NORCOS_YUV422_ENABLE,
        DEFAULT_NORCOS_OUTPUT_CONTROL,
        DEFAULT_NORCOS_LUMA_FLAT_PARAM,
        DEFAULT_NORCOS_LUMA_FLAT_DETECT_GRADE,
        DEFAULT_NORCOS_LUMA_SHARP_GRADE,
        DEFAULT_NORCOS_LUMA_UNSHARP_GRADE,
        DEFAULT_NORCOS_LUMA_IMPULSE_NOISE,
        DEFAULT_NORCOS_LUMA_IMPULSE_NOISE_THRESHOLD,
        DEFAULT_NORCOS_LUMA_TX,
        DEFAULT_NORCOS_LUMA_PEAK_LOW_THRESHOLD,
        DEFAULT_NORCOS_LUMA_PEAK_HIGH_THRESHOLD,
        DEFAULT_NORCOS_LUMA_PSEUDO_NOISE,
        DEFAULT_NORCOS_LUMA_OVER_SHOOT_GAIN,
        DEFAULT_NORCOS_CHROMA_CTRL,
        DEFAULT_NORCOS_CHROMA_FLAT_PARAM,
        DEFAULT_NORCOS_CHROMA_FLAT_DETECT_GRADE,
        DEFAULT_NORCOS_CHROMA_IMPULSE_NOISE,
        DEFAULT_NORCOS_CHROMA_IMPULSE_THRESHOLD,
    }
};

void
Norcos_Commit(uint8_t index)
{
    switch(index)
    {
        case 0:
        {
            /* Norcos is enabled or not, this check needs to be validated. As of now correct method of testing this is not known.
            TBD in future */
            if ((g_Norcos_Ctrl[index].e_Flag_Norcos_Luma_Enable == Flag_e_TRUE) && (g_Norcos_Ctrl[index].e_Flag_Norcos_Chroma_Enable == Flag_e_TRUE) && (g_Norcos_Ctrl[index].e_Flag_Norcos_YUV422_Enable == Flag_e_TRUE ))
            {
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_OUTPUT_CONTROL(g_Norcos_Status[index].u8_Norcos_Output_Control);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_CTRL_word(g_Norcos_Status[index].u16_Norcos_Luma_Control);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_FLAT_PARAM_word(g_Norcos_Status[index].u8_Norcos_Luma_FlatParam);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_FLAT_DETECT_GRADE_word(g_Norcos_Status[index].u8_Norcos_Luma_FlatDetectGrade);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_IMPULSE_NOISE_word(g_Norcos_Status[index].u8_Norcos_Luma_ImpulseNoise);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_SHARP_GRADE_word(g_Norcos_Status[index].u8_Norcos_Luma_SharpGrade);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_UNSHARP_GRADE_word(g_Norcos_Status[index].u8_Norcos_Luma_UnsharpGrade) ;
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_NOISE_LEVEL_word(g_Norcos_Status[index].u8_Norcos_Luma_PseudoNoise);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_NOISE_LEVELTH_word(g_Norcos_Status[index].u8_Norcos_Luma_ImpulseNoiseThreshold);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_TX_word(g_Norcos_Status[index].u8_Norcos_Luma_Tx);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_IPK_LOW_word(g_Norcos_Status[index].u8_Norcos_Luma_PeakLowThreshold);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_IPK_HIGH_word(g_Norcos_Status[index].u8_Norcos_Luma_PeakHighThreshold);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_LUMA_OSHOOT_GAIN_word(g_Norcos_Status[index].u8_Norcos_Luma_OverShootGain);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_CHROMA_CTRL_word(g_Norcos_Status[index].u8_Norcos_Chroma_Ctrl);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_CHROMA_FLAT_PARAM_word(g_Norcos_Status[index].u8_Norcos_Chroma_FlatParam);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_CHROMA_FLAT_DETECT_GRADE_word(g_Norcos_Status[index].u8_Norcos_Chroma_FlatDetectGrade);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_CHROMA_IMPULSE_NOISE_word(g_Norcos_Status[index].u8_Norcos_Chroma_ImpulseNoise);
                Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_CHROMA_NOISE_LEVELTH_word(g_Norcos_Status[index].u8_Norcos_Chroma_ImpulseThreshold);
            }
            Set_ISP_CE0_NORCOS_ISP_CE0_NORCOS_ENABLE(g_Norcos_Status[index].e_Flag_Norcos_Luma_Enable,g_Norcos_Status[index].e_Flag_Norcos_Chroma_Enable,g_Norcos_Status[index].e_Flag_Norcos_YUV422_Enable,0,0);
            break;
        }
        case 1:
        {
            if ((g_Norcos_Ctrl[index].e_Flag_Norcos_Luma_Enable == Flag_e_TRUE) && (g_Norcos_Ctrl[index].e_Flag_Norcos_Chroma_Enable == Flag_e_TRUE) && (g_Norcos_Ctrl[index].e_Flag_Norcos_YUV422_Enable == Flag_e_TRUE ))
            {
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_OUTPUT_CONTROL(g_Norcos_Status[index].u8_Norcos_Output_Control);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_CTRL_word(g_Norcos_Status[index].u16_Norcos_Luma_Control);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_FLAT_PARAM_word(g_Norcos_Status[index].u8_Norcos_Luma_FlatParam);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_FLAT_DETECT_GRADE_word(g_Norcos_Status[index].u8_Norcos_Luma_FlatDetectGrade);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_IMPULSE_NOISE_word(g_Norcos_Status[index].u8_Norcos_Luma_ImpulseNoise);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_SHARP_GRADE_word(g_Norcos_Status[index].u8_Norcos_Luma_SharpGrade);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_UNSHARP_GRADE_word(g_Norcos_Status[index].u8_Norcos_Luma_UnsharpGrade) ;
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_NOISE_LEVEL_word(g_Norcos_Status[index].u8_Norcos_Luma_PseudoNoise);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_NOISE_LEVELTH_word(g_Norcos_Status[index].u8_Norcos_Luma_ImpulseNoiseThreshold);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_TX_word(g_Norcos_Status[index].u8_Norcos_Luma_Tx);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_IPK_LOW_word(g_Norcos_Status[index].u8_Norcos_Luma_PeakLowThreshold) ;
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_IPK_HIGH_word(g_Norcos_Status[index].u8_Norcos_Luma_PeakHighThreshold);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_LUMA_OSHOOT_GAIN_word(g_Norcos_Status[index].u8_Norcos_Luma_OverShootGain);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_CHROMA_CTRL_word(g_Norcos_Status[index].u8_Norcos_Chroma_Ctrl);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_CHROMA_FLAT_PARAM_word(g_Norcos_Status[index].u8_Norcos_Chroma_FlatParam);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_CHROMA_FLAT_DETECT_GRADE_word(g_Norcos_Status[index].u8_Norcos_Chroma_FlatDetectGrade);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_CHROMA_IMPULSE_NOISE_word(g_Norcos_Status[index].u8_Norcos_Chroma_ImpulseNoise);
                Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_CHROMA_NOISE_LEVELTH_word(g_Norcos_Status[index].u8_Norcos_Chroma_ImpulseThreshold);
            }
            Set_ISP_CE1_NORCOS_ISP_CE1_NORCOS_ENABLE(g_Norcos_Status[index].e_Flag_Norcos_Luma_Enable,g_Norcos_Status[index].e_Flag_Norcos_Chroma_Enable,g_Norcos_Status[index].e_Flag_Norcos_YUV422_Enable,0,0);
            break;
        }
        default :
        {
#if USE_TRACE_ERROR
            OstTraceInt1(TRACE_ERROR, "<ERROR>!!Norcos_Commit() pipe index passed is %d",index);
#endif
            // ideally code should not come here
            ASSERT_XP70();
        }
    }
}

void
Norcos_Update(uint8_t index)
{

    if ((index & 0xFE) == 0)  // If the condition is true it means index is either 1 or 0
    {
        g_Norcos_Status[index].u8_Norcos_Output_Control = g_Norcos_Ctrl[index].u8_Norcos_Output_Control;
        g_Norcos_Status[index].u16_Norcos_Luma_Control = g_Norcos_Ctrl[index].u16_Norcos_Luma_Control;
        g_Norcos_Status[index].u8_Norcos_Luma_FlatParam = g_Norcos_Ctrl[index].u8_Norcos_Luma_FlatParam;
        g_Norcos_Status[index].u8_Norcos_Luma_FlatDetectGrade = g_Norcos_Ctrl[index].u8_Norcos_Luma_FlatDetectGrade;
        g_Norcos_Status[index].u8_Norcos_Luma_ImpulseNoise = g_Norcos_Ctrl[index].u8_Norcos_Luma_ImpulseNoise;
        g_Norcos_Status[index].u8_Norcos_Luma_SharpGrade = g_Norcos_Ctrl[index].u8_Norcos_Luma_SharpGrade;
        g_Norcos_Status[index].u8_Norcos_Luma_UnsharpGrade = g_Norcos_Ctrl[index].u8_Norcos_Luma_UnsharpGrade;
        g_Norcos_Status[index].u8_Norcos_Luma_PseudoNoise = g_Norcos_Ctrl[index].u8_Norcos_Luma_PseudoNoise;
        g_Norcos_Status[index].u8_Norcos_Luma_ImpulseNoiseThreshold = g_Norcos_Ctrl[index].u8_Norcos_Luma_ImpulseNoiseThreshold;
        g_Norcos_Status[index].u8_Norcos_Luma_Tx = g_Norcos_Ctrl[index].u8_Norcos_Luma_Tx;
        g_Norcos_Status[index].u8_Norcos_Luma_PeakLowThreshold = g_Norcos_Ctrl[index].u8_Norcos_Luma_PeakLowThreshold;
        g_Norcos_Status[index].u8_Norcos_Luma_PeakHighThreshold = g_Norcos_Ctrl[index].u8_Norcos_Luma_PeakHighThreshold;
        g_Norcos_Status[index].u8_Norcos_Luma_OverShootGain = g_Norcos_Ctrl[index].u8_Norcos_Luma_OverShootGain;
        g_Norcos_Status[index].u8_Norcos_Chroma_Ctrl = g_Norcos_Status[index].u8_Norcos_Chroma_Ctrl;
        g_Norcos_Status[index].u8_Norcos_Chroma_FlatParam = g_Norcos_Ctrl[index].u8_Norcos_Chroma_FlatParam;
        g_Norcos_Status[index].u8_Norcos_Chroma_FlatDetectGrade = g_Norcos_Ctrl[index].u8_Norcos_Chroma_FlatDetectGrade;
        g_Norcos_Status[index].u8_Norcos_Chroma_ImpulseNoise = g_Norcos_Ctrl[index].u8_Norcos_Chroma_ImpulseNoise;
        g_Norcos_Status[index].u8_Norcos_Chroma_ImpulseThreshold = g_Norcos_Ctrl[index].u8_Norcos_Chroma_ImpulseThreshold;
        g_Norcos_Status[index].e_Flag_Norcos_Luma_Enable = g_Norcos_Ctrl[index].e_Flag_Norcos_Luma_Enable;
        g_Norcos_Status[index].e_Flag_Norcos_Chroma_Enable = g_Norcos_Ctrl[index].e_Flag_Norcos_Chroma_Enable;
        g_Norcos_Status[index].e_Flag_Norcos_YUV422_Enable = g_Norcos_Ctrl[index].e_Flag_Norcos_YUV422_Enable;
    }
    else
    {
#if USE_TRACE_ERROR
        OstTraceInt1(TRACE_ERROR, "<ERROR>!!Norcos_Update() pipe index passed is %d",index);
#endif
        // ideally code should not come here
        ASSERT_XP70();
    }
}

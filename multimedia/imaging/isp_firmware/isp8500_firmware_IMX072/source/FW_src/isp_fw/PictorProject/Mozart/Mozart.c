/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/*
\file Mozart.c
 */
#include "Mozart.h"
#include "Mozart_platform_specific.h"

#if 0
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_Mozart_MozartTraces.h"
#endif
#endif
Mozart_Ctrl_ts             g_Mozart_Ctrl =
{
    DEFAULT_MOZART_ENABLE,
    DEFAULT_MOZART_CHROMA_AA,
    DEFAULT_MOZART_M_SB_PEAKER,
    DEFAULT_MOZART_H_SB_PEAKER,
    DEFAULT_MOZART_M_SB_CORING_GAIN,
    DEFAULT_MOZART_H_SB_CORING_GAIN,
    DEFAULT_MOZART_SB_CORING_MIN,
    DEFAULT_MOZART_LUMA_GAIN,
    DEFAULT_MOZART_COLOR_GAIN,
};

Mozart_Ctrl_ts             g_Mozart_Status =
{
    DEFAULT_MOZART_ENABLE,
    DEFAULT_MOZART_CHROMA_AA,
    DEFAULT_MOZART_M_SB_PEAKER,
    DEFAULT_MOZART_H_SB_PEAKER,
    DEFAULT_MOZART_M_SB_CORING_GAIN,
    DEFAULT_MOZART_H_SB_CORING_GAIN,
    DEFAULT_MOZART_SB_CORING_MIN,
    DEFAULT_MOZART_LUMA_GAIN,
    DEFAULT_MOZART_COLOR_GAIN,
};

void
Mozart_Commit(void)
{
    if  (Flag_e_TRUE == g_Mozart_Ctrl.e_Flag_MozartEnable)
    {
        Set_ISP_MOZART_ISP_MOZART_CHROMA_AA_chroma_anti_aliasing(g_Mozart_Status.u8_Mozart_Chroma_AntiAliasing);
        Set_ISP_MOZART_ISP_MOZART_M_SB_PEAKER_m_sb_peaker(g_Mozart_Status.u16_Mozart_M_SBPeaker);
        Set_ISP_MOZART_ISP_MOZART_H_SB_PEAKER_h_sb_peaker(g_Mozart_Status.u16_Mozart_H_SBPeaker);
        Set_ISP_MOZART_ISP_MOZART_M_SB_CORING_GAIN_m_sb_coring_gain(g_Mozart_Status.u8_Mozart_M_SBCoring_gain);
        Set_ISP_MOZART_ISP_MOZART_H_SB_CORING_GAIN_h_sb_coring_gain(g_Mozart_Status.u8_Mozart_H_SBCoring_gain);
        Set_ISP_MOZART_ISP_MOZART_SB_CORING_MIN_sb_coring_min(g_Mozart_Status.u16_Mozart_SBCoring_min);
        Set_ISP_MOZART_ISP_MOZART_LUMA_GAIN_luma_gain(g_Mozart_Status.u8_Mozart_LumaGain);
        Set_ISP_MOZART_ISP_MOZART_COLOR_GAIN_color_gain(g_Mozart_Status.u8_Mozart_ColorGain);
    }
    // Commit the enable/disable of the block
    Set_ISP_MOZART_ISP_MOZART_ENABLE_word(g_Mozart_Status.e_Flag_MozartEnable);
}

void
Mozart_Update(void)
{
    g_Mozart_Status.u8_Mozart_Chroma_AntiAliasing = g_Mozart_Ctrl.u8_Mozart_Chroma_AntiAliasing;
    g_Mozart_Status.u16_Mozart_M_SBPeaker = g_Mozart_Ctrl.u16_Mozart_M_SBPeaker;
    g_Mozart_Status.u16_Mozart_H_SBPeaker = g_Mozart_Ctrl.u16_Mozart_H_SBPeaker;
    g_Mozart_Status.u8_Mozart_M_SBCoring_gain = g_Mozart_Ctrl.u8_Mozart_M_SBCoring_gain;
    g_Mozart_Status.u8_Mozart_H_SBCoring_gain = g_Mozart_Ctrl.u8_Mozart_H_SBCoring_gain;
    g_Mozart_Status.u16_Mozart_SBCoring_min = g_Mozart_Ctrl.u16_Mozart_SBCoring_min;
    g_Mozart_Status.u8_Mozart_LumaGain = g_Mozart_Ctrl.u8_Mozart_LumaGain;
    g_Mozart_Status.u8_Mozart_ColorGain = g_Mozart_Ctrl.u8_Mozart_ColorGain;
    g_Mozart_Status.e_Flag_MozartEnable = g_Mozart_Ctrl.e_Flag_MozartEnable;
}

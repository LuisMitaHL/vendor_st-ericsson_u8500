/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
\file Adsoc.c
 */
#include "Adsoc.h"
#include "Adsoc_ip_interface.h"

#define ADSOC_ENABLE_TRACE  0

Adsoc_PK_Ctrl_ts    g_Adsoc_PK_Ctrl[ADSOC_NO_OF_HARDWARE_PIPE_IN_ISP] =
{
    { DEFAULT_ADSOC_PK_ENABLE, DEFAULT_ADSOC_PK_GAIN, DEFAULT_ADSOC_PK_ADAPTIVE_SHARPENING_ENABLE,
       DEFAULT_ADSCO_PK_CORING_LEVEL, DEFAULT_ADSOC_PK_OVERSHOOT_GAIN_BRIGHT, DEFAULT_ADSOC_PK_OVERSHOOT_GAIN_DARK,
       DEFAULT_ADSOC_PK_EMBOSS_EFFECT_CTRL, DEFAULT_ADSOC_PK_FLIPPER_CTRL, DEFAULT_ADSOC_PK_GRAYBACK_CTRL, },

    { DEFAULT_ADSOC_PK_ENABLE, DEFAULT_ADSOC_PK_GAIN, DEFAULT_ADSOC_PK_ADAPTIVE_SHARPENING_ENABLE,
       DEFAULT_ADSCO_PK_CORING_LEVEL, DEFAULT_ADSOC_PK_OVERSHOOT_GAIN_BRIGHT, DEFAULT_ADSOC_PK_OVERSHOOT_GAIN_DARK,
       DEFAULT_ADSOC_PK_EMBOSS_EFFECT_CTRL, DEFAULT_ADSOC_PK_FLIPPER_CTRL, DEFAULT_ADSOC_PK_GRAYBACK_CTRL, }
};


Adsoc_RP_Ctrl_ts    g_Adsoc_RP_Ctrl[ADSOC_NO_OF_HARDWARE_PIPE_IN_ISP] =
{
    { DEFAULT_RADIAL_ADSOC_RP_HOFFSET, DEFAULT_RADIAL_ADSOC_RP_VOFFSET, DEFAULT_RADIAL_ADSOC_RP_ENABLE,
       DEFAULT_RADIAL_ADSOC_RP_POLYCOEF0, DEFAULT_RADIAL_ADSOC_RP_POLYCOEF1, DEFAULT_RADIAL_ADSOC_RP_COF_SHIFT,
       DEFAULT_RADIAL_ADSOC_RP_OUT_COF_SHIFT, DEFAULT_RADIAL_ADSOC_RP_UNITY },

    { DEFAULT_RADIAL_ADSOC_RP_HOFFSET, DEFAULT_RADIAL_ADSOC_RP_VOFFSET, DEFAULT_RADIAL_ADSOC_RP_ENABLE,
       DEFAULT_RADIAL_ADSOC_RP_POLYCOEF0, DEFAULT_RADIAL_ADSOC_RP_POLYCOEF1, DEFAULT_RADIAL_ADSOC_RP_COF_SHIFT,
       DEFAULT_RADIAL_ADSOC_RP_OUT_COF_SHIFT, DEFAULT_RADIAL_ADSOC_RP_UNITY }
};

Adsoc_RP_Status_ts  g_Adsoc_RP_Status[ADSOC_NO_OF_HARDWARE_PIPE_IN_ISP] =
{
    { DEFAULT_RADIAL_ADSOC_RP_SCALE_X, DEFAULT_RADIAL_ADSOC_RP_SCALE_Y, DEFAULT_RADIAL_ADSOC_RP_HOFFSET, DEFAULT_RADIAL_ADSOC_RP_VOFFSET },

    { DEFAULT_RADIAL_ADSOC_RP_SCALE_X, DEFAULT_RADIAL_ADSOC_RP_SCALE_Y, DEFAULT_RADIAL_ADSOC_RP_HOFFSET, DEFAULT_RADIAL_ADSOC_RP_VOFFSET }
};




// damper structure for pipe 0
//ADSOC_0_Shared_Damper_ts    ADSOC_0_Shared_Damper;

/// damper structure for pipe 1

//ADSOC_1_Shared_Damper_ts    ADSOC_1_Shared_Damper;

/// this function programs the Color Engine with the appropriate image sharpening parameters
void
Adsoc_Commit(
uint8_t e_PipeNo)
{
    if (Pipe_0 == e_PipeNo)
    {
        if (Flag_e_TRUE == g_Adsoc_PK_Ctrl[e_PipeNo].e_Flag_Adsoc_PK_Enable)
        {
#if ADSOC_ENABLE_TRACE
            OstTraceInt1(TRACE_DEBUG, "Flag_e_TRUE == g_Adsoc_PK_Ctrl[%u].e_Flag_Adsoc_PK_Enable", e_PipeNo);
#endif

            /// enable/disable adaptive sharpness
            Set_Pipe0_ADSOC_PK_ADSHARP_EN(g_Adsoc_PK_Ctrl[e_PipeNo].e_Flag_Adsoc_PK_AdaptiveSharpening_Enable);

            /// set adsoc coring level
            Set_Pipe0_ADSOC_PK_CORING_LEVEL(g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_Coring_Level);

            /// set adsoc overshoot bright/dark gain ctrl
            Set_Pipe0_ADSOC_PK_OSHOOT_CTRL(
            g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_OverShoot_Gain_Bright,
            g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_OverShoot_Gain_Dark);

            /// set adsoc effects (Emboss, Flipper, GrayBack)
            Set_Pipe0_ADSOC_PK_EFFECTS(
            g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_Emboss_Effect_Ctrl,
            g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_Flipper_Ctrl,
            g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_GrayBack_Ctrl);

            /// set adsoc gain
            Set_Pipe0_ADSOC_PK_GAIN(g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_Gain);
        }


        if (Flag_e_TRUE == g_Adsoc_RP_Ctrl[e_PipeNo].e_Flag_Adsoc_RP_Enable)
        {
#if ADSOC_ENABLE_TRACE
            OstTraceInt1(TRACE_DEBUG, "Flag_e_TRUE == g_Adsoc_RP_Ctrl[%u].e_Flag_Adsoc_RP_Enable", e_PipeNo);
#endif
            g_Adsoc_RP_Status[e_PipeNo].s16_Adsoc_RP_HOffset = g_Adsoc_RP_Ctrl[e_PipeNo].u16_Lens_Centre_HOffset - Get_ADSOC_RP_SENSOR_HSTART();
            g_Adsoc_RP_Status[e_PipeNo].s16_Adsoc_RP_VOffset = g_Adsoc_RP_Ctrl[e_PipeNo].u16_Lens_Centre_VOffset - Get_ADSOC_RP_SENSOR_VSTART();
            g_Adsoc_RP_Status[e_PipeNo].u16_Adsoc_RP_Scale_X = Get_ADSOC_RP_SCALE_X();
            g_Adsoc_RP_Status[e_PipeNo].u16_Adsoc_RP_Scale_Y = Get_ADSOC_RP_SCALE_Y();

            /// set adsoc radial peaking H-Offset
            Set_Pipe0_ADSOC_RP_LENS_CENTRE_HOFFSET(g_Adsoc_RP_Status[e_PipeNo].s16_Adsoc_RP_HOffset);

            /// set adsoc radial peaking V -Offset
            Set_Pipe0_ADSOC_RP_LENS_CENTRE_VOFFSET(g_Adsoc_RP_Status[e_PipeNo].s16_Adsoc_RP_VOffset);

            /// set adsoc radial peaking X scale value
            Set_Pipe0_ADSOC_RP_SCALE_X(g_Adsoc_RP_Status[e_PipeNo].u16_Adsoc_RP_Scale_X);

            /// set adsoc radial peaking Y scale value
            Set_Pipe0_ADSOC_RP_SCALE_Y(g_Adsoc_RP_Status[e_PipeNo].u16_Adsoc_RP_Scale_Y);

            /// set adsoc radial peaking polycoeff_0
            Set_Pipe0_ADSOC_RP_POLYCOEF0(g_Adsoc_RP_Ctrl[e_PipeNo].u8_Radial_Adsoc_RP_Polycoef0);

            /// set adsoc radial peaking polycoeff_1
            Set_Pipe0_ADSOC_RP_POLYCOEF1(g_Adsoc_RP_Ctrl[e_PipeNo].u8_Radial_Adsoc_RP_Polycoef1);

            /// set adsoc radial peaking COF shift
            Set_Pipe0_ADSOC_RP_COF_SHIFT(g_Adsoc_RP_Ctrl[e_PipeNo].u8_Radial_Adsoc_RP_COF_Shift);

            /// set adsoc radial peaking COF shift out
            Set_Pipe0_ADSOC_RP_COF_OUT_SHIFT(g_Adsoc_RP_Ctrl[e_PipeNo].u8_Radial_Adsoc_RP_Out_COF_Shift);

            /// set adsoc radial peaking RP unity
            Set_Pipe0_ADSOC_RP_UNITY(g_Adsoc_RP_Ctrl[e_PipeNo].u8_Radial_Adsoc_RP_Unity);
        }


        /// enable/disable the adsoc RP block
        Set_Pipe0_ADSOC_PK_ENABLE(g_Adsoc_PK_Ctrl[e_PipeNo].e_Flag_Adsoc_PK_Enable);

        /// enable/disable the adsoc block
        Set_Pipe0_ADSOC_RP_ENABLE(g_Adsoc_RP_Ctrl[e_PipeNo].e_Flag_Adsoc_RP_Enable);
    }
    else
    {
        if (Flag_e_TRUE == g_Adsoc_PK_Ctrl[e_PipeNo].e_Flag_Adsoc_PK_Enable)
        {
#if ADSOC_ENABLE_TRACE
            OstTraceInt1(TRACE_DEBUG, "Flag_e_TRUE == g_Adsoc_PK_Ctrl[%u].e_Flag_Adsoc_PK_Enable", e_PipeNo);
#endif

            /// enable/disable adaptive sharpness
            Set_Pipe1_ADSOC_PK_ADSHARP_EN(g_Adsoc_PK_Ctrl[e_PipeNo].e_Flag_Adsoc_PK_AdaptiveSharpening_Enable);

            /// set adsoc coring level
            Set_Pipe1_ADSOC_PK_CORING_LEVEL(g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_Coring_Level);

            /// set adsoc overshoot bright/dark gain ctrl
            Set_Pipe1_ADSOC_PK_OSHOOT_CTRL(
            g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_OverShoot_Gain_Bright,
            g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_OverShoot_Gain_Dark);

            /// set adsoc effects (Emboss, Flipper, GrayBack)
            Set_Pipe1_ADSOC_PK_EFFECTS(
            g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_Emboss_Effect_Ctrl,
            g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_Flipper_Ctrl,
            g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_GrayBack_Ctrl);

            /// set adsoc gain
            Set_Pipe1_ADSOC_PK_GAIN(g_Adsoc_PK_Ctrl[e_PipeNo].u8_Adsoc_PK_Gain);
        }


        if (Flag_e_TRUE == g_Adsoc_RP_Ctrl[e_PipeNo].e_Flag_Adsoc_RP_Enable)
        {
#if ADSOC_ENABLE_TRACE
            OstTraceInt1(TRACE_DEBUG, "Flag_e_TRUE == g_Adsoc_RP_Ctrl[%u].e_Flag_Adsoc_RP_Enable", e_PipeNo);
#endif
            g_Adsoc_RP_Status[e_PipeNo].s16_Adsoc_RP_HOffset = g_Adsoc_RP_Ctrl[e_PipeNo].u16_Lens_Centre_HOffset - Get_ADSOC_RP_SENSOR_HSTART();
            g_Adsoc_RP_Status[e_PipeNo].s16_Adsoc_RP_VOffset = g_Adsoc_RP_Ctrl[e_PipeNo].u16_Lens_Centre_VOffset - Get_ADSOC_RP_SENSOR_VSTART();
            g_Adsoc_RP_Status[e_PipeNo].u16_Adsoc_RP_Scale_X = (int16_t) Get_ADSOC_RP_SCALE_X();
            g_Adsoc_RP_Status[e_PipeNo].u16_Adsoc_RP_Scale_Y = (int16_t) Get_ADSOC_RP_SCALE_Y();

            /// set adsoc radial peaking H-Offset
            Set_Pipe1_ADSOC_RP_LENS_CENTRE_HOFFSET(g_Adsoc_RP_Status[e_PipeNo].s16_Adsoc_RP_HOffset);

            /// set adsoc radial peaking V -Offset
            Set_Pipe1_ADSOC_RP_LENS_CENTRE_VOFFSET(g_Adsoc_RP_Status[e_PipeNo].s16_Adsoc_RP_VOffset);

            /// set adsoc radial peaking X scale value
            Set_Pipe1_ADSOC_RP_SCALE_X(g_Adsoc_RP_Status[e_PipeNo].u16_Adsoc_RP_Scale_X);

            /// set adsoc radial peaking Y scale value
            Set_Pipe1_ADSOC_RP_SCALE_Y(g_Adsoc_RP_Status[e_PipeNo].u16_Adsoc_RP_Scale_Y);

            /// set adsoc radial peaking polycoeff_0
            Set_Pipe1_ADSOC_RP_POLYCOEF0(g_Adsoc_RP_Ctrl[e_PipeNo].u8_Radial_Adsoc_RP_Polycoef0);

            /// set adsoc radial peaking polycoeff_1
            Set_Pipe1_ADSOC_RP_POLYCOEF1(g_Adsoc_RP_Ctrl[e_PipeNo].u8_Radial_Adsoc_RP_Polycoef1);

            /// set adsoc radial peaking COF shift
            Set_Pipe1_ADSOC_RP_COF_SHIFT(g_Adsoc_RP_Ctrl[e_PipeNo].u8_Radial_Adsoc_RP_COF_Shift);

            /// set adsoc radial peaking COF shift out
            Set_Pipe1_ADSOC_RP_COF_OUT_SHIFT(g_Adsoc_RP_Ctrl[e_PipeNo].u8_Radial_Adsoc_RP_Out_COF_Shift);

            /// set adsoc radial peaking RP unity
            Set_Pipe1_ADSOC_RP_UNITY(g_Adsoc_RP_Ctrl[e_PipeNo].u8_Radial_Adsoc_RP_Unity);
        }


        /// enable/disable the adsoc block
        Set_Pipe1_ADSOC_PK_ENABLE(g_Adsoc_PK_Ctrl[e_PipeNo].e_Flag_Adsoc_PK_Enable);

        /// enable/disable the adsoc block
        Set_Pipe1_ADSOC_RP_ENABLE(g_Adsoc_RP_Ctrl[e_PipeNo].e_Flag_Adsoc_RP_Enable);
    }
}


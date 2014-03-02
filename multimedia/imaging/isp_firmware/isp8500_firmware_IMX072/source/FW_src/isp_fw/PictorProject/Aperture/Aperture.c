/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  Aperture

\details   The Aperure module provides an interface to the host through which Aperture value
           can be set
*/

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      Aperture.c

 \brief

 \ingroup   Aperture
 \endif
*/
#include "Aperture.h"
#include "lla_abstraction.h"
#include "HostInterface.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#   include "PictorProject_Aperture_ApertureTraces.h"
#endif
ApertureConfig_Control_ts   g_ApertureConfig_Control = { DEFAULT_APERTURE_VALUE_X100, 0, Coin_e_Heads };
ApertureConfig_Status_ts    g_ApertureConfig_Status = { DEFAULT_APERTURE_VALUE_X100, 0, 0, Coin_e_Heads };

uint8_t
LLA_Abstraction_Is_Aperture_Supported(void)
{
    if
    (
        (g_camera_details.p_aperture_details != NULL)
    &&  (g_camera_details.p_aperture_details->p_f_number_x_100 != NULL)
    &&  (g_camera_details.p_aperture_details->number_of_supported_apertures > 0)
    )
    {
        return (Flag_e_TRUE);
    }
    else
    {
        return (Flag_e_FALSE);
    }
}


void
LLA_Abstraction_Set_Aperture(void)
{
    uint16_t            u16_f_number_x_100;
    CAM_DRV_RETVAL_E    retVal = CAM_DRV_OK;

    u16_f_number_x_100 = g_ApertureConfig_Control.u16_requested_f_number_x_100;
    retVal = cam_drv_aperture_control(CAM_DRV_APERTURE_VALUE_SET, &u16_f_number_x_100);

    if (CAM_DRV_OK != retVal)
    {
#if USE_TRACE_ERROR
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!cam_drv_aperture_control() returned error!!");
#endif
        ASSERT_XP70();
    }
    else
    {
        g_ApertureConfig_Status.u16_applied_f_number_x_100 = u16_f_number_x_100;
        g_FrameParamStatus.u32_applied_f_number_x100 = g_ApertureConfig_Status.u16_applied_f_number_x_100;
    }
}


void
ReadApertureConfig(void)
{
    if (g_ApertureConfig_Control.e_Coin_Ctrl != g_ApertureConfig_Status.e_Coin_Status)
    {
        if (LLA_Abstraction_Is_Aperture_Supported())
        {
            if ((g_ApertureConfig_Status.u16_number_of_apertures - 1) >= g_ApertureConfig_Control.u16_SelectAperture)
            {
                g_ApertureConfig_Status.u16_f_number_x_100 = g_camera_details.p_aperture_details->p_f_number_x_100[g_ApertureConfig_Control.u16_SelectAperture];
            }
        }
        else
        {
            g_ApertureConfig_Status.u16_f_number_x_100 = DEFAULT_APERTURE_VALUE_X100;
        }


        g_ApertureConfig_Status.e_Coin_Status = g_ApertureConfig_Control.e_Coin_Ctrl;
    }
}


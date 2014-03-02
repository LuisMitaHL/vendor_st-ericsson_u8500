/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  HDR HDR

 \details   The HDR module provides an interface to send n frames with differnt exposure to make
                an HDR frames.
*/

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      HDR.c

 \brief     This file exposes an interface to use HDR feature.

 \ingroup   HDR
 \endif
*/
#include "HDR.h"
#include "Platform.h"
#include "SystemConfig.h"
//#include "PictorhwReg.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_HDR_HDRTraces.h"
#endif

HDR_GainControl_ts g_HDR_GainControl =
{
    DEFAULT_DIGITAL_GAIN_RED_1,
    DEFAULT_DIGITAL_GAIN_GREEN_1,
    DEFAULT_DIGITAL_GAIN_BLUE_1,
    DEFAULT_DIGITAL_GAIN_RED_1,
    DEFAULT_DIGITAL_GAIN_GREEN_1,
    DEFAULT_DIGITAL_GAIN_BLUE_1,
    DEFAULT_DIGITAL_GAIN_RED_1,
    DEFAULT_DIGITAL_GAIN_GREEN_1,
    DEFAULT_DIGITAL_GAIN_BLUE_1,
    DEFAULT_DIGITAL_GAIN_RED_1,
    DEFAULT_DIGITAL_GAIN_GREEN_1,
    DEFAULT_DIGITAL_GAIN_BLUE_1,
    DEFAULT_ANALOG_GAIN_X256_1,
    DEFAULT_ANALOG_GAIN_X256_2,
    DEFAULT_ANALOG_GAIN_X256_3,
    DEFAULT_ANALOG_GAIN_X256_4,
};

HDR_Control_ts g_HDR_Control =
{
    DEFAULT_FRAME_RATE_1,
    DEFAULT_FRAME_RATE_2,
    DEFAULT_FRAME_RATE_3,
    DEFAULT_FRAME_RATE_4,
    DEFAULT_EXPOSURE_US_1,
    DEFAULT_EXPOSURE_US_2,
    DEFAULT_EXPOSURE_US_3,
    DEFAULT_EXPOSURE_US_4,
    DEFAULT_HDR_FRAME_COUNT,
    DEFAULT_HDR_FPS_CONFIG_COUNT,
    DEFAULT_CONTROL_COIN_HDR,
};

volatile HDR_Status_ts g_HDR_Status =
{
    DEFAULT_FLOAT_DUMMY_VALUE,
    DEFAULT_FLOAT_DUMMY_VALUE,
    DEFAULT_FLOAT_DUMMY_VALUE,
    DEFAULT_FLOAT_DUMMY_VALUE,
    DEFAULT_DUMMY_VALUE,
    DEFAULT_DUMMY_VALUE,
    DEFAULT_DUMMY_VALUE,
    DEFAULT_DUMMY_VALUE,
    DEFAULT_DUMMY_VALUE,
    DEFAULT_DUMMY_VALUE,
    DEFAULT_DUMMY_VALUE,
    DEFAULT_DUMMY_VALUE,
    DEFAULT_STATUS_COIN_HDR,
};

HDR_StatusVariable_ts g_HDR_StatusVariable =
{
    DEFAULT_FRAME_RECEIVED,
    DEFAULT_FRAME_RECEIVED,
};


/*
*******************************************************************************
 \fn void HDR_RequestExposure(void)
 \brief
 \param
 \return void
 \ingroup HDR
 \callgraph
 \callergraph
*******************************************************************************/
void HDR_RequestExposure(void)
{
    uint32_t u32_TargetExposureTime_us = 0;
    uint16_t u16_TargetAnalogGain_x256 = 0;
    float_t f_FrameRate_Hz = 0.0;

    if(IS_HDR_COIN_TOGGLED()
         && (g_HDR_StatusVariable.u8_ParamsReqFrameCount < g_HDR_Control.u8_FramesCount)
        )
    {
        switch (g_HDR_StatusVariable.u8_ParamsReqFrameCount)
        {
            // TODO : make it configurable so that it can be applied to any sensor
            case 0:
                u32_TargetExposureTime_us = g_HDR_Control.u32_TargetExposureTime_1_us;
                u16_TargetAnalogGain_x256 = g_HDR_GainControl.u16_TargetAnalogGain_1_x256;
                f_FrameRate_Hz = g_HDR_Control.f_FrameRate_1_Hz;
                g_HDR_StatusVariable.u8_ParamsReqFrameCount = 1;
                break;

            case 1:
                u32_TargetExposureTime_us = g_HDR_Control.u32_TargetExposureTime_2_us;
                u16_TargetAnalogGain_x256 = g_HDR_GainControl.u16_TargetAnalogGain_2_x256;
                f_FrameRate_Hz = g_HDR_Control.f_FrameRate_2_Hz;
                g_HDR_StatusVariable.u8_ParamsReqFrameCount = 2;
                break;

            case 2:
                u32_TargetExposureTime_us = g_HDR_Control.u32_TargetExposureTime_3_us;
                u16_TargetAnalogGain_x256 = g_HDR_GainControl.u16_TargetAnalogGain_3_x256;
                f_FrameRate_Hz = g_HDR_Control.f_FrameRate_3_Hz;
                g_HDR_StatusVariable.u8_ParamsReqFrameCount = 3;
                break;

            case 3:
                u32_TargetExposureTime_us = g_HDR_Control.u32_TargetExposureTime_4_us;
                u16_TargetAnalogGain_x256 = g_HDR_GainControl.u16_TargetAnalogGain_4_x256;
                f_FrameRate_Hz = g_HDR_Control.f_FrameRate_4_Hz;
                g_HDR_StatusVariable.u8_ParamsReqFrameCount = 4;
                break;

            default:
                u32_TargetExposureTime_us = 0;
                u16_TargetAnalogGain_x256 = 0;
                f_FrameRate_Hz = 0;
                OstTraceInt1(TRACE_USER7, "HDR_RequestExposure: Unhandled state: Exp FC:%d", g_HDR_StatusVariable.u8_ParamsReqFrameCount);
                break;
        }

        LLA_Abstraction_ApplyAec(f_FrameRate_Hz, u32_TargetExposureTime_us, u16_TargetAnalogGain_x256);
        LLA_Abstraction_ConfigSet_streaming();
        SystemConfig_SetSensorSettingStatus(UpdateStatus_e_Updating);
    }
}

/*
*******************************************************************************
 \fn void HDR_IsAEC_Absorbed(void)
 \brief
 \param
 \return void
 \ingroup HDR
 \callgraph
 \callergraph
*******************************************************************************/
Flag_te HDR_IsAEC_Absorbed(void)
{
    Flag_te e_AECAbsorbedStatus = Flag_e_FALSE;
    if(IS_HDR_COIN_TOGGLED())
    {
        switch (g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount)
        {
            case 0:
                if(
                    (g_SensorSettings.exposure_time_us == g_HDR_Status.u32_TargetExposureTime_1_us)
                       && ((((uint32_t) g_SensorSettings.analog_gain_x1000 * 256) / 1000) == g_HDR_Status.u16_TargetAnalogGain_1_x256)
                   )
                {
                    g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount = 1;
                    e_AECAbsorbedStatus = Flag_e_TRUE;
                }
                break;

            case 1:
                if(
                    (g_SensorSettings.exposure_time_us == g_HDR_Status.u32_TargetExposureTime_2_us)
                       && ((((uint32_t) g_SensorSettings.analog_gain_x1000 * 256) / 1000) == g_HDR_Status.u16_TargetAnalogGain_2_x256)
                   )
                {
                    g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount = 2;
                    e_AECAbsorbedStatus = Flag_e_TRUE;
                }
                else
                {
                    OstTraceInt1(TRACE_WARNING, "<HDR> Frames Not Consecutive. FrameCnt: %d", g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount);
                }
                break;

            case 2:
                if(
                    (g_SensorSettings.exposure_time_us == g_HDR_Status.u32_TargetExposureTime_3_us)
                       && ((((uint32_t) g_SensorSettings.analog_gain_x1000 * 256) / 1000) == g_HDR_Status.u16_TargetAnalogGain_3_x256)
                   )
                {
                    g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount = 3;
                    e_AECAbsorbedStatus = Flag_e_TRUE;
                }
                else
                {
                    OstTraceInt1(TRACE_WARNING, "<HDR> Frames Not Consecutive. FrameCnt: %d", g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount);
                }
                break;

            case 3:
                if(
                    (g_SensorSettings.exposure_time_us == g_HDR_Status.u32_TargetExposureTime_4_us)
                       && ((((uint32_t) g_SensorSettings.analog_gain_x1000 * 256) / 1000) == g_HDR_Status.u16_TargetAnalogGain_4_x256)
                   )
                {
                    g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount = 4;
                    e_AECAbsorbedStatus = Flag_e_TRUE;
                }
                else
                {
                    OstTraceInt1(TRACE_WARNING, "<HDR> Frames Not Consecutive. FrameCnt: %d", g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount);
                }
                break;
        }
    }
    return e_AECAbsorbedStatus;
}

/*
*******************************************************************************
 \fn void HDR_IsAEC_Absorbed(void)
 \brief
 \param
 \return void
 \ingroup HDR
 \callgraph
 \callergraph
*******************************************************************************/
void HDR_CheckDMAValidFrame(void)
{
    if(IS_HDR_COIN_TOGGLED())
    {
        OstTraceInt1(TRACE_USER6, "<HDR> DMA-OK frame no: %d", g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount);
        if(g_HDR_Control.u8_FramesCount == g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount)
        {
            OstTraceInt0(TRACE_USER6, "<HDR> ALL HDR DMA-OK Sent ");
            RESET_HDR_COIN();
            g_HDR_StatusVariable.u8_ParamsReqFrameCount = 0;
            g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount =0;
            SystemConfig_SetSensorSettingStatus(UpdateStatus_e_Idle);
        }
        else
        {
            SystemConfig_SetSensorSettingStatus(UpdateStatus_e_Updating);
        }
    }
}


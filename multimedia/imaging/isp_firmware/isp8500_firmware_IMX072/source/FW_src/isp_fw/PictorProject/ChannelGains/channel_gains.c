/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\file channel_gains.c
\brief channel gain or digital calculations
\ingroup WBStats
*/
#include "channel_gains_op_interface.h"
#include "smia_sensor_memorymap_defs.h"
#include "SystemConfig.h"
#include "HDR.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_ChannelGains_channel_gainsTraces.h"
#endif

#define SENSOR_TWO_POWER_DATA_WIDTH 1023

//uint16_t ConvertFpToCompFormat(float_t f_Gain);
void                        CalculateOffsetCompensationParameters (void);
void                                                              GetChannelGainValues(float *f_GainValueR, float *f_GainValueG, float *f_GainValueB);

#define DEFALUT_OFFSET                              64
#define DEFAUKT_OFFSET_COMPENSATION_GAIN            1.0
#define DEFAULT_RED_TILT_GAIN                       1.0
#define DEFAULT_GREEN_TILT_GAIN                     1.0
#define DEFAULT_BLUE_TILT_GAIN                      1.0
#define DEFAULT_GUARANTEED_DATA_SATURATION_LEVEL    1023
#define DEFAULT_MINIMUM_SENSOR_RX_PIXEL_VALUE       64
#define DEFAULT_MAXIMUM_SENSOR_RX_PIXEL_VALUE       1023
#define DEFAULT_BLACK_CORRECTION_OFFSET             0

ChannelGains_Control_ts     g_ChannelGains_Control = { Flag_e_TRUE };

OffsetCompensationStatus_ts g_OffsetCompensationStatus = { DEFALUT_OFFSET, DEFAUKT_OFFSET_COMPENSATION_GAIN, };

SensorSetup_ts              g_SensorSetup =
{
    DEFAULT_RED_TILT_GAIN,
    DEFAULT_GREEN_TILT_GAIN,
    DEFAULT_BLUE_TILT_GAIN,
    DEFAULT_GUARANTEED_DATA_SATURATION_LEVEL,
    DEFAULT_MINIMUM_SENSOR_RX_PIXEL_VALUE,
    DEFAULT_MAXIMUM_SENSOR_RX_PIXEL_VALUE,
    DEFAULT_BLACK_CORRECTION_OFFSET,
};

ChannelGains_ts             g_ChannelGains_combined = { 1.0, 1.0, 1.0, 1.0 };

/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \fn      void ChannelGain_Configure(void)
 * \brief   Configures whether Channel gains are to required or not.
 * \return  void
 * \callgraph
 * \callergraph
 * \ingroup WBStats
 * \endif
*/
void
ChannelGain_Configure(void)
{
    if (Flag_e_TRUE == g_ChannelGains_Control.e_Flag_EnableChannelGains)
    {
        Set_ISP_CHG_ISP_CHG_ENABLE_chg_enable__ENABLE();
    }
    else
    {
        Set_ISP_CHG_ISP_CHG_ENABLE_chg_enable__DISABLE();
    }
}


/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \fn      void ChannelGain_CalculateAllGains(void)
 * \brief   Calculates the final gains which are applied in the hw register
 * \return  void
 * \callgraph
 * \callergraph
 * \ingroup WBStats
 * \endif
*/
void
ChannelGain_CalculateAllGains(void)
{
    /*~~~~~~~~~~~~~~~~~~~*/

    //    The Channel gains applied are composed of following gain comonents
    //a.) Digital Gain from WB
    //b.) Digital Gain from EXP
    //c.) OffsetCompensation Gain
    //d.) Tilt.

    float f_ChannelGainRed = 0, f_ChannelGainGreen = 0, f_ChannelGainBlue = 0;

    /*~~~~~~~~~~~~~~~~~~~*/
    if (Flag_e_TRUE == g_ChannelGains_Control.e_Flag_EnableChannelGains)
    {
        CalculateOffsetCompensationParameters();
        GetChannelGainValues(&f_ChannelGainRed, &f_ChannelGainGreen, &f_ChannelGainBlue);

        // Update ChannelGainStatus with combined channel gains.
        g_ChannelGains_combined.f_RedGain = f_ChannelGainRed* Exposure_GetToBeAppliedDigitalGain() *
            g_OffsetCompensationStatus.f_OffsetCompensationGain_RR *
            g_SensorSetup.f_RedTiltGain;
        g_ChannelGains_combined.f_GreenInRedGain = f_ChannelGainGreen * Exposure_GetToBeAppliedDigitalGain() *
            g_OffsetCompensationStatus.f_OffsetCompensationGain_GR *
            g_SensorSetup.f_GreenTiltGain;
        g_ChannelGains_combined.f_GreenInBlueGain = f_ChannelGainGreen * Exposure_GetToBeAppliedDigitalGain() *
            g_OffsetCompensationStatus.f_OffsetCompensationGain_GB *
            g_SensorSetup.f_GreenTiltGain;
        g_ChannelGains_combined.f_BlueGain = f_ChannelGainBlue * Exposure_GetToBeAppliedDigitalGain() *
            g_OffsetCompensationStatus.f_OffsetCompensationGain_BB *
            g_SensorSetup.f_BlueTiltGain;
    }
}


/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \fn      float_t GetChannelGainValue(float, float, float)
 * \brief
 * \return  float_t
 * \callgraph
 * \callergraph
 * \ingroup WBStats
 * \endif
*/
void GetChannelGainValues(float *f_GainValueR, float *f_GainValueG, float *f_GainValueB)
{
    if(IS_HDR_COIN_TOGGLED())
    {
        switch (g_HDR_StatusVariable.u8_ParamsAbsorbedFrameCount)
        {
            case 1:
                *f_GainValueR = g_HDR_GainControl.f_RedGain_1;
                *f_GainValueG = g_HDR_GainControl.f_GreenGain_1;
                *f_GainValueB = g_HDR_GainControl.f_BlueGain_1;
            break;

            case 2:
                *f_GainValueR = g_HDR_GainControl.f_RedGain_2;
                *f_GainValueG = g_HDR_GainControl.f_GreenGain_2;
                *f_GainValueB = g_HDR_GainControl.f_BlueGain_2;
            break;

            case 3:
                *f_GainValueR = g_HDR_GainControl.f_RedGain_3;
                *f_GainValueG = g_HDR_GainControl.f_GreenGain_3;
                *f_GainValueB = g_HDR_GainControl.f_BlueGain_3;
            break;

            case 4:
                *f_GainValueR = g_HDR_GainControl.f_RedGain_4;
                *f_GainValueG = g_HDR_GainControl.f_GreenGain_4;
                *f_GainValueB = g_HDR_GainControl.f_BlueGain_4;
            break;

            default:
                OstTraceInt0(TRACE_WARNING, "<HDR> GetChannelGainValue_Red: Unexpected state");
            break;
        }
    }
    else
    {
        *f_GainValueR =  WhiteBalance_GetRedGain();
        *f_GainValueG  = WhiteBalance_GetGreenGain();
        *f_GainValueB =  WhiteBalance_GetBlueGain();
    }
}

/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \fn      void ChannelGain_ApplyAllGains(void)
 * \brief   Programs the hw registers with the calculated gains.
 * \return  void
 * \callgraph
 * \callergraph
 * \ingroup WBStats
 * \endif
*/
void
ChannelGain_ApplyAllGains(void)
{
    if (Flag_e_TRUE == g_ChannelGains_Control.e_Flag_EnableChannelGains)
    {
        Set_ISP_CHG_ISP_CHG_R_COMP(ConvertFloatToUnsigned_4_DOT_8_format(g_ChannelGains_combined.f_RedGain));           // r_comp
        Set_ISP_CHG_ISP_CHG_G1_COMP(ConvertFloatToUnsigned_4_DOT_8_format(g_ChannelGains_combined.f_GreenInRedGain));   // g1_comp
        Set_ISP_CHG_ISP_CHG_G2_COMP(ConvertFloatToUnsigned_4_DOT_8_format(g_ChannelGains_combined.f_GreenInBlueGain));  // g2_comp
        Set_ISP_CHG_ISP_CHG_B_COMP(ConvertFloatToUnsigned_4_DOT_8_format(g_ChannelGains_combined.f_BlueGain));          // b_comp
        if (RSO_Mode_e_Adaptive == RSO_GetMode())
        {
            Set_RSO_Data_DcTermGr(g_OffsetCompensationStatus.u16_Offset_GR);
            Set_RSO_Data_DcTermR(g_OffsetCompensationStatus.u16_Offset_RR);
            Set_RSO_Data_DcTermB(g_OffsetCompensationStatus.u16_Offset_BB);
            Set_RSO_Data_DcTermGb(g_OffsetCompensationStatus.u16_Offset_GB);
        }
    }
}


/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \fn      void CalculateOffsetCompensationParameters(void)
 * \brief   Calculates the Offset Compensation parameters, u16_Offset & f_OffsetCompensationGain
 * \return  void
 * \callgraph
 * \callergraph
 * \ingroup WBStats
 * \endif
*/
void
CalculateOffsetCompensationParameters(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // the offset to be applied is the sum of the black correction offset and
    // the maximum of the the sensor data pedestal and the Minimum value of
    // the pixel that is possible through the sensor Rx interface
    //uint16_t    u16_max_offset;
    uint16_t    u16_SensorDataPedestalArray[] = { 64 };

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // compute the channel offset...
    //g_OffsetCompensationStatus.u16_Offset = g_SensorSetup.u8_BlackCorrectionOffset + max(u16_SensorDataPedestalArray[0], g_SensorSetup.u16_MinimumSensorRxPixelValue);
    g_OffsetCompensationStatus.u16_Offset_GR = g_OffsetCompensationStatus.u16_Offset_RR = g_OffsetCompensationStatus.
            u16_Offset_BB = g_OffsetCompensationStatus.u16_Offset_GB = g_SensorSetup.u8_BlackCorrectionOffset +
        max(u16_SensorDataPedestalArray[0], g_SensorSetup.u16_MinimumSensorRxPixelValue);

    /*
        u16_max_offset = FindMaximumOffsetInChannel(0, 0, 640, 480, RSO_Channel_e_GR);
        g_OffsetCompensationStatus.u16_Offset_GR = g_SensorSetup.u8_BlackCorrectionOffset + max(u16_max_offset, g_SensorSetup.u16_MinimumSensorRxPixelValue);
        u16_max_offset = FindMaximumOffsetInChannel(0, 0, 640, 480, RSO_Channel_e_RR);
        g_OffsetCompensationStatus.u16_Offset_RR = g_SensorSetup.u8_BlackCorrectionOffset + max(u16_max_offset, g_SensorSetup.u16_MinimumSensorRxPixelValue);
        u16_max_offset = FindMaximumOffsetInChannel(0, 0, 640, 480, RSO_Channel_e_BB);
        g_OffsetCompensationStatus.u16_Offset_BB = g_SensorSetup.u8_BlackCorrectionOffset + max(u16_max_offset, g_SensorSetup.u16_MinimumSensorRxPixelValue);
        u16_max_offset = FindMaximumOffsetInChannel(0, 0, 640, 480, RSO_Channel_e_GB);
        g_OffsetCompensationStatus.u16_Offset_GB = g_SensorSetup.u8_BlackCorrectionOffset + max(u16_max_offset, g_SensorSetup.u16_MinimumSensorRxPixelValue);
    */

    /* OffsetCompensationGain =  (INPUTPROCESSOR_SENSOR_TWOPOWERDATAWIDTH /
                                              (SensorSetup.GuaranteedDataSaturationLevel -
                                               (INPUTPROCESSOR_SENSOR_PEDESTAL + SensorSetup.BlackCorrectionOffset))) */

    // maximum input pixel value is the minimum of the guaranteed data saturation level and
    // the maximum pixel value possible at the sensor Rx interface...
    g_OffsetCompensationStatus.f_OffsetCompensationGain_GR = SENSOR_TWO_POWER_DATA_WIDTH /
        (
            min(g_SensorSetup.u16_GuaranteedDataSaturationLevel, g_SensorSetup.u16_MaximumSensorRxPixelValue) -
            g_OffsetCompensationStatus.u16_Offset_GR
        );
    g_OffsetCompensationStatus.f_OffsetCompensationGain_RR = SENSOR_TWO_POWER_DATA_WIDTH /
        (
            min(g_SensorSetup.u16_GuaranteedDataSaturationLevel, g_SensorSetup.u16_MaximumSensorRxPixelValue) -
            g_OffsetCompensationStatus.u16_Offset_RR
        );
    g_OffsetCompensationStatus.f_OffsetCompensationGain_BB = SENSOR_TWO_POWER_DATA_WIDTH /
        (
            min(g_SensorSetup.u16_GuaranteedDataSaturationLevel, g_SensorSetup.u16_MaximumSensorRxPixelValue) -
            g_OffsetCompensationStatus.u16_Offset_BB
        );
    g_OffsetCompensationStatus.f_OffsetCompensationGain_GB = SENSOR_TWO_POWER_DATA_WIDTH /
        (
            min(g_SensorSetup.u16_GuaranteedDataSaturationLevel, g_SensorSetup.u16_MaximumSensorRxPixelValue) -
            g_OffsetCompensationStatus.u16_Offset_GB
        );

    return;
}


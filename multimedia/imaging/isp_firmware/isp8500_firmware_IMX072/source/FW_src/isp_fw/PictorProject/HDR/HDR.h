/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if        INCLUDE_IN_HTML_ONLY
 \file      HDR.h

 \brief     This file exposes an interface to use HDR feature.

 \ingroup   HDR
 \endif
*/
#ifndef _HDR_H_
#   define _HDR_H_

#include "Platform.h"

#define IS_HDR_COIN_TOGGLED()                                     (g_HDR_Status.e_Coin != g_HDR_Control.e_Coin)
#define RESET_HDR_COIN()                                          (g_HDR_Status.e_Coin = g_HDR_Control.e_Coin)


/**
 \struct    HDR_GainControl_ts
 \brief
 \ingroup
 \endif
*/
typedef struct
{
    /// Digital Gain for 1st HDR frame
    float_t    f_RedGain_1;

    /// Digital Gain for 1st HDR frame
    float_t    f_GreenGain_1;

    /// Digital Gain for 1st HDR frame
    float_t    f_BlueGain_1;

    /// Digital Gain for 1st HDR frame
    float_t    f_RedGain_2;

    /// Digital Gain for 1st HDR frame
    float_t    f_GreenGain_2;

    /// Digital Gain for 1st HDR frame
    float_t    f_BlueGain_2;

    /// Digital Gain for 1st HDR frame
    float_t    f_RedGain_3;

    /// Digital Gain for 1st HDR frame
    float_t    f_GreenGain_3;

    /// Digital Gain for 1st HDR frame
    float_t    f_BlueGain_3;

    /// Digital Gain for 1st HDR frame
    float_t    f_RedGain_4;

    /// Digital Gain for 1st HDR frame
    float_t    f_GreenGain_4;

    /// Digital Gain for 1st HDR frame
    float_t    f_BlueGain_4;

    /// Analog Gain Requested for 1st HDR Frame
    uint16_t    u16_TargetAnalogGain_1_x256;

    /// Analog Gain Requested for 2nd HDR Frame
    uint16_t    u16_TargetAnalogGain_2_x256;

    /// Analog Gain Requested for 3rd HDR Frame
    uint16_t    u16_TargetAnalogGain_3_x256;

    /// Analog Gain Requested for 3rd HDR Frame
    uint16_t    u16_TargetAnalogGain_4_x256;
}HDR_GainControl_ts;


/**
 \struct    HDR_Control_ts
 \brief
 \ingroup
 \endif
*/
typedef struct
{
    /// FrameRate for 1st HDR frame
    float_t    f_FrameRate_1_Hz;

    /// FrameRate for 2nd HDR frame
    float_t    f_FrameRate_2_Hz;

    /// FrameRate for 3rd HDR frame
    float_t    f_FrameRate_3_Hz;

    /// FrameRate for 4th HDR frame
    float_t    f_FrameRate_4_Hz;

    /// Exposure Requested for 1st HDR Frame
    uint32_t    u32_TargetExposureTime_1_us;

    /// Exposure Requested for 2nd HDR Frame
    uint32_t    u32_TargetExposureTime_2_us;

    /// Exposure Requested for 3rd HDR Frame
    uint32_t    u32_TargetExposureTime_3_us;

    /// Exposure Requested for 4th HDR Frame
    uint32_t    u32_TargetExposureTime_4_us;

    /// No of HDR Frames required
    uint8_t     u8_FramesCount;

    /// No of HDR Frames for which different fps is requested
    uint8_t     u8_FPSConfigCount;

    /// Coin for HDR
    uint8_t     e_Coin;
} HDR_Control_ts;


/**
 \struct    HDR_Status_ts
 \brief
 \ingroup
 \endif
*/
typedef struct
{
    /// FrameRate for 1st HDR frame
    float_t    f_FrameRate_1_Hz;

    /// FrameRate for 1st HDR frame
    float_t    f_FrameRate_2_Hz;

    /// FrameRate for 1st HDR frame
    float_t    f_FrameRate_3_Hz;

    /// FrameRate for 1st HDR frame
    float_t    f_FrameRate_4_Hz;

    /// Exposure Requested for 1st HDR Frame
    uint32_t    u32_TargetExposureTime_1_us;

    /// Exposure Requested for 2nd HDR Frame
    uint32_t    u32_TargetExposureTime_2_us;

    /// Exposure Requested for 3rd HDR Frame
    uint32_t    u32_TargetExposureTime_3_us;

    /// Exposure Requested for 3rd HDR Frame
    uint32_t    u32_TargetExposureTime_4_us;

     /// Analog Gain Requested for 1st HDR Frame
    uint16_t    u16_TargetAnalogGain_1_x256;

    /// Analog Gain Requested for 2nd HDR Frame
    uint16_t    u16_TargetAnalogGain_2_x256;

    /// Analog Gain Requested for 3rd HDR Frame
    uint16_t    u16_TargetAnalogGain_3_x256;

    /// Analog Gain Requested for 3rd HDR Frame
    uint16_t    u16_TargetAnalogGain_4_x256;

    /// Status coin
    uint8_t     e_Coin;
} HDR_Status_ts;


/**
 \struct    HDR_StatusVariable_ts
 \brief
 \ingroup
 \endif
*/
typedef struct
{
    /// No of frames for which paramerter has been requested
    uint8_t    u8_ParamsReqFrameCount;

    ///No of frames for which requested parameters has been absorbed and Grab-Ok is sent from FW
    uint8_t    u8_ParamsAbsorbedFrameCount;
}HDR_StatusVariable_ts;


#define DEFAULT_FRAME_RATE_1                                      (10.0)
#define DEFAULT_FRAME_RATE_2                                      (10.0)
#define DEFAULT_FRAME_RATE_3                                      (10.0)
#define DEFAULT_FRAME_RATE_4                                      (10.0)
#define DEFAULT_DIGITAL_GAIN_RED_1                                (1.0)
#define DEFAULT_DIGITAL_GAIN_GREEN_1                              (1.0)
#define DEFAULT_DIGITAL_GAIN_BLUE_1                               (1.0)

#define DEFAULT_EXPOSURE_US_1                                     (10000)
#define DEFAULT_EXPOSURE_US_2                                     (10000)
#define DEFAULT_EXPOSURE_US_3                                     (10000)
#define DEFAULT_EXPOSURE_US_4                                     (10000)

#define DEFAULT_ANALOG_GAIN_X256_1                                (512)
#define DEFAULT_ANALOG_GAIN_X256_2                                (512)
#define DEFAULT_ANALOG_GAIN_X256_3                                (512)
#define DEFAULT_ANALOG_GAIN_X256_4                                (512)

#define DEFAULT_HDR_FRAME_COUNT                                   (3)
#define DEFAULT_HDR_FPS_CONFIG_COUNT                              (1)
#define DEFAULT_CONTROL_COIN_HDR                                  (Coin_e_Heads)

#define DEFAULT_DUMMY_VALUE                                       (Flag_e_FALSE)
#define DEFAULT_FLOAT_DUMMY_VALUE                                 (0.0)
#define DEFAULT_FRAME_COUNTER                                     (0)
#define DEFAULT_STATUS_COIN_HDR                                   (Coin_e_Heads)
#define DEFAULT_FRAME_RECEIVED                                    (0)

extern HDR_Control_ts                                             g_HDR_Control;
extern HDR_GainControl_ts                                         g_HDR_GainControl;
extern volatile HDR_Status_ts                                     g_HDR_Status;
extern HDR_StatusVariable_ts                                      g_HDR_StatusVariable;

extern void HDR_RequestExposure(void)TO_EXT_DDR_PRGM_MEM;
extern Flag_te HDR_IsAEC_Absorbed(void)TO_EXT_DDR_PRGM_MEM;
extern void HDR_CheckDMAValidFrame(void)TO_EXT_DDR_PRGM_MEM;
#endif // _HDR_H_


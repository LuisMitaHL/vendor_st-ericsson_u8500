/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\file channel_gains_op_interface.h
\brief channel gain or digital gain calculation
\ingroup WBStats
*/
#ifndef _CHANNEL_GAINS_OP_INTERFACE_H_
#   define _CHANNEL_GAINS_OP_INTERFACE_H_

#   include "Platform.h"
#   include "PictorhwReg.h"
#   include "GenericFunctions.h"
#   include "awb_statistics_op_interface.h"
#   include "whitebalance_opinterface.h"
#   include "RSO_OPInterface.h"
#   include "Exposure_OPInterface.h"

/**
 \struct
 \brief
 \ingroup
*/
typedef struct
{
    /// Red channel gain
    float_t f_RedGain;

    /// Green in Red channel gain
    float_t f_GreenInRedGain;

    /// Green in Blue channel gain
    float_t f_GreenInBlueGain;

    /// Blue channel gain
    float_t f_BlueGain;
} ChannelGains_ts;

// Page containing control for Channel Gain module. It can be disabled through this Flag.
typedef struct
{
    uint8_t e_Flag_EnableChannelGains;
} ChannelGains_Control_ts;

/// Page specifying the status of the OffsetCompensation
typedef struct
{
    /// The offset compensation gain calculated for GR channel
    float_t     f_OffsetCompensationGain_GR;

    /// The offset compensation gain calculated for R channel
    float_t     f_OffsetCompensationGain_RR;

    /// The offset compensation gain calculated for B channel
    float_t     f_OffsetCompensationGain_BB;

    /// The offset compensation gain calculated for GB channel
    float_t     f_OffsetCompensationGain_GB;

    /// Total offset being applied at the channel offsets for GR channel
    uint16_t    u16_Offset_GR;

    /// Total offset being applied at the channel offsets for R channel
    uint16_t    u16_Offset_RR;

    /// Total offset being applied at the channel offsets for B channel
    uint16_t    u16_Offset_BB;

    /// Total offset being applied at the channel offsets for GB channel
    uint16_t    u16_Offset_GB;
} OffsetCompensationStatus_ts;

/// Page containing the various parameters which are sensor specific and are used while
/// calculating the Digital Gain to be applied in the Pixel Pipe.
typedef struct
{
    /// TILT FOR THE RED CHANNEL
    float_t     f_RedTiltGain;

    /// TILT FOR THE GREEN CHANNEL
    float_t     f_GreenTiltGain;

    /// TILT FOR THE BLUE CHANNEL
    float_t     f_BlueTiltGain;

    /// GuaranteedDataSaturationLevel is the measured pixel saturation value of a certain sensor type.
    uint16_t    u16_GuaranteedDataSaturationLevel;

    /// Minimum value of the pixel that is possible through the sensor Rx interface
    uint16_t    u16_MinimumSensorRxPixelValue;

    /// Maximum value of the pixel that is possible through the sensor Rx interface
    uint16_t    u16_MaximumSensorRxPixelValue;

    /// Black Correction Offset which would be added to the sensor pedestal to get the RE
    /// Offset. This is to improve the black level.
    uint8_t     u8_BlackCorrectionOffset;
} SensorSetup_ts;

extern ChannelGains_Control_ts      g_ChannelGains_Control;
extern OffsetCompensationStatus_ts  g_OffsetCompensationStatus;
extern SensorSetup_ts               g_SensorSetup;
extern ChannelGains_ts              g_ChannelGains_combined;

extern void                         ChannelGain_CalculateAllGains (void);
extern void                         ChannelGain_ApplyAllGains (void);
extern void                         ChannelGain_Configure (void);
#endif //_CHANNEL_GAINS_OP_INTERFACE_H_


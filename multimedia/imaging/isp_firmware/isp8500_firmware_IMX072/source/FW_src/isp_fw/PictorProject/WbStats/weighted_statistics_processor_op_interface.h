/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file weighted_statistics_processor_op_interface.h
 \brief defines input depencies of auto matic white balance statistics block using weighted statistics processor.
 \ingroup awb
*/
#ifndef _WEIGHTED_STATISTICS_PROCESSOR_OP_INTERFACE_H_
#   define _WEIGHTED_STATISTICS_PROCESSOR_OP_INTERFACE_H_

#   include "Platform.h"
#   include "PictorhwReg.h"

#   include "awb_statistics_op_interface.h"
#   include "Zoom_OPInterface.h"
#   include "FrameDimension_op_interface.h"
#   include "SystemConfig.h"

#if WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR

/**
 \enum StatisticsSource_te
 \brief Statistics source for Weighted White balance.
 \ingroup awb
*/
typedef enum
{
    /// Pre remove slant offset channel gains statistics are accumulated
    StatisticsSource_e_PreChannelPreRemoveSlantOffset,

    /// Post grid iron and Pre channel gains statistics are accumulated
    StatisticsSource_e_PreChannelGainsandPostGridiron,

    /// Post channel gains statistics are accumulated
    StatisticsSource_e_PostChannelGains
} StatisticsSource_te;

/**
 \enum WeightedStatisticsMode_te
 \brief AutoMatic or manual weighted statistics accumulation.
 \ingroup awb
*/
typedef enum
{
    /// Accumulator setting programmed by user
    WeightedStatisticsMode_e_Manual,

    /// Accumulator setting calculated by F/W from current window of interest
    WeightedStatisticsMode_e_Auto
} WeightedStatisticsMode_te;

/**
 \struct WeightedStatistics_Controls_ts
 \brief  Page containing the controls for statistics gathering for White Balance
 \ingroup AWB
*/
typedef struct
{
    /// Zone gain for each zone \n
    ///    [31:0] gain_zone[15:0]: gain of corresponding zone \n
    ///    00: (GAIN_0X) No accumulation \n
    ///    01: (GAIN_1X) Gain of 1 \n
    ///    10: (GAIN_2X) Gain of 2 \n
    ///    11: (GAIN_4X) Gain of 4 \n
    ///  [NOTE]: Only valid in manual mode \n
    uint32_t    u32_Gain_Zone15_Zone0;

    /// Low threshold for Weighted Statistics Processor accumulator
    uint16_t    u16_LowThreshold;

    /// High threshold for Weighted Statistics Processor accumulator
    uint16_t    u16_HighThreshold;

    /// WOI offset X \n
    ///  [NOTE]: Only valid in manual mode \n
    uint16_t    u16_WOIOffsetX;

    /// WOI offset Y \n
    ///  [NOTE]: Only valid in manual mode \n
    uint16_t    u16_WOIOffsetY;

    /// X WOI size \n
    ///  [NOTE]: Only valid in manual mode \n
    uint16_t    u16_WOISizeX;

    /// Y WOI size \n
    ///  [NOTE]: Only valid in manual mode \n
    uint16_t    u16_WOISizeY;

    /// Sum of gains of all the zones
    uint8_t     u8_SumOfZoneGains;

    /// Statistics mode \n
    /// [DEFAULT]: e_Auto_Weighted_Statistics
    uint8_t     e_WeightedStatisticsMode_Type;

    /// Type of statistics accumulated for weighted white balance.\n
    /// [DEFAULT]: e_PostChannelGains
    uint8_t     e_StatisticsSource_AccWg;

    /// Programme e_Coin_Command != e_Coin_Status, After command successful, e_Coin_Status will become equal to e_Coin_Command \n
    /// [NOTE]: Above values are only absorbed when command and status coin are different and mode selected is manual
    uint8_t     e_Coin_Command;
} WeightedStatistics_Controls_ts;

/**
\struct  AccWgZones_Statistics_ts
\brief   Statistics calculated for WhiteBalance
\ingroup   WBStats
*/
typedef struct
{
    /// Red Channel Energy
    float_t     f_RedEnergy;

    /// Green Channel Energy
    float_t     f_GreenEnergy;

    /// Blue Channel Energy
    float_t     f_BlueEnergy;

    /// Zone gain for each zone \n
    ///    [31:0] gain_zone[15:0]: gain of corresponding zone \n
    ///    00: (GAIN_0X) No accumulation \n
    ///    01: (GAIN_1X) Gain of 1 \n
    ///    10: (GAIN_2X) Gain of 2 \n
    ///    11: (GAIN_4X) Gain of 4 \n
    uint32_t    u32_Gain_Zone15_Zone0;

    /// WOI offset X
    uint16_t    u16_WOIOffsetX;

    /// WOI offset Y
    uint16_t    u16_WOIOffsetY;

    /// X WOI size
    uint16_t    u16_WOISizeX;

    /// Y WOI size
    uint16_t    u16_WOISizeY;

    /// Sum of gains of all the zones
    uint8_t     u8_SumOfZoneGains;

    /// Statistics Valid or not
    uint8_t     e_Result_Valid;

    /// Type of statistics accumulated for weighted white balance, Status
    uint8_t     e_StatisticsSource_AccWg;

    /// When e_Coin_Command != e_Coin_Status, an attempt is made to perform the required operation
    /// When command is completed, e_Coin_Status is set to e_Coin_Command \n
    uint8_t     e_Coin_Status;
} AccWgZones_Statistics_ts;

/* TODO: <AG>: To be changed to point to zoom */
/*
/// Current X and Y size from Sensor
#   define AWB_GetWOI_X_Size() (400)
#   define AWB_GetWOI_Y_Size() (300)

/// Window of interest starting point
#   define AWB_GetWOI_X_Start()    (15)
#   define AWB_GetWOI_Y_Start()    (15)
*/
/// Current X and Y size from Sensor
#   define AWB_GetWOI_X_Size()    (Zoom_GetCurrentFOVX()/FrameDimension_GetCurrentPreScaleFactor())
#   define AWB_GetWOI_Y_Size()    (g_Zoom_Status.f_FOVY/FrameDimension_GetCurrentPreScaleFactor())

/// Window of interest starting point
#   define AWB_GetWOI_X_Start()   (SystemConfig_IsPipe0Active()?Get_ISP_CE0_CROP_ISP_CE0_CROP_H_START():Get_ISP_CE1_CROP_ISP_CE1_CROP_H_START())
#   define AWB_GetWOI_Y_Start()   (SystemConfig_IsPipe0Active()?Get_ISP_CE0_CROP_ISP_CE0_CROP_V_START():Get_ISP_CE1_CROP_ISP_CE1_CROP_V_START())

/// Applying unity gains on every accumulator
/// 0101 0101    0101 0101    0101 0101    0101 0101 :
/// 0x55555555
#   define WEIGHTED_STATISTICS_ACCUMULATOR_UNITY_GAINS (0xffffffff)

// Lowest and hieghest threshold of pixel values
#   define WEIGHTED_STATISTICS_ACCWZ_ACC_THRESH_LO     (0x20)
#   define WEIGHTED_STATISTICS_ACCWZ_ACC_THRESH_HI     (0xFF)
#   define WEIGHTED_STATISTICS_DEFAULT_WOI_OFFSET_X    (0)
#   define WEIGHTED_STATISTICS_DEFAULT_WOI_OFFSET_Y    (0)
#   define WEIGHTED_STATISTICS_DEFAULT_WOI_SIZE_X      (640)
#   define WEIGHTED_STATISTICS_DEFAULT_WOI_SIZE_Y      (480)

/// Control page element for Weighted Statistics Processor
/// The elements control accumulator Hign and Low threshold
extern WeightedStatistics_Controls_ts   g_WeightedStatisticsControl;

/// Energy for weighted white balance
extern AccWgZones_Statistics_ts         g_WeightedEnergyStatus;

#endif	// WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR

extern void                             Weighted_Statistics_Processor_ExtractMeanStatistics (void);

extern void                             Weighted_Statistics_Processor_Setup_ISR (void);
#endif //_WEIGHTED_STATISTICS_PROCESSOR_OP_INTERFACE_H_


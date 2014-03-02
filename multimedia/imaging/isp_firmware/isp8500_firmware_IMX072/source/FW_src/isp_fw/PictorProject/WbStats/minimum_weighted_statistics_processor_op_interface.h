/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file minimum_weighted_statistics_processor_op_interface.h
 \brief defines input depencies of auto matic white balance statistics block using weighted statistics processor.
 \ingroup awb
*/
#ifndef _MINIMUM_WEIGHTED_STATISTICS_PROCESSOR_OP_INTERFACE_H_
#   define _MINIMUM_WEIGHTED_STATISTICS_PROCESSOR_OP_INTERFACE_H_

#   include "Platform.h"
#   include "PictorhwReg.h"

#   include "awb_statistics_op_interface.h"
#	include "minimum_weighted_statistics_processor_ip_interface.h"
#	include "minimum_weighted_statistics_platform_specific.h"
#   include "GenericFunctions.h"

#if MWWB_INCLUDE_MWWB

/**
 \enum Channel_te
 \brief Channel selection for minimum weighted white balance.
 \ingroup awb
*/
typedef enum
{
    Channel_Green1,
    Channel_Green2,
    Channel_Green1Green2_Mean
} Channel_te;

/**
 \enum MWWB_StatisticsSource_te
 \brief Statistics source for Weighted White balance.
 \ingroup awb
*/
typedef enum
{
    /// Post grid iron and Pre channel gains statistics are accumulated
    MWWB_StatisticsSource_e_MWWB_PreChannelGainsandPostGridiron = 1,

    /// Post channel gains statistics are accumulated
    MWWB_StatisticsSource_e_MWWB_PostChannelGains               = 2
} MWWB_StatisticsSource_te;

/**
 \struct MWWB_Controls_ts
 \brief  Page containing the controls for statistics gathering for White Balance
 \ingroup AWB
*/
typedef struct
{
    /// If any Channel energy(RGB) is greater than this
    /// threshhold value that pixel will be dropped.
    uint16_t    u16_SaturationThreshold;

    /// Statistics source
    uint8_t     e_MWWB_StatisticsSource;

    /// Specify which channel (either green1 or green2 or
    /// average of green1 and green2) to consider while
    /// [DEFAULT]: e_PostChannelGains
    uint8_t     e_Channel_Accumulate;
} MWWB_Controls_ts;

/**
 \struct WeightedStatistics_Controls_ts
 \brief  Page containing the controls for statistics gathering for White Balance
 \ingroup AWB
*/
typedef struct
{
    /// Red Channel Energy
    float_t     f_RedEnergy;

    /// Green Channel Energy
    float_t     f_GreenEnergy;

    /// Blue Channel Energy
    float_t     f_BlueEnergy;

    /// Total number of macro pixel which are accumulated
    uint32_t    u32_NumberMacroPixel;

    /// Zone X (horizontal) offset
    uint16_t    u16_Zone_X_Offset;

    /// Zone Y (vertical) offset
    uint16_t    u16_Zone_Y_Offset;

    /// Zone X size
    uint16_t    u16_Zone_X_Size;

    /// Zone Y size
    uint16_t    u16_Zone_Y_Size;

    /// Statistics Valid or not
    uint8_t     e_Result_Valid;

    /// Statistics source status
    uint8_t     e_MWWB_StatisticsSource;
} MWWB_Status_ts;


/// Control page element for Weighted Statistics Processor
/// The elements control accumulator Hign and Low threshold
extern MWWB_Controls_ts g_MWWB_Statistics_control;

extern MWWB_Status_ts   g_MWWB_Statistics_Status;

#endif	// #if MWWB_INCLUDE_MWWB

extern void             Minimum_Weighted_Statistics_Processor_Setup_ISR (void);

extern void             Minimum_Weighted_Statistics_Processor_ExtractMeanStatistics (void);
#endif //_MINIMUM_WEIGHTED_STATISTICS_PROCESSOR_OP_INTERFACE_H_


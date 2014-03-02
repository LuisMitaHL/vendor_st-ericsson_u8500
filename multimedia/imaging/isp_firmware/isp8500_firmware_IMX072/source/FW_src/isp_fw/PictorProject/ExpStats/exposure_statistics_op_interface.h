/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
\defgroup ExpStats_8x6 Exposure Statistics Module
\brief This Module Programs the ExpStats_8x6 block and extract statistics
*/

/**
\file exposure_statistics_op_interface.h
\brief Provides an interface to the accumulator block. Provides functions to calculate normalised statistics.
       Used by the Exposure Control block.
\ingroup ExpStats_8x6
*/
#ifndef _EXPOSURE_STATISTICS_OP_INTERFACE_H_
#   define _EXPOSURE_STATISTICS_OP_INTERFACE_H_


#   include "exposure_statistics_ip_interface.h"
#   include "exposure_statistics_PlatformSpecific.h"


/**
 \enum ExpStatisticsMode_te
 \brief AutoMatic or manual exposure statistics accumulation.
 \ingroup ExpStats_8x6
*/
typedef enum
{
    /// Accumulator setting programmed by user
	ExpStatisticsMode_e_Manual,

    /// Accumulator setting calculated by F/W from current window of interest
	ExpStatisticsMode_e_Auto
} ExpStatisticsMode_te;

/**
 \enum AccMode_te
 \brief Mode in which Accumulator functions.
 \ingroup ExpStats_8x6
*/
typedef enum
{
	/// Real pixel value is accumulated if greater than low threshold
	AccMode_e_FULL_PIXELVAL,

	/// Pixel value is clipped to high threshold and accumulated if greater than low threshold
	AccMode_e_CLIP_PIXELVAL,

	/// Histogram
	AccMode_e_HISTOGRAM,

	/// No accumulation
	AccMode_e_ZERO
} AccMode_te;

/**
 \enum AccType_te
 \brief Selection of kind of statistics.
 \ingroup ExpStats_8x6
*/
typedef enum
{
	/// zone_gains is used according to pixel location in zones
	AccType_e_ACC_ZONED,

	/// no gain applied
	AccType_e_ACC_FLAT,

	/// only black lines are accumulated
	AccType_e_ACC_BLACK,

	/// only dark lines are accumulated
	AccType_e_ACC_DARK
}AccType_te;


/**
 \enum AccColor_te
 \brief Selection of pixel to accumulate
 \ingroup ExpStats_8x6
*/
typedef enum
{
	/// Green In Red
	AccColor_e_GIR,

	/// Red
	AccColor_e_RED,

	/// Blue
	AccColor_e_BLU,

	/// Green in Blue
	AccColor_e_GIB
}AccColor_te;


/**
 \enum AccSrc_te
 \brief Selection of the input IDP stream for gathering
 \ingroup ExpStats_8x6
*/
typedef enum
{
	/// IDP Stream 0
	AccSrc_e_SRC_IDP_0,

	/// IDP Stream 1
	AccSrc_e_SRC_IDP_1,

	/// IDP Stream 2
	AccSrc_e_SRC_IDP_2,

	/// IDP Stream 3
	AccSrc_e_SRC_IDP_3
}AccSrc_te;

#if EXPOSURE_STATS_PROCESSOR_6x8_ZONES

/**
\struct  ExpStats_8x6_Status_ts
\brief   Contains the status interface for the hw block
\ingroup ExpStats_8x6
*/
typedef struct
{
    ///1-16 Zone Gains
    uint32_t    u32_ZoneGains_1_16;

    //17-32 Zone Gains
    uint32_t    u32_ZoneGains_17_32;

    ///33-48 Zone Gains
    uint32_t    u32_ZoneGains_33_48;

    /// normalized energy
    float_t f_MeanEnergy;

	/// Zone Offset X : X offset for accumulation
	uint16_t u16_ZoneOffset_X;

	/// Zone Offset Y : Y offset for accumulation
	uint16_t u16_ZoneOffset_Y;

	/// Zone Size X : X size of zones for accumulation
	uint16_t u16_ZoneSize_X;

	/// Zone Size Y : Y size of zones for accumulation
	uint16_t u16_ZoneSize_Y;

	/// Acc low Threshold : Lower value of threshold
	uint16_t u16_AccThreshold_Lo;

	/// Acc High Threshold : Upper value of threshold
	uint16_t u16_AccThreshold_Hi;

	/// Exp Mode of Statistics Accumulation
	uint8_t e_ExpStatisticsMode;

	/// Accumulator Mode
	uint8_t e_AccMode;

	/// Accumulator Type
	uint8_t e_AccType;

    /// Colour for Accumulator
    uint8_t e_AccColor;

    /// Selection of the input IDP stream for gathering
    uint8_t e_AccSrc;

    /// Enable/Disable Staggered Zone structure
    uint8_t e_Flag_AccStaggered;

    /// sum of zone gains
    uint8_t u8_SumOfZoneGains;

    /// flag indicating whether stats are valid are not.
    /// Stats are valid when ISP is streaming
    uint8_t e_Flag_StatsValid;

    /// interrupt count of exposure
    uint8_t u8_ExpStatsInterruptCount;
} ExpStats_8x6_Status_ts;


/**
\struct  ExpStats_8x6_Ctrl_ts
\brief   Contains the contorl interface for the hw block
\ingroup ExpStats_8x6
*/
typedef struct
{
	//ExpStats_8x6_Status_ts *ptrExpStats_8x6_Status;
    ///1-16 Zone Gains
    uint32_t    u32_ZoneGains_1_16;

    //17-32 Zone Gains
    uint32_t    u32_ZoneGains_17_32;

    ///33-48 Zone Gains
    uint32_t    u32_ZoneGains_33_48;

	/// Zone Offset X : X offset for accumulation
	uint16_t u16_ZoneOffset_X;

	/// Zone Offset Y : Y offset for accumulation
	uint16_t u16_ZoneOffset_Y;

	/// Zone Size X : X size of zones for accumulation
	uint16_t u16_ZoneSize_X;

	/// Zone Size Y : Y size of zones for accumulation
	uint16_t u16_ZoneSize_Y;

	/// Acc low Threshold : Lower value of threshold
	uint16_t u16_AccThreshold_Lo;

	/// Acc High Threshold : Upper value of threshold
	uint16_t u16_AccThreshold_Hi;

	/// Exp Mode of Statistics Accumulation
	uint8_t e_ExpStatisticsMode;

	/// Accumulator Mode
	uint8_t e_AccMode;

	/// Accumulator Type
	uint8_t e_AccType;

    /// Colour for Accumulator
    uint8_t e_AccColor;

    /// Selection of the input IDP stream for gathering
    uint8_t e_AccSrc;

    /// Enable/Disable Staggered Zone structure
    uint8_t e_Flag_AccStaggered;

    /// Enable the ExpStats_8x6 Block
    uint8_t e_Flag_EnableExpStats;
} ExpStats_8x6_Ctrl_ts;

/// Page containing the exposure statistics
extern ExpStats_8x6_Status_ts       g_ExpStats_8x6_Status;
extern ExpStats_8x6_Ctrl_ts         g_ExpStats_8x6_Ctrl;

// Function which programmes the Statistics Engine Block to calculte the exposure statistics
extern void                     ExpStats_8x6_Setup_ISR (void);

//extern void exposure_SetupStatusLine_ISR(void);
// Function which normalises the exposure statistics using the Pixel Count, Gains
// applied and Subsample measure.
extern void                     ExpStats_8x6_ExtractMeanStatistics (void);
extern void                     ExpStats_8x6_Statistics_ISR (void);

#define ExpStats_GetMeanEnergy()    (g_ExpStats_8x6_Status.f_MeanEnergy)
#define ExpStats_AreStatsValid()    (g_ExpStats_8x6_Status.e_Flag_StatsValid)

#endif              // EXPOSURE_STATS_PROCESSOR_6x8_ZONES

#endif // _EXPOSURE_STATISTICS_OP_INTERFACE_H_


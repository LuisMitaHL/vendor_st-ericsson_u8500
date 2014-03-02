/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file weighted_statistics_processor_ip_interface.c
 \brief
 \ingroup awb
*/
#include "weighted_statistics_processor_op_interface.h"
#include "exposure_statistics_op_interface.h"

#if WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR

/// Page containing the White Balance statistics control
WeightedStatistics_Controls_ts  g_WeightedStatisticsControl =
{
    WEIGHTED_STATISTICS_ACCUMULATOR_UNITY_GAINS,
    WEIGHTED_STATISTICS_ACCWZ_ACC_THRESH_LO,
    WEIGHTED_STATISTICS_ACCWZ_ACC_THRESH_HI,
    WEIGHTED_STATISTICS_DEFAULT_WOI_OFFSET_X,
    WEIGHTED_STATISTICS_DEFAULT_WOI_OFFSET_Y,
    WEIGHTED_STATISTICS_DEFAULT_WOI_SIZE_X,
    WEIGHTED_STATISTICS_DEFAULT_WOI_SIZE_Y,
    64, // sum of zone gains
    WeightedStatisticsMode_e_Auto,
    StatisticsSource_e_PostChannelGains,
    Coin_e_Heads
};

AccWgZones_Statistics_ts        g_WeightedEnergyStatus =
{
    0.0,
    0.0,
    0.0,
    WEIGHTED_STATISTICS_ACCUMULATOR_UNITY_GAINS,
    WEIGHTED_STATISTICS_DEFAULT_WOI_OFFSET_X,
    WEIGHTED_STATISTICS_DEFAULT_WOI_OFFSET_Y,
    WEIGHTED_STATISTICS_DEFAULT_WOI_SIZE_X,
    WEIGHTED_STATISTICS_DEFAULT_WOI_SIZE_Y,
    64,
    Result_e_Failure,
    Coin_e_Heads
};

#endif	// #if WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR

/**
 \fn       void Weighted_Statistics_Processor_Setup_ISR(void)
 \brief    Set up the 4*4 weighted Statistics processor for automatic grey world white balance statistics
 \details   Called in the ISR for Stats Engine when the Stats Ready interrupt occurs
 \return    void
 \callgraph
 \callergraph
 \ingroup WBStats
*/
void
Weighted_Statistics_Processor_Setup_ISR(void)
{
#if WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // X and Y size
    uint16_t    u16_ZoneSizeX;
    uint16_t    u16_ZoneSizeY;

    // X and Y Zone offset in image
    uint16_t    u16_ZoneOffsetX;
    uint16_t    u16_ZoneOffsetY;

    // Low and High threshold for clipping
    uint16_t    u16_LowThreshold;
    uint16_t    u16_HighThreshold;
    uint8_t     u8_StatisticsSource;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // Enable statistics block
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ENABLE_word(0x01);

    if (StatisticsSource_e_PostChannelGains == g_WeightedEnergyStatus.e_StatisticsSource_AccWg)
    {
        u8_StatisticsSource = src_SRC_IDP_2;
    }
    else if (StatisticsSource_e_PreChannelGainsandPostGridiron == g_WeightedEnergyStatus.e_StatisticsSource_AccWg)
    {
        u8_StatisticsSource = src_SRC_IDP_1;
    }
    else
    {
        u8_StatisticsSource = src_SRC_IDP_0;
    }


    u16_LowThreshold = g_WeightedStatisticsControl.u16_LowThreshold;
    u16_HighThreshold = g_WeightedStatisticsControl.u16_HighThreshold;

    /**
     pictor_fs[1]_09.pdf: P80, Statistics processor

     1. Statistics block is divided into 16 zones of 4x4 grouping.
        u16_ZoneSizeX and u16_ZoneSizeY are zone size will be 1/4th of window of interest size.
        Zone size = WOI_Size / 4 or right shift 2.

     2. Accumulator pic every 4th pixel, so size should be multiple of 4 in order to avoid boundary cases.
        Zone size = Zone size & 0xFFFC
    */
    if (WeightedStatisticsMode_e_Manual == g_WeightedStatisticsControl.e_WeightedStatisticsMode_Type)
    {
        if (g_WeightedEnergyStatus.e_Coin_Status != g_WeightedStatisticsControl.e_Coin_Command)
        {
            g_WeightedEnergyStatus.u16_WOIOffsetX = g_WeightedStatisticsControl.u16_WOIOffsetX;
            g_WeightedEnergyStatus.u16_WOIOffsetY = g_WeightedStatisticsControl.u16_WOIOffsetY;
            g_WeightedEnergyStatus.u16_WOISizeX = g_WeightedStatisticsControl.u16_WOISizeX;
            g_WeightedEnergyStatus.u16_WOISizeY = g_WeightedStatisticsControl.u16_WOISizeY;
            g_WeightedEnergyStatus.u32_Gain_Zone15_Zone0 = g_WeightedStatisticsControl.u32_Gain_Zone15_Zone0;
            g_WeightedEnergyStatus.u8_SumOfZoneGains = g_WeightedStatisticsControl.u8_SumOfZoneGains;
        }


        u16_ZoneSizeX = g_WeightedEnergyStatus.u16_WOISizeX;
        u16_ZoneSizeY = g_WeightedEnergyStatus.u16_WOISizeY;
        u16_ZoneOffsetX = g_WeightedEnergyStatus.u16_WOIOffsetX;
        u16_ZoneOffsetY = g_WeightedEnergyStatus.u16_WOIOffsetY;
    }
    else
    {
        u16_ZoneSizeX = g_WeightedEnergyStatus.u16_WOISizeX = (AWB_GetWOI_X_Size() / 4);//(AWB_GetWOI_X_Size() >> 2) & 0xFFFC;    // we want the zone sizes to be a multiple of 4...
        u16_ZoneSizeY = g_WeightedEnergyStatus.u16_WOISizeY = (AWB_GetWOI_Y_Size() / 4);//(AWB_GetWOI_Y_Size() >> 2) & 0xFFFC;    // we want the zone sizes to be a multiple of 4...
        u16_ZoneOffsetX = g_WeightedEnergyStatus.u16_WOIOffsetX = AWB_GetWOI_X_Start();
        u16_ZoneOffsetY = g_WeightedEnergyStatus.u16_WOIOffsetY = AWB_GetWOI_Y_Start();
        g_WeightedEnergyStatus.u8_SumOfZoneGains = 64;

        g_WeightedEnergyStatus.u32_Gain_Zone15_Zone0 = WEIGHTED_STATISTICS_ACCUMULATOR_UNITY_GAINS;
    }


    /*********************************
        ACCUMULATOR 0
        Green in Red Statistics
    *********************************/
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC0_ZONE_X_OFFSET(u16_ZoneOffsetX);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC0_ZONE_X_SIZE(u16_ZoneSizeX);

    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC0_ZONE_Y_OFFSET(u16_ZoneOffsetY);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC0_ZONE_Y_SIZE(u16_ZoneSizeY);

    // Set Thresholds
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC0_THRESH_LO_word(u16_LowThreshold);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC0_THRESH_HI_word(u16_HighThreshold);

    // Clip pixel value
    // Flat accumulation
    // Green in Red statistics
    // Post channel gains
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC0_CTRL(mode_CLIP_PIXELVAL, type_ACC_ZONED, color_GIR, u8_StatisticsSource);

    /*********************************
        ACCUMULATOR 1
        Red Statistics
    *********************************/
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1_ZONE_X_OFFSET(u16_ZoneOffsetX);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1_ZONE_X_SIZE(u16_ZoneSizeX);

    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1_ZONE_Y_OFFSET(u16_ZoneOffsetY);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1_ZONE_Y_SIZE(u16_ZoneSizeY);

    // Set Thresholds
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1_THRESH_LO_word(u16_LowThreshold);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1_THRESH_HI_word(u16_HighThreshold);

    // Clip pixel value
    // Flat accumulation
    // Red statistics
    // Post channel gains
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1_CTRL(mode_CLIP_PIXELVAL, type_ACC_ZONED, color_RED, u8_StatisticsSource);

    /*********************************
        ACCUMULATOR 2
        Blue Statistics
    *********************************/
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC2_ZONE_X_OFFSET(u16_ZoneOffsetX);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC2_ZONE_X_SIZE(u16_ZoneSizeX);

    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC2_ZONE_Y_OFFSET(u16_ZoneOffsetY);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC2_ZONE_Y_SIZE(u16_ZoneSizeY);

    // Set Thresholds
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC2_THRESH_LO_word(u16_LowThreshold);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC2_THRESH_HI_word(u16_HighThreshold);

    // Clip pixel value
    // Flat accumulation
    // Red statistics
    // Post channel gains
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC2_CTRL(mode_CLIP_PIXELVAL, type_ACC_ZONED, color_BLU, u8_StatisticsSource);

    /*********************************
        ACCUMULATOR 3
        Green in Blue Statistics
    *********************************/

    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC3_ZONE_X_OFFSET(u16_ZoneOffsetX);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC3_ZONE_X_SIZE(u16_ZoneSizeX);

    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC3_ZONE_Y_OFFSET(u16_ZoneOffsetY);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC3_ZONE_Y_SIZE(u16_ZoneSizeY);

    // Set Thresholds
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC3_THRESH_LO_word(u16_LowThreshold);
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC3_THRESH_HI_word(u16_HighThreshold);

    // Clip pixel value
    // Flat accumulation
    // Red statistics
    // Post channel gains
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC3_CTRL(mode_CLIP_PIXELVAL, type_ACC_ZONED, color_GIB, u8_StatisticsSource);

    // To be removed, Gain should not be part if flat is used
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ZONE_GAINS_word(g_WeightedEnergyStatus.u32_Gain_Zone15_Zone0);

#else

    // Disable the statistics block if the firmware has not been included
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ENABLE_word(0x00);

#endif	// WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR
    return;
}


/**
\fn void Weighted_Statistics_Processor_ExtractMeanStatistics(void)
\brief     Calculates mean statistics for grey white balance using w
\ingroup   WBStats
\callgraph
\callergraph
*/
void
Weighted_Statistics_Processor_ExtractMeanStatistics(void)
{
#if WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // No of accumulated pixels
    uint32_t    u32_NoOfPixelPerZone = 0;

    // Total gain applied in 16 zones
    uint16_t    u16_SumOfZoneGains = g_WeightedEnergyStatus.u8_SumOfZoneGains;
    uint8_t     u8_NoOfZones = 16;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (Stream_IsISPInputStreaming())
    {
        // Check for any error from zones
        // pictor_fs[1]_09.pdf: P370, ISP_STATS_ACCWZ_ZONE_STATUS
        if (0x00 == (Get_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ZONE_STATUS() & 0x0F))
        {
            // No of pixel for each zone is constant as same size is programmed in all the 4 zones.
            u32_NoOfPixelPerZone = Get_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1_ZONE_X_SIZE() * Get_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1_ZONE_Y_SIZE();

            /*~~~~~~~~~~~~~~~~~~~~~~
                  ACCUMULATOR 1
                  Red Statistics
            ~~~~~~~~~~~~~~~~~~~~~~*/

            g_WeightedEnergyStatus.f_RedEnergy = CalculateNormalisedStatistics(
                Get_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1(),
                u32_NoOfPixelPerZone,
                u16_SumOfZoneGains,
                u8_NoOfZones);

            /*~~~~~~~~~~~~~~~~~~~~~~
                 ACCUMULATOR 0 and 3
                 Green Statistics
            ~~~~~~~~~~~~~~~~~~~~~~*/
            // For Green energy we take the average of Green in Red and Green in Blue
            // GIB and GIR are present in accs 0 and 3
            g_WeightedEnergyStatus.f_GreenEnergy =
                (
                    CalculateNormalisedStatistics(Get_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC0(), u32_NoOfPixelPerZone, u16_SumOfZoneGains,u8_NoOfZones) +
                    CalculateNormalisedStatistics(Get_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC3(), u32_NoOfPixelPerZone, u16_SumOfZoneGains,u8_NoOfZones)
                ) /
                (2.0);

            /*~~~~~~~~~~~~~~~~~~~~~~
                 ACCUMULATOR 2
                 Blue Statistics
            ~~~~~~~~~~~~~~~~~~~~~~*/
            g_WeightedEnergyStatus.f_BlueEnergy = CalculateNormalisedStatistics(
                Get_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC2(),
                u32_NoOfPixelPerZone,
                u16_SumOfZoneGains,
                u8_NoOfZones);

            g_WeightedEnergyStatus.e_Result_Valid = Result_e_Success;
        }
        else
        {
            g_WeightedEnergyStatus.e_Result_Valid = Result_e_Failure;
        }
    }
    else
	{
		g_WeightedEnergyStatus.e_Result_Valid = Result_e_Failure;
	}

#endif	// WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR

    return;
}


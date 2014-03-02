/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\file    stats_engine.c
\brief   The file contain function needed for AWB and exposure statistics calculations.
\ingroup Stats
 */
#include "awb_statistics_op_interface.h"

/// Sub sampling factor applied by accumulator in X direction
#define SAMPLING_FACTOR_IN_PIXELS_X (4)

/// Sub sampling factor applied by accumulator in Y direction
#define SAMPLING_FACTOR_IN_PIXELS_Y (4)

/// Pixel pipe width
#define PIXEL_SIZE_BITS (8)

/// No of bits accumulated by accumulator
#define PIXEL_SIZE_BITS_ACC (6)

#if WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR

/**
 \fn      float_t CalculateNormalisedStatistics(uint32_t u32_Acc, uint32_t u32_NoOfPixelPerZone, uint16_t    u16_SumOfZoneGains, uint8_t     u8_NoOfZones)
 \brief   Calculates avaregae pixel energy
 \details Calculates avererage pixel energy
 \param   u32_Acc: Accumulated energy
 \param   u32_NoOfPixelPerZone: Total no of pixel per zone
 \param   u16_SumOfZoneGains: Zone gain
 \param   u8_NoOfZones: No of active zones
 \return  f_AvgPixelEnergy The normalised statistics
 \callgraph
 \callergraph
*/
float_t
CalculateNormalisedStatistics(
uint32_t    u32_Acc,
uint32_t    u32_NoOfPixelPerZone,
uint16_t    u16_SumOfZoneGains,
uint8_t     u8_NoOfZones)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    float_t f_AvgPixelEnergy;
    /*~~~~~~~~~~~~~~~~~~~~~*/
    /**
             Description of Statistics Calculation
             ======================================

             1) Convert accumulator op to real pixel values
             ----------------------------------------------
             Internally in the statistics hardware accumulator, lower 4 bits are skipped to save memory.
             So actual energy is 2^4 times less.

             Actual accumulated pixel energy  = Accumulated values reported by H/W * 2^4

             Total pixel energy = ACC * 2^4 -----------------------------------------------------------------(1)


             2) Determine total number of accumulated pixels per zone
             ---------------------------------------------------------
             Each accumulator accumulate every 4th pixel on every 4th line to save memory
             Therefore accumulated value should be multiplied by 4*4 = 16

             Total energy per zone = ACC* 2^4 * 16 ----------------------------------------------------------(2)


             3) Determine energy accumulated in WOI
              -------------------------------------
              Total energy in WOI = Energy per zone * Total no of zone accumulated

              Total energy in WOI = ACC* 2^4 * 16 * No of zones ---------------------------------------------(3)


             4) Total no of accumulated pixels in window of interest
             ---------------------------------------------------------
             Sum of zone gain are same as no of zones in case of flat acculmulation. In case of gained
             accumulation, it would same as more no of zones.

             Therefore total no of pixels in current WOI = no of accumulated pixels per zone * Sum of Zone Gain

             Total No of accumulated pixel in current WOI = (u32_NoOfPixelPerZone * u16_SumOfZoneGains ------(4)

             5) Calculate average pixel energy
             ---------------------------------
             Total pixel energy / total number of accumulated pixels

             Equation (3) / Equation(4)

                                                    (ACC * 2^4 * 16 * No of zones)
             Average energy per pixel = -----------------------------------------------
                                          (u32_NoOfPixelPerZone  * u8_SumOfZoneGains)

                                                  (u32_Acc * 16 * 16 * No of zones)
             Average energy per pixel = -------------------------------------------------
                                          (u32_NoOfPixelPerZone  * u8_SumOfZoneGains )
            */

    // Total Energy
    // Subsampling X * Y  = 16
    // 4 Bit truncation: 1 << (PIXEL_SIZE_BITS - PIXEL_SIZE_BITS_ACC)
    //    f_AvgPixelEnergy = ((float_t) u32_Acc * (1 << (PIXEL_SIZE_BITS - PIXEL_SIZE_BITS_ACC)) * (SAMPLING_FACTOR_IN_PIXELS_X * SAMPLING_FACTOR_IN_PIXELS_Y)* u8_NoOfZones);
    f_AvgPixelEnergy =
        (
            (float_t) u32_Acc *
            (1 << (PIXEL_SIZE_BITS - PIXEL_SIZE_BITS_ACC)) *
            (SAMPLING_FACTOR_IN_PIXELS_X * SAMPLING_FACTOR_IN_PIXELS_Y)
        );

    //  Average pixel energy
    f_AvgPixelEnergy = f_AvgPixelEnergy / (u32_NoOfPixelPerZone * u16_SumOfZoneGains);

    return (f_AvgPixelEnergy);
}

#endif  //  WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\file exposure_statistics.c
\brief
\ingroup ExpStats_8x6
*/
#include "exposure_statistics_op_interface.h"

#if EXPOSURE_STATS_PROCESSOR_6x8_ZONES
// Page containing the exposure statistics
ExpStats_8x6_Status_ts      g_ExpStats_8x6_Status =
{
	DEFAULT_ZONE_GAINS_1_16,
	DEFAULT_ZONE_GAINS_17_32,
	DEFAULT_ZONE_GAINS_33_48,
    DEFAULT_MEAN_ENERGY,
	DEFAULT_ZONE_OFFSET_X,
	DEFAULT_ZONE_OFFSET_Y,
	DEFAULT_ZONE_SIZE_X,
	DEFAULT_ZONE_SIZE_Y,
	DEFAULT_ACC_THRESHOLD_LO,
	DEFAULT_ACC_THRESHOLD_HI,
	DEFAULT_EXP_STATISICS_MODE,
	DEFAULT_ACC_MODE,
	DEFAULT_ACC_TYPE,
	DEFAULT_ACC_COLOR,
	DEFAULT_ACC_SRC,
	DEFAULT_ACC_STAGGERED,
    DEFAULT_SUM_OF_ZONES,
    DEFAULT_STATS_VALID,
    DEFAULT_INTERRUPT_COUNT
};

// Ctrl Page containing the exposure statistics
ExpStats_8x6_Ctrl_ts      g_ExpStats_8x6_Ctrl =
{
	DEFAULT_ZONE_GAINS_1_16,
	DEFAULT_ZONE_GAINS_17_32,
	DEFAULT_ZONE_GAINS_33_48,
	DEFAULT_ZONE_OFFSET_X,
	DEFAULT_ZONE_OFFSET_Y,
	DEFAULT_ZONE_SIZE_X,
	DEFAULT_ZONE_SIZE_Y,
	DEFAULT_ACC_THRESHOLD_LO,
	DEFAULT_ACC_THRESHOLD_HI,
	DEFAULT_EXP_STATISICS_MODE,
	DEFAULT_ACC_MODE,
	DEFAULT_ACC_TYPE,
	DEFAULT_ACC_COLOR,
	DEFAULT_ACC_SRC,
	DEFAULT_ACC_STAGGERED,
	DEFAULT_EXP_STATS_ENABLE
};

// internal functions
uint8_t 		SumOfZoneGains(void);
uint8_t 		SumOfZonesIn32BitRegister(uint32_t u32_ZoneGainReg);
float_t         CalculateNormalizedStatistics(uint32_t u32_Acc, uint32_t u32_NoOfPixelPerZone, uint16_t u16_SumOfZoneGains);

/**
 \fn       void ExpStats_8x6_Setup_ISR(void)
 \brief    Sets up Exp Stats block in context of Status ISR
 \param   void
 \return   void
 \callgraph
 \callergraph
 \ingroup ExpStats_8x6
*/
void
ExpStats_8x6_Setup_ISR(void)
{
/*
            stats block is divided into 48 zones in a 8x6 grouping(8 columns x 6 rows). acc_zone_x_size and
            acc_zone_y_size relate to the dimensions of one of these zones.

            stats x and y dimensions should be %4 otherwise boundry conditions will occur
            and the calculation to detemine average energy gets more complicated.
*/

    // Enable statistics block
    Set_EXP_8x6_STATS_ENABLE(g_ExpStats_8x6_Ctrl.e_Flag_EnableExpStats);

	g_ExpStats_8x6_Status.e_AccMode           	= 	g_ExpStats_8x6_Ctrl.e_AccMode;
	g_ExpStats_8x6_Status.e_AccType           	= 	g_ExpStats_8x6_Ctrl.e_AccType;
	g_ExpStats_8x6_Status.e_AccColor          	= 	g_ExpStats_8x6_Ctrl.e_AccColor;
	g_ExpStats_8x6_Status.e_AccSrc            	= 	g_ExpStats_8x6_Ctrl.e_AccSrc;
	g_ExpStats_8x6_Status.e_Flag_AccStaggered 	= 	g_ExpStats_8x6_Ctrl.e_Flag_AccStaggered;

    // program the Exp Stats Block as per the control
    if (ExpStatisticsMode_e_Manual == g_ExpStats_8x6_Ctrl.e_ExpStatisticsMode)
    {
    	g_ExpStats_8x6_Status.u16_ZoneOffset_X    	= 	g_ExpStats_8x6_Ctrl.u16_ZoneOffset_X;
    	g_ExpStats_8x6_Status.u16_ZoneOffset_Y    	= 	g_ExpStats_8x6_Ctrl.u16_ZoneOffset_Y;
    	g_ExpStats_8x6_Status.u16_ZoneSize_X      	= 	g_ExpStats_8x6_Ctrl.u16_ZoneSize_X;
    	g_ExpStats_8x6_Status.u16_ZoneSize_Y      	= 	g_ExpStats_8x6_Ctrl.u16_ZoneSize_Y;
    	g_ExpStats_8x6_Status.u16_AccThreshold_Lo 	= 	g_ExpStats_8x6_Ctrl.u16_AccThreshold_Lo;
//    	g_ExpStats_8x6_Status.u16_AccThreshold_Hi 	= 	g_ExpStats_8x6_Ctrl.u16_AccThreshold_Hi;

    }
    else
    {
    	g_ExpStats_8x6_Status.u16_ZoneOffset_X    	= 	ExpStats_8x6_GetWOI_X_Start();
    	g_ExpStats_8x6_Status.u16_ZoneOffset_Y    	= 	ExpStats_8x6_GetWOI_Y_Start();
    	g_ExpStats_8x6_Status.u16_ZoneSize_X      	= 	((uint16_t)(ExpStats_8x6_GetWOI_X_Size() / 8)) & 0xFFFC;
    	g_ExpStats_8x6_Status.u16_ZoneSize_Y      	= 	((uint16_t)(ExpStats_8x6_GetWOI_Y_Size() / 6)) & 0xFFFC;
    	g_ExpStats_8x6_Status.u16_AccThreshold_Lo 	= 	0x0;
//    	g_ExpStats_8x6_Status.u16_AccThreshold_Hi 	= 	0xff;
    }

	Set_EXP_8x6_STATS_ZONE_OFFSET_X(g_ExpStats_8x6_Status.u16_ZoneOffset_X);
	Set_EXP_8x6_STATS_ZONE_OFFSET_Y(g_ExpStats_8x6_Status.u16_ZoneOffset_Y);

	Set_EXP_8x6_STATS_ZONE_SIZE_X(g_ExpStats_8x6_Status.u16_ZoneSize_X);
	Set_EXP_8x6_STATS_ZONE_SIZE_Y(g_ExpStats_8x6_Status.u16_ZoneSize_Y);

	Set_EXP_8x6_STATS_THRESHOLD_LO(g_ExpStats_8x6_Status.u16_AccThreshold_Lo);
//	Set_EXP_8x6_STATS_THRESHOLD_HI(g_ExpStats_8x6_Status.u16_AccThreshold_Hi);

	Set_EXP_8x6_STATS_ACC_CTRL_REQ(g_ExpStats_8x6_Status.e_AccMode, g_ExpStats_8x6_Status.e_AccType, g_ExpStats_8x6_Status.e_AccColor, g_ExpStats_8x6_Status.e_AccSrc, g_ExpStats_8x6_Status.e_Flag_AccStaggered);

    // Program the statistics processor gain register and acc_type according to the weight
    //  given by the Page Element ExposureControls.bExposureWeight
    switch (ExpStats_8x6_GetMeteringMode())
    {
        case Exposure_Metering_e_Flat:
            // all gains are 3.
        	g_ExpStats_8x6_Status.u32_ZoneGains_1_16  = 0xffffffff;
        	g_ExpStats_8x6_Status.u32_ZoneGains_17_32 = 0xffffffff;
        	g_ExpStats_8x6_Status.u32_ZoneGains_33_48 = 0xffffffff;

            break;

        case Exposure_Metering_e_Centred:
        	g_ExpStats_8x6_Status.u32_ZoneGains_1_16  = 0x5aa55aa5;
        	g_ExpStats_8x6_Status.u32_ZoneGains_17_32 = 0xaffaaffa;
        	g_ExpStats_8x6_Status.u32_ZoneGains_33_48 = 0x5aa55aa5;

            break;

        case Exposure_Metering_e_Backlit:
        	g_ExpStats_8x6_Status.u32_ZoneGains_1_16  = 0x00000000;
        	g_ExpStats_8x6_Status.u32_ZoneGains_17_32 = 0x05500550;
        	g_ExpStats_8x6_Status.u32_ZoneGains_33_48 = 0x55550550;

            break;

        case Exposure_Metering_e_Skintone:

            //Skintone_Configure();

            //g_ExpStats_8x6_Status.u8_SumOfZoneGains = SumOfZoneGains();
            break;

        case Exposure_Metering_e_Exp_Manual:
        	g_ExpStats_8x6_Status.u32_ZoneGains_1_16  = g_ExpStats_8x6_Ctrl.u32_ZoneGains_1_16;
        	g_ExpStats_8x6_Status.u32_ZoneGains_17_32 = g_ExpStats_8x6_Ctrl.u32_ZoneGains_17_32;
        	g_ExpStats_8x6_Status.u32_ZoneGains_33_48 = g_ExpStats_8x6_Ctrl.u32_ZoneGains_33_48;

            break;
    }   //end of switch

    Set_EXP_8x6_STATS_ACC_ZONE_GAINS_1_16(g_ExpStats_8x6_Status.u32_ZoneGains_1_16);
    Set_EXP_8x6_STATS_ACC_ZONE_GAINS_17_32(g_ExpStats_8x6_Status.u32_ZoneGains_17_32);
    Set_EXP_8x6_STATS_ACC_ZONE_GAINS_33_48(g_ExpStats_8x6_Status.u32_ZoneGains_33_48);

    g_ExpStats_8x6_Status.u8_SumOfZoneGains = SumOfZoneGains();

    return;
}


/**
 * \fn      void ExpStats_8x6_ExtractMeanStatistics( void )
 * \brief   Calculates avaregae pixel energy
 * \param   void
 * \return  void
 * \callgraph
 * \callergraph
 * \ingroup ExpStats_8x6
*/
void
ExpStats_8x6_ExtractMeanStatistics(void)
{
    /*

           TotalEnergy = Acc * DataShiftMeasure (out of 12 bits, 8 bits are accumulated)
           DataShiftMeasure = 4bits == *16

           NumPixels = (xSize * ySize / SubSampleMeasure) * SumGains
           SubSampleMeasure = 16  (1 in 16 pixels are accumulated)
           SumGains for FLAT = 16 (16 equaly weighted zones)
           SumGains for ZONED = sum of all the zone_gains.

           Average Pixel Energy = TotalEnergy/NumPixels
                   = (Acc * 256) / (x * y * SumGains)

          */

    // GreenInRed channel
    g_ExpStats_8x6_Status.f_MeanEnergy = CalculateNormalizedStatistics(
    		Get_EXP_8x6_STATS_ACC_TOTAL(),
    		Get_EXP_8x6_STATS_ACC_ZONE_SIZE_X() * Get_EXP_8x6_STATS_ACC_ZONE_SIZE_Y(),
            g_ExpStats_8x6_Status.u8_SumOfZoneGains);

    return;
}

/**
 * \fn      void ExpStats_8x6_Statistics_ISR(void)
 * \brief   calculates mean energy & runs exposure
 * \param   void
 * \return  void
 * \callgraph
 * \callergraph
 * \ingroup ExpStats_8x6
*/
void
ExpStats_8x6_Statistics_ISR(void)
{
    ExpStats_8x6_ExtractMeanStatistics();

    // if Rx is streaming then stats are valid, else invalid
    g_ExpStats_8x6_Status.e_Flag_StatsValid = EXP_8x6_IS_SENSOR_STREAMING();

    g_ExpStats_8x6_Status.u8_ExpStatsInterruptCount++;

}



/**
 * \fn      uint8_t SumOfZoneGains(void)
 * \brief   Calculate the Sum of Zone Gains
 * \param   void
 * \return  u8_Sum The sum of all the zones.
 * \callgraph
 * \callergraph
 * \ingroup ExpStats_8x6
*/
uint8_t
SumOfZoneGains(void)
{
    uint8_t u8_Sum;

	u8_Sum = SumOfZonesIn32BitRegister(g_ExpStats_8x6_Status.u32_ZoneGains_1_16)  +
	         SumOfZonesIn32BitRegister(g_ExpStats_8x6_Status.u32_ZoneGains_17_32) +
             SumOfZonesIn32BitRegister(g_ExpStats_8x6_Status.u32_ZoneGains_33_48);

    return (u8_Sum);
}


/**
 * \fn      uint8_t SumOfZonesIn32BitRegister(uint32_t u32_ZoneGainReg)
 * \brief   Calculate the Sum of Zones on a 32 bit integer.
 * \param   void
 * \return  u8_Sum The sum of all the zones in 32 bit register
 * \callgraph
 * \callergraph
 * \ingroup ExpStats_8x6
*/
uint8_t SumOfZonesIn32BitRegister(uint32_t u32_ZoneGainReg)
{
    uint8_t u8_Counter, u8_Sum, u8_temp;
    u8_Sum = 0;

	for(u8_Counter = 16; u8_Counter>0; u8_Counter--)
	{
        u8_temp = u32_ZoneGainReg & 0x3;
        if(u8_temp == 3)
        {
        	u8_temp = 4;
        }

		u8_Sum = u8_Sum + u8_temp;

	    u32_ZoneGainReg = u32_ZoneGainReg >> 2;
	}

    return (u8_Sum);
}


/**
 \fn      float_t CalculateNormalizedStatistics(uint32_t u32_Acc, uint32_t u32_NoOfPixelPerZone, uint16_t    u16_SumOfZoneGains)
 \brief   Calculates avaregae pixel energy
 \details Calculates avererage pixel energy
 \param   u32_Acc: Accumulated energy
 \param   u32_NoOfPixelPerZone: Total no of pixel per zone
 \param   u16_SumOfZoneGains: Zone gain
 \param   u8_NoOfZones: No of active zones
 \return  f_AvgPixelEnergy The normalised statistics
 \callgraph
 \callergraph
 \ingroup ExpStats_8x6
*/
float_t
CalculateNormalizedStatistics(
uint32_t    u32_Acc,
uint32_t    u32_NoOfPixelPerZone,
uint16_t    u16_SumOfZoneGains)
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

#endif        // EXPOSURE_STATS_PROCESSOR_6x8_ZONES



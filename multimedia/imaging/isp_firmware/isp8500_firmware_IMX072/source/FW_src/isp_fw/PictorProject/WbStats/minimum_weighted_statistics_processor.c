/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file minimum_weighted_statistics_processor_op_interface.c
 \brief
 \ingroup awb
*/
#include "minimum_weighted_statistics_processor_op_interface.h"
#include "minimum_weighted_statistics_platform_specific.h"

#if MWWB_INCLUDE_MWWB

/// Page containing the White Balance statistics control
MWWB_Controls_ts    g_MWWB_Statistics_control;

MWWB_Status_ts      g_MWWB_Statistics_Status;

/**
 \fn       void Macro_Pixel_SubSampler_Setup_ISR(void)
 \brief
 \details
 \return   void
 \callgraph
 \callergraph
 \ingroup WBStats
*/
void
Macro_Pixel_SubSampler_Setup_ISR(void)
{
    // pictor_fs_1_0.pdf: P93, Figure 53, MPSS block
    // pictor_fs_1_0.pdf: P407, ISP_STATS_MPSS
    // Enable the IP
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_ENABLE(0x1);                  //enable

    // use staggered mode
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_CTRL(1);                      //stgrd

    /** Horizontal sub-sample
    [3:0] iqval_h:
    Horizontalsubsample ratio
    Interpretation of iqval_h is as follows
    000 to 001: No SUb sampling, by pass mode
    010: pick1 macro ( 2pixels), Leave 1 macro ( 2 pixels)
    011: pick1 macro ( 2pixels), Leave 3 macro ( 6 pixels)
    100: pick1 macro ( 2pixels), Leave 7 macro ( 14 pixels)
    101: pick1 macro ( 2pixels), Leave 15 macro ( 30 pixels)
    */

    // Use 3 as horizontal sub-sampling
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_PARAM_HOR(3);                 //iqval_h

    /** Vertical sub-sample
    [3:0] iqval_v:
    Vertical subsample ratio
    Interpretation of iqval_v is as follows
    000 to 001: No SUb sampling, by pass mode
    010: pick1 macro ( 2lines), Leave 1 macro ( 2 lines)
    011: pick1 macro ( 2lines), Leave 3 macro ( 6 lines)
    100: pick1 macro ( 2lines), Leave 7 macro ( 14 lines)
    101: pick1 macro ( 2lines), Leave 15 macro ( 30 lines)
    */

    // No vertical sub-sampling
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_PARAM_VER(1);                 //iqval_v

    /** ofirstmin4 Offset
    [6:0] ofirstmin4_offset_cnt:
    Horizontal sub-sample Count to calculate number of clock pulses between ifir
    exactly 4 clocks before first valid ohenv
    Calculation of ofirstmin4_offset_cnt values is as follows
    No Sub Sampling, by pass mode - (ofirstmin4_offset_cnt == 0/1)
    (iqval_h == 2) => (program value for ofirstmin4_offset_cnt == 11)
    (iqval_h == 3) => (program value for ofirstmin4_offset_cnt == 23)
    (iqval_h == 4) => (program value for ofirstmin4_offset_cnt == 47)
    (iqval_h == 5) => (program value for ofirstmin4_offset_cnt == 95)
    */

    // Horizontal sub-sampling is 3, so use 23 as value
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_OFIRSTMIN4_OFFSET_CNT(23);    // ofirstmin4_offset_cnt

    /** No. of Subsampled active lines
    [15:0] num_active_subsampled_lines:
    Program number of subsampled lines expected at the output of MPSS
    Number of Subsampled active lines at o/p MPSS. Its value is to be programmed as per the
    following logic: if (ver_ss_ratio > 1) { num_active_subsampled_lines =
    ((total_active_active_lines_from_sensor>>(ver_ss_ratio)) * 2) - 1 } else {
    num_active_subsampled_lines = tota_active_lines_from_sensor; }Note that vertical subsampling
    ratio cannot be greater than 5. This calculation is done in software. This value is used inside the
    hardware to generate linetype for LAST ACTIVE LINE.
    */
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_NUM_ACTIVE_SS_LINES(MWWB_GetNoOfActiveLinesFromSensor()); // num_active_subsampled_lines
}

#endif	// #if MWWB_INCLUDE_MWWB

/**
 \fn       void Minimum_Weighted_Statistics_Processor_Setup_ISR(void)
 \brief
 \details
 \return   void
 \callgraph
 \callergraph
 \ingroup WBStats
*/
void
Minimum_Weighted_Statistics_Processor_Setup_ISR(void)
{
#if MWWB_INCLUDE_MWWB

    uint32_t    u32_NoOfPixelsInCurrentWOI;
    uint8_t     bQValH;
    uint8_t     bOFirstMin4Offset;

    Macro_Pixel_SubSampler_Setup_ISR();

    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_ENABLE(0x1);

    u32_NoOfPixelsInCurrentWOI = MWWB_GetWOI_X_Size() * MWWB_GetWOI_Y_Size();

    // arrive at the best sub sampling factor
    // that will give the maximum number of macro
    // pixels to be less that 128K (which is the
    // register width of the macro pixels reported
    // by the hardware )
    if ((u32_NoOfPixelsInCurrentWOI >> 4) < MAXIMUM_NUMBER_OF_MACRO_PIXELS_SUPPORTED)
    {
        bQValH = 3;
        bOFirstMin4Offset = 23;
    }
    else if ((u32_NoOfPixelsInCurrentWOI >> 5) < MAXIMUM_NUMBER_OF_MACRO_PIXELS_SUPPORTED)
    {
        bQValH = 4;
        bOFirstMin4Offset = 47;
    }
    else
    {
        bQValH = 5;
        bOFirstMin4Offset = 95;
    }


    // Calculate the values of Offset and Zone size
    g_MWWB_Statistics_Status.u16_Zone_X_Offset = (MWWB_GetWOI_X_Start() >> (bQValH - 1));
    g_MWWB_Statistics_Status.u16_Zone_X_Size = ((uint16_t)MWWB_GetWOI_X_Size() >> (bQValH - 1));

    // make y size a multiple of 16 to make it complete its y size when the exposure stats fire
    g_MWWB_Statistics_Status.u16_Zone_Y_Offset = MWWB_GetWOI_Y_Start();
    g_MWWB_Statistics_Status.u16_Zone_Y_Size = MWWB_GetWOI_Y_Size();

    // Initialize the actual hardware Register
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_ZONE_OFFSET_HOR(g_MWWB_Statistics_Status.u16_Zone_X_Offset);      // horz_offset
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_ZONE_OFFSET_VER(g_MWWB_Statistics_Status.u16_Zone_Y_Offset);      // vert_offset
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_ZONE_SIZE_HOR(g_MWWB_Statistics_Status.u16_Zone_X_Size);          // horz_size
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_ZONE_SIZE_VER(g_MWWB_Statistics_Status.u16_Zone_Y_Size);          // vert_size
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_SATUR_THRESH(g_MWWB_Statistics_control.u16_SaturationThreshold);  //Satur_Thresh
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_RED_TILT_GAIN(ConvertFloatToUnsigned_8_DOT_8_format(MWWB_TILT_GAIN_RED()));
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_GREEN1_TILT_GAIN(ConvertFloatToUnsigned_8_DOT_8_format(MWWB_TILT_GAIN_GREEN1()));
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_GREEN2_TILT_GAIN(ConvertFloatToUnsigned_8_DOT_8_format(MWWB_TILT_GAIN_GREEN2()));
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_BLUE_TILT_GAIN(ConvertFloatToUnsigned_8_DOT_8_format(MWWB_TILT_GAIN_BLUE()));

    Set_ISP_STATS_MUX_MWWB_ISP_STATS_MUX_MWWB_ENABLE(1, 0); // mux2to1_enable,mux2to1_soft_reset
    if (MWWB_StatisticsSource_e_MWWB_PostChannelGains == g_MWWB_Statistics_Status.e_MWWB_StatisticsSource)
    {
        // mux2to1_select: 1 , mux2to1_shadow_en: 0
        Set_ISP_STATS_MUX_MWWB_ISP_STATS_MUX_MWWB_SELECT(1, 0);
    }
    else
    {
        // mux2to1_select: 0 , mux2to1_shadow_en: 0
        Set_ISP_STATS_MUX_MWWB_ISP_STATS_MUX_MWWB_SELECT(0, 0);
    }

#else

    // disable the stats block if the corresponding firmware has not been included
    Set_ISP_STATS_MUX_MWWB_ISP_STATS_MUX_MWWB_ENABLE(0, 0); // mux2to1_enable,mux2to1_soft_reset

#endif	// #if MWWB_INCLUDE_MWWB

    return;
}


/**
\fn void Minimum_Weighted_Statistics_Processor_ExtractMeanStatistics(void)
\brief     Calculates mean statistics for grey white balance using w
\ingroup   WBStats
\return   void
\callgraph
\callergraph
*/
void
Minimum_Weighted_Statistics_Processor_ExtractMeanStatistics(void)
{
#if MWWB_INCLUDE_MWWB
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // No of accumulated pixels
    uint32_t    u32_NumberMacroPixel = 0;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (Stream_IsISPInputStreaming())
    {
        // Check for any error from zones
        // pictor_fs[1]_09.pdf: P370, ISP_STATS_ACCWZ_ZONE_STATUS
        if (0x00 == Get_ISP_STATS_MWWB_ISP_STATS_MWWB_CTRL_STATUS_zones_error())
        {
            // Convert the nuber of macro pixel in the floating point and store in the page element.
            // MinWeightedWBStatus.fpNumberMacroPixel = FPAlu_WordToFloat( MwwbTotalMacros.total_macros );
            u32_NumberMacroPixel = g_MWWB_Statistics_Status.u32_NumberMacroPixel = Get_ISP_STATS_MWWB_ISP_STATS_MWWB_TOTAL_PIXELS();

            g_MWWB_Statistics_Status.f_RedEnergy = Get_ISP_STATS_MWWB_ISP_STATS_MWWB_RED_ENERGY() / u32_NumberMacroPixel;

            if (Channel_Green1 == g_MWWB_Statistics_control.e_Channel_Accumulate)
            {
                g_MWWB_Statistics_Status.f_GreenEnergy = Get_ISP_STATS_MWWB_ISP_STATS_MWWB_GREEN1_ENERGY() / u32_NumberMacroPixel;
            }
            else if (Channel_Green2 == g_MWWB_Statistics_control.e_Channel_Accumulate)
            {
                g_MWWB_Statistics_Status.f_GreenEnergy = Get_ISP_STATS_MWWB_ISP_STATS_MWWB_GREEN2_ENERGY() / u32_NumberMacroPixel;
            }
            else
            {
                g_MWWB_Statistics_Status.f_GreenEnergy =
                    (
                        Get_ISP_STATS_MWWB_ISP_STATS_MWWB_GREEN1_ENERGY() +
                        Get_ISP_STATS_MWWB_ISP_STATS_MWWB_GREEN2_ENERGY()
                    ) /
                    (2 * u32_NumberMacroPixel);
            }


            // For Blue channel
            g_MWWB_Statistics_Status.f_BlueEnergy = Get_ISP_STATS_MWWB_ISP_STATS_MWWB_BLUE_ENERGY();

            g_MWWB_Statistics_Status.e_Result_Valid = Result_e_Success;
        }
        else
        {
            g_MWWB_Statistics_Status.e_Result_Valid = Result_e_Failure;
        }
    }
    else
    {
    	g_MWWB_Statistics_Status.e_Result_Valid = Result_e_Failure;
    }

#endif	// #if MWWB_INCLUDE_MWWB

    return;
}


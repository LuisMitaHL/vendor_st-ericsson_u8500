/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file  PipeAlgorithmCommit.c
 \brief API's to update pixel pipe related variables exist in PipeAlgorithmUpdate. PipeAlgorithmCommit.c contain procedure to
        to prgoramme values in Pixel Pipe.
 \ingroup SystemConfig
 \endif
*/
#include "PipeAlgorithm.h"
#include "Exposure_OPInterface.h"
#include "ZoomTop.h"
#include "Zoom_OPInterface.h"
#include "FrameDimension_op_interface.h"
#include "SystemConfig.h"
#include "histogram_op_interface.h"
#include "Glace_OPInterface.h"

#define STATS_X_SIZE    (500)
#define STATS_Y_SIZE    (400)
#define STATS_X_OFFSET  (15)
#define STATS_Y_OFFSET  (15)

SpecialEffects_Control_ts   g_SpecialEffects_Control[PIPE_COUNT] =
{
    { SFXSolariseControl_Disable, SFXNegativeControl_Disable, BlackAndWhiteControl_Disable, SepiaControl_Disable },
    { SFXSolariseControl_Disable, SFXNegativeControl_Disable, BlackAndWhiteControl_Disable, SepiaControl_Disable }
};

void
INT03_Setup_LBE(void)
{
    //    Set_ISP_LBE_ISP_LBE_ENABLE(1, 0);   // line_blk_elim_enable,line_blk_elim_soft_reset
    Set_ISP_LBE_ISP_LBE_H_SIZE(FrameDimension_GetRequestedVTLineLength());                              // line_blk_elim_h_size
    //    FrameDimension_GetRequestedVTLineLength()
}


void
INT05_Setup_SMIA_RX(void)
{
    // Setup SMIA Rx Line count0 to fire at line 1, pixel 1
    Set_ISP_SMIARX_ISP_SMIARX_INT_COUNT_0(1, 1);                                                        // pix_count,line_count
    Set_ISP_SMIARX_ISP_SMIARX_INT_COUNT_1(3, 3);                                                        // pix_count,line_count
    Set_ISP_SMIARX_ISP_SMIARX_INT_COUNT_2(10, 10);                                                      // pix_count,line_count
    Set_ISP_SMIARX_ISP_SMIARX_INT_COUNT_3(20, 20);                                                      // pix_count,line_count

    // Enable Line count 0 interrupt in Rx
    // rx_line_count_0_int_en,
    // rx_line_count_1_int_en,
    // rx_line_count_2_int_en,
    // rx_line_count_3_int_en,
    // rx_seq_complete_int_en
    Set_ISP_SMIARX_ISP_SMIARX_COUNT_INT_EN(1, 1, 1, 1, 1);
}


void
INT08_Setup_STAT0_256_bins_histogram_AEC(void)
{
    Set_ISP_STATS_MUX_HIST_ISP_STATS_MUX_HIST_ENABLE(1, 0);                                             // mux2to1_enable,mux2to1_soft_reset

    /* TODO: <AG>: Verify for Post grid iron  */

    // Histogram select from Post channel gains
    Set_ISP_STATS_MUX_HIST_ISP_STATS_MUX_HIST_SELECT(1, 0);                                             // mux2to1_select,mux2to1_shadow_en
    Set_ISP_STATS_HIST_ISP_STATS_HIST_ENABLE(1, 0);                                                     // enable,soft_reset
    Set_ISP_STATS_HIST_ISP_STATS_HIST_CMD(01);                                                          // cmd continuous mode
    Set_ISP_STATS_HIST_ISP_STATS_HIST_X_SIZE(STATS_X_SIZE);

    Set_ISP_STATS_HIST_ISP_STATS_HIST_Y_SIZE(STATS_Y_SIZE);

    Set_ISP_STATS_HIST_ISP_STATS_HIST_X_OFFSET(STATS_X_OFFSET);

    Set_ISP_STATS_HIST_ISP_STATS_HIST_X_OFFSET(STATS_Y_OFFSET);

    Set_ISP_STATS_HIST_ISP_STATS_HIST_PIXELIN_SHIFT(1);                                                 //pixel_shift random 1 as shift
}


void
INT09_Setup_STAT1_6X8_Expsoure(void)
{
    // Enable statistics block
    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ENABLE(enable_B_0x1);

    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_OFFSETY_REQ(STATS_X_SIZE / 8);
    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_OFFSETX_REQ(STATS_Y_OFFSET);

    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_SIZEX_REQ(STATS_X_SIZE / 6);
    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_SIZEY_REQ(STATS_Y_OFFSET);

    /* TODO: <AG>: check various modes of operation */
    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_THRESHOLDLO_REQ(0);
    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_CTRL_REQ_acc_mode_req__FULL_PIXELVAL();
    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_CTRL_REQ_acc_color_req__GIR();
    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_CTRL_REQ_acc_src_req__SRC_IDP_2();

    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_GAINS_1_16_REQ(0x55555555);
    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_GAINS_17_32_REQ(0x55555555);
    Set_ISP_STATS_ACC_8x6_ISP_STATS_ACC_8x6_ZONE_GAINS_33_48_REQ(0x55555555);
}


void
INT10_Setup_STAT2_Auto_focus(void)
{
    Set_ISP_STATS_AF_ISP_STATS_AF_ENABLE(1);                                                            // enable
}


void
INT11_Setup_STAT3_MWWB_White_Balance(void)
{
    Set_ISP_STATS_MUX_MWWB_ISP_STATS_MUX_MWWB_ENABLE(1, 0);                                             // mux2to1_enable,mux2to1_soft_reset

    // mux2to1_select: 1 , mux2to1_shadow_en: 0
    Set_ISP_STATS_MUX_MWWB_ISP_STATS_MUX_MWWB_SELECT(1, 0);

    // pictor_fs_1_0.pdf: P93, Figure 53, MPSS block
    // pictor_fs_1_0.pdf: P407, ISP_STATS_MPSS
    // Enable the IP
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_ENABLE(0x1);                                                      //enable

    // use staggered mode
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_CTRL(1);                                                          //stgrd

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
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_PARAM_HOR(3);                                                     //iqval_h

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
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_PARAM_VER(1);                                                     //iqval_v

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
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_OFIRSTMIN4_OFFSET_CNT(23);                                        // ofirstmin4_offset_cnt

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
    Set_ISP_STATS_MPSS_ISP_STATS_MPSS_NUM_ACTIVE_SS_LINES(FrameDimension_GetRequestedOPYOutputSize());  // num_active_subsampled_lines
    Set_ISP_STATS_SNAIL_MWWB_ISP_STATS_SNAIL_MWWB_ENABLE(1, 0);                 // snailnorec_enable,snailnorec_soft_reset
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_ENABLE(0x1);

    // Initialize the actual hardware Register
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_ZONE_OFFSET_HOR(10);                      // horz_offset
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_ZONE_OFFSET_VER(10);                      // vert_offset
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_ZONE_SIZE_HOR(500);                       // horz_size
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_ZONE_SIZE_VER(400);                       // vert_size
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_SATUR_THRESH(0xFFF);                      //Satur_Thresh
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_RED_TILT_GAIN(1024);
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_GREEN1_TILT_GAIN(1024);
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_GREEN2_TILT_GAIN(1024);
    Set_ISP_STATS_MWWB_ISP_STATS_MWWB_BLUE_TILT_GAIN(1024);
}


void
INT12_Setup_STAT4_Skin_tone_detection(void)
{
    Set_ISP_STATS_SKINTONE_ISP_STATS_SKINTONE_ENABLE(1);                        // enable
    Set_ISP_STATS_SKINTONE_ISP_STATS_SKINTONE_ZONE_HOR_OFFSET(STATS_X_OFFSET);  // zone_x_offset
    Set_ISP_STATS_SKINTONE_ISP_STATS_SKINTONE_ZONE_VERT_OFFSET(STATS_Y_OFFSET); // zone_y_offset
    Set_ISP_STATS_SKINTONE_ISP_STATS_SKINTONE_ZONE_HOR_SIZE(STATS_X_OFFSET);    // zone_x_size
    Set_ISP_STATS_SKINTONE_ISP_STATS_SKINTONE_ZONE_VERT_SIZE(STATS_Y_OFFSET);   // zone_y_size
    Set_ISP_STATS_SKINTONE_ISP_STATS_SKINTONE_THRESHOLD(0x0FFF);                // skinTresh

    /* TODO: <AG>: Not sure */

    // Set_ISP_STATS_SKINTONE_ISP_STATS_SKINTONE_PATTERN(pattern);
}


void
INT14_Setup_STAT6_Arctic(void)
{
    /* TODO: <AG>: to be discussed with SS */
}


void
INT15_Setup_STAT7_Acc_wg_zones_White_Balance(void)
{
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

    // Enable statistics block
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ENABLE_word(0x01);

    /* TODO: <AG>: check for all interrupts */
    if (1)
    {
        u8_StatisticsSource = src_SRC_IDP_2;
    }
    else if (1)
    {
        u8_StatisticsSource = src_SRC_IDP_1;
    }
    else
    {
        u8_StatisticsSource = src_SRC_IDP_0;
    }


    /**
         pictor_fs[1]_09.pdf: P80, Statistics processor

         1. Statistics block is divided into 16 zones of 4x4 grouping.
            u16_ZoneSizeX and u16_ZoneSizeY are zone size will be 1/4th of window of interest size.
            Zone size = WOI_Size / 4 or right shift 2.

         2. Accumulator pic every 4th pixel, so size should be multiple of 4 in order to avoid boundary cases.
            Zone size = Zone size & 0xFFFC
            */
    u16_ZoneSizeX = STATS_X_SIZE >> 2;
    u16_ZoneSizeX = u16_ZoneSizeX & 0xFFFC;                             // we want the zone sizes to be a multiple of 4...
    u16_ZoneSizeY = STATS_Y_SIZE >> 2;
    u16_ZoneSizeY = u16_ZoneSizeY & 0xFFFC;

    u16_ZoneOffsetX = STATS_X_OFFSET;
    u16_ZoneOffsetY = STATS_Y_OFFSET;

    u16_LowThreshold = 0;
    u16_HighThreshold = 1023;

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
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC0_CTRL(mode_CLIP_PIXELVAL, type_ACC_FLAT, color_GIR, u8_StatisticsSource);

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
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC1_CTRL(mode_CLIP_PIXELVAL, type_ACC_FLAT, color_RED, u8_StatisticsSource);

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
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC2_CTRL(mode_CLIP_PIXELVAL, type_ACC_FLAT, color_BLU, u8_StatisticsSource);

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
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ACC3_CTRL(mode_CLIP_PIXELVAL, type_ACC_FLAT, color_GIB, u8_StatisticsSource);

    // To be removed, Gain should not be part if flat is used
    Set_ISP_STATS_ACCWZ_ISP_STATS_ACCWZ_ZONE_GAINS_word(0x55555555);
}


void
INT16_Setup_STAT8_Glace_AWB(void)
{
    Set_ISP_STATS_MUX_GLACE_ISP_STATS_MUX_GLACE_ENABLE(1, 0);           // mux2to1_enable,mux2to1_soft_reset

    /* TODO: <AG>: 1 post gain, 0 post grid iron */
    Set_ISP_STATS_MUX_GLACE_ISP_STATS_MUX_GLACE_SELECT(1, 0);           // mux2to1_select,mux2to1_shadow_en
    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_ACC_ENABLE(1, 0);               // GLACE_ACC_ENABLE,SOFT_RESET
    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_H_ROI_START(STATS_X_OFFSET);    // GLACE_H_ROI_START
    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_V_ROI_START(STATS_Y_OFFSET);    // GLACE_V_ROI_START
    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_H_BLK_SIZE(12);                 // GLACE_H_BLK_SIZE
    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_V_BLK_SIZE(12);                 // GLACE_V_BLK_SIZE
    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_H_GRID_SIZE(12);                // GLACE_H_GRID_SIZE
    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_V_GRID_SIZE(12);                // GLACE_V_GRID_SIZE
    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_SAT_LEVEL_R(500);               // GLACE_SAT_LEVEL_R
    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_SAT_LEVEL_G(500);               // GLACE_SAT_LEVEL_G
    Set_ISP_STATS_GLACE_ISP_STATS_GLACE_SAT_LEVEL_B(500);               // GLACE_SAT_LEVEL_B
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t CommitPipe(uint8_t u8_PipeNo)
 \brief     The procedure programme the values calculated in PipeAlgorithmUpdate in the pixel pipe
 \details   The procedure programme the values calculated in PipeAlgorithmUpdate in the pixel pipe
            It can be called from interrupt
 \param u8_PipeNo: Pipe No in ISP
 \return uint8_t
 Result_e_Success: Update successful
 Result_e_Failure: Update unsuccessful
 \ingroup SystemConfig
 \callgraph
 \callergraph
 \endif
*/
uint8_t
CommitPipe(
uint8_t u8_PipeNo)
{
    // update pipe scalar and crop params
    ZoomTop_PipeUpdate(u8_PipeNo);

    return (Result_e_Success);
}


void
PipeAlgorithm_CommitInputPipe(void)
{
    if (SystemConfig_IsSDPipeActive())
    {
        // commit parameters for the SD Pipe
        CommitSDPipe();
    }


    if (SystemConfig_IsRE_Active())
    {
        // Enable Glace and Histogram statistics collection internally.
        // This is needed so that we can export these statistics, in case
        // of Flash lit frame, and frame in which AEC target is absorbed.
        // DAMPER1
        //EnableGlaceInternally();
        //EnableHistogramInternally();
        //Histogram_Commit();
        //Glace_Commit();
        // commit parameters of RE
        CommitRE();
    }


    if (SystemConfig_IsDMCE_Active())
    {
        // commit DMCE
        CommitDMCE();
    }


    return;
}


void
CommitSDPipe(void)
{
    // TODO: move the commits to their respective firmware blocks once they are available
    // Setup LBE
    Set_ISP_LBE_ISP_LBE_ENABLE(0, 0);                       // line_blk_elim_enable,line_blk_elim_soft_reset
    return;
}


void
CommitRE(void)
{
    // commit RSO
    RSO_Commit();

    // TODO: move the commits to their respective firmware blocks once they are available
    // Setup ISP_FLEXTF_LINEAR
    SDL_Commit();

    // Setup ISP_GRIDIRON
    Gridiron_ApplyGridLiveParameters();

    // CommitDG() was supposed to be called here. It is not called here as it has already been called before invoking ApplyISPParams().
    Duster_Commit();

    // Setup ISP_SNAIL_SCORPIO
    Set_ISP_SNAIL_SCORPIO_ISP_SNAIL_SCORPIO_ENABLE(1, 0);   // snailnorec_enable,snailnorec_soft_reset

    // Setup ISP_SCORPIO_CONTROL
    Scorpio_Commit();

    // Setup ISP_SNAIL_DUSTER
    Set_ISP_SNAIL_DUSTER_ISP_SNAIL_DUSTER_ENABLE(snailnorec_enable_ENABLE, snailnorec_soft_reset_SOFT_RESET_DISABLE);   // snailnorec_enable,snailnorec_soft_reset

    // Setup ISP_SNAIL_BINNING
    Set_ISP_SNAIL_BINNING_ISP_SNAIL_BINNING_ENABLE(1, 0);   // snailnorec_enable,snailnorec_soft_reset

    // Setup ISP_BINNING_REPAIR
    BinningRepair_Commit();

    // Bayer crop is not planned to be used for ISP FW
    // Setup ISP_CROP_BAYER
    Set_ISP_CROP_BAYER_ISP_CROP_BAYER_ENABLE(0, 0);         // crop_enable,crop_soft_reset

    // [TBC][V1V2]Setup ISP_COSM
    //Set_ISP_COSM_ISP_COSM_ENABLE(0, 0);                     // cosm_enable,cosm_soft_reset
    return;
}


void
CommitDMCE(void)
{
    // TODO: move the commits to their respective firmware blocks once they are available
    // Setup ISP_SNAIL_BABYLON
    Set_ISP_SNAIL_BABYLON_ISP_SNAIL_BABYLON_ENABLE(1, 0);   // snailnorec_enable,snailnorec_soft_reset

    if(Is_8540v1())
    {
      // Setup ISP_MOZART
         Mozart_Commit();
    }
    else
    {
      // Setup ISP_BABYLON
         Babylon_Commit();
    }

    // Program IT Point for DMCE OUT here
    Set_ISP_ITPOINT_DMCE_OUT_ISP_ITPOINT_DMCE_OUT_ENABLE_enable__ENABLE();
    Set_ISP_ITPOINT_DMCE_OUT_ISP_ITPOINT_DMCE_OUT_PIX_POS_pix_pos(g_Zoom_Status_LLA.u16_output_resX -
        (COLUMNS_LOST_AT_LEFT_EDGE_DUE_TO_COLOUR_INTERPOLATION + COLUMNS_LOST_AT_RIGHT_EDGE_DUE_TO_COLOUR_INTERPOLATION));
    Set_ISP_ITPOINT_DMCE_OUT_ISP_ITPOINT_DMCE_OUT_LINE_POS_line_pos(g_Zoom_Status_LLA.u16_output_resY -
        (ROWS_LOST_AT_TOP_EDGE_DUE_TO_COLOUR_INTERPOLATION + ROWS_LOST_AT_BOTTOM_EDGE_DUE_TO_COLOUR_INTERPOLATION));


    // [TBC][V1V2]Setup ISP_HBARREL
    //Set_ISP_HBARREL_ISP_HBARREL_ENABLE(0, 0);               // hbarrel_enable,hbarrel_soft_reset
    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t PipeAlgorithm_CommitPipe0(void)
 \brief     Programs the hardware registers of pipe0
 \return Nothing
 \ingroup SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
PipeAlgorithm_CommitPipe0(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // TODO: move the commits to their respective firmware blocks once they are available
    //All commits moved to main loop except those for scalar, cropper and output formatter
    if (SystemConfig_IsPipe0Active())
    {
        CommitPipe(0);
    }
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn uint8_t PipeAlgorithm_CommitPipe1(void)
 \brief     Programs the hardware registers of pipe1
 \return Nothing
 \ingroup SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
PipeAlgorithm_CommitPipe1(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // TODO: move the commits to their respective firmware blocks once they are available
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    //All commits moved to main loop except those for scalar, cropper and output formatter
    if (SystemConfig_IsPipe1Active())
    {
        CommitPipe(1);
    }
    else
    {
        // disable pipe0 output
        // Setup ISP_CEx_IDP_GATE
        Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE(0, 0); // mux2to1_enable,mux2to1_soft_reset
        Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_SELECT(0, 0); // mux2to1_select,mux2to1_shadow_en
    }
}


void PipeAlgorithm_CommitGlaceAndHistogram(void)
{
    if (SystemConfig_IsRE_Active())
    {
        // Enable Glace and Histogram statistics collection internally.
        // This is needed so that we can export these statistics, in case
        // of Flash lit frame, and frame in which AEC target is absorbed.
        EnableGlaceInternally();
        EnableHistogramInternally();

        Histogram_Commit();
        Glace_Commit();
    }
}


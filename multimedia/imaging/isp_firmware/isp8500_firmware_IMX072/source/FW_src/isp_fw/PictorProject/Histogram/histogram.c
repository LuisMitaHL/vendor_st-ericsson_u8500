/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\file    histogram.c
\brief   Defines functions for controlling the histogram block.
\ingroup HStats
*/
#include "histogram_op_interface.h"

#include "Flash.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#   include "PictorProject_Histogram_histogramTraces.h"
#endif

#define HISTOGRAM_DUMP_ENABLE (0)

// Page containing the histogram statistics
HistStats_Ctrl_ts       g_HistStats_Ctrl =
{
    DEFAULT_HIST_R_ADDR,
    DEFAULT_HIST_G_ADDR,
    DEFAULT_HIST_B_ADDR,
    DEFAULT_HIST_REL_SIZE_X,
    DEFAULT_HIST_REL_SIZE_Y,
    DEFAULT_HIST_REL_OFFSET_X,
    DEFAULT_HIST_REL_OFFSET_Y,
    DEFAULT_HIST_PIXEL_SHIFT,
    DEFAULT_HIST_INPUT_SRC,
    DEFAULT_HIST_IP_ENABLE,
    DEFAULT_HIST_IP_SOFT_ENABLE,
    DEFAULT_HIST_CMD,
    DEFAULT_HIST_COIN_CTRL,
    DEFAULT_HIST_MODE,
    DEFAULT_HIST_GEOMETRY_MODE    // e_StatisticsFov
};

HistStats_Status_ts     g_HistStats_Status =
{
    DEFAULT_HIST_SIZE_X,
    DEFAULT_HIST_SIZE_Y,
    DEFAULT_HIST_OFFEST_X,
    DEFAULT_HIST_OFFEST_Y,
    DEFAULT_HIST_DARKEST_R,
    DEFAULT_HIST_BRIGHTEST_R,
    DEFAULT_HIST_HIGHEST_R,
    DEFAULT_HIST_DARKEST_G,
    DEFAULT_HIST_BRIGHTEST_G,
    DEFAULT_HIST_HIGHEST_G,
    DEFAULT_HIST_DARKEST_B,
    DEFAULT_HIST_BRIGHTEST_B,
    DEFAULT_HIST_HIGHEST_B,
    DEFAULT_HIST_COIN_STATUS,
    DEFAULT_HIST_EXPORT_STATUS,
    DEFAULT_HIST_GEOMETRY_MODE    // e_StatisticsFov
};

volatile extern Flag_te g_IsHistogramEnabledInternally;

uint8_t                 Histogram_AreStatsValid (void);
void SetupHistogramStatsBlock ( void ) TO_EXT_DDR_PRGM_MEM;
void    UpdateHistogramStatus (void);
void ComputeHistogramParams ( void ) TO_EXT_DDR_PRGM_MEM;
void    Histogram_ExportStatistics (void);
void    Histogram_ISR (void);
void    Histogram_ExportISR (void);
void Histogram_Commit ( void ) TO_EXT_DDR_PRGM_MEM;
void EnableHistogramInternally ( void ) TO_EXT_DDR_PRGM_MEM;

/// [AG/PM] Kept for debugging purpose.
uint8_t
Histogram_AreStatsValid(void)
{
    uint8_t e_Flag = Flag_e_FALSE;

    if (GlaceHistogramStatus_HistogramDone & g_GlaceHistogramStatsFrozen)
    {
        // Glace export completed, no need to do it again.
        OstTraceInt0(TRACE_FLOW, "->Histogram_AreStatsValid: Already exported histo statistics");
        e_Flag = Flag_e_FALSE;
    }
    else
    {   // if any of the following condition is true, export statistics. This will be hit when anything for statistics except e_coin_ctl is called
        if
        (
            (g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug != g_SystemConfig_Status.e_Coin_Glace_Histogram_Status)
        ||  (g_HistStats_Status.e_CoinStatus != g_HistStats_Ctrl.e_CoinCtrl_debug)
        )
        {
            OstTraceInt0(TRACE_FLOW, "->Histogram_AreStatsValid: found coin other than e_coin_ctrl");
            e_Flag = Flag_e_TRUE;
        }
        else
        {
            if (Flash_IsFlashModeActive())  // if flash is active, we need to test both exposure and flash
            {
                if (Flash_IsFlashStatusParamsReceived() && SystemConfig_IsSensorSettingStatusParamsReceived())
                {
                    OstTraceInt0(TRACE_FLOW, "->Histogram_AreStatsValid: Flash case");
                    e_Flag = Flag_e_TRUE;
                }
            }
            else // Need to check for only exposure
            {
                if (SystemConfig_IsSensorSettingStatusParamsReceived())
                {
                    OstTraceInt0(TRACE_FLOW, "->Histogram_AreStatsValid: e_coin_ctrl case");
                    e_Flag = Flag_e_TRUE;
                }
            }
        }
    }


    return (e_Flag);
}


/// The statistics block needs to be programmed in ISP interrupt for the 1st time.
/// the stats will be processed by HOST
/// wait for the frame where the applied parametrs have been absorbed
/// enable the IP with the programmed statistics.
void
SetupHistogramStatsBlock(void)
{
    //  Set_HIST_ENABLE(g_HistStats_Ctrl.e_Flag_Enable,g_HistStats_Ctrl.e_Flag_SoftResest);
    Set_HIST_SIZE_X(g_HistStats_Status.u16_HistSizeX);
    Set_HIST_SIZE_Y(g_HistStats_Status.u16_HistSizeY);
    Set_HIST_OFFSET_X(g_HistStats_Status.u16_HistOffsetX);
    Set_HIST_OFFSET_Y(g_HistStats_Status.u16_HistOffsetY);
    Set_HIST_PIXEL_SHIFT(g_HistStats_Ctrl.u8_HistPixelInputShift);

    Set_HIST_MUX_SELECT(g_HistStats_Ctrl.e_HistInputSrc, Flag_e_TRUE);
}


void
UpdateHistogramStatus(void)
{
    g_HistStats_Status.u16_BrightestBin_B = Get_HIST_BRIGHTEST_B();
    g_HistStats_Status.u16_DarkestBin_B = Get_HIST_DARKEST_B();
    g_HistStats_Status.u16_HighestBin_B = Get_HIST_HIGHEST_B();
    g_HistStats_Status.u16_BrightestBin_G = Get_HIST_BRIGHTEST_G();
    g_HistStats_Status.u16_DarkestBin_G = Get_HIST_DARKEST_G();
    g_HistStats_Status.u16_HighestBin_G = Get_HIST_HIGHEST_G();
    g_HistStats_Status.u16_BrightestBin_R = Get_HIST_BRIGHTEST_R();
    g_HistStats_Status.u16_DarkestBin_R = Get_HIST_DARKEST_R();
    g_HistStats_Status.u16_HighestBin_R = Get_HIST_HIGHEST_R();
}


void
ComputeHistogramParams(void)
{
    float_t     f_FractionSizeX,
                f_FractionSizeY,
                f_FractionOffsetX,
                f_FractionOffsetY,
                f_InvPreScale,
                f_HistOffsetX,
                f_HistOffsetY;
    uint16_t    u16_FOVX,
                u16_FOVY,
                u16_HistSizeX,
                u16_HistSizeY,
                u16_HistOffsetX,
                u16_HistOffsetY;
    uint8_t     u8_forcedSensorFOV = Flag_e_FALSE;

    g_HistStats_Status.e_StatisticsFov = g_HistStats_Ctrl.e_StatisticsFov;

#if ENABLE_HISTOGRAM_TRACES
    OstTraceInt0(TRACE_DEBUG, "\n <histogram> >> ComputeHistogramParams\n");
    if (SystemConfig_IsBayerStore2Active())
    {
        OstTraceInt0(TRACE_DEBUG, "\n <histogram> histogram mode : BMS2\n");
    }


    if (SystemConfig_IsPipe0Active())
    {
        OstTraceInt0(TRACE_DEBUG, "\n <histogram> histogram mode : HR\n");
    }


    if (SystemConfig_IsPipe1Active())
    {
        OstTraceInt0(TRACE_DEBUG, "\n <histogram> histogram mode : LR\n");
    }
#endif

    f_FractionSizeX = Histogram_Min(1.0, g_HistStats_Ctrl.f_HistSizeRelativeToFOV_X);
    f_FractionSizeY = Histogram_Min(1.0, g_HistStats_Ctrl.f_HistSizeRelativeToFOV_Y);

    f_FractionOffsetX = Histogram_Min(1.0, g_HistStats_Ctrl.f_HistOffsetRelativeToFOV_X);
    f_FractionOffsetY = Histogram_Min(1.0, g_HistStats_Ctrl.f_HistOffsetRelativeToFOV_Y);

    /// sum of f_FractionSizeX and f_FractionOffsetX should not be more than 1
    if (f_FractionSizeX + f_FractionOffsetX > 1.0)   // to be changed to prioritse size
    {
        /// <Hem> As discussed with Atul, f_HBlockSizeFraction should not be changed in this case
        f_FractionOffsetX = 1.0 - f_FractionSizeX;
    }

    /// <Hem> sum of f_FractionSizeY and f_FractionOffsetY should not be more than 1
    if (f_FractionSizeY + f_FractionOffsetY > 1.0)
    {
        // As discussed with Atul, f_VBlockSizeFraction should not be changed in this case
        f_FractionOffsetY = 1.0 - f_FractionSizeY;
    }

    if
    (
        (SystemConfig_IsBayerStore2Active())
    &&  (
            (g_Zoom_Status_LLA.u16_woi_resX < g_Zoom_Status.f_FOVX)
        ||  (g_Zoom_Status_LLA.u16_woi_resY < g_Zoom_Status.f_FOVY)
        )
    )
    {
        // If it is case of BMS, and BMS FOV is smaller than master pipe FOV in at least one direction.
        // set an internal flag to mark that all calculations should be based on sensor FOV in this case
        // OstTraceInt0(TRACE_DEBUG, "\n <glace>  BMS mode FOV is smaller than master pipe FOV, switching internally to sensor FOV \n");
        u8_forcedSensorFOV = Flag_e_TRUE;
    }

#if ENABLE_HISTOGRAM_TRACES
    OstTraceInt2(TRACE_DEBUG, "\n <histogram>  g_HistStats_Ctrl.f_HistSizeRelativeToFOV_X = %f, g_HistStats_Ctrl.f_HistSizeRelativeToFOV_Y = %f\n", g_HistStats_Ctrl.f_HistSizeRelativeToFOV_X, g_HistStats_Ctrl.f_HistSizeRelativeToFOV_Y);
    OstTraceInt2(TRACE_DEBUG, "\n <histogram>  f_FractionSizeX = %u, f_FractionSizeY = %u\n", f_FractionSizeX, f_FractionSizeY);
    OstTraceInt2(TRACE_DEBUG, "\n <histogram>  f_FractionOffsetX = %f, f_FractionOffsetY = %f\n", f_FractionOffsetX, f_FractionOffsetY);
    OstTraceInt1(TRACE_DEBUG, "\n <histogram>  g_HistStats_Ctrl.e_StatisticsFov = %u\n", g_HistStats_Ctrl.e_StatisticsFov);
    OstTraceInt1(TRACE_DEBUG, "\n <histogram>  u8_forcedSensorFOV = %u\n", u8_forcedSensorFOV);
#endif

    if ((StatisticsFov_e_Sensor == g_HistStats_Status.e_StatisticsFov) || (Flag_e_TRUE == u8_forcedSensorFOV))
    {
        u16_FOVX = Histogram_GetInputSizeX();
        u16_FOVY = Histogram_GetInputSizeY();

        u16_HistOffsetX = f_FractionOffsetX * u16_FOVX;
        u16_HistOffsetY = f_FractionOffsetY * u16_FOVY;

        u16_HistSizeX = f_FractionSizeX * u16_FOVX;
        u16_HistSizeY = f_FractionSizeY * u16_FOVY;
    }
    else
    {
        u16_FOVX = Histogram_Get_FOV_X();
        u16_FOVY = Histogram_Get_FOV_Y();

        // take into account any prescaling applied in the sensor
        f_InvPreScale = 1.0 / Histogram_GetPreScale();

        f_HistOffsetX = f_FractionOffsetX * u16_FOVX * f_InvPreScale;
        f_HistOffsetY = f_FractionOffsetY * u16_FOVY * f_InvPreScale;

        u16_HistOffsetX = f_HistOffsetX + Histogram_GetFOV_OffsetX();
        u16_HistOffsetY = f_HistOffsetY + Histogram_GetFOV_OffsetY();

        u16_HistSizeX = f_FractionSizeX * u16_FOVX * f_InvPreScale;
        u16_HistSizeY = f_FractionSizeY * u16_FOVY * f_InvPreScale;
    }

    // enforce u16_HistSizeX and u16_HistSizeY to be even numbers. And equal to Glace start offsets
    u16_HistSizeX = u16_HistSizeX & 0xFFFE;
    u16_HistSizeY = u16_HistSizeY & 0xFFFE;

    // make u16_HistOffsetX and u16_HistOffsetY to be even numbers. And equal to Glace start offsets
    u16_HistOffsetX = u16_HistOffsetX & 0xFFFE;
    u16_HistOffsetY = u16_HistOffsetY & 0xFFFE;

    // update the status page
    g_HistStats_Status.u16_HistSizeX = u16_HistSizeX;
    g_HistStats_Status.u16_HistSizeY = u16_HistSizeY;
    g_HistStats_Status.u16_HistOffsetX = u16_HistOffsetX;
    g_HistStats_Status.u16_HistOffsetY = u16_HistOffsetY;


#if ENABLE_HISTOGRAM_TRACES
    OstTraceInt2(TRACE_DEBUG, "\n <histogram>  g_HistStats_Status.u16_HistSizeX = %u, g_HistStats_Status.u16_HistSizeY = %u\n", g_HistStats_Status.u16_HistSizeX, g_HistStats_Status.u16_HistSizeY);
    OstTraceInt2(TRACE_DEBUG, "\n <histogram>  g_HistStats_Status.u16_HistOffsetX = %u, g_HistStats_Status.u16_HistOffsetY = %u\n", g_HistStats_Status.u16_HistOffsetX, g_HistStats_Status.u16_HistOffsetY);
    OstTraceInt0(TRACE_DEBUG, "\n <histogram> << ComputeGlaceParams\n");
#endif
}


void
Histogram_ExportStatistics(void)
{
    uint32_t    *ptru32_SrcR;
    uint32_t    *ptru32_SrcG;
    uint32_t    *ptru32_SrcB;
    uint32_t    *ptru32_DstR;
    uint32_t    *ptru32_DstG;
    uint32_t    *ptru32_DstB;
    uint16_t    u16_count,
                u16_NoOfBinsPerChannel;

    OstTraceInt0(TRACE_FLOW, "<HIST>->Histogram_ExportStatistics(void)");
    u16_NoOfBinsPerChannel = (uint16_t) 1 << ((uint16_t) NUM_OF_BITS_PER_PIXEL - g_HistStats_Ctrl.u8_HistPixelInputShift);

    ptru32_DstR = g_HistStats_Ctrl.ptru32_HistRAddr;
    ptru32_DstG = g_HistStats_Ctrl.ptru32_HistGAddr;
    ptru32_DstB = g_HistStats_Ctrl.ptru32_HistBAddr;

    ptru32_SrcR = ( uint32_t * ) Histogram_GetRStatsAddr();
    ptru32_SrcG = ( uint32_t * ) Histogram_GetGStatsAddr();
    ptru32_SrcB = ( uint32_t * ) Histogram_GetBStatsAddr();

#if HISTOGRAM_DUMP_ENABLE
    OstTraceInt4(TRACE_FLOW, "<HIST> ET: %d AG: %d EL: %d AL: %d ", g_FrameParamStatus.u32_ExposureTime_us, g_FrameParamStatus.u32_AnalogGain_x256, Get_ISP_SMIARX_ISP_SMIARX_COARSE_EXPOSURE_coarse_exposure(), Get_ISP_SMIARX_ISP_SMIARX_ANALOG_GAIN_analog_gain());
#endif
    if (ptru32_DstR != 0 && ptru32_DstG != 0 && ptru32_DstB != 0)
    {
        for (u16_count = 0; u16_count < u16_NoOfBinsPerChannel; u16_count++)
        {
            *(ptru32_DstR + u16_count) = *(ptru32_SrcR + u16_count);
            *(ptru32_DstG + u16_count) = *(ptru32_SrcG + u16_count);
            *(ptru32_DstB + u16_count) = *(ptru32_SrcB + u16_count);
#if HISTOGRAM_DUMP_ENABLE
            OstTraceInt3(TRACE_FLOW,"<HIST> %06d\t%06d\t%06d",*(ptru32_DstR + u16_count),*(ptru32_DstG + u16_count),*(ptru32_DstB + u16_count));
#endif
        }
    }

    OstTraceInt0(TRACE_FLOW, "<HIST><-Histogram_ExportStatistics(void)");
}


/**
 \fn        void Hist_ISR ( void )
 \brief     Function to be called in context of the Hist interrupt
            to copy the Hist statistics to host address space
 \param     void
 \return    None
 \ingroup   Hist
 \callgraph
 \callergraph
*/
void
Histogram_ISR(void)
{
    if (Histogram_AreStatsValid())
    {
        // Disable the IP
        // Set_HIST_ENABLE(Enable, SoftResest)
        Set_HIST_ENABLE(Flag_e_FALSE, Flag_e_FALSE);

        // Disable the Data Input
        // Hist_SetDataInput(IP_ENABLE, IP_SOFT_ENABLE)
        Hist_SetDataInput(Flag_e_FALSE, Flag_e_FALSE);

        // Update the FW Status Registers with the value in HW status registers
        UpdateHistogramStatus();

        g_HistStats_Status.e_ExportStatus = ExportStatus_e_INCOMPLETE;

        // trigger a interrupt into XP70 to schedule the export of statistics.
        Histogram_ScheduleExport();
    }
    else
    {
        g_HistStats_Status.e_ExportStatus = ExportStatus_e_COMPLETE;

        EnableHistogramInternally();
        Histogram_Commit();
    }


    return;
}


void
Histogram_ExportISR(void)
{
    //Don't export stats if user cancelled the request.
    if (SystemConfig_IsStatsRequestCancelled())
    {
        return;
    }


    // Export the hist stats to host address space
    Histogram_ExportStatistics();

    // If g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug has been togglesd, mark a flag that histogram
    // statistics have been frozen. use this flag for sending notification to the host. This is to be
    // done in main loop. similar notification coming from glace module will also be taken into account.
    // If If g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug has not been toggled, send histogram notification
    // straight away.
    if
    (
        (Flash_IsFlashStatusParamsReceived())
    ||  (g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug != g_SystemConfig_Status.e_Coin_Glace_Histogram_Status)
    ||  (SystemConfig_IsSensorSettingStatusParamsReceived())
    )
    {
        // memorize that histogram statistics have been frozen. Shall be recalled in main loop.
        g_GlaceHistogramStatsFrozen |= GlaceHistogramStatus_HistogramDone;
    }
    else
    {
        if (g_HistStats_Status.e_CoinStatus != g_HistStats_Ctrl.e_CoinCtrl_debug)
        {
            g_HistStats_Status.e_ExportStatus = ExportStatus_e_COMPLETE;
            g_HistStats_Status.e_CoinStatus = g_HistStats_Ctrl.e_CoinCtrl_debug;

            // Inform the host that the Histogram statistics have been exported
            Histogram_StatsReadyNotify();

            EnableHistogramInternally();
            Histogram_Commit();
        }
    }


    return;
}


/**
 \fn        void Hist_Commit ( void )
 \brief     Function to setup the Hist statistics before the
            start of a streaming operation.will be called in the status ISR
 \param     void
 \return    None
 \ingroup   Hist
 \callgraph
 \callergraph
*/
void
Histogram_Commit(void)
{
    if (g_HistStats_Status.e_ExportStatus == ExportStatus_e_COMPLETE)
    {
        if (g_HistStats_Ctrl.e_HistogramMode != HistogramMode_e_IDLE)
        {
            if
            (
                (g_HistStats_Status.e_CoinStatus != g_HistStats_Ctrl.e_CoinCtrl_debug)
            ||  (Flag_e_TRUE == g_IsHistogramEnabledInternally)
            )
            {
                // compute the histogram parameters
                ComputeHistogramParams();

                // Enable the IP
                // Set_HIST_ENABLE(Enable, SoftResest)
                Set_HIST_ENABLE(Flag_e_TRUE, Flag_e_FALSE);

                // Enable the Data Input
                // Hist_SetDataInput(IP_ENABLE, IP_SOFT_ENABLE)
                Hist_SetDataInput(Flag_e_TRUE, Flag_e_FALSE);

                // program the hw registers
                SetupHistogramStatsBlock();

                // We want histogram values to be reset at the beginning of each frame
                // ISP FW will must retrieve these values before the beginning of next frame.
                Set_HIST_CMD(HistCmd_e_GRAB);

                g_IsHistogramEnabledInternally = Flag_e_FALSE;
            }
        }
        else
        {
            // histogram is not to be used. so disable the IP
            // Disable the IP
            // Set_HIST_ENABLE(Enable, SoftResest)
            Set_HIST_ENABLE(Flag_e_FALSE, Flag_e_FALSE);

            // Disable the Data Input
            // Hist_SetDataInput(IP_ENABLE, IP_SOFT_ENABLE)
            Hist_SetDataInput(Flag_e_FALSE, Flag_e_FALSE);
        }
    }
}


void
EnableHistogramInternally(void)
{
    if (HistogramMode_e_IDLE == g_HistStats_Ctrl.e_HistogramMode)
    {
        g_HistStats_Ctrl.e_HistogramMode = HistogramMode_e_ONCE;
    }


    g_IsHistogramEnabledInternally = Flag_e_TRUE;

    return;
}


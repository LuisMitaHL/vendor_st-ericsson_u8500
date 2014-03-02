/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  Glace Glace

 \detail    The Glace module is responsible for control of the Glace statistics and
            exporting these statistics at an appropriate point to the host space.
*/

/**
 \file      Glace.c
 \brief     The main Glace control file. It is a part of the Glace module release.
            It implements the user interface necessary to allow the host to control
            the Glace hardware block.

 \ingroup   Glace
*/
#include "Glace_OPInterface.h"
#include "Glace_IPInterface.h"
#include "Glace_PlatformSpecific.h"
#include "Flash.h"
#include "HostInterface.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#   include "PictorProject_Glace_GlaceTraces.h"
#endif
volatile extern Flag_te g_IsGlaceEnabledInternally;

volatile uint8_t        g_GlaceHistogramStatsFrozen = GlaceHistogramStatus_None;

Glace_Control_ts        g_Glace_Control =
{
    GLACE_DEFAULT_CONTROL_H_BLOCK_SIZE_FRACTION,    // f_HBlockSizeFraction
    GLACE_DEFAULT_CONTROL_V_BLOCK_SIZE_FRACTION,    // f_VBlockSizeFraction
    GLACE_DEFAULT_CONTROL_H_ROI_START_FRACTION,     // f_HROIStartFraction
    GLACE_DEFAULT_CONTROL_V_ROI_START_FRACTION,     // f_VROIStartFraction
    GLACE_DEFAULT_CONTROL_STATS_ADDRESS,            // *ptrGlace_Statistics
    GLACE_DEFAULT_CONTROL_RED_SATURATION_LEVEL,     // u8_RedSaturationLevel
    GLACE_DEFAULT_CONTROL_GREEN_SATURATION_LEVEL,   // u8_GreenSaturationLevel
    GLACE_DEFAULT_CONTROL_BLUE_SATURATION_LEVEL,    // u8_BlueSaturationLevel
    GLACE_DEFAULT_CONTROL_H_GRID_SIZE,              // u8_HGridSize
    GLACE_DEFAULT_CONTROL_V_GRID_SIZE,              // u8_VGridSize
    GLACE_DEFAULT_CONTROL_OPERATION_MODE,           // GlaceOperationMode_e_Control
    GLACE_DEFAULT_CONTROL_DATA_SOURCE,              // e_GlaceDataSource
    GLACE_DEFAULT_CONTROL_PARAM_UPDATE_COUNT,       // u8_ParamUpdateCount
    GLACE_DEFAULT_CONTROL_CONTROL_UPDATE_COUNT,     // u8_ControlUpdateCount_debug
    GLACE_DEFAULT_FOV_MODE                          // e_StatisticsFov
};

Glace_Status_ts         g_Glace_Status =
{
    0,  // u32_GlaceMultiplier
    0,  // u16_HROIStart
    0,  // u16_VROIStart
    0,  // u8_HGridSize
    0,  // u8_VGridSize
    0,  // u8_HBlockSize
    0,  // u8_VBlockSize
    0,  // u8_GlaceShift
    GLACE_DEFAULT_STATUS_OPERATION_MODE,        // e_GlaceOperationMode_Status
    GLACE_DEFAULT_STATUS_PARAM_UPDATE_COUNT,    // u8_ParamUpdateCount
    GlaceExportStatus_e_Idle,                   // e_GlaceExportStatus
    GLACE_DEFAULT_STATUS_CONTROL_UPDATE_COUNT,  // u8_ControlUpdateCount
    GLACE_DEFAULT_STATUS_ENABLE_PENDING,        // e_Flag_GlaceEnablePending
    GLACE_DEFAULT_FOV_MODE                      // e_StatisticsFov
};

uint8_t                 Glace_IsStatsValid (void);
void                    CommitGlaceParams (void);
void                    GlaceExport (void);
void ComputeGlaceParams ( void ) TO_EXT_DDR_PRGM_MEM;
void HandleParamUpdate (uint8_t Flag_e_ForceUpdate) TO_EXT_DDR_PRGM_MEM;
void HandleControlUpdate (uint8_t Flag_e_ForceUpdate) TO_EXT_DDR_PRGM_MEM;

#if 1

/// [AG/PM] Kept for debugging purpose.
/// Interface to allow the module to know if the statistics are to be exported
uint8_t
Glace_IsStatsValid(void)
{
    uint8_t e_Flag = Flag_e_FALSE;

    if (GlaceHistogramStatus_GlaceDone & g_GlaceHistogramStatsFrozen)
    {
        // Glace export completed, no need to do it again.
        OstTraceInt0(TRACE_FLOW, "->Glace_IsStatsValid: Already exported Glace statistics");
        e_Flag = Flag_e_FALSE;
    }
    else
    {   // if any of the following condition is true, export statistics. This will be hit when anything for statistics except e_coin_ctl is called
        if
        (
            (g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug != g_SystemConfig_Status.e_Coin_Glace_Histogram_Status)
        ||  (g_Glace_Status.u8_ControlUpdateCount != g_Glace_Control.u8_ControlUpdateCount_debug)
        ||  (GlaceOperationMode_e_Continuous == g_Glace_Control.e_GlaceOperationMode_Control)
        )
        {
            OstTraceInt0(TRACE_FLOW, "->Glace_IsStatsValid: found coin other than e_coin_ctrl");
            e_Flag = Flag_e_TRUE;
        }
        else
        {
            if (Flash_IsFlashModeActive())  // if flash is active, we need to test both exposure and flash
            {
                if (Flash_IsFlashStatusParamsReceived() && SystemConfig_IsSensorSettingStatusParamsReceived())
                {
                    OstTraceInt0(TRACE_FLOW, "->Glace_IsStatsValid: Flash case");
                    e_Flag = Flag_e_TRUE;
                }
            }
            else                            // Need to check for only exposure
            {
                if (SystemConfig_IsSensorSettingStatusParamsReceived())
                {
                    OstTraceInt0(TRACE_FLOW, "->Glace_IsStatsValid: e_coin_ctrl case");
                    e_Flag = Flag_e_TRUE;
                }
            }
        }
    }


    return (e_Flag);
}


#endif

/**
 \fn        void Glace_Commit ( void )
 \brief     Function to setup the Glace statistics before the
            start of a streaming operation. Must be called only
            when it is safe to access the Glace registers and safe
            to update the Glace parameters e.g. in video complete interrupt

 \param     void
 \return    None
 \ingroup   Glace
 \callgraph
 \callergraph
*/
void
Glace_Commit(void)
{
    // If the device is not streaming, then force update the control and param
    if (!Glace_IsStreaming())
    {
        HandleParamUpdate(Flag_e_TRUE);
        HandleControlUpdate(Flag_e_TRUE);
    }
    else if (g_Glace_Status.e_GlaceExportStatus != GlaceExportStatus_e_Busy)
    {
        // we are streaming, handle those cases only where the host had disabled the Glace
        // and now wants to enable it again i.e. transiting out of disabled state
        HandleParamUpdate(Flag_e_FALSE);
        HandleControlUpdate(Flag_e_FALSE);
    }


    return;
}


/**
 \fn        void Glace_ISR ( void )
 \brief     Function to be called in context of the Glace interrupt
            to copy the Glace statistics to host address space
 \param     void
 \return    None
 \ingroup   Glace
 \callgraph
 \callergraph
*/
void
Glace_ISR(void)
{
    if (Glace_IsStatsValid())
    {
        g_Glace_Status.e_Flag_GlaceEnablePending = Flag_e_FALSE;

        // disable the Glace IP, it is immediate
        Glace_Disable();

        // Disable the input to the Glace IP
        Glace_DisableDataInput();

        g_Glace_Status.e_GlaceExportStatus = GlaceExportStatus_e_Busy;

        // Schedule a Glace export task
        Glace_ScheduleExport();
    }
    else
    {
        g_Glace_Status.e_GlaceExportStatus = GlaceExportStatus_e_Idle;

        // the stats are not to be exported, we can consider any possible update
        // of the Glace params or control
        HandleParamUpdate(Flag_e_FALSE);

        HandleControlUpdate(Flag_e_FALSE);

        EnableGlaceInternally();
        Glace_Commit();
    }


    return;
}


/**
 \fn        void Glace_ExportISR ( void )
 \brief     Function to be called in context of the Glace Export interrupt
            to copy the Glace statistics to host address space
 \param     void
 \return    None
 \ingroup   Glace
 \callgraph
 \callergraph
*/
void
Glace_ExportISR(void)
{
    //Don't export stats if user cancelled the request.
    if (SystemConfig_IsStatsRequestCancelled())
    {
        HandleParamUpdate(Flag_e_FALSE);
        HandleControlUpdate(Flag_e_FALSE);

        return;
    }


    // Export Glace Stats
    GlaceExport();

    // If g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug has been togglesd, mark a flag that glace
    // statistics have been frozen. use this flag for sending notification to the host. This is to be
    // done in main loop. similar notification coming from histogram module will also be taken into account.
    // If If g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug has not been toggled, send glace notification
    // straight away.
    if
    (
        (Flash_IsFlashStatusParamsReceived())
    ||  (g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug != g_SystemConfig_Status.e_Coin_Glace_Histogram_Status)
    ||  (SystemConfig_IsSensorSettingStatusParamsReceived())
    )
    {
        // memorize that glace statistics have been frozen. Shall be recalled in main loop.
        g_GlaceHistogramStatsFrozen |= GlaceHistogramStatus_GlaceDone;
    }
    else
    {
        if
        (
            (g_Glace_Status.u8_ControlUpdateCount != g_Glace_Control.u8_ControlUpdateCount_debug)
        ||  (GlaceOperationMode_e_Continuous == g_Glace_Control.e_GlaceOperationMode_Control)
        )
        {
            g_Glace_Status.u8_ControlUpdateCount = g_Glace_Control.u8_ControlUpdateCount_debug;
            g_Glace_Status.e_GlaceExportStatus = GlaceExportStatus_e_Idle;

            //Update remaining Frameparamstatus PEs which were not updated in ISL context
            g_FrameParamStatus.u32_frame_counter = g_RxFrameCounter;

            //export FrameStatus: it is expected HOST will read u32_StatsInvalid before trying to read stats
            FrameStatusExport();

            // Inform the host that the Glace statistics have been exported
            Glace_StatsReadyNotify();

            EnableGlaceInternally();
            Glace_Commit();
        }
    }


    HandleParamUpdate(Flag_e_FALSE);

    HandleControlUpdate(Flag_e_FALSE);

    return;
}


/**
 \if        INCLUDE_IN_HTML_ONLY
 \fn        void GlaceExport ( void )
 \brief     Function to export the Glace statistics into the
            host address space
 \param     void
 \return    None
 \ingroup   Glace
 \callgraph
 \callergraph
 \endif
*/
void
GlaceExport(void)
{
    uint16_t            u16_WordCount;
    uint16_t            u16_Index;
    uint16_t            u16_TargetIndex;
    uint32_t            *pu32_GlaceMem;
    uint32_t            *pu32_TargetRed;
    uint32_t            *pu32_TargetGreen;
    uint32_t            *pu32_TargetBlue;
    uint32_t            *pu32_TargetSaturation;

    uint32_t            u32_Mean0;
    uint32_t            u32_Mean1;
    uint32_t            u32_Mean2;
    uint32_t            u32_Mean3;

    uint32_t            u32_Saturation0;
    uint32_t            u32_Saturation1;
    uint32_t            u32_Saturation2;
    uint32_t            u32_Saturation3;

    uint32_t            u32_Byte0;
    uint32_t            u32_Byte1;
    uint32_t            u32_Byte2;
    uint32_t            u32_Byte3;
    uint32_t            u32_Value;
    uint32_t            u32_MaxSaturatedPixels;

    Glace_Statistics_ts *ptrGlace_Statistics;

    OstTraceInt0(TRACE_FLOW, "->GlaceExport(void)");

    u16_WordCount = (g_Glace_Status.u8_HGridSize * g_Glace_Status.u8_VGridSize) * 2;
    pu32_GlaceMem = ( uint32_t * ) GLACE_MEM_BASE;
    ptrGlace_Statistics = ( Glace_Statistics_ts * ) g_Glace_Control.ptrGlace_Statistics;

    if (0 != ptrGlace_Statistics)
    {
        pu32_TargetRed = (uint32_t *) (ptrGlace_Statistics->u32_TargetRedAddress);
        pu32_TargetGreen = (uint32_t *) (ptrGlace_Statistics->u32_TargetGreenAddress);
        pu32_TargetBlue = (uint32_t *) (ptrGlace_Statistics->u32_TargetBlueAddress);
        pu32_TargetSaturation = (uint32_t *) (ptrGlace_Statistics->u32_TargetSaturationAddress);
        u16_TargetIndex = 0;

        for (u16_Index = 0; u16_Index < u16_WordCount; u16_Index += 8)
        {
            u32_Mean0 = pu32_GlaceMem[u16_Index + 0];
            u32_Mean1 = pu32_GlaceMem[u16_Index + 2];
            u32_Mean2 = pu32_GlaceMem[u16_Index + 4];
            u32_Mean3 = pu32_GlaceMem[u16_Index + 6];

            u32_Saturation0 = pu32_GlaceMem[u16_Index + 1];
            u32_Saturation1 = pu32_GlaceMem[u16_Index + 3];
            u32_Saturation2 = pu32_GlaceMem[u16_Index + 5];
            u32_Saturation3 = pu32_GlaceMem[u16_Index + 7];

            // Mean Blue
            u32_Byte0 = ((u32_Mean0 << 0) & 0x000000FF);
            u32_Byte1 = ((u32_Mean1 << 8) & 0x0000FF00);
            u32_Byte2 = ((u32_Mean2 << 16) & 0x00FF0000);
            u32_Byte3 = ((u32_Mean3 << 24) & 0xFF000000);
            u32_Value = (uint32_t) (u32_Byte0 | u32_Byte1 | u32_Byte2 | u32_Byte3);
            pu32_TargetBlue[u16_TargetIndex] = u32_Value;

            // Mean Green
            u32_Byte0 = ((u32_Mean0 >> 8) & 0x000000FF);
            u32_Byte1 = ((u32_Mean1 << 0) & 0x0000FF00);
            u32_Byte2 = ((u32_Mean2 << 8) & 0x00FF0000);
            u32_Byte3 = ((u32_Mean3 << 16) & 0xFF000000);
            u32_Value = (uint32_t) (u32_Byte0 | u32_Byte1 | u32_Byte2 | u32_Byte3);
            pu32_TargetGreen[u16_TargetIndex] = u32_Value;

            // Mean Red
            u32_Byte0 = ((u32_Mean0 >> 16) & 0x000000FF);
            u32_Byte1 = ((u32_Mean1 >> 8) & 0x0000FF00);
            u32_Byte2 = ((u32_Mean2 << 0) & 0x00FF0000);
            u32_Byte3 = ((u32_Mean3 << 8) & 0xFF000000);
            u32_Value = (uint32_t) (u32_Byte0 | u32_Byte1 | u32_Byte2 | u32_Byte3);
            pu32_TargetRed[u16_TargetIndex] = u32_Value;

            // Saturated
            // the saturated pixel value must be converted to 8 bit domain before exporting.
            // this is done by using a multiplier and shift such that:
            //
            // saturated_8 = (saturated_12 * multiplier) >> shift
            //
            // normally the multiplier and shift will be chosen such that
            // saturated_8 = (saturated_12 * 255)/(hblock_size * vblock_size)
            //
            // The above logic maps the range [0..(hblock_size * vblock_size)] to [0..255]
            //
            //
            u32_MaxSaturatedPixels = g_Glace_Status.u8_HBlockSize * g_Glace_Status.u8_VBlockSize;
            u32_Value = (u32_Saturation0 * 255) / u32_MaxSaturatedPixels;
            u32_Byte0 = Glace_Min(255, u32_Value);

            u32_Value = (u32_Saturation1 * 255) / u32_MaxSaturatedPixels;
            u32_Byte1 = Glace_Min(255, u32_Value);
            u32_Byte1 = u32_Byte1 << 8;

            u32_Value = (u32_Saturation2 * 255) / u32_MaxSaturatedPixels;
            u32_Byte2 = Glace_Min(255, u32_Value);
            u32_Byte2 = u32_Byte2 << 16;

            u32_Value = (u32_Saturation3 * 255) / u32_MaxSaturatedPixels;
            u32_Byte3 = Glace_Min(255, u32_Value);
            u32_Byte3 = u32_Byte3 << 24;

            u32_Value = (uint32_t) (u32_Byte0 | u32_Byte1 | u32_Byte2 | u32_Byte3);
            pu32_TargetSaturation[u16_TargetIndex] = u32_Value;

            u16_TargetIndex += 1;
        }


        // u32_HGridSize
        ptrGlace_Statistics->u32_HGridSize = g_Glace_Status.u8_HGridSize;

        // u32_VGridSize
        ptrGlace_Statistics->u32_VGridSize = g_Glace_Status.u8_VGridSize;
    }


    OstTraceInt0(TRACE_FLOW, "<-GlaceExport(void)");
    return;
}


/**
 \if        INCLUDE_IN_HTML_ONLY
 \fn        void ComputeGlaceParams ( void )
 \brief     Function to translate the host inputs and generate
            Glace registers parameters
 \param     void
 \return    None
 \ingroup   Glace
 \callgraph
 \callergraph
 \endif
*/
void
ComputeGlaceParams(void)
{
    uint8_t     u8_HGridSize;
    uint8_t     u8_VGridSize;
    float_t     f_HBlockSizeFraction;
    float_t     f_VBlockSizeFraction;
    float_t     f_HROIStartFraction;
    float_t     f_VROIStartFraction;
    uint16_t    u16_GlaceInputSizeX;
    uint16_t    u16_GlaceInputSizeY;
    uint8_t     u8_HBlockSize;
    uint8_t     u8_VBlockSize;
    uint16_t    u16_HROIStart;
    uint16_t    u16_VROIStart;

    uint32_t    u32_MaxMultiplier;
    uint32_t    u32_MaxShift;
    uint8_t     u8_Shift;
    uint32_t    u32_Multiplier;
    uint32_t    u32_Value;
    float_t     f_Value;
    uint8_t     u8_forcedSensorFOV = Flag_e_FALSE;

    float_t     f_InvPreScale = 1.0;

#if ENABLE_GLACE_TRACES
    OstTraceInt0(TRACE_DEBUG, "\n <glace> >> ComputeGlaceParams\n");
    if (SystemConfig_IsBayerStore2Active())
    {
        OstTraceInt0(TRACE_DEBUG, "\n <glace> glace mode : BMS2\n");
    }


    if (SystemConfig_IsPipe0Active())
    {
        OstTraceInt0(TRACE_DEBUG, "\n <glace> glace mode : HR\n");
    }


    if (SystemConfig_IsPipe1Active())
    {
        OstTraceInt0(TRACE_DEBUG, "\n <glace> glace mode : LR\n");
    }
#endif

    u8_HGridSize = g_Glace_Control.u8_HGridSize & 0xFE;     // force u8_HGridSize to be even
    u8_VGridSize = g_Glace_Control.u8_VGridSize & 0xFE;     // force u8_VGridSize to be even

    f_HBlockSizeFraction = Glace_Min(1.0, g_Glace_Control.f_HBlockSizeFraction);
    f_VBlockSizeFraction = Glace_Min(1.0, g_Glace_Control.f_VBlockSizeFraction);
    f_HROIStartFraction = Glace_Min(1.0, g_Glace_Control.f_HROIStartFraction);
    f_VROIStartFraction = Glace_Min(1.0, g_Glace_Control.f_VROIStartFraction);

    /// sum of f_HBlockSizeFraction and f_HROIStartFraction should not be more than 1
    if (f_HBlockSizeFraction + f_HROIStartFraction > 1.0)   // to be changed to prioritse size
    {
        /// <Hem> As discussed with Atul, f_HBlockSizeFraction should not be changed in this case
        f_HROIStartFraction = 1.0 - f_HBlockSizeFraction;
    }

    /// <Hem> sum of f_VBlockSizeFraction and f_VROIStartFraction should not be more than 1
    if (f_VBlockSizeFraction + f_VROIStartFraction > 1.0)
    {
        // As discussed with Atul, f_VBlockSizeFraction should not be changed in this case
        f_VROIStartFraction = 1.0 - f_VBlockSizeFraction;
    }

#if ENABLE_GLACE_TRACES
    OstTraceInt2(TRACE_DEBUG, "\n <glace>  g_Glace_Control.f_HROIStartFraction = %f, g_Glace_Control.f_VROIStartFraction = %f\n", g_Glace_Control.f_HROIStartFraction, g_Glace_Control.f_VROIStartFraction);
    OstTraceInt2(TRACE_DEBUG, "\n <glace>  u8_HGridSize = %u, u8_VGridSize = %u\n", u8_HGridSize, u8_VGridSize);
    OstTraceInt2(TRACE_DEBUG, "\n <glace>  f_HBlockSizeFraction = %f, f_VBlockSizeFraction = %f\n", f_HBlockSizeFraction, f_VBlockSizeFraction);
    OstTraceInt2(TRACE_DEBUG, "\n <glace>  f_HROIStartFraction = %f, f_VROIStartFraction = %f\n", f_HROIStartFraction, f_VROIStartFraction);
    OstTraceInt1(TRACE_DEBUG, "\n <glace>  g_Glace_Status.e_StatisticsFov = %u\n", g_Glace_Status.e_StatisticsFov);
    OstTraceInt1(TRACE_DEBUG, "\n <histogram>  u8_forcedSensorFOV = %u\n", u8_forcedSensorFOV);
#endif

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


    if ((StatisticsFov_e_Sensor == g_Glace_Status.e_StatisticsFov) || (Flag_e_TRUE == u8_forcedSensorFOV))
    {
        u16_GlaceInputSizeX = Glace_XInputSize();
        u16_GlaceInputSizeY = Glace_YInputSize();

        u16_HROIStart = (uint16_t) (f_HROIStartFraction * u16_GlaceInputSizeX);
        u16_VROIStart = (uint16_t) (f_VROIStartFraction * u16_GlaceInputSizeY);
    }
    else
    {
        u16_GlaceInputSizeX = Glace_Get_FOV_X();
        u16_GlaceInputSizeY = Glace_Get_FOV_Y();

        // take into account any prescaling applied in the sensor
        f_InvPreScale = 1.0 / Glace_GetPreScale();

        u16_GlaceInputSizeX = (uint16_t) (u16_GlaceInputSizeX * f_InvPreScale);
        u16_GlaceInputSizeY = (uint16_t) (u16_GlaceInputSizeY * f_InvPreScale);

        u16_HROIStart = Glace_GetFOV_OffsetX() + f_HROIStartFraction * u16_GlaceInputSizeX;
        u16_VROIStart = Glace_GetFOV_OffsetY() + f_VROIStartFraction * u16_GlaceInputSizeY;
    }


    u8_HBlockSize = (uint8_t) ((f_HBlockSizeFraction * u16_GlaceInputSizeX) / u8_HGridSize);
    u8_VBlockSize = (uint8_t) ((f_VBlockSizeFraction * u16_GlaceInputSizeY) / u8_VGridSize);

    // enforce u8_HBlockSize and u8_VBlockSize to be even numbers
    u8_HBlockSize = u8_HBlockSize & 0xFE;
    u8_VBlockSize = u8_VBlockSize & 0xFE;

    // enforce u8_HBlockSize and u8_VBlockSize to be even numbers
    u16_HROIStart = u16_HROIStart & 0xFFFE;
    u16_VROIStart = u16_VROIStart & 0xFFFE;

#if ENABLE_GLACE_TRACES
    OstTraceInt2(TRACE_DEBUG, "\n <glace>  u16_GlaceInputSizeX = %u, u16_GlaceInputSizeY = %u\n", u16_GlaceInputSizeX, u16_GlaceInputSizeY);
    OstTraceInt2(TRACE_DEBUG, "\n <glace>  u8_HBlockSize = %u, u8_VBlockSize = %u\n", u8_HBlockSize, u8_VBlockSize);
    OstTraceInt2(TRACE_DEBUG, "\n <glace>  u16_HROIStart = %u, u16_VROIStart = %u\n", u16_HROIStart, u16_VROIStart);
#endif

    // now compute the shift and the multiplier values
    // idea is to represent 1/[(u8_HBlockSize * u8_VBlockSize)/4]
    // as u32_Multiplier/(1 << u8_Shift)
    // ==> 4/(u8_HBlockSize * u8_VBlockSize) == u32_Multiplier/(1 << u8_Shift)
    u32_MaxMultiplier = (uint32_t) ((1 << Glace_GetMultiplierWidth()) - 1);
    u32_MaxShift = (uint32_t) ((1 << Glace_GetShiftWidth()) - 1);

    // assume that we can use the maximum possible multiplier
    f_Value = ((float_t) (u8_HBlockSize * u8_VBlockSize) / 4.0) * u32_MaxMultiplier;
    u32_Value = Glace_Ceiling(f_Value);
    u8_Shift = Glace_Log(u32_Value, 2);

    u8_Shift = Glace_Min(u32_MaxShift, u8_Shift);

    u32_Value = (1 << u8_Shift);

    // if the math was 100% accurate then:
    // u32_Multiplier/(1 << u8_Shift) == 4/(u8_HBlockSize * u8_VBlockSize)
    // however, it must be ensured that
    // u32_Multiplier/(1 << u8_Shift) <= 4/(u8_HBlockSize * u8_VBlockSize)
    // else an overflow in statistics can happen
    // since u32_Value == (1 << u8_Shift)
    // ==> u32_Multiplier <= (uint32_t)((u32_Value * 4)/(u8_HBlockSize * u8_VBlockSize))
    u32_Multiplier = (uint32_t) ((u32_Value * 4) / (u8_HBlockSize * u8_VBlockSize));

    // populate the status page
    g_Glace_Status.u32_GlaceMultiplier = u32_Multiplier;
    g_Glace_Status.u8_HGridSize = u8_HGridSize;
    g_Glace_Status.u8_VGridSize = u8_VGridSize;
    g_Glace_Status.u16_HROIStart = u16_HROIStart;
    g_Glace_Status.u16_VROIStart = u16_VROIStart;
    g_Glace_Status.u8_HBlockSize = u8_HBlockSize;
    g_Glace_Status.u8_VBlockSize = u8_VBlockSize;
    g_Glace_Status.u8_GlaceShift = u8_Shift;

#if ENABLE_GLACE_TRACES
    OstTraceInt0(TRACE_DEBUG, "\n <glace> << ComputeGlaceParams\n");
#endif
    return;
}


/**
 \if        INCLUDE_IN_HTML_ONLY
 \fn        void CommitGlaceParams ( void )
 \brief     Function to program the Glace hardware registers
 \param     void
 \return    None
 \ingroup   Glace
 \callgraph
 \callergraph
 \endif
*/
void
CommitGlaceParams(void)
{
    Glace_Set_H_ROI_START(g_Glace_Status.u16_HROIStart);            // ISP_STATS_GLACE_H_ROI_START
    Glace_Set_V_ROI_START(g_Glace_Status.u16_VROIStart);            // ISP_STATS_GLACE_V_ROI_START
    Glace_Set_H_BLK_SIZE(g_Glace_Status.u8_HBlockSize);             // ISP_STATS_GLACE_H_BLK_SIZE
    Glace_Set_V_BLK_SIZE(g_Glace_Status.u8_VBlockSize);             // ISP_STATS_GLACE_V_BLK_SIZE
    Glace_Set_H_GRID_SIZE(g_Glace_Status.u8_HGridSize);             // ISP_STATS_GLACE_H_GRID_SIZE
    Glace_Set_V_GRID_SIZE(g_Glace_Status.u8_VGridSize);             // ISP_STATS_GLACE_V_GRID_SIZE
    Glace_Set_SAT_LEVEL_R(g_Glace_Control.u8_RedSaturationLevel);   // ISP_STATS_GLACE_SAT_LEVEL_R
    Glace_Set_SAT_LEVEL_G(g_Glace_Control.u8_GreenSaturationLevel); // ISP_STATS_GLACE_SAT_LEVEL_G
    Glace_Set_SAT_LEVEL_B(g_Glace_Control.u8_BlueSaturationLevel);  // ISP_STATS_GLACE_SAT_LEVEL_B
    Glace_Set_AV_MULT(g_Glace_Status.u32_GlaceMultiplier);          // ISP_STATS_GLACE_AV_MULT
    Glace_Set_AV_SHIFT(g_Glace_Status.u8_GlaceShift);               // ISP_STATS_GLACE_AV_SHIFT
    if (GlaceDataSource_e_PostLensShading == g_Glace_Control.e_GlaceDataSource)
    {
        Glace_Set_MUX_GLACE_SELECT_POST_LENS_SHADING();             // Set_ISP_STATS_MUX_GLACE_ISP_STATS_MUX_GLACE_SELECT
    }
    else
    {
        Glace_Set_MUX_GLACE_SELECT_POST_CHANNEL_GAINS();            // Set_ISP_STATS_MUX_GLACE_ISP_STATS_MUX_GLACE_SELECT
    }


    if (Is_8500v2() || Is_9540v1() || Is_8540v1())
    {
        Glace_Enable_MPSS();
    }


    return;
}


/**
 \if        INCLUDE_IN_HTML_ONLY
 \fn        void HandleParamUpdate( uint8_t Flag_e_ForceUpdate )
 \brief     Function to consider host driven changes to the glace parameters
 \param     Flag_e_ForceUpdate : If set to Flag_e_TRUE, then the params are updated regardless
                                 of the host control. Used for setup for start streaming.
                                 If set to Flag_e_FALSE, then the params are updated based on
                                 the value of the g_Glace_Control.u8_ParamUpdateCount and g_Glace_Status.u8_ParamUpdateCount
 \return    None
 \ingroup   Glace
 \callgraph
 \callergraph
 \endif
*/
void
HandleParamUpdate(
uint8_t Flag_e_ForceUpdate)
{
    uint8_t u8_ParamUpdateCount;

    u8_ParamUpdateCount = g_Glace_Control.u8_ParamUpdateCount;

    if
    (
        (Flag_e_TRUE == Flag_e_ForceUpdate)
    ||  (u8_ParamUpdateCount != g_Glace_Status.u8_ParamUpdateCount)
    ||  (Flag_e_TRUE == g_IsGlaceEnabledInternally)
    )
    {
        ComputeGlaceParams();

        CommitGlaceParams();

        g_Glace_Status.u8_ParamUpdateCount = u8_ParamUpdateCount;
    }


    return;
}


/**
 \if        INCLUDE_IN_HTML_ONLY
 \fn        void HandleControlUpdate( uint8_t Flag_e_ForceUpdate )
 \brief     Function to consider host driven changes to the glace control
 \param     Flag_e_ForceUpdate : If set to Flag_e_TRUE, then the controls are updated regardless
                                 of the host control. Used for setup for start streaming.
                                 If set to Flag_e_FALSE, then the params are updated based on
                                 the value of the g_Glace_Control.e_GlaceOperationMode_Control
                                 and g_Glace_Status.e_GlaceOperationMode_Status
 \return    None
 \ingroup   Glace
 \callgraph
 \callergraph
 \endif
*/
void
HandleControlUpdate(
uint8_t Flag_e_ForceUpdate)
{
    uint8_t e_GlaceOperationMode_Control;
    uint8_t e_GlaceOperationMode_Status;
    uint8_t u8_ControlUpdateCount;

    e_GlaceOperationMode_Control = g_Glace_Control.e_GlaceOperationMode_Control;
    e_GlaceOperationMode_Status = g_Glace_Status.e_GlaceOperationMode_Status;
    u8_ControlUpdateCount = g_Glace_Control.u8_ControlUpdateCount_debug;

    // update glace FOV mode
    g_Glace_Status.e_StatisticsFov = g_Glace_Control.e_StatisticsFov;

    if
    (
        (Flag_e_TRUE == Flag_e_ForceUpdate)
    ||  (u8_ControlUpdateCount != g_Glace_Status.u8_ControlUpdateCount)
    ||  (Flag_e_TRUE == g_IsGlaceEnabledInternally)
    )
    {
        if
        (
            (GlaceOperationMode_e_Continuous == e_GlaceOperationMode_Control)
        ||  (GlaceOperationMode_e_Once == e_GlaceOperationMode_Control)
        )
        {
            // Reset the Glace IP
            Glace_Reset();

            // Enable Glace
            Glace_Enable();

            // Enable the input to the Glace IP
            Glace_EnableDataInput();

            g_Glace_Status.e_Flag_GlaceEnablePending = Flag_e_TRUE;
        }
        else
        {
            // disable the Glace IP, it is immediate
            Glace_Disable();

            // Disable the input to the Glace IP
            Glace_DisableDataInput();
        }


        g_Glace_Status.e_GlaceOperationMode_Status = e_GlaceOperationMode_Control;
        g_IsGlaceEnabledInternally = Flag_e_FALSE;
    }
    else
    {
        // no control update was done
        // however, if the module has been in continuous mode, then we need to enable it
        if
        (
            (GlaceOperationMode_e_Continuous == e_GlaceOperationMode_Status)
        &&  (Flag_e_FALSE == g_Glace_Status.e_Flag_GlaceEnablePending)
        )
        {
            // Reset the Glace IP
            Glace_Reset();

            // Enable Glace
            Glace_Enable();

            // Enable the input to the Glace IP
            Glace_EnableDataInput();

            g_Glace_Status.e_Flag_GlaceEnablePending = Flag_e_TRUE;
        }
    }


    return;
}


void
EnableGlaceInternally(void)
{
    if (GlaceOperationMode_e_Disable == g_Glace_Control.e_GlaceOperationMode_Control)
    {
        g_Glace_Control.e_GlaceOperationMode_Control = GlaceOperationMode_e_Once;
    }


    g_IsGlaceEnabledInternally = Flag_e_TRUE;

    return;
}


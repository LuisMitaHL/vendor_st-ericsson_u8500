/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\file FrameDimension.c
\brief Defines functions required for Frame Dimension module
\ingroup Frame Dimension
*/
#include "FrameDimension.h"
#include "FrameDimensionPlatformSpecific.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_FrameDimension_FrameDimensionTraces.h"
#endif

#include "run_mode_ctrl.h"

/// Local function definitions\n
float_t                 ComputeSubSamplingFactor (uint8_t u8_SubSamplingFactor);
uint16_t                ComputeRegionOfInterest (float_t f_ReductionFactor, uint16_t u16_RequestedOPOutputSize);
void                    ComputeVTFrameTime (void);
uint16_t                ComputeImpliedMinimumLineLength (uint16_t u16_VTXOutputSize);
uint16_t                ComputeImpliedMinimumFrameLength (uint16_t u16_VTYOutputSize);
void                    AdjustReadoutCoordinates(uint16_t *pu16_Start, uint16_t *pu16_End, uint16_t u16_Min, uint16_t u16_Max, uint16_t u16_RegionOfInterest);

FrameDimension_ts   g_CurrentFrameDimension =
{
    FDM_DEFAULT_PRE_SCALE_FACTOR,   // f_PreScaleFactor
    FDM_DEFAULT_FRAME_LENGTH_LINES, // u16_VTFrameLengthLines
    FDM_DEFAULT_LINE_LENGTH_PCK,    // u16_VTLineLengthPck
    FDM_DEFAULT_X_ADDR_START,   // u16_VTXAddrStart
    FDM_DEFAULT_Y_ADDR_START,   // u16_VTYAddrStart
    FDM_DEFAULT_X_ADDR_END, // u16_VTXAddrEnd
    FDM_DEFAULT_Y_ADDR_END, // u16_VTYAddrEnd
    FDM_DEFAULT_OP_X_OUTPUT_SIZE,   // u16_OPXOutputSize
    FDM_DEFAULT_OP_Y_OUTPUT_SIZE,   // u16_OPYOutputSize
    FDM_DEFAULT_VT_X_OUTPUT_SIZE,   // u16_VTXOutputSize
    FDM_DEFAULT_VT_Y_OUTPUT_SIZE,   // u16_VTYOutputSize
    FDM_DEFAULT_X_ODD_INC,  // u16_XOddInc
    FDM_DEFAULT_Y_ODD_INC,  // u16_YOddInc
    FDM_DEFAULT_SCALAR_M,   // u16_Scaler_M
    FDM_DEFAULT_NON_ACTIVE_COLUMNS_LEFT_EDGE,   // u16_NumberOfNonActiveColumnsAtTheLeftEdge
    FDM_DEFAULT_NON_ACTIVE_COLUMNS_RIGHT_EDGE,  // u16_NumberOfNonActiveColumnsAtTheRightEdge
    FDM_DEFAULT_NON_ACTIVE_ROWS_TOP_EDGE,   // u16_NumberofNonActiveLinesAtTopEdge
    FDM_DEFAULT_NON_ACTIVE_ROWS_BOTTOM_EDGE,    // u16_NumberofNonActiveLinesAtBottomEdge
    FDM_DEFAULT_STATUS_LINES,   // u8_NumberOfStatusLines
    FDM_DEFAULT_SENSOR_PRESCALE_TYPE,   // e_SensorPrescaleType
    FDM_DEFAULT_SENSOR_SCALING_MODE,    // e_FDSensorScalingMode
};

FrameDimension_ts       g_RequestedFrameDimension;

FrameConstraints_ts     g_SensorFrameConstraints;

FrameDimensionStatus_ts g_FrameDimensionStatus =
{
    0.0,            // f_VTLineLength_us
    0.0,            // f_VTFrameLength_us
    0.0,            // f_CurrentFrameRate
    0,              // u16_VTFrameLengthPending_Lines
    0,              // u16_MinVTLineLengthAtCurrentVTXSize_Pixels
    0,              // u16_MinVTFrameLengthAtCurrentVTYSize_Lines
    0,              // u16_MaximumUsableSensorFOVX
    0,              // u16_MaximumUsableSensorFOVY
    0,              // u8_TotalFOVXShift
    0,              // u8_TotalFOVYShift
    0,              // u8_FOVXShiftInSensor
    0,              // u8_FOVYShiftInSensor
    Flag_e_FALSE,   // e_Flag_IsFrameLengthChangePending
    Flag_e_FALSE    // e_Flag_IsFrameLengthChangeInhibitedForCoarseExposure
};


HostFrameConstraints_ts g_HostFrameConstraints =
{
    FRAME_DIMENSION_DEFAULT_FOV_MARGIN,                             // f_FOVMargin
    FRAME_DIMENSION_DEFAULT_PIPE0_MINIMUM_INTERLINE_BEYOND_SCALAR,  // u8_MinimumPostScalar0LineBlanking_pixels
    FRAME_DIMENSION_DEFAULT_PIPE1_MINIMUM_INTERLINE_BEYOND_SCALAR,  // u8_MinimumPostScalar1LineBlanking_pixels
    FRAME_DIMENSION_DEFAULT_MINIMUM_INTERFRAME_LINES,               // u8_MinimumInterFrame_lines
    FRAME_DIMENSION_DEFAULT_MAXIMUM_PRESCALE,                       // u8_MaximumPreScale
    FRAME_DIMENSION_DEFAULT_MAINS_FREQUENCY_HZ,                     // u8_MainsFrequency_Hz
    FRAME_DIMENSION_DEFAULT_GAURANTEE_STATIC_FLICKER_FRAME_LENGTH,  // e_Flag_GuaranteeStaticFlickerFrameLength
    FRAME_DIMENSION_DEFAULT_CURRENT_FRAME_DIM_PROGRAMMING_MODE,     // e_FrameDimensionProgMode
    FRAME_DIMENSION_DEFAULT_ALLOW_CHANGE_OVER                       // e_Flag_AllowChangeOver
};

AntiFlicker_Status_ts   g_AntiFlicker_Status;

BML_Framedimension_ts   g_BML_Framedimension = {0};


/**
\if INCLUDE_IN_HTML_ONLY
\fn float_t ComputeSubSamplingFactor( uint8_t   u8_SubSamplingFactor )
\brief The function computes the closest possible match for given sub-sampling
factor that is achievable by the sensor.
\param u8_SubSamplingFactor : The sub sampling factor corresponding to which the system achievable sub sampling factor is computed.
\return The closest possible sub sampling factor corresponding to u8_SubSamplingFactor
\ingroup Frame Dimension
\callgraph
\callergraph
\endif
*/
float_t
ComputeSubSamplingFactor(
uint8_t u8_SubSamplingFactor)
{
    /*~~~~~~~~~~~~~~~*/
    uint8_t u8_XOddInc;

    /*~~~~~~~~~~~~~~~*/

    /// x_even_inc = 1.\n
    /// x_sub_sampling_factor = (x_even_inc + x_odd_inc)/2.\n
    /// ==> x_odd_inc = (2 * x_sub_sampling_factor) - x_even_inc.\n
    u8_XOddInc = (u8_SubSamplingFactor << 1) - 1;

    /// Ensure u8_XOddInc is less than max_x_odd_inc.\n
    u8_XOddInc = FrameDimension_Min(u8_XOddInc, g_SensorFrameConstraints.u16_MaxOddInc);

    /// Compute Sub sampling factor (x_even_inc + x_odd_inc)/2.\n
    return ((float_t) ((u8_XOddInc + 1) >> 1));
}


/**
\if INCLUDE_IN_HTML_ONLY
\fn uint16_t ComputeRegionOfInterest( float_t f_ReductionFactor, uint16_t   u16_RequestedOPOutputSize )
\brief The function computes the VT region of interest corresponding to given reduction factor and output size
\param f_ReductionFactor : The data reduction factor.
\param u16_RequestedOPOutputSize : Target output size.
\return The region of interest.
\ingroup Frame Dimension
\callgraph
\callergraph
\endif
*/
uint16_t
ComputeRegionOfInterest(
float_t     f_ReductionFactor,
uint16_t    u16_RequestedOPOutputSize)
{
    return ((uint16_t) ((float_t) (u16_RequestedOPOutputSize) * f_ReductionFactor));
}


/**
\if INCLUDE_IN_HTML_ONLY
\fn void ComputeVTFrameTime( void )
\brief This function computes the VT frame time in us. It assumes that the VT line time
is known when it is invoked.
\param None
\return Nothing
\ingroup Frame Dimension
\callgraph
\callergraph
\endif
*/
void
ComputeVTFrameTime(void)
{
    /// Multiply the line time with Frame length pending lines to get the frame time..\n
    g_FrameDimensionStatus.f_VTFrameLength_us =
        (
            (float_t) (g_FrameDimensionStatus.u16_VTFrameLengthPending_Lines) *
            g_FrameDimensionStatus.f_VTLineLength_us
        );

    /// Compute the frame rate.\n
    /// Divide the frame time by 1000000 to convert it into seconds.\n
    g_FrameDimensionStatus.f_CurrentFrameRate = (1.0 / (g_FrameDimensionStatus.f_VTFrameLength_us / 1000000));
    g_VariableFrameRateStatus.f_CurrentFrameRate_Hz = g_FrameDimensionStatus.f_CurrentFrameRate;

    return;
}


/**
\if INCLUDE_IN_HTML_ONLY
\fn uint16_t ComputeImpliedMinimumLineLength( uint16_t u16_VTXOutputSize )
\brief This function computes the implied minimum VT line length corresponding
to the given VT X output size
\param u16_VTXOutputSize : The video timing X output size.
\return The implied minimum VT line length.
\ingroup Frame Dimension
\callgraph
\callergraph
\endif
*/
uint16_t
ComputeImpliedMinimumLineLength(
uint16_t    u16_VTXOutputSize)
{
    /*~~~~~~~~~~~~~~~~~~*/
    uint16_t    u16_Value;

    /*~~~~~~~~~~~~~~~~~~*/

    /// Compute the implied minimum video timing line length.\n
    /// Implied_min_video_timing_line_length = VTXOutputSize + additional_cols + minimum VT line blanking pixel clocks.\n
    /// min_vt_line_blanking_pck = sav_code_pck + eav_code_pck + checksum_pck + min_line_blanking_pck(from g_SensorFrameConstraints.u16_MinVTLineBlankingPck).\n
    /// sav_code_pck => Start of active video code pixel clocks.\n
    /// eav_code_pck => End of active video code pixel clocks.\n
    u16_Value = FD_CEILING_OF_M_BY_N(SYNC_CODE_WIDTH_BITS, (uint8_t) FD_GET_CCP_RAW_FORMAT()) + FD_CEILING_OF_M_BY_N(
        (SYNC_CODE_WIDTH_BITS + CHECKSUM_WIDTH_BITS),
        (uint8_t) FD_GET_CCP_RAW_FORMAT()) + g_SensorFrameConstraints.u16_MinVTLineBlankingPck;

    u16_Value += u16_VTXOutputSize + FrameDimension_GetSensorConstantCols();

    /// The implied minimum line length must be bigger than minimum VT line length.\n
    u16_Value = max(u16_Value, g_SensorFrameConstraints.u16_MinVTLineLengthPck);

    return (u16_Value);
}


/*
************************************************************************************************
\if INCLUDE_IN_HTML_ONLY
\fn uint16_t ComputeImpliedMinimumFrameLength( uint16_t u16_VTYOutputSize )
\brief This function computes the implied minimum vt frame length
corresponding to the given vt y output size.
\param u16_VTYOutputSize : The video timing y output size.
\return The implied minimum VT frame length.
\ingroup Frame Dimension
\callgraph
\callergraph
\endif
************************************************************************************************
*/
uint16_t
ComputeImpliedMinimumFrameLength(
uint16_t    u16_VTYOutputSize)
{
    uint16_t    u16_MinimumFrameLength;
    /// Compute the implied minimum frame_length.\n
    /// Implied_minimum_frame_length = u16_VTYOutputSize + additional_rows + min(g_SensorFrameConstraints.u16_MinVTFrameBlanking, g_HostFrameConstraints.u8_MinimumInterFrame_lines)

    u16_MinimumFrameLength = FrameDimension_Max(g_SensorFrameConstraints.u16_MinVTFrameBlanking, g_HostFrameConstraints.u8_MinimumInterFrame_lines);
    return (u16_VTYOutputSize + FrameDimension_GetSensorConstantRows() + u16_MinimumFrameLength);
}

void AdjustReadoutCoordinates(uint16_t *pu16_Start, uint16_t *pu16_End, uint16_t u16_Min, uint16_t u16_Max, uint16_t u16_RegionOfInterest)
{
    uint16_t    u16_Start;
    uint16_t    u16_End;
    uint16_t    u16_CurrentSize;
    int16_t     s16_Difference;
    uint16_t    u16_Edge0Margin;
    int16_t     s16_Edge0Adjust;

    u16_Start = *pu16_Start;
    u16_End = *pu16_End;

    u16_CurrentSize = (u16_End - u16_Start + 1);

    // the adjustment of readout coordinates needs to be done
    // only if the current readout is less than the region of inerest
/*
    if (u16_RegionOfInterest > u16_CurrentSize)
    {
        s16_Difference = u16_RegionOfInterest - u16_CurrentSize;
    }
    else
    {
        u16_Difference = 0;
    }
*/
    s16_Difference = u16_RegionOfInterest - u16_CurrentSize;

    // adjust this difference evenly u16_Start and u16_End

    // edge0 is adjustment along start
    u16_Edge0Margin = u16_Start - u16_Min;
    s16_Edge0Adjust = s16_Difference/2;

    // check to see if there is enough margin along edge0
    // to accomodate the edge0 adjustment
    s16_Edge0Adjust = FrameDimension_Min(s16_Edge0Adjust, u16_Edge0Margin);

    // now adjust the left and right read out coordinates
    u16_Start -= s16_Edge0Adjust;

    // start must be even
    if (u16_Start & 0x0001)
    {
        u16_Start--;
    }

    u16_End = u16_Start + u16_RegionOfInterest - 1;

    // u16_End must be odd
    if (!(u16_End & 0x0001))
    {
        u16_End++;
    }

    u16_End = FrameDimension_Min(u16_End, u16_Max);

    // the start and end must span the region of interest
    // else it means that the end did not have enough margin
    // modify start in such a case
    if (u16_RegionOfInterest > (u16_End - u16_Start + 1))
    {
        s16_Difference = u16_RegionOfInterest - (u16_End - u16_Start + 1);
    }
    else
    {
        s16_Difference = 0;
    }

    u16_Start -= s16_Difference;
    // start must be even
    if (u16_Start & 0x0001)
    {
        u16_Start--;
    }

    *pu16_Start = u16_Start;
    *pu16_End = u16_End;

    return;
}

/*
   *************************************************************************************************************
   \fn void FrameDimension_SetNewFrameLength(uint16_t   u16_DesiredFrameLengthLines)
   \brief It is used to change the length of the frame (generally
   to alter the frame rate).


     \param u16_DesiredFrameLengthLines : The new desired frame length

      \return void.
      \ingroup Frame Dimension
      \callgraph
      \callergraph
      *************************************************************************************************************
   */
void
FrameDimension_SetNewFrameLength(
uint16_t    u16_RequestedFrameLength)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint16_t    u16_ClippedFrameLengthLines,
                u16_FlickerFreePeriods,
                u16_TempVar;
    float_t     f_MaximumFrameTime_us,
                f_FlickerFreeFrameTime_us;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /// Ensure that the requested frame length is between the maximum and minimum vt frame length lines
    u16_ClippedFrameLengthLines = FrameDimension_Clip(
        u16_RequestedFrameLength,
        g_FrameDimensionStatus.u16_MinVTFrameLengthAtCurrentVTYSize_Lines,
        g_SensorFrameConstraints.u16_MaxVTFrameLengthLines);

    /// Ensure that if the host wants integer number of flicker free bunches in
    /// the frame length then we increase the frame length to accommodate integer
    /// flicker free bunches (if it does not have them already)
    if (g_HostFrameConstraints.e_Flag_GuaranteeStaticFlickerFrameLength)
    {
        /// Compute the desired frame time
        /// Compute the number of flicker free periods in this frame time
        /// Round up this number to ensure that we are more than minimum frame length

        /// Check if (u16_ClippedFrameLengthLines) * g_FrameDimensionStatus.f_VTLineLength_us)
        /// is an integer multiple of g_AntiFlicker_Status.f_FlickerFreePeriod_us.
        /// If yes, there is no need to add 1 to FlickerFreePeriods
        u16_TempVar = (uint16_t) ((u16_ClippedFrameLengthLines) * g_FrameDimensionStatus.f_VTLineLength_us) / g_AntiFlicker_Status.f_FlickerFreePeriod_us;

        if((u16_TempVar * g_AntiFlicker_Status.f_FlickerFreePeriod_us) == (u16_ClippedFrameLengthLines * g_FrameDimensionStatus.f_VTLineLength_us))
        {
            u16_FlickerFreePeriods = (uint16_t)((u16_ClippedFrameLengthLines * g_FrameDimensionStatus.f_VTLineLength_us) /
                                                                        g_AntiFlicker_Status.f_FlickerFreePeriod_us
                );
        }
        else
        {
            u16_FlickerFreePeriods = (uint16_t)
                (
                    (
                        ((u16_ClippedFrameLengthLines) * g_FrameDimensionStatus.f_VTLineLength_us) /
                        g_AntiFlicker_Status.f_FlickerFreePeriod_us
                    ) + 1.0
                );
        }


        /// compute the FlickerFreeFrameTime
        f_FlickerFreeFrameTime_us = ((float_t) (u16_FlickerFreePeriods) * g_AntiFlicker_Status.f_FlickerFreePeriod_us);

        /// Ensure that this flicker free frame time is less than the maximum frame time
        f_MaximumFrameTime_us =
            (
                (float_t) (g_SensorFrameConstraints.u16_MaxVTFrameLengthLines) *
                g_FrameDimensionStatus.f_VTLineLength_us
            );

        if (f_MaximumFrameTime_us < f_FlickerFreeFrameTime_us)
        {
            /// The flicker free frame time exceeds the maximum frame time.
            /// Subtract one flicker free period from flicker free frame time
            f_FlickerFreeFrameTime_us = (f_FlickerFreeFrameTime_us - g_AntiFlicker_Status.f_FlickerFreePeriod_us);
        }


        /// Now compute the number of lines in f_FlickerFreeFrameTime_us
        /// Round up the number of lines if not already on an integer boundary.

        /// Check if f_FlickerFreeFrameTime_us is an integer multiple of f_VTLineLength_us.
        /// If yes, then there will be no addtion of 1.0
        u16_TempVar = (uint16_t) (f_FlickerFreeFrameTime_us / g_FrameDimensionStatus.f_VTLineLength_us);

        if ((u16_TempVar * g_FrameDimensionStatus.f_VTLineLength_us) == f_FlickerFreeFrameTime_us)
        {
            u16_ClippedFrameLengthLines = (uint16_t) (f_FlickerFreeFrameTime_us / g_FrameDimensionStatus.f_VTLineLength_us);
        }
        else
        {
            u16_ClippedFrameLengthLines = (uint16_t) ((f_FlickerFreeFrameTime_us / g_FrameDimensionStatus.f_VTLineLength_us) + 1.0);
        }

        /// Ensure that the requested frame length is between the maximum and minimum vt frame length lines
        u16_ClippedFrameLengthLines = FrameDimension_Clip(
            u16_ClippedFrameLengthLines,
            g_FrameDimensionStatus.u16_MinVTFrameLengthAtCurrentVTYSize_Lines,
            g_SensorFrameConstraints.u16_MaxVTFrameLengthLines);
    }


    if (u16_ClippedFrameLengthLines != g_FrameDimensionStatus.u16_VTFrameLengthPending_Lines)
    {
        /// A change in the frame length has to be done...
        g_FrameDimensionStatus.u16_VTFrameLengthPending_Lines = u16_ClippedFrameLengthLines;

        /// set the flag to indicate that a frame length change is required...
        g_FrameDimensionStatus.e_Flag_IsFrameLengthChangePending = Flag_e_TRUE;
    }


    return;
}


/**
\fn void FrameDimension_UpdateCurrentVTFrameLength(void)
\brief Copies the u16_VTFrameLengthPending_Lines into the
Current Frame Dimension.
\param void
\return void.
\ingroup Frame Dimension
\callgraph
\callergraph
   */
void
FrameDimension_UpdateCurrentVTFrameLength(void)
{
    /// Update the current vt frame length lines
    g_CurrentFrameDimension.u16_VTFrameLengthLines = g_FrameDimensionStatus.u16_VTFrameLengthPending_Lines;

    /// Compute the current vt frame time
    ComputeVTFrameTime();

    /// Reset the e_Flag_IsFrameLengthChangePending flag to reflect that there is
    /// no frame length change pending...
    g_FrameDimensionStatus.e_Flag_IsFrameLengthChangePending = Flag_e_FALSE;

    return;
}


/**
   \fn void FrameDimension_ComputeMaximumSensorFieldOfView(void)
   \brief The function computes the maximum field of view possible based on following parameters:
   1. Frame rate configured
   2. Configured user restriction bit mask
   3. Aspect ratio

    \param void
     \return void.
     \ingroup Frame Dimension
     \callgraph
     \callergraph
   */
void
FrameDimension_ComputeMaximumSensorFieldOfView(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /// The logic applied is to check if the x_output_size at sensor FFOV and at
    /// current data_reduction_factor (sub_sampling_factor * scaling_factor)
    /// plus dummy columns is an exact multiple of 'n' pixels (where n is such that
    /// the x_output_size satisfies the ccp_data_width_constraint).

    /// If yes, then the sensor will be able to stream at FFOV.
    /// If no, then the sensor cannot stream at FFOV.
    CAM_DRV_SENS_FORMAT_E data_format;
    float_t  f_MasterAR = 0.0, f_maxFov_X = 0.0, f_maxFFOVX = 0.0;
    uint16_t u16_OutputSize, u16_MaxModeFrameRate_x100 = 0, u16_current_prescale = 1;
    uint16_t u16_count = 0, maxFOVcount = 0;
    uint16_t u16_woi_resX, u16_woi_resY;
    uint8_t  u8_ReductionFactor;
    Flag_te  mode_valid = Flag_e_TRUE;

    // Reset pre-scale factor. Pre-scale will depend on list of sensor modes and max fps requested by user.
    g_HostFrameConstraints.u8_MaximumPreScale = 1;

   // get the AR of the master pipe
    if (Master_e_Pipe0 == g_Zoom_Params.e_Master && SystemConfig_IsPipe0Valid())
    {
        f_MasterAR = ((float_t)g_Pipe[0].u16_X_size )/((float_t)g_Pipe[0].u16_Y_size);
    }
    else if(SystemConfig_IsPipe1Valid())
    {
        f_MasterAR = ((float_t)g_Pipe[1].u16_X_size )/((float_t)g_Pipe[1].u16_Y_size);
     }
    else
    {
       // BMS case
       f_MasterAR = ((float_t)g_Pipe_RAW.u16_woi_res_X)/((float_t)g_Pipe_RAW.u16_woi_res_Y);
    }

  // find Maximum usable FOV which is closest to AR and frame rate
  // Following code will scan only those modes which match use-case
  if((g_camera_details.p_sensor_details != NULL)&&
     (g_camera_details.p_sensor_details->output_mode_capability.p_modes != NULL)
    )
    {
        for (u16_count = 0; u16_count < g_camera_details.p_sensor_details->output_mode_capability.number_of_modes; u16_count++)
        {
            u16_MaxModeFrameRate_x100 = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_count].max_frame_rate_x100;

            data_format = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_count].data_format;

          if ((CAM_DRV_USAGE_MODE_UNDEFINED == g_RunMode_Control.e_LLD_USAGE_MODE_usagemode) ||
            ! ((g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_count].usage_restriction_bitmask) & (1 << g_RunMode_Control.e_LLD_USAGE_MODE_usagemode)))
            {
                mode_valid = Flag_e_TRUE;
            }
          else
            {
                mode_valid = Flag_e_FALSE;
            }

            // All three condition must be met to select a perticular mode
            // 1. same data format as asked by user
            // 2. maximum frame rate in that mode must be equal or greater
            // 3. user restriction bitmask must match

            if((data_format == lla_HostRawFormatToLLARawFormat(VideoTiming_GetCsiRawFormat()))&&
                ((FrameRate_GetMaximumFrameRate() * 100) <= u16_MaxModeFrameRate_x100 )&&
                (Flag_e_TRUE == mode_valid))
            {
                u16_woi_resX = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_count].woi_res.width;
                u16_woi_resY = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_count].woi_res.height;

                u16_current_prescale = u16_woi_resX/g_camera_details.p_sensor_details->output_mode_capability.p_modes[u16_count].output_res.width;

                if (u16_current_prescale > g_HostFrameConstraints.u8_MaximumPreScale)
                {
                   g_HostFrameConstraints.u8_MaximumPreScale = u16_current_prescale;
                }

                // Max FOV is calculated only for X
                f_maxFov_X = u16_woi_resY * f_MasterAR;

                if(f_maxFov_X > (float_t)u16_woi_resX )
                {
                    f_maxFov_X = (float_t)u16_woi_resX;
                }

                if(f_maxFFOVX < f_maxFov_X)
                {
                    f_maxFFOVX = f_maxFov_X;
                    //f_maxFFOVY = f_maxFov_X / f_MasterAR;
                    maxFOVcount = u16_count;
                }
            }
        }
    }

    OstTraceInt1(TRACE_FLOW, "FrameDimension_ComputeMaximumSensorFieldOfView(): Max prescale: %d",g_HostFrameConstraints.u8_MaximumPreScale);

    /// Compute the maximum possible reduction over possible FFOV or relative FOV
    u8_ReductionFactor = g_HostFrameConstraints.u8_MaximumPreScale;

    // <agupta>: to be reviewed again
    g_SensorFrameConstraints.u16_MaxOPXOutputSize = g_camera_details.p_sensor_details->output_mode_capability.p_modes[maxFOVcount].woi_res.width;
    g_SensorFrameConstraints.u16_MaxOPYOutputSize = g_camera_details.p_sensor_details->output_mode_capability.p_modes[maxFOVcount].woi_res.height - non_smia_correction_factor;

    u16_OutputSize =  g_camera_details.p_sensor_details->output_mode_capability.p_modes[maxFOVcount].woi_res.width/u8_ReductionFactor;

//    g_FrameDimensionStatus.u16_MaximumUsableSensorFOVX = g_camera_details.p_sensor_details->output_mode_capability.p_modes[maxFOVcount].woi_res.width - u8_ReductionFactor*FrameDimension_GetISPColumnLoss();
//    g_FrameDimensionStatus.u16_MaximumUsableSensorFOVY = g_camera_details.p_sensor_details->output_mode_capability.p_modes[maxFOVcount].woi_res.height - u8_ReductionFactor*FrameDimension_GetISPRowLoss();

    g_FrameDimensionStatus.u16_MaximumUsableSensorFOVX = g_camera_details.p_sensor_details->output_mode_capability.p_modes[maxFOVcount].woi_res.width;
    //[NON_ISL_SUPPORT] - This is done to reduce the Maximum FOVY 
    g_FrameDimensionStatus.u16_MaximumUsableSensorFOVY = g_camera_details.p_sensor_details->output_mode_capability.p_modes[maxFOVcount].woi_res.height - non_smia_correction_factor;


    g_FrameDimensionStatus.u8_TotalFOVXShift = 0;
    g_FrameDimensionStatus.u8_TotalFOVYShift = 0;

    OstTraceInt2(TRACE_FLOW, "Max Usable FOV X: %d, Y: %d",g_FrameDimensionStatus.u16_MaximumUsableSensorFOVX,g_FrameDimensionStatus.u16_MaximumUsableSensorFOVY);
    return;
}


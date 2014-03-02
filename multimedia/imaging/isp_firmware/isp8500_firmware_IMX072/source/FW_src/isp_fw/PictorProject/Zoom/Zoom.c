/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  Zoom Zoom

 \detail    The zoom engine responsible for accepting and processing user zoom commands.
            It is also responsible for application of pan and tilt commands.
            It ensures that a particular zoom step can been applied within the current
            system constraints. If the current system constraints do not allow a particular
            zoom step, it raises an appropriate status.
*/

/**
 \file      Zoom.c
 \brief     The main zoom manager file. It is a part of the zoom module release.
            It implements the zoom engine responsible for accepting and processing
            user zoom and pan/tilt commands.
 \ingroup   Zoom
*/
#include "Zoom_OPInterface.h"
#include "Zoom_IPInterface.h"
#include "Zoom.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#   include "PictorProject_Zoom_ZoomTraces.h"
#endif
Zoom_Control_ts                             g_Zoom_Control =
{
    DEFAULT_ZOOM_STEP,
    DEFAULT_SET_FOV_X,
    DEFAULT_CENTER_OFFSET_X,
    DEFAULT_CENTER_OFFSET_Y,
    DEFAULT_AUTO_ZOOM_CONTROL,
    DEFAULT_PERFORM_DYNAMIC_RESOLUTION_UPDATE
};

Zoom_Control1_ts                            g_Zoom_Control1 = { DEFAULT_Max_DZ, };

Zoom_Params_ts                              g_Zoom_Params = { DEFAULT_ZOOM_RANGE, Master_e_Pipe0 };

Zoom_CommandControl_ts                      g_Zoom_CommandControl =
{
    DEFAULT_ZOOM_CONTROL_COUNT,         // u8_CommandCount
    DEFAULT_ZOOM_COMMAND_CONTROL        // e_ZoomCommand
};

Zoom_CommandStatus_ts                       g_Zoom_CommandStatus =
{
    DEFAULT_ZOOM_STATUS_COUNT,          // u8_CommandCount
    DEFAULT_ZOOM_COMMAND_STATUS,        // e_ZoomCommand
    DEFAULT_ZOOM_COMMAND_STATUS_STATUS, // e_ZoomCmdStatus
    DEFAULT_ZOOM_REQUEST_STATUS         // e_ZoomRequestStatus
};
Zoom_Status_ts                              g_Zoom_Status;

Zoom_Status_LLA_ts                          g_Zoom_Status_LLA;
Zoom_PipeState_ts                           g_PipeState[PIPE_COUNT];

void                                        SetCenter (uint8_t e_Flag_EnableModeSelection);
void                                        SetZoomFOV (float_t f_DesiredFOVX, uint8_t e_Flag_EnableModeSelection);
void                                        SetPipeFOV (
                                            float_t     f_DesiredFOVX,
                                            float_t     f_DesiredFOVY,
                                            uint16_t    u16_Pipe0OpSize_X,
                                            uint16_t    u16_Pipe0OpSize_Y,
                                            uint16_t    u16_Pipe1OpSize_X,
                                            uint16_t    u16_Pipe1OpSize_Y,
                                            uint8_t     e_Flag_EnableModeSelection);
void                                        RefreshOutputSize (void);
float_t                                     GetOutputSizeChangeFactor (float_t f_DesiredFOVX);

/**
 \fn        uint8_t Zoom_Initialise ( void )
 \brief     Top level function to compute the maximum FOV available to the device.
            Must be called everytime there is a change in the streaming parameters
            with respect to bits per pixel or output image resolutions for either pipe.
            Must be called only once the output image resolutions for both the pipes are valid.
 \param     void
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
*/
uint8_t
Zoom_Initialise(void)
{
    float_t f_InputAR;
    float_t f_MasterAR;
    float_t f_MaxARCorrectedFOVX;
    Result_te e_PipeStatusCheck =Result_e_Success;

    /// \par Implementation
        /// - Computes the maximum fov available to the device accounting for the maximum pre scale factor
        /// and any pre scale loss in the ISP.
        /// - Stores the output image resolutions of pipe0 and pipe1.
        /// - Computes the maximum fov available taking into account the
        /// aspect ratio of pipe0.
    // get the maximum available FOV to device post losses in the device
    g_Zoom_Status.f_MaxFOVXAvailableToDevice = Zoom_GetMaxUserFOVX();
    g_Zoom_Status.f_MaxFOVYAvailableToDevice = Zoom_GetMaxUserFOVY();

    g_Zoom_Status.u8_CenterShiftX = (FrameDimension_GetMaximumXOutputSize() - FrameDimension_GetISPColumnLoss() * g_HostFrameConstraints.u8_MaximumPreScale - g_Zoom_Status.f_MaxFOVXAvailableToDevice)/2 ;
    g_Zoom_Status.u8_CenterShiftY = (FrameDimension_GetMaximumYOutputSize() - FrameDimension_GetISPRowLoss() * g_HostFrameConstraints.u8_MaximumPreScale - g_Zoom_Status.f_MaxFOVYAvailableToDevice)/2 ;

    f_InputAR = g_Zoom_Status.f_MaxFOVXAvailableToDevice / g_Zoom_Status.f_MaxFOVYAvailableToDevice;

    // Get the pipe0 params
    if (SystemConfig_IsPipe0Valid())
    {
        g_PipeState[0].u16_OutputSizeX = Zoom_GetPipe0OpSize_X();
        g_PipeState[0].u16_OutputSizeY = Zoom_GetPipe0OpSize_Y();
        g_PipeState[0].f_AspectRatio = ((float_t) g_PipeState[0].u16_OutputSizeX) / ((float_t) g_PipeState[0].u16_OutputSizeY);
        g_PipeState[0].e_Flag_Enabled = Flag_e_TRUE;
        g_PipeStatus[0].e_Flag_Exceed_HW_Limitation = Flag_e_FALSE;
        e_PipeStatusCheck &= Result_e_Success;

        //check if the output size being set is within HW limitaion
      if((g_PipeState[0].u16_OutputSizeX > MAX_OUTPUT_SIZEX_PIPE0) || (g_PipeState[0].u16_OutputSizeY > MAX_OUTPUT_SIZEY))
      {
       g_PipeState[0].e_Flag_Enabled = Flag_e_FALSE;
       g_PipeStatus[0].e_Flag_Exceed_HW_Limitation = Flag_e_TRUE;
       e_PipeStatusCheck &= Result_e_Failure;
      }
    }
    else
    {
    g_PipeState[0].e_Flag_Enabled = Flag_e_FALSE;
    }


    // Get the pipe1 params if present
    if ((PIPE_COUNT > 1) && SystemConfig_IsPipe1Valid())
    {
        g_PipeState[1].u16_OutputSizeX = Zoom_GetPipe1OpSize_X();
        g_PipeState[1].u16_OutputSizeY = Zoom_GetPipe1OpSize_Y();
        g_PipeState[1].f_AspectRatio = ((float_t) g_PipeState[1].u16_OutputSizeX) / ((float_t) g_PipeState[1].u16_OutputSizeY);
        g_PipeState[1].e_Flag_Enabled = Flag_e_TRUE;
        g_PipeStatus[1].e_Flag_Exceed_HW_Limitation = Flag_e_FALSE;
        e_PipeStatusCheck &= Result_e_Success;

        //check if the output size being set is within HW limitaion
      if((g_PipeState[1].u16_OutputSizeX > MAX_OUTPUT_SIZEX_PIPE1) || (g_PipeState[1].u16_OutputSizeY > MAX_OUTPUT_SIZEY))
      {


       g_PipeState[1].e_Flag_Enabled = Flag_e_FALSE;
       g_PipeStatus[1].e_Flag_Exceed_HW_Limitation = Flag_e_TRUE;
       e_PipeStatusCheck &= Result_e_Failure;
      }
    }
    else
    {
        g_PipeState[1].e_Flag_Enabled = Flag_e_FALSE;
    }


    // get the AR of the master pipe
    if (Master_e_Pipe0 == g_Zoom_Params.e_Master && SystemConfig_IsPipe0Valid())
    {
        f_MasterAR = g_PipeState[0].f_AspectRatio;
    }
    else if (SystemConfig_IsPipe1Valid())
    {
        f_MasterAR = g_PipeState[1].f_AspectRatio;
    }
    else
    {
        f_MasterAR = g_Zoom_Status.f_MaxFOVXAvailableToDevice / g_Zoom_Status.f_MaxFOVYAvailableToDevice;
    }


    // now compute the maximum zoom FOV available to the device accounting for losses in ISP and pipe0 aspect ratio
    if (f_MasterAR > f_InputAR)
    {
        f_MaxARCorrectedFOVX = g_Zoom_Status.f_MaxFOVXAvailableToDevice;
    }
    else
    {
        f_MaxARCorrectedFOVX = g_Zoom_Status.f_MaxFOVYAvailableToDevice * f_MasterAR;
    }


    // compute the minimum FOV about array center with respect to the max available fov at center
    g_Zoom_Status.f_MinFOVXAtArrayCenter = f_MaxARCorrectedFOVX / g_Zoom_Params.f_ZoomRange;

    if (Result_e_Failure == e_PipeStatusCheck)
    {
        return (Flag_e_FALSE);
    }
    else
    {
        return (Flag_e_TRUE);
    }
}


/**
 \fn        void Zoom_StepComplete( void )
 \brief     Function invoked to indicate to the zoom engine that the last zoom step
            has been applied.
 \param     void
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
*/
void
Zoom_StepComplete(void)
{
    // zoom is ready to receive more zoom commands
    g_Zoom_CommandStatus.u8_CommandCount = g_Zoom_CommandControl.u8_CommandCount;

    // raise a notification to indicate that zoom step has been completed
    Zoom_StepCompleteNotify();

    return;
}


/**
 \fn        uint8_t Zoom_Run( uint8_t e_Flag_EnableModeSelection )
 \brief     Top level function to process any zoom user commands. Must be called continuously.
 \param     void
 \return    uint8_t
 Flag_e_TRUE: A zoom step is pending
 Flag_e_FALSE: No zoom step is pending
 \ingroup   Zoom
 \callgraph
 \callergraph
*/
uint8_t
Zoom_Run(uint8_t e_Flag_EnableModeSelection)
{
    uint8_t e_Flag_UserCommandPending;
    uint8_t e_ZoomCommand;
    if
    (
        (g_Zoom_CommandControl.u8_CommandCount != g_Zoom_CommandStatus.u8_CommandCount)
    ||  (Flag_e_TRUE == g_SystemConfig_Status.e_Flag_RunBestSensormode)
    )
    {
        e_Flag_UserCommandPending = Flag_e_TRUE;
    }
    else
    {
        e_Flag_UserCommandPending = Flag_e_FALSE;
    }


    // if any user command is pending or we are auto zooming then we are in business
    // care must be taken to execute the loop only if sensor reprogram is not pending
    if
    (
        (
            (ZoomRequestStatus_e_ReProgramRequired != g_Zoom_CommandStatus.e_ZoomRequestStatus)
        &&  ((Flag_e_TRUE == e_Flag_UserCommandPending) || (Flag_e_TRUE == g_Zoom_Status.e_Flag_AutoZooming))
        )
    ||  !Zoom_IsStreaming()
    )
    {
        // if a user command is pending, it has preference
        if (Flag_e_TRUE == e_Flag_UserCommandPending)
        {
            e_ZoomCommand = g_Zoom_CommandControl.e_ZoomCommand;
        }
        else
        {
            e_ZoomCommand = g_Zoom_CommandStatus.e_ZoomCommand;
        }


        if (Flag_e_TRUE == g_SystemConfig_Status.e_Flag_RunBestSensormode)
        {
            g_SystemConfig_Status.e_Flag_RunBestSensormode = Flag_e_FALSE;
        }


        if (ZoomCommand_e_SetCenter == e_ZoomCommand)
        {
            // no auto zoom
            SetCenter(e_Flag_EnableModeSelection);
            g_Zoom_Status.e_Flag_AutoZooming = Flag_e_FALSE;
        }
        else if (ZoomCommand_e_SetFOV == e_ZoomCommand)
        {
            if (g_Zoom_Control.f_SetFOVX > 0)
            {
                SetZoomFOV(g_Zoom_Control.f_SetFOVX, e_Flag_EnableModeSelection);
            }
            else
            {
                SetZoomFOV(g_Zoom_Status.f_MaxAvailableFOVXAtCurrentCenter, e_Flag_EnableModeSelection);
            }


            // no auto zoom
            g_Zoom_Status.e_Flag_AutoZooming = Flag_e_FALSE;
        }
        else if (ZoomCommand_e_RefreshOutputSize == e_ZoomCommand)
        {
            RefreshOutputSize();

            // no auto zoom
            g_Zoom_Status.e_Flag_AutoZooming = Flag_e_FALSE;
        }


#if 0
        switch (e_ZoomCommand)
        {
            case ZoomCommand_e_In:
                /*
                                if (Flag_e_TRUE == g_Zoom_Control.e_Flag_AutoZoom)
                                {
                                    // auto zooming in
                                    g_Zoom_Status.e_Flag_AutoZooming = Flag_e_TRUE;
                                }
                                else
                                {
                                    // single step zoom in
                                    g_Zoom_Status.e_Flag_AutoZooming = Flag_e_FALSE;
                                }
                            */
                g_Zoom_Status.e_Flag_AutoZooming = g_Zoom_Control.e_Flag_AutoZoom;

                break;

            case ZoomCommand_e_Out:
                /*
                                if (Flag_e_TRUE == g_Zoom_Control.e_Flag_AutoZoom)
                                {
                                    // auto zooming out
                                    g_Zoom_Status.e_Flag_AutoZooming = Flag_e_TRUE;
                                }
                                else
                                {
                                    // single step zoom out
                                    g_Zoom_Status.e_Flag_AutoZooming = Flag_e_FALSE;
                                }
                */
                g_Zoom_Status.e_Flag_AutoZooming = g_Zoom_Control.e_Flag_AutoZoom;
                break;

            case ZoomCommand_e_SetFOV:
                SetZoomFOV(g_Zoom_Control.f_SetFOVX);

                // no auto zoom
                g_Zoom_Status.e_Flag_AutoZooming = Flag_e_FALSE;

                break;

            case ZoomCommand_e_SetCenter:
                // no auto zoom
                g_Zoom_Status.e_Flag_AutoZooming = Flag_e_FALSE;
                SetCenter();

                break;

            case ZoomCommand_e_Reset:
                // stop auto zoom
                g_Zoom_Status.e_Flag_AutoZooming = Flag_e_FALSE;

                break;

            case ZoomCommand_e_Stop:
                // stop auto zoom
                g_Zoom_Status.e_Flag_AutoZooming = Flag_e_FALSE;

                break;

            case ZoomCommand_e_None:
                break;

            default:
                break;
        }


#endif

        // by this time, we have the new zoom parameters available
        // check to see if a sensor changeover is required
        // set the last executed command status to be equal to the command status
        g_Zoom_CommandStatus.e_ZoomCommand = g_Zoom_CommandControl.e_ZoomCommand;

        if
        (
            (ZoomCommand_e_SetCenter == g_Zoom_CommandStatus.e_ZoomCommand)
        &&  (ZoomCmdStatus_e_SetOutOfRange == g_Zoom_CommandStatus.e_ZoomCmdStatus)
        )
        {
            // notify that the zoom set center was out of range
            Zoom_CommandStatusSetOutOfRangeNotify();
        }

        // if a zoom request has been denied, then mark the zoom step as complete
        if (ZoomRequestStatus_e_Denied == g_Zoom_CommandStatus.e_ZoomRequestStatus)
        {
            // no command is pending
            e_Flag_UserCommandPending = Flag_e_FALSE;

            // not auto zooming any more either
            g_Zoom_Status.e_Flag_AutoZooming = Flag_e_FALSE;

            // raise a notification (if any) to indicate zoom request failure
            Zoom_ConfigRequestDeniedNotify();

            // mark the step as complete
            Zoom_StepComplete();
        }
        else if (ZoomRequestStatus_e_ReProgramRequired == g_Zoom_CommandStatus.e_ZoomRequestStatus)
        {
            // raise a noification (if any) to indicate that a sensor reprogram
            // is required for the zoom config
            Zoom_ConfigRequestReprogramRequired();
        }
    }   // consider new zoom step

    return (e_Flag_UserCommandPending);
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void RefreshOutputSize( void )
 \brief     Function used to set the output sizes of the pipes.
 \param     void
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
void
RefreshOutputSize(void)
{
    uint16_t    u16_Pipe0OpSize_X;
    uint16_t    u16_Pipe0OpSize_Y;
    uint16_t    u16_Pipe1OpSize_X = 0;
    uint16_t    u16_Pipe1OpSize_Y = 0;

    // Get the pipe0 params
    u16_Pipe0OpSize_X = Zoom_GetPipe0OpSize_X();
    u16_Pipe0OpSize_Y = Zoom_GetPipe0OpSize_Y();

    // Get the pipe1 params if present
    if (PIPE_COUNT > 1)
    {
        u16_Pipe1OpSize_X = Zoom_GetPipe1OpSize_X();
        u16_Pipe1OpSize_Y = Zoom_GetPipe1OpSize_Y();
    }


    // invoke SetPipeFOV to check for any violations
    // the FOV remains the same as that of the current FOV
    // only the output sizes need to be refreshed
    SetPipeFOV(
    g_Zoom_Status.f_FOVX,
    g_Zoom_Status.f_FOVY,
    u16_Pipe0OpSize_X,
    u16_Pipe0OpSize_Y,
    u16_Pipe1OpSize_X,
    u16_Pipe1OpSize_Y,
    Flag_e_TRUE);

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void SetCenter( void )
 \brief     Function used to set the center of the current field of view.
 \param     void
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
void
SetCenter(uint8_t e_Flag_EnableModeSelection)
{
    float_t     f_MaxAvailableFOVXAtRequestedCenter;
    float_t     f_MaxAvailableFOVYAtRequestedCenter;
    float_t     f_DesiredFOVX;
    float_t     f_DesiredFOVY;
    float_t     f_FOVXLoss;
    float_t     f_FOVYLoss;
    float_t     f_MaxFOVXAtMaxFOVY;
    float_t     f_MaxFOVYAtMaxFOVX;
    float_t     f_AspectRatio;
    uint16_t    u16_DesiredFOVX;
    uint16_t    u16_DesiredFOVY;
    int16_t     s16_CenterOffsetX;
    int16_t     s16_CenterOffsetY;
    uint8_t     e_ZoomCmdStatus;
    int16_t     s16_XOffsetSign;
    int16_t     s16_YOffsetSign;
    uint16_t    u16_CenterOffsetX;
    uint16_t    u16_CenterOffsetY;
    uint16_t    u16_RequiredLineLength;

    /// \par Implementation
        /// - While streaming, the device has to ensure that any new command
        /// to set center is able to accomodate the current field of view.
        /// - While not streaming (setup time before starting to stream), the
        /// device gives priority to the center. At this time, initially the
        /// a request is made for the maximum possible FOV at the current center.
        /// - A subsequent ZoomCommand_e_SetFOV command will then set the actual FOV.
    // compute the maximum FOV possible for the requested center
    // FOV loss = abs(offset)*2

    OstTraceInt0(TRACE_FLOW, "SetCenter");

    s16_CenterOffsetX = g_Zoom_Control.s16_CenterOffsetX;
    s16_CenterOffsetY = g_Zoom_Control.s16_CenterOffsetY;

    if (s16_CenterOffsetX >= 0)
    {
        s16_XOffsetSign = 1;
    }
    else
    {
        s16_XOffsetSign = -1;
    }


    if (s16_CenterOffsetY >= 0)
    {
        s16_YOffsetSign = 1;
    }
    else
    {
        s16_YOffsetSign = -1;
    }


    e_ZoomCmdStatus = ZoomCmdStatus_e_OK;

    if (Zoom_IsStreaming())
    {
        u16_CenterOffsetX = s16_XOffsetSign * s16_CenterOffsetX;
        u16_CenterOffsetY = s16_YOffsetSign * s16_CenterOffsetY;

        // if streaming, then consider the current fov offset against the desired center
        f_DesiredFOVX = g_Zoom_Status.f_FOVX;
        f_DesiredFOVY = g_Zoom_Status.f_FOVY;

        if (u16_CenterOffsetX > g_Zoom_Status.u16_MaximumAbsoluteXCenterShift)
        {
            e_ZoomCmdStatus = ZoomCmdStatus_e_SetOutOfRange;
            s16_CenterOffsetX = g_Zoom_Status.u16_MaximumAbsoluteXCenterShift * s16_XOffsetSign;
        }


        if (u16_CenterOffsetY > g_Zoom_Status.u16_MaximumAbsoluteYCenterShift)
        {
            e_ZoomCmdStatus = ZoomCmdStatus_e_SetOutOfRange;
            s16_CenterOffsetY = g_Zoom_Status.u16_MaximumAbsoluteYCenterShift * s16_YOffsetSign;
        }
    }


    f_FOVXLoss = s16_CenterOffsetX * 2 * s16_XOffsetSign;
    f_FOVYLoss = s16_CenterOffsetY * 2 * s16_YOffsetSign;

    f_MaxAvailableFOVXAtRequestedCenter = g_Zoom_Status.f_MaxFOVXAvailableToDevice - f_FOVXLoss;
    f_MaxAvailableFOVYAtRequestedCenter = g_Zoom_Status.f_MaxFOVYAvailableToDevice - f_FOVYLoss;

    if (Master_e_Pipe0 == g_Zoom_Params.e_Master && SystemConfig_IsPipe0Valid())
    {
        f_AspectRatio = g_PipeState[0].f_AspectRatio;
    }
    else if (SystemConfig_IsPipe1Valid())
    {
        f_AspectRatio = g_PipeState[1].f_AspectRatio;
    }
    else
    {
        f_AspectRatio = f_MaxAvailableFOVXAtRequestedCenter / f_MaxAvailableFOVYAtRequestedCenter;
    }


    f_MaxFOVXAtMaxFOVY = f_MaxAvailableFOVYAtRequestedCenter * f_AspectRatio;
    f_MaxFOVYAtMaxFOVX = f_MaxAvailableFOVXAtRequestedCenter / f_AspectRatio;

    if (f_MaxAvailableFOVXAtRequestedCenter > f_MaxFOVXAtMaxFOVY)
    {
        f_MaxAvailableFOVXAtRequestedCenter = f_MaxFOVXAtMaxFOVY;
    }


    if (f_MaxAvailableFOVYAtRequestedCenter > f_MaxFOVYAtMaxFOVX)
    {
        f_MaxAvailableFOVYAtRequestedCenter = f_MaxFOVYAtMaxFOVX;
    }


    if (!Zoom_IsStreaming())
    {
        // if not streaming, then use g_Zoom_Control.f_SetFOVX if it is non zero
        // else use the maximum possible FOV at the requested center
        // if the g_Zoom_Control.f_SetFOVX > 0 thent the host wants to
        // use a specific FOV to start streaming
        if (g_Zoom_Control.f_SetFOVX > 0)
        {
            f_DesiredFOVX = min(g_Zoom_Control.f_SetFOVX,f_MaxAvailableFOVXAtRequestedCenter);
            f_DesiredFOVY = f_DesiredFOVX / f_AspectRatio;

            if (f_DesiredFOVX > f_MaxAvailableFOVXAtRequestedCenter)
            {
                f_DesiredFOVX = f_MaxAvailableFOVXAtRequestedCenter;
            }


            if (f_DesiredFOVY > f_MaxAvailableFOVYAtRequestedCenter)
            {
                f_DesiredFOVY = f_MaxAvailableFOVYAtRequestedCenter;
            }
        }
        else
        {
            f_DesiredFOVX = f_MaxAvailableFOVXAtRequestedCenter;
            f_DesiredFOVY = f_MaxAvailableFOVXAtRequestedCenter/f_AspectRatio;
        }


        // we are not streaming, there is no concept of a line length at the moment
        u16_RequiredLineLength = 0;
    }
    else
    {
        // we are streaming, we need to retain the line length when setting the center
        // <BG> Line length will get updated below once new sizes are known.
        // Do not require to change line length unless sensor mode is changed.
        u16_RequiredLineLength = (uint16_t)(Zoom_GetCurrentSensorLineLength()*VT_32SF_OUTPUT_TIMING_DERATING);
    }


    g_Zoom_CommandStatus.e_ZoomCmdStatus = e_ZoomCmdStatus;

    u16_DesiredFOVX = Zoom_Ceiling(f_DesiredFOVX);
    u16_DesiredFOVY = Zoom_Ceiling(f_DesiredFOVY);

    // request a new zoom configuration with a new center
    // the current line length has to be retained
    // u16_DesiredFOVX, u16_DesiredFOVY, u16_RequiredLineLength, s16_CenterOffsetX, s16_CenterOffsetY, e_Flag_ScaleLineLengthForDerating
    // we do not want the line length to be scaled for derating for setcenter
    g_Zoom_CommandStatus.e_ZoomRequestStatus = Zoom_Request(
        u16_DesiredFOVX,
        u16_DesiredFOVY,
        u16_RequiredLineLength,
        0,
        s16_CenterOffsetX,
        s16_CenterOffsetY,
        Flag_e_FALSE,
        e_Flag_EnableModeSelection);
    if (ZoomRequestStatus_e_Denied != g_Zoom_CommandStatus.e_ZoomRequestStatus)
    {
        // request accepted, update internal parameters
        // update the center only if streaming
        // if not streaming, then the center is set when the FOV is set
        if (Zoom_IsStreaming())
        {
            //g_Zoom_Status.f_XCenter = (Zoom_GetMaxInputSizeXMinusISPLossAtPendingPreScale() - 1) * 0.5 + s16_CenterOffsetX;
            //g_Zoom_Status.f_YCenter = (Zoom_GetMaxInputSizeYMinusISPLossAtPendingPreScale() - 1) * 0.5 + s16_CenterOffsetY;
            g_Zoom_Status.f_XCenter = (Zoom_GetMaxInputSizeXMinusISPLossAtMaxPreScale() - 1) * 0.5 + s16_CenterOffsetX;
            g_Zoom_Status.f_YCenter = (Zoom_GetMaxInputSizeYMinusISPLossAtMaxPreScale() - 1) * 0.5 + s16_CenterOffsetY;
        }

        g_Zoom_Status.f_MaxAvailableFOVXAtCurrentCenter = f_MaxAvailableFOVXAtRequestedCenter;
        g_Zoom_Status.s16_CenterOffsetX = s16_CenterOffsetX;
        g_Zoom_Status.s16_CenterOffsetY = s16_CenterOffsetY;

         if (ZoomRequestStatus_e_ReProgramRequired == g_Zoom_CommandStatus.e_ZoomRequestStatus)
         {
              /// Calculate new line-length here
              /// update pipe status
              //f_PreScaleFactor = Zoom_GetRequestedPreScaleFactorLLA();
              SetPipeFOV(  f_DesiredFOVX,
                           f_DesiredFOVY,
                           Zoom_GetPipe0OpSize_X(),
                           Zoom_GetPipe0OpSize_Y(),
                           Zoom_GetPipe1OpSize_X(),
                           Zoom_GetPipe1OpSize_Y(),
                           e_Flag_EnableModeSelection);
         }
         /// <BG>This above call could result in one more denied..need to handle
    }


    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void SetZoomFOV( float_t f_DesiredFOVX )
 \brief     Function used to set the desired field of view
 \param     f_DesiredFOVX   :   The X component of the desired fov which needs to be set
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
void
SetZoomFOV(
float_t f_DesiredFOVX, uint8_t e_Flag_EnableModeSelection)
{
    float_t     f_AspectRatio;
    float_t     f_DesiredFOVY;
    float_t     f_OutputResolutionChangeFactor;
    uint16_t    u16_DesiredFOVX;
    uint16_t    u16_DesiredFOVY;
    uint16_t    u16_Pipe0OpSize_X;
    uint16_t    u16_Pipe0OpSize_Y;
    uint16_t    u16_Pipe1OpSize_X;
    uint16_t    u16_Pipe1OpSize_Y;

    /// \par Implementation
        /// - Clip the desired fov between the f_MaxAvailableFOVXAtCurrentCenter and f_MinFOVXAtArrayCenter
        /// - Invoke SetPipeFOV to set the field of view in the pipes taking into account the aspect ratio
    // clip the FOV against the maximum and minimum FOV

    OstTraceInt0(TRACE_FLOW, "SetZoomFOV");

    if (f_DesiredFOVX >= g_Zoom_Status.f_MaxAvailableFOVXAtCurrentCenter)
    {
        f_DesiredFOVX = g_Zoom_Status.f_MaxAvailableFOVXAtCurrentCenter;

        g_Zoom_CommandStatus.e_ZoomCmdStatus = ZoomCmdStatus_e_FullyZoomedOut;
    }
    else if (f_DesiredFOVX <= g_Zoom_Status.f_MinFOVXAtArrayCenter)
    {
        f_DesiredFOVX = g_Zoom_Status.f_MinFOVXAtArrayCenter;

        g_Zoom_CommandStatus.e_ZoomCmdStatus = ZoomCmdStatus_e_FullyZoomedIn;
    }
    else
    {
        g_Zoom_CommandStatus.e_ZoomCmdStatus = ZoomCmdStatus_e_OK;
    }


    // the new FOV will depend on the master pipe
    // the slave FOV will always be less than the master
    if (Master_e_Pipe0 == g_Zoom_Params.e_Master && SystemConfig_IsPipe0Valid())
    {
        f_AspectRatio = g_PipeState[0].f_AspectRatio;
    }
    else if (SystemConfig_IsPipe1Valid())
    {
        f_AspectRatio = g_PipeState[1].f_AspectRatio;
    }
    else
    {
        f_AspectRatio = g_Zoom_Status.f_MaxFOVXAvailableToDevice / g_Zoom_Status.f_MaxFOVYAvailableToDevice;
    }


    f_DesiredFOVY = f_DesiredFOVX / f_AspectRatio;

    u16_DesiredFOVX = Zoom_Ceiling(f_DesiredFOVX);
    u16_DesiredFOVY = Zoom_Ceiling(f_DesiredFOVY);

    // request the new FOV, with 0 line length requirement
    // a request of 0 line length means to use the minimum line length
    // u16_DesiredFOVX, u16_DesiredFOVY, u16_RequiredLineLength, s16_CenterOffsetX, s16_CenterOffsetY, e_Flag_ScaleLineLengthForDerating
    // we do not want the line length to be scaled for derating for set zoom FOV
    g_Zoom_CommandStatus.e_ZoomRequestStatus = Zoom_Request(
        u16_DesiredFOVX,
        u16_DesiredFOVY,
        0,
        0,
        g_Zoom_Status.s16_CenterOffsetX,
        g_Zoom_Status.s16_CenterOffsetY,
        Flag_e_FALSE,
        e_Flag_EnableModeSelection);

    if (ZoomRequestStatus_e_Denied != g_Zoom_CommandStatus.e_ZoomRequestStatus)
    {
        if (SystemConfig_IsInputImageSourceSensor())
        {
            if
            (
                !(FD_IS_SENSOR_STREAMING())
            ||  (FrameDimensionRequestStatus_e_ReProgeamRequired == g_Zoom_CommandStatus.e_ZoomRequestStatus)
            )
            {
                g_Zoom_Status.e_ZoomRequestLLDStatus = ZoomRequestLLDStatus_e_ReProgeamRequired;
            }
        }


        // work in 4 decimal place fixed point notation to ensure 4 decimal place precision
        f_OutputResolutionChangeFactor = GetOutputSizeChangeFactor(f_DesiredFOVX) * 10000;

        u16_Pipe0OpSize_X = (uint16_t) ((Zoom_Ceiling(Zoom_GetPipe0OpSize_X() * f_OutputResolutionChangeFactor)) / 10000);
        u16_Pipe0OpSize_Y = (uint16_t) ((Zoom_Ceiling(Zoom_GetPipe0OpSize_Y() * f_OutputResolutionChangeFactor)) / 10000);
        u16_Pipe1OpSize_X = (uint16_t) ((Zoom_Ceiling(Zoom_GetPipe1OpSize_X() * f_OutputResolutionChangeFactor)) / 10000);
        u16_Pipe1OpSize_Y = (uint16_t) ((Zoom_Ceiling(Zoom_GetPipe1OpSize_Y() * f_OutputResolutionChangeFactor)) / 10000);

        SetPipeFOV(
        f_DesiredFOVX,
        f_DesiredFOVY,
        u16_Pipe0OpSize_X,
        u16_Pipe0OpSize_Y,
        u16_Pipe1OpSize_X,
        u16_Pipe1OpSize_Y,
        e_Flag_EnableModeSelection);
    }


    // if we are not streaming, then we also need to update the zoom center
    if (ZoomRequestStatus_e_Denied != g_Zoom_CommandStatus.e_ZoomRequestStatus)
    {
        if (!Zoom_IsStreaming())
        {
            //g_Zoom_Status.f_XCenter = (float_t)(((Zoom_GetMaxInputSizeXMinusISPLossAtPendingPreScale() - 1) * 0.5) + g_Zoom_Status.s16_CenterOffsetX);
            //g_Zoom_Status.f_YCenter = (float_t)(((Zoom_GetMaxInputSizeYMinusISPLossAtPendingPreScale() - 1) * 0.5) + g_Zoom_Status.s16_CenterOffsetY);
            g_Zoom_Status.f_XCenter = (float_t) (((Zoom_GetMaxInputSizeXMinusISPLossAtMaxPreScale() - 1) * 0.5) + g_Zoom_Status.s16_CenterOffsetX);
            g_Zoom_Status.f_YCenter = (float_t) (((Zoom_GetMaxInputSizeYMinusISPLossAtMaxPreScale() - 1) * 0.5) + g_Zoom_Status.s16_CenterOffsetY);
        }


        // update the maximum shift in center allowed at the current FOV...
        g_Zoom_Status.u16_MaximumAbsoluteXCenterShift = (uint16_t) ((g_Zoom_Status.f_MaxFOVXAvailableToDevice - g_Zoom_Status.f_FOVX) * 0.5);
        g_Zoom_Status.u16_MaximumAbsoluteYCenterShift = (uint16_t) ((g_Zoom_Status.f_MaxFOVYAvailableToDevice - g_Zoom_Status.f_FOVY) * 0.5);
    }


    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        float_t GetOutputSizeChangeFactor( float_t f_DesiredFOVX )
 \brief     Function used to return the change factor in output image resolution at a given FOVX
 \param     f_DesiredFOVX   :   The X component of the desired fov at which the change factor in ouput image resolution is needed
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
float_t
GetOutputSizeChangeFactor(
float_t f_DesiredFOVX)
{

    return (1.0);
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void SetPipeFOV( float_t f_DesiredFOVX, float_t f_DesiredFOVY )
 \brief     Function used to set the desired field of into the individual pipes
 \param     f_DesiredFOVX: Desired FOVX
            f_DesiredFOVY: Desired FOVY
 \return    Nothing
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
void
SetPipeFOV(
float_t     f_DesiredFOVX,
float_t     f_DesiredFOVY,
uint16_t    u16_Pipe0OpSize_X,
uint16_t    u16_Pipe0OpSize_Y,
uint16_t    u16_Pipe1OpSize_X,
uint16_t    u16_Pipe1OpSize_Y,
uint8_t     e_Flag_EnableModeSelection)
{
    float_t     f_Pipe0FOVX;
    float_t     f_Pipe0FOVY;
    float_t     f_Pipe1FOVX;
    float_t     f_Pipe1FOVY;
    float_t     f_Pipe0Step;
    float_t     f_Pipe1Step;
    float_t     f_PreScaleFactor;
    float_t     f_InvStepSize;
    float_t     f_Pipe0LineLength;
    float_t     f_Pipe1LineLength;
    float_t     f_RequiredLineLength;
    float_t     f_MasterAR;
    float_t     f_DesiredAR;
    float_t     f_MaxSF0 = 0.0;
    float_t     f_MaxSF1 = 0.0;
    float_t     f_MaxLineLength = 0.0;
    uint16_t    u16_DesiredFOVX;
    uint16_t    u16_DesiredFOVY;
    uint16_t    u16_RequiredLineLength;
    uint16_t    u16_x_size;
    uint16_t    u16_MaxLineLength;

    u16_DesiredFOVX = Zoom_Ceiling(f_DesiredFOVX);
    u16_DesiredFOVY = Zoom_Ceiling(f_DesiredFOVY);

    f_DesiredAR = f_DesiredFOVX / f_DesiredFOVY;

    if (SystemConfig_IsPipe0Valid())
    {
        if (g_PipeState[0].f_AspectRatio > f_DesiredAR)
        {
            f_Pipe0FOVX = f_DesiredFOVX;
            f_Pipe0FOVY = f_Pipe0FOVX / g_PipeState[0].f_AspectRatio;
        }
        else
        {
            f_Pipe0FOVY = f_DesiredFOVY;
            f_Pipe0FOVX = f_Pipe0FOVY * g_PipeState[0].f_AspectRatio;
        }


        f_Pipe0Step = f_Pipe0FOVX / u16_Pipe0OpSize_X;
    }


    if (PIPE_COUNT > 1 && SystemConfig_IsPipe1Valid())
    {
        if (g_PipeState[1].f_AspectRatio > f_DesiredAR)
        {
            f_Pipe1FOVX = f_DesiredFOVX;
            f_Pipe1FOVY = f_Pipe1FOVX / g_PipeState[1].f_AspectRatio;
        }
        else
        {
            f_Pipe1FOVY = f_DesiredFOVY;
            f_Pipe1FOVX = f_Pipe1FOVY * g_PipeState[1].f_AspectRatio;
        }


        f_Pipe1Step = f_Pipe1FOVX / u16_Pipe1OpSize_X;
    }


    // request to set the FOV was successful
    // make a request to set the line length if needed

    /* TODO: <LLA> <AG>: How to tackle pre-scale done in sensor in case of LLA? */
    while (1)
    {
        // fetch the current prescale factor
        f_PreScaleFactor = Zoom_GetRequestedPreScaleFactorLLA();

        // check the line length requirements for pipe0
        // Enabled only if valid (see zoom initialize)
        if (Flag_e_TRUE == g_PipeState[0].e_Flag_Enabled)
        {
            // the required scale factor is (f_Pipe0FOVX/g_PipeState[0].u16_OutputSizeX)/f_PreScaleFactor
            // ==> inverse of step size = (g_PipeState[0].u16_OutputSizeX * f_PreScaleFactor)/f_Pipe0FOVX
            // ==> inverse of step size = f_PreScaleFactor/f_Pipe0Step
            f_InvStepSize = f_PreScaleFactor / f_Pipe0Step;

            OstTraceInt1(TRACE_FLOW, "<zoom> f_inv[0] x 1000:  %d" , (uint32_t) (f_InvStepSize * 1000));

            // min line length required for upscale = (g_PipeState[0].u16_OutputSizeX + min_line_blanking_beyond_scalar) * f_InvStepSize
            // min line length required for downscale = (g_PipeState[0].u16_OutputSizeX / f_InvStepSize) + min_line_blanking_beyond_scalar
            if (f_InvStepSize > 1.0)
            {
                // upscaling
                //Fix proposed by S rao
                f_Pipe0LineLength = (u16_Pipe0OpSize_X + Zoom_GetMinLineBlankingBeyondGPS0()) * Zoom_Ceiling(f_InvStepSize);
            }
            else
            {
                // downscaling
                f_Pipe0LineLength = (((float_t) u16_Pipe0OpSize_X) / f_InvStepSize) + Zoom_GetMinLineBlankingBeyondGPS0();
            }


            f_MaxSF0 = (u16_Pipe0OpSize_X /(g_Zoom_Status.f_MaxAvailableFOVXAtCurrentCenter / g_Zoom_Control1.f_MaxDZ)) * Zoom_GetRequestedPreScaleFactorLLA();
        }
        else
        {
            // pipe disabled, no line length requirement
            f_Pipe0LineLength = 0;
        }


        if (PIPE_COUNT > 1)
        {
            // check the line length requirements for pipe1
            // Enabled only if valid (see zoom initialize)
            if (Flag_e_TRUE == g_PipeState[1].e_Flag_Enabled)
            {
                // the required scale factor is (f_Pipe1FOVX/g_PipeState[1].u16_OutputSizeX)/f_PreScaleFactor
                // ==> inverse of step size = (g_PipeState[1].u16_OutputSizeX * f_PreScaleFactor)/f_Pipe1FOVX
                // ==> inverse of step size = f_PreScaleFactor/f_Pipe1Step
                f_InvStepSize = f_PreScaleFactor / f_Pipe1Step;

                OstTraceInt1(TRACE_FLOW, "<zoom> f_inv[1] x 1000:  %d" , (uint32_t)(f_InvStepSize * 1000));

                // min line length required for upscale = (g_PipeState[1].u16_OutputSizeX + min_line_blanking_beyond_scalar) * f_InvStepSize
                // min line length required for downscale = (g_PipeState[1].u16_OutputSizeX / f_InvStepSize) + min_line_blanking_beyond_scalar
                if (f_InvStepSize > 1.0)
                {
                    // upscaling
                    //Fix proposed by S rao
                    f_Pipe1LineLength = (u16_Pipe1OpSize_X + Zoom_GetMinLineBlankingBeyondGPS1()) * Zoom_Ceiling(f_InvStepSize);
                }
                else
                {
                    // downscaling
                    f_Pipe1LineLength = (((float_t) u16_Pipe1OpSize_X) / f_InvStepSize) + Zoom_GetMinLineBlankingBeyondGPS1();
                }


                f_MaxSF1 = (u16_Pipe1OpSize_X /(g_Zoom_Status.f_MaxAvailableFOVXAtCurrentCenter / g_Zoom_Control1.f_MaxDZ)) * Zoom_GetRequestedPreScaleFactorLLA();
            }
            else
            {
                // pipe disabled, no line length requirement
                f_Pipe1LineLength = 0;
            }
        }
        else
        {
            // single pipe design, no line length requirement for pipe1
            f_Pipe1LineLength = 0;
        }


        if (f_Pipe0LineLength > f_Pipe1LineLength)
        {
            f_RequiredLineLength = f_Pipe0LineLength;
        }
        else
        {
            f_RequiredLineLength = f_Pipe1LineLength;
        }


        u16_RequiredLineLength = Zoom_Ceiling(f_RequiredLineLength);

        if (f_MaxSF0 > f_MaxSF1)
        {
            f_InvStepSize = f_MaxSF0;

            u16_x_size = u16_Pipe0OpSize_X;
        }
        else
        {
            f_InvStepSize = f_MaxSF1;

            u16_x_size = u16_Pipe1OpSize_X;
        }


        if (f_InvStepSize > 1.0)
        {
            // upscaling
            // Fix proposed by S rao
            f_MaxLineLength = (u16_x_size + Zoom_GetMinLineBlankingBeyondGPS1()) * Zoom_Ceiling(f_InvStepSize);
        }


        u16_MaxLineLength = Zoom_Ceiling(f_MaxLineLength);

        // request the new configuration, this time with actual line length requirement
        // u16_DesiredFOVX, u16_DesiredFOVY, u16_RequiredLineLength, s16_CenterOffsetX, s16_CenterOffsetY, e_Flag_ScaleLineLengthForDerating
        // we want the line length to be scaled for any possible derating
        g_Zoom_CommandStatus.e_ZoomRequestStatus = Zoom_Request(
            u16_DesiredFOVX,
            u16_DesiredFOVY,
            u16_RequiredLineLength,
            u16_MaxLineLength,
            g_Zoom_Status.s16_CenterOffsetX,
            g_Zoom_Status.s16_CenterOffsetY,
            Flag_e_TRUE,
            e_Flag_EnableModeSelection);

        // Previous call to Zoom_Request() is done taking into account mandatory as well as indicative requirements
        // If the call fails, Zoom_Request() is called again, this time only with mandatory requirement
        if
        (
            (ZoomRequestStatus_e_Denied == g_Zoom_CommandStatus.e_ZoomRequestStatus)
        &&  !(Flag_e_TRUE == g_PipeState[0].e_Flag_Enabled && Flag_e_TRUE == g_PipeState[1].e_Flag_Enabled)
        )
        {
            if (Flag_e_TRUE == g_PipeState[0].e_Flag_Enabled)
            {
                f_RequiredLineLength = f_Pipe0LineLength;
                if (f_MaxSF0 > 1.0)
                {
                    //f_MaxLineLength = (u16_Pipe0OpSize_X + Zoom_GetMinLineBlankingBeyondGPS1()) * f_MaxSF0;
                    f_MaxLineLength = (u16_Pipe0OpSize_X + Zoom_GetMinLineBlankingBeyondGPS0()) * f_MaxSF0;
                }
                else
                {
                    f_MaxLineLength = 0.0;
                }
            }
            else
            {
                f_RequiredLineLength = f_Pipe1LineLength;
                if (f_MaxSF0 > 1.0)
                {
                    //f_MaxLineLength = (u16_Pipe1OpSize_X + Zoom_GetMinLineBlankingBeyondGPS1()) * f_MaxSF1;
                    f_MaxLineLength = (u16_Pipe1OpSize_X + Zoom_GetMinLineBlankingBeyondGPS1()) * f_MaxSF1;
                }
                else
                {
                    f_MaxLineLength = 0.0;
                }
            }


            u16_RequiredLineLength = Zoom_Ceiling(f_RequiredLineLength);
            u16_MaxLineLength = Zoom_Ceiling(f_MaxLineLength);

            g_Zoom_CommandStatus.e_ZoomRequestStatus = Zoom_Request(
                u16_DesiredFOVX,
                u16_DesiredFOVY,
                u16_RequiredLineLength,
                u16_MaxLineLength,
                g_Zoom_Status.s16_CenterOffsetX,
                g_Zoom_Status.s16_CenterOffsetY,
                Flag_e_TRUE,
                e_Flag_EnableModeSelection);
        }


        // at this time, if the request has been denied, no point searching further, break out of the search
        // if the request is not denied and assigned prescale factor is more than the earlier prescale factor
        // then run the search again with new line length requirements.
        // if the request is not denied and assigned prescale factor is equal to the earlier prescale factor,
        // we have found the correct line length and prescale factor value, break out of the loop
        if (ZoomRequestStatus_e_Denied != g_Zoom_CommandStatus.e_ZoomRequestStatus)
        {
            if (f_PreScaleFactor < Zoom_GetRequestedPreScaleFactorLLA())
                {
                    // request has not been denied
                    // but the assigned prescale factor more than the assumed prescale factor
                    // iterate again through the loop
                    continue;
                }
                else
                {
                    // request has not been denied
                    // and the assigned prescale factor is the same as the assumed prescale factor
                    // update the FOV of the respective pipes and the zoom manager as well
                    // update the FOV of the respective pipes and the zoom manager as well
                    if (SystemConfig_IsInputImageSourceSensor())
                    {
                        // <Todo: BG>: Need to see what minimum config to set here
                        lla_abstraction_ReprogramSensorForZoom(Flag_e_TRUE);

                        if
                        (
                            (g_config_feedback.woi_res.width < (uint16_t) f_DesiredFOVX)
                        &&  (g_config_feedback.woi_res.height < (uint16_t) f_DesiredFOVY)
                        )
                        {
                            g_Zoom_CommandStatus.e_ZoomRequestStatus = ZoomRequestStatus_e_Denied;
                            return;
                        }
                    }


                    g_Zoom_Status.f_FOVX = f_DesiredFOVX;

                    // get the AR of the master pipe
                    //<Todo: BG > This check is necessary only if refreshoutputsize is required
                    if (Master_e_Pipe0 == g_Zoom_Params.e_Master && SystemConfig_IsPipe0Valid())
                    {
                        f_MasterAR = g_PipeState[0].f_AspectRatio;
                    }
                    else if (SystemConfig_IsPipe1Valid())
                    {
                        f_MasterAR = g_PipeState[1].f_AspectRatio;
                    }
                    else
                    {
                        f_MasterAR = g_Zoom_Status.f_MaxFOVXAvailableToDevice / g_Zoom_Status.f_MaxFOVYAvailableToDevice;
                    }


                    g_Zoom_Status.f_FOVY = g_Zoom_Status.f_FOVX / f_MasterAR;

                    if (SystemConfig_IsPipe0Valid())
                    {
                        g_PipeState[0].f_FOVX = f_Pipe0FOVX;
                        g_PipeState[0].f_FOVY = f_Pipe0FOVY;
                        g_PipeState[0].f_Step = f_Pipe0Step;
                        g_PipeState[0].u16_OutputSizeX = u16_Pipe0OpSize_X;
                        g_PipeState[0].u16_OutputSizeY = u16_Pipe0OpSize_Y;
                    }


                    if (SystemConfig_IsPipe1Valid())
                    {
                        g_PipeState[1].f_FOVX = f_Pipe1FOVX;
                        g_PipeState[1].f_FOVY = f_Pipe1FOVY;
                        g_PipeState[1].f_Step = f_Pipe1Step;
                        g_PipeState[1].u16_OutputSizeX = u16_Pipe1OpSize_X;
                        g_PipeState[1].u16_OutputSizeY = u16_Pipe1OpSize_Y;
                    }


                    break;
                }
        }
        else
        {
            // request has been denied, break out of the search
            break;
        }
    }   // while loop

    return;
}


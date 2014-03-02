/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef ZOOM_OPINTERFACE_H_
#define ZOOM_OPINTERFACE_H_

/**
 \file      Zoom_OPInterface.h
  \brief    This file is a part of the zoom module release.
            It provides an interface to allow other modules to access
            the exported zoom functions and functions.
 \ingroup   Zoom
*/
#include "Platform.h"
#include "ZoomPlatformSpecific.h"
#include "ZoomTop.h"


/**
 \enum ZoomCommand_te
 \brief Command set applicable to the zoom engine
 \ingroup Zoom
*/
typedef enum
{
    ZoomCommand_e_None,
    ZoomCommand_e_In,
    ZoomCommand_e_Out,
    ZoomCommand_e_SetCenter,
    ZoomCommand_e_SetFOV,
    ZoomCommand_e_Reset,
    ZoomCommand_e_Stop,
    ZoomCommand_e_RefreshOutputSize
} ZoomCommand_te;

/**
 \enum ZoomOperationStatus_te
 \brief Zoom operation status of the last zoom command
 \ingroup Zoom
*/
typedef enum
{
    ZoomCmdStatus_e_OK,
    ZoomCmdStatus_e_FullyZoomedOut,
    ZoomCmdStatus_e_FullyZoomedIn,
    ZoomCmdStatus_e_SetOutOfRange
} ZoomCmdStatus_te;

/* TODO: <LLA> <AG>: Frame dimension and zoom rely on same values of re-progrm, should be discouraged  */
typedef enum
{
    ZoomRequestStatus_e_None,
    ZoomRequestStatus_e_Accepted,
    ZoomRequestStatus_e_Denied,
    ZoomRequestStatus_e_ReProgramRequired
} ZoomRequestStatus_te;

typedef enum
{
    Master_e_Pipe0,
    Master_e_Pipe1
} Master_te;

typedef enum
{
    ZoomRequestLLDStatus_e_Accepted,
    ZoomRequestLLDStatus_e_Truncated,
    ZoomRequestLLDStatus_e_ReProgeamRequired,
} ZoomRequestLLDStatus_te;

/**
 \struct    Zoom_Control_ts
 \brief     Main control page for the zoom engine.
 \ingroup   Zoom
*/
typedef struct
{
    /// Specifies the zoom step that is applicable at each zoom in
    /// or zoom out step. For a zoom in step, new window of interest
    /// is computed as follows:
    /// new_woi = current_woi * f_ZoomStep
    /// For a zoom out step, new window of interest is computed as follows:
    /// new_woi = current_woi / f_ZoomStep
    float_t     f_ZoomStep;

    /// Specifies the FOV to be used if custom FOV is desired. If this is set
    /// to zero, then the maximum available FOV is used. When starting to stream
    /// after a mode static change, the device will start streaming from
    /// the FOV mentioned in this element.
    float_t     f_SetFOVX;

    /// X offset to be applied to the center of the image.
    /// To be used for for pan and tilt operation. This offset
    /// can be negative as well as positive. The default
    /// center of the image is considered to be the
    /// center of the sensor imaging array. A negative offset will
    /// move the image center to the left while a positive offset
    /// will move the center to the right from its default offset.
    /// Only abosrbed in context of a zoom set command.
    int16_t     s16_CenterOffsetX;

    /// Y offset to be applied to the center of the image.
    /// To be used for for pan and tilt operation. This offset
    /// can be negative as well as positive. The default
    /// center of the image is considered to be the
    /// center of the sensor imaging array. A negative offset will
    /// move the image center up while a positive offset
    /// will move the center down from its default offset.
    /// Only abosrbed in context of a zoom set command.
    int16_t     s16_CenterOffsetY;

    /// Valid for e_ZoomIn and e_ZoomOut commands only. Set to
    /// Flag_e_TRUE if a continuous zoom operation is intended.
    /// In this mode of operation, new zoom step is computed and
    /// applied at the end of each successful zoom step until the zoom
    /// range is exhasted or a changeover boundary is hit. This gives a smooth
    /// zoom effect. If set to Flag_e_FALSE, then only one zoom step is computed
    /// and applied.
    uint8_t     e_Flag_AutoZoom;

    /// Flag to specify if change in output image resolution must be considered
    /// with every zoom step as per the desired field of view.
    /// If e_Flag_PerformDynamicResolutionUpdate == Flag_e_TRUE, then at every change in field of view,
    /// corresponding change in output image resolution is also done as per the
    /// Zoom_DynamicOutputResolutionUpdateLUT LUT specified by the host.
    /// If e_Flag_PerformDynamicResolutionUpdate == Flag_e_FALSE, then output image resolutions are considered
    /// static regardless of the field of view.
    uint8_t     e_Flag_PerformDynamicResolutionUpdate;
} Zoom_Control_ts;

/**
 \struct    Zoom_CommandControl_ts
 \brief     Command page for the zoom engine
 \ingroup   Zoom
*/
typedef struct
{
    /// Specifies max DZ factor
    float_t f_MaxDZ;
} Zoom_Control1_ts;

/**
 \struct    Zoom_CommandControl_ts
 \brief     Command page for the zoom engine
 \ingroup   Zoom
*/
typedef struct
{
    /// Set the Zoom_CommandControl.u8_CommandCount value to be different
    /// from that of to Zoom_CommandStatus.u8_CommandCount to
    /// act as a trigger to absorb the zoom command (e_ZoomCommand).
    uint8_t u8_CommandCount;

    /// Zoom command is issued through this page element. The zoom
    /// command will be fetched only when control coin (Zoom_CommandControl.u8_CommandCount)
    /// is different from status coin (Zoom_CommandStatus.u8_CommandCount).
    uint8_t e_ZoomCommand;
} Zoom_CommandControl_ts;

/**
 \struct    Zoom_CommandStatus_ts
 \brief     Command status page for the zoom engine
 \ingroup   Zoom
*/
typedef struct
{
    /// When a zoom step has been computed and applied, the
    /// Zoom_CommandStatus.u8_CommandCount element becomes equal
    /// to Zoom_CommandControl.u8_CommandCount.
    /// At this point, it is safe to issue another zoom command.
    uint8_t     u8_CommandCount;

    /// Spcecifies the last command executed
    uint8_t     e_ZoomCommand;

    /// Specifies the status of the last zoom operation
    uint8_t     e_ZoomCmdStatus;

    /// Specifies the status of the last request made
    /// by the zoom module to the system to achieve
    /// a particular frame dimension
    uint8_t     e_ZoomRequestStatus;
} Zoom_CommandStatus_ts;

/**
 \struct    Zoom_Params_ts
 \brief     Zoom parameters page
 \ingroup   Zoom
*/
typedef struct
{
    /// Specifies the maximum zoom factor. This will relate
    /// directly to a minimum field of view. Must be programmed before
    /// starting a streaming operation
    float_t f_ZoomRange;

    /// Specifies the master pipe for computation of sensor frame dimension.
    /// If for a given combination of zoom FOV, P0 output and P1 output,
    /// the sensor frame requirements (wrt readout coordinates) are different
    /// for the two pipes, then the master pipe's requirement will take
    /// priority over the other pipe
    uint8_t e_Master;
} Zoom_Params_ts;

/**
 \struct    Zoom_DynamicOutputResolutionUpdateLUT_ts
 \brief     LUT for dynamic output resolution update
 \ingroup   Zoom
*/
typedef struct
{
    /// Specifies the FOV range as a fraction of g_Zoom_Status.f_MaxAvailableFOVXAtCurrentCenter.
    /// Must be setup in decreasing order i.e. f_FOVRange[0] should have the
    /// maximum value.
    /// At runtime, for each zoom step, the following is done:
    /// Consider f_FOVRange[n] = x and f_FOVRange[n+1] = y
    /// If x >= desired_fov/g_Zoom_Status.f_MaxAvailableFOVXAtCurrentCenter > y then
    /// the output image moderation factor used is f_OutputResolutionChangeFactor[n]
    float_t f_FOVRatio[ZOOM_DYNAMIC_OUTPUT_RESOLUTION_CHANGE_LUT_DEPTH];

    /// Specifies the factor by which the output image resolution has to be changed.
    float_t f_OutputResolutionChangeFactor[ZOOM_DYNAMIC_OUTPUT_RESOLUTION_CHANGE_LUT_DEPTH];
} Zoom_DynamicOutputResolutionUpdateLUT_ts;

/**
 \struct    Zoom_Status_ts
 \brief     Status page for the zoom engine
 \ingroup   Zoom
*/
typedef struct
{
    /// Specifies the zero based X coordinate of the current user FOV center
    float_t     f_XCenter;

    /// Specifies the zero based Y coordinate of the current user FOV center
    float_t     f_YCenter;

    /// Specifies the maximum FOV X dimension that will be available
    /// to the user. It does not account for losses in the FOV due to
    /// differences in sensor aspect ratio and output image aspect ratio.
    float_t     f_MaxFOVXAvailableToDevice;

    /// Specifies the maximum FOV Y dimension that will be available
    /// to the user. It does not account for losses in the FOV due to
    /// differences in sensor aspect ratio and output image aspect ratio.
    float_t     f_MaxFOVYAvailableToDevice;

    /// Specifies the minimum FOV X dimension computed in basis of f_ZoomRange.
    /// It is computed as follows:
    /// f_MinFOVXAtArrayCenter = max_ar_corrected_fov_x/f_ZoomRange
    float_t     f_MinFOVXAtArrayCenter;

    /// Specifies the maximum FOV X dimension available at the currently
    /// applicable image center.
    float_t     f_MaxAvailableFOVXAtCurrentCenter;

    /// Specifies the current FOV X dimension
    float_t     f_FOVX;

    /// Specifies the current FOV Y dimension
    float_t     f_FOVY;

    /// Specifies the maximum absolute X offset allowed for horizontal center shift
    /// A shift of +-u16_MaximumAbsoluteXCenterShift is allowed to the center at the
    /// current FOV
    uint16_t    u16_MaximumAbsoluteXCenterShift;

    /// Specifies the maximum absolute Y offset allowed for horizontal center shift
    /// A shift of +-u16_MaximumAbsoluteYCenterShift is allowed to the center at the
    /// current FOV
    uint16_t    u16_MaximumAbsoluteYCenterShift;

    /// Specifies the current horizontal center offset applied to the image
    int16_t     s16_CenterOffsetX;

    /// Specifies the current vertical center offset applied to the image
    int16_t     s16_CenterOffsetY;

    /// Specifies whether an auto zoom is in progress.
    uint8_t     e_Flag_AutoZooming;

    //<Todo: BG>: How to manage this?? <BG><BUG> Please move to lld specific things
    /// Added to indicate that LLD FOV is different than requested FOV
    uint8_t     e_ZoomRequestLLDStatus;

    /// Added to indicate shift in center in X for device FOV calculation
    uint8_t     u8_CenterShiftX;

    /// Added to indicate shift in center in Y for device FOV calculation
    uint8_t     u8_CenterShiftY;
	
} Zoom_Status_ts;

/**
 \struct    Zoom_PipeState_ts
 \brief     Page describing the pipe parameters based on which scalar parameters are computed
 \ingroup   Zoom
*/
typedef struct
{
    /// Specifies the aspect ratio of the output image
    /// required for the pipe. Aspect ratio is defined
    /// as x_size/y_size
    float_t     f_AspectRatio;

    /// Specifies the FOV X dimension of the pipe.
    float_t     f_FOVX;

    /// Specifies the FOV Y dimension of the pipe.
    float_t     f_FOVY;

    /// Specifies the top level step size needed
    /// to scale the user FOV to output image dimension.
    float_t     f_Step;

    /// Specifies the X dimension of the output image
    /// resolution of the pipe.
    uint16_t    u16_OutputSizeX;

    /// Specifies the Y dimension of the output image
    /// resolution of the pipe.
    uint16_t    u16_OutputSizeY;

    /// Specifies whether the pipe is enabled or disabled.
    uint8_t     e_Flag_Enabled;
} Zoom_PipeState_ts;

/**
 \struct    Zoom_Status_LLA_ts
 \brief     Page describing the sizes requested from LLD and feedback from LLD
 \ingroup   Zoom
*/
typedef struct
{
    /// Requested woi selected from sensor
    uint16_t    u16_Ex_woi_resX;
    uint16_t    u16_Ex_woi_resY;

    /// Requested output resoulution selected from sensor
    uint16_t    u16_Ex_output_resX;
    uint16_t    u16_Ex_output_resY;

    /// Current woi selected from sensor
    uint16_t    u16_woi_resX;
    uint16_t    u16_woi_resY;

    /// Current output resoulution selected from sensor
    uint16_t    u16_output_resX;
    uint16_t    u16_output_resY;
} Zoom_Status_LLA_ts;

extern Zoom_CommandControl_ts   g_Zoom_CommandControl;
extern Zoom_CommandStatus_ts    g_Zoom_CommandStatus;
extern Zoom_Control_ts          g_Zoom_Control;
extern Zoom_Control1_ts         g_Zoom_Control1;
extern Zoom_Params_ts           g_Zoom_Params;
extern Zoom_Status_ts           g_Zoom_Status;
extern Zoom_PipeState_ts        g_PipeState[PIPE_COUNT];

extern Zoom_Status_LLA_ts       g_Zoom_Status_LLA;

extern void                     Zoom_StepComplete (void);
extern uint8_t Zoom_Initialise ( void ) TO_EXT_DDR_PRGM_MEM;
extern uint8_t  Zoom_Run (uint8_t e_Flag_EnableModeSelection);

/// Interface to indicate to the zoom engine that a sensor change over operation has been completed
#   define Zoom_SensorChangeOverCompleted()    g_Zoom_CommandStatus.e_ZoomRequestStatus = ZoomRequestStatus_e_Accepted

#   define Zoom_IsFDMRequestPending()          (ZoomRequestStatus_e_ReProgramRequired == g_Zoom_CommandStatus.e_ZoomRequestStatus)

#   define ZoomTop_isFrameApplicationControl_ScalarParamApplied() (g_ZoomTop_ParamAppicationControl.u8_ScalarParamsApplied == g_ZoomTop_ParamAppicationControl.u8_ScalarParamsGenerated)

/// Macro to expose the X component of the maximum field of view available to the device
#   define Zoom_GetMaximumFOVXAvailableToDevice()  READONLY(g_Zoom_Status.f_MaxFOVXAvailableToDevice)

/// Macro to expose the Y component of the maximum field of view available to the device
#   define Zoom_GetMaximumFOVYAvailableToDevice()  READONLY(g_Zoom_Status.f_MaxFOVYAvailableToDevice)

/// Interface to query the current Field of view
#   define Zoom_GetCurrentFOVX()   READONLY(g_Zoom_Status.f_FOVX)

/// Interface to query the current Field of view
#   define Zoom_GetCurrentFOVY()                   READONLY(g_Zoom_Status.f_FOVY)
#   define Zoom_GetCurrentFOVCenterX()             READONLY(g_Zoom_Status.f_XCenter)
#   define Zoom_GetCurrentFOVCenterY()             READONLY(g_Zoom_Status.f_YCenter)
#   define Zoom_GetCurrentFOVCenterShiftOffsetX()  READONLY(g_Zoom_Status.s16_CenterOffsetX)
#   define Zoom_GetCurrentFOVCenterShiftOffsetY()  READONLY(g_Zoom_Status.s16_CenterOffsetY)

/// Interface to query the current pipe0 X output size (it is valid only when pipe0 is enabled)
#   define Zoom_GetPipe0OpSizeX()  READONLY(g_PipeState[0].u16_OutputSizeX)

/// Interface to query the current pipe0 Y output size (it is valid only when pipe0 is enabled)
#   define Zoom_GetPipe0OpSizeY()  READONLY(g_PipeState[0].u16_OutputSizeY)
#   define Zoom_GetPipe0FOVX()     READONLY(g_PipeState[0].f_FOVX)
#   define Zoom_GetPipe0FOVY()     READONLY(g_PipeState[0].f_FOVY)

/// Interface to query the current pipe1 X output size (it is valid only when pipe1 is enabled)
#   define Zoom_GetPipe1OpSizeX()  READONLY(g_PipeState[1].u16_OutputSizeX)

/// Interface to query the current pipe1 Y output size (it is valid only when pipe1 is enabled)
#   define Zoom_GetPipe1OpSizeY()  READONLY(g_PipeState[1].u16_OutputSizeY)
#   define Zoom_GetPipe1FOVX()     READONLY(g_PipeState[1].f_FOVX)
#   define Zoom_GetPipe1FOVY()     READONLY(g_PipeState[1].f_FOVY)

// Get FOV based on configured pipe and datapath
uint16_t Zoom_GetFOVX();
uint16_t    Zoom_GetFOVY (void);

#define  BMS_CROP_XSTART()                               READONLY((FrameDimension_GetCurrentOPXOutputSize() - (g_Zoom_Status.f_FOVX/FrameDimension_GetCurrentPreScaleFactor()))/2.0) 
#define  BMS_CROP_YSTART()                               READONLY((FrameDimension_GetCurrentOPYOutputSize() - (g_Zoom_Status.f_FOVY/FrameDimension_GetCurrentPreScaleFactor()))/2.0)  

#endif /*ZOOM_OPINTERFACE_H_*/


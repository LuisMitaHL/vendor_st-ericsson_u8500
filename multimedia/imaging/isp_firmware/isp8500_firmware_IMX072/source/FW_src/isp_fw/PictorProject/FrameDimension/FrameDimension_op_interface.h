/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\file FrameDimension_op_interface.h
\brief Export Frame Dimension Functions and macros to other module
\details  The file is part of release code and exports functions and macros which may be required
by other modules.The parameters exported by the module are read only i.e. no other module
should change the frame dimension parameters and external modules should only use the
exported macros in this file.
\ingroup Frame Dimension
*/
#ifndef FRAMEDIMENSION_OP_INTERFACE_H_
#   define FRAMEDIMENSION_OP_INTERFACE_H_

#   include "FrameDimension.h"

/**
\enum FDSensorScalingMode_te
\brief Scaling capabilities of sensor
\ingroup Frame Dimension
*/
typedef enum
{
    /// SMIA profile 0 sensor with no scaling support
    FDSensorScalingMode_e_SENSOR_SCALING_NONE,

    /// SMIA profile 1 sensor with only horizontal scaling
    FDSensorScalingMode_e_SENSOR_SCALING_HORIZONTAL_ONLY,

    /// SMIA profile 2 sensor with both horizontal and vertical scaling \n
    /// [NOTE]: Same scaling factor in both direction X and Y will be applied in sensor
    FDSensorScalingMode_e_SENSOR_SCALING_HORIZONTAL_AND_VERTICAL
} FDSensorScalingMode_te;

typedef enum
{
    FrameDimensionRequestStatus_e_Accepted = 1,
    FrameDimensionRequestStatus_e_Denied = 2,
    FrameDimensionRequestStatus_e_ReProgeamRequired = 3
} FrameDimensionRequestStatus_te;

/**
\struct FrameDimension_ts
\brief Specifies the various parameters of the frame dimension.
\ingroup Frame Dimension
*/
typedef struct
{
    /// Sensor scale/sub sample factor.
    /// [NOTE]: Scale factor can only be applied in steps of 1/16.
    float_t     f_PreScaleFactor;

    /// Videotiming frame length in lines.
    uint16_t    u16_VTFrameLengthLines;

    /// Videotiming line length in pixel clocks.
    uint16_t    u16_VTLineLengthPck;

    /// X co-ordinate of the top left corner of Window of Interest in full Field Of View.
    uint16_t    u16_VTXAddrStart;

    /// Y co-ordinate of the top left corner of Window of Interest in full Field Of View.
    uint16_t    u16_VTYAddrStart;

    /// X co-ordinate of the bottom right corner of Window of Interest in full Field Of View.
    uint16_t    u16_VTXAddrEnd;

    /// Y co-ordinate of the bottom right corner of Window of Interest in full Field Of View.
    uint16_t    u16_VTYAddrEnd;

    /// Horizontal size of Window Of Interest in output timing domain.
    uint16_t    u16_OPXOutputSize;

    /// Vertical size of Window Of Interest in output timing domain.
    uint16_t    u16_OPYOutputSize;

    /// Horizontal size of Window Of Interest in video timing domain.
    uint16_t    u16_VTXOutputSize;

    /// Vertical size of Window Of Interest in video timing domain.
    uint16_t    u16_VTYOutputSize;

    /// x_odd_inc value corresponding to the horizontal subsampling factor.
    /// x_even_inc == 1 always.
    uint16_t    u16_XOddInc;

    /// y_odd_inc value corresponding to the vertical subsampling factor.
    /// y_even_inc == 1 always.\n
    uint16_t    u16_YOddInc;

    /// Specifies the scale_m factor with respect to the current f_ScaleFactor.
    uint16_t    u16_Scaler_M;

    /// Number of non active columns at the left edge.
    uint16_t    u16_NumberOfNonActiveColumnsAtTheLeftEdge;

    /// Number of non active columns at the right edge.
    uint16_t    u16_NumberOfNonActiveColumnsAtTheRightEdge;

    /// Number of non active lines at the top edge of the frame
    uint16_t    u16_NumberofNonActiveLinesAtTopEdge;

    /// Number of non active lines at the bottom edge of the frame
    uint16_t    u16_NumberofNonActiveLinesAtBottomEdge;

    /// Number of status lines.
    uint8_t     u8_NumberOfStatusLines;

    /// Specifies whether sensor is scaling or subsampling.
    uint8_t     e_SensorPrescaleType;

    /// Specifies the sensor scaling mode.
    /// No scaling support is required for SMIA profile 0 sensor.
    /// Profile 1 sensor supports only horizontal scaling.
    /// Profile 2 sensor supports both horizontal and vertical scaling.
    /// [NOTE]: The scaling factor should be the same for both X and Y directions.
    uint8_t     e_FDSensorScalingMode;
} FrameDimension_ts;

/**
\struct FrameDimensionStatus_ts
\brief Specifies the frame timings.
\ingroup Frame Dimension
*/
typedef struct
{
    /// Specifies the line length (in us) in video timing domain.
    float_t     f_VTLineLength_us;

    /// Specifies the frame length(in us) in video timing domain.
    float_t     f_VTFrameLength_us;

    /// Specifies the current frame rate.
    float_t     f_CurrentFrameRate;

    /// Specifies the video timing frame length( in number of lines)
    /// that has to be applied to the sensor.
    uint16_t    u16_VTFrameLengthPending_Lines;

    /// Specifies the minimum line length corresponding to the given
    /// video timing X output size.
    uint16_t    u16_MinVTLineLengthAtCurrentVTXSize_Pixels;

    /// Specifies the minimum frame length corresponding to the given
    /// video timing Y output size.
    uint16_t    u16_MinVTFrameLengthAtCurrentVTYSize_Lines;

    /// Specifies the maximum X FOV that the sensor can provide at the
    /// current streaming parameters.
    uint16_t    u16_MaximumUsableSensorFOVX;

    /// Specifies the maximum Y FOV that the sensor can provide at the
    /// current streaming parameters.
    uint16_t    u16_MaximumUsableSensorFOVY;

    /// Specifies the shift in x_addr_start done to align the center of the readout
    /// with the center of the array. This is needed since readout constraints may
    /// reduce the total available FOV
    uint8_t     u8_TotalFOVXShift;

    /// Specifies the shift in y_addr_start done to align the center of the readout
    /// with the center of the array. This is needed since readout constraints may
    /// reduce the total available FOV
    uint8_t     u8_TotalFOVYShift;

    /// Part of u8_TotalFOVXShift done in Sensor
    uint8_t     u8_FOVXShiftInSensor;

    /// Part of u8_TotalFOVYShift done in Sensor
    uint8_t     u8_FOVYShiftInSensor;

    /// Specifies whether video timing frame length change is pending.
    uint8_t     e_Flag_IsFrameLengthChangePending;

    /// Flag to indicate whether a frame length change was inhibited
    /// due to insufficient frame length for coarse exposure.
    uint8_t     e_Flag_IsFrameLengthChangeInhibitedForCoarseExposure;
} FrameDimensionStatus_ts;

/**
\struct FrameConstraints_ts
\brief Specifies the sensor frame constraints.
\ingroup Frame Dimension
*/
typedef struct
{
    /// Minimum value of X coordinate of the top left
    /// corner of Window Of Interest in Full Field of View.
    uint16_t    u16_VTXAddrMin;

    /// Minimum value of Y coordinate of the top left
    /// corner of Window Of Interest in Full Field of View.
    uint16_t    u16_VTYAddrMin;

    /// Maximum value of X coordinate of the bottom right
    /// corner of Window Of Interest in Full Field of View.
    uint16_t    u16_VTXAddrMax;

    /// Maximum value of Y coordinate of the bottom right
    /// corner of Window Of Interest in Full Field of View.
    uint16_t    u16_VTYAddrMax;

    /// Minimum OP X output size.
    uint16_t    u16_MinOPXOutputSize;

    /// Minimum OP Y output size.
    uint16_t    u16_MinOPYOutputSize;

    /// Maximum OP X output size. It is also interpreted as the
    /// X size of the input image for bayer memory load operations.
    uint16_t    u16_MaxOPXOutputSize;

    /// Maximum OP Y output size. It is also interpreted as the
    /// Y size of the input image for bayer memory load operations.
    uint16_t    u16_MaxOPYOutputSize;

    /// Minimum value of video timing frame length (in lines).
    uint16_t    u16_MinVTFrameLengthLines;

    /// Maximum value of video timing frame length (in lines).
    uint16_t    u16_MaxVTFrameLengthLines;

    /// Minimum value of video timing line length (in pixel clocks).
    uint16_t    u16_MinVTLineLengthPck;

    /// Maximum value of video timing line length (in pixel clocks).
    uint16_t    u16_MaxVTLineLengthPck;

    /// Minimum value of video timing line blanking (in pixel clocks).
    uint16_t    u16_MinVTLineBlankingPck;

    /// Minimum value of video timing frame blanking (in lines).
    uint16_t    u16_MinVTFrameBlanking;

    /// Minimum value of scaler_m register.
    uint16_t    u16_ScalerMMin;

    /// Maximum value of scaler_m register.
    uint16_t    u16_ScalerMMax;

    /// Maximum value of odd inc value.
    uint16_t    u16_MaxOddInc;

    /// Specifies the sensor scaling mode (None, Horizonatal
    /// only and Full).
    uint8_t     e_SensorProfile;
} FrameConstraints_ts;

/**
\struct HostFrameConstraints_ts
\brief Specifies the frame constraints that apply in the output timing domain.
\ingroup Frame Dimension
*/
typedef struct
{
    /// Specifies the margin in field of view that will be maintained
    /// at each frame config request. Basically:
    /// provisioned_fov = requested_fov * f_FOVMargin
    /// f_FOVMargin should be greater than 1.0
    float_t     f_FOVMargin;

    /// Minimum interline pixel clocks required by the
    /// host beyond the GPS0.
    uint8_t     u8_MinimumPostScalar0LineBlanking_pixels;

    /// Minimum interline pixel clocks required by the
    /// host beyond the GPS1.
    uint8_t     u8_MinimumPostScalar1LineBlanking_pixels;

    /// Minimum interframe lines required at the sensor output
    uint8_t     u8_MinimumInterFrame_lines;

    /// Maximum pre scale factor allowed
    uint8_t     u8_MaximumPreScale;

    /// AC Frequency - used for flicker free time period calculations.
    /// This mains frequency determines the flicker free time period.
    uint8_t     u8_MainsFrequency_Hz;

    /// If this flag is set to TRUE, then it is ensured that
    /// the frame length will always have an integer number of
    /// flicker free bunches.
    uint8_t     e_Flag_GuaranteeStaticFlickerFrameLength;

    /// Specifies whether the host will program CurrentFrameDimension page,
    /// which will be used for Rx Test pattern streaming and also for BML
    /// operations, or the firmware will calculate the values.
    uint8_t     e_FrameDimensionProgMode;

    /// Specifies whether a sensor changeover (reprogram) is allowed or not
    uint8_t     e_Flag_AllowChangeOver;
} HostFrameConstraints_ts;

/**
\struct AntiFlicker_Status_ts
\brief Status page for Anti-Flicker.
\ingroup Frame Dimension
*/
typedef struct
{
    /// Specifies the flicker free period.
    float_t     f_FlickerFreePeriod_us;

    /// Specifies the gained flicker free period.
    /// It takes into account the minimum analog and digital gain values.
    float_t     f_GainedFlickerFreeTimePeriod_us;

    /// Specifies the maximum number of flicker free bunches
    /// possible at the current vt frame length.
    uint16_t    u16_MaxFlickerFreeBunches;
} AntiFlicker_Status_ts;

/**
\struct BML_Framedimension_ts
\brief Specifies the BML frame parameters
\ingroup Frame Dimension
*/
typedef struct
{
    /// Start offset of BML window width
    uint16_t    u16_BMLXOffset;

    /// Start offset of BML window height
    uint16_t    u16_BMLYOffset;

    /// Size of BML window width
    uint16_t    u16_BMLXSize;

    /// Size of BML window height
    uint16_t    u16_BMLYSize;

} BML_Framedimension_ts;


/*************** Page elements to be exported to user *********************/

/// Current Frame Dimension
///  g_CurrentFrameDimension    NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
extern FrameDimension_ts        g_CurrentFrameDimension TO_EXT_DATA_MEM;
extern FrameDimension_ts        g_RequestedFrameDimension TO_EXT_DATA_MEM;

/// Frame Dimension Status
/// g_FrameDimensionStatus      NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
extern FrameDimensionStatus_ts  g_FrameDimensionStatus TO_EXT_DATA_MEM;

/// Sensor Frame Constraints
/// g_SensorFrameConstraints    NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
extern FrameConstraints_ts      g_SensorFrameConstraints TO_EXT_DATA_MEM;


/// Host Frame Constraints
/// g_HostFrameConstraints      NON_MODE_STATIC_PAGE    READ_WRITE_PAGE
extern HostFrameConstraints_ts  g_HostFrameConstraints TO_EXT_DATA_MEM;

/// Anti Flicker Status
/// g_AntiFlicker_Status        NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
extern AntiFlicker_Status_ts    g_AntiFlicker_Status TO_EXT_DATA_MEM;

/// BML frame dimension status
/// g_BML_Framedimension        NON_MODE_STATIC_PAGE    READ_ONLY_PAGE
extern BML_Framedimension_ts   g_BML_Framedimension TO_EXT_DATA_MEM;



extern void                     FrameDimension_SetNewFrameLength (uint16_t u16_RequestedFrameLength);

extern void                     FrameDimension_UpdateCurrentVTFrameLength (void);

extern void                     FrameDimension_ComputeMaximumSensorFieldOfView (void)TO_EXT_DDR_PRGM_MEM;

/*************** Exported Preprocessing Macros *********************/

/// Line time( in us) of the sensor.
#   define FrameDimension_GetSensorLineTime_us()           READONLY(g_FrameDimensionStatus.f_VTLineLength_us)

/// Interface to expose the maximum usable sensor FOVX
#   define FrameDimension_GetMaxSensorFOVXAtCurrentParam() READONLY(g_FrameDimensionStatus.u16_MaximumUsableSensorFOVX)

/// Interface to expose the maximum usable sensor FOVY
#   define FrameDimension_GetMaxSensorFOVYAtCurrentParam() READONLY(g_FrameDimensionStatus.u16_MaximumUsableSensorFOVY)

/// Requested video timing line length.
#   define FrameDimension_GetRequestedVTLineLength()   READONLY(g_CurrentFrameDimension.u16_VTLineLengthPck)

/// Current Output domain X output size
#   define FrameDimension_GetCurrentOPXOutputSize()  READONLY(g_CurrentFrameDimension.u16_OPXOutputSize)

/// Requested Output Domain Y output size
#   define FrameDimension_GetRequestedOPYOutputSize()  READONLY(g_RequestedFrameDimension.u16_OPYOutputSize)

/// Current Output domain Y output size
#   define FrameDimension_GetCurrentOPYOutputSize()  READONLY(g_CurrentFrameDimension.u16_OPYOutputSize)

/// Requested current video timing line length.
#   define FrameDimension_GetCurrentVTLineLength()   READONLY(g_CurrentFrameDimension.u16_VTLineLengthPck)

/// Get minimum line blanking pixels required post scalar for pipe0
#   define FrameDimension_GetMinimumLineBlankingBeyondGPS0() \
        (g_HostFrameConstraints.u8_MinimumPostScalar0LineBlanking_pixels)

/// Get minimum line blanking pixels required post scalar for pipe1
#   define FrameDimension_GetMinimumLineBlankingBeyondGPS1() \
        (g_HostFrameConstraints.u8_MinimumPostScalar1LineBlanking_pixels)

#   define FrameDimension_GetCurrentVTXAddrStart()   (g_CurrentFrameDimension.u16_VTXAddrStart)

/// Get current Video Timing Y address start
#   define FrameDimension_GetCurrentVTYAddrStart()   (g_CurrentFrameDimension.u16_VTYAddrStart)

/// Set current Video Timing X address start
#   define FrameDimension_SetCurrentVTXAddrStart(x)   (g_CurrentFrameDimension.u16_VTXAddrStart = x)

/// Set current Video Timing Y address start
#   define FrameDimension_SetCurrentVTYAddrStart(x)   (g_CurrentFrameDimension.u16_VTYAddrStart = x)

/// Get sensor constant columns
#   define FrameDimension_GetSensorConstantCols()                               \
        (                                                                       \
            g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheLeftEdge + \
            g_CurrentFrameDimension.u16_NumberOfNonActiveColumnsAtTheRightEdge  \
        )

/// Get sensor constant rows
#   define FrameDimension_GetSensorConstantRows()                          \
        (                                                                  \
            g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtTopEdge +  \
            g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtBottomEdge \
        )

/// Exports the number of status lines in the sensor array for the current sensor
#define FrameDimension_GetNumberOfStatusLines() (g_CurrentFrameDimension.u8_NumberOfStatusLines)

///
#   define FrameDimension_GetNumberOfNonActiveLinesAtFrameTop() \
        (g_CurrentFrameDimension.u16_NumberofNonActiveLinesAtTopEdge)

/// Interface to query the maximum allowed pre scale factor
#define FrameDimension_GetMaxPreScale()     g_HostFrameConstraints.u8_MaximumPreScale

/// Interface to query the maximum X output size
#define FrameDimension_GetMaximumXOutputSize()  g_SensorFrameConstraints.u16_MaxOPXOutputSize

/// Interface to query the maximum Y output size
#define FrameDimension_GetMaximumYOutputSize()  g_SensorFrameConstraints.u16_MaxOPYOutputSize

/// Interface to query the requested pre scale factor
    #define FrameDimension_GetRequestedPreScaleFactor()     (1.0)

/// Interface to query the requested pre scale factor
    #define FrameDimension_GetCurrentPreScaleFactor()   (g_Zoom_Status_LLA.u16_woi_resX/g_Zoom_Status_LLA.u16_output_resX)

/// Interface to query the FOVX Shift to be applied in the ISP
/// It is the shift in FOV that could not be applied in the sensor
/// and hence must be applied in the ISP
#define FrameDimension_GetFOVXShiftToBeAppliedInISP()   (g_FrameDimensionStatus.u8_TotalFOVXShift - g_FrameDimensionStatus.u8_FOVXShiftInSensor)


/// Interface to query the FOVX Shift applied in the sensor
#define FrameDimension_GetFOVXShiftAppliedInSensor()    g_FrameDimensionStatus.u8_FOVXShiftInSensor

/// Interface to query the FOVY Shift to be applied in the ISP
/// It is the shift in FOV that could not be applied in the sensor
/// and hence must be applied in the ISP
#define FrameDimension_GetFOVYShiftToBeAppliedInISP()   (g_FrameDimensionStatus.u8_TotalFOVYShift - g_FrameDimensionStatus.u8_FOVYShiftInSensor)

/// Interface to query the FOVY Shift applied in the sensor
#define FrameDimension_GetFOVYShiftAppliedInSensor()    g_FrameDimensionStatus.u8_FOVYShiftInSensor


/// Interface to get the current frame dimension mode
#define FrameDimension_IsFDMModeAuto()              (FrameDimensionProgMode_e_Auto == g_HostFrameConstraints.e_FrameDimensionProgMode)

#endif /*FRAMEDIMENSION_OP_INTERFACE_H_*/


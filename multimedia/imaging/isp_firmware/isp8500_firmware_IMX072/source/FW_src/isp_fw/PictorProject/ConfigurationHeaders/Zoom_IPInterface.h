/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file  Zoom_IPInterface.h

 \brief This file is NOT a part of the scalar module release code.
        All inputs needed by the zoom module that can only
        be resolved at the project level (at integration time)
        are met through this file.
        It is the responsibility of the integrator to generate
        this file at integration time and meet all the input
        dependencies.

 \ingroup Zoom
*/
#ifndef ZOOM_IPINTERFACE_H_
#   define ZOOM_IPINTERFACE_H_

#   include "FrameDimension_op_interface.h"
#   include "SystemConfig.h"
#   include "HostInterface.h"
#   include "GenericFunctions.h"
#   include "SystemConfig.h"

/// Interface to allow the zoom to access the X component of pipe0 output size
#   define Zoom_GetPipe0OpSize_X() SystemConfig_GetPipe0OpSizeX()

/// Interface to allow the zoom to access the Y component of pipe0 output size
#   define Zoom_GetPipe0OpSize_Y() SystemConfig_GetPipe0OpSizeY()

/// Interface to allow the zoom to access the X component of pipe1 output size
#   define Zoom_GetPipe1OpSize_X() SystemConfig_GetPipe1OpSizeX()

/// Interface to allow the zoom to access the Y component of pipe1 output size
#   define Zoom_GetPipe1OpSize_Y() SystemConfig_GetPipe1OpSizeY()



/// Interface to allow the zoom to know if the ISP is streaming
#   define Zoom_IsStreaming()  Stream_IsISPInputStreaming()

/// Interface to allow the zoom to know the maximum user FOVX available
#   define Zoom_GetMaxUserFOVX()                                                    \
    SystemConfig_IsInputImageSourceSensor() ?                                       \
        (                                                                           \
            FrameDimension_GetMaxSensorFOVXAtCurrentParam() -                       \
                (                                                                   \
                    FrameDimension_GetMaxPreScale() *                               \
                        (                                                           \
                            COLUMNS_LOST_AT_LEFT_EDGE_DUE_TO_COLOUR_INTERPOLATION + \
                            COLUMNS_LOST_AT_RIGHT_EDGE_DUE_TO_COLOUR_INTERPOLATION  \
                        )                                                           \
                )                                                                   \
        ) :                                                                         \
            (                                                                       \
                 g_Zoom_Status_LLA.u16_woi_resX  -                                  \
                (                                                                   \
                    FrameDimension_GetMaxPreScale() *                               \
                        (                                                           \
                            COLUMNS_LOST_AT_LEFT_EDGE_DUE_TO_COLOUR_INTERPOLATION + \
                            COLUMNS_LOST_AT_RIGHT_EDGE_DUE_TO_COLOUR_INTERPOLATION  \
                        )                                                           \
                )                                                                   \
        )

/// Interface to allow the zoom to know the maximum user FOVY available
#   define Zoom_GetMaxUserFOVY()                                                 \
    SystemConfig_IsInputImageSourceSensor() ?                                    \
        (                                                                        \
            FrameDimension_GetMaxSensorFOVYAtCurrentParam() -                    \
                (                                                                \
                    FrameDimension_GetMaxPreScale() *                            \
                        (                                                        \
                            ROWS_LOST_AT_TOP_EDGE_DUE_TO_COLOUR_INTERPOLATION +  \
                            ROWS_LOST_AT_BOTTOM_EDGE_DUE_TO_COLOUR_INTERPOLATION \
                        )                                                        \
                )                                                                \
        ) :                                                                      \
            (                                                                    \
            g_Zoom_Status_LLA.u16_woi_resY -                                     \
                    (                                                            \
                    FrameDimension_GetMaxPreScale() *                            \
                        (                                                        \
                            ROWS_LOST_AT_TOP_EDGE_DUE_TO_COLOUR_INTERPOLATION +  \
                            ROWS_LOST_AT_BOTTOM_EDGE_DUE_TO_COLOUR_INTERPOLATION \
                        )                                                        \
                    )                                                            \
            )

/// Interface to fetch the minimum line blanking required after the GPS0
#   define Zoom_GetMinLineBlankingBeyondGPS0() FrameDimension_GetMinimumLineBlankingBeyondGPS0()

/// Interface to fetch the minimum line blanking required after the GPS1
#   define Zoom_GetMinLineBlankingBeyondGPS1() FrameDimension_GetMinimumLineBlankingBeyondGPS1()

/// Interface to provide the ceiling of a float value
#   define Zoom_Ceiling(f_Value)   GenericFunctions_Ceiling(f_Value)

/// Interface to provide the pending pre scale factor
/* TODO: <AG>: Get prescale code if needed based on FOV and pre-scale, Try to remove it if possible */


#   define Zoom_GetRequestedPreScaleFactorLLA()   (g_Zoom_Status_LLA.u16_Ex_woi_resX/g_Zoom_Status_LLA.u16_Ex_output_resX)

/// Interface to raise an event notification to indicate that a zoom frame config request was denied
#   define Zoom_ConfigRequestDeniedNotify()    EventManager_ZoomConfigRequestDenied_Notify()

/// Interface to raise an event notification to indicate that a zoom frame config request needs a sensor reprogram
#   define Zoom_ConfigRequestReprogramRequired()    EventManager_ZoomConfigRequestReprogramRequired_Notify()

/// Interface to raise an event notification to indicate that the last zoom command resulted in a set out of range
#   define Zoom_CommandStatusSetOutOfRangeNotify()   EventManager_ZoomSetOutOfRange_Notify()

/// Interface to raise an event notification to indicate that the last zoom command has completed and it is safe to issue a new zoom command
#   define Zoom_StepCompleteNotify()    EventManager_ZoomStepComplete_Notify(); \
                                        SystemConfig_SetZoomUpdateDone();



#define Zoom_GetMaxInputSizeXMinusISPLossAtMaxPreScale()        (FrameDimension_GetMaxSensorFOVXAtCurrentParam() - (FrameDimension_GetMaxPreScale() * (COLUMNS_LOST_AT_LEFT_EDGE_DUE_TO_COLOUR_INTERPOLATION + COLUMNS_LOST_AT_RIGHT_EDGE_DUE_TO_COLOUR_INTERPOLATION)))


#define Zoom_GetMaxInputSizeYMinusISPLossAtMaxPreScale()    (FrameDimension_GetMaxSensorFOVYAtCurrentParam() - (FrameDimension_GetMaxPreScale() * (ROWS_LOST_AT_TOP_EDGE_DUE_TO_COLOUR_INTERPOLATION + ROWS_LOST_AT_BOTTOM_EDGE_DUE_TO_COLOUR_INTERPOLATION)))


/// Interface to query the current sensor line length
#   define Zoom_GetCurrentSensorLineLength()   FrameDimension_GetCurrentVTLineLength()

/// Interface to provide service to the zoom module to request a particular frame dimension config
extern uint8_t  Zoom_Request (
                uint16_t    u16_DesiredFOVX,
                uint16_t    u16_DesiredFOVY,
                uint16_t    u16_RequiredLineLength,
                uint16_t    u16_MaxLineLength,
                int16_t     s16_CenterOffsetX,
                int16_t     s16_CenterOffsetY,
                uint8_t     e_Flag_ScaleLineLengthForDerating,
                uint8_t     e_Flag_EnableModeSelection);
#endif /*ZOOM_IPINTERFACE_H_*/


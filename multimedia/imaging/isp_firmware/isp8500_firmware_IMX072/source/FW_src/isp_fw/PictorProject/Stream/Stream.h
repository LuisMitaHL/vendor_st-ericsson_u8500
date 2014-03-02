/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file      Stream.h
 \brief     The stream manager header file. Contains interfaces that are
            to be exposed to other modules.

 \ingroup   Stream
*/
#ifndef STREAM_H_
#   define STREAM_H_

#   include "Platform.h"

typedef enum
{
    // Stream resource is Sensor
    StreamResource_e_Sensor,

    // Stream resource is Rx
    StreamResource_e_Rx
} StreamResource_te;

typedef enum
{
    // Command start
    StreamCommand_e_Start,

    // Command stop
    StreamCommand_e_Stop
} StreamCommand_te;

typedef enum
{
    // Source is stopped
    StreamResourceStatus_e_Stopped,

    // Source is starting
    StreamResourceStatus_e_Starting,

    // Source is waiting to start
    StreamResourceStatus_e_WaitingToStart,

    // Source is running
    StreamResourceStatus_e_Running,

    // Source is stopping
    StreamResourceStatus_e_Stopping,

    // Source is waiting to stop
    StreamResourceStatus_e_WaitingToStop
} StreamResourceStatus_te;

typedef struct
{
    /// Specifies the command to the sensor
    uint8_t e_StreamCommand_Sensor;

    /// Specifies the command to the Rx
    uint8_t e_StreamCommand_Rx;
} Stream_InputControl_ts;

typedef struct
{
    /// Specifies the current status of the sensor
    uint8_t e_StreamResourceStatus_Sensor;

    /// Specifies the current status of the Rx
    uint8_t e_StreamResourceStatus_Rx;

    /// Specifies whether RX Stop has been requested.
    uint8_t e_Flag_RxStopRequested;
} Stream_InputStatus_ts;

extern Stream_InputControl_ts           g_Stream_InputControl;
extern volatile Stream_InputStatus_ts   g_Stream_InputStatus;
extern volatile Flag_te                 e_Flag_FSC_Sent;

extern void                             Stream_StateMachine (void);

/// Command to the stream state machine to start the sensor, the Rx state remains unchanged
#   define Stream_StartSensor()    (g_Stream_InputControl.e_StreamCommand_Sensor = StreamCommand_e_Start)

/// Command to the stream state machine to start the Rx, the sensor state remains unchanged
#   define Stream_StartRx()    (g_Stream_InputControl.e_StreamCommand_Rx = StreamCommand_e_Start)

/// Command to the stream state machine to start the sensor and Rx
#   define Stream_StartSensorAndRx()                                      \
    g_Stream_InputControl.e_StreamCommand_Sensor = StreamCommand_e_Start; \
    g_Stream_InputControl.e_StreamCommand_Rx = StreamCommand_e_Start;

/// Command to the stream state machine to stop the sensor, the Rx state remains unchanged
#   define Stream_StopSensor() (g_Stream_InputControl.e_StreamCommand_Sensor = StreamCommand_e_Stop)

/// Command to the stream state machine to stop the Rx, the sensor state remains unchanged
#   define Stream_StopRx() (g_Stream_InputControl.e_StreamCommand_Rx = StreamCommand_e_Stop)

/// Command to the stream state machine stop the sensor and Rx
#   define Stream_StopSensorAndRx()                                      \
    g_Stream_InputControl.e_StreamCommand_Sensor = StreamCommand_e_Stop; \
    g_Stream_InputControl.e_StreamCommand_Rx = StreamCommand_e_Stop;

/// Interface to query if the ISP input is streaming
#   define Stream_IsISPInputStreaming()                                                              \
        (                                                                                            \
            (StreamResourceStatus_e_Running == g_Stream_InputStatus.e_StreamResourceStatus_Rx)       \
        ||  (StreamResourceStatus_e_WaitingToStop == g_Stream_InputStatus.e_StreamResourceStatus_Rx) \
        )

/// Interface to query if the sensor is streaming
#   define Stream_IsSensorStreaming()                                                                    \
        (                                                                                                \
            (StreamResourceStatus_e_Running == g_Stream_InputStatus.e_StreamResourceStatus_Sensor)       \
        ||  (StreamResourceStatus_e_WaitingToStop == g_Stream_InputStatus.e_StreamResourceStatus_Sensor) \
        )

// <Todo: BG> - Setting status PE from outside is not desirable..need to check if really required
#   define Stream_SetISPInputStatusToStop()  (g_Stream_InputStatus.e_StreamResourceStatus_Rx = StreamResourceStatus_e_Stopped)
#   define Stream_SetISPInputStatusToStreaming()  (g_Stream_InputStatus.e_StreamResourceStatus_Rx = StreamResourceStatus_e_Running)

#   define FD_IS_SENSOR_STREAMING()    Stream_IsSensorStreaming()
#endif /*STREAM_H_*/


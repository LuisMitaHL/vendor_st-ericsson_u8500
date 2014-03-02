/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  ITM ISP Interrupt Manager

 \details   This module provides an abstraction to all the top level modules that
            wish to use the ITM (ISP Interrupt Manager) to raise event notifications.
 \n         The ITM has 4 event numbers and each of these event numbers have 32
            event sources. Hence in all there are 4x32 event notifications that
            are possible through the ITM.
 \n         Apart from this, all the device interrupts like host comms, master cci etc.
            are managed through the ITM.

*/
#ifndef EVENTMANAGER_H_
#   define EVENTMANAGER_H_

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      EventManager.h

 \brief     This file exposes interfaces to allow other modules to raise event notifications
            through the ITM (ISP Interrupt Manager).

 \details   The following event notifications can be raised through the Event Manager
            - HOST_COMMS_READY
            - HOST_COMMS_OPERATION_COMPLETE
            - BOOT_COMPLETE
            - SLEEPING
            - WOKEN_UP

 \ingroup   ITM
 \endif
*/
#   include "PictorhwReg.h"
#   include "Platform.h"
/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct    Event0_test_ts
 \brief     structure to count Event0 events
 \ingroup   ITM
 \endif
*/
typedef struct
{
    uint16_t    u16_EVENT0_0_HOST_COMMS_READY;

    uint16_t    u16_EVENT0_1_HOST_COMMS_OPERATION_COMPLETE;

    uint16_t    u16_EVENT0_2_BOOT_COMPLETE;

    uint16_t    u16_EVENT0_3_SLEEPING;

    uint16_t    u16_EVENT0_4_WOKEN_UP;

    uint16_t    u16_EVENT0_5_ISP_STREAMING;

    uint16_t    u16_EVENT0_6_ISP_STOP;

    uint16_t    u16_EVENT0_7_SENSOR_STOP;

    uint16_t    u16_EVENT0_8_SENSOR_START;

    uint16_t    u16_EVENT0_9_ISP_LOAD_READY;

    uint16_t    u16_EVENT0_10_ZOOM_CONFIG_REQUEST_DENIED;

    uint16_t    u16_EVENT0_11_ZOOM_CONFIG_REQUEST_REPROGAM_REQUIRED;

    uint16_t    u16_EVENT0_12_ZOOM_STEP_COMPLETE;

    uint16_t    u16_EVENT0_13_ZOOM_SET_OUTOF_RANGE;

    uint16_t    u16_EVENT0_14_STREAMING_ERROR;

    uint16_t    u16_EVENT0_15_RESET_ISP_COMPLETE;

    uint16_t    u16_EVENT0_16_MASTER_I2C_ACCESS_FAILED;

    uint16_t    u16_EVENT0_17_GLACE_STATS_READY;

    uint16_t    u16_EVENT0_18_HISTOGRAM_STATS_READY;

    uint16_t    u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE;

    uint16_t    u16_EVENT0_20_AF_STATS_READY;

    uint16_t    u16_EVENT0_21_FLA_DRIVER_LENS_STOP;

    uint16_t    u16_EVENT0_22_ZOOM_OUTPUT_RESOLUTION_READY;

    uint16_t    u16_EVENT0_23_reserved01;

    uint16_t    u16_EVENT0_24_reserved02;

    uint16_t    u16_EVENT0_25_reserved03;

    uint16_t    u16_EVENT0_26_NVM_Export;

    uint16_t    u16_EVENT0_27_SensorTuning_Available;

    uint16_t    u16_EVENT0_28_Power_Notification;

    uint16_t    u16_EVENT0_29_SMS_DONE;

    uint16_t    u16_EVENT0_30_Sensor_Output_Mode_Export_Notification;

    uint16_t    u16_EVENT0_31_Valid_Frame_Notification;

  }Event0_test_ts;

/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct    Event1_test_ts
 \brief     structure to count Event1 events
 \ingroup   ITM
 \endif
*/
typedef struct
{
    uint16_t    u16_EVENT1_0_HOST_TO_MASTER_I2C_ACCESS;

    uint16_t    u16_EVENT1_1_SENSOR_COMMIT;

    uint16_t    u16_EVENT1_2_ISP_COMMIT;

    uint16_t    u16_EVENT1_3_HR_GAMMA_UPDATE_COMPLETE;

    uint16_t    u16_EVENT1_4_LR_GAMMA_UPDATE_COMPLETE;


}Event1_test_ts;

/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct    Event2_test_ts
 \brief     structure to count Event2 events
 \ingroup   ITM
 \endif
*/
typedef struct
{
    uint16_t    u16_Reserved;
}Event2_test_ts;

/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct    Event3_test_ts
 \brief     structure to count Event3 events
 \ingroup   ITM
 \endif
*/
typedef struct
{
    uint16_t    u16_EVENT3_0_DMA_GRAB_NOK;

    uint16_t    u16_EVENT3_0_DMA_GRAB_OK;

    uint16_t    u16_EVENT3_1_DMA_GRAB_VideoStab;

    uint16_t    u16_EVENT3_2_DMA_GRAB_Abort;
}Event3_test_ts;

extern Event0_test_ts  g_Event0_Count TO_EXT_DATA_MEM;
extern Event1_test_ts  g_Event1_Count TO_EXT_DATA_MEM;
extern Event2_test_ts  g_Event2_Count TO_EXT_DATA_MEM;
extern Event3_test_ts  g_Event3_Count TO_EXT_DATA_MEM;

/// Top level macro to enable event notification corresponding to event number 'EventNumber' and source number 'SourceNumber'
#   define EventManager_Enable(EventNumber, SourceNumber) \
    Set_ITM_ITM_FW_EVENT##EventNumber##_EN_BSET_word(     \
    1 << SourceNumber)

/// Top level macro to disable event notification corresponding to event number 'EventNumber' and source number 'SourceNumber'
#   define EventManager_Disable(EventNumber, SourceNumber) \
    Set_ITM_ITM_FW_EVENT##EventNumber##_EN_BCLR_word(      \
    1 << SourceNumber)

/// Top level macro to raise event notification corresponding to event number 'EventNumber' and source number 'SourceNumber'
#   define EventManager_Notify(EventNumber, SourceNumber) \
    Set_ITM_ITM_FW_EVENT##EventNumber##_STATUS_BSET_word( \
    1 << SourceNumber)


//Exported Function declaration-----------------------------------------------

// Host Comms Ready                 : Event_0_0
void EventManager_HostCommsReady_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_HostCommsReady_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_HostCommsReady_Notify(void);

// Host Comms Operation Complete    : Event_0_1
void EventManager_HostCommsOperationComplete_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_HostCommsOperationComplete_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_HostCommsOperationComplete_Notify(void);

// Boot Complete    : Event_0_2
void EventManager_BootComplete_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_BootComplete_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_BootComplete_Notify(void);

// Sleeping : Event_0_3
void EventManager_Sleeping_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_Sleeping_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_Sleeping_Notify(void);

// Woken Up : Event_0_4
void EventManager_WokenUp_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_WokenUp_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_WokenUp_Notify(void);

// ISP Streaming    : Event_0_5
void EventManager_ISPStreaming_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_ISPStreaming_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_ISPStreaming_Notify(void);

// ISP Stop Streaming   : Event_0_6
void EventManager_ISPStopStreaming_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_ISPStopStreaming_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_ISPStopStreaming_Notify(void);

// Sensor Stop Streaming    : Event_0_7
void EventManager_SensorStopStreaming_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_SensorStopStreaming_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_SensorStopStreaming_Notify(void);

// Sensor Start Streaming   : Event_0_8
void EventManager_SensorStartStreaming_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_SensorStartStreaming_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_SensorStartStreaming_Notify(void);


// ISP Load Ready : Event_0_9
void EventManager_ISPLoadReady_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_ISPLoadReady_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_ISPLoadReady_Notify(void);

// Zoom Config Request Denied : Event_0_10
void EventManager_ZoomConfigRequestDenied_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_ZoomConfigRequestDenied_Disable(void );
void EventManager_ZoomConfigRequestDenied_Notify(void );

// Zoom Config Reprogram Required : Event_0_11
void EventManager_ZoomConfigRequestReprogramRequired_Enable(void );
void EventManager_ZoomConfigRequestReprogramRequired_Disable(void );
void EventManager_ZoomConfigRequestReprogramRequired_Notify(void );

// Zoom Step Complete : Event_0_12
void EventManager_ZoomStepComplete_Enable(void );
void EventManager_ZoomStepComplete_Disable(void );
void EventManager_ZoomStepComplete_Notify(void );

// Zoom Set Out of Range : Event_0_13
void EventManager_ZoomSetOutOfRange_Enable(void );
void EventManager_ZoomSetOutOfRange_Disable(void );
void EventManager_ZoomSetOutOfRange_Notify(void );

// Streaming Error : Event_0_14
void EventManager_StreamingError_Enable(void );
void EventManager_StreamingError_Disable(void );
void EventManager_StreamingError_Notify(void );

// Reset ISP Complete : Event_0_15
void EventManager_ResestISPComplete_Enable(void );
void EventManager_ResestISPComplete_Disable(void );
void EventManager_ResestISPComplete_Notify(void );


// Master I2C Access Failure   : Event_0_16
void EventManager_MasterI2CAccessFailed_Enable(void );
void EventManager_MasterI2CAccessFailed_Disable(void );
void EventManager_MasterI2CAccessFailed_Notify(void );

// Glace Statistics Ready   : Event_0_17
void EventManager_GlaceStatsReady_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_GlaceStatsReady_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_GlaceStatsReady_Notify(void );


// Histogram Statistics Ready   : Event_0_18
void EventManager_HistogramStatsReady_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_HistogramStatsReady_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_HistogramStatsReady_Notify(void );

// ExposureAndWbParametersUpdated   : Event_0_19
void EventManager_ExposureAndWbParametersUpdatedy_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_ExposureAndWbParametersUpdated_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_ExposureAndWbParametersUpdated_Notify(void );

// AutoFocus Stats Ready    : Event_0_20
void EventManager_AFStatsReady_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_AFStatsReady_Disable(void );
void EventManager_AFStatsReady_Notify(void );

// FLADriver Lens Movement Stoped     : Event_0_21
void EventManager_FLADRiverLensStop_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_FLADRiverLensStop_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_FLADRiverLensStop_Notify(void );

// Zoom New Output Image Resolution Ready   : Event_0_22
void EventManager_ZoomOutputResolutionReady_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_ZoomOutputResolutionReady_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_ZoomOutputResolutionReady_Notify(void );

// Pipe0 u16_EVENT0_23_reserved01   : Event_0_23
void EventManager_u16_EVENT0_23_reserved01_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_u16_EVENT0_23_reserved01_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_u16_EVENT0_23_reserved01_Notify(void );

// Pipe1 u16_EVENT0_24_reserved01   : Event_0_24
void EventManager_u16_EVENT0_24_reserved02_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_u16_EVENT0_24_reserved02_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_u16_EVENT0_24_reserved02_Notify(void );

// u16_EVENT0_25_reserved03   : Event_0_25
void EventManager_u16_EVENT0_25_reserved03_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_u16_EVENT0_25_reserved03_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_u16_EVENT0_25_reserved03_Notify(void );

//  NVM Data Export Notification  : Event_0_26
void EventManager_NVM_Export_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_NVM_Export_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_NVM_Export_Notify(void );

//  Sensor_tuning  Programmed Notification  : Event_0_27
void EventManager_sensor_tuning_Available_Enable(void)TO_EXT_DDR_PRGM_MEM;
void EventManager_sensor_tuning_Available_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_sensor_tuning_Available_Notify(void );

//  Power Event Notification  : Event_0_28
void EventManager_Power_Notification_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_Power_Notification_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_Power_Notification_Notify(void );

//  SMS Notification  : Event_0_29
void EventManager_SMS_Notification_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_SMS_Notification_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_SMS_Notification_Notify(void );

//  Sensor output mode export Notification  : Event_0_30
void EventManager_Sensor_Output_Mode_Export_Notification_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_Sensor_Output_Mode_Export_Notification_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_Sensor_Output_Mode_Export_Notification_Notify(void );

//  Valid Frame Notification  : Event_0_31
void EventManager_Valid_Frame_Received_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_Valid_Frame_Received_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_Valid_Frame_Received_Notify(void );

// Host To Master I2C Access    : Event_1_0
void EventManager_HostToMasterI2CAccess_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_HostToMasterI2CAccess_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_HostToMasterI2CAccess_Notify(void );

// Sensor Commit Notification     : Event_1_1
void EventManager_SensorCommit_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_SensorCommit_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_SensorCommit_Notify(void);

// ISP Commit Notification   : Event_1_2
void EventManager_ISPCommit_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_ISPCommit_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_ISPCommit_Notify(void);


// Pipe0 GAMMA UPDATE COMPLETE     : Event_1_3   //HR
void EventManager_GammaUpdateCompletePipe0_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_GammaUpdateCompletePipe0_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_GammaUpdateCompletePipe0_Notify(void);

// Pipe1 GAMMA UPDATE COMPLETE     : Event_1_4   //LR
void EventManager_GammaUpdateCompletePipe1_Enable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_GammaUpdateCompletePipe1_Disable(void) TO_EXT_DDR_PRGM_MEM;
void EventManager_GammaUpdateCompletePipe1_Notify(void);




// DMA_GRAB Frame Ready Notification  : Event_3_0
void EventManager_DMA_GRAB_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_DMA_GRAB_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_DMA_GRAB_Notify_NOK(void );
void EventManager_DMA_GRAB_Notify_OK(void );

// VideoStab Frame Ready Notification  : Event_3_1
void EventManager_DMA_GRAB_VideoStab_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_DMA_GRAB_VideoStab_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_DMA_GRAB_VideoStab_Notify(void );

// VideoStab Frame Ready Notification  : Event_3_2
void EventManager_DMA_GRAB_Abort_Enable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_DMA_GRAB_Abort_Disable(void )TO_EXT_DDR_PRGM_MEM;
void EventManager_DMA_GRAB_Abort_Notify(void );

#endif /*EVENTMANAGER_H_*/


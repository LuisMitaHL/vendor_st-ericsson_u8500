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
//#include "Platform.h"
#include "EventManager.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_ITM_EventManagerTraces.h"
#endif

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
extern Event0_test_ts   g_Event0_Count;
extern Event1_test_ts   g_Event1_Count;
extern Event2_test_ts   g_Event2_Count;
extern Event3_test_ts   g_Event3_Count;

// Host Comms Ready                 : Event_0_0

// ============================================

/// Function to enable HOST_COMMS_READY event notification
void
EventManager_HostCommsReady_Enable(void)
{
    EventManager_Enable(0, 0);
    OstTraceInt0(TRACE_DEBUG, "<EVT>HCR Enable");
}


/// Function to disable HOST_COMMS_READY event notification
void
EventManager_HostCommsReady_Disable(void)
{
    EventManager_Disable(0, 0);
    OstTraceInt0(TRACE_DEBUG, "<EVT>HCR Disable");
}


/// Function the raise HOST_COMMS_READY event notification
void
EventManager_HostCommsReady_Notify(void)
{
    EventManager_Notify(0, 0);
    g_Event0_Count.u16_EVENT0_0_HOST_COMMS_READY++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>HCR Notify: %d", g_Event0_Count.u16_EVENT0_0_HOST_COMMS_READY);
}


// Host Comms Operation Complete    : Event_0_1

// ============================================

/// Function to enable HOST_COMMS_OPERATION_COMPLETE event notification
void
EventManager_HostCommsOperationComplete_Enable(void)
{
    EventManager_Enable(0, 1);
    OstTraceInt0(TRACE_DEBUG, "<EVT>HCO Enable");
}


/// Function to disable HOST_COMMS_OPERATION_COMPLETE event notification
void
EventManager_HostCommsOperationComplete_Disable(void)
{
    EventManager_Disable(0, 1);
    OstTraceInt0(TRACE_DEBUG, "<EVT>HCO Disable");
}


/// Function the raise HOST_COMMS_OPERATION_COMPLETE event notification
void
EventManager_HostCommsOperationComplete_Notify(void)
{
    EventManager_Notify(0, 1);
    g_Event0_Count.u16_EVENT0_1_HOST_COMMS_OPERATION_COMPLETE++;
//    OstTraceInt1(TRACE_DEBUG, "<EVT>HCO: %d", g_Event0_Count.u16_EVENT0_1_HOST_COMMS_OPERATION_COMPLETE);
}


// Boot Complete    : Event_0_2

// ============================

/// Function to enable BOOT_COMPLETE event notification
void
EventManager_BootComplete_Enable(void)
{
    EventManager_Enable(0, 2);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Boot Complete Enable");
}


/// Function to disable BOOT_COMPLETE event notification
void
EventManager_BootComplete_Disable(void)
{
    EventManager_Disable(0, 2);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Boot Complete Disable");
}


/// Function to raise BOOT_COMPLETE event notification
void
EventManager_BootComplete_Notify(void)
{
    EventManager_Notify(0, 2);
    g_Event0_Count.u16_EVENT0_2_BOOT_COMPLETE++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Boot Complete Notify: %d", g_Event0_Count.u16_EVENT0_2_BOOT_COMPLETE);
}


// Sleeping : Event_0_3

// ====================

/// Function to enable SLEEPING event notification
void
EventManager_Sleeping_Enable(void)
{
    EventManager_Enable(0, 3);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Sleeping Enable");
}


/// Function to disable SLEEPING event notification
void
EventManager_Sleeping_Disable(void)
{
    EventManager_Disable(0, 3);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Sleeping Disable");
}


/// Function to raise SLEEPING event notification
void
EventManager_Sleeping_Notify(void)
{
    EventManager_Notify(0, 3);
    g_Event0_Count.u16_EVENT0_3_SLEEPING++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Sleeping Notify: %d", g_Event0_Count.u16_EVENT0_3_SLEEPING);
}


// Woken Up : Event_0_4

// ====================

/// Function to enable WOKEN_UP event notification
void
EventManager_WokenUp_Enable(void)
{
    EventManager_Enable(0, 4);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Woken up Enable");
}


/// Function to disable WOKEN_UP event notification
void
EventManager_WokenUp_Disable(void)
{
    EventManager_Disable(0, 4);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Woken up Disable");
}


/// Function to raise WOKEN_UP event notification
void
EventManager_WokenUp_Notify(void)
{
    EventManager_Notify(0, 4);
    g_Event0_Count.u16_EVENT0_4_WOKEN_UP++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Woken up Notify: %d", g_Event0_Count.u16_EVENT0_4_WOKEN_UP);
}


// ISP Streaming    : Event_0_5

// ============================

/// Function to enable ISP Streaming event notification
void
EventManager_ISPStreaming_Enable(void)
{
    EventManager_Enable(0, 5);
    OstTraceInt0(TRACE_DEBUG, "<EVT>ISP Streaming Enable");
}


/// Function to disable ISP Streaming event notification
void
EventManager_ISPStreaming_Disable(void)
{
    EventManager_Disable(0, 5);
    OstTraceInt0(TRACE_DEBUG, "<EVT>ISP Streaming Disable");
}


/// Function to raise ISP Streaming event notification
void
EventManager_ISPStreaming_Notify(void)
{
    EventManager_Notify(0, 5);
    g_Event0_Count.u16_EVENT0_5_ISP_STREAMING++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>ISP Streaming Notify: %d", g_Event0_Count.u16_EVENT0_5_ISP_STREAMING);
}


// ISP Stop Streaming   : Event_0_6

// ================================

/// Function to enable ISP Stop Streaming event notification
void
EventManager_ISPStopStreaming_Enable(void)
{
    EventManager_Enable(0, 6);
    OstTraceInt0(TRACE_DEBUG, "<EVT>ISP Stop Streaming Enable");
}


/// Function to disable ISP Stop Streaming event notification
void
EventManager_ISPStopStreaming_Disable(void)
{
    EventManager_Disable(0, 6);
    OstTraceInt0(TRACE_DEBUG, "<EVT>ISP Stop Streaming Disable");
}


/// Function to raise ISP Stop Streaming event notification
void
EventManager_ISPStopStreaming_Notify(void)
{
    EventManager_Notify(0, 6);
    g_Event0_Count.u16_EVENT0_6_ISP_STOP++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>ISP Stop Streaming Notify: %d", g_Event0_Count.u16_EVENT0_6_ISP_STOP);
}


// Sensor Stop Streaming    : Event_0_7

// ====================================

/// Function to enable Sensor Stop Streaming event notification
void
EventManager_SensorStopStreaming_Enable(void)
{
    EventManager_Enable(0, 7);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Sensor Stop Streaming Enable");
}


/// Function to disable Sensor Stop Streaming event notification
void
EventManager_SensorStopStreaming_Disable(void)
{
    EventManager_Disable(0, 7);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Sensor Stop Streaming Disable");
}


/// Function to raise Sensor Stop Streaming event notification
void
EventManager_SensorStopStreaming_Notify(void)
{
    EventManager_Notify(0, 7);
    g_Event0_Count.u16_EVENT0_7_SENSOR_STOP++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Sensor Stop Streaming Notify: %d", g_Event0_Count.u16_EVENT0_7_SENSOR_STOP);
}


// Sensor Start Streaming   : Event_0_8

// ====================================

/// Function to enable Sensor Stop Streaming event notification
void
EventManager_SensorStartStreaming_Enable(void)
{
    EventManager_Enable(0, 8);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Sensor Start Streaming Enable");
}


/// Function to disable Sensor Stop Streaming event notification
void
EventManager_SensorStartStreaming_Disable(void)
{
    EventManager_Disable(0, 8);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Sensor Start Streaming Disable");
}


/// Function to raise Sensor Stop Streaming event notification
void
EventManager_SensorStartStreaming_Notify(void)
{
    EventManager_Notify(0, 8);
    g_Event0_Count.u16_EVENT0_8_SENSOR_START++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Sensor Start Streaming Notify: %d", g_Event0_Count.u16_EVENT0_8_SENSOR_START);
}


// ISP Load Ready : Event_0_9

// ==========================

/// Function to enable ISP load ready event notification
void
EventManager_ISPLoadReady_Enable(void)
{
    EventManager_Enable(0, 9);
    OstTraceInt0(TRACE_DEBUG, "<EVT>ISP Load Ready Enable");
}


/// Function to disable ISP load ready event notification
void
EventManager_ISPLoadReady_Disable(void)
{
    EventManager_Disable(0, 9);
    OstTraceInt0(TRACE_DEBUG, "<EVT>ISP Load Ready Disable");
}


/// Function to raise ISP load ready event notification
void
EventManager_ISPLoadReady_Notify(void)
{
    EventManager_Notify(0, 9);
    g_Event0_Count.u16_EVENT0_9_ISP_LOAD_READY++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>ISP Load Ready Notify: %d", g_Event0_Count.u16_EVENT0_9_ISP_LOAD_READY);
}


// Zoom Config Request Denied : Event_0_10

// =======================================
/// Function to enable Zoom Config Request Denied event notification
void
EventManager_ZoomConfigRequestDenied_Enable(void)
{
    EventManager_Enable(0, 10);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Zoom Config Request Denied Enable");
}


/// Function to disable Zoom Config Request Denied event notification
void
EventManager_ZoomConfigRequestDenied_Disable(void)
{
    EventManager_Disable(0, 10);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Zoom Config Request Denied Disable");
}


/// Function to raise Zoom Config Request Denied event notification
void
EventManager_ZoomConfigRequestDenied_Notify(void)
{
    EventManager_Notify(0, 10);
    g_Event0_Count.u16_EVENT0_10_ZOOM_CONFIG_REQUEST_DENIED++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Zoom Config Request Denied Notify: %d", g_Event0_Count.u16_EVENT0_10_ZOOM_CONFIG_REQUEST_DENIED);
}


// Zoom Config Reprogram Required : Event_0_11

// ===========================================
/// Function to enable Zoom Config Reprogram Required event notification
void
EventManager_ZoomConfigRequestReprogramRequired_Enable(void)
{
    EventManager_Enable(0, 11);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Zoom Config Request Reprogram Required Enable");
}


/// Function to disable Zoom Config Request Denied event notification
void
EventManager_ZoomConfigRequestReprogramRequired_Disable(void)
{
    EventManager_Disable(0, 11);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Zoom Config Request Reprogram Required Disable");
}


/// Function to raise Zoom Config Request Denied event notification
void
EventManager_ZoomConfigRequestReprogramRequired_Notify(void)
{
    EventManager_Notify(0, 11);
    g_Event0_Count.u16_EVENT0_11_ZOOM_CONFIG_REQUEST_REPROGAM_REQUIRED++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Zoom Config Request Reprogram Required Notify: %d", g_Event0_Count.u16_EVENT0_11_ZOOM_CONFIG_REQUEST_REPROGAM_REQUIRED);
}


// Zoom Step Complete : Event_0_12

// ===============================
/// Function to enable Zoom Step Complete event notification
void
EventManager_ZoomStepComplete_Enable(void)
{
    EventManager_Enable(0, 12);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Zoom Step Complete Enable");
}


/// Function to disable Zoom Step Complete event notification
void
EventManager_ZoomStepComplete_Disable(void)
{
    EventManager_Disable(0, 12);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Zoom Step Complete Disable");
}


/// Function to raise Zoom Step Complete event notification
void
EventManager_ZoomStepComplete_Notify(void)
{
    EventManager_Notify(0, 12);
    g_Event0_Count.u16_EVENT0_12_ZOOM_STEP_COMPLETE++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Zoom Step Complete Notify: %d", g_Event0_Count.u16_EVENT0_12_ZOOM_STEP_COMPLETE);
}


// Zoom Set Out of Range : Event_0_13

// ==================================
/// Function to enable Zoom Set Out of Range event notification
void
EventManager_ZoomSetOutOfRange_Enable(void)
{
    EventManager_Enable(0, 13);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Zoom Set Out of Range Enable");
}


/// Function to disable Zoom Set Out of Range event notification
void
EventManager_ZoomSetOutOfRange_Disable(void)
{
    EventManager_Disable(0, 13);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Zoom Set Out of Range Disable");
}


/// Function to raise Zoom Set Out of Range event notification
void
EventManager_ZoomSetOutOfRange_Notify(void)
{
    EventManager_Notify(0, 13);
    g_Event0_Count.u16_EVENT0_13_ZOOM_SET_OUTOF_RANGE++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Zoom Set Out of Range Notify: %d", g_Event0_Count.u16_EVENT0_13_ZOOM_SET_OUTOF_RANGE);
}


// Streaming Error : Event_0_14

// ============================
/// Function to enable Streaming Error event notification
void
EventManager_StreamingError_Enable(void)
{
    EventManager_Enable(0, 14);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Streaming Error Enable");
}


/// Function to disable Streaming Error event notification
void
EventManager_StreamingError_Disable(void)
{
    EventManager_Disable(0, 14);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Streaming Error Disable");
}


/// Function to raise Streaming Error event notification
void
EventManager_StreamingError_Notify(void)
{
    EventManager_Notify(0, 14);
    g_Event0_Count.u16_EVENT0_14_STREAMING_ERROR++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Streaming Error Notify: %d", g_Event0_Count.u16_EVENT0_14_STREAMING_ERROR);
}


// Reset ISP Complete : Event_0_15

// ====================================
/// Function to enable Reset ISP Complete event notification
void
EventManager_ResestISPComplete_Enable(void)
{
    EventManager_Enable(0, 15);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Reset ISP Complete Enable");
}


/// Function to disable Reset ISP Complete event notification
void
EventManager_ResestISPComplete_Disable(void)
{
    EventManager_Disable(0, 15);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Reset ISP Complete Disable");
}


/// Function to raise Reset ISP Complete event notification
void
EventManager_ResestISPComplete_Notify(void)
{
    EventManager_Notify(0, 15);
    g_Event0_Count.u16_EVENT0_15_RESET_ISP_COMPLETE++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Reset ISP Complete Notify: %d", g_Event0_Count.u16_EVENT0_15_RESET_ISP_COMPLETE);
}


// Master I2C Access Failure   : Event_0_16

// ====================================
/// Function to enable Master I2C Access Failed event notification
void
EventManager_MasterI2CAccessFailed_Enable(void)
{
    EventManager_Enable(0, 16);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Master I2C Access Failed Enable");
}


/// Function to disable Master I2C Access Failed event notification
void
EventManager_MasterI2CAccessFailed_Disable(void)
{
    EventManager_Disable(0, 16);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Master I2C Access Failed Disable");
}


/// Function to raise Master I2C Access Failed event notification
void
EventManager_MasterI2CAccessFailed_Notify(void)
{
    EventManager_Notify(0, 16);
    g_Event0_Count.u16_EVENT0_16_MASTER_I2C_ACCESS_FAILED++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Master I2C Access Failed Notify: %d", g_Event0_Count.u16_EVENT0_16_MASTER_I2C_ACCESS_FAILED);
}


// Glace Statistics Ready   : Event_0_17

// =====================================
/// Function to enable Glace Statistics Ready event notification
void
EventManager_GlaceStatsReady_Enable(void)
{
    EventManager_Enable(0, 17);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Glace Stats Ready Enable");
}


/// Function to disable Glace Statistics Ready event notification
void
EventManager_GlaceStatsReady_Disable(void)
{
    EventManager_Disable(0, 17);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Glace Stats Ready Disable");
}


/// Function to raise Glace Statistics Ready event notification
void
EventManager_GlaceStatsReady_Notify(void)
{
    EventManager_Notify(0, 17);
    g_Event0_Count.u16_EVENT0_17_GLACE_STATS_READY++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Glace Stats Ready Notify: %d", g_Event0_Count.u16_EVENT0_17_GLACE_STATS_READY);
}


// Histogram Statistics Ready   : Event_0_18

// =====================================
/// Function to enable Histogram Statistics Ready event notification
void
EventManager_HistogramStatsReady_Enable(void)
{
    EventManager_Enable(0, 18);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Histogram Stats Ready Enable");
}


/// Function to disable Histogram Statistics Ready event notification
void
EventManager_HistogramStatsReady_Disable(void)
{
    EventManager_Disable(0, 18);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Histogram Stats Ready Disable");
}


/// Function to raise Histogram Statistics Ready event notification
void
EventManager_HistogramStatsReady_Notify(void)
{
    EventManager_Notify(0, 18);
    g_Event0_Count.u16_EVENT0_18_HISTOGRAM_STATS_READY++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Histogram Stats Ready Notify: %d", g_Event0_Count.u16_EVENT0_18_HISTOGRAM_STATS_READY);
}


// ExposureAndWbParametersUpdated   : Event_0_19

// =====================================
/// Function to enable ExposureAndWbParametersUpdated event notification
void
EventManager_ExposureAndWbParametersUpdatedy_Enable(void)
{
    EventManager_Enable(0, 19);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Exposure And Wb Parameters Updated Enable");
}


/// Function to disable ExposureAndWbParametersUpdated event notification
void
EventManager_ExposureAndWbParametersUpdated_Disable(void)
{
    EventManager_Disable(0, 19);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Exposure And Wb Parameters Updated Disable");
}


/// Function to raise ExposureAndWbParametersUpdated event notification
void
EventManager_ExposureAndWbParametersUpdated_Notify(void)
{
    EventManager_Notify(0, 19);
    g_Event0_Count.u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Exposure And Wb Parameters Updated Notify: %d", g_Event0_Count.u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE);
}


// AutoFocus Stats Ready    : Event_0_20

// =====================================
/// Function to enable AFStats Ready event notification
void
EventManager_AFStatsReady_Enable(void)
{
    EventManager_Enable(0, 20);
    OstTraceInt0(TRACE_DEBUG, "<EVT>AF Stats Ready Enable");
}


/// Function to disable AFStats Ready event notification
void
EventManager_AFStatsReady_Disable(void)
{
    EventManager_Disable(0, 20);
    OstTraceInt0(TRACE_DEBUG, "<EVT>AF Stats Ready Disable");
}


/// Function to raise AFStats Ready  event notification
void
EventManager_AFStatsReady_Notify(void)
{
    EventManager_Notify(0, 20);
    g_Event0_Count.u16_EVENT0_20_AF_STATS_READY++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>AF Stats Ready Notify: %d", g_Event0_Count.u16_EVENT0_20_AF_STATS_READY);
}


// FLADriver Lens Movement Stoped     : Event_0_21

// =====================================
/// Function for FLADriver Lens Stop Enable  notification
void
EventManager_FLADRiverLensStop_Enable(void)
{
    EventManager_Enable(0, 21);
    OstTraceInt0(TRACE_DEBUG, "<EVT>FLA Driver Lens Stop Enable");
}


/// Function for FLADriver Lens Stop disable notification
void
EventManager_FLADRiverLensStop_Disable(void)
{
    EventManager_Disable(0, 21);
    OstTraceInt0(TRACE_DEBUG, "<EVT>FLA Driver Lens Stop Disable");
}


/// Function to raise FLADriver Lens Stop event notification
void
EventManager_FLADRiverLensStop_Notify(void)
{
    EventManager_Notify(0, 21);
    g_Event0_Count.u16_EVENT0_21_FLA_DRIVER_LENS_STOP++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>FLA Driver Lens Stop Notify: %d", g_Event0_Count.u16_EVENT0_21_FLA_DRIVER_LENS_STOP);
}


// Zoom New Output Image Resolution Ready   : Event_0_22

// =====================================================
/// Function to enable ZoomOutputResolutionReady event notification
void
EventManager_ZoomOutputResolutionReady_Enable(void)
{
    EventManager_Enable(0, 22);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Zoom Output Resolution Ready Enable");
}


/// Function to disable ZoomOutputResolutionReady event notification
void
EventManager_ZoomOutputResolutionReady_Disable(void)
{
    EventManager_Disable(0, 22);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Zoom Output Resolution Ready Disable");
}


/// Function to raise ZoomOutputResolutionReady event notification
void
EventManager_ZoomOutputResolutionReady_Notify(void)
{
    EventManager_Notify(0, 22);
    g_Event0_Count.u16_EVENT0_22_ZOOM_OUTPUT_RESOLUTION_READY++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Zoom Output Resolution Ready Notify: %d", g_Event0_Count.u16_EVENT0_22_ZOOM_OUTPUT_RESOLUTION_READY);
}


// Pipe0 u16_EVENT0_23_reserved01   : Event_0_23

// ==================================================
/// Function to enable u16_EVENT0_23_reserved01 event notification
void
EventManager_u16_EVENT0_23_reserved01_Enable(void)
{
    EventManager_Enable(0, 23);
    OstTraceInt0(TRACE_DEBUG, "<EVT>u16_EVENT0_23_reserved01 Complete Enable");
}


/// Function to disable u16_EVENT0_23_reserved01 event notification
void
EventManager_u16_EVENT0_23_reserved01_Disable(void)
{
    EventManager_Disable(0, 23);
    OstTraceInt0(TRACE_DEBUG, "<EVT>u16_EVENT0_23_reserved01 Complete Disable");
}


/// Function to raise u16_EVENT0_23_reserved01 event notification
void
EventManager_u16_EVENT0_23_reserved01_Notify(void)
{
    EventManager_Notify(0, 23);
    g_Event0_Count.u16_EVENT0_23_reserved01++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>u16_EVENT0_23_reserved01 Notify: %d", g_Event0_Count.u16_EVENT0_23_reserved01);
}


// Pipe1 u16_EVENT0_24_reserved01   : Event_0_24

// ==================================================
/// Function to enable u16_EVENT0_23_reserved02 event notification
void
EventManager_u16_EVENT0_24_reserved02_Enable(void)
{
    EventManager_Enable(0, 24);
    OstTraceInt0(TRACE_DEBUG, "<EVT>u16_EVENT0_23_reserved02 Enable");
}


/// Function to disable u16_EVENT0_23_reserved02 event notification
void
EventManager_u16_EVENT0_24_reserved02_Disable(void)
{
    EventManager_Disable(0, 24);
    OstTraceInt0(TRACE_DEBUG, "<EVT>u16_EVENT0_23_reserved02 Disable");
}


/// Function to raise u16_EVENT0_23_reserved02 event notification
void
EventManager_u16_EVENT0_24_reserved02_Notify(void)
{
    EventManager_Notify(0, 24);
    g_Event0_Count.u16_EVENT0_24_reserved02++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>u16_EVENT0_24_reserved02 Notify: %d", g_Event0_Count.u16_EVENT0_24_reserved02);
}


// u16_EVENT0_25_reserved03   : Event_0_25

// ==================================
/// Function to enable u16_EVENT0_25_reserved03 event notification
void
EventManager_u16_EVENT0_25_reserved03_Enable(void)
{
    EventManager_Enable(0, 25);
    OstTraceInt0(TRACE_DEBUG, "<EVT>u16_EVENT0_25_reserved03 Enable");
}


/// Function to disable u16_EVENT0_25_reserved03 event notification
void
EventManager_u16_EVENT0_25_reserved03_Disable(void)
{
    EventManager_Disable(0, 25);
    OstTraceInt0(TRACE_DEBUG, "<EVT>u16_EVENT0_25_reserved03 Disable");
}


/// Function to raise u16_EVENT0_23_reserved03 event notification
void
EventManager_u16_EVENT0_25_reserved03_Notify(void)
{
    EventManager_Notify(0, 25);

    g_Event0_Count.u16_EVENT0_25_reserved03++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>u16_EVENT0_25_reserved03 Notify: %d", g_Event0_Count.u16_EVENT0_25_reserved03);
}


//  NVM Data Export Notification  : Event_0_26

// ====================================
/// Function to enable NVM Data Export event notification
void
EventManager_NVM_Export_Enable(void)
{
    EventManager_Enable(0, 26);
    OstTraceInt0(TRACE_DEBUG, "<EVT>NVM Export Enable");
}


/// Function to disable NVM Data Export event notification
void
EventManager_NVM_Export_Disable(void)
{
    EventManager_Disable(0, 26);
    OstTraceInt0(TRACE_DEBUG, "<EVT>NVM Export Disable");
}


/// Function to raise NVM Data Export event notification
void
EventManager_NVM_Export_Notify(void)
{
    EventManager_Notify(0, 26);
    g_Event0_Count.u16_EVENT0_26_NVM_Export++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>NVM Export Notify: %d", g_Event0_Count.u16_EVENT0_26_NVM_Export);
}


//  sensor_tuning Programmed Notification  : Event_0_27

// ====================================
/// Function to enable sensor_tuning Available event notification
void
EventManager_sensor_tuning_Available_Enable(void)
{
    EventManager_Enable(0, 27);
    OstTraceInt0(TRACE_DEBUG, "<EVT>NVM Export Enable");
}


/// Function to disable NVM Data Export event notification
void
EventManager_sensor_tuning_Available_Disable(void)
{
    EventManager_Disable(0, 27);
    OstTraceInt0(TRACE_DEBUG, "<EVT>NVM Export Disable");
}


/// Function to raise sensor_tuning data programmed event notification
void
EventManager_sensor_tuning_Available_Notify(void)
{
    EventManager_Notify(0, 27);
    g_Event0_Count.u16_EVENT0_27_SensorTuning_Available++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>SENSOR_TUNING Available Notify: %d", g_Event0_Count.u16_EVENT0_27_SensorTuning_Available);
}


//  Power Event Notification  : Event_0_28

// =======================================
/// Function to enable Power event notification
void
EventManager_Power_Notification_Enable(void)
{
    EventManager_Enable(0, 28);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Power Notification Enable");
}


/// Function to disable Power event notification
void
EventManager_Power_Notification_Disable(void)
{
    EventManager_Disable(0, 28);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Power Notification Disable");
}


/// Function to raise Power event notification
void
EventManager_Power_Notification_Notify(void)
{
    EventManager_Notify(0, 28);
    g_Event0_Count.u16_EVENT0_28_Power_Notification++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Power Event Notify: %d", g_Event0_Count.u16_EVENT0_28_Power_Notification);
}


//  SMS Notification  : Event_0_29

// ====================================
/// Function to enable SMS notification
void
EventManager_SMS_Notification_Enable(void)
{
    EventManager_Enable(0, 29);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Power Notification Enable");
}


/// Function to disable SMS notification
void
EventManager_SMS_Notification_Disable(void)
{
    EventManager_Disable(0, 29);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Power Notification Disable");
}


/// Function to raise SMS notification
void
EventManager_SMS_Notification_Notify(void)
{
    EventManager_Notify(0, 29);
    g_Event0_Count.u16_EVENT0_29_SMS_DONE++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Power Event Notify: %d", g_Event0_Count.u16_EVENT0_29_SMS_DONE);
}


//  Sensor output mode export Notification  : Event_0_30

// ====================================
/// Function to enable Stream configuration export notification
void
EventManager_Sensor_Output_Mode_Export_Notification_Enable(void)
{
    EventManager_Enable(0, 30);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Sensor output mode export Notification Enable");
}


/// Function to disable Stream configuration export notification
void
EventManager_Sensor_Output_Mode_Export_Notification_Disable(void)
{
    EventManager_Disable(0, 30);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Sensor output mode export Notification Disable");
}


/// Function to raise Stream configuration export notification
void
EventManager_Sensor_Output_Mode_Export_Notification_Notify(void)
{
    EventManager_Notify(0, 30);
    g_Event0_Count.u16_EVENT0_30_Sensor_Output_Mode_Export_Notification++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Sensor output mode export Event Notify: %d", g_Event0_Count.u16_EVENT0_30_Sensor_Output_Mode_Export_Notification);
}


//  Valid Frame Notification  : Event_0_31

// ====================================
/// Function to enable Valid Frame notification
void
EventManager_Valid_Frame_Received_Enable(void)
{
    EventManager_Enable(0, 31);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Valid Frame Notification Enable");
}


/// Function to disable Valid Frame notification
void
EventManager_Valid_Frame_Received_Disable(void)
{
    EventManager_Disable(0, 31);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Valid Frame Notification Disable");
}


/// Function to raise Valid Frame notification
void
EventManager_Valid_Frame_Received_Notify(void)
{
    EventManager_Notify(0, 31);
    g_Event0_Count.u16_EVENT0_31_Valid_Frame_Notification++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Valid Frame Event Notify: %d", g_Event0_Count.u16_EVENT0_31_Valid_Frame_Notification);
}


// Host To Master I2C Access    : Event_1_0

// ====================================
/// Function to enable Host To Master I2C Access event notification
void
EventManager_HostToMasterI2CAccess_Enable(void)
{
    EventManager_Enable(1, 0);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Host To Master I2C Access Enable");
}


/// Function to disable Host To Master I2C Access event notification
void
EventManager_HostToMasterI2CAccess_Disable(void)
{
    EventManager_Disable(1, 0);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Host To Master I2C Access Disable");
}


/// Function to raise Host To Master I2C Access event notification
void
EventManager_HostToMasterI2CAccess_Notify(void)
{
    EventManager_Notify(1, 0);
    g_Event1_Count.u16_EVENT1_0_HOST_TO_MASTER_I2C_ACCESS++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Host To Master I2C Access Notify: %d", g_Event1_Count.u16_EVENT1_0_HOST_TO_MASTER_I2C_ACCESS);
}


// Sensor Commit    : Event_1_1
// ===========================
/// Function to enable Sensor Commit event notification
void
EventManager_SensorCommit_Enable(void)
{
    EventManager_Enable(1, 1);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Sensor Commit Enable");
}


/// Function to disable Sensor Commit event notification
void
EventManager_SensorCommit_Disable(void)
{
    EventManager_Disable(1, 1);
    OstTraceInt0(TRACE_DEBUG, "<EVT>Sensor Commit Disable");
}


/// Function to raise Sensor Commit event notification
void
EventManager_SensorCommit_Notify(void)
{
    EventManager_Notify(1, 1);
    g_Event1_Count.u16_EVENT1_1_SENSOR_COMMIT++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>Sensor CommitNotify: %d", g_Event1_Count.u16_EVENT1_1_SENSOR_COMMIT);
}


// ISP Commit    : Event_1_2
// ===========================
/// Function to enable ISP Commit event notification
void
EventManager_ISPCommit_Enable(void)
{
    EventManager_Enable(1, 2);
    OstTraceInt0(TRACE_DEBUG, "<EVT>ISP Commit Enable");
}


/// Function to disable ISP Commit event notification
void
EventManager_ISPCommit_Disable(void)
{
    EventManager_Disable(1, 2);
    OstTraceInt0(TRACE_DEBUG, "<EVT>ISP Commit Disable");
}


/// Function to raise ISP Commit event notification
void
EventManager_ISPCommit_Notify(void)
{
    EventManager_Notify(1, 2);
    g_Event1_Count.u16_EVENT1_2_ISP_COMMIT++;
    OstTraceInt1(TRACE_DEBUG, "<EVT>ISP Commit Notify: %d", g_Event1_Count.u16_EVENT1_2_ISP_COMMIT);
}



// Pipe0 GAMMA UPDATE COMPLETE     : Event_1_3

// ====================================
/// Function to enable HR pipe gamma update event notification
void
EventManager_GammaUpdateCompletePipe0_Enable(void)
{
    EventManager_Enable(1, 3);
    OstTraceInt0(TRACE_DEBUG, "<EVT>HR pipe gamma update event Enable");
}


/// Function to disable HR pipe gamma update event notification
void
EventManager_GammaUpdateCompletePipe0_Disable(void)
{
    EventManager_Disable(1, 3);
    OstTraceInt0(TRACE_DEBUG, "<EVT>HR pipe gamma update event Disable");
}


/// Function to raise LR pipe gamma update event notification
void
EventManager_GammaUpdateCompletePipe0_Notify(void)
{
//   If the host of the ISP FW (ITE-NMF, OMX componets) is event driven, event_count should be incremented before the event is notified.
    g_Event1_Count.u16_EVENT1_3_HR_GAMMA_UPDATE_COMPLETE++;
    EventManager_Notify(1, 3);
    OstTraceInt1(TRACE_DEBUG, "<EVT>HR pipe gamma update event Notify: %d", g_Event1_Count.u16_EVENT1_3_HR_GAMMA_UPDATE_COMPLETE);
}


// Pipe1 GAMMA UPDATE COMPLETE     : Event_1_4

// ====================================
/// Function to enable LR pipe gamma update event notification
void
EventManager_GammaUpdateCompletePipe1_Enable(void)
{
    EventManager_Enable(1, 4);
    OstTraceInt0(TRACE_DEBUG, "<EVT>LR pipe gamma update event Enable");
}


/// Function to disable LR pipe gamma update event notification
void
EventManager_GammaUpdateCompletePipe1_Disable(void)
{
    EventManager_Disable(1, 4);
    OstTraceInt0(TRACE_DEBUG, "<EVT>LR pipe gamma update event Disable");
}


/// Function to raise LR pipe gamma update event notification
void
EventManager_GammaUpdateCompletePipe1_Notify(void)
{
//   If the host of the ISP FW (ITE-NMF, OMX componets) is event driven, event_count should be incremented before the event is notified.
    g_Event1_Count.u16_EVENT1_4_LR_GAMMA_UPDATE_COMPLETE++;
    EventManager_Notify(1, 4);
    OstTraceInt1(TRACE_DEBUG, "<EVT>LR pipe gamma update event Notify: %d", g_Event1_Count.u16_EVENT1_4_LR_GAMMA_UPDATE_COMPLETE);
}


// DMA_GRAB Frame Ready Notification  : Event_3_0

// ==============================================
/// Function to enable DMA_GRAB event notification
void
EventManager_DMA_GRAB_Enable(void)
{
    EventManager_Enable(3, 0);
    OstTraceInt0(TRACE_DEBUG, "<EVT><DMA> GRAB Enable");
}


/// Function to disable DMA_GRAB event notification
void
EventManager_DMA_GRAB_Disable(void)
{
    EventManager_Disable(3, 0);
    OstTraceInt0(TRACE_DEBUG, "<EVT><DMA> GRAB Disable");
}


/// Function to raise DMA_GRAB event notification
void
EventManager_DMA_GRAB_Notify_NOK(void)
{
    EventManager_Notify(3, 0);
    g_Event3_Count.u16_EVENT3_0_DMA_GRAB_NOK++;
    OstTraceInt1(TRACE_DEBUG, "<EVT><DMA> GRAB Notify NOK: %d", g_Event3_Count.u16_EVENT3_0_DMA_GRAB_NOK);
}


/// Function to raise DMA_GRAB event notification
void
EventManager_DMA_GRAB_Notify_OK(void)
{
    EventManager_Notify(3, 0);
    g_Event3_Count.u16_EVENT3_0_DMA_GRAB_OK++;
    OstTraceInt1(TRACE_DEBUG, "<EVT><DMA> GRAB Notify OK: %d", g_Event3_Count.u16_EVENT3_0_DMA_GRAB_OK);
}


// VideoStab Frame Ready Notification  : Event_3_1

// ====================================
/// Function to enable VideoStab event notification
void
EventManager_DMA_GRAB_VideoStab_Enable(void)
{
    EventManager_Enable(3, 1);
    OstTraceInt0(TRACE_DEBUG, "<EVT><DMA> GRAB VideoStab Enable");
}


/// Function to disable VideoStab event notification
void
EventManager_DMA_GRAB_VideoStab_Disable(void)
{
    EventManager_Disable(3, 1);
    OstTraceInt0(TRACE_DEBUG, "<EVT><DMA> GRAB VideoStab Disable");
}


/// Function to raise VideoStab event notification
void
EventManager_DMA_GRAB_VideoStab_Notify(void)
{
    EventManager_Notify(3, 1);
    g_Event3_Count.u16_EVENT3_1_DMA_GRAB_VideoStab++;
    OstTraceInt1(TRACE_DEBUG, "<EVT><DMA> GRAB VideoStab Notify: %d", g_Event3_Count.u16_EVENT3_1_DMA_GRAB_VideoStab);
}

// ====================================
/// Function to abort Grab in case of CSI/CCP errors
void
EventManager_DMA_GRAB_Abort_Enable(void)
{
    EventManager_Enable(3, 2);
    OstTraceInt0(TRACE_DEBUG, "<EVT><DMA> GRAB Abort Enable");
}


/// Function to disable VideoStab event notification
void
EventManager_DMA_GRAB_Abort_Disable(void)
{
    EventManager_Disable(3, 2);
    OstTraceInt0(TRACE_DEBUG, "<EVT><DMA> GRAB Abort Disable");
}

/// Function to raise VideoStab Abort event notification for CSI/CCP errors
void
EventManager_DMA_GRAB_Abort_Notify(void)
{
    EventManager_Notify(3, 2);
    g_Event3_Count.u16_EVENT3_2_DMA_GRAB_Abort++;
    OstTraceInt1(TRACE_DEBUG, "<EVT><DMA> GRAB Abort Notify: %d", g_Event3_Count.u16_EVENT3_2_DMA_GRAB_Abort);
}


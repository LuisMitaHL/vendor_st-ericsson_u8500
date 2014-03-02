/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\if INCLUDE_IN_HTML_ONLY
\file  SystemConfig.c
\brief Contain API's and data structure for system configuration

  \ingroup SystemConfig
  \endif
*/
#include "SystemConfig.h"
#include "Platform.h"
#include "CRMhwReg.h"
#include "SDL_op_interface.h"
#include "CRM.h"

/// Specifies the default sensor input clock frequency in Mhz
#define TOP_LEVEL_DEFAULT_SENSOR_INPUT_CLOCK_FREQ_MHZ   12.0

/// Specifies the default MCU input clock frequency in Mhz
#define TOP_LEVEL_DEFAULT_MCU_CLOCK_FREQ_MHZ    (200.0)

/// Specifies the default host clock frequency in MHz
#define TOP_LEVEL_DEFAULT_CLOCK_HOST_FREQ_MHZ   (200.0)

/// Specifies the default time to wait after issuing start sensor command.
/// After this time clock output from the sensor will be good.
#define TOP_LEVEL_DEFAULT_SENSOR_START_DELAY_US 100000.0

/// Specifies the default sensor device ID
#define TOP_LEVEL_DEFAULT_SENSOR_DEVICE_ID  0x20

/// Specifies the default time to wait after pulling XSHUTDOWN of \n
/// the sensor high to ensure that the sensor is ready to handle  \n
/// i2c read and write requests. Using default calculation for delay, \n
/// a value of 8 will result in generating a delay of {8 * 200/(4*4)} = 100us \n
#define TOP_LEVEL_DEFAULT_SENSOR_XSHUTDOWN_DELAY_US 8

/// Specifies the default sensor device address type
#define TOP_LEVEL_DEFAULT_SENSOR_DEVICE_ADDRESS_TYPE    DeviceAddress_e_7BitDeviceAddress

/// Specifies the default sensor device index type
#define TOP_LEVEL_DEFAULT_SENSOR_DEVICE_INDEX_TYPE  DeviceIndex_e_16BitDataIndex

/// Specifies the default input image source
#define TOP_LEVEL_DEFAULT_INPUT_IMAGE_SOURCE    InputImageSource_e_Sensor0

/// Specifies the default input image interface
#define TOP_LEVEL_DEFAULT_INPUT_IMAGE_INTERFACE InputImageInterface_CSI2_0

/// Specifies the default number of CSI2 data lines
#define TOP_LEVEL_DEFAULT_NUMBER_OF_CSI2_DATA_LINES 3

/// Specifies the default test pattern type applicable if \n
/// input image source is Rx
#define TOP_LEVEL_DEFAULT_RX_TEST_PATTERN_TYPE  RxTestPattern_e_Normal
/// Specifies if by default, the ISP firmware should perform the IPP setup
#define TOP_LEVEL_DEFAULT_PERFORM_IPP_SETUP Flag_e_TRUE

/// Specifies the number of frames to stream for the current streaming operation. \n
/// Value of 0 specifies infinite streaming
#define TOP_LEVEL_DEFAULT_FRAME_COUNT   0

/// Specifies the number of lines to wait after Rx STOP command to allow the ISP pipe to be flushed. \n
/// As mentioned by <DENIS HORY>(PictorBug #86385)
#define TOP_LEVEL_DEFAULT_LINES_TO_WAIT_AT_RX_STOP  (32)

/// Default system metering mode
#define TOP_LEVEL_DEFAULT_SYSTEM_METERING_MODE  SystemMeteringMode_e_SINGLE_STEP

/// Default Flag inhibit exp metering
#define TOP_LEVEL_DEFAULT_FLAG_INHIBIT_EXP_METERING Flag_e_FALSE

/// Default Flag inhibit wb metering
#define TOP_LEVEL_DEFAULT_FLAG_INHIBIT_WB_METERING  Flag_e_FALSE

/// Default System Metering Coin Control
#define TOP_LEVEL_DEFAULT_COIN_SYSTEM_METERING_CTRL Coin_e_Tails

/// Default System mirror Control
#define TOP_LEVEL_DEFAULT_MIRROR_CTRL   (Flag_e_FALSE)

/// Default System flip Control
#define TOP_LEVEL_DEFAULT_FLIP_CTRL (Flag_e_FALSE)

/// Default System glace and whitebalance link mode
#define TOP_LEVEL_DEFAULT_COIN_GLACE_HISTOGRAM  (0)

/// Default System Idle Mode
#define TOP_LEVEL_DEFAULT_IDLE_MODE (IdleMode_e_IDLE_MODE_NONE)

/// Default AEC Stats Cancel
#define TOP_LEVEL_DEFAULT_FLAG_STATS_CANCEL (Flag_e_TRUE)

/// Default value of update notification flag
#define TOP_LEVEL_DEFAULT_UPDATE_NOTIFICATION_PENDING   (Flag_e_FALSE)

/// Default System Num of frames to be skipped
#define TOP_LEVEL_DEFAULT_NUM_OF_FRAMES_SKIPPED      (0)

/// Default value of BMS_GRAB_MODE_CTRL
#define TOP_LEVEL_DEFAULT_FLAG_GRAB_MODE_CTRL (GrabMode_e_NORMAL)

/// Default value of e_Flag_ZoomdStatsOnly
#define TOP_LEVEL_DEFAULT_FLAG_ZOOM_STATS_ONLY   (Flag_e_FALSE)

/// Default value of e_Flag_FastBML
#define TOP_LEVEL_DEFAULT_FLAG_FAST_BML              (Flag_e_FALSE)

/// Default value of e_Flag_BMSFrameNotifyReq
#define TOP_LEVEL_DEFAULT_FLAG_VALID_FRAME_NOTIFICATION   (Flag_e_TRUE)

/// Default value of BMS_GRAB_MODE_STATUS
#define TOP_LEVEL_DEFAULT_FLAG_GRAB_MODE_STATUS   (GrabMode_e_NORMAL)


/// Specifies the default value of CSI2_0 data lanes map
///Value changed to 0x8D1 for 9540 compatibility
///Note: Use only last 9 bits(i.e.,0xD1) when using for 8500v2/v1
///Masking is done in function CSI2_0_Setup_8500()
#define TOP_LEVEL_DEFAULT_DATA_LANE_MAP_CSI2_0  0x8D1

/// Specifies the default value of CSI2_1 data lanes map
#define TOP_LEVEL_DEFAULT_DATA_LANE_MAP_CSI2_1  0xD1

#define TOP_LEVEL_DEFAULT_CSI0_SWAP_PIN_CL      0
#define TOP_LEVEL_DEFAULT_CSI0_HS_INVERT_CL     0
#define TOP_LEVEL_DEFAULT_CSI0_SWAP_PIN_DL1     0
#define TOP_LEVEL_DEFAULT_CSI0_HS_INVERT_DL1    0
#define TOP_LEVEL_DEFAULT_CSI0_SWAP_PIN_DL2     0
#define TOP_LEVEL_DEFAULT_CSI0_HS_INVERT_DL2    0
#define TOP_LEVEL_DEFAULT_CSI0_SWAP_PIN_DL3     0
#define TOP_LEVEL_DEFAULT_CSI0_HS_INVERT_DL3    0
///For 4 datalane Support
#define TOP_LEVEL_DEFAULT_CSI0_SWAP_PIN_DL4     0
#define TOP_LEVEL_DEFAULT_CSI0_HS_INVERT_DL4    0

/// Specifies the default value of DPHY0_Ctrl
#define TOP_LEVEL_DEFAULT_DPHY0_CTRL    ((TOP_LEVEL_DEFAULT_CSI0_SWAP_PIN_CL << 0) | (TOP_LEVEL_DEFAULT_CSI0_HS_INVERT_CL << 1) | (TOP_LEVEL_DEFAULT_CSI0_SWAP_PIN_DL1 << 2) | (TOP_LEVEL_DEFAULT_CSI0_HS_INVERT_DL1 << 3) | (TOP_LEVEL_DEFAULT_CSI0_SWAP_PIN_DL2 << 4) | (TOP_LEVEL_DEFAULT_CSI0_HS_INVERT_DL2 << 5) | (TOP_LEVEL_DEFAULT_CSI0_SWAP_PIN_DL3 << 6) | (TOP_LEVEL_DEFAULT_CSI0_HS_INVERT_DL3 << 7))

///For 4 datalane Support
#define TOP_LEVEL_DEFAULT_DATA_LANE_4_DPHY0_CTRL ((TOP_LEVEL_DEFAULT_CSI0_SWAP_PIN_DL4 << 0) | (TOP_LEVEL_DEFAULT_CSI0_HS_INVERT_DL4 << 1))


#define TOP_LEVEL_DEFAULT_CSI1_SWAP_PIN_CL      0
#define TOP_LEVEL_DEFAULT_CSI1_HS_INVERT_CL     0
#define TOP_LEVEL_DEFAULT_CSI1_SWAP_PIN_DL1     0
#define TOP_LEVEL_DEFAULT_CSI1_HS_INVERT_DL1    0

/// Specifies the default value of DPHY1_Ctrl
#define TOP_LEVEL_DEFAULT_DPHY1_CTRL    ((TOP_LEVEL_DEFAULT_CSI1_SWAP_PIN_CL << 0) | (TOP_LEVEL_DEFAULT_CSI1_HS_INVERT_CL << 1) | (TOP_LEVEL_DEFAULT_CSI1_SWAP_PIN_DL1 << 2) | (TOP_LEVEL_DEFAULT_CSI1_HS_INVERT_DL1 << 3))

/// Specifies the default value of the sensor csi2 version on interface 0
#define TOP_LEVEL_DEFAULT_CSI2_VERSION_0    SensorCSI2Version_0_90

/// Specifies the default value of the sensor csi2 version on interface 1
#define TOP_LEVEL_DEFAULT_CSI2_VERSION_1    SensorCSI2Version_0_90

/// Specifies the default enable/disable command for pipe0
#define TOP_LEVEL_DEFAULT_PIPE0_ENABLE  Flag_e_TRUE

/// Specifies the default enable/disable command for pipe1
#define TOP_LEVEL_DEFAULT_PIPE1_ENABLE  Flag_e_TRUE

/// Specifies the default enable/disable command for pipeRAW
#define TOP_LEVEL_DEFAULT_PIPERAW_ENABLE    Flag_e_FALSE

/// Specifies the default enable/disable command for bayer store0
#define TOP_LEVEL_DEFAULT_BAYER_STORE0_ENABLE   Flag_e_FALSE

/// Specifies the default enable/disable command for bayer store1
#define TOP_LEVEL_DEFAULT_BAYER_STORE1_ENABLE   Flag_e_FALSE

/// Specifies the default enable/disable command for bayer store2
#define TOP_LEVEL_DEFAULT_BAYER_STORE2_ENABLE   Flag_e_FALSE

/// Specifies the default source of bayer store2
#define TOP_LEVEL_DEFAULT_BAYER_STORE2_SOURCE   BayerStore2Source_e_DusterOutput

/// Specifies the default enable/disable command for rgb store
#define TOP_LEVEL_DEFAULT_RGB_STORE_ENABLE  Flag_e_FALSE

/// Specifies the default value of source for RGB Store data path
#define TOP_LEVEL_DEFAULT_RGB_STORE_SOURCE  RGBStoreSource_e_BabylonOutput

/// Specifies the default value of coin for pipe control
#define TOP_LEVEL_DEFAULT_COIN_PIPEENABLE   Coin_e_Tails

/// Specifies the default x output size for pipe0
#define TOP_LEVEL_DEFAULT_PIPE0_X_OUTPUT_SIZE   2048

/// Specifies the default y output size for pipe0
#define TOP_LEVEL_DEFAULT_PIPE0_Y_OUTPUT_SIZE   1536

/// Specifies the default output format for pipe0
#define TOP_LEVEL_DEFAULT_PIPE0_OUTPUT_FORMAT   OutputFormat_e_YUV

/// Specifies the default setting for toggle pix valid for pipe0
#define TOP_LEVEL_DEFAULT_PIPE0_TOGGLE_PIX_VALID    Flag_e_TRUE

/// Specifies the default validity for pipe
#define TOP_LEVEL_DEFAULT_PIPE_Valid    Flag_e_TRUE

/// Specifies the default setting for pix valid line types for pipe0
#define TOP_LEVEL_DEFAULT_PIPE0_PIX_VALID_LINE_TYPES    0x20

/// Specifies the default setting for flip CB and CR for pipe0
#define TOP_LEVEL_DEFAULT_PIPE0_FLIP_CB_CR  Flag_e_FALSE

/// Specifies the default setting for flip Y and CBCR for pipe0
#define TOP_LEVEL_DEFAULT_PIPE0_FLIP_Y_CBCR Flag_e_FALSE

/// Specifies the default x output size for pipe1
#define TOP_LEVEL_DEFAULT_PIPE1_X_OUTPUT_SIZE   640

/// Specifies the default y output size for pipe1
#define TOP_LEVEL_DEFAULT_PIPE1_Y_OUTPUT_SIZE   480

/// Specifies the default output format for pipe1
#define TOP_LEVEL_DEFAULT_PIPE1_OUTPUT_FORMAT   OutputFormat_e_RGB565_PEAKED

/// Specifies the default setting for toggle pix valid for pipe1
#define TOP_LEVEL_DEFAULT_PIPE1_TOGGLE_PIX_VALID    Flag_e_TRUE

/// Specifies the default setting for pix valid line types for pipe1
#define TOP_LEVEL_DEFAULT_PIPE1_PIX_VALID_LINE_TYPES    0x20

/// Specifies the default setting for flip CB and CR for pipe1
#define TOP_LEVEL_DEFAULT_PIPE1_FLIP_CB_CR  Flag_e_FALSE

/// Specifies the default setting for flip Y and CBCR for pipe0
#define TOP_LEVEL_DEFAULT_PIPE1_FLIP_Y_CBCR Flag_e_FALSE

/// Specifies the default x output size for pipe1
#define TOP_LEVEL_DEFAULT_PIPE_RAW_X_OUTPUT_SIZE    (3280)

/// Specifies the default y output size for pipe1
#define TOP_LEVEL_DEFAULT_PIPE_RAW_Y_OUTPUT_SIZE    (2464)

/// Specifies the default value of number of frame streamed in pipe0 since last run
#define TOP_LEVEL_DEFAULT_PIPE0_NUMBER_OF_FRAMES_STREAMED_IN_SINCE_LAST_RUN 0

/// Specifies the default value of number of frame streamed out of pipe0 since last run
#define TOP_LEVEL_DEFAULT_PIPE0_NUMBER_OF_FRAMES_STREAMED_OUT_SINCE_LAST_RUN    0

/// Specifies the default value of vid complete pending for pipe0
#define TOP_LEVEL_PIPE0_VID_COMPLETE_PENDING    Flag_e_FALSE

/// Specifies the default value of number of frame streamed in pipe1 since last run
#define TOP_LEVEL_DEFAULT_PIPE1_NUMBER_OF_FRAMES_STREAMED_IN_SINCE_LAST_RUN 0

/// Specifies the default value of number of frame streamed out of pipe1 since last run
#define TOP_LEVEL_DEFAULT_PIPE1_NUMBER_OF_FRAMES_STREAMED_OUT_SINCE_LAST_RUN    0

/// Specifies the default value of vid complete pending for pipe0
#define TOP_LEVEL_PIPE1_VID_COMPLETE_PENDING    Flag_e_FALSE

/// Specifies the default value of e_Flag_abortRx_OnStop
#define TOP_LEVEL_DEFAULT_ABORT_Rx_ON_STOP (Flag_e_FALSE)

/**
System configuration, to be set before boot
*/
SystemSetup_ts                      g_SystemSetup =
{
    TOP_LEVEL_DEFAULT_SENSOR_INPUT_CLOCK_FREQ_MHZ,  // f_SensorInputClockFreq_Mhz
    TOP_LEVEL_DEFAULT_MCU_CLOCK_FREQ_MHZ,           // f_MCUClockFreq_Mhz
    TOP_LEVEL_DEFAULT_CLOCK_HOST_FREQ_MHZ,          // f_ClockHost_Mhz
    TOP_LEVEL_DEFAULT_SENSOR_START_DELAY_US,        // f_SensorStartDelay_us
    TOP_LEVEL_DEFAULT_SENSOR_DEVICE_ID,             // u16_DeviceID
    TOP_LEVEL_DEFAULT_SENSOR_XSHUTDOWN_DELAY_US,    // u16_SensorXshutdownDelay_us
    TOP_LEVEL_DEFAULT_SENSOR_DEVICE_ADDRESS_TYPE,   // e_DeviceAddress_Type
    TOP_LEVEL_DEFAULT_SENSOR_DEVICE_INDEX_TYPE,     // e_DeviceIndex_Type
    TOP_LEVEL_DEFAULT_INPUT_IMAGE_SOURCE,           // e_InputImageSource
    TOP_LEVEL_DEFAULT_INPUT_IMAGE_INTERFACE,        // e_InputImageInterface
    TOP_LEVEL_DEFAULT_NUMBER_OF_CSI2_DATA_LINES,    // u8_NumberOfCSI2DataLines
    TOP_LEVEL_DEFAULT_RX_TEST_PATTERN_TYPE,         // e_RxTestPattern
    TOP_LEVEL_DEFAULT_PERFORM_IPP_SETUP,            // e_Flag_PerformIPPSetup
    TOP_LEVEL_DEFAULT_FRAME_COUNT,                  // u8_FrameCount
    TOP_LEVEL_DEFAULT_LINES_TO_WAIT_AT_RX_STOP,     // u8_LinesToWaitAtRxStop
    TOP_LEVEL_DEFAULT_SYSTEM_METERING_MODE,         // e_SystemMeteringMode_Control
    TOP_LEVEL_DEFAULT_FLAG_INHIBIT_EXP_METERING,    // e_Flag_InhibitExpMetering
    TOP_LEVEL_DEFAULT_FLAG_INHIBIT_WB_METERING,     // e_Flag_InhibitWbMetering
    TOP_LEVEL_DEFAULT_COIN_SYSTEM_METERING_CTRL,    // e_Coin_Ctrl
    TOP_LEVEL_DEFAULT_MIRROR_CTRL,                  // e_Flag_mirror
    TOP_LEVEL_DEFAULT_FLIP_CTRL,                    // e_Flag_flip
    TOP_LEVEL_DEFAULT_COIN_GLACE_HISTOGRAM,         // e_Coin_Glace_Histogram_Ctrl_debug
    TOP_LEVEL_DEFAULT_IDLE_MODE,                    // e_IdleMode_Ctrl
    DEFAULT_CLK_PIPE_IN_DIV,                        // crm_clk_pipe_in_div
    TOP_LEVEL_DEFAULT_ABORT_Rx_ON_STOP,             // e_Flag_abortRx_OnStop
    TOP_LEVEL_DEFAULT_FLAG_STATS_CANCEL,            // e_Flag_AecStatsCancel
    TOP_LEVEL_DEFAULT_NUM_OF_FRAMES_SKIPPED,        // u8_NumOfFramesSkipped
    TOP_LEVEL_DEFAULT_FLAG_GRAB_MODE_CTRL,          // e_GrabMode_Ctrl
    TOP_LEVEL_DEFAULT_FLAG_ZOOM_STATS_ONLY,         // e_Flag_ZoomdStatsOnly
    TOP_LEVEL_DEFAULT_FLAG_FAST_BML,                // e_Flag_FastBML
    TOP_LEVEL_DEFAULT_FLAG_VALID_FRAME_NOTIFICATION // e_Flag_Request_ValidFrameNotification_Ctrl
};

CSIControl_ts                       g_CSIControl =
{
    TOP_LEVEL_DEFAULT_DATA_LANE_MAP_CSI2_0,         // u16_DataLanesMapCSI2_0
    TOP_LEVEL_DEFAULT_DATA_LANE_MAP_CSI2_1,         // u16_DataLanesMapCSI2_1
    TOP_LEVEL_DEFAULT_DPHY0_CTRL,                   // u8_DPHY0Ctrl
    TOP_LEVEL_DEFAULT_DPHY1_CTRL,                   // u8_DPHY1Ctrl
    TOP_LEVEL_DEFAULT_CSI2_VERSION_0,               // e_SensorCSI2Version_0
    TOP_LEVEL_DEFAULT_CSI2_VERSION_1,               // e_SensorCSI2Version_1
    TOP_LEVEL_DEFAULT_DATA_LANE_4_DPHY0_CTRL        // u8_DPHY0Ctrl_4th_lane For 4 Datalane Support
};

DataPathSetup_ts                    g_DataPathControl =
{
    TOP_LEVEL_DEFAULT_PIPE0_ENABLE,                 // e_Flag_Pipe0Enable
    TOP_LEVEL_DEFAULT_PIPE1_ENABLE,                 // e_Flag_Pipe1Enable
    TOP_LEVEL_DEFAULT_BAYER_STORE0_ENABLE,          // e_Flag_BayerStore0Enable
    TOP_LEVEL_DEFAULT_BAYER_STORE1_ENABLE,          // e_Flag_BayerStore1Enable
    TOP_LEVEL_DEFAULT_BAYER_STORE2_ENABLE,          // e_Flag_BayerStore2Enable
    TOP_LEVEL_DEFAULT_BAYER_STORE2_SOURCE,          // e_BayerStore2Source
    TOP_LEVEL_DEFAULT_RGB_STORE_ENABLE,             // e_Flag_RGBStoreEnable
    TOP_LEVEL_DEFAULT_RGB_STORE_SOURCE,             // e_RGBStoreSource
    TOP_LEVEL_DEFAULT_PIPERAW_ENABLE,               // e_Flag_Pipe0Enable
    TOP_LEVEL_DEFAULT_COIN_PIPEENABLE,
};

/// Control page element for cold start
ColdStart_ts                        g_ColdStartControl = { Coin_e_Heads, Coin_e_Heads, Coin_e_Heads };

/// Status page element for cold start
ColdStart_ts                        g_ColdStartStatus = { Coin_e_Tails, Coin_e_Tails, Coin_e_Tails };

DataPathSetup_ts                    g_DataPathStatus =
{
    Flag_e_FALSE,                               // e_Flag_Pipe0Enable
    Flag_e_FALSE,                               // e_Flag_Pipe1Enable
    Flag_e_FALSE,                               // e_Flag_BayerStore0Enable
    Flag_e_FALSE,                               // e_Flag_BayerStore1Enable
    Flag_e_FALSE,                               // e_Flag_BayerStore2Enable
    Flag_e_FALSE,                               // e_BayerStore2Source
    Flag_e_FALSE,                               // e_Flag_RGBStoreEnable
    Flag_e_FALSE,                               // e_RGBStoreSource;
    Flag_e_FALSE,                               // e_Flag_PipeRAWEnable,
    TOP_LEVEL_DEFAULT_COIN_PIPEENABLE
};

Event0_test_ts          g_Event0_Count = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
Event1_test_ts          g_Event1_Count = { 0, 0, 0, 0, 0 };
Event2_test_ts          g_Event2_Count = {0};
Event3_test_ts          g_Event3_Count = { 0, 0, 0, 0 };

DMASetup_ts                         g_DMASetup = { DMADataBitsInMemory_12, DMADataAccessAlignment_e_4Byte };
DMAControl_ts                       g_DMAControl;

/**
Pipe setup for Pipe0 and Pipe1. Set before RUN
*/
PipeSetup_ts                        g_Pipe[NO_OF_HARDWARE_PIPE_IN_ISP] =
{
    { TOP_LEVEL_DEFAULT_PIPE0_X_OUTPUT_SIZE,    // u16_X_size
        TOP_LEVEL_DEFAULT_PIPE0_Y_OUTPUT_SIZE,  // u16_Y_size
        TOP_LEVEL_DEFAULT_PIPE0_OUTPUT_FORMAT,  // e_OutputFormat_Pipe
        TOP_LEVEL_DEFAULT_PIPE0_TOGGLE_PIX_VALID,                               // e_Flag_TogglePixValid
        TOP_LEVEL_DEFAULT_PIPE0_PIX_VALID_LINE_TYPES,                           // u8_PixValidLineTypes
        TOP_LEVEL_DEFAULT_PIPE0_FLIP_CB_CR,                                     // e_Flag_Flip_Cb_Cr
        TOP_LEVEL_DEFAULT_PIPE0_FLIP_Y_CBCR,                                    // e_Flag_Flip_Y_CbCr
        TOP_LEVEL_DEFAULT_PIPE_Valid, },
    { TOP_LEVEL_DEFAULT_PIPE1_X_OUTPUT_SIZE,                                    // u16_X_size
        TOP_LEVEL_DEFAULT_PIPE1_Y_OUTPUT_SIZE,                                  // u16_Y_size
        TOP_LEVEL_DEFAULT_PIPE1_OUTPUT_FORMAT,                                  // e_OutputFormat_Pipe
        TOP_LEVEL_DEFAULT_PIPE1_TOGGLE_PIX_VALID,                               // e_Flag_TogglePixValid
        TOP_LEVEL_DEFAULT_PIPE1_PIX_VALID_LINE_TYPES,                           // u8_PixValidLineTypes
        TOP_LEVEL_DEFAULT_PIPE1_FLIP_CB_CR,                                     // e_Flag_Flip_Cb_Cr
        TOP_LEVEL_DEFAULT_PIPE1_FLIP_Y_CBCR,                                    // e_Flag_Flip_Y_CbCr
        TOP_LEVEL_DEFAULT_PIPE_Valid, }
};

PipeSetup_RAW_ts                    g_Pipe_RAW =
{
    TOP_LEVEL_DEFAULT_PIPE_RAW_X_OUTPUT_SIZE,                                   // u16_X_size
    TOP_LEVEL_DEFAULT_PIPE_RAW_Y_OUTPUT_SIZE,                                   // u16_Y_size
    TOP_LEVEL_DEFAULT_PIPE_RAW_X_OUTPUT_SIZE,
    TOP_LEVEL_DEFAULT_PIPE_RAW_Y_OUTPUT_SIZE
};

volatile PipeStatus_ts              g_PipeStatus[NO_OF_HARDWARE_PIPE_IN_ISP] =
{
    { TOP_LEVEL_DEFAULT_PIPE0_NUMBER_OF_FRAMES_STREAMED_IN_SINCE_LAST_RUN,      // u8_FramesStreamedInPipeLastRun
        TOP_LEVEL_DEFAULT_PIPE0_NUMBER_OF_FRAMES_STREAMED_OUT_SINCE_LAST_RUN,   // u8_FramesStreamedOutOfPipeLastRun
        TOP_LEVEL_PIPE0_VID_COMPLETE_PENDING                                // e_Flag_VideoCompleteInterruptPending
    },
    { TOP_LEVEL_DEFAULT_PIPE1_NUMBER_OF_FRAMES_STREAMED_IN_SINCE_LAST_RUN,  // u8_FramesStreamedInPipeLastRun
        TOP_LEVEL_DEFAULT_PIPE1_NUMBER_OF_FRAMES_STREAMED_OUT_SINCE_LAST_RUN,   // u8_FramesStreamedOutOfPipeLastRun
        TOP_LEVEL_PIPE1_VID_COMPLETE_PENDING        // e_Flag_VideoCompleteInterruptPending
    }
};

SystemConfig_Status_ts              g_SystemConfig_Status =
{
    Flag_e_FALSE,                                   // e_Flag_InputPipeUpdated
    Flag_e_TRUE,                                    // e_Flag_ExposureOutOfSync
    Coin_e_Tails,                                   // e_Coin_Status
    Flag_e_FALSE,                                   // e_Flag_ZoomUpdateDone
    Flag_e_FALSE,                                   // e_Flag_mirror
    Flag_e_FALSE,                                   // e_Flag_flip
    TOP_LEVEL_DEFAULT_UPDATE_NOTIFICATION_PENDING,  // e_Flag_UpdateNotificationPending
    UpdateStatus_e_Idle,                            // e_UpdateStatus_Exposure
    UpdateStatus_e_Idle,                            // e_UpdateStatus_FrameRate
    UpdateStatus_e_Idle,                            // e_UpdateStatus_Flash
    TOP_LEVEL_DEFAULT_COIN_GLACE_HISTOGRAM,         // e_Coin_Glace_Histogram_Ctrl_debug
    0,                                              // u8_Sensor_Pixel_Order
    Flag_e_FALSE,                                   // e_Flag_RunBestSensormode
    Flag_e_FALSE,                                   // e_Flag_FireDummyVidComplete0
    Flag_e_FALSE,                                   // e_Flag_FireDummyVidComplete1
    TOP_LEVEL_DEFAULT_RX_TEST_PATTERN_TYPE,         // e_RxTestPattern_Status
    Flag_e_FALSE,                                   // e_Flag_ZoomdStatsOnly
    TOP_LEVEL_DEFAULT_FLAG_FAST_BML,                // e_Flag_FastBML
    TOP_LEVEL_DEFAULT_FLAG_GRAB_MODE_STATUS,        // e_GrabMode_Status
    TOP_LEVEL_DEFAULT_FLAG_VALID_FRAME_NOTIFICATION // e_Flag_Request_ValidFrameNotification_Status
};

volatile FrameInterrupts_Status_ts  g_FrameInterrupts_Status =
{
    Flag_e_FALSE,   // e_Flag_SMIARx_0
    Flag_e_FALSE,   // e_Flag_SMIARx_1
    Flag_e_FALSE,   // e_Flag_SMIARx_2
    Flag_e_FALSE,   // e_Flag_SMIARx_3
    Flag_e_FALSE,   // e_Flag_VidComplete_Pipe0
    Flag_e_FALSE,   // e_Flag_VidComplete_Pipe1
    Flag_e_FALSE,   // e_Flag_Stats_Glace
    Flag_e_FALSE    // e_Flag_Stats_Histo;
};


TestPattern_SolidColor_ts g_testpattern_SolidColor_data = {
                                                                                              0,     // u16_SolidColor_data_red
                                                                                              0,     // u16_SolidColor_data_gir
                                                                                              0,     // u16_SolidColor_data_blu
                                                                                              0      // u16_SolidColor_data_gib
                                                                                             };

TestPattern_Cursors_ts g_testpattern_Cursors_values = {
                                                                                            0,  // u8_hcur_posn_per
                                                                                            0,  // u8_hcur_width
                                                                                            0,  // u8_vcur_posn_per
                                                                                            0   // u8_vcur_width
                                                                                         };


/**
\fn         uint8_t SystemConfig_IsPipe0Idle ( void )
\brief      Function to determine if pipe specified by u8_PipeNumber has completely streamed out its last frame.
Must be called once the Rx has stopped streaming frames.
\param  u8_PipeNumber : Pipe number for which the idle status is queried
\return     uint8_t
Flag_e_TRUE     : If pipe is idle or if pipe is disabled
Flag_e_FALSE    : If pipe is not idle
\ingroup    SystemConfig
\callgraph
\callergraph
*/
uint8_t
SystemConfig_IsPipeIdle(
uint8_t u8_PipeNumber)
{
    if (Flag_e_TRUE == g_PipeStatus[u8_PipeNumber].e_Flag_VideoCompleteInterruptPending)
    {
        return (Flag_e_FALSE);
    }
    else
    {
        return (Flag_e_TRUE);
    }
}


/**
Funtion            SystemConfig_UpdateRxPattern
Description      This Function is used to set Different RxTestPattern types, which are controlled by
                      2 pages (TestPattern_SolidColor_ts and TestPattern_Cursors_ts)  and
                      e_RxTestPattern page element from SystemSetup_ts
Input/Output   void/void
                      e_RxTestPattern_Status element is used to maintain the current RxPattern Type and will be updated by e_RxTestPattern if modified by the Host system.
                      e_coin_SolidColor_update is used to check the soild color coin status and updates the data accordingly.
                      e_coin_cursor_update is used to check the cursor coin status and updates the value accordingly.

**/
void SystemConfig_UpdateRxPattern(void)

{
    switch(g_SystemSetup.e_RxTestPattern)
    {

    case RxTestPattern_e_Normal:
        Set_ISP_SMIARX_ISP_SMIARX_SETUP_pattern_type__PATTERN_DISABLE();
        break;

    case RxTestPattern_e_ColourBars :
        Set_ISP_SMIARX_ISP_SMIARX_SETUP_pattern_type__PATTERN_COLOUR_BAR();
        break;

    case RxTestPattern_e_GraduatedColourBars :
        Set_ISP_SMIARX_ISP_SMIARX_SETUP_pattern_type__PATTERN_GRAD_BAR();
        break;

    case RxTestPattern_e_DiagonalGreyscale:
        Set_ISP_SMIARX_ISP_SMIARX_SETUP_pattern_type__PATTERN_DIAG_GREY();
        break;

    case RxTestPattern_e_PsuedoRandom:
        Set_ISP_SMIARX_ISP_SMIARX_SETUP_pattern_type__PATTERN_PSEUDORANDOM();
        break;

    case RxTestPattern_e_HorizontalGreyscale:
        Set_ISP_SMIARX_ISP_SMIARX_SETUP_pattern_type__PATTERN_HOR_GREY();
        break;

    case RxTestPattern_e_VerticalGreyscale :
        Set_ISP_SMIARX_ISP_SMIARX_SETUP_pattern_type__PATTERN_VERT_GREY();
        break;

    case RxTestPattern_e_SolidColour :
        // Update R,GR,B,BG values in RG anf BG Registers
        Set_ISP_SMIARX_ISP_SMIARX_TPAT_DATA_RG(g_testpattern_SolidColor_data.u16_SolidColor_data_gir,g_testpattern_SolidColor_data.u16_SolidColor_data_red);
        Set_ISP_SMIARX_ISP_SMIARX_TPAT_DATA_BG(g_testpattern_SolidColor_data.u16_SolidColor_data_gib,g_testpattern_SolidColor_data.u16_SolidColor_data_blu);
        Set_ISP_SMIARX_ISP_SMIARX_SETUP_pattern_type__PATTERN_SOLID();
        break;

    default:
        // Deault Value is set to Disable the Test pattern.
        Set_ISP_SMIARX_ISP_SMIARX_SETUP_pattern_type__PATTERN_DISABLE();
        break;

    }

    g_SystemConfig_Status.e_RxTestPattern_Status = g_SystemSetup.e_RxTestPattern;

    // Update Cursor Position, Width in Horizontal and Vertical Registers
    Set_ISP_SMIARX_ISP_SMIARX_TPAT_HCUR_WP((uint32_t)(((g_CurrentFrameDimension.u16_OPXOutputSize - FrameDimension_GetSensorConstantCols())* g_testpattern_Cursors_values.u8_hcur_posn_per)/100), g_testpattern_Cursors_values.u8_hcur_width);
    Set_ISP_SMIARX_ISP_SMIARX_TPAT_VCUR_WP((uint32_t)(((g_CurrentFrameDimension.u16_OPYOutputSize - FrameDimension_GetSensorConstantRows())* g_testpattern_Cursors_values.u8_vcur_posn_per)/100), g_testpattern_Cursors_values.u8_vcur_width);

}


void
SystemConfig_RunTimeDataPathUpdate(void)
{
    // only BMS0, BMS1, BMS2, BMS2 source, RMS, RMS source can be changed at runtime
    g_DataPathStatus.e_Flag_BayerStore0Enable = g_DataPathControl.e_Flag_BayerStore0Enable;
    g_DataPathStatus.e_Flag_BayerStore1Enable = g_DataPathControl.e_Flag_BayerStore1Enable;
    g_DataPathStatus.e_Flag_BayerStore2Enable = g_DataPathControl.e_Flag_BayerStore2Enable;
    g_DataPathStatus.e_BayerStore2Source = g_DataPathControl.e_BayerStore2Source;
    g_DataPathStatus.e_Flag_RGBStoreEnable = g_DataPathControl.e_Flag_RGBStoreEnable;
    g_DataPathStatus.e_RGBStoreSource = g_DataPathControl.e_RGBStoreSource;

    return;
}


void
SystemConfig_RunTimeDataPathCommit(void)
{
    // we now need to work out clk_bms
    // if any memory store data path is active then
    // clk_bms must be enabled
    if
    (
        SystemConfig_IsRGBStoreActive()
    ||  SystemConfig_IsBayerStore2Active()
    ||  SystemConfig_IsBayerStore1Active()
    ||  SystemConfig_IsBayerStore0Active()
    )
    {
        // enable clk_bms
        Set_CRM_CRM_EN_CLK_BMS(1);
    }
    else
    {
        // disable clk_bms
        Set_CRM_CRM_EN_CLK_BMS(0);
    }


    // now enable the data paths for any possible intermediate stores
    if (SystemConfig_IsRGBStoreActive())
    {
        // enable dmce rgb store data path
        Set_ISP_DMCE_RGB_STORE_ISP_DMCE_RGB_STORE_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);              // mux2to1_enable,mux2to1_soft_reset

        // enable RGB store data path
        if (SystemConfig_IsRGBStoreSourceBabylon() ||SystemConfig_IsRGBStoreSourceMozart())
        {
            // set RGB store source to Babylon (0) or Mozart which ever is applicable 
            Set_ISP_DMCE_RGB_STORE_ISP_DMCE_RGB_STORE_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);             // (mux2to1_select,mux2to1_shadow_en
        }


        //else  //[V1V2] HBarrel removed.
        //{
        // set RGB store source to HBarrel (1)
        //Set_ISP_DMCE_RGB_STORE_ISP_DMCE_RGB_STORE_SELECT(mux2to1_select_IDP1, mux2to1_shadow_en_B_0x1);             // (mux2to1_select,mux2to1_shadow_en
        //}
        if ((Is_8500v2()|| Is_8540v1()|| Is_9540v1()) && (SystemConfig_IsRGBStoreSourceChannelMerge()))
        {
            // set RGB store source to Channel Merge (1)
            Set_ISP_DMCE_RGB_STORE_ISP_DMCE_RGB_STORE_SELECT(mux2to1_select_IDP1, mux2to1_shadow_en_B_0x1);
        }
    }
    else
    {
        // disable dmce rgb store data path
        Set_ISP_DMCE_RGB_STORE_ISP_DMCE_RGB_STORE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);             // mux2to1_enable,mux2to1_soft_reset
    }


    if (SystemConfig_IsBayerStore2Active())
    {
        // enable bayer store2 data path
        Set_ISP_BAYER_STORE_RE_OUT_ISP_BAYER_STORE_RE_OUT_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);      // mux2to1_enable,mux2to1_soft_reset
        if (SystemConfig_IsBayerStore2SourceDuster())
        {
            // set bayer store2 source to duster(1) (for V1 hardware revision) Or to Scorpio (1) (for V2 hardware revision)
            Set_ISP_BAYER_STORE_RE_OUT_ISP_BAYER_STORE_RE_OUT_SELECT(mux2to1_select_IDP1, mux2to1_shadow_en_B_0x1);     // mux2to1_select,mux2to1_shadow_en
        }
        else
        {
            // set bayer store2 source to bayer crop(0)
            Set_ISP_BAYER_STORE_RE_OUT_ISP_BAYER_STORE_RE_OUT_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);     // mux2to1_select,mux2to1_shadow_en
        }
    }
    else
    {
        // disable bayer store2 data path
        Set_ISP_BAYER_STORE_RE_OUT_ISP_BAYER_STORE_RE_OUT_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);     // mux2to1_enable,mux2to1_soft_reset
    }


    if (SystemConfig_IsBayerStore1Active())
    {
        // enable bayer store1 data path
        Set_ISP_BAYER_STORE_PIX_CLK_ISP_BAYER_STORE_PIX_CLK_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);    // mux2to1_enable,mux2to1_soft_reset

        // set bayer store 1 source to LBE output (0);
        Set_ISP_BAYER_STORE_PIX_CLK_ISP_BAYER_STORE_PIX_CLK_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);       // mux2to1_select,mux2to1_shadow_en
    }
    else
    {
        // disable bayer store1 data path
        Set_ISP_BAYER_STORE_PIX_CLK_ISP_BAYER_STORE_PIX_CLK_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);   // mux2to1_enable,mux2to1_soft_reset
    }


    if (SystemConfig_IsBayerStore0Active())
    {
        // enable bayer store0 data path
        Set_ISP_BAYER_STORE_SD_CLK_ISP_BAYER_STORE_SD_CLK_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);      // mux2to1_enable,mux2to1_soft_reset

        // set bayer store 1 source to sensor output (0);
        Set_ISP_BAYER_STORE_SD_CLK_ISP_BAYER_STORE_SD_CLK_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);         // mux2to1_select,mux2to1_shadow_en
    }
    else
    {
        // if the BMS clock or the clk_sd_hi_speed or clk_sd_lo_speed is already disabled, then nothing needs to be done
        if
        (
            (0 != Get_CRM_CRM_EN_CLK_SD_HISPEED())
        &&  (0 != Get_CRM_CRM_EN_CLK_SD_LOSPEED())
        &&  (0 != Get_CRM_CRM_EN_CLK_BMS())
        )
        {
            // disable bayer store0 data path
            Set_ISP_BAYER_STORE_SD_CLK_ISP_BAYER_STORE_SD_CLK_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0); // mux2to1_enable,mux2to1_soft_reset
        }   //if end
        else if ((0 != Get_CRM_CRM_EN_CLK_SD_HISPEED()) && (0 != Get_CRM_CRM_EN_CLK_SD_LOSPEED()))
        {
            // Even if clk_bms is disable but mux Bayer store sd is working on clk_sd_hispeed,which is active at this time.
            // and mux clk will remain active,we need to disable this mux if mux control is set by user = Disable
            Set_ISP_BAYER_STORE_SD_CLK_ISP_BAYER_STORE_SD_CLK_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0); // mux2to1_enable,mux2to1_soft_reset
        }   //else if end
    }       //else bayer store0 active end

    return;
}


void
SystemConfig_StaticDataPathCommit(void)
{
    // populate the appropriate status elements
    g_DataPathStatus.e_Flag_Pipe0Enable = g_DataPathControl.e_Flag_Pipe0Enable;
    g_DataPathStatus.e_Flag_Pipe1Enable = g_DataPathControl.e_Flag_Pipe1Enable;

    SystemConfig_RunTimeDataPathUpdate();

    // if the input image source is not under ISP control,
    // then the clk_sd_hispeed, clk_sd_lospeed and clk_gpio
    // must be disabled else they must be enabled
    if (SystemConfig_IsInputImageSourceUnderISPControl())
    {
        // enable clk_sd_hispeed, clk_sd_lospeed
        Set_CRM_CRM_EN_CLK_SD_HISPEED(1, 1);

        // if neither the RE, nor the DMCE nor any output pipe is enabled then
        // it is a case of fast bayer capture. Disable clk_sd_lospeed
        Set_CRM_CRM_EN_CLK_SD_LOSPEED(1, 1);
        if (SystemConfig_IsRE_Active() || SystemConfig_IsBayerStore1Active())
        {
            // enable input to SDP Pipe
            Set_ISP_SD_IDP_GATE_ISP_SD_IDP_GATE_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);    // mux2to1_enable,mux2to1_soft_reset
            Set_ISP_SD_IDP_GATE_ISP_SD_IDP_GATE_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);       // mux2to1_select,mux2to1_shadow_en
        }
        else
        {
            Set_ISP_SD_IDP_GATE_ISP_SD_IDP_GATE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);   // mux2to1_enable,mux2to1_soft_reset
            Set_CRM_CRM_EN_CLK_SD_LOSPEED(0, 0);    // disable the lo_speed clocks, they are not needed
        }


        // clock source for the ISP will be sensor clocks, disable clk_pipe_in
        Set_CRM_CRM_EN_CLK_PIPE_IN(0);

        // TODO: enable clk_gpio
        // disable clk_bml
        Set_CRM_CRM_EN_CLK_BML(0);

        // set the CRM_STATIC_CLK_BMS_SOURCE_SEL to be sensor clocks
        Set_CRM_CRM_STATIC_CLK_BMS_SOURCE_SEL(STATIC_CLK_BMS_SOURCE_SEL_B_0x0);

        // Set CRM_STATIC_CLK_SD_HISPEED_SOURCE_SEL to sensor clocks
        Set_CRM_CRM_STATIC_CLK_SD_HISPEED_SOURCE_SEL(STATIC_CLK_SD_HISPEED_SOURCE_SEL_B_0x0);

        // Set CRM_STATIC_CLK_SD_LOSPEED_SOURCE_SEL to sensor clocks
        Set_CRM_CRM_STATIC_CLK_SD_LOSPEED_SOURCE_SEL(STATIC_CLK_SD_LOSPEED_SOURCE_SEL_B_0x0);

        // Set CRM_STATIC_CLK_PIPE_SOURCE_SEL to sensor clocks
        Set_CRM_CRM_STATIC_CLK_PIPE_SOURCE_SEL(STATIC_CLK_PIPE_SOURCE_SEL_B_0x0);
    }
    else
    {
        if(Is_8540v1())
         {
           Set_CRM_CRM_EN_CLK_PIPE(1, 1, 1, 1, 1, 1, 1);  // CRM_EN_CLK_PIPE  //EN_CLK_PIPE_DXO 
         }
        else
         {
          Set_CRM_CRM_EN_CLK_PIPE(1, 1, 1, 1, 1, 1, 0);  // CRM_EN_CLK_PIPE //EN_CLK_PIPE_DXO  NOT PRESENT
         }

        // set clk_sd_hispeed, clk_sd_lospeed source to emulated sensor clocks
        Set_CRM_CRM_STATIC_CLK_SD_HISPEED_SOURCE_SEL(STATIC_CLK_SD_HISPEED_SOURCE_SEL_B_0x0);
        Set_CRM_CRM_STATIC_CLK_SD_LOSPEED_SOURCE_SEL(STATIC_CLK_SD_LOSPEED_SOURCE_SEL_B_0x0);

        // enable clk_sd_hispeed, clk_sd_lospeed
        Set_CRM_CRM_EN_CLK_SD_HISPEED(1, 1);
        Set_CRM_CRM_EN_CLK_SD_LOSPEED(1, 1);

        // disable input to SDP Pipe
        Set_ISP_SD_IDP_GATE_ISP_SD_IDP_GATE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);                   // mux2to1_enable,mux2to1_soft_reset

        // disable clk_sd_hispeed, clk_sd_lospeed
        Set_CRM_CRM_EN_CLK_SD_HISPEED(0, 0);
        Set_CRM_CRM_EN_CLK_SD_LOSPEED(0, 0);

        // clock source for the ISP will be clk_pipe_in, enable clk_pipe_in
        Set_CRM_CRM_EN_CLK_PIPE_IN(1);

        // TODO: disable clk_gpio
        // enable clk_bml
        Set_CRM_CRM_EN_CLK_BML(1);

        // set the CRM_STATIC_CLK_BMS_SOURCE_SEL to be clk_pipe_in
        Set_CRM_CRM_STATIC_CLK_BMS_SOURCE_SEL(STATIC_CLK_BMS_SOURCE_SEL_B_0x1);

        // CRM_STATIC_CLK_SD_HISPEED_SOURCE_SEL does not matter, it is disabled
        // CRM_STATIC_CLK_SD_LOSPEED_SOURCE_SEL does not matter, it is disabled
        // Set CRM_STATIC_CLK_PIPE_SOURCE_SEL to clk_pipe_in
        Set_CRM_CRM_STATIC_CLK_PIPE_SOURCE_SEL(STATIC_CLK_PIPE_SOURCE_SEL_B_0x1);
    }


    // we have worked out the state of clk_sd_hispeed, clk_sd_lospeed and clk_gpio and clk_bml
    // we now need to work out clk_bms and clk_pipe
    // if either of the pipes are enabled or bayer store 1 or bayer store 2
    // or RGB store is enabled then clk_pipe must be enabled

    /*
                if
                (
                    SystemConfig_IsAnyPipeActive()
                ||  SystemConfig_IsRGBStoreActive()
                ||  SystemConfig_IsBayerStore2Active()
                ||  SystemConfig_IsBayerStore1Active()
                )
                {
                    // enable clk_pipe
                    Set_CRM_CRM_EN_CLK_PIPE(1);
                }
                else
                {
                    // disable clk_pipe
                    Set_CRM_CRM_EN_CLK_PIPE(0);
                }
            */
         if(Is_8540v1())
          {
            Set_CRM_CRM_EN_CLK_PIPE(1, 1, 1, 1, 1, 1, 1);  // CRM_EN_CLK_PIPE  //EN_CLK_PIPE_DXO 
          }
         else
          {
           Set_CRM_CRM_EN_CLK_PIPE(1, 1, 1, 1, 1, 1, 0);  // CRM_EN_CLK_PIPE //EN_CLK_PIPE_DXO  NOT PRESENT
          }
   
    if (SystemConfig_IsDMCE_Active())
    {
        // the DMCE is active, enable both dmce_bayer_load and dmce_rgb_load mux
        Set_ISP_DMCE_BAYER_LOAD_ISP_DMCE_BAYER_LOAD_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);            // mux2to1_enable,mux2to1_soft_reset
        Set_ISP_DMCE_RGB_LOAD_ISP_DMCE_RGB_LOAD_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);                // mux2to1_enable,mux2to1_soft_reset
        if (Is_8500v2() || Is_9540v1()||Is_8540v1())
        {
            // These are to be modified later based on requirements.
            // Disable Channel Merge
            Set_ISP_CHANNELMERGE_ISP_CHANNELMERGE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);             // mux2to1_enable,mux2to1_soft_reset

            // Disable LBE (Line Burstiness Elimination)
            Set_ISP_LINE_BURSTY_ELIM_ISP_LINE_BURSTY_ELIM_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);     // mux2to1_enable,mux2to1_soft_reset

            // Enable DMCE mux demosaic
            Set_ISP_DMCE_MUX_DEMOSAIC_ISP_DMCE_MUX_DEMOSAIC_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);    // mux2to1_enable,mux2to1_soft_reset

            // Select Babylon by default, as the source of demosaic mux.
            Set_ISP_DMCE_BAYER_LOAD_ISP_DMCE_BAYER_LOAD_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);           // mux2to1_select,mux2to1_shadow_en
        }
    }
    else
    {
        // the DMCE is not active, disable both dmce_bayer_load and dmce_rgb_load mux
        Set_ISP_DMCE_BAYER_LOAD_ISP_DMCE_BAYER_LOAD_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);           // mux2to1_enable,mux2to1_soft_reset
        Set_ISP_DMCE_RGB_LOAD_ISP_DMCE_RGB_LOAD_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);               // mux2to1_enable,mux2to1_soft_reset
        if (Is_8500v2() || Is_9540v1() || Is_8540v1() )
        {
            // Disable DMCE mux demosaic
            Set_ISP_DMCE_MUX_DEMOSAIC_ISP_DMCE_MUX_DEMOSAIC_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);   // mux2to1_enable,mux2to1_soft_reset
        }
    }


    if (SystemConfig_IsRE_Active())
    {
        // the RE is active, enable re_bayer_load mux
        Set_ISP_RE_BAYER_LOAD_ISP_RE_BAYER_LOAD_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);                // mux2to1_enable,mux2to1_soft_reset
        if (Is_8500v2() || Is_9540v1() || Is_8540v1())
        {
            // Disable Post Gain; To be modified later based on requirements
            Set_ISP_ITPOINT_POST_GAIN_ISP_ITPOINT_POST_GAIN_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);   // mux2to1_enable,mux2to1_soft_reset
        }
    }


    // enable the input data path that is active for the current operation
    if (SystemConfig_IsInputImageSourceBayerLoad1())
    {
        // set RE bayer load1 source to load data
        Set_ISP_RE_BAYER_LOAD_ISP_RE_BAYER_LOAD_SELECT(mux2to1_select_IDP1, mux2to1_shadow_en_B_0x1);                   // mux2to1_select,mux2to1_shadow_en

        // set dmce bayer load2 source to IDP data
        Set_ISP_DMCE_BAYER_LOAD_ISP_DMCE_BAYER_LOAD_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);               // mux2to1_select,mux2to1_shadow_en

        // set dmce rgb load source to IDP data
        Set_ISP_DMCE_RGB_LOAD_ISP_DMCE_RGB_LOAD_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);                   // (mux2to1_select,mux2to1_shadow_en
    }
    else if (SystemConfig_IsInputImageSourceBayerLoad2())
    {
        // set RE bayer load1 source to IDP data
        Set_ISP_RE_BAYER_LOAD_ISP_RE_BAYER_LOAD_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);                   // mux2to1_select,mux2to1_shadow_en

        // set dmce bayer load2 source to load data
        Set_ISP_DMCE_BAYER_LOAD_ISP_DMCE_BAYER_LOAD_SELECT(mux2to1_select_IDP1, mux2to1_shadow_en_B_0x1);               // mux2to1_select,mux2to1_shadow_en

        // set dmce rgb load source to IDP data
        Set_ISP_DMCE_RGB_LOAD_ISP_DMCE_RGB_LOAD_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);                   // (mux2to1_select,mux2to1_shadow_en
    }
    else if (SystemConfig_IsInputImageSourceRGBLoad())
    {
        // set RE bayer load1 source to IDP data
        Set_ISP_RE_BAYER_LOAD_ISP_RE_BAYER_LOAD_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);                   // mux2to1_select,mux2to1_shadow_en

        // set dmce bayer load2 source to IDP data
        Set_ISP_DMCE_BAYER_LOAD_ISP_DMCE_BAYER_LOAD_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);               // mux2to1_select,mux2to1_shadow_en

        // set dmce rgb load source to IDP data
        Set_ISP_DMCE_RGB_LOAD_ISP_DMCE_RGB_LOAD_SELECT(mux2to1_select_IDP1, mux2to1_shadow_en_B_0x1);                   // (mux2to1_select,mux2to1_shadow_en
    }
    else
    {
        // No Bayer or rgb load is active, so connect pipe output
        // set RE bayer load1 source to IDP data
        Set_ISP_RE_BAYER_LOAD_ISP_RE_BAYER_LOAD_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);                   // mux2to1_select,mux2to1_shadow_en

        // set dmce bayer load2 source to IDP data
        Set_ISP_DMCE_BAYER_LOAD_ISP_DMCE_BAYER_LOAD_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);               // mux2to1_select,mux2to1_shadow_en

        // set dmce rgb load source to IDP data
        Set_ISP_DMCE_RGB_LOAD_ISP_DMCE_RGB_LOAD_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);                   // (mux2to1_select,mux2to1_shadow_en
    }


    if (SystemConfig_IsInputImageSourceSensor0())
    {
        /*
         Sensor switch is managed at boot time. if sensor needs to be changed, please use change sensor command
        */

        // sensor0 is the input image source
        // pull XSHUTDOWN for sensor0 high
        // pull XSHUTDOWN for sensor1 low
    }
    else if (SystemConfig_IsInputImageSourceSensor1())
    {
        /*
         Sensor switch is managed at boot time. if sensor needs to be changed, please use change sensor command
        */

        // sensor1 is the input image source
        // TODO: pull XSHUTDOWN for sensor1 high
        // TODO: pull XSHUTDOWN for sensor0 low
    }
    else if (SystemConfig_IsInputImageSourceRx())
    {
        /* TODO: <AG>: We should be able to test Rx streaming at run time :) for test purposes */

        // Rx is the input image source
        // TODO: pull XSHUTDOWN for sensor1 low
        // TODO: pull XSHUTDOWN for sensor0 low
    }


    if (SystemConfig_IsPipe0Active())
    {
        // enable input to pipe0
        Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);                  // mux2to1_enable,mux2to1_soft_reset
        Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);                     // mux2to1_select,mux2to1_shadow_en

        //for mult error: enable shadow behaviour of gps_mux, cropper, sharp and unsharp mux.
        Set_ISP_CE0_MUX_GPS_ISP_CE0_MUX_GPS_SELECT_mux2to1_shadow_en__B_0x1();
        Set_ISP_CE0_MUX_SHARP_ISP_CE0_MUX_SHARP_SELECT_mux2to1_shadow_en__B_0x1();
        Set_ISP_CE0_MUX_UNSHARP_ISP_CE0_MUX_UNSHARP_SELECT_mux2to1_shadow_en__B_0x1();
        Set_ISP_CE0_CROP_ISP_CE0_CROP_SHADOW_CROPS_crop_shadow_crops(1);
    }
    else
    {
        // disable input to pipe0
        Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);                 // mux2to1_enable,mux2to1_soft_reset
    }


    if (SystemConfig_IsPipe1Active())
    {
        // enable input to pipe1
        Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);                  // mux2to1_enable,mux2to1_soft_reset
        Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);                     // mux2to1_select,mux2to1_shadow_en

         //for mult error: enable shadow behaviour of gps_mux, cropper, sharp and unsharp mux.
        Set_ISP_CE1_MUX_GPS_ISP_CE1_MUX_GPS_SELECT_mux2to1_shadow_en__B_0x1();
        Set_ISP_CE1_MUX_SHARP_ISP_CE1_MUX_SHARP_SELECT_mux2to1_shadow_en__B_0x1();
        Set_ISP_CE1_MUX_UNSHARP_ISP_CE1_MUX_UNSHARP_SELECT_mux2to1_shadow_en__B_0x1();
        Set_ISP_CE1_CROP_ISP_CE1_CROP_SHADOW_CROPS_crop_shadow_crops(1);
    }
    else
    {
        // disable input to pipe1
        Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);                 // mux2to1_enable,mux2to1_soft_reset
    }


    return;
}


void
DisableMemoryBasedHardware(void)
{
    //Once all Clocks are enabled, Specifically Disable all the IP's which need to access Memories
    //SDL Disable
    SDL_DisableHW();

    //Gridiron Disable
    Set_ISP_GRIDIRON_ISP_GRIDIRON_ENABLE_enable_gridiron__DISABLE();

    // Enable GridIron Mem_init
    Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_mem_init__B_0x1();

    //Duster Disable
    Set_ISP_DUSTER_ISP_DUSTER_ENABLE_duster_enable__DISABLE();

    //CE0 Gamma Sharp Disable
    Set_ISP_CE0_FLEXTF_SHARP_ISP_CE0_FLEXTF_SHARP_ENABLE_enable__DISABLE();

    //CE0 Gamma Unsharp Disable
    Set_ISP_CE0_FLEXTF_UNSHARP_ISP_CE0_FLEXTF_UNSHARP_ENABLE_enable__DISABLE();

    //CE1 Gamma Sharp Disable
    Set_ISP_CE1_FLEXTF_SHARP_ISP_CE1_FLEXTF_SHARP_ENABLE_enable__DISABLE();

    //CE1 Gamma Unsharp Disable
    Set_ISP_CE1_FLEXTF_UNSHARP_ISP_CE1_FLEXTF_UNSHARP_ENABLE_enable__DISABLE();
}


void
SystemConfig_RunTimePipeEnable(void)
{
    // If any pipe is enabled o the fly, fire dummy vid complete to configure pipe and enable output
    if (Flag_e_TRUE == g_DataPathControl.e_Flag_Pipe0Enable && Flag_e_FALSE == g_DataPathStatus.e_Flag_Pipe0Enable)
    {
        g_SystemConfig_Status.e_Flag_FireDummyVidComplete0 = Flag_e_TRUE;
    }


    if (Flag_e_TRUE == g_DataPathControl.e_Flag_Pipe1Enable && Flag_e_FALSE == g_DataPathStatus.e_Flag_Pipe1Enable)
    {
        g_SystemConfig_Status.e_Flag_FireDummyVidComplete1 = Flag_e_TRUE;
    }


    // populate the appropriate status elements
    g_DataPathStatus.e_Flag_Pipe0Enable = g_DataPathControl.e_Flag_Pipe0Enable;
    g_DataPathStatus.e_Flag_Pipe1Enable = g_DataPathControl.e_Flag_Pipe1Enable;

    g_DataPathStatus.e_Flag_PipeRAWEnable = g_DataPathControl.e_Flag_PipeRAWEnable;

    //SystemConfig_RunTimeDataPathUpdate();
    // Get the pipe0 params
    if (SystemConfig_IsPipe0Valid())
    {
        g_PipeState[0].u16_OutputSizeX = SystemConfig_GetPipe0OpSizeX();
        g_PipeState[0].u16_OutputSizeY = SystemConfig_GetPipe0OpSizeY();
        g_PipeState[0].f_AspectRatio = ((float_t) g_PipeState[0].u16_OutputSizeX) / ((float_t) g_PipeState[0].u16_OutputSizeY);

        g_PipeState[0].e_Flag_Enabled = SystemConfig_IsPipe0Active();
    }
    else
    {
        g_PipeState[0].e_Flag_Enabled = Flag_e_FALSE;
    }


    // Get the pipe1 params if present
    if ((PIPE_COUNT > 1) && SystemConfig_IsPipe1Valid())
    {
        g_PipeState[1].u16_OutputSizeX = SystemConfig_GetPipe1OpSizeX();
        g_PipeState[1].u16_OutputSizeY = SystemConfig_GetPipe1OpSizeY();
        g_PipeState[1].f_AspectRatio = ((float_t) g_PipeState[1].u16_OutputSizeX) / ((float_t) g_PipeState[1].u16_OutputSizeY);

        g_PipeState[1].e_Flag_Enabled = SystemConfig_IsPipe1Active();
    }
    else
    {
        g_PipeState[1].e_Flag_Enabled = Flag_e_FALSE;
    }


    if (SystemConfig_IsPipe0Active())
    {
        // enable is done in vid complete
        // activate shadow here
        //Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);        // mux2to1_enable,mux2to1_soft_reset
        Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);                     // mux2to1_select,mux2to1_shadow_en
    }
    else
    {
        // disable input to pipe0
        Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);                 // mux2to1_enable,mux2to1_soft_reset
    }


    if (SystemConfig_IsPipe1Active())
    {
        // enable is done in vid complete
        // activate shadow here
        //Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);        // mux2to1_enable,mux2to1_soft_reset
        Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_SELECT(mux2to1_select_IDP0, mux2to1_shadow_en_B_0x1);                     // mux2to1_select,mux2to1_shadow_en
    }
    else
    {
        // disable input to pipe1
        Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);                 // mux2to1_enable,mux2to1_soft_reset
    }
}


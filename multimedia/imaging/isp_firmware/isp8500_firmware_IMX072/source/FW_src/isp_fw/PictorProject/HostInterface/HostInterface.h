/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if        INCLUDE_IN_HTML_ONLY
 \file      HostInterface.h

 \brief     This file exposes an interface to allow other modules of the device
            to access the Host Interface module.

 \ingroup   HostInterface
 \endif
*/
#ifndef _HOSTINTERFACE_H_
#   define _HOSTINTERFACE_H_

#   include "Platform.h"
#   include "SleepManager.h"
#   include "PipeAlgorithm.h"
#   include "Stream.h"
#   include "Exposure_OPInterface.h"
#   include "Profiler.h"
#   include "histogram_op_interface.h"
#   include "Glace_OPInterface.h"
#   include "Glace_IPInterface.h"
#   include "lla_abstraction.h"
#   include "FrameRate.h"
#   include "DeviceParameter.h"
#   include "RgbToYuvMatrix.h"
#   include "YuvToRgbMatrix.h"

typedef enum
{
    // The device has been powered up but not received any command
    // Page element read writes possible in this state
    HostInterfaceLowLevelState_e_RAW_UNINITIALISED  = 0x00,

    // The device has received a BOOT command and is processing it
    HostInterfaceLowLevelState_e_BOOTING            = 0x01,

    // Stopped state, no image output
    HostInterfaceLowLevelState_e_STOPPED            = 0x10,

    // Rx stopped state, sensor streaming but Rx is stopped
    HostInterfaceLowLevelState_e_RXSTOPPED            = 0x13,

    // Program the relevant data paths for the current streaming operation
    HostInterfaceLowLevelState_e_DATA_PATH_SETUP    = 0x20,

    // Compute all parameters like video timing, frame dimension, exposure etc.
    HostInterfaceLowLevelState_e_ALGORITHM_UPDATE   = 0x21,

    // Program the various sensor parameters for the current streaming operation
    HostInterfaceLowLevelState_e_SENSOR_COMMIT      = 0x22,

    // Program the ISP for the current streaming operation
    HostInterfaceLowLevelState_e_ISP_SETUP          = 0x23,

    // Command the low level state machine to run
    HostInterfaceLowLevelState_e_COMMANDING_RUN     = 0x24,

    // Wait for the state machine to start streaming
    HostInterfaceLowLevelState_e_WAITING_FOR_RUN    = 0x25,

    // Running state
    HostInterfaceLowLevelState_e_RUNNING            = 0x30,

    // Command the low level state machine to stop
    HostInterfaceLowLevelState_e_COMMANDING_STOP    = 0x31,

    // Wait for the state machine to stop
    HostInterfaceLowLevelState_e_WAITING_FOR_STOP   = 0x32,

    // The device is preparing to switch to low power state
    HostInterfaceLowLevelState_e_GOING_TO_SLEEP     = 0x11,

    // The device is preparing to switch to low power state
    HostInterfaceLowLevelState_e_RESET_ISP          = 0x12,

    // The device is in low power state
    HostInterfaceLowLevelState_e_SLEPT              = 0x40,

    // The device is preparing to swith to high power non streaming state
    HostInterfaceLowLevelState_e_WAKING_UP          = 0x41,

    // Error state, the device is entering this state after a
    // failed attempt to start streaming
    HostInterfaceLowLevelState_e_ENTERING_ERROR     = 0x50,

    // Error state, the device enters this state after a
    // failed attempt to start streaming
    HostInterfaceLowLevelState_e_ERROR              = 0x51
} HostInterfaceLowLevelState_te;

typedef enum
{
    // The device has been powered up
    HostInterfaceHighLevelState_e_INIT              = 0x00,

    // The device is in stopped state, no streaming output is being generated.
    // The device is not processing any image data
    HostInterfaceHighLevelState_e_STOPPED           = 0x10,

    // The device is starting the streaming operation
    HostInterfaceHighLevelState_e_SETUP             = 0x20,

    // The device is actively streaming and generating output image
    HostInterfaceHighLevelState_e_RUNNING           = 0x30,

    // The device is currently in a low power state
    HostInterfaceHighLevelState_e_SLEPT             = 0x40,

    // The device is currently in a low power state
    HostInterfaceHighLevelState_e_ERROR             = 0x50
} HostInterfaceHighLevelState_te;

typedef enum
{
    /// Initial or default command
    HostInterfaceCommand_e_INIT_COMMAND,

    /// BOOT command should be issued only once when system is powered up.
    /// Sensor will be in S/W standby mode after BOOT
    HostInterfaceCommand_e_BOOT,

    /// RUN command stream the system
    HostInterfaceCommand_e_RUN,

    /// STOP the sensor and the ISP
    HostInterfaceCommand_e_STOP,

    /// STOP the ISP
    HostInterfaceCommand_e_STOP_ISP,

    /// Command used internally by fimware
    HostInterfaceCommand_e_AUTO_STOP,

    /// SLEEP will put sensor in H/W standby mode. It should be be used to put sensor in low power mode.
    /// After SLEEP, In H/W standby state, Sensor looses all the user programmed registers.
    HostInterfaceCommand_e_SLEEP,

    /// WAKEUP will put sensor in S/W standby mode and I2C read / write are possible.
    /// After WAKEUP, User should right all the required sensor registers again.
    HostInterfaceCommand_e_WAKEUP,

    /// SWTICH SENSOR will allow user to choose sensor. The command can be issued only in STOPPED State.
    /// After swithc sensor command user must change active sensor and then issue BOOT.
    /// After sensor switch, Firmware will discard all the information of previous selected sensor.
    HostInterfaceCommand_e_SWITCH_SENSOR,

    /// RESET_ISP will allow host to reset the ISP, whenever a issue is detected at HOST side.
    /// This is needed for wake up & debug phases.
    /// The FW will reset all the IPs in the ISP.
    HostInterfaceCommand_e_RESET_ISP
} HostInterfaceCommand_te;

/**
 \struct    FrameParamStatus_ts
 \brief     Page for holding sensor exposure and pipe WB gain values exported to memory
 \ingroup   Glace
*/
typedef struct
{
    /// Memory address where the device will dump actual sensor exposure and pipe DG parameters
    uint32_t    *ptru32_SensorParametersTargetAddress;

    /// Used exposure time in microseconds
    uint32_t    u32_ExposureTime_us;

    /// Used analogue gain as multiplier (units: gain x 256, e.g. 1536)
    uint32_t    u32_AnalogGain_x256;

    /// Current Red  channel gain
    uint32_t    u32_RedGain_x1000;

    /// Current Green  channel gain
    uint32_t    u32_GreenGain_x1000;

    /// Current Blue channel gain
    uint32_t    u32_BlueGain_x1000;

    /// count of frame (1-256)
    uint32_t    u32_frame_counter;

    // frame rate
    uint32_t    u32_frameRate_x100;

    /// flash-lit frame indicator
    /// Value = 1 indicate it is flash lit frame
    /// Value = 0 indicate it is normal frame
    uint32_t    u32_flash_fired;

    /// Signal Level with or without ND Filter. A provided value of 1250 means a transparency of 12.5%.
    /// A value of 0 means ND Filter is not supported.
    /// [DEFAULT]:  0
    uint32_t    u32_NDFilter_Transparency_x100;

    /// Current status of ND Filter (whether applied or not), provided it is supported.
    /// [DEFAULT]: 0
    uint32_t    u32_Flag_NDFilter;

    /// exposure quantisation step
    /// [DEFAULT]: 0
    uint32_t    u32_ExposureQuantizationStep_us;

    /// Active data readout time in microsecs
    /// [DEFAULT]: 0
    uint32_t    u32_ActiveData_ReadoutTime_us;

    /// Minimum exposure time in micro seconds
    uint32_t    u32_SensorExposureTimeMin_us;

    /// Maximum exposure time in micro seconds
    uint32_t    u32_SensorExposureTimeMax_us;

    /// Currently applied f_numberx100 in the sensor
    uint32_t    u32_applied_f_number_x100;
} FrameParamStatus_ts;

/**
 \struct    FrameParamStatus_Extn_ts
 \brief     Page for holding sensor exposure and pipe WB gain values exported to memory
 \ingroup   Glace
*/
typedef struct
{
    /// Minimum analog gain imposed by sensor (units: gain x 256, e.g. 1536)
    uint32_t    u32_SensorParametersAnalogGainMin_x256;

    /// Maximum analog gain imposed by sensor (units: gain x 256)
    uint32_t    u32_SensorParametersAnalogGainMax_x256;

    /// Analog gain step size (units: gain x 256)
    uint32_t    u32_SensorParametersAnalogGainStep_x256;

    /// Whether stats exported are valid or not. \n
    /// Value = 0 indicates stats are valid. \n
    /// Value = 1 indicates stats are invalid, and notification is to be ignored. \n
    /// [DEFAULT]: 1 [INVALID]
    uint32_t    u32_StatsInvalid;

     /// "sizeof(FrameParamStatus_ts) + sizeof(FrameParamStatus_Extn_ts)"
    uint32_t    u32_SizeOfFrameParamStatus;

    /// Sensor Focal length
    uint32_t    u32_focal_length_x100;
} FrameParamStatus_Extn_ts;

/**
 \struct    FrameParamStatus_Af_ts
 \brief     Page for exporting Af values exported to memory
 \ingroup   HostInterface
*/
typedef struct
{
    /// host specified address for FrameParamStatus_Af exporting to external memory .
    uint32_t    *pu32_HostAssignedFrameStatusAddr;

    /// Whether or not the AF stats are valid. \n
    /// Value = 1 indicates stats are valid. \n
    /// Value = 0 indicates stats are invalid. \n
    /// [DEFAULT]: 0 [INVALID]
    uint32_t    u32_AfStatsValid;

    /// Current Lens Position
    uint32_t    u32_AfStatsLensPos;

    /// Frame Id in which AF has exported valid statistics
    uint32_t    u32_AfStatsFrameId;

     /// "sizeof(FrameParamStatus_ts) + sizeof(FrameParamStatus_Extn_ts) + sizeof(FrameParamStatus_Af_ts)"
    uint32_t    u32_SizeOfFrameParamStatus;
} FrameParamStatus_Af_ts;

/** Structure that defines one output mode.
    Frame rate given is the maximum possible frame rate for that
    mode, i.e. lower frame rates can be programmed.
    ==> Note : 'u32_woi_res_width' and 'u32_woi_res_height' are not kept
    in a separate structure because of issue with 8500DocumentParameters.py.
    This script blindly adds "isp_" prefix to the name of member variables, which
    was causing compilation problem for HOST test application. */
typedef struct
{
    ///  WOI resolution width for the mode
    uint32_t    u32_woi_res_width;

    ///  WOI resolution height for the mode
    uint32_t    u32_woi_res_height;

    /// Output resolution width for the mode
    uint32_t    u32_output_res_width;

    /// Output resolution height for the mode
    uint32_t    u32_output_res_height;

    ///  Data format of this mode
    /// 0x0A0A    :    CAM_DRV_SENS_FORMAT_RAW10
    /// 0x0808    :   CAM_DRV_SENS_FORMAT_RAW8
    /// 0x0A08    :   CAM_DRV_SENS_FORMAT_RAW8_DPCM
    /// 0x0A06    :   CAM_DRV_SENS_FORMAT_RAW10TO6_DPCM
    uint32_t    u32_data_format;

    /// restriction mask
    /// not being used as of now
    uint32_t    u32_usage_restriction_bitmask;

    /// Maximum frame rate for this mode
    uint32_t    u32_max_frame_rate_x100;
} Sensor_Output_Mode_ts;

typedef enum
{
    HostInterfaceRunSubCommand_e_COLD_START,
    HostInterfaceRunSubCommand_e_HALF_BUTTON_PRESS,
    HostInterfaceRunSubCommand_e_HALF_BUTTON_RELEASE,
    HostInterfaceRunSubCommand_e_FULL_BUTTON_PRESS,
    HostInterfaceRunSubCommand_e_FULL_BUTTON_RELEASE
} HostInterfaceRunSubCommand_te;

#   define DEFAULT_TEST_PATTERN_DATA   (0x0100)

typedef enum
{
    /// Normal streaming operation
    TestPattern_e_Normal,

    /// Solid colour
    TestPattern_e_SolidColour,

    /// 100% Solid colour bars
    TestPattern_e_SolidColourBars,

    /// Face to grey colour bars
    TestPattern_e_SolidColourBarsFade,

    /// PN9 pattern
    TestPattern_e_PN9,

    /// No supported
    TestPattern_e_NotSupported,
} TestPattern_te;

typedef enum
{
    /// Output data format of the sensor is RAW10
    DataFormat_e_RAW10,

    /// Output data format of the sensor is RAW8
    DataFormat_e_RAW8,

    /// Output data format of the sensor is RAW8 compressed with DPCM/PCM encoder with simple predictor
    DataFormat_e_RAW8_DPCM,

    /// Output data format of the sensor is RAW6 compressed from RAW10 with DPCM/PCM encoder with simple predictor
    DataFormat_e_RAW10TO6_DPCM,
} DataFormat_te;

/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct    HostInterface_Control_ts
 \brief     Control page of the HostInterface module. All commands
            to the device are issued through this page.
 \ingroup   HostInterface
 \endif
*/
typedef struct
{
    /// Set to Flag_e_TRUE to single step through the state machine states.
    /// If e_Flag_StepThroughStates == Flag_e_TRUE, then one cycle of the state
    /// machine will be executed if e_Coin_StepThroughControl != e_Coin_StepThroughStatus.
    /// After execution of a single cycle, e_Coin_StepThroughStatus
    /// becomes equal to e_Coin_StepThroughControl.
    uint8_t e_Flag_StepThroughStates;

    /// Control coin to single step through the state machine states.
    /// Valid only when e_Flag_StepThroughStates == Flag_e_TRUE. To single step
    /// through the state machine states, flip the value of e_Coin_StepThroughControl.
    uint8_t e_Coin_StepThroughControl;

    /// The command element of the state machine.
    /// All commands to the state machine should
    /// be issued through this element.
    uint8_t e_HostInterfaceCommand_User;

    /// Sub command applicable to the RUN command
    uint8_t e_HostInterfaceRunSubCommand;
} HostInterface_Control_ts;

/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct    HostInterface_Status_ts
 \brief     Status page of the HostInterface module.
 \ingroup   HostInterface
 \endif
*/
typedef struct
{
    /// Current low level state of the device
    uint8_t e_HostInterfaceLowLevelState_Current;

    /// Current high level state of the device
    uint8_t e_HostInterfaceHighLevelState;

    /// Next low level state of the device
    uint8_t e_HostInterfaceLowLevelState_Next;

    /// Status coin to single step through the state machine states.
    /// Valid only when e_Flag_StepThroughStates == Flag_e_TRUE. When single stepping
    /// through the state machine states, after execution of a
    /// single cycle of the state machine, e_Coin_StepThroughStatus becomes
    /// equal to e_Coin_StepThroughControl.
    uint8_t e_Coin_StepThroughStatus;
} HostInterface_Status_ts;

/**
\struct  TestPattern_ts
\brief   Control parameters for the test pattern
\ingroup self_test
*/
typedef struct
{
    /// The test data used to replace red pixel data
    /// Valid only for TestPattern_SolidColour
    uint16_t    u16_test_data_red;

    /// The test data used to replace green pixel data on rows that also have red pixels
    /// Valid only for TestPattern_SolidColour
    uint16_t    u16_test_data_greenR;

    /// The test data used to replace blue pixel data
    /// Valid only for TestPattern_SolidColour
    uint16_t    u16_test_data_blue;

    /// The test data used to replace green pixel data on rows that also have blue pixels
    /// Valid only for TestPattern_SolidColour
    uint16_t    u16_test_data_greenB;

    /// Test pattern from sensor
    /// [Note]: Set the parameter before RUN command
    uint8_t     e_TestPattern;
} TestPattern_ts;

/**
 \struct    ReadLLAConfig_Control_ts
 \brief
 \ingroup
 \endif
*/
typedef struct
{
    /// pointer to memory location to which sensor streaming configuration will be copied
    uint32_t    ptr32_Sensor_Output_Mode_Data_Address;

    /// Select whiich configuration to read
    uint16_t    u16_SelectMode;

    /// Control coin
    uint8_t     e_Coin_ReadLLAConfigControl;
} ReadLLAConfig_Control_ts;

/**
 \struct    ReadLLAConfig_Status_ts
 \brief
 \ingroup
 \endif
*/
typedef struct
{
    /// size of sensor mode data
    uint32_t    u32_sensor_Output_Mode_data_size;

    /// Number of modes supported by LLD
    uint16_t    u16_number_of_modes;

    /// Width of the WOI of the selected mode
    uint16_t    u16_woi_res_width;

    /// Height of the WOI of the selected mode
    uint16_t    u16_woi_res_height;

    /// Width of the output resolution of selected mode
    uint16_t    u16_output_res_width;

    /// Height of the output resolution of selected mode
    uint16_t    u16_output_res_height;

    /// Max frame rate of the selected mode
    uint16_t    u16_max_frame_rate_x100;

    ///  Data format of this mode
    /// 0x0A0A   :    CAM_DRV_SENS_FORMAT_RAW10
    /// 0x0808    :   CAM_DRV_SENS_FORMAT_RAW8
    /// 0x0A08    :   CAM_DRV_SENS_FORMAT_RAW8_DPCM
    /// 0x0A06    :   CAM_DRV_SENS_FORMAT_RAW10TO6_DPCM
    uint16_t    u16_data_format;

    /// Status coin ReadLLAConfig_Status_ts becomes
    /// equal to e_Coin_ReadLLAConfigControl when values
    /// of the requested mode are populated
    uint8_t     e_Coin_ReadLLAConfigStatus;
} ReadLLAConfig_Status_ts;

typedef struct
{
    /// Frame rate to be used in calculation.
    /// [DEFAULT]: 0
    uint32_t    u32_FrameRate_x100;

    /// WOI X
    /// [DEFAULT]: 0
    uint16_t    u16_WOI_X_size;

    /// WOI size Y
    /// [DEFAULT]: 0
    uint16_t    u16_WOI_Y_size;

    /// Output size X
    /// [DEFAULT]: 0
    uint16_t    u16_X_size;

    /// Output size Y
    /// [DEFAULT]: 0
    uint16_t    u16_Y_size;

    /// for example RAW8: 0x0808, RAW10: 0x0A0A etc.\n
    /// For more details on data format, Please refer SMIA_Functional_specification_1.0.pdf\n
    /// [DEFAULT]: RAW8: 0x0808
    uint16_t    u16_CsiRawFormat;

    /// Control to trigger SMS: (Head/Tails). \n
    /// Host should program a different value than status PE to activate SMS
    /// [DEFAULT]: Coin_e_Heads
    uint8_t     e_Coin_Ctrl;
}SMS_Control_ts;

typedef struct
{
    /// Line length of frame (in pixel clocks)
    /// [DEFAULT]: 0
    uint32_t    u32_LineLength_pck;

    /// Length of frame (in lines)
    /// [DEFAULT]: 0
    uint32_t    u32_FrameLength_lines;

    /// Minimum exposure that can be configured and achieved (in micro secs)
    /// [DEFAULT]: 0
    uint32_t    u32_Min_ExposureTime_us;

    /// Maximum exposure that can be configured and achieved (in micro secs)
    /// [DEFAULT]: 0
    uint32_t    u32_Max_ExposureTime_us;

    /// exposure quantisation step
    /// [DEFAULT]: 0
    uint32_t    u32_ExposureQuantizationStep_us;

    /// Active Frame Data Readout time (in micro secs)
    /// [DEFAULT]: 0
    uint32_t    u32_ActiveData_ReadoutTime_us;

    /// Status Coin: (Head/Tails)
    /// [DEFAULT]: Coin_e_Heads
    uint8_t     e_Coin_Status;
} SMS_Status_ts;

/**
 \enum PowerCommand_te
 \brief SMIA++ power on commands.
 \ingroup
*/
typedef enum
{
    /// Switch on voltage
    PowerCommand_e_voltage_on,

    /// Switch off voltage
    PowerCommand_e_voltage_off,

    /// Switch on external clock
    PowerCommand_e_ext_clk_on,

    /// Switch off external clock
    PowerCommand_e_ext_clk_off,

    /// xshutdown on
    PowerCommand_e_x_shutdown_on,

    /// xshutdown off
    PowerCommand_e_x_shutdown_off,
} PowerCommand_te;

/**
 \enum StatisticsFov_te
 \brief Mode of glace/histogram geometry selection
 \ingroup
*/
typedef enum
{
    /// Geometry calculations to be based on sensor output size
    StatisticsFov_e_Sensor,
    
    /// Geometry calculations to be based on master pipe FOV
    StatisticsFov_e_Master_Pipe
}StatisticsFov_te;


/**
 \enum PowerAction_te
 \brief SMIA++ power action.
 \ingroup
*/
typedef enum
{
    // no command given
    PowerAction_e_idle,

    // power command requested
    PowerAction_e_requested,

    // power command completed, action to be set by host
    PowerAction_e_complete,
} PowerAction_te;


/**
 \struct sensor_power_management_ts
 \brief
 \ingroup
*/
typedef struct
{
    /// The value of variable is valid when e_PowerCommand == PowerCommand_e_ext_clk_on and e_Flag_command = Flag_e_TRUE
    /// Host should apply the voltage when ext_clk_on command is requested
    /// Typical frequency in first phase: 9.0, 9.6, 10.0, 12.0, 19.2, 26.0 MHz
    uint32_t    u32_ExternalClockFreq_MHz_x100;

    /// Analog voltage
    uint16_t    u16_VoltageAnalog_x100;

    /// Digital voltage
    uint16_t    u16_VoltageDigital_x100;

    /// I/O voltage
    uint16_t    u16_VoltageIO_x100;

    /// command from isp fw to host. Host should read this variable on recieving event from ISP FW.
    /// Based on the command read above PE's
    uint8_t     e_PowerCommand; // RO PE
} SensorPowerManagement_Status_ts;

/**
 \struct sensor_power_management_ts
 \brief
 \ingroup
*/
typedef struct
{
    /// Action PE to HOST. Host should set the value to  PowerAction_e_complete when requested command is complete
    /// Before raising power change sequence event, ISP FW will always set this PE to PowerAction_e_requested and wait for its value to become PowerAction_e_complete.
    /// [DEFAULT] = Flag_e_FALSE
    uint8_t e_PowerAction;                          // RW PE. Shouldn't it be of type PowerAction_te??

    /// Result of operation. Host must program this variable before setting e_PowerAction
    /// Flag_e_FALSE = Failure
    /// Flag_e_TRUE = Success
    uint8_t e_Flag_Result;                          // "expected result". RW PE. Shouldn't it be of type PowerAction_te??

    /// Flag to enable/disable SMIA++ power-up sequence
    /// Flag_e_FALSE = Disable SMIA++ power-up sequence
    /// Flag_e_TRUE = Enable SMIA++ power-up sequence
    uint8_t e_Flag_EnableSMIAPP_PowerUpSequence;    // "expected result". RW PE. Shouldn't it be of type PowerAction_te??
} SensorPowerManagement_Control_ts;

#define DEFAULT_EXT_CLOCK_FREQ_MHZ_X100 (0)
#define DEFAULT_VOLTAGE_ANA_X100 (0)
#define DEFAULT_VOLTAGE_DIG_X100 (0)
#define DEFAULT_VOLTAGE_IO_X100 (0)
#define DEFAULT_POWER_COMMAND (PowerCommand_e_voltage_on)
#define DEFAULT_POWER_ACTION (PowerAction_e_idle)
#define DEFAULT_FLAG_RESULT (Flag_e_FALSE)
#define DEFAULT_ENABLE_SMIAPP_POWER_UP_SEQUENCE (Flag_e_FALSE)

#define SYSTEM_CONFIG_PARAMS_ABSORBED()   \
        ( \
         (SystemConfig_IsSensorSettingStatusParamsReceived() && (Flash_IsFlashStatusIdle() || Flash_IsFlashStatusParamsReceived())) \
         || \
         (Flash_IsFlashStatusParamsReceived() && (SystemConfig_IsSensorSettingStatusIdle() || SystemConfig_IsSensorSettingStatusParamsReceived())) \
        )


#define AEC_STATS_COPIED() \
        ((GlaceHistogramStatus_GlaceDone | GlaceHistogramStatus_HistogramDone) == g_GlaceHistogramStatsFrozen)

#define AEC_STATS_REQUESTED()   \
           ( \
             (g_SystemConfig_Status.e_Coin_Status != g_SystemSetup.e_Coin_Ctrl) \
              || \
             (g_Glace_Status.u8_ControlUpdateCount != g_Glace_Control.u8_ControlUpdateCount_debug) \
              || \
             (g_HistStats_Status.e_CoinStatus != g_HistStats_Ctrl.e_CoinCtrl_debug) \
              || \
             (g_SystemConfig_Status.e_Coin_Glace_Histogram_Status != g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug) \
           )

#define COMPLETE_AEC_STATS_AND_NOTIFY_AFTER_STOP() \
          ( \
           AEC_STATS_REQUESTED() \
           && \
          (Flag_e_FALSE == g_SystemSetup.e_Flag_AecStatsCancel) \
           && \
          (Flag_e_FALSE == g_AecStatsNotifyPending) \
          )


/// Data format programmed for currently active sensor i.e. RAW8 : 0x0808, RAW10: 0x0A0A, 10to08: 0x0A08
#define SMS_GetCsiRawFormat()    READONLY(g_SMS_Control.u16_CsiRawFormat)


extern HostInterface_Control_ts                     g_HostInterface_Control;
extern HostInterface_Status_ts                      g_HostInterface_Status;

extern CpuCycleProfiler_ts                          g_Profile_BOOT;
extern CpuCycleProfiler_ts                          g_Profile_SensorCommit;

extern TestPattern_ts                               g_TestPattern_Ctrl;
extern TestPattern_ts                               g_TestPattern_Status;

extern ReadLLAConfig_Control_ts                     g_ReadLLAConfig_Control;
extern ReadLLAConfig_Status_ts                      g_ReadLLAConfig_Status;

extern SMS_Control_ts                               g_SMS_Control;
extern SMS_Status_ts                                g_SMS_Status;

extern FrameParamStatus_ts                          g_FrameParamStatus;
extern FrameParamStatus_Extn_ts                     g_FrameParamStatus_Extn;
extern FrameParamStatus_Af_ts                       g_FrameParamStatus_Af;
extern volatile SensorPowerManagement_Status_ts     g_SensorPowerManagement_Status;
extern volatile SensorPowerManagement_Control_ts    g_SensorPowerManagement_Control;

volatile extern uint8_t                             g_Config_set_lock;

extern void                                         HostInterface_Run (void);
extern void ResetISPBlocks ( void ) TO_EXT_DDR_PRGM_MEM;
extern void                                         FrameStatusExport (void);

//[NON_ISL_SUPPORT]
extern void                                         OverridePixelOrder(void);

#endif // _HOSTINTERFACE_H_


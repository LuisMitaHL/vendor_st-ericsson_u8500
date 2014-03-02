/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  HostInterface Host Interface

 \details   The HostInterface module provides an interface to the host through which the device
            may be driven. All commands to the device to start streaming, stop streaming, boot up
            etc. will be through this module.
*/

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      HostInterface.c

 \brief     This file contains the main command state machine of the device.

 \ingroup   HostInterface
 \endif
*/
#include "HostInterface.h"
#include "BootManager.h"
#include "SystemConfig.h"
#include "CRM.h"
#include "Stream.h"
#include "Platform.h"
#include "ITM.h"
#include "videotiming_op_interface.h"
#include "ZoomTop.h"
#include "PictorhwReg.h"
#include "IPPhwReg.h"
#include "Gamma_op_interface.h"
#include "run_mode_ctrl.h"
#include "PEDebug.h"
#include "string.h"
#include "HDR.h"


#if INCLUDE_FOCUS_MODULES
#   include "FocusControl_OPInterface.h"
#endif
#include "Aperture.h"
#include "Flash.h"
#include "PipeAlgorithm.h"
#   include "Profiler.h"
#include "Aperture.h"
#include "Sensor_Tuning.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_HostInterface_HostInterfaceTraces.h"
#endif
#include "ErrorHandler.h"

void    StateMachine (void)TO_EXT_PRGM_MEM;
uint8_t AlgorithmUpdate ( void ) TO_EXT_DDR_PRGM_MEM;
void CommandingRun ( void ) TO_EXT_DDR_PRGM_MEM;
void CommandingStop ( void ) TO_EXT_DDR_PRGM_MEM;
void ISPSetup ( void ) TO_EXT_DDR_PRGM_MEM;
void IPPSetup ( void ) TO_EXT_DDR_PRGM_MEM;
uint8_t RawUninitialised ( void ) TO_EXT_DDR_PRGM_MEM;
uint8_t Run (void)TO_EXT_DDR_PRGM_MEM;
void SensorCommit ( void ) TO_EXT_DDR_PRGM_MEM;
uint8_t Sleeping (void)TO_EXT_DDR_PRGM_MEM;
uint8_t Stopped (void)TO_EXT_PRGM_MEM;
uint8_t RxStopped()TO_EXT_PRGM_MEM;
uint8_t WaitingForRun (void)TO_EXT_DDR_PRGM_MEM;
uint8_t WaitingForStop (void)TO_EXT_DDR_PRGM_MEM;
uint8_t HandleError (void)TO_EXT_DDR_PRGM_MEM;
uint8_t ResetISP ( void ) TO_EXT_DDR_PRGM_MEM;
void ResetISPBlocks(void) TO_EXT_DDR_PRGM_MEM;
void                                                ResetSystemConfigState (void) TO_EXT_DDR_PRGM_MEM;
void                                                ResetSystemConfigState_BMS0(void) TO_EXT_DDR_PRGM_MEM;
static void                                         NotifyGlaceHistogramStatistics (void);
static void                                         HandlePendingAecRequest(void);
void                                                CancelAecStatistics (void)TO_EXT_DDR_PRGM_MEM;
static void                                         UpdateFrameStatus (void);
static void                                         CompleteAecNotify(void)TO_EXT_DDR_PRGM_MEM;

static void                                         CommitAecAndFrameRate (void)TO_EXT_DDR_PRGM_MEM;
static void                                         CommitDG (void) ;
static void                                         ReadLLAConfig (void);
static void                                         SetLLAConfig (void)TO_EXT_DDR_PRGM_MEM;
static void                                         Notify_DMA_GRAB (void);
static void                                         ReadLLASensorSettings (void);
extern void                                         NVM_Export (void)TO_EXT_DDR_PRGM_MEM;
static void                                         HostInterface_ISPSettings(void);
static void                                         HostInterface_SensorSettings(void);
static void                                         ApplyISPParams (void);
static void                                         UpdateISPModules (void);
static void                                         CommitISPModules (void);
void                                                HW_CSI2_0_Setup_8500(uint8_t u8_Value, uint8_t u8_NumberOfCSI2DataLines,uint8_t u8_CSI2Version);
void                                                HW_CSI2_0_Setup_9540(uint8_t u8_Value, uint8_t u8_NumberOfCSI2DataLines,uint8_t u8_CSI2Version);

//[NON_ISL_SUPPORT]
void                                                OverridePixelOrder(void);

extern volatile uint16_t           g_FLADriver_LensMoveErrorCount;

volatile SensorPowerManagement_Status_ts            g_SensorPowerManagement_Status =
{
    DEFAULT_EXT_CLOCK_FREQ_MHZ_X100,
    DEFAULT_VOLTAGE_ANA_X100,
    DEFAULT_VOLTAGE_DIG_X100,
    DEFAULT_VOLTAGE_IO_X100,
    DEFAULT_POWER_COMMAND,
};

volatile SensorPowerManagement_Control_ts           g_SensorPowerManagement_Control =
{
    DEFAULT_POWER_ACTION,
    DEFAULT_FLAG_RESULT,
    DEFAULT_ENABLE_SMIAPP_POWER_UP_SEQUENCE,
};

volatile DMA_GRAB_Indicator_Params_ts g_GrabNotify  TO_FIXED_DATA_ADDR_MEM;

volatile uint8_t                                    g_Config_set_lock = Flag_e_FALSE;

CpuCycleProfiler_ts                                 g_Profile_BOOT = { 0, 0, 0, 0 };
CpuCycleProfiler_ts                                 g_Profile_SensorCommit = { 0, 0, 0, 0 };

HostInterface_Control_ts                            g_HostInterface_Control =
{
    Flag_e_FALSE,                           // e_Flag_StepThroughStates
    Coin_e_Heads,                           // e_Coin_StepThroughControl
    HostInterfaceCommand_e_INIT_COMMAND,    // e_Command_User
    HostInterfaceRunSubCommand_e_COLD_START // e_RunSubCommand
};

HostInterface_Status_ts                             g_HostInterface_Status =
{
    HostInterfaceLowLevelState_e_RAW_UNINITIALISED, // e_HostInterfaceLowLevelState_Current
    HostInterfaceHighLevelState_e_INIT,             // e_HostInterfaceHighLevelState
    HostInterfaceLowLevelState_e_RAW_UNINITIALISED, // e_HostInterfaceLowLevelState_Next
    Coin_e_Heads                // e_Coin_StepThroughStatus
};

TestPattern_ts                                      g_TestPattern_Ctrl =
{
    DEFAULT_TEST_PATTERN_DATA,  //u16_test_data_red
    DEFAULT_TEST_PATTERN_DATA,  //u16_test_data_greenR
    DEFAULT_TEST_PATTERN_DATA,  //u16_test_data_blue
    DEFAULT_TEST_PATTERN_DATA,  //u16_test_data_greenB
    TestPattern_e_Normal        //e_TestPattern
};

TestPattern_ts                                      g_TestPattern_Status =
{
    0,                          // u16_test_data_red
    0,                          // u16_test_data_greenR
    0,                          // u16_test_data_blue
    0,                          // u16_test_data_greenB
    TestPattern_e_Normal        // e_TestPattern
};

FrameParamStatus_ts g_FrameParamStatus = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
FrameParamStatus_Extn_ts g_FrameParamStatus_Extn = { 0, 0, 0, DEFAULT_STATS_VALID,0,0};
#if  INCLUDE_FOCUS_MODULES
FrameParamStatus_Af_ts g_FrameParamStatus_Af =
{
    0,                                 //pu32_HostAssignedFrameStatusAddr
    0,                                 //u32_AfStatsValid
    0,                                 //u32_AfStatsLensPos
    0,                                 //u32_AfFrameId
    0                                  //u32_SizeOfFrameParamStatus
};
#ifdef AF_DEBUG
volatile Flag_te            g_request_time_marked = Flag_e_FALSE;
#endif //AF_DEBUG
#endif //INCLUDE_FOCUS_MODULES

Sensor_Pipe_Settings_ts         g_SensorPipeSettings_Control = {Coin_e_Heads, Coin_e_Heads};
Sensor_Pipe_Settings_ts         g_SensorPipeSettings_Status = {Coin_e_Heads, Coin_e_Heads};

ReadLLAConfig_Control_ts    g_ReadLLAConfig_Control = { 0, 0, Coin_e_Heads };
ReadLLAConfig_Status_ts     g_ReadLLAConfig_Status = { 0, 0, 0, 0, 0, 0, 0, 0, Coin_e_Heads };
SMS_Control_ts              g_SMS_Control = {0, 0, 0, 0, 0, 0x0808, Coin_e_Heads};
SMS_Status_ts               g_SMS_Status = {0, 0, 0, 0, 0, 0, Coin_e_Heads};

volatile Flag_te            g_IsGlaceEnabledInternally = Flag_e_FALSE;
volatile Flag_te            g_IsHistogramEnabledInternally = Flag_e_FALSE;
volatile uint8_t            g_AECTargetFrameCounter = 0;
volatile Flag_te            g_IsGrabOKNotification = Flag_e_FALSE;
volatile Flag_te            g_isAecStatsCancelled = Flag_e_TRUE;
volatile Flag_te            g_AecStatsNotifyPending = Flag_e_FALSE;
volatile uint8_t            g_RxFrameCounter = 0;


/**
      \if           INCLUDE_IN_HTML_ONLY
      \fn           void HostInterface_Run (void)
      \brief        This is the top level function that should be invoked to allow
                    the device to process host commands.
      \return       void
      \callgraph
      \callergraph
      \ingroup      HostInterface
      \endif
*/
void
HostInterface_Run(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint8_t e_Flag_CycleStateMachine;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    e_Flag_CycleStateMachine = Flag_e_FALSE;

    if (Flag_e_FALSE == g_HostInterface_Control.e_Flag_StepThroughStates)
    {
        // Host does not want to step through states
        // Run normal...
        e_Flag_CycleStateMachine = Flag_e_TRUE;
    }
    else
    {
        // Stepping through states
        // Run the state machine only if the control and status coins are not equal
        if (g_HostInterface_Control.e_Coin_StepThroughControl != g_HostInterface_Status.e_Coin_StepThroughStatus)
        {
            e_Flag_CycleStateMachine = Flag_e_TRUE;
        }
    }


    if (Flag_e_TRUE == e_Flag_CycleStateMachine)
    {
        StateMachine();

        g_HostInterface_Status.e_Coin_StepThroughStatus = g_HostInterface_Control.e_Coin_StepThroughControl;
    }


    // invoke the state HostToMasterI2CAccess to allow the host to access the master i2c channel
    HostToMasterI2CAccess_Run();

    ReadLLAConfig();
    ReadApertureConfig();

    // Fetch SMS (sensor mode selection) settings, if requested
    if (g_SMS_Status.e_Coin_Status != g_SMS_Control.e_Coin_Ctrl)
    {
        lla_abstraction_StartVFInTrial();
    }

    return;
}


/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn           void StateMachine( void )
  \brief        This function implements the command state machine of the device.
  \return       void
  \callgraph
  \callergraph
  \ingroup      HostInterface
  \endif
*/
void
StateMachine(void)
{
    g_HostInterface_Status.e_HostInterfaceLowLevelState_Current = g_HostInterface_Status.e_HostInterfaceLowLevelState_Next;
    g_HostInterface_Status.e_HostInterfaceHighLevelState = g_HostInterface_Status.e_HostInterfaceLowLevelState_Current & 0xF0;

    switch (g_HostInterface_Status.e_HostInterfaceLowLevelState_Current)
    {
        case HostInterfaceLowLevelState_e_RAW_UNINITIALISED:
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = RawUninitialised();
            break;

        case HostInterfaceLowLevelState_e_BOOTING:
    #ifdef PROFILER_USING_XTI
         OstTraceInt0(TRACE_USER8, "start_xti_profiler_boot");
    #endif

    #ifdef DEBUG_PROFILER
        CpuCycleProfiler_ts *ptr_CPUProfilerData = NULL;
        ptr_CPUProfilerData = &g_Profile_BOOT;
        START_PROFILER();
    #endif
            BootManager_Boot();
    #ifdef DEBUG_PROFILER
        Profiler_Update(ptr_CPUProfilerData);
    #endif

    #ifdef PROFILER_USING_XTI
        OstTraceInt1(TRACE_USER8, "stop_xti_profiler_boot : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
    #endif

            break;

        case HostInterfaceLowLevelState_e_STOPPED:
            OstTraceInt0(TRACE_DEBUG, "HostInterfaceLowLevelState_e_STOPPED");
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = Stopped();
            if (Flag_e_FALSE == g_Sensor_Tuning_Control.e_Flag_ReadConfigBeforeBoot)
            {
                sensor_tuning_Read_sensor_tuningConfig();
            }

            NVM_Export();

            break;

        case HostInterfaceLowLevelState_e_RXSTOPPED:

            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_RXSTOPPED");
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = RxStopped();

            break;

        case HostInterfaceLowLevelState_e_RESET_ISP:
            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_RESET_ISP");
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = ResetISP();
            EventManager_ResestISPComplete_Notify();

            break;

        case HostInterfaceLowLevelState_e_DATA_PATH_SETUP:
    #ifdef PROFILER_USING_XTI
            OstTraceInt0(TRACE_USER8, "start_xti_profiler_streaming");
    #endif
            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_DATA_PATH_SETUP");
    #ifdef DEBUG_PROFILER
            START_PROFILER();
    #endif
            SystemConfig_StaticDataPathCommit();
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_ALGORITHM_UPDATE;
            break;

        case HostInterfaceLowLevelState_e_ALGORITHM_UPDATE:
            // perform all algorithm computations
            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_ALGORITHM_UPDATE");
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = AlgorithmUpdate();

            break;

        case HostInterfaceLowLevelState_e_SENSOR_COMMIT:
            // commit sensor params
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_ISP_SETUP;
            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_SENSOR_COMMIT");

            break;

        case HostInterfaceLowLevelState_e_ISP_SETUP:
    #ifdef DEBUG_PROFILER
            Profiler_Update(&g_Profile_SensorCommit);
    #endif
            ISPSetup();
            IPPSetup();
    #if 0
            // left for debugging purpose
            memset(&g_Profile_LCO, 0x00, sizeof(CpuCycleProfiler_ts));
            memset(&g_Profile_ISPUpdate, 0x00, sizeof(CpuCycleProfiler_ts));
            memset(&g_Profile_AEC_Statistics, 0x00, sizeof(CpuCycleProfiler_ts));
            memset(&g_Profile_AWB_Statistics, 0x00, sizeof(CpuCycleProfiler_ts));
            memset(&g_Profile_VID0, 0x00, sizeof(CpuCycleProfiler_ts));
            memset(&g_Profile_VID1, 0x00, sizeof(CpuCycleProfiler_ts));
    #endif
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_COMMANDING_RUN;
            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_ISP_SETUP");

            break;

        case HostInterfaceLowLevelState_e_COMMANDING_RUN:
            CommandingRun();
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_WAITING_FOR_RUN;
            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_COMMANDING_RUN");

            break;

        case HostInterfaceLowLevelState_e_WAITING_FOR_RUN:
            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_WAITING_FOR_RUN");
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = WaitingForRun();
            break;

        case HostInterfaceLowLevelState_e_ENTERING_ERROR:
            // issue a notification for error state
            EventManager_StreamingError_Notify();
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_ERROR;
            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_ENTERING_ERROR");

            break;

        case HostInterfaceLowLevelState_e_ERROR:
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = HandleError();
//            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_ERROR");

            break;

        case HostInterfaceLowLevelState_e_RUNNING:
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = Run();

            break;

        case HostInterfaceLowLevelState_e_COMMANDING_STOP:
            CommandingStop();
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_WAITING_FOR_STOP;
            OstTraceInt0(TRACE_DEBUG, "HostInterfaceLowLevelState_e_COMMANDING_STOP");

            break;

        case HostInterfaceLowLevelState_e_WAITING_FOR_STOP:
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = WaitingForStop();

            break;

        case HostInterfaceLowLevelState_e_GOING_TO_SLEEP:
            OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_GOING_TO_SLEEP");
            SleepManager_Sleep();
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_SLEPT;

            break;

        case HostInterfaceLowLevelState_e_SLEPT:
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = Sleeping();

            break;

        case HostInterfaceLowLevelState_e_WAKING_UP:
            // OstTraceInt0(TRACE_FLOW, "HostInterfaceLowLevelState_e_WAKING_UP");
            SleepManager_Wakeup();
            g_HostInterface_Status.e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_STOPPED;

            break;
    }

#ifdef PEDEBUG_ENABLE
	//Process debug PE request
    PEDEBUG_CHECKREQ();
#endif

    return;
}


uint8_t
RawUninitialised(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // only one possible command
    // BOOT
    uint8_t e_HostInterfaceLowLevelState_Next;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_RAW_UNINITIALISED;

    if (HostInterfaceCommand_e_BOOT == g_HostInterface_Control.e_HostInterfaceCommand_User)
    {
        OstTraceInt0(TRACE_DEBUG, "HostInterfaceLowLevelState_e_RAW_UNINITIALISED-> HostInterfaceCommand_e_BOOT");
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_BOOTING;
    }


    return (e_HostInterfaceLowLevelState_Next);
}


uint8_t
Stopped(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // two possible commands to process
    // SLEEP and RUN
    uint8_t e_HostInterfaceLowLevelState_Next;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_STOPPED;

    if (HostInterfaceCommand_e_SLEEP == g_HostInterface_Control.e_HostInterfaceCommand_User)
    {
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_GOING_TO_SLEEP;
        OstTraceInt0(TRACE_DEBUG, "HostInterfaceLowLevelState_e_STOPPED-> HostInterfaceCommand_e_SLEEP");
    }
    else if (HostInterfaceCommand_e_RUN == g_HostInterface_Control.e_HostInterfaceCommand_User)
    {
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_DATA_PATH_SETUP;
        OstTraceInt0(TRACE_DEBUG, "HostInterfaceLowLevelState_e_STOPPED-> HostInterfaceCommand_e_RUN");
    }
    else if (HostInterfaceCommand_e_SWITCH_SENSOR == g_HostInterface_Control.e_HostInterfaceCommand_User)
    {
        lla_abstractionSensorOff();
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_RAW_UNINITIALISED;
        OstTraceInt0(TRACE_DEBUG, "HostInterfaceLowLevelState_e_STOPPED-> HostInterfaceCommand_e_SWITCH_SENSOR");
    }
    else if (HostInterfaceCommand_e_RESET_ISP == g_HostInterface_Control.e_HostInterfaceCommand_User)
    {
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_RESET_ISP;
        OstTraceInt0(TRACE_DEBUG, "HostInterfaceLowLevelState_e_STOPPED-> HostInterfaceCommand_e_RESET_ISP");
    }


    return (e_HostInterfaceLowLevelState_Next);
}

uint8_t
RxStopped()
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    // two possible commands to process
    // STOP and RUN
    uint8_t e_HostInterfaceLowLevelState_Next;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_RXSTOPPED;

    if (HostInterfaceCommand_e_STOP == g_HostInterface_Control.e_HostInterfaceCommand_User)
    {
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_COMMANDING_STOP;
        OstTraceInt0(TRACE_DEBUG, "HostInterfaceLowLevelState_e_STOPPED-> HostInterfaceLowLevelState_e_COMMANDING_STOP");
    }
    else if (HostInterfaceCommand_e_RUN == g_HostInterface_Control.e_HostInterfaceCommand_User)
    {
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_DATA_PATH_SETUP;
        OstTraceInt0(TRACE_DEBUG, "HostInterfaceLowLevelState_e_STOPPED-> HostInterfaceCommand_e_RUN");
    }

    return (e_HostInterfaceLowLevelState_Next);
}


uint8_t
AlgorithmUpdate(void)
{
    uint8_t e_HostInterfaceLowLevelState_Next;

    OstTraceInt0(TRACE_DEBUG, "> AlgorithmUpdate");

	g_VariableFrameRateStatus.e_Flag = g_VariableFrameRateControl.e_Flag;

    if(Flag_e_TRUE == g_VariableFrameRateControl.e_Flag)
    {
        g_VariableFrameRateStatus.f_MaximumFrameRate_Hz = g_VariableFrameRateControl.f_MaximumFrameRate_Hz;
        g_VariableFrameRateStatus.f_CurrentFrameRate_Hz = g_VariableFrameRateControl.f_CurrentFrameRate_Hz;
    }

    // In this state, the following need to be worked out:
    // - Zoom settings
    // - Frame dimension settings based on zoom
    // - Videotiming settings based on frame dimension settings
    // - Exposure settings based on video timing and frame dimension settings (pixel clock period and vt line length)
    // - Flash settings based on exposure (in Auto mode)
    // - Focus point
    // Perform setup with respect to zoom, frame dimension, video timing
    // if frame update fails then no need to perform the rest of the update
    SystemConfig_SetSensorSettingStatus(UpdateStatus_e_Idle);

    if (Flag_e_TRUE == PipeAlgortihm_FrameUpdate())
    {
        if ( (Stream_IsSensorStreaming()) && (Zoom_IsFDMRequestPending()) )
        {
                // If sensor is streaming, reprogram needs to be done

                g_Zoom_Status.e_ZoomRequestLLDStatus = ZoomRequestLLDStatus_e_Accepted;

                e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_COMMANDING_STOP;

                OstTraceInt0(TRACE_DEBUG, "Restarting sensor since reprogram is required :: HostInterfaceLowLevelState_e_COMMANDING_STOP");
        }
        else
        {
            OstTraceInt0(TRACE_DEBUG, "PipeAlgortihm_FrameUpdate()");
            if (SystemConfig_IsSensorSettingStatusToBeUpdated())
            {
                SystemConfig_SetSensorSettingStatus(UpdateStatus_e_Updating);
            }


        // Before the running state, we are checking the value of System coins. If they are toggled
        // then we have to copy the values of e_GrabMode_Ctrl to e_GrabMode_Status and
        // e_Flag_Request_ValidFrameNotification_Ctrl to e_Flag_Request_ValidFrameNotification_Status
        g_SystemConfig_Status.e_GrabMode_Status = g_SystemSetup.e_GrabMode_Ctrl;
        g_SystemConfig_Status.e_Flag_Request_ValidFrameNotification_Status = g_SystemSetup.e_Flag_Request_ValidFrameNotification_Ctrl;

            /*
            OstTraceInt2(TRACE_DEBUG, "g_Zoom_Status_LLA.u16_woi_resX = %d,   g_Zoom_Status_LLA.u16_woi_resY = %d", g_Zoom_Status_LLA.u16_woi_resX,  g_Zoom_Status_LLA.u16_woi_resY);
            OstTraceInt2(TRACE_DEBUG, "g_Zoom_Status_LLA.u16_woi_resX = %d,   g_Zoom_Status_LLA.u16_output_resY = %d", g_Zoom_Status_LLA.u16_output_resX, g_Zoom_Status_LLA.u16_output_resY);
            OstTraceInt1(TRACE_DEBUG, "default g_GridironControl.u16_GridWidth = %d", g_GridironControl.u16_GridWidth);
            OstTraceInt2(TRACE_DEBUG, "g_CurrentFrameDimension.u16_VTXAddrStart = %d,   g_CurrentFrameDimension.u16_VTYAddrStart = %d", g_CurrentFrameDimension.u16_VTXAddrStart, g_CurrentFrameDimension.u16_VTYAddrStart);
            OstTraceInt2(TRACE_DEBUG, "g_CurrentFrameDimension.u16_OPXOutputSize = %d,   g_RequestedFrameDimension.u16_OPYOutputSize = %d", g_CurrentFrameDimension.u16_OPXOutputSize, g_RequestedFrameDimension.u16_OPYOutputSize);
            */

            // Apply grid iron static configuration parameters
            Gridiron_ApplyGridStaticParameters();

            // Update and Commit Dampers
            CommitDG();             // Commit DG to pipe

            ApplyISPParams();

            SystemConfig_SetExposureInSync();

            e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_ISP_SETUP;

        }
    }
    else
    {
        // frame update request failed
        // transition to FAILED state
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_ENTERING_ERROR;
    }

    OstTraceInt0(TRACE_DEBUG, "< AlgorithmUpdate");
    return (e_HostInterfaceLowLevelState_Next);
}


uint8_t
HandleError(void)
{
    uint8_t e_HostInterfaceLowLevelState_Next;

    // if the host issues a stop command, transition to stopped state
    // else remain in the ERROR state
    if (HostInterfaceCommand_e_STOP == g_HostInterface_Control.e_HostInterfaceCommand_User)
    {
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_COMMANDING_STOP;
    }
    else
    {
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_ERROR;
    }


    return (e_HostInterfaceLowLevelState_Next);
}

void
IPPSetup(void)
{
    uint8_t     u8_SensorOutputPixelWidth;
    uint8_t     u8_Value;
    uint8_t     u8_DS;
    uint8_t     u8_BPP;
    uint8_t     u8_DataTypeCode;
    uint8_t     u8_NumberOfCSI2DataLines;
    uint8_t     u8_CSI2Version;
    uint16_t    u16_pix_width;
    uint8_t     u8_SensorInputPixelWidth;

    // if the input image source is a sensor then the CRM_STATIC_SD_CCP_PIXEL_CLOCK_DIV must be setup
    u8_SensorOutputPixelWidth = VideoTiming_GetSensorOutputPixelWidth();
    u8_BPP = u8_SensorOutputPixelWidth;
    u8_SensorInputPixelWidth = VideoTiming_GetSensorInputPixelWidth();
    u8_NumberOfCSI2DataLines = SystemConfig_GetNumberOfCSI2DataLines();

    if (u8_SensorInputPixelWidth != u8_SensorOutputPixelWidth)
    {
        /// DPCM
        if (0x08 == u8_SensorOutputPixelWidth)
        {
            u8_DataTypeCode = g_camera_details.p_sensor_details->csi_signalling_options.csi2_data_type_ids.dpcm_10_to_8_dt;
        }
        else if (0x06 == u8_SensorOutputPixelWidth)
        {
            u8_DataTypeCode = g_camera_details.p_sensor_details->csi_signalling_options.csi2_data_type_ids.dpcm_10_to_6_dt;
        }
    }
    else
    {
        /// RAW
        if (0x08 == u8_BPP)
        {
            u8_DataTypeCode = g_camera_details.p_sensor_details->csi_signalling_options.csi2_data_type_ids.raw_8_dt;
        }
        else if (0x0A == u8_BPP)
        {
            u8_DataTypeCode = g_camera_details.p_sensor_details->csi_signalling_options.csi2_data_type_ids.raw_10_dt;
        }
    }


    if (VideoTiming_IsDataClockMode())
    {
        u8_SensorOutputPixelWidth = u8_SensorOutputPixelWidth << 1;

        u8_DS = 1;
    }
    else
    {
        u8_DS = 0;
    }


    Set_CRM_CRM_STATIC_SD_CCP_PIXEL_CLOCK_DIV(u8_SensorOutputPixelWidth);

    if (SystemConfig_IPPSetupRequired() && !Stream_IsSensorStreaming())
    {
        if (SystemConfig_IsInputImageSourceSensor())
        {
            // disable the IPP_DPHY_TOP_IF
            Set_IPP_IPP_DPHY_TOP_IF_EN(0);

            // input image source is a sensor
            if (SystemConfig_IsInputInterfaceCSI2_0())
            {
                if (SystemConfig_IsSensorCSI2_0_0_90())
                {
                    u8_CSI2Version = 1;
                }
                else
                {
                    u8_CSI2Version = 0;
                }


                // input image source is CSI2_0
                Set_IPP_IPP_STATIC_TOP_IF_SEL(STATIC_TOP_IF_SEL_B_0x0);

                // setup IPP_CSI2_DPHY0_CL_CTRL
                u8_Value = (uint8_t)((4000.0 * u8_NumberOfCSI2DataLines) /(g_config_feedback.op_pix_clk_freq * u8_BPP));
                if ( Is_8500v1() || Is_8500v2())
                {
                     HW_CSI2_0_Setup_8500(u8_Value, u8_NumberOfCSI2DataLines,u8_CSI2Version);

                }
                else if (Is_9540v1() || Is_8540v1())
                {
                     // Both 8540 and 9540 have IPP_CSI2_DPHY0_DL4_CTRL data lanes so calling the same function for 8540 and 9540 
                     HW_CSI2_0_Setup_9540(u8_Value, u8_NumberOfCSI2DataLines,u8_CSI2Version);

                }
            }
            else if (SystemConfig_IsInputInterfaceCSI2_1())
            {
                if (SystemConfig_IsSensorCSI2_1_0_90())
                {
                    u8_CSI2Version = 1;
                }
                else
                {
                    u8_CSI2Version = 0;
                }


                // input image source is CSI2_1
                Set_IPP_IPP_STATIC_TOP_IF_SEL(STATIC_TOP_IF_SEL_B_0x1);
                // setup IPP_CSI2_DPHY1_CL_CTRL
               // u8_Value = (uint8_t) (4000.0 / VideoTiming_GetSensorDataRate());
                u8_Value = (uint8_t)((4000.0 * u8_NumberOfCSI2DataLines) /(g_config_feedback.op_pix_clk_freq * u8_BPP));
                Set_IPP_IPP_CSI2_DPHY1_CL_CTRL(
                u8_Value,
                u8_CSI2Version,
                SystemConfig_GetSwapPinClCSI2_1(),
                SystemConfig_GetInvertClCSI2_1(),
                0,
                0,
                0);                             // CSI0_UI_X4, CSI0_SPECS_90_81B, CSI0_SWAP_PINS_CL, CSI0_HS_INVERT_CL, CSI0_HSRX_TERM_SHIFT_UP_CL, CSI0_HSRX_TERM_SHIFT_DOWN_CL, CSI0_TEST_RESERVED_1_CL

                // setup IPP_CSI2_DPHY1_DL1_CTRL
                Set_IPP_IPP_CSI2_DPHY1_DL1_CTRL(
                SystemConfig_GetSwapPinDl1CSI2_1(),
                SystemConfig_GetInvertDl1CSI2_1(),
                0,
                1,
                0,
                0,
                0,
                0);                             // CSI0_SWAP_PINS_DL1, CSI0_HS_INVERT_DL1, CSI0_FORCE_RX_MODE_DL1, CSI0_CD_OFF_DL1, CSI0_EOT_BYPASS_DL1, CSI0_HSRX_TERM_SHIFT_UP_DL1, CSI0_HSRX_TERM_SHIFT_DOWN_DL1, CSI0_TEST_RESERVED_1_DL1

                // setup IPP_STATIC_CSI2_DATA_LANES
                u8_NumberOfCSI2DataLines = 1;   // for CSI2_1 interface, number of data lines hard coded to 1
                Set_IPP_IPP_STATIC_CSI0_DATA_LANES(u8_NumberOfCSI2DataLines, SystemConfig_GetDataLanesMapCSI2_1()); // STATIC_CSI0_DATA_LANES_NB, STATIC_CSI0_DATA_LANES_MAP
            }
            else
            {
                // input image source is CCP
                Set_IPP_IPP_STATIC_TOP_IF_SEL(0x02);
            }


            // setup IPP_STATIC_CCP_IF
            Set_IPP_IPP_STATIC_CCP_IF(u8_DS, u8_BPP);   // STATIC_CCP_DSCLK, STATIC_CCP_BPP
            if (Is_8500v2() || Is_9540v1() || Is_8540v1())
            {
                // the hw is of version v2
                // This is a workaround suggested by Thomas Burg related to a hardware bug in 8500 V2 hardware.
                // Do not change the order of the data types being accpeted i.e. data type zero should always be RAW8(0x8)
                u16_pix_width = 0x8 | (u8_BPP << 4) | (u8_BPP << 8);

                // setup IPP_STATIC_CSI2RX_PIX_WIDTH_W
                Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_new_word(u16_pix_width);

                // This is a workaround suggested by Thomas Burg related to a hardware bug in 8500 V2 hardware.
                // Do not change the order of the data types being accpeted i.e. data type zero should always be RAW8(0x37)
                Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_1_W(0x37, u8_DataTypeCode);
                Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_2_W(g_camera_details.p_sensor_details->csi_signalling_options.csi2_data_type_ids.ancillary_data_dt);
            }
            else if (Is_8500v1())
            {
                // the hw is of version v1
                u16_pix_width = u8_BPP | u8_BPP << 4;

                // setup IPP_STATIC_CSI2RX_PIX_WIDTH_W
                Set_IPP_IPP_STATIC_CSI2RX_PIX_WIDTH_W_word(u16_pix_width);

                // setup IPP_STATIC_CSI2RX_DATA_TYPE_W
                Set_IPP_IPP_STATIC_CSI2RX_DATA_TYPE_W(u8_DataTypeCode, g_camera_details.p_sensor_details->csi_signalling_options.csi2_data_type_ids.ancillary_data_dt);

            }


            // IPP_DPHY_TOP_IF will be enabled after the started streaming timeout expires
        }
    }
}

//Added new function for CSI2 setup if 8500 Platform is selected
void
HW_CSI2_0_Setup_8500(uint8_t u8_Value, uint8_t u8_NumberOfCSI2DataLines,uint8_t u8_CSI2Version)
{

     Set_IPP_IPP_CSI2_DPHY0_CL_CTRL(
     u8_Value,
     u8_CSI2Version,
     SystemConfig_GetSwapPinClCSI2_0(),
     SystemConfig_GetInvertClCSI2_0(),
     0,
     0,
     0);             // CSI0_UI_X4, CSI0_SPECS_90_81B, CSI0_SWAP_PINS_CL, CSI0_HS_INVERT_CL, CSI0_HSRX_TERM_SHIFT_UP_CL, CSI0_HSRX_TERM_SHIFT_DOWN_CL, CSI0_TEST_RESERVED_1_CL

     // setup IPP_CSI2_DPHY0_DL1_CTRL
     Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL(
     SystemConfig_GetSwapPinDl1CSI2_0(),
     SystemConfig_GetInvertDl1CSI2_0(),
     0,
     1,
     0,
     0,
     0,
     0);             // CSI0_SWAP_PINS_DL1, CSI0_HS_INVERT_DL1, CSI0_FORCE_RX_MODE_DL1, CSI0_CD_OFF_DL1, CSI0_EOT_BYPASS_DL1, CSI0_HSRX_TERM_SHIFT_UP_DL1, CSI0_HSRX_TERM_SHIFT_DOWN_DL1, CSI0_TEST_RESERVED_1_DL1

     // setup IPP_CSI2_DPHY0_DL2_CTRL
     Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL(
     SystemConfig_GetSwapPinDl2CSI2_0(),
     SystemConfig_GetInvertDl2CSI2_0(),
     0,
     1,
     0,
     0,
     0,
     0);             // CSI0_SWAP_PINS_DL2, CSI0_HS_INVERT_DL2, CSI0_FORCE_RX_MODE_DL2, CSI0_CD_OFF_DL2, CSI0_EOT_BYPASS_DL2, CSI0_HSRX_TERM_SHIFT_UP_DL2, CSI0_HSRX_TERM_SHIFT_DOWN_DL2, CSI0_TEST_RESERVED_1_DL2

     // setup IPP_CSI2_DPHY0_DL3_CTRL
     Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL(
     SystemConfig_GetSwapPinDl3CSI2_0(),
     SystemConfig_GetInvertDl3CSI2_0(),
     0,
     1,
     0,
     0,
     0,
     0);             // CSI0_SWAP_PINS_DL3, CSI0_HS_INVERT_DL3, CSI0_FORCE_RX_MODE_DL3, CSI0_CD_OFF_DL3, CSI0_EOT_BYPASS_DL3, CSI0_HSRX_TERM_SHIFT_UP_DL3, CSI0_HSRX_TERM_SHIFT_DOWN_DL3, CSI0_TEST_RESERVED_1_DL3

     // setup IPP_STATIC_CSI2_DATA_LANES
     if (u8_NumberOfCSI2DataLines > 3)
     {
         u8_NumberOfCSI2DataLines = 3;
     }


   //And-ing SystemConfig_GetDataLanesMapCSI2_1 with 0x1FF as only last 9 bits (i.e. ,0xD1 of 0x8D1) are required for DataLanesMapCSI2_0
    Set_IPP_IPP_STATIC_CSI0_DATA_LANES(u8_NumberOfCSI2DataLines, (SystemConfig_GetDataLanesMapCSI2_0()& 0x1FF)); // STATIC_CSI0_DATA_LANES_NB, STATIC_CSI0_DATA_LANES_MAP

}


//Added new function for CSI2 setup if 9540 Platform is selected
void
HW_CSI2_0_Setup_9540(uint8_t u8_Value, uint8_t u8_NumberOfCSI2DataLines,uint8_t u8_CSI2Version)
{

        Set_IPP_IPP_CSI2_DPHY0_CL_CTRL(
        u8_Value,
        u8_CSI2Version,
        SystemConfig_GetSwapPinClCSI2_0(),
        SystemConfig_GetInvertClCSI2_0(),
        0,
        0,
        0);             // CSI0_UI_X4, CSI0_SPECS_90_81B, CSI0_SWAP_PINS_CL, CSI0_HS_INVERT_CL, CSI0_HSRX_TERM_SHIFT_UP_CL, CSI0_HSRX_TERM_SHIFT_DOWN_CL, CSI0_TEST_RESERVED_1_CL

        // setup IPP_CSI2_DPHY0_DL1_CTRL
        Set_IPP_IPP_CSI2_DPHY0_DL1_CTRL(
        SystemConfig_GetSwapPinDl1CSI2_0(),
        SystemConfig_GetInvertDl1CSI2_0(),
        0,
        1,
        0,
        0,
        0,
        0);             // CSI0_SWAP_PINS_DL1, CSI0_HS_INVERT_DL1, CSI0_FORCE_RX_MODE_DL1, CSI0_CD_OFF_DL1, CSI0_EOT_BYPASS_DL1, CSI0_HSRX_TERM_SHIFT_UP_DL1, CSI0_HSRX_TERM_SHIFT_DOWN_DL1, CSI0_TEST_RESERVED_1_DL1

        // setup IPP_CSI2_DPHY0_DL2_CTRL
        Set_IPP_IPP_CSI2_DPHY0_DL2_CTRL(
        SystemConfig_GetSwapPinDl2CSI2_0(),
        SystemConfig_GetInvertDl2CSI2_0(),
        0,
        1,
        0,
        0,
        0,
        0);             // CSI0_SWAP_PINS_DL2, CSI0_HS_INVERT_DL2, CSI0_FORCE_RX_MODE_DL2, CSI0_CD_OFF_DL2, CSI0_EOT_BYPASS_DL2, CSI0_HSRX_TERM_SHIFT_UP_DL2, CSI0_HSRX_TERM_SHIFT_DOWN_DL2, CSI0_TEST_RESERVED_1_DL2

        // setup IPP_CSI2_DPHY0_DL3_CTRL
        Set_IPP_IPP_CSI2_DPHY0_DL3_CTRL(
        SystemConfig_GetSwapPinDl3CSI2_0(),
        SystemConfig_GetInvertDl3CSI2_0(),
        0,
        1,
        0,
        0,
        0,
        0);             // CSI0_SWAP_PINS_DL3, CSI0_HS_INVERT_DL3, CSI0_FORCE_RX_MODE_DL3, CSI0_CD_OFF_DL3, CSI0_EOT_BYPASS_DL3, CSI0_HSRX_TERM_SHIFT_UP_DL3, CSI0_HSRX_TERM_SHIFT_DOWN_DL3, CSI0_TEST_RESERVED_1_DL3

        // setup IPP_CSI2_DPHY0_DL4_CTRL
        Set_IPP_IPP_CSI2_DPHY0_DL4_CTRL(
        SystemConfig_GetSwapPinDl4CSI2_0(),
        SystemConfig_GetInvertDl4CSI2_0(),
        0,
        1,
        0,
        0,
        0,
        0);             // CSI0_SWAP_PINS_DL3, CSI0_HS_INVERT_DL3, CSI0_FORCE_RX_MODE_DL3, CSI0_CD_OFF_DL3, CSI0_EOT_BYPASS_DL3, CSI0_HSRX_TERM_SHIFT_UP_DL3, CSI0_HSRX_TERM_SHIFT_DOWN_DL3, CSI0_TEST_RESERVED_1_DL3


        // setup IPP_STATIC_CSI2_DATA_LANES
        if (u8_NumberOfCSI2DataLines > 4)
        {
            u8_NumberOfCSI2DataLines = 4;
        }

        Set_IPP_IPP_STATIC_CSI0_DATA_LANES(u8_NumberOfCSI2DataLines, SystemConfig_GetDataLanesMapCSI2_0()); // STATIC_CSI0_DATA_LANES_NB, STATIC_CSI0_DATA_LANES_MAP

}



void
ISPSetup(void)
{
    // The following need to be setup for the current streaming operation:
    // If pipe0 has been enabled, setup colour engine 0
    // If pipe1 has been enabled, setup colour engine 1
    // Demosaic colour engine
    // Reconstruction engine
    // Sensor Data Pipe
    // perform all static setup i.e. setup that needs to be done only once

    if (SystemConfig_IsInputImageSourceUnderISPControl())
    {
        /* Enable SMIA RX Line Count interrupts:
            rx_line_count_0_int_en = 1,
            rx_line_count_1_int_en = 0,
            rx_line_count_2_int_en = 0,
            rx_line_count_3_int_en = 1,
            rx_seq_complete_int_en = 0
        */
        Set_ISP_SMIARX_ISP_SMIARX_COUNT_INT_EN(1, 0, 0, 1, 0);

        /* Setup SMIA Rx Line count 0 to fire at the end of the status lines:
            pix_count = 0,
            line_count = NumISLLines
        */
        Set_ISP_SMIARX_ISP_SMIARX_INT_COUNT_0(0, FrameDimension_GetNumberOfStatusLines());

        /* Setup SMIA Rx Line count 3 to fire at 2 lines before the end of active lines:
            pix_count = 0,
            line_count = NumISLLines + ActiveDataLines - 2
        */
        Set_ISP_SMIARX_ISP_SMIARX_INT_COUNT_3(0,(FrameDimension_GetNumberOfStatusLines() + FrameDimension_GetCurrentOPYOutputSize() - 2));

    }

    /* Trigger the dummy status line interrupt to complete exposure setup. */
    ITM_Trigger_Interrupt_RX0_Status_Line();

    /* Trigger the dummy active line end interrupt for data path update */
    ITM_Trigger_Interrupt_RX3_Commit_ISP();

    if (SystemConfig_IsPipe0Active())
    {
        g_PipeStatus[0].e_Flag_VideoCompleteInterruptPending = Flag_e_TRUE;

        // trigger vid complete0 which will program CE0
        ITM_TriggerVidComplete0Interupt();
    }


    if (SystemConfig_IsPipe1Active())
    {
        g_PipeStatus[1].e_Flag_VideoCompleteInterruptPending = Flag_e_TRUE;

        // trigger vid complete1 which will program CE1
        ITM_TriggerVidComplete1Interupt();
    }


    // if neither of the pipes are active, even then
    // we must trigger one vid complete to enable
    // the programming of the input pipe
    if (!SystemConfig_IsAnyPipeActive())
    {
        ITM_TriggerVidComplete0Interupt();
    }

    // Call this here once irrespective any pipe state
    Set_ITM_ITM_DMCEPIPE_STATUS_BSET_DMCE_OUT_STATUS_BSET(1);

    //Clear internal stats cancel request flag
    SystemConfig_SetStatsCancelStatus(Flag_e_FALSE);

    PipeAlgorithm_CommitGlaceAndHistogram();

    return;
}


void
CommandingRun(void)
{
    // if the input image source is a sensor
    // then start the sensor and Rx else only
    // start the Rx
    // if neither the sensor nor the Rx is used
    // as an input image source, then do nothing
    if (SystemConfig_IsInputImageSourceSensor())
    {
        // US: 5/12 Front End Error Recovery Abort Flag. It is reset before every new Run
        g_ErrorHandler.e_Flag_Error_Abort = Flag_e_FALSE;
        g_ErrorHandler.e_Flag_Error_Recover = Flag_e_FALSE;
        g_ErrorHandler.u8_CSI_Error_Counter = 0;
        g_ErrorHandler.u8_CCP_Error_Counter = 0;

        // g_FLADriver_LensMoveError is reset before every new Run
        g_FLADriver_LensMoveErrorCount = 0;

        // both the sensor and Rx need to be started
        Stream_StartSensorAndRx();
    }
    else if (SystemConfig_IsInputImageSourceRx())
    {
        // only the Rx needs to be started
        Stream_StartRx();
    }
    else
    {
        // the input image source is one of the BML
        // signal that the firmware is ready to accept data
        EventManager_ISPLoadReady_Notify();

       // Notify DMA for BML phase also. This is needed as MMDSP will not be aware of usecase
       // and can not differntiate whether or not it should wait for DMA OK notification.
       g_GrabNotify.u32_DMA_GRAB_Indicator = 1;
       EventManager_DMA_GRAB_Notify_OK();
    }


    return;
}


uint8_t
WaitingForRun(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint8_t e_HostInterfaceLowLevelState_Next;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // if the input image source is under ISP control
    // i.e. the input image source is either sensor0 or sensor1 or Rx
    // then we must wait for streaming to start
    if (SystemConfig_IsInputImageSourceUnderISPControl())
    {
        // query for the ISP input to be streaming
        if (Stream_IsISPInputStreaming())
        {
            // ISP input streaming resource started
            // transition to the RUNNING state
            OstTraceInt0(TRACE_FLOW, "SENSOR->HostInterfaceLowLevelState_e_WAITING_FOR_RUN-> HostInterfaceLowLevelState_e_RUNNING");
#ifdef PROFILER_USING_XTI
        OstTraceInt0(TRACE_USER8, "stop_xti_profiler_streaming");
#endif
            e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_RUNNING;
        }
        else
        {
            // ISP input streaming resource not started yet
            // wait in the current state
            e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_WAITING_FOR_RUN;
        }
    }
    else
    {
        // we are in a mode where the streaming start is not in ISP control
        // like bayer load operation
        // transition to running state straight away
            OstTraceInt0(TRACE_FLOW, "BML->HostInterfaceLowLevelState_e_WAITING_FOR_RUN-> HostInterfaceLowLevelState_e_RUNNING");
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_RUNNING;
    }


    return (e_HostInterfaceLowLevelState_Next);
}


void
LLA_Run(void)
{
    if (Is_SOF_Arrived())
    {
        //Check ISL
        ReadLLASensorSettings();

        //Check whether Pipe params coin toggled
        HostInterface_ISPSettings();
        Set_SOF_Arrived(Flag_e_FALSE);

        // Request exposure value if HDR coin is in toggled state
        HDR_RequestExposure();

        /* Exposure, awb, framerate and flash processing is done immediately for SMIA case else will be handled in sync with SOF */
        if(SENSOR_VERSION_NON_SMIA == g_SensorInformation.u8_smia_version)
        {
            // Check for exposure, awb, framerate and flash
            SetLLAConfig();
        }
    }

    //Check if any cam drv message is pending
    if (LLA_IS_CAM_DRV_MSG_PENDING())
    {
        //handle pending camera driver message
        LLA_Abstraction_CamDrvHandleMsg();
    }


    // Notify statistics IFF both glace and histogram statistics have been received
    NotifyGlaceHistogramStatistics();

    /* Exposure, awb, framerate and flash processing is done immediately for SMIA case else will be handled in sync with SOF */
    if(SENSOR_VERSION_NON_SMIA != g_SensorInformation.u8_smia_version)
    {
        // Check for exposure, awb, framerate and flash
        SetLLAConfig();
    }

    //Check for AEC coin toggled
    HostInterface_SensorSettings();

    // This is done to support mode changes of VF/capture without stop/start and preflash
    // for flash, need to postpone the flash related call when config set to be called for exposure in idle loop;
    if
    (
        (g_RunMode_Control.e_Coin_Ctrl != g_RunMode_Status.e_Coin_Status)
    ||  (g_SystemSetup.e_Flag_mirror != g_SystemConfig_Status.e_Flag_mirror)
    ||  (g_SystemSetup.e_Flag_flip != g_SystemConfig_Status.e_Flag_flip)
    )
    {
        // Call VF/Capture api
        g_SystemConfig_Status.e_Flag_RunBestSensormode = Flag_e_TRUE;
        lla_abstraction_StartSensor();

        //[NON_ISL_SUPPORT]
        if(SENSOR_VERSION_NON_SMIA == g_SensorInformation.u8_smia_version)
        {
            OverridePixelOrder();
        }

        g_RunMode_Status.e_Coin_Status = g_RunMode_Control.e_Coin_Ctrl;
    }
}


//[NON_ISL_SUPPORT]
//The function will be called only for NON ISL sensor case to override the value of pixel order for mirror flip etc
//because ISL is not present and value of pixel order from sensor wont be available.
void OverridePixelOrder(void)
{
    // Write the value of Pixel Order in hardware and Status PE depending upon the requested configuration of mirror and flip
    // Default pixel order can be differnt for different sensors. Using the sensor specific values returned from LLCD
    if (g_SystemConfig_Status.e_Flag_mirror)
    {
        if(g_SystemConfig_Status.e_Flag_flip)
        {
            g_SystemConfig_Status.e_PixelOrder = g_camera_details.p_sensor_details->pixel_orders.mirrored_and_flipped;
            Set_ISP_SMIARX_ISP_SMIARX_PIXEL_ORDER(g_camera_details.p_sensor_details->pixel_orders.mirrored_and_flipped);
        }
        else
        {
            g_SystemConfig_Status.e_PixelOrder = g_camera_details.p_sensor_details->pixel_orders.mirrored;
            Set_ISP_SMIARX_ISP_SMIARX_PIXEL_ORDER(g_camera_details.p_sensor_details->pixel_orders.mirrored);
        }
    }
    else
    {
        if(g_SystemConfig_Status.e_Flag_flip)
        {
            g_SystemConfig_Status.e_PixelOrder = g_camera_details.p_sensor_details->pixel_orders.flipped;
            Set_ISP_SMIARX_ISP_SMIARX_PIXEL_ORDER(g_camera_details.p_sensor_details->pixel_orders.flipped);
        }
        else
        {
            g_SystemConfig_Status.e_PixelOrder = g_camera_details.p_sensor_details->pixel_orders.normal;
            Set_ISP_SMIARX_ISP_SMIARX_PIXEL_ORDER(g_camera_details.p_sensor_details->pixel_orders.normal);
        }
    }
}


uint8_t
Run(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // Two possible commands to process
    // e_STOP
    // e_STOP_ISP
    // Three possible RUN sub commands to handle
    uint8_t e_HostInterfaceLowLevelState_Next;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_RUNNING;

    switch (g_HostInterface_Control.e_HostInterfaceCommand_User)
    {
        case HostInterfaceCommand_e_STOP:
        case HostInterfaceCommand_e_STOP_ISP:
        case HostInterfaceCommand_e_AUTO_STOP:
           /*  [CN]We need to ensure that the lens is stopped before the stop command is processed. In the current
            *  design (performance optimisation: parallel grab, sw3a and isp stop), it is possible that the host
            *  issues a stop command immediatedly after it has issued a lens movement command and the lens is
            *  still moving (i.e. it does not wait for lens_stopped notification)
            */
            //[PM][TODO]Ideal way to handle this is to use cam_drv_lens_stop().
            //If Host wants to complete stats before STOP (and notify later), hold STOP request.
            if(!(COMPLETE_AEC_STATS_AND_NOTIFY_AFTER_STOP() || Complete_AFStats_And_Notify_After_Stop()))
            {
                LLA_Abstraction_Lens_Stop();
                e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_COMMANDING_STOP;
                break;
            }
           /*  [CN]NOTE: Intentionally break is missing; need to fall-through to the below case so that the lens_stopped
            *  notification is processed before the STOP command is processed
            */
        case HostInterfaceCommand_e_RUN:
            // TODO: Handle a sensor change over
            // TODO: Handle finite streaming case
            // Handle zoom commands if any
            if (SystemConfig_IsInputImageSourceSensor())
            {
                LLA_Run();

                // <HD> From BG
                                                /// If there is a change in pipe enable control, update status accordingly
                if (g_DataPathStatus.e_Coin_PipeEnable != g_DataPathControl.e_Coin_PipeEnable)
                {
                    SystemConfig_RunTimePipeEnable();
                    g_SystemConfig_Status.e_Flag_RunBestSensormode = Flag_e_TRUE;
                    g_DataPathStatus.e_Coin_PipeEnable = g_DataPathControl.e_Coin_PipeEnable;
                }


                if (SystemConfig_IsAnyPipeActive())
                {
                    ZoomTop_Run();
                }


    #if INCLUDE_FOCUS_MODULES
                // we allow direct actuator ontrol only if
                //  - the actuator is present and
                //  - the system has booted
                if (FocusControl_IsLensActuatorPresent())
                {
                    FocusControl_TopLevelInterfaceOnIdle();
                }
      #ifdef AF_DEBUG
                if ((g_request_time_marked == Flag_e_FALSE) &&
                 (g_AFStats_Controls.e_Coin_AFStatsExportCmd != g_AFStats_Status.e_Coin_AFStatsExportStatus))
                {
                      OstTraceInt1(TRACE_USER1,"[AF Optimization]Received AF Stats Req in Frame = %u", Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter());
                      g_request_time_marked = Flag_e_TRUE;
                }
      #endif //AF_DEBUG
    #endif //INCLUDE_FOCUS_MODULES
                //[CN/PM]Hold sensor changeover if lens is moving.
                if ( (Flag_e_FALSE == FLADriver_GetLensISMoving()) && (Zoom_IsFDMRequestPending()) )
                {
                    // a sensor changeover (reprogram) needs to be done
                    g_Zoom_Status.e_ZoomRequestLLDStatus = ZoomRequestLLDStatus_e_Accepted;
                    e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_COMMANDING_STOP;
                }
                else
                {
                    // no sensor re-program, handle other events
                    switch (g_HostInterface_Control.e_HostInterfaceRunSubCommand)
                    {
                        case HostInterfaceRunSubCommand_e_HALF_BUTTON_PRESS:                        // TODO: handle HALF BUTTON PRESS EVENT break;
                        case HostInterfaceRunSubCommand_e_HALF_BUTTON_RELEASE:                      // TODO: handle HALF BUTTON RELEASE EVENT break;
                        case HostInterfaceRunSubCommand_e_FULL_BUTTON_PRESS:                        // TODO: handle FULL BUTTON PRESS EVENT break;
                        case HostInterfaceRunSubCommand_e_FULL_BUTTON_RELEASE:                      // TODO: handle FULL BUTTON RELEASE EVENT break;
                            break;
                    }
                }
            }


            // US: 5/12 Front End Error Recovery
            if (g_ErrorHandler.e_Flag_Error_Abort)
            {
                e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_COMMANDING_STOP;
            }


            break;
    }


    return (e_HostInterfaceLowLevelState_Next);
}


void
CommandingStop(void)
{
    // if the input image source is under ISP control
    // then stop the resource as per the stop command
    // else do nothing
    if (SystemConfig_IsInputImageSourceUnderISPControl())
    {
        if (HostInterfaceCommand_e_STOP_ISP == g_HostInterface_Control.e_HostInterfaceCommand_User)
        {
            // handle explicit STOP ISP command
            Stream_StopRx();
        }
        else
        {
            // US: 5/12 Front End Error Recovery. If Recovery flag is set, FW will try to
            // recover by resetting the Rx path
            if (Flag_e_TRUE == g_ErrorHandler.e_Flag_Error_Recover)
            {
                Stream_StopRx();
            }
            else
            {
                // either there is a STOP command or a sensor change over request
                // both the sensor and Rx need to stop
                Stream_StopSensorAndRx();
            }
        }
    }


    return;
}


uint8_t
WaitingForStop(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint8_t e_HostInterfaceLowLevelState_Next;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // if the input image source is under ISP control, then check for the
    // resource to stop as per the stop command, else do nothing
    // check to see if the low level state machine has stopped
    if (SystemConfig_IsInputImageSourceUnderISPControl())
    {
        // wait for the ISP input to stop
        if (Stream_IsISPInputStreaming())
        {
            // ISP input not stopped yet, wait in the current state
            e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_WAITING_FOR_STOP;
        }
        else
        {
            // the ISP has stopped streaming
            if (SystemConfig_IsRGBStoreActive())
            {
                // ideally we would have wanted to wait for RGB Store operation
                // to complete, but we have no notification for this, do nothing
                // at the moment
            }


            if (SystemConfig_IsBayerStore2Active())
            {
                // ideally we would have wanted to wait for Bayer Store2 operation
                // to complete, but we have no notification for this, do nothing
                // at the moment
            }


            if (SystemConfig_IsBayerStore1Active())
            {
                // ideally we would have wanted to wait for Bayer Store1 operation
                // to complete, but we have no notification for this, do nothing
                // at the moment
            }


            if (SystemConfig_IsBayerStore0Active())
            {
                // ideally we would have wanted to wait for Bayer Store1 operation
                // to complete, but we have no notification for this, do nothing
                // at the moment
            }


            //If RX and Sensor has stopped, we can stop all the IPs which are accessing memories.
            //Clocks are enabled so far, so after receiving STOP command, Disable all the IP's which need to access Memories
            DisableMemoryBasedHardware();

            //Handle pending AEC request, if any.
            HandlePendingAecRequest();

	    // Check if Lens movement has stopped, if not then fire a blocking timer
	    while (FLADriver_FocusControlGetCmdControlCoin() != FLADriver_FocusControlGetCmdStatusCoin())
	    {
                LLA_Abstraction_CamDrvHandleMsg();
	    }
            //Handle pending AF request, if any.
            HandlePendingAFRequest();

            // US 5/12 Front End Error Recovery. After Commanding Stop, We don't want to Enter Stopped
            if((Flag_e_TRUE == g_ErrorHandler.e_Flag_Error_Abort)
            && (Flag_e_FALSE == g_ErrorHandler.e_Flag_Error_Recover))
            {
                // Flag reset as it's use is complete. Also to Remove the local loop of Entering stop -> Entering Error
                g_ErrorHandler.e_Flag_Error_Abort = Flag_e_FALSE;

                // Notify mmdsp grab that an IPP error occurred and smia-Rx abort has been done
                g_GrabNotify.u32_DMA_GRAB_Abort = 1;
                EventManager_DMA_GRAB_Abort_Notify();

                // wait till grab component takes appropriate action on this DMA grab notification
                // grab nmf component must reset g_GrabNotify.u32_DMA_GRAB_Abort thereafter
                while (g_GrabNotify.u32_DMA_GRAB_Abort)
                    ;

                OstTraceInt0(TRACE_ERROR, "CSI front end error encountered - Entering ERROR");
                e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_ENTERING_ERROR;
            }
            else
            {
                  if( HostInterfaceCommand_e_STOP_ISP == g_HostInterface_Control.e_HostInterfaceCommand_User)
                 {
                       // RX input stopped, transition to the current state
                       e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_RXSTOPPED;
                 }
                 else
                 {
                      // ISP input stopped, transition to the current state
                      e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_STOPPED;
                 }
            }
        }
    }
    else
    {
        // Input image source is not under ISP control
        // Raise an event notification to signal that the ISP has stopped streaming
        // and straight away transition to STOPPED state
        EventManager_ISPStopStreaming_Notify();
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_STOPPED;
    }


 /* g_Exposure_CompilerStatus should be set to '0' when we are going to stop state.
	** This will allow the gain to be applied when same gain are passed from HOST next time.
	*/

	if(HostInterfaceLowLevelState_e_STOPPED == e_HostInterfaceLowLevelState_Next)
	{
		/*Reset exposure compiler status*/
		g_Exposure_CompilerStatus.u16_AnalogGainPending_x256 			= 0;
		g_Exposure_CompilerStatus.u16_frameRate_x100              		= 0;
		g_Exposure_CompilerStatus.u32_TotalIntegrationTimePending_us 	= 0;
	}


    return (e_HostInterfaceLowLevelState_Next);
}


uint8_t
Sleeping(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint8_t e_HostInterfaceLowLevelState_Next;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_SLEPT;
    if (HostInterfaceCommand_e_WAKEUP == g_HostInterface_Control.e_HostInterfaceCommand_User)
    {
        e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_WAKING_UP;
    }


    return (e_HostInterfaceLowLevelState_Next);
}


void
ResetISPBlocks(void)
{
    // <TODO> best option here is to look which clocks are enabled (DataPathCommit)
    // and reset those accordingly.
    // OR : first check if a block has its clock enabled, then reset that block.

/*
    // master_cci
    Set_MASTER_CCI_MASTER_CCI_ENABLE_soft_reset(0x1);
    Set_MASTER_CCI_MASTER_CCI_ENABLE_soft_reset(0x0);

    // user_interface
    Set_USER_IF_USER_IF_ENABLE_soft_reset__B_0x1();
    Set_USER_IF_USER_IF_ENABLE_soft_reset__B_0x0();

    if (Get_CRM_CRM_EN_CLK_PICTOR_GPIO())
    {
        // gpio
        Set_GPIO_GPIO_ENABLE_soft_reset__ENABLE();
        Set_GPIO_GPIO_ENABLE_soft_reset__DISABLE();
    }
*/
    if(Get_CRM_CRM_EN_CLK_SD_HISPEED())
    {
        /*************************  RESET SD PIPE ***************************/

        // bayer_store_sd_clk
        Set_ISP_BAYER_STORE_SD_CLK_ISP_BAYER_STORE_SD_CLK_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_BAYER_STORE_SD_CLK_ISP_BAYER_STORE_SD_CLK_ENABLE_mux2to1_soft_reset__B_0x0();
    }


    if (Get_CRM_CRM_EN_CLK_SD_LOSPEED())
    {
        // sd_idp_gate
        Set_ISP_SD_IDP_GATE_ISP_SD_IDP_GATE_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_SD_IDP_GATE_ISP_SD_IDP_GATE_ENABLE_mux2to1_soft_reset__B_0x0();

        // isp_lbe
        Set_ISP_LBE_ISP_LBE_ENABLE_line_blk_elim_soft_reset__ENABLE();
        Set_ISP_LBE_ISP_LBE_ENABLE_line_blk_elim_soft_reset__DISABLE();
    }


    if (Get_CRM_CRM_EN_CLK_PIPE())
    {
        // bayer_store_pix_clk
        Set_ISP_BAYER_STORE_PIX_CLK_ISP_BAYER_STORE_PIX_CLK_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_BAYER_STORE_PIX_CLK_ISP_BAYER_STORE_PIX_CLK_ENABLE_mux2to1_soft_reset__B_0x0();

        /*************************  RESET SD PIPE ***************************/

        /*************************  RESET RE BLOCK ***************************/

        // rx_abort
        Set_ISP_SMIARX_ISP_SMIARX_CTRL_rx_abort(0x1);
        Set_ISP_SMIARX_ISP_SMIARX_CTRL_rx_abort(0x0);

        // re_bayer_load
        Set_ISP_RE_BAYER_LOAD_ISP_RE_BAYER_LOAD_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_RE_BAYER_LOAD_ISP_RE_BAYER_LOAD_ENABLE_mux2to1_soft_reset__B_0x0();

        // rso
        // flextf
        Set_ISP_FLEXTF_LINEAR_ISP_FLEXTF_LINEAR_ENABLE_soft_reset__B_0x1();
        Set_ISP_FLEXTF_LINEAR_ISP_FLEXTF_LINEAR_ENABLE_soft_reset__B_0x0();

        // gridiron
        Set_ISP_GRIDIRON_ISP_GRIDIRON_ENABLE_soft_reset__ACTIVE();
        Set_ISP_GRIDIRON_ISP_GRIDIRON_ENABLE_soft_reset__INACTIVE();

        // chg
        Set_ISP_CHG_ISP_CHG_ENABLE_chg_soft_reset__ENABLE_SOFT_RESET();
        Set_ISP_CHG_ISP_CHG_ENABLE_chg_soft_reset__DISABLE_SOFT_RESET();

        // snail scorpio
        Set_ISP_SNAIL_SCORPIO_ISP_SNAIL_SCORPIO_ENABLE_snailnorec_soft_reset__SOFT_RESET_DISABLE();
        Set_ISP_SNAIL_SCORPIO_ISP_SNAIL_SCORPIO_ENABLE_snailnorec_soft_reset__SOFT_RESET_ENABLE();

        // scorpio
        Set_ISP_SCORPIO_ISP_SCORPIO_CONTROL_scorpio_soft_reset__ENABLE();
        Set_ISP_SCORPIO_ISP_SCORPIO_CONTROL_scorpio_soft_reset__DISABLE();

        // snail duster
        Set_ISP_SNAIL_DUSTER_ISP_SNAIL_DUSTER_ENABLE_snailnorec_soft_reset__SOFT_RESET_ENABLE();
        Set_ISP_SNAIL_DUSTER_ISP_SNAIL_DUSTER_ENABLE_snailnorec_soft_reset__SOFT_RESET_DISABLE();

        // duster
        Set_ISP_DUSTER_ISP_DUSTER_ENABLE_duster_soft_reset(0x1);
        Set_ISP_DUSTER_ISP_DUSTER_ENABLE_duster_soft_reset(0x0);

        // snail binning
        Set_ISP_SNAIL_BINNING_ISP_SNAIL_BINNING_ENABLE_snailnorec_soft_reset__SOFT_RESET_ENABLE();
        Set_ISP_SNAIL_BINNING_ISP_SNAIL_BINNING_ENABLE_snailnorec_soft_reset__SOFT_RESET_DISABLE();

        // binning repair
        Set_ISP_BINNING_REPAIR_ISP_BINNING_REPAIR_CONTROL_soft_reset__ENABLE();
        Set_ISP_BINNING_REPAIR_ISP_BINNING_REPAIR_CONTROL_soft_reset__DISABLE();

        // crop bayer
        Set_ISP_CROP_BAYER_ISP_CROP_BAYER_ENABLE_crop_soft_reset__SOFT_RST_ENABLE();
        Set_ISP_CROP_BAYER_ISP_CROP_BAYER_ENABLE_crop_soft_reset__SOFT_RST_DISABLE();

        /*************************  RESET RE BLOCK ***************************/

        /*************************  RESET DMCE BLOCK ***************************/

        // snail_babylon
        Set_ISP_SNAIL_BABYLON_ISP_SNAIL_BABYLON_ENABLE_snailnorec_soft_reset__SOFT_RESET_DISABLE();
        Set_ISP_SNAIL_BABYLON_ISP_SNAIL_BABYLON_ENABLE_snailnorec_soft_reset__SOFT_RESET_ENABLE();

        if(Is_8540v1())
        {
            // Mozart  DMCE is being reset here
             Set_ISP_MOZART_ISP_MOZART_ENABLE_soft_reset__B_0x1();
             Set_ISP_MOZART_ISP_MOZART_ENABLE_soft_reset__B_0x0();
        }
        else
        {
            // babylon
             Set_ISP_BABYLON_ISP_BABYLON_ENABLE_soft_reset__B_0x1();
             Set_ISP_BABYLON_ISP_BABYLON_ENABLE_soft_reset__B_0x0();
        }

        // dmce_bayer_load
        Set_ISP_DMCE_BAYER_LOAD_ISP_DMCE_BAYER_LOAD_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_DMCE_BAYER_LOAD_ISP_DMCE_BAYER_LOAD_ENABLE_mux2to1_soft_reset__B_0x0();

        // dmce_rgb_load
        Set_ISP_DMCE_RGB_LOAD_ISP_DMCE_RGB_LOAD_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_DMCE_RGB_LOAD_ISP_DMCE_RGB_LOAD_ENABLE_mux2to1_soft_reset__B_0x0();

        // dmce_rgb_store
        Set_ISP_DMCE_RGB_STORE_ISP_DMCE_RGB_STORE_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_DMCE_RGB_STORE_ISP_DMCE_RGB_STORE_ENABLE_mux2to1_soft_reset__B_0x0();

        /*************************  RESET DMCE BLOCK ***************************/

        /*************************  RESET CE BLOCK ***************************/

        // ce0_idp_gate
        Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_ENABLE_mux2to1_soft_reset__B_0x0();

        // ce0_crop
        Set_ISP_CE0_CROP_ISP_CE0_CROP_ENABLE_crop_soft_reset__SOFT_RST_ENABLE();
        Set_ISP_CE0_CROP_ISP_CE0_CROP_ENABLE_crop_soft_reset__SOFT_RST_DISABLE();

        // ce0_radial_adsoc_pk
        // ce0_radial_adsoc_rp
        // ce0_mux_gps
        Set_ISP_CE0_MUX_GPS_ISP_CE0_MUX_GPS_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_CE0_MUX_GPS_ISP_CE0_MUX_GPS_ENABLE_mux2to1_soft_reset__B_0x0();

        // ce0_mux_sharp
        Set_ISP_CE0_MUX_SHARP_ISP_CE0_MUX_SHARP_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_CE0_MUX_SHARP_ISP_CE0_MUX_SHARP_ENABLE_mux2to1_soft_reset__B_0x0();

        // ce0_mux_unsharp
        Set_ISP_CE0_MUX_UNSHARP_ISP_CE0_MUX_UNSHARP_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_CE0_MUX_UNSHARP_ISP_CE0_MUX_UNSHARP_ENABLE_mux2to1_soft_reset__B_0x0();

        // ce0_flextf_sharp
        Set_ISP_CE0_FLEXTF_SHARP_ISP_CE0_FLEXTF_SHARP_ENABLE_soft_reset__B_0x1();
        Set_ISP_CE0_FLEXTF_SHARP_ISP_CE0_FLEXTF_SHARP_ENABLE_soft_reset__B_0x0();

        // ce0_flextf_unsharp
        Set_ISP_CE0_FLEXTF_UNSHARP_ISP_CE0_FLEXTF_UNSHARP_ENABLE_soft_reset__B_0x1();
        Set_ISP_CE0_FLEXTF_UNSHARP_ISP_CE0_FLEXTF_UNSHARP_ENABLE_soft_reset__B_0x0();

        // ce0_mux_matrix_adsoc
        Set_ISP_CE0_MUX_MATRIX_ADSOC_ISP_CE0_MUX_MATRIX_ADSOC_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_CE0_MUX_MATRIX_ADSOC_ISP_CE0_MUX_MATRIX_ADSOC_ENABLE_mux2to1_soft_reset__B_0x0();

        // ce0_snail0_adsoc
        Set_ISP_CE0_SNAIL0_ADSOC_ISP_CE0_SNAIL0_ADSOC_ENABLE_snailnorec_soft_reset__SOFT_RESET_ENABLE();
        Set_ISP_CE0_SNAIL0_ADSOC_ISP_CE0_SNAIL0_ADSOC_ENABLE_snailnorec_soft_reset__SOFT_RESET_DISABLE();

        // ce0_snail1_adsoc
        Set_ISP_CE0_SNAIL1_ADSOC_ISP_CE0_SNAIL1_ADSOC_ENABLE_snailnorec_soft_reset__SOFT_RESET_ENABLE();
        Set_ISP_CE0_SNAIL1_ADSOC_ISP_CE0_SNAIL1_ADSOC_ENABLE_snailnorec_soft_reset__SOFT_RESET_DISABLE();

        // ce0_matrix
        Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_ENABLE_soft_reset__ENABLE();
        Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_ENABLE_soft_reset__DISABLE();

        // ce0_pixdelay_0
        Set_ISP_CE0_PIXDELAY_0_ISP_CE0_PIXDELAY_0_ENABLE_pix_delay_soft_reset__SOFT_RST_ENABLE();
        Set_ISP_CE0_PIXDELAY_0_ISP_CE0_PIXDELAY_0_ENABLE_pix_delay_soft_reset__SOFT_RST_DISABLE();

        // ce0_pixdelay_1
        Set_ISP_CE0_PIXDELAY_1_ISP_CE0_PIXDELAY_1_ENABLE_pix_delay_soft_reset__SOFT_RST_ENABLE();
        Set_ISP_CE0_PIXDELAY_1_ISP_CE0_PIXDELAY_1_ENABLE_pix_delay_soft_reset__SOFT_RST_DISABLE();

        // ce0_special_fx
        Set_ISP_CE0_SPECIALFX_ISP_CE0_SPECIALFX_SOL_ENABLE_sfx_soft_reset__ENABLE_SOFT_RESET();
        Set_ISP_CE0_SPECIALFX_ISP_CE0_SPECIALFX_SOL_ENABLE_sfx_soft_reset__DISABLE_SOFT_RESET();

        // dither
        // ce0_gps_scalar
        Set_ISP_CE0_GPSSCALER_ISP_CE0_GPSSCALER_ENABLE_gps_soft_reset__ENABLE();
        Set_ISP_CE0_GPSSCALER_ISP_CE0_GPSSCALER_ENABLE_gps_soft_reset__DISABLE();

        // ce0_coder
        Set_ISP_CE0_CODER_ISP_CE0_CODER_ENABLE_soft_reset__ENABLE();
        Set_ISP_CE0_CODER_ISP_CE0_CODER_ENABLE_soft_reset__DISABLE();

        /// For Pipe 1
        // ce1_idp_gate
        Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE_mux2to1_soft_reset__B_0x0();

        // ce1_crop
        Set_ISP_CE1_CROP_ISP_CE1_CROP_ENABLE_crop_soft_reset__SOFT_RST_ENABLE();
        Set_ISP_CE1_CROP_ISP_CE1_CROP_ENABLE_crop_soft_reset__SOFT_RST_DISABLE();

        // ce1_radial_adsoc_pk
        // ce1_radial_adsoc_rp
        // ce1_mux_gps
        Set_ISP_CE1_MUX_GPS_ISP_CE1_MUX_GPS_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_CE1_MUX_GPS_ISP_CE1_MUX_GPS_ENABLE_mux2to1_soft_reset__B_0x0();

        // ce1_mux_sharp
        Set_ISP_CE1_MUX_SHARP_ISP_CE1_MUX_SHARP_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_CE1_MUX_SHARP_ISP_CE1_MUX_SHARP_ENABLE_mux2to1_soft_reset__B_0x0();

        // ce1_mux_unsharp
        Set_ISP_CE1_MUX_UNSHARP_ISP_CE1_MUX_UNSHARP_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_CE1_MUX_UNSHARP_ISP_CE1_MUX_UNSHARP_ENABLE_mux2to1_soft_reset__B_0x0();

        // ce1_flextf_sharp
        Set_ISP_CE1_FLEXTF_SHARP_ISP_CE1_FLEXTF_SHARP_ENABLE_soft_reset__B_0x1();
        Set_ISP_CE1_FLEXTF_SHARP_ISP_CE1_FLEXTF_SHARP_ENABLE_soft_reset__B_0x0();

        // ce1_flextf_unsharp
        Set_ISP_CE1_FLEXTF_UNSHARP_ISP_CE1_FLEXTF_UNSHARP_ENABLE_soft_reset__B_0x1();
        Set_ISP_CE1_FLEXTF_UNSHARP_ISP_CE1_FLEXTF_UNSHARP_ENABLE_soft_reset__B_0x0();

        // ce1_mux_matrix_adsoc
        Set_ISP_CE1_MUX_MATRIX_ADSOC_ISP_CE1_MUX_MATRIX_ADSOC_ENABLE_mux2to1_soft_reset__B_0x1();
        Set_ISP_CE1_MUX_MATRIX_ADSOC_ISP_CE1_MUX_MATRIX_ADSOC_ENABLE_mux2to1_soft_reset__B_0x0();

        // ce1_snail0_adsoc
        Set_ISP_CE1_SNAIL0_ADSOC_ISP_CE1_SNAIL0_ADSOC_ENABLE_snailnorec_soft_reset__SOFT_RESET_ENABLE();
        Set_ISP_CE1_SNAIL0_ADSOC_ISP_CE1_SNAIL0_ADSOC_ENABLE_snailnorec_soft_reset__SOFT_RESET_DISABLE();

        // ce1_snail1_adsoc
        Set_ISP_CE1_SNAIL1_ADSOC_ISP_CE1_SNAIL1_ADSOC_ENABLE_snailnorec_soft_reset__SOFT_RESET_ENABLE();
        Set_ISP_CE1_SNAIL1_ADSOC_ISP_CE1_SNAIL1_ADSOC_ENABLE_snailnorec_soft_reset__SOFT_RESET_DISABLE();

        // ce1_matrix
        Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_ENABLE_soft_reset__ENABLE();
        Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_ENABLE_soft_reset__DISABLE();

        // ce1_pixeldelay_0
        Set_ISP_CE1_PIXDELAY_0_ISP_CE1_PIXDELAY_0_ENABLE_pix_delay_soft_reset__SOFT_RST_ENABLE();
        Set_ISP_CE1_PIXDELAY_0_ISP_CE1_PIXDELAY_0_ENABLE_pix_delay_soft_reset__SOFT_RST_DISABLE();

        // ce1_pixeldelay_1
        Set_ISP_CE1_PIXDELAY_1_ISP_CE1_PIXDELAY_1_ENABLE_pix_delay_soft_reset__SOFT_RST_ENABLE();
        Set_ISP_CE1_PIXDELAY_1_ISP_CE1_PIXDELAY_1_ENABLE_pix_delay_soft_reset__SOFT_RST_DISABLE();

        // ce1_special_fx
        Set_ISP_CE1_SPECIALFX_ISP_CE1_SPECIALFX_SOL_ENABLE_sfx_soft_reset__ENABLE_SOFT_RESET();
        Set_ISP_CE1_SPECIALFX_ISP_CE1_SPECIALFX_SOL_ENABLE_sfx_soft_reset__DISABLE_SOFT_RESET();

        // dither
        // ce1_gps_scalar
        Set_ISP_CE1_GPSSCALER_ISP_CE1_GPSSCALER_ENABLE_gps_soft_reset__ENABLE();
        Set_ISP_CE1_GPSSCALER_ISP_CE1_GPSSCALER_ENABLE_gps_soft_reset__DISABLE();

        // ce1_coder
        Set_ISP_CE1_CODER_ISP_CE1_CODER_ENABLE_soft_reset__ENABLE();
        Set_ISP_CE1_CODER_ISP_CE1_CODER_ENABLE_soft_reset__DISABLE();

        /*************************  RESET CE BLOCK ***************************/

        /*************************  RESET OPF BLOCK ***************************/

        // opf0
        Set_ISP_OPF0_ISP_OPF0_ENABLE_soft_reset__B_0x1();
        Set_ISP_OPF0_ISP_OPF0_ENABLE_soft_reset__B_0x0();

        // opf1
        Set_ISP_OPF1_ISP_OPF1_ENABLE_soft_reset__B_0x1();
        Set_ISP_OPF1_ISP_OPF1_ENABLE_soft_reset__B_0x0();

        /*************************  RESET OPF BLOCK ***************************/
    }


    return;
}


uint8_t
ResetISP(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    uint8_t e_HostInterfaceLowLevelState_Next;

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    e_HostInterfaceLowLevelState_Next = HostInterfaceLowLevelState_e_STOPPED;

    g_HostInterface_Control.e_HostInterfaceCommand_User = HostInterfaceCommand_e_AUTO_STOP;

    ResetISPBlocks();

    return (e_HostInterfaceLowLevelState_Next);
}


/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn           void ResetSystemConfigState( void )
  \brief        Function to reset/initialise system config and stats related variables.
                Must be called at or before beginning of Rx Streaming, in a start-stop-start sequence.
                Also may be called during error recovery. This function must be modified whenever any
                new PE or internal status variable is added/removed.
  \return       void
  \ingroup      HostInterface
  \endif
*/
void
ResetSystemConfigState(void)
{
    /* Reset Page Elements */
    g_SystemConfig_Status.e_Coin_Status = g_SystemSetup.e_Coin_Ctrl;

    SystemConfig_SetSensorSettingStatus(UpdateStatus_e_Idle);
    Flash_SetFlashStatus(UpdateStatus_e_Idle);

    Exposure_ResetErrorState();
    Flash_ResetErrorState();

    g_SystemConfig_Status.e_Coin_Glace_Histogram_Status = g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug;

    g_Glace_Status.u8_ControlUpdateCount = g_Glace_Control.u8_ControlUpdateCount_debug;
    g_Glace_Status.e_GlaceExportStatus = GlaceExportStatus_e_Idle;

    g_HistStats_Status.e_ExportStatus = ExportStatus_e_COMPLETE;
    g_HistStats_Status.e_CoinStatus = g_HistStats_Ctrl.e_CoinCtrl_debug;

    // legacy PEs
    g_SystemConfig_Status.e_UpdateStatus_Exposure = UpdateStatus_e_Idle;
    g_SystemConfig_Status.e_UpdateStatus_FrameRate = UpdateStatus_e_Idle;

    /* Reset Internal variables */
    g_IsGrabOKNotification = Flag_e_FALSE;
    g_GlaceHistogramStatsFrozen = GlaceHistogramStatus_None;
}


void ResetSystemConfigState_BMS0(void)
{
    g_SystemConfig_Status.e_Coin_Status = g_SystemSetup.e_Coin_Ctrl;
    SystemConfig_SetSensorSettingStatus(UpdateStatus_e_Idle);
    Flash_SetFlashStatus(UpdateStatus_e_Idle);
    Exposure_ResetErrorState();
    Flash_ResetErrorState();
    // legacy PEs
    g_SystemConfig_Status.e_UpdateStatus_Exposure = UpdateStatus_e_Idle;
    g_SystemConfig_Status.e_UpdateStatus_FrameRate = UpdateStatus_e_Idle;
}


static void
ReadLLAConfig(void)
{
    Sensor_Output_Mode_ts   sensor_output_mode;
    uint32_t                u32_count = 0;
    if (g_ReadLLAConfig_Control.e_Coin_ReadLLAConfigControl != g_ReadLLAConfig_Status.e_Coin_ReadLLAConfigStatus)
    {
        if
        (
            (g_camera_details.p_sensor_details != NULL)
        &&  (g_camera_details.p_sensor_details->output_mode_capability.p_modes != NULL)
        )
        {
            //g_ReadLLAConfig_Status.u16_number_of_modes = g_camera_details.p_sensor_details->output_mode_capability.number_of_modes;
            if (g_ReadLLAConfig_Status.u16_number_of_modes > g_ReadLLAConfig_Control.u16_SelectMode)
            {
                g_ReadLLAConfig_Status.u16_output_res_height = g_camera_details.p_sensor_details->output_mode_capability.p_modes[g_ReadLLAConfig_Control.u16_SelectMode].output_res.height;
                g_ReadLLAConfig_Status.u16_output_res_width = g_camera_details.p_sensor_details->output_mode_capability.p_modes[g_ReadLLAConfig_Control.u16_SelectMode].output_res.width;

                g_ReadLLAConfig_Status.u16_woi_res_width = g_camera_details.p_sensor_details->output_mode_capability.p_modes[g_ReadLLAConfig_Control.u16_SelectMode].woi_res.width;
                g_ReadLLAConfig_Status.u16_woi_res_height = g_camera_details.p_sensor_details->output_mode_capability.p_modes[g_ReadLLAConfig_Control.u16_SelectMode].woi_res.height;

                g_ReadLLAConfig_Status.u16_max_frame_rate_x100 = g_camera_details.p_sensor_details->output_mode_capability.p_modes[g_ReadLLAConfig_Control.u16_SelectMode].max_frame_rate_x100;
                g_ReadLLAConfig_Status.u16_data_format = ConvertLLCDRawFormatToCSIRawFormat(g_camera_details.p_sensor_details->output_mode_capability.p_modes[g_ReadLLAConfig_Control.u16_SelectMode].data_format);
            }

          if((0 != g_ReadLLAConfig_Control.ptr32_Sensor_Output_Mode_Data_Address)&&(0 != g_ReadLLAConfig_Status.u32_sensor_Output_Mode_data_size))
          {
             for(u32_count = 0; u32_count<g_camera_details.p_sensor_details->output_mode_capability.number_of_modes; u32_count++)
             {
                // copy sensor output mode data for individual modes to the local variable, and than copy this structure to its proper position in
                // HOST provided buffer
               sensor_output_mode.u32_output_res_width = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u32_count].output_res.width;
               //[NON_ISL_SUPPORT]
               //This is done to change the mode details read by ite_nmf. Here non_smia_correction_factor
               //is subtracted from the y_size of each mode
               sensor_output_mode.u32_output_res_height = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u32_count].output_res.height - non_smia_correction_factor;

               sensor_output_mode.u32_woi_res_width = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u32_count].woi_res.width;
               //[NON_ISL_SUPPORT]
               //This is done to change the mode details read by ite_nmf. Here non_smia_correction_factor
               //is subtracted from the y_size of each mode
               sensor_output_mode.u32_woi_res_height = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u32_count].woi_res.height - non_smia_correction_factor;

               sensor_output_mode.u32_data_format = ConvertLLCDRawFormatToCSIRawFormat(g_camera_details.p_sensor_details->output_mode_capability.p_modes[u32_count].data_format);
               sensor_output_mode.u32_usage_restriction_bitmask = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u32_count].usage_restriction_bitmask;
               sensor_output_mode.u32_max_frame_rate_x100 = g_camera_details.p_sensor_details->output_mode_capability.p_modes[u32_count].max_frame_rate_x100;

               memcpy((uint32_t*)(g_ReadLLAConfig_Control.ptr32_Sensor_Output_Mode_Data_Address + u32_count * sizeof(Sensor_Output_Mode_ts)), &sensor_output_mode, sizeof(Sensor_Output_Mode_ts));
              }
          }
        }


        g_ReadLLAConfig_Status.e_Coin_ReadLLAConfigStatus = g_ReadLLAConfig_Control.e_Coin_ReadLLAConfigControl;

        // <HDD> After discussion with Atul. This notification should be sent in both
        // cases. (1) When data corresponding to ALL OUTPUT MODES is exported
        // (2) When data corresponding to only one mode is updated in g_ReadLLAConfig_Status PE.
        EventManager_Sensor_Output_Mode_Export_Notification_Notify();
    }
}


static void
SetLLAConfig(void)
{
    // if HOST gave simultaneous zoom and system-params update (or stats) requests,
    // process zoom command first.
    if (SystemConfig_IsSensorSettingStatusToBeUpdated() && (g_Zoom_CommandControl.u8_CommandCount != g_Zoom_CommandStatus.u8_CommandCount))
    {
        return;
    }


    if (SystemConfig_IsSensorSettingStatusToBeUpdated())
    {
#ifdef PROFILER_USING_XTI
    OstTraceInt0(TRACE_USER8, "start_xti_profiler_exposure");
#endif
        //OstTraceInt0(TRACE_DEBUG, "SetLLAConfig-> Requesting new AEC target");
        //clear all configuration bits, before setting the appropriate bits
        lla_abstraction_ResetConfigContents();

            /// <Todo: BG>: This is a blocking call as per current LLD implementation(22 ms blocking wait)
            /// Therefore this call only returns after aperture is set to the new value. This takes 22ms.
            /// Once timer based LLD implemetation is released(when??), cam_drv_config_set() api needs to
            /// be called first to inquire if new value of aperture is applied, and then call config_set()
            /// to update exposure. Since exposure & aperture are closely related,
            /// it need to be ascertained that config_set() does not get called when aperture is still changing.
#if 0
            if(LLA_Abstraction_Is_Aperture_Supported()&&
            (g_ApertureConfig_Control.u16_requested_f_number_x_100 != g_ApertureConfig_Status.u16_applied_f_number_x_100))
        {
            LLA_Abstraction_Set_Aperture();
        }
#endif

        // Whenever system coin is toggled, we have to copy the values of e.GrabMode_Ctrl to e_GrabMode_Status
        // and e_Flag_Request_ValidFrameNotification_Ctrl to e_Flag_Request_ValidFrameNotification_Status.

        // In code we read from the status values i.e. e_GrabMode_Status and e_Flag_Request_ValidFrameNotification_Status
        g_SystemConfig_Status.e_GrabMode_Status = g_SystemSetup.e_GrabMode_Ctrl;
        g_SystemConfig_Status.e_Flag_Request_ValidFrameNotification_Status = g_SystemSetup.e_Flag_Request_ValidFrameNotification_Ctrl;

        // Apply exposure/gain/FR iff they are different from current settings
        if (SystemConfig_IsAgExposureOrFrameRateChanged())
        {
            CommitAecAndFrameRate();
            SystemConfig_SetExposureOutOfSync();
        }


        // Memorize that new AEC target request has come
          SystemConfig_SetSensorSettingStatus(UpdateStatus_e_Updating);

        //check if flash is to be triggered
        if (Flash_IsFlashToBeTriggered())
        {
            lla_abstraction_ConfigureFlash();
            Flash_SetFlashStatus(UpdateStatus_e_Updating);
        }


        //Configure TestPattern, if user changed its flag
        if (g_TestPattern_Ctrl.e_TestPattern != g_TestPattern_Status.e_TestPattern)
        {
            lla_abstraction_Configure_test_picture();
        }


        // now call config_set if either exposure or flash coin (or both) is toggled
if(   (g_cam_drv_config.config_contents.config_awb_data!= 0) ||
        (g_cam_drv_config.config_contents.config_camera_data !=0) ||
        (g_cam_drv_config.config_contents.config_flash_strobe != 0) ||
        (g_cam_drv_config.config_contents.config_frame_rate!= 0)  ||
        (g_cam_drv_config.config_contents.config_gain_and_exp_time!= 0) ||
        (g_cam_drv_config.config_contents.config_sensor_mode!= 0) ||
        (g_cam_drv_config.config_contents.config_test_picture!= 0) ||
        (g_cam_drv_config.config_contents.config_woi!= 0) ||
        (g_cam_drv_config.config_contents.config_zoom_roi != 0) )
    {
           g_Config_set_lock = Flag_e_TRUE;
           LLA_Abstraction_ConfigSet_streaming();
           g_Config_set_lock = Flag_e_FALSE;
         }
        // set ND filter, if user changed its flag
        LLA_Abstraction_Set_NDFilter();

        OstTraceInt0(TRACE_DEBUG, "Requesting new config complete SetLLAConfig -< ");
    }
}


static void
ReadLLASensorSettings(void)
{
    //Switching to this state could be useful for debugging and error handling.
    if (SystemConfig_IsSensorSettingStatusUpdating())
    {
        SystemConfig_SetSensorSettingStatus(UpdateStatus_e_WaitingForParams);

        // maintain legacy PEs
        g_SystemConfig_Status.e_UpdateStatus_Exposure = UpdateStatus_e_WaitingForParams;
        g_SystemConfig_Status.e_UpdateStatus_FrameRate = UpdateStatus_e_WaitingForParams;
    }


    if (Flash_IsFlashStatusUpdating())
    {
        Flash_SetFlashStatus(UpdateStatus_e_WaitingForParams);
    }


    // Inquire sensor settings for both flash and exposure
    if (
         (SystemConfig_IsSensorSettingStatusWaitingForParams())
         || (Flash_IsFlashStatusWaitingForParams())
         || (0 == g_RxFrameCounter)     /*first frame usecase */
         || (GlaceOperationMode_e_Continuous == g_Glace_Control.e_GlaceOperationMode_Control)   /*continuous glace usecase*/
       )
    {
        LLA_Abstraction_InterpretSensorSettings();
    }

    if (SystemConfig_IsSensorSettingStatusWaitingForParams())
    {
        // confirm if exposure gains have been absorbed or not.
        // FIDO ER#355597: ISL contents matching should not be done in case of testpattern mode
        if (
            ( (Exposure_GetForceInputProcUpdationStatus()) || (LLA_Abstraction_IsExpGainSettingsAbsorbed()) )
              ||
            ( TestPattern_e_Normal != g_TestPattern_Ctrl.e_TestPattern )
           )
        {
            CommitDG();                         // commit DG to pipe

            //Update and Commit Dampers
            ApplyISPParams();

            //[TBC]update AEC target frame counter at this stage
            g_AECTargetFrameCounter = Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter();

            SystemConfig_SetExposureInSync();   // set exp in sync
            FrameDimension_UpdateCurrentVTFrameLength();                    // update the current vt frame length...
            Exposure_ResetErrorState();                                     // reset the exposure error manager

            // maintain legacy PEs
            g_SystemConfig_Status.e_UpdateStatus_Exposure = UpdateStatus_e_ParamsReceived;
            g_SystemConfig_Status.e_UpdateStatus_FrameRate = UpdateStatus_e_ParamsReceived;

            EventManager_ExposureAndWbParametersUpdated_Notify();           // notify host of the event

            // Finally change the status. This will allow glace/histo interrupt to process.
            SystemConfig_SetSensorSettingStatus(UpdateStatus_e_ParamsReceived);

            //Also take care of flash + exposure simultaneous request.
            //In that case, send notify only if flash lit frame has arrived.
            if (SYSTEM_CONFIG_PARAMS_ABSORBED())
            {
                g_IsGrabOKNotification = Flag_e_TRUE;
            }
        }
        else
        {
            SystemConfig_SetExposureOutOfSync();
            Exposure_FrameSyncFailed();
            SystemConfig_SetSensorSettingStatus(UpdateStatus_e_Updating);   // synch not done in this frame, so wait til smiaRx for next frame is received
        }
    }


    if (Flash_IsFlashStatusWaitingForParams())
    {
        Flash_HandleFlashTrigger();
    }
    else if (0 == g_RxFrameCounter)  //first frame usecase
    {
        if (LLA_IS_LIT_FRAME())
        {
            OstTraceInt0(TRACE_DEBUG, "->ReadLLASensorSettings: flash fired in first frame!");

            //increment flash fired count
            g_FlashStatus.u8_FlashFiredFrameCount = 1;

            //set flash-fired page element
            g_FrameParamStatus.u32_flash_fired = 1;

            //set frame status frame no to zero
            g_FrameParamStatus.u32_frame_counter = 0;

            //trigger DMA Grab OK
            g_IsGrabOKNotification = Flag_e_TRUE;
        }
    }

    // Fix for PictorBug #115933
    //read pixel order value
    if (SystemConfig_IsInputImageSourceSensor())
    {
        //[NON_ISL_SUPPORT] - for non ISL sensors, pixel order is to be read from override registers
        if(SENSOR_VERSION_NON_SMIA == g_SensorInformation.u8_smia_version)
        {
            g_SystemConfig_Status.e_PixelOrder = Get_ISP_SMIARX_ISP_SMIARX_PIXEL_ORDER_pixel_order_ovr();
        }
        else
        {
            g_SystemConfig_Status.e_PixelOrder = Get_ISP_SMIARX_ISP_SMIARX_PIXEL_ORDER_pixel_order();
        }
    }

    //Handle notification to host
    Notify_DMA_GRAB();

   /*continuous glace usecase*/
     if (GlaceOperationMode_e_Continuous == g_Glace_Control.e_GlaceOperationMode_Control)
    {
        LLA_Abstraction_IsExpGainSettingsAbsorbed();
    }

    lla_abstraction_ResetInterpretSensorSettingsContents();
}


static void
CommitAecAndFrameRate(void)
{
    LLA_Abstraction_ApplyAecAndFreameRate(&g_Exposure_DriverControls);

    g_Exposure_CompilerStatus.u16_AnalogGainPending_x256 = g_Exposure_DriverControls.u16_TargetAnalogGain_x256;

    g_Exposure_CompilerStatus.u16_frameRate_x100 = (uint16_t) (FrameRate_GetAskedFrameRate() * 100);
    g_Exposure_CompilerStatus.u32_TotalIntegrationTimePending_us = g_Exposure_DriverControls.u32_TargetExposureTime_us;

}


static void
CommitDG(void)
{
    g_WhiteBalanceStatus.f_RedGain = g_WhiteBalanceControl.f_RedManualGain;
    g_FrameParamStatus.u32_RedGain_x1000 = g_WhiteBalanceControl.f_RedManualGain * 1000;

    g_WhiteBalanceStatus.f_GreenGain = g_WhiteBalanceControl.f_GreenManualGain;
    g_FrameParamStatus.u32_GreenGain_x1000 = g_WhiteBalanceControl.f_GreenManualGain * 1000;

    g_WhiteBalanceStatus.f_BlueGain = g_WhiteBalanceControl.f_BlueManualGain;
    g_FrameParamStatus.u32_BlueGain_x1000 = g_WhiteBalanceControl.f_BlueManualGain * 1000;

    ChannelGain_Configure();
    ChannelGain_CalculateAllGains();    //Calculate digital gains to be applied to pipe
    ChannelGain_ApplyAllGains();        // Load the channel gain module with  the calculated gains
}


/**
 \fn        void HostInterface_SensorSettings ( void )
 \brief     Function called from RUN state to apply ISP params, in case the Host has
            toggled ApplyAEC Coin.
 \param     void
 \return    void
 \ingroup   HostInterface
*/
static void
HostInterface_SensorSettings(void)
{
    if (g_SensorPipeSettings_Status.e_Coin_SensorSettings != g_SensorPipeSettings_Control.e_Coin_SensorSettings)
    {
        OstTraceInt0(TRACE_FLOW, "->HostInterface_SensorSettings: toggled!");
        CommitAecAndFrameRate();

        g_Config_set_lock = Flag_e_TRUE;
        LLA_Abstraction_ConfigSet_streaming();
        g_Config_set_lock = Flag_e_FALSE;

        g_SensorPipeSettings_Status.e_Coin_SensorSettings = g_SensorPipeSettings_Control.e_Coin_SensorSettings;
        EventManager_SensorCommit_Notify();
        OstTraceInt0(TRACE_FLOW, "<-HostInterface_SensorSettings: notified");
    }
}


/**
 \fn        void HostInterface_ISPSettings ( void )
 \brief     Function called from RUN state to apply ISP params, in case the Host has
            toggled CommitPipe Coin.
 \param     void
 \return    void
 \ingroup   HostInterface
*/
static void
HostInterface_ISPSettings(void)
{
    //[PM][TBC]To prevent the dual application of ISP Params for first frame.
    if (g_SensorPipeSettings_Status.e_Coin_ISPSettings != g_SensorPipeSettings_Control.e_Coin_ISPSettings)
    {
        OstTraceInt0(TRACE_FLOW, "->HostInterface_ISPSettings: toggled!");

        CommitDG();        //commit DG [FIDO ER#371356]
        ApplyISPParams();  //commit remaining parameters
        g_SensorPipeSettings_Status.e_Coin_ISPSettings = g_SensorPipeSettings_Control.e_Coin_ISPSettings;
        EventManager_ISPCommit_Notify();

        OstTraceInt0(TRACE_FLOW, "<-HostInterface_ISPSettings: configset called");
    }
}


/**
 \fn        void NotifyGlaceHistogramStatistics ( void )
 \brief     Function called from RUN state to notify HOST that valid stats have arrived.
 \param     void
 \return    void
 \ingroup   HostInterface
*/
static void
NotifyGlaceHistogramStatistics(void)
{
    //If any of either exposure OR flash param absorption pending, return.
    if (SYSTEM_CONFIG_PARAMS_ABSORBED())
    {
        if (AEC_STATS_COPIED())
        {
            //mark stats as valid
            g_FrameParamStatus_Extn.u32_StatsInvalid = 0;

            //Notify statistics
            UpdateFrameStatus();

            if (COMPLETE_AEC_STATS_AND_NOTIFY_AFTER_STOP())
            {
                //Hold stats notification
                g_AecStatsNotifyPending = Flag_e_TRUE;
            }
            else
            {
                //Send notification and Complete Aec command
                CompleteAecNotify();
#ifdef PROFILER_USING_XTI
        OstTraceInt0(TRACE_USER8, "stop_xti_profiler_exposure");
#endif

            }
        }
        else
        {
            if(SystemConfig_IsBayerStore0Active())
            {
                //Marking stats as invalid
                g_FrameParamStatus_Extn.u32_StatsInvalid = 1;
                UpdateFrameStatus();
                ResetSystemConfigState_BMS0();
            }
        }
    }

    return;
}


/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn           void HandlePendingAecRequest( void )
  \brief        Function to handle pending stats request, if any.
                Handling depends on the value of g_SystemSetup.e_Flag_AecStatsCancel.
  \return       void
  \ingroup      HostInterface
  \endif
*/
static void
HandlePendingAecRequest(void)
{
    // Cancel AEC pending stats request, if any
    if
    (
        (Flag_e_TRUE == g_SystemSetup.e_Flag_AecStatsCancel)
    ||  (Flag_e_TRUE == g_SystemSetup.e_Flag_abortRx_OnStop)
    ||  (Flag_e_TRUE == g_ErrorHandler.e_Flag_Error_Abort)
    )
    {
        CancelAecStatistics();
    }
    else
    {
        //Complete AEC pending stats notification, if any.
        if (Flag_e_TRUE == g_AecStatsNotifyPending)
        {
            CompleteAecNotify();
            g_AecStatsNotifyPending = Flag_e_FALSE;
        }
    }


    return;
}


/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn           void CancelAecStatistics( void )
  \brief        Function to cancel pending stats request, if any, and
                raise a dummy notification to the HOST.
  \return       void
  \ingroup      HostInterface
  \endif
*/
void
CancelAecStatistics(void)
{
    //If its sensor changover usecase, don't do anything with stats.
    if (Zoom_IsFDMRequestPending())
    {
        return;
    }


    //In future, the BMS AutoStop usecase could be added here.
    if (AEC_STATS_REQUESTED())
    {
        //set internal stats cancel request flag as early as possible, so that
        //there is more probability to stop cancel stats export.
        SystemConfig_SetStatsCancelStatus(Flag_e_TRUE);

        // disable the Glace IP, it is immediate
        Glace_Disable();

        // Disable the input to the Glace IP
        Glace_DisableDataInput();

        // Disable Histo IP
        // Set_HIST_ENABLE(Enable, SoftResest)
        Set_HIST_ENABLE(Flag_e_FALSE, Flag_e_FALSE);

        // Disable the Data Input
        // Hist_SetDataInput(IP_ENABLE, IP_SOFT_ENABLE)
        Hist_SetDataInput(Flag_e_FALSE, Flag_e_FALSE);

        //mark stats as invalid
        g_FrameParamStatus_Extn.u32_StatsInvalid = 1;

        //Trigger dummy notification
        UpdateFrameStatus();

        //Reset system variables and send notification
        CompleteAecNotify();
    }


    return;
}


/**
 \fn        void UpdateFrameStatus ( void )
 \brief     Internal Function used to update FrameParamStatus, before notifying the Host.
            g_FrameParamStatus_Extn.u32_StatsInvalid MUST be set (to 0/1) before calling this function.
 \param     void
 \return    void
 \ingroup   HostInterface
*/
static void
UpdateFrameStatus(void)
{
    //Case 1: If both Lit frame and AEC target absorbed in a simultaneous request.
    //Take frame counter for the one which has been applied later.
    if (Flash_IsFlashStatusParamsReceived() && SystemConfig_IsSensorSettingStatusParamsReceived())
    {
        //In this case it is certain that flash is applied after exposure, so use flash counter
        g_FrameParamStatus.u32_frame_counter = g_FlashLitFrameCounter;

        //OstTraceInt0(TRACE_DEBUG, "[BUG 323464]FLASH & AEC simultaneous use case");
    }
    else if (Flash_IsFlashStatusParamsReceived())      //Case 2: If it was a flash Lit frame.
    {
        g_FrameParamStatus.u32_frame_counter = g_FlashLitFrameCounter;

        //this check is for sequence flash case
#if 0
        if (g_FlashStatus.u8_FlashFiredFrameCount == g_FlashControl.u8_FrameCount)
        {
            g_FlashControl.e_Flag_FlashMode = Flag_e_FALSE;
        }


#endif

        //OstTraceInt0(TRACE_DEBUG, "[BUG 323464]FLASH use case");
    }
    else if (SystemConfig_IsSensorSettingStatusParamsReceived())     //Case 3: If user requested new AEC target
    {
        g_FrameParamStatus.u32_frame_counter = g_AECTargetFrameCounter;

        //[FIDO 349641]reset flash-fired page element, since this is a non flash use case.
        g_FrameParamStatus.u32_flash_fired = 0;

        //OstTraceInt0(TRACE_DEBUG, "[BUG 323464] AEC use case");
    }
    else if (g_SystemConfig_Status.e_Coin_Glace_Histogram_Status != g_SystemSetup.e_Coin_Glace_Histogram_Ctrl_debug)      //Case 4: If user had toggled e_Coin_Glace_Histogram_Status coin.
    {
        //[TBD] update frame counter here. To be added in future.
        //g_FrameParamStatus.u32_frame_counter = g_GlaceHistoFrameCounter;
    }

    //export FrameStatus: it is expected HOST will read u32_StatsInvalid before trying to read stats
    FrameStatusExport();
}


/**
 \fn        void CompleteAecNotify ( void )
 \brief     Internal Function used to complete pending AEC notification to the Host. This will be called
            also when the notification was held back because of sensor stop command.
 \param     void
 \return    void
 \ingroup   HostInterface
*/
static void
CompleteAecNotify(void)
{
    //reset all system variables
    ResetSystemConfigState();

    //notify glace
    Glace_StatsReadyNotify();

    //If Rx streaming, restart stat collection
    if (Stream_IsISPInputStreaming())
    {
        PipeAlgorithm_CommitGlaceAndHistogram();
    }
}


void
FrameStatusExport(void)
{
    volatile FrameParamStatus_ts        *ptr_FrameParamStatus;
    volatile FrameParamStatus_Extn_ts   *ptr_FrameParamStatus_Extn;

    ptr_FrameParamStatus = ( FrameParamStatus_ts * ) g_FrameParamStatus.ptru32_SensorParametersTargetAddress;
    ptr_FrameParamStatus_Extn = (FrameParamStatus_Extn_ts *) (g_FrameParamStatus.ptru32_SensorParametersTargetAddress + sizeof(FrameParamStatus_ts) / 4);

    // copy params to external memory iff ptr_FrameParamStatus is valid
    if (0 != ptr_FrameParamStatus)
    {
        memcpy((uint32_t*)ptr_FrameParamStatus, &g_FrameParamStatus, sizeof(FrameParamStatus_ts));
        memcpy((uint32_t*)ptr_FrameParamStatus_Extn, &g_FrameParamStatus_Extn, sizeof(FrameParamStatus_Extn_ts));
    }

    OstTraceInt2(TRACE_DEBUG, "FrameStatusExport: Requested- Exp: %d, AG_x256: %d", g_Exposure_DriverControls.u32_TargetExposureTime_us, g_Exposure_DriverControls.u16_TargetAnalogGain_x256);
    OstTraceInt2(TRACE_DEBUG, "FrameStatusExport: Feedback - Exp: %d, AG_x256: %d", g_Exposure_ParametersApplied.u32_TotalIntegrationTime_us, g_Exposure_ParametersApplied.u16_AnalogGain_x256);
    OstTraceInt2(TRACE_DEBUG, "FrameStatusExport: Applied  - Exp: %d, AG_x256: %d", g_FrameParamStatus.u32_ExposureTime_us, g_FrameParamStatus.u32_AnalogGain_x256);
    OstTraceInt2(TRACE_DEBUG, "FrameStatusExport: FrameRate_x100: %d, frame_counter: %d", g_FrameParamStatus.u32_frameRate_x100, g_FrameParamStatus.u32_frame_counter);
//    OstTraceInt2(TRACE_DEBUG, "Req Line Len %d, LineLen %d", g_RequestedFrameDimension.u16_VTLineLengthPck, g_CurrentFrameDimension.u16_VTLineLengthPck);
}


static void
ApplyISPParams(void)
{
    //STEP1: Updating IPs
    UpdateISPModules();

    //STEP2: Commiting IPs
    CommitISPModules();

    return;
}


static void
UpdateISPModules(void)
{
    // update the SD pipe params
    if (SystemConfig_IsSDPipeActive())
    {
        PipeAlgorithm_UpdateSDPipe();
    }


    // update the RE params
    if (SystemConfig_IsRE_Active())
    {
        PipeAlgorithm_UpdateRE();
    }


    // update the DMCE params
    if (SystemConfig_IsDMCE_Active())
    {
        PipeAlgorithm_UpdateDMCE();
    }


    // if pipe0 is enabled then perform all computations for pipe0
    if (SystemConfig_IsPipe0Active())
    {
        PipeAlgorithm_UpdatePipe0();
    }


    // if pipe1 is enabled then perform all computations for pipe1
    if (SystemConfig_IsPipe1Active())
    {
        PipeAlgorithm_UpdatePipe1();
    }


    return;
}


static void
CommitISPModules(void)
{
    OstTraceInt0(TRACE_DEBUG, "->CommitISPModules");

    // IPs to be commited in PipeAlgorithm_CommitInputPipe are :-
    // RSO, Linearisation, Gridiron, Duster, Scorpio and Binning.
    // Channel Gain is not commited here as it is done before calling this function.

    PipeAlgorithm_CommitInputPipe();


    if (!Zoom_IsFDMRequestPending())
    {

        if (SystemConfig_IsPipe0Active())
        {
            OstTraceInt0(TRACE_DEBUG, "PIPE 0 commit");

            // Update Colour engine related paramters
            ColourMatrix_Commit(0);

            Adsoc_Commit(0);

            Update_Gamma_Pipe(0);

            if (Is_8540v1())
            {
                Norcos_Commit(0);
                /* Norcos is enabled or not, this check needs to be validated. As of now correct method of testing this is not known.
                TBD in future */
                if (Is_ISP_CE0_NORCOS_ISP_CE0_NORCOS_ENABLE_norcos_luma_enable_ENABLE() && Is_ISP_CE0_NORCOS_ISP_CE0_NORCOS_ENABLE_norcos_chroma_enable_ENABLE() && Get_ISP_CE0_NORCOS_ISP_CE0_NORCOS_ENABLE_norcos_yuv422_enable() )
                {
                    RgbToYuvMatrix_Commit(0);
                    // Pixel shift U10->U8 done in HW
                    YuvToRgbMatrix_Commit(0);
                    // Pixel shift U8->U10done in HW
                    Set_ISP_CE0_MUX_0_UNPEAKING_ISP_CE0_MUX_0_UNPEAKING_SELECT(1,0);
                    Set_ISP_CE0_MUX_1_UNPEAKING_ISP_CE0_MUX_1_UNPEAKING_SELECT(1,0);
                    Set_ISP_CE0_MUX_1_PEAKING_ISP_CE0_MUX_1_PEAKING_SELECT(1,0);
                }
                else
                {
                    Set_ISP_CE0_MUX_0_UNPEAKING_ISP_CE0_MUX_0_UNPEAKING_SELECT(0,0);
                    Set_ISP_CE0_MUX_1_UNPEAKING_ISP_CE0_MUX_1_UNPEAKING_SELECT(0,0);
                    Set_ISP_CE0_MUX_1_PEAKING_ISP_CE0_MUX_1_PEAKING_SELECT(0,0);
                }
                Set_ISP_CE0_MUX_0_UNPEAKING_ISP_CE0_MUX_0_UNPEAKING_ENABLE(1,0);
                Set_ISP_CE0_MUX_1_UNPEAKING_ISP_CE0_MUX_1_UNPEAKING_ENABLE(1,0);
                Set_ISP_CE0_MUX_0_PEAKING_ISP_CE0_MUX_0_PEAKING_ENABLE(1,0);
                Set_ISP_CE0_MUX_0_PEAKING_ISP_CE0_MUX_0_PEAKING_SELECT(0,0);
                Set_ISP_CE0_MUX_1_PEAKING_ISP_CE0_MUX_1_PEAKING_ENABLE(1,0);
            }

            RGBtoYUVCoder_Commit(0);

            Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_ENABLE(0);  // rdpk_enable
            // ISP_CEy_SNAILx_ADSOC
            Set_ISP_CE0_SNAIL0_ADSOC_ISP_CE0_SNAIL0_ADSOC_ENABLE(1, 0);     // snailnorec_enable,snailnorec_soft_reset
            Set_ISP_CE0_SNAIL1_ADSOC_ISP_CE0_SNAIL1_ADSOC_ENABLE(1, 0);     // snailnorec_enable,snailnorec_soft_reset

            // ISP_CEx_MATRIX
            Set_ISP_CE0_MATRIX_ISP_CE0_MATRIX_ENABLE(1, 0);                 // enable,soft_reset

            // ISP_CEy_PIXDELAY_x
            Set_ISP_CE0_PIXDELAY_0_ISP_CE0_PIXDELAY_0_ENABLE(1, 0);         // pix_delay_enable,pix_delay_soft_reset
            Set_ISP_CE0_PIXDELAY_1_ISP_CE0_PIXDELAY_1_ENABLE(1, 0);         // pix_delay_enable,pix_delay_soft_reset

            // Setup ISP_CEx_SPECIALFX
            if (SFXSolariseControl_Enable == g_SpecialEffects_Control[0].e_SFXSolarisControl)
            {
                Set_ISP_CE0_SPECIALFX_ISP_CE0_SPECIALFX_SOL_ENABLE(1, 0);   // sfx_sol_enable,sfx_soft_reset
            }
            else
            {
                Set_ISP_CE0_SPECIALFX_ISP_CE0_SPECIALFX_SOL_ENABLE(0, 0);   // sfx_sol_enable,sfx_soft_reset
            }


            if (SFXNegativeControl_Enable == g_SpecialEffects_Control[0].e_SFXNegativeControl)
            {
                Set_ISP_CE0_SPECIALFX_ISP_CE0_SPECIALFX_NEG_ENABLE(1);      // sfx_neg_enable
            }
            else
            {
                Set_ISP_CE0_SPECIALFX_ISP_CE0_SPECIALFX_NEG_ENABLE(0);      // sfx_neg_enable
            }


            // Setup ISP_CEx_DITHER
            Set_ISP_CE0_DITHER_ISP_CE0_DITHER_ENABLE(0);                    // enable

            // Setup  ISP_CEx_CODER
            Set_ISP_CE0_CODER_ISP_CE0_CODER_ENABLE(1, 0);                   // enable,soft_reset
        }


        if (SystemConfig_IsPipe1Active())
        {
            OstTraceInt0(TRACE_DEBUG, "PIPE 1 commit");

            // Update Colour engine related paramters
            ColourMatrix_Commit(1);

            Adsoc_Commit(1);

            Update_Gamma_Pipe(1);

            if (Is_8540v1())
            {
                Norcos_Commit(1);
                /* Norcos is enabled or not, this check needs to be validated. As of now correct method of testing this is not known.
                TBD in future */
                if (Is_ISP_CE1_NORCOS_ISP_CE1_NORCOS_ENABLE_norcos_luma_enable_ENABLE() && Is_ISP_CE1_NORCOS_ISP_CE1_NORCOS_ENABLE_norcos_chroma_enable_ENABLE() && Get_ISP_CE1_NORCOS_ISP_CE1_NORCOS_ENABLE_norcos_yuv422_enable())
                {
                    RgbToYuvMatrix_Commit(1);
                    // Pixel shift U10->U8 done in HW
                    YuvToRgbMatrix_Commit(1);
                    // Pixel shift U8->U10 done in HW
                    Set_ISP_CE1_MUX_0_UNPEAKING_ISP_CE1_MUX_0_UNPEAKING_SELECT(1,0);
                    Set_ISP_CE1_MUX_1_UNPEAKING_ISP_CE1_MUX_1_UNPEAKING_SELECT(1,0);
                    Set_ISP_CE1_MUX_1_PEAKING_ISP_CE1_MUX_1_PEAKING_SELECT(1,0);
                }
                else
                {
                    Set_ISP_CE1_MUX_0_UNPEAKING_ISP_CE1_MUX_0_UNPEAKING_SELECT(0,0);
                    Set_ISP_CE1_MUX_1_UNPEAKING_ISP_CE1_MUX_1_UNPEAKING_SELECT(0,0);
                    Set_ISP_CE1_MUX_1_PEAKING_ISP_CE1_MUX_1_PEAKING_SELECT(0,0);
                }
                Set_ISP_CE1_MUX_0_UNPEAKING_ISP_CE1_MUX_0_UNPEAKING_ENABLE(1,0);
                Set_ISP_CE1_MUX_1_UNPEAKING_ISP_CE1_MUX_1_UNPEAKING_ENABLE(1,0);
                Set_ISP_CE1_MUX_0_PEAKING_ISP_CE1_MUX_0_PEAKING_ENABLE(1,0);
                Set_ISP_CE1_MUX_0_PEAKING_ISP_CE1_MUX_0_PEAKING_SELECT(0,0);
                Set_ISP_CE1_MUX_1_PEAKING_ISP_CE1_MUX_1_PEAKING_ENABLE(1,0);
            }

            RGBtoYUVCoder_Commit(1);

            Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_ENABLE(0);  // rdpk_enable
            // ISP_CEy_SNAILx_ADSOC
            Set_ISP_CE1_SNAIL0_ADSOC_ISP_CE1_SNAIL0_ADSOC_ENABLE(1, 0);     // snailnorec_enable,snailnorec_soft_reset
            Set_ISP_CE1_SNAIL1_ADSOC_ISP_CE1_SNAIL1_ADSOC_ENABLE(1, 0);     // snailnorec_enable,snailnorec_soft_reset

            // ISP_CEx_MATRIX
            Set_ISP_CE1_MATRIX_ISP_CE1_MATRIX_ENABLE(1, 0);                 // enable,soft_reset

            // ISP_CEy_PIXDELAY_x
            Set_ISP_CE1_PIXDELAY_0_ISP_CE1_PIXDELAY_0_ENABLE(1, 0);         // pix_delay_enable,pix_delay_soft_reset
            Set_ISP_CE1_PIXDELAY_1_ISP_CE1_PIXDELAY_1_ENABLE(1, 0);         // pix_delay_enable,pix_delay_soft_reset

            // Setup ISP_CEx_SPECIALFX
            if (SFXSolariseControl_Enable == g_SpecialEffects_Control[1].e_SFXSolarisControl)
            {
                Set_ISP_CE1_SPECIALFX_ISP_CE1_SPECIALFX_SOL_ENABLE(1, 0);   // sfx_sol_enable,sfx_soft_reset
            }
            else
            {
                Set_ISP_CE1_SPECIALFX_ISP_CE1_SPECIALFX_SOL_ENABLE(0, 0);   // sfx_sol_enable,sfx_soft_reset
            }


            if (SFXNegativeControl_Enable == g_SpecialEffects_Control[1].e_SFXNegativeControl)
            {
                Set_ISP_CE1_SPECIALFX_ISP_CE1_SPECIALFX_NEG_ENABLE(1);      // sfx_neg_enable
            }
            else
            {
                Set_ISP_CE1_SPECIALFX_ISP_CE1_SPECIALFX_NEG_ENABLE(0);      // sfx_neg_enable
            }


            // Setup ISP_CEx_DITHER
            Set_ISP_CE1_DITHER_ISP_CE1_DITHER_ENABLE(0);                    // enable

            // Setup  ISP_CEx_CODER
            Set_ISP_CE1_CODER_ISP_CE1_CODER_ENABLE(1, 0);                   // enable,soft_reset
        }
    }


    return;
}


static void
Notify_DMA_GRAB(void)
{
    // Grab_OK/NOK shouldbe supported for both BMS & HR
    if (IsBMS2Enabled() || SystemConfig_IsPipe0Active())
    {
        /* set OK notification when its GrabMode_e_FORCE_OK usecase; OR
         * its the normal BMS use case in which AEC targets have been absorbed.
         */
        if (
            ((Flag_e_TRUE == g_IsGrabOKNotification) && (GrabMode_e_NORMAL == g_SystemConfig_Status.e_GrabMode_Status))
            ||
            (GrabMode_e_FORCE_OK == g_SystemConfig_Status.e_GrabMode_Status)
           )
        {
            /* set the fixed-address shared-variable to VALID and notify host;
             * update frame counter in the fixed address space: this should be the actual frame counter
             * and not AECFrameCounter or FlashFrameCounter;
             * send grab and valid frame notifications
             */
            g_GrabNotify.u32_DMA_GRAB_Indicator = 1;
            g_GrabNotify.u32_DMA_GRAB_Frame_Id = Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter();
            HDR_CheckDMAValidFrame();
            EventManager_DMA_GRAB_Notify_OK();

            // If g_SystemConfig_Status.e_Flag_Request_ValidFrameNotification_Status flag is true then only we
            // send "Valid Frame Notification" otherwise not.
            if (Flag_e_TRUE == g_SystemConfig_Status.e_Flag_Request_ValidFrameNotification_Status)
            {
                EventManager_Valid_Frame_Received_Notify();
            }

            /* Added the log messages to notify about the applied Gain/EXP/LineLenPck/FrameLenLines
            values for captured frame. */
            {
                static uint32_t GrabCnt = 0;
                GrabCnt++;
                OstTraceInt3(TRACE_USER6, "GrabOK - %d, AGx1000: %d, EXP: %dus", GrabCnt, g_SensorSettings.analog_gain_x1000, g_SensorSettings.exposure_time_us);
                OstTraceInt3(TRACE_USER6, "GrabOK - %d, LineLengthPck = %d, FrameLengthLines = %d",GrabCnt, g_CurrentFrameDimension.u16_VTLineLengthPck, g_CurrentFrameDimension.u16_VTFrameLengthLines);
            }
        }
        /* In all other use cases, send DMA NOTOK. It includes the use case
         * (GrabMode_e_FORCE_NOK == g_SystemSetup.e_GrabMode_Ctrl)
         * which the Host can set in case of Pre-Flash.
         */
        else
        {
            /* set the fixed-address shared-variable to INVALID and notify host;
             * send notification */
            g_GrabNotify.u32_DMA_GRAB_Indicator = 0;

            //Updating the frame counter even for DMA NOK frames as it will help in debugging frame sync issues at MMDSP level.
            g_GrabNotify.u32_DMA_GRAB_Frame_Id = Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter();
            EventManager_DMA_GRAB_Notify_NOK();
        }
     }

    /* Set this flag to false unconditionally here itself, as its work its over now, and
     * to prevent next frame to be incorrectly marked as OK. This also safeguards the usecase
     * when this flag was true, but e_GrabMode_Ctrl = GrabMode_e_FORCE_NOK.
     */
     g_IsGrabOKNotification = Flag_e_FALSE;

}


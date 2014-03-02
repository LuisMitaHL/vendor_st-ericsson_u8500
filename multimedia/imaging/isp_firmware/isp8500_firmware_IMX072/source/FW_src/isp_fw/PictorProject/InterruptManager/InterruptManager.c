/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      InterruptManager.c

 \brief     This file implements all the top level interrupt handlers. It also initialises the STXP70 core
            interrupt controller with respect to the interrupt priority, initial interrupt trigger for all
            the interrupts that routed through the ISP interrupt controller to the core interrupt controller.

 \details   The following interrupt handlers are implemented in this file:

            - Host Comms
            - Master Cci
            - NMI
            - Default handler for IRQ inputs not being actively used

 \ingroup   InterruptHandler
 \endif
*/
#include "InterruptManager.h"
#include "STXP70_OPInterface.h"
#include "PictorhwReg.h"
#include "EventManager.h"
#include "ITM.h"
#include "weighted_statistics_processor_op_interface.h"
#include "minimum_weighted_statistics_processor_op_interface.h"
#include "CRM.h"
#include "Platform.h"
#include "GPIOManager_OpInterface.h"
#include "PipeAlgorithm.h"
#include "histogram_op_interface.h"
#include "exposure_statistics_ip_interface.h"
#include "ExpSensor.h"
#include "Exposure.h"
#include "Glace_OPInterface.h"
#include "FrameRate_op_interface.h"
#include "ErrorHandler.h"

#include "Gamma_op_interface.h"
#include "Zoom_OPInterface.h"
#include "PictorhwReg.h"


#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_InterruptManager_InterruptManagerTraces.h"
#endif

#if INCLUDE_FOCUS_MODULES
#include "AFStats_IPInterface.h"
#endif

#include "Profiler.h"


CpuCycleProfiler_ts g_Profile_LCO = DEFAULT_VALUES_CPUCYCLEPROFILER;
CpuCycleProfiler_ts g_Profile_ISPUpdate = DEFAULT_VALUES_CPUCYCLEPROFILER;
CpuCycleProfiler_ts g_Profile_AEC_Statistics = DEFAULT_VALUES_CPUCYCLEPROFILER;
CpuCycleProfiler_ts g_Profile_AWB_Statistics = DEFAULT_VALUES_CPUCYCLEPROFILER;
CpuCycleProfiler_ts g_Profile_VID0 = DEFAULT_VALUES_CPUCYCLEPROFILER;
CpuCycleProfiler_ts g_Profile_VID1 = DEFAULT_VALUES_CPUCYCLEPROFILER;

Interrupts_test_ts  g_Interrupts_Count = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t gu8_ExpStatsTriggered = Flag_e_FALSE;
uint8_t gu8_WbStatsTriggered = Flag_e_FALSE;
volatile uint8_t gu8_DMCEInterruptRxed = Flag_e_FALSE;

void ProcessVideoCompletePipe(uint8_t pipe_no);

/**
  \if   INCLUDE_IN_HTML_ONLY
  \fn   void InterruptManager_Initialise( void )
  \brief    This is the top level function that is used to initialise the core
    interrupt controller for all the interrupts that are routed to it
    through ISP interrupt controller.

  \details      This function must be invoked before any interrupt is used. The following
                interrupts are initialised by the function:
                - Host Comms
                - Master cci
  \return       void
  \callgraph
  \callergraph
  \ingroup      InterruptHandler
  \endif
*/
void
InterruptManager_Initialise(void)
{
    /// \par Implementation
    /// The following operations are performed in sequence:
    /// - Clear any interrupts at the core interrupt controller.
    /// - Set the Interrupt Mask Level of the core interrupt controller to be 0.
    /// This ensures that interrupts of all levels are able to interrupt to STXP70.
    /// - Initialise the host comms interrupt input.
    /// - Initialise the master cci interrupt input.
    /// - Enable the host comms interrupt input at the ISP interrupt controller.
    /// - Enable the master cci interrupt input at the ISP interrupt controller.
    // Clear all possible interrupt sources at ITM
    ITM_ClearHostCommsInterrupt();

    // Set the interrupt mask level for the processor. No IRQ
    // below the specified IML will trigger an interrupt into the processor.
    STXP70_SetIml(0);

    // Initialise all the interrupts and the STXP70 Interrupt controller
    // Interrupt 0: Host comms / USER I/F
    STXP70_InitStaticIrq(IT_NO_USER_IF, IT_LEVEL_USER_IF, IT_SYNCH_USER_IF, IT_ENABLE_USER_IF, IT_TRIGGER_USER_IF);

    // Interrupt 1: Glace Schedule Interrupt
    STXP70_InitStaticIrq(IT_NO_GLACE_SCHEDULE, IT_LEVEL_GLACE_SCHEDULE, IT_SYNCH_GLACE_SCHEDULE, IT_ENABLE_GLACE_SCHEDULE, IT_TRIGGER_GLACE_SCHEDULE);

    // Interrupt 2: MASTERCCI
    STXP70_InitStaticIrq(
    IT_NO_MASTER_CCI,
    IT_LEVEL_MASTER_CCI,
    IT_SYNCH_MASTER_CCI,
    IT_ENABLE_MASTER_CCI,
    IT_TRIGGER_MASTER_CCI);

    // Interrupt 3: LBE
    STXP70_InitStaticIrq(IT_NO_LBE, IT_LEVEL_LBE, IT_SYNCH_LBE, IT_ENABLE_LBE, IT_TRIGGER_LBE);

    // Interrupt 4: RE Pipe Complete
    if (Is_8500v2() || Is_9540v1() || Is_8540v1())
    {
      STXP70_InitStaticIrq(IT_NO_REPIPE, IT_LEVEL_REPIPE, IT_SYNCH_REPIPE, IT_ENABLE_REPIPE, IT_TRIGGER_REPIPE);
    }

    // Interrupt 5: SMIARX
    STXP70_InitStaticIrq(IT_NO_SMIA_RX, IT_LEVEL_SMIA_RX, IT_SYNCH_SMIA_RX, IT_ENABLE_SMIA_RX, IT_TRIGGER_SMIA_RX);

    // Interrupt 6: VIDEOPIP0
    STXP70_InitStaticIrq(
    IT_NO_VIDEO_PIPE0,
    IT_LEVEL_VIDEO_PIPE0,
    IT_SYNCH_VIDEO_PIPE0,
    IT_ENABLE_VIDEO_PIPE0,
    IT_TRIGGER_VIDEO_PIPE0);

    // Interrupt 7: VIDEOPIP0
    STXP70_InitStaticIrq(
    IT_NO_VIDEO_PIPE1,
    IT_LEVEL_VIDEO_PIPE1,
    IT_SYNCH_VIDEO_PIPE1,
    IT_ENABLE_VIDEO_PIPE1,
    IT_TRIGGER_VIDEO_PIPE1);

    // Interrupt 8: 256-bins histogram STAT0
    STXP70_InitStaticIrq(
    IT_NO_256_BINS_HISTOGRAM,
    IT_LEVEL_256_BINS_HISTOGRAM,
    IT_SYNCH_256_BINS_HISTOGRAM,
    IT_ENABLE_256_BINS_HISTOGRAM,
    IT_TRIGGER_256_BINS_HISTOGRAM);

    // Interrupt 09: 6x8 exposure STAT1
    // pictor_fs.pdf, P92, Table 8
    STXP70_InitStaticIrq(
    IT_NO_STAT1_ACC_6x8_EXPOSURE,
    IT_LEVEL_STAT1_ACC_6x8_EXPOSURE,
    IT_SYNCH_STAT1_ACC_6x8_EXPOSURE,
    IT_ENABLE_STAT1_ACC_6x8_EXPOSURE,
    IT_TRIGGER_STAT1_ACC_6x8_EXPOSURE);

    // Interrupt 10: Auto focus STAT2
    STXP70_InitStaticIrq(
    IT_NO_AUTO_FOCUS,
    IT_LEVEL_AUTO_FOCUS,
    IT_SYNCH_AUTO_FOCUS,
    IT_ENABLE_AUTO_FOCUS,
    IT_TRIGGER_AUTO_FOCUS);

    // Interrupt 11: White Balance STAT3
    STXP70_InitStaticIrq(
    IT_NO_STAT3_WHITE_BALACE,
    IT_LEVEL_STAT3_WHITE_BALACE,
    IT_SYNCH_STAT3_WHITE_BALACE,
    IT_ENABLE_STAT3_WHITE_BALACE,
    IT_TRIGGER_STAT3_WHITE_BALACE);

    // Interrupt 12: Skin tone detection STAT4
    STXP70_InitStaticIrq(
    IT_NO_SKIN_TONE_DETECTION,
    IT_LEVEL_SKIN_TONE_DETECTION,
    IT_SYNCH_SKIN_TONE_DETECTION,
    IT_ENABLE_SKIN_TONE_DETECTION,
    IT_TRIGGER_SKIN_TONE_DETECTION);

    // Interrupt 13: DMCE Pipe Complete
    if (Is_8500v2() || Is_9540v1() || Is_8540v1())
    {
      STXP70_InitStaticIrq(IT_NO_DMCEPIPE, IT_LEVEL_DMCEPIPE, IT_SYNCH_DMCEPIPE, IT_ENABLE_DMCEPIPE, IT_TRIGGER_DMCEPIPE);
    }

    // Interrupt 14: Arctic STAT6
    STXP70_InitStaticIrq(IT_NO_ARCTIC, IT_LEVEL_ARCTIC, IT_SYNCH_ARCTIC, IT_ENABLE_ARCTIC, IT_TRIGGER_ARCTIC);

    // Interrupt 15: STAT7 Acc WG Zones
    STXP70_InitStaticIrq(
    IT_NO_STAT7_ACC_WG_ZONES,
    IT_LEVEL_STAT7_ACC_WG_ZONES,
    IT_SYNCH_STAT7_ACC_WG_ZONES,
    IT_ENABLE_STAT7_ACC_WG_ZONES,
    IT_TRIGGER_STAT7_ACC_WG_ZONES);

    // Interrupt 16: STAT8 Glace (AWB)
    STXP70_InitStaticIrq(
    IT_NO_STAT8_GLACE_AWB,
    IT_LEVEL_STAT8_GLACE_AWB,
    IT_SYNCH_STAT8_GLACE_AWB,
    IT_ENABLE_STAT8_GLACE_AWB,
    IT_TRIGGER_STAT8_GLACE_AWB);

    // Interrupt 17: GPIO
    STXP70_InitStaticIrq(
    IT_NO_GPIO,
    IT_LEVEL_GPIO,
    IT_SYNCH_GPIO,
    IT_ENABLE_GPIO,
    IT_TRIGGER_GPIO);

    // Interrupt 18: Line BURSTY Elimination
    if (Is_8500v2() || Is_9540v1() || Is_8540v1())
    {
      STXP70_InitStaticIrq(IT_NO_LINE_BURSTY_ELIM, IT_LEVEL_LINE_BURSTY_ELIM, IT_SYNCH_LINE_BURSTY_ELIM, IT_ENABLE_LINE_BURSTY_ELIM, IT_TRIGGER_LINE_BURSTY_ELIM);
    }

    // Interrupt 20: Histogram Schedule Interrupt
    STXP70_InitStaticIrq(IT_NO_HISTOGRAM_SCHEDULE, IT_LEVEL_HISTOGRAM_SCHEDULE, IT_SYNCH_HISTOGRAM_SCHEDULE, IT_ENABLE_HISTOGRAM_SCHEDULE, IT_TRIGGER_HISTOGRAM_SCHEDULE);


    // Interrupt : CRM
    STXP70_InitStaticIrq(IT_NO_CRM, IT_LEVEL_CRM, IT_SYNCH_CRM, IT_ENABLE_CRM, IT_TRIGGER_CRM);

    // Interrupt 27: ERROR_EOF - SENSOR_MODE_ERROR // Umang
    STXP70_InitStaticIrq(IT_NO_ERROR_EOF, IT_LEVEL_ERROR_EOF, IT_SYNCH_ERROR_EOF, IT_ENABLE_ERROR_EOF, IT_TRIGGER_ERROR_EOF);

    // now enable the corresponding interrupt out of the ITM
    // 00
    ITM_EnableHostCommsInterrupt();

    // 01
    ITM_EnableMasterCCIInterrupt();

    // 02 reserved
    // 03
    ITM_Enable_Interrupt_LBE();

    // 04
    // V1: reserved
    // V2: RE PIPE - IN, POSTGAIN, OUTSTORE
    // Initially disabled; to be enabled when the feature is required
    if (Is_8500v2() || Is_9540v1() || Is_8540v1())
    {
      ITM_DisableInterrupt_REPIPE();
    }

    // 05
    ITM_EnableInterrupt_SMIA_RX_SOF();

    // 06
    ITM_EnableInterrupt_VIDEO_PIPE0();

    // 07
    ITM_EnableInterrupt_VIDEO_PIPE1();

    // 08
    ITM_Enable_Interrupt_STAT0_256_Bin_Histogram_AEC();

    // 09
    ITM_Enable_Interrupt_STAT1_Acc_8x6zones();

    // 10
    ITM_Enable_Interrupt_STAT2_Auto_Focus();

    // 11
    ITM_Enable_Interrupt_STAT3_WHITE_BALANCE();

    // 12
    ITM_Enable_Interrupt_STAT4_Skin_tone_detection();

    // 13
    // V1: reserved
    // V2: DMCE PIPE
    // Initially disabled; to be enabled when the feature is required
    if (Is_8500v2() || Is_9540v1() || Is_8540v1())
    {
      ITM_EnableInterrupt_DMCEPIPE();
    }

    // 15
    ITM_Enable_Interrupt_STAT7_ACC_WG_ZONES();

    // 16
    ITM_Enable_Interrupt_STAT8_GLACE_AWB();

    // 18
    // V1: reserved
    // V2: Line BURSTY Elimination
    // Initially disabled; to be enabled when the feature is required
    if (Is_8500v2() || Is_9540v1() || Is_8540v1())
    {
       ITM_Disable_Interrupt_LINE_BURSTY_ELIM();
    }

#if CRM_CSI2_CLOCK_DETECTION
    // 23
    // Enable both clock ok and loss interrupts
    ITM_Enable_Clock_Detect_LOSS_Interrupt_CRM();
#else
    ITM_Disable_Clock_Detect_LOSS_Interrupt_CRM();
#endif

    // 27 ERROR_EOF - SD_ERROR, CD_ERROR, CD_RAW_EOF, BML_EOF
    //ITM_Enable_Interrupt_CD_RAW_EOF();
    //ITM_Enable_Interrupt_CD_ERROR();
    ITM_Enable_Interrupt_SD_ERROR();
    ITM_Enable_CSIPacket_SD_ERROR();

    return;
}


/**
  \if       INCLUDE_IN_HTML_ONLY
  \fn       void DefaultHandler( void )
  \brief    This is the place holder for interrupt service routines for interrupts that
            are not being used actively for the moment.
  \return   void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
DefaultHandler(void)
{
    return;
}


/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn           void UserInterface_ISR( void )
  \brief        This is the top level interrupt service routine for the host comms.
  \return       void
  \callgraph
  \callergraph
  \ingroup      InterruptHandler
  \endif
*/
INTERRUPT void
UserInterface_ISR(void)
{
    /// \par Implementation
    /// The following operations are performed in sequence:
    /// - Invoke the host comms module interrupt handler.
    /// - Clear the host comms interrupt at ISP interrupt handler.
    /// - Raise a HOST_COMMS_OPERATION_COMPLETE event notification to the host through the Event Manager.
    // Invoke the ISR of the corresponding IP
    HostComms_ISRs();

    g_Interrupts_Count.u16_INT00_USER_INTERFACE++;

    // Clear the interrupt output of the ITM
    ITM_ClearHostCommsInterrupt();

    // Raise an event to notify completion of the host comms operation
    EventManager_HostCommsOperationComplete_Notify();

    return;
}


/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn           void MasterCCI_ISR( void )
  \brief        This is the top level interrupt service routine for the master cci.
  \return       void
  \callgraph
  \callergraph
  \ingroup      InterruptHandler
  \endif
*/
INTERRUPT void
MasterCCI_ISR(void)
{
    /// \par Implementation
    /// The following operations are performed in sequence:
    /// - Invoke the master cci module interrupt handler.
    /// - Clear the host comms interrupt at ISP interrupt handler.
    // Invoke the ISR of the corresponding IP
    MasterI2C_Isr();

    g_Interrupts_Count.u16_INT02_MASTERCCI++;

    // Clear the interrupt output of the ITM
    ITM_ClearMasterCCIInterrupt();
    return;
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void Line_Blanking_Elimination_ISR( void )
  \brief    Top level ISR
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
Line_Blanking_Elimination_ISR(void)
{
    //    MasterI2C_Isr();
    // Clear the interrupt output of the ITM
    g_Interrupts_Count.u16_INT03_Line_blaning_elimination++;

    ITM_Clear_Interrupt_LBE();

    return;
}

/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn void Smia_Rx_ISR( void )
  \brief    This is the top level interrupt service routine for the SMIA RX.
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
Smia_Rx_ISR(void)
{
    /// \par Implementation
    /// The following operations are performed in sequence:
    /// - Invoke the SMIA Rx routine
    /// - Clear the SMIA Rx interrupt at ISP interrupt handler.
    // Branch to SMIA Rx Routine

#ifdef DEBUG_PROFILER
    CpuCycleProfiler_ts* ptr_CPUProfilerData = NULL;
#endif

    g_Interrupts_Count.u16_INT05_SMIARX++;

    if (Get_ITM_ITM_SMIARX_STATUS_SMIARX_0_STATUS())
    {
#ifdef PROFILER_USING_XTI
        OstTraceInt0(TRACE_USER8, "start_xti_profiler_SMIARX_0");
#endif

#ifdef DEBUG_PROFILER
        ptr_CPUProfilerData = &g_Profile_LCO;
        START_PROFILER();
#endif

        g_Interrupts_Count.u16_INT05_SMIARX0++;
        SMIA_LC0_ISR();
        Set_ITM_ITM_SMIARX_STATUS_BCLR_SMIARX_0_STATUS_BCLR(1);

#ifdef DEBUG_PROFILER
        Profiler_Update(ptr_CPUProfilerData);
#endif

#ifdef PROFILER_USING_XTI
        OstTraceInt1(TRACE_USER8, "stop_xti_profiler_SMIARX_0 : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif
    }

    if (Get_ITM_ITM_SMIARX_STATUS_SMIARX_1_STATUS())
    {
#ifdef PROFILER_USING_XTI
        OstTraceInt0(TRACE_USER8, "start_xti_profiler_SMIARX_1");
#endif

#ifdef DEBUG_PROFILER
        ptr_CPUProfilerData = &g_Profile_ISPUpdate;
        START_PROFILER();
#endif

        g_Interrupts_Count.u16_INT05_SMIARX1++;
        SMIA_ISPUpdate_ISR();
        Set_ITM_ITM_SMIARX_STATUS_BCLR_SMIARX_1_STATUS_BCLR(1);

#ifdef DEBUG_PROFILER
        Profiler_Update(ptr_CPUProfilerData);
#endif

#ifdef PROFILER_USING_XTI
        OstTraceInt1(TRACE_USER8, "stop_xti_profiler_SMIARX_1 : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif
    }

    if (Get_ITM_ITM_SMIARX_STATUS_SMIARX_2_STATUS())
    {
#ifdef PROFILER_USING_XTI
        OstTraceInt0(TRACE_USER8, "start_xti_profiler_SMIARX_2");
        CpuCycleProfiler_ts cpu_profiler_data = DEFAULT_VALUES_CPUCYCLEPROFILER;
        ptr_CPUProfilerData = &cpu_profiler_data;
        START_PROFILER();
#endif

        g_Interrupts_Count.u16_INT05_SMIARX2++;
        // SMIA_STOP_RX_ISR();
        Set_ITM_ITM_SMIARX_STATUS_BCLR_SMIARX_2_STATUS_BCLR(1);

#ifdef PROFILER_USING_XTI
        Profiler_Update(ptr_CPUProfilerData);
        OstTraceInt1(TRACE_USER8, "stop_xti_profiler_SMIARX_2 : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif
    }

    if (Get_ITM_ITM_SMIARX_STATUS_SMIARX_3_STATUS())
    {
#ifdef PROFILER_USING_XTI
        OstTraceInt0(TRACE_USER8, "start_xti_profiler_SMIARX_3");
        CpuCycleProfiler_ts cpu_profiler_data = DEFAULT_VALUES_CPUCYCLEPROFILER;
        ptr_CPUProfilerData = &cpu_profiler_data;
        START_PROFILER();
#endif

        g_Interrupts_Count.u16_INT05_SMIARX3++;
        SMIA_RX_ISR_FRAME_END();
        Set_ITM_ITM_SMIARX_STATUS_BCLR_SMIARX_3_STATUS_BCLR(1);

#ifdef PROFILER_USING_XTI
        Profiler_Update(ptr_CPUProfilerData);
        OstTraceInt1(TRACE_USER8, "stop_xti_profiler_SMIARX_3 : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif
    }

    if (Get_ITM_ITM_SMIARX_STATUS_SMIARX_4_STATUS())
    {
#ifdef PROFILER_USING_XTI
        OstTraceInt0(TRACE_USER8, "start_xti_profiler_SMIARX_4");
        CpuCycleProfiler_ts cpu_profiler_data = DEFAULT_VALUES_CPUCYCLEPROFILER;
        ptr_CPUProfilerData = &cpu_profiler_data;
        START_PROFILER();
#endif

        g_Interrupts_Count.u16_INT05_SMIARX4++;
        Set_ITM_ITM_SMIARX_STATUS_BCLR_SMIARX_4_STATUS_BCLR(1);

#ifdef PROFILER_USING_XTI
        Profiler_Update(ptr_CPUProfilerData);
        OstTraceInt1(TRACE_USER8, "stop_xti_profiler_SMIARX_4 : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif
    }

    return;
}

/**
  \if           INCLUDE_IN_HTML_ONLY
  \fn           void Video_Pipe0_ISR( void )
  \brief        This is the top level interrupt service routine for video complete PIPE0.
  \return       void
  \callgraph
  \callergraph
  \ingroup      InterruptHandler
  \endif
*/
INTERRUPT void
Video_Pipe0_ISR(void)
{
#ifdef PROFILER_USING_XTI
    OstTraceInt0(TRACE_USER8, "start_xti_profiler_Video_Pipe0_ISR");
#endif

#ifdef DEBUG_PROFILER
    CpuCycleProfiler_ts *ptr_CPUProfilerData = NULL;
    ptr_CPUProfilerData = &g_Profile_VID0;
    START_PROFILER();
#endif

/*
    OstTraceInt1(TRACE_DEBUG, "VID0: gu8_DMCEInterruptRxed: %d", gu8_DMCEInterruptRxed);
    OstTraceInt2(TRACE_DEBUG, "VID0: Active Pipe0: %d, Pipe1: %d", SystemConfig_IsPipe0Active(), SystemConfig_IsPipe1Active());
    OstTraceInt2(TRACE_DEBUG, "VID0: Interrupt Pending Pipe0: %d, Pipe1: %d", g_PipeStatus[0].e_Flag_VideoCompleteInterruptPending, g_PipeStatus[1].e_Flag_VideoCompleteInterruptPending);
*/

    //Here it is checking whether any request is there for zoom or gamma update on Pipe0. If yes then it is disabling the CE0.
    if
    (
        (!ZoomTop_isFrameApplicationControl_ScalarParamApplied()) || (Gamma_isUpdateGammaRequestPending(0))
    )
    {

        OstTraceInt0(TRACE_FLOW, "DISABLING CE0");
        Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);

    }


    if (Flag_e_TRUE == gu8_DMCEInterruptRxed)
    {
        //DMCE interrupt already came and other pipe interrupt is either not expected or already came. So do the required processing.
        if(!SystemConfig_IsPipe1Active())
        {
                ProcessVideoCompletePipe(0);
                gu8_DMCEInterruptRxed = Flag_e_FALSE;
        }
        else
        {
            if(g_PipeStatus[1].e_Flag_VideoCompleteInterruptPending != Flag_e_FALSE)
            {
                //we will program the both pipes in vid complete of other pipe
            }
            else
            {
                ProcessVideoCompletePipe(0);
                ProcessVideoCompletePipe(1);
                gu8_DMCEInterruptRxed = Flag_e_FALSE;
            }
        }
    }

    g_PipeStatus[0].e_Flag_VideoCompleteInterruptPending = Flag_e_FALSE;
    if( Flag_e_TRUE == g_SystemConfig_Status.e_Flag_FireDummyVidComplete0 )
    {
        g_SystemConfig_Status.e_Flag_FireDummyVidComplete0 = Flag_e_FALSE;
        //ProcessVideoCompletePipe0();
    }

    // Clear the interrupt output of the ITM
    ITM_ClearInterrupt_VIDEO_PIPE0();
#ifdef DEBUG_PROFILER
    Profiler_Update(ptr_CPUProfilerData);
#endif

#ifdef PROFILER_USING_XTI
    OstTraceInt1(TRACE_USER8, "stop_xti_profiler_Video_Pipe0_ISR : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif
    return;
}


/**
  \if       INCLUDE_IN_HTML_ONLY
  \fn       void Video_Pipe1_ISR( void )
  \brief    This is the top level interrupt service routine for video complete PIPE1.
  \return   void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
Video_Pipe1_ISR(void)
{
#ifdef PROFILER_USING_XTI
    OstTraceInt0(TRACE_USER8, "start_xti_profiler_Video_Pipe1_ISR");
#endif

#ifdef DEBUG_PROFILER
    CpuCycleProfiler_ts *ptr_CPUProfilerData = NULL;
    ptr_CPUProfilerData = &g_Profile_VID1;
    START_PROFILER();
#endif

/*
    OstTraceInt1(TRACE_DEBUG, "VID1: gu8_DMCEInterruptRxed: %d", gu8_DMCEInterruptRxed);
    OstTraceInt2(TRACE_DEBUG, "VID1: Active Pipe0: %d, Pipe1: %d", SystemConfig_IsPipe0Active(), SystemConfig_IsPipe1Active());
    OstTraceInt2(TRACE_DEBUG, "VID1: Interrupt Pending Pipe0: %d, Pipe1: %d", g_PipeStatus[0].e_Flag_VideoCompleteInterruptPending, g_PipeStatus[1].e_Flag_VideoCompleteInterruptPending);
*/

     //Here it is checking whether any request is there for zoom or gamma update on Pipe1. If yes then it is disabling the CE1.
     if
     (
        (!ZoomTop_isFrameApplicationControl_ScalarParamApplied()) || (Gamma_isUpdateGammaRequestPending(1))
     )
     {
        OstTraceInt0(TRACE_FLOW, "DISABLING CE1");
        Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0);
     }

    if (Flag_e_TRUE == gu8_DMCEInterruptRxed)
    {
        //DMCE interrupt already came and other pipe interrupt is either not expected or already came. So do the required processing.
        if(!SystemConfig_IsPipe0Active())
        {
                ProcessVideoCompletePipe(1);
                gu8_DMCEInterruptRxed = Flag_e_FALSE;
        }
        else
        {
            if(g_PipeStatus[0].e_Flag_VideoCompleteInterruptPending != Flag_e_FALSE)
            {
                //we will program the both pipes in vid complete of other pipe.
            }
            else
            {
                ProcessVideoCompletePipe(0);
                ProcessVideoCompletePipe(1);
                gu8_DMCEInterruptRxed = Flag_e_FALSE;
            }
        }
    }

    g_PipeStatus[1].e_Flag_VideoCompleteInterruptPending = Flag_e_FALSE;

    if( Flag_e_TRUE == g_SystemConfig_Status.e_Flag_FireDummyVidComplete1 )
    {
       g_SystemConfig_Status.e_Flag_FireDummyVidComplete1 = Flag_e_FALSE;
//	ProcessVideoCompletePipe1();
    }

    // Clear the interrupt output of the ITM
    ITM_ClearInterrupt_VIDEO_PIPE1();
#ifdef DEBUG_PROFILER
    Profiler_Update(ptr_CPUProfilerData);
#endif

#ifdef PROFILER_USING_XTI
    OstTraceInt1(TRACE_USER8, "stop_xti_profiler_Video_Pipe1_ISR : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif

    return;
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void STAT0_256_bins_histogram_AEC_ISR( void )
  \brief    Top level ISR
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
STAT0_256_bins_histogram_AEC_ISR(void)
{
#ifdef DEBUG_PROFILER
    CpuCycleProfiler_ts *ptr_CPUProfilerData = NULL;
#endif

#ifdef PROFILER_USING_XTI
    OstTraceInt0(TRACE_USER8, "start_xti_profiler_STAT0_256_bins_histogram_AEC_ISR");
#endif

#ifdef DEBUG_PROFILER
    ptr_CPUProfilerData = &g_Profile_AEC_Statistics;
    START_PROFILER();
#endif
    OstTraceInt0(TRACE_DEBUG,"<INT> HISTO");
    Histogram_ISR();
    g_Interrupts_Count.u16_INT08_STAT0_256_bins_histogram++;

    // Clear the interrupt output of the ITM
    ITM_Clear_Interrupt_STAT0_256_Bin_Histogram_AEC();
#ifdef DEBUG_PROFILER
    Profiler_Update(ptr_CPUProfilerData);
#endif

#ifdef PROFILER_USING_XTI
    OstTraceInt1(TRACE_USER8, "stop_xti_profiler_STAT0_256_bins_histogram_AEC_ISR : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif
    return;
}

/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void Histogram_AEC_SCHEDULE_ISR( void )
  \brief    Top level Histogram ISR
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
Histogram_AEC_SCHEDULE_ISR(void)
{
#ifdef PROFILER_USING_XTI
        OstTraceInt0(TRACE_USER8, "start_xti_profiler_Histogram_Export_ISR");
        CpuCycleProfiler_ts *ptr_CPUProfilerData = NULL;
        CpuCycleProfiler_ts cpu_profiler_data = DEFAULT_VALUES_CPUCYCLEPROFILER;
        ptr_CPUProfilerData = &cpu_profiler_data;
        START_PROFILER();
#endif
    g_Interrupts_Count.u16_INT20_STAT0_256_bins_histogram_Schedule++;

    Histogram_ExportISR();

    // Clear the interrupt output of the ITM
    ITM_Clear_Interrupt_Histogram_Schedule();
#ifdef PROFILER_USING_XTI
    Profiler_Update(ptr_CPUProfilerData);
    OstTraceInt1(TRACE_USER8, "stop_xti_profiler_Histogram_Export_ISR : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif

    return;
}

/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void STAT1_Acc_6x8_Exposure( void )
  \brief    This is the top level interrupt service routine for exposure.
  \return   void
  \callgraph
  \callergraph
  \ingroup InterruptHandler
  \endif
*/
INTERRUPT void
STAT1_Acc_6x8_Exposure(void)
{
#if EXPOSURE_STATS_PROCESSOR_6x8_ZONES
#ifdef DEBUG_PROFILER
    START_PROFILER();
#endif

    ExpStats_8x6_Statistics_ISR();

    g_Interrupts_Count.u16_INT09_STAT1_6X8_Exposure++;

    gu8_ExpStatsTriggered = Flag_e_TRUE;

    // Clear the interrupt output of the ITM
    ITM_Clear_Interrupt_STAT1_Acc_8x6zones();

#ifdef DEBUG_PROFILER
    Profiler_Update(&g_Profile_AEC_Statistics);
#endif
    return;
#endif                  // EXPOSURE_STATS_PROCESSOR_6x8_ZONES
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void STAT2_Auto_Focus_ISR( void )
  \brief    Top level ISR
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
STAT2_Auto_Focus_ISR(void)
{
    //    MasterI2C_Isr();
    g_Interrupts_Count.u16_INT10_STAT2_Auto_Focus++;

#if INCLUDE_FOCUS_MODULES
        if(GET_AFSTATS_ITM_AFZONE_0_STATUS())
        {
                AFStats_SingleZoneStatsReadyISR(0);
                SET_AFSTATS_ITM_AFZONE_0_STATUS_BCLR(0x1);
        }
        if(GET_AFSTATS_ITM_AFZONE_1_STATUS())
        {
                AFStats_SingleZoneStatsReadyISR(1);
                SET_AFSTATS_ITM_AFZONE_1_STATUS_BCLR(0x1);
        }
        if(GET_AFSTATS_ITM_AFZONE_2_STATUS())
        {
                AFStats_SingleZoneStatsReadyISR(2);
                SET_AFSTATS_ITM_AFZONE_2_STATUS_BCLR(0x1);
        }
        if(GET_AFSTATS_ITM_AFZONE_3_STATUS())
        {
                AFStats_SingleZoneStatsReadyISR(3);
                SET_AFSTATS_ITM_AFZONE_3_STATUS_BCLR(0x1);
        }
        if(GET_AFSTATS_ITM_AFZONE_4_STATUS())
        {
                AFStats_SingleZoneStatsReadyISR(4);
                SET_AFSTATS_ITM_AFZONE_4_STATUS_BCLR(0x1);
        }
        if(GET_AFSTATS_ITM_AFZONE_5_STATUS())
        {
                AFStats_SingleZoneStatsReadyISR(5);
                SET_AFSTATS_ITM_AFZONE_5_STATUS_BCLR(0x1);
        }
        if(GET_AFSTATS_ITM_AFZONE_6_STATUS())
        {
                AFStats_SingleZoneStatsReadyISR(6);
                SET_AFSTATS_ITM_AFZONE_6_STATUS_BCLR(0x1);
        }
        if(GET_AFSTATS_ITM_AFZONE_7_STATUS())
        {
                AFStats_SingleZoneStatsReadyISR(7);
                SET_AFSTATS_ITM_AFZONE_7_STATUS_BCLR(0x1);
        }
        if(GET_AFSTATS_ITM_AFZONE_8_STATUS())
        {
                AFStats_SingleZoneStatsReadyISR(8);
                SET_AFSTATS_ITM_AFZONE_8_STATUS_BCLR(0x1);
        }
        if(GET_AFSTATS_ITM_AFZONE_9_STATUS())
        {
                AFStats_SingleZoneStatsReadyISR(9);
                SET_AFSTATS_ITM_AFZONE_9_STATUS_BCLR(0x1);
        }
#else
    // Clear the interrupt output of the ITM
           ITM_Clear_Interrupt_STAT2_Auto_Focus();
#endif //#if INCLUDE_FOCUS_MODULES
        return;
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void STAT3_White_balance_ISR( void )
  \brief    This is the top level interrupt service routine for Minimum weighted White balance.
  \return   void
  \callgraph
  \callergraph
  \ingroup InterruptHandler
  \endif
*/
INTERRUPT void
STAT3_White_balance_ISR(void)
{
#if WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR
    g_Interrupts_Count.u16_INT11_STAT3_MWWB_White_Balance++;

    Minimum_Weighted_Statistics_Processor_ExtractMeanStatistics();

    gu8_WbStatsTriggered = Flag_e_TRUE;

    // Clear the interrupt output of the ITM
    ITM_Clear_Interrupt_STAT3_WHITE_BALANCE();

    return;
#endif     // WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void STAT4_Skin_tone_detection_ISR( void )
  \brief    Top level ISR
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
STAT4_Skin_tone_detection_ISR(void)
{
    //    MasterI2C_Isr();
    g_Interrupts_Count.u16_INT12_STAT4_Skin_Tone_Detection++;

    // Clear the interrupt output of the ITM
    ITM_Clear_Interrupt_STAT4_Skin_tone_detection();

    return;
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void STAT7_ACC_Wg_Zones_ISR( void )
  \brief    This is the top level interrupt service routine for White balance Acc Wg Zones
  \return   void
  \callgraph
  \callergraph
  \ingroup InterruptHandler
  \endif
*/
INTERRUPT void
STAT7_ACC_Wg_Zones_ISR(void)
{
#if WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR
#ifdef DEBUG_PROFILER
    START_PROFILER();
#endif

    g_Interrupts_Count.u16_INT15_STAT7_Acc_Wg_Zones_White_Balance++;

    Weighted_Statistics_Processor_ExtractMeanStatistics();

    gu8_WbStatsTriggered = Flag_e_TRUE;

    // Clear the interrupt output of the ITM
    ITM_Clear_Interrupt_STAT7_ACC_WG_ZONES();

#ifdef DEBUG_PROFILER
    Profiler_Update(&g_Profile_AWB_Statistics);
#endif
    return;
#endif                 // WEIGHTED_STATS_PROCESSOR_INCLUDE_WEIGHTED_STATS_PROCESSOR
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void STAT8_GLACE_AWB_ISR( void )
  \brief    Top level ISR
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
STAT8_GLACE_AWB_ISR(void)
{
#ifdef PROFILER_USING_XTI
    OstTraceInt0(TRACE_USER8, "start_xti_profiler_STAT8_GLACE_AWB_ISR");
#endif

#ifdef DEBUG_PROFILER
    CpuCycleProfiler_ts *ptr_CPUProfilerData = NULL;
    ptr_CPUProfilerData = &g_Profile_AWB_Statistics;
    START_PROFILER();
#endif
    OstTraceInt0(TRACE_DEBUG,"<INT> GLACE");
    g_Interrupts_Count.u16_INT16_STAT8_Glace_AWB++;

    Glace_ISR();

    // Clear the interrupt output of the ITM
    ITM_Clear_Interrupt_STAT8_GLACE_AWB();
#ifdef DEBUG_PROFILER
    Profiler_Update(ptr_CPUProfilerData);
#endif

#ifdef PROFILER_USING_XTI
    OstTraceInt1(TRACE_USER8, "stop_xti_profiler_STAT8_GLACE_AWB_ISR : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif

    return;
}

/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void STAT8_GLACE_AWB_ISR( void )
  \brief    Top level Glace ISR
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
STAT8_GLACE_SCHEDULE_ISR(void)
{
#ifdef PROFILER_USING_XTI
    CpuCycleProfiler_ts *ptr_CPUProfilerData = NULL;
    OstTraceInt0(TRACE_USER8, "start_xti_profiler_GLACE_EXPORT_ISR");
    CpuCycleProfiler_ts cpu_profiler_data = DEFAULT_VALUES_CPUCYCLEPROFILER;
    ptr_CPUProfilerData = &cpu_profiler_data;
    START_PROFILER();
#endif
    g_Interrupts_Count.u16_INT01_STAT8_Glace_Schedule++;

    Glace_ExportISR();

    // Clear the interrupt output of the ITM
    ITM_Clear_Interrupt_STAT8_GLACE_AWB();
#ifdef PROFILER_USING_XTI
    Profiler_Update(ptr_CPUProfilerData);
    OstTraceInt1(TRACE_USER8, "stop_xti_profiler_GLACE_EXPORT_ISR : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif

    return;
}

/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void ITM_GPIO_ISR( void )
  \brief    Top level ISR
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
ITM_GPIO_ISR(void)
{
    g_Interrupts_Count.u16_INT17_GPIO++;

    if ( (Get_ITM_ITM_GPIO_EN_STATUS_GPIO_TIMER_0_EN_STATUS()) && (Get_ITM_ITM_GPIO_STATUS_GPIO_TIMER_0_STATUS()) )
    {
        // OstTraceInt0(TRACE_DEBUG,"<INT> TIMER_0 expired");

        // Disable Timer 0 interrupt in ITM
        Set_ITM_ITM_GPIO_EN_BCLR_GPIO_TIMER_0_EN_BCLR(0x1) ;

        // Disable timer 0
        GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() & 0xfe);

        // Clear status register
        Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_0_STATUS_BCLR(0x1);

        // Increment global reference counter.
        if (GLOBAL_COUNTER_MAX_VALUE == g_gpio_debug.u32_GlobalRefCounter)
        {
            g_gpio_debug.u32_GlobalRefCounter = GLOBAL_COUNTER_MIN_VALUE;
        }
        g_gpio_debug.u32_GlobalRefCounter++;

        // enable timer 0
        GM_SET_GPIO_TIMER_CTRL_WRD(GM_GET_GPIO_TIMER_CTRL() | 0x01);

        //Enable timer interrupt for timer0 in ITM block
        Set_ITM_ITM_GPIO_EN_BSET_GPIO_TIMER_0_EN_BSET(0x1);
    }

    if (Get_ITM_ITM_GPIO_EN_STATUS_GPIO_TIMER_1_EN_STATUS()&&Get_ITM_ITM_GPIO_STATUS_GPIO_TIMER_1_STATUS())
    {

        OstTraceInt0(TRACE_DEBUG,"<INT> TIMER_1 expired: LLCD Timer2 Focus");

         /// Disable Timer 1 interrupt in ITM
         Set_ITM_ITM_GPIO_EN_BCLR_GPIO_TIMER_1_EN_BCLR(0x1) ;

        // Clear the interrupt output of the ITM
        Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_1_STATUS_BCLR(0x1);

 #if INCLUDE_FOCUS_MODULES
       // Calling the FLAD Interrupt timer ISR , timer2
           LLA_InterruptTimer2_ISR();
#endif
    }


    if (Get_ITM_ITM_GPIO_EN_STATUS_GPIO_TIMER_2_EN_STATUS()&&Get_ITM_ITM_GPIO_STATUS_GPIO_TIMER_2_STATUS())
    {

         OstTraceInt0(TRACE_DEBUG,"<INT> TIMER_2 expired: LLCD Timer1");
         /// Disable Timer 2 interrupt in ITM
         Set_ITM_ITM_GPIO_EN_BCLR_GPIO_TIMER_2_EN_BCLR(0x1) ;

        Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_2_STATUS_BCLR(0x1);

         ///timer2 from GPIO block has been used to implement interrupt timer
         LLA_InterruptTimer_ISR();
    }


    if (Get_ITM_ITM_GPIO_EN_STATUS_GPIO_TIMER_3_EN_STATUS()&&Get_ITM_ITM_GPIO_STATUS_GPIO_TIMER_3_STATUS())
    {
        OstTraceInt0(TRACE_DEBUG,"<INT> TIMER_3 expired");

         /// Disable Timer 3 interrupt in ITM
         Set_ITM_ITM_GPIO_EN_BCLR_GPIO_TIMER_3_EN_BCLR(0x1) ;

         /// Clear status register
         Set_ITM_ITM_GPIO_STATUS_BCLR_GPIO_TIMER_3_STATUS_BCLR(0x1);
    }

    return;
}

/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void CRM_ISR(void)
  \brief    Top level ISR for CRM
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
ClockResetManager_ISR(void)
{
    OstTraceInt0(TRACE_DEBUG,"<INT> CRM");

    // Invoke the module interrupt service routine
    CRM_ISR();

    g_Interrupts_Count.u16_INT23_CRM++;

    // Clear the interrupt output of the ITM
//    ITM_Clear_Interrupt_CRM();

    // g_Interrupts_Count.u16_INT23_CRM++;
    return;
}

/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void ITM_ERROR_EOF_ISR(void)
  \brief    Top level ISR for ERROR and EOF
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
ITM_ERROR_EOF_ISR(void)
{
    // Invoke the module interrupt service routine
    OstTraceInt0(TRACE_DEBUG,"<INT> ERROR_EOF");

    g_Interrupts_Count.u16_INT27_ERROR_EOF++;

    ErrorEOF_ISR();

    return;
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn void DMCE_ISR(void)
  \brief    Top level ISR for DMCE
  \return void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
DMCE_ISR(void)
{
    if (Get_ITM_ITM_DMCEPIPE_STATUS() & 0x1) //DMCE_IN_STATUS_BCLR
    {
        //clear the pending bit.
        Set_ITM_ITM_DMCEPIPE_STATUS_BCLR_DMCE_IN_STATUS_BCLR(1);
    }
    if (Get_ITM_ITM_DMCEPIPE_STATUS() & 0x2) //DMCE_OUT_STATUS_BCLR
    {
        OstTraceInt1(TRACE_DEBUG, "DMCE: gu8_DMCEInterruptRxed: %d", gu8_DMCEInterruptRxed);
        //OstTraceInt2(TRACE_DEBUG, "DMCE: Active Pipe0: %d, Pipe1: %d", SystemConfig_IsPipe0Active(), SystemConfig_IsPipe1Active());
        //OstTraceInt2(TRACE_DEBUG, "DMCE: Interrupt Pending Pipe0: %d, Pipe1: %d", g_PipeStatus[0].e_Flag_VideoCompleteInterruptPending, g_PipeStatus[1].e_Flag_VideoCompleteInterruptPending);

        if 
        (
            (SystemConfig_IsPipe0Active() && g_PipeStatus[0].e_Flag_VideoCompleteInterruptPending != Flag_e_FALSE) // vid 0 expected
            ||
            (SystemConfig_IsPipe1Active() && g_PipeStatus[1].e_Flag_VideoCompleteInterruptPending != Flag_e_FALSE) // vid 1 expected
        )
        {
            gu8_DMCEInterruptRxed = Flag_e_TRUE;
        }
        else
        {
            //Vid complete of the enabled pipes has already come. Program the enabled pipe in that case.
            gu8_DMCEInterruptRxed = Flag_e_FALSE;
            if(SystemConfig_IsPipe0Active())
            {
                ProcessVideoCompletePipe(0);
            }
            if(SystemConfig_IsPipe1Active())
            {
                ProcessVideoCompletePipe(1);
            }
         }   

         //clear the pending bit.
         Set_ITM_ITM_DMCEPIPE_STATUS_BCLR_DMCE_OUT_STATUS_BCLR(1);
    }
    if (Get_ITM_ITM_DMCEPIPE_STATUS() & 0x4) //RGB_STORE_STATUS_BCLR
    {
        //clear the pending bit.
        Set_ITM_ITM_DMCEPIPE_STATUS_BCLR_RGB_STORE_STATUS_BCLR(1);
    }
}


void ProcessVideoCompletePipe(uint8_t pipe_no)
{

#ifdef DEBUG_PROFILER
    START_PROFILER();
#endif

    if(0 == pipe_no)
    {
        VideoComplete_Pipe0();
        g_Interrupts_Count.u16_INT06_VIDEOPIPE0++;

        if(Is_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_ENABLE_mux2to1_enable_DISABLE())
        {
            Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);
            OstTraceInt0(TRACE_FLOW, "ENABLING CE0");
        }

#if DEBUG_PROFILER
    Profiler_Update(&g_Profile_VID0);
#endif

    }

    else if (1 == pipe_no)
    {
        VideoComplete_Pipe1();
        g_Interrupts_Count.u16_INT07_VIDEOPIPE1++;

        if(Is_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE_mux2to1_enable_DISABLE())
        {
            Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE(mux2to1_enable_ENABLE, mux2to1_soft_reset_B_0x0);
            OstTraceInt0(TRACE_FLOW, "ENABLING CE1");
        }

#if DEBUG_PROFILER
    Profiler_Update(&g_Profile_VID1);
#endif

    }
}


/**
  \if INCLUDE_IN_HTML_ONLY
  \fn   void NMIHandler( void )
  \brief    This is the top level handler for the NMI. It is not being used at the moment.
  \return   void
  \callgraph
  \callergraph
  \ingroup  InterruptHandler
  \endif
*/
INTERRUPT void
NMIHandler(void)
{
    return;
}


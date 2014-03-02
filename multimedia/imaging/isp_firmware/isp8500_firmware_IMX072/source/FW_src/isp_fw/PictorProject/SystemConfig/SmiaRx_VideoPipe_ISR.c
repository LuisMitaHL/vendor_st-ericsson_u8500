/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file  SmiaRx_VideoPipe_ISR.c
 \brief Contains the interrupt service handlers for the video complete
        and SMIA Rx interrupts.

 \ingroup SystemConfig
 \endif
*/
#include "SystemConfig.h"
#include "FrameDimension_op_interface.h"
#include "HostInterface.h"
#include "FrameRate_op_interface.h"
#include "Exposure_OPInterface.h"
#include "Stream.h"
#include "videotiming_op_interface.h"
#include "GenericFunctions.h"
#include "Duster_op_interface.h"
#include "Flash.h"
#include "Gamma_op_interface.h"
#if INCLUDE_FOCUS_MODULES
#include "AFStats_IPInterface.h"
#endif

#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_SystemConfig_SmiaRx_VideoPipe_ISRTraces.h"
#endif
volatile UpdateStatus_te    g_SensorSettingStatus = UpdateStatus_e_Idle;

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void SMIA_LC0_ISR( void )
 \brief     The interrupt service handler for SMIA Rx interrupt used as a start of frame indicator.
            It is invoked from the top level interrupt service routine.
 \param     None
 \return    None
 \ingroup   SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
SMIA_LC0_ISR(void)
{
    /* If code should be executed only when Rx is steaming. */
    if (Stream_IsISPInputStreaming())
    {
        /* To avoid config set I2C writes when Rx interrupt comes in between
           config_set() API call.
        */
        if (!g_Config_set_lock)
        {
            Set_SOF_Arrived(Flag_e_TRUE);
        }

        g_RxFrameCounter = Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter();

        OstTraceInt1(TRACE_DEBUG, "<INT> FSC: %d ", g_RxFrameCounter);

        /* For Debugging AF (include "GPIOManager_OpInterface.h" for compilation) */
        //OstTraceInt2(TRACE_USER1, "[AF Optimization]: Value of Ref Counter = %u at SOF No. %u", g_gpio_debug.u32_GlobalRefCounter, Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter());

        /* We need to pass Frame start notification to low level driver, LC0
           interrupt is used for this purpose.
        */
        LLA_Abstraction_SendSignal_FSC();
        e_Flag_FSC_Sent = Flag_e_TRUE;

        /* Setup Video complete pending. */
        if (SystemConfig_IsPipe0Active())
        {
            g_PipeStatus[0].u8_FramesStreamedInPipeLastRun++;
            g_PipeStatus[0].e_Flag_VideoCompleteInterruptPending = Flag_e_TRUE;
        }

        if (SystemConfig_IsPipe1Active())
        {
            g_PipeStatus[1].u8_FramesStreamedInPipeLastRun++;
            g_PipeStatus[1].e_Flag_VideoCompleteInterruptPending = Flag_e_TRUE;
        }

#if INCLUDE_FOCUS_MODULES
        /* [FIDO Ticket 325360]: Setup AFStats irrespective of presence of lens
           actuator.
        */
        FocusControl_SOFISR();
#endif
    }
    else
    {
        /* Clean the isp_pipe's dataflow related variables */
        g_PipeStatus[0].u8_FramesStreamedInPipeLastRun = 0;
        g_PipeStatus[1].u8_FramesStreamedInPipeLastRun = 0;

        g_PipeStatus[0].e_Flag_VideoCompleteInterruptPending = Flag_e_FALSE;
        g_PipeStatus[1].e_Flag_VideoCompleteInterruptPending = Flag_e_FALSE;

        g_RxFrameCounter = 0;
    }

    return;
}

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void SMIA_LC1_ISR( void )
 \brief     The routine services line count 1 interrupt of the SMIA Rx.
            It is used as a reference for computing the damper values for
            the complete ISP.
 \param     None
 \return    None
 \ingroup   SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
SMIA_ISPUpdate_ISR(void)
{
    return;
}

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void SMIA_STOP_RX_ISR( void )
  \brief    ISR for stopping RX
  \return void
  \callgraph
  \callergraph
  \ingroup  SystemConfig
  \endif
*/
void
SMIA_STOP_RX_ISR(void)
{
    if (g_Stream_InputStatus.e_Flag_RxStopRequested == Flag_e_TRUE)
    {
        // write stop_rx on to the RX_CTRL hw register
        Set_ISP_SMIARX_ISP_SMIARX_CTRL(0, 1, 0);    // rx_start,rx_stop,rx_abort
        g_Stream_InputStatus.e_Flag_RxStopRequested = Flag_e_FALSE;
    }


    return;
}

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn void SMIA_RX_ISR_FRAME_END( void )
  \brief    ISR commit Input Pipe
  \return void
  \callgraph
  \callergraph
  \ingroup
  \endif
*/
void
SMIA_RX_ISR_FRAME_END(void)
{
    /*
        <BG>: In case of BMS no pipe is active & hence FEC will not be reported back to LLD
             from vid complete isr. As a temporary fix this is done here; But once capture
             with mechanical shutter is implemented this needs to be changed since LLD opens
             up the lens once it gets back the FEC notification and if this notification is
             not accurate then there might be a case when shutter gets opened when readout is
             still on.
    */
    /* We should check for Rx status rather than sensor status. */
    if ((Flag_e_TRUE == e_Flag_FSC_Sent) &&
        (Stream_IsISPInputStreaming()))
    {
        OstTraceInt0(TRACE_DEBUG, "<INT> FEC");

        LLA_Abstraction_SendSignal_FEC();
        e_Flag_FSC_Sent = Flag_e_FALSE;
    }

    if ((Stream_IsISPInputStreaming()) &&
        (g_SystemConfig_Status.e_RxTestPattern_Status != g_SystemSetup.e_RxTestPattern))
    {
        /* Update RxPattern and Cursor Position values if modified by HOST
           during RunTime.
        */
        SystemConfig_UpdateRxPattern();
    }

    /* Update the runtime data path parameters. */
    SystemConfig_RunTimeDataPathUpdate();

    /* Since the data path output are aligned to frame boundaries, it is safe
       to enable them at this time to ease pressure towards the end of the frame
    */
    SystemConfig_RunTimeDataPathCommit();

    return;
}

/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void VideoComplete_Pipe0(void)
 \brief     The interrupt service handler video complete for pipe0. It is invoked from
            the top level interrupt service routine.
 \param     None
 \return    None
 \ingroup   SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
VideoComplete_Pipe0(void)
{

    //checking for zoom whether any request is pending
    if (!Zoom_IsFDMRequestPending())
    {
        PipeAlgorithm_CommitPipe0();
    }

    //Check if new Gamma settings available
    Update_Gamma_Pipe(0);

    if (Flag_e_FALSE == g_SystemConfig_Status.e_Flag_FireDummyVidComplete0)
    {
        if (Stream_IsISPInputStreaming())
        {
            g_PipeStatus[0].u8_FramesStreamedOutOfPipeLastRun++;
        }
        else
        {
            g_PipeStatus[0].u8_FramesStreamedOutOfPipeLastRun = 0;
        }
    }

    OstTraceInt0(TRACE_DEBUG, "<INT> VID0 ");
    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void VideoComplete_Pipe1(void)
 \brief     The interrupt service handler video complete for pipe1. It is invoked from
            the top level interrupt service routine.
 \param     None
 \return    None
 \ingroup   SystemConfig
 \callgraph
 \callergraph
 \endif
*/
void
VideoComplete_Pipe1(void)
{

    if (!Zoom_IsFDMRequestPending())
    {
        PipeAlgorithm_CommitPipe1();
    }

    //Check if new Gamma settings available
    Update_Gamma_Pipe(1);

    if (Flag_e_FALSE == g_SystemConfig_Status.e_Flag_FireDummyVidComplete1)
    {
        if (Stream_IsISPInputStreaming())
        {
            g_PipeStatus[1].u8_FramesStreamedOutOfPipeLastRun++;
        }
        else
        {
            g_PipeStatus[1].u8_FramesStreamedOutOfPipeLastRun = 0;
        }
    }

    OstTraceInt0(TRACE_DEBUG, "<INT> VID1 ");

    return;
}


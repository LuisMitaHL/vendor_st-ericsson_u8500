/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  Stream

 \detail    The main stream manager which handles starting and stopping of the
            sensor and the receiver. It is also responsible for performing
            clock switches at appropriate time while starting and stopping
            the sensor.
*/

/**
 \file      Stream.c
 \brief     The main stream manager file. Contains functions to start receiver,
            stop sensor, stop receiver and stop sensor
 \ingroup   Stream
*/
#include "Stream.h"
#include "PictorhwReg.h"
#include "EventManager.h"
#include "MissingModules.h"
#include "smia_sensor_memorymap_defs.h"
#include "SystemConfig.h"
#include "videotiming_op_interface.h"
#include "CRM.h"
#include "GPIOManager_OpInterface.h"
#include "SensorManager.h"
#include "STXP70_OPInterface.h"
#include "InterruptManager.h"
#include "Platform.h"
#include "IPPhwReg.h"
#include "Flash.h"
#include "PictorhwReg_v1.h"
#include "ITM.h"
#include "Zoom_OPInterface.h"
#include "ErrorHandler.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_Stream_StreamTraces.h"
#endif
Stream_InputControl_ts          g_Stream_InputControl = { StreamCommand_e_Stop, StreamCommand_e_Stop }; // e_StreamCommand_Sensor, e_StreamCommand_Rx
volatile Stream_InputStatus_ts  g_Stream_InputStatus =
{
    StreamResourceStatus_e_Stopped,
    StreamResourceStatus_e_Stopped,
    Flag_e_FALSE
};  // e_StreamResourceStatus_Sensor, e_StreamResourceStatus_Rx, e_Flag_RxStopRequested
volatile Flag_te                e_Flag_FSC_Sent = Flag_e_FALSE;

static void StateMachine
    (uint8_t e_StreamResource, uint8_t e_StreamCommand, uint8_t * pe_StreamResourceStatus) TO_EXT_PRGM_MEM;
void    StartResource (uint8_t e_StreamResource);
void StartRx ( void ) TO_EXT_DDR_PRGM_MEM;
void    StartSensor (void);
void StopResource (uint8_t e_StreamResource) TO_EXT_DDR_PRGM_MEM;
void    StopSensor (void);
void    WaitForResourceToStart (uint8_t e_StreamResource);
void    WaitForResourceToStop (uint8_t e_StreamResource);

// Reference: SMIA_Functional_specification_1.0.pdf, Page67, Table 27 2-byte Generic Frame Format Descriptor
#define EMBEDDED_DATA_CODE      (0x1)
#define VISIBLE_PIXEL_DATA_CODE (0x5)

/**
 \fn        void Stream_StateMachine( void )
 \brief     Top level Stream state machine which manages command to start and stop the sensor
 \param     None
 \return    None
 \ingroup   Stream
 \callgraph
 \callergraph
*/
void
Stream_StateMachine(void)
{
    // possible states of Sensor and Rx and their respective constraints are as follows:
    // Sensor       Rx          Constraint
    // Stopped      Stopped     (sensor start and Rx start possible, if both sensor and Rx are started, sensor start has priority)
    // Stopped      Running     (only Rx stop allowed)
    // Running      Stopped     (only sensor stop and Rx start possible but not together)
    // Running      Running     (Sensor stop and Rx stop allowed, if both are stopped, Rx stop has priority, if sensor is stopped, Rx must stop)
    switch (g_Stream_InputStatus.e_StreamResourceStatus_Sensor)
    {
        case StreamResourceStatus_e_Stopped:
            switch (g_Stream_InputStatus.e_StreamResourceStatus_Rx)
            {
                case StreamResourceStatus_e_Stopped:
                    // both sensor and Rx stopped, if sensor has to be started, it has priority
                    // invoke the state machine to start the sensor if required
                    StateMachine(
                    StreamResource_e_Sensor,
                    g_Stream_InputControl.e_StreamCommand_Sensor,
                    (uint8_t *) (&g_Stream_InputStatus.e_StreamResourceStatus_Sensor));

                    // invoke the state machine to start the Rx if required
                    StateMachine(
                    StreamResource_e_Rx,
                    g_Stream_InputControl.e_StreamCommand_Rx,
                    (uint8_t *) (&g_Stream_InputStatus.e_StreamResourceStatus_Rx));

                    break;

                case StreamResourceStatus_e_Running:
                    // sensor is stopped and Rx is running
                    // only an Rx stop is possible
                    // invoke the state machine to start the Rx if required
                    StateMachine(
                    StreamResource_e_Rx,
                    g_Stream_InputControl.e_StreamCommand_Rx,
                    (uint8_t *) (&g_Stream_InputStatus.e_StreamResourceStatus_Rx));

                    // by not considering the sensor command, we make the state machine immune to any potential sensor start command
                    break;
            }


            break;

        case StreamResourceStatus_e_Running:
            // the sensor is running, there can be no case when the sensor will stop and we want to run the rx
            // such a case must be ignored
            if
            (
                (StreamCommand_e_Stop == g_Stream_InputControl.e_StreamCommand_Sensor)
            &&  (StreamCommand_e_Start == g_Stream_InputControl.e_StreamCommand_Rx)
            )
            {
                break;
            }


            switch (g_Stream_InputStatus.e_StreamResourceStatus_Rx)
            {
                case StreamResourceStatus_e_Stopped:
                    // invoke the state machine to stop the sensor
                    StateMachine(
                    StreamResource_e_Sensor,
                    g_Stream_InputControl.e_StreamCommand_Sensor,
                    (uint8_t *) (&g_Stream_InputStatus.e_StreamResourceStatus_Sensor));

                    // invoke the state machine to start the Rx
                    StateMachine(
                    StreamResource_e_Rx,
                    g_Stream_InputControl.e_StreamCommand_Rx,
                    (uint8_t *) (&g_Stream_InputStatus.e_StreamResourceStatus_Rx));

                    break;

                case StreamResourceStatus_e_Running:
                    // invoke the state machine to stop the Rx first if required
                    StateMachine(
                    StreamResource_e_Rx,
                    g_Stream_InputControl.e_StreamCommand_Rx,
                    (uint8_t *) (&g_Stream_InputStatus.e_StreamResourceStatus_Rx));

                    // invoke the state machine to stop the sensor now if required
                    StateMachine(
                    StreamResource_e_Sensor,
                    g_Stream_InputControl.e_StreamCommand_Sensor,
                    (uint8_t *)(&g_Stream_InputStatus.e_StreamResourceStatus_Sensor));

                    break;
            }


            break;
    }


    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void StateMachine( uint8_t e_StreamResource, uint8_t e_StreamCommand, uint8_t *pe_StreamResourceStatus )
 \brief     Low level common state machine to handle the start and stop to the sensor and Rx
 \param     e_StreamResource        :   The resource for which the machine has been invoked, possible values are StreamResource_e_Sensor and StreamResource_e_Rx
 \param     e_StreamCommand         :   The command for the e_StreamResource
 \param     pe_StreamResourceStatus :   Pointer to the status of the resource to allow update of the status
 \return    None
 \ingroup   Stream
 \callgraph
 \callergraph
 \endif
*/
static void
StateMachine(
uint8_t e_StreamResource,
uint8_t e_StreamCommand,
uint8_t *pe_StreamResourceStatus)
{
    volatile uint8_t    e_StreamResourceStatus;

    e_StreamResourceStatus = *pe_StreamResourceStatus;

    switch (e_StreamResourceStatus)
    {
        case StreamResourceStatus_e_Stopped:
            if (StreamCommand_e_Start == e_StreamCommand)
            {
                // command to start the resource
                // update the status
                e_StreamResourceStatus = StreamResourceStatus_e_Starting;

                // update the resource status as well
                *pe_StreamResourceStatus = StreamResourceStatus_e_Starting;

                // intentionally drop down to the next level
            }
            else
            {
                // nothing to do, break out
                break;
            }


        case StreamResourceStatus_e_Starting:
            // start the resource
            StartResource(e_StreamResource);

            // update the status
            e_StreamResourceStatus = StreamResourceStatus_e_WaitingToStart;

            // update the resource status as well
            *pe_StreamResourceStatus = StreamResourceStatus_e_WaitingToStart;

        // intentionally drop down to the next state
        case StreamResourceStatus_e_WaitingToStart:
            // wait for the resource to start
            // this is a blocking call and will
            // return only once the wait is over
            WaitForResourceToStart(e_StreamResource);

            if (StreamResource_e_Rx == e_StreamResource)
            {
                // Raise an event notification to signal that the ISP is streaming
                EventManager_ISPStreaming_Notify();

                //Once ISP Streaming Notification has raised ,we can read pixel order value
                // [NON_ISL_SUPPORT]: Non SMIA Sensor case
                if(SENSOR_VERSION_NON_SMIA == g_SensorInformation.u8_smia_version)
                {
                    g_SystemConfig_Status.e_PixelOrder = Get_ISP_SMIARX_ISP_SMIARX_PIXEL_ORDER_pixel_order_ovr();
                }
                else
                {
                    g_SystemConfig_Status.e_PixelOrder = Get_ISP_SMIARX_ISP_SMIARX_PIXEL_ORDER_pixel_order();
                }
            }
            else
            {
                // Raise an event notification to signal that the sensor is streaming
                EventManager_SensorStartStreaming_Notify();
            }


            // we have come out of the function
            // this means that the resource has started
            // update the status
            e_StreamResourceStatus = StreamResourceStatus_e_Running;

            // update the resource status as well
            *pe_StreamResourceStatus = StreamResourceStatus_e_Running;

        case StreamResourceStatus_e_Running:
            if (StreamCommand_e_Stop == e_StreamCommand)
            {
                // command to stop the resource
                // update the status
                e_StreamResourceStatus = StreamResourceStatus_e_Stopping;

                // update the resource status as well
                *pe_StreamResourceStatus = StreamResourceStatus_e_Stopping;

                // intentionally drop down to the next level
            }
            else
            {
                // nothing to do, break out
                break;
            }


        case StreamResourceStatus_e_Stopping:
            // stop the resource
            StopResource(e_StreamResource);

            // update the status
            e_StreamResourceStatus = StreamResourceStatus_e_WaitingToStop;

            // update the resource status as well
            *pe_StreamResourceStatus = StreamResourceStatus_e_WaitingToStop;

        // intentionally drop down to the next state
        case StreamResourceStatus_e_WaitingToStop:
            // wait for the resource to stop
            // this is a blocking call and will
            // return only once the wait is over
            WaitForResourceToStop(e_StreamResource);

            // we have come out of the function
            // this means that the resource has stopped
            // update the status
            e_StreamResourceStatus = StreamResourceStatus_e_Stopped;

            // update the resource status as well
            *pe_StreamResourceStatus = StreamResourceStatus_e_Stopped;

            break;
    }


    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void StartResource(uint8_t e_StreamResource)
 \brief     Low level function to handle starting of a resource
 \param     e_StreamResource    :   The resource which is to be started, possible values are StreamResource_e_Sensor and StreamResource_e_Rx
 \return    None
 \ingroup   Stream
 \callgraph
 \callergraph
 \endif
*/
void
StartResource(
uint8_t e_StreamResource)
{
    if (StreamResource_e_Rx == e_StreamResource)
    {
        // In case of CSI2, Rx is started before sensor to enable grabbing of the first frame.
        // This is done in the context of StartResource for sensor.
        // But the stream state machine still follows the following order, start sensor --> start Rx
        // As a result in case of normal start streming sequence(RUN command from user), Rx is already
        // started when the code hits this point. But in case of RUN command after ISP_STOP command,
        // sensor is not restarted, only Rx is started. This check is added to start Rx in such condition.
        if(SystemConfig_IsInputInterfaceCCP() || (!Is_ISP_SMIARX_ISP_SMIARX_STATUS_rx_state_GEN_RUNNING()))
        {
            OstTraceInt0(TRACE_FLOW, "SystemConfig_IsInputInterfaceCCP : StartRx()");
            StartRx();
        }
        else if(g_Stream_InputStatus.e_StreamResourceStatus_Sensor == StreamResourceStatus_e_Running)
        {
            StartRx();
        }
    }
    else if (StreamResource_e_Sensor == e_StreamResource)
    {
        if (SystemConfig_IsInputInterfaceCCP())
        {
            // Disable Interrupts in CRM
            ITM_Disable_Clock_Detect_LOSS_Interrupt_CRM();

            // For CCP sensor, after starting sensor, wait for some time so that the clocks
            // available from the sensor are good.
            StartSensor();

            //GPIOManager_Toggle_GPIO_Pin();
            GPIOManager_Delay_us(g_SystemSetup.f_SensorStartDelay_us);

            if (SystemConfig_IPPSetupRequired())
            {
                // enable IPP_SD_STATIC_CCP_EN
                Set_IPP_IPP_SD_STATIC_CCP_EN(STATIC_SD_CCP_EN_B_0x1);

                // enable the IPP_DPHY_TOP_IF
                Set_IPP_IPP_DPHY_TOP_IF_EN(1);

                // enable IPP internal CCP clock
                Set_IPP_IPP_INTERNAL_EN_CLK_CCP(INTERNAL_EN_CLK_CCP_B_0x1);
            }


            // switch to real sensor clocks as sensor has started & clocks are good.
            CRM_SwitchToRealSensorClocks();

            // set sensor clock available in the Clock Manager.
            CRM_SetSensorClockAvailable();
        }
        else
        {
#if CRM_CSI2_CLOCK_DETECTION
            CRM_PreRunUpdate();
            g_CRM_Status.e_Flag_StartEnabled = Flag_e_TRUE;
            // Rx start can be done before starting Sensor for CSI2 sensor
            // Reference : section 7.2 of "x500 start and stop sequence" document
            OstTraceInt0(TRACE_FLOW, "calling StartRx()");
            StartRx();
#endif

            // For CSI sensor, after starting sensor, no need to wait for
            // before enabling the IPP_DPHY
            if (SystemConfig_IPPSetupRequired())
            {
                // enable the IPP_DPHY_TOP_IF
                Set_IPP_IPP_DPHY_TOP_IF_EN(1);
            }


            StartSensor();

#if CRM_CSI2_CLOCK_DETECTION
         OstTraceInt0(TRACE_FLOW, "Waiting for clocks to be available");
         while(CRM_GetIsSensorClockUnavailable());
#else
            //GPIOManager_Toggle_GPIO_Pin();
            GPIOManager_Delay_us(g_SystemSetup.f_SensorStartDelay_us);

            // switch to real sensor clocks as sensor has started & clocks are good.
            CRM_SwitchToRealSensorClocks();

            // set sensor clock available in the Clock Manager.
            CRM_SetSensorClockAvailable();
#endif
        }
    }


    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void WaitForResourceToStart(uint8_t e_StreamResource)
 \brief     Low level function to wait on the resource to start
 \param     e_StreamResource    :   The resource which is to be started, possible values are StreamResource_e_Sensor and StreamResource_e_Rx
 \return    None
 \ingroup   Stream
 \callgraph
 \callergraph
 \endif
*/
void
WaitForResourceToStart(
uint8_t e_StreamResource)
{
    if (StreamResource_e_Rx == e_StreamResource)
    {
        // poll on the Rx to transition to running state
        while (!Is_ISP_SMIARX_ISP_SMIARX_STATUS_rx_state_GEN_RUNNING())
            ;
    }
    else if (StreamResource_e_Sensor == e_StreamResource)
    {
        // poll on availability of clocks from the sensor
        while (CRM_GetIsSensorClockUnavailable())
            ;

        // TODO: clocks are now enable the input clock pads
    }


    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void StopResource(uint8_t e_StreamResource)
 \brief     Low level function to stop a resource
 \param     e_StreamResource    :   The resource which is to be stopped, possible values are StreamResource_e_Sensor and StreamResource_e_Rx
 \return    None
 \ingroup   Stream
 \callgraph
 \callergraph
 \endif
*/
void
StopResource(
uint8_t e_StreamResource)
{
    if (StreamResource_e_Rx == e_StreamResource)
    {
        if
        (
            (
                (Flag_e_TRUE == g_SystemSetup.e_Flag_abortRx_OnStop)
            ||  (Flag_e_TRUE == g_ErrorHandler.e_Flag_Error_Abort)
            )
        &&  (!Zoom_IsFDMRequestPending())
        )
        {
            OstTraceInt0(TRACE_FLOW, "Aborting smia-rx");

            // write abort_rx on to the RX_CTRL hw register
            Set_ISP_SMIARX_ISP_SMIARX_CTRL(0, 0, 1);    // rx_start,rx_stop,rx_abort

            // reset ISP
            ResetISPBlocks();

            if (Flag_e_TRUE == e_Flag_FSC_Sent)
            {
                OstTraceInt0(TRACE_DEBUG, "<INT> FEC");
                LLA_Abstraction_SendSignal_FEC();
                e_Flag_FSC_Sent = Flag_e_FALSE;
            }


            // reset flag to memorize that smia-rx has been aborted. stop is not needed now.
            g_Stream_InputStatus.e_Flag_RxStopRequested = Flag_e_FALSE;
        }
        else
        {
            Set_ISP_SMIARX_ISP_SMIARX_CTRL(0, 1, 0);    // rx_start,rx_stop,rx_abort

            // set the StopRequestedFlag to True.\n
            // the stop will be written on to HW in the context of interrupt.
            g_Stream_InputStatus.e_Flag_RxStopRequested = Flag_e_FALSE;
        }
    }
    else if (StreamResource_e_Sensor == e_StreamResource)
    {
        // procedure to stop the sensor:
        // 0) Launch timer (1 frame delay + configurable margin)
        // 1) switch to emulated sensor clock
        // 2) Disables DPHY
        // - IPP_INTERNAL_EN_CLK_CCP = 0 (only if CCP, not CSI2)
        // - wait ofr 15us (pictor_fs.pdf- page 5)
        // - (DPHY_TOP_IF_EN=0, actually stops DPHY clock)
        // 3) Stop sensor (through i2c command)
        // 4) After timer expires, free to restart the sensor if needed
        // since we have a blocking timeout, we cannot launch the timer first, we launch the timer AFTER stop sensor command

        /* Reset the Cropper of both pipes i.e. CE0 and CE1. The values to be used by cropper should be recalculated in BMS case and
        upon restarting any of the pipe. So reseting the values here.*/
        // TODO: AG : remove workaround to make BMS work
        Set_ISP_CE1_CROP_ISP_CE1_CROP_H_START_crop_h_start(0x0);
        Set_ISP_CE1_CROP_ISP_CE1_CROP_V_START_crop_v_start(0x0);

        Set_ISP_CE1_CROP_ISP_CE1_CROP_H_SIZE_crop_h_size(0x0);
        Set_ISP_CE1_CROP_ISP_CE1_CROP_V_SIZE_crop_v_size(0x0);

        Set_ISP_CE0_CROP_ISP_CE0_CROP_H_START_crop_h_start(0x0);
        Set_ISP_CE0_CROP_ISP_CE0_CROP_V_START_crop_v_start(0x0);

        Set_ISP_CE0_CROP_ISP_CE0_CROP_H_SIZE_crop_h_size(0x0);
        Set_ISP_CE0_CROP_ISP_CE0_CROP_V_SIZE_crop_v_size(0x0);

        if (SystemConfig_IPPSetupRequired())
        {
            if (SystemConfig_IsInputInterfaceCCP())
            {

            /**
                CCP stop sequence is documented in x500 raw-bayer camera start/stop sequences
                Refer 8.4 8500V2 CCP2 Sensor STOP sequence – last frame kept, Page 23-24
                Figure 7
            */
                // before stopping sensor, switch to emulated clock
                CRM_SwitchToEmulatedSensorClocks();

                Set_IPP_IPP_INTERNAL_EN_CLK_CCP(0);

                // wait for 15us
                GPIOManager_Delay_us(15);

                // Disable DPHY
                Set_IPP_IPP_DPHY_TOP_IF_EN(0);


                // Reset ISP Front end, IPP_SD_RESET = 0x1 << 8
                // NOTE1: Any IPP register access should be on 16 bit boundary from xP70, Individual access of bit will lead to undefined behaviour.
                // This problem was faced on actual customer h/w.
                Set_IPP_IPP_SD_RESET(0,0,1);


                StopSensor();

                CRM_SetSensorClockUnavailable();

                //<TODO>: Declare a macro in FrameDimension module.
                // workaround implemented in 8815. Take little more buffer than the frame length.
                GPIOManager_Delay_us((1.2 * g_FrameDimensionStatus.f_VTFrameLength_us));


                // Reset ISP Front end, IPP_SD_RESET = 0x1 << 8
                // NOTE1: Any IPP register access should be on 16 bit boundary from xP70, Individual access of bit will lead to undefined behaviour.
                // This problem was faced on actual customer h/w.
                // NOTE2: Reset of IPP_SD_RESET is suggested again in document. Might not be needed strictly. Safe to do it again.
                Set_IPP_IPP_SD_RESET(0,0,1);
            }
            else    // csi
            {
#if CRM_CSI2_CLOCK_DETECTION
                g_CRM_Status.e_Flag_StopEnabled = Flag_e_TRUE;
#else
                // before stopping sensor, switch to emulated clock
                CRM_SwitchToEmulatedSensorClocks();
#endif
                StopSensor();

#if CRM_CSI2_CLOCK_DETECTION
                OstTraceInt0(TRACE_FLOW, "Waiting for clocks to be lost");
                while (CRM_GetIsSensorClockAvailable())
                    ;
#else
                CRM_SetSensorClockUnavailable();

                //<TODO>: Declare a macro in FrameDimension module.
                // workaround implemented in 8815. Take little more buffer than the frame length.
                GPIOManager_Delay_us((1.2 * g_FrameDimensionStatus.f_VTFrameLength_us));
#endif
            }

            // Ref: Page 248, STn8500V2_SIA_HW_specification_v0.8.pdf only for V2
            // The sequence should be done only for V2. If done on V1, this will reset the full pry also.
            // issue reset only for IPP_SD_RESET - Bit 8, SDI
            // [15:9] RESERVED: default to 0
            // [8] SDI: reset sensor front-end (Sensor Data Interface) only (CSI2RX, CCPRX)
            // [7] BRML: reset BML/RML
        // [6:1] RESERVED: default to 0
        // [0] ALL: reset all sensor datapath (legacy SD_RESET)

        // Reset ISP Front end, IPP_SD_RESET = 0x1 << 8
        // NOTE1: Any IPP register access should be on 16 bit boundary from xP70, Individual access of bit will lead to undefined behaviour.
        // This problem was faced on actual customer h/w.
        // NOTE2: Reset of IPP_SD_RESET is suggested again in document. Might not be needed strictly. Safe to do it again.
        Set_IPP_IPP_SD_RESET(0,0,1);

        }
    }


    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void WaitForResourceToStop(e_StreamResource)
 \brief     Low level function to wait on a resource to stop
 \param     e_StreamResource    :   The resource which is to be stopped, possible values are StreamResource_e_Sensor and StreamResource_e_Rx
 \return    None
 \ingroup   Stream
 \callgraph
 \callergraph
 \endif
*/
void
WaitForResourceToStop(
uint8_t e_StreamResource)
{
    if (StreamResource_e_Rx == e_StreamResource)
    {
        // poll on the Rx to transition to running state
        while (!Is_ISP_SMIARX_ISP_SMIARX_STATUS_rx_state_GEN_IDLE())
            ;
#if 0
        // undo the override of the LINE_LENGTH register in SMIARX
        //        Set_ISP_SMIARX_ISP_SMIARX_OVERRIDE(0);
                /// AG: added here as:
                /// if SMIA RX is stopped, clock will not be coming to PIPE and we will not recieve video complete
        CRM_SwitchToEmulatedSensorClocks();

        if (SystemConfig_IPPSetupRequired())
        {
            if (SystemConfig_IsInputInterfaceCCP())
            {
                Set_IPP_IPP_INTERNAL_EN_CLK_CCP(0);

                // wait for 15us
                GPIOManager_Delay_us(15);

                // disable the IPP_DPHY_TOP_IF
                Set_IPP_IPP_DPHY_TOP_IF_EN(0);
            }
        }


#endif
        // If an abort-rx operation is not being done, ISP FW should wait
        // for active pipes to become idle, before proceeding further.
        if((Flag_e_FALSE == g_SystemSetup.e_Flag_abortRx_OnStop)
        && (Flag_e_FALSE == g_ErrorHandler.e_Flag_Error_Abort))
        {
        if (SystemConfig_IsPipe0Active())
        {
            while (!SystemConfig_IsPipe0Idle());
        }
        if (SystemConfig_IsPipe1Active())
        {
            while (!SystemConfig_IsPipe1Idle());
        }
        }
        else
        {
            // in case abort-rx is requested on STOP, set e_Flag_VideoCompleteInterruptPending
            // for both pipes
            g_PipeStatus[0].e_Flag_VideoCompleteInterruptPending = Flag_e_FALSE;
            g_PipeStatus[1].e_Flag_VideoCompleteInterruptPending = Flag_e_FALSE;
        }


        EventManager_ISPStopStreaming_Notify();
    }
    else if (StreamResource_e_Sensor == e_StreamResource)
    {
        // TODO: Wait on the sensor clocks to stop before raising sensor stop streaming notification
        EventManager_SensorStopStreaming_Notify();
    }


    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void StartRx( void )
 \brief     Low level function responsible for starting the Rx
 \param     None
 \return    None
 \ingroup   Stream
 \callgraph
 \callergraph
 \endif
*/
void
StartRx(void)
{
    // TODO: Move the Rx programming to an appropriate place
    // pull the Rx out of reset
    // set the ip and op pixel width
    // ccp_sel
    //
    // ip_pix_width,
    // op_pix_width,
    // parallel_enable,
    // vsync_polarity,
    // hsync_polarity,
    // ds_mode,
    // dcpx_enable,
    // ccp_sel,
    // reset_ccp_rx_n,
    // auto_sync,
    // pattern_type,
    // no_sof,
    // clr_ccp_shift_sync,
    // clr_ccp_crc_error,
    // clr_pixel_sync_error,
    // clr_aborted_iframe,
    // clr_insuff_iline,
    // fifo_rd_rst
    uint8_t u8_SensorInputPixelWidth;
    uint8_t u8_SensorOutputPixelWidth;
    uint8_t u8_DcpxEnable;
    uint8_t u8_DSMode;
    uint8_t u8_ParallelEnable;
    uint8_t no_sof;
    uint8_t ffhc;

    //[NON_ISL_SUPPORT] - SMIARx module can not decode ISL data of non smia sensor.
    // Hence in either case, whether sensor can output ISL or not, SMIARx should be programmed as if no ISL is coming from sensor.
    // If sensor CAN output ISL Lines, then SMIARx will use this line or the first of these lines (if there are multiple ISL lines) as SOF. This will avoid reducing the FOV for NON-SMIA sensor.
    // If sensor CAN NOT output ISL Lines, then SMIARx will use first line as SOF. To maintain color order, OPF will be programmed to cut off 2 lines from top.
    if(SENSOR_VERSION_NON_SMIA == g_SensorInformation.u8_smia_version)
    {
        //This is for non SMIA sensors
        no_sof = 1;

        //disable mode
        ffhc = 0;
    }
    else
    {
        no_sof = 0;

        //auto mode
        ffhc = 1;
    }

    u8_SensorInputPixelWidth = VideoTiming_GetSensorInputPixelWidth();
    u8_SensorOutputPixelWidth = VideoTiming_GetSensorOutputPixelWidth();

    if (u8_SensorInputPixelWidth > u8_SensorOutputPixelWidth)
    {
        u8_DcpxEnable = 1;
    }
    else
    {
        u8_DcpxEnable = 0;
    }

    if (VideoTiming_IsDataStrobeMode())
    {
        u8_DSMode = 1;
    }
    else
    {
        u8_DSMode = 0;
    }

    if (SystemConfig_IsInputInterfaceCCP())
    {
        u8_ParallelEnable = 0;
    }
    else
    {
        u8_ParallelEnable = 1;
    }

    Set_ISP_SMIARX_ISP_SMIARX_SETUP(
    u8_SensorOutputPixelWidth,
    u8_SensorInputPixelWidth,
    u8_ParallelEnable,
    1,
    1,
    u8_DSMode,
    u8_DcpxEnable,
    1,
    1,
    0,
    0,
    no_sof,
    0,
    0,
    0,
    0,
    0,
    0);

    // latency_lines < min sensor blanking .\n
    // g_SystemSetup.u8_LinesToWaitAtRxStop < g_SensorFrameConstraints.u16_MinVTLineBlankingPck
    // Set_ISP_SMIARX_ISP_SMIARX_SETUP(u8_SensorOutputPixelWidth, 12, 0, 1, 1, u8_DSMode, u8_DcpxEnable, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        Set_ISP_SMIARX_ISP_SMIARX_FRAME_CTRL(g_SystemSetup.u8_LinesToWaitAtRxStop, 0, g_SystemSetup.u8_NumOfFramesTobeSkipped, ffhc, 1);    //   latency_lines,stream_size,start_grab_dly,ffhc_control,stop_at_interframe

    // Update RxPattern and Cursor Position values if modified by HOST.
    SystemConfig_UpdateRxPattern();

    if (SystemConfig_IsInputInterfaceCSI2_0() || SystemConfig_IsInputInterfaceCSI2_1())
    {
        if (Is_8500v2() || Is_9540v1() || Is_8540v1())
        {
            //[NON_ISL_SUPPORT] -- for non smia sensor it will go in if condition
            if(SENSOR_VERSION_NON_SMIA == g_SensorInformation.u8_smia_version)
            {
                Set_ISP_SMIARX_ISP_SMIARX_OVERRIDE(0x7F000F);
                Set_ISP_SMIARX_ISP_SMIARX_MODEL_SUBTYPE_frame_format_model_subtype_ovr(0x12);
                Set_ISP_SMIARX_ISP_SMIARX_DESCRIPTOR_OVR_0_frame_format_descriptor_ovr((VISIBLE_PIXEL_DATA_CODE << 13) | (g_Zoom_Status_LLA.u16_output_resX));
                Set_ISP_SMIARX_ISP_SMIARX_DESCRIPTOR_OVR_1_frame_format_descriptor_ovr((EMBEDDED_DATA_CODE << 13) | (FrameDimension_GetNumberOfStatusLines()));
                Set_ISP_SMIARX_ISP_SMIARX_DESCRIPTOR_OVR_2_frame_format_descriptor_ovr((VISIBLE_PIXEL_DATA_CODE << 13) | ((g_Zoom_Status_LLA.u16_output_resY)));

                //mode subtype and frame format descriptor are already done above
                Set_ISP_SMIARX_ISP_SMIARX_FRAME_LENGTH_frame_length_ovr(g_CurrentFrameDimension.u16_VTFrameLengthLines);
                Set_ISP_SMIARX_ISP_SMIARX_LINE_LENGTH_line_length_ovr(g_CurrentFrameDimension.u16_VTLineLengthPck);

                // Override Pixel order for NON SMIA sensor using the values returned from LLCD
                OverridePixelOrder();
                Set_ISP_SMIARX_ISP_SMIARX_ADDR_INCREMENTS_x_even_inc_lsb_ovr(0x1);
                Set_ISP_SMIARX_ISP_SMIARX_ADDR_INCREMENTS_x_odd_inc_lsb_ovr(0x1);
                Set_ISP_SMIARX_ISP_SMIARX_ADDR_INCREMENTS_y_even_inc_lsb_ovr(0x1);
                Set_ISP_SMIARX_ISP_SMIARX_ADDR_INCREMENTS_y_odd_inc_lsb_ovr(0x1);
            }
            else
            {
                /*Three valid descriptor only, 1 for column and 2 for rows*/
                Set_ISP_SMIARX_ISP_SMIARX_MODEL_SUBTYPE_frame_format_model_subtype_ovr(0x12);
                Set_ISP_SMIARX_ISP_SMIARX_OVERRIDE(0x000F);
                Set_ISP_SMIARX_ISP_SMIARX_DESCRIPTOR_OVR_0_frame_format_descriptor_ovr((VISIBLE_PIXEL_DATA_CODE << 13) | (g_Zoom_Status_LLA.u16_output_resX));

                // Embedded line
                Set_ISP_SMIARX_ISP_SMIARX_DESCRIPTOR_OVR_1_frame_format_descriptor_ovr((EMBEDDED_DATA_CODE << 13) | (FrameDimension_GetNumberOfStatusLines()));
                Set_ISP_SMIARX_ISP_SMIARX_DESCRIPTOR_OVR_2_frame_format_descriptor_ovr((VISIBLE_PIXEL_DATA_CODE << 13) | (g_Zoom_Status_LLA.u16_output_resY));
            }
        }
        else
        {
            //[NON_ISL_SUPPORT] -- for non smia sensor it will go in if condition
            if(SENSOR_VERSION_NON_SMIA == g_SensorInformation.u8_smia_version)
            {
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_OVERRIDE(0x7F000F);
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_MODEL_SUBTYPE_frame_format_model_subtype_ovr(0x12);
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_0_frame_format_descriptor_ovr((VISIBLE_PIXEL_DATA_CODE << 12) | (g_Zoom_Status_LLA.u16_output_resX));
                Set_ISP_SMIARX_ISP_SMIARX_DESCRIPTOR_OVR_1_frame_format_descriptor_ovr((EMBEDDED_DATA_CODE << 13) | (FrameDimension_GetNumberOfStatusLines()));
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_2_frame_format_descriptor_ovr((VISIBLE_PIXEL_DATA_CODE << 12) | ((g_Zoom_Status_LLA.u16_output_resY)));

                //mode subtype and frame format descriptor are already done above
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_FRAME_LENGTH_frame_length_ovr(g_CurrentFrameDimension.u16_VTFrameLengthLines);
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_LINE_LENGTH_line_length_ovr(g_CurrentFrameDimension.u16_VTLineLengthPck);

                // Override Pixel order for NON SMIA sensor using the values returned from LLCD
                OverridePixelOrder();
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_x_even_inc_lsb_ovr(0x1);
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_x_odd_inc_lsb_ovr(0x1);
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_y_even_inc_lsb_ovr(0x1);
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_ADDR_INCREMENTS_y_odd_inc_lsb_ovr(0x1);
            }
            else
            {
                /*Three valid descriptor only, 1 for column and 2 for rows*/
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_MODEL_SUBTYPE_frame_format_model_subtype_ovr(0x12);
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_OVERRIDE(0x000F);
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_0_frame_format_descriptor_ovr((VISIBLE_PIXEL_DATA_CODE << 12) | (g_Zoom_Status_LLA.u16_output_resX));

                // Embedded line
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_1_frame_format_descriptor_ovr((EMBEDDED_DATA_CODE << 12) | (FrameDimension_GetNumberOfStatusLines()));
                Set_ISP_SMIARX_V1_ISP_SMIARX_V1_DESCRIPTOR_2_frame_format_descriptor_ovr((VISIBLE_PIXEL_DATA_CODE << 12) | (g_Zoom_Status_LLA.u16_output_resY));
            }
        }
    }

    // To read flash status register using ISP SMIARX. This register is for status of FLASH firing.
    Set_ISP_SMIARX_ISP_SMIARX_HK_REQ_ADDR_0(FLASH_STATUS_REGSTER);

    // To read Digital Gain  status register using ISP SMIARX. This register is for status of Digital Gain.
    Set_ISP_SMIARX_ISP_SMIARX_HK_REQ_ADDR_1(DIGITAL_GAIN_GREENR_HI);
    Set_ISP_SMIARX_ISP_SMIARX_HK_REQ_ADDR_2(DIGITAL_GAIN_GREENR_LOW);

    // Rx will be started by the test script if the CRM is not controlled by the firmware
    // start the receiver
    Set_ISP_SMIARX_ISP_SMIARX_CTRL(1, 0, 0);    // rx_start,rx_stop,rx_abort
    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void StartSensor( void )
 \brief     Low level function responsible for starting the sensor.
 \param     None
 \return    None
 \ingroup   Stream
 \callgraph
 \callergraph
 \endif
*/
void
StartSensor(void)
{
    lla_abstraction_StartSensor();
    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void StopSensor( void )
 \brief     Low level function responsible for stopping the sensor.
 \param     None
 \return    None
 \ingroup   Stream
 \callgraph
 \callergraph
 \endif
*/
void
StopSensor(void)
{
    LLA_Abstraction_SetStreamStatus(Flag_e_FALSE);
    return;
}


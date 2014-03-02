/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      ZoomTop.c
 \brief     Implements the top level zoom interface. It uses the low level
            zoom module to implement and manage sensor changeover, application
            of each zoom step, setting up of scalars for the first streaming
            step, programming of IP blocks not related to zoom (like MUX for
            different data paths)

 \ingroup   Zoom
 \endif
*/
#include "Platform.h"
#include "Zoom_IPInterface.h"
#include "Zoom_OPInterface.h"
#include "ZoomPlatformSpecific.h"
#include "ZoomTop.h"
#include "Zoom.h"
#include "FrameDimension_op_interface.h"
#include "EventManager.h"
#include "ITM.h"
#include "SystemConfig.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#   include "PictorProject_ZoomTop_ZoomTopTraces.h"
#endif
#define DEFAULT_PHYS_REG_SIZE       32
#define DEFAULT_PHYS_REG_WIDTH      6
#define DEFAULT_B_SIZE              2
#define DEFAULT_CRISP_LEVEL         8
#define DEFAULT_ANTI_ZIP_STATE      Flag_e_TRUE

#define Zoom_GetFixedLeftXLoss()    COLUMNS_LOST_AT_LEFT_EDGE_DUE_TO_COLOUR_INTERPOLATION
#define Zoom_GetFixedRightXLoss()   COLUMNS_LOST_AT_RIGHT_EDGE_DUE_TO_COLOUR_INTERPOLATION
#define Zoom_GetFixedTopYLoss()     ROWS_LOST_AT_TOP_EDGE_DUE_TO_COLOUR_INTERPOLATION
#define Zoom_GetFixedBottomYLoss()  ROWS_LOST_AT_BOTTOM_EDGE_DUE_TO_COLOUR_INTERPOLATION

/**
 \struct    Scalar_Coefficients_ts
 \brief     Data structure to hold the scalar coefficients for a pipe
 \ingroup   Zoom
*/
typedef struct
{
    int16_t s16_PipeCofs[DEFAULT_PHYS_REG_SIZE];
} Scalar_Coefficients_ts;

volatile ZoomTop_ParamAppicationControl_ts  g_ZoomTop_ParamAppicationControl = { 0, 0, 0, 0 };

Scalar_UserParams_ts                        g_Pipe_Scalar_UserParams[] =
{
    { DEFAULT_PHYS_REG_SIZE, DEFAULT_PHYS_REG_WIDTH, DEFAULT_B_SIZE, DEFAULT_CRISP_LEVEL, DEFAULT_ANTI_ZIP_STATE },
    { DEFAULT_PHYS_REG_SIZE, DEFAULT_PHYS_REG_WIDTH, DEFAULT_B_SIZE, DEFAULT_CRISP_LEVEL, DEFAULT_ANTI_ZIP_STATE }
};

Scalar_Inputs_ts                            g_Pipe_Scalar_Inputs[PIPE_COUNT];

Scalar_Output_ts                            g_Pipe_Scalar_Output[PIPE_COUNT];

Scalar_Stripe_InputParams_ts                g_Pipe_Scalar_StripeInputs[] = { { 0, 0 }, { 0, 0 } };

Scalar_Stripe_OutputParams_ts               g_Pipe_Scalar_StripeOutputs[PIPE_COUNT];

Scalar_Coefficients_ts                      g_PipeCofs[PIPE_COUNT];

void PrepareScalarInputs
    (
        Zoom_PipeState_ts *
        p_PipeState, Scalar_Inputs_ts *
        p_Scalar_Inputs, Scalar_Stripe_InputParams_ts *
        p_Scalar_Stripe_Inputs
    ) TO_EXT_DDR_PRGM_MEM;
void ProgramHardware (uint8_t u8_PipeNo) TO_EXT_DDR_PRGM_MEM;
uint8_t ComputeScalarParams ( void ) TO_EXT_DDR_PRGM_MEM;
static void ProgramOPF(uint8_t u8_PipeNo);
/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void ZoomTop_Initialise( void )
 \brief     Used to initialise the zoom top module. Must be invoked
            once after BOOT and before any streaming.
 \param     None
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
void
ZoomTop_Initialise(void)
{
    /// \par Implementation
    /// - Stores the base address of the pipe0 coefficient array in scalar output page for pipe0.
    /// - Stores the base address of the pipe0 coefficient array in scalar output page for pipe1.

    g_Pipe_Scalar_Output[0].ptrs16_Cof = (int16_t *)&g_PipeCofs[0];

    if (PIPE_COUNT > 1)
    {
        g_Pipe_Scalar_Output[1].ptrs16_Cof = ( int16_t * ) &g_PipeCofs[1];
    }


    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        uint8_t ZoomTop_PreRunUpdate( void )
 \brief     Used to prepare the zoom for a streaming operation.
            Must be called before streaming.
 \param     None
 \return    Flag_e_FALSE: Zoom pre update failed
            Flag_e_TRUE: Zoom pre update successful
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
uint8_t
ZoomTop_PreRunUpdate(void)
{
    uint8_t e_ZoomRequestStatus;
    Flag_te e_ZoomInitializeStatus = Flag_e_FALSE;

    /// \par Implementation
    /// - Sets the center for the current streaming operation.
    /// - Sets the FOV for the current streaming operation.
    /// - Computes the scalar params corresponding to the current zoom step for both the pipes.
    e_ZoomRequestStatus = ZoomRequestStatus_e_None;

    if (ZoomRequestStatus_e_ReProgramRequired != g_Zoom_CommandStatus.e_ZoomRequestStatus)
    {
        // a sensor change over is not pending
        // normal startup process
       e_ZoomInitializeStatus = Zoom_Initialise();

        if(Flag_e_TRUE == e_ZoomInitializeStatus)
        {
         //Zoom_Initialise is true and HW limita
        // we are preparing for the streaming operation
        // the center must be set first
        g_Zoom_CommandControl.e_ZoomCommand = ZoomCommand_e_SetCenter;
        g_Zoom_CommandControl.u8_CommandCount += 1;
        Zoom_Run(Flag_e_TRUE);

        if (ZoomRequestStatus_e_Denied != g_Zoom_CommandStatus.e_ZoomRequestStatus)
        {
            // now set the desired FOV
            g_Zoom_CommandControl.e_ZoomCommand = ZoomCommand_e_SetFOV;
            g_Zoom_CommandControl.u8_CommandCount += 1;
            Zoom_Run(Flag_e_TRUE);

            //            g_Zoom_Status.e_ZoomRequestLLDStatus = ZoomRequestLLDStatus_e_Accepted;
            e_ZoomRequestStatus = g_Zoom_CommandStatus.e_ZoomRequestStatus;
        }
        else
        {
            e_ZoomRequestStatus = ZoomRequestStatus_e_Denied;
        }
        }
        else
        {
          //e_ZoomInitializeStatus is false
          //nothin needs to be done
        }
    }
    else
    {
        // a sensor changeover is pending,
        // nothing needs to be done
        e_ZoomInitializeStatus = Flag_e_TRUE;
    }


    if ((ZoomRequestStatus_e_Denied == e_ZoomRequestStatus) || (Flag_e_FALSE== e_ZoomInitializeStatus))
    {
        return (Flag_e_FALSE);
    }
    else
    {
        g_Zoom_Status.e_ZoomRequestLLDStatus = ZoomRequestLLDStatus_e_Accepted;
        return (Flag_e_TRUE);
    }
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        uint8_t ComputeScalarParams( void )
 \brief     Computes the scalar params required for the current zoom step.
 \param     None
 \return    Number of scalar parameters generated
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
uint8_t
ComputeScalarParams(void)
{
    uint8_t u8_PipeToUpdate;

    u8_PipeToUpdate = 0;

    /// \par Implementation
    /// - Prepares the input parameters required for scalar parameter computation for
    /// pipe0 and pipe1 (if they are enabled) corresponding to the current zoom step.
    /// - Generates the scalar parameters required for the current zoom step for the
    /// pipes that are enabled.
    // create the zoom input parameters and perform the
    // corresponding scalar computations.
    if (Zoom_GetPipe0Enabled())
    {
        PrepareScalarInputs(&g_PipeState[0], &g_Pipe_Scalar_Inputs[0], &g_Pipe_Scalar_StripeInputs[0]);
        Scalar_Compute(
        &g_Pipe_Scalar_UserParams[0],
        &g_Pipe_Scalar_Inputs[0],
        &g_Pipe_Scalar_Output[0],
        &g_Pipe_Scalar_StripeInputs[0],
        &g_Pipe_Scalar_StripeOutputs[0],
        0);

        u8_PipeToUpdate = 1;
    }


    // create the zoom input parameters and perform the
    // corresponding scalar computations.
    if (Zoom_GetPipe1Enabled())
    {
        PrepareScalarInputs(&g_PipeState[1], &g_Pipe_Scalar_Inputs[1], &g_Pipe_Scalar_StripeInputs[1]);
        Scalar_Compute(
        &g_Pipe_Scalar_UserParams[1],
        &g_Pipe_Scalar_Inputs[1],
        &g_Pipe_Scalar_Output[1],
        &g_Pipe_Scalar_StripeInputs[1],
        &g_Pipe_Scalar_StripeOutputs[1],
        1);

        u8_PipeToUpdate += 1;
    }


    return (u8_PipeToUpdate);
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void ZoomTop_FDMRequestAccepted( void )
 \brief     Top level zoom function to be called to indicate
            that a pending sensor change over has been absorbed.
 \param     None
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
void
ZoomTop_FDMRequestAccepted(void)
{
    uint8_t u8_ScalarParamsGenerated;

    // compute the scalar params
    u8_ScalarParamsGenerated = ComputeScalarParams();

    if (Flag_e_TRUE == g_Zoom_Control.e_Flag_PerformDynamicResolutionUpdate)
    {
        EventManager_ZoomOutputResolutionReady_Notify();
        EventManager_DMA_GRAB_VideoStab_Notify();
        g_GrabNotify.u32_DMA_GRAB_Indicator_For_VideoStab = 0;
    }


    g_ZoomTop_ParamAppicationControl.u8_ScalarParamsGenerated += u8_ScalarParamsGenerated;

    // flag sensor changeover complete to the zoom module as well
    Zoom_SensorChangeOverCompleted();

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void ZoomTop_Run( void )
 \brief     Top level zoom function to be called continuously to grab
            a zoom command. It is envisaged that this function will be
            called in idle state of the mcu (i.e. not in interrupts)
 \param     None
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
void
ZoomTop_Run(void)
{
    uint8_t u8_ScalarParamsGenerated;
    uint8_t e_Flag_UserCommandPending = Flag_e_FALSE;

    /// \par Implementation
    /// - If a previous zoom step application is not pending, it invokes
    /// Zoom_Run to process any user zoom commands.
    /// - If any user command is pending, it invokes ComputeScalarParams
    /// to generate the corresponding scalar parameters.
    if
    (
        g_ZoomTop_ParamAppicationControl.u8_ScalarParamsApplied == g_ZoomTop_ParamAppicationControl.
            u8_ScalarParamsGenerated
    )
    {
        // no scalar update is pending from a previous zoom step
        e_Flag_UserCommandPending = Zoom_Run(!(g_SystemConfig_Status.e_Flag_ZoomdStatsOnly));

        // if a user command is pending and a sensor reprogram is not required
        // then generate the scalar params
        // if a sensor reprogram is required then the scalar params will be
        // generated after the changeover has happened.
        if ((Flag_e_TRUE == e_Flag_UserCommandPending) && (!Zoom_IsFDMRequestPending()))
        {
#ifdef PROFILER_USING_XTI
    OstTraceInt0(TRACE_USER8, "start_xti_profiler_zoom");
#endif
            u8_ScalarParamsGenerated = ComputeScalarParams();

            if (Flag_e_TRUE == g_Zoom_Control.e_Flag_PerformDynamicResolutionUpdate)
            {
                // disable pipe0 output only if it is enabled; enabled when ack from GRAB is received
                if (Flag_e_TRUE == g_PipeState[0].e_Flag_Enabled)
                {
                    Set_ISP_CE0_IDP_GATE_ISP_CE0_IDP_GATE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0); // mux2to1_disable,mux2to1_soft_reset
                }


                // disable pipe1 output only if it is enabled; enabled when ack from GRAB is received
                if (Flag_e_TRUE == g_PipeState[1].e_Flag_Enabled)
                {
                    Set_ISP_CE1_IDP_GATE_ISP_CE1_IDP_GATE_ENABLE(mux2to1_enable_DISABLE, mux2to1_soft_reset_B_0x0); // mux2to1_disable,mux2to1_soft_reset
                }


                EventManager_ZoomOutputResolutionReady_Notify();
                EventManager_DMA_GRAB_VideoStab_Notify();
                g_GrabNotify.u32_DMA_GRAB_Indicator_For_VideoStab = 0;
            }
            if(Flag_e_FALSE == g_SystemConfig_Status.e_Flag_ZoomdStatsOnly)
            {
                // Zoom is applied to ISP on the basis of this flag. Notification is sent in vid complete
            g_ZoomTop_ParamAppicationControl.u8_ScalarParamsGenerated += u8_ScalarParamsGenerated;
            }
            else
            {
                // Notify immediately to Host as zoom is not applied to ISP and all calculations are over
                Zoom_StepComplete();
            }

            if (Flag_e_TRUE == g_SystemConfig_Status.e_Flag_FireDummyVidComplete0)
            {
                // trigger vid complete0 which will program CE0
                ITM_TriggerVidComplete0Interupt();
            }


            if (Flag_e_TRUE == g_SystemConfig_Status.e_Flag_FireDummyVidComplete1)
            {
                // trigger vid complete1 which will program CE1
                ITM_TriggerVidComplete1Interupt();
            }

            if ((Flag_e_TRUE == g_SystemConfig_Status.e_Flag_FireDummyVidComplete0) ||
            (Flag_e_TRUE == g_SystemConfig_Status.e_Flag_FireDummyVidComplete1) )
            {
                Set_ITM_ITM_DMCEPIPE_STATUS_BSET_DMCE_OUT_STATUS_BSET(1);
            }
        }
    }


    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void ProgramHardware(uint8_t u8_PipeNo)
 \brief     Programs the scalar, cropper and other related IP blocks for the current zoom step
 \param     u8_PipeNo : Specifies the pipe number to be updated
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
void
ProgramHardware(
uint8_t u8_PipeNo)
{
    ISP_CE0_GPSSCALER_IP_ts         *p_ISP_CE_GPSSCALER_IP;
    ISP_CE0_CROP_IP_ts              *p_ISP_CE_CROP_IP;

    // ISP_CE0_CROP_IP_ts *p_ISP_CE_CROP_IP_another;
    ISP_CE0_MUX_GPS_IP_ts           *p_ISP_CE_MUX_GPS_IP;
    ISP_CE0_MUX_SHARP_IP_ts         *p_ISP_CE_MUX_SHARP_IP;
    ISP_CE0_MUX_UNSHARP_IP_ts       *p_ISP_CE_MUX_UNSHARP_IP;
    ISP_CE0_MUX_MATRIX_ADSOC_IP_ts  *p_ISP_CE_MUX_MATRIX_ADSOC_IP;
    ISP_CE0_IDP_GATE_IP_ts          *p_ISP_CE_IDP_GATE_IP;
    Scalar_Inputs_ts                *Pipe_Scalar_Inputs;
    Scalar_Output_ts                *Pipe_Scalar_Output;
    Scalar_Stripe_InputParams_ts    *Pipe_Scalar_StripeInputs;
    Scalar_Stripe_OutputParams_ts   *Pipe_Scalar_StripeOutputs;
    Scalar_Stripe_OutputParams_ts   *Pipe_Scalar_StripeOutputs_another;
    uint8_t                         u8_MinimumLineBlankingBeyondScalar;

    uint8_t                         *p_HCofHwReg;
    uint8_t                         *p_VCofHwReg;
    uint8_t                         u8_CofCount;
    uint16_t                        u16_VBulk;
    uint16_t                        u16_VFrac;
    uint16_t                        u16_HBulk;
    uint16_t                        u16_HFrac;
    uint8_t                         u8_FirTaps;
    float_t                         f_SensorPrescaleInv;
    uint16_t                        u8_MinNoOfPixPickedInOneGoFromDma = 1;
    uint16_t                        u16_DMACropValueRemainder_0 = 0,
                                    u16_DMACropValueRemainder_1 = 0;
    uint16_t                        u16_MinIntegerInDMA_0 = 0,
                                    u16_MinIntegerInDMA_1 = 0,
                                    u16_MinIntegerInDMA;
    uint16_t                        u16_Value;
    uint8_t                         e_Flag_OtherPipeEnabled;

    // int16_t  s16_DMAStartDiff = 0;
    // uint16_t     u16_HorizontalOffset = 0;
    f_SensorPrescaleInv = 1.0 / FrameDimension_GetCurrentPreScaleFactor();

    // Normally it is preferable to use the Macros exposed by the MemoryMapwrapper.
    // However in this case, since the number of hardware registers are so big, it
    // is better to program them individually through pointers. Pointer manipulation
    // in XP70 is not heavy
    if (0 == u8_PipeNo)
    {
        p_ISP_CE_GPSSCALER_IP = (ISP_CE0_GPSSCALER_IP_ts *) p_ISP_CE0_GPSSCALER_IP;
        p_ISP_CE_CROP_IP = (ISP_CE0_CROP_IP_ts *) p_ISP_CE0_CROP_IP;
        p_ISP_CE_MUX_GPS_IP = (ISP_CE0_MUX_GPS_IP_ts *) p_ISP_CE0_MUX_GPS_IP;
        p_ISP_CE_MUX_SHARP_IP = (ISP_CE0_MUX_SHARP_IP_ts *) p_ISP_CE0_MUX_SHARP_IP;
        p_ISP_CE_MUX_UNSHARP_IP = (ISP_CE0_MUX_UNSHARP_IP_ts *) p_ISP_CE0_MUX_UNSHARP_IP;
        p_ISP_CE_MUX_MATRIX_ADSOC_IP = (ISP_CE0_MUX_MATRIX_ADSOC_IP_ts *) p_ISP_CE0_MUX_MATRIX_ADSOC_IP;
        p_ISP_CE_IDP_GATE_IP = (ISP_CE0_IDP_GATE_IP_ts *) p_ISP_CE0_IDP_GATE_IP;
        Pipe_Scalar_Inputs = &g_Pipe_Scalar_Inputs[0];
        Pipe_Scalar_Output = &g_Pipe_Scalar_Output[0];
        Pipe_Scalar_StripeInputs = &g_Pipe_Scalar_StripeInputs[0];
        Pipe_Scalar_StripeOutputs = &g_Pipe_Scalar_StripeOutputs[0];
        u8_MinimumLineBlankingBeyondScalar = Zoom_GetMinLineBlankingBeyondGPS0();

        // p_ISP_CE_CROP_IP_another = (ISP_CE0_CROP_IP_ts *)p_ISP_CE1_CROP_IP;
        Pipe_Scalar_StripeOutputs_another = &g_Pipe_Scalar_StripeOutputs[1];

        if (Zoom_GetPipe1Enabled())
        {
            e_Flag_OtherPipeEnabled = Flag_e_TRUE;
        }
        else
        {
            e_Flag_OtherPipeEnabled = Flag_e_FALSE;
        }
    }
    else
    {
        p_ISP_CE_GPSSCALER_IP = ( ISP_CE0_GPSSCALER_IP_ts * ) p_ISP_CE1_GPSSCALER_IP;
        p_ISP_CE_CROP_IP = ( ISP_CE0_CROP_IP_ts * ) p_ISP_CE1_CROP_IP;
        p_ISP_CE_MUX_GPS_IP = ( ISP_CE0_MUX_GPS_IP_ts * ) p_ISP_CE1_MUX_GPS_IP;
        p_ISP_CE_MUX_SHARP_IP = ( ISP_CE0_MUX_SHARP_IP_ts * ) p_ISP_CE1_MUX_SHARP_IP;
        p_ISP_CE_MUX_UNSHARP_IP = ( ISP_CE0_MUX_UNSHARP_IP_ts * ) p_ISP_CE1_MUX_UNSHARP_IP;
        p_ISP_CE_MUX_MATRIX_ADSOC_IP = ( ISP_CE0_MUX_MATRIX_ADSOC_IP_ts * ) p_ISP_CE1_MUX_MATRIX_ADSOC_IP;
        p_ISP_CE_IDP_GATE_IP = ( ISP_CE0_IDP_GATE_IP_ts * ) p_ISP_CE1_IDP_GATE_IP;
        Pipe_Scalar_Inputs = &g_Pipe_Scalar_Inputs[1];
        Pipe_Scalar_Output = &g_Pipe_Scalar_Output[1];
        Pipe_Scalar_StripeInputs = &g_Pipe_Scalar_StripeInputs[1];
        Pipe_Scalar_StripeOutputs = &g_Pipe_Scalar_StripeOutputs[1];
        u8_MinimumLineBlankingBeyondScalar = Zoom_GetMinLineBlankingBeyondGPS1();

        // p_ISP_CE_CROP_IP_another = p_ISP_CE0_CROP_IP;
        Pipe_Scalar_StripeOutputs_another = &g_Pipe_Scalar_StripeOutputs[0];
        if (Zoom_GetPipe0Enabled())
        {
            e_Flag_OtherPipeEnabled = Flag_e_TRUE;
        }
        else
        {
            e_Flag_OtherPipeEnabled = Flag_e_FALSE;
        }
    }


    //Before Programming Cropper Start Coordinates,first check whether DMA can send this or need to be aligned further
    if (Pipe_Scalar_StripeInputs->u8_StripeCount >= 1)
    {
        if (DMADataAccessAlignment_e_2Byte == SystemConfig_DMADataAccessAlignment())
        {
            if (DMADataBitsInMemory_10 == SystemConfig_DMADataBitsInOneMemoryWord())
            {
                //Each Pixel is 10 bits and stored as packed form in DMA
                //8*10=80 bits and DMA Data Alignment is 16 bits ,so its possible because 80/16= 5 (integer number)
                u8_MinNoOfPixPickedInOneGoFromDma = 8;
            }
            else if (DMADataBitsInMemory_12 == SystemConfig_DMADataBitsInOneMemoryWord())
            {
                //Each Pixel is 12 bits and stored as packed form in DMA
                //4*12=48 bits and DMA Data Alignment is 16 bits ,so its possible because 48/16= 3 (integer number)
                u8_MinNoOfPixPickedInOneGoFromDma = 4;
            }
            else if (DMADataBitsInMemory_16 == SystemConfig_DMADataBitsInOneMemoryWord())
            {
                //Each Pixel is 16 bits and stored as unpacked form in DMA,means taking 16 bits in memory
                //1*16=16 bits and DMA Data Alignment is 16 bits ,so its possible because 16/16= 1 (integer number)
                u8_MinNoOfPixPickedInOneGoFromDma = 1;
            }
            else
            {
            }
        }
        else if (DMADataAccessAlignment_e_4Byte == SystemConfig_DMADataAccessAlignment())
        {
            if (DMADataBitsInMemory_10 == SystemConfig_DMADataBitsInOneMemoryWord())
            {
                //Each Pixel is 10 bits and stored as packed form in DMA
                //16*10=160 bits and DMA Data Alignment is 32 bits ,so its possible because 160/32= 5 (integer number)
                u8_MinNoOfPixPickedInOneGoFromDma = 16;
            }
            else if (DMADataBitsInMemory_12 == SystemConfig_DMADataBitsInOneMemoryWord())
            {
                //Each Pixel is 12 bits and stored as packed form in DMA
                //8*12=96 bits and DMA Data Alignment is 32 bits ,so its possible because 96/32= 3 (integer number)
                u8_MinNoOfPixPickedInOneGoFromDma = 8;
            }
            else if (DMADataBitsInMemory_16 == SystemConfig_DMADataBitsInOneMemoryWord())
            {
                //Each Pixel is 16 bits and stored as unpacked form in DMA,means taking 16 bits in memory
                //2*16=32 bits and DMA Data Alignment is 32 bits ,so its possible because 32/32= 1 (integer number)
                u8_MinNoOfPixPickedInOneGoFromDma = 2;
            }
        }
        else if (DMADataAccessAlignment_e_8Byte == SystemConfig_DMADataAccessAlignment())
        {

			if (DMADataBitsInMemory_8 == SystemConfig_DMADataBitsInOneMemoryWord())
            {
                //Each Pixel is 8 bits and stored as packed form in DMA
                //8*8=64 bits and DMA Data Alignment is 64 bits ,so its possible because 64/64= 1 (integer number)
                u8_MinNoOfPixPickedInOneGoFromDma = 8;
            }
            else if (DMADataBitsInMemory_10 == SystemConfig_DMADataBitsInOneMemoryWord())
            {
                //Each Pixel is 10 bits and stored as packed form in DMA
                //32*10=320 bits and DMA Data Alignment is 64 bits ,so its possible because 320/64= 5 (integer number)
                u8_MinNoOfPixPickedInOneGoFromDma = 32;
            }
            else if (DMADataBitsInMemory_12 == SystemConfig_DMADataBitsInOneMemoryWord())
            {
                //Each Pixel is 12 bits and stored as packed form in DMA
                //16*12=192 bits and DMA Data Alignment is 64 bits ,so its possible because 192/64= 3 (integer number)
                u8_MinNoOfPixPickedInOneGoFromDma = 16;
            }
            else if (DMADataBitsInMemory_16 == SystemConfig_DMADataBitsInOneMemoryWord())
            {
                //Each Pixel is 16 bits and stored as unpacked form in DMA,means taking 16 bits in memory
                //4*16=64 bits and DMA Data Alignment is 64 bits ,so its possible because 64/64= 1 (integer number)
                u8_MinNoOfPixPickedInOneGoFromDma = 4;
            }
        }
        else
        {
            u8_MinNoOfPixPickedInOneGoFromDma = 500;
        }


        u16_DMACropValueRemainder_0 = Pipe_Scalar_StripeOutputs->s16_StripeInCropHStart % u8_MinNoOfPixPickedInOneGoFromDma;
        u16_DMACropValueRemainder_1 = Pipe_Scalar_StripeOutputs_another->s16_StripeInCropHStart % u8_MinNoOfPixPickedInOneGoFromDma;

        if (u16_DMACropValueRemainder_0 != 0)
        {
            u16_MinIntegerInDMA_0 = u8_MinNoOfPixPickedInOneGoFromDma * (Pipe_Scalar_StripeOutputs->s16_StripeInCropHStart / u8_MinNoOfPixPickedInOneGoFromDma);
        }
        else
        {
            u16_MinIntegerInDMA_0 = Pipe_Scalar_StripeOutputs->s16_StripeInCropHStart;              ///u8_MinNoOfPixPickedInOneGoFromDma * (Pipe_Scalar_StripeOutputs->s16_StripeInCropHStart / u8_MinNoOfPixPickedInOneGoFromDma);
        }


        if (Flag_e_TRUE == e_Flag_OtherPipeEnabled)
        {
            if (u16_DMACropValueRemainder_1 != 0)
            {
                u16_MinIntegerInDMA_1 = u8_MinNoOfPixPickedInOneGoFromDma * (Pipe_Scalar_StripeOutputs_another->s16_StripeInCropHStart / u8_MinNoOfPixPickedInOneGoFromDma);
            }
            else
            {
                u16_MinIntegerInDMA_1 = Pipe_Scalar_StripeOutputs_another->s16_StripeInCropHStart;  ///u8_MinNoOfPixPickedInOneGoFromDma * (Pipe_Scalar_StripeOutputs->s16_StripeInCropHStart / u8_MinNoOfPixPickedInOneGoFromDma);
            }
        }
        else
        {
            u16_MinIntegerInDMA_1 = u16_MinIntegerInDMA_0;
        }


        u16_MinIntegerInDMA = (u16_MinIntegerInDMA_0 < u16_MinIntegerInDMA_1) ? u16_MinIntegerInDMA_0 : u16_MinIntegerInDMA_1;

        /*
                s16_DMAStartDiff = u16_MinIntegerInDMA_0 - u16_MinIntegerInDMA;

                u16_HorizontalOffset = Pipe_Scalar_StripeOutputs->s16_StripeInCropHStart - u16_MinIntegerInDMA_0;

                if(s16_DMAStartDiff  > 0)
                    u16_HorizontalOffset += s16_DMAStartDiff;
        */

        /* For debugging only */

        /*
                g_DMAControl.u8_MinNoOfPixPickedInOneGoFromDma = u8_MinNoOfPixPickedInOneGoFromDma;
                g_DMAControl.u16_DMACropValueRemainder_0 = u16_DMACropValueRemainder_0;
                g_DMAControl.u16_DMACropValueRemainder_1 = u16_DMACropValueRemainder_1;
                g_DMAControl.u16_MinIntegerInDMA_0 = u16_MinIntegerInDMA_0;
                g_DMAControl.u16_MinIntegerInDMA_1 = u16_MinIntegerInDMA_1;
        */

        // g_DMAControl.u16_HorizontalOffset  = u16_HorizontalOffset;
        // g_DMAControl.s16_DMAStartDiff         = s16_DMAStartDiff;
        /*g_DMAControl.u8_MinNoOfPixPickedInOneGoFromDma = 1;
                        g_DMAControl.u16_DMACropValueRemainder_0 = 2;
                        g_DMAControl.u16_DMACropValueRemainder_1 = 3;
                        g_DMAControl.u16_MinIntegerInDMA_0 = 4;
                        g_DMAControl.u16_MinIntegerInDMA_1 = 5;
                        g_DMAControl.u16_HorizontalOffset  = 6;
                        g_DMAControl.s16_DMAStartDiff        = 7; */
    }   //if(StripeCount>=1)

    // disable the pipe input to ensure that in case we are not able to update the pipe
    // before the start of the next frame, the frame is dropped instead of sending a corrupt frame

    // Program the GPS MUX
    p_ISP_CE_MUX_GPS_IP->ISP_CE0_MUX_GPS_ENABLE.ISP_CE0_MUX_GPS_ENABLE_ts.mux2to1_enable = 0;
    p_ISP_CE_MUX_SHARP_IP->ISP_CE0_MUX_SHARP_ENABLE.ISP_CE0_MUX_SHARP_ENABLE_ts.mux2to1_enable = 0;
    p_ISP_CE_MUX_UNSHARP_IP->ISP_CE0_MUX_UNSHARP_ENABLE.ISP_CE0_MUX_UNSHARP_ENABLE_ts.mux2to1_enable = 0;
    p_ISP_CE_MUX_MATRIX_ADSOC_IP->ISP_CE0_MUX_MATRIX_ADSOC_ENABLE.ISP_CE0_MUX_MATRIX_ADSOC_ENABLE_ts.mux2to1_enable = 0;

    p_ISP_CE_MUX_GPS_IP->ISP_CE0_MUX_GPS_ENABLE.ISP_CE0_MUX_GPS_ENABLE_ts.mux2to1_soft_reset = 1;
    p_ISP_CE_MUX_SHARP_IP->ISP_CE0_MUX_SHARP_ENABLE.ISP_CE0_MUX_SHARP_ENABLE_ts.mux2to1_soft_reset = 1;
    p_ISP_CE_MUX_UNSHARP_IP->ISP_CE0_MUX_UNSHARP_ENABLE.ISP_CE0_MUX_UNSHARP_ENABLE_ts.mux2to1_soft_reset = 1;
    p_ISP_CE_MUX_MATRIX_ADSOC_IP->ISP_CE0_MUX_MATRIX_ADSOC_ENABLE.ISP_CE0_MUX_MATRIX_ADSOC_ENABLE_ts.mux2to1_soft_reset = 1;

    p_ISP_CE_MUX_GPS_IP->ISP_CE0_MUX_GPS_ENABLE.ISP_CE0_MUX_GPS_ENABLE_ts.mux2to1_soft_reset = 0;
    p_ISP_CE_MUX_SHARP_IP->ISP_CE0_MUX_SHARP_ENABLE.ISP_CE0_MUX_SHARP_ENABLE_ts.mux2to1_soft_reset = 0;
    p_ISP_CE_MUX_UNSHARP_IP->ISP_CE0_MUX_UNSHARP_ENABLE.ISP_CE0_MUX_UNSHARP_ENABLE_ts.mux2to1_soft_reset = 0;
    p_ISP_CE_MUX_MATRIX_ADSOC_IP->ISP_CE0_MUX_MATRIX_ADSOC_ENABLE.ISP_CE0_MUX_MATRIX_ADSOC_ENABLE_ts.mux2to1_soft_reset = 0;

    p_ISP_CE_MUX_GPS_IP->ISP_CE0_MUX_GPS_ENABLE.ISP_CE0_MUX_GPS_ENABLE_ts.mux2to1_enable = 1;
    p_ISP_CE_MUX_SHARP_IP->ISP_CE0_MUX_SHARP_ENABLE.ISP_CE0_MUX_SHARP_ENABLE_ts.mux2to1_enable = 1;
    p_ISP_CE_MUX_UNSHARP_IP->ISP_CE0_MUX_UNSHARP_ENABLE.ISP_CE0_MUX_UNSHARP_ENABLE_ts.mux2to1_enable = 1;
    p_ISP_CE_MUX_MATRIX_ADSOC_IP->ISP_CE0_MUX_MATRIX_ADSOC_ENABLE.ISP_CE0_MUX_MATRIX_ADSOC_ENABLE_ts.mux2to1_enable = 1;

    // Disable scalar and enable soft reset
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_ENABLE.word = ((uint32_t) 0 << 0 | (uint32_t) 0 << 1 | (uint32_t) 1 << 4);

    if (0==u8_PipeNo)
    {
        Set_ISP_OPF0_ISP_OPF0_ENABLE_enable__DISABLE();
        Set_ISP_OPF0_ISP_OPF0_ENABLE_soft_reset__B_0x1();
        Set_ISP_OPF0_ISP_OPF0_ENABLE_soft_reset__B_0x0();
        //Set_ISP_OPF0_ISP_OPF0_ENABLE(0, 1); // enable,soft_reset
    }
    else
    {
        Set_ISP_OPF1_ISP_OPF1_ENABLE_enable__DISABLE();
        Set_ISP_OPF1_ISP_OPF1_ENABLE_soft_reset__B_0x1();
        Set_ISP_OPF1_ISP_OPF1_ENABLE_soft_reset__B_0x0();
        //Set_ISP_OPF1_ISP_OPF1_ENABLE(1, 1); // enable,soft_reset
    }

    p_ISP_CE_MUX_GPS_IP->ISP_CE0_MUX_GPS_SELECT.ISP_CE0_MUX_GPS_SELECT_ts.mux2to1_shadow_en = 1;
    p_ISP_CE_MUX_SHARP_IP->ISP_CE0_MUX_SHARP_SELECT.ISP_CE0_MUX_SHARP_SELECT_ts.mux2to1_shadow_en = 1;
    p_ISP_CE_MUX_UNSHARP_IP->ISP_CE0_MUX_UNSHARP_SELECT.ISP_CE0_MUX_UNSHARP_SELECT_ts.mux2to1_shadow_en = 1;
    p_ISP_CE_MUX_MATRIX_ADSOC_IP->ISP_CE0_MUX_MATRIX_ADSOC_SELECT.ISP_CE0_MUX_MATRIX_ADSOC_SELECT_ts.mux2to1_shadow_en = 1;

    if (Flag_e_TRUE == Pipe_Scalar_Output->e_Flag_DownScaling)
    {
        p_ISP_CE_MUX_GPS_IP->ISP_CE0_MUX_GPS_SELECT.ISP_CE0_MUX_GPS_SELECT_ts.mux2to1_select = 0;
        p_ISP_CE_MUX_SHARP_IP->ISP_CE0_MUX_SHARP_SELECT.ISP_CE0_MUX_SHARP_SELECT_ts.mux2to1_select = 0;
        p_ISP_CE_MUX_UNSHARP_IP->ISP_CE0_MUX_UNSHARP_SELECT.ISP_CE0_MUX_UNSHARP_SELECT_ts.mux2to1_select = 0;
        p_ISP_CE_MUX_MATRIX_ADSOC_IP->ISP_CE0_MUX_MATRIX_ADSOC_SELECT.ISP_CE0_MUX_MATRIX_ADSOC_SELECT_ts.mux2to1_select  = 0;
    }
    else
    {
        p_ISP_CE_MUX_GPS_IP->ISP_CE0_MUX_GPS_SELECT.ISP_CE0_MUX_GPS_SELECT_ts.mux2to1_select = 1;
        p_ISP_CE_MUX_SHARP_IP->ISP_CE0_MUX_SHARP_SELECT.ISP_CE0_MUX_SHARP_SELECT_ts.mux2to1_select = 1;
        p_ISP_CE_MUX_UNSHARP_IP->ISP_CE0_MUX_UNSHARP_SELECT.ISP_CE0_MUX_UNSHARP_SELECT_ts.mux2to1_select = 1;
        p_ISP_CE_MUX_MATRIX_ADSOC_IP->ISP_CE0_MUX_MATRIX_ADSOC_SELECT.ISP_CE0_MUX_MATRIX_ADSOC_SELECT_ts.mux2to1_select  = 1;
    }
    // Program the crop registers
    // u8_CropHStartOffset = (FrameDimension_GetMaxPreScale() - FrameDimension_GetCurrentPreScaleFactor()) * Zoom_GetFixedLeftXLoss();
    // u8_CropVStartOffset = (FrameDimension_GetMaxPreScale() - FrameDimension_GetCurrentPreScaleFactor()) * Zoom_GetFixedTopYLoss();
    //Program CROP HSTART,HSIZE,VSTART,VSIZE from Pipe_Scalar_StripeOutputs if data is coming from BML
    //Else program it from Pipe_Scalar_Output
    if (SystemConfig_IsInputImageSourceSensor())
    {
        //Crop Programming
        p_ISP_CE_CROP_IP->ISP_CE0_CROP_ENABLE.data = 0x01;
        // <Todo: BG>
        // Here VT[X/Y]AddrStart is not shifted by u8_FOV[X/Y]ShiftInSensor unlike IMD implementation
        // This may cause problem when, (start + size) > actual input to the cropper, since start is shifted by some margin
        // One solution may be not to use start reported by ScalerBorderManager module and use only size
        // But need to check how to support pan/tilt feature.

         OstTraceInt1(TRACE_DEBUG, "<inverse Prescale when cropper is programmed>: %f\n", f_SensorPrescaleInv);

        p_ISP_CE_CROP_IP->ISP_CE0_CROP_H_START.data = Pipe_Scalar_Output->u16_HCropStart -FrameDimension_GetCurrentVTXAddrStart() * f_SensorPrescaleInv;

        p_ISP_CE_CROP_IP->ISP_CE0_CROP_V_START.data = Pipe_Scalar_Output->u16_VCropStart -FrameDimension_GetCurrentVTYAddrStart() * f_SensorPrescaleInv ;

        p_ISP_CE_CROP_IP->ISP_CE0_CROP_H_SIZE.data = Pipe_Scalar_Output->u16_HCropSize;
        p_ISP_CE_CROP_IP->ISP_CE0_CROP_V_SIZE.data = Pipe_Scalar_Output->u16_VCropSize;

        //GPS Programming
        // ISP_CEx_GPSSCALER_H_V_IPSIZES
        p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_IPSIZES.ISP_CE0_GPSSCALER_H_V_IPSIZES_ts.v_input_size = Pipe_Scalar_Output->u16_VCropSize;
        p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_IPSIZES.ISP_CE0_GPSSCALER_H_V_IPSIZES_ts.h_input_size = Pipe_Scalar_Output->u16_HCropSize;

        // ISP_CEx_GPSSCALER_H_V_OPSIZES
        p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_OPSIZES.ISP_CE0_GPSSCALER_H_V_OPSIZES_ts.v_input_size = Pipe_Scalar_Inputs->u16_VScalarOutputSize;
        p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_OPSIZES.ISP_CE0_GPSSCALER_H_V_OPSIZES_ts.h_input_size = Pipe_Scalar_Inputs->u16_HScalarOutputSize;
    }
    else
    {
        //Crop Programming
        p_ISP_CE_CROP_IP->ISP_CE0_CROP_ENABLE.data = 0x01;
        if (Pipe_Scalar_StripeInputs->u8_StripeCount >= 1)
        {
            //Incase of striping output values are stores in Pipe_Scalar_StripeOutputs
            // p_ISP_CE_CROP_IP->ISP_CE0_CROP_H_START.data = u16_HorizontalOffset - FrameDimension_GetCurrentVTXAddrStart();
            // align the crop to the crop applied in the DMA wrt FFOV
            p_ISP_CE_CROP_IP->ISP_CE0_CROP_H_START.data = Pipe_Scalar_StripeOutputs->s16_StripeInCropHStart - u16_MinIntegerInDMA;

            // p_ISP_CE_CROP_IP->ISP_CE0_CROP_V_START.data = Pipe_Scalar_StripeOutputs->s16_StripeInCropVStart - FrameDimension_GetCurrentVTYAddrStart();
            // nothing to crop vertically, since the DMA will already do a readout aligned to Pipe_Scalar_StripeOutputs->s16_StripeInCropVStart
            p_ISP_CE_CROP_IP->ISP_CE0_CROP_V_START.data = 0;

            p_ISP_CE_CROP_IP->ISP_CE0_CROP_H_SIZE.data = Pipe_Scalar_StripeOutputs->s16_SScalerInputHSize;
            p_ISP_CE_CROP_IP->ISP_CE0_CROP_V_SIZE.data = Pipe_Scalar_StripeOutputs->s16_SScalerInputVSize;

            //GPS Programming
            // ISP_CEx_GPSSCALER_H_V_IPSIZES
            p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_IPSIZES.ISP_CE0_GPSSCALER_H_V_IPSIZES_ts.v_input_size = Pipe_Scalar_StripeOutputs->s16_SScalerInputVSize;
            p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_IPSIZES.ISP_CE0_GPSSCALER_H_V_IPSIZES_ts.h_input_size = Pipe_Scalar_StripeOutputs->s16_SScalerInputHSize;

            // ISP_CEx_GPSSCALER_H_V_OPSIZES
            p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_OPSIZES.ISP_CE0_GPSSCALER_H_V_OPSIZES_ts.v_input_size = Pipe_Scalar_StripeOutputs->s16_StripeScalarOutputVSize;
            p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_OPSIZES.ISP_CE0_GPSSCALER_H_V_OPSIZES_ts.h_input_size = Pipe_Scalar_StripeOutputs->s16_StripeScalarOutputHSize;

            //Incase of Striping just add Fixed Loss into Stripe Input Parameters to make all calculations correct
            //Pipe_Scalar_StripeOutputs->s16_StripeInCropHSize += (Zoom_GetFixedLeftXLoss() + Zoom_GetFixedRightXLoss() + u16_HorizontalOffset);
            // Pipe_Scalar_StripeOutputs->s16_StripeInCropHSize += u16_HorizontalOffset;
            Pipe_Scalar_StripeOutputs->s16_StripeInCropHSize += Pipe_Scalar_StripeOutputs->s16_StripeInCropHStart - u16_MinIntegerInDMA;

            // align Pipe_Scalar_StripeOutputs->s16_StripeInCropHSize to u8_MinNoOfPixPickedInOneGoFromDma i.e. DMA pixel boundary
            u16_Value = Pipe_Scalar_StripeOutputs->s16_StripeInCropHSize % u8_MinNoOfPixPickedInOneGoFromDma;

            if (u16_Value != 0)
            {
                Pipe_Scalar_StripeOutputs->s16_StripeInCropHSize += (u8_MinNoOfPixPickedInOneGoFromDma - u16_Value);
            }


            // Pipe_Scalar_StripeOutputs->s16_StripeInCropHStart = u16_MinIntegerInDMA_0;
            Pipe_Scalar_StripeOutputs->s16_StripeInCropHStart = u16_MinIntegerInDMA;
        }
        else
        {
            if(Flag_e_FALSE == g_SystemConfig_Status.e_Flag_FastBML)
            {
                    //Incase of not striping output values are stores in Pipe_Scalar_Output
                    p_ISP_CE_CROP_IP->ISP_CE0_CROP_H_START.data = Pipe_Scalar_Output->u16_HCropStart -
                        (
                            (
                                FrameDimension_GetCurrentVTXAddrStart() -
                                FrameDimension_GetFOVXShiftAppliedInSensor() +
                                FrameDimension_GetFOVXShiftToBeAppliedInISP()
                            ) * f_SensorPrescaleInv
                        );
                    p_ISP_CE_CROP_IP->ISP_CE0_CROP_V_START.data = Pipe_Scalar_Output->u16_VCropStart -
                        (
                            (
                                FrameDimension_GetCurrentVTYAddrStart() -
                                FrameDimension_GetFOVYShiftAppliedInSensor() +
                                FrameDimension_GetFOVYShiftToBeAppliedInISP()
                            ) * f_SensorPrescaleInv
                        );
                }
            else
            {
                    //Currently we are not cropping in X.
                    p_ISP_CE_CROP_IP->ISP_CE0_CROP_H_START.data = Pipe_Scalar_Output->u16_HCropStart -
                        (
                            (
                                FrameDimension_GetCurrentVTXAddrStart() -
                                FrameDimension_GetFOVXShiftAppliedInSensor() +
                                FrameDimension_GetFOVXShiftToBeAppliedInISP()
                            ) * f_SensorPrescaleInv
                        );

                    // < Todo: BG> no boundary check is done
                    p_ISP_CE_CROP_IP->ISP_CE0_CROP_V_START.data = Pipe_Scalar_Output->u16_VCropStart - g_BML_Framedimension.u16_BMLYOffset;
            }
            p_ISP_CE_CROP_IP->ISP_CE0_CROP_H_SIZE.data = Pipe_Scalar_Output->u16_HCropSize;
            p_ISP_CE_CROP_IP->ISP_CE0_CROP_V_SIZE.data = Pipe_Scalar_Output->u16_VCropSize;

            //GPS Programming
            // ISP_CEx_GPSSCALER_H_V_IPSIZES
            p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_IPSIZES.ISP_CE0_GPSSCALER_H_V_IPSIZES_ts.v_input_size = Pipe_Scalar_Output->u16_VCropSize;
            p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_IPSIZES.ISP_CE0_GPSSCALER_H_V_IPSIZES_ts.h_input_size = Pipe_Scalar_Output->u16_HCropSize;

            // ISP_CEx_GPSSCALER_H_V_OPSIZES
            p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_OPSIZES.ISP_CE0_GPSSCALER_H_V_OPSIZES_ts.v_input_size = Pipe_Scalar_Inputs->u16_VScalarOutputSize;
            p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_OPSIZES.ISP_CE0_GPSSCALER_H_V_OPSIZES_ts.h_input_size = Pipe_Scalar_Inputs->u16_HScalarOutputSize;
        }
    }


    // TODO: Program the correct value into the IACTIVE pixels and lines.
    // The current programming to 0xFFFF will simply make it useless.
    p_ISP_CE_CROP_IP->ISP_CE0_CROP_IACTIVE_PIXS.data = 0xFFFF;
    p_ISP_CE_CROP_IP->ISP_CE0_CROP_IACTIVE_LINES.data = 0xFFFF;

    // Program the GPS registers
    // ISP_CEx_GPSSCALER_ENABLE
//    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_ENABLE.word = ((uint32_t) 1 << 0 | (uint32_t) 1 << 1 | (uint32_t) 0 << 4);

    // ISP_CEx_GPSSCALER_DOWNSCALING
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_DOWNSCALING.word = Pipe_Scalar_Output->e_Flag_DownScaling;

    // ISP_CEx_GPSSCALER_LR_BORD_PIXS
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_LR_BORD_PIXS.ISP_CE0_GPSSCALER_LR_BORD_PIXS_ts.h_left_border_pixs = Pipe_Scalar_Output->u16_LeftBorder;
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_LR_BORD_PIXS.ISP_CE0_GPSSCALER_LR_BORD_PIXS_ts.h_right_border_pixs = Pipe_Scalar_Output->u16_RightBorder;

    // ISP_CEx_GPSSCALER_TB_BORD_LINES
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_TB_BORD_LINES.ISP_CE0_GPSSCALER_TB_BORD_LINES_ts.v_top_border_lines = Pipe_Scalar_Output->u16_TopBorder;
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_TB_BORD_LINES.ISP_CE0_GPSSCALER_TB_BORD_LINES_ts.v_bottom_border_lines = Pipe_Scalar_Output->u16_BottomBorder;

    // ISP_CEx_GPSSCALER_H_V_FIRTAPS
    if (Flag_e_TRUE == Pipe_Scalar_Output->e_Flag_DownScaling)
    {
        u8_FirTaps = Pipe_Scalar_Output->u8_NSize;
    }
    else
    {
        u8_FirTaps = u8_MinimumLineBlankingBeyondScalar;
    }


    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_FIRTAPS.ISP_CE0_GPSSCALER_H_V_FIRTAPS_ts.v_fir_taps = u8_FirTaps;
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_FIRTAPS.ISP_CE0_GPSSCALER_H_V_FIRTAPS_ts.h_fir_taps = u8_FirTaps;

    // ISP_CEx_GPSSCALER_H_V_LOGPHYSPOLYS
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_LOGPHYSPOLYS.ISP_CE0_GPSSCALER_H_V_LOGPHYSPOLYS_ts.v_log_phys_polys =
            Pipe_Scalar_Output->u8_LogMSize -
        1;
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_LOGPHYSPOLYS.ISP_CE0_GPSSCALER_H_V_LOGPHYSPOLYS_ts.h_log_phys_polys =
            Pipe_Scalar_Output->u8_LogMSize -
        1;

    u16_VBulk = (uint16_t) (Pipe_Scalar_Output->f_VGPSCrop);
    u16_VFrac = (uint16_t) ((Pipe_Scalar_Output->f_VGPSCrop * 0x10000) + 0.5);

    u16_HBulk = (uint16_t) (Pipe_Scalar_Output->f_HGPSCrop);
    u16_HFrac = (uint16_t) ((Pipe_Scalar_Output->f_HGPSCrop * 0x10000) + 0.5);

    // ISP_CEx_GPSSCALER_H_V_KICKOFF_DLY
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_KICKOFF_DLY.ISP_CE0_GPSSCALER_H_V_KICKOFF_DLY_ts.v_kickoff_delay = u16_VBulk;
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_KICKOFF_DLY.ISP_CE0_GPSSCALER_H_V_KICKOFF_DLY_ts.h_kickoff_delay = u16_HBulk;

    // ISP_CEx_GPSSCALER_H_V_CROPFRAC
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_CROPFRAC.ISP_CE0_GPSSCALER_H_V_CROPFRAC_ts.v_crop_frac = u16_VFrac;
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_CROPFRAC.ISP_CE0_GPSSCALER_H_V_CROPFRAC_ts.h_crop_frac = u16_HFrac;

    u16_VBulk = (uint16_t) (Pipe_Scalar_Inputs->f_Step);
    u16_VFrac = (uint16_t) ((Pipe_Scalar_Inputs->f_Step * 0x10000) + 0.5);

    // ISP_CEx_GPSSCALER_H_V_STEPBULK
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_STEPBULK.ISP_CE0_GPSSCALER_H_V_STEPBULK_ts.v_step_bulk = u16_VBulk;
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_STEPBULK.ISP_CE0_GPSSCALER_H_V_STEPBULK_ts.h_step_bulk = u16_VBulk;

    // ISP_CEx_GPSSCALER_H_V_STEPFRAC
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_STEPFRAC.ISP_CE0_GPSSCALER_H_V_STEPFRAC_ts.v_step_frac = u16_VFrac;
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_H_V_STEPFRAC.ISP_CE0_GPSSCALER_H_V_STEPFRAC_ts.h_step_frac = u16_VFrac;

    // ISP_CEx_GPSSCALER_HCOF
    // The hardware registers are arranged in the following manner in mcu memory space:
    // cof3, cof2, cof1, cof0, cof7, cof6, cof5, cof4...
    // while the cofs in memory are in the following manner:
    // cof0, cof1, cof2, cof3, cof4, cof5, cof6, cof7...
    for (u8_CofCount = 0; u8_CofCount < Pipe_Scalar_Output->u8_CofCount; u8_CofCount++)
    {
        if (!(u8_CofCount & 0x03))
        {
            p_HCofHwReg = (uint8_t *) (&p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_HCOF_0_3.data) + u8_CofCount + 3;
            p_VCofHwReg = (uint8_t *) (&p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_VCOF_0_3.data) + u8_CofCount + 3;
        }


        *p_HCofHwReg = (uint8_t) (*(Pipe_Scalar_Output->ptrs16_Cof + u8_CofCount));
        *p_VCofHwReg = (uint8_t) (*(Pipe_Scalar_Output->ptrs16_Cof + u8_CofCount));

        p_HCofHwReg -= 1;
        p_VCofHwReg -= 1;
    }

    // Program the GPS registers
    // ISP_CEx_GPSSCALER_ENABLE
    p_ISP_CE_GPSSCALER_IP->ISP_CE0_GPSSCALER_ENABLE.word = ((uint32_t) 1 << 0 | (uint32_t) 1 << 1 | (uint32_t) 0 << 4);

    return;
}


/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void ZoomTop_PipeUpdate( uint8_t u8_PipeNo )
 \brief     Top level function to programs the scalar, cropper and other related IP blocks for the current zoom step
 \param     u8_PipeNo : Pipe to be updated
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
void
ZoomTop_PipeUpdate(
uint8_t u8_PipeNo)
{
    /// \par Implementation
        /// - Generates the base addresses of the related IP blocks to be updated corresponding to the pipe being updated.
        /// - Invokes ProgramHardware function with the base addresses of the related IP blocks corresponding to the
        /// pipe being updated.
        /// Increments g_ZoomTop_ParamAppicationControl.u8_ScalarParamsApplied to count the application of a zoom step to a pipe.
        /// If g_ZoomTop_ParamAppicationControl.u8_ScalarParamsApplied == g_ZoomTop_ParamAppicationControl.u8_ScalarParamsGenerated
        /// then it indicates the completion of a zoom step to the zoom manager.
    if
    (
        g_ZoomTop_ParamAppicationControl.u8_ScalarParamsApplied != g_ZoomTop_ParamAppicationControl.
            u8_ScalarParamsGenerated
    )
    {
        // new scalar params are ready to be applied...
        // program pipe scalar
        ProgramHardware(u8_PipeNo);

        // Program OPF here before enabling IDP gate for CE
        ProgramOPF(u8_PipeNo);

        g_ZoomTop_ParamAppicationControl.u8_ScalarParamsApplied += 1;

        if
        (
            g_ZoomTop_ParamAppicationControl.u8_ScalarParamsApplied == g_ZoomTop_ParamAppicationControl.
                u8_ScalarParamsGenerated
        )
        {
            // in case of a refresh output size command, wait for the host ACK before enabling the
            // the pipe output
            // for all other zoom commands, the pipe can be enabled without waiting
            if
            (
                (ZoomCommand_e_RefreshOutputSize == g_Zoom_CommandStatus.e_ZoomCommand)
            ||  (Flag_e_TRUE == g_Zoom_Control.e_Flag_PerformDynamicResolutionUpdate)
            )
            {
                //while (g_ZoomTop_ParamAppicationControl.u8_RefreshOutputSizeControlCount == g_ZoomTop_ParamAppicationControl.u8_RefreshOutputSizeStatusCount);
                while (!g_GrabNotify.u32_DMA_GRAB_Indicator_For_VideoStab)
                    ;

                // host has given a go ahead for enabling the pipe, set both the counts to be the same
                g_ZoomTop_ParamAppicationControl.u8_RefreshOutputSizeStatusCount = g_ZoomTop_ParamAppicationControl.u8_RefreshOutputSizeControlCount;
            }
#ifdef PROFILER_USING_XTI
    OstTraceInt0(TRACE_USER8, "stop_xti_profiler_zoom");
#endif

            Zoom_StepComplete();
        }
    }


    return;
}


void ProgramOPF(uint8_t u8_PipeNo)
{
    uint8_t     u8_TogglePixValid;
    uint8_t     u8_CbCrFlip;
    uint8_t     u8_YCbCrFlip;
    uint8_t     u8_OpfOutput;
    uint8_t     u8_RGB444Format;
    uint16_t    u16_VsyncRisingCoarse;

    if (0 == u8_PipeNo)
    {
        // vsync_rising_coarse is computed as follows:
        // vsync_rising_coarse = non_active_lines_at_frame_top + constant_loss_at_frame_top + crop_at_frame_top
        // TODO<RU>: Extract the Get_ISP_CE0_CROP_ISP_CE0_CROP_V_START information from Zoom
        u16_VsyncRisingCoarse = FrameDimension_GetNumberOfNonActiveLinesAtFrameTop() +
            ROWS_LOST_AT_TOP_EDGE_DUE_TO_COLOUR_INTERPOLATION +
            ROWS_LOST_AT_BOTTOM_EDGE_DUE_TO_COLOUR_INTERPOLATION +
            Get_ISP_CE0_CROP_ISP_CE0_CROP_V_START();

        if (g_Pipe_Scalar_StripeInputs[0].u8_StripeCount >= 1)
        {
            //Striping is enable
            //There should be some change for striping here
            Set_ISP_OPF0_ISP_OPF0_HSYNC_RISING(g_Pipe_Scalar_StripeOutputs[0].s16_StripeOutCropHStart); // hsync_rising
            Set_ISP_OPF0_ISP_OPF0_HSYNC_FALLING(((g_Pipe_Scalar_StripeOutputs[0].s16_StripeOutCropHStart + g_Pipe_Scalar_StripeOutputs[0].s16_StripeOutCropHSize))); // hsync_falling
            Set_ISP_OPF0_ISP_OPF0_VSYNC_RISING_FINE(0); // vsync_rising_fine
            Set_ISP_OPF0_ISP_OPF0_VSYNC_FALLING_FINE((g_Pipe_Scalar_StripeOutputs[0].s16_StripeOutCropHStart + g_Pipe_Scalar_StripeOutputs[0].s16_StripeOutCropHSize));    // vsync_falling_fine
            Set_ISP_OPF0_ISP_OPF0_HCROP_RISING(g_Pipe_Scalar_StripeOutputs[0].s16_StripeOutCropHStart);         // hcrop_rising
            Set_ISP_OPF0_ISP_OPF0_HCROP_FALLING((g_Pipe_Scalar_StripeOutputs[0].s16_StripeOutCropHStart + g_Pipe_Scalar_StripeOutputs[0].s16_StripeOutCropHSize - 1)); // hcrop_falling
            Set_ISP_OPF0_ISP_OPF0_VCROP_RISING_COARSE(0);                                   // vcrop_rising_coarse
            Set_ISP_OPF0_ISP_OPF0_VCROP_FALLING_COARSE(0xFFFF);                             // vcrop_falling_coarse
        }
        else
        {
            Set_ISP_OPF0_ISP_OPF0_HSYNC_RISING(0);                                          // hsync_rising
            Set_ISP_OPF0_ISP_OPF0_HSYNC_FALLING(Zoom_GetPipe0OpSizeX());                    // hsync_falling
            Set_ISP_OPF0_ISP_OPF0_VSYNC_RISING_FINE(0);                                     // vsync_rising_fine
            Set_ISP_OPF0_ISP_OPF0_VSYNC_FALLING_FINE(Zoom_GetPipe0OpSizeX());               // vsync_falling_fine
            Set_ISP_OPF0_ISP_OPF0_HCROP_RISING(0);                                          // hcrop_rising
            //Set_ISP_OPF0_ISP_OPF0_HCROP_FALLING(0xFFFF);                                    // hcrop_falling
            Set_ISP_OPF0_ISP_OPF0_HCROP_FALLING(Zoom_GetPipe0OpSizeX()-1);
            Set_ISP_OPF0_ISP_OPF0_VCROP_RISING_COARSE(0);                                   // vcrop_rising_coarse
            Set_ISP_OPF0_ISP_OPF0_VCROP_FALLING_COARSE(0xFFFF);                             // vcrop_falling_coarse
        }


        //If Upscale Case and Top Border Value is 0x3ff then increment this VSyncRisingCoarse. Bug Fix legacy from our 98x.
        if (SystemConfig_GetPipe0UpScaling() && SystemConfig_GetPipe0TopBorders())
        {
            u16_VsyncRisingCoarse++;
        }


        Set_ISP_OPF0_ISP_OPF0_VSYNC_RISING_COARSE(u16_VsyncRisingCoarse);

        // now we want the vsync coarse signal to fall at the begining of the last active line
        // the last line is validated by vsync_falling_fine
        Set_ISP_OPF0_ISP_OPF0_VSYNC_FALLING_COARSE((Zoom_GetPipe0OpSizeY() + u16_VsyncRisingCoarse - 1));

        // Set_ISP_OPF0_ISP_OPF0_VSYNC_FALLING_COARSE(0xFFFF);  // vsync_falling_coarse, let the vsync qualify the complete frame,
        // the pix valid toggle will validate the pixels
        u8_TogglePixValid = SystemConfig_IsPipe0TogglePixValid();
        u8_CbCrFlip = SystemConfig_IsPipe0CbCrFlip();
        u8_YCbCrFlip = SystemConfig_IsPipe0YCbCrFlip();

        u8_OpfOutput = SystemConfig_GetPipe0OutputFormat();
        if (SystemConfig_IsPipe0OuptutPacked())
        {
            u8_RGB444Format = rgb444_format_RGB444_PACKED;
        }
        else
        {
            u8_RGB444Format = rgb444_format_RGB444_NOPACK;
        }


        // sync_based_pixvalid,
        // toggle_pix_valid,
        // rgb_flip,
        // rgb_bit_flip,
        // embedded_code_en,
        // false_sync_code_protect_en,
        // invert_hsync,
        // invert_vsync,
        // idata_ch0_en,
        // idata_ch1_en,
        // idata_ch2_en,
        // opf_output,
        // rgb444_format
        Set_ISP_OPF0_ISP_OPF0_CONTROL(0, u8_TogglePixValid, 0, 0, 0, 0, 0, 0, 1, 1, 1, u8_OpfOutput, u8_RGB444Format);
        Set_ISP_OPF0_ISP_OPF0_PIXVALID_LTYPES(SystemConfig_GetPipe0PixValidLineTypes(), 0); // pixvalid_linetypes,eof_blank_only_en
        Set_ISP_OPF0_ISP_OPF0_YCBCR_FLIP(u8_CbCrFlip, u8_YCbCrFlip);
        
        // ISP_OPFx
        Set_ISP_OPF0_ISP_OPF0_ENABLE(1, 0); // enable,soft_reset

    }
    else if (1 == u8_PipeNo)
    {
        // vsync_rising_coarse is computed as follows:
        // vsync_rising_coarse = non_active_lines_at_frame_top + constant_loss_at_frame_top + crop_at_frame_top
        // TODO<RU>: Extract the Get_ISP_CE1_CROP_ISP_CE1_CROP_V_START information from Zoom
        u16_VsyncRisingCoarse = FrameDimension_GetNumberOfNonActiveLinesAtFrameTop() +
            ROWS_LOST_AT_TOP_EDGE_DUE_TO_COLOUR_INTERPOLATION +
            ROWS_LOST_AT_BOTTOM_EDGE_DUE_TO_COLOUR_INTERPOLATION +
            Get_ISP_CE1_CROP_ISP_CE1_CROP_V_START();

        if (g_Pipe_Scalar_StripeInputs[1].u8_StripeCount >= 1)
        {
            //Striping is enable
            //There should be some change for striping here
            Set_ISP_OPF1_ISP_OPF1_HSYNC_RISING(g_Pipe_Scalar_StripeOutputs[1].s16_StripeOutCropHStart);                         // hsync_rising
            Set_ISP_OPF1_ISP_OPF1_HSYNC_FALLING((g_Pipe_Scalar_StripeOutputs[1].s16_StripeOutCropHStart + g_Pipe_Scalar_StripeOutputs[1].s16_StripeOutCropHSize));    // hsync_falling
            Set_ISP_OPF1_ISP_OPF1_VSYNC_RISING_FINE(0); // vsync_rising_fine
            Set_ISP_OPF1_ISP_OPF1_VSYNC_FALLING_FINE((g_Pipe_Scalar_StripeOutputs[1].s16_StripeOutCropHStart + g_Pipe_Scalar_StripeOutputs[1].s16_StripeOutCropHSize));    // vsync_falling_fine
            Set_ISP_OPF1_ISP_OPF1_HCROP_RISING(g_Pipe_Scalar_StripeOutputs[1].s16_StripeOutCropHStart);         // hcrop_rising
            Set_ISP_OPF1_ISP_OPF1_HCROP_FALLING((g_Pipe_Scalar_StripeOutputs[1].s16_StripeOutCropHStart + g_Pipe_Scalar_StripeOutputs[1].s16_StripeOutCropHSize - 1)); // hcrop_falling
            Set_ISP_OPF1_ISP_OPF1_VCROP_RISING_COARSE(0);                                   // vcrop_rising_coarse
            Set_ISP_OPF1_ISP_OPF1_VCROP_FALLING_COARSE(0xFFFF);                             // vcrop_falling_coarse
        }
        else
        {
            Set_ISP_OPF1_ISP_OPF1_HSYNC_RISING(0);                                          // hsync_rising
            Set_ISP_OPF1_ISP_OPF1_HSYNC_FALLING(Zoom_GetPipe1OpSizeX());                    // hsync_falling
            Set_ISP_OPF1_ISP_OPF1_VSYNC_RISING_FINE(0);                                     // vsync_rising_fine
            Set_ISP_OPF1_ISP_OPF1_VSYNC_FALLING_FINE(Zoom_GetPipe1OpSizeX());               // vsync_falling_fine
            Set_ISP_OPF1_ISP_OPF1_HCROP_RISING(0);                                          // hcrop_rising
            //Set_ISP_OPF1_ISP_OPF1_HCROP_FALLING(0xFFFF);                                    // hcrop_falling
	    Set_ISP_OPF1_ISP_OPF1_HCROP_FALLING(Zoom_GetPipe1OpSizeX()-1);
            Set_ISP_OPF1_ISP_OPF1_VCROP_RISING_COARSE(0);                                   // vcrop_rising_coarse
            Set_ISP_OPF1_ISP_OPF1_VCROP_FALLING_COARSE(0xFFFF);                             // vcrop_falling_coarse
        }


        //If Upscale Case and Top Border Value is 0x3ff then increment this VSyncRisingCoarse. Bug Fix legacy from our 98x.
        if (SystemConfig_GetPipe1UpScaling() && SystemConfig_GetPipe1TopBorders())
        {
            u16_VsyncRisingCoarse++;
        }


        Set_ISP_OPF1_ISP_OPF1_VSYNC_RISING_COARSE(u16_VsyncRisingCoarse);

        // now we want the vsync coarse signal to fall at the begining of the last active line
        // the last line is validated by vsync_falling_fine
        Set_ISP_OPF1_ISP_OPF1_VSYNC_FALLING_COARSE((Zoom_GetPipe1OpSizeY() + u16_VsyncRisingCoarse - 1));

        // Set_ISP_OPF1_ISP_OPF1_VSYNC_FALLING_COARSE(0xFFFF);  // vsync_falling_coarse, let the vsync qualify the complete frame,
        // the pix valid toggle will validate the pixels
        u8_TogglePixValid = SystemConfig_IsPipe1TogglePixValid();
        u8_CbCrFlip = SystemConfig_IsPipe1CbCrFlip();
        u8_YCbCrFlip = SystemConfig_IsPipe1YCbCrFlip();

        u8_OpfOutput = SystemConfig_GetPipe1OutputFormat();
        if (SystemConfig_IsPipe1OuptutPacked())
        {
            u8_RGB444Format = rgb444_format_RGB444_PACKED;
        }
        else
        {
            u8_RGB444Format = rgb444_format_RGB444_NOPACK;
        }


        // sync_based_pixvalid,
        // toggle_pix_valid,
        // rgb_flip,
        // rgb_bit_flip,
        // embedded_code_en,
        // false_sync_code_protect_en,
        // invert_hsync,
        // invert_vsync,
        // idata_ch0_en,
        // idata_ch1_en,
        // idata_ch2_en,
        // opf_output,
        // rgb444_format
        Set_ISP_OPF1_ISP_OPF1_CONTROL(0, u8_TogglePixValid, 0, 0, 0, 0, 0, 0, 1, 1, 1, u8_OpfOutput, u8_RGB444Format);
        Set_ISP_OPF1_ISP_OPF1_PIXVALID_LTYPES(SystemConfig_GetPipe1PixValidLineTypes(), 0); // pixvalid_linetypes,eof_blank_only_en
        Set_ISP_OPF1_ISP_OPF1_YCBCR_FLIP(u8_CbCrFlip, u8_YCbCrFlip);
  
        // ISP_OPFx
        Set_ISP_OPF1_ISP_OPF1_ENABLE(1, 0); // enable,soft_reset
  }
}
/**
 \if INCLUDE_IN_HTML_ONLY
 \fn        void PrepareScalarInputs(Zoom_PipeState_ts * p_PipeState, Scalar_Inputs_ts * p_Scalar_Inputs, Scalar_Stripe_InputParams_ts *p_Scalar_Stripe_Inputs)
 \brief     Function to generate the input parameters for current zoom step computation corresponding to p_PipeState
 \param     p_PipeState : Pointer to the pipe state structure for the pipe being updated.
            p_Scalar_Inputs : This is an output argument for this function. The function generates the scalar inputs
            and stores in this structure.
            p_Scalar_Stripe_Inputs: Scalar Stripe Inputs
 \return    None
 \ingroup   Zoom
 \callgraph
 \callergraph
 \endif
*/
void
PrepareScalarInputs(
Zoom_PipeState_ts               *p_PipeState,
Scalar_Inputs_ts                *p_Scalar_Inputs,
Scalar_Stripe_InputParams_ts    *p_Scalar_Stripe_Inputs)
{
    float_t f_SensorPrescaleInv;
    float_t f_Step_D2;
    float_t f_FOVX_D2;
    float_t f_FOVY_D2;

    float_t f_XCenter;
    float_t f_YCenter;

    // pipe0 is enabled, generate its input parameters
    //
    // The pipe state has the total step required to scale from FOV to output
    // The pre scale factor if any in the sensor must also be factored

    /* TODO: <LLA> <AG>: The step will be different as we are trying to hid pre-scale done in sensor from the code */
    f_SensorPrescaleInv = 1.0 / FrameDimension_GetCurrentPreScaleFactor();
    p_Scalar_Inputs->f_Step = p_PipeState->f_Step * f_SensorPrescaleInv;

    f_Step_D2 = p_Scalar_Inputs->f_Step * 0.5;

    // f_FOVX_D2 is the center of the pipe FOVX (0 based)
    f_FOVX_D2 = (p_PipeState->f_FOVX - 1) * 0.5;

    // f_FOVX_D2 is the center of the pipe FOVY (0 based)
    f_FOVY_D2 = (p_PipeState->f_FOVY - 1) * 0.5;

    // Compute the top level H and V crop. It is the distance between
    // the array center and the center of the pipe FOV
    // The pre scale factor in the sensor must be factored while computing
    // the H and V crop
    f_XCenter = g_Zoom_Status.f_XCenter + ((FrameDimension_GetMaxPreScale() - FrameDimension_GetCurrentPreScaleFactor()) * Zoom_GetFixedLeftXLoss());
    f_YCenter = g_Zoom_Status.f_YCenter + ((FrameDimension_GetMaxPreScale() - FrameDimension_GetCurrentPreScaleFactor()) * Zoom_GetFixedTopYLoss());

    p_Scalar_Inputs->f_HCrop = ((f_XCenter - f_FOVX_D2) * f_SensorPrescaleInv) + f_Step_D2;
    p_Scalar_Inputs->f_VCrop = ((f_YCenter - f_FOVY_D2) * f_SensorPrescaleInv) + f_Step_D2;

    if (p_Scalar_Stripe_Inputs->u8_StripeCount >= 1)
    {
        p_Scalar_Inputs->u16_HCropperInputSize = FrameDimension_GetCurrentOPXOutputSize() - (Zoom_GetFixedLeftXLoss() + Zoom_GetFixedRightXLoss());
        p_Scalar_Inputs->u16_VCropperInputSize = FrameDimension_GetCurrentOPYOutputSize() - (Zoom_GetFixedTopYLoss() + Zoom_GetFixedBottomYLoss());
    }
    else
    {
        p_Scalar_Inputs->u16_HCropperInputSize =
                (
                    FrameDimension_GetMaxSensorFOVXAtCurrentParam() *
                    f_SensorPrescaleInv
                ) -
                (Zoom_GetFixedLeftXLoss() + Zoom_GetFixedRightXLoss());
        p_Scalar_Inputs->u16_VCropperInputSize =
                (
                    FrameDimension_GetMaxSensorFOVYAtCurrentParam() *
                    f_SensorPrescaleInv
                ) -
                (Zoom_GetFixedTopYLoss() + Zoom_GetFixedBottomYLoss());
    }


    p_Scalar_Inputs->u16_HScalarOutputSize = p_PipeState->u16_OutputSizeX;
    p_Scalar_Inputs->u16_VScalarOutputSize = p_PipeState->u16_OutputSizeY;

    return;
}

// Get FOV based on configured pipe and datapath
uint16_t
Zoom_GetFOVX(void)
{
    return (g_Zoom_Status.f_FOVX);
}


uint16_t
Zoom_GetFOVY(void)
{
    return (g_Zoom_Status.f_FOVY);
}


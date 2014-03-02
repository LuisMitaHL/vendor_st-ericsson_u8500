/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      ZoomTop.h
 \brief     Implements the top level zoom interface. It uses the low level
            zoom module to implement and manage sensor changeover, application
            of each zoom step, setting up of scalars for the first streaming
            step, programming of IP blocks not related to zoom (like MUX for
            different data paths)

 \ingroup   Zoom
 \endif
*/
#ifndef ZOOMTOP_H_
#   define ZOOMTOP_H_

#   include "Platform.h"
#   include "Scalar_OPInterface.h"
#   include "ZoomPlatformSpecific.h"

/**
 \struct    ZoomTop_ParamAppicationControl_ts
 \brief     Data structure used to manage and synchronise the application
            of scalar parameters in context of the zoom step.
 \ingroup   Zoom
*/
typedef struct
{
    /// Used as a counter to indicate the number of times the scalar params
    /// have been generated. For dual pipe operation, it is incremented
    /// by 2 for each zoom step and for a single pipe operation, it is
    /// incremented by 1 for each zoom step. The host should not write onto this element.
    uint8_t u8_ScalarParamsGenerated;

    /// Used as a counter to indicate the number of time the scalar params
    /// have been applied onto the hardware. For dual pipe operation, it is incremented
    /// by 2 for each zoom step and for a single pipe operation, it is
    /// incremented by 1 for each zoom step. Whenever a zoom step has been fully
    /// absorbed, u8_ScalarParamsApplied == u8_ScalarParamsGenerated.
    /// The host should not write onto this element.
    uint8_t u8_ScalarParamsApplied;

    /// Control counter used by the host to signal the device to enable the
    /// pipe output after a ZoomCommand_e_RefreshOutputSize command.
    /// Whenever the control counter is different from the status counter
    /// the device will enable the pipe. It is ignored for all other zoom commands.
    uint8_t u8_RefreshOutputSizeControlCount;

    /// Status counter used by the host to signal the device to enable the
    /// pipe output after a ZoomCommand_e_RefreshOutputSize command.
    /// Whenever the control counter is different from the status counter
    /// the device will enable the pipe. It is ignored for all other zoom commands.
    uint8_t u8_RefreshOutputSizeStatusCount;
} ZoomTop_ParamAppicationControl_ts;

extern Scalar_UserParams_ts                         g_Pipe_Scalar_UserParams[PIPE_COUNT];
extern Scalar_Inputs_ts                             g_Pipe_Scalar_Inputs[PIPE_COUNT];
extern Scalar_Output_ts                             g_Pipe_Scalar_Output[PIPE_COUNT];
extern Scalar_Stripe_InputParams_ts                 g_Pipe_Scalar_StripeInputs[PIPE_COUNT];
extern Scalar_Stripe_OutputParams_ts                g_Pipe_Scalar_StripeOutputs[PIPE_COUNT];
extern volatile ZoomTop_ParamAppicationControl_ts   g_ZoomTop_ParamAppicationControl;

extern void ZoomTop_Initialise ( void ) TO_EXT_DDR_PRGM_MEM;
extern uint8_t ZoomTop_PreRunUpdate ( void ) TO_EXT_DDR_PRGM_MEM;
extern void ZoomTop_FDMRequestAccepted ( void ) TO_EXT_DDR_PRGM_MEM;
extern void ZoomTop_Run ( void ) TO_EXT_DDR_PRGM_MEM;
extern void ZoomTop_PipeUpdate (uint8_t u8_PipeNo) TO_EXT_DDR_PRGM_MEM;

/// Interface to allow the zoom to know if pipe0 has been enabled
#   define Zoom_GetPipe0Enabled()  SystemConfig_IsPipe0Active()

/// Interface to allow the zoom to know if pipe1 has been enabled
#   define Zoom_GetPipe1Enabled()  SystemConfig_IsPipe1Active()

#endif /*ZOOMTOP_H_*/


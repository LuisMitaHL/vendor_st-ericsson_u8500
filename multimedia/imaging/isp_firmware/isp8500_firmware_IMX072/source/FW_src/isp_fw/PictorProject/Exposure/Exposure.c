/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \file    Exposure.c
 * \brief   Contains the Exposure Algorithm and Exposure Compiler Functions
 * \ingroup ExpCtrl
 * \endif
*/
#include "Exposure.h"
#include "Exposure_OPInterface.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_Exposure_ExposureTraces.h"
#endif


Exposure_CompilerStatus_ts      g_Exposure_CompilerStatus =
{
    EXPOSURE_DEFAULT_STATUS_ANALOG_GAIN_PENDING,
    EXPOSURE_DEFAULT_STATUS_DIGITAL_GAIN_PENDING,
    EXPOSURE_DEFAULT_STATUS_COMPILED_EXPOSURE_TIME_US,
    EXPOSURE_DEFAULT_STATUS_TOTAL_INTEGRATION_TIME_US,
    EXPOSURE_DEFAULT_STATUS_COARSE_INTEGRATION_PENDING_LINES,
    EXPOSURE_DEFAULT_STATUS_FINE_INTEGRATION_PENDING_PIXELS,
    EXPOSURE_DEFAULT_STATUS_CODED_ANALOG_GAIN_PENDING,
    0 
};

/// Initialization of the Exposure Driver Controls Page.
Exposure_DriverControls_ts      g_Exposure_DriverControls =
{
    EXPOSURE_DEFAULT_DRIVER_MANUAL_TOTAL_TARGET_EXPOSURE_TIME_US,
    EXPOSURE_DEFAULT_DRIVER_MANUAL_TARGET_EXPOSURE_TIME_US,
    EXPOSURE_DEFAULT_DRIVER_MANUAL_TARGET_ANALOG_GAIN_x256,
    EXPOSURE_DEFAULT_DRIVER_MANUAL_APERTURE,
    EXPOSURE_DEFAULT_DRIVER_MANUAL_FLASH_STATE,
    EXPOSURE_DEFAULT_DRIVER_MANUAL_DISTANCE_FROM_CONVERGENCE,
    EXPOSURE_DEFAULT_DRIVER_MANUAL_FLAG_ND_FILTER,
    EXPOSURE_DEFAULT_DRIVER_MANUAL_FLAG_AEC_CONVERGED,
};

/// Initialization of the Exposure PArameters Applied Page.
Exposure_ParametersApplied_ts   g_Exposure_ParametersApplied =
{
    EXPOSURE_DEFAULT_PARAMETERS_APPLIED_DIGITAL_GAIN,
    EXPOSURE_DEFAULT_PARAMETERS_APPLIED_TOTAL_INTEGRATION_TIME,
    EXPOSURE_DEFAULT_PARAMETERS_APPLIED_COARSE_INTEGRATION_LINES,
    EXPOSURE_DEFAULT_PARAMETERS_APPLIED_FINE_INTEGRATION_PIXELS,
    EXPOSURE_DEFAULT_PARAMETERS_APPLIED_CODED_ANALOG_GAIN,
};

/// Initialization of the Exposure Error Status Page.
Exposure_ErrorStatus_ts         g_Exposure_ErrorStatus =
{
    EPOSURE_DEFAULT_ERROR_STATUS_NUMBER_OF_FORCED_INPUT_PROC_UPDATES,
    EPOSURE_DEFAULT_ERROR_STATUS_NUMBER_OF_CONSECUTIVE_DELAYED_FRAMES,
    EPOSURE_DEFAULT_ERROR_STATUS_SYNC_ERROR_COUNT,
    EPOSURE_DEFAULT_ERROR_STATUS_FORCE_INPUT_PROC_UPDATION,
};

/// Initialization of the Exposure Error Controls Page.
Exposure_ErrorControl_ts        g_Exposure_ErrorControl =
{
    EXPOSURE_DEFAULT_MAXIMUM_FRAMES_FOR_EXPOSURE_ERROR
};


/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \fn      void Exposure_FrameSyncFailed( void )
 * \brief   Performs those actions that are to be done when the status line exposure
 *          parameters do not match with the parameters sent by exposure control.
 * \return  void
 * \ingroup ExpCtrl
 * \callgraph
 * \callergraph
 * \endif
*/
void Exposure_FrameSyncFailed( void )
{
    // record the number of delays
    (g_Exposure_ErrorStatus.u8_NumberOfConsecutiveDelayedFrames)++;
    (g_Exposure_ErrorStatus.u8_ExposureSyncErrorCount)++;

    /* now if this loop count is greater than 1... it seems like an unwanted delay
       * in the analog gain and integration time...
       * but we compare against the maximum number of frames which are allowed for the delay.
       */
    if (g_Exposure_ErrorStatus.u8_NumberOfConsecutiveDelayedFrames >= g_Exposure_ErrorControl.u8_MaximumNumberOfFrames)
    {
        /*  if greater than the maximum number, increment the counter of ForcedInputProcUpdates
             *  this would give an idea about the number of times such a thing has been done in the system.
             */
        g_Exposure_ErrorStatus.u8_NumberOfForcedInputProcUpdates++;
        g_Exposure_ErrorStatus.u8_NumberOfConsecutiveDelayedFrames = 0;
        g_Exposure_ErrorStatus.e_Flag_ForceInputProcUpdation = Flag_e_TRUE;
        OstTraceInt0(TRACE_WARNING, "<EXP>!!Warning!! timeout");
    }


    return;
}


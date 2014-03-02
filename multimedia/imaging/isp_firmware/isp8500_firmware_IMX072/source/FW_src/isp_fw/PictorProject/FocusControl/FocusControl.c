/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \file    FocusControl.c
 * \brief   It's FocusControl module which basically control the activities and providing
 *            control to various other AF modules.
 * \ingroup FcousControl
 * \endif
*/
#include "FocusControl.h"
#include "FocusControlPlatformSpecific.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_FocusControl_FocusControlTraces.h"
#endif

// include Auto Focus 4 Modules (AutoFocus+AFstats+FocusControl+FLADriver)
#if INCLUDE_FOCUS_MODULES

///  FUNCTION PROTOTYPES
void                        FocusControl_AFStatsReadyISR (void);
void                        FocusControl_SOFISR (void);
void                        FocusControl_BootInitialize (void);
void                        FocusControl_TopLevelInterfaceOnIdle (void);

// GLOBAL VARIABLES
FocusControl_Dummy_ts       g_FocusControl_Dummy = { 0 };

/// Default value for the FocusControl PE , this can be set by the host to perform the specific action .
FocusControl_Controls_ts    g_FocusControl_Controls =
{
    FOCUSCONTROL_DEFAULT_CONTROL_LENS_COMMAND,
    FOCUSCONTROL_DEFAULT_CONTROL_COIN,
    FOCUSCONTROL_DEFAULT_CONTROL_STATS_WITH_LENS_MOVE,
};

/// Default value for the Status of the Focus control PE are shown.
FocusControl_Status_ts      g_FocusControl_Status =
{
    FOCUSCONTROL_DEFAULT_STATUS_LENS_COMMAND,
    FOCUSCONTROL_DEFAULT_STATUS_ERROR_CODE,
    FOCUSCONTROL_DEFAULT_STATUS_COIN,
    FOCUSCONTROL_DEFAULT_STATUS_LENS_IS_MOVING_AT_THE_SOF,
    FOCUSCONTROL_DEFAULT_STATUS_IS_STABLE,
    FOCUSCONTROL_DEFAULT_STATUS_CYCLES,
    FOCUSCONTROL_DEFAULT_STATUS_ERROR,
    FOCUSCONTROL_DEFAULT_STATUS_STATS_WITH_LENS_MOVE,
};

/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FocusControl_AFStatsReadyISR(void)
 * \brief           Interrupt Service Routine for Focus Control and AFStats.
 * \details             As soon as the stats for the Last zone are finished gathering ,
 *              The interrupt service  routine is called , from here FocusControl_Main function is
 *              called .
 * \return          void
 * \ingroup         FocusControl
 * \endif
 */
void
FocusControl_AFStatsReadyISR(void)
{
    //[PM][FIDO Ticket 325360]:Send AFStats irrespective of presence of lens actuator.
    g_FocusControl_Status.u16_Cycles++;

    FocusControl_AFStatsExtractMeanStatistics();
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FocusControl_BootInitialize(void)
 * \brief           Initialization at the time of booting .
 * \details     FocusControl initialization executed at BOOT time:
 *                      usually they may require some input from the host
 *                      and can't be overwritten by the host until the boot has completed
 * \return          void
 * \ingroup         FocusControl
 * \endif
 */
void
FocusControl_BootInitialize(void)
{
    // check if the Actuator Lens is present , if yes call the FLADriver Low Level Function.
    if (FocusControl_IsLensActuatorPresent())
    {
        g_FocusControl_Status.e_Flag_Error = FocusControl_FLADriverInitialize();
        g_FocusControl_Status.e_FocusControl_FocusMsg_Status = FocusControl_FocusMsg_e_NO_ERROR;
        g_FocusControl_Status.u16_Cycles = FOCUSCONTROL_DEFAULT_STATUS_CYCLES;
    }
    else
    {
        g_FLADriver_NVMStoredData.e_Flag_NVMActuatorLensPresent = Flag_e_FALSE;
        g_FLADriver_NVMStoredData.e_Flag_NVMPositionSensorPresent = Flag_e_FALSE;
        g_FocusControl_Status.e_FocusControl_FocusMsg_Status = FocusControl_FocusMsg_e_NO_ACTUATOR_LENS;
    }


    return;
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FocusControl_TopLevelInterfaceOnIdle(void)
 * \brief       Host interface for manual control.It runs in the idle loop.
 * \details
 * \return      void
 * \ingroup     FocusControl
 * \endif
 */
void
FocusControl_TopLevelInterfaceOnIdle(void)
{
    // calling the Host comms for the FLADriver , function has the different command of  the Lens Movement.
    FocusControl_FLADriverHostCommsManager();
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FocusControl_SOFISR(void)
 * \brief       To be called at the SOF to setup stats and update AF driver status.
 * \details
 * \return      void
 * \ingroup     FocusControl
 * \endif
 */
void
FocusControl_SOFISR(void)
{
    if (FocusControl_AFStatsGetAutoRefresh())
    {
        // update the zones settings according to the current WOI
        FocusControl_AFStatsSetupStats();
    }


    if (FocusControl_IsLensActuatorPresent())
    {
        if (FocusControl_FLADriverGetLensISMoving() && (!g_lensMoveCmdabsorbedInFrame))
        {
            OstTraceInt0(TRACE_WARNING, "!!WARNING!!<AF>Forced lens moved done");

            // Stop Timer - call StartTimer with arguement zero.
            FLADriver_StartTimer(0);

            // Stop Lens : [PM][TBD]How to handle non-blocking call?
            LLA_Abstraction_Lens_Stop();

            // Update status
            FLADriver_UpdateStatus();

            // Update Error Status
            g_FLADriver_LensLLDParam.e_FLADriver_APIError_Type = FLADriver_APIError_e_LENS_MOVE_REACHED_FRAME_TOLERANCE;
        }


        if (g_lensMoveCmdabsorbedInFrame)
        {
            g_lensMoveCmdabsorbedInFrame--;
        }


        g_FocusControl_Status.e_Flag_LensIsMovingAtTheSOF = FocusControl_FLADriverGetLensISMoving();

        // This is the test mode control for the FLADriver FW test .
        g_FocusControl_Status.e_Flag_IsStable =
            (
                FocusControl_FLADriverIsStable()
            &&  !(g_FocusControl_Status.e_Flag_LensIsMovingAtTheSOF)
            );
    }
}


// include Focus 3 Modules (AFstats+FocusControl+FLADriver)
#endif // INCLUDE_FOCUS_MODULES


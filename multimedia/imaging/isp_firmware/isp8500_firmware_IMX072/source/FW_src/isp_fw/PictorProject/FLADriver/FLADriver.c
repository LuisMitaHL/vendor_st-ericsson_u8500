/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \file    FLADriver.c
 * \brief   It's the Focus Lens Actuato Driver used by the Focus Control to drive the Focus Lens Actuator
 * \ingroup FLADriver
 * \endif
*/
#include "FLADriver.h"
#include "FLADriverPlatformSpecific.h"
#include "FLADriver_OPInterface.h"
#include "lla_abstraction.h"
#include "GPIOManager_OpInterface.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_FLADriver_FLADriverTraces.h"
#endif

// global variables
// include Auto Focus 4 Modules (AutoFocus+AFstats+FocusControl+FLADriver)
#if INCLUDE_FOCUS_MODULES
FLADriver_LensLLDParam_ts       g_FLADriver_LensLLDParam =
{
    0,
    0,
    0,
    0,
    0,
    FLADriver_APIError_e_NO_ERROR,
    FLADriver_TimerId_e_TIMER_2_NOT_STARTED
};

FLADriver_LLLCtrlStatusParam_ts g_FLADriver_LLLCtrlStatusParam =
{
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_MIN_POSITION,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_MAX_POSITION,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_REST_POSITION,
    0,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_INFINITY_HOR_POSITION,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_HYPERFOCAL_POSITION,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_HOR_MACRO_POSITION,
    0,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_TOLERANCE,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_TARGET_SET_POSITION,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_MANUAL_STEP_SIZE,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_CURRENT_POSITION,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_TARGET_POSITION,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_PREVIOUS_POSITION,
    FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_LAST_STEP_SIZE_EXECUTED,
    Flag_e_FALSE,
};

FLADriver_Controls_ts           g_FLADriver_Controls =
{
    FLAD_DEFAULT_CTRL_TIME_LIMIT_MS,
    FLAD_DEFAULT_CTRL_RANGE_DEF_RANGE_CTRL,
    5,
};

FLADriver_Status_ts             g_FLADriver_Status =
{
    FLAD_DEFAULT_STATUS_CYCLES,
    FLAD_DEFAULT_STATUS_LENS_IS_MOVING,
    FLAD_DEFAULT_STATUS_LIMITS_EXCEEDED,
    FLAD_DEFAULT_STATUS_LOW_LEVEL_DRIVER_INITIALIZED,
    FLAD_DEFAULT_STATUS_AF_SENSOR_ORIENTATION_TYPE,
    FLAD_DEFAULT_STATUS_RANGE_DEF_RANGE_STATUS,
};

FLADriver_NVMStoredData_ts      g_FLADriver_NVMStoredData =
{
    FLAD_DEFAULT_NVM_MACRO_POS,
    FLAD_DEFAULT_NVM_INFINITY_POS,
    FLAD_DEFAULT_NVM_MACRO_HOR,
    FLAD_DEFAULT_NVM_INFINITY_HOR,
    FLAD_DEFAULT_NVM_HYPERFOCAL_POS,
    FLAD_DEFAULT_NVM_REST_POS,
    FLAD_DEFAULT_NVM_DATA_PRESENT,
    FLAD_DEFAULT_NVM_ACTUATOR_LENS_PRESENT,
    FLAD_DEFAULT_NVM_POSITION_SENSOR_PRESENT
};

volatile uint16_t               g_lensMoveCmdabsorbedInFrame;
//volatile uint32_t               g_u32_LensMoveStartTime = 0;  //required just for performance measurements
volatile uint32_t               g_u32_LensMoveStopTime = 0;

volatile uint16_t               g_FLADriver_LensMoveErrorCount = 0;

uint8_t                         FLADriver_LowLevelInitialization (void);
void                            FLADriver_GoToInfinityFarEnd (void);
void                            FLADriver_GoToMacroNearEnd (void);
uint8_t                         FLADriver_Initialize (void);
uint16_t                        FLADriver_Move (int16_t, uint8_t);
void                            FLADriver_MoveTo (int16_t);
void                            FLADriver_AssignDefaultDetails (void);
void                            FLADriver_AssignNVMDetails (void);
void                            FLADriver_GoToTargetPosition (void);
void                            FLADriver_HostCommsManager (void);
void                            FLADriver_ReadNvmDetails (void);
void                            FLADriver_LLAEventHandler (CAM_DRV_EVENT_E, CAM_DRV_EVENT_INFO_U *, uint8_t);
void                            FLADriver_AssignDefaultDetails (void);
void                            FLADriver_AssignNVMDetails (void);

/**
         * \if          INCLUDE_IN_HTML_ONLY
         * \fn          void FLADriver_LLAEventHandler(CAM_DRV_EVENT_E event_code, CAM_DRV_EVENT_INFO_U *p_event_info, bool8 isr_event)
         * \brief       LLA event handler function for FLADriver Related events .
         * \return
         * \ingroup     FLADriver
         * \endif
         */
void
FLADriver_LLAEventHandler(
CAM_DRV_EVENT_E         event_code,
CAM_DRV_EVENT_INFO_U    *p_event_info,
uint8_t                 isr_event)
{
    if (event_code == CAM_DRV_AF_LENS_MOVED)
    {
        // note time to check validity of stats later.
        g_u32_LensMoveStopTime = g_gpio_debug.u32_GlobalRefCounter + GLOBAL_TIMER_ERROR_MARGIN_MS;

        g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos = (int16_t) (p_event_info->lens_position);

        g_FLADriver_LensLLDParam.e_FLADriver_Timer2Id_Type = FLADriver_TimerId_e_TIMER_2_EXPIRED;

        // call update status for FLADriver .
        FLADriver_UpdateStatus();

        OstTraceInt1(TRACE_FLOW, "<AF>FLADriver_LLAEventHandler(): LENS moved to => %d ", p_event_info->lens_position);
    }
    else if (event_code == CAM_DRV_AF_LENS_POSITION_MEASURED)
    {
        g_FLADriver_LensLLDParam.e_FLADriver_APIError_Type = FLADriver_APIError_e_LENS_POSITION_MEASURED;

        g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos = (int16_t) (p_event_info->lens_position);

        OstTraceInt1(TRACE_FLOW, "<AF>FLADriver_LLAEventHandler(): LENS Measured to => %d ", p_event_info->lens_position);
    }
    else if (event_code == CAM_DRV_LENS_STOPPED)
    {
        g_FLADriver_LensLLDParam.e_FLADriver_APIError_Type = FLADriver_APIError_e_LENS_STOPPED;

        g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos = (int16_t) (p_event_info->lens_position);

        OstTraceInt1(TRACE_FLOW, "<AF>FLADriver_LLAEventHandler(): LENS stopped to => %d ", p_event_info->lens_position);
    }
}


// function definitions .

/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          uint16_t FLADriver_Move(int16_t s16_Step, uint8_t e_FLADriver_Direction_Dir)
 * \brief       move the lens of the given amount of steps in the given direction.
 * \details
 * \return      uint8_t (the numbers of steps effectively programmed)
 * \ingroup     FLADriver
 * \endif
 */
uint16_t
FLADriver_Move(
int16_t s16_LowLevelStep,
uint8_t e_FLADriver_Direction_Dir)
{
    int16_t s16_LowLevelTargetPosition = 0;

    if (g_FLADriver_Status.e_Flag_LensIsMoving)
    {
        // if the lens is already moving we don't
        return (0);
    }


    if ( /*(s16_LowLevelStep == 0) || */ (e_FLADriver_Direction_Dir == FLADriver_Direction_e_DIR_STOP))
    {
        // no real movement.
        return (0);
    }


    // check if the desired position is within the sw limits
    // (it's assumed that the sw limits are within hw limits)
    if (e_FLADriver_Direction_Dir == FLADriver_Direction_e_BACKWARDS)
    {
        // check the End limit
        if
        (
            (
                g_FLADriver_LLLCtrlStatusParam.u16_MaxPos -
                g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos
            ) <= s16_LowLevelStep
        )
        {
            s16_LowLevelStep = g_FLADriver_LLLCtrlStatusParam.u16_MaxPos - g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos;
            g_FLADriver_Status.e_Flag_LimitsExceeded = Flag_e_TRUE;
        }
        else
        {
            g_FLADriver_Status.e_Flag_LimitsExceeded = Flag_e_FALSE;
        }
    }
    else if (e_FLADriver_Direction_Dir == FLADriver_Direction_e_FORWARDS)
    {
        // check the Start limit
        if
        (
            (
                g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos -
                g_FLADriver_LLLCtrlStatusParam.u16_MinPos
            ) <= s16_LowLevelStep
        )
        {
            s16_LowLevelStep = g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos - g_FLADriver_LLLCtrlStatusParam.u16_MinPos;
            g_FLADriver_Status.e_Flag_LimitsExceeded = Flag_e_TRUE;
        }
        else
        {
            g_FLADriver_Status.e_Flag_LimitsExceeded = Flag_e_FALSE;
        }
    }


    // Evaluate next Lens position
    switch (e_FLADriver_Direction_Dir)
    {
        case FLADriver_Direction_e_BACKWARDS:
            s16_LowLevelTargetPosition = g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos + s16_LowLevelStep;
            break;

        case FLADriver_Direction_e_FORWARDS:
            s16_LowLevelTargetPosition = g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos - s16_LowLevelStep;
            break;

        default:
            break;
    }


    // Effectively start moving the lens
    FLADriver_MoveTo(s16_LowLevelTargetPosition);
    return (uint16_t) s16_LowLevelStep;
}   //FLADriver_Move

/**
 * \if              INCLUDE_IN_HTML_ONLY
 * \fn              void FLADriver_UpdateStatus(void)
 * \brief           to update the Lens Actuator Status depending
 *                  if the lens is still moving or not.
 * \details
 * \return          nothing
 * \ingroup    FLADriver
 * \endif
 */
void
FLADriver_UpdateStatus(void)
{

    if (g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos != g_FLADriver_LLLCtrlStatusParam.u16_TargetPos)
    {
        g_FLADriver_LensLLDParam.u16_DiffFromTarget = FDLADriver_ABS_DIFF(
            g_FLADriver_LLLCtrlStatusParam.u16_TargetPos,
            g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos);

        g_FLADriver_LensLLDParam.e_FLADriver_APIError_Type = FLADriver_APIError_e_TIMER_EXPIRE_BEFORE_REACHING_POS;
    }
    else
    {
        g_FLADriver_LensLLDParam.u16_DiffFromTarget = 0;

        g_FLADriver_LensLLDParam.e_FLADriver_APIError_Type = FLADriver_APIError_e_LENS_MOVED_TO_TARGET;
    }


    // conversion of the Parameters should happen if the Lens stop moving .
    g_FLADriver_LLLCtrlStatusParam.s16_LastStepSizeExecuted = FDLADriver_ABS_DIFF(
        g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos,
        g_FLADriver_LLLCtrlStatusParam.u16_PreviousPos);

    g_FLADriver_Status.e_Flag_LensIsMoving = Flag_e_FALSE;
    g_lensMoveCmdabsorbedInFrame = 0;
    FLADriver_FocusControlSetCmdStatusCoin(FLADriver_FocusControlGetCmdControlCoin());

    FLADriver_LensStopNotify();
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FLADriver_GoToHyperfocal(void)
 * \brief
 * \details
 * \return      void
 * \ingroup     FLADriver
 * \endif
 */
void
FLADriver_GoToHyperfocal(void)
{
    FLADriver_MoveTo(g_FLADriver_LLLCtrlStatusParam.u16_HyperFocalPos);
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FLADriver_GoToRest(void)
 * \brief
 * \details
 * \return      void
 * \ingroup     FLADriver
 * \endif
 */
void
FLADriver_GoToRest(void)
{
    FLADriver_MoveTo(g_FLADriver_LLLCtrlStatusParam.u16_RestPos);
}   // FLADriver_GoToRest

/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FLADriver_GoToTargetPosition(void)
 * \brief       Go to the Target Pos of the current range.
 * \details
 * \return      nothing
 * \ingroup     FLADriver
 * \endif
 */
void
FLADriver_GoToTargetPosition(void)
{
    FLADriver_MoveTo(g_FLADriver_LLLCtrlStatusParam.u16_TarSetPos);
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FLADriver_GoToInfinityFarEnd(void)
 * \brief       Go to the Min Pos of the current range
 *              (not necessary the Min of the Phys Range)
 * \details
 * \return      nothing
 * \ingroup     FLADriver
 * \endif
 */
void
FLADriver_GoToInfinityFarEnd(void)
{
    FLADriver_MoveTo(g_FLADriver_LLLCtrlStatusParam.u16_InfinityFarEndPos);
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FLADriver_GoToInfinityHor(void)
 * \brief       Go to the Min hor Pos of the current range .
 * \details
 * \return      nothing
 * \ingroup     FLADriver
 * \endif
 */
void
FLADriver_GoToInfinityHor(void)
{
    FLADriver_MoveTo(g_FLADriver_LLLCtrlStatusParam.u16_InfinityHorPos);
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FLADriver_GoToMacroHor(void)
 * \brief       Go to the MacroHor position.
 * \details
 * \return      nothing
 * \ingroup     FLADriver
 * \endif
 */
void
FLADriver_GoToMacroHor(void)
{
    FLADriver_MoveTo(g_FLADriver_LLLCtrlStatusParam.u16_MacroHorPos);
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FLADriver_GoToMacroNearEnd(void)
 * \brief       Go to the Max Pos of the current range
 *              (not necessary the Max of the Phys Range)
 * \details
 * \return      nothing
 * \ingroup     FLADriver
 * \endif
 */
void
FLADriver_GoToMacroNearEnd(void)
{
    FLADriver_MoveTo(g_FLADriver_LLLCtrlStatusParam.u16_MacroNearEndPos);
}   //FLADriver_GoToMacroNearEnd

/**
 * \if         INCLUDE_IN_HTML_ONLY
 * \fn         void FLADriver_HostCommsManager(void)
 * \brief      Execute the LensActuator command that are issued directly by the host
 *             this appends only in manual focus.
 * \details
 * \return     nothing
 * \ingroup    FLADriver
 * \endif
 */
void
FLADriver_HostCommsManager(void)
{
    if
    (
        (FLADriver_FocusControlGetCmdControlCoin() != FLADriver_FocusControlGetCmdStatusCoin())
    &&  FocusControl_FLADriverIsStable()
    )
    {
        //OstTraceInt1(TRACE_USER1,"[AF Optimization]Lens Move Command Requested Time = %d",g_gpio_debug.u32_GlobalRefCounter);
        FLADriver_FocusControlSetStatusTLILensCommand(FLADriver_FocusControlGetTLILensCommand());

        // Adding command coin mechanism , to ensure that host can toggle the coin again
        FocusControl_SetStatusWithLensMove(FocusControl_GetControlWithLensMove());

        // switching to appropriate command mode .
        switch (FLADriver_FocusControlGetStatusTLILensCommand())
        {
            case FLADriver_FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_INFINITY:
                g_FLADriver_LLLCtrlStatusParam.u16_ManualStepSize = FLADriver_Move(
                    g_FLADriver_LLLCtrlStatusParam.u16_ManualStepSize,
                    FLADriver_Direction_e_FORWARDS);
                break;

            case FLADriver_FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_MACRO:
                g_FLADriver_LLLCtrlStatusParam.u16_ManualStepSize = FLADriver_Move(
                    g_FLADriver_LLLCtrlStatusParam.u16_ManualStepSize,
                    FLADriver_Direction_e_BACKWARDS);
                break;

            case FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_FAR_END:
                FLADriver_GoToInfinityFarEnd();
                break;

            case FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_HOR:
                FLADriver_GoToInfinityHor();
                break;

            case FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_HOR:
                FLADriver_GoToMacroHor();
                break;

            case FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_NEAR_END:
                FLADriver_GoToMacroNearEnd();
                break;

            case FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_REST:
                FLADriver_GoToRest();
                break;

            case FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_HYPERFOCAL:
                FLADriver_GoToHyperfocal();
                break;

            case FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_TARGET_POSITION:
                FLADriver_GoToTargetPosition();
                break;

            default:
                ;
        }
    }
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FLADriver_ReadNvmDetails(void)
 * \brief       Reading from NVM and storing in Fw structure .
 * \details
 * \return      void
 * \ingroup     FLADriver
 * \endif
 */
void
FLADriver_ReadNvmDetails(void)
{
    g_FLADriver_NVMStoredData.e_Flag_NVMActuatorLensPresent = Flag_e_TRUE;
    g_FLADriver_NVMStoredData.e_Flag_NVMPositionSensorPresent = g_camera_details.p_lens_details->af_lens_pos_measure_ability;
    g_FLADriver_LensLLDParam.u32_NVMLensUnitMovementTime_us = g_camera_details.p_lens_details->movement_times.horizontal_us;

    g_FLADriver_NVMStoredData.s32_NVMMacroNearEndPos = (int16_t) (g_camera_details.p_lens_details->positions.near_end);
    g_FLADriver_NVMStoredData.s32_NVMInfinityFarEndPos = (int16_t) (g_camera_details.p_lens_details->positions.far_end);
    g_FLADriver_NVMStoredData.s32_NVMInfinityHorPos = (int16_t) (g_camera_details.p_lens_details->positions.infinity);
    g_FLADriver_NVMStoredData.s32_NVMMacroHorPos = (int16_t) (g_camera_details.p_lens_details->positions.macro);
    g_FLADriver_NVMStoredData.s32_NVMHyperfocalPos = (int16_t) (g_camera_details.p_lens_details->positions.hyperfocal);
    g_FLADriver_NVMStoredData.s32_NVMRestPos = (int16_t) (g_camera_details.p_lens_details->positions.rest);
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FLADriver_AssignNVMDetails(void)
 * \brief       Assigning the details read from NVM to FW structure .
 * \details
 * \return      void
 * \ingroup     FLADriver
 * \endif
 */
void
FLADriver_AssignNVMDetails(void)
{
    g_FLADriver_LLLCtrlStatusParam.u16_MacroNearEndPos = g_FLADriver_NVMStoredData.s32_NVMMacroNearEndPos;
    g_FLADriver_LLLCtrlStatusParam.u16_InfinityFarEndPos = g_FLADriver_NVMStoredData.s32_NVMInfinityFarEndPos;
    g_FLADriver_LLLCtrlStatusParam.u16_InfinityHorPos = g_FLADriver_NVMStoredData.s32_NVMInfinityHorPos;
    g_FLADriver_LLLCtrlStatusParam.u16_MacroHorPos = g_FLADriver_NVMStoredData.s32_NVMMacroHorPos;
    g_FLADriver_LLLCtrlStatusParam.u16_RestPos = g_FLADriver_NVMStoredData.s32_NVMRestPos;
    g_FLADriver_LLLCtrlStatusParam.u16_HyperFocalPos = g_FLADriver_NVMStoredData.s32_NVMHyperfocalPos;
    g_FLADriver_NVMStoredData.e_Flag_NVMDataPresent = Flag_e_TRUE;
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          void FLADriver_AssignDefaultDetails(void)
 * \brief       Assigning the Default values for the Actuator .
 * \details
 * \return      void
 * \ingroup     FLADriver
 * \endif
 */
void
FLADriver_AssignDefaultDetails(void)
{
    g_FLADriver_LLLCtrlStatusParam.u16_MacroNearEndPos = FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_MACRO_NEAR_END_POSITION;
    g_FLADriver_LLLCtrlStatusParam.u16_InfinityFarEndPos = FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_INFINITY_FAR_END_POSITION;
    g_FLADriver_LLLCtrlStatusParam.u16_InfinityHorPos = FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_INFINITY_HOR_POSITION;

    g_FLADriver_LLLCtrlStatusParam.u16_MacroHorPos = FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_HOR_MACRO_POSITION;
    g_FLADriver_LLLCtrlStatusParam.u16_RestPos = FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_REST_POSITION;
    g_FLADriver_LLLCtrlStatusParam.u16_HyperFocalPos = FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_HYPERFOCAL_POSITION;

    g_FLADriver_LLLCtrlStatusParam.u16_ManualStepSize = FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_MANUAL_STEP_SIZE;
    g_FLADriver_LLLCtrlStatusParam.s16_ToleranceSize = FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_TOLERANCE;
    g_FLADriver_LLLCtrlStatusParam.u16_TarSetPos = FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_TARGET_SET_POSITION;
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          uint8_t FLADriver_LowLevelinitialization(void)
 * \brief       Initializating FLADriver Parameters .
 * \details
 * \return      uint8_t
 * \ingroup     FLADriver
 * \endif
 */
uint8_t
FLADriver_LowLevelInitialization(void)
{
    FLADriver_ReadNvmDetails();

    if (g_FLADriver_Controls.e_FLADriver_RangeDef_CtrlRange == FLADriver_RangeDef_e_NVM_LEVEL_RANGE)
    {
        // if no data is found after reading the nvm registers , the default value -1 will remain in g_FLADriver_NVMStoredData parameters
        // Lets inform about no NVM present to host .
        if
        (
            (g_FLADriver_NVMStoredData.s32_NVMInfinityFarEndPos == (0))
        &&  (g_FLADriver_NVMStoredData.s32_NVMMacroNearEndPos == (0))
        )
        {
            g_FLADriver_NVMStoredData.e_Flag_NVMDataPresent = Flag_e_FALSE;

            // if nvm data not found and read from nvm has been initiated , then store some value in releven parameters of fw , so that it can work .
            // this information has been found after doing some investigation using LMI , and comes up with some generic values , so that it can be applicable to all the modules .
            FLADriver_AssignDefaultDetails();
            FLADriver_FocusControlSetErrorMsg(FocusControl_FocusMsg_e_NO_NVM_DATA_DEFAULT_ASSIGNED);
        }
        else
        {
            FLADriver_AssignNVMDetails();
        }


        g_FLADriver_Status.e_FLADriver_RangeDef_StatusRange = FLADriver_RangeDef_e_NVM_LEVEL_RANGE;
    }
    else
    {
        if
        (
            (g_FLADriver_LLLCtrlStatusParam.u16_InfinityFarEndPos == (0))
        &&  (g_FLADriver_LLLCtrlStatusParam.u16_MacroNearEndPos == (0))
        )
        {
            // if no values from the host has been assigned then in that case assign the NVM values in case of HOST define Range.
            FLADriver_AssignNVMDetails();

            //Again if the values assigned from the NVM is all 0 then assign the default values .
            if
            (
                (g_FLADriver_LLLCtrlStatusParam.u16_InfinityFarEndPos == (0))
            &&  (g_FLADriver_LLLCtrlStatusParam.u16_MacroNearEndPos == (0))
            )
            {
                g_FLADriver_NVMStoredData.e_Flag_NVMDataPresent = Flag_e_FALSE;

                // if nvm data not found and read from nvm has been initiated , then store some value in releven parameters of fw , so that it can work .
                // this information has been found after doing some investigation using LMI , and comes up with some generic values , so that it can be applicable to all the modules .
                FLADriver_AssignDefaultDetails();
                FLADriver_FocusControlSetErrorMsg(FocusControl_FocusMsg_e_NO_HOST_DATA_DEFAULT_ASSIGNED);
            }
            else
            {
                FLADriver_FocusControlSetErrorMsg(FocusControl_FocusMsg_e_NO_HOST_DATA_NVM_ASSIGNED);
            }
        }


        g_FLADriver_Status.e_FLADriver_RangeDef_StatusRange = FLADriver_RangeDef_e_HOST_DEFINED_RANGE;
    }


    // once the Low level parameters for the Lens has been initialized , Lets decide now for the orientation , based on the value of Low level.
    // Assigning the orientation of the actuator , either aligned to the low level or
    // if MACRO > INFINTY & both are non zero values (orientation is reversed)
    // if MACRO < INFINITY & both are non zero values (orientation is aligned to high level)
    if (g_FLADriver_LLLCtrlStatusParam.u16_InfinityFarEndPos < g_FLADriver_LLLCtrlStatusParam.u16_MacroNearEndPos)
    {
        g_FLADriver_Status.e_FLADriver_ActuatorOrientation_Type = FLADriver_ActuatorOrientation_e_DIRECTION_REVERSED_WRT_TO_STANDARDS;

        // copying initially the Max and min position (worst case Macro and infinity in case of Piezo)read from the NVM to  min and max variable of g_FLADriver_LLLCtrlStatusParam.
        g_FLADriver_LLLCtrlStatusParam.u16_MinPos = g_FLADriver_LLLCtrlStatusParam.u16_InfinityFarEndPos;
        g_FLADriver_LLLCtrlStatusParam.u16_MaxPos = g_FLADriver_LLLCtrlStatusParam.u16_MacroNearEndPos;
    }
    else if (g_FLADriver_LLLCtrlStatusParam.u16_InfinityFarEndPos > g_FLADriver_LLLCtrlStatusParam.u16_MacroNearEndPos)
    {
        g_FLADriver_Status.e_FLADriver_ActuatorOrientation_Type = FLADriver_ActuatorOrientation_e_DIRECTION_ALIGNED_TO_STANDARDS;

        // copying initially the Max and min position (worst case Macro and infinity in case of non Piezo)read from the NVM to  min and max variable of g_FLADriver_LLLCtrlStatusParam.
        g_FLADriver_LLLCtrlStatusParam.u16_MinPos = g_FLADriver_LLLCtrlStatusParam.u16_MacroNearEndPos;
        g_FLADriver_LLLCtrlStatusParam.u16_MaxPos = g_FLADriver_LLLCtrlStatusParam.u16_InfinityFarEndPos;
    }


    // setting the status of the Previous Position of the Lens .
    g_FLADriver_LLLCtrlStatusParam.u16_PreviousPos = g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos;

    // calling the Low Level Api funtion for the Acual movement of the Lens.
    if (cam_drv_lens_measure_pos() != CAM_DRV_OK)
    {
        g_FLADriver_LensLLDParam.e_FLADriver_APIError_Type = FLADriver_APIError_e_CANT_MEASURE_LENS_POS;
#if USE_TRACE_ERROR
        OstTraceInt0(TRACE_ERROR, "<ERROR>!!FLADriver_LowLevelInitialization() - cam_drv_lens_measure_pos() returned error!!");
#endif
        ASSERT_XP70();
        return (Flag_e_TRUE);
    }


    // initialising low level driver , just to check the initialization has performed .
    g_FLADriver_Status.e_Flag_LowLevelDriverInitialized = 1;

    return (Flag_e_FALSE);
}


/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \fn          uint8_t FLADriver_Initialize(void)
 * \brief       Initialization at NVM level .
 * \details
 * \return      Flag_e_FALSE ( successful operation )
 * \ingroup     FLADriver
 * \endif
 */
uint8_t
FLADriver_Initialize(void)
{
    // FLADriver few elements has to be initialised here .
    g_FLADriver_Status.e_Flag_LimitsExceeded = Flag_e_FALSE;
    g_FLADriver_Status.e_Flag_LensIsMoving = Flag_e_FALSE;
    g_FLADriver_Status.e_Flag_LowLevelDriverInitialized = Flag_e_FALSE;
    g_FLADriver_Status.u16_Cycles = 0;

    // if the Actuator and the FW driver with Action on is present then
    //setting the lens present and calling the Low Level initialization
    g_FLADriver_NVMStoredData.e_Flag_NVMActuatorLensPresent = Flag_e_TRUE;
    g_FLADriver_NVMStoredData.e_Flag_NVMPositionSensorPresent = g_camera_details.p_lens_details->af_lens_pos_measure_ability;

    // return TRUE if error , else return FALSE .
    return (FLADriver_LowLevelInitialization());
}


/**
* \if           INCLUDE_IN_HTML_ONLY
* \fn           void FLADriver_MoveTo(int16_t s16_TargetPosition)
* \brief        to move to a given target position,
*               according to the g_FLADriver_Controls
* \details
* \return       Nothing
* \ingroup      FLADriver
* \endif
*/
void
FLADriver_MoveTo(
int16_t s16_TargetPosition)
{
    OstTraceInt1(TRACE_FLOW, "<AF>FLADriver_MoveTo(): Target position: %d ", s16_TargetPosition);

    // Only if the driver and action on for the FLADriver fw .
    g_FLADriver_LLLCtrlStatusParam.u16_TargetPos = s16_TargetPosition;
    g_FLADriver_LLLCtrlStatusParam.u16_PreviousPos = g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos;

    // Note time to check validity of stats later.
    // g_u32_LensMoveStartTime = g_gpio_debug.u32_GlobalRefCounter + GLOBAL_TIMER_ERROR_MARGIN_MS;
    OstTraceInt1(TRACE_USER1,"[AF Optimization]Lens Move Start Time = %u",(g_gpio_debug.u32_GlobalRefCounter + GLOBAL_TIMER_ERROR_MARGIN_MS));

    // If Target Lens Pos is different from Current Lens Pos
    if (g_FLADriver_LLLCtrlStatusParam.u16_TargetPos != g_FLADriver_LLLCtrlStatusParam.u16_CurrentPos)
    {
       // Calling the Low level APIs here in order to move the Lens.
      if (CAM_DRV_OK != cam_drv_lens_move_to_pos((int32_t) g_FLADriver_LLLCtrlStatusParam.u16_TargetPos))
       {
          g_FLADriver_LensLLDParam.e_FLADriver_APIError_Type = FLADriver_APIError_e_LENS_MOVE_API_FAILURE;
          OstTraceInt0(TRACE_ERROR, "<ERROR>!!FLADriver_MoveTo() - cam_drv_lens_move_to_pos() returned error!!");
          OstTraceInt1(TRACE_ERROR, "<ERROR>!!FLADriver_MoveTo() ERROR COUNT = %d",g_FLADriver_LensMoveErrorCount);

           // We do not have any interface to notify OMX that there is a HW error
           // Note we are not asserting here even if lens is not moving but sending stat of previous lens positions

          g_u32_LensMoveStopTime = g_gpio_debug.u32_GlobalRefCounter + GLOBAL_TIMER_ERROR_MARGIN_MS;

          g_FLADriver_LensLLDParam.e_FLADriver_Timer2Id_Type = FLADriver_TimerId_e_TIMER_2_NOT_STARTED;

          // Call equalling coin and LensStopNotify .
          FLADriver_FocusControlSetCmdStatusCoin(FLADriver_FocusControlGetCmdControlCoin());
          FLADriver_LensStopNotify();

          // Note this counter is reset whenever we issue run command
          g_FLADriver_LensMoveErrorCount++;

          return;
       }

       // Set lens moving to true.
       g_FLADriver_Status.e_Flag_LensIsMoving = Flag_e_TRUE;
       FocusControl_SetStatusLensIsMovingAtTheSOF(Flag_e_TRUE);
       FLADriver_FocusControlSetLensIsMovingAtTheSOF(Flag_e_TRUE);
    }
    //Requested Target Position is same as Current Lens Position!
    else
    {
       // Lense Moving Should Always Be False Here
       OstTraceInt0(TRACE_FLOW, "<AF>FLADriver_MoveTo():u16_CurrentPos same as u16_TargetPos!");

      if(g_FLADriver_Status.e_Flag_LensIsMoving == Flag_e_TRUE)
       {
        // ER for Modified ASSERT_XP70 macro (using a parameter) to be raised seperately
        ASSERT_XP70();
       }

       // Dummy event for lens move, in case of target pos = current pos.
       // Calling this here to trigger stats request as early as possible.
       // Note time to check validity of stats later: zero as lens move request not sent
        g_u32_LensMoveStopTime = g_gpio_debug.u32_GlobalRefCounter + GLOBAL_TIMER_ERROR_MARGIN_MS;

        g_FLADriver_LensLLDParam.e_FLADriver_Timer2Id_Type = FLADriver_TimerId_e_TIMER_2_NOT_STARTED;

        // Call update status for FLADriver .
        FLADriver_UpdateStatus();

        OstTraceInt0(TRACE_FLOW, "<AF>FLADriver_MoveTo(): LENS was not moved as CurrentPos same as TargetPos!");

    }

    // Calling the stats gathering from here if the stats-with-Lens-move Flag is on .
    if (FocusControl_GetStatusWithLensMove() == Flag_e_TRUE)
    {
        OstTraceInt1(TRACE_USER1,"[AF Optimization]Received Stats Req with Lens move in frame = %u", Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter());

        FLADriver_AFStatsSetStatsHostcmdCtrl(AFStats_HostCmd_e_REQ_STATS_ONCE);

       // Set this status to TRUE, irrespective of whether lens is moving or not,
       // So that the Stats gathering can be started .
       // This flag is set FALSE when the Exporting of stats is done .
       // This is called after lens move to avoid AF stats interrupt at undesired time i.e.,
       // After setting this flag and before lens move command is given.

        FLADriver_AFStatsSetStatsWithLensMoveFWStatus(Flag_e_TRUE);
    }

    /// Maximum tolerance for this command to get absorbed is 5 frames (default); dirty patch for autofocus bug
    g_lensMoveCmdabsorbedInFrame = g_FLADriver_Controls.u8_FrameTolerance;

    // Just for debugging purposes
    g_FLADriver_Status.u16_Cycles++;
}

/**
* \if           INCLUDE_IN_HTML_ONLY
* \fn           void FLADriver_StartTimer(uint32_t  time_in_10us)
* \brief        To start or stop the GPIO timer for Lens movement.
*               Will stop the timer if time_in_10us = 0.
* \details
* \return       void
* \ingroup      FLADriver
* \endif
*/
void
FLADriver_StartTimer(
uint32_t    time_in_10us)
{
    g_FLADriver_LensLLDParam.u32_FLADTimer2CallCount++;

    // Analyse the time to Move in us.
    if (time_in_10us != 0)
    {
        // Trigger LLCD Timer 2
        GPIOManager_StartIntTimerFLAD_us(time_in_10us * g_FLADriver_Controls.u16_Ctrl_TimeLimit_ms);

        // Update PE status
        g_FLADriver_LensLLDParam.e_FLADriver_Timer2Id_Type = FLADriver_TimerId_e_TIMER_2_STARTED;
    }
    else
    {
        // This will stop and disable the timer.
        GPIOManager_StartIntTimerFLAD_us(0);

        // Update PE status
        g_FLADriver_LensLLDParam.e_FLADriver_Timer2Id_Type = FLADriver_TimerId_e_TIMER_2_STOPPED;
    }

}
#endif // INCLUDE_FOCUS_MODULES


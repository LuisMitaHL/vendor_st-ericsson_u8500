/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \if      INCLUDE_IN_HTML_ONLY
 * \file    Flash.c
 * \brief
 * \ingroup Flash
 * \endif
*/
#include "Flash.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_Flash_FlashTraces.h"
#endif

extern volatile Flag_te           g_IsGrabOKNotification;
extern volatile UpdateStatus_te   g_SensorSettingStatus;

volatile uint8_t                  g_FlashLitFrameCounter = 0;

FlashControl_ts         g_FlashControl =
{
    FLASH_STROBE_DEFAULT_DELAY_FROM_START_POINT,                    // int32_t    s32_DelayFromStartPoint_lines
    FLASH_STROBE_DEFAULT_LENGTH,                                    // uint32_t   u32_StrobeLength_us
    FLASH_STROBE_DEFAULT_STROBES_PER_FRAME,                         // uint8_t    u8_StrobesPerFrame
    FLASH_STROBE_DEFAULT_FRAME_COUNT,                               // uint8_t    u8_FrameCount
    FLASH_STROBE_DEFAULT_MAXIMUM_FRAMES_TO_WAIT_FOR_FLASH_TRIGGER,  // uint8_t    u8_MaxFramesToWaitForFlashTrigger;
    FLASH_STROBE_DEFAULT_START_POINT,                               // uint8_t    e_StrobeStartPoint
    FLASH_STROBE_DEFAULT_GBRST_FRAME_ONLY,                          // uint8_t    e_GlobalResetFrameOnly
    FLASH_STROBE_DEFAULT_STROBE_MODULATION,                         // uint8_t    e_DoStrobeModulation
    FLASH_STROBE_DEFAULT_FLASH_MODE                                // uint8_t    e_Flag_FlashMode
};

FlashStatus_ts          g_FlashStatus =
{
    FLASH_STROBE_DEFAULT_MIN_STROBE_LENGTH,         // uint32_t    u32_MinStrobeLength_us
    FLASH_STROBE_DEFAULT_MAX_STROBE_LENGTH,         // uint32_t    u32_MaxStrobeLength_us
    FLASH_STROBE_DEFAULT_STROBE_LENGTH_STEP,        // uint32_t    u32_StrobeLengthStep
    FLASH_STROBE_DEFAULT_STROBES_PER_FRAME,         // uint8_t     u8_StrobesPerFrame
    FLASH_STROBE_DEFAULT_FLASH_FIRED_FRAME_COUNT,   // uint8_t     u8_FlashFiredFrameCount
    FLASH_STROBE_DEFAULT_ERROR_STATUS_NUMBER_OF_FORCED_INPUT_PROC_UPDATES,  // uint8_t  u8_NumberOfForcedInputProcUpdates
    FLASH_STROBE_DEFAULT_ERROR_STATUS_NUMBER_OF_CONSECUTIVE_DELAYED_FRAMES, // uint8_t  u8_NumberOfConsecutiveDelayedFrames
    FLASH_STROBE_DEFAULT_ERROR_STATUS_SYNC_ERROR_COUNT,                     // uint8_t  u8_ExposureSyncErrorCount
    FLASH_STROBE_DEFAULT_ERROR_STATUS_FORCE_INPUT_PROC_UPDATION,            // uint8_t  e_Flag_ForceInputProcUpdation
    FLASH_STROBE_DEFAULT_ERROR_INFO,        // uint8_t      e_FlashError_Info
    FLASH_STROBE_DEFAULT_STROBE_MODULATION // uint8_t      e_Flag_StrobeModulationSupported
};

static void             Flash_FrameSyncFailed(void);


/**
 * \if   INCLUDE_IN_HTML_ONLY
 * \fn   void Flash_HandleFlashTrigger(void)
 * \brief  This function decides whether or not to send the Flash notification to host.
 * \details Will be called once for each new frame, provided a flash trigger is expected.
 *          Assumes that LLA_Abstraction_InterpretSensorSettings has been called prior to this.
 * \return
 * \ingroup Flash
 * \callgraph
 * \callergraph
 * \endif
*/
void
Flash_HandleFlashTrigger(void)
{
    //confirm whether flash was fired or not, or if its a case of force updation
    if (LLA_IS_LIT_FRAME())
    {
        //[TBC]update flash frame counter at this stage
        g_FlashLitFrameCounter = Get_ISP_SMIARX_ISP_SMIARX_STATUS_frame_counter();

        OstTraceInt0(TRACE_DEBUG, "->HandleFlashTrigger: flash fired!");
        
        //set flash-fired page element
        g_FrameParamStatus.u32_flash_fired = 1;

        //increment flash fired count
        g_FlashStatus.u8_FlashFiredFrameCount++;

        g_FlashStatus.u8_NumberOfConsecutiveDelayedFrames = 0;

        g_FlashStatus.e_FlashError_Info = FlashError_e_FLASH_ERROR_NONE;

        //if user requests for multiple frame lits , keep status as "updating" till those many lit frames are received
        if (g_FlashStatus.u8_FlashFiredFrameCount < g_FlashControl.u8_FrameCount)
        {
            Flash_SetFlashStatus(UpdateStatus_e_Updating);
        }
        else
        {
            // Finally change the status. This will allow glace/histo interrupt to process.
            Flash_SetFlashStatus(UpdateStatus_e_ParamsReceived);

            //Also take care of flash + exposure simultaneous request.
            //In that case, send notify only if flash lit frame has arrived.
            if (SYSTEM_CONFIG_PARAMS_ABSORBED())
            {
                g_IsGrabOKNotification = Flag_e_TRUE;
            }
        }
    }
    else
    {
        //perform failed sync handling
        Flash_FrameSyncFailed();

        // synch not done in this frame, so wait till next smiaRx
        Flash_SetFlashStatus(UpdateStatus_e_Updating);
    }

    //if flash did not fire for u8_MaxFramesToWaitForFlashTrigger frames,
    //discard further checking and reset to idle
    if (Flash_GetForceInputProcUpdationStatus())
    {
        //update error info
        g_FlashStatus.e_FlashError_Info = FlashError_e_FLASH_ERROR_STROBE_DID_NOT_TRIGGER;

        //handle error
        Flash_HandleFlashErrors();
    }
}


/**
 * \if   INCLUDE_IN_HTML_ONLY
 * \fn   void Flash_HandleFlashErrors(void)
 * \brief  Some basic error handling for flash errors.
 * \details
 * \return
 * \ingroup Flash
 * \callgraph
 * \callergraph
 * \endif
*/
void
Flash_HandleFlashErrors(void)
{
    switch (g_FlashStatus.e_FlashError_Info)
    {
      case FlashError_e_FLASH_ERROR_CONFIGURATION:
          break;

      case FlashError_e_FLASH_ERROR_OUT_OF_FLASH_API_CONTEXT:
           break;

      case FlashError_e_FLASH_ERROR_STROBE_DID_NOT_TRIGGER:
           //Flash_ResetErrorState();
           break;

      case FlashError_e_FLASH_ERROR_NONE:
           /*should never reach here*/
           break;

      default:
           break;

    }

    Flash_SetFlashStatus(UpdateStatus_e_Idle);
    return;
}


static void
Flash_FrameSyncFailed(void)
{
    // record the number of delays
    (g_FlashStatus.u8_NumberOfConsecutiveDelayedFrames)++;
    (g_FlashStatus.u8_FlashSyncErrorCount)++;

    /* now if this loop count is greater than 1... it seems like an unwanted delay
     * in the analog gain and integration time...
     * but we compare against the maximum number of frames which are allowed for the delay.
     */
    if (g_FlashStatus.u8_NumberOfConsecutiveDelayedFrames >= g_FlashControl.u8_MaxFramesToWaitForFlashTrigger)
    {
       /*  if greater than the maximum number, increment the counter of ForcedInputProcUpdates
        *  this would give an idea about the number of times such a thing has been done in the system.
        */
        g_FlashStatus.u8_NumberOfForcedInputProcUpdates++;
        g_FlashStatus.u8_NumberOfConsecutiveDelayedFrames = 0;
        g_FlashStatus.e_Flag_ForceInputProcUpdation = Flag_e_TRUE;
    }

    return;
}


/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 * \defgroup ExpCtrl Exposure Control Module
 * \brief This Module deals with the Exposure Algorithm applied on to the Image.
*/

/**

 * \file      Exposure.h
 * \brief     Header File containing external function declarations and defines for Exposure Algorithm and Exposure Compiler
 * \ingroup   ExpCtrl

*/
#ifndef FLASH_H_
#define FLASH_H_

#include "SystemConfig.h"
#include "EventManager.h"

/**
    Flash Input Interface defines
*/
/// Checks whether flash is idle
#define Flash_IsFlashStatusIdle() \
        (UpdateStatus_e_Idle == g_SystemConfig_Status.e_UpdateStatus_Flash)
      
/// Checks whether flash params are being applied
#define Flash_IsFlashStatusUpdating() \
        (UpdateStatus_e_Updating == g_SystemConfig_Status.e_UpdateStatus_Flash)

/// Checks whether firmware is waiting for flash trigger confirmation
#define Flash_IsFlashStatusWaitingForParams() \
        (UpdateStatus_e_WaitingForParams == g_SystemConfig_Status.e_UpdateStatus_Flash)

/// Checks whether flash trigger confirmation has been received
#define Flash_IsFlashStatusParamsReceived() \
        (UpdateStatus_e_ParamsReceived == g_SystemConfig_Status.e_UpdateStatus_Flash)

/// Sets the flash status flag to the required state
#define Flash_SetFlashStatus(value) \
        (g_SystemConfig_Status.e_UpdateStatus_Flash = (value))

#if(FLASH_TYPE == FLASHTYPE_SENSORDRIVEN)
/// Checks whether flash is supported by the device
#define Flash_IsFlashSupported() \
        (0 < g_FlashStatus.u8_MaxStrobesPerFrame)
#else
    #define Flash_IsFlashSupported() Flag_e_TRUE // For Pin driven Flash, it enabled by default
#endif

#define Flash_IsFlashModeActive() \
         (Flag_e_TRUE == g_FlashControl.e_Flag_FlashMode)

/// Checks whether user toggled flash coin and whether flash is in proper state for configuring
#define Flash_IsFlashToBeTriggered() \
          (Flash_IsFlashStatusIdle() \
          && Flash_IsFlashModeActive() \
          && (g_SystemConfig_Status.e_Coin_Status != g_SystemSetup.e_Coin_Ctrl))     

#define Flash_ResetErrorState() \
        (g_FlashStatus.u8_NumberOfConsecutiveDelayedFrames = 0);   \
        (g_FlashStatus.e_Flag_ForceInputProcUpdation = Flag_e_FALSE)

#define Flash_GetForceInputProcUpdationStatus() \
        (Flag_e_TRUE == g_FlashStatus.e_Flag_ForceInputProcUpdation)


/**
    Flash Platform Specific defines
*/

/// Specifies the default value of Number of lines delay from start point
#define FLASH_STROBE_DEFAULT_DELAY_FROM_START_POINT                            (0)

/// Specifies the default value of Strobe Length in Micro secs
#define FLASH_STROBE_DEFAULT_LENGTH                                            (1)

/// Specifies the default value of Number of strobes per frame
#define FLASH_STROBE_DEFAULT_STROBES_PER_FRAME                                 (1)

/// Specifies the default value of Repeat set of pulses for these many frames
#define FLASH_STROBE_DEFAULT_FRAME_COUNT                                       (1)

/// Specifies the default value of Repeat set of pulses actually fired, with respect to what was requested
#define FLASH_STROBE_DEFAULT_FLASH_FIRED_FRAME_COUNT                           (0)

/// Specifies the default value of Whether the flash is to be fired or not
#define FLASH_STROBE_DEFAULT_FLASH_TO_BE_FIRED                                  (Flag_e_FALSE)

/// Specifies the default value of For the host to specify the starting point of strobe
#define FLASH_STROBE_DEFAULT_START_POINT                                       (StrobeStartPoint_e_EXPOSURE_START)

/// Specifies the default value of Whether the frame is GBRST frame
#define FLASH_STROBE_DEFAULT_GBRST_FRAME_ONLY                                  (Flag_e_FALSE)

/// Specifies the default value of Strobe Modulation support (At present not supported)
#define FLASH_STROBE_DEFAULT_STROBE_MODULATION                                 (Flag_e_FALSE)

/// Specifies the default mode for Flash
#define FLASH_STROBE_DEFAULT_FLASH_MODE                                        (Flag_e_FALSE)

/// Specifies the default value of minimum strobe length
#define FLASH_STROBE_DEFAULT_MIN_STROBE_LENGTH                                 (0)

/// Specifies the default value of minimum strobe length
#define FLASH_STROBE_DEFAULT_MAX_STROBE_LENGTH                                 (1)

/// Specifies the default value of minimum strobe length
#define FLASH_STROBE_DEFAULT_STROBE_LENGTH_STEP                                (1)

/// Specifies the default value of flash error
#define FLASH_STROBE_DEFAULT_ERROR_INFO                                        (FlashError_e_FLASH_ERROR_NONE)

/// Specifies the default value of flash forced input proc updates
#define FLASH_STROBE_DEFAULT_ERROR_STATUS_NUMBER_OF_FORCED_INPUT_PROC_UPDATES  (0)

/// Specifies the default value of number of consecutive delayed frames
#define FLASH_STROBE_DEFAULT_ERROR_STATUS_NUMBER_OF_CONSECUTIVE_DELAYED_FRAMES (0)

/// Specifies the default value of sync error count
#define FLASH_STROBE_DEFAULT_ERROR_STATUS_SYNC_ERROR_COUNT                     (0)

/// Specifies the default value of input proc updation
#define FLASH_STROBE_DEFAULT_ERROR_STATUS_FORCE_INPUT_PROC_UPDATION            (Flag_e_FALSE)

/// Specifies the default value of maximum no of frames for which the system should wait for the applied flash to appear
#define FLASH_STROBE_DEFAULT_MAXIMUM_FRAMES_TO_WAIT_FOR_FLASH_TRIGGER          (10)

/// Its a standard SMIA++ register. SMIA++ expects this reg to be snooped to know Flash Status.
#define FLASH_STATUS_REGSTER                                        (0x0C1C)


/// Its a standard SMIA register. SMIA expects this reg to be snooped to know Digital Gain Status.
#define DIGITAL_GAIN_GREENR_HI                                      (0x020E)
#define DIGITAL_GAIN_GREENR_LOW                                     (0x020F)


/**
    Flash Pages
*/
typedef enum
{
    /// Starting at the beginning of exposure
    StrobeStartPoint_e_EXPOSURE_START,

    /// Starting at the beginning of pixel readout
    StrobeStartPoint_e_READOUT_START
} StrobeStartPoint_te;

typedef enum
{
    /// No error
    FlashError_e_FLASH_ERROR_NONE,

    /// Error in Flash Configuration
    FlashError_e_FLASH_ERROR_CONFIGURATION,

    /// Error specifying Flash did not trigger
    FlashError_e_FLASH_ERROR_STROBE_DID_NOT_TRIGGER,
    
    /// Internal Error thrown outside any flash API call
    FlashError_e_FLASH_ERROR_OUT_OF_FLASH_API_CONTEXT
} FlashError_te;

typedef struct
{
    /// Strobe Delay from Start Point (e_StrobeStartPoint_Frame) in Number of lines
    /// [DEFAULT]:  0
    int32_t                     s32_DelayFromStartPoint_lines;

    /// Length of Strobe in Micro secs
    /// [DEFAULT]:  0
    uint32_t                    u32_StrobeLength_us;

    /// Number of strobes per frame
    /// [DEFAULT]:  1
    uint8_t                     u8_StrobesPerFrame;

    /// Repeat set of pulses for these many frames
    /// [DEFAULT]:  1
    uint8_t                     u8_FrameCount;

    /// Number of frames for which the system should wait for the applied flash
    /// to appear. If it does not appear within this number of frames,
    /// an error case is generated and e_Flag_ForceInputProcUpdation is updated accordingly.
    /// [DEFAULT]:  10
    uint8_t                     u8_MaxFramesToWaitForFlashTrigger;

    /// For the host to specify the starting point of strobe
    /// [DEFAULT]: FLASH_STROBE_AT_EXPOSURE_START
    uint8_t                     e_StrobeStartPoint_Frame;

    /// Whether the frame is GBRST frame
    /// [DEFAULT]: Flag_e_FALSE
    uint8_t                     e_Flag_GlobalResetFrameOnly;

    /// Strobe Modulation support (At present not supported)
    /// [DEFAULT]: Flag_e_FALSE
    uint8_t                     e_Flag_DoStrobeModulation;

    /// Flag to indicate Flash Mode. Flash can be triggered only if this is TRUE.
    /// [DEFAULT]: Flag_e_FALSE
    uint8_t                     e_Flag_FlashMode;
}FlashControl_ts;


typedef struct
{
    /// Minimum Length of Strobe supported by the device
    /// [DEFAULT]:  0
    uint32_t                    u32_MinStrobeLength_us;

    /// Maximum Length of Strobe supported by the device
    /// [DEFAULT]:  1
    uint32_t                    u32_MaxStrobeLength_us;

    /// Strobe Length step
    /// [DEFAULT]:  1
    uint32_t                    u32_StrobeLengthStep;

    /// A value of 0 means no flash support 
    /// Other values indicate possible number of strobe pulses per frame
    /// [DEFAULT]:  1
    uint8_t                     u8_MaxStrobesPerFrame;

    /// Number of frames for which flash is triggered (with respect to u8_FrameCount request in FlashControl_ts)
    /// [DEFAULT]:  0
    uint8_t                     u8_FlashFiredFrameCount;

    /// Gives the number of times Input Proc has been forcibly updated.
    /// Flash request to the sensor has not appeared in a frame for a consecutive
    /// number of frames which is as specified by the Control Page.
    /// [DEFAULT]:  0
    uint8_t                     u8_NumberOfForcedInputProcUpdates;

    /// Gives the number of consecutive frames for which flash
    /// has not appeared in a frame after it has been applied on the sensor.
    /// In an ideal scenario, it appears in the very next frame in which it is applied.
    /// [DEFAULT]:  0
    uint8_t                     u8_NumberOfConsecutiveDelayedFrames;

    /// Gives the total count of frames for which flash was out of sync
    /// [DEFAULT]:  0
    uint8_t                     u8_FlashSyncErrorCount;

    /// Flag which indicates to the SOF isr that it has to do the forced updation
    /// of Input Proc and let flash be configured again, even though
    /// due to some error flash applied on the sensor
    /// has not appeared in the MaximumNumberOfFrames as specified by the control page
    /// [DEFAULT]:  Flag_e_FALSE
    uint8_t                     e_Flag_ForceInputProcUpdation;

    /// Flash Errors
    /// [DEFAULT]: FlashError_e_FLASH_ERROR_NONE
    uint8_t                     e_FlashError_Info;

    /// Strobe Modulation support (At present not supported)
    /// [DEFAULT]: Flag_e_FALSE
    uint8_t                     e_Flag_StrobeModulationSupported;

}FlashStatus_ts;

/**
    Globals and Externs
*/
extern              FlashControl_ts   g_FlashControl;
extern              FlashStatus_ts    g_FlashStatus;
extern  volatile    uint8_t           g_FlashLitFrameCounter;
//This variable could be used in future instead of g_SystemConfig_Status.e_UpdateStatus_Flash
//extern              UpdateStatus_te   g_UpdateStatus_Flash;

/**
    Exported Functions
*/
extern void         Flash_HandleFlashTrigger(void) TO_EXT_DDR_PRGM_MEM;
extern void         Flash_HandleFlashErrors(void) TO_EXT_DDR_PRGM_MEM;

#endif  //FLASH_H_


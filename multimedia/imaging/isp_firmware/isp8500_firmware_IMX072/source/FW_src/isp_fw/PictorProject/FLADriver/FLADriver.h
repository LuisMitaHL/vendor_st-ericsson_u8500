/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \defgroup   FLADriver Module
 * \brief           This Module deals with the Actuator Driver .
*/

/**
 * \file        FLADriver.h
 * \brief       It's the Focus Lens Actuato Driver used by the Focus Control to drive the Focus Lens Actuator.
 * \ingroup     FLADriver
*/
#ifndef FLADRIVER_H_
#   define FLADRIVER_H_

#   include "Platform.h"
#   include "FLADriver_IPInterface.h"
#   include "FLADriver_OPInterface.h"
#   include "FLADriverSensor.h"

/**
 \enum      FLADriver_TimerId_te
 \brief         Timer2 is used as the timer for the Lens Processing Time , and so this Enum values corresponds to it At various points of execution .
 \ingroup   FLADriver
*/


typedef enum
{
    /// by default this value specify the Timer has not yet started .
    FLADriver_TimerId_e_TIMER_2_NOT_STARTED     = 0,

    /// This value shows the Timer 2 has started for a Lens Movement.
    FLADriver_TimerId_e_TIMER_2_STARTED,

    /// This shows the Timer has expired and the Suitable action has to be taken by the FW based on this .
    FLADriver_TimerId_e_TIMER_2_EXPIRED,

    /// This shows timer is waiting for The stop .
    FLADriver_TimerId_e_TIMER_2_WAITING_FOR_STOP,

    /// This shows the Timer has now Stopped .
    FLADriver_TimerId_e_TIMER_2_STOPPED
} FLADriver_Timer2Id_te;

/**
 \enum      FLADriver_APIError_te
 \brief         This enum shows the Lens Related Errors and Warnings during and after the Lens Movements .
 \ingroup   FLADriver
*/
typedef enum
{
    /// When No Error , this is the Default Set for the enum.
    FLADriver_APIError_e_NO_ERROR,

    /// This Shows the Timer for the Lens Movement started is expired before actully reaching to Final Position.
    FLADriver_APIError_e_TIMER_EXPIRE_BEFORE_REACHING_POS,

    ///  Lens Moved To target Position Sccessfully .
    FLADriver_APIError_e_LENS_MOVED_TO_TARGET,

    ///  The Lens Position Has been Measure , in respect to Position Measure Event.
    FLADriver_APIError_e_LENS_POSITION_MEASURED,

    /// The Lens Has Stoped Moving , in view of the Lens Stop Event.
    FLADriver_APIError_e_LENS_STOPPED,

    /// This Shows the Lens Position Can't be Measured , due to call to Low Level API .
    FLADriver_APIError_e_CANT_MEASURE_LENS_POS,

    /// If the Lens API Movement shows the Error , During the Call To Low Level API when the Actual Movement Takes Place .
    FLADriver_APIError_e_LENS_MOVE_API_FAILURE,

    /// Lens movement reached frame tolerance as set in g_FLADriver_Controls.u8_FrameTolerance.
    FLADriver_APIError_e_LENS_MOVE_REACHED_FRAME_TOLERANCE,
} FLADriver_APIError_te;

//-------------------------------------------------------
// END OF THE DEBUG

/**
 \enum      FLADriver_RangeDef_te
 \brief         Range for the Actuator movement ,
                selected at driver (Low Level)level ,
                it can be NVM level(data read from NVM ), HOST level(Host defined range).

 \ingroup FLADriver
*/
typedef enum
{
    /// The Default Range is the NVM range , which is basically taken or Read from the NVM and the
    /// True Movement of the Lens Is then Based on this Range.
    FLADriver_RangeDef_e_NVM_LEVEL_RANGE                    = 0x0,

    /// The Host Defined Range , In this Type of the Range Host set the Macro infinity and various
    /// other Nvm Parameters , based on the Notion of th eLens Movement for a particular Kind of Actuator
    /// or some standard values are assigned .
    FLADriver_RangeDef_e_HOST_DEFINED_RANGE                 = 0x1,
} FLADriver_RangeDef_te;

/**
 \enum      FLADriver_Direction_te
 \brief         Direction of actuator during the AF search .
 \ingroup   FLADriver
*/
typedef enum
{
    /// This enum value shows that no Direction is selected .
    FLADriver_Direction_e_DIR_STOP                          = 0,

    /// This Shows the Movement of the Lens Is in Forward direction
    FLADriver_Direction_e_FORWARDS                          = 1,

    /// This shows the Movement of the Lens is in opposite Direction .
    FLADriver_Direction_e_BACKWARDS                         = 255
} FLADriver_Direction_te;

/**
 \enum      FLADriver_ActuatorOrientation_te
 \brief         This enum depicts the Direction orientation of the actuator , either the high level end points
 \              are aligned with the end points of the Actuator or the end points of the actuator are reversed wrt to High level as in case of VCM.
 \ingroup   FLADriver
*/
typedef enum
{
    /// This Enum value shows that the Orientaion of the Lens Movement is a standard Movement,
    /// it means the Lens value Lower is on the Macro Side , While the Higher Value is on the Infinity Side .
    FLADriver_ActuatorOrientation_e_DIRECTION_ALIGNED_TO_STANDARDS,

    /// this Enum Value is the Default Values for the VCM and the Piezo Actuator, this Shows just opposite to the Above Values ,
    /// it shows the Greater value is on the MACRO side While the Lower Value is on the infinity side .
    FLADriver_ActuatorOrientation_e_DIRECTION_REVERSED_WRT_TO_STANDARDS
} FLADriver_ActuatorOrientation_te;

/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct        FLADriver_Dummy_ts
 \brief         Dummy Page Elements to be put in order to reamin with the binary compatablity .
 \ingroup       FLADriver
 \endif
*/
typedef struct
{
    uint16_t    u16_Dummy;
} FLADriver_Dummy_ts;

/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct        FLADriver_Status_ts
 \brief         This Structure Contains all the information in Low Level for Lens Movement .
                    Structure provide the Info about the lens at a Particular instance .
                    this basically gives the status information for Lens during the search .
 \ingroup       FLADriver
 \endif
*/
typedef struct
{
    ///  Status of the Min position that can be reached by the Lens during the Manual Focus Operation .
    uint16_t    u16_MinPos;

    ///  Status of the Max position that can be reached by the Lens during the Manual Focus Operation .
    uint16_t    u16_MaxPos;

    ///  Rest Position at low Level . position of lens when the lens doesn't do anything.
    uint16_t    u16_RestPos;

    ///  This Position is worst case Infinity Position (when camera is looking up and object at a
    ///  distance greater then 1 m has to be focused)at Driver Level .
    uint16_t    u16_InfinityFarEndPos;

    ///  Horizontal position of lens at low level when camera is held horizontally and focus the object at
    ///  a distance greater then 1 m .
    uint16_t    u16_InfinityHorPos;

    ///  Hyperfocal  Position of Lens in low Level, this position is the position just before the infinity ,
    ///  so that the object placed beyond this (From this position to infinity )are at same focus.
    uint16_t    u16_HyperFocalPos;

    ///  Position at which the object at 10cm is in perfect focus, when camera is held horizontal .
    ///  Macro Position of the Lens at low Level .
    uint16_t    u16_MacroHorPos;

    ///  this position is the worst case macro position , this is when the camera is held upside down
    ///  and focus at distance of 10 cm .
    uint16_t    u16_MacroNearEndPos;

    ///  The status of the Tolerance set by the host , the Tolerance is in low level and specify
    ///  the acceptance of error margin(in terms of absolute size , not in Percentage level) during the focus .
    int16_t     s16_ToleranceSize;

    /// status of the target set by the host in LowlevelParameters PE. target must be within the min and Max position .
    uint16_t    u16_TarSetPos;

    /// Focus Lens Manual step size required for step to macro and infinity movemnent .
    uint16_t    u16_ManualStepSize;

    ///  Low level position of the Lens , at actuator  driver level .Depending on the DAC size and Macro
    ///  and infinity position for the Module , this can vary from the Macro to infinity
    ///  (Low level) . also for many actuator this position is reverse . like VCM actuator .
    uint16_t    u16_CurrentPos;

    ///  Actual position that has to be reached when host issues a particular manual command .
    uint16_t    u16_TargetPos;

    ///  This position specify the previous position attained by the Lens when the command finishes its execution.
    uint16_t    u16_PreviousPos;

    ///  this variable specify the the actual displacement of the Lens when command is finished and Lens has been stoped .
    int16_t     s16_LastStepSizeExecuted;

    ///  This is to indicate if lens movement returned any error
    uint8_t     u8_Error;
} FLADriver_LLLCtrlStatusParam_ts;

/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct        FLADriver_Controls_ts
 \brief         this Structure provide the direct interface to the Host to configure
                    few Parameters of Actuator, but at fw level(High Level).
                    These parameter values are  used by the fw to Control the Actuator Movement.
 \ingroup       FLADriver
 \endif
*/
typedef struct
{
    /// specifies the maximum time multiplication factor in ms,applied to the time read from the LLA.
    uint16_t    u16_Ctrl_TimeLimit_ms;

    /// This variable specify that the Low level parameters should be initialised with the NVM values or HOST specified values .
    uint8_t     e_FLADriver_RangeDef_CtrlRange;

    /// This value is used for tolerance for lens move command absorption
    uint8_t     u8_FrameTolerance;
} FLADriver_Controls_ts;

/**
 \if        INCLUDE_IN_HTML_ONLY
 \struct        FLADriver_Status_ts
 \brief         This Structure Contains all the information in Low Level for Lens Movement .
                    Structure provide the Info about the lens at a Particular instance .
                    this basically gives the status information for Lens during the search .
 \ingroup       FLADriver
 \endif
*/
typedef struct
{
    ///  Number of times the Move to function is called .
    uint16_t    u16_Cycles;

    ///  specifies if the lens is currently moving or not
    uint8_t     e_Flag_LensIsMoving;

    ///  specifies if the range limits have been reached, this variable is set by the
    ///  fw if during the command opetation Lens reached either boundary(at Macro or infinity)
    uint8_t     e_Flag_LimitsExceeded;

    ///  Flag to indicate if the Low Level inititalization for the FLADriver has been done or not ,
    ///  if TRUE then the initialization has been done successfully otherwise not .
    uint8_t     e_Flag_LowLevelDriverInitialized;

    ///  This variable actually tells abt the orientation of the Macro- Infinity Range . it means that
    uint8_t     e_FLADriver_ActuatorOrientation_Type;

    /// This variable specify that status of the  Low level parameters settings happens based on the
    ///  NVM values or HOST specified values .
    uint8_t     e_FLADriver_RangeDef_StatusRange;
} FLADriver_Status_ts;

/**
 \if            INCLUDE_IN_HTML_ONLY
 \struct        FLADriver_NVMStoredData_ts
 \brief         The Data which is stored inside NVM .
 \          The stored data is read from the NVM and stored in this structure .
 \ingroup   FLADriver
 \endif
*/
typedef struct
{
    ///  Worst case infinity position read from NVM  (fixed inside sensor memory ,
    ///  fetched in Fw during the initialization , this value represents when the camera is
    ///  looking up at objectplaced at a distant greater then 1m ).
    int32_t s32_NVMInfinityFarEndPos;

    ///  Position stored in nvm for the current module when camera is held horizontal and
    ///  focuses the object placed greater then 1 m distance.
    int32_t s32_NVMInfinityHorPos;

    ///  NVM stored Hyperfocal position , definition is same as explained in above structures
    int32_t s32_NVMHyperfocalPos;

    ///  The Stored position for the current module when camera is held horizontally and focus at a distance of 10 cm .
    int32_t s32_NVMMacroHorPos;

    ///  The Rest position read from the NVM , This position can have different sifnificance for different actuator ,
    ///  but for Piezo , this is the position when the Actuator consume lowest power . so its low power state position.
    int32_t s32_NVMRestPos;

    ///     Worst case Macro position read from NVM (fixed inside sensor memory ,
    ///  fetched in Fw during the initialization , this value represents when the camera is looking down at 10 cm distant object).
    int32_t s32_NVMMacroNearEndPos;

    ///  This Variable is updated during the boot process , based on thr NVM content are present or not
    ///  this variable is updated to inform the upper layer about the present/absent of NVM information for the current module .
    uint8_t e_Flag_NVMDataPresent;

    ///  This variable is updated during the boot phase , where it reads the Lens information  from the LLD and initialize this flag .
    ///  it gives the information about the Presence/absence of Actuator .
    uint8_t e_Flag_NVMActuatorLensPresent;

    ///  This variable signifies that the Actuator in use has the Position sensor ,
    ///  it means that the actuator used has such a interface so that befor/after or uring the movement
    ///  of Lens the Actual position can be read from the HW and can be reflected by the PE .
    uint8_t e_Flag_NVMPositionSensorPresent;
} FLADriver_NVMStoredData_ts;

/**
 \if                INCLUDE_IN_HTML_ONLY
 \struct        FLADriver_LensLLDParam_ts
 \brief         Parameters for the Debugging Purposes when integrating with LLD APIs .

 \ingroup       FLADriver
 \endif
*/
typedef struct
{
    ///  Total time in micro seconds consumed by the Lens during 1 unit movement of lens  .
    uint32_t    u32_NVMLensUnitMovementTime_us;

    ///  Total time in micro seconds consumed by the Lens during a particular command movemnt set by the host .
    uint32_t    u32_TimeTakenByLensAPIs_us;

    /// Count for Timer interrupt call for the Timer2 .
    uint32_t    u32_FLADIntTimer2Count;

    /// The Count for the no of times the Timer2 is set for the Lens movement , total no of count for starting the lens Movement timer2.
    uint32_t    u32_FLADTimer2CallCount;

    /// the Actual difference between the target and the Current , after the movement command has been issued by the host .
    /// the difference is at the low level .
    uint16_t    u16_DiffFromTarget;

    ///  This shows the Error for the Timer APIs and Lens Movement  at various Level .
    uint8_t     e_FLADriver_APIError_Type;

    /// This variable shows the    State of the Timer2 at different level .shows information about Timer2 has NOT_STARTED,
    /// EXPIRED , WAITING FOR STOP , or STOPPED.
    uint8_t     e_FLADriver_Timer2Id_Type;
} FLADriver_LensLLDParam_ts;

//global variable ......
extern FLADriver_LensLLDParam_ts        g_FLADriver_LensLLDParam;
extern FLADriver_Status_ts              g_FLADriver_Status;
extern FLADriver_Controls_ts            g_FLADriver_Controls;
extern FLADriver_NVMStoredData_ts       g_FLADriver_NVMStoredData;
extern FLADriver_LLLCtrlStatusParam_ts  g_FLADriver_LLLCtrlStatusParam;
extern FLADriver_Dummy_ts               g_FLADriver_Dummy;
extern FLADriver_Dummy_ts               g_FLADriver_Dummy1;
extern FLADriver_Dummy_ts               g_FLADriver_Dummy2;
extern volatile uint16_t                g_lensMoveCmdabsorbedInFrame;

// global functions
extern uint8_t FLADriver_LowLevelInitialization ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_GoToTargetPosition ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_GoToInfinityFarEnd ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_GoToMacroNearEnd ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_HostCommsManager ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_ReadNvmDetails ( void ) TO_EXT_DDR_PRGM_MEM;
extern uint8_t FLADriver_Initialize ( void ) TO_EXT_DDR_PRGM_MEM;
extern uint16_t FLADriver_Move (int16_t, uint8_t) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_MoveTo (int16_t) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_AssignDefaultDetails ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_AssignNVMDetails ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_GoToRest ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_GoToHyperfocal ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_GoToInfinityHor ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_GoToMacroHor ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_LLAEventHandler (CAM_DRV_EVENT_E, CAM_DRV_EVENT_INFO_U *, uint8_t) TO_EXT_DDR_PRGM_MEM;
extern void FLADriver_StartTimer(uint32_t) TO_EXT_DDR_PRGM_MEM;

#endif /*FLADRIVER_H_*/


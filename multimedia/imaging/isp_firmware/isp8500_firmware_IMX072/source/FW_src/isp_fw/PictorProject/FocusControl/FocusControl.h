/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \defgroup FocusControl Module
 * \brief Contains the Focus Fontrol module and the management of the Autofocus features
*/

/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \file            FocusControl.h
 * \brief           Header File containing external function declarations and defines for
 *              FocusControl Algorithm and FocusControl Compiler
 * \ingroup         FocusControl
 * \endif
*/
#ifndef FOCUSCONTROL_H_
#   define FOCUSCONTROL_H_

//Includes
#   include "Platform.h"
#   include "FocusControl_IPInterface.h"
#   include "FocusControl_OPInterface.h"

//global variables

/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \enum        FocusControl_AFFailureMsg_te
 * \brief           Enum to give the AF failure and error Message ..
 * \details         According to the status, This Depicts different types of error and warning for the Focus Control Machananism.
 * \ingroup         FocusControl
 * \endif
*/
typedef enum
{
    /// when no error .
    FocusControl_FocusMsg_e_NO_ERROR,

    /// When there is no Lens Present inside Module , usually the Information collected From the Low Level API.
    FocusControl_FocusMsg_e_NO_ACTUATOR_LENS,

    /// when the NVM data is not Present and the READ from NVM is Selected , then in that Case , assigning the Default NVM Data.
    FocusControl_FocusMsg_e_NO_NVM_DATA_DEFAULT_ASSIGNED,

    /// When the Host Mode is selected and no data has been added to the variables , in that case by default fiest the NVM data will be assigned , if present .
    FocusControl_FocusMsg_e_NO_HOST_DATA_NVM_ASSIGNED,

    /// in case if NVM data is also not present then the Default Values will be assigned to the variables which are hardcoded defines.
    FocusControl_FocusMsg_e_NO_HOST_DATA_DEFAULT_ASSIGNED,
} FocusControl_FocusMsg_te;

/*
 * \if          INCLUDE_IN_HTML_ONLY
* \enum             FocusControl_LensCommand_te
* \brief            Command given to Lens during the Manual Command Mode .
* \             Host can issue any of these command and fw
* \             will provide command to actuator driver to move
* \
* \             the Lens to Appropriate Position. Generally known as LensCommand .
 *\ingroup      FLADriver
*/
typedef enum
{
    /// issue a  move forward command.
        ///the step is determined by g_FLADriver_LLLCtrlStatusParam.u16_ManualStepSize.
    FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_INFINITY,

    /// issue a move backward command
        ///the step is determined by g_FLADriver_LLLCtrlStatusParam.u16_ManualStepSize.
    FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_MACRO,

    /// issue the Lens driver to go to the Worst case infinity position.
    FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_FAR_END,

    /// issue the Lens driver to go to the worst case macro position.
    FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_NEAR_END,

    /// issue the Lens driver to go to the rest position.
    FocusControl_LensCommand_e_LA_CMD_GOTO_REST,

    /// issue the Lens driver to go to the target position.
    FocusControl_LensCommand_e_LA_CMD_GOTO_TARGET_POSITION,

    /// issue the Lens driver to go to the Infinity Horizontal position.
    FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_HOR,

    /// issue the Lens driver to go to the Horizontal Macro position.
    FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_HOR,

    /// issue the Lens driver to go to the Hyperfocal  position.
    FocusControl_LensCommand_e_LA_CMD_GOTO_HYPERFOCAL,

    /// driver level initialization, command , reinitialize the driver as done during the first boot time.
} FocusControl_LensCommand_te;

/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \struct      FocusControl_Dummy_ts
 * \brief           Dummy to PAD the PE so that the Binary compatiblity will remains .
 * \details
 * \ingroup         FocusControl
 * \endif
*/
typedef struct
{
    uint16_t    u16_Dummy;
} FocusControl_Dummy_ts;

/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \struct      FocusControl_Controls_ts
 * \brief           Structure to give Status about the Focuscontrol PE  .
 * \details         Status of the command, mode, range, error , driver and AF stablity etc are shown in by the elements of this Page .
 * \ingroup         FocusControl
 * \endif
*/
typedef struct
{
    /// Manual focus commands and other actuator commands.
    uint8_t e_FocusControl_LensCommand_Control;

    /// Flag to behave as a Control Side of Coin, This Flag intiate the Lens Movement when Toggled by the Host.
    uint8_t e_Coin_Control;

    /// this variable is set to TRUE if the host want to export stats  as well as the Lens Movement at the same time.
    uint8_t e_Flag_StatsWithLensMove_Control;
} FocusControl_Controls_ts;

/**
 * \if          INCLUDE_IN_HTML_ONLY
 * \struct      FocusControl_Status_ts
 * \brief           Structure to give Status about the Focuscontrol PE  .
 * \details         Status of the command, mode, range, error , driver and AF stablity etc are shown in by the elements of this Page .
 * \ingroup         FocusControl
 * \endif
*/
typedef struct
{
    /// Focus Control Cycles, take the count of the AFStats Ready ISR execution.
    uint16_t    u16_Cycles;

    /// This valiable shows the Status of the Manual focus ccommands and other actuator commands
    uint8_t     e_FocusControl_LensCommand_Status;

    /// This element shows the details about any Error/Warning detected by the Focus Control module.
    uint8_t     e_FocusControl_FocusMsg_Status;

    /// Flag to behave as a Status Side of Coin, which is use to Toggle the Coin for Processing the Command Execution.
    uint8_t     e_Coin_Status;

    /// This element tell to the Focus control that at the SOF the lens is still moving
        /// so that the current frame probably isn't very good for Stats Exporting.
    uint8_t     e_Flag_LensIsMovingAtTheSOF;

    /// Signaling the  the Focus System is correctly stable, and the command For the New Manual Focus can be anticipated.
    uint8_t     e_Flag_IsStable;

    /// Signaling a serious error detected by the Focus Control module, this is generally the initialization error.
    uint8_t     e_Flag_Error;

    /// status of the variable set for the AFstats Exporting as well as for the Command Mode at the same time.
    uint8_t     e_Flag_StatsWithLensMove_Status;
} FocusControl_Status_ts;

// Global Variables exported from FocusControl.c
/// Dummy PE , Crerated in order to have the same Binary compatiblity.
extern FocusControl_Dummy_ts    g_FocusControl_Dummy;

/// status of the Focus control PE are shown.
extern FocusControl_Status_ts   g_FocusControl_Status;

/// control element for the Focus ,this can be set by the host to perform the specific action .
extern FocusControl_Controls_ts g_FocusControl_Controls;
#endif /*FOCUSCONTROL_H_*/


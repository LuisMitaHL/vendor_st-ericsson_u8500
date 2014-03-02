/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \file    FocusControl_OPInterface.h
 * \brief   Header File containing the output interface provided by the FocusControl Module.
 * \ingroup FocusControl
 */
#ifndef FOCUSCONTROL_OPINTERFACE_H_
#   define FOCUSCONTROL_OPINTERFACE_H_

#   include "FocusControl.h"
#   include "cam_drv.h"

// Making AutoFocus Present
extern void FocusControl_AFStatsReadyISR ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FocusControl_BootInitialize ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FocusControl_TopLevelInterfaceOnIdle ( void ) TO_EXT_DDR_PRGM_MEM;
extern void FocusControl_SOFISR ( void ) TO_EXT_DDR_PRGM_MEM;

#   define FocusControl_SetTLILensCommand(x)               (g_FocusControl_Controls.e_FocusControl_LensCommand_Control = x)
#   define FocusControl_GetTLILensCommand()                (g_FocusControl_Controls.e_FocusControl_LensCommand_Control)
#   define FocusControl_SetStatusTLILensCommand(x)         (g_FocusControl_Status.e_FocusControl_LensCommand_Status = x)
#   define FocusControl_GetFocusStatusTLILensCommand()     (g_FocusControl_Status.e_FocusControl_LensCommand_Status)
#   define FocusControl_GetStatusLensIsMovingAtTheSOF()    (g_FocusControl_Status.e_Flag_LensIsMovingAtTheSOF)
#   define FocusControl_SetStatusLensIsMovingAtTheSOF(x)   (g_FocusControl_Status.e_Flag_LensIsMovingAtTheSOF = x)
#   define FocusControl_SetCmdControlCoin(cmd_coin)        (g_FocusControl_Controls.e_Coin_Control = cmd_coin)
#   define FocusControl_GetCmdControlCoin()                (g_FocusControl_Controls.e_Coin_Control)

// Getting Coin of the Focus control for status of the coin .
#   define FocusControl_SetCmdStatusCoin(cmd_coin) (g_FocusControl_Status.e_Coin_Status = cmd_coin)
#   define FocusControl_GetCmdStatusCoin()         (g_FocusControl_Status.e_Coin_Status)
#   define FocusControl_GetStatusWithLensMove()    (g_FocusControl_Status.e_Flag_StatsWithLensMove_Status)
#   define FocusControl_SetStatusWithLensMove(x)   (g_FocusControl_Status.e_Flag_StatsWithLensMove_Status = x)
#   define FocusControl_GetControlWithLensMove()   (g_FocusControl_Controls.e_Flag_StatsWithLensMove_Control)
#   define FocusControl_SetControlWithLensMove(x)  (g_FocusControl_Controls.e_Flag_StatsWithLensMove_Control = x)
#   define FocusControl_SetErrorMsg(x)             (g_FocusControl_Status.e_FocusControl_FocusMsg_Status = x)
#   define FocusControl_GetFullRangeMinPosition(x) (g_FLADriver_LLLCtrlStatusParam.u16_MinPos)
#   define FocusControl_GetFullRangeMaxPosition(x) (g_FLADriver_LLLCtrlStatusParam.u16_MaxPos)
#   define FocusControl_IsLensActuatorPresent()    (g_camera_details.p_lens_details != NULL)
#endif /*FOCUSCONTROL_OPINTERFACE_H_*/


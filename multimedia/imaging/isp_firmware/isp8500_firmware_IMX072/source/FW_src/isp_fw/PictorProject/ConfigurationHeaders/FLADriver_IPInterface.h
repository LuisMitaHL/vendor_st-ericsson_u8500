/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file FLADriver_IPInterface.h
 \brief Header File defining the Input Dependencies of the FLADriver Module.
 \ingroup FLADriver
*/
#ifndef FLADRIVER_IPINTERFACE_H_
#   define FLADRIVER_IPINTERFACE_H_

#   include "MasterI2C_op_interface.h"
#   include "FocusControl.h"
#   include "Exposure_OPInterface.h"
#   include "SensorManager.h"
#   include "SystemConfig.h"
#   include "EventManager.h"
#   include "Platform.h"

///  This Macro set the status of the lens is moving at the start of the frame with the value .
#   define FLADriver_FocusControlSetLensIsMovingAtTheSOF(x)    FocusControl_SetStatusLensIsMovingAtTheSOF(x)

///  This Macro get the status abt the TOP Level INTERFACE  Lens command , move to /macro , infinity , move to target value or , move step towards infinity , or may be stop actuator , move to horizontal position etc etc  .
#   define FLADriver_FocusControlGetStatusTLILensCommand() FocusControl_GetFocusStatusTLILensCommand()

///  This Macro get the Control abt the TOP Level INTERFACE Lens command , move to /macro , infinity , move to target value or , move step towards infinity , or may be stop actuator , move to horizontal position etc etc  .
#   define FLADriver_FocusControlGetTLILensCommand()   FocusControl_GetTLILensCommand()

///  This Macro Set the status of the TOP Level INTERFACE  Lens command  with value can be , move to Macro , infinity , move to target value or , move step towards infinity , or may be stop actuator , move to horizontal position etc etc  .
#   define FLADriver_FocusControlSetStatusTLILensCommand(x)    FocusControl_SetStatusTLILensCommand(x)


#   define FLADriver_FocusControlGetCmdControlCoin()           FocusControl_GetCmdControlCoin()
#   define FLADriver_FocusControlSetCmdStatusCoin(cmd_coin)    FocusControl_SetCmdStatusCoin(cmd_coin)
#   define FLADriver_FocusControlGetCmdStatusCoin()            FocusControl_GetCmdStatusCoin()
#   define FLADriver_FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_INFINITY \
        FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_INFINITY
#   define FLADriver_FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_MACRO \
        FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_MACRO
#   define FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_FAR_END \
        FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_FAR_END
#   define FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_HOR \
        FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_HOR
#   define FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_NEAR_END \
        FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_NEAR_END
#   define FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_REST   FocusControl_LensCommand_e_LA_CMD_GOTO_REST
#   define FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_TARGET_POSITION \
        FocusControl_LensCommand_e_LA_CMD_GOTO_TARGET_POSITION
#   define FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_HOR  FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_HOR
#   define FLADriver_FocusControl_LensCommand_e_LA_CMD_GOTO_HYPERFOCAL FocusControl_LensCommand_e_LA_CMD_GOTO_HYPERFOCAL

#   define FLADriver_FocusControlSetErrorMsg(x)                        FocusControl_SetErrorMsg(x)
#   define FLADriver_AFStatsSetStatsHostcmdCtrl(x)                     AFStats_SetStatsHostcmdCtrl(x)


#   define FLADriver_AFStatsGetStatsWithLensMoveFWStatus()             AFStats_GetStatsWithLensMoveFWStatus()
#   define FLADriver_AFStatsSetStatsWithLensMoveFWStatus(x)            AFStats_SetStatsWithLensMoveFWStatus(x)



#   define MOD(a)                                                      ((a > 0) ? (a) : (-a))
#   define FDLADriver_ABS(x)                                           MOD(x)
#   define FDLADriver_ABS_DIFF(x, y)                                   MOD((x - y))


/// This Macro Calculate the ABS difference between two values Passed x and y.
#   define FLADriver_LensStopNotify()  EventManager_FLADRiverLensStop_Notify()
#endif /*FLADRIVER_IPINTERFACE_H_*/


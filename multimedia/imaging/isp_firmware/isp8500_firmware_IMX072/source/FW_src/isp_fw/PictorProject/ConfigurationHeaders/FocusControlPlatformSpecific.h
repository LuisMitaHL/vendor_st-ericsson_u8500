/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 * \file 		  FocusControlPlatformSpecific.h
 * \brief 		  This header file is a part of the Focus Control Module .
 * 				  header file Contains the defines for the default value for the Control
 * 				  and status of Focus control module .
 * 				  Apart from this also conatins default for the range which are generally
 * 				  taken from the FLADriverSensor file .
 * \ingroup 	  FocusControl
 */
#ifndef FOCUSCONTROLPLATFORMSPECIFIC_H_
#   define FOCUSCONTROLPLATFORMSPECIFIC_H_

// these file to be included at the time of integration .
#   include "FLADriverSensor.h"

#   define FOCUSCONTROL_DEFAULT_CONTROL_LENS_COMMAND               FocusControl_LensCommand_e_LA_CMD_GOTO_REST
#   define FOCUSCONTROL_DEFAULT_CONTROL_COIN                       Coin_e_Heads
#   define FOCUSCONTROL_DEFAULT_CONTROL_STATS_WITH_LENS_MOVE       Flag_e_FALSE

#   define FOCUSCONTROL_DEFAULT_STATUS_LENS_COMMAND                FocusControl_LensCommand_e_LA_CMD_GOTO_REST
#   define FOCUSCONTROL_DEFAULT_STATUS_ERROR_CODE                  FocusControl_FocusMsg_e_NO_ERROR
#   define FOCUSCONTROL_DEFAULT_STATUS_COIN                        Coin_e_Heads
#   define FOCUSCONTROL_DEFAULT_STATUS_LENS_IS_MOVING_AT_THE_SOF   Flag_e_FALSE
#   define FOCUSCONTROL_DEFAULT_STATUS_IS_STABLE                   Flag_e_FALSE
#   define FOCUSCONTROL_DEFAULT_STATUS_ERROR                       Flag_e_FALSE
#   define FOCUSCONTROL_DEFAULT_STATUS_CYCLES                      0
#   define FOCUSCONTROL_DEFAULT_STATUS_STATS_WITH_LENS_MOVE        Flag_e_FALSE
#endif /*FOCUSCONTROLPLATFORMSPECIFIC_H_*/


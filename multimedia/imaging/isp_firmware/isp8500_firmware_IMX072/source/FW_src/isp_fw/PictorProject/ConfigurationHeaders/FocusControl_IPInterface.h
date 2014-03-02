/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file  FocusControl_IPInterface.h
 \brief Header File defining the Input Dependencies of the FocusControl Module.
 \ingroup FocusControl
*/
#ifndef FOCUSCONTROL_IPINTERFACE_H_
#   define FOCUSCONTROL_IPINTERFACE_H_

#   include "DeviceParameter.h"
#   include "AFStats_OPInterface.h"
#   include "FLADriver_OPInterface.h"
#   include "SystemConfig.h"
#   include "Exposure_OPInterface.h"

/// Defines for Focus control taken from AutoFocus Moduel (Functions of AutoFocus)

/// This Macro is for the call of AutoFocus modules AutoFocus_ColdStart function .
/// This function perform a full initialization of af. To be used to enable af, when a big scene


/// This Macro actually fetch the Stats value from hw registers to the  fw structure .
#   define FocusControl_AFStatsExtractMeanStatistics()  AFStats_ProcessingHostCmd()

/// This macro actually get the flag value for the autoRefresh .
/// this flag Control the automatic update of af stats settings at the beginning of each frame.
#   define FocusControl_AFStatsGetAutoRefresh()    AFStats_GetAutoRefreshCtrl()

/// This Macro setup the AF stats .
#   define FocusControl_AFStatsSetupStats()            AFStats_SetupStats()


/// Defines for Focus control taken from FLADriver_OPInterface.h file (Defines of FLADriver_OPInterface)*/
/// Get the value for the Limit has been excedded for the Low level driver (control flag if value out of Low level range ).


/// Macro Basically calls the Host comms function of the Fladriver .
#   define FocusControl_FLADriverHostCommsManager()    FLADriver_HostCommsManager()

/// Macro initializes the FLADriver .
#   define FocusControl_FLADriverInitialize()  FLADriver_Initialize()

/// Macro Execute the FLADriver Command .
/// get the Status of the FLADriver stablity .
#   define FocusControl_FLADriverIsStable()    FLADriver_IsStable()

//#define FocusControl_FLADriverIsUpdateRequired()				FLADriver_IsUpdateRequired()
#   define FocusControl_FLADriverGetLensISMoving() FLADriver_GetLensISMoving()


#endif /*FOCUSCONTROL_IPINTERFACE_H_*/


/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef HOSTCOMMS_OPINTERFACE_H_
#define HOSTCOMMS_OPINTERFACE_H_

/**
 \file HostComms_OPInterface.h
 \brief This file is a part of the module release code. It provides an 
        interface to the host comms module. All functionalities offered by 
        the module are available through this file.
 \ingroup HostComms
*/

#include "HostComms.h"


extern void HostComms_ISRs (void);


/// Returns TRUE if a mode static page has been written onto and FALSE otherwise.
/// Whenever a write to a mode static page happens, the status will
/// be retained until cleared through HostComms_ResetModeStaticSetupChanged() 
#define HostComms_IsModeStaticSetupChanged()		(g_HostComms_Status.bo_ModeStaticSetupChanged)


/// Should be used to clear the mode static setup status. 
#define HostComms_ResetModeStaticSetupChanged()		(g_HostComms_Status.bo_ModeStaticSetupChanged = FALSE)

extern HostComms_Status_ts g_HostComms_Status;


#endif /*HOSTCOMMS_OPINTERFACE_H_*/

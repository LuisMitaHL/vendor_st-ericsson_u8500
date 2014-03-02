/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* \brief   This file contains the methods to read a dummy page element to facilitate HSM state transitions for YUV module
 *               It uses the "mExtIspctlCommand" interface coming from the CExtIspctlComponentManager to send message to the EXTISPTCL
 *               The answers to these messsages  are received in the callbacks provided  in CExtIspctlComponentManager.
 */

#include "ENS_DBC.h"
#include "ext_ispctl_communication_manager.h"
#include "debug_trace.h"
#include <cm/inc/cm.hpp>

void CExtIspctlCom::setExtIspctlCmd(Iext_ispctl_api_cmd val){
    mExtIspctlCommand = val;
}

Iext_ispctl_api_cmd CExtIspctlCom::getExtIspctlCmd(){
    return mExtIspctlCommand;
}

/*
 * will read one PE
 */
void CExtIspctlCom::requestPE(t_uint16 addr){

    if(mPanic==OMX_TRUE)return;

    mExtIspctlCommand.readPageElement(addr, clientId);
}


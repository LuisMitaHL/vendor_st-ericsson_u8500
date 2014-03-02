/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_MpcController.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "ENS_Nmf.h"
#include "AFM_MpcController.h"
#include "AFMNmfMpc_ProcessingComp.h"
#include "ENS_Component.h"
#include "ENS_Port.h"


OMX_ERRORTYPE
AFM_MpcController::instantiate(OMX_U32 priority) {
    OMX_ERRORTYPE error;

    error = ENS::instantiateNMFComponent(
            mENSComponent.getNMFDomainHandle(), 
            "misc.controller", "ctrl", &mNmfCtrl, priority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            mENSComponent.getNMFDomainHandle(), 
            "fsm.generic", "genericfsm", &mNmfGenericFsm, priority);
    if (error != OMX_ErrorNone) return error;
    
 	error = ENS::bindComponent(
            mNmfCtrl, "osttrace", mAFMProcessingComp.getOstTrace(), "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfCtrl, "genericfsm", mNmfGenericFsm, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(
	     mNmfCtrl, "sendcommand", mAFMProcessingComp.getNmfSendCommandItf(), NB_MAX_PORTS);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(
            mNmfCtrl, "fsminit", mAFMProcessingComp.getNmfFsmInitItf(), 1);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(
            mNmfCtrl, "configure", &mIconfigure, 1);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentToHost(
	    mENSComponent.getOMXHandle(), mNmfCtrl, "proxy", mAFMProcessingComp.getEventHandlerCB(), NB_MAX_PORTS);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_MpcController::deInstantiate(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponentToHost(mENSComponent.getOMXHandle(), mNmfCtrl, "proxy");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost(&mIconfigure);
    if (error != OMX_ErrorNone) return error;
    
    error = ENS::unbindComponentFromHost((NMF::InterfaceReference *) mAFMProcessingComp.getNmfFsmInitItf());
    if(error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost((NMF::InterfaceReference *) mAFMProcessingComp.getNmfSendCommandItf());
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfCtrl, "genericfsm");
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfCtrl, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfGenericFsm);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfCtrl);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_MpcController::start(void) {
    OMX_ERRORTYPE error;
    
    error = ENS::startNMFComponent(mNmfGenericFsm);
    if (error != OMX_ErrorNone) return error;
       
    error = ENS::startNMFComponent(mNmfCtrl);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_MpcController::stop(void) {
    OMX_ERRORTYPE error;
    
    error = ENS::stopNMFComponent(mNmfGenericFsm);
    if (error != OMX_ErrorNone) return error;
        
    error = ENS::stopNMFComponent(mNmfCtrl);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_MpcController::configure(void) {
    int portDirections = 0;

    for(unsigned int i = 0; i < mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->getDirection() == OMX_DirOutput) {
            portDirections |= 1 << i;
        }
    }

    mIconfigure.setParameter(portDirections);

    return OMX_ErrorNone;
}

static const char* bindingItfName[NB_MAX_PORTS]=
{
    "binding[0]",
    "binding[1]",
    "binding[2]",
    "binding[3]",
    "binding[4]",
    "binding[5]",
    "binding[6]",
    "binding[7]",
    "binding[8]",
    "binding[9]"
};

static const char* bindingInitItfName[NB_MAX_PORTS]=
{
    "bindinginit[0]",
    "bindinginit[1]",
    "bindinginit[2]",
    "bindinginit[3]",
    "bindinginit[4]",
    "bindinginit[5]",
    "bindinginit[6]",
    "bindinginit[7]",
    "bindinginit[8]",
    "bindinginit[9]"
};

static const char* ctrlItfName[NB_MAX_PORTS]=
{
    "eventhandler[0]",
    "eventhandler[1]",
    "eventhandler[2]",
    "eventhandler[3]",
    "eventhandler[4]",
    "eventhandler[5]",
    "eventhandler[6]",
    "eventhandler[7]",
    "eventhandler[8]",
    "eventhandler[9]"
};


OMX_ERRORTYPE
AFM_MpcController::bindMain(t_cm_instance_handle mainComp) {
    OMX_ERRORTYPE error;

    error = ENS::bindComponentAsynchronousEx(
        mNmfCtrl, "maincomponent", mainComp, "sendcommand", 2); 
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronousEx(
        mNmfCtrl, "maincompinit", mainComp, "fsminit",2);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronousEx(
            mainComp, "proxy", mNmfCtrl, "maineventhandler", 4);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_MpcController::unbindMain(t_cm_instance_handle mainComp) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponentAsynchronous(mNmfCtrl, "maincomponent");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous(mNmfCtrl, "maincompinit");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous(mainComp, "proxy");
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_MpcController::bindBinding(t_cm_instance_handle bindingComp, int portIdx) {
    OMX_ERRORTYPE error;

    DBC_ASSERT(portIdx < NB_MAX_PORTS);

    // the portIdx s checked by the assert above
    //coverity[overrun-local]
    error = ENS::bindComponentAsynchronousEx(
         mNmfCtrl, bindingItfName[portIdx], bindingComp, "sendcommand",2);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronousEx(
        mNmfCtrl, bindingInitItfName[portIdx], bindingComp, "fsminit",2);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronousEx(
            bindingComp, "proxy", mNmfCtrl, ctrlItfName[portIdx], 4);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_MpcController::unbindBinding(t_cm_instance_handle bindingComp, int portIdx) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponentAsynchronous(mNmfCtrl, bindingItfName[portIdx]);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous(mNmfCtrl, bindingInitItfName[portIdx]);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous(bindingComp, "proxy");
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}



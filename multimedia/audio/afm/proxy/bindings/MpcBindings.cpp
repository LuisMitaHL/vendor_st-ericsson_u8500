/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MpcBindings.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "MpcBindings.h"
#include "AFM_nmf.h"

MpcBindings::MpcBindings(AFM_Port &port, AFM_Controller &controller, OMX_U32 priority)
  : Bindings(port, controller), mNmfHandle(0), mOstTrace(0), mPriority(priority), 
    mNmfGenericFsmLib(0), mNmfComponentFsmLib(0)
{
    OMX_PARAM_PORTDEFINITIONTYPE portdef;

    portdef.nSize           = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);    
    port.getParameter(OMX_IndexParamPortDefinition, &portdef);
    
    if(portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM) {
        mIsPCM = OMX_TRUE;
    } else {
        mIsPCM = OMX_FALSE;
    }
}

OMX_ERRORTYPE MpcBindings::start(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfHandle);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mNmfGenericFsmLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mNmfComponentFsmLib);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MpcBindings::stop(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfHandle);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent(mNmfGenericFsmLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent(mNmfComponentFsmLib);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MpcBindings::instantiate(void) {
    OMX_ERRORTYPE error;
    OMX_U32 priority = getPortPriorityLevel();

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "osttrace.mmdsp", 
            "OSTTRACE", &mOstTrace, priority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "fsm.generic", "genericfsm", 
            &mNmfGenericFsmLib, priority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "fsm.component", "componentfsm", 
            &mNmfComponentFsmLib, priority);
    if (error != OMX_ErrorNone) return error;

    error = instantiateBindingComponent();
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfHandle, "genericfsm", mNmfGenericFsmLib, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfHandle, "componentfsm", mNmfComponentFsmLib, "componentfsm");
    if (error != OMX_ErrorNone) return error;
   
	error = ENS::bindComponent(
            mNmfComponentFsmLib, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfComponentFsmLib, "genericfsm", mNmfGenericFsmLib, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronous(
        mNmfHandle, "me", mNmfHandle, "postevent", 2);
    if (error != OMX_ErrorNone) return error;

    error = mController.bindBinding(mNmfHandle, mPort.getPortIndex());
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MpcBindings::deInstantiate(void) {
    OMX_ERRORTYPE error;

    error = mController.unbindBinding(mNmfHandle, mPort.getPortIndex());
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous(mNmfHandle, "me");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(
            mNmfHandle, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(
            mNmfHandle, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(
            mNmfComponentFsmLib, "genericfsm");
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(
            mNmfComponentFsmLib, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = deInstantiateBindingComponent();
    if (error != OMX_ErrorNone) return error;

	error = ENS::destroyNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfComponentFsmLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfGenericFsmLib);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MpcBindings::connectToInput(t_cm_instance_handle hNmfOut,const char * outSuffix)
{
    OMX_ERRORTYPE error;
    
    error = AFM::connectPorts(hNmfOut, outSuffix, mNmfHandle, "", mIsPCM);

    return error;
    
}

OMX_ERRORTYPE MpcBindings::connectToOutput(t_cm_instance_handle hNmfOut,const char * outSuffix)
{
    OMX_ERRORTYPE error;
    
    error = AFM::connectPorts(mNmfHandle, "",hNmfOut, outSuffix, mIsPCM);

    return error;
    
}

OMX_ERRORTYPE MpcBindings::disconnectFromInput(t_cm_instance_handle hNmfIn,const char * inSuffix)
{
    OMX_ERRORTYPE error;
    
     error = AFM::disconnectPorts(hNmfIn, inSuffix, mNmfHandle,"", mIsPCM);

     return error;
}

OMX_ERRORTYPE MpcBindings::disconnectFromOutput(t_cm_instance_handle hNmfIn,const char * inSuffix)
{
    OMX_ERRORTYPE error;
    
     error = AFM::disconnectPorts(mNmfHandle,"", hNmfIn, inSuffix, mIsPCM);

     return error;
}


OMX_U32 MpcBindings::getPortPriorityLevel()
{
    OMX_U32 prio_comp=0,prio_tunneledcomp=0,prio_binding=0;
    if (mPort.getTunneledComponent() && !mPort.useStandardTunneling())
    {
        prio_tunneledcomp = ((AFM_Port *)(mPort.getENSTunneledPort()))->getPortPriorityLevel();
    }

    prio_comp= ((AFM_Port *)&mPort)->getPortPriorityLevel();

    prio_binding=MAX(prio_comp,prio_tunneledcomp);
    if (prio_binding > NMF_SCHED_URGENT)
    {
        prio_binding = NMF_SCHED_URGENT;
    }
    
    return prio_binding;
}    


/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostBindings.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "share/inc/macros.h"
#include "HostBindings.h"

HostBindings::HostBindings(AFM_Port &port, AFM_Controller &controller, OMX_HANDLETYPE omxhandle) 
  : Bindings(port, controller), mNmfHandle((NMF::Composite *)0), mOMXHandle(omxhandle)
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

    mControllerItfIdx   = 0;
}

inline OMX_ERRORTYPE HostBindings::start(void) {

    mNmfHandle->start();

    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE HostBindings::stop(void) {

    mNmfHandle->stop_flush();

    return OMX_ErrorNone;
}

OMX_ERRORTYPE HostBindings::instantiate(void) {
    OMX_ERRORTYPE error;

    error = instantiateBindingComponent();
    if (error != OMX_ErrorNone) return error;

//     error = mNmfHandle.bindComponentAsynchronous(
//         mNmfHandle, "me", mNmfHandle, "postevent", 2);
//     if (error != OMX_ErrorNone) return error;

    error = mController.bindBinding(mNmfHandle, mPort.getPortIndex());
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE HostBindings::deInstantiate(void) {
    OMX_ERRORTYPE error;

    error = mController.unbindBinding(mNmfHandle, mPort.getPortIndex());
    if (error != OMX_ErrorNone) return error;

//     error = ENS::unbindComponentAsynchronous(mNmfHandle, "me");
//     if (error != OMX_ErrorNone) return error;

    error = deInstantiateBindingComponent();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE HostBindings::connectToInput(NMF::Composite* hNmfOut,const char * outSuffix)
{
    OMX_ERRORTYPE error;
    
    error = AFM::hostConnectPorts(hNmfOut, outSuffix, mNmfHandle, "", mIsPCM);

    return error;
    
}

OMX_ERRORTYPE HostBindings::connectToOutput(NMF::Composite* hNmfOut,const char * outSuffix)
{
    OMX_ERRORTYPE error;
    
    error = AFM::hostConnectPorts(mNmfHandle, "",hNmfOut, outSuffix, mIsPCM);

    return error;
    
}

OMX_ERRORTYPE HostBindings::disconnectFromInput(NMF::Composite* hNmfIn,const char * inSuffix)
{
    OMX_ERRORTYPE error;
    
     error = AFM::hostDisconnectPorts(hNmfIn, inSuffix, mNmfHandle,"", mIsPCM);

     return error;
}

OMX_ERRORTYPE HostBindings::disconnectFromOutput(NMF::Composite* hNmfIn,const char * inSuffix)
{
    OMX_ERRORTYPE error;
    
     error = AFM::hostDisconnectPorts(mNmfHandle,"", hNmfIn, inSuffix, mIsPCM);

     return error;
}

OMX_U32 HostBindings::getPortPriorityLevel()
{
    OMX_U32 prio_comp=0,prio_tunneledcomp=0,prio_binding=0;
    if (mPort.getTunneledComponent() && !mPort.useStandardTunneling())
    {
        prio_tunneledcomp = ((AFM_Port *)(mPort.getENSTunneledPort()))->getPortPriorityLevel();
    }

    prio_comp= ((AFM_Port *)&mPort)->getPortPriorityLevel();

    prio_binding=MAX(prio_comp,prio_tunneledcomp);
    return prio_binding;
}


AFM_API_EXPORT OMX_ERRORTYPE
AFM::hostConnectPorts(
        NMF::Composite * hNmfOut, 
        const char * outSuffix,
        NMF::Composite * hNmfIn,
        const char * inSuffix,
        bool pcm) 
{
    t_nmf_error   nmf_error;

    //coverity[forward_null]
    DBC_ASSERT(hNmfOut && hNmfIn);
    //coverity[forward_null]
    DBC_ASSERT(outSuffix && inSuffix);

    ENS_String<20> outputport("outputport");
    ENS_String<20> fillthisbuffer("fillthisbuffer");
    ENS_String<20> outputsettings("outputsettings");

    //coverity[forward_null]
    outputport.concat(outSuffix);
    fillthisbuffer.concat(outSuffix);
    outputsettings.concat(outSuffix);

    ENS_String<20> emptythisbuffer("emptythisbuffer");
    ENS_String<20> inputport("inputport");
    ENS_String<20> pcmsettings("pcmsettings");

    //coverity[forward_null]
    emptythisbuffer.concat(inSuffix);
    inputport.concat(inSuffix);
    pcmsettings.concat(inSuffix);

    //coverity[forward_null]
    nmf_error = hNmfOut->bindComponent((const char*)outputport, hNmfIn, (const char*)emptythisbuffer);
    if (nmf_error != NMF_OK) return OMX_ErrorUndefined;
    
    //coverity[forward_null]
    nmf_error = hNmfIn->bindComponent((const char*)inputport, hNmfOut, (const char*)fillthisbuffer);
    if (nmf_error != NMF_OK) return OMX_ErrorUndefined;

    if (pcm) {
      nmf_error = hNmfOut->bindAsynchronous((const char*)outputsettings, 1, hNmfIn, (const char*)pcmsettings);
        if (nmf_error != NMF_OK) return OMX_ErrorUndefined;
    }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFM::hostDisconnectPorts(
        NMF::Composite * hNmfOut, 
        const char * outSuffix,
        NMF::Composite * hNmfIn,
        const char * inSuffix,
        bool pcm) 
{

    t_nmf_error nmf_error;
    //coverity[forward_null]
    DBC_ASSERT(hNmfOut && hNmfIn);
    //coverity[forward_null]
    DBC_ASSERT(outSuffix && inSuffix);

    ENS_String<20> outputport("outputport");
    ENS_String<20> fillthisbuffer("fillthisbuffer");
    ENS_String<20> outputsettings("outputsettings");

    //coverity[forward_null]
    outputport.concat(outSuffix);
    fillthisbuffer.concat(outSuffix);
    outputsettings.concat(outSuffix);

    ENS_String<20> emptythisbuffer("emptythisbuffer");
    ENS_String<20> inputport("inputport");
    ENS_String<20> pcmsettings("pcmsettings");

    //coverity[forward_null]
    emptythisbuffer.concat(inSuffix);
    inputport.concat(inSuffix);
    pcmsettings.concat(inSuffix);

    //coverity[forward_null]
    nmf_error = hNmfOut->unbindComponent((const char*)outputport, hNmfIn, (const char*)emptythisbuffer);
    if (nmf_error != NMF_OK) return OMX_ErrorUndefined;
    
    //coverity[forward_null]
    nmf_error = hNmfIn->unbindComponent((const char*)inputport, hNmfOut, (const char*)fillthisbuffer);
    if (nmf_error != NMF_OK) return OMX_ErrorUndefined;
    
    if (pcm) {
      nmf_error = hNmfOut->unbindAsynchronous((const char*)outputsettings, hNmfIn, (const char*)pcmsettings);
        if (nmf_error != NMF_OK) return OMX_ErrorUndefined;
    }

    return OMX_ErrorNone;
}


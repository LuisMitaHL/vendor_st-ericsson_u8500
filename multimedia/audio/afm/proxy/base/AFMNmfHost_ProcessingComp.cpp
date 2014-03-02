/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFMNmfHost_ProcessingComp.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "share/inc/macros.h"
#include "AFMNmfHost_ProcessingComp.h"
#ifndef HOST_ONLY
#include "AFMNmfHostMpc_ProcessingComp.h"
#endif
#include "HostBindings.h"
#include "HostSHMin.h"
#include "HostSHMout.h"
#include "HostSHM_PcmIn.h"
#include "HostSHM_PcmOut.h"
#include "ENS_Wrapper_Services.h"
#include "HostPcmAdapter.h"
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_base_AFMNmfHost_ProcessingCompTraces.h"
#endif

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::construct(void)
{
    OMX_ERRORTYPE error;

#ifndef HOST_ONLY
    CM_REGISTER_SKELS(afm_cpp);
#endif //#ifndef HOST_ONLY

    mNmfMain = 0;

    mBindings =  new HostBindings_p[mENSComponent.getPortCount()];
    if(mBindings == 0){
        return OMX_ErrorInsufficientResources;
    }

    mNmfBufferSupplier = new OMX_BUFFERSUPPLIERTYPE[mENSComponent.getPortCount()];
    if(mNmfBufferSupplier == 0){
        return OMX_ErrorInsufficientResources;
    }

    mIsPortInstantiated = new bool[mENSComponent.getPortCount()];
    if(mIsPortInstantiated == 0){
        return OMX_ErrorInsufficientResources;
    }

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        mBindings[i] = 0;
        mIsPortInstantiated[i] = false;
        ENS_Port * port = mENSComponent.getPort(i);
        port->setMpc(false);
        if(port->getDirection() == OMX_DirInput) {
            mNmfBufferSupplier[i] = OMX_BufferSupplyOutput;
        } else {
            mNmfBufferSupplier[i] = OMX_BufferSupplyInput;
        }
    }

    error = NmfHost_ProcessingComponent::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::destroy(void)
{
    OMX_ERRORTYPE error;

    error = NmfHost_ProcessingComponent::destroy();
    if (error != OMX_ErrorNone) return error;

    delete [] mBindings;
    mBindings = 0;
    delete [] mNmfBufferSupplier;
    mNmfBufferSupplier = 0;
    delete [] mIsPortInstantiated;
    mIsPortInstantiated = 0;

#ifndef HOST_ONLY
    CM_UNREGISTER_SKELS(afm_cpp);
#endif //#ifndef HOST_ONLY

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
    OMX_ERRORTYPE error;

    if(eCmd == OMX_CommandPortEnable) {
        if(nData == OMX_ALL) {
            for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
                error = specificSendEnablePortCommand(i);
                if(error != OMX_ErrorNone) return error;
            }
        } else {
            error = specificSendEnablePortCommand(nData);
            if(error != OMX_ErrorNone) return error;
        }
    }

    bDeferredCmd = OMX_FALSE;

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE AFMNmfHost_ProcessingComp::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferEventHandler)
{
    OMX_ERRORTYPE error;

    if(event == OMX_EventCmdComplete && nData1 == OMX_CommandPortDisable) {
        if(nData2 == OMX_ALL) {
            for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
                error = disablePortCommandComplete(i);
                if(error != OMX_ErrorNone) return error;
            }
        } else {
            error = disablePortCommandComplete(nData2);
            if(error != OMX_ErrorNone) return error;
        }
    }

    deferEventHandler = OMX_FALSE;

    return OMX_ErrorNone;
}

////////////////////////////////
// Bindings - Proprietary Comm
////////////////////////////////
AFM_API_EXPORT NMF::Composite * AFMNmfHost_ProcessingComp::getNmfHandle(OMX_U32 portidx)const{
    return mNmfMain;
}

AFM_API_EXPORT const char * AFMNmfHost_ProcessingComp::getNmfSuffix(OMX_U32 portidx)const{
    return "";
}

NMF::Composite * AFMNmfHost_ProcessingComp::getNmfPortHandle(OMX_U32 portIdx) const {  
    return mBindings[portIdx] == 0 ? getNmfHandle(portIdx) : mBindings[portIdx]->getNmfHostHandle(); 
}

const char * AFMNmfHost_ProcessingComp::getNmfPortSuffix(OMX_U32 portIdx) const {
    return mBindings[portIdx] == 0 ? getNmfSuffix(portIdx) : (char*)""; 
}


OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::bindPortForProprietaryCom(ENS_Port * port,
        AFMNmfHost_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::hostConnectPorts(
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::hostConnectPorts(
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                isPcm);
    }
}


OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::unBindPortForProprietaryCom(ENS_Port * port,
        AFMNmfHost_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::hostDisconnectPorts(
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::hostDisconnectPorts(
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfPortHandle(port->getPortIndex()),
                getNmfPortSuffix(port->getPortIndex()),
                isPcm);
    }
}

#ifndef HOST_ONLY
AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::bindPortForProprietaryCom(ENS_Port * port,
        AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::hostConnectPorts(
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfHostPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::hostConnectPorts(
                pTunneledProcessingComp.getNmfHostPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                isPcm);
    }
}

OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::unBindPortForProprietaryCom(ENS_Port * port,
        AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::hostDisconnectPorts(
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfHostPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::hostDisconnectPorts(
                pTunneledProcessingComp.getNmfHostPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfPortHandle(port->getPortIndex()),
                getNmfPortSuffix(port->getPortIndex()),
                isPcm);
    }
}
#endif

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::instantiate(void) {
    OMX_ERRORTYPE error;

    setPriorityLevel((OMX_U32)getPortPriorityLevel(0));

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiate call mController.instantiate()");
    error = mController.instantiate(getPriorityLevel());
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiate call mController.instantiate() return error=0x%x",error);
        return error;
    }

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiate call instantiateMain()");
    error = instantiateMain();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiate call instantiateMain() return error=0x%x",error);
        return error;
    }

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiate call mController.bindMain()");
    error = mController.bindMain(mNmfMain);
    if (error != OMX_ErrorNone) 
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiate call mController.bindMain() return error=0x%x",error);
        return error;
    }

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        ENS_Port *ensport = static_cast<ENS_Port *>(mENSComponent.getPort(i));
        if ((ensport->getDomain() == OMX_PortDomainAudio) && (ensport->isEnabled())) {
            AFM_Port *port = static_cast<AFM_Port *>(ensport);
            OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiate call instantiateBindingComponent(port_idx=%d)",i);
            error = instantiateBindingComponent(i);
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiate call instantiateBindingComponent(port_idx=%d) return error=0x%x",i,error);
                return error;
            }

            mIsPortInstantiated[i] = true; //TODO : thread safety 

            if (port->useProprietaryCommunication() == OMX_TRUE) {
                // for proprietary communication, bind enabled ports to the tunneled port
                // if the tunneled component is already instantiated
                ENS_Component * pTunneledComp = (ENS_Component *)EnsWrapper_getEnsComponent(port->getTunneledComponent());
                if( port->getProprietaryCommInUse() == AFM_HOST_PROPRIETARY_COMMUNICATION)
                {
                    AFMNmfHost_ProcessingComp & pTunneledProcessingComp = 
                        static_cast<AFMNmfHost_ProcessingComp &>(pTunneledComp->getProcessingComponent());
                    if (pTunneledProcessingComp.isMainInstantiated() == true &&
                            pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                        error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                        if (error != OMX_ErrorNone) return error;
                    }
                }
#ifndef HOST_ONLY
                else {
                    AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp = 
                        static_cast<AFMNmfHostMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
                    if (pTunneledProcessingComp.isMainInstantiated() == true &&
                            pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                        error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                        if (error != OMX_ErrorNone) return error;
                    }
                }
#endif
            }
        } 
        else {
            if(ensport->isEnabled()) mIsPortInstantiated[i] = true;
        }//if (port->isEnabled()) 
    }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::deInstantiate(void) {
    OMX_ERRORTYPE error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        AFM_Port *port = static_cast<AFM_Port *>(mENSComponent.getPort(i));
        if (port->isEnabled()) {
            if (port->useProprietaryCommunication() == OMX_TRUE) {
                // for proprietary communication, unbind enabled ports to the tunneled port
                // if the tunneled component is not yet deInstantiated state
                ENS_Component * pTunneledComp = (ENS_Component *)
                    EnsWrapper_getEnsComponent(port->getTunneledComponent());
                if( port->getProprietaryCommInUse() == AFM_HOST_PROPRIETARY_COMMUNICATION)
                {
                    AFMNmfHost_ProcessingComp & pTunneledProcessingComp = 
                        static_cast<AFMNmfHost_ProcessingComp &>(pTunneledComp->getProcessingComponent());
                    if (pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                        error = unBindPortForProprietaryCom(port, pTunneledProcessingComp);
                        if (error != OMX_ErrorNone) return error;
                    }
                }
#ifndef HOST_ONLY
                else {
                    AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp = 
                        static_cast<AFMNmfHostMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
                    if (pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                        error = unBindPortForProprietaryCom(port, pTunneledProcessingComp);
                        if (error != OMX_ErrorNone) return error;
                    }
                } 
#endif
            }
            OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiate call deInstantiateBindingComponent(port_idx=%d)",i);
            error = deInstantiateBindingComponent(i);
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiate call deInstantiateBindingComponent(port_idx=%d) return error=0x%x",i,error);
                return error;
            }

            mIsPortInstantiated[i] = false;
        }
    }

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiate call mController.unbindMain()");
    error = mController.unbindMain(mNmfMain);
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiate call mController.unbindMain() return error=0x%x",error);
        return error;
    }

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiate call deInstantiateMain()");
    error = deInstantiateMain();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiate call deInstantiateMain() return error=0x%x",error);
        return error;
    }
    mNmfMain = 0;

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiate call mController.deInstantiate()");
    error = mController.deInstantiate();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiate call mController.deInstantiate() return error=0x%x",error);
        return error;
    }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::configure(void) {
    OMX_ERRORTYPE error;

    error = mController.configure();
    if (error != OMX_ErrorNone) return error;

    error = configureMain();
    if (error != OMX_ErrorNone) return error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->isEnabled() && mBindings[i] != 0) {
            error = mBindings[i]->configure();
            if (error != OMX_ErrorNone) return error; 
        }
    }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::start(void) {
    OMX_ERRORTYPE error;

    error = mController.start();
    if (error != OMX_ErrorNone) return error;

    error = startMain();
    if (error != OMX_ErrorNone) return error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->isEnabled() && mBindings[i] != 0) {
            error = mBindings[i]->start();
            if (error != OMX_ErrorNone) return error; 
        }
    }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::stop(void) {
    OMX_ERRORTYPE error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->isEnabled() && mBindings[i] != 0) {
            error = mBindings[i]->stop();
            if (error != OMX_ErrorNone) return error; 
        }
    }

    error = stopMain();
    if (error != OMX_ErrorNone) return error;

    error = mController.stop();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_ProcessingComp::doBufferAllocation(OMX_U32 nBufferIndex,
        OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo) {

    // TODO: maybe register this buffer allocation to avoid memory leak
    *ppData = (OMX_U8*) new char[nSizeBytes];

    if (*ppData == NULL) {
        return OMX_ErrorInsufficientResources;
    }

    *(OMX_U32*)bufferAllocInfo = (OMX_U32)*ppData;

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_ProcessingComp::doBufferDeAllocation(
        OMX_U32 nBufferIndex, void *bufferAllocInfo) {

    delete [] (OMX_U8*) bufferAllocInfo;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::instantiateBindingComponent(OMX_U32 portIdx) {
    OMX_ERRORTYPE error;
    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(portIdx));
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiateBindingComponent(portIdx=%d)",portIdx);

    portdef.nSize           = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);

    port->getParameter(OMX_IndexParamPortDefinition, &portdef);

    if (port->getTunneledComponent() == 0 || port->useStandardTunneling()) {
        if (portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM) {
            error = createNmfShmPcm(port);
            if (error != OMX_ErrorNone) return error;
        } else {
            error = createNmfShm(port);
            if (error != OMX_ErrorNone) return error;
        }
    } else if (port->useProprietaryCommunication() && portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM) {
        if (port->getDirection() == OMX_DirOutput) {
            error = addBindingComponentOutputPortProprietaryCom(static_cast<AFM_PcmPort *>(port));
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiateBindingComponent call addBindingComponentOutputPortProprietaryCom(portIdx=%d) return error=0x%x",portIdx,error);    
                return error;
            }

        } else {
            error = addBindingComponentInputPortProprietaryCom(static_cast<AFM_PcmPort *>(port));
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiateBindingComponent call addBindingComponentInputPortProprietaryCom(portIdx=%d) return error=0x%x",portIdx,error); 
                return error;
            }

        }
    }

    if (mBindings[portIdx] == 0) {
        return OMX_ErrorNone;
    }

    error = mBindings[portIdx]->instantiate();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiateBindingComponent call mBindings[portIdx=%d]->instantiate() return error=0x%x",portIdx,error); 
        return error;
    }

    if (port->getDirection() == OMX_DirInput) {
        error = mBindings[portIdx]->connectToOutput(getNmfHandle(portIdx), getNmfSuffix(portIdx));
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiateBindingComponent call mBindings[portIdx=%d]->connectToOutput() return error=0x%x",portIdx,error); 
            return error;
        }
    }
    else {
        error = mBindings[portIdx]->connectToInput(getNmfHandle(portIdx), getNmfSuffix(portIdx));
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::instantiateBindingComponent call mBindings[portIdx=%d]->connectToInput() return error=0x%x",portIdx,error); 
            return error;
        }
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::deInstantiateBindingComponent(OMX_U32 portIdx) {
    OMX_ERRORTYPE error;
    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(portIdx));

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiateBindingComponent(portIdx=%d)",portIdx); 

    if(mBindings[portIdx] == 0) return OMX_ErrorNone;

    if (port->getDirection() == OMX_DirInput) {
        error = mBindings[portIdx]->disconnectFromOutput(getNmfHandle(portIdx), getNmfSuffix(portIdx));
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiateBindingComponent call mBindings[portIdx=%d]->disconnectFromOutput() return error=0x%x",portIdx,error); 
            return error;
        }
    }
    else {
        error = mBindings[portIdx]->disconnectFromInput(getNmfHandle(portIdx), getNmfSuffix(portIdx));
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::deInstantiateBindingComponent call mBindings[portIdx=%d]->disconnectFromInput() return error=0x%x",portIdx,error); 
            return error;
        }
    }

    error = mBindings[portIdx]->deInstantiate();
    if (error != OMX_ErrorNone) return error; 

    delete mBindings[portIdx];
    mBindings[portIdx] = 0;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
AFMNmfHost_ProcessingComp::createNmfShmPcm(AFM_Port *port)
{
    if(port->getDirection() == OMX_DirInput) {
        mBindings[port->getPortIndex()] = new HostSHM_PcmIn(*port, mController, getENSComponent().getOMXHandle(),
                getEmptyThisBufferInterface(port->getPortIndex()),
                getEmptyBufferDoneCB());
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHost_ProcessingComp::createNmfShmPcm(portIdx=%d) binding=HostSHM_PcmIn",port->getPortIndex());
    } else {
        mBindings[port->getPortIndex()] = new HostSHM_PcmOut(*port, mController, getENSComponent().getOMXHandle(),
                getFillThisBufferInterface(port->getPortIndex()),
                getFillBufferDoneCB());
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHost_ProcessingComp::createNmfShmPcm(portIdx=%d) binding=HostSHM_PcmOut",port->getPortIndex());
    }

    if (mBindings[port->getPortIndex()] == 0) {
        return OMX_ErrorInsufficientResources;
    }
    else
    {
        mBindings[port->getPortIndex()]->setTraceInfo(mENSComponent.getSharedTraceInfoPtr(),port->getPortIndex());
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
AFMNmfHost_ProcessingComp::createNmfShm(AFM_Port *port)
{
    if(port->getDirection() == OMX_DirInput) {
        mBindings[port->getPortIndex()] = new HostSHMin(*port, mController, getENSComponent().getOMXHandle(),
                getEmptyThisBufferInterface(port->getPortIndex()),
                getEmptyBufferDoneCB());
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHost_ProcessingComp::createNmfShm(portIdx=%d) binding=HostSHMin",port->getPortIndex()); 

    } else {
        mBindings[port->getPortIndex()] = new HostSHMout(*port, mController, getENSComponent().getOMXHandle(),
                getFillThisBufferInterface(port->getPortIndex()),
                getFillBufferDoneCB());
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHost_ProcessingComp::createNmfShm(portIdx=%d) binding=HostSHMout",port->getPortIndex());
    }

    if (mBindings[port->getPortIndex()] == 0) {
        return OMX_ErrorInsufficientResources;
    }
    else
    {
        mBindings[port->getPortIndex()]->setTraceInfo(mENSComponent.getSharedTraceInfoPtr(),port->getPortIndex());
    }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_ProcessingComp::addBindingComponentInputPortProprietaryCom(AFM_PcmPort *port)
{
    DBC_ASSERT(mNmfBufferSupplier[port->getPortIndex()] == OMX_BufferSupplyOutput);

    if (port->getBufferSupplier() == OMX_BufferSupplyInput) {
        mBindings[port->getPortIndex()] = new HostPcmAdapter(*port, mController, getENSComponent().getOMXHandle(),OMX_FALSE);
        if (mBindings[port->getPortIndex()] == 0) {
            return OMX_ErrorInsufficientResources;
        }
        else
        {
            mBindings[port->getPortIndex()]->setTraceInfo(mENSComponent.getSharedTraceInfoPtr(),port->getPortIndex());
        }

        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHost_ProcessingComp::addBindingComponentInputPortProprietaryCom(portIdx=%d) binding=HostPcmAdapter",port->getPortIndex());
    }
    else { DBC_ASSERT(port->getBufferSupplier() == OMX_BufferSupplyOutput); }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
AFMNmfHost_ProcessingComp::addBindingComponentOutputPortProprietaryCom(AFM_PcmPort *port)
{
    if (port->getBufferSupplier() == OMX_BufferSupplyOutput) {
        if (mNmfBufferSupplier[port->getPortIndex()] ==  OMX_BufferSupplyInput) {
            mBindings[port->getPortIndex()] = new HostPcmAdapter(*port, mController, getENSComponent().getOMXHandle(),OMX_TRUE);
            if (mBindings[port->getPortIndex()] == 0) {
                return OMX_ErrorInsufficientResources;
            }
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHost_ProcessingComp::addBindingComponentOutputPortProprietaryCom(portIdx=%d) binding=HostPcmAdapter",port->getPortIndex());
        }
        else if (mNmfBufferSupplier[port->getPortIndex()] == OMX_BufferSupplyUnspecified) {
            AFM_PcmPort *tunneledPort = static_cast<AFM_PcmPort *>(port->getENSTunneledPort());

            if (port->getPcmLayout() != tunneledPort->getPcmLayout()) {
                mNmfBufferSupplier[port->getPortIndex()] = OMX_BufferSupplyInput;

                mBindings[port->getPortIndex()] = new HostPcmAdapter(*port, mController, getENSComponent().getOMXHandle(),OMX_TRUE);
                if (mBindings[port->getPortIndex()] == 0) {
                    return OMX_ErrorInsufficientResources;
                }
                OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHost_ProcessingComp::addBindingComponentOutputPortProprietaryCom(portIdx=%d) binding=HostPcmAdapter",port->getPortIndex());
            }
            else {
                mNmfBufferSupplier[port->getPortIndex()] = OMX_BufferSupplyOutput;
            }
        }
        else { DBC_ASSERT(0); }

        if (mBindings[port->getPortIndex()] != 0) 
        {
            mBindings[port->getPortIndex()]->setTraceInfo(mENSComponent.getSharedTraceInfoPtr(),port->getPortIndex());
        }
    }
    else if (port->getBufferSupplier() == OMX_BufferSupplyInput) {
        if (mNmfBufferSupplier[port->getPortIndex()] == OMX_BufferSupplyUnspecified) {
            mNmfBufferSupplier[port->getPortIndex()] = OMX_BufferSupplyInput;
        }
        else {
            DBC_ASSERT(mNmfBufferSupplier[port->getPortIndex()] ==  OMX_BufferSupplyInput); 
        }
    }
    else { DBC_ASSERT(0); }

    return OMX_ErrorNone;
}



AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_ProcessingComp::allocateBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex, 
        OMX_U32 nSizeBytes, 
        OMX_U8 **ppData, 
        void **bufferAllocInfo, 
        void **portPrivateInfo)
{
    return allocateBufferHeap(
            nPortIndex,
            nBufferIndex, 
            nSizeBytes, 
            ppData, 
            bufferAllocInfo, 
            portPrivateInfo);
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_ProcessingComp::useBufferHeader(
        OMX_DIRTYPE dir,
        OMX_BUFFERHEADERTYPE* pBuffer)
{
    return useBufferHeaderHeap(
            dir,
            pBuffer);

}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_ProcessingComp::useBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex, 
        OMX_BUFFERHEADERTYPE* pBufferHdr,
        void **portPrivateInfo)
{
    return useBufferHeap(
            nPortIndex,
            nBufferIndex, 
            pBufferHdr,
            portPrivateInfo);
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_ProcessingComp::freeBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex, 
        OMX_BOOL bBufferAllocated, 
        void *bufferAllocInfo,
        void *portPrivateInfo)
{
    return freeBufferHeap(
            nPortIndex,
            nBufferIndex, 
            bBufferAllocated, 
            bufferAllocInfo,
            portPrivateInfo);

}



OMX_ERRORTYPE AFMNmfHost_ProcessingComp::specificSendEnablePortCommand(OMX_U32 portIndex)
{
    OMX_ERRORTYPE error;

    ENS_Port *ensPort = static_cast<ENS_Port *>(mENSComponent.getPort(portIndex));
    if (ensPort->getDomain() != OMX_PortDomainAudio) {
        mIsPortInstantiated[portIndex] = true;
        return OMX_ErrorNone;
    }

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::specificSendEnablePortCommand call instantiateBindingComponent(portIdx=%d)",portIndex);
    error = instantiateBindingComponent(portIndex);
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::specificSendEnablePortCommand call instantiateBindingComponent(port_idx=%d) return error=0x%x",portIndex,error);
        return error;
    }

    mIsPortInstantiated[portIndex] = true; //TODO: thread safety

    if(mBindings[portIndex] != 0) {
        error = mBindings[portIndex]->start();
        if (error != OMX_ErrorNone) return error;
        error = mBindings[portIndex]->configure();
        if (error != OMX_ErrorNone) return error;
    }

    AFM_Port *port = static_cast<AFM_Port *>(mENSComponent.getPort(portIndex));
    if(port->useProprietaryCommunication() == OMX_TRUE) {
        // for proprietary communication, bind to the tunneled port
        ENS_Component * pTunneledComp = (ENS_Component *)EnsWrapper_getEnsComponent(port->getTunneledComponent());
        if( port->getProprietaryCommInUse() == AFM_HOST_PROPRIETARY_COMMUNICATION)
        {
            AFMNmfHost_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfHost_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if (pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
#ifndef HOST_ONLY
        else
        {
            AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfHostMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if (pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
#endif
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AFMNmfHost_ProcessingComp::disablePortCommandComplete(OMX_U32 portIndex)
{
    OMX_ERRORTYPE error;
    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::disablePortCommandComplete(portIdx=%d)",portIndex);

    AFM_Port *port = static_cast<AFM_Port *>(mENSComponent.getPort(portIndex));
    if (port->useProprietaryCommunication() == OMX_TRUE) {
        // for proprietary communication, unbind to the tunneled port
        ENS_Component * pTunneledComp = (ENS_Component *)
            EnsWrapper_getEnsComponent(port->getTunneledComponent());
        if( port->getProprietaryCommInUse() == AFM_HOST_PROPRIETARY_COMMUNICATION)
        {
            AFMNmfHost_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfHost_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if(pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = unBindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
#ifndef HOST_ONLY
        else
        {
            AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfHostMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if(pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = unBindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
#endif
    }

    if(mBindings[portIndex] != 0) {
        error = mBindings[portIndex]->stop();
        if (error != OMX_ErrorNone) return error;
    }

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHost_ProcessingComp::disablePortCommandComplete call deInstantiateBindingComponent(portIdx=%d)",portIndex);
    error = deInstantiateBindingComponent(portIndex);
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHost_ProcessingComp::disablePortCommandComplete call deInstantiateBindingComponent(port_idx=%d) return error=0x%x",portIndex,error);
        return error;
    }

    mIsPortInstantiated[portIndex] = false;

    error = disablePortNotification(portIndex);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


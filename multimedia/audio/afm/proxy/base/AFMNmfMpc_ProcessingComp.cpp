/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFMNmfMpc_ProcessingComp.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "AFMNmfMpc_ProcessingComp.h"
#include "AFMNmfHostMpc_ProcessingComp.h"
#include "ENS_Nmf.h"
#include "AFM_nmf.h"
#include "MpcBindings.h"
#include "PcmAdapter.h"
#include "SHMin.h"
#include "SHMout.h"
#include "SHM_PcmIn.h"
#include "SHM_PcmOut.h"
#include "AFM_Index.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_base_AFMNmfMpc_ProcessingCompTraces.h"
#endif

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::construct(void)
{
    OMX_ERRORTYPE error;

    registerStubsAndSkels();

    mBindings =  new MpcBindings_p[mENSComponent.getPortCount()];
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
        port->setMpc(true);
        if(port->getDirection() == OMX_DirInput) {
            mNmfBufferSupplier[i] = OMX_BufferSupplyOutput;
        } else {
            mNmfBufferSupplier[i] = OMX_BufferSupplyInput;
        }
    }

    error = NmfMpc_ProcessingComponent::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::destroy(void)
{
    OMX_ERRORTYPE error;

    error = NmfMpc_ProcessingComponent::destroy();
    if (error != OMX_ErrorNone) return error;

    delete [] mBindings;
    mBindings = 0;
    delete [] mNmfBufferSupplier;
    mNmfBufferSupplier = 0;
    delete [] mIsPortInstantiated;
    mIsPortInstantiated = 0;

    unregisterStubsAndSkels();

    return OMX_ErrorNone;
}

AFM_API_EXPORT AFMNmfMpc_ProcessingComp::~AFMNmfMpc_ProcessingComp(void)
{
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
    OMX_ERRORTYPE error;

    if(eCmd == OMX_CommandPortEnable) {
        if(nData == OMX_ALL) {
            for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
                error = sendEnablePortCommand(i);
                if(error != OMX_ErrorNone) return error;
            }
        } else {
            error = sendEnablePortCommand(nData);
            if(error != OMX_ErrorNone) return error;
        }
    }

    bDeferredCmd = OMX_FALSE;

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE AFMNmfMpc_ProcessingComp::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler)
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
    // OMX_FALSE is setted by default
    //deferEventHandler = OMX_FALSE;

    return OMX_ErrorNone;
}

void AFMNmfMpc_ProcessingComp::registerStubsAndSkels(void)
{
    CM_REGISTER_STUBS_SKELS(afm_cpp);
}

void AFMNmfMpc_ProcessingComp::unregisterStubsAndSkels(void)
{
    CM_UNREGISTER_STUBS_SKELS(afm_cpp);
}

AFM_API_EXPORT t_cm_instance_handle 
AFMNmfMpc_ProcessingComp::getNmfHandle(OMX_U32 portIdx) const {  
    return mNmfMain; 
}

AFM_API_EXPORT const char * 
AFMNmfMpc_ProcessingComp::getNmfSuffix(OMX_U32 portIdx) const {
    return "";
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::instantiate(void) {
    OMX_ERRORTYPE error;

    MEMORY_TRACE_ENTER2("AFMNmfMpc_ProcessingComp::instantiate (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiate call mController.instantiate()");

    {
        //FIXME NMF Mpc only support 3 priorities level
        OMX_U32 priority = getPortPriorityLevel(0);
        if (priority > NMF_SCHED_URGENT) {
            priority = NMF_SCHED_URGENT;
        }
        setPriorityLevel(priority);
    }

    error = mController.instantiate(getPriorityLevel());
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiate call mController.instantiate() return error=0x%x",error);
        return error;
    }

    MEMORY_TRACE_ENTER2("AFMNmfMpc_ProcessingComp::instantiateMain (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    error = instantiateMain();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiate call instantiateMain() return error=0x%x",error);
        return error;
    }
    MEMORY_TRACE_LEAVE2("AFMNmfMpc_ProcessingComp::instantiateMain (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);
    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiate call mController.bindMain()");
    error = mController.bindMain(mNmfMain);
    if (error != OMX_ErrorNone) 
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiate call mController.bindMain() return error=0x%x",error);
        return error;
    }

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        ENS_Port *ensport = static_cast<ENS_Port *>(mENSComponent.getPort(i));
        if ((ensport->getDomain() == OMX_PortDomainAudio) && (ensport->isEnabled())) {
            AFM_Port *port = static_cast<AFM_Port *>(ensport);
            OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiate call instantiateBindingComponent(port_idx=%d)",i);
            error = instantiateBindingComponent(i);
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiate call instantiateBindingComponent(port_idx=%d) return error=0x%x",i,error);
                return error;
            }
            mIsPortInstantiated[i] = true; //TODO : thread safety 

            if (port->useProprietaryCommunication() == OMX_TRUE) {
                // for proprietary communication, bind enabled ports to the tunneled port
                // if the tunneled component is already instantiated
                ENS_Component * pTunneledComp = (ENS_Component *)
                    EnsWrapper_getEnsComponent(port->getTunneledComponent());
                if( port->getProprietaryCommInUse() == AFM_MPC_PROPRIETARY_COMMUNICATION)
                {
                    AFMNmfMpc_ProcessingComp & pTunneledProcessingComp = 
                        static_cast<AFMNmfMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
                    if (pTunneledProcessingComp.isMainInstantiated() == true &&
                            pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                        error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                        if (error != OMX_ErrorNone) return error;
                    }
                } else {
                    AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp = 
                        static_cast<AFMNmfHostMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
                    if (pTunneledProcessingComp.isMainInstantiated() == true &&
                            pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                        error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                        if (error != OMX_ErrorNone) return error;
                    }
                }
            }
        } 
        else {
            if(ensport->isEnabled()) mIsPortInstantiated[i] = true;
        }
    }

    MEMORY_TRACE_LEAVE2("AFMNmfMpc_ProcessingComp::instantiate (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    if (supportsPcmProbe())
    {
        OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiate call mPcmProbe.instantiate()");

        error = mPcmProbe.instantiate(getPriorityLevel());
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiate call mPcmProbe.instantiate() return error=0x%x",error);
            return error;
        }
    }
    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::deInstantiate(void) {
    OMX_ERRORTYPE error;

    if (supportsPcmProbe())
    {
        OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deinstantiate call mPcmProbe.deinstantiate()");

        error = mPcmProbe.deInstantiate();
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deinstantiate call mPcmProbe.deinstantiate() return error=0x%x",error);
            return error;
        }
    }

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        AFM_Port *port = static_cast<AFM_Port *>(mENSComponent.getPort(i));
        if (port->isEnabled()) {
            if (port->useProprietaryCommunication() == OMX_TRUE) {
                // for proprietary communication, unbind enabled ports to the tunneled port
                // if the tunneled component is not yet deInstantiated state
                ENS_Component * pTunneledComp = (ENS_Component *)
                    EnsWrapper_getEnsComponent(port->getTunneledComponent());
                if( port->getProprietaryCommInUse() == AFM_MPC_PROPRIETARY_COMMUNICATION)
                {
                    AFMNmfMpc_ProcessingComp & pTunneledProcessingComp = 
                        static_cast<AFMNmfMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
                    if (pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                        error = unBindPortForProprietaryCom(port, pTunneledProcessingComp);
                        if (error != OMX_ErrorNone) return error;
                    }
                } else {
                    AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp = 
                        static_cast<AFMNmfHostMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
                    if (pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                        error = unBindPortForProprietaryCom(port, pTunneledProcessingComp);
                        if (error != OMX_ErrorNone) return error;
                    }
                }
            }
            OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiate call deInstantiateBindingComponent(port_idx=%d)",i);
            error = deInstantiateBindingComponent(i);
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiate call deInstantiateBindingComponent(port_idx=%d) return error=0x%x",i,error);
                return error;
            }

            mIsPortInstantiated[i] = false;
        }
    }

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiate call mController.unbindMain()");
    error = mController.unbindMain(mNmfMain);
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiate call mController.unbindMain() return error=0x%x",error);
        return error;
    }

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiate call deInstantiateMain()");
    error = deInstantiateMain();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiate call deInstantiateMain() return error=0x%x",error);
        return error;
    }
    mNmfMain = 0;

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiate call mController.deInstantiate()");
    error = mController.deInstantiate();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiate call mController.deInstantiate() return error=0x%x",error);
        return error;
    }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::configure(void) {
    OMX_ERRORTYPE error;

    error = mController.configure();
    if (error != OMX_ErrorNone) return error;

    error = configureMain();
    if (error != OMX_ErrorNone) return error;

    if (supportsPcmProbe()) {
        error = mPcmProbe.configure();
        if (error != OMX_ErrorNone) return error;
    }

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->isEnabled() && mBindings[i] != 0) {
            error = mBindings[i]->configure();
            if (error != OMX_ErrorNone) return error; 
        }
    }

    return OMX_ErrorNone;
}

t_cm_instance_handle 
AFMNmfMpc_ProcessingComp::getNmfPortHandle(OMX_U32 portIdx) const {  
    return mBindings[portIdx] == 0 ? 
        getNmfHandle(portIdx) : mBindings[portIdx]->getNmfMpcHandle(); 
}

const char * 
AFMNmfMpc_ProcessingComp::getNmfPortSuffix(OMX_U32 portIdx) const {
    return mBindings[portIdx] == 0 ? getNmfSuffix(portIdx) : (char*)""; 
}

OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::bindPortForProprietaryCom(ENS_Port * port,
        AFMNmfMpc_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::connectPorts(
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::connectPorts(
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                isPcm);
    }
}

OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::bindPortForProprietaryCom(ENS_Port * port,
        AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::connectPorts(
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfMpcPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::connectPorts(
                pTunneledProcessingComp.getNmfMpcPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                isPcm);
    }
}

OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::unBindPortForProprietaryCom(ENS_Port * port,
        AFMNmfMpc_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::disconnectPorts(
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::disconnectPorts(
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfPortHandle(port->getPortIndex()),
                getNmfPortSuffix(port->getPortIndex()),
                isPcm);
    }
}

OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::unBindPortForProprietaryCom(ENS_Port * port,
        AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::disconnectPorts(
                getNmfPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfMpcPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::disconnectPorts(
                pTunneledProcessingComp.getNmfMpcPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfPortHandle(port->getPortIndex()),
                getNmfPortSuffix(port->getPortIndex()),
                isPcm);
    }
}

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::start(void) {
    OMX_ERRORTYPE error;

    if (supportsPcmProbe())
    {
        error = mPcmProbe.start();
        if (error != OMX_ErrorNone) return error;
    }

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
AFMNmfMpc_ProcessingComp::stop(void) {
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

    if (supportsPcmProbe())
    {
        error = mPcmProbe.stop();
        if (error != OMX_ErrorNone) return error;
    }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_U32
AFMNmfMpc_ProcessingComp::getBufferPhysicalAddress(void * bufferAllocInfo, OMX_U8 *, OMX_U32) {
    t_cm_memory_handle  memHdl = (t_cm_memory_handle) bufferAllocInfo;
    t_cm_system_address sysAddr;

    CM_GetMpcMemorySystemAddress(memHdl, &sysAddr);

    return sysAddr.physical;
}

AFM_API_EXPORT OMX_U32
AFMNmfMpc_ProcessingComp::getBufferMpcAddress(void * bufferAllocInfo) {
    t_cm_memory_handle  memHdl = (t_cm_memory_handle) bufferAllocInfo;
    t_uint32            dspAddr;

    CM_GetMpcMemoryMpcAddress(memHdl, &dspAddr);

    return dspAddr;
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfMpc_ProcessingComp::doBufferAllocation(OMX_U32 nPortIndex, OMX_U32 nBufferIndex,
        OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo) {

    OMX_U8 *            pBuffer;
    t_cm_memory_handle  memHdl; 
    t_cm_system_address sysAddr;
    OMX_ERRORTYPE       error;
    OMX_U32             sizeWord = nSizeBytes / 2 + nSizeBytes % 2;

    error = ENS::allocMpcMemory(
            mENSComponent.getNMFDomainHandle(nPortIndex), 
            CM_MM_MPC_SDRAM16, sizeWord, CM_MM_ALIGN_4096BYTES, &memHdl);
    if (error != OMX_ErrorNone) return error;

    CM_GetMpcMemorySystemAddress(memHdl, &sysAddr);
    pBuffer = (OMX_U8 *)sysAddr.logical;
    DBC_ASSERT(pBuffer != 0);

    *bufferAllocInfo    = (void *) memHdl;
    *ppData             = pBuffer;

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfMpc_ProcessingComp::doBufferDeAllocation(OMX_U32 nPortIndex, 
        OMX_U32 nBufferIndex, void *bufferAllocInfo) {

    OMX_ERRORTYPE error;

    error = ENS::freeMpcMemory((t_cm_memory_handle) bufferAllocInfo);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::instantiateBindingComponent(OMX_U32 portIdx) {
    OMX_ERRORTYPE error;
    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(portIdx));
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    MEMORY_TRACE_ENTER3("AFMNmfMpc_ProcessingComp::instantiateBindingComponent[%u] (%s) @0x%08X", (unsigned int)portIdx, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiateBindingComponent(portIdx=%d)",portIdx);

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);

    if (port->getDomain() == OMX_PortDomainOther) {
        MEMORY_TRACE_LEAVE3("AFMNmfMpc_ProcessingComp::instantiateBindingComponent[%u] (%s) @0x%08X", (unsigned int)portIdx, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);
        return OMX_ErrorNone;		
    }

    if (port->getTunneledComponent() == 0 || port->useStandardTunneling()) {
        if(port->getDirection() == OMX_DirOutput
                && mNmfBufferSupplier[portIdx] == OMX_BufferSupplyUnspecified) {
            mNmfBufferSupplier[portIdx] = OMX_BufferSupplyInput;
        }
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
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiateBindingComponent call addBindingComponentOutputPortProprietaryCom(portIdx=%d) return error=0x%x",portIdx,error);    
                return error;
            }

        } else {
            error = addBindingComponentInputPortProprietaryCom(static_cast<AFM_PcmPort *>(port));
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiateBindingComponent call addBindingComponentInputPortProprietaryCom(portIdx=%d) return error=0x%x",portIdx,error); 
                return error;
            }

        }
    }

    if (mBindings[portIdx] == 0) {
        MEMORY_TRACE_LEAVE3("AFMNmfMpc_ProcessingComp::instantiateBindingComponent[%u] (%s) @0x%08X", (unsigned int)portIdx, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);
        return OMX_ErrorNone;
    }

    error = mBindings[portIdx]->instantiate();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiateBindingComponent call mBindings[portIdx=%d]->instantiate() return error=0x%x",portIdx,error); 
        return error;
    }

    if (port->getDirection() == OMX_DirInput) {
        error = mBindings[portIdx]->connectToOutput(getNmfHandle(portIdx), getNmfSuffix(portIdx));
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiateBindingComponent call mBindings[portIdx=%d]->connectToOutput() return error=0x%x",portIdx,error); 
            return error;
        }
    }
    else {
        error = mBindings[portIdx]->connectToInput(getNmfHandle(portIdx), getNmfSuffix(portIdx));
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::instantiateBindingComponent call mBindings[portIdx=%d]->connectToInput() return error=0x%x",portIdx,error); 
            return error;
        }
    }
    MEMORY_TRACE_LEAVE3("AFMNmfMpc_ProcessingComp::instantiateBindingComponent[%u] (%s) @0x%08X", (unsigned int)portIdx, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFMNmfMpc_ProcessingComp::deInstantiateBindingComponent(OMX_U32 portIdx) {
    OMX_ERRORTYPE error;
    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(portIdx));

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiateBindingComponent(portIdx=%d)",portIdx); 

    if(mBindings[portIdx] == 0) return OMX_ErrorNone;

    if (port->getDirection() == OMX_DirInput) {
        error = mBindings[portIdx]->disconnectFromOutput(getNmfHandle(portIdx), getNmfSuffix(portIdx));
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiateBindingComponent call mBindings[portIdx=%d]->disconnectFromOutput() return error=0x%x",portIdx,error); 
            return error;
        }
    }
    else {
        error = mBindings[portIdx]->disconnectFromInput(getNmfHandle(portIdx), getNmfSuffix(portIdx));
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::deInstantiateBindingComponent call mBindings[portIdx=%d]->disconnectFromInput() return error=0x%x",portIdx,error); 
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
AFMNmfMpc_ProcessingComp::createNmfShmPcm(AFM_Port *port)
{
    if(port->getDirection() == OMX_DirInput) {
        mBindings[port->getPortIndex()] = new SHM_PcmIn(*port, mController, getPriorityLevel(), getNmfSharedBuf(port->getPortIndex()));
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfMpc_ProcessingComp::createNmfShmPcm(portIdx=%d) binding=SHM_PcmIn",port->getPortIndex());
    } else {
        mBindings[port->getPortIndex()] = new SHM_PcmOut(*port, mController, getPriorityLevel(), getNmfSharedBuf(port->getPortIndex()));
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfMpc_ProcessingComp::createNmfShmPcm(portIdx=%d) binding=SHM_PcmOut",port->getPortIndex());
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
AFMNmfMpc_ProcessingComp::createNmfShm(AFM_Port *port)
{
    if(port->getDirection() == OMX_DirInput) {
        mBindings[port->getPortIndex()] = new SHMin(*port, mController, getPriorityLevel(), getNmfSharedBuf(port->getPortIndex()));
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfMpc_ProcessingComp::createNmfShm(portIdx=%d) binding=SHMin",port->getPortIndex()); 
    } else {
        mBindings[port->getPortIndex()] = new SHMout(*port, mController, getPriorityLevel(), getNmfSharedBuf(port->getPortIndex()));
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfMpc_ProcessingComp::createNmfShm(portIdx=%d) binding=SHMout",port->getPortIndex());
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
AFMNmfMpc_ProcessingComp::addBindingComponentInputPortProprietaryCom(AFM_PcmPort *port)
{
    DBC_ASSERT(mNmfBufferSupplier[port->getPortIndex()] == OMX_BufferSupplyOutput);

    if (port->getBufferSupplier() == OMX_BufferSupplyInput) {
        mBindings[port->getPortIndex()] = new PcmAdapter(*port, mController, getPriorityLevel());
        if (mBindings[port->getPortIndex()] == 0) {
            return OMX_ErrorInsufficientResources;
        }
        else
        {
            mBindings[port->getPortIndex()]->setTraceInfo(mENSComponent.getSharedTraceInfoPtr(),port->getPortIndex());
        }

        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfMpc_ProcessingComp::addBindingComponentInputPortProprietaryCom(portIdx=%d) binding=PcmAdapter",port->getPortIndex());
    }
    else { DBC_ASSERT(port->getBufferSupplier() == OMX_BufferSupplyOutput); }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
AFMNmfMpc_ProcessingComp::addBindingComponentOutputPortProprietaryCom(AFM_PcmPort *port)
{
    if (port->getBufferSupplier() == OMX_BufferSupplyOutput) {
        if (mNmfBufferSupplier[port->getPortIndex()] ==  OMX_BufferSupplyInput) {
            mBindings[port->getPortIndex()] = new PcmAdapter(*port, mController, getPriorityLevel());
            if (mBindings[port->getPortIndex()] == 0) {
                return OMX_ErrorInsufficientResources;
            }
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfMpc_ProcessingComp::addBindingComponentOutputPortProprietaryCom(portIdx=%d) binding=PcmAdapter",port->getPortIndex());
        }
        else if (mNmfBufferSupplier[port->getPortIndex()] == OMX_BufferSupplyUnspecified) {
            AFM_PcmPort *tunneledPort = static_cast<AFM_PcmPort *>(port->getENSTunneledPort());

            if (port->getPcmLayout() != tunneledPort->getPcmLayout()) {
                mNmfBufferSupplier[port->getPortIndex()] = OMX_BufferSupplyInput;
                mBindings[port->getPortIndex()] = new PcmAdapter(*port, mController, getPriorityLevel());
                if (mBindings[port->getPortIndex()] == 0) {
                    return OMX_ErrorInsufficientResources;
                }
                OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfMpc_ProcessingComp::addBindingComponentOutputPortProprietaryCom(portIdx=%d) binding=PcmAdapter",port->getPortIndex());
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

OMX_ERRORTYPE AFMNmfMpc_ProcessingComp::sendEnablePortCommand(OMX_U32 portIndex)
{
    OMX_ERRORTYPE error;

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::specificSendEnablePortCommand call instantiateBindingComponent(portIdx=%d)",portIndex);
    error = instantiateBindingComponent(portIndex);
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::specificSendEnablePortCommand call instantiateBindingComponent(port_idx=%d) return error=0x%x",portIndex,error);
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
        if( port->getProprietaryCommInUse() == AFM_MPC_PROPRIETARY_COMMUNICATION)
        {
            AFMNmfMpc_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if (pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
        else
        {
            AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfHostMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if (pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AFMNmfMpc_ProcessingComp::disablePortCommandComplete(OMX_U32 portIndex)
{
    OMX_ERRORTYPE error;
    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::disablePortCommandComplete(portIdx=%d)",portIndex);

    AFM_Port *port = static_cast<AFM_Port *>(mENSComponent.getPort(portIndex));
    if (port->useProprietaryCommunication() == OMX_TRUE) {
        // for proprietary communication, unbind to the tunneled port
        ENS_Component * pTunneledComp = (ENS_Component *)
            EnsWrapper_getEnsComponent(port->getTunneledComponent());
        if( port->getProprietaryCommInUse() == AFM_MPC_PROPRIETARY_COMMUNICATION)
        {
            AFMNmfMpc_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if(pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = unBindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
        else
        {
            AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfHostMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if(pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = unBindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
    }

    if(mBindings[portIndex] != 0) {
        error = mBindings[portIndex]->stop();
        if (error != OMX_ErrorNone) return error;
    }

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::disablePortCommandComplete call deInstantiateBindingComponent(portIdx=%d)",portIndex);
    error = deInstantiateBindingComponent(portIndex);
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfMpc_ProcessingComp::disablePortCommandComplete call deInstantiateBindingComponent(port_idx=%d) return error=0x%x",portIndex,error);
        return error;
    }

    mIsPortInstantiated[portIndex] = false;

    error = disablePortNotification(portIndex);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AFMNmfMpc_ProcessingComp::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE error;

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::applyConfig (nConfigIndex=%d)", nConfigIndex);

    if ((nConfigIndex == (OMX_INDEXTYPE) AFM_IndexConfigPcmProbe) &&
        supportsPcmProbe())
    {
        AFM_AUDIO_CONFIG_PCM_PROBE * pConfig = static_cast<AFM_AUDIO_CONFIG_PCM_PROBE *>(pComponentConfigStructure);

        error = mPcmProbe.applyConfig(pConfig);
    }
    else
    {
        error = NmfMpc_ProcessingComponent::applyConfig(nConfigIndex,
                                                        pComponentConfigStructure);
    }
    return error;
}

OMX_ERRORTYPE AFMNmfMpc_ProcessingComp::retrieveConfig(OMX_INDEXTYPE nConfigIndex,
                                                       OMX_PTR pComponentConfigStructure)
{
    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfMpc_ProcessingComp::retrieveConfig (nConfigIndex=%d)", nConfigIndex);

  if ((nConfigIndex == (OMX_INDEXTYPE)AFM_IndexConfigPcmProbe) &&
        supportsPcmProbe())
    {
        AFM_AUDIO_CONFIG_PCM_PROBE * pConfig = static_cast<AFM_AUDIO_CONFIG_PCM_PROBE *>(pComponentConfigStructure);

        return mPcmProbe.retrieveConfig(pConfig);
    }
    else
    {
        return NmfMpc_ProcessingComponent::retrieveConfig(nConfigIndex,
                                                          pComponentConfigStructure);
    }
}

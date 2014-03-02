/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFMNmfHostMpc_ProcessingComp.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "share/inc/macros.h"
#include "AFMNmfHostMpc_ProcessingComp.h"
#include "AFMNmfMpc_ProcessingComp.h"
#include "AFMNmfHost_ProcessingComp.h"
#include "AFM_nmf.h"
#include "HostBindings.h"
#include "HostSHMin.h"
#include "HostSHMout.h"
#include "HostSHM_PcmIn.h"
#include "HostSHM_PcmOut.h"
#include "MpcBindings.h"
#include "SHMin.h"
#include "SHMout.h"
#include "SHM_PcmIn.h"
#include "SHM_PcmOut.h"
#include "ENS_Wrapper_Services.h"
#include "PcmAdapter.h"
#include "HostPcmAdapter.h"
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_base_AFMNmfHostMpc_ProcessingCompTraces.h"
#endif

////////////////////////
// Component life cycle
////////////////////////
AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::construct(void)
{
    OMX_ERRORTYPE error;

    CM_REGISTER_STUBS_SKELS(afm_cpp);

    mNmfMain = 0;

    mBindings =  new Bindings_p[mENSComponent.getPortCount()];
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
        mBindings[i]  = 0;
        mIsPortInstantiated[i] = false;
        ENS_Port * port = mENSComponent.getPort(i);
        if(port->getDirection() == OMX_DirInput) {
            mNmfBufferSupplier[i] = OMX_BufferSupplyOutput;
        } else {
            mNmfBufferSupplier[i] = OMX_BufferSupplyInput;
        }
    }

    error = NmfHostMpc_ProcessingComponent::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::destroy(void)
{
    OMX_ERRORTYPE error;

    error = NmfHostMpc_ProcessingComponent::destroy();
    if (error != OMX_ErrorNone) return error;

    delete [] mBindings;
    mBindings = 0;
    delete [] mNmfBufferSupplier;
    mNmfBufferSupplier = 0;
    delete [] mIsPortInstantiated;
    mIsPortInstantiated = 0;

    CM_UNREGISTER_STUBS_SKELS(afm_cpp);

    return OMX_ErrorNone;
}


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::instantiate(void)
{  
    OMX_ERRORTYPE error;

    MEMORY_TRACE_ENTER2("AFMNmfHostMpc_ProcessingComp::instantiate (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiate call mController.instantiate()");

    setPriorityLevel(getPortPriorityLevel(0));
    
    error = mController.instantiate(getPriorityLevel());
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiate call mController.instantiate() return error=0x%x",error);
        return error;
    }

    MEMORY_TRACE_ENTER2("AFMNmfHostMpc_ProcessingComp::instantiateMain (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiate call instantiateMain()");

    error = instantiateMain();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiate call instantiateMain() return error=0x%x",error);
        return error;
    }

    MEMORY_TRACE_LEAVE2("AFMNmfHostMpc_ProcessingComp::instantiateMain (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiate call mController.bindMain()");

    error = mController.bindMain(mNmfMain);
    if (error != OMX_ErrorNone) 
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiate call mController.bindMain() return error=0x%x",error);
        return error;
    }

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
        if ((port->getDomain() == OMX_PortDomainAudio) && (port->isEnabled()))
        {
            OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiate call instantiateBindingComponent(port_idx=%d)",i);

            error = instantiateBindingComponent(i);
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiate call instantiateBindingComponent(port_idx=%d) return error=0x%x",i,error);
                return error;
            }

            OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiate call setupProprietaryCom((port_idx=%d)",i);

            error = setupProprietaryCom(static_cast<AFM_Port *>(port));
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiate call setupProprietaryCom((port_idx=%d) return error=0x%x",i,error);
                return error;
            }
        }
        else {
            if(port->isEnabled()) mIsPortInstantiated[i] = true;
        }//if (port->isEnabled()) 
    }

    MEMORY_TRACE_LEAVE2("AFMNmfHostMpc_ProcessingComp::instantiate (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    return OMX_ErrorNone;
}


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::deInstantiate(void)
{
    OMX_ERRORTYPE error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        AFM_Port *port = static_cast<AFM_Port *>(mENSComponent.getPort(i));
        if (port->isEnabled())
        {
            OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiate call cancelProprietaryCom(port_idx=%d)",i);
            error= cancelProprietaryCom(port);
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiate call cancelProprietaryCom(port_idx=%d) return error=0x%x",i,error);
                return error;
            }
            OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiate call deInstantiateBindingComponent(port_idx=%d)",i);
            error = deInstantiateBindingComponent(i);
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiate call deInstantiateBindingComponent(port_idx=%d) return error=0x%x",i,error);
                return error;
            }
        }
    }

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiate call mController.unbindMain()");
    error = mController.unbindMain(mNmfMain);
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiate call mController.unbindMain() return error=0x%x",error);
        return error;
    }

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiate call deInstantiateMain()");
    error = deInstantiateMain();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiate call deInstantiateMain() return error=0x%x",error);
        return error;
    }

    mNmfMain = 0;

    OstTraceFiltInst0(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiate call mController.deInstantiate()");
    error = mController.deInstantiate();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst1(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiate call mController.deInstantiate() return error=0x%x",error);
        return error;
    }

    return OMX_ErrorNone;
} 


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::start(void)
{
    OMX_ERRORTYPE error;

    error = mController.start();
    if (error != OMX_ErrorNone) return error;

    error = startMain();
    if (error != OMX_ErrorNone) return error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->isEnabled())
        {
            if( mBindings[i] != 0) {
                error = mBindings[i]->start();
                if (error != OMX_ErrorNone) return error; 
            }
        }
    }

    return OMX_ErrorNone;
}


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::stop(void)
{
    OMX_ERRORTYPE error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->isEnabled())
        {
            if(mBindings[i] != 0) {
                error = mBindings[i]->stop();
                if (error != OMX_ErrorNone) return error; 
            }
        }
    }

    error = stopMain();
    if (error != OMX_ErrorNone) return error;

    error = mController.stop();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::configure(void)
{
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



////////////////////////
// Command - event 
////////////////////////
AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferEventHandler)
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


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
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



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::specificSendEnablePortCommand(OMX_U32 portIndex)
{
    OMX_ERRORTYPE error;

    error = instantiateBindingComponent(portIndex);
    if (error != OMX_ErrorNone) return error;

    if(mBindings[portIndex] != 0) {
        error = mBindings[portIndex]->start();
        if (error != OMX_ErrorNone) return error;
        error = mBindings[portIndex]->configure();
        if (error != OMX_ErrorNone) return error;
    }

    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(portIndex));
    error = setupProprietaryCom(port);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::disablePortCommandComplete(OMX_U32 portIndex)
{
    OMX_ERRORTYPE error;

    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(portIndex));
    error= cancelProprietaryCom(port);
    if (error != OMX_ErrorNone) return error;

    if(mBindings[portIndex] != 0) {
        error = mBindings[portIndex]->stop();
        if (error != OMX_ErrorNone) return error;
    }

    error = deInstantiateBindingComponent(portIndex);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}




////////////////////////////////
// Buffer allocation - callback
////////////////////////////////
AFM_API_EXPORT
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::allocateBuffer(OMX_U32 nPortIndex,OMX_U32 nBufferIndex, 
        OMX_U32 nSizeBytes,OMX_U8 **ppData, 
        void **bufferAllocInfo,void **portPrivateInfo)
{
    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(nPortIndex));
    if(port->isMpc())
    {
        return NmfHostMpc_ProcessingComponent::allocateBuffer(nPortIndex,nBufferIndex,nSizeBytes, 
                ppData,bufferAllocInfo,portPrivateInfo);
    }
    else
    {
        return allocateBufferHeap(nPortIndex,nBufferIndex,nSizeBytes, 
                ppData,bufferAllocInfo,portPrivateInfo);
    }
}


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::freeBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex,
        OMX_BOOL bBufferAllocated, void *bufferAllocInfo,
        void *portPrivateInfo)
{
    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(nPortIndex));
    if(port->isMpc())
    {
        return NmfHostMpc_ProcessingComponent::freeBuffer(nPortIndex, nBufferIndex, bBufferAllocated,
                bufferAllocInfo, portPrivateInfo);
    }
    else
    {
        return freeBufferHeap(nPortIndex, nBufferIndex, bBufferAllocated,
                bufferAllocInfo, portPrivateInfo);
    }
}


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::useBufferHeader(OMX_DIRTYPE dir,OMX_BUFFERHEADERTYPE* pBufferHdr)
{
    if(((dir == OMX_DirInput) && (pBufferHdr->pInputPortPrivate)) ||
            ((dir == OMX_DirOutput) && (pBufferHdr->pOutputPortPrivate)))
    {
        return NmfHostMpc_ProcessingComponent::useBufferHeader(dir, pBufferHdr);
    }
    else
    {
        return useBufferHeaderHeap(dir, pBufferHdr);
    }
}


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, 
        OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo)
{
    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(nPortIndex));
    if(port->isMpc())
    {
        return NmfHostMpc_ProcessingComponent::useBuffer(nPortIndex,nBufferIndex, 
                pBufferHdr,portPrivateInfo);
    }
    else
    {
        return useBufferHeap(nPortIndex,nBufferIndex, 
                pBufferHdr,portPrivateInfo);
    }
}


AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::doBufferAllocation(OMX_U32 nPortIndex, OMX_U32 nBufferIndex,
        OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo)
{
    OMX_U8 *            pBuffer;
    t_cm_memory_handle  memHdl; 
    t_cm_system_address sysAddr;
    OMX_ERRORTYPE       error;
    OMX_U32             sizeWord = nSizeBytes / 2 + nSizeBytes % 2;

    error = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(nPortIndex), 
            CM_MM_MPC_SDRAM16, sizeWord, CM_MM_ALIGN_4096BYTES, &memHdl);
    if (error != OMX_ErrorNone) return error;

    CM_GetMpcMemorySystemAddress(memHdl, &sysAddr);
    pBuffer = (OMX_U8 *)sysAddr.logical;
    DBC_ASSERT(pBuffer != 0);

    *bufferAllocInfo    = (void *) memHdl;
    *ppData             = pBuffer;

    return OMX_ErrorNone;
}

AFM_API_EXPORT 
OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::doBufferDeAllocation(OMX_U32 nPortIndex, 
        OMX_U32 nBufferIndex, void *bufferAllocInfo)
{
    OMX_ERRORTYPE error;

    error = ENS::freeMpcMemory((t_cm_memory_handle) bufferAllocInfo);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT 
OMX_U32 AFMNmfHostMpc_ProcessingComp::getBufferPhysicalAddress(void * bufferAllocInfo, OMX_U8 *, OMX_U32)
{
    t_cm_memory_handle  memHdl = (t_cm_memory_handle) bufferAllocInfo;
    t_cm_system_address sysAddr;

    CM_GetMpcMemorySystemAddress(memHdl, &sysAddr);

    return sysAddr.physical;
}


AFM_API_EXPORT 
OMX_U32 AFMNmfHostMpc_ProcessingComp::getBufferMpcAddress(void * bufferAllocInfo)
{
    t_cm_memory_handle  memHdl = (t_cm_memory_handle) bufferAllocInfo;
    t_uint32            dspAddr;

    CM_GetMpcMemoryMpcAddress(memHdl, &dspAddr);

    return dspAddr;
}




////////////////////////////////
// Bindings - Proprietary Comm
////////////////////////////////
NMF::Composite * AFMNmfHostMpc_ProcessingComp::getNmfHostHandle(void)const{
    return mNmfMain;
}



const char * AFMNmfHostMpc_ProcessingComp::getNmfSuffix(OMX_U32 portidx)const{
    return "";
}



NMF::Composite * AFMNmfHostMpc_ProcessingComp::getNmfHostPortHandle(OMX_U32 portIdx) const {  
    return mBindings[portIdx] == 0 ? 
        getNmfHostHandle() : mBindings[portIdx]->getNmfHostHandle(); 
}



t_cm_instance_handle AFMNmfHostMpc_ProcessingComp::getNmfMpcPortHandle(OMX_U32 portIdx) const {  
    return mBindings[portIdx] == 0 ? 
        getNmfMpcHandle(portIdx) : mBindings[portIdx]->getNmfMpcHandle(); 
}



const char * AFMNmfHostMpc_ProcessingComp::getNmfPortSuffix(OMX_U32 portIdx) const {
    return mBindings[portIdx] == 0 ? getNmfSuffix(portIdx) : (char*)""; 
}



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::instantiateBindingComponent(OMX_U32 portIdx)
{
    OMX_ERRORTYPE error;
    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(portIdx));
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    MEMORY_TRACE_ENTER3("AFMNmfHostMpc_ProcessingComp::instantiateBindingComponent[%u] (%s) @0x%08X", (unsigned int)portIdx, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiateBindingComponent(portIdx=%d)",portIdx); 

    portdef.nSize           = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);

    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
  
    if (port->getDomain() == OMX_PortDomainOther) {
        return OMX_ErrorNone;		
    }
   
   if (!port->useProprietaryCommunication())
    {
        if(port->getDirection() == OMX_DirOutput &&
                mNmfBufferSupplier[portIdx] == OMX_BufferSupplyUnspecified) {
            mNmfBufferSupplier[portIdx] = OMX_BufferSupplyInput;
        }

        if (portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM) {
            error = createNmfShmPcm(port);
            if (error != OMX_ErrorNone) return error;
        } else {
            error = createNmfShm(port);
            if (error != OMX_ErrorNone) return error;
        }
    }
    else if(portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM)
    {
        if (port->getDirection() == OMX_DirOutput) {
            error = addBindingComponentOutputPortProprietaryCom(static_cast<AFM_PcmPort *>(port));
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiateBindingComponent call addBindingComponentOutputPortProprietaryCom(portIdx=%d) return error=0x%x",portIdx,error);    
                return error;
            }
        } else {
            error = addBindingComponentInputPortProprietaryCom(static_cast<AFM_PcmPort *>(port));  
            if (error != OMX_ErrorNone)
            {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiateBindingComponent call addBindingComponentInputPortProprietaryCom(portIdx=%d) return error=0x%x",portIdx,error); 
                return error;
            }
        }
    }

    if (mBindings[portIdx] == 0)
    {
        mIsPortInstantiated[portIdx] = true; //todo thread safety
        MEMORY_TRACE_LEAVE3("AFMNmfHostMpc_ProcessingComp::instantiateBindingComponent[%u] (%s) @0x%08X", (unsigned int)portIdx, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);
        return OMX_ErrorNone;
    }

    error = mBindings[portIdx]->instantiate();
    if (error != OMX_ErrorNone)
    {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiateBindingComponent call mBindings[portIdx=%d]->instantiate() return error=0x%x",portIdx,error); 
        return error;
    }

    if (port->getDirection() == OMX_DirInput) {
        if(port->isMpc())
            error = mBindings[portIdx]->connectToOutput(getNmfMpcHandle(portIdx), getNmfSuffix(portIdx));
        else
            error = mBindings[portIdx]->connectToOutput(getNmfHostHandle(), getNmfSuffix(portIdx));
        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiateBindingComponent call mBindings[portIdx=%d]->connectToOutput() return error=0x%x",portIdx,error); 
            return error;
        }
    }
    else {
        if(port->isMpc())
            error = mBindings[portIdx]->connectToInput(getNmfMpcHandle(portIdx), getNmfSuffix(portIdx));
        else
            error = mBindings[portIdx]->connectToInput(getNmfHostHandle(), getNmfSuffix(portIdx));

        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::instantiateBindingComponent call mBindings[portIdx=%d]->connectToInput() return error=0x%x",portIdx,error); 
            return error;
        }
    }

    mIsPortInstantiated[portIdx] = true; //todo thread safety

    MEMORY_TRACE_LEAVE3("AFMNmfHostMpc_ProcessingComp::instantiateBindingComponent[%u] (%s) @0x%08X", (unsigned int)portIdx, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    return OMX_ErrorNone;
}



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::deInstantiateBindingComponent(OMX_U32 portIdx)
{
    OMX_ERRORTYPE error;
    AFM_Port * port = static_cast<AFM_Port *>(mENSComponent.getPort(portIdx));

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiateBindingComponent(portIdx=%d)",portIdx); 

    if(mBindings[portIdx] == 0)
    {
        mIsPortInstantiated[portIdx] = false; //todo thread safety
        return OMX_ErrorNone;
    }

    if (port->getDirection() == OMX_DirInput) {
        if(port->isMpc())
            error = mBindings[portIdx]->disconnectFromOutput(getNmfMpcHandle(portIdx), getNmfSuffix(portIdx));
        else
            error = mBindings[portIdx]->disconnectFromOutput(getNmfHostHandle(), getNmfSuffix(portIdx));

        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiateBindingComponent call mBindings[portIdx=%d]->disconnectFromOutput() return error=0x%x",portIdx,error); 
            return error;
        }
    }
    else {
        if(port->isMpc())
            error = mBindings[portIdx]->disconnectFromInput(getNmfMpcHandle(portIdx), getNmfSuffix(portIdx));
        else
            error = mBindings[portIdx]->disconnectFromInput(getNmfHostHandle(), getNmfSuffix(portIdx));

        if (error != OMX_ErrorNone)
        {
            OstTraceFiltInst2(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::deInstantiateBindingComponent call mBindings[portIdx=%d]->disconnectFromInput() return error=0x%x",portIdx,error); 
            return error;
        }

    }

    error = mBindings[portIdx]->deInstantiate();
    if (error != OMX_ErrorNone) return error; 

    delete mBindings[portIdx];
    mBindings[portIdx] = 0;

    mIsPortInstantiated[portIdx] = false; //todo thread safety
    return OMX_ErrorNone;
}



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::createNmfShmPcm(AFM_Port *port)
{
    if(port->isMpc())
    {
        if(port->getDirection() == OMX_DirInput) {
            mBindings[port->getPortIndex()] = new SHM_PcmIn(*port, mController, getPriorityLevel(),
                    getNmfSharedBuf(port->getPortIndex()));
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::createNmfShmPcm(portIdx=%d) binding=SHM_PcmIn",port->getPortIndex());
        } else {
            mBindings[port->getPortIndex()] = new SHM_PcmOut(*port, mController, getPriorityLevel(), 
                    getNmfSharedBuf(port->getPortIndex()));
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::createNmfShmPcm(portIdx=%d) binding=SHM_PcmOut",port->getPortIndex());
        }
    }
    else
    {
        if(port->getDirection() == OMX_DirInput) {
            mBindings[port->getPortIndex()] = new HostSHM_PcmIn(*port, mController, getENSComponent().getOMXHandle(),
                    getEmptyThisBufferInterface(port->getPortIndex()),
                    getEmptyBufferDoneCB());
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::createNmfShmPcm(portIdx=%d) binding=HostSHM_PcmIn",port->getPortIndex());
        } else {
            mBindings[port->getPortIndex()] = new HostSHM_PcmOut(*port, mController, getENSComponent().getOMXHandle(),
                    getFillThisBufferInterface(port->getPortIndex()),
                    getFillBufferDoneCB());
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::createNmfShmPcm(portIdx=%d) binding=HostSHM_PcmOut",port->getPortIndex());
        }
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



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::createNmfShm(AFM_Port *port)
{
    if(port->isMpc())
    {
        if(port->getDirection() == OMX_DirInput) {
            mBindings[port->getPortIndex()] = new SHMin(*port, mController, getPriorityLevel(), 
                    getNmfSharedBuf(port->getPortIndex()));
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::createNmfShm(portIdx=%d) binding=SHMin",port->getPortIndex());
        } 
        else {
            mBindings[port->getPortIndex()] = new SHMout(*port, mController, getPriorityLevel(), 
                    getNmfSharedBuf(port->getPortIndex()));
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::createNmfShm(portIdx=%d) binding=SHMout",port->getPortIndex());
        }
    }
    else
    {
        if(port->getDirection() == OMX_DirInput) {
            mBindings[port->getPortIndex()] = new HostSHMin(*port, mController, getENSComponent().getOMXHandle(),
                    getEmptyThisBufferInterface(port->getPortIndex()),
                    getEmptyBufferDoneCB());
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::createNmfShm(portIdx=%d) binding=HostSHMin",port->getPortIndex());
        } 
        else {
            mBindings[port->getPortIndex()] = new HostSHMout(*port, mController, getENSComponent().getOMXHandle(),
                    getFillThisBufferInterface(port->getPortIndex()),
                    getFillBufferDoneCB());

            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::createNmfShm(portIdx=%d) binding=HostSHMin",port->getPortIndex());
        }
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



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::setupProprietaryCom(AFM_Port *port)
{
    OMX_ERRORTYPE error;

    if (port->useProprietaryCommunication() == OMX_TRUE) {
        // for proprietary communication, bind enabled ports to the tunneled port
        // if the tunneled component is already instantiated
        ENS_Component * pTunneledComp = (ENS_Component *)EnsWrapper_getEnsComponent(port->getTunneledComponent());
        if( port->getProprietaryCommInUse() == AFM_MPC_PROPRIETARY_COMMUNICATION)
        {
            AFMNmfMpc_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if (pTunneledProcessingComp.isMainInstantiated() == true &&
                    pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        } else if ((port->getProprietaryCommInUse() == AFM_HYBRIDMPC_PROPRIETARY_COMMUNICATION) ||
                (port->getProprietaryCommInUse() == AFM_HYBRIDHOST_PROPRIETARY_COMMUNICATION)) {
            AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfHostMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if (pTunneledProcessingComp.isMainInstantiated() == true &&
                    pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        } else if ( port->getProprietaryCommInUse() == AFM_HOST_PROPRIETARY_COMMUNICATION){
            AFMNmfHost_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfHost_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if (pTunneledProcessingComp.isMainInstantiated() == true &&
                    pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = bindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
        else
        {
            error = OMX_ErrorNotImplemented;
            OstTraceFiltInst3(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::setupProprietaryCom (portId=%d) (proprietaryCommIsUse=0x%x) return error=0x%x",port->getPortIndex(),port->getProprietaryCommInUse(),error);
            return error;
        }
    }

    return OMX_ErrorNone;
}



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::cancelProprietaryCom(AFM_Port *port)
{
    OMX_ERRORTYPE error;

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
        } else if ((port->getProprietaryCommInUse() == AFM_HYBRIDMPC_PROPRIETARY_COMMUNICATION) ||
                (port->getProprietaryCommInUse() == AFM_HYBRIDHOST_PROPRIETARY_COMMUNICATION)) {
            AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfHostMpc_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if (pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = unBindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
        else if ( port->getProprietaryCommInUse() == AFM_HOST_PROPRIETARY_COMMUNICATION){
            AFMNmfHost_ProcessingComp & pTunneledProcessingComp = 
                static_cast<AFMNmfHost_ProcessingComp &>(pTunneledComp->getProcessingComponent());
            if (pTunneledProcessingComp.isPortInstantiated(port->getTunneledPort()) == true) {
                error = unBindPortForProprietaryCom(port, pTunneledProcessingComp);
                if (error != OMX_ErrorNone) return error;
            }
        }
        else
        {
            error = OMX_ErrorNotImplemented;
            OstTraceFiltInst3(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::cancelProprietaryCom (portId=%d) (proprietaryCommIsUse=0x%x) return error=0x%x",port->getPortIndex(),port->getProprietaryCommInUse(),error);
            return error;
        }
    }

    return OMX_ErrorNone;
}



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::bindPortForProprietaryCom(ENS_Port * port,
        AFMNmfMpc_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::connectPorts(getNmfMpcPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::connectPorts(pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfMpcPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                isPcm);
    }
}



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::bindPortForProprietaryCom(ENS_Port * port,
        AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if(port->isMpc())
    {
        if (port->getDirection() == OMX_DirOutput) {
            return AFM::connectPorts(getNmfMpcPortHandle(port->getPortIndex()), 
                    getNmfPortSuffix(port->getPortIndex()), 
                    pTunneledProcessingComp.getNmfMpcPortHandle(port->getTunneledPort()), 
                    pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                    isPcm);
        } else {
            return AFM::connectPorts(pTunneledProcessingComp.getNmfMpcPortHandle(port->getTunneledPort()), 
                    pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                    getNmfMpcPortHandle(port->getPortIndex()), 
                    getNmfPortSuffix(port->getPortIndex()), 
                    isPcm);
        }
    }
    else
    {
        if (port->getDirection() == OMX_DirOutput) {
            return AFM::hostConnectPorts(getNmfHostPortHandle(port->getPortIndex()), 
                    getNmfPortSuffix(port->getPortIndex()), 
                    pTunneledProcessingComp.getNmfHostPortHandle(port->getTunneledPort()), 
                    pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                    isPcm);
        } else {
            return AFM::hostConnectPorts(pTunneledProcessingComp.getNmfHostPortHandle(port->getTunneledPort()), 
                    pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                    getNmfHostPortHandle(port->getPortIndex()), 
                    getNmfPortSuffix(port->getPortIndex()), 
                    isPcm);
        }
    }
}


OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::bindPortForProprietaryCom(ENS_Port * port,
        AFMNmfHost_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::hostConnectPorts(getNmfHostPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::hostConnectPorts(pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfHostPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                isPcm);
    }
}



OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::unBindPortForProprietaryCom(ENS_Port * port,
        AFMNmfMpc_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::disconnectPorts(getNmfMpcPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::disconnectPorts(pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfMpcPortHandle(port->getPortIndex()),
                getNmfPortSuffix(port->getPortIndex()),
                isPcm);
    }
}


OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::unBindPortForProprietaryCom(ENS_Port * port,
        AFMNmfHostMpc_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if(port->isMpc())
    {
        if (port->getDirection() == OMX_DirOutput) {
            return AFM::disconnectPorts(getNmfMpcPortHandle(port->getPortIndex()), 
                    getNmfPortSuffix(port->getPortIndex()), 
                    pTunneledProcessingComp.getNmfMpcPortHandle(port->getTunneledPort()), 
                    pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                    isPcm);
        } else {
            return AFM::disconnectPorts(pTunneledProcessingComp.getNmfMpcPortHandle(port->getTunneledPort()), 
                    pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                    getNmfMpcPortHandle(port->getPortIndex()),
                    getNmfPortSuffix(port->getPortIndex()),
                    isPcm);
        }
    }
    else
    {
        if (port->getDirection() == OMX_DirOutput) {
            return AFM::hostDisconnectPorts(getNmfHostPortHandle(port->getPortIndex()), 
                    getNmfPortSuffix(port->getPortIndex()), 
                    pTunneledProcessingComp.getNmfHostPortHandle(port->getTunneledPort()), 
                    pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                    isPcm);
        } else {
            return AFM::hostDisconnectPorts(pTunneledProcessingComp.getNmfHostPortHandle(port->getTunneledPort()), 
                    pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                    getNmfHostPortHandle(port->getPortIndex()),
                    getNmfPortSuffix(port->getPortIndex()),
                    isPcm);
        }
    }
}


OMX_ERRORTYPE AFMNmfHostMpc_ProcessingComp::unBindPortForProprietaryCom(ENS_Port * port,
        AFMNmfHost_ProcessingComp & pTunneledProcessingComp)
{
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);
    bool isPcm = portdef.format.audio.eEncoding == OMX_AUDIO_CodingPCM ? true : false;

    if (port->getDirection() == OMX_DirOutput) {
        return AFM::hostDisconnectPorts(getNmfHostPortHandle(port->getPortIndex()), 
                getNmfPortSuffix(port->getPortIndex()), 
                pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                isPcm);
    } else {
        return AFM::hostDisconnectPorts(pTunneledProcessingComp.getNmfPortHandle(port->getTunneledPort()), 
                pTunneledProcessingComp.getNmfPortSuffix(port->getTunneledPort()),
                getNmfHostPortHandle(port->getPortIndex()),
                getNmfPortSuffix(port->getPortIndex()),
                isPcm);
    }
}


OMX_ERRORTYPE  AFMNmfHostMpc_ProcessingComp::addBindingComponentInputPortProprietaryCom(AFM_PcmPort *port)
{
    DBC_ASSERT(mNmfBufferSupplier[port->getPortIndex()] == OMX_BufferSupplyOutput);
    OMX_ERRORTYPE error;

    if (port->getBufferSupplier() == OMX_BufferSupplyInput) {
        if(port->getProprietaryCommInUse() & AFM_MPC_PROPRIETARY_COMMUNICATION)
        {
            mBindings[port->getPortIndex()] = new PcmAdapter(*port, mController, getPriorityLevel());
            if (mBindings[port->getPortIndex()] == 0)
            {return OMX_ErrorInsufficientResources;
            }
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::addBindingComponentInputPortProprietaryCom(portIdx=%d) binding=PcmAdapter",port->getPortIndex());
        }
        else if(port->getProprietaryCommInUse() & AFM_HOST_PROPRIETARY_COMMUNICATION)
        {
            mBindings[port->getPortIndex()] = new HostPcmAdapter(*port, mController, getENSComponent().getOMXHandle(),OMX_FALSE);
            if (mBindings[port->getPortIndex()] == 0) {
                return OMX_ErrorInsufficientResources;
            }
            OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::addBindingComponentInputPortProprietaryCom(portIdx=%d) binding=HostPcmAdapter",port->getPortIndex());
        }
        else
        {
            error = OMX_ErrorNotImplemented;
            OstTraceFiltInst3(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::addBindingComponentInputPortProprietaryCom OMX_BufferSupplyInput (portId=%d) (proprietaryCommIsUse=0x%x) return error=0x%x",port->getPortIndex(),port->getProprietaryCommInUse(),error); 
            return error;
        }
        if (mBindings[port->getPortIndex()] != 0) 
        {
            mBindings[port->getPortIndex()]->setTraceInfo(mENSComponent.getSharedTraceInfoPtr(),port->getPortIndex());
        }
    }
    else { DBC_ASSERT(port->getBufferSupplier() == OMX_BufferSupplyOutput); }

    return OMX_ErrorNone;
}



OMX_ERRORTYPE  AFMNmfHostMpc_ProcessingComp::addBindingComponentOutputPortProprietaryCom(AFM_PcmPort *port)
{
    OMX_ERRORTYPE error;
    if (port->getBufferSupplier() == OMX_BufferSupplyOutput) {
        if (mNmfBufferSupplier[port->getPortIndex()] ==  OMX_BufferSupplyInput) {
            if(port->getProprietaryCommInUse() & AFM_MPC_PROPRIETARY_COMMUNICATION)
            {
                mBindings[port->getPortIndex()] = new PcmAdapter(*port, mController, getPriorityLevel());
                if (mBindings[port->getPortIndex()] == 0) return OMX_ErrorInsufficientResources;
                OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::addBindingComponentOutputPortProprietaryCom(portIdx=%d) binding=PcmAdapter",port->getPortIndex());

            }
            else if(port->getProprietaryCommInUse() & AFM_HOST_PROPRIETARY_COMMUNICATION)
            {
                mBindings[port->getPortIndex()] = new HostPcmAdapter(*port, mController, getENSComponent().getOMXHandle(),OMX_TRUE);
                if (mBindings[port->getPortIndex()] == 0) {
                    return OMX_ErrorInsufficientResources;
                }
                OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::addBindingComponentOutputPortProprietaryCom(portIdx=%d) binding=HostPcmAdapter",port->getPortIndex());

            }
            else
            {
                error = OMX_ErrorNotImplemented;
                OstTraceFiltInst3(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::addBindingComponentOutputPortProprietaryCom OMX_BufferSupplyOutput (portId=%d) (proprietaryCommIsUse=0x%x) return error=0x%x",port->getPortIndex(),port->getProprietaryCommInUse(),error); 

                return error;
            }
        }
        else if (mNmfBufferSupplier[port->getPortIndex()] == OMX_BufferSupplyUnspecified) {
            AFM_PcmPort *tunneledPort = static_cast<AFM_PcmPort *>(port->getENSTunneledPort());

            if (port->getPcmLayout() != tunneledPort->getPcmLayout()) {
                mNmfBufferSupplier[port->getPortIndex()] = OMX_BufferSupplyInput;
                if(port->getProprietaryCommInUse() & AFM_MPC_PROPRIETARY_COMMUNICATION)
                {
                    mBindings[port->getPortIndex()] = new PcmAdapter(*port, mController, getPriorityLevel());
                    if (mBindings[port->getPortIndex()] == 0) return OMX_ErrorInsufficientResources;
                    OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::addBindingComponentOutputPortProprietaryCom(portIdx=%d) binding=PcmAdapter",port->getPortIndex());

                }
                else if(port->getProprietaryCommInUse() & AFM_HOST_PROPRIETARY_COMMUNICATION)
                {
                    mBindings[port->getPortIndex()] = new HostPcmAdapter(*port, mController, getENSComponent().getOMXHandle(),OMX_TRUE);
                    if (mBindings[port->getPortIndex()] == 0) {
                        return OMX_ErrorInsufficientResources;
                    }
                    OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::addBindingComponentOutputPortProprietaryCom(portIdx=%d) binding=HostPcmAdapter",port->getPortIndex());

                }
                else
                {
                    error = OMX_ErrorNotImplemented;
                    OstTraceFiltInst3(TRACE_ERROR, "AFM_PROXY: AFMNmfHostMpc_ProcessingComp::addBindingComponentOutputPortProprietaryCom OMX_BufferSupplyUnspecified (portId=%d) (proprietaryCommIsUse=0x%x) return error=0x%x",port->getPortIndex(),port->getProprietaryCommInUse(),error); 
                    return error;
                }
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


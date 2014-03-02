/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpc_ProcessingComponent.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Port.h"
#include "NmfMpc_ProcessingComponent.h"
#include "SharedBuffer.h"
#include <cm/inc/cm_macros.h>
#include  "OMX_CoreExt.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ens_proxy_ENS_Component_NmfMpc_ProcessingComponentTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE
ENS_API_EXPORT void NmfMpc_ProcessingComponentCB::emptyThisBuffer(t_uint32 dspBufferHdrArmAddress) {

  mProcessingComp->fillBufferDone_cb(dspBufferHdrArmAddress);
}

ENS_API_EXPORT void NmfMpc_ProcessingComponentCB::fillThisBuffer(t_uint32 dspBufferHdrArmAddress){
  mProcessingComp->emptyBufferDone_cb(dspBufferHdrArmAddress);
}

ENS_API_EXPORT void NmfMpc_ProcessingComponent::setPriorityLevel(OMX_U32 priority){
    mMyPriority = priority;
}

ENS_API_EXPORT OMX_U32 NmfMpc_ProcessingComponent::getPriorityLevel(void) const{
    return mMyPriority;
}

ENS_API_EXPORT t_cm_instance_handle NmfMpc_ProcessingComponent::getNmfSharedBuf(OMX_U32 portIdx) const {
    return mNmfSharedBuf[portIdx];
}

OMX_ERRORTYPE NmfMpc_ProcessingComponent::allocateInterfaces(void) {

    mIemptyThisBuffer = new Ishared_emptythisbuffer [mENSComponent.getPortCount()];
    if (mIemptyThisBuffer == 0) return OMX_ErrorInsufficientResources;

    mIfillThisBuffer = new Ishared_fillthisbuffer [mENSComponent.getPortCount()];
    if (mIfillThisBuffer == 0) return OMX_ErrorInsufficientResources;

    mNmfSharedBuf = new t_cm_instance_handle [mENSComponent.getPortCount()];
    if (mNmfSharedBuf == 0) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

void NmfMpc_ProcessingComponent::freeInterfaces(void) {
    delete [] mIemptyThisBuffer;
    delete [] mIfillThisBuffer;
    delete [] mNmfSharedBuf;
}

OMX_ERRORTYPE NmfMpc_ProcessingComponent::instantiateNmfSharedBuffers(ENS_Port *port) {
    OMX_ERRORTYPE error;

    if(port->getDirection() == OMX_DirInput) {

        error = ENS::instantiateNMFComponent(
                mENSComponent.getNMFDomainHandle(), "nmf.mpc.shared_bufin", 
                "shared_bufin", &mNmfSharedBuf[port->getPortIndex()], getPriorityLevel());
        if (error != OMX_ErrorNone) return error;

        error = ENS::bindComponentFromHost(
                mNmfSharedBuf[port->getPortIndex()], "emptythisbuffer",
                &mIemptyThisBuffer[port->getPortIndex()], port->getBufferCountActual());
        if(error != OMX_ErrorNone) return error;

        error = ENS::bindComponentToHost(
                port->getComponent(), mNmfSharedBuf[port->getPortIndex()], 
                "host", (shared_fillthisbufferDescriptor *)&callbackHandler, port->getBufferCountActual());
        if (error != OMX_ErrorNone) return error;

    } else {

        error = ENS::instantiateNMFComponent(
                mENSComponent.getNMFDomainHandle(), "nmf.mpc.shared_bufout", 
                "shared_bufout", &mNmfSharedBuf[port->getPortIndex()], getPriorityLevel());
        if (error != OMX_ErrorNone) return error;

        error = ENS::bindComponentFromHost(
                mNmfSharedBuf[port->getPortIndex()], "fillthisbuffer",
                &mIfillThisBuffer[port->getPortIndex()], port->getBufferCountActual());
        if(error != OMX_ErrorNone) return error;

        error = ENS::bindComponentToHost(
                port->getComponent(), mNmfSharedBuf[port->getPortIndex()], 
                "host", (shared_emptythisbufferDescriptor *)&callbackHandler, port->getBufferCountActual());
        if (error != OMX_ErrorNone) return error;

    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpc_ProcessingComponent::deInstantiateNmfSharedBuffers(ENS_Port *port) {
    OMX_ERRORTYPE error;

    if(port->getDirection() == OMX_DirInput) {

        error = ENS::unbindComponentFromHost(&mIemptyThisBuffer[port->getPortIndex()]);
        if (error != OMX_ErrorNone) return error;

    } else {

        error = ENS::unbindComponentFromHost(&mIfillThisBuffer[port->getPortIndex()]);
        if (error != OMX_ErrorNone) return error;

    }

    error = ENS::unbindComponentToHost(port->getComponent(), mNmfSharedBuf[port->getPortIndex()], "host");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfSharedBuf[port->getPortIndex()]);
    if (error != OMX_ErrorNone) return error;

    mNmfSharedBuf[port->getPortIndex()] = 0;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpc_ProcessingComponent::startNmfSharedBuf(OMX_U32 nPortIdx) {
    return ENS::startNMFComponent(mNmfSharedBuf[nPortIdx]);
}

OMX_ERRORTYPE NmfMpc_ProcessingComponent::stopNmfSharedBuf(OMX_U32 nPortIdx) {
    return ENS::stopNMFComponent(mNmfSharedBuf[nPortIdx]);
}

OMX_ERRORTYPE NmfMpc_ProcessingComponent::instantiateLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::instantiateNMFComponent(
            mENSComponent.getNMFDomainHandle(), "fsm.generic", 
            "genericfsm", &mNmfGenericFsmLib, getPriorityLevel());
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            mENSComponent.getNMFDomainHandle(), "fsm.component", 
            "componentfsm", &mNmfComponentFsmLib, getPriorityLevel());
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            mENSComponent.getNMFDomainHandle(), "osttrace.mmdsp", 
            "OSTTRACE", &mOstTrace, getPriorityLevel());
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfComponentFsmLib, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfComponentFsmLib, "genericfsm", mNmfGenericFsmLib, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    t_cm_error cm_error = CM_WriteComponentAttribute (mOstTrace, "trace_mode", mMpcTracingMode);
    if (cm_error != CM_OK) return OMX_ErrorUndefined;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpc_ProcessingComponent::startLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfGenericFsmLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mNmfComponentFsmLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpc_ProcessingComponent::stopLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfGenericFsmLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent(mNmfComponentFsmLib);
    if (error != OMX_ErrorNone) return error;

	error = ENS::stopNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpc_ProcessingComponent::deInstantiateLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponent(
            mNmfComponentFsmLib, "genericfsm");
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfComponentFsmLib, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfComponentFsmLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfGenericFsmLib);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::construct(void) {
    OMX_ERRORTYPE error;

    CM_REGISTER_STUBS_SKELS(ens_cpp);

    MEMORY_TRACE_ENTER2("NmfMpc_ProcessingComponent::construct (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    traceMemoryStatus(); // output all available memory

	// trace stuff
	// create ExtraZone if needed
	error = mENSComponent.getTraceBuilder()->createExtraZone(mENSComponent.getNMFDomainHandle());
    if (error != OMX_ErrorNone) return error;

	setTraceInfo(mENSComponent.getSharedTraceInfoPtr(), mENSComponent.getPortCount()); // has to be done before NMF initialization

    error = instantiateLibraries();
    if (error != OMX_ErrorNone) return error;

    error = allocateInterfaces();
    if (error != OMX_ErrorNone) return error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
        if (port->useProprietaryCommunication() == OMX_FALSE && 
                port->isEnabled() == OMX_TRUE  &&
			port->getDomain() != OMX_PortDomainOther) {
			//Instantiate Nmf shared buffer components
            error = instantiateNmfSharedBuffers(port);
            if (error != OMX_ErrorNone) return error;
        }
    }

    error = instantiate();
    if (error != OMX_ErrorNone) return error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->useProprietaryCommunication() == OMX_FALSE &&
                mENSComponent.getPort(i)->isEnabled() == OMX_TRUE &&
				mENSComponent.getPort(i)->getDomain() != OMX_PortDomainOther) {
            //Start Nmf shared buffer components
            error = startNmfSharedBuf(i);
            if (error != OMX_ErrorNone) return error;
        }
    }

    error = startLibraries();
    if (error != OMX_ErrorNone) return error;

    error = start();
    if (error != OMX_ErrorNone) return error;

    error = configure();
    if (error != OMX_ErrorNone) return error;

    // Initialize NMF component state machine (ports + component)
 	fsmInit_t initFsm = {0,0,0,0};
	
    initFsm.portsDisabled = 0;
    for (OMX_U32 i =0 ; i< mENSComponent.getPortCount(); i++) {
        if(mENSComponent.getPort(i)->isEnabled() == OMX_FALSE){
            initFsm.portsDisabled |= 1 << i;
        }
    }

	initFsm.portsTunneled = 0;
	for (OMX_U32 i =0 ; i< mENSComponent.getPortCount(); i++) {
        if(mENSComponent.getPort(i)->getTunneledComponent()){
			initFsm.portsTunneled |= 1 << i;
        }
    }

	initFsm.traceInfoAddr = (TraceInfo_t *)getDspAddr();
   
	fsmInit(&initFsm);

    MEMORY_TRACE_LEAVE2("NmfMpc_ProcessingComponent::construct (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    return OMX_ErrorNone;
}

ENS_API_EXPORT void NmfMpc_ProcessingComponent::fsmInit(fsmInit_t * initFsm) {
    mIfsmInit.fsmInit(*initFsm);
}

ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::destroy(void) {
    OMX_ERRORTYPE error;
	
	// delete ExtraZone if needed
	error = mENSComponent.getTraceBuilder()->deleteExtraZone();
    if (error != OMX_ErrorNone) return error;
    
    error = stopLibraries();
    if (error != OMX_ErrorNone) return error;

    error = stop();
    if (error != OMX_ErrorNone) return error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->useProprietaryCommunication() == OMX_FALSE &&
                mENSComponent.getPort(i)->isEnabled() == OMX_TRUE &&
				mENSComponent.getPort(i)->getDomain() != OMX_PortDomainOther) {
            //Stop Nmf shared buffer components
            error = stopNmfSharedBuf(i);
            if (error != OMX_ErrorNone) return error;
        }
    }

    error = deInstantiate();
    if (error != OMX_ErrorNone) return error;

    error = deInstantiateLibraries();
    if (error != OMX_ErrorNone) return error;

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
        if (mENSComponent.getPort(i)->useProprietaryCommunication() == OMX_FALSE &&
                mENSComponent.getPort(i)->isEnabled() == OMX_TRUE &&
			mENSComponent.getPort(i)->getDomain() != OMX_PortDomainOther) {
            //deInstantiate Nmf shared buffer components
            error = deInstantiateNmfSharedBuffers(mENSComponent.getPort(i));
            if (error != OMX_ErrorNone) return error;
        }
    }

    CM_UNREGISTER_STUBS_SKELS(ens_cpp);

    freeInterfaces();

    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData) {

    OMX_ERRORTYPE error;
    OMX_BOOL deferredCmd = OMX_FALSE;

    if(eCmd == OMX_CommandPortEnable) {
        error = sendEnablePortCommand(nData, deferredCmd);
        if(error != OMX_ErrorNone) return error;
    } else {
        error = doSpecificSendCommand(eCmd, nData, deferredCmd);
        if(error != OMX_ErrorNone) return error;
    }

    if(deferredCmd == OMX_FALSE) {
        mIsendCommand.sendCommand(eCmd, nData);
    }
    
    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::emptyThisBuffer(
        OMX_BUFFERHEADERTYPE* pBuffer) {

    SharedBuffer *sharedbuffer = static_cast<SharedBuffer *>(pBuffer->pInputPortPrivate);

    DBC_ASSERT(pBuffer->nFilledLen <= pBuffer->nAllocLen);

    sharedbuffer->updateMPCHeader();
    
    mIemptyThisBuffer[pBuffer->nInputPortIndex].emptyThisBuffer(sharedbuffer->getMPCHeader());

    return OMX_ErrorNone;
}


ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::fillThisBuffer(
        OMX_BUFFERHEADERTYPE* pBuffer) {
    
    SharedBuffer *sharedbuffer = static_cast<SharedBuffer *>(pBuffer->pOutputPortPrivate);

    pBuffer->nFlags = 0;
    pBuffer->nFilledLen = 0; 
    pBuffer->nOffset = 0;

    sharedbuffer->updateMPCHeader();
    
    mIfillThisBuffer[pBuffer->nOutputPortIndex].fillThisBuffer(sharedbuffer->getMPCHeader());

    return OMX_ErrorNone;
}


ENS_API_EXPORT void NmfMpc_ProcessingComponent::eventHandler(
        OMX_EVENTTYPE event, 
        OMX_U32 nData1, 
        OMX_U32 nData2)
{
    OMX_ERRORTYPE error;
    OMX_BOOL deferEventHandler = OMX_FALSE; // This variable is set to true by processing component 
                                            // if calling event handler callback up to the proxy has to be defered

    if(event == (OMX_EVENTTYPE)OMX_DSP_EventIndexSettingChanged){
        event = (OMX_EVENTTYPE)OMX_EventIndexSettingChanged;
    }
    if (event == OMX_EventCmdComplete && nData1 == OMX_CommandPortDisable) {
      disablePortIndication(nData2, deferEventHandler);
    } else {
        error = doSpecificEventHandler_cb(event, nData1, nData2, deferEventHandler);
        if (error != OMX_ErrorNone) {
            mENSComponent.eventHandler(OMX_EventError, (OMX_U32)error, 0);
            return;
        }
    }
    if (!deferEventHandler) {
        mENSComponent.eventHandler(event, nData1, nData2);
    }
}

ENS_API_EXPORT SharedBuffer * NmfMpc_ProcessingComponent::getSharedBuffer(t_uint32 dspBufferHdrArmAddress, bool enableCopy) {

	ARMSharedBuffer_t *arm_shared_buffer = (ARMSharedBuffer_t *)dspBufferHdrArmAddress;
	SharedBuffer      *sharedbuffer      = (SharedBuffer *)(arm_shared_buffer->hostClassAddr);
	if (sharedbuffer == 0) { 
		DBC_ASSERT(0);
		return 0;
	}
	sharedbuffer->updateOMXHeader(enableCopy);
	return sharedbuffer;
}

ENS_API_EXPORT void NmfMpc_ProcessingComponent::emptyBufferDone_cb(t_uint32 dspBufferHdrArmAddress) {

    SharedBuffer * sharedbuffer =  getSharedBuffer(dspBufferHdrArmAddress, false);

    doSpecificEmptyBufferDone_cb(sharedbuffer->getOMXHeader());
    OstTraceFiltInst2(TRACE_DEBUG, "ENS_PROXY: NmfMpc_ProcessingComponent:emptyBufferDone_cb  (HOST)0x%x (MPC)0x%x", (unsigned int)sharedbuffer->getOMXHeader(), (unsigned int)sharedbuffer->getMPCHeader());
    mENSComponent.emptyBufferDone(sharedbuffer->getOMXHeader());
}

ENS_API_EXPORT void NmfMpc_ProcessingComponent::fillBufferDone_cb(t_uint32 dspBufferHdrArmAddress) {
    
    SharedBuffer * sharedbuffer =  getSharedBuffer(dspBufferHdrArmAddress, true);

    doSpecificFillBufferDone_cb(sharedbuffer->getOMXHeader());
    OstTraceFiltInst2(TRACE_DEBUG, "ENS_PROXY: NmfMpc_ProcessingComponent:fillBufferDone_cb  (HOST)0x%x (MPC)0x%x", (unsigned int)sharedbuffer->getOMXHeader(), (unsigned int)sharedbuffer->getMPCHeader());
    mENSComponent.fillBufferDone(sharedbuffer->getOMXHeader());
}

ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::allocateBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex, 
        OMX_U32 nSizeBytes, 
        OMX_U8 **ppData, 
        void **bufferAllocInfo,
        void **portPrivateInfo)
{
    OMX_ERRORTYPE   error;

    error = doBufferAllocation(nPortIndex, nBufferIndex, nSizeBytes, ppData, bufferAllocInfo);
    if (error != OMX_ErrorNone) return error;
    DBC_ASSERT(bufferAllocInfo != 0);

	
    OMX_U32 bufPhysicalAddr = getBufferPhysicalAddress(*bufferAllocInfo, *ppData, nSizeBytes);
    OMX_U32 bufMpcAddress = getBufferMpcAddress(*bufferAllocInfo);
    SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(), 
            nSizeBytes, *ppData, bufPhysicalAddr, bufMpcAddress, *bufferAllocInfo, error); 
    if (sharedBuf == 0) return OMX_ErrorInsufficientResources;
    if (error != OMX_ErrorNone) return error;

    *portPrivateInfo = sharedBuf;

    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::useBufferHeader(
                OMX_DIRTYPE dir,
                OMX_BUFFERHEADERTYPE* pBufferHdr)
{
    SharedBuffer *sharedBuf = 0;

    if(!pBufferHdr) return OMX_ErrorBadParameter;

    if(dir == OMX_DirInput) {
        sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pInputPortPrivate);
    } else {
        sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pOutputPortPrivate);
    }
    if(!sharedBuf) return OMX_ErrorBadParameter;

    return sharedBuf->setOMXHeader(pBufferHdr);
}

ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::freeBuffer(
		OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_BOOL bBufferAllocated,
        void *bufferAllocInfo,
        void *portPrivateInfo) 
{
    if (nPortIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(nPortIndex)==0) {
        return OMX_ErrorBadPortIndex;
    }
    
    OMX_ERRORTYPE error;
    SharedBuffer *sharedBuf = static_cast<SharedBuffer *>(portPrivateInfo);

    if (bBufferAllocated) {
        error = doBufferDeAllocation(nPortIndex, nBufferIndex, bufferAllocInfo);
        if(error != OMX_ErrorNone) return error;
    } else if (useBufferNeedsMemcpy()){
        error = doBufferDeAllocation(nPortIndex, nBufferIndex, (void*)sharedBuf->getBufferAllocInfo());
        if(error != OMX_ErrorNone) return error;
    } else {
        ENS_Port *port = mENSComponent.getPort(nPortIndex);
		MMHwBuffer *sharedChunk = (MMHwBuffer *)port->getSharedChunk();
		if (sharedChunk && (nBufferIndex == 0)) {    // ER342234
			error = MMHwBuffer::Close(sharedChunk);
			DBC_ASSERT(error == OMX_ErrorNone);
            port->setSharedChunk(0);
		}
    }

    delete sharedBuf;

    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::freeSharedBuffer(
		OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_BOOL bBufferAllocated,
        void *bufferAllocInfo,
        void *portPrivateInfo) 
{
    SharedBuffer *sharedBuf = static_cast<SharedBuffer *>(portPrivateInfo);
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    MMHwBuffer *sharedChunk = (MMHwBuffer *)port->getSharedChunk();
    if (sharedChunk && (nBufferIndex == port->getBufferCountActual()-1) ) {
            //error = MMHwBuffer::Close(sharedChunk);
            //DBC_ASSERT(error == OMX_ErrorNone);
            port->setSharedChunk(0);
    }


    delete sharedBuf;

    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::useBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex, 
        OMX_BUFFERHEADERTYPE* pBufferHdr,
        void **portPrivateInfo) 
{
    OMX_ERRORTYPE error;
    void *bufferAllocInfo = 0;
    OMX_U8 *pBuffer;
    
    if (nPortIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(nPortIndex)==0) {
        return OMX_ErrorBadPortIndex;
    }
    
    if (useBufferNeedsMemcpy()) {
        error = doBufferAllocation(nPortIndex, nBufferIndex, pBufferHdr->nAllocLen, &pBuffer, &bufferAllocInfo);
        if (error != OMX_ErrorNone) return error;
    } else {
        ENS_Port *port = mENSComponent.getPort(nPortIndex);
        bufferAllocInfo = port->getSharedChunk();
        pBuffer = pBufferHdr->pBuffer;

        OMX_ERRORTYPE error = ((MMHwBuffer *)bufferAllocInfo)->AddBufferInfo(nBufferIndex, (OMX_U32)pBufferHdr->pBuffer, pBufferHdr->nAllocLen);
        if (error != OMX_ErrorNone)
            return error;
    }

    OMX_U32 bufPhysicalAddr = getBufferPhysicalAddress(bufferAllocInfo, pBuffer, pBufferHdr->nAllocLen);
    OMX_U32 bufMpcAddress = getBufferMpcAddress(bufferAllocInfo);
    SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(), 
            pBufferHdr->nAllocLen, pBuffer, bufPhysicalAddr, bufMpcAddress, bufferAllocInfo, error); 
    if (sharedBuf == 0) return OMX_ErrorInsufficientResources;
    if (error != OMX_ErrorNone) return error;

    sharedBuf->setOMXHeader(pBufferHdr);

    *portPrivateInfo = sharedBuf;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfMpc_ProcessingComponent::sendEnablePortCommand(OMX_U32 portIndex, OMX_BOOL &bDeferredCmd) {

    OMX_ERRORTYPE error;
    t_uint16 isTunneled;

    MEMORY_TRACE_ENTER3("NmfMpc_ProcessingComponent::enablePort[%u] (%s) @0x%08X", (unsigned int)portIndex, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    if(portIndex == OMX_ALL) {
        for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
            ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
            DBC_ASSERT(port != 0);

            if (port->useProprietaryCommunication() == OMX_FALSE &&
                port->getDomain() != OMX_PortDomainOther) {
                //Instantiate Nmf shared buffer components
                error = instantiateNmfSharedBuffers(port);
                if (error != OMX_ErrorNone) return error;
            }
        }
    } else {
        if (portIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(portIndex)==0) {
            return OMX_ErrorBadPortIndex;
        }
        
        ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(portIndex));
        DBC_ASSERT(port != 0);

        if (port->useProprietaryCommunication() == OMX_FALSE &&
            port->getDomain() != OMX_PortDomainOther) {
            //Instantiate Nmf shared buffer components
            error = instantiateNmfSharedBuffers(port);
            if (error != OMX_ErrorNone) return error;
        }
    }

    error = doSpecificSendCommand(OMX_CommandPortEnable, portIndex, bDeferredCmd);
    if(error != OMX_ErrorNone) return error;

    isTunneled=0;
    if(portIndex == OMX_ALL) {
        for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
            ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
            DBC_ASSERT(port != 0);

            if (port->useProprietaryCommunication() == OMX_FALSE &&
                port->getDomain() != OMX_PortDomainOther) {
                //Start Nmf shared buffer components
                error = startNmfSharedBuf(i);
                if (error != OMX_ErrorNone) return error;
            }
            //Update port tunneling status to NMF component
            if(port->getTunneledComponent()){
                isTunneled |= 1 << i;
            }
        }
        mIfsmInit.setTunnelStatus(-1,isTunneled);
    } else {
        if (portIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(portIndex)==0) {
            return OMX_ErrorBadPortIndex;
        }
    
        ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(portIndex));
        DBC_ASSERT(port != 0);

        if (port->useProprietaryCommunication() == OMX_FALSE &&
                port->getDomain() != OMX_PortDomainOther) {
            //Start Nmf shared buffer components
            error = startNmfSharedBuf(portIndex);
            if (error != OMX_ErrorNone) return error;
        }
        //Update port tunneling status to NMF component
        isTunneled = 0; 
        if(port->getTunneledComponent()){
            isTunneled = 1 << portIndex;
        }
        mIfsmInit.setTunnelStatus(portIndex,isTunneled);
    }

    MEMORY_TRACE_LEAVE3("NmfMpc_ProcessingComponent::enablePort[%u] (%s) @0x%08X", (unsigned int)portIndex, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    return OMX_ErrorNone;
}

void NmfMpc_ProcessingComponent::disablePortIndication(OMX_U32 portIndex, OMX_BOOL &deferEventHandler) {

    OMX_ERRORTYPE error;

    if(portIndex == OMX_ALL) {
        for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
            ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
            DBC_ASSERT(port!=0);

            if (port->useProprietaryCommunication() == OMX_FALSE &&
                    port->getDomain() != OMX_PortDomainOther) {
                //Stop Nmf shared buffer components
                error = stopNmfSharedBuf(i);
                if (error != OMX_ErrorNone) {
                    mENSComponent.eventHandler(OMX_EventError, (OMX_U32)error, 0);
                    return;
                }
            }
        }
    } else {
        if (portIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(portIndex)==0) {
            mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorBadPortIndex, 0);
        }
    
        ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(portIndex));
        DBC_ASSERT(port!=0);

        if (port->useProprietaryCommunication() == OMX_FALSE &&
                port->getDomain() != OMX_PortDomainOther) {
            //Stop Nmf shared buffer components
            error = stopNmfSharedBuf(portIndex);
            if (error != OMX_ErrorNone) {
                mENSComponent.eventHandler(OMX_EventError, (OMX_U32)error, 0);
                return;
            }
        }
    }

    error = doSpecificEventHandler_cb(OMX_EventCmdComplete, OMX_CommandPortDisable, portIndex, deferEventHandler);
    if (error != OMX_ErrorNone) {
        mENSComponent.eventHandler(OMX_EventError, (OMX_U32)error, 0);
        return;
    }

    if(portIndex == OMX_ALL) {
        for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
            ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
            DBC_ASSERT(port!=0);

            if (port->useProprietaryCommunication() == OMX_FALSE &&
                        port->getDomain() != OMX_PortDomainOther) {
                //deInstantiate Nmf shared buffer components
                error = deInstantiateNmfSharedBuffers(port);
                if (error != OMX_ErrorNone) {
                    mENSComponent.eventHandler(OMX_EventError, (OMX_U32)error, 0);
                    return;
                }
            }
        }
    } else {
        if (portIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(portIndex)==0) {
            mENSComponent.eventHandler(OMX_EventError, (OMX_U32)OMX_ErrorBadPortIndex, 0);
        }
    
        ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(portIndex));
        DBC_ASSERT(port!=0);

        if (port->useProprietaryCommunication() == OMX_FALSE &&
                    port->getDomain() != OMX_PortDomainOther) {
            //deInstantiate Nmf shared buffer components
            error = deInstantiateNmfSharedBuffers(port);
            if (error != OMX_ErrorNone) {
                mENSComponent.eventHandler(OMX_EventError, (OMX_U32)error, 0);
                return;
            }
        }
    }
}

ENS_API_EXPORT OMX_ERRORTYPE NmfMpc_ProcessingComponent::getMMHWBufferInfo(
        OMX_U32 nPortIndex,
        OMX_U32 nSizeBytes, 
        OMX_U8 *pData, 
        void **bufferAllocInfo,
        void **portPrivateInfo)
{
    OMX_ERRORTYPE   error;
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    *bufferAllocInfo = port->getSharedChunk();

    OMX_U32 bufPhysicalAddr = getBufferPhysicalAddress(*bufferAllocInfo, pData, nSizeBytes);
    OMX_U32 bufMpcAddress = getBufferMpcAddress(*bufferAllocInfo);
    SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(),
            nSizeBytes, pData, bufPhysicalAddr, bufMpcAddress, *bufferAllocInfo, error);
    if (sharedBuf == 0) return OMX_ErrorInsufficientResources;
    if (error != OMX_ErrorNone) return error;

    *portPrivateInfo = sharedBuf;

    return OMX_ErrorNone;
}

void NmfMpc_ProcessingComponent::configureTraceModeMpc(OMX_U32 nTraceMode)
{
    mMpcTracingMode = nTraceMode;
}

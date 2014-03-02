/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfHost_ProcessingComponent.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Port.h"
#include "NmfHost_ProcessingComponent.h"
#include <cpp.hpp>
#include "mmhwbuffer.h"
#include "fsmInit.idt.h"

const char * OMX_EVENTTYPE_NAME[]=
{
  STRING(OMX_EventCmdComplete),
  STRING(OMX_EventError),
  STRING(OMX_EventMark),
  STRING(OMX_EventPortSettingsChanged),
  STRING(OMX_EventBufferFlag),
  STRING(OMX_EventResourcesAcquired),
  STRING(OMX_EventComponentResumed),
  STRING(OMX_EventDynamicResourcesAvailable),
  STRING(OMX_EventPortFormatDetected),
  STRING(OMX_EventMax)
};

const char* OMX_STATETYPE_NAME[]=
{
  STRING(OMX_StateInvalid),
  STRING(OMX_StateLoaded),
  STRING(OMX_StateIdle),
  STRING(OMX_StateExecuting),
  STRING(OMX_StatePause),
  STRING(OMX_StateTransientToIdle),
  STRING(OMX_StateWaitForResources),
  STRING(OMX_StateTransient),
  STRING(OMX_StateLoadedToIdleHOST),
  STRING(OMX_StateLoadedToIdleDSP),
  STRING(OMX_StateIdleToLoaded),
  STRING(OMX_StateIdleToExecuting),
  STRING(OMX_StateIdleToPause),
  STRING(OMX_StateExecutingToIdle),
  STRING(OMX_StateExecutingToPause),
  STRING(OMX_StatePauseToIdle),
  STRING(OMX_StatePauseToExecuting),
  STRING(OMX_StateMax)
};

const char* OMX_COMMANDTYPE_NAME[]=
{
  STRING(OMX_CommandStateSet),
  STRING(OMX_CommandFlush),
  STRING(OMX_CommandPortDisable),
  STRING(OMX_CommandPortEnable),
  STRING(OMX_CommandMarkBuffer),
  STRING(OMX_CommandMax)
};


ENS_API_EXPORT void NmfHost_ProcessingComponentCB::emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) 	{
  //NMF_LOG("NmfHost_ProcessingComponent: emptythisbuffer done: 0x%x\n", buffer);
    NmfHost_ProcessingComponent *pProc = (NmfHost_ProcessingComponent *)(&mENSComponent->getProcessingComponent());
    pProc->doSpecificFillBufferDone_cb((OMX_BUFFERHEADERTYPE *) buffer);
    mENSComponent->fillBufferDone((OMX_BUFFERHEADERTYPE *) buffer);
}

ENS_API_EXPORT void NmfHost_ProcessingComponentCB::fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) {
// NMF_LOG("NmfHost_ProcessingComponent: fillthisbuffer done: 0x%x\n", buffer);
    NmfHost_ProcessingComponent *pProc = (NmfHost_ProcessingComponent *)(&mENSComponent->getProcessingComponent());
    pProc->doSpecificEmptyBufferDone_cb((OMX_BUFFERHEADERTYPE *) buffer);
    mENSComponent->emptyBufferDone((OMX_BUFFERHEADERTYPE *)buffer);
}

// method inherited from eventhandlerDescriptor
ENS_API_EXPORT void NmfHost_ProcessingComponent::eventHandler(OMX_EVENTTYPE event, t_uint32 nData1, t_uint32 nData2)  {
    OMX_ERRORTYPE error;
    OMX_BOOL deferEventHandler = OMX_FALSE;
    /*NMF_LOG("NmfHost_ProcessingComponent: eventhandler - %s - %s (0x%x) %s (0x%x)\n", 
     * OMX_EVENTTYPE_NAME[event], 
     * (event == OMX_EventCmdComplete)?OMX_COMMANDTYPE_NAME[nData1]:NULL, nData1, 
     * ((event == OMX_EventCmdComplete) &&(nData2 == OMX_CommandStateSet))?OMX_STATETYPE_NAME[nData2]:NULL, nData2);
     */
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

ENS_API_EXPORT NmfHost_ProcessingComponent::NmfHost_ProcessingComponent(ENS_Component & enscomp) : ProcessingComponent(enscomp) {
	callbackHandler.setProcessingComponent(&enscomp) ;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::construct() {
    OMX_ERRORTYPE error;

#ifndef HOST_ONLY
    CM_REGISTER_STUBS_SKELS(ens_cpp);
#endif //#ifndef HOST_ONLY

	// trace stuff
	// create ExtraZone if needed
	error = mENSComponent.getTraceBuilder()->createExtraZone(mENSComponent.getNMFDomainHandle());
    if (error != OMX_ErrorNone) return error;

	setTraceInfo(mENSComponent.getSharedTraceInfoPtr(), mENSComponent.getPortCount()); 

    error = instantiate();
    if (error != OMX_ErrorNone) return error;

    error = start();
    if (error != OMX_ErrorNone) return error;

    error = configure();
    if (error != OMX_ErrorNone) return error;

    // Initialize NMF component state machine (ports + component)
	fsmInit_t init = {0,0,0,0};
    init.portsDisabled = 0;
    for (OMX_U32 i =0 ; i< mENSComponent.getPortCount(); i++) {
        if(!(mENSComponent.getPort(i)->isEnabled())){
            init.portsDisabled |= 1 << i;
        }
    }

	init.portsTunneled = 0;
	for (OMX_U32 i =0 ; i< mENSComponent.getPortCount(); i++) {
		if(mENSComponent.getPort(i)->getTunneledComponent()){
			init.portsTunneled |= 1 << i;
		}
	}

	init.traceInfoAddr = mENSComponent.getSharedTraceInfoPtr();
	
	mIfsmInit.fsmInit(init);

	return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::destroy() {
    OMX_ERRORTYPE error;

    error = stop();
    if (error != OMX_ErrorNone) return error;

    error = deInstantiate();
    if (error != OMX_ErrorNone) return error;

    // delete ExtraZone if needed
	error = mENSComponent.getTraceBuilder()->deleteExtraZone();
    if (error != OMX_ErrorNone) return error;

#ifndef HOST_ONLY
    CM_UNREGISTER_STUBS_SKELS(ens_cpp);
#endif //#ifndef HOST_ONLY

    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData) {

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

// plumbing...
ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer) {
  mIemptyThisBuffer[pBuffer->nInputPortIndex].emptyThisBuffer(pBuffer);
  return OMX_ErrorNone;
}

// plumbing...
ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer) {
  mIfillThisBuffer[pBuffer->nOutputPortIndex].fillThisBuffer(pBuffer);
  return OMX_ErrorNone;
}


ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::allocateBufferHeap(OMX_U32 nPortIndex,
							 OMX_U32 nBufferIndex,
							 OMX_U32 nSizeBytes,
							 OMX_U8 **ppData,
							 void **bufferAllocInfo,
							 void **portPrivateInfo) {
  //NMF_LOG("allocateBuffer call...%d\n", nBufferIndex);

  OMX_U8 * ptr = (OMX_U8 *) new char[nSizeBytes];

  if (ptr ==NULL) {
    //NMF_LOG("OMX_ErrorInsufficientResources");
	return OMX_ErrorInsufficientResources;
  }

  *ppData = ptr;

  *((OMX_U32 **) bufferAllocInfo) = (OMX_U32 *) ptr;

  *portPrivateInfo = (void *)0;
  
  return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::useBufferHeap(OMX_U32 nPortIndex,
						    OMX_U32 nBufferIndex,
						    OMX_BUFFERHEADERTYPE* pBufferHdr,
						    void **portPrivateInfo) {
  //NMF_LOG("useBuffer call...%d 0x%x 0x%x\n", nBufferIndex, pBufferHdr, portPrivateInfo);
  *portPrivateInfo = (void *)0;
  
  return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::freeBufferHeap(OMX_U32 nPortIndex,
						     OMX_U32 nBufferIndex,
						     OMX_BOOL bBufferAllocated,
						     void *bufferAllocInfo,
						     void *portPrivateInfo) {

  //NMF_LOG("freeBuffer call...%d\n", nBufferIndex);

  if (bBufferAllocated) {
    delete [] (char *) bufferAllocInfo;
  }

  return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::useBufferHeaderHeap(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer) {

  //NMF_LOG("useBufferHeader call...%d 0x%x\n", pBuffer);

  return OMX_ErrorNone;
}

// FIXME: bufferAllocInfo could be removed when pAppPrivate mechanism deprecated 
ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::allocateBufferVisual(OMX_U32 nPortIndex,
							 OMX_U32 nBufferIndex,
							 OMX_U32 nSizeBytes,
							 OMX_U8 **ppData,
							 void **bufferAllocInfo,
							 void **portPrivateInfo,
                             OMX_BOOL isCached)
{
    MMHwBuffer *sharedChunk=0;
    OMX_ERRORTYPE error;
    if (nPortIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(nPortIndex)==0) {
        return OMX_ErrorBadPortIndex;
    }
    
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
	if (nBufferIndex == 0) {
        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        poolAttrs.iBuffers = port->getBufferCountActual();      // Number of buffers in the pool
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;      // Memory type
        poolAttrs.iDomainID = 0;                                // MPC Domain ID (only requested for MPC memory type)
        poolAttrs.iSize = nSizeBytes;                           // Size (in byte) of a buffer
        poolAttrs.iAlignment = 256;                             // Alignment applied to the base address of each buffer in the pool
		                                                        //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
        poolAttrs.iCacheAttr = (isCached ? (MMHwBuffer::ENormalCached) : (MMHwBuffer::ENormalUnCached));
        error = MMHwBuffer::Create(poolAttrs, mENSComponent.getOMXHandle(), sharedChunk);
		if (error != OMX_ErrorNone) return OMX_ErrorInsufficientResources;
        port->setSharedChunk(sharedChunk);
	}

	// Let's now retrieve the "current" buffer
    sharedChunk = port->getSharedChunk();
    MMHwBuffer::TBufferInfo bufferInfo;
    error = sharedChunk->BufferInfo(nBufferIndex, bufferInfo);
	if (error != OMX_ErrorNone) return OMX_ErrorInsufficientResources;
    
    *ppData = (OMX_U8 *)bufferInfo.iLogAddr;

    *portPrivateInfo = sharedChunk;
    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::useBufferVisual(OMX_U32 nPortIndex,
						    OMX_U32 nBufferIndex,
						    OMX_BUFFERHEADERTYPE* pBufferHdr,
						    void **portPrivateInfo)
{
    if (nPortIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(nPortIndex)==0) {
        return OMX_ErrorBadPortIndex;
    }
    
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    MMHwBuffer *sharedChunk = port->getSharedChunk();
    if (sharedChunk) {
        OMX_ERRORTYPE error = sharedChunk->AddBufferInfo(nBufferIndex, (OMX_U32)pBufferHdr->pBuffer, pBufferHdr->nAllocLen);
        if (error != OMX_ErrorNone) {
            return error;
        }
        *portPrivateInfo = sharedChunk;
    } else {
        *portPrivateInfo = 0;
    }
    return OMX_ErrorNone;
}

// FIXME: bufferAllocInfo, portPrivateInfo pPortPoolid could be removed when pAppPrivate mechanism deprecated 
ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::freeBufferVisual(OMX_U32 nPortIndex,
						     OMX_U32 nBufferIndex,
						     OMX_BOOL bBufferAllocated,
						     void *bufferAllocInfo,
						     void *portPrivateInfo)
{
    if (nPortIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(nPortIndex)==0) {
        return OMX_ErrorBadPortIndex;
    }
    
    // No need to reset shared chunk as done by the freeBuffer of the port
    OMX_ERRORTYPE error;
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    MMHwBuffer *sharedChunk = port->getSharedChunk();
    if (sharedChunk) {
        DBC_ASSERT(sharedChunk==portPrivateInfo);
        if (nBufferIndex == 0) {    // ER342234
            if (bBufferAllocated) {
                error = MMHwBuffer::Destroy(sharedChunk);
                DBC_ASSERT(error == OMX_ErrorNone);
            } else {
                error = MMHwBuffer::Close(sharedChunk);
                DBC_ASSERT(error == OMX_ErrorNone);
            }
            port->setSharedChunk(0);
        }
    }
    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::freeSharedBuffer(OMX_U32 nPortIndex,
						     OMX_U32 nBufferIndex,
						     OMX_BOOL bBufferAllocated,
						     void *bufferAllocInfo,
						     void *portPrivateInfo)
{
    // No need to reset shared chunk as done by the freeBuffer of the port
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    MMHwBuffer *sharedChunk = port->getSharedChunk();
    if (sharedChunk) {
        DBC_ASSERT(sharedChunk==portPrivateInfo);
        if (nBufferIndex == port->getBufferCountActual()-1) {
            port->setSharedChunk(0);
        }
    }
    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::useBufferHeaderVisual(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer)
{
    return OMX_ErrorNone;
}

ENS_API_EXPORT void NmfHost_ProcessingComponent::disablePortIndication(OMX_U32 portIndex, OMX_BOOL &deferEventHandler) {

    OMX_ERRORTYPE error;

    error = doSpecificEventHandler_cb(OMX_EventCmdComplete, OMX_CommandPortDisable, portIndex, deferEventHandler);
    if (error != OMX_ErrorNone) {
        mENSComponent.eventHandler(OMX_EventError, (OMX_U32)error, 0);
        return;
    }
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::sendEnablePortCommand(OMX_U32 portIndex, OMX_BOOL &bDeferredCmd) {

    OMX_ERRORTYPE error;
    t_uint16 isTunneled;

    error = doSpecificSendCommand(OMX_CommandPortEnable, portIndex, bDeferredCmd);
    if(error != OMX_ErrorNone) return error;

    isTunneled=0;
    if(portIndex == OMX_ALL) {
        for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
            ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
            DBC_ASSERT(port != 0);
            
            //Update port tunneling status to NMF component
            if(port->getTunneledComponent()){
                isTunneled |= 1<<i;
            }
        }
        mIfsmInit.setTunnelStatus(-1,isTunneled);
    } else {
        if (portIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(portIndex)==0) {
            return OMX_ErrorBadPortIndex;
        }
        
        ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(portIndex));
        DBC_ASSERT(port != 0);
        
		//Update port tunneling status to NMF component
        isTunneled = 0;
		if(port->getTunneledComponent()){
            isTunneled = 1 << portIndex;
		}
		mIfsmInit.setTunnelStatus(portIndex,isTunneled);
    }

    return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHost_ProcessingComponent::getMMHWBufferInfo(
        OMX_U32 nPortIndex,
        OMX_U32 nSizeBytes, 
        OMX_U8 *pData, 
        void **bufferAllocInfo,
        void **portPrivateInfo)
{
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    *bufferAllocInfo = port->getSharedChunk();
    *portPrivateInfo = port->getSharedChunk();

    return OMX_ErrorNone;
}

ENS_API_EXPORT void NmfHost_ProcessingComponent::doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
}

ENS_API_EXPORT void NmfHost_ProcessingComponent::doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
}

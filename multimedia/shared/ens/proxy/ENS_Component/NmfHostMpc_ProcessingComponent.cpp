/*****************************************************************************/
/**
 *  (c) ST-Ericsson, 2009 - All rights reserved
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson
 *
 * \file   NmfHost_ProcessingComponent.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Port.h"
#include "NmfHostMpc_ProcessingComponent.h"
#include "SharedBuffer.h"
#include <cm/inc/cm_macros.h>

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ens_proxy_ENS_Component_NmfHostMpc_ProcessingComponentTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE

ENS_API_EXPORT void NmfHostMpc_ProcessingComponentCB::emptyThisBuffer(t_uint32 dspBufferHdrArmAddress) {

  mProcessingComp->fillBufferDoneMpc_cb(dspBufferHdrArmAddress);
}

ENS_API_EXPORT void NmfHostMpc_ProcessingComponentCB::fillThisBuffer(t_uint32 dspBufferHdrArmAddress){
  mProcessingComp->emptyBufferDoneMpc_cb(dspBufferHdrArmAddress);
}


ENS_API_EXPORT NmfHostMpc_ProcessingComponent::NmfHostMpc_ProcessingComponent(ENS_Component & enscomp)
  : NmfHost_ProcessingComponent(enscomp), mMyPriority((OMX_U32)NMF_SCHED_NORMAL)
{
  callbackHandler.setProcessingComponent(this);

  for(int i=0;i<MAX_PORTS_COUNT;i++)
  {
    mNmfSharedBuf[i] = 0;
  }
}

OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::instantiateNmfSharedBuffers(ENS_Port *port) {
  OMX_ERRORTYPE error;

  // As sharedbuffer are no longer deleted on disable port, it may already be instantiate when calling this function
  if(mNmfSharedBuf[port->getPortIndex()]) return OMX_ErrorNone;

  if(port->getDirection() == OMX_DirInput)
  {
    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(), "nmf.mpc.shared_bufin",
					 "shared_bufin", &(mNmfSharedBuf[port->getPortIndex()]), getPriorityLevel());
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHost(mNmfSharedBuf[port->getPortIndex()], "emptythisbuffer",
				       &(mIemptyThisBufferMpc[port->getPortIndex()]),
				       port->getBufferCountActual());
    if(error != OMX_ErrorNone) return error;

    error = ENS::bindComponentToHost(port->getComponent(), mNmfSharedBuf[port->getPortIndex()], "host",
				     (shared_fillthisbufferDescriptor *)&callbackHandler, port->getBufferCountActual());
    if (error != OMX_ErrorNone) return error;
  }
  else
  {
    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(), "nmf.mpc.shared_bufout",
					 "shared_bufout", &(mNmfSharedBuf[port->getPortIndex()]), getPriorityLevel());
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHost(mNmfSharedBuf[port->getPortIndex()], "fillthisbuffer",
				       &(mIfillThisBufferMpc[port->getPortIndex()]),
				       port->getBufferCountActual());
    if(error != OMX_ErrorNone) return error;

    error = ENS::bindComponentToHost(port->getComponent(), mNmfSharedBuf[port->getPortIndex()], "host",
				     (shared_emptythisbufferDescriptor *)&callbackHandler, port->getBufferCountActual());
    if (error != OMX_ErrorNone) return error;
  }

  return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::deInstantiateNmfSharedBuffers(ENS_Port *port) {
  OMX_ERRORTYPE error;

  if(port->getDirection() == OMX_DirInput)
  {
    error = ENS::unbindComponentFromHost(&(mIemptyThisBufferMpc[port->getPortIndex()]));
    if (error != OMX_ErrorNone) return error;
  }
  else
  {
    error = ENS::unbindComponentFromHost(&(mIfillThisBufferMpc[port->getPortIndex()]));
    if (error != OMX_ErrorNone) return error;
  }

  error = ENS::unbindComponentToHost(port->getComponent(), mNmfSharedBuf[port->getPortIndex()], "host");
  if (error != OMX_ErrorNone) return error;

  error = ENS::destroyNMFComponent(mNmfSharedBuf[port->getPortIndex()]);
  if (error != OMX_ErrorNone) return error;

  mNmfSharedBuf[port->getPortIndex()] = 0;
  return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::startNmfSharedBuf(OMX_U32 nPortIdx) {
  return ENS::startNMFComponent(mNmfSharedBuf[nPortIdx]);
}


OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::stopNmfSharedBuf(OMX_U32 nPortIdx) {
  return ENS::stopNMFComponent(mNmfSharedBuf[nPortIdx]);
}


ENS_API_EXPORT t_cm_instance_handle NmfHostMpc_ProcessingComponent::getNmfSharedBuf(OMX_U32 portIdx) const {
    return mNmfSharedBuf[portIdx];
}


ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::construct() {
  OMX_ERRORTYPE error;

  CM_REGISTER_STUBS_SKELS(ens_cpp);

  MEMORY_TRACE_ENTER2("NmfHostMpc_ProcessingComponent::construct (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

  traceMemoryStatus(); // output all available memory

  // trace stuff
  // create ExtraZone if needed
  error = mENSComponent.getTraceBuilder()->createExtraZone(mENSComponent.getNMFDomainHandle());
  if (error != OMX_ErrorNone) return error;

  setTraceInfo(mENSComponent.getSharedTraceInfoPtr(), mENSComponent.getPortCount());

  for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++)
  {
    ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
    if (port->useProprietaryCommunication() == OMX_FALSE &&
	port->isEnabled() == OMX_TRUE  &&
	port->getDomain() != OMX_PortDomainOther &&
	port->isMpc())
    {
      error = instantiateNmfSharedBuffers(port);
      if (error != OMX_ErrorNone) return error;
    }
  }

  error = instantiate();
  if (error != OMX_ErrorNone) return error;

  for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++)
  {
    ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
    if (port->useProprietaryCommunication() == OMX_FALSE &&
	port->isEnabled() == OMX_TRUE  &&
	port->getDomain() != OMX_PortDomainOther &&
	port->isMpc())
    {
      //Start Nmf shared buffer components
      error = startNmfSharedBuf(i);
      if (error != OMX_ErrorNone) return error;
    }
  }

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

  MEMORY_TRACE_LEAVE2("NmfHostMpc_ProcessingComponent::construct (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

  return OMX_ErrorNone;
}


ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::destroy() {
  OMX_ERRORTYPE error;

  // delete ExtraZone if needed
  error = mENSComponent.getTraceBuilder()->deleteExtraZone();
  if (error != OMX_ErrorNone) return error;

  error = stop();
  if (error != OMX_ErrorNone) return error;

  for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++)
  {
    ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
    if (port->useProprietaryCommunication() == OMX_FALSE &&
	port->isEnabled() == OMX_TRUE  &&
	port->getDomain() != OMX_PortDomainOther &&
	port->isMpc())
    {
      error = stopNmfSharedBuf(i);
      if (error != OMX_ErrorNone) return error;
    }
  }

  error = deInstantiate();
  if (error != OMX_ErrorNone) return error;

  for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++)
  {
    ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
    if (port->useProprietaryCommunication() == OMX_FALSE &&
	((port->isEnabled() == OMX_TRUE) || mNmfSharedBuf[i]) &&
	port->getDomain() != OMX_PortDomainOther &&
	port->isMpc())
    {
      error = deInstantiateNmfSharedBuffers(mENSComponent.getPort(i));
      if (error != OMX_ErrorNone) return error;
    }
  }


  CM_UNREGISTER_STUBS_SKELS(ens_cpp);

  return OMX_ErrorNone;
}


ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
  OMX_U32 portIndex = pBuffer->nInputPortIndex;
  if (portIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(pBuffer->nInputPortIndex)==0) {
      return OMX_ErrorBadPortIndex;
  }

  ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(pBuffer->nInputPortIndex));

  if(port->isMpc())
  {
    SharedBuffer *sharedbuffer = static_cast<SharedBuffer *>(pBuffer->pInputPortPrivate);

    DBC_ASSERT(pBuffer->nFilledLen <= pBuffer->nAllocLen);

    sharedbuffer->updateMPCHeader();
    mIemptyThisBufferMpc[pBuffer->nInputPortIndex].emptyThisBuffer(sharedbuffer->getMPCHeader());
  }
  else
  {
    mIemptyThisBuffer[pBuffer->nInputPortIndex].emptyThisBuffer(pBuffer);
  }

  return OMX_ErrorNone;
}


ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
  OMX_U32 portIndex = pBuffer->nOutputPortIndex;
  if (portIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(pBuffer->nOutputPortIndex)==0) {
      return OMX_ErrorBadPortIndex;
  }

  ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(pBuffer->nOutputPortIndex));

  if(port->isMpc())
  {
    SharedBuffer *sharedbuffer = static_cast<SharedBuffer *>(pBuffer->pOutputPortPrivate);

    pBuffer->nFlags = 0;
	pBuffer->nFilledLen = 0;
	pBuffer->nOffset = 0;

    sharedbuffer->updateMPCHeader();
    mIfillThisBufferMpc[pBuffer->nOutputPortIndex].fillThisBuffer(sharedbuffer->getMPCHeader());
  }
  else
  {
    mIfillThisBuffer[pBuffer->nOutputPortIndex].fillThisBuffer(pBuffer);
  }

  return OMX_ErrorNone;
}




// only for mpc port
ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::allocateBuffer(OMX_U32 nPortIndex,
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



ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::useBufferHeader(OMX_DIRTYPE dir,
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




ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::freeBuffer(OMX_U32 nPortIndex,
									OMX_U32 nBufferIndex,
									OMX_BOOL bBufferAllocated,
									void *bufferAllocInfo,
									void *portPrivateInfo)
{
  OMX_ERRORTYPE error;
  SharedBuffer *sharedBuf = static_cast<SharedBuffer *>(portPrivateInfo);

  if (bBufferAllocated) {
    error = doBufferDeAllocation(nPortIndex, nBufferIndex, bufferAllocInfo);
    if(error != OMX_ErrorNone) return error;
  } else if (useBufferNeedsMemcpy()){
    error = doBufferDeAllocation(nPortIndex, nBufferIndex, (void*)sharedBuf->getBufferAllocInfo());
    if(error != OMX_ErrorNone) return error;
  }

  delete sharedBuf;

  return OMX_ErrorNone;
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::useBuffer(OMX_U32 nPortIndex,
								       OMX_U32 nBufferIndex,
								       OMX_BUFFERHEADERTYPE* pBufferHdr,
								       void **portPrivateInfo)
{
  OMX_ERRORTYPE error;
  void *bufferAllocInfo = 0;
  OMX_U8 *pBuffer;

  if (useBufferNeedsMemcpy()) {
    error = doBufferAllocation(nPortIndex, nBufferIndex, pBufferHdr->nAllocLen, &pBuffer, &bufferAllocInfo);
    if (error != OMX_ErrorNone) return error;
  } else {
    *portPrivateInfo = pBufferHdr->pPlatformPrivate;    // FIXME: should be removed when pAppPrivate mechanism deprecated
    pBuffer = pBufferHdr->pBuffer;
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

//
// suppose that in case of OMX_ALL, ALL port are enabled
//
ENS_API_EXPORT void NmfHostMpc_ProcessingComponent::disablePortIndication(OMX_U32 portIndex, OMX_BOOL &deferEventHandler) {

  OMX_ERRORTYPE error;

  if(portIndex == OMX_ALL) {
    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
      ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
      DBC_ASSERT(port!=0);

      if (port->useProprietaryCommunication() == OMX_FALSE &&
		  port->getDomain() != OMX_PortDomainOther &&
		  port->isMpc())
      {
		error = stopNmfSharedBuf(i);
		if (error != OMX_ErrorNone) {
		  mENSComponent.eventHandler(OMX_EventError, (OMX_U32)error, i);
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
		port->getDomain() != OMX_PortDomainOther &&
		port->isMpc())
    {
      error = stopNmfSharedBuf(portIndex);
      if (error != OMX_ErrorNone) {
		mENSComponent.eventHandler(OMX_EventError, (OMX_U32)error, portIndex);
		return;
      }
    }
  }

  error = doSpecificEventHandler_cb(OMX_EventCmdComplete, OMX_CommandPortDisable, portIndex, deferEventHandler);
  if (error != OMX_ErrorNone) {
    mENSComponent.eventHandler(OMX_EventError, (OMX_U32)error, 0);
    return;
  }

  // No longer deleted sharedbuffer, as deleting them may lead to lose NMF messages,
  // because in hybrid component, event messages are routed by NMF-ARM and buffers (for MPC port) are routed by NMF-DSP.
  // As there is no synchro between those 2 paths, we no longer delete NMF-DSP path.
}

ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::sendEnablePortCommand(OMX_U32 portIndex, OMX_BOOL &bDeferredCmd) {

  OMX_ERRORTYPE error;

  MEMORY_TRACE_ENTER3("NmfHostMpc_ProcessingComponent::enablePort[%u] (%s) @0x%08X", (unsigned int)portIndex, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

  if(portIndex == OMX_ALL)
  {
    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++) {
      ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
      DBC_ASSERT(port != 0);

      if (port->useProprietaryCommunication() == OMX_FALSE &&
	  port->getDomain() != OMX_PortDomainOther &&
	  port->isMpc()) {
	error = instantiateNmfSharedBuffers(port);
	if (error != OMX_ErrorNone) return error;
      }
    }
  }
  else
  {
    if (portIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(portIndex)==0) {
      return OMX_ErrorBadPortIndex;
    }

    ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(portIndex));
    DBC_ASSERT(port != 0);

    if (port->useProprietaryCommunication() == OMX_FALSE &&
	port->getDomain() != OMX_PortDomainOther &&
	port->isMpc()) {
      error = instantiateNmfSharedBuffers(port);
      if (error != OMX_ErrorNone) return error;
    }
  }

  error = doSpecificSendCommand(OMX_CommandPortEnable, portIndex, bDeferredCmd);
  if(error != OMX_ErrorNone) return error;

  if(portIndex == OMX_ALL)
  {
	t_uint16 isTunneled = 0;
    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++)
    {
      ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(i));
      DBC_ASSERT(port != 0);

      if (port->useProprietaryCommunication() == OMX_FALSE &&
		  port->getDomain() != OMX_PortDomainOther &&
		  port->isMpc()){
		error = startNmfSharedBuf(i);
		if (error != OMX_ErrorNone) return error;
      }

	  //Update port tunneling status to NMF component
	  if(port->getTunneledComponent()){
		isTunneled |= 1<<i;
	  }
	}

	//Update port tunneling status to NMF component
	mIfsmInit.setTunnelStatus(-1, isTunneled);
  }
  else
  {
    if (portIndex>=mENSComponent.getPortCount() || mENSComponent.getPort(portIndex)==0) {
      return OMX_ErrorBadPortIndex;
    }

    ENS_Port * port = static_cast<ENS_Port *>(mENSComponent.getPort(portIndex));
    DBC_ASSERT(port != 0);

    if (port->useProprietaryCommunication() == OMX_FALSE &&
	port->getDomain() != OMX_PortDomainOther &&
	port->isMpc()) {
      //Start Nmf shared buffer components
      error = startNmfSharedBuf(portIndex);
      if (error != OMX_ErrorNone) return error;
    }

	//Update port tunneling status to NMF component
	t_uint16 isTunneled = 0;
	if(port->getTunneledComponent()){
	  isTunneled = 1 << portIndex;
	}
	mIfsmInit.setTunnelStatus(portIndex,isTunneled);
  }

  MEMORY_TRACE_LEAVE3("NmfHostMpc_ProcessingComponent::enablePort[%u] (%s) @0x%08X", (unsigned int)portIndex, (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

  return OMX_ErrorNone;
}



ENS_API_EXPORT SharedBuffer * NmfHostMpc_ProcessingComponent::getSharedBuffer(t_uint32 dspBufferHdrArmAddress, bool enableCopy)
{
  ARMSharedBuffer_t *arm_shared_buffer = (ARMSharedBuffer_t *)dspBufferHdrArmAddress;
  SharedBuffer      *sharedbuffer      = (SharedBuffer *)(arm_shared_buffer->hostClassAddr);
  if (sharedbuffer == 0) {
    DBC_ASSERT(0);
    return 0;
  }
  sharedbuffer->updateOMXHeader(enableCopy);
  return sharedbuffer;
}

ENS_API_EXPORT void NmfHostMpc_ProcessingComponent::emptyBufferDoneMpc_cb(t_uint32 dspBufferHdrArmAddress)
{
  SharedBuffer * sharedbuffer =  getSharedBuffer(dspBufferHdrArmAddress, false);

  doSpecificEmptyBufferDone_cb(sharedbuffer->getOMXHeader());
  OstTraceFiltInst2(TRACE_DEBUG, "ENS_PROXY: NmfHostMpc_ProcessingComponent:emptyBufferDone_cb  (HOST)0x%x (MPC)0x%x", (unsigned int)sharedbuffer->getOMXHeader(), (unsigned int)sharedbuffer->getMPCHeader());
  mENSComponent.emptyBufferDone(sharedbuffer->getOMXHeader());
}

ENS_API_EXPORT void NmfHostMpc_ProcessingComponent::fillBufferDoneMpc_cb(t_uint32 dspBufferHdrArmAddress)
{
  SharedBuffer * sharedbuffer =  getSharedBuffer(dspBufferHdrArmAddress, true);

  doSpecificFillBufferDone_cb(sharedbuffer->getOMXHeader());
  OstTraceFiltInst2(TRACE_DEBUG, "ENS_PROXY: NmfHostMpc_ProcessingComponent:fillBufferDone_cb  (HOST)0x%x (MPC)0x%x", (unsigned int)sharedbuffer->getOMXHeader(), (unsigned int)sharedbuffer->getMPCHeader());
  mENSComponent.fillBufferDone(sharedbuffer->getOMXHeader());
}

ENS_API_EXPORT void NmfHostMpc_ProcessingComponent::setPriorityLevel(OMX_U32 priority){
    mMyPriority = priority;
}

ENS_API_EXPORT OMX_U32 NmfHostMpc_ProcessingComponent::getPriorityLevel(void) const{
    return mMyPriority;
}


ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::getMMHWBufferInfo(
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


ENS_API_EXPORT OMX_ERRORTYPE NmfHostMpc_ProcessingComponent::freeSharedBuffer(
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

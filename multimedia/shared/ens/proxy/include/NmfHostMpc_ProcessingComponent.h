/*****************************************************************************/
/**
 *  (c) ST-Ericsson, 2009 - All rights reserved
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson
 *
 * \file   NmfHostMpc_ProcessingComponent.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _HOST_MPC_NMF_PROCESSING_COMPONENT_H_
#define _HOST_MPC_NMF_PROCESSING_COMPONENT_H_

#include "ENS_Redefine_Class.h"

#include "host/shared_emptythisbuffer.hpp"
#include "host/shared_fillthisbuffer.hpp"
#include "NmfHost_ProcessingComponent.h"

#ifdef OMX_TRACE_UID
#undef OMX_TRACE_UID
#endif

#define OMX_TRACE_UID (1<<6)

#ifdef OMXCOMPONENT
#undef OMXCOMPONENT
#endif

#define OMXCOMPONENT "HOST_MPC_NMF_PROCESSING_COMPONENT"

class SharedBuffer;
class NmfHostMpc_ProcessingComponent;

class NmfHostMpc_ProcessingComponentCB : public shared_emptythisbufferDescriptor, public shared_fillthisbufferDescriptor {
public :	
	void setProcessingComponent(NmfHostMpc_ProcessingComponent * processingcomp) { mProcessingComp = processingcomp ;}
	
	// methods inherited from shared_emptythisbufferDescriptor
	ENS_API_IMPORT virtual void emptyThisBuffer(t_uint32 dspBufferHdrArmAddress) ;
	// methods inherited from shared_fillthisbufferDescriptor
	ENS_API_IMPORT virtual void fillThisBuffer(t_uint32 dspBufferHdrArmAddress) ;
	
private :
	NmfHostMpc_ProcessingComponent * mProcessingComp;
} ;


class NmfHostMpc_ProcessingComponent: public NmfHost_ProcessingComponent
{
 public:

  ENS_API_IMPORT NmfHostMpc_ProcessingComponent(ENS_Component & enscomp) ;

  ENS_API_IMPORT virtual OMX_ERRORTYPE construct();
  ENS_API_IMPORT virtual OMX_ERRORTYPE destroy();

  ENS_API_IMPORT virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer); 
  ENS_API_IMPORT virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

  ENS_API_IMPORT OMX_U32 getPriorityLevel(void) const;
  
  friend class NmfHostMpc_ProcessingComponentCB;
  
 protected:
  ENS_API_IMPORT virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, 
						      OMX_U32 nSizeBytes, OMX_U8 **ppData, 
						      void **bufferAllocInfo, void **portPrivateInfo);

  ENS_API_IMPORT virtual OMX_ERRORTYPE useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer);

  ENS_API_IMPORT virtual OMX_ERRORTYPE useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, 
						 OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo);

  ENS_API_IMPORT virtual OMX_ERRORTYPE freeBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, 
						  OMX_BOOL bBufferAllocated, void *bufferAllocInfo,
						  void *portPrivateInfo);

  ENS_API_IMPORT static SharedBuffer * getSharedBuffer(t_uint32 dspBufferHdrArmAddress, bool enableCopy);

  /// Get the NMF handle of the shared buffer (used for standard tunneling
  /// or non-tunneled communication
  ENS_API_IMPORT t_cm_instance_handle getNmfSharedBuf(OMX_U32 portIdx) const;
	
  void emptyBufferDoneMpc_cb(t_uint32 dspBufferHdrArmAddress); 
  void fillBufferDoneMpc_cb(t_uint32 dspBufferHdrArmAddress);
	
  ENS_API_IMPORT void setPriorityLevel(OMX_U32 priority);
  
  Ishared_emptythisbuffer  mIemptyThisBufferMpc[MAX_PORTS_COUNT];
  Ishared_fillthisbuffer   mIfillThisBufferMpc[MAX_PORTS_COUNT];
  t_cm_instance_handle     mNmfSharedBuf[MAX_PORTS_COUNT];
  
  /// Following methods allow to allocate/deallocate buffers for standard 
  /// tunneling or non-tunneled communication
  virtual OMX_ERRORTYPE doBufferAllocation(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, 
					   OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo) = 0;
  
  virtual OMX_ERRORTYPE doBufferDeAllocation(OMX_U32 nPortIndex, OMX_U32 nBufferIndex,
					     void *bufferAllocInfo) = 0;
  
  virtual OMX_U32 getBufferPhysicalAddress(void *bufferAllocInfo, OMX_U8 *,OMX_U32) = 0;
  virtual OMX_U32 getBufferMpcAddress(void *bufferAllocInfo) = 0;
  virtual bool useBufferNeedsMemcpy(void) const { return false; }

  /// These methods are called inside emptyBufferDone_cb/fillBufferDone_cb to allow 
  /// derived classes to update OMX buffer before returning it to ILClient in case of 
  /// non-tunneled communication or tunneled component in case of standard tunneling
  virtual void doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer) = 0;
  virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer) = 0;
  
 private:
  OMX_ERRORTYPE instantiateNmfSharedBuffers(ENS_Port *port);
  OMX_ERRORTYPE deInstantiateNmfSharedBuffers(ENS_Port *port);
  OMX_ERRORTYPE startNmfSharedBuf(OMX_U32 nPortIdx);
  OMX_ERRORTYPE stopNmfSharedBuf(OMX_U32 nPortIdx);

  NmfHostMpc_ProcessingComponentCB  callbackHandler ;
  
  ENS_API_IMPORT virtual OMX_ERRORTYPE sendEnablePortCommand(OMX_U32 portIndex, OMX_BOOL &bDeferredCmd);
  ENS_API_IMPORT virtual void disablePortIndication(OMX_U32 portIndex, OMX_BOOL &deferEventHandler);

  OMX_U32                     mMyPriority;

        //----------------------------------------------------------//
        //      Buffer sharing methods & data members
        //----------------------------------------------------------//
        
   private:
        ENS_API_IMPORT virtual OMX_ERRORTYPE getMMHWBufferInfo(OMX_U32 nPortIndex,
                                                               OMX_U32 nSizeBytes, 
                                                               OMX_U8 *pData, 
                                                               void **bufferAllocInfo,
                                                               void **portPrivateInfo);

        ENS_API_IMPORT virtual OMX_ERRORTYPE freeSharedBuffer(OMX_U32 nPortIndex,
                                                              OMX_U32 nBufferIndex,
                                                              OMX_BOOL bBufferAllocated,
                                                              void *bufferAllocInfo,
                                                              void *portPrivateInfo);

};

#endif // _HOST_NMF_PROCESSING_COMPONENT_H_

/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpc_ProcessingComponent.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _NMF_PROCESSING_COMPONENT_H_
#define _NMF_PROCESSING_COMPONENT_H_

#include "ENS_Redefine_Class.h"

#include "ProcessingComponent.h"
#include "host/sendcommand.hpp"
#include "host/shared_emptythisbuffer.hpp"
#include "host/shared_fillthisbuffer.hpp"
#include "host/fsminit.hpp"
#include "host/eventhandler.hpp"

class SharedBuffer;
class NmfMpc_ProcessingComponent;

class NmfMpc_ProcessingComponentCB : public shared_emptythisbufferDescriptor, public shared_fillthisbufferDescriptor {
public :	
	void setProcessingComponent(NmfMpc_ProcessingComponent * processingcomp) { mProcessingComp = processingcomp ;}
	
	// methods inherited from shared_emptythisbufferDescriptor
	ENS_API_IMPORT virtual void emptyThisBuffer(t_uint32 dspBufferHdrArmAddress) ;
	// methods inherited from shared_fillthisbufferDescriptor
	ENS_API_IMPORT virtual void fillThisBuffer(t_uint32 dspBufferHdrArmAddress) ;
	
private :
	NmfMpc_ProcessingComponent * mProcessingComp;
} ;


class NmfMpc_ProcessingComponent: public ProcessingComponent, public eventhandlerDescriptor {

    public:

        NmfMpc_ProcessingComponent(ENS_Component &enscomp) 
            : ProcessingComponent(enscomp), mNmfGenericFsmLib(0), mNmfComponentFsmLib(0),
            mOstTrace(0), mMpcTracingMode(0), mMyPriority((OMX_U32)NMF_SCHED_NORMAL)
        {
	  callbackHandler.setProcessingComponent(this) ;
        }
        
        ENS_API_IMPORT virtual OMX_ERRORTYPE construct(void);
        ENS_API_IMPORT virtual OMX_ERRORTYPE destroy(void);

        ENS_API_IMPORT virtual OMX_ERRORTYPE sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData);

        ENS_API_IMPORT virtual OMX_ERRORTYPE emptyThisBuffer(
                OMX_BUFFERHEADERTYPE* pBuffer); 

        ENS_API_IMPORT virtual OMX_ERRORTYPE fillThisBuffer(
                OMX_BUFFERHEADERTYPE* pBuffer);

        ENS_API_IMPORT virtual OMX_ERRORTYPE allocateBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_U32 nSizeBytes, 
                OMX_U8 **ppData, 
                void **bufferAllocInfo, 
                void **portPrivateInfo);

        ENS_API_IMPORT virtual OMX_ERRORTYPE useBufferHeader(
                OMX_DIRTYPE dir,
                OMX_BUFFERHEADERTYPE* pBuffer);

        ENS_API_IMPORT virtual OMX_ERRORTYPE useBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_BUFFERHEADERTYPE* pBufferHdr,
                void **portPrivateInfo);

        ENS_API_IMPORT virtual OMX_ERRORTYPE freeBuffer(
				OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_BOOL bBufferAllocated, 
                void *bufferAllocInfo,
                void *portPrivateInfo);

        ENS_API_IMPORT virtual void fsmInit(fsmInit_t * initFsm);

        /// Get the component's fsmInit interface
        NMF::InterfaceReference * getNmfFsmInitItf(void) const {
	  return  (NMF::InterfaceReference *) &mIfsmInit; }

        /// Get the component's sendcommand interface
        NMF::InterfaceReference * getNmfSendCommandItf(void) const {
            return  (NMF::InterfaceReference *) &mIsendCommand; }

        /// Get the NMF components priority level
        ENS_API_IMPORT OMX_U32 getPriorityLevel(void) const;

        /// Get the NMF handle of the shared buffer (used for standard tunneling
        /// or non-tunneled communication
        ENS_API_IMPORT t_cm_instance_handle getNmfSharedBuf(OMX_U32 portIdx) const;

        eventhandlerDescriptor * getEventHandlerCB() { return (eventhandlerDescriptor *)this; }
  
        t_cm_instance_handle getOstTrace(void) const {
            return mOstTrace;
        }

        friend class NmfMpc_ProcessingComponentCB;
    protected:

        t_cm_instance_handle        mNmfGenericFsmLib;
        t_cm_instance_handle        mNmfComponentFsmLib;
		t_cm_instance_handle        mOstTrace;

        // NMF interfaces
        Isendcommand    mIsendCommand;
        Ifsminit	mIfsmInit;
        
        Ishared_emptythisbuffer *   mIemptyThisBuffer;
        Ishared_fillthisbuffer *    mIfillThisBuffer;

        //----------------------------------------------------------------
        /// Following methods must be implemented by derived classes.
        //----------------------------------------------------------------
        
        virtual OMX_ERRORTYPE instantiate(void) = 0;
        virtual OMX_ERRORTYPE deInstantiate(void) = 0;
        virtual OMX_ERRORTYPE start(void) = 0;
        virtual OMX_ERRORTYPE stop(void) = 0;
        virtual OMX_ERRORTYPE configure(void) = 0;

        /// Following methods allow to allocate/deallocate buffers for standard 
        /// tunneling or non-tunneled communication
        virtual OMX_ERRORTYPE doBufferAllocation(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, 
                OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo) = 0;

        virtual OMX_ERRORTYPE doBufferDeAllocation(OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, void *bufferAllocInfo) = 0;

        virtual OMX_U32 getBufferPhysicalAddress(void *bufferAllocInfo, OMX_U8 *,OMX_U32) = 0;
        virtual OMX_U32 getBufferMpcAddress(void *bufferAllocInfo) = 0;

        /// This method is called inside eventHandler_cb to allow derived classes to do
        /// some specific stuffs.
        virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2,OMX_BOOL & deferEventHandler) = 0;

        /// These methods are called inside emptyBufferDone_cb/fillBufferDone_cb to allow 
        /// derived classes to update OMX buffer before returning it to ILClient in case of 
        /// non-tunneled communication or tunneled component in case of standard tunneling
        virtual void doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer) = 0;
        virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer) = 0;

        virtual bool useBufferNeedsMemcpy(void) const { return false; }

        /// This method is called inside sendCommand to allow derived classes to do
        /// some specific stuffs before calling the NMFCALL.
        /// The bDefereedCmd return value allows to perform (or not) the NMFCALL.
        virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd) = 0;

        // NMF callbacks, methods inherited from eventhandlerDescriptor
        ENS_API_IMPORT virtual void eventHandler(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2);

        ENS_API_IMPORT static SharedBuffer * getSharedBuffer(t_uint32 dspBufferHdrArmAddress, bool enableCopy);
        
        void emptyBufferDone_cb(t_uint32 dspBufferHdrArmAddress); 
        void fillBufferDone_cb(t_uint32 dspBufferHdrArmAddress);
        
        ENS_API_IMPORT void setPriorityLevel(OMX_U32 priority);
        
    private:

        t_cm_instance_handle *      mNmfSharedBuf;


        OMX_U32                     mMpcTracingMode;
        OMX_U32                     mMyPriority;

        NmfMpc_ProcessingComponentCB  callbackHandler ;
  
        OMX_ERRORTYPE instantiateNmfSharedBuffers(ENS_Port *port);
        OMX_ERRORTYPE deInstantiateNmfSharedBuffers(ENS_Port *port);
        OMX_ERRORTYPE startNmfSharedBuf(OMX_U32 nPortIdx);
        OMX_ERRORTYPE stopNmfSharedBuf(OMX_U32 nPortIdx);

        OMX_ERRORTYPE instantiateLibraries(void);
        OMX_ERRORTYPE deInstantiateLibraries(void);
        OMX_ERRORTYPE startLibraries(void);
        OMX_ERRORTYPE stopLibraries(void);

        OMX_ERRORTYPE allocateInterfaces(void);
        void freeInterfaces(void);

        OMX_ERRORTYPE sendEnablePortCommand(OMX_U32 portIndex, OMX_BOOL &bDeferredCmd);
        void disablePortIndication(OMX_U32 portIndex, OMX_BOOL &deferEventHandler);
        void configureTraceModeMpc(OMX_U32 nTraceMode);


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

#endif // _NMF_PROCESSING_COMPONENT_H_

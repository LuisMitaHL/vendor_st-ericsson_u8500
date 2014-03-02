/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfHost_ProcessingComponent.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _HOST_NMF_PROCESSING_COMPONENT_H_
#define _HOST_NMF_PROCESSING_COMPONENT_H_

#include "ENS_Redefine_Class.h"

#include "ProcessingComponent.h"

#include "host/sendcommand.hpp"
#include "host/armnmf_emptythisbuffer.hpp"
#include "host/armnmf_fillthisbuffer.hpp"
#include "host/eventhandler.hpp"
#include "host/fsminit.hpp"
#include "osi_trace.h"

#include <cpp.hpp>
#include <inc/typedef.h>

extern const char * OMX_EVENTTYPE_NAME[];

extern const char* OMX_STATETYPE_NAME[];

extern const char* OMX_COMMANDTYPE_NAME[];

#define STRING(x) #x
#ifdef OMX_TRACE_UID
#undef OMX_TRACE_UID
#endif

#define OMX_TRACE_UID (1<<6)

#ifdef OMXCOMPONENT
#undef OMXCOMPONENT
#endif

#define OMXCOMPONENT "HOST_NMF_PROCESSING_COMPONENT"


class NmfHost_ProcessingComponentCB : public armnmf_emptythisbufferDescriptor, public armnmf_fillthisbufferDescriptor {
public :	
	void setProcessingComponent(ENS_Component * enscomp) { mENSComponent = enscomp ;}
	
	// methods inherited from armnmf_emptythisbufferDescriptor
	ENS_API_IMPORT virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) ;
	// methods inherited from armnmf_fillthisbufferDescriptor
	ENS_API_IMPORT virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) ;
	
private :
	ENS_Component * mENSComponent;	
} ;


class NmfHost_ProcessingComponent: public ProcessingComponent , public eventhandlerDescriptor {


    public:

        ENS_API_IMPORT NmfHost_ProcessingComponent(ENS_Component & enscomp) ;

        ENS_API_IMPORT virtual OMX_ERRORTYPE construct();
        ENS_API_IMPORT virtual OMX_ERRORTYPE destroy();

        ENS_API_IMPORT virtual OMX_ERRORTYPE sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData);

        ENS_API_IMPORT virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer); 

        ENS_API_IMPORT virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

	    virtual Iarmnmf_emptythisbuffer *  getEmptyThisBufferInterface(OMX_U32 portIdx) { return &mIemptyThisBuffer[portIdx]; }
	    virtual Iarmnmf_fillthisbuffer  *  getFillThisBufferInterface(OMX_U32 portIdx) { return &mIfillThisBuffer[portIdx]; }
	    virtual Isendcommand *      getNmfSendCommandItf(void)  { return &mIsendCommand; }
		
		armnmf_fillthisbufferDescriptor * getEmptyBufferDoneCB() { return (armnmf_fillthisbufferDescriptor *)&callbackHandler; }
		armnmf_emptythisbufferDescriptor *  getFillBufferDoneCB()  { return (armnmf_emptythisbufferDescriptor *)&callbackHandler; } 
		eventhandlerDescriptor *    getEventHandlerCB()    { return (eventhandlerDescriptor *)this; }

        /// Get the component's fsmInit interface
        NMF::InterfaceReference * getNmfFsmInitItf(void) const { return  (NMF::InterfaceReference *) &mIfsmInit; }
        
        /// These methods are called inside emptyBufferDone_cb/fillBufferDone_cb to allow 
        /// derived classes to update OMX buffer before returning it to ILClient in case of 
        /// non-tunneled communication or tunneled component in case of standard tunneling
        ENS_API_IMPORT virtual void doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
        ENS_API_IMPORT virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);


    protected:
        ENS_API_IMPORT virtual OMX_ERRORTYPE allocateBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_U32 nSizeBytes, 
                OMX_U8 **ppData, 
                void **bufferAllocInfo, 
                void **portPrivateInfo)=0;

        ENS_API_IMPORT virtual OMX_ERRORTYPE useBufferHeader(
                OMX_DIRTYPE dir,
                OMX_BUFFERHEADERTYPE* pBuffer)=0;

        ENS_API_IMPORT virtual OMX_ERRORTYPE useBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_BUFFERHEADERTYPE* pBufferHdr,
                void **portPrivateInfo)=0;

        ENS_API_IMPORT virtual OMX_ERRORTYPE freeBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_BOOL bBufferAllocated, 
                void *bufferAllocInfo,
                void *portPrivateInfo)=0;

        ENS_API_IMPORT virtual OMX_ERRORTYPE allocateBufferHeap(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_U32 nSizeBytes, 
                OMX_U8 **ppData, 
                void **bufferAllocInfo, 
                void **portPrivateInfo);

        ENS_API_IMPORT virtual OMX_ERRORTYPE useBufferHeaderHeap(
                OMX_DIRTYPE dir,
                OMX_BUFFERHEADERTYPE* pBuffer);

        ENS_API_IMPORT virtual OMX_ERRORTYPE useBufferHeap(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_BUFFERHEADERTYPE* pBufferHdr,
                void **portPrivateInfo);

        ENS_API_IMPORT virtual OMX_ERRORTYPE freeBufferHeap(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_BOOL bBufferAllocated, 
                void *bufferAllocInfo,
                void *portPrivateInfo);

        ENS_API_IMPORT virtual OMX_ERRORTYPE allocateBufferVisual(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_U32 nSizeBytes, 
                OMX_U8 **ppData, 
                void **bufferAllocInfo, 
                void **portPrivateInfo,
                OMX_BOOL isCached=OMX_FALSE);

        ENS_API_IMPORT virtual OMX_ERRORTYPE useBufferHeaderVisual(
                OMX_DIRTYPE dir,
                OMX_BUFFERHEADERTYPE* pBuffer);

        ENS_API_IMPORT virtual OMX_ERRORTYPE useBufferVisual(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_BUFFERHEADERTYPE* pBufferHdr,
                void **portPrivateInfo);

        ENS_API_IMPORT virtual OMX_ERRORTYPE freeBufferVisual(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex, 
                OMX_BOOL bBufferAllocated, 
                void *bufferAllocInfo,
                void *portPrivateInfo);
        
	// methods inherited from eventhandlerDescriptor
	ENS_API_IMPORT virtual void eventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2) ;
	
        /// This method is called inside eventHandler_cb to allow derived classes to do
        /// some specific stuffs.
	ENS_API_IMPORT virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferEventHandler) = 0;

        /// This method is called inside sendCommand to allow derived classes to do
        /// some specific stuffs before calling the NMF interface
        /// The bDefereedCmd return value allows to perform (or not) the NMF interface
        ENS_API_IMPORT virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd) = 0;

        virtual OMX_ERRORTYPE instantiate() = 0;
        virtual OMX_ERRORTYPE deInstantiate() = 0;
        virtual OMX_ERRORTYPE start() = 0;
        virtual OMX_ERRORTYPE stop() = 0;
        virtual OMX_ERRORTYPE configure() = 0;

#define MAX_PORTS_COUNT 10
        // NMF interfaces
	      Isendcommand      mIsendCommand;
        Ifsminit          mIfsmInit;
        Iarmnmf_emptythisbuffer  mIemptyThisBuffer[MAX_PORTS_COUNT];
        Iarmnmf_fillthisbuffer   mIfillThisBuffer[MAX_PORTS_COUNT];

        NmfHost_ProcessingComponentCB  callbackHandler ;

        ENS_API_IMPORT virtual OMX_ERRORTYPE sendEnablePortCommand(OMX_U32 portIndex, OMX_BOOL &bDeferredCmd);
        ENS_API_IMPORT virtual void disablePortIndication(OMX_U32 portIndex, OMX_BOOL &deferEventHandler);

		//----------------------------------------------------------------//
		//		Buffer sharing methods & data members
		//----------------------------------------------------------------//

	
	private:

		ENS_API_IMPORT virtual OMX_ERRORTYPE getMMHWBufferInfo(OMX_U32 nPortIndex,
													   OMX_U32 nSizeBytes, 
													   OMX_U8 *pData, 
														void **bufferAllocInfo,
														void **portPrivateInfo);
	
		ENS_API_IMPORT virtual OMX_ERRORTYPE freeSharedBuffer(
				OMX_U32 nPortIndex,
				OMX_U32 nBufferIndex,
				OMX_BOOL bBufferAllocated,
				void *bufferAllocInfo,
				void *portPrivateInfo) ;


};

#endif // _HOST_NMF_PROCESSING_COMPONENT_H_

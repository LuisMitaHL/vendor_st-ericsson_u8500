/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

/**
* \brief   Implementation of Img_EnsWrapper ensuring correct execution model
*          of wrapped components
*/

#if 0
#ifdef __linux
	#include <pthread.h>
	#include <signal.h>
#else
#endif
#include <stdio.h>
#include <stdlib.h>
#define LOG_TAG "Img_EnsWrapper"

#include "ENS_IOMX.h"
#include "BSD_list.h"
#include "osal_semaphore.h"
#include "osal_mutex.h"
#include "osal_thread.h"

// Rme defines
#include "rme_types.h"
#include "rme.h"

//Omx definitions
#include "OMX_Component.h"

// ENS
#include "ENS_Component.h"

#include "Img_EnsWrapper_Shared.h"
#include "Img_EnsQueue.h"
#include "Img_EnsWrapper_Rme.h"
#include "Img_EnsWrapper_Construct.h"

#include "osttrace.h"

using namespace rme;

#include "Img_EnsWrapper.h"
#include "Img_EnsWrapper_services.h"
#include "Img_EnsWrapper_Log.h"


// This class is not used for the moment

class GlobalWrapper
//********************************************************************************
{
public:
	/**
	* The OMX components interfaces the OMX IL client will call
	* The wrapper will return the following functions as component entry point
	*/
	static OMX_ERRORTYPE GetComponentVersion(OMX_HANDLETYPE hComponent,
		OMX_STRING pComponentName,
		OMX_VERSIONTYPE* pComponentVersion,
		OMX_VERSIONTYPE* pSpecVersion,
		OMX_UUIDTYPE* pComponentUUID);

	static OMX_ERRORTYPE SendCommand(OMX_HANDLETYPE hComponent,
		OMX_COMMANDTYPE Cmd,
		OMX_U32 nParam,
		OMX_PTR pCmdData);

	static OMX_ERRORTYPE GetParameter(OMX_HANDLETYPE hComponent,
		OMX_INDEXTYPE nIndex,
		OMX_PTR pComponentParameterStructure);

	static OMX_ERRORTYPE SetParameter(OMX_HANDLETYPE hComponent,
		OMX_INDEXTYPE nIndex,
		OMX_PTR pComponentParameterStructure);

	static OMX_ERRORTYPE GetConfig(OMX_HANDLETYPE hComponent,
		OMX_INDEXTYPE nIndex,
		OMX_PTR pComponentConfigStructure);

	static OMX_ERRORTYPE SetConfig(OMX_HANDLETYPE hComponent,
		OMX_INDEXTYPE nIndex,
		OMX_PTR pComponentConfigStructure);

	static OMX_ERRORTYPE GetExtensionIndex(OMX_HANDLETYPE hComponent,
		OMX_STRING cParameterName,
		OMX_INDEXTYPE* pIndexType);

	static  OMX_ERRORTYPE GetState(OMX_HANDLETYPE hComponent,
		OMX_STATETYPE* pState);

	static  OMX_ERRORTYPE ComponentTunnelRequest(OMX_HANDLETYPE hComponent,
		OMX_U32 nPort,
		OMX_HANDLETYPE hTunneledComp,
		OMX_U32 nTunneledPort,
		OMX_TUNNELSETUPTYPE* pTunnelSetup);

	static  OMX_ERRORTYPE UseBuffer(OMX_HANDLETYPE hComponent,
		OMX_BUFFERHEADERTYPE** ppBufferHdr,
		OMX_U32 nPortIndex,
		OMX_PTR pAppPrivate,
		OMX_U32 nSizeBytes,
		OMX_U8* pBuffer);

	static  OMX_ERRORTYPE AllocateBuffer(OMX_HANDLETYPE hComponent,
		OMX_BUFFERHEADERTYPE** ppBufferHdr,
		OMX_U32 nPortIndex,
		OMX_PTR pAppPrivate,
		OMX_U32 nSizeBytes);

	static  OMX_ERRORTYPE FreeBuffer(OMX_HANDLETYPE hComponent,
		OMX_U32 nPortIndex,
		OMX_BUFFERHEADERTYPE* pBuffer);

	static  OMX_ERRORTYPE EmptyThisBuffer(OMX_HANDLETYPE hComponent,
		OMX_BUFFERHEADERTYPE* pBuffer);

	static  OMX_ERRORTYPE FillThisBuffer(OMX_HANDLETYPE hComponent,
		OMX_BUFFERHEADERTYPE* pBuffer);

	static  OMX_ERRORTYPE SetCallbacks(OMX_HANDLETYPE hComponent,
		OMX_CALLBACKTYPE* pCallbacks,
		OMX_PTR pAppData);

	static  OMX_ERRORTYPE ComponentDeInit(OMX_HANDLETYPE hComponent);

	static  OMX_ERRORTYPE UseEGLImage(OMX_HANDLETYPE hComponent,
		OMX_BUFFERHEADERTYPE** ppBufferHdr,
		OMX_U32 nPortIndex,
		OMX_PTR pAppPrivate,
		void* eglImage);

	static  OMX_ERRORTYPE ComponentRoleEnum(OMX_HANDLETYPE hComponent,
		OMX_U8 *cRole,
		OMX_U32 nIndex);

	/**
	* The OMX IL client call backs passed to the OSI components
	*/

	static  OMX_ERRORTYPE EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
		OMX_IN OMX_PTR pAppData,
		OMX_IN OMX_EVENTTYPE eEvent,
		OMX_IN OMX_U32 nData1,
		OMX_IN OMX_U32 nData2,
		OMX_IN OMX_PTR pEventData);

	static  OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
		OMX_IN OMX_PTR pAppData,
		OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

	static  OMX_ERRORTYPE FillBufferDone(OMX_OUT OMX_HANDLETYPE hComponent,
		OMX_OUT OMX_PTR pAppData,
		OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);
protected:
};

/********************************************************************************
* Wrapper funtions being called by client to then call ENS code
********************************************************************************/

// SetCallbacks wrapper
OMX_ERRORTYPE GlobalWrapper::SetCallbacks(
	OMX_HANDLETYPE hComponent,
	OMX_CALLBACKTYPE* pCallbacks,
	OMX_PTR pAppData)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	// We register those new callbacks to the wrapper
	wrapper->mOMXClientCB = pCallbacks;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("SetCallbacks %s hComponent=0x%08x, pCallbacks=0x%08x, pAppData=0x%08x\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent, (unsigned int) pCallbacks, (unsigned int) pAppData);

	// and call the component to update pAppData with wrapper's CB
	OMX_ERRORTYPE result =
		wrapper->m_pEnsComponent->SetCallbacks(wrapper->mOMXWrapperCB, pAppData);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - SetCallbacks hComponent=0x%08x\n",
		strOMX_Error(result, result_str), (unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// ComponentDeInit wrapper
OMX_ERRORTYPE GlobalWrapper::ComponentDeInit(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_OMX_LOG("FreeHandle %s hComponent=0x%08x\n", (const char*) wrapper->m_pEnsComponent->getName(), (unsigned int) hComponent);

	// Wrapper lock is to protect ENS proxy code and not CM -> otherwise dead lock is possible
	IMGWRAPPER_LOCK(omx_comp);

	// and call the component to update pAppData with wrapper's CB
	OMX_ERRORTYPE result =
		wrapper->m_pEnsComponent->ComponentDeInit();

	// Upon ComponentDeInit ENSComponent object is ready to be deleted
	delete wrapper->m_pEnsComponent;
	wrapper->m_pEnsComponent = NULL;

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - FreeHandle hComponent=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent);

	// We update trace level on every component creation/destruction in case property has been set
	// after start of session (lock must be held)
	updateTraceLevel(omx_comp);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// GetComponentVersion wrapper
OMX_ERRORTYPE GlobalWrapper::GetComponentVersion(OMX_HANDLETYPE hComponent,
	OMX_STRING       pComponentName,
	OMX_VERSIONTYPE* pComponentVersion,
	OMX_VERSIONTYPE* pSpecVersion,
	OMX_UUIDTYPE   * pComponentUUID)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("GetComponentVersion %s hComponent=0x%08x\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->GetComponentVersion(pComponentName, pComponentVersion, pSpecVersion, pComponentUUID);

	char bufferCUUID[128*2+5];

	if(GlobalWrapper::OMX_trace_level)
	{
		if(pComponentUUID)
		{
			bufferCUUID[0] = '\0';
			char * tmpi = bufferCUUID;
			int index = 0;
			while(index < 128)
			{
				if((index != 0) && (index%32 == 0))
				{
					*(tmpi++) = '\0';
				}
				sprintf(tmpi, "%02x", (unsigned int) pComponentUUID[index++]);
				tmpi += 2;
			}
		}

		char result_str[128];
		LOGD("return %s - GetComponentVersion : hComponent=0x%08x pComponentName=%s"
			"pComponentVersion=0x%08x pSpecVersion=0x%08x pComponentUUID=0x%08x\n",
			strOMX_Error(result, result_str),
			(unsigned int) hComponent, pComponentName,
			(unsigned int) pComponentVersion, (unsigned int) pSpecVersion,
			(unsigned int) pComponentUUID);

		if(pComponentVersion)
		{
			LOGD("              + ComponentVersion.nVersionMajor=%d\n",
				(int) pComponentVersion->s.nVersionMajor);
			LOGD("              + ComponentVersion.nVersionMinor=%d\n",
				(int) pComponentVersion->s.nVersionMinor);
			LOGD("              + ComponentVersion.nRevision=%d\n",
				(int) pComponentVersion->s.nRevision);
			LOGD("              + ComponentVersion.nStep=%d\n",
				(int) pComponentVersion->s.nStep);
		}
		if(pSpecVersion)
		{
			LOGD("              + SpecVersion.nVersionMajor=%d\n",
				(int) pSpecVersion->s.nVersionMajor);
			LOGD("              + SpecVersion.nVersionMinor=%d\n",
				(int) pSpecVersion->s.nVersionMinor);
			LOGD("              + SpecVersion.nRevision=%d\n",
				(int) pSpecVersion->s.nRevision);
			LOGD("              + SpecVersion.nStep=%d\n",
				(int) pSpecVersion->s.nStep);
		}
		if(pComponentUUID)
		{
			LOGD("              + ComponentUUID[ 0...31]=%s\n", bufferCUUID);
			LOGD("              + ComponentUUID[32...63]=%s\n", bufferCUUID+64+1);
			LOGD("              + ComponentUUID[64...95]=%s\n", bufferCUUID+128+2);
			LOGD("              + ComponentUUID[96..127]=%s\n", bufferCUUID+192+3);
		}
	}

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// SendCommand wrapper
OMX_ERRORTYPE GlobalWrapper::SendCommand(OMX_HANDLETYPE hComponent,
	OMX_COMMANDTYPE Cmd,
	OMX_U32         nParam,
	OMX_PTR         pCmdData)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

#ifdef EXECUTE_COMMANDS_SEQUENTIALLY

	OSAL_MutexLock(wrapper->mWrapperMutex_id);

	if (OMX_TRUE == wrapper->mCmdBeingProcessed) {
		wrapper->mCommandWaiting++;
		OSAL_MutexUnlock(wrapper->mWrapperMutex_id);

		IMGWRAPPER_OMX_LOG("EXECUTE_COMMANDS_SEQUENTIALLY: holding incomming SendCommand "
			"request on %s hComponent=0x%08x\n",
			(const char*) wrapper->m_pEnsComponent->getName(),
			(unsigned int) hComponent);

		OSAL_SemaphoreWait(wrapper->m_Semaphore_waitingForCmdToComplete);

		IMGWRAPPER_OMX_LOG("EXECUTE_COMMANDS_SEQUENTIALLY: released SendCommand request "
			"on %s hComponent=0x%08x\n",
			(const char*) wrapper->m_pEnsComponent->getName(),
			(unsigned int) hComponent);

		OSAL_MutexLock(wrapper->mWrapperMutex_id);
		wrapper->mCommandWaiting--;
		OSAL_MutexUnlock(wrapper->mWrapperMutex_id);
	} else {
		if (Cmd != OMX_CommandPortEnable) {
			wrapper->mCmdBeingProcessed = OMX_TRUE;
		}
		OSAL_MutexUnlock(wrapper->mWrapperMutex_id);
	}

#endif // #ifdef EXECUTE_COMMANDS_SEQUENTIALLY

	IMGWRAPPER_LOCK(omx_comp);

	displaySendCommand((const char*) wrapper->m_pEnsComponent->getName(), hComponent, Cmd, nParam,  pCmdData);

	wrapper->mNbCmdReceivedToWait =
		wrapper->m_pEnsComponent->getNbCmdReceivedEventsToWait(Cmd, nParam);

	// Use local value for nbCmdReceivedtoWait and not wrapper->mNbCmdReceivedToWait
	// as we must wait on semaphore even if notified first
	unsigned int nbCmdReceivedtoWait =  wrapper->mNbCmdReceivedToWait;

	IMGWRAPPER_OMX_LOG("nbCmdReceivedtoWait=%d: SendCommand "
		"request on %s hComponent=%#08x\n", nbCmdReceivedtoWait,
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->SendCommand(Cmd, nParam, pCmdData);

#ifdef EXECUTE_COMMANDS_SEQUENTIALLY

	if (OMX_ErrorNone != result)
	{
		// In case SendCommand has failed then no call back will be returned so we
		// must release next commands here
		OSAL_MutexLock(wrapper->mWrapperMutex_id);
		if (wrapper->mCommandWaiting != 0) {
			// We have a SendCommand on hold : release held lock and notify to unblock thread
			OSAL_MutexUnlock(wrapper->mWrapperMutex_id);
			OSAL_SemaphoreNotify(wrapper->m_Semaphore_waitingForCmdToComplete);
		} else {
			// no waiting thread, just reset property
			wrapper->mCmdBeingProcessed = OMX_FALSE;
			OSAL_MutexUnlock(wrapper->mWrapperMutex_id);
		}
	}

#endif // #ifdef EXECUTE_COMMANDS_SEQUENTIALLY

	/* To ensure correct execution order from client of SendCommand to multiple components
	to change to a state needing information exchange between components the ENS layer
	sends a CmdReceived event to release the client thread */
	if (result == OMX_ErrorNone)
	{
		IMGWRAPPER_UNLOCK(omx_comp);
		if(nbCmdReceivedtoWait != 0)
		{
			IMGWRAPPER_OMX_LOG("hComponent=%#08x - released lock, "
				"awaiting %d EventCmdReceived within 4s",
				(unsigned int) hComponent, nbCmdReceivedtoWait);
			// Expect nbCmdReceivedtoWait back from ENS within 4s
			if (OSAL_SemaphoreWaitTimeOut(wrapper->m_Semaphore_WaitingForCmdReceived, 4000))
			{
				LOGE("hComponent=%#08x - 4s timeout expired - " "missing %d EventCmdReceived", (unsigned int) hComponent, wrapper->mNbCmdReceivedToWait);
				PrintComponentList();
				result = OMX_ErrorTimeout;
			}
		}
	} else {
		wrapper->mNbCmdReceivedToWait = 0;
		IMGWRAPPER_UNLOCK(omx_comp);
	}

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - SendCommand %s hComponent=%#08x\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		strOMX_Error(result, result_str),
		(unsigned int) hComponent);

	return result;
}

// GetParameter wrapper
OMX_ERRORTYPE GlobalWrapper::GetParameter(OMX_HANDLETYPE hComponent,
	OMX_INDEXTYPE nIndex,
	OMX_PTR pComponentParameterStructure)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("GetParameter %s hComponent=0x%08x\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->GetParameter(nIndex,
		pComponentParameterStructure);

	displayParameterAndConfig(nIndex, pComponentParameterStructure);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - GetParameter hComponent=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// SetParameter wrapper
OMX_ERRORTYPE GlobalWrapper::SetParameter(OMX_HANDLETYPE hComponent,
	OMX_INDEXTYPE nIndex,
	OMX_PTR       pComponentParameterStructure)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("SetParameter %s hComponent=0x%08x\n", (const char*) wrapper->m_pEnsComponent->getName(), (unsigned int) hComponent);

	displayParameterAndConfig(nIndex, pComponentParameterStructure);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->SetParameter(nIndex, pComponentParameterStructure);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - SetParameter hComponent=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// GetConfig wrapper
OMX_ERRORTYPE GlobalWrapper::GetConfig(OMX_HANDLETYPE hComponent,
	OMX_INDEXTYPE nIndex,
	OMX_PTR       pComponentConfigStructure)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("GetConfig %s hComponent=0x%08x\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->GetConfig(nIndex,
		pComponentConfigStructure);

	displayParameterAndConfig(nIndex, pComponentConfigStructure);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - GetConfig hComponent=0x%08x\n",
		strOMX_Error(result, result_str),
		(unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// SetConfig wrapper
OMX_ERRORTYPE GlobalWrapper::SetConfig(OMX_HANDLETYPE hComponent,
	OMX_INDEXTYPE nIndex,
	OMX_PTR pComponentConfigStructure)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("SetConfig %s hComponent=0x%08x\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent);

	displayParameterAndConfig(nIndex, pComponentConfigStructure);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->SetConfig(nIndex,
		pComponentConfigStructure);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - SetConfig hComponent=0x%08x\n",
		strOMX_Error(result, result_str),
		(unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// GetExtensionIndex wrapper
OMX_ERRORTYPE GlobalWrapper::GetExtensionIndex(OMX_HANDLETYPE hComponent,
	OMX_STRING cParameterName,
	OMX_INDEXTYPE* pIndexType)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("GetExtensionIndex %s hComponent=0x%08x cParameterName=%s\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent, cParameterName);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->GetExtensionIndex(cParameterName,
		pIndexType);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - GetExtensionIndex hComponent=0x%08x IndexType=0x%08x\n",
		strOMX_Error(result, result_str),
		(unsigned int) hComponent, (pIndexType ? (unsigned int) *pIndexType : 0));

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// GetState wrapper
OMX_ERRORTYPE GlobalWrapper::GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE* pState)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("GetState %s hComponent=0x%08x\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->GetState(pState);

	char buffer[1024];
	buffer[0] = '\0';

	if(GlobalWrapper::OMX_trace_level)
	{
		if(pState)
			strcatOMX_STATETYPE(buffer, *pState);
		char result_str[128];
		IMGWRAPPER_OMX_LOG("return %s - GetState hComponent=0x%08x pState=%s\n",
			strOMX_Error(result, result_str),
			(unsigned int) hComponent, buffer);
	}

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// ComponentTunnelRequest wrapper
OMX_ERRORTYPE GlobalWrapper::ComponentTunnelRequest(OMX_HANDLETYPE hComponent,
	OMX_U32              nPort,
	OMX_HANDLETYPE       hTunneledComp,
	OMX_U32              nTunneledPort,
	OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	char buffer[1024];

	if(GlobalWrapper::OMX_trace_level)
	{
		buffer[0] = '\0';
		strcatOMX_TUNNELSETUPTYPE(buffer, pTunnelSetup);
	}

	IMGWRAPPER_OMX_LOG("ComponentTunnelRequest: %s hComponent=0x%08x nPort=%d "
		"hTunneledComp=0x%08x pTunneledPort=%d %s\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent, (int) nPort,
		(unsigned int) hTunneledComp, (int) nTunneledPort, buffer);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->ComponentTunnelRequest(nPort,
		hTunneledComp,
		nTunneledPort,
		pTunnelSetup);
	if(GlobalWrapper::OMX_trace_level)
	{
		buffer[0] = '\0';
		strcatOMX_TUNNELSETUPTYPE(buffer, pTunnelSetup);
	}

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - ComponentTunnelRequest: hComponent=0x%08x nPort=%d "
		"hTunneledComp=0x%08x pTunneledPort=%d %s\n",
		strOMX_Error(result, result_str),
		(unsigned int) hComponent, (int) nPort,
		(unsigned int) hTunneledComp, (int) nTunneledPort, buffer);


	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// UseBuffer wrapper
OMX_ERRORTYPE GlobalWrapper::UseBuffer(OMX_HANDLETYPE hComponent,
	OMX_BUFFERHEADERTYPE** ppBufferHdr,
	OMX_U32 nPortIndex,
	OMX_PTR pAppPrivate,
	OMX_U32 nSizeBytes,
	OMX_U8* pBuffer)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper    = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("UseBuffer %s hComponent=0x%08x nPortIndex=%d pAppPrivate=0x%08x"
		" nSizeBytes=%d pBuffer=0x%08x\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent, (int) nPortIndex, (unsigned int) pAppPrivate,
		(int) nSizeBytes, (unsigned int) pBuffer);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->UseBuffer(ppBufferHdr,
		nPortIndex, pAppPrivate, nSizeBytes, pBuffer);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - UseBuffer: hComponent=0x%08x pBufferHdr=0x%08x\n",
		strOMX_Error(result, result_str),
		(unsigned int) hComponent, (unsigned int) (ppBufferHdr ? *ppBufferHdr : 0));

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


//AllocateBuffer wrapper
OMX_ERRORTYPE GlobalWrapper::AllocateBuffer(OMX_HANDLETYPE hComponent,
	OMX_BUFFERHEADERTYPE** ppBufferHdr,
	OMX_U32 nPortIndex,
	OMX_PTR pAppPrivate,
	OMX_U32 nSizeBytes)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("AllocateBuffer %s hComponent=0x%08x nPortIndex=%d pAppPrivate=0x%08x nSizeBytes=%d\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent, (int) nPortIndex, (unsigned int) pAppPrivate,
		(int) nSizeBytes);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->AllocateBuffer(ppBufferHdr, nPortIndex,
		pAppPrivate, nSizeBytes);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - AllocateBuffer: hComponent=0x%08x pBufferHdr=0x%08x\n",
		strOMX_Error(result, result_str),
		(unsigned int) hComponent, (unsigned int) (ppBufferHdr ? *ppBufferHdr : 0));

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// FreeBuffer wrapper
OMX_ERRORTYPE GlobalWrapper::FreeBuffer(OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper    = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("FreeBuffer %s hComponent=0x%08x nPortIndex=%d pBuffer=0x%08x\n", (const char*) wrapper->m_pEnsComponent->getName(), (unsigned int) hComponent, (int) nPortIndex, (int) pBuffer);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->FreeBuffer(nPortIndex, pBuffer);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - FreeBuffer: hComponent=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// EmptyThisBuffer wrapper
OMX_ERRORTYPE GlobalWrapper::EmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper    = (GlobalWrapper *) omx_comp->pComponentPrivate;
	char header_buffer[1024];
	int dump_buffer_header = 0;

	IMGWRAPPER_LOCK(omx_comp);

#ifdef FORCE_OMX_BUFFERFLAG_STARTTIME_ON_FIRST_EMPTYTHISBUFFER
	// Thierry: this really is to be removed !!!
	if (wrapper->bFirstBufferSend == false)
	{
		pBuffer->nFlags |= OMX_BUFFERFLAG_STARTTIME;
		wrapper->bFirstBufferSend = true;
	}
#endif

	if (GlobalWrapper::OMX_trace_level >= OMX_LOG_LEVEL_BUFFER)
	{
		LOGD("EmptyThisBuffer %s hComponent=0x%08x pBuffer=0x%08x\n", (const char*) wrapper->m_pEnsComponent->getName(), (unsigned int) hComponent, (unsigned int)  pBuffer);
		dumpOMX_BUFFERHEADERTYPE(pBuffer, header_buffer);
		dump_buffer_header = 1;
	}

	if (OMX_dump_level && (OMX_dump_output != 1))
	{
		if (strcmp((const char*) wrapper->m_pEnsComponent->getName(), OMX_dump_name) == 0)
		{
			if(!dump_buffer_header)
			{
				dumpOMX_BUFFERHEADERTYPE(pBuffer, header_buffer);
			}
			dumpOMX_BUFFERDATA(pBuffer, header_buffer);
			dump_buffer_header = 0;
		}
	}

	if(dump_buffer_header)
	{
		LOGD("%s", header_buffer);
	}

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->EmptyThisBuffer(pBuffer);

	if (GlobalWrapper::OMX_trace_level >= OMX_LOG_LEVEL_BUFFER)
	{
		char result_str[128];
		LOGD("return %s - EmptyThisBuffer hComponent=0x%08x pBuffer=0x%08x\n",
			strOMX_Error(result, result_str),
			(unsigned int) hComponent, (unsigned int) pBuffer);
	}

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// FillThisBuffer wrapper
OMX_ERRORTYPE GlobalWrapper::FillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	if (GlobalWrapper::OMX_trace_level >= OMX_LOG_LEVEL_BUFFER)
	{
		LOGD("FillThisBuffer %s hComponent=0x%08x pBuffer=0x%08x\n",
			(const char*) wrapper->m_pEnsComponent->getName(),
			(unsigned int) hComponent, (unsigned int) pBuffer);
	}

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->FillThisBuffer(pBuffer);

	if (GlobalWrapper::OMX_trace_level >= OMX_LOG_LEVEL_BUFFER) {
		char result_str[128];
		LOGD("return %s - FillThisBuffer hComponent=0x%08x pBuffer=0x%08x\n",
			strOMX_Error(result, result_str),
			(unsigned int) hComponent, (unsigned int) pBuffer);
	}

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// UseEGLImage wrapper
OMX_ERRORTYPE GlobalWrapper::UseEGLImage(OMX_HANDLETYPE hComponent,
	OMX_BUFFERHEADERTYPE** ppBufferHdr,
	OMX_U32 nPortIndex,
	OMX_PTR pAppPrivate,
	void* eglImage)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("UseEGLImage %s hComponent=0x%08x nPortIndex=%d pAppPrivate=0x%08x eglImage=0x%08x\n",
		(const char*)  wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent, (int) nPortIndex,
		(unsigned int) pAppPrivate, (unsigned int) eglImage);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->UseEGLImage(ppBufferHdr, nPortIndex,
		pAppPrivate, eglImage);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - UseEGLImage hComponent=0x%08x\n",
		strOMX_Error(result, result_str),
		(int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// ComponentRoleEnum wrapper
OMX_ERRORTYPE GlobalWrapper::ComponentRoleEnum(OMX_HANDLETYPE hComponent,
	OMX_U8* cRole,
	OMX_U32 nIndex)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("ComponentRoleEnum %s hComponent=0x%08x nIndex=%d\n",
		(const char*) wrapper->m_pEnsComponent->getName(),
		(unsigned int) hComponent, (int) nIndex);

	OMX_ERRORTYPE result = wrapper->m_pEnsComponent->ComponentRoleEnum(cRole, nIndex);


	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - ComponentRoleEnum hComponent=0x%08x Role=\"%s\"\n",
		strOMX_Error(result, result_str),
		(unsigned int) hComponent, (result == OMX_ErrorNone ? cRole: (OMX_U8*) ""));

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

#if 0
/********************************************************************************
* OMX IL callbacks
********************************************************************************/

CB_Message * GlobalWrapper::popPreAllocatedCBMessage()
{
	CB_Message * message = (CB_Message *) mClientCBfreeMessageQueue.Pop();
	IMG_DBC_ASSERT(message != NULL);
	return message;
}

void GlobalWrapper::pushPreAllocatedCBMessage(CB_Message * message)
{
	int result = mClientCBfreeMessageQueue.Push(message);
	if (result !=0)
	{
		IMG_DBC_ASSERT( result== 0);
	}
}

void GlobalWrapper::pushCBMessage(CB_Message * message, GlobalWrapper * wrapper)
{
	char buffer[1024];
	// Record the wrapper instance
	message->wrapper = wrapper;
	// Push CB message to wrapper's queue
	int result= mClientCBmessageQueue.Push(message);
	if (result !=0)
	{
		IMG_DBC_ASSERT( result== 0);
	}
	sprintCBMessage(buffer, message);
	IMGWRAPPER_DEBUG_LOG("pushCBMessage - thread_id=%d - on wrapper %d - pushed message (%d) %s\n",
		OSAL_GetCurrentThreadId(), (unsigned int) wrapper, (unsigned int) message, buffer);
	OSAL_SemaphoreNotify(m_Semaphore_waitingForClientCallbackProcessing);
}
#endif

OMX_ERRORTYPE GlobalWrapper::EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
	OMX_IN OMX_PTR pAppData,
	OMX_IN OMX_EVENTTYPE eEvent,
	OMX_IN OMX_U32 nData1,
	OMX_IN OMX_U32 nData2,
	OMX_IN OMX_PTR pEventData)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;
	CB_Message * message;

	message = wrapper->popPreAllocatedCBMessage();

	message->type = OMX_EVENTHANDLER;
	message->args.eventHandler.pAppData   = pAppData;
	message->args.eventHandler.eEvent     = eEvent;
	message->args.eventHandler.nData1     = nData1;
	message->args.eventHandler.nData2     = nData2;
	message->args.eventHandler.pEventData = pEventData;

	wrapper->pushCBMessage(message, wrapper);

	// We never return any error ...
	return OMX_ErrorNone;
}

OMX_ERRORTYPE GlobalWrapper::EmptyBufferDone(OMX_HANDLETYPE hComponent,
	OMX_PTR pAppData,
	OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;
	CB_Message * message = wrapper->popPreAllocatedCBMessage();

	message->type = OMX_EMPTYBUFFERDONE;
	message->args.bufferDone.pAppData = pAppData;
	message->args.bufferDone.pBuffer  = pBuffer;

	wrapper->pushCBMessage(message, wrapper);

	// We never return any error ...
	return OMX_ErrorNone;
}


OMX_ERRORTYPE GlobalWrapper::FillBufferDone(OMX_HANDLETYPE hComponent,
	OMX_PTR pAppData,
	OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	GlobalWrapper * wrapper = (GlobalWrapper *) omx_comp->pComponentPrivate;
	CB_Message * message = wrapper->popPreAllocatedCBMessage();

	message->type = OMX_FILLBUFFERDONE;
	message->args.bufferDone.pAppData = pAppData;
	message->args.bufferDone.pBuffer  = pBuffer;

	wrapper->pushCBMessage(message, wrapper);

	// We never return any error ...
	return OMX_ErrorNone;
}


#endif


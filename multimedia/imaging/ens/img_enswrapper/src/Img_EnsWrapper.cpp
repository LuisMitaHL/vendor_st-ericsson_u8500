/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
/**
* \brief   Implementation of Img_EnsWrapper ensuring correct execution model of wrapped components
*/
#include "Img_EnsWrapper_Shared.h"
#include <stdio.h>
#include <stdlib.h>

#define LOG_TAG "ImgEns_Wrapper"
#include "Img_EnsWrapper_Log.h"
#include "ImgEns_IOmx.h"
#include "BSD_list.h"
#include "osi_toolbox_lib.h"
#include "omx_toolbox_lib.h"

//Omx definitions
#include <omxil/OMX_Component.h>

// ENS
#include "ImgEns_Trace.h"
#include "ImgEns_Component.h"

#include "Img_EnsWrapper.h"
#include "Img_EnsWrapper_services.h"
#include "Img_EnsWrapper_Log.h"

#ifdef ANDROID
  #define FORCE_OMX_BUFFERFLAG_STARTTIME_ON_FIRST_EMPTYTHISBUFFER
#endif

#define OMX_LOG_LEVEL_BUFFER 3

// Some macro to ease the development
#define IMGWRAPPER_CHILD_LOCK(uid)     GetComponentManager().GlobalLock  (ExtendedMutex::eLock_Child , (const int) uid, __func__)
#define IMGWRAPPER_MASTER_LOCK(uid)    GetComponentManager().GlobalLock  (ExtendedMutex::eLock_Master, (const int) uid, __func__)
#define IMGWRAPPER_UNLOCK(uid)         GetComponentManager().GlobalUnlock((const int) uid, __func__)

// Just for debug purpose
IMG_ENSWRAPPER_API int Global_ImgWrapperVersion=GLOBAL_IMGWRAPPER_VERSION;

/// Global callbacks dedicated to wrapper
const OMX_CALLBACKTYPE g_OmxCallbacks_Wrapper ={Img_EnsWrapper::EventHandler, Img_EnsWrapper::EmptyBufferDone, Img_EnsWrapper::FillBufferDone};

//Global instance
static ComponentManager g_GlobalComponentManager;

ComponentManager &GetComponentManager()
//********************************************************************************
{ //Return the single instance 
	return(g_GlobalComponentManager);
}

int ComponentManager::Init_OMX_COMPONENTTYPE(OMX_COMPONENTTYPE * pOmxComponentType, void *pComponentPrivate) const
//********************************************************************************
{ //// Set all entry points for OMX
	if (pOmxComponentType)
	{
		// Store this wrapper instance
		pOmxComponentType->pComponentPrivate          = pComponentPrivate;
		// Set all OMX callback functions
		pOmxComponentType->GetComponentVersion        = Img_EnsWrapper::GetComponentVersion;
		pOmxComponentType->SendCommand                = Img_EnsWrapper::SendCommand;
		pOmxComponentType->GetParameter               = Img_EnsWrapper::GetParameter;
		pOmxComponentType->SetParameter               = Img_EnsWrapper::SetParameter;
		pOmxComponentType->GetConfig                  = Img_EnsWrapper::GetConfig;
		pOmxComponentType->SetConfig                  = Img_EnsWrapper::SetConfig;
		pOmxComponentType->GetExtensionIndex          = Img_EnsWrapper::GetExtensionIndex;
		pOmxComponentType->GetState                   = Img_EnsWrapper::GetState;

		pOmxComponentType->ComponentTunnelRequest     = Img_EnsWrapper::ComponentTunnelRequest;
		pOmxComponentType->UseBuffer                  = Img_EnsWrapper::UseBuffer;
		pOmxComponentType->AllocateBuffer             = Img_EnsWrapper::AllocateBuffer;
		pOmxComponentType->FreeBuffer                 = Img_EnsWrapper::FreeBuffer;
		pOmxComponentType->EmptyThisBuffer            = Img_EnsWrapper::EmptyThisBuffer;
		pOmxComponentType->FillThisBuffer             = Img_EnsWrapper::FillThisBuffer;

		pOmxComponentType->SetCallbacks               = Img_EnsWrapper::SetCallbacks;
		pOmxComponentType->ComponentDeInit            = Img_EnsWrapper::ComponentDeInit;

		pOmxComponentType->UseEGLImage                = Img_EnsWrapper::UseEGLImage;
		pOmxComponentType->ComponentRoleEnum          = Img_EnsWrapper::ComponentRoleEnum;
	}
	return(S_OK);
}



//********************************************************************************
//********************************************************************************
//********************************************************************************

ExtendedMutex::ExtendedMutex(const char *name)
: m_Name(name)
//********************************************************************************
{
	m_Mutex.Create();
	m_InternalMutex.Create();
	m_Lock_ThreadId    = (t_ThreadId)eThread_None;
	m_Lock_Depth       = 0;
	m_Lock_Owner       = eUid_None;
}

ExtendedMutex::~ExtendedMutex()
//********************************************************************************
{
	m_Mutex.Destroy(); // release the mutex
	m_InternalMutex.Destroy();
}

int ExtendedMutex::Lock(enumLockType /*type*/, const int pHandle, const char *aComment)
//********************************************************************************
{
	// MutexedCode CriticalSection(m_InternalMutex); //Protect function against concurrent action
	size_t iter=0;
	t_ThreadId tid = steGetCurrentThreadId();
	OTB_ASSERT(pHandle && aComment);
#ifdef _DEBUG
	static int counter=0;
	++counter;
#endif
#ifdef NEW_LOCK_MANAGEMENT
	enumLockType target_lock=lock;
	while (tid != 0)
	{
		if (target_lock==eLock_Singleton)
		{
		}
		else if (target_lock==eLock_Child)
		{
			if(m_Lock_Depth == 0)

		}
		else if (target_lock==eLock_Master)
		{
		}
	}
#else
	// Check if we already hold the lock
	while (tid != 0)
	{
		//LOGD("\n ***************Img_EnsWrapper::GlobalLock[0x%08x](%d) entry=%d pHandle=0x%08x aComment=%s \n", tid, counter, m_Lock_Depth, pHandle, aComment);
		if (m_Lock_ThreadId == tid)
		{// Thread already holds the lock - let's rock
			++m_Lock_Depth;
			if(m_Lock_Owner != pHandle)
			{
				LOGD("LOCK depth increased with different Handles! Component=0x%08x != lock holder hComponent=0x%08x - '%s'\n", (unsigned int)pHandle, (unsigned int)m_Lock_Owner, aComment);
			}
			LOGD("LOCK - hComponent=0x%08x - got mutex count=%d - '%s'", (unsigned int)pHandle, m_Lock_Depth, aComment);
			return S_OK;
		}
		else
		{// Use a timed lock to detect soft lockup - wait no more than 3s
			if (m_Lock_ThreadId != (t_ThreadId)eThread_None)
				LOGD("Img_EnsWrapper::GlobalLock[0x%08x] Currently lock by tid=[0x%08x], try WAIT.", tid, m_Lock_ThreadId);
			else
				LOGD("Img_EnsWrapper::GlobalLock[0x%08x] try WAIT.", tid);
			if (m_Mutex.LockTimeout(3000) == 0)
			{ 
				++m_Lock_Depth ;
				m_Lock_ThreadId    = tid;
				m_Lock_Owner       = pHandle;
				m_Lock_Comment     = aComment;
				// LOGD("Ok LOCK - hComponent=0x%08x - got mutex - '%s'\n", (unsigned int)pHandle, aComment);
				return S_OK;
			}
			else
			{// Got timeout
				++iter;
				LOGE("ExtendedMutex::Lock TIMEOUT  hComponent=0x%08x - Failed(%d) '%s' - held by tid=%d from '%s'\n", (unsigned int)pHandle, iter, aComment, (int) m_Lock_ThreadId, m_Lock_Comment);
				OTB_ASSERT(0);
			}
		}
		
	} //while. Just try again as we cannot exit anyway
#endif
	return(S_OK);
}

int ExtendedMutex::Unlock(const int pHandle, const char *aComment)
//********************************************************************************
{
	// t_ThreadId tid = steGetCurrentThreadId();
	//LOGD("\n ***************IGlobal Unlock[0x%08x] entry=%d pHandle=0x%08x aComment=%s\n", tid, m_Lock_Depth, pHandle, aComment);
	OTB_ASSERT(pHandle && aComment);
	if(m_Lock_Depth == 0)
	{
		LOGE("UNLOCK called while lock not held - '%s'", aComment);
		OTB_ASSERT(0);
		return S_OK;
	}
	//Decrement the lock count
	m_Lock_Depth--;

	if ( ( (int)m_Lock_Owner != pHandle) && (m_Lock_Owner < (int)eUid_Specific) && (pHandle < (int)eUid_Specific) )
	{ 
		LOGE("UNLOCK  ERROR : lock depth decreased with hComponent=0x%08x != " "lock holder hComponent=0x%08x - '%s'\n", (unsigned int)pHandle, (unsigned int)m_Lock_Owner, aComment);
		OTB_ASSERT(0); 
	}

	if(m_Lock_Depth == 0)
	{	// Then release the lock to allow other threads to enter OMX component
		m_Lock_ThreadId  = (t_ThreadId)eThread_None;
		m_Lock_Owner    = eUid_None;

		// OSAL_MutexUnlock(m_Mutex);
		LOGD("UNLOCK - hComponent=0x%08x - released mutex - %s", (unsigned int)pHandle, aComment);
		m_Mutex.Unlock();
	}
	return(S_OK);
}

//********************************************************************************
//********************************************************************************
//********************************************************************************
//********************************************************************************

ComponentManager::ComponentManager()
//********************************************************************************
{
	LIST_INIT(&omx_cmp_list);      // Init OMX cmp list as emtpy
	LIST_INIT(&omx_cmp_trace_list);// Init OMX cmp trace list as emtpy
}

ComponentManager::~ComponentManager()
//********************************************************************************
{
}


int ComponentManager::GlobalLock (ExtendedMutex::enumLockType type, const int pHandle, const char *aComment)
//********************************************************************************
{
	int result=m_GlobalMutex.Lock(type, (const int)pHandle, aComment);
	return(result);
}

int ComponentManager::GlobalUnlock(const int pHandle, const char *aComment)
//********************************************************************************
{
	int result=m_GlobalMutex.Unlock((const int)pHandle, aComment);
	return(result);
}

unsigned int *ComponentManager::GetTraceLevelVariable(int what) const
//********************************************************************************
{ // return the address of the internal trace variable
	switch(what)
	{
	case eTrace_Debug:
		return((unsigned int *)&TraceLevel[eTrace_Debug]);
	case eTrace_Omx:
		return((unsigned int *)&TraceLevel[eTrace_Omx]);
	default:
		return NULL;
	}
}

unsigned int ComponentManager::GetTraceLevel(int what) const
//********************************************************************************
{
	unsigned int *pVar=GetTraceLevelVariable(what);
	if (pVar)
		return(*pVar);
	else
		return(0);
}

unsigned int ComponentManager::SetTraceLevel(int what, unsigned int newlevel)
//********************************************************************************
{
	unsigned int *pVar=GetTraceLevelVariable(what);
	int old=0;
	if (pVar)
	{
		old  = *pVar;
		*pVar= newlevel;
	}
	return(old); //Return old value;
}

/**
* This function is to be called while holding the gobal lock to ensure
* no other thread is currently changing the list
*/
bool ComponentManager::IsComponentActive(OMX_COMPONENTTYPE * comphdl) const
//********************************************************************************
{
	omx_cmp_list_elem_t * elem = NULL;
	LIST_FOREACH(elem, &omx_cmp_list, list_entry)
	{
		if(elem->omx_component == comphdl)
		{
			return true;
		}
	}
	return false;
}

int ComponentManager::RemoveHandleFromActiveList(OMX_COMPONENTTYPE * comphdl)
//********************************************************************************
{/// This function is to be called while holding the gobal lock to ensure no other thread is currently changing the list
	omx_cmp_list_elem_t * iter_elem = NULL;
	LIST_FOREACH(iter_elem, &omx_cmp_list, list_entry)
	{
		if(iter_elem->omx_component == comphdl)
		{
			LIST_REMOVE(iter_elem, list_entry);
			return(S_OK);
		}
	}
	IMG_DBC_ASSERT(found_elem);
	return(-1);
}

int  ComponentManager::RegisterHandle(OMX_COMPONENTTYPE *pHandle)
//********************************************************************************
{/// Add to list of active components the provided handle
	// OTB_ASSERT(pHandle);
	if (pHandle ==NULL)
	{
		LOGE("Try to register NULL handle!\n");
		return(eError_RegisterNullHandle);
	}
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) pHandle;
	Img_EnsWrapper * wrapper    = (Img_EnsWrapper *) omx_comp->pComponentPrivate;
	GlobalLock(ExtendedMutex::eLock_Child, eUid_ComponentManager, __FUNCTION__);
	{
		LIST_INSERT_HEAD(&omx_cmp_list, &wrapper->list_elem, list_entry);
	}
	GlobalUnlock(eUid_ComponentManager, __FUNCTION__);
	return(S_OK);
}

void ComponentManager::PrintComponentList() /*const */
//********************************************************************************
{
	omx_cmp_list_elem_t * iter_elem = NULL;
	Img_EnsWrapper * wrapper;
	GlobalLock(ExtendedMutex::eLock_Child, eUid_ComponentManager, __FUNCTION__);
	LOGD("List of active OMX components\n");
	LOGD("|   ID   |   Handle   | CR | Name\n");
	LIST_FOREACH(iter_elem, &omx_cmp_list, list_entry)
	{
		wrapper = (Img_EnsWrapper *) iter_elem->omx_component->pComponentPrivate;
		LOGD("  %8p - 0x%08x - %s\n", wrapper, (unsigned int) iter_elem->omx_component, (const char*) wrapper->getImgEns_Component()->getName());
	}
	GlobalUnlock(eUid_ComponentManager, __FUNCTION__);
}


/********************************************************************************
* Constructor
********************************************************************************/

Img_EnsWrapper::Img_EnsWrapper(ImgEns_IOmx *pEnsComponent, OMX_COMPONENTTYPE * pOmxComponentType)
//********************************************************************************
: m_ComponentManager(GetComponentManager())
, m_pOmxComponent(pOmxComponentType)
, m_pEnsComponent(pEnsComponent)
, m_Omx_UserCallbacks(NULL)
{
#ifdef _DEBUG
	m_ComponentManager.SetTraceLevel(eTrace_Omx, 0xFFFF);
	m_ComponentManager.SetTraceLevel(eTrace_Debug, 0xFFFF);
#else
#endif
#if 0
	OMX_dump_level  = 0;
	OMX_trace_level = 0;
	OMX_dump_output = 0;
#endif
	IMGWRAPPER_OMX_LOG("Img_EnsWrapper::Img_EnsWrapper(m_pOmxComponent=\"%s\", hComponent=0x%08x)\n", (const char*) m_pEnsComponent->getName(), (unsigned int) m_pOmxComponent);
	// Store this wrapper instance
	m_pOmxComponent = pOmxComponentType;
	m_pEnsComponent = pEnsComponent;

	pOmxComponentType->nSize                      =  sizeof(OMX_COMPONENTTYPE);
	pOmxComponentType->nVersion.nVersion          =  OMX_VERSION;
	m_ComponentManager.Init_OMX_COMPONENTTYPE(pOmxComponentType, this); // Set all entry points
	if (pEnsComponent)
	{ 
		pEnsComponent->SetWrapper(this);
		pEnsComponent->setOMXHandle(pOmxComponentType);
		pEnsComponent->SetCallbacks(&g_OmxCallbacks_Wrapper, this);
	}

	m_Semaphore_WaitingForCmdReceived.Create(0);
#ifndef NEW_CMD_RECEIVED_TO_WAIT
	mNbCmdReceivedToWait                = 0;
#endif
	list_elem.omx_component = m_pOmxComponent;

	// We aquire the global lock to increment unique ID and register ourself to the omx_cmp_list
	IMGWRAPPER_MASTER_LOCK(m_pOmxComponent);

	m_ComponentManager.RegisterHandle(pOmxComponentType);
	if (m_pOmxComponent)
	{
		IMGWRAPPER_OMX_LOG("GetHandle ComponentName=\"%s\", hComponent=0x%08x pAppData=0x%08x\n", (const char*) m_pEnsComponent->getName(), (unsigned int) m_pOmxComponent, (unsigned int) m_pOmxComponent->pApplicationPrivate);
	}
	else
	{
		IMGWRAPPER_OMX_LOG("GetHandle ComponentName=\"%s\", hComponent=0x%08x\n", (const char*) m_pEnsComponent->getName(), (unsigned int) m_pOmxComponent);
	}

	// We update trace level on every component creation/destruction in case property has been set after start of session (lock must be held)
	// updateTraceLevel(m_pOmxComponent);
	IMGWRAPPER_UNLOCK(m_pOmxComponent);
}

Img_EnsWrapper::~Img_EnsWrapper()
//********************************************************************************
{
	m_Semaphore_WaitingForCmdReceived.Destroy();
}


/********************************************************************************
* Warpper Threads entries
********************************************************************************/
/* static */
ImgEns_Component*  getImgEns_Component(OMX_HANDLETYPE hComp)
//********************************************************************************
{ // Return the component attached to omx handle
	OMX_COMPONENTTYPE *pOmxComp = (OMX_COMPONENTTYPE *) hComp;
	Img_EnsWrapper    * wrapper = (Img_EnsWrapper *) pOmxComp->pComponentPrivate;
	return (ImgEns_Component *)wrapper->getImgEns_Component();
}

/********************************************************************************
* Wrapper funtions being called by client to then call ENS code
********************************************************************************/


OMX_ERRORTYPE Img_EnsWrapper::SetCallbacks(OMX_HANDLETYPE hComponent, OMX_CALLBACKTYPE* pCallbacks, OMX_PTR pAppData)
//********************************************************************************
{// SetCallbacks wrapper
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	// We register those new callbacks to the wrapper
	wrapper->m_Omx_UserCallbacks = pCallbacks;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("SetCallbacks %s hComponent=0x%08x, pCallbacks=0x%08x, pAppData=0x%08x\n", (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent, (unsigned int) pCallbacks, (unsigned int) pAppData);

	// and call the component to update pAppData with wrapper's CB
	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->SetCallbacks(&g_OmxCallbacks_Wrapper, pAppData);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - SetCallbacks hComponent=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


OMX_ERRORTYPE Img_EnsWrapper::ComponentDeInit(OMX_HANDLETYPE hComponent)
//********************************************************************************
{//static  ComponentDeInit wrapper
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper    * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	// Wrapper lock is to protect ENS proxy code and not CM -> otherwise dead lock is possible
	IMGWRAPPER_MASTER_LOCK(omx_comp);
	IMGWRAPPER_OMX_LOG("FreeHandle %s hComponent=0x%08x\n", (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent);


	// and call the component to update pAppData with wrapper's CB
	ImgEns_IOmx *pComponent= wrapper->getImgEns_Component();
	OMX_ERRORTYPE result =pComponent->ComponentDeInit();

	// Upon ComponentDeInit ENSComponent object is ready to be deleted
	delete pComponent;
	delete wrapper;
	//wrapper->getImgEns_Component() = NULL;

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - FreeHandle hComponent=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent);

	// We update trace level on every component creation/destruction in case property has been set after start of session (lock must be held)
	//updateTraceLevel(omx_comp);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

OMX_ERRORTYPE Img_EnsWrapper::GetComponentVersion(OMX_HANDLETYPE hComponent,
	OMX_STRING       pComponentName,
	OMX_VERSIONTYPE* pComponentVersion,
	OMX_VERSIONTYPE* pSpecVersion,
	OMX_UUIDTYPE   * pComponentUUID)
//********************************************************************************
{// GetComponentVersion wrapper
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("GetComponentVersion %s hComponent=0x%08x\n",
		(const char*) wrapper->getImgEns_Component()->getName(),
		(unsigned int) hComponent);

	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->GetComponentVersion(pComponentName, pComponentVersion, pSpecVersion, pComponentUUID);

	char bufferCUUID[128*2+5];

	if(wrapper->OMX_trace_level)
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

OMX_ERRORTYPE Img_EnsWrapper::SendCommand(OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData)
//********************************************************************************
{//static SendCommand wrapper
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	if (wrapper->OMX_trace_level)
		displaySendCommand((const char*) wrapper->getImgEns_Component()->getName(), hComponent, Cmd, nParam,  pCmdData);

	// Use local value for nbCmdReceivedtoWait and not wrapper->mNbCmdReceivedToWait, as we must wait on semaphore even if notified first
	unsigned int nbCmdReceivedtoWait = 1;
	if ( ( (Cmd == OMX_CommandPortDisable) || (Cmd == OMX_CommandPortEnable) ) && (nParam == OMX_ALL) ) 
	{//In case of disablePort/enablePort with index OMX_ALL -> each port will sent the cmdReceived event.
		nbCmdReceivedtoWait= wrapper->getImgEns_Component()->getPortCount();
	}
	else if ((Cmd == OMX_CommandFlush) || (Cmd == OMX_CommandMarkBuffer))
	{ //In this case, ENS component will not send any cmdReceived event.
		nbCmdReceivedtoWait= 0;
	}

	IMGWRAPPER_OMX_LOG("nbCmdReceivedtoWait=%d: SendCommand " "request on %s hComponent=%#08x\n", nbCmdReceivedtoWait, (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent);
#ifndef NEW_CMD_RECEIVED_TO_WAIT
	wrapper->mNbCmdReceivedToWait    = nbCmdReceivedtoWait;
#endif

	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->SendCommand(Cmd, nParam, pCmdData);

	/* To ensure correct execution order from client of SendCommand to multiple components
	to change to a state needing information exchange between components the ENS layer
	sends a CmdReceived event to release the client thread */
	if (result == OMX_ErrorNone)
	{
#ifdef NEW_CMD_RECEIVED_TO_WAIT
		while(nbCmdReceivedtoWait != 0)
		{
			if (wrapper->m_Semaphore_WaitingForCmdReceived.WaitTimeout(4000))
			{ // Expect nbCmdReceivedtoWait back from ENS within 4s
				LOGE("hComponent=%#08x - 4s timeout expired - " "missing %d EventCmdReceived", (unsigned int) hComponent, nbCmdReceivedtoWait);
				OTB_ASSERT(0);
				result = OMX_ErrorTimeout;
			}
			else
				--nbCmdReceivedtoWait;
		}
		IMGWRAPPER_UNLOCK(omx_comp);
#else
		IMGWRAPPER_UNLOCK(omx_comp);
		if(nbCmdReceivedtoWait != 0)
		{
			IMGWRAPPER_OMX_LOG("hComponent=%#08x - released lock, ""awaiting %d EventCmdReceived within 4s", (unsigned int) hComponent, nbCmdReceivedtoWait);
			// Expect nbCmdReceivedtoWait back from ENS within 4s
//			if (OSAL_SemaphoreWaitTimeOut(wrapper->m_Semaphore_WaitingForCmdReceived, 4000))
			if ( wrapper->m_Semaphore_WaitingForCmdReceived.WaitTimeout(4000)  )
			{
				LOGE("hComponent=%#08x - 4s timeout expired - " "missing %d EventCmdReceived", (unsigned int) hComponent, nbCmdReceivedtoWait);
				OTB_ASSERT(0);
				GetComponentManager().PrintComponentList();
				result = OMX_ErrorTimeout;
			}
			else
				--nbCmdReceivedtoWait;
		}
#endif
	}
	else
	{
#ifndef NEW_CMD_RECEIVED_TO_WAIT
		wrapper->mNbCmdReceivedToWait = 0;
#endif
		IMGWRAPPER_UNLOCK(omx_comp);
	}
	if (GetComponentManager().GetTraceLevel(eTrace_Omx))
	{
		char result_str[128];
		LOGD("return %s - SendCommand %s hComponent=%#08x\n", (const char*) wrapper->getImgEns_Component()->getName(), strOMX_Error(result, result_str), (unsigned int) hComponent);
	}
	return result;
}

// GetParameter wrapper
OMX_ERRORTYPE Img_EnsWrapper::GetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentParameterStructure)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("GetParameter %s hComponent=0x%08x\n", (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent);

	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->GetParameter(nIndex, pComponentParameterStructure);

	displayParameterAndConfig(nIndex, pComponentParameterStructure);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - GetParameter hComponent=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

OMX_ERRORTYPE Img_EnsWrapper::SetParameter(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR       pComponentParameterStructure)
//********************************************************************************
{// SetParameter wrapper

	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("SetParameter %s hComponent=0x%08x\n", (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent);

	displayParameterAndConfig(nIndex, pComponentParameterStructure);

	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->SetParameter(nIndex, pComponentParameterStructure);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - SetParameter hComponent=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// GetConfig wrapper
OMX_ERRORTYPE Img_EnsWrapper::GetConfig(OMX_HANDLETYPE hComponent,
	OMX_INDEXTYPE nIndex,
	OMX_PTR       pComponentConfigStructure)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("GetConfig %s hComponent=0x%08x\n", (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent);

	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->GetConfig(nIndex,
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
OMX_ERRORTYPE Img_EnsWrapper::SetConfig(OMX_HANDLETYPE hComponent, OMX_INDEXTYPE nIndex, OMX_PTR pComponentConfigStructure)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("SetConfig %s hComponent=0x%08x\n",
		(const char*) wrapper->getImgEns_Component()->getName(),
		(unsigned int) hComponent);

	displayParameterAndConfig(nIndex, pComponentConfigStructure);

	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->SetConfig(nIndex,
		pComponentConfigStructure);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - SetConfig hComponent=0x%08x\n",
		strOMX_Error(result, result_str),
		(unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// GetExtensionIndex wrapper
OMX_ERRORTYPE Img_EnsWrapper::GetExtensionIndex(OMX_HANDLETYPE hComponent, OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("GetExtensionIndex %s hComponent=0x%08x cParameterName=%s\n",
		(const char*) wrapper->getImgEns_Component()->getName(),
		(unsigned int) hComponent, cParameterName);

	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->GetExtensionIndex(cParameterName, pIndexType);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - GetExtensionIndex hComponent=0x%08x IndexType=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent, (pIndexType ? (unsigned int) *pIndexType : 0));

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


OMX_ERRORTYPE Img_EnsWrapper::GetState(OMX_HANDLETYPE hComponent, OMX_STATETYPE* pState)
//********************************************************************************
{ // GetState wrapper static
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->GetState(pState);
	IMGWRAPPER_OMX_LOG("GetState %s hComponent=0x%08x State= %s(%d)\n", (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent, GetStringOmxState(*pState), *pState);

	if(wrapper->OMX_trace_level)
	{
		char buffer[1024];
		buffer[0] = '\0';
		if(pState)
			strcatOMX_STATETYPE(buffer, *pState);
		char result_str[128];
		IMGWRAPPER_OMX_LOG("return %s - GetState hComponent=0x%08x pState=%s\n", strOMX_Error(result, result_str), (unsigned int) hComponent, buffer);
	}

	IMGWRAPPER_UNLOCK(omx_comp);
	return result;
}


// ComponentTunnelRequest wrapper
OMX_ERRORTYPE Img_EnsWrapper::ComponentTunnelRequest(OMX_HANDLETYPE hComponent,
	OMX_U32              nPort,
	OMX_HANDLETYPE       hTunneledComp,
	OMX_U32              nTunneledPort,
	OMX_TUNNELSETUPTYPE* pTunnelSetup)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	char buffer[1024];

	if(wrapper->OMX_trace_level)
	{
		buffer[0] = '\0';
		strcatOMX_TUNNELSETUPTYPE(buffer, pTunnelSetup);
	}

	IMGWRAPPER_OMX_LOG("ComponentTunnelRequest: %s hComponent=0x%08x nPort=%d "
		"hTunneledComp=0x%08x pTunneledPort=%d %s\n",
		(const char*) wrapper->getImgEns_Component()->getName(),
		(unsigned int) hComponent, (int) nPort,
		(unsigned int) hTunneledComp, (int) nTunneledPort, buffer);

	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->ComponentTunnelRequest(nPort, hTunneledComp, nTunneledPort, pTunnelSetup);
	if(wrapper->OMX_trace_level)
	{
		buffer[0] = '\0';
		strcatOMX_TUNNELSETUPTYPE(buffer, pTunnelSetup);
	}

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - ComponentTunnelRequest: hComponent=0x%08x nPort=%d hTunneledComp=0x%08x pTunneledPort=%d %s\n", strOMX_Error(result, result_str), (unsigned int) hComponent, (int) nPort, (unsigned int) hTunneledComp, (int) nTunneledPort, buffer);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// UseBuffer wrapper
OMX_ERRORTYPE Img_EnsWrapper::UseBuffer(OMX_HANDLETYPE hComponent,
	OMX_BUFFERHEADERTYPE** ppBufferHdr,
	OMX_U32 nPortIndex,
	OMX_PTR pAppPrivate,
	OMX_U32 nSizeBytes,
	OMX_U8* pBuffer)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper    = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("UseBuffer %s hComponent=0x%08x nPortIndex=%d pAppPrivate=0x%08x"
		" nSizeBytes=%d pBuffer=0x%08x\n",
		(const char*) wrapper->getImgEns_Component()->getName(),
		(unsigned int) hComponent, (int) nPortIndex, (unsigned int) pAppPrivate,
		(int) nSizeBytes, (unsigned int) pBuffer);

	//Send command to component
	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->UseBuffer(ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - UseBuffer: hComponent=0x%08x pBufferHdr=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent, (unsigned int) (ppBufferHdr ? *ppBufferHdr : 0));

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


//AllocateBuffer wrapper
OMX_ERRORTYPE Img_EnsWrapper::AllocateBuffer(OMX_HANDLETYPE hComponent,
	OMX_BUFFERHEADERTYPE** ppBufferHdr,
	OMX_U32 nPortIndex,
	OMX_PTR pAppPrivate,
	OMX_U32 nSizeBytes)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("AllocateBuffer %s hComponent=0x%08x nPortIndex=%d pAppPrivate=0x%08x nSizeBytes=%d\n",
		(const char*) wrapper->getImgEns_Component()->getName(),
		(unsigned int) hComponent, (int) nPortIndex, (unsigned int) pAppPrivate,
		(int) nSizeBytes);

	//Send command to component
	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->AllocateBuffer(ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - AllocateBuffer: hComponent=0x%08x pBufferHdr=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent, (unsigned int) (ppBufferHdr ? *ppBufferHdr : 0));

	IMGWRAPPER_UNLOCK(omx_comp);
	return result;
}


// FreeBuffer wrapper
OMX_ERRORTYPE Img_EnsWrapper::FreeBuffer(OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_BUFFERHEADERTYPE* pBuffer)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper    = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("FreeBuffer %s hComponent=0x%08x nPortIndex=%d pBuffer=0x%08x\n", (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent, (int) nPortIndex, (int) pBuffer);

	//Send command to component
	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->FreeBuffer(nPortIndex, pBuffer);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - FreeBuffer: hComponent=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// EmptyThisBuffer wrapper
OMX_ERRORTYPE Img_EnsWrapper::EmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper    = (Img_EnsWrapper *) omx_comp->pComponentPrivate;


	IMGWRAPPER_MASTER_LOCK(omx_comp);

#ifdef FORCE_OMX_BUFFERFLAG_STARTTIME_ON_FIRST_EMPTYTHISBUFFER
	// Thierry: this really is to be removed !!!
	if (wrapper->bFirstBufferSend == false)
	{
		pBuffer->nFlags |= OMX_BUFFERFLAG_STARTTIME;
		wrapper->bFirstBufferSend = true;
	}
#endif

#if 0
	if (wrapper->OMX_trace_level >= OMX_LOG_LEVEL_BUFFER)
	{
		char header_buffer[1024];
		int dump_buffer_header = 0;
		LOGD("EmptyThisBuffer %s hComponent=0x%08x pBuffer=0x%08x\n", (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent, (unsigned int)  pBuffer);
		dumpOMX_BUFFERHEADERTYPE(pBuffer, header_buffer);
		dump_buffer_header = 1;
	}

	if (wrapper->OMX_dump_level && (wrapper->OMX_dump_output != 1))
	{
		if (strcmp((const char*) wrapper->getImgEns_Component()->getName(), OMX_dump_name) == 0)
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
#endif
	//Send command to component
	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->EmptyThisBuffer(pBuffer);
#if 0
	if (wrapper->OMX_trace_level >= OMX_LOG_LEVEL_BUFFER)
	{
		char result_str[128];
		LOGD("return %s - EmptyThisBuffer hComponent=0x%08x pBuffer=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent, (unsigned int) pBuffer);
	}
#endif
	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

// FillThisBuffer wrapper
OMX_ERRORTYPE Img_EnsWrapper::FillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	if (wrapper->OMX_trace_level >= OMX_LOG_LEVEL_BUFFER)
	{
		LOGD("FillThisBuffer %s hComponent=0x%08x pBuffer=0x%08x\n", (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent, (unsigned int) pBuffer);
	}

	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->FillThisBuffer(pBuffer);

	if (wrapper->OMX_trace_level >= OMX_LOG_LEVEL_BUFFER) 
	{
		char result_str[128];
		LOGD("return %s - FillThisBuffer hComponent=0x%08x pBuffer=0x%08x\n", strOMX_Error(result, result_str), (unsigned int) hComponent, (unsigned int) pBuffer);
	}

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// UseEGLImage wrapper
OMX_ERRORTYPE Img_EnsWrapper::UseEGLImage(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, void* eglImage)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);
	IMGWRAPPER_OMX_LOG("UseEGLImage %s hComponent=0x%08x nPortIndex=%d pAppPrivate=0x%08x eglImage=0x%08x\n",
		(const char*)  wrapper->getImgEns_Component()->getName(),
		(unsigned int) hComponent, (int) nPortIndex,
		(unsigned int) pAppPrivate, (unsigned int) eglImage);

	//Send command to component
	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->UseEGLImage(ppBufferHdr, nPortIndex, pAppPrivate, eglImage);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - UseEGLImage hComponent=0x%08x\n", strOMX_Error(result, result_str), (int) hComponent);

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}


// ComponentRoleEnum wrapper
OMX_ERRORTYPE Img_EnsWrapper::ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8* cRole, OMX_U32 nIndex)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	IMGWRAPPER_MASTER_LOCK(omx_comp);

	IMGWRAPPER_OMX_LOG("ComponentRoleEnum %s hComponent=0x%08x nIndex=%d\n", (const char*) wrapper->getImgEns_Component()->getName(), (unsigned int) hComponent, (int) nIndex);

	//Send command to component
	OMX_ERRORTYPE result = wrapper->getImgEns_Component()->ComponentRoleEnum(cRole, nIndex);

	char result_str[128];
	IMGWRAPPER_OMX_LOG("return %s - ComponentRoleEnum hComponent=0x%08x Role=\"%s\"\n", strOMX_Error(result, result_str), (unsigned int) hComponent, (result == OMX_ErrorNone ? cRole: (OMX_U8*) ""));

	IMGWRAPPER_UNLOCK(omx_comp);

	return result;
}

/* static  */
OMX_ERRORTYPE Img_EnsWrapper::EventHandler(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_EVENTTYPE eEvent, OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2, OMX_IN OMX_PTR pEventData)
//********************************************************************************
{
	OMX_ERRORTYPE error= OMX_ErrorNone;
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;

	if ((int)eEvent== eIMGOMX_EventCmdReceived)
	{ // STE OMX internal event - not sent to OMX client
#ifdef NEW_CMD_RECEIVED_TO_WAIT
		// Just signal the downcount is done in send command
		wrapper->m_Semaphore_WaitingForCmdReceived.Notify();
#else
		IMGWRAPPER_MASTER_LOCK(wrapper->m_pOmxComponent);
		if(wrapper->mNbCmdReceivedToWait > 0)
		{	/* We can unlock the client thread waiting for this internal event
			The event is internal and must not be send to client */
			wrapper->mNbCmdReceivedToWait --;
			if(wrapper->mNbCmdReceivedToWait == 0)
			{
				// OSAL_SemaphoreNotify(wrapper->m_Semaphore_WaitingForCmdReceived);
				wrapper->m_Semaphore_WaitingForCmdReceived.Notify();
			}
		}
		else
		{
		}
		IMGWRAPPER_UNLOCK(wrapper->m_pOmxComponent);
#endif
		return error; // don't send this event to user
	}
	// now send to user
	if (wrapper->m_Omx_UserCallbacks && (wrapper->m_Omx_UserCallbacks->EventHandler != NULL))
	{ // Finally call User callback
		error=wrapper->m_Omx_UserCallbacks->EventHandler(hComponent, pAppData, eEvent, nData1, nData2, pEventData);
	}
	return error;
}

OMX_ERRORTYPE Img_EnsWrapper::EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;
	OMX_ERRORTYPE error= OMX_ErrorNone;
	if (wrapper->m_Omx_UserCallbacks && (wrapper->m_Omx_UserCallbacks->EmptyBufferDone != NULL))
	{ // Finally call User callback
		error=wrapper->m_Omx_UserCallbacks->EmptyBufferDone(hComponent, pAppData, pBuffer);
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE Img_EnsWrapper::FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
//********************************************************************************
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComponent;
	Img_EnsWrapper * wrapper = (Img_EnsWrapper *) omx_comp->pComponentPrivate;
	OMX_ERRORTYPE error= OMX_ErrorNone;
	if (wrapper->m_Omx_UserCallbacks && (wrapper->m_Omx_UserCallbacks->FillBufferDone != NULL))
	{ // Finally call User callback
		error=wrapper->m_Omx_UserCallbacks->FillBufferDone(hComponent, pAppData, pBuffer);
	}
	return OMX_ErrorNone;
}

#if 0
void Img_EnsWrapper__sigAction(int signum, siginfo* info, void*)
{
	if (signum || info) { } // Just to avoid warning
#ifdef ANDROID
	// memStatus();
#else
	LOGE("Memory leak tracing not implemented\n");
#endif
}
#endif

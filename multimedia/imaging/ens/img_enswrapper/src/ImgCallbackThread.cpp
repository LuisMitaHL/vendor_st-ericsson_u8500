/*****************************************************************************
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *****************************************************************************/
#include "ImgEns_Shared.h"
#include "osi_toolbox_lib.h"
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

#include <omxil/OMX_Core.h>
#include <omxil/OMX_Component.h>
#include <omxil/OMX_IVCommon.h>

# include "processing_message.h"
# include "processing_observer.h"

#include "Img_EnsWrapper.h"
#include "ImgCallbackThread.h"

const Observer_Message Observer_EmptyMsg = { OBSERVER_EMPTY_MSG, NULL};

// Max number of observer messages queued
static const size_t Max_ObserverMessages        = 256;
static const size_t Max_NumberObserverRegistred = MAX_NUM_OBSERVERS_REGISTERED;


Img_CallbackThread::Img_CallbackThread(size_t default_stack_size)
: m_QueueMessages(Max_ObserverMessages, m_Semaphore_WaitingFoMessage)
//*************************************************************************************************************
{ // Creation of the singleton
	m_NbRegistered      = 0;
	m_bIsThreadStarted  = false;
	m_bAskThreadToStop  = false; // Set to true to stop the thread
	m_DefaultStack_size = default_stack_size;
}

Img_CallbackThread::~Img_CallbackThread()
//*************************************************************************************************************
{ // Destruction of the singleton
	m_bIsThreadStarted = false;
}

int Img_CallbackThread::Start(int priority, size_t stack)
//*************************************************************************************************************
{ // start the callback thread if not already done
	if (IsThreadStarted() == true)
		return (eError_None);

	// Create semaphores
	m_Semaphore_WaitingFoMessage.Create(0);
	m_Semaphore_WaitingForExit.Create(0);
	m_bAskThreadToStop = false;

	OTB_ASSERT(m_bIsThreadStarted == false);

	m_QueueMessages.Init(&Observer_EmptyMsg);

	// Start the thread managing the observers requests
	steCreateThread(staticObserversThread, this, stack, priority, "IMG-OBSERVER-CB-THREAD");
	return(S_OK);
}

int Img_CallbackThread::Stop()
//*************************************************************************************************************
{
	if (m_NbRegistered != 0)
		return (eError_None);
	m_bAskThreadToStop=true;
	m_Semaphore_WaitingFoMessage.Notify();

	// Wait for observer processing thread to exit
	m_Semaphore_WaitingForExit.Wait();
	OTB_ASSERT(IsThreadStarted()== false);

	// Destroy semaphores
	m_Semaphore_WaitingFoMessage.Destroy();
	m_Semaphore_WaitingForExit  .Destroy();
	m_QueueMessages.Clean();
	return (eError_None);
}

t_ThreadReturnType Img_CallbackThread::staticObserversThread(void* param)
//*************************************************************************************************************
{ // static Entry point for new thread
	Img_CallbackThread *pImgThread = (Img_CallbackThread *) param;
	if (pImgThread)
	{
		OTB_ASSERT(pImgThread->IsThreadStarted()== false);
		pImgThread->ObserversThread(param);     // call the thread main routine
		RETURN_THREAD(0);
	}
	else
	{
		OTB_ASSERT(0); // Cannot create the thread
		RETURN_THREAD((t_ThreadReturnType)-1);
	}
}

t_ThreadReturnType Img_CallbackThread::ObserversThread(void* /*  No param */)
//*************************************************************************************************************
{ // Thread that manage imaging's component callbacks
	m_bIsThreadStarted = true;
	while (1)
	{
		// Wait message notification
		m_Semaphore_WaitingFoMessage.Wait();

		if ( (m_QueueMessages.GetUsedCount()==0) && (m_bAskThreadToStop==true))
		{
			break;
		}
		// Unqueue message to be processed
		Observer_Message* pMessage = NULL;
		m_QueueMessages.PopUsed(pMessage);
		OTB_ASSERT(pMessage != NULL);

		if (pMessage->eType == OBSERVER_EXIT_MSG)
		{// old method should be remvoed
			break;
		}

		if (m_bAskThreadToStop==true)
		{
		OTB_ASSERT(pMessage != NULL);
		}

		// Before executing the event, check that the ENS Wrapper is (still) unregistered from the observer
		// Note: OMX components must be locked to ensure that there is no modification of Img_EnsWrapper data by concurrent OMX components.
		bool bFound = IsRegistredObserver(pMessage->pObserver);
		if (bFound == true)
		{ // If observer is registered execute it
			//GetComponentManager().GlobalLock(ExtendedMutex::eLock_Child, (const int)g_Handle_ImgCallbackThread, __FUNCTION__);
			pMessage->pObserver->Execute();// Execute event
			//GetComponentManager().GlobalUnlock((const int)g_Handle_ImgCallbackThread, __FUNCTION__);
		}
		m_QueueMessages.PushFree(pMessage);
	}

	//DBGT_EPILOG("ObserversThread - exit\n");
	m_bIsThreadStarted = false;
	m_Semaphore_WaitingForExit.Notify();
	RETURN_THREAD(S_OK);
}

bool Img_CallbackThread::IsRegistredObserver(const c_ProcessingObserver* pObserver, size_t *p_index) const
//*************************************************************************************************************
{ // Check if the component is registred
	for (size_t i = 0; i < m_NbRegistered; i++)
	{
		if (m_Array_RegisteredObserver[i] == pObserver)
		{
			if (p_index)
				*p_index = i;
			return (true);
		}
	}
	return (false);
}

int Img_CallbackThread::RegisterObserver(c_ProcessingObserver* pObserver, void * /*pContext*/)
//*************************************************************************************************************
{   // Before executing the event, check that the Img_EnsWrapper is (still) unregistered from the observer
// Note: OMX components must be locked to ensure that there is no modification of Img_EnsWrapper data by
// concurrent OMX components.
//	printf("\n************** Img_CallbackThread::RegisterObserver\n");
	OTB_ASSERT(pObserver != NULL);
	// Check if we can accept the request
	// Note: component is locked
	if (m_NbRegistered >= Max_NumberObserverRegistred)
	{	// Max number of registered client reached
		OTB_ASSERT(0);
		return eError_ToManyRegistredComponent;
	}
	// Check if observer is already registered
	if (IsRegistredObserver(pObserver))
	{	// Observer already registered
		OTB_ASSERT(0);
		return eError_ComponentAlreadyRegistred;
	}

	// Set observer notif function
	e_ProcObsError eObsErr = pObserver->SetNotifFunction(Img_CallbackThread::staticObserverNotifFunction, (void *) this);
	if (eObsErr != PROCOBS_ERROR_NONE)
	{	// ENS Wrapper failed to register
		OTB_ASSERT(eObsErr == PROCOBS_ERROR_NONE);
		return eError_CannotRegisterNotifyFunction;
	}
	// Successfully registered: store the new entry
	m_Array_RegisteredObserver[m_NbRegistered] = pObserver;
	m_NbRegistered++;
	return eError_None;
}

int Img_CallbackThread::UnregisterObserver(c_ProcessingObserver* pObserver, void * /*pContext*/)
//********************************************************************************
{
//	printf("\n ************** Img_CallbackThread::UnregisterObserver\n");
	OTB_ASSERT(pObserver != NULL);
	// Check if we can accept the request
	// Note: component is locked
	size_t index;
	if (IsRegistredObserver(pObserver, &index) == false)
	{	// The requester is not registered
		return eError_ComponentNotRegistred;
	}

	// Unset notif function from observer
	e_ProcObsError eObsErr = pObserver->UnsetNotifFunction();
	OTB_ASSERT(eObsErr == PROCOBS_ERROR_NONE);
	// In case of error, we continue anyway so that the ENS wrapper is really

	// unregistered from the observer

	// Swap last entry with current (now free) one so that the array has no holes
	m_Array_RegisteredObserver[index]              = m_Array_RegisteredObserver[m_NbRegistered - 1];
	m_Array_RegisteredObserver[m_NbRegistered - 1] = NULL;
	m_NbRegistered--;

	return eObsErr == PROCOBS_ERROR_NONE ? eError_None : eError_CannotUnregisterNotifyFunction;
}

void Img_CallbackThread::staticObserverNotifFunction(void *pContext, c_ProcessingObserver *pObserver)
//*************************************************************************************************************
{ // static function used by c_observer
	Img_CallbackThread *pImgWrapper = (Img_CallbackThread *) pContext;
	if (pImgWrapper)
	{
		pImgWrapper->ObserverNotifFunction(pContext, pObserver); // Call the instance
	}
	else
	{
		OTB_ASSERT(0);
	}
}

void Img_CallbackThread::ObserverNotifFunction(void *pContext, c_ProcessingObserver *pObserver)
//*************************************************************************************************************
{ // Memorize the observer request
//	printf("\n ************** Img_CallbackThread::ObserverNotifFunction\n");
	OTB_ASSERT(pObserver != NULL);
	(void) pContext;
	Observer_Message* pMessage = NULL;
	m_QueueMessages.PopFree(pMessage);
	OTB_ASSERT(pMessage != NULL);
	if (pMessage != NULL)
	{
		pMessage->eType     = OBSERVER_NOTIF_MSG;
		pMessage->pObserver = pObserver;
		m_QueueMessages.PushUsed(pMessage);
	}
}


//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
/**
ImgMasterCallbackThread is a singleton implementation of ImgMasterCallbackThread.
All function entry is protected by mutex for avoiding conflicting call to base class
*/
// ImgMasterCallbackThread *ImgMasterCallbackThread::m_pSingleton = NULL; // One and only one instance
SmartPtr<ImgMasterCallbackThread> ImgMasterCallbackThread::m_pSingleton = NULL; // One and only one instance

ste_Mutex ImgMasterCallbackThread::m_Mutex(true);

ImgMasterCallbackThread::ImgMasterCallbackThread()
//*************************************************************************************************************
{ // return the global instance of the class
}

ImgMasterCallbackThread::~ImgMasterCallbackThread()
//*************************************************************************************************************
{
	// m_pSingleton = NULL;
}

int ImgMasterCallbackThread::staticRegisterObserver(c_ProcessingObserver* pObserver, void * pContext)
//*************************************************************************************************************
{  // Register the observer and create the singleton instance if required
	//printf("\n\n\n\n\n************** ImgMasterCallbackThread::staticRegisterObserver\n\n\n\n\n\n");
	//printf("staticRegisterObserver\n");
	MutexedCode ProtectedCode(m_Mutex);
	OTB_ASSERT(pObserver != NULL);
	ImgMasterCallbackThread *pImgCallbackThread = __GetInstance();
	if (pImgCallbackThread == NULL)
	{ // Cannot get the instance
		OTB_ASSERT(pImgCallbackThread != NULL);
		return (eError_CannotGetInstance);
	}
	int result = ((Img_CallbackThread *) pImgCallbackThread)->RegisterObserver(pObserver, pContext);
	if (result != eError_None)
	{ // Registry of observer failed
		OTB_ASSERT(result == eError_None);
	}
	else
	{ // Start if not done
		if (pImgCallbackThread->IsThreadStarted() != true)
			result = pImgCallbackThread->Start();
	}
	return (result);
}

int ImgMasterCallbackThread::staticUnregisterObserver(c_ProcessingObserver* pObserver, void * pContext)
//*************************************************************************************************************
{
	OTB_ASSERT(pObserver != NULL);
	MutexedCode ProtectedCode(m_Mutex);
	ImgMasterCallbackThread *pImgCallbackThread = __GetInstance();
	if (pImgCallbackThread == NULL)
	{
		OTB_ASSERT(pImgCallbackThread != NULL);
		return (eError_CannotGetInstance);
	}
	int result = ((Img_CallbackThread *) pImgCallbackThread)->UnregisterObserver(pObserver, pContext);
	if (result != eError_None)
	{
		OTB_ASSERT(result == eError_None);
	}
	if (pImgCallbackThread->GetNbRegistredObserver() == 0)
	{ // No more observer, stop the thead and delete object
		pImgCallbackThread->Stop();
		// delete pImgCallbackThread; //Deinstantiate the object
		m_pSingleton = NULL; // The smart pointer will distroy the pointer
	}

	return (result);
}

/* static */ImgMasterCallbackThread *ImgMasterCallbackThread::staticGetInstance()
//*************************************************************************************************************
{  // Mutexed code. Create an instance if required
	MutexedCode ProtectedCode(m_Mutex);
	ImgMasterCallbackThread * pImgCallbackThread = __GetInstance(); // Call the non protected one
	return (pImgCallbackThread);
}

/* static */ImgMasterCallbackThread *ImgMasterCallbackThread::__GetInstance()
//*************************************************************************************************************
{  //  protected NON Mutexed code. Create an instance if required
	if (m_pSingleton)
		return (m_pSingleton);
	//else
	m_pSingleton = new ImgMasterCallbackThread;
	OTB_ASSERT(m_pSingleton);
	return (m_pSingleton);

}


size_t ImgMasterCallbackThread::UpCount()
//*************************************************************************************************************
{ // Memorize the observer request
	return(GetNbRegistered());
}

size_t ImgMasterCallbackThread::DownCount()
//*************************************************************************************************************
{ // Memorize the observer request
	//ImgMasterCallbackThread *ptr=m_pSingleton;
	//OTB_ASSERT(ptr);
	if (this)
	{
		delete this;
	}
	return(0);
}


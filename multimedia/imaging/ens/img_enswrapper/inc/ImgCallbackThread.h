/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _IMGCALLBACKTHREAD_H_
#define _IMGCALLBACKTHREAD_H_

enum e_ObserverMessageType
{
	OBSERVER_EMPTY_MSG =0,
	OBSERVER_NOTIF_MSG,
	OBSERVER_EXIT_MSG
};

class c_ProcessingObserver;

struct Observer_Message
{
	e_ObserverMessageType  eType;
	c_ProcessingObserver  *pObserver;
};

// Max number of observer clients registered to ENS Wrapper
#define MAX_NUM_OBSERVERS_REGISTERED 64

// Stack size for Img thread 8K
#define IMG_THREADOBSERVER_STACK_SIZE (8*1024)


class IMG_ENSWRAPPER_API Img_CallbackThread
//*************************************************************************************************************
{
public:
	Img_CallbackThread(size_t default_stack_size=IMG_THREADOBSERVER_STACK_SIZE);
	~Img_CallbackThread();
	size_t GetNbRegistredObserver() const { return m_NbRegistered; }
	bool   IsThreadStarted()        const { return(m_bIsThreadStarted); }

	bool         IsRegistredObserver  (const c_ProcessingObserver* pObserver, size_t *p_index=NULL) const;
	virtual int  UnregisterObserver   (c_ProcessingObserver* pObserver, void *pContext = NULL);
	virtual int  RegisterObserver     (c_ProcessingObserver* pObserver, void *pContext = NULL);

	void ObserverNotifFunction      (void *pContext, c_ProcessingObserver *pObserver);
	static void staticObserverNotifFunction(void *pContext, c_ProcessingObserver *pObserver);

	size_t GetNbRegistered() const { return(m_NbRegistered);}

protected:
	int Start(int priority=eThreadPriority_Urgent, size_t stack=0); // start the thread
	int Stop ();
	static  t_ThreadReturnType staticObserversThread(void* /*  No param */); // Static function for starting thread
	virtual t_ThreadReturnType ObserversThread(void* /*  No param */); // Overridable thread function

	enum
	{
		eError_None =0,
		eError_ComponentNotRegistred,
		eError_ComponentAlreadyRegistred,
		eError_ToManyRegistredComponent,
		eError_CannotRegisterNotifyFunction,
		eError_CannotUnregisterNotifyFunction,
		eError_CannotGetInstance,
		eError_CannotCreateThread,
	};

protected:
	DualPortsQueue<Observer_Message> m_QueueMessages;
	// Semaphores
	ste_Semaphore m_Semaphore_WaitingFoMessage;
	ste_Semaphore m_Semaphore_WaitingForExit;

	// Registred component
	size_t                m_NbRegistered;
	c_ProcessingObserver* m_Array_RegisteredObserver[MAX_NUM_OBSERVERS_REGISTERED];

	bool                  m_bAskThreadToStop; // Set to true to stop the thread
	bool                  m_bIsThreadStarted;
	size_t                m_DefaultStack_size;
private:
};


class IMG_ENSWRAPPER_API ImgMasterCallbackThread: protected Img_CallbackThread
//*************************************************************************************************************
{ // Singleton that manage life cycle of Img_CallbackThread
public:
	static ImgMasterCallbackThread *staticGetInstance();
	static size_t GetNbRegistredObserver() /*const*/ {if (m_pSingleton) return m_pSingleton->m_NbRegistered; else return 0;}

	static int staticUnregisterObserver   (c_ProcessingObserver* pObserver, void *pContext = NULL);
	static int staticRegisterObserver     (c_ProcessingObserver* pObserver, void *pContext = NULL);
protected:
	ImgMasterCallbackThread();
	~ImgMasterCallbackThread();
	static ImgMasterCallbackThread *__GetInstance(); // Not mutexed one
public:
	size_t UpCount();
	size_t DownCount();

// Member
protected:
	static SmartPtr<ImgMasterCallbackThread> m_pSingleton; // One and only one instance
	static ste_Mutex          m_Mutex;
};


#endif // _IMGCALLBACKTHREAD_H_

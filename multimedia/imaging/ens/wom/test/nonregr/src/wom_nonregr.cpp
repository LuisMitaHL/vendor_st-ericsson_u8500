/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
/*
Demonstrator for asynchronous component
*/
#include <stdio.h>
#include <string.h>
#include "omxil/OMX_Types.h"
#include "omxil/OMX_Core.h"
#include "omxil/OMX_IVCommon.h"
#include "omxil/OMX_Component.h"

#include "wom_shared.h"
#include "wom_queue.h"
#include "wom_fsm.h"
#include "wom_async_port.h"
#include "wom_async_processor.h"

#include "processing_thread.h"
#include "processing_message.h"
#include "processing_observer.h"

#include "Img_EnsWrapper_Lib.h"

#define State_WaitForResponse (OMX_StatePauseSuspended+1)
unsigned int global_state=State_WaitForResponse;


//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

struct MyMsg
{
	unsigned int Event;
	unsigned int Param1;
	unsigned int Param2;
};

class MyProcessor
	: public Wom_Processor
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
public:
	WOM_API MyProcessor();
	WOM_API virtual ~MyProcessor();

	virtual void process(void);
	virtual void disablePortIndication(t_uint32 portIdx);
	virtual void enablePortIndication (t_uint32 portIdx);
	virtual void flushPortIndication  (t_uint32 portIdx);
protected:
};

MyProcessor::MyProcessor()
: Wom_Processor("MyProcessor", NULL)
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
}

MyProcessor::~MyProcessor()
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
}

void MyProcessor::process()
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
}

void MyProcessor::disablePortIndication(t_uint32 /*portIdx*/)
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
}

void MyProcessor::enablePortIndication(t_uint32 /*portIdx*/)
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
}

void MyProcessor::flushPortIndication(t_uint32 /*portIdx*/)
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

class AsyncProcessorEventHandler: public c_ProcessingObserver
//*************************************************************************************************************
{
public:
	AsyncProcessorEventHandler()
	{
		//WOM_ASSERT(0);
	}
};

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

class ProxyProcessor: public c_ProcessingThread, public Interface_ProcessorCB
//*************************************************************************************************************
{
public:
	ProxyProcessor(Wom_Processor &Processor, const char* ap_InstanceName);
	virtual ~ProxyProcessor();
	virtual int Construct( const e_ProThPriority a_Priority, const unsigned int a_StackSize, const unsigned int a_InputMsgNum, const unsigned int a_InputMsgSize, const unsigned int a_OutputMsgNum, const unsigned int a_OutputMsgSize, c_ProcessingObserver &Handler);
	//Dispatched function
	int SetState(unsigned int state);
	int ScheduleProcess(int why);

	ProxyProcessor & operator =(const ProxyProcessor & ) { WOM_ASSERT(0); return(*this);}

	static void Execute_CBFromProcessor(void *pContext); // After notification from processor via  c_WorkerEvent

	// Implementation of Interface_ProcessorCB Interface
	virtual int OmxEventFeedback(int _event, int param1, int param2);
	virtual int RequestProcess(int why);
	virtual int EmptyBufferDone(OMX_BUFFERHEADERTYPE * /*buf*/) { return(S_OK); }
	virtual int FillBufferDone (OMX_BUFFERHEADERTYPE * /*buf*/) { return(S_OK); }

protected:
	Wom_Processor    &m_pProcessorFsm;
	c_ProcessingObserver Observer;
public:
	virtual void ExecuteIncomingMsg(void* ap_Msg);
	virtual void NotifyOutgoingMsg();
};


ProxyProcessor::ProxyProcessor(Wom_Processor &Processor, const char* ap_InstanceName)
	: c_ProcessingThread(ap_InstanceName)
	, m_pProcessorFsm(Processor)
	, Observer("Observer_ProxyProcessor")
//*************************************************************************************************************
{
}

ProxyProcessor::~ProxyProcessor()
//*************************************************************************************************************
{
	ImgMasterCallbackThread::staticUnregisterObserver     (&Observer,  (void *)this);
	Destroy();
}

int ProxyProcessor::Construct(
	const e_ProThPriority a_Priority, const unsigned int a_StackSize
	, const unsigned int a_InputMsgNum , const unsigned int a_InputMsgSize
	, const unsigned int a_OutputMsgNum, const unsigned int a_OutputMsgSize
	, c_ProcessingObserver &Handler)
//*************************************************************************************************************
{
	int res=c_ProcessingThread::Construct(a_Priority, a_StackSize, a_InputMsgNum , a_InputMsgSize, a_OutputMsgNum, a_OutputMsgSize);
	//BindEventHandler(&Handler);
	ImgMasterCallbackThread::staticRegisterObserver     (&Observer, (void *)this);


	// Now we can call the processor
	m_pProcessorFsm.Init(2, this);
	m_pProcessorFsm.SetPortConfig(0, 1, Wom_AsyncPort::eDir_Input , 0, OMX_PortDomainVideo, true);
	m_pProcessorFsm.SetPortFormat(0, 0/*OMX_ColorRGB888*/, 640, 480, 0, 0);

	m_pProcessorFsm.SetPortConfig(1, 1, Wom_AsyncPort::eDir_Output, 0, OMX_PortDomainVideo, true);
	m_pProcessorFsm.SetPortFormat(1, 0/*OMX_ColorRGB888*/, 640, 480, 0, 0);

	m_pProcessorFsm.Start();
	return(res);
}

void ProxyProcessor::ExecuteIncomingMsg(void* ap_Msg)
//*************************************************************************************************************
{
	MyMsg &Evt=*(MyMsg *)ap_Msg;
	WomOmxEvent omxEvent;
	switch(Evt.Event)
	{
	case OMX_CommandStateSet:
		omxEvent.fsmEvent.signal    = ProcessorFSM::eSignal_SetState;
		omxEvent.args.setState.state= (OMX_STATETYPE)Evt.Param1;
		m_pProcessorFsm.GetFsm()->dispatch((WomFsmEvent *)&omxEvent);
		break;

	default:
		WOM_ASSERT(0);
		break;
	}
}

int ProxyProcessor::SetState(unsigned int state)
//*************************************************************************************************************
{
	global_state=State_WaitForResponse;
	MyMsg Evt={OMX_CommandStateSet, state};
	PushInputMsg(&Evt, sizeof(Evt));
	return(S_OK);
}

int ProxyProcessor::ScheduleProcess(int why)
//*************************************************************************************************************
{
	global_state=State_WaitForResponse;
	MyMsg Evt={eProxyEvent_ScheduleProcess, why};
	PushInputMsg(&Evt, sizeof(Evt));
	return(S_OK);
}

#if 0
int ProxyProcessor::ToProxy(int _event, int param1, int param2)
//*************************************************************************************************************
{
	MyMsg Evt= {_event, param1, param2};
	int res=PushOutputMsg(&Evt, sizeof(Evt));
	WOM_ASSERT(res==S_OK);
	return(res);
}
#endif

int ProxyProcessor::OmxEventFeedback(int _event, int param1, int param2)
//*************************************************************************************************************
{
	MyMsg Evt= {_event, param1, param2};
	int res=PushOutputMsg(&Evt, sizeof(Evt));
	WOM_ASSERT(res==S_OK);
	return(res);
}


int ProxyProcessor::RequestProcess(int why)
//*************************************************************************************************************
{ //Schedule a process event asynchronously
	MyMsg Evt= {eProcessEvent_ScheduleProcess, why};
	int res=PushOutputMsg(&Evt, sizeof(Evt));
	WOM_ASSERT(res==S_OK);
	return(res);
}


/* static */void ProxyProcessor::Execute_CBFromProcessor(void *pContext)
//*************************************************************************************************************
{ // Run in the proxy context pop the output queue and execute
	printf("\nExecute CB from ProxyProcessor ");
	if (!pContext)
	{
		WOM_ASSERT(0);
		return;
	}

	ProxyProcessor *pProxy=(ProxyProcessor*) pContext;
	MyMsg Evt;
	//Try to unqueue stored event and execute it
	if (pProxy->PopOutputMsg(&Evt, sizeof(Evt))!=PROTH_ERROR_NONE)
	{
		WOM_ASSERT(0);
		return;
	}
	switch(Evt.Event)
	{
	case OMX_CommandStateSet:
		printf("SetState complete");
		break;
	default:
		break;
	}
}

void ProxyProcessor::NotifyOutgoingMsg()
//*************************************************************************************************************
{
	WOM_ASSERT(0);
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

class AsyncProcessorThread
//*************************************************************************************************************
{
public:
	AsyncProcessorThread();
	~AsyncProcessorThread();
	int Start(const char *name, e_osal_thread_priority priority=OSAL_THREAD_NORMAL_PRIORITY);
	enum
	{
		eThread_NotInit,
		eThread_Start,
		eThread_Pause,
		eThread_Stop,
	};
protected:
	ste_Thread     m_ThreadId;
	ste_Semaphore m_Semaphore;

	static t_osal_thread_return_type thread_routine(void *);
	virtual int Dispatcher()=0; //Main thread routine
 private:
	int m_ThreadStatus;
};

AsyncProcessorThread::AsyncProcessorThread()
//*************************************************************************************************************
{
	m_ThreadStatus = eThread_NotInit;
}


AsyncProcessorThread::~AsyncProcessorThread()
//*************************************************************************************************************
{
}

int AsyncProcessorThread::Start(const char *name, e_osal_thread_priority priority)
//*************************************************************************************************************
{
	if (m_ThreadStatus != eThread_NotInit)
	{
		WOM_ASSERT(0);
		return(-1);
	}
	int res= m_ThreadId.Create(
		thread_routine
		, this      // arg,
		, 0         // stacksize,
		, priority  // e_osal_thread_priority priority,
		, name      //name
		);
	if (res == S_OK)
		return(S_OK);
	else
	{
		WOM_ASSERT(0);
		return(-2);
	}
}

t_osal_thread_return_type /* Static */ AsyncProcessorThread::thread_routine(void *pThread)
//*************************************************************************************************************
{
	int res=S_OK;
	AsyncProcessorThread *pIfmThread= (AsyncProcessorThread *)pThread;
	if (pIfmThread!=NULL)
	{
		pIfmThread->m_Semaphore.Create();
		pIfmThread->m_ThreadStatus=eThread_Start;

		res=pIfmThread->Dispatcher(); //Now call the main routine

		pIfmThread->m_ThreadStatus =eThread_NotInit;
		pIfmThread->m_Semaphore.Destroy();
	}
	else
	{
		res=-1;
	}
#if defined(_MSC_VER) //Microsoft specific
	return(res);
#endif
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

class Wrapper: public AsyncProcessorThread
//*************************************************************************************************************
{
public:
	Wrapper();
	~Wrapper() {};
	static void SignalEvt(void *pContext, c_ProcessingObserver *pHandler);
protected:
	virtual int Dispatcher(); //Main thread routine

	void                 *m_pEventContext;
	c_ProcessingObserver *m_pObserver;
private:
};

Wrapper::Wrapper()
//*************************************************************************************************************
{
	m_pEventContext = NULL;
	m_pObserver = NULL;
}

/*static*/ void Wrapper::SignalEvt(void *pContext, c_ProcessingObserver *pHandler)
//*************************************************************************************************************
{
	//Should store the event
	// int res=S_OK;
	if (pContext!=NULL)
	{
		Wrapper *pWrapper=(Wrapper *)pContext;
		pWrapper->m_pEventContext = pContext;
		pWrapper->m_pObserver = pHandler;
		pWrapper->m_Semaphore.Notify();
	}
}

int Wrapper::Dispatcher()
//*************************************************************************************************************
{//Main thread routine
	int res=S_OK;
	printf("\n--> Thread Wrapper start");
	while(1)
	{
		m_Semaphore.Wait();
		printf("\n--> Thread Wrapper process event");
		if (m_pObserver ==NULL)
		{
			WOM_ASSERT(0);
			continue;
		}
		m_pObserver->Execute();
	}
	printf("\n--> Thread Wrapper end");
	return(res);
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************


int main(int /*argc*/, const char ** /*argv*/)
//*************************************************************************************************************
{
	int result=0;
	AsyncProcessorEventHandler ThreadProcessorCallback;
	MyProcessor Processor;
	ProxyProcessor Proxy(Processor, "MyProxy");
	Wrapper MyWrapper;
	MyWrapper.Start("Wrapper"); // Start the listener thread
	ThreadProcessorCallback.SetExecuteFunction(ProxyProcessor::Execute_CBFromProcessor, (void *)&Proxy);
	ThreadProcessorCallback.SetNotifFunction  (Wrapper::SignalEvt , (void *)&MyWrapper);
	const unsigned int StackSize=1024;
	result=Proxy.Construct(PROTH_PRIORITY_NORMAL, StackSize, 50, sizeof(MyMsg), 50, sizeof(MyMsg), ThreadProcessorCallback);
	if(result != S_OK)
	{
		return(result);
	}

	// Processor.init(2, MyPorts, /*&myEventHandler, NULL*/ &Proxy, false /* is Invalid */);
	for (size_t nbWait=50; nbWait>0; --nbWait)
	{
		switch(global_state)
		{
		case OMX_StateIdle:
			Proxy.SetState(OMX_StateExecuting);
			break;
		case OMX_StateExecuting:
			Proxy.SetState(OMX_StateIdle);
			break;
		case State_WaitForResponse:
		case OMX_StateInvalid:
		default:
			break;
		}
		printf("\nmain %04d", nbWait);
		SleepMillisecond(100);
	}

	return(result);
}

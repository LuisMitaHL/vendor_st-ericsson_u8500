/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "wom_shared.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <omxil/OMX_Types.h>
#include <omxil/OMX_Core.h>
#include "ImgEns_Lib.h"

#include "wom_tools.h"
#include "wom_port.h"
#include "wom_queue.h"
#include "wom_fsm.h"
#include "wom_async_port.h"
#include "wom_async_processor.h"
#include "processing_thread.h"
#include "processing_message.h"
#include "processing_observer.h"
#include "wom_syncproxy.h"
#include "wom_proxy.h"


#undef    OMXCOMPONENT
#if (!defined(__SYMBIAN32__))
	#define   OMXCOMPONENT GetComponentName()
#else
	#define   OMXCOMPONENT "Async_Proxy"
#endif
#define   TRACENAME "Async_Proxy"

Async_Proxy::Async_Proxy(const char *name, ImgEns_Component &component, Interface_ProcessorToComponent &IProcessorToComponent, fnCreateAsyncProcessor _fnCreateAsyncProcessor)
	: Sync_Proxy(name, component, IProcessorToComponent)
	, c_ProcessingThread(name)
//*************************************************************************************************************
{
	m_fnCreateAsyncProcessor =_fnCreateAsyncProcessor;
	m_pProcessor             = NULL;
	m_AsyncProcessPriority   = 0;
	ImgMasterCallbackThread::staticGetInstance(); //Force the creation of the thread
	m_pObserver              = NULL;
	component.setProcessingComponent(this);
}

Async_Proxy::~Async_Proxy()
//*************************************************************************************************************
{
	destroy();
}


OMX_ERRORTYPE Async_Proxy::construct(void)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("construct\n");
	WOM_ASSERT(m_fnCreateAsyncProcessor);
	if (m_fnCreateAsyncProcessor==NULL)
		return(OMX_ErrorInvalidComponent);
	m_pObserver = new c_ProcessingObserver(m_UserName);
	m_pObserver->SetExecuteFunction(staticExecute_CBFromProcessor, (void *)this);

	unsigned int     StackSize       = 4096; // 4K by default
	unsigned int     StackInputSize  = 20;
	unsigned int     StackOutputSize = 20;
	e_ProThPriority Priority         = PROTH_PRIORITY_NORMAL;

	int res=c_ProcessingThread::Construct(Priority, StackSize, sizeof(AsyncMsg) , StackInputSize, sizeof(AsyncMsg), StackOutputSize);
	if (res != PROTH_ERROR_NONE)
	{
		WOM_ASSERT(res==PROTH_ERROR_NONE);
	}
	ImgMasterCallbackThread::staticRegisterObserver(m_pObserver, this);

	//First instantiate the processor
	m_pProcessor =m_fnCreateAsyncProcessor(GetUserName(), NULL);
	if (m_pProcessor==NULL)
	{
		WOM_ASSERT(0);
		return(OMX_ErrorInvalidComponent);
	}

	// Basic Initialisation
	res= m_pProcessor->Init((Wom_Component &)mENSComponent, mENSComponent.getPortCount(), (Interface_ProcessorCB *)this);
	if (res != S_OK)
	{
		WOM_ASSERT(0);
		return(OMX_ErrorInsufficientResources);
	}
	// Configure ports
	ImgEns_Port *pPort;
	OMX_PARAM_PORTDEFINITIONTYPE PortDef;
	for (unsigned int index=0; index < mENSComponent.getPortCount(); ++ index)
	{ // configure each port
		pPort= mENSComponent.getPort(index);
		PortDef=  pPort->getValueParamPortDefinition();
		if (PortDef.eDomain==OMX_PortDomainVideo)
		{
			m_pProcessor->SetPortConfig(index, pPort->getBufferCountCurrent(), pPort->getDirection()==OMX_DirInput?Wom_AsyncPort::eDir_Input: Wom_AsyncPort::eDir_Output, PortDef.nBufferSize, PortDef.eDomain, PortDef.bEnabled?true:false);
			m_pProcessor->SetPortFormat(index, PortDef.format.video.eColorFormat, PortDef.format.video.nFrameWidth,  PortDef.format.video.nFrameHeight, PortDef.format.video.nStride, PortDef.format.video.nSliceHeight);
		}
		else if (PortDef.eDomain==OMX_PortDomainImage)
		{
			m_pProcessor->SetPortConfig(index, pPort->getBufferCountCurrent(), pPort->getDirection()==OMX_DirInput?Wom_AsyncPort::eDir_Input: Wom_AsyncPort::eDir_Output, PortDef.nBufferSize, PortDef.eDomain, PortDef.bEnabled?true:false);
			m_pProcessor->SetPortFormat(index, PortDef.format.image.eColorFormat, PortDef.format.image.nFrameWidth,  PortDef.format.image.nFrameHeight, PortDef.format.image.nStride, PortDef.format.image.nSliceHeight);
		}
		else if ( (PortDef.eDomain==OMX_PortDomainOther) || (PortDef.eDomain==OMX_PortDomainAudio) )
		{
			m_pProcessor->SetPortConfig(index, pPort->getBufferCountCurrent(), pPort->getDirection()==OMX_DirInput?Wom_AsyncPort::eDir_Input: Wom_AsyncPort::eDir_Output, PortDef.nBufferSize, PortDef.eDomain, PortDef.bEnabled?true:false);
			m_pProcessor->SetPortFormat(index, PortDef.format.image.eColorFormat, 0 /*nFrameWidth*/,  0 /*nFrameHeight*/, 0 /*nStride*/, 0 /*nSliceHeight*/);
		}
		else
		{
			WOM_ASSERT(0); // invalid port type
			status = OMX_ErrorFormatNotDetected;
		}
	}
	// Start the fsm
	res= m_pProcessor->Start();
	if (res != S_OK)
	{
		WOM_ASSERT(0);
		return(OMX_ErrorInsufficientResources);
	}
	return(status);
}

OMX_ERRORTYPE Async_Proxy::destroy(void)
//*************************************************************************************************************
{ // member of Img_ProcessingComponent
	OMX_ERRORTYPE status=OMX_ErrorNone;
	Debug("destroy\n");
	//First User stop
	int res;
	if (m_pProcessor)
	{
		res=m_pProcessor->Stop();
		res=m_pProcessor->DeInit();

		delete m_pProcessor;
		m_pProcessor=NULL;
	}
	if ( m_pObserver)
	{
		ImgMasterCallbackThread::staticUnregisterObserver(m_pObserver, this);
		delete m_pObserver;
		m_pObserver=NULL;
	}
	// Carmello api change 
	if (c_ProcessingThread::IsConstructed()==true)
		c_ProcessingThread::Destroy();
	return(status);
}

/* static */void Async_Proxy::staticExecute_CBFromProcessor(void *pContext)
//*************************************************************************************************************
{ // Run in the proxy context pop the output queue and execute
	//printf("\nExecute CB from Async_Proxy ");
	if (!pContext)
	{
		WOM_ASSERT(0);
		return;
	}

	Async_Proxy *pProxy=(Async_Proxy*) pContext;
	AsyncMsg Evt;
	//Try to unqueue stored event and execute it
	if (pProxy->PopOutputMsg(&Evt, sizeof(Evt))!=PROTH_ERROR_NONE)
	{
		WOM_ASSERT(0);
		return;
	}
	pProxy->Execute_CBFromProcessor(Evt);
}

int Async_Proxy::Execute_CBFromProcessor(AsyncMsg &Evt)
//*************************************************************************************************************
{ // Run in the proxy context pop the output queue and execute
	switch(Evt.Event)
	{
	//case OMX_CommandStateSet:
	//	printf("SetState complete");
	//	break;
	case OMX_EventCmdComplete:
		mENSComponent.eventHandler((OMX_EVENTTYPE)Evt.Event, Evt.Param1, Evt.Param2);
		break;
	case eIMGOMX_EventCmdReceived:
		mENSComponent.eventHandler((OMX_EVENTTYPE)Evt.Event, Evt.Param1, Evt.Param2);
		break;

	case OMX_EventError:
	case OMX_EventMark:
	case OMX_EventPortSettingsChanged:
	case OMX_EventBufferFlag:
	case OMX_EventResourcesAcquired:
	case OMX_EventComponentResumed:
	case OMX_EventDynamicResourcesAvailable:
	case OMX_EventPortFormatDetected:
	case eIMGOMX_EventResourcesReceived:
		mENSComponent.eventHandler((OMX_EVENTTYPE)Evt.Event, Evt.Param1, Evt.Param2);
		break;

	case eProcessEvent_ScheduleProcess:
		ScheduleProcess(Evt.Param1); // Reschedule an asynchrone request process
		break;
	case eProcessEvent_FillBufferDone:
		mENSComponent.fillBufferDone( (OMX_BUFFERHEADERTYPE*)Evt.Param1);
		break;
	case eProcessEvent_EmptyBufferDone:
		mENSComponent.emptyBufferDone( (OMX_BUFFERHEADERTYPE*) Evt.Param1);
		break;
	default:
		//printf("Invalid msg=%d in NewSplitter_AsyncProxy::Execute_CBFromProcessor", Evt.Event);
		WOM_ASSERT(0);
		break;
	}
	return(S_OK);
}

//Interface Processor to Proxy

void Async_Proxy::NotifyOutgoingMsg()
//*************************************************************************************************************
{ // From c_workerThread. Executed in the processor thread
	m_pObserver->Notify(); // Notify the ens wrapper
}

int Async_Proxy::OmxEventFeedback(int _event, int param1, int param2)
//*************************************************************************************************************
{ // part of interface ProcessorToProxy  Interface_ProcessorCB
	AsyncMsg Evt= {_event, param1, param2};
	int res=PushOutputMsg(&Evt, sizeof(Evt));
	WOM_ASSERT(res==S_OK);
	return(res);
}

int Async_Proxy::RequestProcess(int why)
//*************************************************************************************************************
{ // Ask to Schedule a process event asynchronously
	AsyncMsg Evt= {eProcessEvent_ScheduleProcess, why, 0};
	int res=PushOutputMsg(&Evt, sizeof(Evt));
	WOM_ASSERT(res==S_OK);
	return(res);
}

int Async_Proxy::EmptyBufferDone(OMX_BUFFERHEADERTYPE *buff)
//*************************************************************************************************************
{
	AsyncMsg Evt={eProcessEvent_EmptyBufferDone, (unsigned int)buff};
	PushOutputMsg(&Evt, sizeof(Evt));
	return(S_OK);
}

int Async_Proxy::FillBufferDone(OMX_BUFFERHEADERTYPE *buff)
//*************************************************************************************************************
{
	AsyncMsg Evt={eProcessEvent_FillBufferDone, (unsigned int)buff};
	PushOutputMsg(&Evt, sizeof(Evt));
	return(S_OK);
}


// Interface ProxyToProcessor

OMX_ERRORTYPE Async_Proxy::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	AsyncMsg Evt={eProxyEvent_EmptyThisBuffer, (int)pBuffer};
	PushInputMsg(&Evt, sizeof(Evt));
	return(status);
}

OMX_ERRORTYPE Async_Proxy::fillThisBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	AsyncMsg Evt={eProxyEvent_FillThisBuffer, (int)pBuffer};
	PushInputMsg(&Evt, sizeof(Evt));
	return(status);
}

OMX_ERRORTYPE Async_Proxy::sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData)
//*************************************************************************************************************
{
	OMX_ERRORTYPE status=OMX_ErrorNone;
	AsyncMsg Evt={eProxyEvent_SendCommand, eCmd, nData};
	PushInputMsg(&Evt, sizeof(Evt));
	return(status);
}

// Async command to async processor

int Async_Proxy::SetState(unsigned int state)
//*************************************************************************************************************
{
	AsyncMsg Evt={OMX_CommandStateSet, state};
	PushInputMsg(&Evt, sizeof(Evt));
	return(S_OK);
}

int Async_Proxy::ScheduleProcess(int why)
//*************************************************************************************************************
{ //Schedule asynchronously a process
	AsyncMsg Evt={eProxyEvent_ScheduleProcess, why};
	PushInputMsg(&Evt, sizeof(Evt));
	return(S_OK);
}

int Async_Proxy::Marshall_AsyncCommand(int _event, int param1, int param2, int param3)
//*************************************************************************************************************
{ //Schedule asynchronously a process
	AsyncMsg Evt={_event, param1, param2, param3};
	PushInputMsg(&Evt, sizeof(Evt));
	return(S_OK);
}

void Async_Proxy::ExecuteIncomingMsg(void* ap_Msg)
//*************************************************************************************************************
{ // From c_workerThread. Executed in the processor thread
	AsyncMsg &Evt=*(AsyncMsg *)ap_Msg;
	switch(Evt.Event)
	{
	case OMX_CommandStateSet:
		m_pProcessor->SendAsyncCommand(Evt);
		break;
	case eProxyEvent_FillThisBuffer:
	case eProxyEvent_EmptyThisBuffer:
	case eProxyEvent_SendCommand:
		m_pProcessor->SendAsyncCommand(Evt);
		break;
	case eProxyEvent_ScheduleProcess:
		m_pProcessor->SendAsyncCommand(Evt);
		break;
	case eProxyEvent_SetConfig:
		m_pProcessor->SendAsyncCommand(Evt);
		break;

	default:
		//printf("Invalid msg=%d in Async_Proxy::ExecuteIncomingMsg", Evt.Event);
		WOM_ASSERT(0);
		break;
	}
}


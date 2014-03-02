/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "wom_shared.h"
#include <stdio.h>
#include <string.h>
#include <omxil/OMX_Core.h>
#include <omxil/OMX_IVCommon.h>
#include <omxil/OMX_Types.h>
#include <omxil/OMX_Component.h>
#include "ImgEns_Port.h"
#include "wom_queue.h"
#include "wom_fsm.h"
#include "wom_async_port.h"
#include "wom_async_processor.h"
#include "wom_port.h"
#include "wom_component.h"
#include "wom_async_port.h"

void portInformation::InitDefault()
//*******************************************************************************
{ // Set default value for port info
	width         = 0;
	height        = 0;
	colorFormat   = OMX_COLOR_FormatUnused;
	stride        = 0;
	sliceHeight   = 0;
	omxPortIndex  = 0;
	bufferCount   = 0;
	direction     = OMX_DirInput; //Port direction Input or Output
	bufferSupplier= 0;
	bufferSize    = 0;
	domain        = OMX_PortDomainVideo;
	enable        = true; // Port enabled by default
}
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
AsyncProcessor::AsyncProcessor(const char *name, void *UsrPtr)
{ 
	m_pIProcessorCB = NULL;
	m_NbPort        = 0;
	m_UserPtr       = UsrPtr;
	if (name ==NULL)
		name = "Wom_processor";
	strncpy(m_UserName, name, WOM_MAX_NAME_SIZE);
}

int AsyncProcessor::Init(unsigned int NbPort, Interface_ProcessorCB *pInterface)
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
	int status=S_OK;
	m_pIProcessorCB   = pInterface;
	m_NbPort          = NbPort;
	return(status);
}

void AsyncProcessor::SetComponentName(const char *name)
//*************************************************************************************************************
{
	strncpy(m_UserName, name, WOM_MAX_NAME_SIZE);
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************

ProcessorFSM::ProcessorFSM()
//*************************************************************************************************************
{
	m_bHavePendingEvent = false;
	m_PortCount         = 0;
	m_pPorts            = NULL;
	m_TransientState    = OMX_StateLoaded;

	m_pIProcessorCB     = NULL;
	m_pIProcessor       = NULL;
}

ProcessorFSM::~ProcessorFSM()
//*************************************************************************************************************
{
	m_Mutex.Destroy();
}

void ProcessorFSM::deliverBuffer(int portIndex, OMX_BUFFERHEADERTYPE *buf)
//*************************************************************************************************************
{
	m_pPorts[portIndex].queueBuffer((OMX_BUFFERHEADERTYPE *)buf);
	scheduleProcessEvent();
}

void ProcessorFSM::deliverBufferCheck(int portIndex, OMX_BUFFERHEADERTYPE* buf)
//*************************************************************************************************************
{
	if( m_pPorts[portIndex].isEnabled())
	{
		deliverBuffer(portIndex, buf);
	}
	else
	{
		m_pPorts[portIndex].returnUnprocessedBuffer(buf);
	}
}


void ProcessorFSM::returnBufferAsync(OMX_U32 portIdx, OMX_BUFFERHEADERTYPE * buf)
{ // Asynchronous processing : Event must be dispatched on component m_StateHandlerFunction machine to update it.
	WomOmxEvent evt;
	evt.fsmEvent.signal           = eSignal_ReturnBuffer;
	evt.args.returnBuffer.buffer  = buf;
	evt.args.returnBuffer.portIdx = portIdx;
	dispatch((WomFsmEvent*)&evt);
}

void ProcessorFSM::scheduleProcessEvent()
{
	bool need_process = false;
	m_Mutex.Lock();
	if (!(m_bHavePendingEvent))
	{
		m_bHavePendingEvent = true;
		need_process = true;
	}
	m_Mutex.Unlock();
	if (need_process)
	{
		m_pIProcessorCB->RequestProcess(eProcessEvent_BufferAvailable);
	}
}


void ProcessorFSM::processEvent()
{
	WomOmxEvent ev;
	ev.fsmEvent.signal = eSignal_Process;
	m_Mutex.Lock();
	m_bHavePendingEvent = false;
	m_Mutex.Unlock();
	dispatch((WomFsmEvent *)&ev);
}


void ProcessorFSM::sendCommand(OMX_COMMANDTYPE cmd, t_uword param)
{
	WomOmxEvent ev;
	if (  (cmd == OMX_CommandStateSet)
		||(cmd == OMX_CommandFlush)
		||(cmd == OMX_CommandPortDisable)
		||(cmd == OMX_CommandPortEnable))
	{
		m_pIProcessorCB->OmxEventFeedback(eIMGOMX_EventCmdReceived, cmd, param);
	}

	if (cmd == OMX_CommandStateSet)
	{
		ev.fsmEvent.signal = eSignal_SetState;
		ev.args.setState.state = (OMX_STATETYPE) param;
	}
	else if (cmd == OMX_CommandFlush)
	{
		ev.fsmEvent.signal = eSignal_Flush;
		WOM_ASSERT((param==OMX_ALL) || (param < (unsigned int)m_PortCount));
		ev.args.portCmd.portIdx = param;
	}
	else if (cmd == OMX_CommandPortDisable)
	{
		ev.fsmEvent.signal = eSignal_DisablePort;
		WOM_ASSERT((param==OMX_ALL) || (param < (unsigned int)m_PortCount));
		ev.args.portCmd.portIdx = param;
	}
	else if (cmd == OMX_CommandPortEnable)
	{
		ev.fsmEvent.signal = eSignal_EnablePort;
		WOM_ASSERT((param==OMX_ALL) || (param < (unsigned int)m_PortCount));
		ev.args.portCmd.portIdx = param;
	}
	else
	{
		WOM_ASSERT(0);
	}
	dispatch((WomFsmEvent *)&ev);
}


OMX_STATETYPE ProcessorFSM::getOmxState(void) const
{
	t_FsmState FsmState = WomFsm::getState();

	if      (FsmState == (t_FsmState)&ProcessorFSM::State_Idle)
		return OMX_StateIdle;
	else if (FsmState == (t_FsmState)&ProcessorFSM::State_Executing)
		return OMX_StateExecuting;
	else if (FsmState == (t_FsmState)&ProcessorFSM::State_Pause)
		return OMX_StatePause;
	else if (FsmState == (t_FsmState)&ProcessorFSM::State_TransientToIdle)
		return (OMX_STATETYPE)OMX_StateTransientToIdle;
	else if (FsmState == (t_FsmState)&ProcessorFSM::waitForPortFlushedOrDisabled)
		return this->m_TransientState;
	else
	{
		OstTraceFiltInst0(TRACE_ERROR, "ImgEns_HOST: ProcessorFSM::getState assert !!");
		WOM_ASSERT(0);
		return OMX_StateInvalid;
	}
}

bool ProcessorFSM::allPortsDisabled(void) const
//*************************************************************************************************************
{
	unsigned int i;
	for(i = 0; i < m_PortCount; i++)
	{
		if (m_pPorts[i].isEnabled() == true)
		{
			return false;
		}
	}
	return true;
}

bool ProcessorFSM::allPortsEnabled(void) const
//*************************************************************************************************************
{
	unsigned int i;
	for(i = 0; i < m_PortCount; i++)
	{
		if (m_pPorts[i].isEnabled() == false)
		{
			return false;
		}
	}
	return true;
}

bool ProcessorFSM::isOnePortFlushing(void) const
//*************************************************************************************************************
{
	unsigned int i;
	for(i = 0; i < m_PortCount; i++)
	{
		if (m_pPorts[i].isFlushing() == true)
		{
			return true;
		}
	}
	return false;
}

int ProcessorFSM::init(unsigned int portCount, Wom_AsyncPort *pPorts, Interface_ProcessorCB *pIntCallback, Interface_Processor * pIProcessor, bool invalid)
//*************************************************************************************************************
{
	int status =S_OK;
	m_PortCount          = portCount;
	m_pPorts             = pPorts;
	m_bHavePendingEvent  = false;
	m_pIProcessorCB      = pIntCallback;
	m_pIProcessor        = pIProcessor;

	this->m_TransientState = OMX_StateLoaded;
	OstTraceFiltInst0(TRACE_API, "ProcessorFSM: Enter ProcessorFSM::init function");

	if ( (m_Mutex.Create() != S_OK) || (invalid) )
	{
		WOM_ASSERT(0);
		WomFsm::init((t_FsmState)&ProcessorFSM::State_Invalid);
		return -1;
	}

	/* Look for an invalid port */
	unsigned int ValidPort=0;
	while ( (ValidPort<m_PortCount) && (!m_pPorts[ValidPort].isInvalid() ) )
		ValidPort++;

	if (ValidPort < m_PortCount)
	{
		WomFsm::init((t_FsmState)&ProcessorFSM::State_Invalid);
	}
	else
	{
		WomFsm::init((t_FsmState)&ProcessorFSM::State_Idle);
	}
	return(status);
}

bool ProcessorFSM::postProcessCheck() const
{
	for(unsigned int i = 0; i < m_PortCount; i++)
	{
		if (!m_pPorts[i].postProcessCheck())
		{
			return false;
		}
	}
	return true;
}

ProcessorFSM::t_FsmState ProcessorFSM::getStateFromPorts()
{
	unsigned int         i;
	t_FsmState   newState       = 0;
	unsigned int executingPorts          = 0;
	unsigned int idlePorts               = 0;
	unsigned int pausedPorts             = 0;
	unsigned int transientToIdlePorts    = 0;
	unsigned int transientToDisablePorts = 0;
	unsigned int flushingPorts           = 0;

	for (i = 0; i < m_PortCount; i++)
	{
		switch ((int)m_pPorts[i].getState())
		{
		case OMX_StateIdle:
			idlePorts++;
			break;
		case OMX_StatePause:
			pausedPorts++;
			break;
		case OMX_StateExecuting:
			executingPorts++;
			break;
		case OMX_StateTransientToIdle:
			transientToIdlePorts++;
			break;
		case OMX_StateTransientToDisable:
			transientToDisablePorts++;
			break;
		case OMX_StateFlushing:
			flushingPorts++;
			break;

		default:
			OstTraceFiltInst0(TRACE_ERROR, "ImgEns_HOST: ProcessorFSM::getStateFromPorts assert !!");
			WOM_ASSERT(0);
			break;
		}
	} /* for (i = 0; i < m_PortCount; i++) */

	if (executingPorts == m_PortCount)
	{
		newState = (t_FsmState) &ProcessorFSM::State_Executing;
	}
	else if (pausedPorts == m_PortCount)
	{
		newState = (t_FsmState) &ProcessorFSM::State_Pause;
	}
	else if (idlePorts == m_PortCount)
	{
		newState = (t_FsmState) &ProcessorFSM::State_Idle;
	}
	else if (idlePorts + transientToIdlePorts == m_PortCount)
	{
		newState = (t_FsmState) &ProcessorFSM::State_TransientToIdle;
	}
	else if (
		(executingPorts + transientToDisablePorts == m_PortCount)
		|| (pausedPorts + transientToDisablePorts == m_PortCount)
		|| (executingPorts + flushingPorts == m_PortCount)
		|| (pausedPorts + flushingPorts == m_PortCount))
	{
		newState = (t_FsmState) &ProcessorFSM::waitForPortFlushedOrDisabled;
	}
	else
	{
		OstTraceFiltInst0(TRACE_ERROR, "ImgEns_HOST: ProcessorFSM::getStateFromPorts m_PortCount assert !!");
		WOM_ASSERT(0);
	}
	return newState;
}


void ProcessorFSM::updateState(t_FsmState currentState)
{
	t_FsmState   newState = getStateFromPorts();
	if (newState != currentState)
	{
		m_StateHandlerFunction = (t_FsmState)newState;
	}
}

void ProcessorFSM::setState(const WomOmxEvent *evt, t_FsmState currentState)
{
	unsigned int i;
	for (i = 0; i < m_PortCount; i++)
	{
		m_pPorts[i].dispatch((WomFsmEvent *)evt);
	}
	updateState(currentState);
}

void ProcessorFSM::State_Idle(const WomOmxEvent *evt)
{// component in idle m_StateHandlerFunction => all m_pPorts are in idle m_StateHandlerFunction
	switch(OmxEvent_FSMSIGnal(evt))
	{
	case eSignal_Entry:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component IdleState FSM_ENTRY_SIG");
		stateChangeIndication(this->m_TransientState, OMX_StateIdle);
		this->m_TransientState = OMX_StateIdle;
		m_pIProcessorCB->OmxEventFeedback(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle);
		break;

	case eSignal_Exit:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component IdleState FSM_EXIT_SIG");
		break;

	case eSignal_SetState:
		setState(evt, (t_FsmState)&ProcessorFSM::State_Idle);
		break;

	case eSignal_Process:
		break;

	case eSignal_Flush:
	case eSignal_DisablePort:
	case eSignal_EnablePort:
		{
			unsigned int i;
			if(evt->args.portCmd.portIdx == OMX_ALL)
			{
				for (i = 0; i < m_PortCount; i++)
				{
					m_pPorts[i].dispatch((WomFsmEvent *)evt);
				}
			}
			else
			{
				m_pPorts[evt->args.portCmd.portIdx].dispatch((WomFsmEvent *)evt);
			}
		}
		break;

	default:
		OstTraceFiltInst0(TRACE_ERROR, "ImgEns_HOST: ProcessorFSM::State_Idle assert !!");
		WOM_ASSERT(0);
		break;
	}
}

void ProcessorFSM::State_Executing(const WomOmxEvent *evt)
{ // component in executing m_StateHandlerFunction => all m_pPorts are in executing m_StateHandlerFunction
	switch(OmxEvent_FSMSIGnal(evt))
	{
	case eSignal_Entry:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component ExecutingState FSM_ENTRY_SIG");
		if(this->m_TransientState  != OMX_StateExecuting) {
			stateChangeIndication(this->m_TransientState, OMX_StateExecuting);
			this->m_TransientState = OMX_StateExecuting;
			m_pIProcessorCB->OmxEventFeedback(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateExecuting);
			WOM_ASSERT(postProcessCheck());
		}
		//
		// we may already have some buffers to process for e.g
		// when transitionning from pause m_StateHandlerFunction
		m_pIProcessor->process();
		break;

	case eSignal_Exit:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component ExecutingState FSM_EXIT_SIG");
		break;

	case eSignal_Process:
		m_pIProcessor->process();
		WOM_ASSERT(postProcessCheck());
		break;

	case eSignal_ReturnBuffer:
		m_pPorts[evt->args.returnBuffer.portIdx].dispatch((WomFsmEvent *)evt);
		break;

	case eSignal_SetState:
		setState(evt, (t_FsmState) &ProcessorFSM::State_Executing);
		break;

	case eSignal_Flush:
	case eSignal_DisablePort:
		{
			unsigned int i;
			if(evt->args.portCmd.portIdx == OMX_ALL)
			{
				for (i = 0; i < m_PortCount; i++)
				{
					Wom_AsyncPort *port = &this->m_pPorts[i];
					port->dispatch((WomFsmEvent *)evt);
				}
			}
			else
			{
				Wom_AsyncPort *port = &this->m_pPorts[evt->args.portCmd.portIdx];
				port->dispatch((WomFsmEvent *)evt);
			}
			updateState((t_FsmState) &ProcessorFSM::State_Executing);
		}
		break;

	case eSignal_EnablePort:
		{
			unsigned int i;
			if(evt->args.portCmd.portIdx == OMX_ALL)
			{
				for (i = 0; i < m_PortCount; i++)
				{
					m_pPorts[i].dispatch((WomFsmEvent *)evt);
				}
			} 
			else 
			{
				m_pPorts[evt->args.portCmd.portIdx].dispatch((WomFsmEvent *)evt);
			}
			scheduleProcessEvent(); //Avoid race condition when enabling m_pPorts in executing m_StateHandlerFunction!!
		}
		break;

	default:
		OstTraceFiltInst0(TRACE_ERROR, "ImgEns_HOST: ProcessorFSM::State_Executing assert !!");
		WOM_ASSERT(0);
		break;
	}
}

void ProcessorFSM::State_Pause(const WomOmxEvent *evt)
{// component in pause m_StateHandlerFunction => all m_pPorts are in pause m_StateHandlerFunction
	switch(OmxEvent_FSMSIGnal(evt))
	{
	case eSignal_Entry:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component PauseState FSM_ENTRY_SIG");
		if(this->m_TransientState  != OMX_StatePause)
		{
			stateChangeIndication(this->m_TransientState, OMX_StatePause);
			this->m_TransientState  = OMX_StatePause;
			m_pIProcessorCB->OmxEventFeedback(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StatePause);
		}
		break;

	case eSignal_Exit:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component PauseState FSM_EXIT_SIG");
		break;

	case eSignal_Process:
		break;

	case eSignal_ReturnBuffer:
		{
			// no FIFO on return buffer path, so we must send them even in Pause m_StateHandlerFunction
			Wom_AsyncPort *port = &m_pPorts[evt->args.returnBuffer.portIdx];
			port->dispatch((WomFsmEvent *)evt);
		}
		break;

	case eSignal_SetState:
		setState(evt, (t_FsmState) &ProcessorFSM::State_Pause);
		break;

	case eSignal_Flush:
	case eSignal_DisablePort:
		{
			unsigned int i;
			if(evt->args.portCmd.portIdx == OMX_ALL)
			{
				for (i = 0; i < m_PortCount; i++)
				{
					Wom_AsyncPort *port = &this->m_pPorts[i];
					port->dispatch((WomFsmEvent *)evt);
				}
			} else {
				Wom_AsyncPort *port = &this->m_pPorts[evt->args.portCmd.portIdx];
				port->dispatch((WomFsmEvent *)evt);
			}
			updateState((t_FsmState) &ProcessorFSM::State_Pause);
		}
		break;

	case eSignal_EnablePort:
		{
			unsigned int i;
			if(evt->args.portCmd.portIdx == OMX_ALL)
			{
				for (i = 0; i < m_PortCount; i++)
				{
					m_pPorts[i].dispatch((WomFsmEvent *)evt);
				}
			} else {
				m_pPorts[evt->args.portCmd.portIdx].dispatch((WomFsmEvent *)evt);
			}
		}
		break;

	default:
		OstTraceFiltInst0(TRACE_ERROR, "ImgEns_HOST: ProcessorFSM::State_Pause assert !!");
		WOM_ASSERT(0);
		break;
	}
} /* State_Pause */



void ProcessorFSM::State_TransientToIdle(const WomOmxEvent *evt)
{// component in transientToIdle m_StateHandlerFunction  => all m_pPorts are either in transientToIdle or idle m_StateHandlerFunction
	switch(OmxEvent_FSMSIGnal(evt))
	{
	case eSignal_Entry:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component TransientToIdleState FSM_ENTRY_SIG");
		break;

	case eSignal_Exit:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component TransientToIdleState FSM_EXIT_SIG");
		break;

	case eSignal_ReturnBuffer:
		{
			Wom_AsyncPort *port = &m_pPorts[evt->args.returnBuffer.portIdx];
			OMX_STATETYPE currState = port->getState();

			port->dispatch((WomFsmEvent *)evt);

			if (port->getState() != currState)
			{
				updateState((t_FsmState) &ProcessorFSM::State_TransientToIdle);
			}
		}
		break;

	case eSignal_Process:
		{
			unsigned int i;
			bool needStateUpdate = false;
			for (i = 0; i < m_PortCount; i++)
			{
				Wom_AsyncPort *port = &m_pPorts[i];
				OMX_STATETYPE currState = port->getState();
				if (port->isEnabled())
				{
					port->dispatch((WomFsmEvent *)evt);
					if (port->getState() != currState)
					{
						needStateUpdate = true;
					}
				}
			}
			if (needStateUpdate)
			{
				updateState((t_FsmState) &ProcessorFSM::State_TransientToIdle);
			}
		}
		break;

	case eSignal_Flush:
		OstTraceFiltInst0(TRACE_ERROR, "ImgEns_HOST: ProcessorFSM::State_TransientToIdle FLUSH_FSMSIG assert !!");
		WOM_ASSERT(0);
		break;

	default:
		OstTraceFiltInst0(TRACE_ERROR, "ImgEns_HOST: ProcessorFSM::State_TransientToIdle assert !!");
		WOM_ASSERT(0);
		break;
	} /* switch(OmxEvent_FSMSIGnal(evt)) */
}



void ProcessorFSM::waitForPortFlushedOrDisabled(const WomOmxEvent *evt)
{// component in waitForPortFlushedOrDisabled m_StateHandlerFunction --> one port at least is in transientToDisabledState or in State_TransientToFlush
	switch(OmxEvent_FSMSIGnal(evt))
	{
	case eSignal_Entry:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component waitForPortFlushedOrDisabled FSM_ENTRY_SIG");
		break;

	case eSignal_Exit:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component waitForPortFlushedOrDisabled FSM_EXIT_SIG");
		break;

	case eSignal_Process:
		{
			unsigned int i;
			bool portStateChanged = false;

			for (i = 0; i < m_PortCount; i++)
			{
				Wom_AsyncPort *port = &this->m_pPorts[i];
				int  currState = port->getState();
				if ((currState == OMX_StateTransientToDisable) || (currState == OMX_StateFlushing))
				{
					port->dispatch((WomFsmEvent *)evt);
					if ((portStateChanged == false) && (port->getState() != currState))
					{
						portStateChanged = true;
					}
				} /* if ((currState == OMX_StateTransientToDisable) ... */
			} /* for (i = 0; i < m_PortCount; i++) */

			if(portStateChanged == true)
			{
				updateState((t_FsmState) &ProcessorFSM::waitForPortFlushedOrDisabled);
			}

			if(this->m_TransientState == OMX_StateExecuting)
			{
				m_pIProcessor->process();
				updateState((t_FsmState) &ProcessorFSM::waitForPortFlushedOrDisabled);
			}
		}
		break;
	case eSignal_ReturnBuffer:
		{
			Wom_AsyncPort *port = &this->m_pPorts[evt->args.returnBuffer.portIdx];
			// OMX_STATETYPE currState = port->getState(); // Why was this retrieved ?
			port->dispatch((WomFsmEvent *)evt);
			updateState((t_FsmState) &ProcessorFSM::waitForPortFlushedOrDisabled);
		}
		break;

	default:
		OstTraceFiltInst0(TRACE_ERROR, "ImgEns_HOST: ProcessorFSM::waitForPortFlushedOrDisabled assert !!");
		WOM_ASSERT(0);
	}
}

void ProcessorFSM::State_Invalid(const WomOmxEvent *evt)
{// component in State_Invalid m_StateHandlerFunction => Nothing else to do, something goes wrong...
	switch(OmxEvent_FSMSIGnal(evt))
	{
	case eSignal_Entry:
		OstTraceFiltInst0(TRACE_FLOW, "ImgEns_HOST: Component State_Invalid FSM_ENTRY_SIG");
		m_pIProcessorCB->OmxEventFeedback(OMX_EventError, (t_uint32)OMX_ErrorInvalidState, 0);
		break;
	case eSignal_Exit:
		break;
	default:
		break;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
//*************************************************************************************************************
Wom_Processor::Wom_Processor(const char *name, void *UsrPtr)
: AsyncProcessor(name, UsrPtr)
//*************************************************************************************************************
{
	m_pProcessorFsm     = NULL;
	m_pPort             = NULL;
	m_pPortInfo         = NULL;
	m_NbPort            = 0;
	m_bUseBufferSharing = false;

	//Old wrapper openmax framework
	// don't set any C function by default
	m_fn_UserProcessBuffer = NULL;
	m_fn_UserClose         = NULL;
	m_fn_UserOpen          = NULL;
	m_fn_UserSetConfig     = NULL;
	m_fn_UserSetTuning     = NULL;

	m_UserPtr              = NULL;

	m_pWom_Component       = NULL;
	m_InternalState        = eInternalState_Idle;
}

Wom_Processor::~Wom_Processor()
//*************************************************************************************************************
{
	DeInit();
}

int Wom_Processor::Init(Wom_Component &aComponent, unsigned int NbPort, Interface_ProcessorCB *pInterface)
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
	int status=AsyncProcessor::Init(NbPort, pInterface);
	m_pWom_Component = &aComponent;
	// WOM_ASSERT(m_pProcessorFsm==NULL);
	if (m_pProcessorFsm==NULL)
	{ //Create the fsm if required
		m_pProcessorFsm= (ProcessorFSM *)new ProcessorFSM;
	}
	Wom_AsyncPort *pPort= new Wom_AsyncPort[NbPort];
	m_pPort  = pPort;
	if ( (m_pPortInfo!=NULL) && (NbPort > m_NbPort))
	{
		delete [] m_pPortInfo;
		m_pPortInfo= NULL;
	}
	if (m_pPortInfo==NULL)
	{
		m_pPortInfo= new portInformation[NbPort];
	}
	m_NbPort = NbPort;
	m_InternalState   = eInternalState_Init;
	return(status);
}

int Wom_Processor::DeInit()
//*************************************************************************************************************
{ // Manage the state machine in asynchronous side
	int status=S_OK;
	if(m_pProcessorFsm!=NULL)
	{
		delete m_pProcessorFsm;
		m_pProcessorFsm=NULL;
	}
	m_NbPort = 0;
	if (m_pPort != NULL)
		delete [] m_pPort;
	m_pPort = NULL;
	if (m_pPortInfo)
	{
		delete [] m_pPortInfo;
		m_pPortInfo=NULL;
	}
	m_InternalState   = eInternalState_Idle;
	return(status);
}

int Wom_Processor::Start()
//*************************************************************************************************************
{ // Start the processing component  (user point of view)
	int status=S_OK;
	if (m_pProcessorFsm)
	{
		m_pProcessorFsm->init(m_NbPort, m_pPort, m_pIProcessorCB, this, false);
	}
	m_InternalState   = eInternalState_Start;
	return(status);
}


int Wom_Processor::Stop()
//*************************************************************************************************************
{ // Stop the processing component (user point of view)
	int status=S_OK;
	m_InternalState   = eInternalState_Stop;
	return(status);
}

int Wom_Processor::SetPortConfig(int portIndex, int buffercount, int direction, size_t buffersize, int domain, bool bEnabled)
//*************************************************************************************************************
{
	int status=S_OK;
	if (portIndex >= (int)m_NbPort)
	{
		WOM_ASSERT(0);
		return(-1);
	}
	if (m_pProcessorFsm==NULL)
	{
		WOM_ASSERT(0);
		return(-2);
	}

	Wom_AsyncPort *pPort=m_pPort+portIndex;

	//     init(portIdx  ,  enumDirection                                                     , *sharingPort, bufferCount, isDisabled, isTunneled)
	pPort->init(portIndex, direction==0? Wom_AsyncPort::eDir_Input: Wom_AsyncPort::eDir_Output, NULL        , buffercount, false     , false      , m_pProcessorFsm);
	portInformation* pInfo=GetPortInfo(portIndex);
	if (pInfo ==NULL)
	{
		WOM_ASSERT(0);
		return(eError_WrongPortIndex);
	}
	pInfo->omxPortIndex      = portIndex;
	pInfo->bufferCount       = buffercount;
	pInfo->direction         = (OMX_DIRTYPE)direction; //Port direction Input or Output
	pInfo->bufferSupplier    = 0;
	pInfo->bufferSize        = buffersize;
	pInfo->domain            = domain;
	pInfo->enable            = bEnabled;
	return(status);
}

int Wom_Processor::SetPortFormat(int portIndex, int colorFormat, int width, int height, int stride, int sliceHeight)
//*************************************************************************************************************
{
	int status=S_OK;
	if (portIndex >= (int)m_NbPort)
	{
		WOM_ASSERT(0);
		return(-1);
	}
	portInformation* pInfo=GetPortInfo(portIndex);
	if (pInfo ==NULL)
	{
		WOM_ASSERT(0);
		return(eError_WrongPortIndex);
	}
	pInfo->width             = width;
	pInfo->height            = height;
	pInfo->colorFormat       = (OMX_COLOR_FORMATTYPE)colorFormat;
	pInfo->stride            = stride;
	pInfo->sliceHeight       = sliceHeight;
	pInfo->omxPortIndex      = portIndex;
	return(status);
}

int Wom_Processor::SendAsyncCommand(AsyncMsg &Evt)
//*************************************************************************************************************
{
	int status=S_OK;
	WomOmxEvent omxEvent;
	switch(Evt.Event)
	{
	case eProxyEvent_SendCommand:
		switch(Evt.Param1)
		{
		case OMX_CommandStateSet:
			omxEvent.fsmEvent.signal     = ProcessorFSM::eSignal_SetState;
			omxEvent.args.setState.state = (OMX_STATETYPE)Evt.Param2;
			break;
		case OMX_CommandFlush:
			omxEvent.fsmEvent.signal     = ProcessorFSM::eSignal_Flush;
			omxEvent.args.setState.state = (OMX_STATETYPE)Evt.Param2;
			break;
		case OMX_CommandPortDisable:
			omxEvent.fsmEvent.signal     = ProcessorFSM::eSignal_DisablePort;
			omxEvent.args.portCmd.portIdx = (OMX_STATETYPE)Evt.Param2;
			break;
		case OMX_CommandPortEnable:
			omxEvent.fsmEvent.signal     = ProcessorFSM::eSignal_EnablePort;
			omxEvent.args.portCmd.portIdx= (OMX_STATETYPE)Evt.Param2;
			break;
		case OMX_CommandMarkBuffer:
			WOM_ASSERT(0); // Not supported today
			break;
		default:
			WOM_ASSERT(0);
			return(-1);
		}
		m_pIProcessorCB->OmxEventFeedback(eIMGOMX_EventCmdReceived, Evt.Param1, Evt.Param2); //Must inform clien that the cmd has been reveived
		m_pProcessorFsm->dispatch((WomFsmEvent *)&omxEvent);
		break;
	case eProxyEvent_FillThisBuffer:
		m_pProcessorFsm->deliverBuffer( ((OMX_BUFFERHEADERTYPE *)Evt.Param1)->nOutputPortIndex, (OMX_BUFFERHEADERTYPE *)Evt.Param1);
		break;
	case eProxyEvent_EmptyThisBuffer:
		m_pProcessorFsm->deliverBuffer( ((OMX_BUFFERHEADERTYPE *)Evt.Param1)->nInputPortIndex, (OMX_BUFFERHEADERTYPE *)Evt.Param1);
		break;
	case eProxyEvent_ScheduleProcess:
		m_pProcessorFsm->processEvent();
		break;
	case eProxyEvent_SetConfig:
		status = SetConfig(Evt.Param1, (void *)Evt.Param2);
		break;
	default:
		WOM_ASSERT(0);
		return(-1);
	}
	return(status);
}

int Wom_Processor::SetConfig(size_t index, void* /*opaque_ptr*/)
//*************************************************************************************************************
{ // By default unsupported index
	WOM_ASSERT(0);
	m_pIProcessorCB->OmxEventFeedback(OMX_EventError, OMX_ErrorUnsupportedIndex, index);
	return(-1);
}


int Wom_Processor::InitBufferInfo(struct sBufferInfo &Info, const int Index, bool bDequeue)
//*******************************************************************************
{ //Init BufferInfo struct and dequeueBuffer
	//WOM_ASSERT(0);
	int res=S_OK;
#if 1
	portInformation *pPortInfo=NULL;
	portInformation EnsPortInfo;
	ImgEns_Port *pPort=NULL;

	// Set input struct
	if (Index >=0)
	{
		Wom_Component *pEnsCpt=GetComponent();
		if (pEnsCpt!=NULL)
		{
			pPort=pEnsCpt->getPort(Index);
			if (pPort!=NULL)
			{
				OMX_VIDEO_PORTDEFINITIONTYPE *pDef=pPort->getVideoPortDefinition();
				if (pDef!=NULL)
				{
					EnsPortInfo.colorFormat= pDef->eColorFormat;
					EnsPortInfo.width      = pDef->nFrameWidth;
					EnsPortInfo.height     = pDef->nFrameHeight;
					EnsPortInfo.stride     = pDef->nStride;
					pPortInfo=&EnsPortInfo;
				}
			}
		}
		else
		{
			WOM_ASSERT(0);
		}
		if (pPortInfo==NULL)
		{
			pPortInfo= GetPortInfo(Index);
		}
		if (pPortInfo==NULL)
		{
			res=-1;
			InitBufferInfo(Info, -1, false);
			return(res);
		}
		if (bDequeue)
			Info.pBufferHdr = GetPort(Index)->dequeueBuffer();
		else
			Info.pBufferHdr = NULL;
		Info.Format   = pPortInfo->colorFormat;
		Info.Width    = pPortInfo->width;
		Info.Height   = pPortInfo->height;
		Info.Stride   = pPortInfo->stride;
		Info.pPort    = GetPort(Index);
		Info.Error    = 0;
		if (pPort)
			Info.pImageInfo= &((Wom_PortOther *)pPort)->GetImageInfo();
		else
			Info.pImageInfo= NULL;
	}
	else
	{ //Set default value
		pPortInfo= NULL;
		Info.pBufferHdr = NULL;
		Info.Format     = OMX_COLOR_FormatUnused;
		Info.Width      = 0;
		Info.Height     = 0;
		Info.Stride     = 0;
		Info.pPort      = NULL;
		Info.Error      = 0;
		Info.pImageInfo = NULL;
	}
#endif
	return(res);
}

int Wom_Processor::InitProcessingInfo(_tConvertionConfig &Config, bool bDequeue, const int InputIndex, const int OutputIndex, const int MetadataIndex)
//*******************************************************************************
{ //Init info struct before call Process function
	//portInformation *pInfo;
	Config.pProcessor = this;

	// memcpy( &Config.ImageConfig, &proc_config_FILTER, sizeof(proc_config_FILTER));
	Config.UserPtr      = m_UserPtr;   // Give the pointer dedicated to user
	Config.DataPtr      = NULL;        // Use for internal processing

	// Set input struct
	InitBufferInfo(Config.Input, InputIndex, bDequeue);

	// set data struct for output buffer
	InitBufferInfo(Config.Output, OutputIndex, bDequeue);

	// set data struct for output buffer
	InitBufferInfo(Config.Metadata, MetadataIndex, bDequeue);

	return(0);
}




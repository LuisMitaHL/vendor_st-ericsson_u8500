/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "wom_shared.h"
#include <stdio.h>
#include <string.h>

#include <omxil/OMX_Types.h>
#include <omxil/OMX_Core.h>
#include <omxil/OMX_IVCommon.h>

#include "wom_queue.h"
#include "wom_fsm.h"
#include "wom_async_port.h"
#include "wom_async_processor.h"

Wom_AsyncPort::Wom_AsyncPort()
//*************************************************************************************************************
{
	Default();
}

void Wom_AsyncPort::Default()
//*************************************************************************************************************
{
	m_Direction               = eDir_Input;
	m_InternalBufferSupplier  = false;
	m_IsHWport                = false;
	m_pSharingPort            = NULL;

	m_PortIndex               = 0;
	m_pComponentOwner         = NULL;
	m_OmxState_Flush          = OMX_StateInvalid;
	m_NotifyStateTransition   = true;
	m_IsPortTunneled          = false;
	m_EventAppliedOnEachPorts = false;
	m_IsBufferResetReq        = true;

	m_DequeuedCount           = 0;
}

WOM_API void Wom_AsyncPort::init(int PortIndex,  enumDirection dir, Wom_AsyncPort *pSharingPort, int bufferCount, bool isDisabled, bool isTunneled, ProcessorFSM *pComponentOwner )
{
	m_Direction              = (dir==eDir_Input)?eDir_Input: eDir_Output;
	m_pSharingPort           = pSharingPort;

	m_PortIndex              = PortIndex;
	m_pComponentOwner        = pComponentOwner;
	m_OmxState_Flush         = OMX_StateInvalid;
	m_IsPortTunneled         = isTunneled;

	// trace init
	setTraceInfo(pComponentOwner->getTraceInfoPtr(), pComponentOwner->getId1());

	if (m_pSharingPort && m_InternalBufferSupplier)
		m_DequeuedCount = (unsigned char)bufferCount;
	else
		m_DequeuedCount = 0;

	/* Init queue */
	if (m_BufferQueue.init(bufferCount, true) != 0)
	{
		WomFsm::init((t_FsmState) &Wom_AsyncPort::State_Invalid);
		return ;
	}

#if 0
	if ((!m_pSharingPort) && (bufferSupplier))
	{
		WOM_ASSERT(ap_BufferHeaderArray!=NULL);
		for (int i=0; i<bufferCount; i++)
			queueBuffer(ap_BufferHeaderArray[i]) ;
	}
#endif
	if (isDisabled)
	{
		m_OmxState_Disabled        = OMX_StateIdle;
		m_NotifyStateTransition = false;
		WomFsm::init((t_FsmState) &Wom_AsyncPort::State_Disabled);
	}
	else
	{
		m_OmxState_Disabled = OMX_StateInvalid ;
		WomFsm::init((t_FsmState) &Wom_AsyncPort::State_Idle);
	}
}


WOM_API OMX_STATETYPE Wom_AsyncPort::getState() const
{
	if (m_StateHandlerFunction == (t_FsmState)&Wom_AsyncPort::State_Idle)
		return OMX_StateIdle;
	else if (m_StateHandlerFunction == (t_FsmState)&Wom_AsyncPort::State_Executing)
		return OMX_StateExecuting;
	else if ((m_StateHandlerFunction == (t_FsmState)&Wom_AsyncPort::State_Pause) || (m_StateHandlerFunction == (t_FsmState)&Wom_AsyncPort::State_PauseFromIdleOrDisabled))
		return OMX_StatePause;
	else if (m_StateHandlerFunction == (t_FsmState)&Wom_AsyncPort::State_TransientToIdleOrDisabled)
	{
		if (m_OmxState_Disabled == OMX_StateInvalid)
			return (OMX_STATETYPE)OMX_StateTransientToIdle;
		else
			return (OMX_STATETYPE)OMX_StateTransientToDisable;
	}
	else if (m_StateHandlerFunction == (t_FsmState)&Wom_AsyncPort::State_TransientToFlush)
		return (OMX_STATETYPE)OMX_StateFlushing ;
	else if (m_StateHandlerFunction == (t_FsmState)&Wom_AsyncPort::State_Disabled)
		return m_OmxState_Disabled;
	else if (m_StateHandlerFunction == (t_FsmState)&Wom_AsyncPort::State_Invalid)
		return OMX_StateInvalid;
	else {
		OstTraceFiltInst1(TRACE_ERROR, "ImgEns_HOST: Wom_AsyncPort %d Wom_AsyncPort::getState assert !!", m_PortIndex);
		WOM_ASSERT(0);
		return OMX_StateInvalid;
	}
} /* getState */

bool Wom_AsyncPort::isFlushing(void) const
{
	if(m_OmxState_Flush != OMX_StateInvalid)
	{
		return true;
	}
	return false;
}

WOM_API void Wom_AsyncPort::returnBuffer(OMX_BUFFERHEADERTYPE * buf)
{
	WomOmxEvent evt;
	// WOM_ASSERT_MSG(m_pComponentOwner->isDispatching(), "Wom_AsyncPort::returnBuffer while ComponentOwner is not dispatching an event") ;
	evt.fsmEvent.signal = ProcessorFSM::eSignal_ReturnBuffer;
	evt.args.returnBuffer.buffer = buf;
	dispatch((WomFsmEvent*)&evt);
}

WOM_API void Wom_AsyncPort::setTunnelStatus(bool aisTunneled)
{
	m_IsPortTunneled = aisTunneled;
}

/*FIXME: temporary patch to disable resetBufferFlags call
seems to be used when buffer sharing
*/
WOM_API void Wom_AsyncPort::setBufferResetReqStatus(bool flag)
{
	m_IsBufferResetReq = flag;
}


bool Wom_AsyncPort::postProcessCheck() const
{
	if (m_IsHWport)
	{// buffers are processed by HW - no check
		return true;
	}
	if (m_pSharingPort)
	{
		if ( dequeuedBufferCount() > ( m_pSharingPort->bufferCount() - m_pSharingPort->dequeuedBufferCount() ) )
		{// all dequeued buffers not held by sharing port
			return false;
		}
	}
	else if (dequeuedBufferCount())
	{ // only sharing ports are allowed to have dequeued buffers
		return false;
	}
	return true;
}

OMX_BUFFERHEADERTYPE * Wom_AsyncPort::dequeueBuffer()
{
	m_DequeuedCount++;
	return (OMX_BUFFERHEADERTYPE *)m_BufferQueue.pop_front();
}

void Wom_AsyncPort::requeueBuffer(OMX_BUFFERHEADERTYPE * buf)
{
	m_DequeuedCount--;
	m_BufferQueue.push_front(buf);
}



//---------------------------------------------------------------------
//              Private methods
//---------------------------------------------------------------------

void resetBufferFlags(OMX_BUFFERHEADERTYPE * ptr)
{
	ptr->nFilledLen      = 0 ;
	ptr->nFlags          = 0 ;
}

WOM_API void Wom_AsyncPort::returnUnprocessedBuffer(OMX_BUFFERHEADERTYPE *buf)
{
	if (m_Direction == eDir_Input)
	{
		m_pComponentOwner->GetInterface_ProcessorCB()->EmptyBufferDone(buf);
	}
	else
	{
		m_pComponentOwner->GetInterface_ProcessorCB()->FillBufferDone(buf);
	}
}

void Wom_AsyncPort::returnBufferInternal(OMX_BUFFERHEADERTYPE * buf)
{
	m_DequeuedCount--;
	if (m_Direction == eDir_Input)
	{
		if ( (m_IsBufferResetReq) && ( m_IsPortTunneled || (m_OmxState_Flush == OMX_StateInvalid) ) )
		{
			resetBufferFlags(buf);
		}
		m_pComponentOwner->GetInterface_ProcessorCB()->EmptyBufferDone(buf);
	}
	else
	{
		m_pComponentOwner->GetInterface_ProcessorCB()->FillBufferDone(buf);
	}
}


void Wom_AsyncPort::reset()
{
	if (m_InternalBufferSupplier && m_pSharingPort)
	{
		WOM_ASSERT(dequeuedBufferCount() == bufferCount());
	}
	else if (m_InternalBufferSupplier)
	{
		WOM_ASSERT(queuedBufferCount() == bufferCount());
	}
	else
	{
		WOM_ASSERT(queuedBufferCount() == 0 && dequeuedBufferCount() == 0);
	}
	for (int i = 0; i < queuedBufferCount(); i++)
	{
		resetBufferFlags(getBuffer(i));
	}
} /* reset */


Wom_AsyncPort::t_FsmState Wom_AsyncPort::translateState(OMX_STATETYPE m_StateHandlerFunction)
{//static function
	switch (m_StateHandlerFunction)
	{
	case OMX_StateIdle:
		return (t_FsmState)&Wom_AsyncPort::State_Idle;
	case OMX_StateExecuting:
		return (t_FsmState)&Wom_AsyncPort::State_Executing;
	case OMX_StatePause:
		return (t_FsmState)&Wom_AsyncPort::State_Pause;
	default:
		WOM_ASSERT(0);
		return 0;
	}
}


void Wom_AsyncPort::flushBuffers()
{
	int i, queuedBuffer;

	OstTraceFiltInst1(TRACE_API, "ImgEns_HOST: Wom_AsyncPort %d Enter Wom_AsyncPort::flushBuffers function", m_PortIndex);

	if (m_pSharingPort && m_InternalBufferSupplier)
	{
		transferQueuedBuffersToSharingPort();
	}
	else
	{
		queuedBuffer = queuedBufferCount();
		for(i = 0; i < queuedBuffer; i++)
		{
			OMX_BUFFERHEADERTYPE * buf = dequeueBuffer();
			returnBufferInternal(buf);
		}
	}
}

void Wom_AsyncPort::State_Idle(const WomOmxEvent * evt)
{
	switch (OmxEvent_FSMSIGnal(evt))
	{
		case ProcessorFSM::eSignal_Entry:
			OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d IdleState FSM_ENTRY_SIG", m_PortIndex);
			reset();
			break;

		case ProcessorFSM::eSignal_Exit:
			{
				t_FsmState targetState = m_StateHandlerFunction;
				OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d IdleState FSM_EXIT_SIG", m_PortIndex);
				if (targetState == (t_FsmState) &Wom_AsyncPort::State_Executing
					&& m_InternalBufferSupplier
					&& m_Direction == eDir_Input)
				{ // buffer-supplier input ports must send all their buffers when transitionning from idle to executing
					flushBuffers();
				}
			}
			break;

		case ProcessorFSM::eSignal_SetState:
			{
				t_FsmState newState = translateState(evt->args.setState.state);
				WOM_ASSERT(newState != (t_FsmState) &Wom_AsyncPort::State_Idle);
				if (newState == (t_FsmState) &Wom_AsyncPort::State_Pause && m_InternalBufferSupplier && m_Direction == eDir_Input) {
					m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_PauseFromIdleOrDisabled ;
				} else {
					m_StateHandlerFunction = (t_FsmState)newState ;
				}
			}
			break;

		case ProcessorFSM::eSignal_Process:
			break;

		case ProcessorFSM::eSignal_Flush:
			if(evt->args.portCmd.portIdx != OMX_ALL)
			{
				m_pComponentOwner->GetInterface_ProcessorCB()->OmxEventFeedback(
					OMX_EventCmdComplete, OMX_CommandFlush, m_PortIndex);
			}
			else if(m_PortIndex == (unsigned)(m_pComponentOwner->getPortCount() - 1))
			{
				m_pComponentOwner->GetInterface_ProcessorCB()->OmxEventFeedback(
					OMX_EventCmdComplete, OMX_CommandFlush, OMX_ALL);
			}
			break;

		case ProcessorFSM::eSignal_DisablePort:
			if(evt->args.portCmd.portIdx == OMX_ALL)
			{
				m_EventAppliedOnEachPorts = true;
			}
			m_OmxState_Disabled    = OMX_StateIdle;
			m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_Disabled ;
			break;

		default:
			OstTraceFiltInst1(TRACE_ERROR, "ImgEns_HOST: Wom_AsyncPort %d State_Idle assert !!", m_PortIndex);
			WOM_ASSERT(0);
			break;
	} /* switch (OmxEvent_FSMSIGnal(evt)) */
} 


void Wom_AsyncPort::transferQueuedBuffersToSharingPort()
{
	int nbuf = queuedBufferCount();
	OMX_BUFFERHEADERTYPE * buf;
	for (int i = 0; i < nbuf; i++)
	{
		buf = dequeueBuffer();
		resetBufferFlags(buf); // not processed so say it is empty
		m_pSharingPort->returnBuffer((OMX_BUFFERHEADERTYPE *)buf) ;
	}
}

void Wom_AsyncPort::flushComplete(bool fromTransientToFlushState)
{
	OstTraceFiltInst1(TRACE_API, "ImgEns_HOST: Wom_AsyncPort %d enters flushComplete function", m_PortIndex);

	m_pComponentOwner->GetInterface_Processor()->flushPortIndication(m_PortIndex);
	m_OmxState_Flush = OMX_StateInvalid;

	if (m_EventAppliedOnEachPorts == false)
	{
		m_pComponentOwner->GetInterface_ProcessorCB()->OmxEventFeedback(OMX_EventCmdComplete, OMX_CommandFlush, m_PortIndex);
	}
	else if (m_pComponentOwner->isOnePortFlushing() == false)
	{
		if (fromTransientToFlushState || m_PortIndex == (unsigned)(m_pComponentOwner->getPortCount() - 1))
		{
			m_pComponentOwner->GetInterface_ProcessorCB()->OmxEventFeedback(OMX_EventCmdComplete, OMX_CommandFlush, OMX_ALL);
		}
	}
	m_EventAppliedOnEachPorts = false;
}

void Wom_AsyncPort::goToFlushBuffers()
{
	OstTraceFiltInst1(TRACE_API, "ImgEns_HOST: Wom_AsyncPort %d enters goToFlushBuffers function", m_PortIndex);
	WOM_ASSERT(m_OmxState_Flush != OMX_StateInvalid);
	if (m_InternalBufferSupplier && m_pSharingPort)
	{
		transferQueuedBuffersToSharingPort();
		if (dequeuedBufferCount() == bufferCount())
		{ // all buffers returned to sharing port so complete the flush
			flushComplete(false);
		}
		else
		{ // need to wait for missing buffers to be returned by tunneled port so go to transient m_StateHandlerFunction
			m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_TransientToFlush;
		}
	}
	else if (m_InternalBufferSupplier)
	{
		if (queuedBufferCount() == bufferCount())
		{
			// all buffers at port queue so complete the flush
			flushComplete(false);
		}
		else
		{
			// need to wait for missing buffers to be returned by tunneled port
			// so go to transient m_StateHandlerFunction
			m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_TransientToFlush;
		}
	}
	else {
		// non-supplier port
		if (dequeuedBufferCount() == 0)
		{
			// no dequeued buffers so all buffers are
			// either queued or held by tunneled port
			flushBuffers();
			flushComplete(false);
		} else {
			// need to wait for dequeued buffers to be returned before
			// completing the command, so go to transient m_StateHandlerFunction
			m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_TransientToFlush;
		}
	}
}

void Wom_AsyncPort::goToIdleOrDisable()
{

	t_FsmState targetState;
	if (m_OmxState_Disabled == OMX_StateInvalid) targetState = (t_FsmState)&Wom_AsyncPort::State_Idle;
	else targetState = (t_FsmState)&Wom_AsyncPort::State_Disabled;


	if (m_InternalBufferSupplier && m_pSharingPort)
	{

		transferQueuedBuffersToSharingPort();

		if (dequeuedBufferCount() == bufferCount())
		{
			// all buffers returned to sharing port
			m_StateHandlerFunction = targetState ;
		} else {
			m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_TransientToIdleOrDisabled ;
		}
	}
	else if (m_InternalBufferSupplier)
	{
		if (queuedBufferCount() == bufferCount())
		{
			m_StateHandlerFunction = targetState ;
		} else {
			m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_TransientToIdleOrDisabled ;
		}
	}
	else
	{
		// non-supplier port
		if (dequeuedBufferCount() == 0)
		{
			// no dequeued buffers so all buffers are
			// either queued or held by tunneled port
			flushBuffers();
			m_StateHandlerFunction = targetState ;
		}
		else
		{
			m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_TransientToIdleOrDisabled ;
		}
	}
} /* goToIdleOrDisable */


void Wom_AsyncPort::State_Executing(const WomOmxEvent * evt)
{
	switch(OmxEvent_FSMSIGnal(evt))
	{
	case ProcessorFSM::eSignal_Entry:
		OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d ExecutingState FSM_ENTRY_SIG", m_PortIndex);
		break;

	case ProcessorFSM::eSignal_Exit:
		OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d ExecutingState FSM_EXIT_SIG", m_PortIndex);
		break;

	case ProcessorFSM::eSignal_SetState:
		{
			t_FsmState newState = translateState(evt->args.setState.state);
			WOM_ASSERT(newState != (t_FsmState) &Wom_AsyncPort::State_Executing);

			if (newState == (t_FsmState) &Wom_AsyncPort::State_Idle)
			{
				goToIdleOrDisable();
			}
			else if (newState == (t_FsmState) &Wom_AsyncPort::State_Pause)
			{
				m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_Pause ;
			}
		}
		break;

	case ProcessorFSM::eSignal_ReturnBuffer:
		returnBufferInternal(evt->args.returnBuffer.buffer);
		break;

	case ProcessorFSM::eSignal_Flush:
		if(evt->args.portCmd.portIdx == OMX_ALL)
		{
			m_EventAppliedOnEachPorts = true;
		}
		m_OmxState_Flush = OMX_StateExecuting;
		goToFlushBuffers();
		break;

	case ProcessorFSM::eSignal_DisablePort:
		if(evt->args.portCmd.portIdx == OMX_ALL)
		{
			m_EventAppliedOnEachPorts = true;
		}
		m_OmxState_Disabled = OMX_StateExecuting;
		goToIdleOrDisable();
		break;

	default:
		OstTraceFiltInst1(TRACE_ERROR, "ImgEns_HOST: Wom_AsyncPort %d State_Executing assert !!", m_PortIndex);
		WOM_ASSERT(0);
		break;
	} /* switch(OmxEvent_FSMSIGnal(evt)) */
} /* State_Executing */


void Wom_AsyncPort::State_Pause(const WomOmxEvent * evt)
{
	switch(OmxEvent_FSMSIGnal(evt))
	{
	case ProcessorFSM::eSignal_Entry:
		OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d PauseState FSM_ENTRY_SIG", m_PortIndex);
		break;

	case ProcessorFSM::eSignal_Exit:
		OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d PauseState FSM_EXIT_SIG", m_PortIndex);
		break;

	case ProcessorFSM::eSignal_SetState:
		{
			t_FsmState newState = translateState(evt->args.setState.state);

			if (newState == (t_FsmState) &Wom_AsyncPort::State_Idle) goToIdleOrDisable();
			else if (newState == (t_FsmState) &Wom_AsyncPort::State_Executing) m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_Executing ;
		}
		break;

	case ProcessorFSM::eSignal_ReturnBuffer:
		returnBufferInternal(evt->args.returnBuffer.buffer) ;
		break;

	case ProcessorFSM::eSignal_Flush:
		if(evt->args.portCmd.portIdx == OMX_ALL)
		{
			m_EventAppliedOnEachPorts = true;
		}
		m_OmxState_Flush = OMX_StatePause;
		goToFlushBuffers();
		break;

	case ProcessorFSM::eSignal_DisablePort:
		if(evt->args.portCmd.portIdx == OMX_ALL)
		{
			m_EventAppliedOnEachPorts = true;
		}
		m_OmxState_Disabled = OMX_StatePause;
		goToIdleOrDisable();
		break;

	default:
		OstTraceFiltInst1(TRACE_ERROR, "ImgEns_HOST: Wom_AsyncPort %d State_Pause assert !!", m_PortIndex);
		WOM_ASSERT(0);
		break;
	} /* switch(OmxEvent_FSMSIGnal(evt)) */
} /* State_Pause */


void Wom_AsyncPort::State_PauseFromIdleOrDisabled(const WomOmxEvent * evt)
{
	switch(OmxEvent_FSMSIGnal(evt))
	{
	case ProcessorFSM::eSignal_Entry:
		OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d State_PauseFromIdleOrDisabled FSM_ENTRY_SIG", m_PortIndex);
		WOM_ASSERT(m_InternalBufferSupplier && m_Direction == eDir_Input);
		break;

	case ProcessorFSM::eSignal_Exit:
		{
			t_FsmState targetState = m_StateHandlerFunction;
			OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d State_PauseFromIdleOrDisabled FSM_EXIT_SIG", m_PortIndex);
			if (targetState == (t_FsmState) &Wom_AsyncPort::State_Executing)
			{
				// buffer-supplier input ports must send all their
				// buffers when transitionning from idle to executing
				flushBuffers();
			}
		}
		break;

	case ProcessorFSM::eSignal_SetState:
		{
			t_FsmState newState = translateState(evt->args.setState.state);

			if (newState == (t_FsmState) &Wom_AsyncPort::State_Idle) goToIdleOrDisable();
			else if (newState == (t_FsmState) &Wom_AsyncPort::State_Executing) m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_Executing ;
		}
		break;

	case ProcessorFSM::eSignal_ReturnBuffer:
		returnBufferInternal(evt->args.returnBuffer.buffer);
		break;

	case ProcessorFSM::eSignal_Flush:
		if(evt->args.portCmd.portIdx != OMX_ALL)
		{
			m_pComponentOwner->GetInterface_ProcessorCB()->OmxEventFeedback(
				OMX_EventCmdComplete, OMX_CommandFlush, m_PortIndex);
		}
		else if (m_PortIndex == (unsigned)(m_pComponentOwner->getPortCount() - 1))
		{
			m_pComponentOwner->GetInterface_ProcessorCB()->OmxEventFeedback(
				OMX_EventCmdComplete, OMX_CommandFlush, OMX_ALL);
		}
		break;

	case ProcessorFSM::eSignal_DisablePort:
		if(evt->args.portCmd.portIdx == OMX_ALL)
		{
			m_EventAppliedOnEachPorts = true;
		}
		m_OmxState_Disabled = OMX_StatePause;
		m_StateHandlerFunction = (t_FsmState)&Wom_AsyncPort::State_Disabled ;
		break;

	default:
		OstTraceFiltInst1(TRACE_ERROR, "ImgEns_HOST: Wom_AsyncPort %d State_PauseFromIdleOrDisabled assert !!", m_PortIndex);
		WOM_ASSERT(0);
		break;
	}
} /* State_PauseFromIdleOrDisabled */


void Wom_AsyncPort::State_TransientToIdleOrDisabled(const WomOmxEvent * evt)
{
	t_FsmState targetState;
	if (m_OmxState_Disabled == OMX_StateInvalid) targetState = (t_FsmState)&Wom_AsyncPort::State_Idle;
	else targetState = (t_FsmState)&Wom_AsyncPort::State_Disabled;

	switch(OmxEvent_FSMSIGnal(evt))
	{
		case ProcessorFSM::eSignal_Entry:
			OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d State_TransientToIdleOrDisabled FSM_ENTRY_SIG", m_PortIndex);
			break;

		case ProcessorFSM::eSignal_Exit:
			OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d State_TransientToIdleOrDisabled FSM_EXIT_SIG", m_PortIndex);
			break;

		case ProcessorFSM::eSignal_Process:
			if (m_InternalBufferSupplier && m_pSharingPort)
			{

				transferQueuedBuffersToSharingPort();
				if (dequeuedBufferCount() == bufferCount())
				{
					// all buffers returned to sharing port
					m_StateHandlerFunction = targetState ;
				}
			}
			else if (m_InternalBufferSupplier)
			{
				if (queuedBufferCount() == bufferCount())
				{
					// all buffers are queued
					m_StateHandlerFunction = targetState ;
				}
			}
			break;

		case ProcessorFSM::eSignal_ReturnBuffer:
			if (m_InternalBufferSupplier)
			{
				// dequeued buffers on buffer supplier sharing ports
				// should never occur since the port reuses buffers from its sharing port
				WOM_ASSERT(!m_pSharingPort);
				// buffer is queued since we are transitionning to idle
				queueBuffer((OMX_BUFFERHEADERTYPE *)evt->args.returnBuffer.buffer);
				if (queuedBufferCount() == bufferCount())
				{ // all buffers are queued
					m_StateHandlerFunction = targetState ;
				}
			}
			else 
			{
				returnBufferInternal(evt->args.returnBuffer.buffer);
				if (dequeuedBufferCount() == 0)
				{
					// all buffers are either queued or held by tunneled port
					flushBuffers();
					m_StateHandlerFunction = targetState ;
				}
			}
			break;

		default:
			OstTraceFiltInst1(TRACE_ERROR, "ImgEns_HOST: Wom_AsyncPort %d State_TransientToIdleOrDisabled assert !!", m_PortIndex);
			WOM_ASSERT(0);
			break;
	} /* switch(OmxEvent_FSMSIGnal(evt)) { */
} /* State_TransientToIdleOrDisabled */


void Wom_AsyncPort::State_TransientToFlush(const WomOmxEvent * evt)
{
	switch(OmxEvent_FSMSIGnal(evt)) {
		case ProcessorFSM::eSignal_Entry:
			OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d State_TransientToFlush FSM_ENTRY_SIG", m_PortIndex);
			break;

		case ProcessorFSM::eSignal_Exit:
			OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d State_TransientToFlush FSM_EXIT_SIG", m_PortIndex);
			break;

		case ProcessorFSM::eSignal_Process:
			if (m_InternalBufferSupplier && m_pSharingPort)
			{
				transferQueuedBuffersToSharingPort();
				if (dequeuedBufferCount() == bufferCount())
				{
					// all buffers returned to sharing port so complete the command
					m_StateHandlerFunction = translateState(m_OmxState_Flush);
					flushComplete(true);
				}
			}
			else if (m_InternalBufferSupplier) 
			{
				if (queuedBufferCount() == bufferCount())
				{
					// all buffers are at port queue so complete the command
					m_StateHandlerFunction = translateState(m_OmxState_Flush);
					flushComplete(true);
				}
			}
			break;

		case ProcessorFSM::eSignal_ReturnBuffer:
			returnBufferInternal(evt->args.returnBuffer.buffer) ;
			if (dequeuedBufferCount() == 0)
			{ // all buffers are either queued or held by tunneled port so complete the command
				flushBuffers();
				m_StateHandlerFunction = translateState(m_OmxState_Flush);
				flushComplete(true);
			}
			break;

		default:
			OstTraceFiltInst1(TRACE_ERROR, "ImgEns_HOST: Wom_AsyncPort %d State_TransientToFlush assert !!", m_PortIndex);
			WOM_ASSERT(0);
			break;
	}
} /* State_TransientToFlush */


void Wom_AsyncPort::State_Disabled(const WomOmxEvent * evt)
{
	switch (OmxEvent_FSMSIGnal(evt))
	{
		case ProcessorFSM::eSignal_Entry:
			OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d DisabledState FSM_ENTRY_SIG", m_PortIndex);

			if(m_NotifyStateTransition)
			{
				reset();
				m_BufferQueue.setEnabled(false) ;
				m_pComponentOwner->GetInterface_Processor()->disablePortIndication(m_PortIndex);
				if(m_EventAppliedOnEachPorts == false)
				{
					m_pComponentOwner->GetInterface_ProcessorCB()->OmxEventFeedback( OMX_EventCmdComplete, OMX_CommandPortDisable, m_PortIndex);
				}
				else if (m_pComponentOwner->allPortsDisabled())
				{
					m_pComponentOwner->GetInterface_ProcessorCB()->OmxEventFeedback( OMX_EventCmdComplete, OMX_CommandPortDisable, OMX_ALL);
				}
				m_EventAppliedOnEachPorts = false;
			}
			else
			{
				m_BufferQueue.setEnabled(false) ;
				m_NotifyStateTransition = true;
			}
			break;

		case ProcessorFSM::eSignal_Exit:
			{
				t_FsmState targetState = m_StateHandlerFunction;
				if (targetState == (t_FsmState) &Wom_AsyncPort::State_Executing && m_InternalBufferSupplier && m_Direction == eDir_Input)
				{// buffer-supplier input ports must send all their buffers when transitionning from idle to executing
					flushBuffers();
				}

				m_pComponentOwner->GetInterface_Processor()->enablePortIndication(m_PortIndex);

				if(m_EventAppliedOnEachPorts == false)
				{
					m_pComponentOwner->GetInterface_ProcessorCB()->OmxEventFeedback(OMX_EventCmdComplete, OMX_CommandPortEnable, m_PortIndex);
				}
				else if (m_pComponentOwner->allPortsEnabled())
				{
					m_pComponentOwner->GetInterface_ProcessorCB()->OmxEventFeedback(OMX_EventCmdComplete, OMX_CommandPortEnable, OMX_ALL);
				}
				m_EventAppliedOnEachPorts = false;
				OstTraceFiltInst1(TRACE_FLOW, "ImgEns_HOST: Wom_AsyncPort %d DisabledState FSM_EXIT_SIG", m_PortIndex);
			}
			break;

		case ProcessorFSM::eSignal_SetState:
			{
				WOM_ASSERT(evt->args.setState.state != m_OmxState_Disabled);
				m_OmxState_Disabled = evt->args.setState.state;
			}
			break;

		case ProcessorFSM::eSignal_EnablePort:
			{
				t_FsmState newState = translateState(m_OmxState_Disabled);
				m_OmxState_Disabled = OMX_StateInvalid;

				if(evt->args.portCmd.portIdx == OMX_ALL)
				{
					m_EventAppliedOnEachPorts = true;
				}

				m_BufferQueue.setEnabled(true) ;
				if (newState == (t_FsmState) &Wom_AsyncPort::State_Pause && m_InternalBufferSupplier && m_Direction == eDir_Input)
				{
					m_StateHandlerFunction = (t_FsmState) &Wom_AsyncPort::State_PauseFromIdleOrDisabled ;
				}
				else
				{
					if (newState == (t_FsmState) &Wom_AsyncPort::State_Executing && m_InternalBufferSupplier && m_Direction == eDir_Input)
					{
						flushBuffers();
					}
					m_StateHandlerFunction = newState;
				}
			}
			break;

		default:
			OstTraceFiltInst1(TRACE_ERROR, "ImgEns_HOST: Wom_AsyncPort %d State_Disabled assert !!", m_PortIndex);
			WOM_ASSERT(0);
			break;
	}
} /* State_Disabled */


void Wom_AsyncPort::State_Invalid(const WomOmxEvent *evt)
{
	switch(OmxEvent_FSMSIGnal(evt))
	{
		case ProcessorFSM::eSignal_Entry:
			OstTraceFiltInst1(TRACE_ERROR, "ImgEns_HOST: Wom_AsyncPort %d State_Invalid assert !!", m_PortIndex);
			break;

		case ProcessorFSM::eSignal_Exit:
			break;

		default:
			break ;
	}
} /* State_Invalid */


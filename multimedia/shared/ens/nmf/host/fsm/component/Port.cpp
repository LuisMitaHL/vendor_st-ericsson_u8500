/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Port.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "Port.h"
#include "Component.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ens_nmf_host_fsm_component_PortTraces.h"
#endif

//---------------------------------------------------------------------
//              Public methods
//---------------------------------------------------------------------

ENS_API_EXPORT void Port::init(
        Direction dir,
        bool bufferSupplier,
        bool isHWport,
        Port *sharingPort,
        OMX_BUFFERHEADERTYPE** buffers,
        int bufferCount,
        NMF::InterfaceReference *nmfitf,
        int portIdx,
        bool isDisabled,
		bool isTunneled,
        Component *componentOwner
    )
{

    this->direction                 = dir;
    this->isBufferSupplier          = bufferSupplier;
    this->isHWport                  = isHWport;
    this->sharingPort               = sharingPort;

    this->portIdx                   = portIdx;
    this->componentOwner            = componentOwner;
    this->flush_State         	    = OMX_StateInvalid;
    this->notifyStateTransition     = true;
	this->isPortTunneled		    = isTunneled;
    this->eventAppliedOnEachPorts   = false;
	this->isBufferResetReq		= true;

	// trace init
	setTraceInfo(componentOwner->getTraceInfoPtr(), componentOwner->getId1());

	OstTraceFiltInst3(TRACE_API, "ENS_HOST: Port %d Enter Port::init funtion bufferSupplier %d sharingPort Ptr 0x%x", portIdx, bufferSupplier, (unsigned int)sharingPort);

    if (this->sharingPort && this->isBufferSupplier) this->dequeuedbuffercount = bufferCount;
    else this->dequeuedbuffercount = 0;

	if (this->direction == InputPort) {
		this->nmfItf.armnmf_fillthisbuffer = (Iarmnmf_fillthisbuffer *)nmfitf ;
	} else {
		this->nmfItf.armnmf_emptythisbuffer = (Iarmnmf_emptythisbuffer *)nmfitf ;
	}

	/* Init queue */
	if (bufferQueue.init(bufferCount, true) != 0) {
		FSM::init((FSM_State) &Port::invalidState);
		return ;
	}

	if ((!this->sharingPort) && (bufferSupplier)) {
		for (int i=0; i<bufferCount; i++) queueBuffer(buffers[i]) ;
	}

    if (isDisabled) {
		disabled_State = OMX_StateIdle;
        notifyStateTransition = false;
        FSM::init((FSM_State) &Port::disabledState);
    } else {
		disabled_State = OMX_StateInvalid ;
        FSM::init((FSM_State) &Port::idleState);
    }
} /* init */


ENS_API_EXPORT OMX_STATETYPE Port::getState() {
	FSM_State state = FSM::getState() ;

	if      (state == (FSM_State)&Port::idleState)		 return OMX_StateIdle;
    else if (state == (FSM_State)&Port::executingState) return OMX_StateExecuting;
    else if ((state == (FSM_State)&Port::pauseState) || (state == (FSM_State)&Port::pauseFromIdleOrDisabledState)) return OMX_StatePause;
    else if (state == (FSM_State)&Port::transientToIdleOrDisabledState)  {
		if (disabled_State == OMX_StateInvalid) return OMX_StateTransientToIdle;
        else return OMX_StateTransientToDisable;
    } else if (state == (FSM_State)&Port::transientToFlushState) return OMX_StateFlushing ;
	else if (state == (FSM_State)&Port::disabledState) return disabled_State;
	else if (state == (FSM_State)&Port::invalidState)  return OMX_StateInvalid;
    else {
		OstTraceFiltInst1(TRACE_ERROR, "ENS_HOST: Port %d Port::getState assert !!", this->portIdx);
        ARMNMF_DBC_ASSERT(0);
        return OMX_StateInvalid;
    }
} /* getState */

bool Port::isFlushing(void){
    if(this->flush_State != OMX_StateInvalid) {
        return true;
    }
    return false;
}

ENS_API_EXPORT void Port::returnBuffer(OMX_BUFFERHEADERTYPE * buf) {
    	OmxEvent evt;
		ARMNMF_DBC_ASSERT_MSG(componentOwner->isDispatching(), "Port::returnBuffer while ComponentOwner is not dispatching an event") ;

    	evt.fsmEvent.signal = (FSM_FSMSIGnal)OMX_RETURNBUFFER_FSMSIG;
    	evt.args.returnBuffer.buffer = buf;
    	dispatch((FsmEvent*)&evt);
} /* returnBuffer */

ENS_API_EXPORT void Port::setTunnelStatus(bool isTunneled) {
	isPortTunneled = isTunneled;
}

/*FIXME: temporary patch to disable Buffer_reset call
To be removed after Wakeup camp */
ENS_API_EXPORT void Port::setBufferResetReqStatus(bool flag){
	isBufferResetReq = flag;
}


bool Port::postProcessCheck() {
    if (this->isHWport) {
        // buffers are processed by HW - no check
        return true;
    }
    if (this->sharingPort) {
        if (dequeuedBufferCount() > sharingPort->bufferCount() - sharingPort->dequeuedBufferCount()) {
            // all dequeued buffers not held by sharing port
            return false;
        }
    }
    else if (dequeuedBufferCount()) {
        // only sharing ports are allowed to have dequeued buffers
        return false;
    }

    return true;
} /* postProcessCheck */


//---------------------------------------------------------------------
//              Private methods
//---------------------------------------------------------------------

static inline void Buffer_reset(OMX_BUFFERHEADERTYPE_p ptr) {
    OMX_BUFFERHEADERTYPE * tptr = (OMX_BUFFERHEADERTYPE *) ptr ;

    tptr->nFilledLen      = 0 ;
    tptr->nFlags          = 0 ;
} /* Buffer_reset */


void Port::returnBufferInternalNmf(OMX_BUFFERHEADERTYPE_p buf) {

  if (this->direction == InputPort) {
	this->nmfItf.armnmf_fillthisbuffer->fillThisBuffer(buf) ;
  } else {
	this->nmfItf.armnmf_emptythisbuffer->emptyThisBuffer(buf);
  }
} /* returnBufferInternalNmf */


void Port::returnBufferInternal(OMX_BUFFERHEADERTYPE_p buf) {
    this->dequeuedbuffercount--;

    if (this->direction == InputPort) {
	  if(this->isBufferResetReq){
		if(this->isPortTunneled || (this->flush_State == OMX_StateInvalid)){
		  Buffer_reset(buf);
		}
	  }
	  this->nmfItf.armnmf_fillthisbuffer->fillThisBuffer(buf) ;
	} else {
	  this->nmfItf.armnmf_emptythisbuffer->emptyThisBuffer(buf);
	}
} /* returnBufferInternal */


void Port::reset() {
    int i;

    if (this->isBufferSupplier && this->sharingPort) {
		ARMNMF_DBC_PRECONDITION(dequeuedBufferCount() == bufferCount());
    }
    else if (this->isBufferSupplier) {
		ARMNMF_DBC_PRECONDITION(queuedBufferCount() == bufferCount());
    }
    else {
		ARMNMF_DBC_PRECONDITION(queuedBufferCount() == 0 && dequeuedBufferCount() == 0);
    }

    for (i = 0; i < queuedBufferCount(); i++) {
        Buffer_reset(getBuffer(i));
    }
} /* reset */


FSM_State Port::translateState(OMX_STATETYPE state) {
    switch (state) {
        case OMX_StateIdle:         return (FSM_State)&Port::idleState;
        case OMX_StateExecuting:    return (FSM_State)&Port::executingState;
        case OMX_StatePause:        return (FSM_State)&Port::pauseState;

        default:
            ARMNMF_DBC_ASSERT(0);
            return 0;
    } /* switch (state) */
} /* translateState */


void Port::flushBuffers() {
    int i, queuedBuffer;

	OstTraceFiltInst1(TRACE_API, "ENS_HOST: Port %d Enter Port::flushBuffers function", this->portIdx);

    if (sharingPort && isBufferSupplier) {
      transferQueuedBuffersToSharingPort();
    } 
    else {
      queuedBuffer = queuedBufferCount();
      
      for(i = 0; i < queuedBuffer; i++) {
        OMX_BUFFERHEADERTYPE_p buf = dequeueBuffer();
	 	returnBufferInternal(buf);
      }
	}
} /* flushBuffers */



void Port::idleState(const OmxEvent * evt) {
    switch (OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d IdleState FSM_ENTRY_SIG", this->portIdx);
            reset();
            break;

        case FSM_EXIT_FSMSIG:
            {
                FSM_State targetState = this->state;
				OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d IdleState FSM_EXIT_SIG", this->portIdx);
                if (targetState == (FSM_State) &Port::executingState
                        && this->isBufferSupplier
                        && this->direction == InputPort) {
                    // buffer-supplier input ports must send all their
                    // buffers when transitionning from idle to executing
                    flushBuffers();
                }
            }
            break;

        case OMX_SETSTATE_FSMSIG:
            {
                FSM_State newState = translateState(evt->args.setState.state);
                ARMNMF_DBC_PRECONDITION(newState != (FSM_State) &Port::idleState);
                if (newState == (FSM_State) &Port::pauseState && this->isBufferSupplier && this->direction == InputPort) {
					this->state = (FSM_State)&Port::pauseFromIdleOrDisabledState ;
                } else {
					this->state = (FSM_State)newState ;
                }
            }
            break;

        case OMX_PROCESS_FSMSIG:
            break;

		case OMX_FLUSH_FSMSIG:
            if(evt->args.portCmd.portIdx != OMX_ALL) {
                componentOwner->getEventHandler()->eventHandler(
                        OMX_EventCmdComplete, OMX_CommandFlush, this->portIdx);
            } 
            else if(this->portIdx == (unsigned)(componentOwner->getPortCount() - 1)) {
                componentOwner->getEventHandler()->eventHandler(
                        OMX_EventCmdComplete, OMX_CommandFlush, OMX_ALL);
            }
            break;

        case OMX_DISABLE_PORT_FSMSIG:
            if(evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
            disabled_State = OMX_StateIdle;
            this->state = (FSM_State)&Port::disabledState ;
            break;

        default:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_HOST: Port %d idleState assert !!", this->portIdx);
            ARMNMF_DBC_ASSERT(0);
            break;
    } /* switch (OmxEvent_FSMSIGnal(evt)) */
} /* idleState */


void Port::transferQueuedBuffersToSharingPort() {
    int nbuf = queuedBufferCount();
    OMX_BUFFERHEADERTYPE_p buf;

    for (int i = 0; i < nbuf; i++) {
        buf = dequeueBuffer();
		// not processed so say it is empty
		Buffer_reset(buf);
        sharingPort->returnBuffer((OMX_BUFFERHEADERTYPE *)buf) ;
    }
} /* transferQueuedBuffersToSharingPort */

void
Port::flushComplete(bool fromTransientToFlushState) {

	OstTraceFiltInst1(TRACE_API, "ENS_HOST: Port %d enters flushComplete function", this->portIdx);

    componentOwner->flushPortIndication(portIdx);
    flush_State = OMX_StateInvalid;

    if (eventAppliedOnEachPorts == false) {
        componentOwner->getEventHandler()->eventHandler(OMX_EventCmdComplete, OMX_CommandFlush, portIdx);
    } 
    else if (componentOwner->isOnePortFlushing() == false) {
        if (fromTransientToFlushState || portIdx == (unsigned)(componentOwner->getPortCount() - 1)) {
            componentOwner->getEventHandler()->eventHandler(OMX_EventCmdComplete, OMX_CommandFlush, OMX_ALL);
        }
    }
    eventAppliedOnEachPorts = false;
}

void Port::goToFlushBuffers() {
    
	OstTraceFiltInst1(TRACE_API, "ENS_HOST: Port %d enters goToFlushBuffers function", this->portIdx);

    ARMNMF_DBC_ASSERT(flush_State != OMX_StateInvalid);

    if (isBufferSupplier && sharingPort) {

        transferQueuedBuffersToSharingPort();

        if (dequeuedBufferCount() == bufferCount()) {
            // all buffers returned to sharing port so complete the flush
            flushComplete(false);
        } else {
            // need to wait for missing buffers to be returned by tunneled port
            // so go to transient state
            state = (FSM_State)&Port::transientToFlushState;
        } 
    } 
    else if (isBufferSupplier) {
        if (queuedBufferCount() == bufferCount()) {
            // all buffers at port queue so complete the flush
            flushComplete(false);
        } else {
            // need to wait for missing buffers to be returned by tunneled port
            // so go to transient state
            state = (FSM_State)&Port::transientToFlushState;
        }
    }
    else { 
        // non-supplier port
        if (dequeuedBufferCount() == 0) {
            // no dequeued buffers so all buffers are 
            // either queued or held by tunneled port
            flushBuffers();
            flushComplete(false);
        } else {
            // need to wait for dequeued buffers to be returned before
            // completing the command, so go to transient state
            state = (FSM_State)&Port::transientToFlushState;
        }
    }
}

void Port::goToIdleOrDisable() {

	FSM_State targetState;
	if (disabled_State == OMX_StateInvalid) targetState = (FSM_State)&Port::idleState;
	else targetState = (FSM_State)&Port::disabledState;


    if (this->isBufferSupplier && this->sharingPort) {

        transferQueuedBuffersToSharingPort();

        if (dequeuedBufferCount() == bufferCount()) {
            // all buffers returned to sharing port
			this->state = targetState ;
        } else {
			this->state = (FSM_State)&Port::transientToIdleOrDisabledState ;
        }
    }
    else if (this->isBufferSupplier) {
        if (queuedBufferCount() == bufferCount()) {
            this->state = targetState ;
        } else {
            this->state = (FSM_State)&Port::transientToIdleOrDisabledState ;
        }
    }
    else {
        // non-supplier port
        if (dequeuedBufferCount() == 0) {
            // no dequeued buffers so all buffers are
            // either queued or held by tunneled port
            flushBuffers();
            this->state = targetState ;
        } else {
            this->state = (FSM_State)&Port::transientToIdleOrDisabledState ;
        }
    }
} /* goToIdleOrDisable */


void Port::executingState(const OmxEvent * evt) {
    switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d ExecutingState FSM_ENTRY_SIG", this->portIdx);
            break;

        case FSM_EXIT_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d ExecutingState FSM_EXIT_SIG", this->portIdx);
            break;

        case OMX_SETSTATE_FSMSIG:
            {
                FSM_State newState = translateState(evt->args.setState.state);
                ARMNMF_DBC_PRECONDITION(newState != (FSM_State) &Port::executingState);

                if (newState == (FSM_State) &Port::idleState) {
					goToIdleOrDisable();
                }
                else if (newState == (FSM_State) &Port::pauseState) {
					this->state = (FSM_State)&Port::pauseState ;
                }
            }
            break;

        case OMX_RETURNBUFFER_FSMSIG:
            returnBufferInternal(evt->args.returnBuffer.buffer);
            break;

        case OMX_FLUSH_FSMSIG:
            if(evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
			flush_State = OMX_StateExecuting;
			goToFlushBuffers();
            break;

        case OMX_DISABLE_PORT_FSMSIG:
            if(evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
            disabled_State = OMX_StateExecuting;
            goToIdleOrDisable();
            break;

        default:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_HOST: Port %d executingState assert !!", this->portIdx);
            ARMNMF_DBC_ASSERT(0);
            break;
    } /* switch(OmxEvent_FSMSIGnal(evt)) */
} /* executingState */


void Port::pauseState(const OmxEvent * evt) {
    switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d PauseState FSM_ENTRY_SIG", this->portIdx);
            break;

        case FSM_EXIT_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d PauseState FSM_EXIT_SIG", this->portIdx);
            break;

        case OMX_SETSTATE_FSMSIG:
            {
                FSM_State newState = translateState(evt->args.setState.state);

                if (newState == (FSM_State) &Port::idleState) goToIdleOrDisable();
                else if (newState == (FSM_State) &Port::executingState) this->state = (FSM_State)&Port::executingState ;
            }
            break;

        case OMX_RETURNBUFFER_FSMSIG:
            returnBufferInternal(evt->args.returnBuffer.buffer) ;
            break;

        case OMX_FLUSH_FSMSIG:
            if(evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
        	flush_State = OMX_StatePause;
        	goToFlushBuffers();
            break;

        case OMX_DISABLE_PORT_FSMSIG:
            if(evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
			disabled_State = OMX_StatePause;
			goToIdleOrDisable();
			break;

        default:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_HOST: Port %d pauseState assert !!", this->portIdx);
            ARMNMF_DBC_ASSERT(0);
            break;
    } /* switch(OmxEvent_FSMSIGnal(evt)) */
} /* pauseState */


void Port::pauseFromIdleOrDisabledState(const OmxEvent * evt) {
    switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d pauseFromIdleOrDisabledState FSM_ENTRY_SIG", this->portIdx);
            ARMNMF_DBC_ASSERT(this->isBufferSupplier && this->direction == InputPort);
            break;

        case FSM_EXIT_FSMSIG:
            {
                FSM_State targetState = this->state;
				OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d pauseFromIdleOrDisabledState FSM_EXIT_SIG", this->portIdx);
                if (targetState == (FSM_State) &Port::executingState) {
                    // buffer-supplier input ports must send all their
                    // buffers when transitionning from idle to executing
                    flushBuffers();
                }
            }
            break;

        case OMX_SETSTATE_FSMSIG:
            {
                FSM_State newState = translateState(evt->args.setState.state);

                if (newState == (FSM_State) &Port::idleState) goToIdleOrDisable();
                else if (newState == (FSM_State) &Port::executingState) this->state = (FSM_State)&Port::executingState ;
            }
            break;

        case OMX_RETURNBUFFER_FSMSIG:
            returnBufferInternal(evt->args.returnBuffer.buffer);
            break;

        case OMX_FLUSH_FSMSIG:
            if(evt->args.portCmd.portIdx != OMX_ALL) {
                componentOwner->getEventHandler()->eventHandler(
                        OMX_EventCmdComplete, OMX_CommandFlush, this->portIdx);
            } 
            else if (this->portIdx == (unsigned)(componentOwner->getPortCount() - 1)) {
                componentOwner->getEventHandler()->eventHandler(
                        OMX_EventCmdComplete, OMX_CommandFlush, OMX_ALL);
            }
            break;

        case OMX_DISABLE_PORT_FSMSIG:
            if(evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
            disabled_State = OMX_StatePause;
            this->state = (FSM_State)&Port::disabledState ;
            break;

        default:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_HOST: Port %d pauseFromIdleOrDisabledState assert !!", this->portIdx);
            ARMNMF_DBC_ASSERT(0);
            break;
    }
} /* pauseFromIdleOrDisabledState */


void Port::transientToIdleOrDisabledState(const OmxEvent * evt) {

	FSM_State targetState;
	if (disabled_State == OMX_StateInvalid) targetState = (FSM_State)&Port::idleState;
	else targetState = (FSM_State)&Port::disabledState;

	switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d transientToIdleOrDisabledState FSM_ENTRY_SIG", this->portIdx);
            break;

        case FSM_EXIT_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d transientToIdleOrDisabledState FSM_EXIT_SIG", this->portIdx);
            break;

        case OMX_PROCESS_FSMSIG:
            if (this->isBufferSupplier && this->sharingPort) {

                transferQueuedBuffersToSharingPort();
                if (dequeuedBufferCount() == bufferCount()) {
                    // all buffers returned to sharing port
					this->state = targetState ;
                }
            }
            else if (this->isBufferSupplier) {
                if (queuedBufferCount() == bufferCount()) {
                    // all buffers are queued
					this->state = targetState ;
                }
            }
            break;

        case OMX_RETURNBUFFER_FSMSIG:
            if (this->isBufferSupplier) {
                // dequeued buffers on buffer supplier sharing ports
                // should never occur since the port reuses buffers from
                // its sharing port
                ARMNMF_DBC_ASSERT(!this->sharingPort);
                // buffer is queued since we are transitionning to idle
                queueBuffer((OMX_BUFFERHEADERTYPE *)evt->args.returnBuffer.buffer);
                if (queuedBufferCount() == bufferCount()) {
                    // all buffers are queued
					this->state = targetState ;
                }
            }
            else {
                returnBufferInternal(evt->args.returnBuffer.buffer);
                if (dequeuedBufferCount() == 0) {
                    // all buffers are either queued or held by
                    // tunneled port
                    flushBuffers();
                    this->state = targetState ;
                }
            }
            break;

        default:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_HOST: Port %d transientToIdleOrDisabledState assert !!", this->portIdx);
            ARMNMF_DBC_ASSERT(0);
            break;
    } /* switch(OmxEvent_FSMSIGnal(evt)) { */
} /* transientToIdleOrDisabledState */


void Port::transientToFlushState(const OmxEvent * evt) {
	switch(OmxEvent_FSMSIGnal(evt)) {
		case FSM_ENTRY_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d transientToFlushState FSM_ENTRY_SIG", this->portIdx);
			break;

		case FSM_EXIT_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d transientToFlushState FSM_EXIT_SIG", this->portIdx);
			break;

		case OMX_PROCESS_FSMSIG:
            if (isBufferSupplier && sharingPort) {

                transferQueuedBuffersToSharingPort();

                if (dequeuedBufferCount() == bufferCount()) {
                    // all buffers returned to sharing port so complete the command
                    state = translateState(flush_State);
                    flushComplete(true);
                }
            } 
            else if (isBufferSupplier) {
                if (queuedBufferCount() == bufferCount()) {
                    // all buffers are at port queue so complete the command
                    state = translateState(flush_State);
                    flushComplete(true);
                }
            } 
			break;

		case OMX_RETURNBUFFER_FSMSIG:
			returnBufferInternal(evt->args.returnBuffer.buffer) ;
            if (dequeuedBufferCount() == 0) {
                // all buffers are either queued or held by 
                // tunneled port so complete the command
                flushBuffers();
                state = translateState(flush_State);
                flushComplete(true);
            }
			break;

		default:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_HOST: Port %d transientToFlushState assert !!", this->portIdx);
			ARMNMF_DBC_ASSERT(0);
			break;
	}
} /* transientToFlushState */


void Port::disabledState(const OmxEvent * evt) {
    switch (OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d DisabledState FSM_ENTRY_SIG", this->portIdx);

            if(notifyStateTransition) {
                reset();
                bufferQueue.setEnabled(false) ;
                componentOwner->disablePortIndication(this->portIdx);
                if(this->eventAppliedOnEachPorts == false) {
                    componentOwner->getEventHandler()->eventHandler(
                            OMX_EventCmdComplete, OMX_CommandPortDisable, this->portIdx);
                } else if (componentOwner->allPortsDisabled()) {
                    componentOwner->getEventHandler()->eventHandler(
                            OMX_EventCmdComplete, OMX_CommandPortDisable, OMX_ALL);
                }
                this->eventAppliedOnEachPorts = false;
            } else {
                bufferQueue.setEnabled(false) ;
                this->notifyStateTransition = true;
            }
            break;

        case FSM_EXIT_FSMSIG:
            {
                FSM_State targetState = this->state;
                if (targetState == (FSM_State) &Port::executingState && this->isBufferSupplier && this->direction == InputPort) {
                    // buffer-supplier input ports must send all their
                    // buffers when transitionning from idle to executing
                    flushBuffers();
                }

			    componentOwner->enablePortIndication(this->portIdx);

                if(this->eventAppliedOnEachPorts == false) {
                    componentOwner->getEventHandler()->eventHandler(
                            OMX_EventCmdComplete, OMX_CommandPortEnable, this->portIdx);
                } else if (componentOwner->allPortsEnabled()) {
                    componentOwner->getEventHandler()->eventHandler(
                            OMX_EventCmdComplete, OMX_CommandPortEnable, OMX_ALL);
                }
                this->eventAppliedOnEachPorts = false;
				OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Port %d DisabledState FSM_EXIT_SIG", this->portIdx);
            }
            break;

        case OMX_SETSTATE_FSMSIG:
            {
				ARMNMF_DBC_PRECONDITION(evt->args.setState.state != this->disabled_State);
                this->disabled_State = evt->args.setState.state;
            }
            break;

        case OMX_ENABLE_PORT_FSMSIG:
            {
                FSM_State newState = translateState(this->disabled_State);
                this->disabled_State = OMX_StateInvalid;

                if(evt->args.portCmd.portIdx == OMX_ALL) {
                    this->eventAppliedOnEachPorts = true;
                }

                bufferQueue.setEnabled(true) ;
                if (newState == (FSM_State) &Port::pauseState && this->isBufferSupplier && this->direction == InputPort) {
                    this->state = (FSM_State) &Port::pauseFromIdleOrDisabledState ;
                } else {
                    if (newState == (FSM_State) &Port::executingState && this->isBufferSupplier && this->direction == InputPort) {
                        flushBuffers();
                    }
                    this->state = newState;
                }
            }
            break;

        default:
			OstTraceFiltInst1(TRACE_ERROR, "ENS_HOST: Port %d disabledState assert !!", this->portIdx);
        	ARMNMF_DBC_ASSERT(0);
            break;
    }
} /* disabledState */


void Port::invalidState(const OmxEvent *evt) {
	switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
        	break;

        case FSM_EXIT_FSMSIG:
            break;

        default:
        	break ;
    }
} /* invalidState */

ENS_API_EXPORT void Port::returnUnprocessedBuffer(OMX_BUFFERHEADERTYPE *buf)
{
    returnBufferInternalNmf(buf);
}


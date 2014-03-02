/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Component.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "Component.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ens_nmf_host_fsm_component_ComponentTraces.h"
#endif


ENS_API_EXPORT void Component::returnBufferAsync(OMX_U32 portIdx, OMX_BUFFERHEADERTYPE * buf) {
	// Asynchronous processing : Event must be dispatched on component state machine to update it.
	OmxEvent evt;
	evt.fsmEvent.signal = (FSM_FSMSIGnal)OMX_RETURNBUFFER_FSMSIG;
	evt.args.returnBuffer.buffer = buf;
	evt.args.returnBuffer.portIdx = portIdx;
			
	dispatch((FsmEvent*)&evt);
}

ENS_API_EXPORT void Component::scheduleProcessEvent() {
	int need_process = 0 ;

	NMF_mutex_lock(lock) ;
	if (!(this->pendingEvent)) {
		this->pendingEvent = true;
		need_process = 1 ;
	}
	NMF_mutex_unlock(lock) ;

	if (need_process) nmf_me->processEvent();
} /* scheduleProcessEvent */


ENS_API_EXPORT void Component::processEvent() {
	OmxEvent ev;
	
	NMF_mutex_lock(lock) ;
	this->pendingEvent = false;
	NMF_mutex_unlock(lock) ;
	
	ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_PROCESS_FSMSIG;

	dispatch((FsmEvent *)&ev);
} /* processEvent */


ENS_API_EXPORT void Component::sendCommand(OMX_COMMANDTYPE cmd, t_uword param) {
	OmxEvent ev;

    if ( (cmd == OMX_CommandStateSet) ||
            (cmd == OMX_CommandFlush) ||
            (cmd == OMX_CommandPortDisable) ||
            (cmd == OMX_CommandPortEnable)) {
		eventHandler->eventHandler(OMX_EventCmdReceived, cmd, param);
    }

	if (cmd == OMX_CommandStateSet) {
		ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_SETSTATE_FSMSIG;
		ev.args.setState.state = (OMX_STATETYPE) param;
	}
	else if (cmd == OMX_CommandFlush) {
		ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_FLUSH_FSMSIG;
        ARMNMF_DBC_ASSERT((param==OMX_ALL) || (param < (unsigned int)portCount));
		ev.args.portCmd.portIdx = param;
	}
    else if (cmd == OMX_CommandPortDisable) {
        ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_DISABLE_PORT_FSMSIG; 
        ARMNMF_DBC_ASSERT((param==OMX_ALL) || (param < (unsigned int)portCount));
        ev.args.portCmd.portIdx = param;
    }
    else if (cmd == OMX_CommandPortEnable) {
        ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_ENABLE_PORT_FSMSIG;
        ARMNMF_DBC_ASSERT((param==OMX_ALL) || (param < (unsigned int)portCount));
        ev.args.portCmd.portIdx = param;
    }
	else { ARMNMF_DBC_ASSERT(0); }

	dispatch((FsmEvent *)&ev);
} /* sendCommand */


OMX_STATETYPE Component::getState(void) {
    FSM_State state = FSM::getState() ;

	if      (state == (FSM_State)&Component::idleState)		 return OMX_StateIdle;
    else if (state == (FSM_State)&Component::executingState) return OMX_StateExecuting;
    else if (state == (FSM_State)&Component::pauseState)     return OMX_StatePause;
    else if (state == (FSM_State)&Component::transientToIdleState)  return OMX_StateTransientToIdle;
    else if (state == (FSM_State)&Component::waitForPortFlushedOrDisabled) return this->transientState ;
    else {
		OstTraceFiltInst0(TRACE_ERROR, "ENS_HOST: Component::getState assert !!");
        ARMNMF_DBC_ASSERT(0);
        return OMX_StateInvalid;
    }
} /* getState */

bool Component::allPortsDisabled(void) {
    int i; 
    for(i = 0; i < this->portCount; i++) {
        if (ports[i].isEnabled() == true) {
            return false;
        }
    }
    return true;
}

bool Component::allPortsEnabled(void) {
    int i; 
    for(i = 0; i < this->portCount; i++) {
        if (ports[i].isEnabled() == false) {
            return false;
        }
    }
    return true;
}

bool Component::isOnePortFlushing(void) {
    int i; 
    for(i = 0; i < this->portCount; i++) {
        if (ports[i].isFlushing() == true) {
            return true;
        }
    }
    return false;
}

ENS_API_EXPORT void Component::init(int portCount, Port *ports, Ieventhandler *nmfitf, Ipostevent *nmf_me, bool invalid) {
	this->portCount     = portCount; /* should definitely be unsigned */
	this->ports         = ports;
	this->pendingEvent  = false;
	this->eventHandler  = nmfitf;
	this->nmf_me		= nmf_me ;	
	this->transientState = OMX_StateLoaded;
	this->lock = NMF_mutex_create() ;
	
	OstTraceFiltInst0(TRACE_API, "ENS_HOST: Enter Component::init function");
	
	if ((this->lock == 0) || (invalid)) {
		FSM::init((FSM_State)&Component::invalidState);
		return ;
	} /* if ((this->lock == 0) || (invalid)) */
		
	/* Look for an invalid port */
	int i=0 ;
	while ((i<portCount) && (!ports[i].isInvalid())) i++ ;
	
	if (i<portCount) FSM::init((FSM_State)&Component::invalidState) ;
	else FSM::init((FSM_State)&Component::idleState) ;
} /* init */

ENS_API_EXPORT Component::Component()
    : portCount(0),
      ports(0),
      pendingEvent(false),
      eventHandler(0),
      nmf_me(0),
      transientState(OMX_StateLoaded),
      lock(0)
{} /* Component */

ENS_API_EXPORT Component::~Component() {
    if (this->lock != 0) {
        if (!NMF_mutex_destroy(lock)) {
            lock = 0;
        }
    }
} /* ~Component */

/* ---------------------- */
bool Component::postProcessCheck() {
    int i; 
    for(i = 0; i < this->portCount; i++) {
        if (!ports[i].postProcessCheck()) {
            return false;
        }
    }
    return true;
} /* postProcessCheck */


FSM_State Component::getStateFromPorts() {

	FSM_State   newState = 0;
	int         i;
	int         executingPorts = 0, 
				idlePorts = 0, 
				pausedPorts = 0,
				transientToIdlePorts = 0,
				transientToDisablePorts = 0,
				flushingPorts = 0;
	
	int         portCount = this->portCount;   
	
	for (i = 0; i < portCount; i++) {
		switch (ports[i].getState()) {
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
				OstTraceFiltInst0(TRACE_ERROR, "ENS_HOST: Component::getStateFromPorts assert !!");
				ARMNMF_DBC_ASSERT(0);
				break;
		}
	} /* for (i = 0; i < portCount; i++) */
	
	if (executingPorts == portCount) {
		newState = (FSM_State) &Component::executingState;
	}
	else if (pausedPorts == portCount) {
		newState = (FSM_State) &Component::pauseState;
	}
	else if (idlePorts == portCount) {
		newState = (FSM_State) &Component::idleState;
	}
	else if (idlePorts + transientToIdlePorts == portCount) {
		newState = (FSM_State) &Component::transientToIdleState;
	}
    else if ((executingPorts + transientToDisablePorts == portCount) ||
             (pausedPorts + transientToDisablePorts == portCount) ||
             (executingPorts + flushingPorts == portCount) ||
             (pausedPorts + flushingPorts == portCount)) {
             
             newState = (FSM_State) &Component::waitForPortFlushedOrDisabled;
    }
	else {
		OstTraceFiltInst0(TRACE_ERROR, "ENS_HOST: Component::getStateFromPorts portCount assert !!");
		ARMNMF_DBC_ASSERT(0);
	}
	
	return newState ;

} /* getStateFromPorts */


void Component::updateState(FSM_State currentState) {
	FSM_State   newState = getStateFromPorts() ;

    if (newState != currentState) {
		this->state = (FSM_State)newState ;
    }
} /* updateState */


void Component::setState(const OmxEvent *evt, FSM_State currentState) {
    int i;
    
    for (i = 0; i < this->portCount; i++) {
        ports[i].dispatch((FsmEvent *)evt);
    }

    updateState(currentState);
} /* setState */




// component in idle state 
// => all ports are in idle state
void Component::idleState(const OmxEvent *evt) {
    switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG: 
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Component IdleState FSM_ENTRY_SIG");
            stateChangeIndication(this->transientState, OMX_StateIdle);
            this->transientState = OMX_StateIdle;
            reset();
			eventHandler->eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle);
            break;

        case FSM_EXIT_FSMSIG:
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Component IdleState FSM_EXIT_SIG");
            break;

        case OMX_SETSTATE_FSMSIG: 
            setState(evt, (FSM_State)&Component::idleState);
            break;

        case OMX_PROCESS_FSMSIG:
            break;

        case OMX_FLUSH_FSMSIG:
        case OMX_DISABLE_PORT_FSMSIG:
        case OMX_ENABLE_PORT_FSMSIG:
            {
                int i;
                if(evt->args.portCmd.portIdx == OMX_ALL) {
                    for (i = 0; i < this->portCount; i++) {
                        ports[i].dispatch((FsmEvent *)evt);
                    }
                } else {
                    ports[evt->args.portCmd.portIdx].dispatch((FsmEvent *)evt) ;
                }
            }
            break;
            
        default:
			OstTraceFiltInst0(TRACE_ERROR, "ENS_HOST: Component::idleState assert !!");
            ARMNMF_DBC_ASSERT(0);
            break;
    }
} /* idleState */


// component in executing state 
// => all ports are in executing state
void Component::executingState(const OmxEvent *evt) {
    switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG: 
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Component ExecutingState FSM_ENTRY_SIG");
            if(this->transientState  != OMX_StateExecuting) {
                stateChangeIndication(this->transientState, OMX_StateExecuting);
                this->transientState = OMX_StateExecuting;
				eventHandler->eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateExecuting);
				ARMNMF_DBC_POSTCONDITION(postProcessCheck());
        	}            
			//
			// we may already have some buffers to process for e.g
			// when transitionning from pause state
			process();
            break;

        case FSM_EXIT_FSMSIG:
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Component ExecutingState FSM_EXIT_SIG");
            break;

        case OMX_PROCESS_FSMSIG:
            process();
            ARMNMF_DBC_POSTCONDITION(postProcessCheck());
            break;

        case OMX_RETURNBUFFER_FSMSIG:
            ports[evt->args.returnBuffer.portIdx].dispatch((FsmEvent *)evt);
            break;
            
        case OMX_SETSTATE_FSMSIG: 
            setState(evt, (FSM_State) &Component::executingState);
            break;

        case OMX_FLUSH_FSMSIG:
        case OMX_DISABLE_PORT_FSMSIG:
            {
                int i;
                if(evt->args.portCmd.portIdx == OMX_ALL) {
                    for (i = 0; i < this->portCount; i++) {
                        Port *port = &this->ports[i];
                        port->dispatch((FsmEvent *)evt);
                    }
                } else {
                    Port *port = &this->ports[evt->args.portCmd.portIdx];
                    port->dispatch((FsmEvent *)evt);
                }
                updateState((FSM_State) &Component::executingState);
            }
            break;
            
        case OMX_ENABLE_PORT_FSMSIG:
            {
                int i;
                if(evt->args.portCmd.portIdx == OMX_ALL) {
                    for (i = 0; i < this->portCount; i++) {
                        ports[i].dispatch((FsmEvent *)evt);
                    }
                } else {
                    ports[evt->args.portCmd.portIdx].dispatch((FsmEvent *)evt);
                }
                scheduleProcessEvent(); //Avoid race condition when enabling ports in executing state!!                    
            }
            break;
        
        default:
			OstTraceFiltInst0(TRACE_ERROR, "ENS_HOST: Component::executingState assert !!");
            ARMNMF_DBC_ASSERT(0);
			break ;
    }
} /* executingState */


// component in pause state 
// => all ports are in pause state
void Component::pauseState(const OmxEvent *evt) {
    switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Component PauseState FSM_ENTRY_SIG");
            if(this->transientState  != OMX_StatePause) {
                stateChangeIndication(this->transientState, OMX_StatePause);
                this->transientState  = OMX_StatePause;
				eventHandler->eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StatePause);
        	}
            break;

        case FSM_EXIT_FSMSIG:
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Component PauseState FSM_EXIT_SIG");
            break;

        case OMX_PROCESS_FSMSIG:
            break;

        case OMX_RETURNBUFFER_FSMSIG:
            {
                // no FIFO on return buffer path
                // so we must send them even in Pause state
                Port *port = &ports[evt->args.returnBuffer.portIdx];
                port->dispatch((FsmEvent *)evt);
            }
            break;

        case OMX_SETSTATE_FSMSIG: 
            setState(evt, (FSM_State) &Component::pauseState);
            break;

        case OMX_FLUSH_FSMSIG:
        case OMX_DISABLE_PORT_FSMSIG:
            {
                int i;
                if(evt->args.portCmd.portIdx == OMX_ALL) {
                    for (i = 0; i < this->portCount; i++) {
                        Port *port = &this->ports[i];
                        port->dispatch((FsmEvent *)evt);
                    }
                } else {
                    Port *port = &this->ports[evt->args.portCmd.portIdx];
                    port->dispatch((FsmEvent *)evt);
                }
                updateState((FSM_State) &Component::pauseState);
            }
            break;
            
        case OMX_ENABLE_PORT_FSMSIG:
            {
                int i;
                if(evt->args.portCmd.portIdx == OMX_ALL) {
                    for (i = 0; i < this->portCount; i++) {
                        ports[i].dispatch((FsmEvent *)evt);
                    }
                } else {
                    ports[evt->args.portCmd.portIdx].dispatch((FsmEvent *)evt);
                }
            }
            break;
			            
        default:
			OstTraceFiltInst0(TRACE_ERROR, "ENS_HOST: Component::pauseState assert !!");
            ARMNMF_DBC_ASSERT(0);
			break ;
    }
} /* pauseState */


// component in transientToIdle state 
// => all ports are either in transientToIdle or idle state
void Component::transientToIdleState(const OmxEvent *evt) {
    switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Component TransientToIdleState FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_FSMSIG:
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Component TransientToIdleState FSM_EXIT_SIG");
            break;

        case OMX_RETURNBUFFER_FSMSIG:
            {
                Port *port = &ports[evt->args.returnBuffer.portIdx];
                OMX_STATETYPE currState = port->getState();

                port->dispatch((FsmEvent *)evt);

                if (port->getState() != currState) {
                    updateState((FSM_State) &Component::transientToIdleState);
                }
            }
            break;

        case OMX_PROCESS_FSMSIG:
            {
                int i;
                bool needStateUpdate = false;

                for (i = 0; i < this->portCount; i++) {
                    Port *port = &ports[i];
                    OMX_STATETYPE currState = port->getState();

                    if (port->isEnabled()) {
						port->dispatch((FsmEvent *)evt);
						if (port->getState() != currState) {
							needStateUpdate = true;
						}
					}
                }

                if (needStateUpdate) {
                    updateState((FSM_State) &Component::transientToIdleState);
                }
            }
            break;

        case OMX_FLUSH_FSMSIG:
			OstTraceFiltInst0(TRACE_ERROR, "ENS_HOST: Component::transientToIdleState FLUSH_FSMSIG assert !!");
            ARMNMF_DBC_ASSERT(0);
            break;

        default:
			OstTraceFiltInst0(TRACE_ERROR, "ENS_HOST: Component::transientToIdleState assert !!");
            ARMNMF_DBC_ASSERT(0);
			break ;
    } /* switch(OmxEvent_FSMSIGnal(evt)) */
} /* transientToIdleState */



// component in waitForPortFlushedOrDisabled state 
// => one port at least is in transientToDisabledState or in transientToFlushState
void Component::waitForPortFlushedOrDisabled(const OmxEvent *evt) {
    switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG: 
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Component waitForPortFlushedOrDisabled FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_FSMSIG:
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Component waitForPortFlushedOrDisabled FSM_EXIT_SIG");
            break;

        case OMX_PROCESS_FSMSIG:
            {
                int i;
                bool portStateChanged = false;

                for (i = 0; i < this->portCount; i++) {
                    Port *port = &this->ports[i];
                    OMX_STATETYPE currState = port->getState();
                    if ((currState == OMX_StateTransientToDisable) || (currState == OMX_StateFlushing)){
						port->dispatch((FsmEvent *)evt);
                        if ((portStateChanged == false) && (port->getState() != currState)) {
                            portStateChanged = true;
                        }
                    } /* if ((currState == OMX_StateTransientToDisable) ... */
                } /* for (i = 0; i < this->portCount; i++) */
				
                if(portStateChanged == true) {
                    updateState((FSM_State) &Component::waitForPortFlushedOrDisabled);
                }
                
				if(this->transientState == OMX_StateExecuting) {
                    process();
					updateState((FSM_State) &Component::waitForPortFlushedOrDisabled);
                }
            }
            break;

        case OMX_RETURNBUFFER_FSMSIG:
            {
                Port *port = &this->ports[evt->args.returnBuffer.portIdx];
                // OMX_STATETYPE currState = port->getState(); // Why was this retrieved ?
                
                port->dispatch((FsmEvent *)evt);
                updateState((FSM_State) &Component::waitForPortFlushedOrDisabled);
            }
            break;
            
        default:
			OstTraceFiltInst0(TRACE_ERROR, "ENS_HOST: Component::waitForPortFlushedOrDisabled assert !!");
        	ARMNMF_DBC_ASSERT(0);
    }
} /* waitForPortFlushedOrDisabled */

// component in invalidState state 
// => Nothing else to do, something goes wrong... 
void Component::invalidState(const OmxEvent *evt) {
	switch(OmxEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG: 
        	eventHandler->eventHandler(OMX_EventError, (t_uint32)OMX_ErrorInvalidState, 0);
        	break;

        case FSM_EXIT_FSMSIG:            
            break;
            
        default:
        	break ;
    }
} /* invalidState */




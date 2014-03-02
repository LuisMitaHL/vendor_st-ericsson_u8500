/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Controller.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "Controller.h"
#include "OMX_Core.h"

/* Temporary fix to activate/desactivate trace */
#define DEBUGLOG(X)
//#define DEBUGLOG(X) NMF_LOG X 

#define OSTTRACE 

#ifdef OSTTRACE
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ens_nmf_host_fsm_controller_ControllerTraces.h"
#endif
#endif

ENS_API_EXPORT void Controller::init(t_uint8 slaveCount, Ieventhandler *nmfitf, Ieventhandler *previousController, Ieventhandler  *nextController, bool bufferUser) {
	DEBUGLOG(("(Ctrl %x) init\n", this)) ;
#ifdef OSTTRACE
	OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::init");
#endif
	ARMNMF_DBC_PRECONDITION(nmfitf && previousController && nextController) ; // Not null
	
	this->slaveCount    = slaveCount;
	this->eventHandler  = nmfitf;
	this->standbyInfo.state  = OMX_StateInvalid ;
	this->isBufferUser = bufferUser ;
    this->localState = OMX_StateTransient ;
	
	controllers[PREVIOUS_CONTROLLER_ID].itf = previousController ;
	controllers[PREVIOUS_CONTROLLER_ID].state = OMX_StateIdle ; //Idle because we may reach "pause" before the other component reach "init" state.
	controllers[NEXT_CONTROLLER_ID].itf = nextController ;
	controllers[NEXT_CONTROLLER_ID].state = OMX_StateIdle ;
	
	slavesArray = new slaveInfo_t[slaveCount] ;
		
	if (slavesArray == 0) {
		FSM::init((FSM_State) &Controller::invalidState);
		return ;
	} /* if (slavesArray == 0) */
	
	for (int i=0; i<slaveCount; i++) {
		slavesArray[i].itf = 0 ;
		slavesArray[i].state = OMX_StateInvalid ;
		slavesArray[i].commandReceived = OMX_FALSE ;
		slavesArray[i].remainingCommands = 0 ;
	} /* for (int i=0; i<slaveCount; i++) */
	
    FSM::init((FSM_State) &Controller::transientToIdleState);
} /* init */


ENS_API_EXPORT void Controller::setSlave(t_uint8 slaveIndex, Isendcommand * slave) {
	ARMNMF_DBC_PRECONDITION(slaveIndex < slaveCount) ;
	slavesArray[slaveIndex].itf = slave ;
	
	DEBUGLOG(("(Ctrl %x) setSlave %d \n", this, slaveIndex)) ;
#ifdef OSTTRACE
	OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::setSlave %d", slaveIndex);
#endif

} /* setSlave */

ENS_API_EXPORT void Controller::sendCommand(OMX_COMMANDTYPE cmd, t_uword param) {
	CtrlEvent ev;
	DEBUGLOG(("(Ctrl %x) >> sendCommand %d %d\n",this, cmd, param)) ;
#ifdef OSTTRACE
	OstTraceFiltInst2(TRACE_FLOW, "ENS_HOST: Controller::sendCommand %d %d", cmd, param);
#endif
	
	ev.args.cmd.cmd = cmd ;
	ev.args.cmd.param = param ;
	
	if (cmd == OMX_CommandStateSet) {
		ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_SETSTATE_FSMSIG;
	}
	else if (cmd == OMX_CommandFlush) {
		ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_FLUSH_FSMSIG;
	}
    else if (cmd == OMX_CommandPortDisable) {
        ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_DISABLE_PORT_FSMSIG; 
    }
    else if (cmd == OMX_CommandPortEnable) {
        ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_ENABLE_PORT_FSMSIG;
    }
	else { ARMNMF_DBC_ASSERT(0); }

	dispatch((FsmEvent *) &ev);
} /* sendCommand */


ENS_API_EXPORT void Controller::slavesEventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 id) {
	CtrlEvent ctrlEv ;
	DEBUGLOG(("(Ctrl %x) >> slavesEventHandler %d %d %d - %d\n",this, ev, data1, data2, id)) ;
#ifdef OSTTRACE
	OstTraceFiltInst2(TRACE_FLOW, "ENS_HOST: Controller::slavesEventHandler ev = %d, data1 = %d", ev, data1);
	OstTraceFiltInst2(TRACE_FLOW, "ENS_HOST: Controller::slavesEventHandler data2 = %d, id = %d", data2, id);
#endif
	ctrlEv.fsmEvent.signal = (FSM_FSMSIGnal) CTRL_EVENT_FSMSIG;
		
	ctrlEv.args.ev.event = ev ;
	ctrlEv.args.ev.data1 = data1 ;
	ctrlEv.args.ev.data2 = data2 ;
	ctrlEv.args.ev.id    = id ;

	dispatch((FsmEvent *) &ctrlEv) ;
} /* slavesEventHandler */

ENS_API_EXPORT void Controller::controllersEventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 id) {
	CtrlEvent ctrlEv ;
	DEBUGLOG(("(Ctrl %x) >> controllersEventHandler %d %d %d - %d\n", this, ev, data1, data2, id)) ;
#ifdef OSTTRACE
	OstTraceFiltInst2(TRACE_FLOW, "ENS_HOST: Controller::controllersEventHandler ev = %d, data1 = %d", ev, data1);
	OstTraceFiltInst2(TRACE_FLOW, "ENS_HOST: Controller::controllersEventHandler data2 = %d, id = %d", data2, id);
#endif
	
	/* Update state here because we always want to update state */
	controllers[id].state = (OMX_STATETYPE)data2 ;
	
	/* Then dispatch a event to say that something change */
	ctrlEv.fsmEvent.signal = (FSM_FSMSIGnal) CTRL_NEIGHBOUR_EVENT_FSMSIG;
	
	ctrlEv.args.ev.event = ev ;
	ctrlEv.args.ev.data1 = data1 ;
	ctrlEv.args.ev.data2 = data2 ;
	ctrlEv.args.ev.id    = id ;

	dispatch((FsmEvent *) &ctrlEv) ;
} /* controllersEventHandler */


bool Controller::controllersNotInState(OMX_STATETYPE state) {
  return (((controllers[PREVIOUS_CONTROLLER_ID].itf->IsNullInterface()) || (controllers[PREVIOUS_CONTROLLER_ID].state != state)) &&
	  ((controllers[NEXT_CONTROLLER_ID].itf->IsNullInterface()) || (controllers[NEXT_CONTROLLER_ID].state != state))) ;
} /* controllersNotInState */

bool Controller::controllersInState(OMX_STATETYPE state) {
  return (((controllers[PREVIOUS_CONTROLLER_ID].itf->IsNullInterface()) || (controllers[PREVIOUS_CONTROLLER_ID].state == state)) &&
	  ((controllers[NEXT_CONTROLLER_ID].itf->IsNullInterface()) || (controllers[NEXT_CONTROLLER_ID].state == state))) ;
} /* controllersInState */



FSM_State Controller::translateState(OMX_STATETYPE state) {
    switch (state) {
        case OMX_StateIdle:         return (FSM_State)&Controller::idleState;
        case OMX_StateExecuting:    return (FSM_State)&Controller::executingState;
        case OMX_StatePause:        return (FSM_State)&Controller::pauseState;
        
        default:
            ARMNMF_DBC_ASSERT(0);
            return 0;
    } /* switch (state) */
} /* translateState */


ENS_API_EXPORT Controller::~Controller() {
	if (slavesArray != 0) delete slavesArray ;
} 

ENS_API_EXPORT void Controller::forwardStateChange(const OMX_STATETYPE targetState) {
	/* Dispatch state Change */
	for (int i=0; i < slaveCount; i++) {
	  if (!slavesArray[i].itf->IsNullInterface()) slavesArray[i].itf->sendCommand(OMX_CommandStateSet, targetState) ;
	} /* for (int i=0; i < slaveCount; i++) { */
} /* forwardStateChange */


void Controller::forwardStateToControllers(const OMX_STATETYPE targetState) {
  /* Dispatch to neighbours */
  if (!controllers[PREVIOUS_CONTROLLER_ID].itf->IsNullInterface())
    controllers[PREVIOUS_CONTROLLER_ID].itf->eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, targetState) ;
  if (!controllers[NEXT_CONTROLLER_ID].itf->IsNullInterface())
    controllers[NEXT_CONTROLLER_ID].itf->eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, targetState) ;
}/* forwardStateToControllers */

ENS_API_EXPORT void Controller::waitCommandFromSlave(t_uint8 slaveIndex, OMX_COMMANDTYPE command, t_uint8 count) {
	ARMNMF_DBC_PRECONDITION(slaveIndex < slaveCount) ;
	
	slavesArray[slaveIndex].remainingCommands = count ;
	slavesArray[slaveIndex].currentCommand = command ;

	DEBUGLOG(("(Ctrl %x) Waiting %d commands[%d] from slave %d\n", this, count, command, slaveIndex)) ;
#ifdef OSTTRACE
	OstTraceFiltInst2(TRACE_FLOW, "ENS_HOST: Controller::waitCommandFromSlave slaveIndex = %d, command = %d", slaveIndex, command);
	OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::waitCommandFromSlave count = %d", count);
#endif

} /* waitCommandFromSlave */


/* ---------------------- */
void Controller::defaultState(const CtrlEvent *evt, OMX_STATETYPE currentState) {
	switch(CtrlEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:                    	
        	if (standbyInfo.state != OMX_StateInvalid) {
				DEBUGLOG(("(Ctrl %x) Return to state %d\n", this, currentState)) ;
#ifdef OSTTRACE
				OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::defaultState FSM_ENTRY_FSMSIG Return to state %d", currentState);
#endif
				standbyInfo.state = OMX_StateInvalid ;				
				forwardPortEvent(standbyInfo.command, standbyInfo.id) ;
        	}
        	else {
				DEBUGLOG(("(Ctrl %x) Enter state %d\n", this, currentState)) ;
#ifdef OSTTRACE
				OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::defaultState FSM_ENTRY_FSMSIG Enter state %d", currentState);
#endif
                this->localState = currentState ;
				eventHandler->eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, currentState);
				forwardStateToControllers(currentState) ;
        	}
            break;
            
		case FSM_EXIT_FSMSIG:
            if (standbyInfo.state == OMX_StateInvalid) {
                this->localState = OMX_StateTransient ;
            }
			break;
			
		case CTRL_NEIGHBOUR_EVENT_FSMSIG:
			break;
			
		case CTRL_EVENT_FSMSIG:
			DEBUGLOG(("(Ctrl %x) Unexpected event in state %d\n", this, currentState)) ;
#ifdef OSTTRACE
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::defaultState CTRL_EVENT_FSMSIG Unexpected event in state %d", currentState);
#endif
			this->state = (FSM_State) &Controller::invalidState ;
			break;
		   
		default:
			forwardPortCommand(evt->args.cmd.cmd, evt->args.cmd.param) ;
			{
				/* check if slave are in waitForCommand state */
				int i = 0 ;
				while ((i<slaveCount) && (slavesArray[i].remainingCommands == 0)) i++;
				if (i < slaveCount) {
					//Save context
					standbyInfo.state = currentState ;
					standbyInfo.command = evt->args.cmd.cmd ;
					standbyInfo.id = evt->args.cmd.param ;

					this->state = (FSM_State)&Controller::waitForCommandState ;
				} /* if (i!=slaveCount) */
			}
			break;
	} /* switch(CtrlEvent_FSMSIGnal(evt)) */
} /* defaultState */



void Controller::idleState(const CtrlEvent *evt) {
    switch(CtrlEvent_FSMSIGnal(evt)) {
    
        case OMX_SETSTATE_FSMSIG: 
        	DEBUGLOG(("(Ctrl %x) Idle - SetState %d\n", this,evt->args.cmd.param)) ;
#ifdef OSTTRACE
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::idleState OMX_SETSTATE_FSMSIG %d", evt->args.cmd.param);
#endif
        	{
				OMX_STATETYPE newOMXState = (OMX_STATETYPE)evt->args.cmd.param ;				
	
				if (newOMXState == OMX_StateExecuting) {
					this->state = (FSM_State)&Controller::idleToExecutingState ;					
				} else if (newOMXState == OMX_StatePause) {
					this->state = (FSM_State)&Controller::idleToPauseState ;
				} else { ARMNMF_DBC_ASSERT(0); }
				
				forwardStateChange(OMX_StatePause) ;
        	}
            break;
           
        default:
        	defaultState(evt, OMX_StateIdle) ;
            break;
    }
} /* idleState */


void Controller::executingState(const CtrlEvent *evt) {
	switch(CtrlEvent_FSMSIGnal(evt)) {
	
		case OMX_SETSTATE_FSMSIG: 
			DEBUGLOG(("(Ctrl %x) Executing - setState %d\n", this, evt->args.cmd.param)) ;
#ifdef OSTTRACE
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::executingState OMX_SETSTATE_FSMSIG %d", evt->args.cmd.param);
#endif
			{
				OMX_STATETYPE newOMXState = (OMX_STATETYPE)evt->args.cmd.param ;				
	
				if (newOMXState == OMX_StateIdle) {
					this->state = (FSM_State)&Controller::executingToIdleState ;					
				} else if (newOMXState == OMX_StatePause) {
					this->state = (FSM_State)&Controller::executingToPauseState ;
				} else { ARMNMF_DBC_ASSERT(0); }
				
				forwardStateChange(OMX_StatePause) ;
			}
			break;
		   
		default:
			defaultState(evt, OMX_StateExecuting) ;
			break;
	}
} /* executingState */


void Controller::pauseState(const CtrlEvent *evt) {
    switch(CtrlEvent_FSMSIGnal(evt)) {

		case OMX_SETSTATE_FSMSIG: 
			DEBUGLOG(("(Ctrl %x) Pause - setState %d\n", this, evt->args.cmd.param)) ;
#ifdef OSTTRACE
			OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::pauseState OMX_SETSTATE_FSMSIG %d", evt->args.cmd.param);
#endif
			{
				OMX_STATETYPE newOMXState = (OMX_STATETYPE)evt->args.cmd.param ;			
	
				if (newOMXState == OMX_StateExecuting) {
					this->state = (FSM_State) &Controller::pauseToExecutingState ;					
				} else if (newOMXState == OMX_StateIdle) {
					this->state = (FSM_State) &Controller::pauseToIdleState ;
				} else { ARMNMF_DBC_ASSERT(0); }
				
				forwardStateChange(newOMXState) ;
			}
			break;
		   
		default:
			defaultState(evt, OMX_StatePause) ;
			break;
    }
} /* pauseState */


bool Controller::waitForNextState(const CtrlEvent *evt, OMX_STATETYPE nextState) {
	switch(CtrlEvent_FSMSIGnal(evt)) {
		case FSM_ENTRY_FSMSIG:		
			DEBUGLOG(("(Ctrl %x)  -> enter\n", this)) ;
#ifdef OSTTRACE
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::waitForNextState FSM_ENTRY_FSMSIG");
#endif
			break;
		case FSM_EXIT_FSMSIG:
			break;
			
		case CTRL_NEIGHBOUR_EVENT_FSMSIG:
			break;
			
		case CTRL_EVENT_FSMSIG:			
			if (
					(evt->args.ev.event == OMX_EventCmdComplete) && 
					(evt->args.ev.data1 == OMX_CommandStateSet) &&
					(evt->args.ev.data2 == (unsigned int)nextState)
				) {				
				slavesArray[evt->args.ev.id].state = nextState ;				
				DEBUGLOG(("(Ctrl %x)  -> slave %d in nextState\n", this, evt->args.ev.id)) ;
#ifdef OSTTRACE
				OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::waitForNextState CTRL_EVENT_FSMSIG slave %d in nextState", evt->args.ev.id);
#endif
				
				/* check if all slave are in nextState */
				int i = 0 ;
				while 
				  (					
				   (i<slaveCount) && 
				    (
				      (slavesArray[i].itf->IsNullInterface()) || 
				      (slavesArray[i].state == nextState)
				    )
				  ) i++ ;
				if (i == slaveCount) {
					return true ;
				} /* if (i==slaveCount) */
	
			} else if (evt->args.ev.event == OMX_EventCmdReceived) {
				slavesArray[evt->args.ev.id].commandReceived = OMX_TRUE;

				DEBUGLOG(("(Ctrl %x) waitForNextState -> node %d acknowledged a command\n", this, evt->args.ev.id)) ;
#ifdef OSTTRACE
				OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::waitForNextState CTRL_EVENT_FSMSIG node %d acknowledged a command", evt->args.ev.id);
#endif
				int i=0 ;
				while ((i<slaveCount) && (OMX_TRUE == slavesArray[i].commandReceived)) i++;
				if (i == slaveCount) {
					eventHandler->eventHandler(OMX_EventCmdReceived, evt->args.ev.data1, evt->args.ev.data2);
					for (i=0; i<slaveCount; i++) slavesArray[i].commandReceived = OMX_FALSE;
				}  
			} else {
				DEBUGLOG(("(Ctrl %x)  -> unexpected event\n", this)) ;
#ifdef OSTTRACE
				OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::waitForNextState CTRL_EVENT_FSMSIG unexpected event");
#endif
				this->state = (FSM_State) &Controller::invalidState ;
			}     	
			break;
		   
		default:
			ARMNMF_DBC_ASSERT(0);
			break;
	}
	
	return false ;
} /* toIdleState */


void Controller::pauseToExecutingState(const CtrlEvent *evt) {
	DEBUGLOG(("(Ctrl %x) > Pause to exec\n", this)) ;
#ifdef OSTTRACE
	OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::pauseToExecutingState");
#endif
	if (waitForNextState(evt,OMX_StateExecuting)) {
		this->state = (FSM_State)&Controller::executingState ;
		
	}
} /* pauseToExecutingState */

void Controller::executingToPauseState(const CtrlEvent *evt) {
	DEBUGLOG(("(Ctrl %x) > Exec to pause\n", this)) ;
#ifdef OSTTRACE
	OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::executingToPauseState");
#endif
	if (waitForNextState(evt, OMX_StatePause)) {
		this->state = (FSM_State)&Controller::pauseState ;
	}
} /* executingToPauseState */

void Controller::idleToPauseState(const CtrlEvent *evt) {
	DEBUGLOG(("(Ctrl %x) > Idle to pause\n", this)) ;
#ifdef OSTTRACE
	OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::idleToPauseState");
#endif
	if (waitForNextState(evt, OMX_StatePause)) {
		this->state = (FSM_State)&Controller::pauseState ;
	}
} /* pauseToIdleState */

void Controller::pauseToIdleState(const CtrlEvent *evt) {
	DEBUGLOG(("(Ctrl %x) > Pause to idle\n", this)) ;
#ifdef OSTTRACE
	OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::pauseToIdleState");
#endif
	if (waitForNextState(evt, OMX_StateIdle)) {
		this->state = (FSM_State)&Controller::idleState ;
	}
} /* pauseToIdleState */

void Controller::transientToIdleState(const CtrlEvent *evt) {
	DEBUGLOG(("(Ctrl %x) > Transient to idle\n", this)) ;
#ifdef OSTTRACE
	OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::transientToIdleState");
#endif
	if (waitForNextState(evt,OMX_StateIdle)) {
		this->state = (FSM_State)&Controller::idleState ;
	}
} /* transientToIdleState */ 

void Controller::transientToExecutingState(const CtrlEvent *evt) {
	DEBUGLOG(("(Ctrl %x) > Transient to executing\n", this)) ;
#ifdef OSTTRACE
	OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::transientToExecutingState");
#endif
	if (waitForNextState(evt,OMX_StateExecuting)) {
		this->state = (FSM_State)&Controller::executingState ;
	}
} /* transientToExecutingState */

void Controller::idleToExecutingState(const CtrlEvent *evt) {
	DEBUGLOG(("(Ctrl %x) > Idle to executing\n", this)) ;
#ifdef OSTTRACE
	OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::idleToExecutingState");
#endif
	if (waitForNextState(evt, OMX_StatePause)) {		
		forwardStateToControllers(OMX_StatePause) ;
		
		if ((isBufferUser) || (controllersNotInState(OMX_StateIdle))) { 
			forwardStateChange(OMX_StateExecuting) ;
			this->state = (FSM_State) &Controller::transientToExecutingState ;
		} else {
			//Save context
			standbyInfo.state = OMX_StateIdle ;
			standbyInfo.command = OMX_CommandStateSet ;
			standbyInfo.id = 0 ;

			this->state = (FSM_State)&Controller::waitForControllersPause ;
		}
	}
} /* idleToExecutingState */

void Controller::executingToIdleState(const CtrlEvent *evt) {
	DEBUGLOG(("(Ctrl %x) > Executing to idle\n", this)) ;
#ifdef OSTTRACE
	OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::executingToIdleState");
#endif
	if (waitForNextState(evt, OMX_StatePause)) {	
		forwardStateToControllers(OMX_StatePause) ;
	
		if ((isBufferUser) || (controllersNotInState(OMX_StateExecuting))) { 
			forwardStateChange(OMX_StateIdle) ;
			this->state = (FSM_State) &Controller::transientToIdleState ;
		} else {
			//Save context
			standbyInfo.state = OMX_StateExecuting ;
			standbyInfo.command = OMX_CommandStateSet ;
			standbyInfo.id = 0 ;
	
			this->state = (FSM_State)&Controller::waitForControllersPause ;
		}
	}
} /* executingToIdleState */


void Controller::waitForCommandState(const CtrlEvent *evt) {
	switch(CtrlEvent_FSMSIGnal(evt)) {
		case FSM_ENTRY_FSMSIG:		
			DEBUGLOG(("(Ctrl %x) waitForCommandState -> enter\n", this)) ;
#ifdef OSTTRACE
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::waitForCommandState FSM_ENTRY_FSMSIG");
#endif
			break;
		case FSM_EXIT_FSMSIG:
			break;
			
		case CTRL_NEIGHBOUR_EVENT_FSMSIG:
			break;
			
		case CTRL_EVENT_FSMSIG:			
			if (evt->args.ev.data1 == (unsigned int)slavesArray[evt->args.ev.id].currentCommand) {
				if (evt->args.ev.event == OMX_EventCmdComplete) {

					slavesArray[evt->args.ev.id].remainingCommands-- ;
					DEBUGLOG(("(Ctrl %x) waitForCommandState -> node %d complete a command\n", this, evt->args.ev.id)) ;
#ifdef OSTTRACE
					OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::waitForCommandState CTRL_EVENT_FSMSIG node %d complete a command", evt->args.ev.id);
#endif

					/* check if all command are executed */
					int i = 0 ;
					while ((i<slaveCount) && (slavesArray[i].remainingCommands == 0)) i++;
					if (i == slaveCount) {

						this->state = translateState(standbyInfo.state) ;
					} /* if (i==slaveCount) */

				} else if (evt->args.ev.event == OMX_EventCmdReceived) {
					slavesArray[evt->args.ev.id].commandReceived = OMX_TRUE;

					DEBUGLOG(("(Ctrl %x) waitForCommandState -> node %d acknowledged a command\n", this, evt->args.ev.id)) ;
#ifdef OSTTRACE
					OstTraceFiltInst1(TRACE_FLOW, "ENS_HOST: Controller::waitForCommandState CTRL_EVENT_FSMSIG node %d acknowledged a command", evt->args.ev.id);
#endif
					int i=0 ;
					while ((i<slaveCount) && (OMX_TRUE == slavesArray[i].commandReceived)) i++;
					if (i == slaveCount) {
						eventHandler->eventHandler(OMX_EventCmdReceived, evt->args.ev.data1, evt->args.ev.data2);
						for (i=0; i<slaveCount; i++) slavesArray[i].commandReceived = OMX_FALSE;
					}  
				} else {
					DEBUGLOG(("(Ctrl %x) waitForCommandState -> unexpected event\n", this)) ;
#ifdef OSTTRACE
					OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::waitForCommandState CTRL_EVENT_FSMSIG unexpected event");
#endif
					this->state = (FSM_State) &Controller::invalidState ;
				}
			}
			break;
		   
		default:
			ARMNMF_DBC_ASSERT(0);
			break;
	}
} /* waitForCommandState */


void Controller::waitForControllersPause(const CtrlEvent *evt) {
	switch(CtrlEvent_FSMSIGnal(evt)) {
		case FSM_ENTRY_FSMSIG:		
			DEBUGLOG(("(Ctrl %x) waitForControllersPause -> enter\n", this)) ;
#ifdef OSTTRACE
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::waitForControllersPause FSM_ENTRY_FSMSIG");
#endif
			break;
		case FSM_EXIT_FSMSIG:
			break;
			
		case CTRL_NEIGHBOUR_EVENT_FSMSIG:
			ARMNMF_DBC_PRECONDITION(evt->args.ev.data1 == (unsigned int)standbyInfo.command) ;
			
			if (controllersInState(OMX_StatePause)) {
				DEBUGLOG(("(Ctrl %x) waitForControllersPause -> neigbours in pause\n", this)) ;
#ifdef OSTTRACE
				OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::waitForControllersPause CTRL_NEIGHBOUR_EVENT_FSMSIG neighbours in pause");
#endif
				
				if (standbyInfo.state == OMX_StateIdle) {
					this->state = (FSM_State) &Controller::transientToExecutingState ;
					forwardStateChange(OMX_StateExecuting) ;
				}
				else if (standbyInfo.state == OMX_StateExecuting) {
					this->state = (FSM_State) &Controller::transientToIdleState ;
					forwardStateChange(OMX_StateIdle) ;
				}
				else this->state = (FSM_State) &Controller::invalidState ;
				
				standbyInfo.state = OMX_StateInvalid ;
			}			
			break;
		   
		default:
			ARMNMF_DBC_ASSERT(0);
			break;
	}
} /* waitForControllersPause */


void Controller::invalidState(const CtrlEvent * evt) {
	switch(CtrlEvent_FSMSIGnal(evt)) {
		case FSM_ENTRY_FSMSIG: 
			DEBUGLOG(("(Ctrl %x) Invalid - enter\n", this)) ;
#ifdef OSTTRACE
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::invalidState FSM_ENTRY_FSMSIG");
#endif
			eventHandler->eventHandler(OMX_EventError, (t_uint32)OMX_ErrorInvalidState, 0);
			break;
			
		default:
			DEBUGLOG(("(Ctrl %x) Invalid - default\n", this)) ;
#ifdef OSTTRACE
			OstTraceFiltInst0(TRACE_FLOW, "ENS_HOST: Controller::invalidState default");
#endif
			break ;
	}
} /* invalidState */


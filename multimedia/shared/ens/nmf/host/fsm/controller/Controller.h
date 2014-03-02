/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Controller.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "ENS_Redefine_Class.h"

#include "FSM.h"
#include "CtrlEvent.h"

#include "eventhandler.hpp"
#include "sendcommand.hpp"          

typedef struct {
  Isendcommand  * itf ;     // Not NULL, but may be not binded
  
  OMX_STATETYPE  	   state ;

  OMX_BOOL             commandReceived ; 

  OMX_COMMANDTYPE      currentCommand ;  // Defined if remainingCommands != 0
  t_uint8 			   remainingCommands ;
} slaveInfo_t;

typedef struct {
	Ieventhandler  * itf ; // Not NULL, but may be not binded
    OMX_STATETYPE  	     state ;
} controllerInfo_t;

typedef struct {
	OMX_STATETYPE          state ;
	OMX_COMMANDTYPE        command ;
	t_uword                id ;
} standbyInfo_t ;


/* Mapping for controllers */
#define PREVIOUS_CONTROLLER_ID  ((int)(0))
#define NEXT_CONTROLLER_ID      ((int)(1))


class Controller : public FSM {

public:
	ENS_API_IMPORT virtual ~Controller() = 0 ;	
	
	// Interface from proxy - Catch all command
	ENS_API_IMPORT void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) ;
	// Interface from slaves - Catch OMX_EventCmdComplete and OMX_EventError(->Invalid State)
	ENS_API_IMPORT void slavesEventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 id) ;
	// Interface from binded controllers
	ENS_API_IMPORT void controllersEventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 id) ;
	
protected:
	/* 
	 * bufferUser parameter is used to optimize state transition when controller is binded to other controllers. 
	 * If false (should be the default answer), the controller synchronize itself with neighbour transitions, to avoid race conditions.
	 * If the component supervised by this controller is bufferUser, this synchronization is not mandatory.
	 */
	ENS_API_IMPORT void init(t_uint8 slaveCount, Ieventhandler *nmfitf, Ieventhandler *previousController, Ieventhandler  *nextController, bool bufferUser) ;
	
	ENS_API_IMPORT void setSlave(t_uint8 slaveIndex, Isendcommand * slave) ;
	ENS_API_IMPORT void forwardStateChange(const OMX_STATETYPE targetState) ;
	ENS_API_IMPORT void waitCommandFromSlave(t_uint8 slaveIndex, OMX_COMMANDTYPE command, t_uint8 count) ;
	
	/* 
	 * getState returns : OMX_StateIdle or OMX_StateExecuting or OMX_StatePause or ... OMX_StateTransient. 
	 * It could be extended if needed.
	 */
	OMX_STATETYPE getState() { return localState; }
	
	
	/* 
	 * Delegate forward of OMX_CommandPortDisable, OMX_CommandPortEnable and OMX_CommandFlush
	 * Called by the controller FSM each time a Command must be forwared to some sub-components
	 */	
	virtual  void forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) = 0 ;

	/* 
	 * Delegate forward of OMX_EventCmdComplete for OMX_CommandPortDisable, OMX_CommandPortEnable and OMX_CommandFlush
	 * Called by the controller FSM each time a Command is completed (all waitCommandFromSlave are completed), and an event must be
	 * forwarded to the proxy.
	 */
	virtual  void forwardPortEvent(OMX_COMMANDTYPE cmd, t_uword id) = 0 ;
	
	
private:	
	FSM_State translateState(OMX_STATETYPE state) ;
	void forwardStateToControllers(const OMX_STATETYPE targetState) ;
	bool controllersNotInState(OMX_STATETYPE state) ;
	bool controllersInState(OMX_STATETYPE state) ;
	
	void idleState(const CtrlEvent *);
	void executingState(const CtrlEvent *);
	void pauseState(const CtrlEvent *);
	void invalidState(const CtrlEvent *);

	void transientToIdleState(const CtrlEvent *);
	void idleToPauseState(const CtrlEvent *);
	void pauseToExecutingState(const CtrlEvent *);
	void executingToPauseState(const CtrlEvent *);
	void pauseToIdleState(const CtrlEvent *);
	
	void transientToExecutingState(const CtrlEvent *);
	void idleToExecutingState(const CtrlEvent *);
	void executingToIdleState(const CtrlEvent *);
	
	void defaultState(const CtrlEvent *evt, OMX_STATETYPE currentState) ;
	bool waitForNextState(const CtrlEvent *evt, OMX_STATETYPE nextState) ;
	void waitForCommandState(const CtrlEvent *evt) ;
	void waitForControllersPause(const CtrlEvent *evt) ;
	
	
private:
	t_uint8       	   slaveCount ;
	bool 			   isBufferUser ;

	slaveInfo_t          * slavesArray ;  // Array of slaveInfo_t
    Ieventhandler * eventHandler ; // Proxy
    standbyInfo_t		   standbyInfo ;
    
    controllerInfo_t       controllers[2] ; // binded controllers
    OMX_STATETYPE          localState ;

} ; /* Controller */

#endif /* _CONTROLLER_H_ */

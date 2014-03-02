/*****************************************************************************
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _IMGENS_FSM_H_
#define _IMGENS_FSM_H_

#include <omxil/OMX_Core.h>

class IMGENS_API ImgEns_FsmEvent
//*****************************************************************************
{
public:
	ImgEns_FsmEvent(int signal) { mSignal = signal; }
	virtual ~ImgEns_FsmEvent(void) {}

	int signal(void) const { return mSignal;}
protected:
	int mSignal;
};


// Abstract base class for Finite State Machines
class IMGENS_API ImgEns_Fsm
//*****************************************************************************
{
public:
	typedef OMX_ERRORTYPE (ImgEns_Fsm::*_fnState)(const ImgEns_FsmEvent *);
	enum enumFsmSignal
	{
		eSignal_Entry,
		eSignal_Exit,
		eSignal_LastEnum,  //Last
	};
	enum enumFsmError
	{
		eError_None=0,
		eError_IncorrectStateOperation,
	};

	ImgEns_Fsm();
	virtual ~ImgEns_Fsm() = 0;

	void init(_fnState initialState);
	int dispatch(const ImgEns_FsmEvent* event);
	_fnState getFsmState() const { return mState; }
	void stateTransition(_fnState targetState) { mState = targetState;}

	//Notification function 
	virtual int NotifyChangeState(const _fnState /*currentState*/, const _fnState /*nextState*/) const {return 0;}

protected:
	_fnState mState;
};


// Struct for having verbose description of fsm states
typedef struct 
{
	ImgEns_Fsm::_fnState fn_state; //Fsm state
	const char *Name;
	int fsmState;
	int parentState;
} s_FsmStateDescription;



class IMGENS_API ImgEns_EntryEvt: public ImgEns_FsmEvent
//*****************************************************************************
{
public:
	ImgEns_EntryEvt(ImgEns_Fsm::_fnState st)
		: ImgEns_FsmEvent(ImgEns_Fsm::eSignal_Entry) { mPreviousState = st; }
	ImgEns_Fsm::_fnState previousState(void) const { return mPreviousState; }
private:
	ImgEns_Fsm::_fnState mPreviousState;
};

#if 0
enum
{

  OMX_StateTransientToIdle =OMX_StateVendorStartUnused,
  OMX_StateTransient,
  OMX_StateLoadedToIdleHOST,
  OMX_StateLoadedToIdleDSP,
  OMX_StateIdleToLoaded,
  OMX_StateIdleToExecuting,
  OMX_StateIdleToPause,
  OMX_StateExecutingToIdle,
  OMX_StateExecutingToPause,
  OMX_StatePauseToIdle,
  OMX_StatePauseToExecuting,
  OMX_StateTransientToDisable,
  OMX_StateLoadedToTestForResources,
  OMX_StateCheckRscAvailability,
  OMX_StateFlushing,
  OMX_StateIdleSuspended,
  OMX_StatePauseSuspended,
};
#endif

enum
{
	eIMGOMX_EventCmdReceived = OMX_EventVendorStartUnused,
	eIMGOMX_EventResourcesReceived,
	eIMGOMX_EventLast,
};
#endif // _IMGENS_FSM_H_

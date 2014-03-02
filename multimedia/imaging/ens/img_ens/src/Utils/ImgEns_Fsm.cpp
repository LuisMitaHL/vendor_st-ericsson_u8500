/*****************************************************************************
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "ImgEns_Shared.h"
#include "ImgEns_Fsm.h"
#include "ImgEns_FsmEvent.h"

ImgEns_Fsm::ImgEns_Fsm()
{
	mState=0;
}

ImgEns_Fsm::~ImgEns_Fsm()
{
}

void ImgEns_Fsm::init(_fnState initialState)
{ // set current state to initial state
	mState = initialState;
}

int ImgEns_Fsm::dispatch(const ImgEns_FsmEvent* evt)
{
	int error             = eError_None;
	_fnState currentState = mState;

#ifdef BELLAGIO_NOT_COMPATIBLE
	mState = 0;
	// process event in current state
	if(!currentState)
		return eError_IncorrectStateOperation;
#endif
	error = (this->*currentState)(evt);
	if (error != eError_None)
	{
		mState = currentState;
		return error;
	}

#ifdef BELLAGIO_NOT_COMPATIBLE
	if (mState != 0)
#else
	if (mState != currentState)
#endif
	{
		NotifyChangeState(currentState, mState);
		// state transition taken so exit current state
		ImgEns_FsmEvent fsmEvt(eSignal_Exit);
		(this->*currentState)(&fsmEvt);

		// enter target state
		ImgEns_EntryEvt fsmEvt_2(currentState);
		(this->*mState)(&fsmEvt_2);
	}
#ifdef BELLAGIO_NOT_COMPATIBLE
	else
	{  //restore initial state
		mState = currentState;
	}
#endif

	return eError_None;
}


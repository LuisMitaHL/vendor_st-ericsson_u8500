/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Fsm.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Fsm.h"
#include "ENS_FsmEvent.h"
#include "ENS_DBC.h"
#define OMXCOMPONENT "ENS_FSM"
#include "osi_trace.h"

ENS_API_EXPORT ENS_Fsm::ENS_Fsm()
    : mState(0)
{}

ENS_API_EXPORT ENS_Fsm::~ENS_Fsm()
{}

ENS_API_EXPORT void ENS_Fsm::init(FsmState initialState) {
    // set current state to initial state
    mState = initialState;
}

ENS_API_EXPORT OMX_ERRORTYPE ENS_Fsm::dispatch(const ENS_FsmEvent* evt)
{
    OMX_ERRORTYPE error;
    FsmState currentState = mState;

#ifdef BELLAGIO_NOT_COMPATIBLE
    mState       = 0;
#endif

    // process event in current state
#ifdef BELLAGIO_NOT_COMPATIBLE
    if(!currentState)
        return OMX_ErrorIncorrectStateOperation;
#endif
    error = (this->*currentState)(evt);
    if(error != OMX_ErrorNone) {
        mState = currentState;
        return error;
    }

#ifdef BELLAGIO_NOT_COMPATIBLE
    if (mState != 0)
#else
    if (mState != currentState)
#endif
    {
        // state transition taken so
        // exit current state
        ENS_FsmEvent fsmEvt(EXIT_SIG);
        (this->*currentState)(&fsmEvt);
        
        // enter target state
        ENS_EntryEvt fsmEvt_2(currentState);
        (this->*mState)(&fsmEvt_2);
    } 
#ifdef BELLAGIO_NOT_COMPATIBLE
    else
    {
        mState = currentState;
    }
#endif
    
    return OMX_ErrorNone;
} 





/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FSM.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "FSM.h"

ENS_API_EXPORT FSM::FSM() {
    this->state = 0 ;

#ifndef NDBC
    this->dispatching = 0;
#endif	
}

ENS_API_EXPORT FSM::~FSM() {
}

ENS_API_EXPORT void FSM::init(FSM_State initialState) {
    // check that the initial state is entered only once
    ARMNMF_DBC_PRECONDITION(this->state == 0);
    this->state = initialState;
    
    FsmEvent e;
    e.signal = FSM_ENTRY_FSMSIG;
    
    // enter initial state
    (this->*state)(&e);
} /* init */


ENS_API_EXPORT void FSM::dispatch(const FsmEvent *evt) {
    FsmEvent e;
    FSM_State currentState;

#ifndef NDBC
    // check that we don't dispatch an event
    // while already processing an event
	ARMNMF_DBC_ASSERT_MSG(!this->dispatching, "FSM is already dispatching an event") ;
    this->dispatching = true;
#endif

    ARMNMF_DBC_PRECONDITION(this->state != 0);

    currentState    = this->state;
    this->state     = 0;

    (this->*currentState)(evt);

    if (this->state != 0) {
        // state transition taken so 
        // exit the source state
        e.signal = FSM_EXIT_FSMSIG;
        (this->*currentState)(&e);

        // enter target state
        e.signal = FSM_ENTRY_FSMSIG;
        (this->*state)(&e);
    } 
    else {
        this->state = currentState;
    }
   
#ifndef NDBC
    this->dispatching = false;
#endif    
} /* dispatch */


/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FSM.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "fsm/generic.nmf"
#include <dbc.h>

void FSM_init(FSM_p this, FSM_State initialState) {
    FsmEvent e;
    e.signal = FSM_ENTRY_SIG;

    // check that the initial state is entered only once
    PRECONDITION(this->state == 0);
    this->state = initialState;

    // enter initial state
    (*initialState)(this, &e);
}

void FSM_dispatch(FSM_p this, const FsmEvent *evt) {
    FsmEvent  e;
    FSM_State currentState;

#ifndef NDBC
    // check that we don't dispatch an event
    // while already processing an event
    PRECONDITION(!this->dispatching);
    this->dispatching = true;
#endif

    PRECONDITION(this->state != 0);

    currentState = this->state;
    this->state  = 0;

    (*currentState)(this, evt);

    if (this->state != 0) {
        // state transition taken so
        // exit the source state
        e.signal = FSM_EXIT_SIG;
        (*currentState)(this, &e);

        // enter target state
        e.signal = FSM_ENTRY_SIG;
        (*this->state)(this, &e);
    } else {
        this->state = currentState;
    }

#ifndef NDBC
    this->dispatching = false;
#endif
}

void FSM_traceInit(FSM_p this, TraceInfo_t *addr, t_uint16 id) {
    if (addr != 0) {     // as fsmInit() may be call again to enable port
        this->traceObject.mTraceInfoPtr = addr;
        this->traceObject.mId1          = id;
    }
}

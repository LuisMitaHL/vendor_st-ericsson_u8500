/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Component.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "fsm/generic/include/FSM.h"
#include "fsm/component/include/Port.h"
#include <omxevent.idt>

// local type matching interface structure generated
// by nmf tools for eventhandler interface
typedef struct {
    void *THIS;
    void (*eventHandler)(
        OMX_EVENTTYPE event, unsigned long data1, unsigned long data2);
} Imyeventhandler;

typedef struct Component {
    FSM fsm;

    int   portCount;
    Port *ports;

    volatile bool pendingEvent;

    OMX_STATETYPE transientState;

    const Imyeventhandler *eventHandler;

    // virtual functions
    // NOTE: "reset()" function will be removed in a future version.
    // Is replaced by "StateChangeIndication()" when newState is OMX_StateIdle.
    void (*reset)(struct Component *this);
    void (*process)(struct Component *this);
    void (*disablePortIndication)(t_uint32 portIdx);
    void (*enablePortIndication)(t_uint32 portIdx);
    void (*flushPortIndication)(t_uint32 portIdx);
    void (*stateChangeIndication)(OMX_STATETYPE oldState, OMX_STATETYPE newState);

} Component;

#endif // _COMPONENT_H_

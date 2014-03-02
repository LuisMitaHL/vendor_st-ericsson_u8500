/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   OmxEvent.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _OMX_EVENT_H_
#define _OMX_EVENT_H_

#include "fsm/generic/include/FsmEvent.h"
#include <buffer.idt>
#include <omxstate.idt>

typedef enum {
    OMX_PROCESS_SIG = FSM_USER_SIG,
    OMX_RETURNBUFFER_SIG,
    OMX_FLUSH_SIG,
    OMX_SETSTATE_SIG,
    OMX_DISABLE_PORT_SIG,
    OMX_ENABLE_PORT_SIG,
    OMX_LAST_SIG
} OmxSignal;

typedef struct {
    FsmEvent fsmEvent;
    union {
        struct {
            Buffer_p buffer;
            int      portIdx;
        } returnBuffer;

        struct {
            OMX_STATETYPE state;
        } setState;

        struct {
            int portIdx;
        } portCmd;
    } args;
} OmxEvent;

static inline OmxEvent_signal(const OmxEvent * evt) {
    return evt->fsmEvent.signal;
}
#endif // _OMX_EVENT_H_

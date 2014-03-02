/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FSM.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _FSM_H_
#define _FSM_H_

#include <stdbool.h>
#include "FsmEvent.h"
#include "trace.idt"

typedef  struct Trace {
    t_uint16     mId1;
    TraceInfo_t *mTraceInfoPtr;
} TRACE_t;

struct FSM;

typedef void (*FSM_State)(struct FSM *, const FsmEvent *);

typedef struct {
    TRACE_t   traceObject;
    FSM_State state;
#ifndef NDBC
    bool dispatching;
#endif
} FSM;

typedef FSM *FSM_p;

#define FSM_TRANSITION(targetState) (((FSM *)this)->state = (FSM_State)targetState)

#define OMX_ALL 0xFFFFFFU

#endif // _FSM_H_

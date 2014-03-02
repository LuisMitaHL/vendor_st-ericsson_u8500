/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TRACE_H_
#define _TRACE_H_

#ifdef TRACE_DEBUG
#include "itc_api.h"
#endif

#include "t1xhv_types.h"

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/***
 * for irp trace, IDs of different messages
 */
typedef enum {    
/*F001*/IRP_TRACE_CLK_SWITCH = 0xF001U,
/*F002*/IRP_TRACE_EVENT,
/*F003*/IRP_TRACE_EVENT_QUEUED,
/*F004*/IRP_TRACE_PE_WRITE,
/*F005*/IRP_TRACE_PE_READ,
/*F006*/IRP_TRACE_READ_COMPD,
/*F007*/IRP_TRACE_INTERCEPTED_PE,
/*F008*/IRP_TRACE_COMPLETED_GE,
/*F009*/IRP_TRACE_EWARP_IT,
/*F00A*/IRP_TRACE_HOST_IT,
/*F00B*/IRP_TRACE_STATE_SWITCH,
/*F00C*/IRP_TRACE_SUBSTATE_SWITCH,
/*F00D*/IRP_TRACE_LOCK,
/*F00E*/IRP_TRACE_ERROR,
/*F00F*/IRP_TRACE_CLK_DETECTED,
/*F010*/IRP_TRACE_CLK_LOST,

/*F080*/GRB_TRACE_START = 0xF080,
/*F081*/GRB_TRACE_END,
/*F082*/GRB_TRACE_ABORT

} trace_id_t;

typedef struct {
    t_uint16 timestamp;
    trace_id_t trace_id;
    t_uint16 data1;
    t_uint16 data2;
} trace_t;

/*------------------------------------------------------------------------
 * Variables (exported)
 *----------------------------------------------------------------------*/

#ifdef TRACE_DEBUG
#define TRACE_SIZE 2048U
extern volatile trace_t G_trace[TRACE_SIZE];
extern volatile t_uint16 G_trace_idx;
#endif

/*------------------------------------------------------------------------
 * Functions (inlined)
 *----------------------------------------------------------------------*/
#pragma inline
static void TRACE(trace_id_t trace_id, t_uint16 data1, t_uint16 data2)
{
#ifdef TRACE_DEBUG

    t_uint32 current_time;
    t_uint16 trunked_time;
    current_time = ITC_GET_TIMER_32();
    trunked_time = (t_uint16)(current_time & 0xFFFFU);
    G_trace[G_trace_idx].timestamp = trunked_time;
    G_trace[G_trace_idx].trace_id = trace_id;
    G_trace[G_trace_idx].data1 = data1;
    G_trace[G_trace_idx].data2 = data2;
        
    G_trace_idx++;
    if (G_trace_idx == TRACE_SIZE - 1) G_trace_idx = 1; /* skip first and last elements, which are used as delimiters */
    G_trace[G_trace_idx].timestamp = 0xDEADU;
    G_trace[G_trace_idx].trace_id = 0xF00DU;
    G_trace[G_trace_idx].data1 = 0xDEADU;
    G_trace[G_trace_idx].data2 = 0xF00DU;
#endif /* TRACE_DEBUG */
}

#endif /* _TRACE_H_ */

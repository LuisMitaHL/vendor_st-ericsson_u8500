/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef GRAB_CORE_H_
#define GRAB_CORE_H_


struct s_current_grab {
    int      grab_running;
    t_uint16 client_id;
    t_uint16 buf_id;
    int      stab_enabled;
};

extern struct s_current_grab G_current_grab[5];
extern enum e_grabInfo G_end_of_task[5];

#include <FSM.h>

#ifdef GRB_GLOBAL 
#define TRCS_EXTERN 
#else
#define TRCS_EXTERN extern
#endif
/*
  Since grab_core is not derived from ENS FSM, 
  we need to declare here locally the structures
  needed by OST macros. 
*/
  
// typedef struct {
//	t_uint32 parentHandle;  
/*              this is the OMX handle of the parent component, it appears in the header of traces
 *              that are sent to the host and can be used by the host to filter them at its end */
//	t_uint16 traceEnable;
/*              this is a mask on the desired trace level. It acts as a filter on the emitter side,
 *              in the sense that if this mask is not enabled the traces are not generated at all.
 *              This mask ccrresponds to the definitions of TRACE_DEBUG, TRACE_WARNING etc. */ 
//	t_uint32 dspAddr;
/*              this is unused in our case (it is used in ENS to keep track of shared trace info area 
 *              between ARM and DSP) */
//} TraceInfo_t;

TRCS_EXTERN TraceInfo_t traceInfo
#ifdef GRB_GLOBAL
   = {0xDEADF00DUL,0xFFFFU,0}  
#endif
;

//typedef  struct Trace {
//	t_uint16      mId1;
/*              this appears in the header of traces that are sent to the host and can be used by 
 *              the host to filter them at its end */
//	TraceInfo_t * mTraceInfoPtr;
//} TRACE_t;

TRCS_EXTERN TRACE_t     traceObject
#ifdef GRB_GLOBAL
 = {0,&traceInfo}
#endif
;

TRCS_EXTERN TRACE_t *   this
#ifdef GRB_GLOBAL
 = (TRACE_t *)&traceObject;
#endif
;


#endif /*GRAB_CORE_H_*/

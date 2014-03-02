/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated trace.idt defined type */
#if !defined(__TRACE_H_)
#define __TRACE_H_


enum comp_id {
  CONTROLLER_ID=0,
  BINDINGS_ID=10,
  COMP_ID=100,
  ENS_COMP_ID=1000,
  ENS_COMP_FSM_ID,
  PROCESSING_COMP_ID,
  ENS_PORT_ID=1010,
  ENS_PORT_FSM_ID=1100};

typedef struct t_xyuv_TraceInfo_t {
  t_uint32 parentHandle;
  t_uint16 traceEnable;
  t_uint32 dspAddr;
} TraceInfo_t;

typedef struct t_xyuv_TraceInfoDspCreate_t {
  t_uint32 parentHandleH;
  t_uint32 parentHandleL;
  t_uint32 traceEnable;
} TraceInfoDspCreate_t;

#endif

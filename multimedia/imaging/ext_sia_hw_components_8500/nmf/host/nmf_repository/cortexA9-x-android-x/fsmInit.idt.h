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

/* Generated fsmInit.idt defined type */
#if !defined(__FSMINIT_H_)
#define __FSMINIT_H_

#include <trace.idt.h>

typedef struct t_xyuv_fsmInit_t {
  t_uint16 portsDisabled;
  t_uint16 portsTunneled;
  TraceInfo_t* traceInfoAddr;
  t_uint16 id1;
} fsmInit_t;

#endif

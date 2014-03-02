/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/trace/xti/inc/xti.h - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#ifndef __INC_XTI_H
#define __INC_XTI_H

#include <inc/type.h>

typedef struct {
    t_shared_reg reg;
    t_shared_reg RESERVED[15];
} t_xti_trace_reg;

#define XTI_NUM_CHANNEL 256

typedef volatile struct {
    t_xti_trace_reg osmo[XTI_NUM_CHANNEL];
    t_xti_trace_reg osmoT[XTI_NUM_CHANNEL];
} t_xti_osmo_space;
#endif /* __INC_XTI_H */

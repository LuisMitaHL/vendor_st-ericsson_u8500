/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "trace.h"

#ifdef TRACE_DEBUG
#pragma align  4
volatile t_uint16 G_trace_idx;
volatile trace_t G_trace[TRACE_SIZE];
#endif

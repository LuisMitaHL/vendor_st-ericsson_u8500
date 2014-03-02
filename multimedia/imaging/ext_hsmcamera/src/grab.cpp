/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_grab.h"

#undef   DBGT_LAYER
#define  DBGT_LAYER 1
#undef  DBGT_PREFIX
#define DBGT_PREFIX "GRAB"
#include "debug_trace.h"

void CGrab::execute(enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id)
{
	DBGT_PTRACE("CGrab::execute: pipe_id=%d client_id=0x%X", pipe, client_id);
	mGrabCmd.execute(pipe,params,client_id);
}


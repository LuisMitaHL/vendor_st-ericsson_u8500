/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include "grab.h"

void CGrab::execute(enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id)
{
	mGrabCmd.execute(pipe,params,client_id);
}

void CGrab::disableDspFineLowPower(void)
{
  mGrabCmd.disableDspFineLowPower();
}

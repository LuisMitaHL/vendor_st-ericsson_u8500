/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef GRAB_H_
#define GRAB_H_

#include "host/grab/api/cmd.hpp"

#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(Camera);
#endif
class CGrab
{
	public :
		void execute(enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id);	
		void disableDspFineLowPower(void);	
		
		Igrab_api_cmd mGrabCmd;
};

#endif /* GRAB_H_ */

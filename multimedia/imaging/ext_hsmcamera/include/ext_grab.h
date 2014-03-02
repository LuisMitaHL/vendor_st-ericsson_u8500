/*
 * Copyright (C) ST-Ericsson SA 2011 All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_GRAB_H_
#define _EXT_GRAB_H_

#include "host/ext_grab/api/cmd.hpp"

class CGrab
{
	public :
		void execute(
                enum e_grabPipeID pipe,
                struct s_grabParams params,
                t_uint16 client_id);

		Iext_grab_api_cmd mGrabCmd;
};

#endif /* _EXT_GRAB_H_ */

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NMF_FRAMERATE_H_
#define ITE_NMF_FRAMERATE_H_

//For Linux
#include <inc/typedef.h>

#include "grab_types.idt.h"


t_uint32 ITE_NMF_CheckReadFramerate(float framerate_in);
t_uint32 ITE_NMF_CheckFrameRateCount(enum e_grabPipeID pipe,float framerate_in);

#endif /* ITE_NMF_FRAMERATE_H_ */

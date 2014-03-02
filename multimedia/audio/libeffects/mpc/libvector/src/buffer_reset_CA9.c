/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************
File: buffer_reset_CA9.c 

Description:
------------
The file contains routine for buffer reset for Cortex A9 and Neon.

Flags used:
1. ARM : contains implementation for fixed point for CA9.
*******************************************************************************/
#include "vector.h"
#include "btrue.h"
#ifdef ARM
AUDIO_API_EXPORT void
buffer_reset_arm(char *p, char val, int size)
{

	unsigned int i;
	for(i=size;i>0;i--)
	{
		*p = val;
		p++;
	} 
}											/* end buffer_reset() */

#endif /* ARM */

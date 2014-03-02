/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************
File: vector_scale_CA9.c 

Description:
------------
The file contains routine for scaling for Cortex A9 and Neon.

Flags used:
1. ARM : contains implementation for fixed point for CA9.
*******************************************************************************/
#ifdef ARM

#include "vector.h"
void vector_scale_arm(int *src, int *dest, int scale, int len)
{
	int sval ;
	int xval ;
	unsigned int i;
    unsigned int len_cnt = (unsigned int)len;

	sval = scale;
	for(i= len_cnt; i > 0;i--)
    {
	    xval = *src++;
		*dest++ = (int)( ((long long)xval *sval )>>31) ;
    }
    return;
}
#endif


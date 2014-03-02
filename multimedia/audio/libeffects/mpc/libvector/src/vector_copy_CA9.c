/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************
File: vector_copy_CA9.c 

Description:
------------
The file contains routine for buffer copy for Cortex A9 and Neon.

Flags used:
1. ARM : contains implementation for fixed point for CA9.
*******************************************************************************/
#include "vector.h"

#ifdef ARM

void
vector_copy_arm_simd(int * __restrict p, int * __restrict q, int len)
{
	unsigned int i;
    int tmp_val;

	for (i = len; i > 0; i--) {

		tmp_val = *q;
		q += 1;

		*p = tmp_val;
		p +=1;
	}
}

void
vector_copy_arm(int *p, int *q, int len, int incr1, int incr2)
{
	int             i;

	for (i = 0; i < len; i++) {
		*p = (*q);
		p += incr1;
		q += incr2;
	}
}

#endif /* ARM */

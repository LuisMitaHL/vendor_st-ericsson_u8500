/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "vector.h"
#include "btrue.h"

#ifndef __flexcc2__

void
buffer_reset(char *p, char val, int size)
{
	int             local_size;

	while (size > 0) {

		local_size = wmin(size, 1023);

		HWLOOP(local_size) {
			*p = val;
			p++;
		} END_HWLOOP;
		size -= local_size;
	}
}											/* end buffer_reset() */

#else // __flexcc2__

void
buffer_reset(char *p, char val, int size)
{
	/* The compiler generates ubyte access, cast to int to force reset
	 * of msb as well */
	int            *p_int = (int *) p;
	int             local_size;

	while (size > 0) {

		local_size = wmin(size, 1023);

		HWLOOP(local_size) {
			*p_int = val;
			p_int++;
		} END_HWLOOP;
		size -= local_size;
	}
}											/* end buffer_reset() */

#endif // __flexcc2__

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "vector.h"
#include "btrue.h"

#ifndef __flexcc2__

#ifdef ARM
AUDIO_API_EXPORT  void
buffer_copy_arm(char *p, char *q, int size)
{
	unsigned int i;
	for(i=size;i>0;i--)
	{
		*p = *q;
		p++;
		q++;
	} 
}											/* end buffer_copy_arm() */

#endif /* ARM */

void
buffer_copy(char *p, char *q, int size)
{
	int             local_size;

	while (size > 0) {

		local_size = wmin(size, 1023);

		HWLOOP(local_size) {
			*p = *q;
			p++;
			q++;
		} END_HWLOOP;
		size -= local_size;
	}
}											/* end buffer_copy() */

#else // __flexcc2__

void
buffer_copy(char *p, char *q, int size)
{
	/* The compiler generates ubyte access, cast to int to force reset
	 * of msb as well */
	int            *p_int = (int *) p;
	int            *q_int = (int *) q;
	int             local_size;

	while (size > 0) {

		local_size = wmin(size, 1023);

		HWLOOP(local_size) {
			*p_int = *q_int;
			p_int++;
			q_int++;
		} END_HWLOOP;
		size -= local_size;
	}
}											/* end buffer_copy() */

#endif // __flexcc2__

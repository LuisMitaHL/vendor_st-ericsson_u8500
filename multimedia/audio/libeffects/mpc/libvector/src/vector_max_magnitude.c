/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef _NMF_MPC_
#include <libeffects/mpc/libvector.nmf>
#endif //#ifdef _NMF_MPC_
#include "vector.h"

#ifndef MMDSP

Float
vector_max_magnitude(Float *src1, int len, int incr)
{
	Float           max;
	int             i;
	Float           tmp;

	max = (Float) 0;

	for (i = 0; i < len; i++) {
		tmp = *src1;
		if (tmp < 0) {
			tmp = -tmp;
		}
		if (max < tmp) {
			max = tmp;
		}
		src1 += incr;
	}

	return max;
}											/* end vector_max_magnitude() */

#else

Float
vector_max_magnitude(Float *src1, int len, int incr)
{
	Float           max;
	int             local_len;
	assert(len >= 0);
	assert(len <= 1024);
	
	max = (Float) 0;

	while ( len>0 ) {
		local_len = wmin( len, 1023 );
		
		HWLOOP(local_len) {
			max = wmaxm(*src1, max);
			src1 += incr;
		} END_HWLOOP;
		len -= local_len;
	}
		
	return max;
}											/* end vector_max_magnitude() */

#endif /* MMDSP */

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

void
vector_copy_neg(Float *p, Float *q, int len, int incr1, int incr2)
{
	int             i;

	for (i = 0; i < len; i++) {

		*p = -(*q);
		p += incr1;
		q += incr2;
	}
}											/* end vector_copy_neg */

#else

void
vector_copy_neg(Float *p, Float *q, int len, int incr1, int incr2)
{
	register int ix1 AT_REG(IX1);
	register int ix2 AT_REG(IX2);
	int local_len;
	
	ix1 = incr1;
	ix2 = incr2;

	while ( len > 0 ) {
		local_len = wmin(len,1023);
		
		HWLOOP(local_len) {
			*p = wneg(*q);
			q += ix2;
			p += ix1;
		} END_HWLOOP;
		len -= local_len;
	}
}											/* end vector_copy_neg */

#endif /* MMDSP */

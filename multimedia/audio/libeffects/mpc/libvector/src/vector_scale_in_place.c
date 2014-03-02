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

#ifdef ARM
#include "cortexa9_common.h"
#endif

#ifndef MMDSP

void
vector_scale_in_place(Float *src, Float scale, int len, int incr)
{
	int             i;

	for (i = 0; i < len; i++) {
		*src = (*src) * scale;
		src += incr;
	}
}											/* end vector_scale() */

#ifdef ARM
void
vector_scale_in_place_arm(int *src, int scale, int len, int incr)
{
	int             i;
  
	for (i = 0; i < len; i++) {
        *src = builtin_mpf_32((*src) , scale);		
		src += incr;
	}
}
#endif
#else

void
vector_scale_in_place(Float *src, Float scale, int len, int incr)
{
	register Float sval AT_REG(R0H);
	register Float xval AT_REG(R0L);
	register int   idx  AT_REG(IX1);
	register Float *res AT_REG(AX1);
	
	idx  = incr;
	res  = src;
	sval = scale;

	/* soft pile */
	xval = *src;
	src += idx;
	
	assert(len > 0);
	assert(len <= 1024);
	HWLOOP(len - 1) {
		*res = wfmulr(xval, sval);
		xval = *src;
		src += idx;
		res += idx;
	} END_HWLOOP;
	*res = wfmulr(xval, sval);

}											/* end vector_scale() */

#endif /* MMDSP */

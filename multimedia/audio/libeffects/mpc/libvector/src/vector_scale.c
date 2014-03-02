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
vector_scale(Float *src, Float *dest, Float scale, int len)
{
	int             i;

	for (i = 0; i < len; i++) {
		*dest++ = (*src++) * scale;
	}
}											/* end vector_scale() */
#else

void
vector_scale(Float *src, Float *dest, Float scale, int len)
{
	register Float sval AT_REG(R0H);
	register Float xval AT_REG(R0L);

	sval = scale;
	xval = *src++;
	assert(len > 0);
	assert(len <= 1024);
	HWLOOP(len - 1) {
		*dest++ = wfmulr(xval, sval);
		xval = *src++;
	} END_HWLOOP;
	*dest = wfmulr(xval, sval);

}											/* end vector_scale() */

#endif /* MMDSP */

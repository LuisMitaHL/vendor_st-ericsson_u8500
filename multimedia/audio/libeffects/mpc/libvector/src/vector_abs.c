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

#include <math.h>
void
vector_abs(Float *p,Float *q,int len,int incr1)
{
	int i;

	for (i = 0; i < len; i++) {
		*q = fabs(*p);
		p += incr1;
		q += incr1;
	}
}      /* end of vector_abs */

#else

void
vector_abs(Float *p,Float *q,int len,int incr1)
{
	register int ix1 AT_REG(IX1);

	Float val_p;
	Float *p_res;

	assert(len>0);
	assert(len <=1024);

	ix1 = incr1;
	p_res = q;

	val_p = *p;
	p += ix1;
	HWLOOP(len-1) {
		*p_res = (Float) wabssat((int) val_p);
		p_res += ix1;
		val_p = *p;
		p += ix1;
	} END_HWLOOP;

	*p_res = (Float) wabssat((int) val_p);
}     /* end of vector_abs */

#endif /* MMDSP */
	

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
vector_swap(Float *p, Float *q, int len, int incr1, int incr2)
{
	int             i;
	Float           save;

	for (i = 0; i < len; i++) {
		save = *q;
		*q = *p;
		*p = save;

		p += incr1;
		q += incr2;
	}
}											/* end vector_swap */

#else

void
vector_swap(Float *p, Float *q, int len, int incr1, int incr2)
{
	register int ix1 AT_REG(IX1);
	register int ix2 AT_REG(IX2);
	Float           save;

	ix2 = incr2;
	ix1 = incr1;

	assert(len > 0);
	assert(len <= 1024);

	save = *q;
	*q = *p;
	*p = save;

	p += ix1;
	q += ix2;

	HWLOOP(len - 1) {
		save = *q;
		*q = *p;
		*p = save;

		p += ix1;
		q += ix2;
	} END_HWLOOP;

}											/* end vector_swap */

#endif /* MMDSP */

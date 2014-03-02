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
vector_add_copy(Float *res, Float *p, Float *q, int len, int incr)
{
	int             i;

	for (i = 0; i < len; i++) {
		*res = (*p) + (*q);
		res += incr;
		p += incr;
		q += incr;
	}
}											/* end vector_add_copy */

#ifdef ARM
void
vector_add_copy_arm(int *res, int *p, int *q, int len, int incr)
{
	int             i;

	for (i = 0; i < len; i++) {
		*res = (*p) + (*q);
		res += incr;
		p += incr;
		q += incr;
	}
}	
#endif
#else

void
vector_add_copy(Float *res, Float *p, Float *q, int len, int incr)
{
	register int ix AT_REG(IX1);
	Float           val_p;
	Float           val_q;
	Float           val_res;

	assert(len > 0);
	assert(len <= 1024);
		
	ix = incr;
	val_p = *p;
	p += ix;
	val_q = *q;
	q += ix;

	HWLOOP(len - 1) {
		val_res = (Float) waddsat((int) val_p, (int) val_q);
		val_p = *p;
		p += ix;
		val_q = *q;
		q += ix;
		*res = val_res;
		res += ix;

	} END_HWLOOP;
	val_res = (Float) waddsat((int) val_p, (int) val_q);
	*res = val_res;

}											/* end vector_add_copy */

#endif /* MMDSP */

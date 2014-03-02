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
vector_add(Float *p, Float *q, int len, int incr1, int incr2)
{
	int             i;

	for (i = 0; i < len; i++) {
		*p += (*q);
		p += incr1;
		q += incr2;
	}
}											/* end vector_add */

#ifdef ARM
void
vector_add_arm(int *p, int *q, int len, int incr1, int incr2)
{
	int             i;

	for (i = 0; i < len; i++) {
		*p += (*q);
		p += incr1;
		q += incr2;
	}
}
#endif
#else

void
vector_add(Float *p, Float *q, int len, int incr1, int incr2)
{
	register int ix1 AT_REG(IX1);
	register int ix2 AT_REG(IX2);

	Float           val_p, val_q, val_res;
	Float          *p_res;

	assert(len > 0);
	assert( len <= 1024);

	ix1 = incr1;
	ix2 = incr2;
	p_res = p;

	val_p = *p;
	p += ix1;
	val_q = *q;
	q += ix2;

	HWLOOP(len - 1) {
		val_res = (Float) waddsat((int) val_p, (int) val_q);
		val_p = *p;
		p += ix1;
		val_q = *q;
		q += ix2;
		*p_res = val_res;
		p_res += ix1;
	} END_HWLOOP;
	val_res = (Float) waddsat((int) val_p, (int) val_q);
	*p_res = val_res;

}											/* end vector_add */

#endif /* MMDSP */

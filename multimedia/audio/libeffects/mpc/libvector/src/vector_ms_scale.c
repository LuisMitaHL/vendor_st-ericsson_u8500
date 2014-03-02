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
vector_ms_scale(Float *m, Float *s, Float scale, int len)
{
	Float           acc1;
	Float           acc2;
	int             i;

	for (i = 0; i < len; i++) {
		acc1 = (*m) * scale;
		acc2 = (*s) * scale;
		*m++ = acc1 + acc2;
		*s++ = acc1 - acc2;
	}

}											/* end vector_ms_scale() */

#else

void
vector_ms_scale(Float *m, Float *s, Float scale, int len)
{
	WORD56          acc1;
	WORD56          acc2;
	register Float mval AT_REG(R0H);
	register Float sval AT_REG(R0L);
	register Float temp AT_REG(R1L);

	assert(len >= 0);
	assert(len < 1024);
#if 0
	m_n = m + 1;
	s_n = s + 1;

	mval = *m;
	sval = *s;

	acc1 = wX_fmul(mval, scale);

	acc2 = wX_fmul(sval, scale);
	mval_n = *m_n++;

	mval = waddr(acc1, acc2);
	sval_n = *s_n++;

	sval = wsubr(acc1, acc2);
	*m++ = mval;

	HWLOOP(len - 2) {
		acc1 = wX_fmul(mval_n, scale);
		*s++ = sval;

		acc2 = wX_fmul(sval_n, scale);
		mval_n = *m_n++;

		mval = waddr(acc1, acc2);
		sval_n = *s_n++;

		sval = wsubr(acc1, acc2);
		*m++ = mval;
	}

	/* epilogue : do not read outside buffer boundaries */
	acc1 = wX_fmul(mval_n, scale);
	*s++ = sval;

	acc2 = wX_fmul(sval_n, scale);
	mval = waddr(acc1, acc2);
	sval = wsubr(acc1, acc2);
	*m = mval;
	*s = sval;
#endif


	temp = scale;
	HWLOOP(len) {

		mval = *m;
		sval = *s;

		acc1 = wX_fmul(mval, temp);
		acc2 = wX_fmul(sval, temp);

		mval = waddr(acc1, acc2);
		sval = wsubr(acc1, acc2);

		*m++ = mval;
		*s++ = sval;

	} END_HWLOOP;


}											/* end vector_ms_scale() */

#endif /* MMDSP */


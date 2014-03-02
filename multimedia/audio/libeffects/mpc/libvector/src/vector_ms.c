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
static Float const EXTERN one_over_sqrt2 =
FORMAT_FLOAT(0.7071067811865, MAXVAL);

#ifndef MMDSP
void
vector_ms(Float *m, Float *s, int len)
{
	Float           acc1;
	Float           acc2;
	int             i;

	for (i = 0; i < len; i++) {
		acc1 = (*m) * one_over_sqrt2;
		acc2 = (*s) * one_over_sqrt2;
		*m++ = acc1 + acc2;
		*s++ = acc1 - acc2;
	}

}											/* end vector_ms() */
#else

void
vector_ms(Float *m, Float *s, int len)
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

	acc1 = wX_fmul(mval, one_over_sqrt2);

	acc2 = wX_fmul(sval, one_over_sqrt2);
	mval_n = *m_n++;

	mval = waddr(acc1, acc2);
	sval_n = *s_n++;

	sval = wsubr(acc1, acc2);
	*m++ = mval;

	HWLOOP(len - 2) {
		acc1 = wX_fmul(mval_n, one_over_sqrt2);
		*s++ = sval;

		acc2 = wX_fmul(sval_n, one_over_sqrt2);
		mval_n = *m_n++;

		mval = waddr(acc1, acc2);
		sval_n = *s_n++;

		sval = wsubr(acc1, acc2);
		*m++ = mval;
	}

	/* epilogue : do not read outside buffer boundaries */
	acc1 = wX_fmul(mval_n, one_over_sqrt2);
	*s++ = sval;

	acc2 = wX_fmul(sval_n, one_over_sqrt2);
	mval = waddr(acc1, acc2);
	sval = wsubr(acc1, acc2);
	*m = mval;
	*s = sval;
#endif


	temp = one_over_sqrt2;
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


}	/* end vector_ms() */

#endif /* MMDSP */


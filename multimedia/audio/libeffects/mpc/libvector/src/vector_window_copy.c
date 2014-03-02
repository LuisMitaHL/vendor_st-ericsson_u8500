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
vector_window_copy(Float *p, Float *q, Float *win, int len,
				   int incr1, int incr2, int incr3)
{
	int             i;

	for (i = 0; i < len; i++) {
		(*p) = (*q) * (*win);
		p += incr1;
		q += incr2;
		win += incr3;
	}
}		
									/* end vector_window_copy() */
#ifdef ARM
void
vector_window_copy_arm(int *p, int *q, int *win, int len,
				   int incr1, int incr2, int incr3)
{
	int             i;
	
	for (i = 0; i < len; i++) {
       *p  = (builtin_mpf_32((*q) ,(*win)));
		p += incr1;
		q += incr2;
		win += incr3;
	}
}
#endif
#else

void
vector_window_copy(Float *p, Float *q, Float WINDOW_MEM * win, int len,
				   int incr1, int incr2, int incr3)
{
	register int ix1 AT_REG(IX1);
	register int ix2 AT_REG(IX2);
	register int ix3 AT_REG(IX3);

	Float           val_q;
	Float           val_win;
	Float           val_p;

	assert(len > 0);
	assert(len <= 1024);
	
	ix1 = incr1;
	ix2 = incr2;
	ix3 = incr3;

	val_win = *win;
	val_q = *q;
	win += ix3;
	q += ix2;

	HWLOOP(len - 1) {
		val_p = wfmulr(val_q, val_win);
		val_win = *win;
		val_q = *q;
		win += ix3;
		q += ix2;

		*p = val_p;
		p += ix1;
	} END_HWLOOP;

	val_p = wfmulr(val_q, val_win);
	*p = val_p;

}											/* end vector_window_copy() */

#endif /* MMDSP */

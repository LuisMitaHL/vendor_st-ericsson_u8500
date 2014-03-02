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
vector_window(Float *p, Float WINDOW_MEM *win, int len, int incr1, int incr2)
{
	int             i;

	for (i = 0; i < len; i++) {
		(*p) *= (*win);
		p += incr1;
		win += incr2;
	}
}											/* end vector_window() */

#ifdef ARM
void
vector_window_arm(int *p, int WINDOW_MEM *win, int len, int incr1, int incr2)
{
	int             i;

	for (i = 0; i < len; i++) {		
		(*p) = builtin_mpf_32(*p,*win);
		p += incr1;
		win += incr2;
	}
}
#endif
#else

void
vector_window(Float *p, Float WINDOW_MEM * win, int len, int incr1, int incr2)
{
	register int ix1 AT_REG(IX1);
	register int ix2 AT_REG(IX2);

	Float           val_p;
	Float           val_win;
	Float           val_res;
	Float          *p_res;

	assert(len > 0);
	assert(len <= 1024);
	
	ix1 = incr1;
	ix2 = incr2;
	p_res = p;

	val_p = *p;
	val_win = *win;
	p += ix1;
	win += ix2;

	HWLOOP(len - 1) {
		val_res = (Float) wfmulr((int) val_p, (int) val_win);
		val_p = *p;
		p += ix1;
		val_win = *win;
		win += ix2;

		*p_res = val_res;
		p_res += ix1;
	} END_HWLOOP;

	val_res = (Float) wfmulr((int) val_p, (int) val_win);
	*p_res = val_res;

}											/* end vector_window() */

#endif /* MMDSP */

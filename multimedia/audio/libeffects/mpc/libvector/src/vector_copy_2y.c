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


/*void vector_copy_2y(Float YMEM *p, Float *q, int len, int incr1, int incr2)
{
	int i;

	for (i = 0; i < len; i++) {
		*p = (*q);
		p += incr1;
		q += incr2;
	}
}	*/										/* end vector_copy_2y */



#ifndef MMDSP

void
vector_copy_2y(Float YMEM*p, Float const *q, int len, int incr1, int incr2)
{
	int             i;

	for (i = 0; i < len; i++) {
		*p = (*q);
		p += incr1;
		q += incr2;
	}
}											/* end vector_copy */

#else

void
vector_copy_2y(Float YMEM*p, Float const *q, int len, int incr1, int incr2)
{
	register int ix1 AT_REG(IX1);
	register int ix2 AT_REG(IX2);

	ix1 = incr1;
	ix2 = incr2;

    while (len>1023) {
		HWLOOP(1023) {
            *p = (*q);
            q += ix2;
            p += ix1;
		} END_HWLOOP;
		len -= 1023;
	}

	HWLOOP(len) {
		*p = (*q);
		q += ix2;
		p += ix1;
	} END_HWLOOP;
}											/* end vector_copy */

#endif /* MMDSP */

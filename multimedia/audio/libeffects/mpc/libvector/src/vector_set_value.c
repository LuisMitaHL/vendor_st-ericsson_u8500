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
vector_set_value(Float *p, int len, Float value, int incr)
{
	int             i;

	for (i = 0; i < len; i++) {
		*p = value;
		p += incr;
	}
}		
/* end vector_set_value() */

#ifdef ARM 
void
vector_set_value_arm(int *p, int len, int value, int incr)
{
	int             i;

	for (i = 0; i < len; i++) {
		*p = value;		
		p += incr;
	}	
}		
#endif
								

#else

void
vector_set_value(Float *p, int len, Float value, int incr)
{
	register int ix AT_REG(IX1);
	int local_len;
	
	while (len > 0) {

		local_len = wmin(len, 1023);
		ix = incr;

		HWLOOP(local_len) {
			*p = value;
			p += ix;
		} END_HWLOOP;

		len -= local_len;
	}
}/* end vector_set_value() */


#endif /* MMDSP */

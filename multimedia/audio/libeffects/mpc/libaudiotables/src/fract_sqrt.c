/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "audiotables.h"

#ifdef MMDSP

static const SQRT_FRACT Float one_half = FORMAT_FLOAT(0.50000000f, MAXVAL);
static const SQRT_FRACT Float one_over_sqrt_2 = FORMAT_FLOAT(0.7071067690849304, MAXVAL);

dFloat
AudioTables_fract_sqrt(Float val)
{
	int exponent;
	int index;
	Float temp;
	dFloat res;

	if(val == (Float)0)
		return val;
	
	exponent = wedge(val);
	temp = wmsl(val,exponent); // val << exponent;
	temp = temp - one_half;
	index = wmsr(temp,13); //index = temp >> 13;

	temp = AudioTables_fract_sqrtTab[index];
	if(exponent & 1 )
	{
		res = wX_fmul(temp,AudioTables_fract_sqrtTab[0]);
	}
	else
	{
		res = wX_depmsp(temp);
	}
	res = ( res >> ( (exponent >>1 ) )  );
		
	return res ;
}

#endif /* MMDSP */

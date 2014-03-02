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

void vector_stereo_interleave_pan(Float *left, Float *scratch, int len, int nb_chan)
{
	/* assumption: right channel is contiguous to left and is located at &left[len] */
	if (nb_chan == 1)
	{
		/* copy left channel to scratch */
		vector_copy( scratch, left, len, 1, 1);

		/* pan by copying left channel to right */
		vector_copy( &left[len], left, len, 1, 1);
		
		/* de-interleave right channel */
		vector_copy( &left[1], &left[len], len, 2, 1);
		    
		/* de-interleave left channel */
		vector_copy( left, scratch, len, 2, 1);
	}
	else
	{
		/* copy left channel to scratch */
		vector_copy( scratch, left, len, 1, 1);
		
		/* de-interleave right channel */
		vector_copy( &left[1], &left[len], len, 2, 1);
		    
		/* de-interleave left channel */
		vector_copy( left, scratch, len, 2, 1);
	}
} /* end vector_stereo_interleave_pan() */

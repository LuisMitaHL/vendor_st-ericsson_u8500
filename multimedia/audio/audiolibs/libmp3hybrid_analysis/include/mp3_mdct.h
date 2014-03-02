/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _mp3_mdct_h_
#define _mp3_mdct_h_

#define MDCT_HALF      18
#define MDCT_HALF_SHORT 6

extern void mp3_mdct(Float *in0,Float *in1,int istride, int sbstride, int niter,
                     Float *out, int block_type,Float const MP3HYBRID_MEM *win);
extern void mp3_mdct_scaling(Float *in0, Float *in1, int istride, int sbstride, int niter,
                             int *sbScale, int common_scale,
                             Float *out, int block_type,Float const MP3HYBRID_MEM *win);

#endif /* Do not edit below this line */

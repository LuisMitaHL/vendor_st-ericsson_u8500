/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _local_imdct_h_
#define _local_imdct_h_

#define	npow2(x)		(1L << (x))	/* 2^x */

extern void
pre_twiddle(Float *Spectra, Float *pre_tw, int length, Float const MDCT_MEM *xcos_sin);

extern void
post_twiddle(Float *fft_out, Float *post_tw, int length, Float const MDCT_MEM *xcos_sin);

extern void
pre_twiddle_ramxy(Float *Spectra, Float *pre_re, Float YMEM *pre_im,
		  int length, Float const MDCT_MEM *xcos_sin);

extern void
post_twiddle_ramxy(Float *fft_re, Float YMEM *fft_im, Float *post_tw,
		   int length, Float const MDCT_MEM *xcos_sin);

#endif /* Do not edit below this line */


     

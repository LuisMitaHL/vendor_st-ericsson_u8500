/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _mdct_h_
#define _mdct_h_

#include "audiolibs_common.h"
#include "mdct_tables.h"

/* To enable the usage of mdct_formula() function */

#define BIT_REVERSE

#ifdef MDCT_FORMULA
extern CPPCALL void mdct_formula(Float samples[], Float out[], int ln, int *scale);
#endif

extern CPPCALL void mdct_ramxy(
    Float samples[], Float out[],
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
	int ln, int *scale);

extern CPPCALL void mdct_ramxy_scaled(
    Float samples[], Float out[],
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
    int ln, int *scale);

extern CPPCALL void mdct_ramxy_dyn_scale(
    Float samples[], Float out[],
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
    int ln, int *scale);
         
#ifdef MDCT_FORMULA
extern CPPCALL void mdct_formula_split(Float samples[], Float out[], int ln, int *scale);
#endif

extern CPPCALL void mdct_ramxy_split(
    Float samples[], Float out[],
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
    int ln, int *scale);

extern CPPCALL void mdct_ramxy_scaled_split(
    Float samples[], Float out[],
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
    int ln, int *scale);

extern CPPCALL void mdct_ramxy_dyn_scale_split(
    Float samples[], Float out[],
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
    int ln, int *scale);

#endif /* _mdct_h_ */

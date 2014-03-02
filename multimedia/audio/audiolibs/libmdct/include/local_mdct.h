/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _local_mdct_h_
#define _local_mdct_h_

#include "audiolibs_common.h"

#define npow2(x)     (1 << (x)) /* 2^x */

extern CPPCALL void
mdct_post_modulation(
    Float *out,
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
    int ln_2,
	Float const MDCT_MEM *pcos_sin
	);
         
extern CPPCALL void
mdct_pre_modulation(
    Float *samples,
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
    int ln_2,
	Float const MDCT_MEM *pcos_sin
	);
  	
   	
extern CPPCALL void
mdct_pre_modulation_scaled(
    Float *samples,
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
    int ln_2,
	Float const MDCT_MEM *pcos_sin
	);


extern CPPCALL void
mdct_pre_modulation_dyn_scaled(
    Float *samples,
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
    int ln_2,
	Float const MDCT_MEM *pcos_sin);          

extern CPPCALL void
mdct_pre_modulation_split(
    Float *sample_first, Float *sample_second,
    Float *fft_buff_real, Float YMEM * fft_buff_imag,
    int ln_2,
	Float const MDCT_MEM *pcos_sin
	);
	

extern CPPCALL void
mdct_pre_modulation_scaled_split(
    Float *sample_first, Float *sample_second,
    Float *fft_buff_real, Float YMEM * fft_buff_imag, 
    int ln_2,
	Float const MDCT_MEM *pcos_sin
	);


extern CPPCALL void
mdct_pre_modulation_dyn_scaled_split(
    Float *sample_first, Float *sample_second,
    Float *fft_buff_real, Float YMEM * fft_buff_imag, 
    int ln_2,
	Float const MDCT_MEM *pcos_sin);


#endif /* _local_mdct_h_ */

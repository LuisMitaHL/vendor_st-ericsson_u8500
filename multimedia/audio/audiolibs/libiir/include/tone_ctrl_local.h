/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _tone_ctrl_local_h_
#define _tone_ctrl_local_h_

#include "audiolibs_common.h"

#define MAX_CHANNELS 2
#define TONE_CTRL_RIGHT_SHIFT 3
#define TONE_CTRL_MAX_GAIN    8.0

#ifndef PI
#define PI 3.1415926536
#endif /* PI */

extern int us_frac_div( int n, int d );

extern int frac_div( int n, int d );

extern Float get_gain_from_dB(int gain);

extern float get_gain_from_dB_float(int gain);

extern void 
tone_compute_coefs_float(float *coef,int fs, int fn, float gain, int lowpass );


extern void 
tone_compute_coefs(Float *coef,int fs, int fn, Float gain, int lowpass );

extern void
tone_filter(Float *buffer,int len,Float *coef,Float *context);

#endif /* Do not edit below this point */

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _fft_h_
#define _fft_h_

#include "audiolibs_common.h"
#include "vector.h"
#include "lib_macros.h"

#ifdef __flexcc2__ 
#define FFT_MEM  YMEM
#else
#define FFT_MEM 
#endif


#define FFT_512_PT_SUPPORT
//#define FFT_1024_PT_SUPPORT
//#define FFT_2048_PT_SUPPORT

#if defined(FFT_512_PT_SUPPORT)
#define  MaxN     2048
#elif defined(FFT_1024_PT_SUPPORT)
#define  MaxN     4096
#elif defined(FFT_2048_PT_SUPPORT)
#define  MaxN     8192
#else
#error "FFT size not supported"
#endif

/* Define window sizes for filter bank */
#define  MaxN_x2  (MaxN * 2)
#define  MaxN_x4  (MaxN * 4)
#define  MaxN_d2  (MaxN/2)
#define  MaxN_d4  (MaxN/4)

#define  MAX_FFTSIZE      MaxN_d4
#define  MAX_FFTSIZE_d2   MAX_FFTSIZE/2
#define  MAX_FFTSIZE_d4   MAX_FFTSIZE/4

#define  FFT_INTERLEAVED_TABLE_LENGTH     (4 * MaxN/16+2)   /* 514 */
#define  FFT_TABLE_LENGTH                 (3 * MaxN / 16)   /* 384 */

/* standard fft */
/* WARNING: This version requires indata_ptr to be aligned on a power
   of two. All other versions of the fft use relative bit-reverse */
extern void
fft(Float *indata_ptr, int order);

/* dual access fft, input is linear, output is bit-reversed */
extern void
fft_ramxy(Float * Re,           /* Real part is in X      */
	  Float YMEM * Img,     /* Imaginary part is in Y */
	  int ln);              /* order                  */

/* dual access fft, input is real linear vector, output is bit-reversed */
extern void
real_fft_ramxy(Float * Re,           /* Real part is in X      */
	  Float YMEM * Img,     /* Imaginary part is in Y */
	  int ln);              /* order                  */




/* dual access fft, input is linear, output is bit-reversed, scaling
   by one performed at each bufferly */
extern void
fft_ramxy_scaled(Float * Re,           /* Real part is in X      */
		 Float YMEM * Img,     /* Imaginary part is in Y */
		 int ln);              /* order                  */

/* dual access fft, input is linear, output is bit-reversed, scaling
   by two performed at each bufferly */
extern void
fft_ramxy_scaled_2(Float * Re,           /* Real part is in X      */
		 Float YMEM * Img,     /* Imaginary part is in Y */
		 int ln);              /* order                  */

/* dual access fft, input is bit-reversed, output is linear, scaling
   by one performed at each pass of the bufferly */
extern void
fft_ramxy_scaled_input_br(Float * Re,           /* Real part is in X      */
			  Float YMEM * Img,     /* Imaginary part is in Y */
			  int ln);              /* order                  */

/* dual access fft, input is linear, output is bit-reversed. Scaling
 * is performed at each pass of the fft if needed */
extern int
fft_ramxy_dyn_scale(Float *Re,		    /* Real part is in X      */
					Float YMEM * Img,	/* Imaginary part is in Y */
					int ln);             /* order                  */

/* dual access fft, input is bit-reversed, output is linear. Scaling
 * is performed at each pass of the fft if needed */
extern int
fft_ramxy_dyn_scale_input_br(Float *Re,		    /* Real part is in X      */
							 Float YMEM * Img,	/* Imaginary part is in Y */
							 int ln);             /* order                  */

/* dual access fft, input is bit-reversed, output is linear */
extern void
fft_ramxy_input_br(Float * Re,           /* Real part is in X      */
		   Float YMEM * Img,     /* Imaginary part is in Y */
		   int ln);              /* order                  */

/* reorder samples in bit-reverse addressing */
extern void
buffer_bit_reverse(Float *Re,Float *Img,int ln);

extern void
buffer_bit_reverse_ramxy(Float *Re,Float YMEM *Img,int ln);

/*

Interface for FFT DSP Library:
This is Split Radix implementation of FFT. for varios input points: [64, 512]

outData: o/p   : pointer to output FFT data
inpData: i/p   : pointer to input data, 2 bit headroom required , 
		         this buffer is shared inside
nPoints: i/p   : FFT size
nLog2N : i/p   : log2(nPoints)
shift  : i/p   : any shift to the FFT output

*/
#if(defined ARM)&&(defined NEON)
extern void libFFT(int *outData, int *inpData, long nPoints, long nLog2N, long shift );
#else
extern void libFFT(Float *outData, Float *inpData, long nPoints, long nLog2N, long shift );
#endif

#define FFT_LGTH 2048

typedef struct
{
    short dp_vr [FFT_LGTH / 2];
    short dp_vi [FFT_LGTH / 2];
    short sum_r [FFT_LGTH / 4 - 1];
    short sum_i [FFT_LGTH / 4 - 1];
    short diff_r[FFT_LGTH / 4 - 1];
    short diff_i[FFT_LGTH / 4 - 1];
} t_fft_struct;

extern void fft16(short        *p_vect_in,
                  t_fft_struct *p_fft_struct,
                  short        *p_x_out_r,
                  short        *p_x_out_i,
                  int          length);


#endif // _fft_h_ 

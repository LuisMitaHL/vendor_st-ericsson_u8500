
#ifndef _DSPLIB_FILTERBANK_H_
#define _DSPLIB_FILTERBANK_H_

#include "lib_macros.h"
#include "lib_filterbank_api.h"

#define MAX_NUM_QMF_BANDS_DSPLIB    128

#ifdef  BUILD_FIXED
#define FORMAT_FLOAT_QMF(a,b)  (Float)((a) * (b))
#else
#define FORMAT_FLOAT_QMF(a,b)  (Float)(a)
#endif

#define WORD_SIZE   32
#define RSHIFT_DCT3 6
#define RSHIFT_DCT2 2

#define RSHIFT_DCTIN  6

#define MULT_FLOAT(a,b)  (float)((a) * (b))



#ifdef  BUILD_FIXED

#define MULT_INT_FDCT2(a,b)    (Float)((( (dFloat)(a) * (dFloat)(b)) << 1) >> (WORD_SIZE - RSHIFT_DCT2))
#define MULT_INT_FDCT3(a,b)    (Float)((( (dFloat)(a) * (dFloat)(b)) << 1) >> (WORD_SIZE - RSHIFT_DCT3))
#define MULT_INT_64PT_FDCT3(a,b)    (Float)((( (dFloat)(a) * (dFloat)(b)) << 1) >> (WORD_SIZE - RSHIFT_DCT3))
#define MULT_INT_32PT_FDCT3(a,b)    (Float)((( (dFloat)(a) * (dFloat)(b)) << 1) >> (WORD_SIZE - (RSHIFT_DCT3-1)))
#define MULT_INT_16PT_FDCT3(a,b)    (Float)((( (dFloat)(a) * (dFloat)(b)) << 1) >> (WORD_SIZE - (RSHIFT_DCT3-2)))
#define MULT_INT_8PT_FDCT3(a,b)    (Float)((( (dFloat)(a) * (dFloat)(b)) << 1) >> (WORD_SIZE - (RSHIFT_DCT3-3)))
#define MULT_INT_4PT_FDCT3(a,b)    (Float)((( (dFloat)(a) * (dFloat)(b)) << 1) >> (WORD_SIZE - (RSHIFT_DCT3-4)))

#else

#define MULT_INT_FDCT2(a,b)         MULT_FLOAT(a,b)
#define MULT_INT_FDCT3(a,b)         MULT_FLOAT(a,b)
#define MULT_INT_64PT_FDCT3(a,b)    MULT_FLOAT(a,b)
#define MULT_INT_32PT_FDCT3(a,b)    MULT_FLOAT(a,b)
#define MULT_INT_16PT_FDCT3(a,b)    MULT_FLOAT(a,b)
#define MULT_INT_8PT_FDCT3(a,b)     MULT_FLOAT(a,b)
#define MULT_INT_4PT_FDCT3(a,b)     MULT_FLOAT(a,b)

#endif

#endif

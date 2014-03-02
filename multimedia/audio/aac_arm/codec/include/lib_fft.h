

#ifndef _DSPLIB_FFT_H_
#define _DSPLIB_FFT_H_

#include "lib_macros.h"
#define FFT_FORWARD   0
#define FFT_INVERSE   1

#define PI          (double)3.1415926535897932384626433832795

#ifdef BUILD_FIXED
#define mul32x32(a, b)    MUL_INT(a,b)
#define Rshift(a,b)      ((a) >> (b))
#define Lshift(a,b)      ((a) << (b))
#else
#define mul32x32(a, b)   ((MUL_INT(a,b)) / (float)(2.0))
#define Rshift(a,b)      ((a) / (1<<b))
#define Lshift(a,b)      ((a) * (1<<b))
#endif

#define FORMAT_FLOAT_FFT(a,b)  (Float)((a) * (b))
#define FORMAT_FLOAT_FFT_NEON(a,b)  (int)((a) * (b))

#endif //__LEE_MACROS__

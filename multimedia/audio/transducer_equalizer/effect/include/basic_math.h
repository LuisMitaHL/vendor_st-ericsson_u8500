/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   basic_math.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _BASIC_MATH_H_
#define _BASIC_MATH_H_

#ifdef __cplusplus
extern "C"
{
#endif

// Flag to choose between 64 bits floating point (double) or 32 bits floating point (float)
#define FLOAT64

// Flag to choose between internal mathematics functions approximations (such as cos, sqrt, log, exp, etc...)
// or standard library mathematics functions
//#define USE_MY_MATH


#ifdef FLOAT64
typedef double t_float;
#else // FLOAT64
typedef float  t_float;
#endif // FLOAT64

#define PI    3.1415926535897932384626433832795
#define LOG_2 0.69314718055994530941723212145818


#ifdef USE_MY_MATH

#   define Floor(x) my_floor((t_float) (x))
#   define Ceil(x)  my_ceil((t_float) (x))
#   define Fabs(x)  my_fabs((t_float) (x))
#   define Sqrt(x)  my_sqrt((t_float) (x))
#   define Log(x)   my_log((t_float) (x))
#   define Log2(x)  my_log2((t_float) (x))
#   define Log10(x) my_log10((t_float) (x))
#   define Exp(x)   my_exp((t_float) (x))
#   define Pow(x,y) my_pow((t_float) (x), (t_float) (y))
#   define Pow2(x)  my_pow2((t_float) (x))
#   define Pow10(x) my_pow10((t_float) (x))
#   define Cos(x)   my_cos((t_float) (x))
#   define Sin(x)   my_sin((t_float) (x))
#   define Cosh(x)  my_cosh((t_float) (x))
#   define Sinh(x)  my_sinh((t_float) (x))

    extern t_float my_floor(t_float x);
    extern t_float my_ceil(t_float x);
    extern t_float my_fabs(t_float x);
    extern t_float my_sqrt(t_float x);
    extern t_float my_log(t_float x);
    extern t_float my_log2(t_float x);
    extern t_float my_log10(t_float x);
    extern t_float my_exp(t_float x);
    extern t_float my_pow2(t_float x);
    extern t_float my_pow10(t_float x);
    extern t_float my_pow(t_float x, t_float y);
    extern t_float my_cos(t_float x);
    extern t_float my_sin(t_float x);
    extern t_float my_cosh(t_float x);
    extern t_float my_sinh(t_float x);

#else // USE_MY_MATH

#   include <math.h>

#   ifdef FLOAT64
#       define Floor(x) floor((t_float) (x))
#       define Ceil(x)  ceil((t_float) (x))
#       define Fabs(x)  fabs((t_float) (x))
#       define Sqrt(x)  sqrt((t_float) (x))
#       define Log(x)   log((t_float) (x))
#       define Log2(x)  (log((t_float) (x)) / LOG_2)
#       define Log10(x) log10((t_float) (x))
#       define Exp(x)   exp((t_float) (x))
#       define Pow(x,y) pow((t_float) (x), (t_float) (y))
#       define Pow2(x)  pow(2.0, (t_float) (x))
#       define Pow10(x) pow(10.0, (t_float) (x))
#       define Cos(x)   cos((t_float) (x))
#       define Sin(x)   sin((t_float) (x))
#       define Cosh(x)  cosh((t_float) (x))
#       define Sinh(x)  sinh((t_float) (x))
#   else // FLOAT64
#       define Floor(x) floorf((t_float) (x))
#       define Ceil(x)  ceilf((t_float) (x))
#       define Fabs(x)  fabsf((t_float) (x))
#       define Sqrt(x)  sqrtf((t_float) (x))
#       define Log(x)   logf((t_float) (x))
#       define Log2(x)  (logf((t_float) (x)) / LOG_2)
#       define Log10(x) log10f((t_float) (x))
#       define Exp(x)   expf((t_float) (x))
#       define Pow(x,y) powf((t_float) (x), (t_float) (y))
#       define Pow2(x)  powf(2.0, (t_float) (x))
#       define Pow10(x) powf(10.0, (t_float) (x))
#       define Cos(x)   cosf((t_float) (x))
#       define Sin(x)   sinf((t_float) (x))
#       define Cosh(x)  coshf((t_float) (x))
#       define Sinh(x)  sinhf((t_float) (x))
#   endif // FLOAT64

#endif // USE_MY_MATH

#ifdef __cplusplus
}
#endif

#endif // _BASIC_MATH_H_

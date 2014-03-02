/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*!
	CVS Id    $Id$

	\file	  nmxMathdefs.h
 
	\author   Luigi Arnone luigi.arnone@gmail.com

*/

#ifndef H_nmxMathdefs
#define H_nmxMathdefs 1

#include <stdint.h>
/* #include <values.h> */
#include <limits.h>
#include <float.h>
#include <math.h>

#ifdef _MSC_VER /* Microsoft C compiler ? */
#define isnan(x) _isnan(x)
#endif

/*
	Limits maximum and minimum of integer type in stdint.h E.g.:
		INT8_MIN, UINT64_MAX,
	for other see stdint.h
	For double and float use:
		DBL_MAX, FLT_MAX, DBL_MIN and FLT_MIN
	remember that DBL_MIN is the minimun double > 0
*/

/*
	Useful constants from <math.h> like M_[something]; e.e.:
		M_E for e, M_PI for 3.14..., M_SQRT2 for sqrt(2), ...
	The following definitions sometimes do not appear under <math.h>.
	For this reason, they are repeated here. The original definitions
	were copied from GNU-C's <math.h>.
*/
#ifndef M_LOG2E
# define M_E		2.7182818284590452354	/* e */
# define M_LOG2E	1.4426950408889634074	/* log_2 e */
# define M_LOG10E	0.43429448190325182765	/* log_10 e */
# define M_LN2		0.69314718055994530942	/* log_e 2 */
# define M_LN10		2.30258509299404568402	/* log_e 10 */
# define M_PI		3.14159265358979323846	/* pi */
# define M_PI_2		1.57079632679489661923	/* pi/2 */
# define M_PI_4		0.78539816339744830962	/* pi/4 */
# define M_1_PI		0.31830988618379067154	/* 1/pi */
# define M_2_PI		0.63661977236758134308	/* 2/pi */
# define M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
# define M_SQRT2	1.41421356237309504880	/* sqrt(2) */
# define M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */
#endif

#ifndef INFINITY

# ifdef _MSC_VER

__inline float nmxInternalInfinity() { 
  int value = 0x7f800000; 
  return *(float*)&value; 
} 

#  define INFINITY (nmxInternalInfinity())

# else

#  define INFINITY (1.0f/0.0f)

# endif

#endif

#define NMX_ABS(x)		(((x) >= 0) ? (x) : -(x))
#define NMX_ABS_DIFF(a,b)	((a)>(b)?(a)-(b):(b)-(a))
#define NMX_INTERP(a,b,k)	((a) + ((b)-(a))*(k))
#define NMX_INTERP_U(a,b,k)	((b)*(k) + (a)*(1.0-(k)))
#define NMX_MAX(a,b)		(((a)>(b))?(a):(b))
#define NMX_MAX_ABS(a,b)	NMX_MAX(NMX_ABS(a),NMX_ABS(b))
#define NMX_MIN(a,b)		(((a)<(b))?(a):(b))
#define NMX_MIN_ABS(a,b)	NMX_MIN(NMX_ABS(a),NMX_ABS(b))
#define NMX_NELEM(x)		(sizeof(x)/sizeof(x[0]))
#define NMX_NULL(type)		((type)0)
#define NMX_ROUND(x)		((x)>=0 ? (int32_t)((x)+0.5) : (int32_t)((x)-0.5))
#define NMX_ROUND_64(x)		((x)>=0 ? (int64_t)((x)+0.5) : (int64_t)((x)-0.5))
#define NMX_SIGN(a,b)		((b) >= 0.0 ? fabs(a) : -fabs(a))
#define NMX_SQR(a)		((a) * (a))
#define NMX_SUM_ABS(a,b)	(NMX_ABS(a)+NMX_ABS(b))
#define NMX_SWAP(a,b,t)		{(t) = (a); (a)=(b); (b)=(t);}

#endif /* H_nmxMathdefs */


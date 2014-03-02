/*****************************************************************************
 *                            C STMicroelectronics
 *    Reproduction and Communication of this document is strictly prohibited
 *      unless specifically authorized in writing by STMicroelectronics.
 *----------------------------------------------------------------------------
 * 	                             HPC/PMG/AP3
 *****************************************************************************/
 /**
 * \file 	stab_common.h
 * \brief 	common declarations, macros for stab
 * \author 	Jean-Marc VOLLE
 * \date 	Fri Oct  5 17:46:27 2007
 *
 *
 **/
/*****************************************************************************/

#ifndef _STAB_COMMON_
#define _STAB_COMMON_

#ifdef STANDALONE

/* -------------------------------- */
/* GCC/DSP common types, macros,... */
/* -------------------------------- */

typedef  unsigned char      t_uint8; /* 8-bit data types*/
typedef  signed char        t_sint8;

typedef  unsigned short     t_uint16; /* 16-bit container on 32-bits architectures */
typedef  signed short       t_sint16;
typedef  signed short       t_bool;

typedef  unsigned long int  t_uint32; /* 32-bit container on 32-bits architectures */
typedef  signed long int    t_sint32;

typedef unsigned char	      byte;

#ifndef NULL
#  define NULL    0
#endif

#define ON      1
#define OFF     0

#define TRUE    1
#define FALSE   0

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#else /* STANDALONE */
#include <cm/inc/cm_type.h>
#endif /* STANDALONE */

typedef  float              t_f32; /* 32 bit ("float"  C type) floating point registers */
typedef  double             f64_t; /* 64 bit ("double" C type) floating point registers */

#if defined(ENABLE_ASSERT)
#define ASSERT(x) assert(value)
#else
#define ASSERT(x) 
#endif



#if defined(__GNUC__)
/* ---------------------------------------- */
/* GCC specific defines, types, macros,.... */
/* ---------------------------------------- */

/* Inlining keyword: */
#define INLINE __inline__


/* Standard C macros */
/* ----------------- */
#define ABS(a) ((a)>0?(a):-(a))
//#define SIGN(a)((a)>0?1:-1)
#define SIGN(a) ((a)==0?0:(a)>0?1:-1)
#define SIGN_FOR_STAB(a) ( ((a) >= 0) ? 0 : 1 )

#define ROUNDED_FP_TO_INT(a) ((t_sint32)(((a) < 0) ? ((a) - ((float) 0.5)) : ((a) + ((float) 0.5))))
#define CLIP(value,min,max)   MIN(MAX((min),(value)),(max))
#define CLIP_0_255(value) ((value)<0?0:((value)>255?255:(value)))
#define CLIP_0_N(value,n) CLIP((value),0,(n))
#define HI_BYTE(value) ((t_uint8)((value) >> 8))
#define LO_BYTE(value) ((t_uint8)((value) & 0xff))
#define BIT(value,pos)       (((value)>>(pos))&0x1)
#define MERGE_BYTE(msb,lsb) (((msb)<<8)|(lsb))
#define RESTRICT
#define XYCURVES_LOW_S16_CLIP(a) (((a) > 32767) ? 32767 : (((a) < (-32768)) ? (-32768) : (a)))

#define ASSERT_HW_LOOP(value) ASSERT((value)<1024);


#elif defined (__arm__)
/*TODO create other entries according to ARM architecture used */
/* ---------------------------------------- */
/* ARM specific defines, types, macros,.... */
/* ---------------------------------------- */
#define ABS(a) ((a)>0?(a):-(a))
//#define SIGN(a)((a)>0?1:-1)
#define SIGN(a) ((a)==0?0:(a)>0?1:-1)
#define SIGN_FOR_STAB(a) ( ((a) >= 0) ? 0 : 1 )

#define ROUNDED_FP_TO_INT(a) ((t_sint32)(((a) < 0) ? ((a) - ((float) 0.5)) : ((a) + ((float) 0.5))))
#define CLIP(value,min,max)   MIN(MAX((min),(value)),(max))
#define CLIP_0_255(value) ((value)<0?0:((value)>255?255:(value)))
#define CLIP_0_N(value,n) CLIP((value),0,(n))
#define HI_BYTE(value) ((t_uint8)((value) >> 8))
#define LO_BYTE(value) ((t_uint8)((value) & 0xff))
#define BIT(value,pos)       (((value)>>(pos))&0x1)
#define MERGE_BYTE(msb,lsb) (((msb)<<8)|(lsb))
#define RESTRICT
#define XYCURVES_LOW_S16_CLIP(a) (((a) > 32767) ? 32767 : (((a) < (-32768)) ? (-32768) : (a)))

#define ASSERT_HW_LOOP(value) ASSERT((value)<1024);



#else
#error  "GCC or ARM target should be defined"
#endif

/* -------------- */
/* Maximum sizes  */
/* -------------- */


/*------------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/
extern t_bool Dbg;


#endif  /* _STAB_COMMON_ */

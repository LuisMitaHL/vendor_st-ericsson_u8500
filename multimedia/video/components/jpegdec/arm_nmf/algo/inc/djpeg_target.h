/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_target.h
 * \brief 	 target specific defines to be able to compile under GCC 
 * \author  ST-Ericsson
 *    
 */
/*****************************************************************************/


#ifdef __NMF

//#include <stwdsp.h>

#endif

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

//#ifndef _BASE_TYPE_
//#define _BASE_TYPE_

/** @{ \name base types definition */
/*---------------------------------*/

/* typedef  unsigned char       t_uint8; /\**< \brief unsigned 8 bits type   *\/ */
/* typedef    signed char       t_sint8; /\**< \brief signed 8 bits type   *\/ */

/* typedef  unsigned short     t_uint16; /\**< \brief unsigned 16 bits type   *\/ */
/* typedef    signed short     t_sint16; /\**< \brief signed 16 bits type   *\/ */

/* typedef  unsigned long int  t_uint32; /\**< \brief unsigned 32 bits type   *\/ */
/* typedef    signed long int  t_sint32; /\**< \brief signed 32 bits type   *\/ */
/* typedef  t_sint16              t_bool; /\**< \brief boolean type   *\/ */

/* typedef t_uint32            t_address; /\**< \brief used to exchange addresses between hamac and host   *\/ */
/*  typedef t_uint32            t_packed_short;/\*brief used on dsp to identify data used for packed arithmetic  *\/ */

#ifndef TRUE 
#define TRUE 1
#endif 

#ifndef FALSE
#define FALSE 0
#endif 
/** @}*/

//#endif /* _BASE_TYPE_ */



/*------------------------------------------------------------------------
 * Macros                                                              
 *----------------------------------------------------------------------*/


/** \brief macro to clip a value between min and max*/
#define CLIP(value,min,max)   ((value)<(min)?(min):((value)>(max)?max:(value)))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define ABS(a) ((a)>0?(a):-(a))
#define SIGN(a)((a)>0?1:-1)
#define CLIP_0_255(value) ((value)<0?0:((value)>255?255:(value)))
#define CLIP_0_N(value,n) CLIP((value),0,(n))
#define HI_BYTE(value) ((t_uint8)((value) >> 8))
#define LO_BYTE(value) ((t_uint8)((value) & 0xff))



 /* _DJPEG_TARGET_H_ */

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_TARGET_H_
#define _VC1D_TARGET_H_

/* GCC keywords */
/* ------------ */

#define INLINE

/* Under GCC, we use only one kind of memory */
#define const 
#define XMEM
#define EXTMEM
#define YMEM	
#define ESRAM
#define CIRCULAR_BUFFER


#ifndef ZZ_MEM
#define ZZ_MEM YMEM  // zigzag tables are placed in YMEM
#endif

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/
#if !defined(FALSE)
#define FALSE 0
#endif

#if !defined(TRUE)
#define TRUE 1
#endif


/*------------------------------------------------------------------------
 * Macros                                                              
 *----------------------------------------------------------------------*/
#ifndef _COMMON_MACROS_H_


/** \brief macro to clip a value between min and max*/

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define ABS(a) ((a)>0?(a):-(a))
//#define SIGN(a)((a)>0?1:-1)
#define SIGN(a) ((a)==0?0:(a)>0?1:-1)
#define CLIP(value,min,max)   MIN(MAX((min),(value)),(max))
#define CLIP_0_255(value) ((value)<0?0:((value)>255?255:(value)))
#define CLIP_0_N(value,n) CLIP((value),0,(n))
#define HI_BYTE(value) ((t_uint8)((value) >> 8))
#define LO_BYTE(value) ((t_uint8)((value) & 0xff))
#define BIT(value,pos)       (((value)>>(pos))&0x1)
#define MERGE_BYTE(msb,lsb) (((msb)<<8)|(lsb))
#define RESTRICT 
#define VLC_LUT_SIZE(lut_val) ((lut_val)&0xf)
#define VLC_LUT_VAL(lut_val) ((lut_val)>>4)

#define ASSERT_HW_LOOP(value) ASSERT((value)<1024);

#endif // _COMMON_MACROS_H_
#endif /* _VC1D_TARGET_H_ */

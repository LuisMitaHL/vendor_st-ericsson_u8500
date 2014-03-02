/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _DSPLIB_MACROS_H_
#define _DSPLIB_MACROS_H_
#include "audiolibs_common.h"


    /* Macro Defines :  Arithmetic Operations */
    
#ifdef  BUILD_FIXED  /* To Build  the Float Code */

	#define MAX32    (Float)(0X7FFFFFFF)

#ifdef _dsp_specific_
    /* DSP Specifc Intrinsics of below given MACROS */
	//ADD_WORD(a,b)  arm_add(a,b)   /*this is merely an example*/
#else /* to build GCC code */

	#define SHIFT_RIGHT(a,n)      ((a) >> (n))
	#define SHIFT_LEFT(a,n)       ((a) << (n))

	/* Fixed Point Arithmetic */
	#define ADD_WORD(a,b)            (Float) ((dFloat)(a) + (dFloat)(b))
	#define SUB_WORD(a,b)            (Float) ((dFloat)(a) - (dFloat)(b))
	#define MUL_WORD(a,b)            (Float)(((dFloat)(a) * (dFloat)(b)) >> 31)
	#define MUL_INT(a,b)             (Float)(((dFloat)(a) * (dFloat)(b)) >> 32)
	#define DIV_WORD(a,b)                       /* Division Code */
	#define MAC_WORD(c, a, b)        (Float)(ADD_WORD(c, ((MUL_WORD(a, b)))))
	#define MSU_WORD(c, a, b)        (Float)(SUB_WORD(c, ((MUL_WORD(a, b)))))

	/* Add here IntrinsicsModels.h File for intrinsics not expressed through above macros */
	/* IntrinsicsModels.h File requires IntrinsicsModels.c File for C models of DSP specific Intrinsic */
	/* #include "IntrinsicsModels.h" */

#endif

#else   /* To Build  the Integer Code */

	#define MAX32    (Float)(1.0)

#ifdef _dsp_specific_
    /* DSP Specifc Intrinsics of below given MACROS */
    //ADD_WORD(a,b)  arm_add(a,b)   /*this is merely an example*/
#else /* to build GCC code */

	#define SHIFT_RIGHT(a,n)      ((a) >> (n))
	#define SHIFT_LEFT(a,n)       ((a) << (n))

	/* Floating Point Arithmetic */
	#define ADD_WORD(a,b)            (Float)((a) + (b))
	#define SUB_WORD(a,b)            (Float)((a) - (b))
	#define MUL_WORD(a,b)            (Float)((a) * (b))
	#define MUL_INT(a,b)             (Float)((a) * (b))
	#define DIV_WORD(a,b)            (Float)((a) / (b))
	#define MAC_WORD(c, a, b)        (Float)(ADD_WORD(c, ((MUL_WORD(a, b)))))
	#define MSU_WORD(c, a, b)        (Float)(SUB_WORD(c, ((MUL_WORD(a, b)))))

	/* Add here IntrinsicsModels.h File for intrinsics not expressed through above macros */
	/* IntrinsicsModels.h File requires IntrinsicsModels.c File for C models of DSP specific Intrinsic */
	/* #include "IntrinsicsModels.h" */

#endif

#endif

#endif

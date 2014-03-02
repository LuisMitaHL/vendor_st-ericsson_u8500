/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 *
 * This source module contains some general-purpose macros (such as
 * computation of absolute value, minimum and maximum) and specialized
 * macros (1D DCT and IDCT, 1D Hadamard, etc.)
 */

#ifndef MACROS_H
#define MACROS_H

/**
 * \brief Define a value for PSKIP macroblock type
 */
 
#define PSKIP 40

/**
 * \brief Define some max and min value
 */
 
#define MAX_UINT_16     65535
#define MAX_SINT_32     2147483647
#define MIN_SINT_32    -MAX_SINT_32-1
#define MAX_UINT_32     4294967295

/**
 * \brief Extract pixel stored in the LSB of a 16 bit word
 */
 
#define RIGHT_PEL(x)	((x)&0xFF)

/**
 * \brief Extract pixel stored in the MSB of a 16 bit word
 */
 
#define LEFT_PEL(x)	((x)>>8)

/**
 * \brief Clips a value to make it lie in the range [low,high].
 */

#define CLIP(x,low,high)    (((x)>(high)) ? (high) : ((x)<(low)) ? (low) : (x))


/**
 * \brief Finds the maximum value between a and b.
 */

#ifndef MAX
#define MAX(a,b)    (((a)>(b)) ? (a) : (b))
#endif

/**
 * \brief Finds the minimum value between a and b.
 */
#ifndef MIN
#define MIN(a,b)    (((a)<(b)) ? (a) : (b))
#endif

/**
 * \brief Finds the median value between a, b and c.
 */
 
#define MEDIAN(a,b,c,m)                     \
{									        \
	t_sint16 x1 = (a), x2 = (b), x3 = (c);	\
	t_sint16 tmp = x1;						\
	if (x2<x1) {x1=x2; x2=tmp;}			    \
	tmp = x2;							    \
	if (x3<x2) {x2=x3; x3=tmp;}			    \
	tmp = x1;							    \
	if (x2<x1) {x1=x2; x2=tmp;}			    \
	(m) = x2;							    \
}


/**
 * \brief Swaps the bytes in a 16-bit word.
 */

#define SWAP16(x)   (((x)>>8) | ((x)<<8))


/**
 * \brief Swaps the bytes in a 32-bit word
 */

#define SWAP32(N)   { N = (((N)>>24) | (((N)>>8)&0xff00) | (((N)&0xff00)<<8) | ((N)<<24)); }


/**
 * \brief Finds absolute value of x
 */

#define ABS(x)          ((x) >= 0 ? (x) : -(x))



/**
 * \brief 1D IDCT for one row of 4 pixels
 * \param p_b Pointer to the row of pixels
 * \author Filippo Santinello
 * \attention This macro is only used within ANSI C code.
 */

#define ANSI_ROW_IDCT(p_b)                \
{                                       \
    t_sint16 t[4];                        \
    *(t) = ((*(p_b) + *(p_b+2)));           \
    *(t+1) = ((*(p_b) - *(p_b+2)));           \
    *(t+2) = (((*(p_b+1)>>1) - *(p_b+3)));      \
    *(t+3) = ((*(p_b+1) + (*(p_b+3)>>1)));      \
    *(p_b) = ((*(t) + *(t+3)));           \
    *(p_b+1) = ((*(t+1) + *(t+2)));           \
    *(p_b+2) = ((*(t+1) - *(t+2)));           \
    *(p_b+3) = ((*(t) - *(t+3)));           \
}



/**
 * \brief 1D IDCT for one col of 4 pixels
 * \param p_b Pointer to the col of pixels
 * \author Denis Zinato
 * \attention This macro is only used within ANSI C code.
 */
 
#define ANSI_COL_IDCT(p_b)                \
{                                       \
    t_sint16 t[4];                        \
    *(t) = ((*(p_b) + *(p_b+8)));           \
    *(t+1) = ((*(p_b) - *(p_b+8)));           \
    *(t+2) = (((*(p_b+4)>>1) - *(p_b+12)));      \
    *(t+3) = ((*(p_b+4) + (*(p_b+12)>>1)));      \
    *(p_b) = ((((*(t) + *(t+3)))+(1<<5))>>6);           \
    *(p_b+4) = ((((*(t+1) + *(t+2)))+(1<<5))>>6);           \
    *(p_b+8) = ((((*(t+1) - *(t+2)))+(1<<5))>>6);           \
    *(p_b+12) = ((((*(t) - *(t+3)))+(1<<5))>>6);           \
}


/**
 * \brief Post-scaling and saturation after IDCT on a 4x4 block
 * \param p_b Pointer to the 4x4 block data
 * \author Filippo Santinello
 * \attention This macro is only used within ANSI C code.
 */

#define ANSI_IDCT_POST_SCALE(p_b)         	\
{                                      	 	\
    t_sint16 i;                           	\
    for (i=0; i<16; i++) {              	\
        *(p_b+i) = *(p_b+i);        \
    }                                   	\
}



/**
 * \brief 2D IDCT for one 4x4 residual block
 * \param p_b Pointer to the 4x4 block data
 * \author Filippo Santinello
 * \attention This macro is only used within ANSI C code.
 */

#define ANSI_IDCT(p_b)                    \
{                                       \
    ANSI_ROW_IDCT(p_b);                   \
    ANSI_ROW_IDCT(p_b+4);                 \
    ANSI_ROW_IDCT(p_b+8);                 \
    ANSI_ROW_IDCT(p_b+12);                \
    ANSI_COL_IDCT(p_b);                   \
    ANSI_COL_IDCT(p_b+1);                 \
    ANSI_COL_IDCT(p_b+2);                 \
    ANSI_COL_IDCT(p_b+3);                \
}



/**
 * \brief 1D Hadamard transform for one row of 4 DC coefficients
 * \param p_b Pointer to the row of coefficients
 * \author Filippo Santinello
 * \attention This macro is only used within ANSI C code.
 */

#define ANSI_ROW_HADAMARD(p_b)            \
{                                       \
    t_sint16 t[4];                        \
    *(t) = *((p_b)) + *((p_b)+1);       \
    *(t+1) = *((p_b)) - *((p_b)+1);       \
    *(t+2) = *((p_b)+2) + *((p_b)+3);       \
    *(t+3) = *((p_b)+2) - *((p_b)+3);       \
    *((p_b)) = *(t) + *(t+2);         \
    *((p_b)+1) = *(t) - *(t+2);         \
    *((p_b)+2) = *(t+1) - *(t+3);         \
    *((p_b)+3) = *(t+1) + *(t+3);         \
}



/**
 * \brief 1D Hadamard transform for one col of 4 DC coefficients
 * \param p_b Pointer to the col of coefficients
 * \author Denis Zinato
 * \attention This macro is only used within ANSI C code.
 */
 
#define ANSI_COL_HADAMARD(p_b)            \
{                                       \
    t_sint16 t[4];                        \
    *(t) = *((p_b)) + *((p_b)+4);       \
    *(t+1) = *((p_b)) - *((p_b)+4);       \
    *(t+2) = *((p_b)+8) + *((p_b)+12);       \
    *(t+3) = *((p_b)+8) - *((p_b)+12);       \
    *((p_b)) = *(t) + *(t+2);         \
    *((p_b)+4) = *(t) - *(t+2);         \
    *((p_b)+8) = *(t+1) - *(t+3);         \
    *((p_b)+12) = *(t+1) + *(t+3);         \
}



/**
 * \brief 2D Hadamard transform for one 4x4 block of DC coefficients
 * \param p_b Pointer to the block of coefficients
 * \author Filippo Santinello
 * \attention This macro is only used within ANSI C code.
 */

#define ANSI_HADAMARD(p_b)        \
{                               \
    ANSI_ROW_HADAMARD(p_b);       \
    ANSI_ROW_HADAMARD(p_b+4);     \
    ANSI_ROW_HADAMARD(p_b+8);     \
    ANSI_ROW_HADAMARD(p_b+12);    \
    ANSI_COL_HADAMARD(p_b);       \
    ANSI_COL_HADAMARD(p_b+1);     \
    ANSI_COL_HADAMARD(p_b+2);     \
    ANSI_COL_HADAMARD(p_b+3);    \
}



/**
 * \brief Transposition of a 4x4 block
 * \param p_b Pointer to the block to be transposed
 * \author Filippo Santinello
 * \attention This macro is only used within ANSI C code.
 */

#define ANSI_TRANSPOSE(p_b)                                       \
{                                                               \
    register t_sint16 coef;                                       \
    coef = *((p_b)+ 4); *((p_b)+ 4) = *((p_b)+ 1); *((p_b)+ 1) = coef;  \
    coef = *((p_b)+ 8); *((p_b)+ 8) = *((p_b)+ 2); *((p_b)+ 2) = coef;  \
    coef = *((p_b)+12); *((p_b)+12) = *((p_b)+ 3); *((p_b)+ 3) = coef;  \
    coef = *((p_b)+ 9); *((p_b)+ 9) = *((p_b)+ 6); *((p_b)+ 6) = coef;  \
    coef = *((p_b)+13); *((p_b)+13) = *((p_b)+ 7); *((p_b)+ 7) = coef;  \
    coef = *((p_b)+14); *((p_b)+14) = *((p_b)+11); *((p_b)+11) = coef;  \
}


#endif

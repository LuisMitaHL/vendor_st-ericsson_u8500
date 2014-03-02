/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef	cortexa9_common_h_
#define cortexa9_common_h_

#define wlsr(a,b)  ((a)>>(b))

#define wextractu(a,b) (((((b)>>8)& 0x3f) != 32 )? (((a) >>((b)& 0x3f))&(((long)1<<(((b)>>8)& 0x3f))-1)): ((a) >>((b)& 0x3f)))

#define wextractum(a,b) (((b & 0x3f) != 32 )? (a &(((long)1<<(b & 0x3f))-1)):a)

#define wextract(a,b) (((((b)>>8)& 0x3f) != 32 )? (((((a) >>((b)& 0x3f))&(((long)1<<(((b)>>8)& 0x3f))-1)))<<(32 -(((b)>>8)& 0x3f)))>>(32 -(((b)>>8)& 0x3f)): ((a) >>((b)& 0x3f)))

#define winsertu(a,b,c) ((((unsigned int)(c))&(~((((long)1<< (((b)>>8)&0x3f))-1)<<((b)&0x3f))))| ((((unsigned int)(a)) & (((long)1<< ((b >> 8) & 0x3f))-1))<<( b & 0x3f)))

#define winsert(a,b,c) ((((int)(c))&(~((((long)1<< (((b)>>8)&0x3f))-1)<<((b)&0x3f))))| ((((int)(a)) & (((long)1<< ((b >> 8) & 0x3f))-1))<<( b & 0x3f)))

#define wextract_l(a) ((int)(a)&(0xffffffffL))

#define wmerge(a,b) ((((a)&0xff)<<8)|((b)&0xff))

#define winterval(a,b) (((a)<0)? 0: (((a)>(b))? (b):(a)) )

#define wmin(a,b) ((a < b)? a:b)
#define mod_add(reg_value,index,Max_value,Min_value) (((unsigned int)reg_value+ (unsigned int)index*sizeof(int))>=(unsigned int)Max_value? ((unsigned long)reg_value+ (unsigned int)index*sizeof(int))-((unsigned int) Max_value-(unsigned int) Min_value):(((unsigned int)reg_value+ (unsigned int)index*sizeof(int))<(unsigned int)Min_value)?((unsigned long)reg_value+ (unsigned int)index*sizeof(int))+((unsigned long) Max_value-(unsigned int) Min_value):((unsigned long)reg_value+ (unsigned int)index*sizeof(int)))

#define wextract_h(a)  ((int)(a>>32)&(0xffffffffL))

//fractional multiplication with rounding
#define builtin_mpf_32_r(a, b) (int)(((((long long)(a) * (long long)(b)) <<1)+0x80000000LL)>>32)
//fractional multiplication 
#define builtin_mpf_32(a, b) (int)(((((long long)(a) * (long long)(b)) <<1))>>32)
//static  unsigned int __clz(unsigned int input);
#ifndef STM_ARM_MODELS
#ifndef MMDSP
#if(!defined __CC_ARM )||(defined RVDS_2_2)
/* Count Leading Zeroes */
const static char l1d8[256] =	// AG Lookup table to evaluate exponent
{
	  8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};	

static  unsigned int __clz(unsigned int input)
{
	int one, two, three, four;
	long  retval;
	one = ((unsigned char) (input >> 24));   
	two = ((unsigned char) (input >> 16));
	three = ((unsigned char)(input >> 8));
	four  = ((unsigned char)(input));

	retval = one ? l1d8[one]  : two ? 8+ l1d8[two] : three ? 16 + l1d8[three] :
	24 + l1d8[four];
	return (retval); 
}
#endif
#endif
#endif

#endif /* cortexa9_commom_h_ */

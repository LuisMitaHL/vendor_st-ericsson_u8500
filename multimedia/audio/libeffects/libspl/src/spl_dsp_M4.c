/*####################################################################*/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * 
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   spl_dsp_M4.c
 * \brief  
 * \author ST-Ericsson
 */
/**********************************************************************/
/**
  \file spl_dsp_M4.c
  \brief mathematical and signal processing routines 
  \author Zong Wenbo
  \email wenbo.zong@st.com
  \date DEC 1,  2009
  \last rev Matthieu Durnerin March 3, 2010
  */

#include "spl_defines.h"
#ifdef M4
#include "audiolibs_common.h"
#include "spl_algo_M4.h"
#include "spl_dsp_M4.h"
#include <stdio.h>
// input:  Q47 
// result: Q16
int spl_log10(long long Input)
{
	int i,n;
	long long X;
	int Y;
	int Tmp, SQY2, Y3;
	X = llabs(Input);
//-------------------------------------------------------------
//	n = wedge_X_M4(X);        // Q47, # of bits to shift left
	n=0;
	X=llabs(X);
	for(i=56;i>=2;i--)
	{
		if (X & 1) n=(int) i;
		X=X>>1;
	}
	n=n-10;
//------------------------------------------------------------------
	X = Input<<n;
	X = (Q47- X); // 1-x in Q47
	Y = (int)((X+(long long)0x800000)>>24);       // 1-x in Q23
	Tmp = (int)((((long long)Y*(long long)Y)+0x400000)>>23);
	Y3 = (int)((((long long)Tmp*(long long)Y)+0x400000)>>23);
	SQY2 = Tmp>>1;
	Y3 = (int)((((long long)Y3*(long long)2796203)+0x400000)>>23); // 1/3 in Q23
	X = (long long)(-n)* (long long)2525223;	// log10(2) in Q23, result in Q23
	X = X - 
		(int)((((long long) Y)*((long long) 3643126))>>23) - 
		(int)((((long long)SQY2)*((long long) 3643126))>>23) - 
		(int)((((long long)Y3)*((long long) 3643126))>>23); // 1/ln(10) in Q23
	X = X<<17; //Q23==>Q40
	Y = (int)((X+(long long)0x800000)>>24);       // Q40==>>Q16
	return Y; 
}
#define POW10_FRAC          16
#define POW10_FRAC_MASK     ((1<<POW10_FRAC)-1)
#define POW10_K             6966588  // 3.21, 3.321928
#define POW2_F3             662868	 // 1.23, 0.07902 
#define POW2_F2             1880105  // 1.23, 0.224126 
#define POW2_F1             5845509  // 1.23, 0.696839 
#define POW2_F0	            8387031  // 1.23, 0.999812
#define POW10_GAIN_FRAC     22          // < POW10_FRAC  
#define POW10_GAIN_INT      (24-POW10_GAIN_FRAC) 
int spl_pow2(int z);
// X: 8.16
// result: 1.23 
void spl_pow10(int X, int *exp, int *mant)
{
	int z;

	// 10^X = 2^(k*x) = 2^z
	// a = floor(z)
	// b = z - a
	// 2^z = 2^(a+b) = 2^a * 2^b
	// 2^b = f3 * b^3 + f2 * b^2 + f1 * b^1 + f0
	z=(int)((((long long)X)*((long long) POW10_K))>>23); // 8.16 * 3.21 -> 10.14
	*exp = z>>14;
	*mant = z - (*exp<<14);
	*mant = *mant<<2; // 8.16
	*mant = spl_pow2(*mant); // 2.22
	*exp = *exp + 1; // convert mantissa to 1.23
	return ;
}
// z: 8.16
// result: 2.22 
int spl_pow2(int z)
{
	int a;
	int b;
	int pow2;
	int res; 
	// 2^z
	// a = floor(z)
	// b = z - a
	// 2^z = 2^(a+b) = 2^a * 2^b
	// 2^b = f3 * b^3 + f2 * b^2 + f1 * b^1 + f0
	a = ((z & (~POW10_FRAC_MASK)) >> POW10_FRAC); 
	b = (z & POW10_FRAC_MASK); // 8.16 
	b = (b << (23-POW10_FRAC)); // 1.23 
	if (b != 0)
	{
		pow2 =(int)((((long long)b)*((long long) POW2_F3))>>23); // 1.23
		pow2 = ((pow2 + POW2_F2)>>1); // 2.22
		pow2 =(int)((((long long)b)*((long long) pow2))>>23); // 2.22
		pow2 = pow2 + (POW2_F1>>1); // 2.22
		pow2 =(int)((((long long)b)*((long long) pow2))>>23); // 2.22
		pow2 = pow2 + (POW2_F0>>1); // 2.22
	}
	else
	{
		pow2 = (1<<22); // 2.22
	}
	a = (a > (POW10_GAIN_INT-2) ? (POW10_GAIN_INT-2) : (a < -POW10_GAIN_FRAC ? -POW10_GAIN_FRAC : a));
	//pow2 = (pow2 >> (30-POW10_GAIN_FRAC)); // 2.22
	//res = (a > 0? pow2 << a : pow2 >> (-a)); 
	// the above two steps can be combined into one
	res = pow2 >> (22-POW10_GAIN_FRAC - a); // (22-POW10_GAIN_FRAC - a) = # of bits to shift right, in the range [0 22]
	return res;
}
// input:  Q23, good approximation for the range [-0.05 0.05]; supported range [-0.5 0.5]  
// result: Q21
int spl_pow10_coef(int Input)
{
	int Base;
	int tmp;
	int i;
	int t;
	// 10^X = e^(X*ln(10))
	// e^y = 1 + y + y^2/2
	Base = Input; 
	t = 0;
	while (abs(Base) > 3643120) // to prevent overflow when multiplying ln10
	{
		Base>>=1;
		t++;
	}
	tmp = (int)((((long long)4828871*(long long)Base)+0x400000)>>23); // ln10 in Q21
	Base = tmp;			             // Q21
	tmp = (int)((((long long)tmp*(long long)tmp)+0x400000)>>23); // Q21xQ21 ==>> Q19
	tmp = Base + 2097152 + (tmp<<1); // Q21, 2097152=2^21
	// 10^(X/(2m)) = (10^X)^(1/(2m)) = A
	// 10^X = A^(2m) = (A^2)^m	
	for (i=0; i<t; i++)
	{
		tmp =(int)((((long long)tmp)*((long long) tmp))>>23); // Q21xQ21 ==>> Q19
		tmp = tmp<<2; //Q21
	}
	return tmp;
}
#endif //ifdef M4



 

#ifndef _softpfloat_conv_inlines_h_
#define _softpfloat_conv_inlines_h_

#include "softpfloat.h"

#ifndef __flexcc2__
static INLINE spfloat
float2spf_inline( float value)
{
	float mant;
	int   exponent;
	int   mantisse;
	spfloat res_pfloat = 0;
	
	mant=(float)frexp(value,&exponent);

	/* handle specific -0.5 case : the range for the mantissa is
	   [-1.0,1.0[, but frexp always returns an absolute value in
	   [-.5,1[
	*/
	if ( mant == -0.5 ) {
		mant = -1.0;
		exponent = exponent -1;
	}
	
	if(  exponent >= SPFLOAT_MAX_EXP) {
		exponent = SPFLOAT_MAX_EXP; 
	}
	if(  exponent <= SPFLOAT_MIN_EXP) {
		exponent = SPFLOAT_MIN_EXP; 
	}
	   
	//mantisse = FORMAT_FLOAT( mant, (1<<(SPFLOAT_MANT_LENGTH-1)) );
	mantisse = (int) (mant * (SPFLOAT_MAX_MANT+1));

	mantisse <<= (24-SPFLOAT_MANT_LENGTH);
	
	// build new spfloat word
	res_pfloat = spfloat_msp_mant_exp(res_pfloat, mantisse, exponent);

	return res_pfloat;
	
} /* spfloat float2sfp_inline( float value) */


static INLINE float
spf2float_inline(spfloat a)
{
	float res;
	
	int exponent = spfloat_get_exponent(a);
	int mantisse = spfloat_get_mant(a);
	
	res = (float) ldexp((float)mantisse/(float)(1<<(SPFLOAT_MANT_LENGTH-1)), exponent);
    return res;
} /* float spf2float_inline(spfloat a) */

#endif /* __flexcc2__ */
#endif /* _softpfloat_conv_inlines_h_ */ 



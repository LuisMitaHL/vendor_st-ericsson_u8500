

#ifndef _softpfloat_inlines_h_
#define _softpfloat_inlines_h_

#include "softpfloat.h"

#define SPFLOAT_CONST_1_0 INIT_SPFLOAT(1.000000000000000e00f , 16384, 1)

#pragma inline
static INLINE spfloat
spfloat_add_inline( spfloat a, spfloat b)
{	
	int expa, expb, res_exp, res_edge;
	int tempa, tempb;
	WORD56  manta, mantb;
	int diff_exp;
	int res_mant;
	spfloat res_pfloat;

	// get exponent
	expa = spfloat_get_exponent(a);
	expb = spfloat_get_exponent(b);

    // get mantisses, equal exponents if zero 
	tempa = spfloat_get_mant(a);
	cmove( tempa == 0, expa, expb);
	tempb = spfloat_get_mant(b);
	cmove( tempb == 0, expb, expa);

	// put mant in high part
	manta = spfloat_make_acc(manta, tempa);
	mantb = spfloat_make_acc(mantb, tempb);
	
	// set a and b with same exponent
	diff_exp = wsub(expa, expb);
	if ( diff_exp >= 0 ){
		mantb = wX_msr(mantb, diff_exp);
		res_exp = expa;
	} else {
		manta = wX_msl(manta, diff_exp);
		res_exp = expb;
	}

	// add mantisses
	manta = wX_add(manta, mantb);

		// normalize
	res_edge = wedge_X(manta);
	res_exp = wsubsat(res_exp, res_edge);

	// clip exponent
	spfloat_clip_exp(res_exp);
	
	manta = wX_msl(manta, res_edge);
	res_mant = wround_L(manta);

	// if result mantissa is null clear exponent
#ifndef __flexcc2__
	cmoveonZ(res_exp, 0);
#else
	if( res_mant == 0 ) {
		res_exp = 0;
	}
#endif /* __flexcc2__ */

	// round before mask
	spfloat_round_mant(res_mant);
	
	// build new spfloat word
	res_pfloat = spfloat_msp_mant_exp(res_pfloat, res_mant, res_exp);

	return res_pfloat;
	
}/* spfloat spfloat_add_inline( spfloat a, spfloat b) */

#pragma inline
static INLINE spfloat
spfloat_sub_inline( spfloat a, spfloat b)
{	
	int expa, expb, res_exp, res_edge;
	int    tempa, tempb;
	WORD56 manta, mantb;
	int diff_exp;
	int res_mant;
	spfloat res_pfloat;
	
	// get exponent
	expa = spfloat_get_exponent(a);
	expb = spfloat_get_exponent(b);

    // get mantisses, equal exponents if zero 
	tempa = spfloat_get_mant(a);
	cmove( tempa == 0, expa, expb);
	tempb = spfloat_get_mant(b);
	cmove( tempb == 0, expb, expa);

	// put mant in high part
	manta = spfloat_make_acc(manta, tempa);
	mantb = spfloat_make_acc(mantb, tempb);

	// set a and b with same exponent
	diff_exp = wsub(expa, expb);
	if ( diff_exp >= 0 ){
		mantb = wX_msr(mantb, diff_exp);
		res_exp = expa;
	} else {
		manta = wX_msl(manta, diff_exp);
		res_exp = expb;
	}

	// add mantisses
	manta = wX_sub(manta, mantb);
	
	// normalize
	res_edge = wedge_X(manta);
	res_exp = wsubsat(res_exp, res_edge);

	// clip exponent
	spfloat_clip_exp(res_exp);
		
	manta = wX_msl(manta, res_edge);
	res_mant = wround_L(manta);

	// if result mantissa is null clear exponent
#ifndef __flexcc2__
	cmoveonZ(res_exp, 0);
#else
	if( res_mant == 0 ) {
		res_exp = 0;
	}
#endif /* __flexcc2__ */

	// round before mask
	spfloat_round_mant(res_mant);
	
	// build new spfloat word
	res_pfloat = spfloat_msp_mant_exp(res_pfloat, res_mant, res_exp);

	return res_pfloat;
	
}/* spfloat spfloat_sub_inline( spfloat a, spfloat b) */

#pragma inline
static INLINE spfloat
spfloat_mul_inline( spfloat a, spfloat b)
{
	int expa, expb, res_exp,exp;
	int manta, mantb;
	int res_mant;
	WORD48 res;
	spfloat res_pfloat;
	
    // get mantisses
	manta = spfloat_get_mant( a);
	mantb = spfloat_get_mant( b);
		
	// add exponent
	expa = spfloat_get_exponent(a);
	expb = spfloat_get_exponent(b);
	res_exp = wadd(expa, expb);
	
	// multiply mantissa
	res = wX_fmul(manta,mantb);

	// normalize result
	exp = wedge_X(res);
	res = wL_msl(res,exp);
	res_exp -= exp;
		
	// Compensate for right shift of mantissas if needed
#if SPFLOAT_EXP_LENGTH!=24
	res_exp += (2*SPFLOAT_EXP_LENGTH);
#endif

	// clip exponent
	spfloat_clip_exp(res_exp);

	// extract mantissa
	res_mant = wround_L(res);

	/* clear exponent if result mantissa is null */
#ifndef __flexcc2__
	cmoveonZ(res_exp, 0);
#else
	if (res_mant == 0) {
		res_exp = 0;
	}
#endif /* __flexcc2__ */

	// round before mask
	spfloat_round_mant(res_mant);
	
	// build new spfloat word
	res_pfloat = spfloat_msp_mant_exp(res_pfloat, res_mant, res_exp);

	return res_pfloat;

} /* spfloat spfloat_mul_inline( spfloat a, spfloat b) */

#pragma inline
static INLINE spfloat
spfloat_imul_inline( int a, spfloat b)
{
	int res_exp,exp;
	int mantb;
	int res_mant;
	WORD48 res;
	spfloat res_pfloat;
	
    // get mantisses
	mantb = spfloat_get_mant(b);
	
	// multiply mantissa
	res = wL_imul(a,mantb);

	// add exponent
	res_exp = spfloat_get_exponent(b);
	
	// normalize result
	exp = wedge_X(res);
	res = wL_msl(res,exp);
	res_exp -= exp;
		
	// Compensate for right shift of mantissas if needed
#if SPFLOAT_EXP_LENGTH!=24
	res_exp += (SPFLOAT_EXP_LENGTH+24);
#else
	res_exp += (24;
#endif

	// clip exponent
	spfloat_clip_exp(res_exp);

	// extract mantissa
	res_mant = wround_L(res);

	/* clear exponent if result mantissa is null */
#ifndef __flexcc2__
	cmoveonZ(res_exp, 0);
#else
	if (res_mant == 0) {
		res_exp = 0;
	}
#endif /* __flexcc2__ */

	// round before mask
	spfloat_round_mant(res_mant);
	
	// build new spfloat word
	res_pfloat = spfloat_msp_mant_exp(res_pfloat, res_mant, res_exp);

	return res_pfloat;

} /* spfloat spfloat_imul_inline( spfloat a, spfloat b) */

#pragma inline
static INLINE spfloat
spfloat_mul3_inline( spfloat a, spfloat b, spfloat c )
{
	spfloat res_pfloat;
	
	res_pfloat = spfloat_mul(a,b);
	res_pfloat = spfloat_mul(res_pfloat,c);
	
	return res_pfloat;

} /* spfloat spfloat_mul3_inline( spfloat a, spfloat b, spfloat c ) */

#pragma inline
static INLINE spfloat
spfloat_square_inline( spfloat a)
{
	spfloat res_pfloat;
	
	res_pfloat = spfloat_mul(a,a);

	return res_pfloat;

} /* spfloat spfloat_square_inline(spfloat a) */


#pragma inline
static INLINE spfloat
spfloat_div_2power_inline(spfloat a, int b)
{
	int res_exp;
	spfloat res_pfloat;

	if(a == 0)
		return a;
	
	res_exp = spfloat_get_exponent(a);
	res_exp -= b;

	// clip exponent
	spfloat_clip_exp(res_exp);

	// build new spfloat word
	res_pfloat = spfloat_msp_mant_exp(res_pfloat, a, res_exp);
	
	return(res_pfloat);
}/*  spfloat spfloat_div_2power_inline(spfloat a) */

#pragma inline
static INLINE int
spfloat_eq_inline( spfloat a, spfloat b)
{
	int expa, expb;
    WORD56 manta, mantb;
	
    // get mantisse in high part
	manta = spfloat_get_mant(a);
	mantb = spfloat_get_mant(b);

	if(manta != mantb)
		return (FALSE);
	
	// get exponent
	expa = spfloat_get_exponent(a);
	expb = spfloat_get_exponent(b);

	if(expa != expb)
		return (FALSE);	

	return (TRUE);

} /* int spfloat_eq_inline(spfloat a, spfloat b) */

#pragma inline
static INLINE int
spfloat_gt_inline( spfloat a, spfloat b)
{
	int expa, expb;
	int    tempa, tempb;
    WORD56 manta, mantb;
	int diff_exp;

	// get exponent
	expa = spfloat_get_exponent(a);
	expb = spfloat_get_exponent(b);

    // get mantisses, equal exponents if zero 
	tempa = spfloat_get_mant(a);
	cmove( tempa == 0, expa, expb);
	tempb = spfloat_get_mant(b);
	cmove( tempb == 0, expb, expa);

	// put mant in high part
	manta = spfloat_make_acc(manta, tempa);
	mantb = spfloat_make_acc(mantb, tempb);

	// set a and b with same exponent
	diff_exp = wsub(expa, expb);
	if ( diff_exp >= 0 ){
		mantb = wX_msr(mantb, diff_exp);
	} else {
		manta = wX_msl(manta, diff_exp);
	}

	// add mantisses
	manta = wX_sub(manta, mantb);

	if(manta > 0)
		return (TRUE);
	else
		return (FALSE);
} /* int spfloat_gt_inline(spfloat a, spfloat b) */

#pragma inline
static INLINE int
spfloat_ge_inline( spfloat a, spfloat b)
{
	int expa, expb;
	int    tempa, tempb;
    WORD56 manta, mantb;
	int diff_exp;

	// get exponent
	expa = spfloat_get_exponent(a);
	expb = spfloat_get_exponent(b);

    // get mantisses, equal exponents if zero 
	tempa = spfloat_get_mant(a);
	cmove( tempa == 0, expa, expb);
	tempb = spfloat_get_mant(b);
	cmove( tempb == 0, expb, expa);

	// put mant in high part
	manta = spfloat_make_acc(manta, tempa);
	mantb = spfloat_make_acc(mantb, tempb);

	// set a and b with same exponent
	diff_exp = wsub(expa, expb);
	if ( diff_exp >= 0 ){
		mantb = wX_msr(mantb, diff_exp);
	} else {
		manta = wX_msl(manta, diff_exp);
	}

	// add mantisses
	manta = wX_sub(manta, mantb);

	if(manta >= 0)
		return (TRUE);
	else
		return (FALSE);
} /* int spfloat_ge_inline(spfloat a, spfloat b) */

#pragma inline
static INLINE int
spfloat_lt_inline( spfloat a, spfloat b)
{
	int expa, expb;
	int    tempa, tempb;
    WORD56 manta, mantb;
	int diff_exp;
	
	// get exponent
	expa = spfloat_get_exponent(a);
	expb = spfloat_get_exponent(b);

    // get mantisses, equal exponents if zero 
	tempa = spfloat_get_mant(a);
	cmove( tempa == 0, expa, expb);
	tempb = spfloat_get_mant(b);
	cmove( tempb == 0, expb, expa);

	// put mant in high part
	manta = spfloat_make_acc(manta, tempa);
	mantb = spfloat_make_acc(mantb, tempb);
	// set a and b with same exponent
	diff_exp = wsub(expa, expb);
	if ( diff_exp >= 0 ){
		mantb = wX_msr(mantb, diff_exp);
	} else {
		manta = wX_msl(manta, diff_exp);
	}

	// add mantisses
	manta = wX_sub(manta, mantb);

	if(manta < 0)
		return (TRUE);
	else
		return (FALSE);
} /* int spfloat_lt_inline(spfloat a, spfloat b) */

#pragma inline
static INLINE int
spfloat_gt_one_inline(spfloat a)
{
	int mant = spfloat_get_mant(a);
	int tmp = (( 1 << (SPFLOAT_MANT_LENGTH-2)));
	if( mant > tmp ){
		int expa = spfloat_get_exponent(a);		
		if(expa >= 1)
			return TRUE;
	} else if( mant == tmp ){		
		int expa = spfloat_get_exponent(a);
		if(expa > 1)
			return TRUE;
	}
	return FALSE;

}/* int spfloat_gt_one_inline(spfloat a) */

#pragma inline
static INLINE spfloat
spfloat_abs_inline(spfloat a)
{
	int mant = spfloat_get_mant(a);
	if( mant < 0 ) {
		int shift;
		int exponent = spfloat_get_exponent(a);
		mant = -mant;

		// in the case of 1.0, need to shift right to avoid saturations
		shift = wedge(mant) - SPFLOAT_EXP_LENGTH;
		exponent -= shift;

		// clip exponent
		spfloat_clip_exp(exponent);

		// shift left
		mant = wmsl(mant,shift+SPFLOAT_EXP_LENGTH);

		// form result
		a = spfloat_msp_mant_exp(a, mant, exponent);
		
	}
	return a;
} /* int spfloat_abs_inline(spfloat a) */

#pragma inline
static INLINE spfloat
spfloat_neg_inline(spfloat a)
{
	int mant = spfloat_get_mant(a);
	if( mant != 0 ) {
		
		int shift;
		int exponent = spfloat_get_exponent(a);
		mant = -mant;

		// in the case of 1.0, need to shift right to avoid saturations
		shift = wedge(mant) - SPFLOAT_EXP_LENGTH;
		exponent -= shift;

		// clip exponent
		spfloat_clip_exp(exponent);

		// shift left
		mant = wmsl(mant,shift+SPFLOAT_EXP_LENGTH);

		// form result
		a = spfloat_msp_mant_exp(a, mant, exponent);
		
	}

	return a;
} /* int spfloat_neg_inline(spfloat a) */

#pragma inline
static INLINE spfloat
spfloat_max_inline( spfloat a, spfloat b)
{
	int res = spfloat_gt_inline(a, b);
	if( res == TRUE ) {
		return (a);
	} else {
		return (b);
	}
} /* int spfloat_max_inline(spfloat a, spfloat b) */

#pragma inline
static INLINE spfloat
spfloat_min_inline( spfloat a, spfloat b)
{		
	if( spfloat_gt_inline(a, b) == TRUE) {
		return (b);
	} else {
		return (a);
	}
} /* int spfloat_max_inline(spfloat a, spfloat b) */

#pragma inline
static INLINE int
spfloat_eq_zero_inline(spfloat a)
{
	int mant = spfloat_get_mant(a);
	if ( mant == 0) {
		return TRUE;
	}
	return FALSE;
	
}/* int spfloat_eq_zero_inline(spfloat a) */

#pragma inline
static INLINE int
spfloat_ge_zero_inline(spfloat a)
{
	int mant = spfloat_get_mant(a);
	if ( mant>=0 ) {
		return TRUE; 
	} 
	return FALSE;	
} /* int spfloat_ge_zero_inline(spfloat a) */

#pragma inline
static INLINE int
spfloat_gt_zero_inline(spfloat a)
{
	int mant = spfloat_get_mant(a);
	if ( mant>0 ) {
 		return TRUE;
	}
	return FALSE;

} /* int spfloat_gt_zero_inline(spfloat a) */

#pragma inline
static INLINE int
spfloat_lt_zero_inline(spfloat a)
{
	int mant = spfloat_get_mant(a);
	
	if ( mant<0 ) {
		return TRUE; 
	} 
	return FALSE; 
	
} /* int spfloat_lt_zero_inline(spfloat a)*/

#pragma inline
static INLINE int
spfloat_neq_zero_inline(spfloat a) 
{
	int mant = spfloat_get_mant(a);
	if ( mant != 0 ) {
		return TRUE;
	}
	return FALSE;
} /* int spfloat_neq_zero_inline(spfloat a) */

#pragma inline
static INLINE spfloat
fract2spf_inline(int value)
{
	spfexp exp;
	spfloat res_pfloat = 0;
	
	exp = wedge(value);
	value = wmsl(value,exp);
	if ( value != 0 ) {
		exp = -exp;
		res_pfloat = spfloat_msp_mant_exp(res_pfloat, value, exp);
	}
	return res_pfloat;
	
} /*spfloat fract2spf_inline(fract value) */

#pragma inline
static INLINE spfloat
dfract2spf_inline(long long value)
{
	spfexp exp;
	spfloat res_pfloat = 0;
	int    val;
	exp = wedge_X(value);
	val = wX_msl(value,exp-24);
	if ( val != 0 ) {
		exp = -exp;
		res_pfloat = spfloat_msp_mant_exp(res_pfloat, val, exp);
	}
	return res_pfloat;
	
} /*spfloat dfract2spf_inline(long long value) */

#pragma inline
static INLINE int
spf2fract_inline(spfloat a)
{
#ifdef PFLOAT_TO_INT_BEST_CONVERSION
	WORD48 mant;
	int     res_mant;
	spfexp  exp;

	exp = spfloat_get_exponent(a);
	//mant = 0;
	//mant = winsert_h(mant,  spfloat_get_mant(a) );// get mantisse in high part
	mant = wX_depmsp(spfloat_get_mant(a));
	
	// saturation if exp > 0
	mant = wL_msl(mant,exp + (24-SPFLOAT_MANT_LENGTH));

	// extract mantissa
	res_mant = wround_L(mant);

	return res_mant;
#else
	WORD48 mant;
	int     res_mant;
	int     tmp;
	spfexp  exp;

	exp = spfloat_get_exponent(a);
	//mant = 0;
	tmp = spfloat_get_mant(a);
	//mant = winsert_h(mant,  spfloat_get_mant(a) );// get mantisse in high part
	mant = wX_depmsp(tmp);
	

	if( tmp < 0 )
		mant = -mant;
	
	// saturation if exp > 0
	mant = wL_msl(mant,exp + (24-SPFLOAT_MANT_LENGTH));

	// extract mantissa
	res_mant = wround_L(mant);

	if( tmp < 0 )
		res_mant = -res_mant;
	
	return res_mant;
	
#endif
	
}/* fract spf2fract_inline(spfloat a) */

#pragma inline
static INLINE int
spf2fractNoRound_inline(spfloat a)
{
	int     mant;
	int     res_mant;
	spfexp  exp;

	exp = spfloat_get_exponent(a);
	mant = 0;
	mant = spfloat_get_mant(a);

	// saturation if exp > 0
	res_mant = wmsl(mant,exp + (24-SPFLOAT_MANT_LENGTH));

	return res_mant;
	
}/* fract spf2fractNoRound_inline(spfloat a) */

#pragma inline
static INLINE WORD48
spf2dfract_inline(spfloat a)
{
	WORD48 mant;
	spfexp  exp;

	exp = spfloat_get_exponent(a);
	//mant = 0;
	//mant = winsert_h(mant,  spfloat_get_mant(a) );// get mantisse in high part
	mant = wX_depmsp(spfloat_get_mant(a));
	
	// saturation if exp > 0
	mant = wL_msl(mant,exp + (24-SPFLOAT_MANT_LENGTH));

	return mant;
} /* dfract spf2dfract(spfloat a) */

#pragma inline
static INLINE spfloat
int2spf_inline(int value)
{
	spfexp exp;
	spfloat res_pfloat = 0;
	
	exp = wedge(value);
	value = wmsl(value,exp);
	if ( value != 0 ) {
		exp = 23 - exp;
		res_pfloat = spfloat_msp_mant_exp(res_pfloat, value, exp);
	}
	return res_pfloat;
	
} /*spfloat int2spf_inline(int value) */

#ifdef PFLOAT_TO_INT_BEST_CONVERSION
#pragma inline
static INLINE int
spf2int_inline(spfloat a)
{
	WORD48 mant;
	int     res_mant;
	spfexp  exp;

	exp = spfloat_get_exponent(a);
	//mant = winsert_h(mant, spfloat_get_mant(a));
	mant = wX_depmsp(spfloat_get_mant(a));
	
	mant = wL_msl(mant,exp - (23-(24-SPFLOAT_MANT_LENGTH)));

	// extract mantissa
	res_mant = wround_L(mant);

	return res_mant;
	
}/* int spf2int_inline(spfloat a) */

#pragma inline
static INLINE int
spf2intNoRound_inline(spfloat a)
{
	int     mant;
	int     res_mant;
	spfexp  exp;

	exp = spfloat_get_exponent(a);
	mant = spfloat_get_mant(a);

	res_mant = wmsl(mant,exp-(23-(24-SPFLOAT_MANT_LENGTH)));

	return res_mant;
	
}/* int spf2intNoRound_inline(spfloat a) */
#else
#pragma inline
static INLINE int
spf2int_inline(spfloat a)
{
	int     mant;
	int     res_mant;
	spfexp  exp;

	exp = spfloat_get_exponent(a) -(SPFLOAT_MANT_LENGTH-1);
	mant = spfloat_get_mant(a);

	if( exp > 0 ) {
		res_mant = wmsl(mant,exp);
	} else {
		exp = wabssat(exp);
		if ( mant< 0 ) {

			/* warning : for negative values, fhg truncates to higher values.
			 * For compatibility, we add one lsb
			 */
			mant += (1<<exp);
		}
		res_mant = wmsr(mant,exp);
	}

	return res_mant;
	
}/* int spf2int_inline(spfloat a) */

#pragma inline
static INLINE int
spf2intNoRound_inline(spfloat a)
{
	int     mant;
	int     res_mant;
	spfexp  exp;

	exp = spfloat_get_exponent(a)-(SPFLOAT_MANT_LENGTH-1);;
	mant = spfloat_get_mant(a);

	res_mant = wmsl(mant,exp);

	return res_mant;
	
}/* int spf2intNoRound_inline(spfloat a) */
#endif

#pragma inline
static INLINE int
spf_floor_int_inline(spfloat a)
{
	int res;
	spfloat b;
	
	res = spf2int(a);
	b = int2spf(res);

	if ( spfloat_lt(a,b) ) {
		res = res - 1;
	}

	return res;
} /* end int spf_floor_int_inline(spfloat a) */

#pragma inline
static INLINE spfloat
spf_floor_inline(spfloat a)
{
	int res;

	res = spf_floor_int_inline(a);
	a = int2spf(res);
	
	return a;
} /* spfloat spf_floor_inline(spfloat a) */

#pragma inline
static INLINE int
spf_ceil_int_inline(spfloat a)
{
	int res;
	spfloat b;

	res = spf2intNoRound(a);
	b = int2spf(res);

	if ( spfloat_gt(a,b) ) {
		res = res + 1;
	}

	return res;
	
} /* int spf_ceil_int_inline(spfloat a) */

#endif /* Do not edit below this line */



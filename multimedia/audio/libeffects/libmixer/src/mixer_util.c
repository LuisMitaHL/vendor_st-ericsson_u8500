/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mixer_util.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _NMF_MPC_
#include "aep.h"
#endif
#include "mixer_include.h"


/*
 * envelop detection 
 *
 * 
 */
#ifdef MMDSP
MMshort mixer_envelop_detection(MMlong abs_input_sample, MIXER_LOCAL_STRUCT_T *mixer,
		MMshort channel)
{
	MIXER_ENVELOP_STRUCT_T  *env_st = &(mixer->env_st[channel]);
	MMshort abs_x0,abs_x1,abs_x2,abs_in,envelop,alpha,minus_alpha;
#ifdef NEW_ENVELOPE_DETECT
	MMshort shiftval;
#endif
	/************************/
	/* sample normalisation */
	/************************/
	abs_x0 = env_st->abs_x0;
	abs_x1 = env_st->abs_x1;
	abs_x2 = wabssat(wextract_h(abs_input_sample << (24 - GUARD_BIT)));
	env_st->abs_x0 = abs_x1;
	env_st->abs_x1 = abs_x2;

	if((abs_x0 <= abs_x1) &&
			(abs_x2 <= abs_x1))
	{
		env_st->abs_in = abs_x1;
		abs_in = abs_x1;
	}
	else
	{
		abs_in = env_st->abs_in;
	}

	/* ------------------ */
	/* Envelop estimation */
	/* ------------------ */
	envelop =  env_st->envelop;

	if(abs_in < envelop)
	{
		alpha = ALPHA_DW_DEFAULT;
		minus_alpha = (0x800000U - ALPHA_DW_DEFAULT);
#ifdef NEW_ENVELOPE_DETECT
		shiftval=ALPHA_DW_DEFAULT_SHIFTVAL;
#endif
	}
	else
	{
		alpha = ALPHA_UP_DEFAULT;
		minus_alpha = (0x800000U - ALPHA_UP_DEFAULT);
#ifdef NEW_ENVELOPE_DETECT
		shiftval=ALPHA_UP_DEFAULT_SHIFTVAL;
#endif
	}
#ifdef NEW_ENVELOPE_DETECT
	env_st->envelop_long = wL_add(wL_fmul(minus_alpha,abs_in),wL_sub(env_st->envelop_long,wL_msr(env_st->envelop_long,shiftval)));
	env_st->envelop=wround_L(env_st->envelop_long);
#else
	env_st->envelop = waddsat(wfmulr(minus_alpha,abs_in),wfmulr(alpha,envelop));
#endif
	return env_st->envelop;
}


/*********************************************/   
/* Polynom evaluation                        */    
/* Horner Method                             */
/* x = point to evaluate                     */
/* coef = polynom coef (coef[0] X^order coef */
/*                  coef[order] is X^0 coef) */
/* order = polynom order                     */
/*********************************************/   
void mixer_poly_horner_eval(MMshort x,MMlong *coef,MMshort *y,MMshort order)
{
	MMshort i;
	MMlong tmp;
	tmp=coef[0];

	for(i=0;i<order;i++)
	{
		tmp= coef[i+1] + wL_fmul(x,wround_L(tmp));
	}

	/* the result is left shifted because the coefs are
	   right shifted */
	*y = wround_L(wL_msl(tmp,COEF_SHIFT)); 
}



#if !defined(MMDSP) || defined(__flexcc2__) // MMDSP+ version
#pragma ckf wX_div	250001
	asm struct long_long_pair
wX_div( MMlonglong a, MMlonglong b, MMlonglong c )
{
#ifndef __binutils__	
	dmv @{c}, xr2
#else
		dmvl @{c}, xr2	  
#endif
		X_div @{a}, @{b}, @{first}
#ifndef __binutils__  
	dmv xr2, @{second}
#else
	dmvl xr2, @{second}  
#endif
}
#endif

#pragma inline
MMshort mixer_divfract(MMshort n, MMshort d)
{
	struct long_long_pair div_st;
	MMlonglong b;
	MMshort i;

	div_st.first  = n;
	div_st.second = 0LL;
	b = d;

	for ( i = 0 ; i < (MODE - 1) ; i++ )
	{
#if defined(MMDSP) && !defined(__flexcc2__)
		div_st.first = wX_div(div_st.first, b, &div_st.second);
#else
		div_st = wX_div(div_st.first, b, div_st.second);
#endif
	}

	return div_st.second;
}


MMshort mixer_compute_gain(MIXER_LOCAL_STRUCT_T *mixer, MMshort channel)
{
	MMshort x;
	MMshort gain;

	x = mixer_divfract(mixer->threshold,mixer->env_st[channel].envelop);
	mixer_poly_horner_eval(x,mixer->coef_poly,&gain,3);
	return gain;
}


MMlong mixer_short_long_mult(MMshort short_var, MMlong long_var)
{
	MMlong  result;
	MMshort negative = 0;

	if(long_var > 0)
	{
		result = long_var << 1;
	}
	else
	{
		negative =1;
		result = (-long_var) << 1;
	}

	result = wL_imul(wextract_h(result),short_var) +
		(MMlong)wextract_h(wL_imuluu(wextract_l(result),short_var));

	if(negative)
		result = wL_negsat(result);

	return result;
}
#else //if MMDSP

MMlong mixer_envelop_detection(MMlong abs_input_sample, MIXER_LOCAL_STRUCT_T *mixer,
		MMshort channel)
{
	MIXER_ENVELOP_STRUCT_T  *env_st = &(mixer->env_st[channel]);
	MMlong abs_x0_f,abs_x1_f,abs_x2_f,abs_in_f,envelop_f,alpha_f,minus_alpha_f;
	/************************/
	/* sample normalisation */
	/************************/
	abs_x0_f = env_st->abs_x0;
	abs_x1_f = env_st->abs_x1;
	abs_x2_f=fabs(abs_input_sample*GUARD_ATTENUATOR*SIGNAL_ATTENUATOR);
	env_st->abs_x0 = abs_x1_f;
	env_st->abs_x1 = abs_x2_f;

	if((abs_x0_f <= abs_x1_f) &&
			(abs_x2_f <= abs_x1_f))
	{
		env_st->abs_in = abs_x1_f;
		abs_in_f = abs_x1_f;
	}
	else
	{
		abs_in_f = env_st->abs_in;
	}

	/* ------------------ */
	/* Envelop estimation */
	/* ------------------ */
	envelop_f =  env_st->envelop;
	if(abs_in_f < envelop_f)
		alpha_f = mixer->alpha_dw;
	else
		alpha_f = mixer->alpha_up;
	minus_alpha_f=1.0-alpha_f;
	env_st->envelop=
		minus_alpha_f*abs_in_f+alpha_f*envelop_f;
	return (env_st->envelop);
}



// Polynom evaluation                            
// Horner Method                             
// x = point to evaluate                     
// coef = polynom coef (coef[0] X^order coef 
//                  coef[order] is X^0 coef) 
// order = polynom order                     

void mixer_poly_horner_eval(MMlong x,MMlong *coef,MMlong *y,MMshort order)
{
	MMshort i;
	double tmp;
	tmp=coef[0];

	for(i=0;i<order;i++)
		tmp=coef[i+1]+x*tmp;
	*y=tmp;
}


MMlong mixer_compute_gain(MIXER_LOCAL_STRUCT_T *mixer, MMshort channel)
{
	MMlong x;
	MMlong gain_f;
	if ((mixer->env_st[channel].envelop)==0.0)
		return 1.0;
	x = ((mixer->threshold)/(mixer->env_st[channel].envelop));
	mixer_poly_horner_eval(x,mixer->coef_poly,&gain_f,3);
	return gain_f;
}


#endif //#ifdef MMDSP

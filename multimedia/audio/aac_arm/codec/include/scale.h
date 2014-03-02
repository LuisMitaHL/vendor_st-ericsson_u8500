#ifndef _conv_pcm_h_
#define _conv_pcm_h_

extern void
aac_rescale_with_block_exponent(int len,Float *coef,int *exponent,
			    int incr);

void
aac_scale_in_place(Float *coef, int sc_factor,int size,int sc_flag);

void
aac_scale_to_mant_exp(Float *coef,int *exponent, int sc_factor,int size,int sc_flag);

#ifdef ARM

void
aac_scale_data(Info * info, int *coef,  int len,
			   int SSR_profile,UCHAR blockType,int decim_enbl,UCHAR *group);

void
aac_scale_data1(int *coef_fix,  int len, UCHAR blockType);


void
aac_scale_up(int *coef_fix, int size, int profile, int headroom,int decim);

#else
extern void
aac_scale_data(Info * info, Float *coef, int *exponent, int len, int SSR_profile,
					UCHAR blockType,int decim_enbl,UCHAR *group);


extern void
aac_scale_up(Float *coef, int size, int profile, int headroom, int decim);

#endif



void
aac_loop_scale_data(Info * info, Float *coef, int *exponent, int len);
			   
void
aac_scale_subblock(Float **mant,
						 int **ex,
						 SHORT const SR_MEM * band, int n_sblocks, int nbands
						,Info * info

				);
			   

#ifdef MAIN_PROFILE
extern void
convert_fract2ieee(Float *res, int *mant, int *ex, int n);

extern void
convert_ieee2fract(Float *ori, int *mant, int *ex, int n);

#endif /* MAIN_PROFILE */

#endif /* Do not edit below this line */

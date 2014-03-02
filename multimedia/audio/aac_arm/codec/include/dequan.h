#ifndef _dequan_h_
#define _dequan_h_

extern
void aac_pns_init(long *seed);



#ifdef ARM

extern void aac_loop_pns( int *coef,  int width,long *seed, int offset);

extern void aac_mono_dequan(Info *info, Float *coef, UCHAR *group, 
						 SHORT *factors,  UCHAR *cb_map
#ifdef MPEG4V1PNS
						 , long *seed
#endif
#ifdef MPEG4BSAC							
							 ,unsigned short bsac_flag
#endif
						 );

extern void
aac_dequan_mono_subblock(Float **coef,													
							 SHORT **fact,
							 SHORT const * band, int n_sblocks, int nbands, UCHAR *cb_map
#ifdef MPEG4V1PNS	
							 ,long *seed
#endif
							 ,Info * info
#ifdef MPEG4BSAC							
							 ,unsigned short bsac_flag
#endif
							 );

extern int aac_stereo_dequan(Info *info, 
						  UCHAR *mask, UCHAR *group, 
						  UCHAR *cb_map_l,UCHAR *cb_map_r, 
						  SHORT *factors_l, SHORT *factors_r,
						  Float *left, Float *right 					  
#ifdef MPEG4V1PNS
						  ,long *seed
#endif
#ifdef MPEG4BSAC							
							 ,unsigned short bsac_flag
#endif
						  );

extern void aac_stereo_dequan_pass2(Info *info, 
							 UCHAR *mask, UCHAR *group, 
							 UCHAR *cb_map_l,UCHAR *cb_map_r, 
							 SHORT *factors_l, SHORT *factors_r,
							 Float *left, Float *right
							 ,long *seed
					   );

#else /* ARM */
extern
void aac_loop_pns(Float *mant, int *ex, int width,long *seed, int offset);

extern void
aac_dequan_mono_subblock(Float **mant,
						 int **ex,
						 SHORT ** fact,
						 SHORT const SR_MEM * band, int n_sblocks, int nbands, UCHAR *cb_map
#ifdef MPEG4V1PNS	
						,long *seed
#endif
						,Info * info

						);

extern void
aac_mono_dequan(Info * info, Float *coef, UCHAR * group,
				SHORT * factors, int *exponent, UCHAR *cb_map
#ifdef MPEG4V1PNS	
				,long *seed
#endif
				);

extern int
aac_stereo_dequan(Info * info,
				  UCHAR * mask, UCHAR * group,
				  UCHAR * cb_map_l,UCHAR * cb_map_r,
				  SHORT * factors_l, SHORT * factors_r,
				  Float *f_left, Float *f_right, int *exp_left,
				  int *exp_right
#ifdef MPEG4V1PNS	
				  ,long *seed
#endif
				  );

#ifdef MPEG4V1PNS	
extern void
aac_stereo_dequan_pass2(Info * info,
				  UCHAR * mask, UCHAR * group,
				  UCHAR * cb_map_l,UCHAR * cb_map_r,
				  SHORT * factors_l, SHORT * factors_r,
				  Float *f_left, Float *f_right, int *exp_left,
				  int *exp_right
				  ,long *seed
				  );
#endif


#endif /* ARM */

#ifdef ARM 

extern void aac_loop_dequan_is(Float *coef_l, 
					   Float *coef_r,
					   int width, int sign_sfb,
					   int offset_l,int offset_r, int cb
#ifdef MPEG4BSAC							
							 ,unsigned short bsac_flag
#endif
							 );

extern void aac_loop_dequan_ms(Float *coef_l, 
					   Float *coef_r,
					   int width, int offset_l ,int offset_r,int cb_r, int cb_l
#ifdef MPEG4BSAC							
					   ,unsigned short bsac_flag
#endif
					   );

extern void aac_loop_dequan(Float *coef, int width, int offset,int cb
#ifdef MPEG4BSAC							
							 ,unsigned short bsac_flag
#endif
							);

#else
extern void aac_loop_dequan_is(Float *coef_l, int *exp,
				   Float *coef_r, int *exp_r,
				   int width, int offset_l, int offset_r, int sign_sfb);

extern void aac_loop_dequan_ms(Float *coef_l, int *exp,
				   Float *coef_r, int *exp_r,
				   int width, int offset_l, int offset_r);

extern void aac_loop_dequan(Float *coef, int *exp, int width, int offset);
#endif



#endif /* Do not edit below this point */

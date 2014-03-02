


#ifndef _inv_imdct_h_
#define _inv_imdct_h_

#ifdef ARM

#ifdef NEON 

extern void mp3_imdct_shortblk_dual_neon(int *in, int *ou,int *prevblck);
extern void mp3_imdct_longblk_dual_neon(int *Input, int* Win_tab, int *Output, int *prevblk);

#elif (defined VECT )

extern void mp3_imdct_shortblk_dual_vect(int *in, int *ou,int *prevblck);
extern void mp3_imdct_longblk_dual_vect(int *Input, int* Win_tab, int *Output, int *prevblk);

#else

extern void mp3_imdct_shortblk_arm(int *in , int *out ,int *prevblck_in); 
extern void mp3_imdct_longblk_arm(int *Input , int *Win_tab,int *Output , int *prevblck );
extern void g9PointIIIDCT(Word32 *X, Word32 *Y);
extern void mp3hybrid_invert_subband_samples_arm(int *bandPtr, unsigned int limit1, unsigned int limit2);

#endif /* NEON */

#endif /* ARM */

extern void inv_mdct(Float *in,Float *out, int block_type);


#endif /* Do not edit below this line */

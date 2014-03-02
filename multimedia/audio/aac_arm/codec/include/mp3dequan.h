
#ifndef _mp3dequan_h_
#define _mp3dequan_h_

#include "audiolibs_common.h"


#define MP3DEQUAN_TABLE_MEM
#define MP3DEQUAN_MEM

#define MP3_SCALING_FACTOR 21
#define MAX_BINS_PER_SFB 100


extern int const MP3DEQUAN_TABLE_MEM mp3dequan_tab_pow2[4];
int MP3DEQUAN_TABLE_MEM  * fn_mp3dequan_tab_pow2(void);

#ifndef _NMF_MPC_

/* entry points */
#ifdef MMDSP
extern void
mp3_loop_dequan(Float *mant, int width,int offset );

extern void
aac_loop_dequan(int *mant, int *scale,int width,int offset);

extern void
aac_loop_dequan_ms(int *mant, int *scale,
		   int *mant_r, int *scale_r,
		   int width,
		   int offset, int offset_r );

extern void
aac_loop_dequan_is(int *mant, int *scale,
		   int *mant_r, int *scale_r,
		   int width,
		   int offset, int offset_r,
		   int sign_sfb);
		   
extern void
aac_normalize_data(int *coef, int *exponent, int width, int max_scale);

#endif // MMDSP
		   
#endif // _NMF_MPC_
		   
#ifdef ARM
extern void mp3_loop_dequan_big_arm(int *mant, int width, int offset);
extern void mp3_loop_dequan_arm(int *mant, int width, int offset);
extern void mp3_loop_dequan_arm(int *mant, int width,int offset );
#endif /* ARM */

#ifndef MMDSP
extern void         mp3dequan(int index, Float *mant, int *scale);
extern float        exptable_plus[201];
extern float        tab_intensity[121];
#endif /* MMDSP */

#endif /* Do not edit below this line */


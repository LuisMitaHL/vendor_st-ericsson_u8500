
 

#ifndef _mp3hybrid_common_h_
#define _mp3hybrid_common_h_

#define MP3_MAX_ALIAS 8

extern void
mp3hybrid_invert_subband_samples(Float *bandPtr, unsigned int limit1, unsigned int limit2);

extern void
mp3hybrid_antialias(Float *in, int sblim,Float const YMEM *hybrid_ca);

#ifdef ARM

#ifdef NEON
extern void
mp3hybrid_antialias_neon(int *in, int sblim,int const YMEM *hybrid_ca);

#elif (defined VECT)

extern void
mp3hybrid_antialias_vect(int *in, int sblim,int const YMEM *hybrid_ca);

#else

extern void
mp3hybrid_antialias_arm(int *in, int sblim,int const YMEM *hybrid_ca);

#endif
#endif /* ARM */

extern const Float YMEM * AUDIOLIBS_UPLOAD_MEM mp3_hybrid_win[4];

#endif /* Do not edit below this line */

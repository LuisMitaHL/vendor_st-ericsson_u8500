
 

#ifndef _mp3hybrid_h_
#define _mp3hybrid_h_

#include "audiolibs_common.h"

#ifdef __flexcc2__
#define MP3HYBRID_MEM  YMEM
#else // __flexcc2__ 
#define MP3HYBRID_MEM
#endif // __flexcc2__

#define MP3_SHIFT_LEFT_FB_OUTPUT   2

#ifndef _NMF_MPC_ 

#ifdef ARM 
extern void mp3hybrid_antialias_arm(int *in, int sblim,int const YMEM *hybrid_ca);




#ifdef NEON
extern void
MP3_hybrid_filterbank_neon(int *in , int *prevblck ,
                      int *polysyn_fifo ,int *polysyn_offset ,
                      int sblim, int block_type, int maxsb,
                      int *scratch ,  int scratch_len,
                      int headroom);

#elif (defined VECT)
extern void
MP3_hybrid_filterbank_vect(int *in , int *prevblck ,
                      int *polysyn_fifo ,int *polysyn_offset ,
                      int sblim, int block_type, int maxsb,
                      int *scratch ,  int scratch_len,
                      int headroom);

#else // NEON || VECT

extern void
MP3_hybrid_filterbank_arm(int *in , int *prevblck ,
                      int *polysyn_fifo ,int *polysyn_offset ,
                      int sblim, int block_type, int maxsb,
                      int *scratch ,  int scratch_len,
                      int headroom);
#endif // NEON

extern void
MP3_hybrid_filterbank_init_arm(int *prevblck,
                           int **polysyn_fifo, int *polysyn_offset,
                           int num_ch);

#endif // ARM
extern void 
MP3_hybrid_filterbank_init(Float *prevblck,
			   Float **polysyn_fifo, int *polysyn_offset,
			   int num_ch);

extern void
MP3_hybrid_filterbank(Float *in, Float *prevblck,
		      Float *polysyn_fifo, int *polysyn_offset,
                        
		      int sblim, int block_type,int maxsb,
		      Float *scratch, int scratch_len,
			  int headroom);

#endif // _NMF_MPC_

#endif // _mp3hybrid_h_

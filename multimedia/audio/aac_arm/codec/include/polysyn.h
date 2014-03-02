
#ifndef _polysyn_h_
#define _polysyn_h_

#include "audiolibs_common.h"

#ifdef __flexcc2__
#define POLYSYN_MEM  YMEM
#else // __flexcc2__
#define POLYSYN_MEM
#endif

#define POLYSYN_FIFO_SIZE 512

#ifndef _NMF_MPC_
#ifdef ARM
extern void init_polysyn_arm(int *buf,int *bufOffset);

#ifdef NEON
extern void polysyn_neon(int *bandPtr, int *samples,
		    int *buf,int *bufOffset,
		    int nb_iter,int reverse, int shift);
#elif (defined VECT)
extern void polysyn_vect(int *bandPtr, int *samples,
		    int *buf,int *bufOffset,
		    int nb_iter,int reverse, int shift);

#else // NEON || VECT

extern void polysyn_arm(int *bandPtr, int *samples,
		    int *buf,int *bufOffset,
		    int nb_iter,int reverse, int shift);
#endif  // NEON || VECT

#endif /* ARM */

extern void init_polysyn(Float *buf,int *bufOffset);
extern void polysyn(Float *bandPtr, Float *samples,
		    Float *buf,int *bufOffset,
		    int nb_iter,int reverse, int shift);

#endif //#ifndef _NMF_MPC_


#ifdef ARM
extern const int HAN_WINDOW[264];
extern const int polysyn_ctab[4];
extern const int polysyn_ctab1[4];
extern const int polysyn_ctab2[8];
extern const int polysyn_ctab3[16];
#endif


#endif /* Do not edit below this line */

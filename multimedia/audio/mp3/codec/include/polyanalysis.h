
 

#ifndef _polyanalysis_h_
#define _polyanalysis_h_

#include "audiolibs_common.h"
#include "vector.h"

#define POLYANA_WINDOW_SIZE 512

struct POLYANALYSIS_CONTEXT {
    Float  polyBuffer[POLYANA_WINDOW_SIZE];
    int    curNdx;
} ;

#ifdef __flexcc2__ 
#define POLYANALYSIS_MEM  YMEM
#else // __flexcc2__
#define POLYANALYSIS_MEM
#endif // __fleacc2__

/* To avoid saturations the output is scaled to the right after the
   windowing, as the modulation may induce 5-bit excursion */

#define POLYANA_WINDOWING_LEFT_SHIFT   3   /* normalization of window coefficients */
#define POLYANA_MODULATION_RIGHT_SHIFT 5   /* Compensation for excursion */

#define POLYANALYSIS_RIGHT_SHIFT  (POLYANA_MODULATION_RIGHT_SHIFT-POLYANA_WINDOWING_LEFT_SHIFT)

#if !defined(MMDSP) && (POLYANALYSIS_RIGHT_SHIFT<0)
#error "Saturations may occur in polyanalysis library, check scaling"
#endif


extern CPPCALL void polyanalysis_init(struct POLYANALYSIS_CONTEXT *hPolyanalyis);
extern CPPCALL void polyanalysis(Float *buffer, int istride,
								 struct POLYANALYSIS_CONTEXT *hPolyanalyis,
								 Float *subband,
								 int iteration);
extern CPPCALL void polyanalysis_scaling(Float *buffer, int istride,
										 struct POLYANALYSIS_CONTEXT *hPolyanalyis,
										 Float *subband,
										 int   *scale,
										 int iteration);

extern CPPCALL CODEC_INIT_T polyanalysis_open(void);
extern CPPCALL void         polyanalysis_close(void);

#endif // _polyanalysis_h_  

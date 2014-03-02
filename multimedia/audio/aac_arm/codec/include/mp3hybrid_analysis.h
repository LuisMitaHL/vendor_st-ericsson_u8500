
#ifndef _mp3hybrid_analysis_h_
#define _mp3hybrid_analysis_h_

#include "audiolibs_common.h"
#include "mp3hybrid.h"
#include "mp3hybrid_common.h"
#include "polyanalysis.h"
#include "vector.h"

/* MDCT sum is 2.61 -> divide by 4 to avoid saturations */
#define MP3_HYBRID_MDCT_RIGHT_SHIFT 2

#define MP3_HYBRID_RIGHT_SHIFT  (MP3_HYBRID_MDCT_RIGHT_SHIFT+POLYANALYSIS_RIGHT_SHIFT)

extern CPPCALL int
MP3_hybrid_analysis(Float *sbsamples, Float *sb_prevblck,
					Float *out, int maxsb, int block_type);

extern CPPCALL int
MP3_hybrid_analysis_scaling(Float *sbsamples, Float *sb_prevblck,
							int *sbscale,int common_scale,
							Float *out, int maxsb, int block_type);
extern CPPCALL int
MP3_hybrid_calc_common_scale(int *sbscale);


#endif /* Do not edit below this line */

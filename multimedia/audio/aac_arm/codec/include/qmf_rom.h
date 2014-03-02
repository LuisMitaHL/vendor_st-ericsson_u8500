
#ifndef _QMF_ROM_H_
#define _QMF_ROM_H_

#include "audiolibs_common.h"

#define MAX_NUM_CHANNELS 1

#ifdef __flexcc2__
#define QMFMEM_OPT   __EXTERN   
#define QMFMEM_NOPT  __EXTERN   
#else
#define QMFMEM_OPT
#define QMFMEM_NOPT
#endif

/* optimized memory*/
extern unsigned int QMFMEM_OPT qmf_ref_count;
extern const YMEM Float * QMFMEM_OPT qmf_p_filter;
extern const YMEM Float sbr_qmf_64_640[325];
extern const QMFMEM_OPT Float qmf_pre_twiddle_L32[32];
extern const QMFMEM_OPT Float qmf_pre_twiddle_L64[64];
extern const QMFMEM_OPT Float qmf_post_twiddle_L32[32];
extern const QMFMEM_OPT Float qmf_post_twiddle_L64[64];

extern const QMFMEM_OPT  Float post_mod_twiddle_L32[64];
extern const QMFMEM_OPT  Float post_mod_twiddle_L64[128];

/* non-optimized memory */
extern const QMFMEM_NOPT Float sbrDecoderFilterbankCoefficients[640];

extern const QMFMEM_NOPT Float qmf_ana_cos_tab_32[2048];
extern const QMFMEM_NOPT Float qmf_ana_sin_tab_32[2048];
extern const QMFMEM_NOPT Float qmf_ana_cos_tab_64[8192];
extern const QMFMEM_NOPT Float qmf_ana_sin_tab_64[8192];

extern const QMFMEM_NOPT Float qmf_syn_cos_tab_32[32*64];
extern const QMFMEM_NOPT Float qmf_syn_sin_tab_32[32*64];
extern const QMFMEM_NOPT Float qmf_syn_cos_tab_64[64*128];
extern const QMFMEM_NOPT Float qmf_syn_sin_tab_64[64*128];

#ifndef PI
#define PI 3.1415926536
#endif /* PI */


#endif

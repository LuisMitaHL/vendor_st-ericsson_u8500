/*
  Complex qmf analysis/synthesis
*/
#ifndef __QMF_DEC_H
#define __QMF_DEC_H

#include "sbrdecsettings.h"
#include "ps_dec.h"


struct dct4Twiddle
{
  const Float *cos_twiddle;
  const Float *sin_twiddle;
  const Float *alt_sin_twiddle;
};

typedef struct
{
  int no_channels;

#if 0
  const Float *p_filter;

  struct dct4Twiddle *pDct4Twiddle;

#ifndef LP_SBR_ONLY

  const Float *cos_twiddle;
  const Float *sin_twiddle;
  const Float *alt_sin_twiddle;

  const Float *t_cos;
  const Float *t_sin;

  int qmf_filter_state_size;
#endif
#endif
    
  Float *FilterStatesAna;
  Float *FilterStatesSyn;

  int no_col;

  int lsb;
  int usb;



}
SBR_QMF_FILTER_BANK;

typedef SBR_QMF_FILTER_BANK *HANDLE_SBR_QMF_FILTER_BANK;

void
cplxAnalysisQmfFiltering (
                          const Float *timeIn,
                          Float **qmfReal,
#ifndef LP_SBR_ONLY
                          Float **qmfImag,
#endif
                          HANDLE_SBR_QMF_FILTER_BANK qmfBank,
                          int   bUseLP);

void
cplxSynthesisQmfFiltering (Float **qmfReal,
#ifndef LP_SBR_ONLY
                           Float **qmfImag,
#endif
                           Float *timeOut,
                           HANDLE_SBR_QMF_FILTER_BANK qmfBank,
                           int   bUseLP,
                           HANDLE_PS_DEC ps_d,
                           int   active
                           );


int
createCplxAnalysisQmfBank (HANDLE_SBR_QMF_FILTER_BANK h_sbrQmf,
                           int noCols,
                           int lsb,
                           int usb,
                           int chan);

int
createCplxSynthesisQmfBank (HANDLE_SBR_QMF_FILTER_BANK h_sbrQmf,
                            int noCols,
                            int lsb,
                            int usb,
                            int chan,
                            int bDownSample);
#endif

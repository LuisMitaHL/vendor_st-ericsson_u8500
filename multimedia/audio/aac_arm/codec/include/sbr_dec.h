/*
  Sbr decoder
*/
#ifndef __SBR_DEC_H
#define __SBR_DEC_H

#include "sbr_const.h"
#include "lpp_tran.h"
#include "qmf_dec.h"
#include "env_calc.h"
#include "qmf.h"


typedef struct
{
  Float * ptrsbr_OverlapBuffer;

  SBR_QMF_FILTER_BANK     CodecQmfBank;
  SBR_QMF_FILTER_BANK     SynthesisQmfBank;
  SBR_CALCULATE_ENVELOPE  SbrCalculateEnvelope;
  SBR_LPP_TRANS           LppTrans;

  unsigned char qmfLpChannel;
  unsigned char bApplyQmfLp;

  SBR_SCALING SbrScaling;

}
SBR_DEC;

typedef SBR_DEC *HANDLE_SBR_DEC;


typedef struct
{
  HANDLE_SBR_PREV_FRAME_DATA hPrevFrameData;
  SBR_DEC SbrDec;
#ifdef SBR_MC
  HANDLE_SBR_FRAME_DATA hFrameDataLeft;
  HANDLE_SBR_FRAME_DATA hFrameDataRight;
#endif
}
SBR_CHANNEL;

 


typedef struct
{
  SBRBITSTREAM * Bitstream;
  int          FrameOk;
}
SBR_CONCEAL_DATA;

typedef SBR_CONCEAL_DATA *HANDLE_SBR_CONCEAL_DATA;

void sbr_dec (HANDLE_SBR_DEC hSbrDec,
              Float *timeIn,
              Float *timeOut,
              Float *scratch_ptr,
              Float YMEM *scratch_ptr_y,
              Float *InterimResult,
              HANDLE_SBR_HEADER_DATA hHeaderData,
              HANDLE_SBR_FRAME_DATA hFrameData,
              HANDLE_SBR_PREV_FRAME_DATA hPrevFrameData,
              int applyProcessing,
              struct PS_DEC *h_ps_d,
              HANDLE_SBR_DEC hSbrDecRight,
              int nChannels,
			  int *bUseLP_config);

int
createSbrDec (SBR_CHANNEL * hSbrChannel,
              HANDLE_SBR_HEADER_DATA hHeaderData,
              int chan,
              int bApplyQmfLp,
              int sampleFreq);
int
createSbrQMF (SBR_CHANNEL * hSbrChannel,
              HANDLE_SBR_HEADER_DATA hHeaderData,
              int chan,
              int bDownSample);

int
resetSbrQMF (HANDLE_SBR_DEC hSbrDec,
             HANDLE_SBR_HEADER_DATA hHeaderData,
             int sbrChannel,
             int nChannels,
             HANDLE_SBR_PREV_FRAME_DATA hPrevFrameData,
			 int bUseLP);

#endif

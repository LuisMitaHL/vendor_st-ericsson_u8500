/*
  Memory layout
*/
#ifndef __SBR_RAM_H
#define __SBR_RAM_H

#include "sbrdecsettings.h" /* for MAXNRSBRCHANNELS */
#include "sbrdecoder.h"
#include "env_extr.h"

/* extern Float EXTERN qmfRealTmp[32][NO_ACTUAL_SYNTHESIS_CHANNELS]; */
/* extern Float EXTERN qmfImagTmp[32][NO_ACTUAL_SYNTHESIS_CHANNELS]; */


//extern  Float  sbr_CodecQmfStatesAnalysis[];
//extern  Float  sbr_QmfStatesSynthesis[];

//extern  Float sbr_GainSmooth[MAXNRSBRCHANNELS][MAX_FREQ_COEFFS];
//extern  Float sbr_NoiseSmooth[MAXNRSBRCHANNELS][MAX_FREQ_COEFFS];
//extern  char   sbr_GainSmooth_e[MAXNRSBRCHANNELS][MAX_FREQ_COEFFS];

//extern FREQ_BAND_DATA FreqBandData;

/* #ifdef LP_SBR_ONLY */
/* extern Float   sbr_OverlapBuffer[MAXNRQMFCHANNELS][MAX_OV_COLS * NO_SYNTHESIS_CHANNELS]; */
/* #else */
/* extern Float   sbr_OverlapBuffer[MAXNRQMFCHANNELS][2 * MAX_OV_COLS * NO_SYNTHESIS_CHANNELS]; */
/* #endif */

extern Float EXTERN  sbr_LpcFilterStatesReal[MAXNRQMFCHANNELS][LPC_ORDER][NO_ANALYSIS_CHANNELS];
#ifndef LP_SBR_ONLY
extern Float EXTERN  sbr_LpcFilterStatesImag[MAXNRQMFCHANNELS][LPC_ORDER][NO_ANALYSIS_CHANNELS];
#endif

//extern TRANSPOSER_SETTINGS sbr_TransposerSettings;

//extern SBR_PREV_FRAME_DATA PrevFrameData[MAXNRSBRCHANNELS];

//extern SBRBITSTREAM sbr_PrevBitstream;


#ifdef LP_SBR_ONLY
extern Float   *InterimResult;
#else
//extern Float   *WorkBuffer2;
//extern Float   InterimResult[MAX_FRAME_SIZE];
#endif

#endif


/*
  parametric stereo decoding structs and prototypes
*/
#ifndef __PS_DEC_H
#define __PS_DEC_H

#include "sbr_bitb.h"
#ifndef ARM   //To suppress coverity error of self inclusion header.
#include "sbrdecoder.h"
#endif 
#include "hybrid.h"
#include "sbrdecsettings.h"
#include "qmf.h"

#define NO_SUB_QMF_CHANNELS         12
#define NO_QMF_CHANNELS_IN_HYBRID   3
#define NO_QMF_CHANNELS             64
#define NO_ALLPASS_CHANNELS         23
#define NO_DELAY_CHANNELS           (NO_QMF_CHANNELS-NO_ALLPASS_CHANNELS)
#define DELAY_ALLPASS               2
#define SHORT_DELAY_START           12
#define SHORT_DELAY                 1
#define LONG_DELAY                  14
#define NO_QMF_ALLPASS_CHANNELS    (NO_ALLPASS_CHANNELS-NO_QMF_CHANNELS_IN_HYBRID)
#define NO_QMF_ICC_CHANNELS        (NO_QMF_ALLPASS_CHANNELS+NO_DELAY_CHANNELS)
#define HYBRIDGROUPS                8
#define DECAY_CUTOFF                3
#define DECAY_SLOPE                 FORMAT_FLOAT(0.05f,MAXVAL)    /* value optimised for precision */
#define PEAK_DECAY_FACTOR           FORMAT_FLOAT(0.765928338364649f,MAXVAL)
#define NO_SERIAL_ALLPASS_LINKS     3
#define MAX_NO_PS_ENV               5

#ifdef MMDSP 
#define PI                          FORMAT_FLOAT( 3.1415926535897932/4,MAXVAL);        /* divided by 4 */
#define PSC_SQRT2                   FORMAT_FLOAT( 1.41421356237309504880/2,MAXVAL )    /* divided by 2 */
#define PSC_PIF                         ( ( Float )PI )                                /* divided by 4 */
#define PSC_2PIF                        ( ( Float )( PI ) )                            /* divided by 8 */
#define PSC_PI2F                        ( ( Float )( PI ) )                            /* divided by 2 */
#define PSC_SQRT2F                      ( ( Float )PSC_SQRT2 )                         /* divided by 2 */
#define INV_PSC_SQRT2F             FORMAT_FLOAT(1/1.41421356237309504880,MAXVAL )      /* not divided */

#else
#define PI                          3.1415926535897932
#define PSC_SQRT2                       ( 1.41421356237309504880 )
#define PSC_PIF                         ( ( Float )PI )
#define PSC_2PIF                        ( ( Float )( 2 * PI ) )
#define PSC_PI2F                        ( ( Float )( PI / 2 ) )
#define PSC_SQRT2F                      ( ( Float )PSC_SQRT2 )
#endif /* MMDSP */

#define NEGATE_IPD_MASK                 ( 0x00001000 )
#define NRG_INT_COEFF                    0.25f
#define NO_BINS                         ( 20 )
#define NO_HI_RES_BINS                  ( 34 )
#define NO_LOW_RES_BINS                 ( NO_IID_BINS / 2 )
#define NO_IID_BINS                     ( NO_BINS )
#define NO_ICC_BINS                     ( NO_BINS )
#define NO_LOW_RES_IID_BINS             ( NO_LOW_RES_BINS )
#define NO_LOW_RES_ICC_BINS             ( NO_LOW_RES_BINS )
#define SUBQMF_GROUPS                   ( 10 )
#define QMF_GROUPS                      ( 12 )
#define NO_IID_GROUPS                   ( SUBQMF_GROUPS + QMF_GROUPS )
#define NO_IID_STEPS                    ( 7 )
#define NO_IID_STEPS_FINE               ( 15 )
#define NO_ICC_STEPS                    ( 8 )
#define NO_IID_LEVELS                   ( 2 * NO_IID_STEPS + 1 )
#define NO_IID_LEVELS_FINE              ( 2 * NO_IID_STEPS_FINE + 1 )
#define NO_ICC_LEVELS                   ( NO_ICC_STEPS )

/****************************************************************
  Type definitions
 ****************************************************************/
struct PS_DEC {
  int bForceMono;
  unsigned int noSubSamples;
  Float invNoSubSamples;

  int delay_buffer_scale;
  int usb;
  int lastUsb;

  int bPsDataAvail;

  int bEnableIid;
  int bEnableIcc;

  int bEnableExt;
  int bFineIidQ;
  int aIidPrevFrameIndex[NO_HI_RES_BINS];						/* 34 */
  int aIccPrevFrameIndex[NO_HI_RES_BINS];						/* 34 */
  int aaIidIndex[MAX_NO_PS_ENV+1][NO_HI_RES_BINS];				/* 6x34 = 204 */
  int aaIccIndex[MAX_NO_PS_ENV+1][NO_HI_RES_BINS];				/* 6x34 = 204 */

  int freqResIid;
  int freqResIcc;

  int bFrameClass;
  int noEnv;
  int aEnvStartStop[MAX_NO_PS_ENV+1];							/* 5+1 */				

  int abIidDtFlag[MAX_NO_PS_ENV];								/* 5 */
  int abIccDtFlag[MAX_NO_PS_ENV];								/* 5 */

  int   delayBufIndex;
  Float **aaRealDelayBufferQmf;
  Float **aaImagDelayBufferQmf;

  int aDelayRBufIndexSer[NO_SERIAL_ALLPASS_LINKS];				/* 3 */
  int aNoSampleDelayRSer[NO_SERIAL_ALLPASS_LINKS];				/* 3 */

  Float **aaaRealDelayRBufferSerQmf[NO_SERIAL_ALLPASS_LINKS];	/* 3 */
  Float **aaaImagDelayRBufferSerQmf[NO_SERIAL_ALLPASS_LINKS];	/* 3 */

  Float **aaaRealDelayRBufferSerSubQmf[NO_SERIAL_ALLPASS_LINKS];/* 3 */
  Float **aaaImagDelayRBufferSerSubQmf[NO_SERIAL_ALLPASS_LINKS];/* 3 */

  Float **aaRealDelayBufferSubQmf;
  Float **aaImagDelayBufferSubQmf;

  Float *aPeakDecayFast;
  Float *aPrevNrg;
  Float *aPrevPeakDiff;
#ifdef MMDSP
  Float *aPeakDecayFastScale;
  Float *aPrevNrgScale;
  Float *aPrevPeakDiffScale;
#endif
  Float *mHybridRealLeft;
  Float *mHybridImagLeft;
  Float *mHybridRealRight;
  Float *mHybridImagRight;

  HANDLE_HYBRID hHybrid;

  int aDelayBufIndex[NO_DELAY_CHANNELS];						/* 41 */
  int aNoSampleDelay[NO_DELAY_CHANNELS];						/* 41 */

  Float h11Prev[NO_IID_GROUPS];									/* 22 */
  Float h12Prev[NO_IID_GROUPS];
  Float h21Prev[NO_IID_GROUPS];
  Float h22Prev[NO_IID_GROUPS];

  Float H11[NO_IID_GROUPS];
  Float H12[NO_IID_GROUPS];
  Float H21[NO_IID_GROUPS];
  Float H22[NO_IID_GROUPS];

  Float deltaH11[NO_IID_GROUPS];
  Float deltaH12[NO_IID_GROUPS];
  Float deltaH21[NO_IID_GROUPS];
  Float deltaH22[NO_IID_GROUPS];
    
    Float *ps_scratch1;
    Float *ps_scratch2;
    Float *ps_scratch3;
    Float *ps_scratch4;
    Float *ps_scratch5;
    Float *ps_scratch6;
    Float *ps_scratch7;
/* #ifdef MMDSP */
/*     Float *ps_scratch5; */
/* #endif */
};

typedef struct PS_DEC *HANDLE_PS_DEC;

/****************************************************************
  Functions
 ****************************************************************/
void
InitPsScale(HANDLE_PS_DEC ps_dec,             
            SBR_SCALING  *sbrScaleFactor);
  
int
CreatePsDec(HANDLE_PS_DEC h_ps_dec,
            unsigned int noSubSamples);

void
InitRotationEnvelope(HANDLE_PS_DEC h_ps_dec,
                     int env,
                     int usb);
void
ApplyPsSlot(HANDLE_PS_DEC h_ps_dec,
            Float **rBufferLeft,
            Float **iBufferLeft,
            Float *rBufferRight,
            Float *iBufferRight
#ifdef MMDSP
            ,int lsb, 
            int slot,
            SBR_SCALING *sbr_scaling
#endif
            );
#endif

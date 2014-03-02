/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   spl_algo.h
 * \brief  
 * \author ST-Ericsson
 */
/****************************************************************/
/* FILE: spl_algo.h                                             */
/* AUTHOR: ZONG Wenbo, DURNERIN Matthieu                        */
/* EMAIL: wenbo.zong@st.com                                     */
/* DIV: 3GP/3GM/PSA/UCAA                                        */
/* DATE: 2008 May 16                                            */
/* LAST REV: 2010 March 26                                      */
/****************************************************************/
/* GOAL: defines constants, macros, and local structure,        */
/*       and includes function prototypes                       */
/****************************************************************/
#ifndef _spl_algo_h_
#define _spl_algo_h_

#include "audiolibs_common.h"

/*
 * Sample width is dependent on the platform. On MMDSP, use 24. On CA9, use 32
 */
#define SAMPLE_NBITS 24 // sample bit size, {8, 16, 24, 32}

#ifdef __flexcc2__
#define MMshort short
#define MMlong long
#define MMlonglong long long
#else 
#ifndef MMDSP
#define MMshort short
#define MMlong long
#define MMlonglong long long
#endif
#endif

//#define SPL_DEBUG // to print debugging info 

#define SPL_MAIN_CH_MAX 2 // L, R

// limiter settings
#define SPL_RMS_MEASURE                1            // RMS measure (1) or Peak measure (0)
#define SPL_PEAK_MEASURE               0            // RMS measure (1) or Peak measure (0)
#define SPL_BLOCK_SIZE_DEFAULT         48           // logical block size, 0.25 ~ 1 ms
#define SPL_BLOCK_SIZE_PEAK_DEFAULT    16           // logical block size
#define SPL_BLOCK_SIZE_MIN             1            // logical block size min
#define SPL_BLOCK_SIZE_MAX             96           // logical block size max, 0.25 ~ 1 ms
#define SPL_LOOKAHEAD_LEN_DEFAULT      144          // 3 msec, must be longer than the default attack time
#define SPL_LOOKAHEAD_LEN_PEAK_DEFAULT 48           // 1 msec, must be longer than the default attack time
#define SPL_LOOKAHEAD_LEN_MIN          0            // min value
#define SPL_LOOKAHEAD_LEN_MAX          288          // 5 msec, must be longer than the default attack time
#define SPL_GAIN_ATTACK_TIME_DEFAULT   1000         // in microseconds
#define SPL_GAIN_ATTACK_TIME_PEAK_DEFAULT 400000    // in microseconds
#define SPL_THRESH_ATTACK_TIME_PEAK_DEFAULT 1000    // in microseconds
#define SPL_GAIN_ATTACK_TIME_MIN       0            // in microseconds
#define SPL_GAIN_ATTACK_TIME_MAX       5000000      // in microseconds
#define SPL_GAIN_RELEASE_TIME_DEFAULT  100000       // in microseconds
#define SPL_GAIN_RELEASE_TIME_PEAK_DEFAULT  100000  // in microseconds
#define SPL_THRESH_RELEASE_TIME_PEAK_DEFAULT 100000 // in microseconds
#define SPL_GAIN_RELEASE_TIME_MIN      0            // in microseconds
#define SPL_GAIN_RELEASE_TIME_MAX      5000000      // in microseconds
#define SPL_GAIN_HOLD_TIME_DEFAULT     40000        // in microseconds
#define SPL_GAIN_HOLD_TIME_PEAK_DEFAULT 40000       // in microseconds
#define SPL_THRESH_HOLD_TIME_PEAK_DEFAULT 500       // in microseconds
#define SPL_GAIN_HOLD_TIME_MIN         0            // in microseconds
#define SPL_GAIN_HOLD_TIME_MAX         5000000      // in microseconds
#define SPL_POWER_ATTACK_TIME_DEFAULT  1200000      // 1.2 second in microseconds
#define SPL_POWER_ATTACK_TIME_PEAK_DEFAULT  40      // in microseconds
#define SPL_POWER_ATTACK_TIME_MIN      0            // in microseconds
#define SPL_POWER_ATTACK_TIME_MAX      5000000      // in microseconds
#define SPL_POWER_RELEASE_TIME_DEFAULT 800000       // 0.8 second in microseconds
#define SPL_POWER_RELEASE_TIME_PEAK_DEFAULT 100000  // in microseconds
#define SPL_POWER_RELEASE_TIME_MIN     0            // in microseconds
#define SPL_POWER_RELEASE_TIME_MAX     5000000      // in microseconds
#define SPL_USERGAIN_DEFAULT           (0)          // in millibel
#define SPL_USERGAIN_MIN               (-12000)     // in millibel
#define SPL_USERGAIN_MAX               (12000)      // in millibelfor peak limiter
#define SPL_THRESHOLD_DEFAULT          (0)          // in millibel
#define SPL_THRESHOLD_MIN              (-12000)     // in millibel
#define SPL_THRESHOLD_MAX              (0)          // in millebel
#define SPL_PEAKL_GAIN_DEFAULT         (0)          // in millebel
#define SPL_PEAKL_GAIN_MIN             (-12000)     // in millebel
#define SPL_PEAKL_GAIN_MAX             (12000)      // in millebel
#define SPL_ENABLE_COUNTER_MAX          750   // 750msec for disable wait at fs=48000 and block size of 48
#define SPL_THRESHOLD_INCR              4     // 3000 msec for threshold modif of full scale -12000 at fs=48000 and block size of 48 

#define SPL_POWER_DB_MINUS_INF  (-6*(SAMPLE_NBITS-1))  // taken as minus infinity when computing power
#define SPL_ALMOST_ZERO_THRESH  1

#define Q23 8388607
#define Q23_ONE_HUNDREDTH 83886
#define Q23_ONE_TENTH     838861
#define Q23_FiveOverEight 5242880
#define Q31 2147483648LL
#define Q47 140737488355327LL

// math macros
#define MIN(A, B) ((A) > (B) ? (B) : (A))
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define SAT_16(a) ( (a) >= 32767 ? 32767 : ((a) < -32768 ? -32768 : (a)))
#define spl_mul48x24(a,b)  wL_addsat(wL_fmul(b, wround_L(a)), wfmul(b, wextract_l(a)))
        
/* CIRCULAR ADDRESSING MACROS */
//-----------------------------------------------------//
//! Circular Addressing with higher boudary check
#define CIRC_ADD_H(X,R)  ( (X) >= (R) ? ((X) - (R)) :  (X) )
//-----------------------------------------------------//

typedef enum {
    SPL_SOUND_PRESSURE_LIMITER = 0,
    SPL_PEAK_LIMITER
} SPL_LIMIT_TYPE;

typedef enum {
    SPL_LIMITATION = 0,
    SPL_NORMALIZATION
} SPL_LIMIT_MODE;

typedef enum {
    SPL_NO_CLIPPING = 0,
    SPL_SOFT_CLIPPING,
    SPL_HARD_CLIPPING
} SPL_PEAK_CLIPPING_MODE;

typedef struct {
    int SampleFreq;    /* Sample rate */
    int NumberChannel; /* Number of channel in input (same as output) */
} SplStream_t;

typedef struct {
    int Type; /* type of limiter sound pressure or peak */
} SplParam_t;

typedef struct {
    unsigned int PeakClippingMode; /* Clipping mode for last limiter stage in Peak limiter mode */
    unsigned int PowerAttackTime;  /* Attack time constant to measure RMS power level, microsecond [1000, 5000000] */
    unsigned int PowerReleaseTime; /* Release time constant to measure RMS power level, microsecond [1000, 5000000] */
    unsigned int GainAttackTime;   /* Attack time constant to compute gain to apply to signal, microsecond [1000, 5000000] */
    unsigned int GainReleaseTime;  /* Release time constant to compute gain to apply to signal, microsecond [1000, 5000000] */
    unsigned int GainHoldTime;     /* Hold time constant to compute gain to apply to signal, microsecond [1000, 5000000] */
    unsigned int ThreshAttackTime; /* Attack time constant to compute gain to limit to threshold, microsecond [1000, 5000000] */
    unsigned int ThreshReleaseTime;/* Release time constant to compute gain to limit to threshold, microsecond [1000, 5000000] */
    unsigned int ThreshHoldTime;   /* Hold time constant to compute gain to limit to threshold, microsecond [1000, 5000000] */
    unsigned int BlockSize;        /* Internal block size [1, 1024] */
    unsigned int LookAheadSize;    /* Internal lookahead buffer size [1, 8192] */
} SplTuning_t;

typedef struct {
    int Enable;       /* enable/disable SPL */
    int Threshold;    /* reference threshold dBFS Peak in millibel, i.e. 1/100 dB [-9000, 0] */
    int SPL_Mode;     /* Limitation or Normalization mode for SPL, not available for now */
    int SPL_UserGain; /* user gain (attenuation) applied after SPL dBFS Peak in millibel, i.e. 1/100 dB [-9000, 0] */
    int PeakL_Gain;   /* Peak Limiter gain to be applied to the signal */
} SplConfig_t;

/*
 * local heap structure
 */
typedef struct
{
    int SampleFreq;
    int NumberChannel;
    int DataInOffset;
    int DataOutOffset;
    MMshort *SysMainInPtr[SPL_MAIN_CH_MAX];
    MMshort *SysMainOutPtr[SPL_MAIN_CH_MAX];

    // IO related
    int RmsMeasure;
    int ConsumedSamples;
    int ValidSamples;
    MMshort MainInBuf[SPL_MAIN_CH_MAX][SPL_BLOCK_SIZE_MAX];
    MMshort MainOutBuf[SPL_MAIN_CH_MAX][SPL_BLOCK_SIZE_MAX*2]; /* x2 to accomodate mismatch b/w burst processing rate and outputting */
    int InBufSamp; // # of samples in the input buffer
    int OutRdIdx;  // index to the reading location of the output buffer
    int OutWtIdx;  // index to the writing location of the output buffer

    /* Parameters */
    int Type; /* Static parameter, Sound Pressure Limiter or Peak Limiter type */
    SplTuning_t Tuning;
    SplConfig_t Config;
    int PeakClipping; /* Peak Limiter clipping mode */

    MMshort LimiterLABuf [SPL_MAIN_CH_MAX][SPL_LOOKAHEAD_LEN_MAX]; /* buffer to subband lookahead */
    int LimiterLaIdx;
    MMshort PowerAttackCoef; // attack coef for power estimation
    MMshort PowerReleaseCoef;
    MMshort GainAttackCoef;  // attack coef for limiter
    MMshort GainReleaseCoef;
    MMshort ThreshAttackCoef;  // attack coef for limiter
    MMshort ThreshReleaseCoef;
    MMshort ThresholddB;
    MMshort ThresholdExp;
    MMshort ThresholdMant;
    MMshort ThresholdInvExp;
    MMshort ThresholdInvMant;
    MMshort Threshold;
    MMshort TargetThreshold;
    MMshort CurrentThreshold;
    MMshort UpdateThreshold;
    MMshort PeakL_GaindB;
    MMshort AppliedGaindB;
    Word48 PowerMeas[SPL_MAIN_CH_MAX]; // last peak
    MMshort GainMant;    // last gain applied
    MMshort GainExp;     // last gain applied
    int GainHoldRem;
    int GainHtSamp;
    int ThreshHtSamp;

    int MeanGain;
    int NbGain;
    int Enable_counter;
    int Enable_intern;
} SPL_LOCAL_STRUCT_T;


/*
 * functions
 */
extern void spl_main(MMshort *inbuf, MMshort *outbuf, short size, SPL_LOCAL_STRUCT_T *Heap);
/* init:                                     */
/* if pParam  = NULL, use default parameters */
/* if pStream = NULL, use default parameters */
/* if pTuning = NULL, use default parameters */
/* return <0 if error in the parameters      */
/* but value are rounded, could keep on      */
extern int spl_init(SPL_LOCAL_STRUCT_T * Heap, SplParam_t *pParam, SplStream_t *pStream, SplTuning_t *pTuning);
extern void spl_reset(SPL_LOCAL_STRUCT_T * Heap);
/* spl_set_parameter should be used if static parameters have to be set after */
/* the init. If applied before the spl_init function, be sure to use the same */
/* parameters in the init function to avoid reinitializing the parameters */
extern int spl_set_parameter(SPL_LOCAL_STRUCT_T * Heap, SplParam_t *pParam, 
                             SplStream_t *pStream, SplTuning_t *pTuning);
/* spl_set_stream_param must be called before spl_init */
/* stream param can also be set directly in the init function */
extern int spl_set_stream_param(SPL_LOCAL_STRUCT_T * Heap, SplStream_t *pStream);
extern int spl_set_config(SPL_LOCAL_STRUCT_T * Heap, SplConfig_t *Config);

#endif //_spl_algo_h_

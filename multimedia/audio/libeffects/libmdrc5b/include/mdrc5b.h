/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: local struct definition
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/****************************************************************/
/* FILE: mdrc5b.h                                               */
/* AUTHOR: ZONG Wenbo                                           */
/* EMAIL: wenbo.zong@st.com                                     */
/* DIV: MMC/CSD/MMSU                                            */
/* DATE: 2008 May 16                                            */
/* LAST REV: 2008 May 22                                        */
/****************************************************************/
/* GOAL: defines constants, macros, and local structure,        */
/*      and includes function prototypes                        */
/****************************************************************/

#ifndef _mdrc5b_h_
#define _mdrc5b_h_

#ifdef __arm__
#include <arm_neon.h>
#endif // __arm__

//#define MDRC5B_DEBUG
#ifdef MDRC5B_DEBUG
#include <stdio.h>
extern long counter;
#endif // MDRC5B_DEBUG

#ifdef  __cplusplus
extern "C" {
#endif


#ifdef __arm            //  RVCT only
#ifdef ARM_CYCLES
#include "../../../tools/ARM/pmu/include/pmu_api.h"

typedef enum
{
    PMU_MEASURE_MRDC5B_MAIN,
    PMU_MEASURE_MRDC5B_READ,
    PMU_MEASURE_MRDC5B_WRITE,
    PMU_MEASURE_MRDC5B_MOVE,
    PMU_MEASURE_MRDC5B_APPLY_COMPRESSOR,
    PMU_MEASURE_MRDC5B_LEGACY_FILTER,
    PMU_MEASURE_MRDC5B_BIQUAD_FILTER,
    PMU_MEASURE_MRDC5B_FIR_FILTER,
    PMU_MEASURE_MRDC5B_COMPRESSOR_INSERT_NEW_SUBBAND,
    PMU_MEASURE_MRDC5B_COMPRESSOR_COMPUTE_POWER,
    PMU_MEASURE_MRDC5B_COMPRESSOR_SMOOTH_AND_MAX,
    PMU_MEASURE_MRDC5B_COMPRESSOR_GAIN,
    PMU_MEASURE_MRDC5B_COMPRESSOR_SMOOTH_GAIN,
    PMU_MEASURE_MRDC5B_COMPRESSOR_OUTPUT,
    PMU_MEASURE_MRDC5B_APPLY_LIMITER,
    PMU_MEASURE_MRDC5B_LIMITER_COMPUTE_MAX_SHIFT_LEFT,
    PMU_MEASURE_MRDC5B_LIMITER_INSERT_NEW_SUBBAND,
    PMU_MEASURE_MRDC5B_LIMITER_SMOOTH_GAIN,
    PMU_MEASURE_MRDC5B_LIMITER_APPLY_GAIN,
    NB_PMU_MEASURES
} t_PMU_measure;

extern int PMU_index      [NB_PMU_MEASURES];
extern int cpt_PMU_measure[NB_PMU_MEASURES];

#define START_PMU_MEASURE(index) PMU_tap(2*(index));
#define STOP_PMU_MEASURE(index) PMU_tap(2*(index)+1); PMU_index[index] = PMU_quiet(2*(index)+1, 2*(index)); cpt_PMU_measure[index]++;
#endif // ARM_CYCLES
#else
#undef ARM_CYCLES
#endif // __arm

#ifndef START_PMU_MEASURE
#define START_PMU_MEASURE(index)
#endif // !START_PMU_MEASURE
#ifndef STOP_PMU_MEASURE
#define STOP_PMU_MEASURE(index)
#endif // !STOP_PMU_MEASURE


#include "libeffects/libmdrc5b/include/mdrc5b_options.h"
#include "audiolibs_common.h"
#include "libeffects/libmdrc5b/include/mdrc5b_filters.h"
#include "libeffects/libmdrc5b/include/debug_samples.h"


#ifdef DEBUG_MDRC_SAMPLES_IN_OUT
#define NB_MAX_SAMPLES 100000
typedef struct
{
    int nb_channels;
    int cpt_samples;
    int save_cpt_samples;
    int samples_in_out[NB_MAX_SAMPLES];
} t_debug_mdrc_samples;
extern t_debug_mdrc_samples *p_mdrc_samples_in_out;
#endif // DEBUG_MDRC_SAMPLES_IN_OUT


#ifdef __flexcc2__
#define MDRC5B_EXTERN EXTMEM
#define FILTER_COEF   YMEM
#define FILTER_MEM

#define MMshort short
#define MMlong long
#define MMlonglong long long

#else
#define MDRC5B_EXTERN
#define FILTER_COEF
#define FILTER_MEM
#endif /* __flexcc2__ */


#ifndef MMDSP
#define MMshort    int
#define MMlong     long long
#define MMlonglong long long
#endif // !MMDSP

#ifdef BIT_TRUE
#include "wtype.h"
#endif

#ifdef __arm__
#define SAMPLES_24_BITS  // to be removed
#else // __arm__
#define SAMPLES_24_BITS
#endif // __arm__

#define IVC_LOUDER_CH_PRIORITY // Give priority to the louder channel when computing signal power


#define MDRC5B_MAIN_CH_MAX                      MDRC_CHANNELS_MAX   // L, R
#define MDRC5B_SUBBAND_MAX                      MDRC_BANDS_MAX
#define MDRC5B_KNEEPOINTS_MAX                   10


#define MDRC5B_BLOCK_SIZE                       48      // logical block size, 0.25 ~ 1 ms
#define MDRC5B_BLOCK_SIZE_MSF                   24      // block size for medium sampling frequencies
#define MDRC5B_BLOCK_SIZE_LSF                   16      // block size for lower  sampling frequencies


// compressor settings
#define MDRC5B_LOOKAHEAD_LEN                    240     // 5 msec
#define MDRC5B_LOOKAHEAD_DURATION               5       // msec
#define MDRC5B_POSTGAIN_MAX                     4500    // in millibel, changed from 2000 by XY
#define MDRC5B_POSTGAIN_MIN                     (-4500) // in millibel
#define MDRC5B_COM_AT_TIME_MAX                  1000000 // in microseconds
#define MDRC5B_COM_AT_TIME_MIN                  1000    // in microseconds
#define MDRC5B_COM_RE_TIME_MAX                  5000000 // in microseconds
#define MDRC5B_COM_RE_TIME_MIN                  10000   // in microseconds
#define MDRC5B_RMS_ATTACK_TIME                  4000    // in microseconds
#define MDRC5B_RMS_RELEASE_TIME                 100000  // in microseconds
#define MDRC5B_HOLD_TIME                        40000   // in microseconds


// limiter settings
#define MDRC5B_LIMITER_LOOKAHEAD_LEN            144     // 3 msec, must be longer than the default attack time
#define MDRC5B_LIMITER_LOOKAHEAD_DURATION       3       // msec
#define MDRC5B_LIMITER_ATTACK_TIME_DEFAULT      1000    // in microseconds
#define MDRC5B_LIMITER_RELEASE_TIME_DEFAULT     100000  // in microseconds
#define MDRC5B_LIMITER_EST_ATTACK_TIME_DEFAULT  40      // in microseconds
#define MDRC5B_LIMITER_EST_RELEASE_TIME_DEFAULT 100000  // in microseconds
#define MDRC5B_LIMITER_THRESH_MB_DEFAULT        (-200)  // in millibel
#define MDRC5B_LIMITER_THRESH_MB_MAX            (-100)  // in millibel, leave 1 dB buffer to saturation
#define MDRC5B_LIMITER_THRESH_MB_MIN            (-6000) // in millibel
#define MDRC5B_LIMITER_HOLD_TIME                40000   // in microseconds
#define MDRC5B_LIMITER_ATTACK_COEF              7995702
#define MDRC5B_LIMITER_RELEASE_COEF             8384585
#define MDRC5B_LIMITER_PEAK_ATTACK_COEF         838861
#define MDRC5B_LIMITER_PEAK_RELEASE_COEF        8384585


#define MDRC5B_POWER_DB_MINUS_INF               (-6*(16-1))            //to match with floating point output
#ifdef SAMPLES_24_BITS
#define MDRC5B_ALMOST_ZERO_THRESH               (1<<15)
#else // SAMPLES_24_BITS
#define MDRC5B_ALMOST_ZERO_THRESH               (1LL<<31)
#endif // SAMPLES_24_BITS
#define MDRC5B_INPUT_MIN_MB                     (-13800)               // for 24 bits
#define MDRC5B_INPUT_MAX_MB                     0

#define Q23                                     8388607
#define Q23_ONE_HUNDREDTH                       83886
#define Q23_ONE_TENTH                           838861
#define Q31                                     2147483647
#define Q31_ONE_HUNDREDTH                       21474836
#define Q31_ONE_TENTH                           2147484
#define Q47                                     140737488355327LL
#define LOG10_OF_2                              0.30102999566398119521373889472449
#define GAIN_MINUS_60DB_Q23                     0x20C4
#define GAIN_MINUS_60DB_Q31                     0x20C49B

// math macros
#define MIN(A, B) ((A) > (B) ? (B) : (A))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

#define HEADROOM 1 // 1 bit headroom for internal processing

/* CIRCULAR ADDRESSING MACROS */
//-----------------------------------------------------//
//! Circular Addressing with both boudary checks
#define CIRC_ADD(X,R)    ( (X) >= (R) ?  ((X)-(R)) : ( (X) < 0 ? ( (R) + (X) ) : (X) ))

//! Circular Addressing with lower boudary check
#define CIRC_ADD_L(X,R)  ( (X) < 0 ? ((R) + (X)) : (X) )

//! Circular Addressing with higher boudary check
#define CIRC_ADD_H(X,R)  ( (X) >= (R) ? ((X) - (R)) :  (X) )
//-----------------------------------------------------//


// config structure for a subband compressor for the main channels
typedef struct
{
    int Enable;
    int FreqCutoff;                                // in Hz
    int PostGain;                                  // in millibel, i.e. 1/100 dB
    int KneePoints;
    int DynamicResponse[MDRC5B_KNEEPOINTS_MAX][2]; // in millibel, i.e. 1/100 dB
    int AttackTime;                                // in microseconds
    int ReleaseTime;                               // in microseconds
} T_MDRC5B_BAND_CONFIG;


// config structure for the limiter, same for both main channels
typedef struct
{
    int Enable;
    int RmsMeasure; // 1 if use RMS as input level measure
    int Thresh;     // in millibel, i.e. 1/100 dB
} T_MDRC5B_LIMITER_CONFIG;


enum eProcessingModeMask
{
    COMPRESSOR = 1, // 001
    LIMITER    = 2  // 010
};

typedef struct
{
    MMshort LPFilt[MDRC5B_SUBBAND_MAX-1][5]; // 2nd order low-pass  filter, {b0, b1, b2, a1, a2)
    MMshort HPFilt[MDRC5B_SUBBAND_MAX-1][5]; // 2nd order high-pass filter, {b0, b1, b2, a1, a2}
} t_filter_coef;

typedef struct
{
    MMshort LPFiltHist[MDRC5B_MAIN_CH_MAX][MDRC5B_SUBBAND_MAX-1][8]; // low-pass  history, x[-1] x[-2] y[-1] y[-2], two biquads cascaded
    MMshort HPFiltHist[MDRC5B_MAIN_CH_MAX][MDRC5B_SUBBAND_MAX-1][8]; // high-pass history, x[-1] x[-2] y[-1] y[-2], two biquads cascaded
} t_filter_mem;

typedef struct
{
    MMshort MainSubBuf[MDRC5B_MAIN_CH_MAX][MDRC5B_SUBBAND_MAX][MDRC5B_BLOCK_SIZE];    // buffer to store data during subband processing
} t_filter_buffer;




/*
 * local heap structure
 */
typedef struct MDRC5B_LOCAL_STRUCT
{
    t_mdrc_filter_kind        mdrc_filter_kind;
    int                       Mode;

    int                       SampleFreq;
    int                       BlockSize;
    int                       sample_bit_size;
    int                       NumMainCh;
    int                       DataInOffset;
    int                       DataOutOffset;
    void                      *SysMainInPtr;
    void                      *SysMainOutPtr;


    // IO related
    int                       ConsumedSamples;
    int                       ValidSamples;
    Word48                    MainInBuf [MDRC5B_MAIN_CH_MAX][MDRC5B_BLOCK_SIZE];
    MMshort                   MainOutBuf[MDRC5B_MAIN_CH_MAX][MDRC5B_BLOCK_SIZE * 2];  // x2 to accomodate mismatch b/w burst processing rate and outputting
    int                       InBufSamp; // # of samples in the input buffer
    int                       OutRdIdx;  // index to the reading location of the output buffer
    int                       OutWtIdx;  // index to the writing location of the output buffer


#ifdef MDRC5B_COMPRESSOR_ACTIVE
    int                       CompressorEnable;
    int                       NumBands;
    int                       MdrcLALen;
    T_MDRC5B_BAND_CONFIG      BandCompressors[MDRC5B_SUBBAND_MAX];

    MMshort                   ChWeight[MDRC5B_MAIN_CH_MAX];

    // subband filters derived from config info (legacy filters)
    t_filter_coef FILTER_COEF *p_filter_coef;
    t_filter_mem  FILTER_MEM  *p_filter_mem;

    // new Butterworth or FIR filters
    t_biquad_kind             biquad_kind;
    t_FIR_kind                FIR_kind;
    t_mdrc_filters            *p_mdrc_filters;
    MMshort                   *p_band_input [MDRC_CHANNELS_MAX];
    MMshort                   *p_band_output[MDRC_CHANNELS_MAX][MDRC_BANDS_MAX];

    int                       ComPostGaindB  [MDRC5B_SUBBAND_MAX];
    int                       ComThreshdBOrig[MDRC5B_SUBBAND_MAX][MDRC5B_KNEEPOINTS_MAX-1][2]; // originally derived from config. last points not needed
    int                       ComThreshdB    [MDRC5B_SUBBAND_MAX][MDRC5B_KNEEPOINTS_MAX-1][2]; // Q16
    int                       ComSlope       [MDRC5B_SUBBAND_MAX][MDRC5B_KNEEPOINTS_MAX-1];    // Q16

    // smoothing filters
    MMshort                   RmsAtCoef [MDRC5B_SUBBAND_MAX];
    MMshort                   RmsReCoef [MDRC5B_SUBBAND_MAX];
    MMshort                   ComAtCoef [MDRC5B_SUBBAND_MAX];
    MMshort                   ComReCoef [MDRC5B_SUBBAND_MAX];
    int                       ComHoldRem[MDRC5B_SUBBAND_MAX];
    int                       HtSamp    [MDRC5B_SUBBAND_MAX];


    // subband history
    Word48                    MainSubRms     [MDRC5B_SUBBAND_MAX]; // Q47
    MMshort                   ComTargetGaindB[MDRC5B_SUBBAND_MAX]; // Q23
    MMshort                   ComGainMant    [MDRC5B_SUBBAND_MAX]; // mantissa of last linear gain applied, Q23
    int                       ComGainExp     [MDRC5B_SUBBAND_MAX]; // exponent of last linear gain applied


    // lookahead buffers
    MMshort                   MainSubLABuf[MDRC5B_MAIN_CH_MAX][MDRC5B_SUBBAND_MAX][MDRC5B_LOOKAHEAD_LEN]; // buffer to subband lookahead
    t_filter_buffer           *p_filter_buffer;                                                           // buffer to store data during subband processing
    int                       LaIdx; // index to the lookahead buffer
#endif // MDRC5B_COMPRESSOR_ACTIVE

#ifdef MDRC5B_LIMITER_ACTIVE
    T_MDRC5B_LIMITER_CONFIG   Limiter;

    int                       LimiterLALen;
    Word48                    LimiterLABuf[MDRC5B_MAIN_CH_MAX][MDRC5B_LIMITER_LOOKAHEAD_LEN]; // buffer to subband lookahead
    int                       LimiterLaIdx;
    MMshort                   LimiterPeakAtCoef;               // attack coef for peak estimation
    MMshort                   LimiterPeakReCoef;
    MMshort                   LimiterAtCoef;                   // attack coef for limiter
    MMshort                   LimiterReCoef;
    MMshort                   LimiterThreshdB;
    Word48                    LimiterPeak[MDRC5B_MAIN_CH_MAX]; // last peak
    MMshort                   LimiterGainMant;                 // mantissa of last linear gain applied
    MMshort                   LimiterGainExp;                  // exponent of last linear gain applied
    int                       LimiterHoldRem;
    int                       LimiterHtSamp;
    int                       PrevShiftBits;
#endif // MDRC5B_LIMITER_ACTIVE

    void                      (*processing) (struct MDRC5B_LOCAL_STRUCT *Heap);
} MDRC5B_LOCAL_STRUCT_T;



/*
 * system interface
 */
typedef struct
{
    int                     Mode;
    int                     NumBands;
    T_MDRC5B_BAND_CONFIG    BandCompressors[MDRC5B_SUBBAND_MAX];
    T_MDRC5B_LIMITER_CONFIG Limiter;
}Mdrc5bAlgoConfig_t;



/*
 * functions
 */
extern void mdrc5b_main                   (MDRC5B_LOCAL_STRUCT_T *Heap, void *inbuf, void *outbuf, int size);
extern void mdrc5b_init                   (MDRC5B_LOCAL_STRUCT_T *Heap);
extern void mdrc5b_reset                  (MDRC5B_LOCAL_STRUCT_T *Heap);
extern void mdrc5b_reset_subband_data     (MDRC5B_LOCAL_STRUCT_T *Heap);
extern void mdrc5b_compute_subband_filters(MDRC5B_LOCAL_STRUCT_T *Heap);
extern int  applyEffectConfig             (MDRC5B_LOCAL_STRUCT_T *Heap, Mdrc5bAlgoConfig_t *pConfig);


#ifdef  __cplusplus
}
#endif

#endif //_mdrc5b_h_

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: local struct definition
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _mdrc5b_filters_design_h_
#define _mdrc5b_filters_design_h_

#ifdef __cplusplus
extern "C"
{
#endif

#include <math.h>
#include <stdlib.h>

#include "audiolibs/common/include/audiolibs_types.h"
#include "config.idt"

#define USE_LIB_MATH
//#define SEARCH_MAX_GAIN_FIR


#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif


typedef enum {BUTTERWORTH_LOW_PASS_BIQUAD, BUTTERWORTH_HIGH_PASS_BIQUAD, OTHER_LOW_PASS_BIQUAD, OTHER_HIGH_PASS_BIQUAD} t_biquad_sub_kind;


typedef struct
{
    double b0, b1, b2, a1, a2;
} t_biquad;


typedef struct
{
    t_biquad_sub_kind kind;
    int               gain_exp;
    int               b0, b1, b2, a1, a2;
} t_biquad_int32;



typedef struct
{
    t_FIR_kind kind;
    int        order;
    int        gain_exp;
    int        coef[MDRC_FIR_ORDER_MAX];
} t_FIR_int32;



extern int mdrc_bands_butterworth(double              *pCutFreq,
                                  int                 nbCutFreq,
                                  double              fs,
                                  int                 biquad_order,
                                  t_biquad            **p_p_biquads_LP,
                                  t_biquad            **p_p_biquads_HP);

extern int mdrc_bands_FIR        (int                 free_cos_table,
#ifdef SEARCH_MAX_GAIN_FIR
                                  double              *pMaxGain,
                                  double              *pMaxFreq,
#endif // SEARCH_MAX_GAIN_FIR
                                  double              *pCutFreq,
                                  int                 nbCutFreq,
                                  double              fs,
                                  int                 FIR_order,
                                  double              **p_p_FIR);


extern int convert_biquad_int32  (t_biquad            *p_biquads,
                                  t_biquad_int32      *p_biquads_int32,
                                  int                 biquad_order,
                                  t_biquad_sub_kind   biquad_kind);

extern int convert_FIR_int32     (double              *p_FIR,
                                  t_FIR_int32         *p_FIR_int32,
                                  int                 FIR_order,
                                  t_FIR_kind          FIR_kind);


extern int convert_biquad_hst2mpc(t_biquad_int32      *p_biquad_hst,
                                  t_biquad_cell_int16 *p_biquad_mpc);

extern int convert_FIR_hst2mpc   (t_FIR_int32         *p_FIR_hst,
                                  t_FIR_int16         *p_FIR_mpc);

extern int optimize_gains_FIR_mB (double              *p_CutFreq,
                                  double              fs,
                                  int                 nb_bands,
                                  double              **p_p_FIR,
                                  int                 FIR_size,
                                  int                 *p_gains_in_mB,
                                  int                 *p_gains_out_mB);


#ifdef __cplusplus
}
#endif

#endif // _mdrc5b_filters_design_h_

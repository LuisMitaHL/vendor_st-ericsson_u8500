/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   TEQ_SHAI.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _TEQ_SHAI_H_
#define _TEQ_SHAI_H_

#ifdef __cplusplus
extern "C"
{
#endif

// matlab script generation for response curve display
//#define MATLAB_TEQ_SHAI_CONFIG

// Debug level written into text file (if 0 : no debug)
#define DEBUG_TEQ_SHAI_CONFIG_LEVEL 0


#if defined(MATLAB_TEQ_SHAI_CONFIG) || (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0)
#include <stdio.h>
#endif // MATLAB_TEQ_SHAI_CONFIG || (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0)


#define NB_MAX_INSTANCE_TEQ 16

#ifdef MATLAB_TEQ_SHAI_CONFIG
extern FILE *matlab_TEQ_SHAI_file[NB_MAX_INSTANCE_TEQ];
extern void open_matlab_TEQ_SHAI_file(int instance);
extern void close_matlab_TEQ_SHAI_file(int instance);
#endif // MATLAB_TEQ_SHAI_CONFIG

#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0)
extern FILE *debug_TEQ_SHAI_file[NB_MAX_INSTANCE_TEQ];
extern void open_debug_TEQ_SHAI_file(int instance);
extern void write0_debug_TEQ_SHAI_file(int instance, const char *format);
extern void write1_debug_TEQ_SHAI_file(int instance, const char *format, int val1);
extern void write2_debug_TEQ_SHAI_file(int instance, const char *format, int val1, int val2);
extern void close_debug_TEQ_SHAI_file(int instance);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0

#include "audiolibs/common/include/audiolibs_types.h"
#include "libeffects/libtransducer_equalizer/include/config.idt"
#include "audio_transducer_chipset_api.h"
#include "basic_math.h"

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define TREQ_SHAI_DEFAULT_FIR_ORDER 99

typedef struct
{
    t_float re, im;
} t_complex;

typedef struct complex_root
{
    t_complex           root;
    t_float             gain;
    t_float             distance_to_unit_circle;
    struct complex_root *p_next;
} t_complex_root;

typedef struct real_root
{
    t_float          root;
    t_float          gain;
    t_float          distance_to_unit_circle;
    struct real_root *p_next;
} t_real_root;

typedef struct
{
    t_float q0, q1, q2;
} t_quad;

typedef struct biquad_cell_float
{
    t_quad                         num, den;
    t_complex_root                 *p_complex_zeros[2];
    t_complex_root                 *p_complex_poles[2];
    t_real_root                    *p_real_zeros   [2];
    t_real_root                    *p_real_poles   [2];
    struct biquad_cell_float       *p_next;
} t_biquad_cell_float;

typedef struct
{
    t_biquad_cell_float *p_biquad_cell_list;
    t_biquad_cell_float *p_last_biquad_cell;
    t_biquad_cell_float *p_biquad_cell_list_sorted;
#ifdef MATLAB_TEQ_SHAI_CONFIG
    OMX_U32             nSamplingRate;
#endif // MATLAB_TEQ_SHAI_CONFIG
} t_IIR_filter_float;

typedef struct
{
    struct
    {
        OMX_S16 b_exp;
        OMX_S32 b0;
        OMX_S32 b1;
        OMX_S32 b2;
        OMX_S32 a1;
        OMX_S32 a2;
    } biquad_cell[NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS];
    int     nb_max_cells;
    OMX_S16 gain_exp;
    OMX_S32 gain_mant;
} t_IIR_filter_quantified;

typedef struct
{
    OMX_S32 coef[NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS];
    int     nb_max_coefs;
    OMX_S16 gain_exp;
    OMX_S32 gain_mant;
} t_FIR_filter_quantified;


typedef enum {BIQUAD_NO_SORT, BIQUAD_DOWN_SORT, BIQUAD_UP_SORT} t_biquad_sort;

extern OMX_S32 float2Q31 (t_float x, int nb_bits_quantif);
extern void    mult_gains(OMX_S32 *p_gain_mant, OMX_S16 *p_gain_exp, OMX_S32 gain_mB, int nb_bits_quantif);


extern t_float module_avg_biquad(int compute_avg, int b_exp_int, OMX_S32 b0_int, OMX_S32 b1_int, OMX_S32 b2_int, OMX_S32 a1_int, OMX_S32 a2_int, int SamplingRate, t_float **p_p_module2, int instance);
extern t_float module_avg_FIR(int nb_coefs, int coef_offset, OMX_S32 *p_coef, int SamplingRate);


extern OMX_ERRORTYPE TEQ_SHAI_compute_biquad(OMX_AUDIO_CONFIG_IIR_EQUALIZER *iirConfig,
                                             OMX_U32                        nSamplingRate,
                                             t_IIR_filter_float             *p_IIR_filter_float,
                                             int                            instance);

extern OMX_ERRORTYPE TEQ_SHAI_compute_IIR   (int                            nb_cells,
                                             t_IIR_filter_float             *p_IIR_filter_float,
                                             t_IIR_filter_quantified        *p_IIR_filter_quantified,
                                             t_biquad_sort                  biquad_sort,
                                             int                            nb_bits_quantif,
                                             int                            instance);

extern OMX_ERRORTYPE TEQ_SHAI_compute_FIR   (OMX_AUDIO_PHASERESPONSETYPE    ePhaseResponseTargetFir,
                                             OMX_AUDIO_CONFIG_FIR_RESPONSE  *p_firConfig,
                                             OMX_U32                        nSamplingRate,
                                             t_FIR_filter_quantified        *p_FIR_filter_quantified,
                                             int                            FIR_size,
                                             int                            nb_bits_quantif,
                                             int                            instance);

#ifdef __cplusplus
}
#endif

#endif // _TEQ_SHAI_H_

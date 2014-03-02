/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   transducer_equalizer_CA9.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef transducer_equalizer_CA9_h
#define transducer_equalizer_CA9_h

#ifdef  __cplusplus
extern "C" {
#endif


#define TEQ_CA9_TEST
//#define TEQ_CA9_VERIF_OPTIM_NEON
//#define TEQ_CA9_PRINT_NEON_CYCLES
//#define TEQ_CA9_PRINT_ALL_NEON_CYCLES
//#define TEQ_CA9_NEON_PROFILING
//#define TEQ_CA9_PRINT_ALL_DIFF


#ifdef __arm
#   include <arm_neon.h>
#   define TEQ_CA9_OPTIM_VERSION
#   ifdef TEQ_CA9_VERIF_OPTIM_NEON
#       define TEQ_CA9_NO_OPTIM_VERSION
#   endif /* TEQ_CA9_VERIF_OPTIM_NEON */
#   ifndef TEQ_CA9_BOARD_TEST
#       undef TEQ_CA9_PRINT_NEON_CYCLES
#       undef TEQ_CA9_PRINT_ALL_NEON_CYCLES
#       undef TEQ_CA9_NEON_PROFILING
#   endif /* !TEQ_CA9_BOARD_TEST */
#else /* __arm */
#   define TEQ_CA9_NO_OPTIM_VERSION
#   undef TEQ_CA9_BOARD_TEST
#   undef TEQ_CA9_VERIF_OPTIM_NEON
#   undef TEQ_CA9_PRINT_NEON_CYCLES
#   undef TEQ_CA9_PRINT_ALL_NEON_CYCLES
#   undef TEQ_CA9_NEON_PROFILING
#endif /* __arm */


#define YMEM
#define MMshort    int
#define MMlong     long long
#define MMlonglong long long

#define MIN_SHORT ((MMshort) 0x80000000)
#define MAX_SHORT ((MMshort) 0x7FFFFFFF)
#define MIN_LONG  ((MMlong)  0x8000000000000000LL)
#define MAX_LONG  ((MMlong)  0x7FFFFFFFFFFFFFFFLL)

#include "transducer_equalizer.h"

#include <stdio.h>

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
#   include <math.h>
    extern void PerfEnableDisable(int);
    extern void PerfSetTickRate(int);
    extern void PerfReset(int);
    extern int  PerfGetCount(int);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


#if defined(TEQ_CA9_PRINT_NEON_CYCLES) || defined(TEQ_CA9_VERIF_OPTIM_NEON) || defined(TEQ_CA9_TEST)
    extern void Tr_EQ_test_buffer_printf_flush(void);
    extern char *Tr_EQ_p_buffer_printf;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES || TEQ_CA9_VERIF_OPTIM_NEON || TEQ_CA9_TEST */

#if defined(TEQ_CA9_PRINT_NEON_CYCLES) || defined(TEQ_CA9_TEST)
#   define NB_BIQUAD_MIN   5
#   define NB_BIQUAD_MAX   30
#   define NB_FIR_COEF_MIN 20
#   define NB_FIR_COEF_MAX 100
    typedef struct
    {
        int       cpt;
        int       min;
        int       max;
        long long sum;
        long long sum2;
    } Tr_EQ_t_stat;
    extern void Tr_EQ_add_stat(Tr_EQ_t_stat *p_stat, int val);
    extern Tr_EQ_t_stat Tr_EQ_stat_biquad_mono          [], Tr_EQ_stat_biquad_stereo1          [], Tr_EQ_stat_biquad_stereo2          [];
    extern Tr_EQ_t_stat Tr_EQ_stat_FIR_no_mem_shift_mono[], Tr_EQ_stat_FIR_no_mem_shift_stereo1[], Tr_EQ_stat_FIR_no_mem_shift_stereo2[];
    extern Tr_EQ_t_stat Tr_EQ_stat_FIR_mem_shift_mono   [], Tr_EQ_stat_FIR_mem_shift_stereo1   [], Tr_EQ_stat_FIR_mem_shift_stereo2   [];
#endif /* TEQ_CA9_PRINT_NEON_CYCLES || TEQ_CA9_TEST */

#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
extern void transducer_equalizer_filter_no_optim(int                                 nb_samples,
                                                 MMshort                             *p_input,
                                                 MMshort                             *p_output,
                                                 TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct);
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */

extern void smooth_transition1_stereo(MMshort *p_input,
                                      MMshort *p_output,
                                      int     nb_samples,
                                      int     coef_num_smooth_input,
                                      int     coef_num_smooth_output,
                                      int     shift_div_transition,
                                      MMshort coef_div_transition,
                                      MMshort transition_gain_mant_l,
                                      int     transition_gain_exp_l,
                                      MMshort transition_gain_mant_r,
                                      int     transition_gain_exp_r);

extern void smooth_transition1_mono  (MMshort *p_input,
                                      MMshort *p_output,
                                      int     nb_samples,
                                      int     coef_num_smooth_input,
                                      int     coef_num_smooth_output,
                                      int     shift_div_transition,
                                      MMshort coef_div_transition,
                                      MMshort transition_gain_mant,
                                      int     transition_gain_exp);

extern void smooth_transition2_stereo(MMshort *p_input,
                                      MMshort *p_output,
                                      int     nb_samples,
                                      int     coef_num_smooth_input,
                                      int     coef_num_smooth_output,
                                      int     shift_div_transition,
                                      MMshort coef_div_transition,
                                      MMshort transition_gain_mant_l,
                                      int     transition_gain_exp_l,
                                      MMshort transition_gain_mant_r,
                                      int     transition_gain_exp_r);

extern void smooth_transition2_mono  (MMshort *p_input,
                                      MMshort *p_output,
                                      int     nb_samples,
                                      int     coef_num_smooth_input,
                                      int     coef_num_smooth_output,
                                      int     shift_div_transition,
                                      MMshort coef_div_transition,
                                      MMshort transition_gain_mant,
                                      int     transition_gain_exp);

#ifdef  __cplusplus
}
#endif

#endif // transducer_equalizer_CA9_h

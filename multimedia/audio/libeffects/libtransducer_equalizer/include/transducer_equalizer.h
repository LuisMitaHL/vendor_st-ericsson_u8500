/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   transducer_equalizer.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
/**
 * transducer_equalizer.h
 * This file containts all the information needed
 * for AEP integration
 *
 **/
#ifndef _transducer_equalizer_h_
#define _transducer_equalizer_h_

#ifdef  __cplusplus
extern "C" {
#endif


#include "api_base_audiolibs_types.h" // for t_saa_sample_freq

#ifdef __flexcc2__
#define MMshort    short
#define MMlong     long
#define MMlonglong long long
#else
#define FIR_MEM_OVERHEAD 16
#ifndef MMshort
#define MMshort    int
#define MMlong     long long
#define MMlonglong long long
#endif
#endif

/*
  This is the local stucture for transducer_equalizer
*/

typedef struct
{
    MMshort b_exp;
    MMshort b0, b1, b2, a1, a2;   // a1 is divided by 2 to be in the [-1,+1] range
} t_biquad_cell;

typedef struct
{
    MMshort b_exp_l, b_exp_r;
    MMshort b0_l, b0_r, b1_l, b1_r, b2_l, b2_r, a1_l, a1_r, a2_l, a2_r; // a1 is divided by 2 to be in the [-1,+1] range
} t_biquad_cell_st;

typedef void (*t_filter) (int nb_samples, MMshort *p_input, MMshort *p_output, void *p_void_transducer_equalizer_struct);

typedef struct
{
#ifndef _NMF_MPC_
    int                   aep_id;               // for AEP only
    int                   position_id;          // for AEP only
    int                   alert_on;             // for AEP only
#else // !_NMF_MPC_
    int                   memory_preset;        // for NMF MPC only
#endif // !_NMF_MPC_

    int                   alloc_done;           // for NMF only
    int                   param_received;       // for NMF only
    int                   config_received;      // for NMF only
    int                   alloc_chan_nb;        // for NMF only
    int                   new_chan_nb;          // for NMF only
    int                   enable;               // for NMF only
    int                   smooth_config_change; // for NMF only

    int                   stereo_config;
    int                   stereo_process;
    int                   same_biquad_l_r;
    int                   same_FIR_l_r;
    int                   biquad_first;
    t_saa_sample_freq     sample_freq;
    int                   chan_nb;
    int                   offset;

    MMshort               biquad_gain_exp_l;
    MMshort               biquad_gain_mant_l;
    MMshort               biquad_gain_exp_r;
    MMshort               biquad_gain_mant_r;
    MMshort               FIR_gain_exp_l;
    MMshort               FIR_gain_mant_l;
    MMshort               FIR_gain_exp_r;
    MMshort               FIR_gain_mant_r;
#ifndef __flexcc2__
    int                   FIR_index;
#endif /* !__flexcc2__ */

    int                   nb_alloc_biquad_cells_per_channel;
    int                   nb_biquad_cells_per_channel;
    int                   nb_alloc_FIR_coefs_per_channel;
    int                   nb_FIR_coefs_per_channel;

    void                  *p_alloc_biquad_coef;
    t_biquad_cell         *p_biquad_coef;
    t_biquad_cell_st      *p_biquad_coef_st;
    MMshort          YMEM *p_biquad_mem;
    MMshort               *p_FIR_coef;
#ifdef __flexcc2__
    MMshort __circ   YMEM *p_FIR_mem;
#else /* __flexcc2__ */
    MMshort               *p_FIR_mem;
#endif /* __flexcc2__ */

    t_filter              first_filter;
    t_filter              second_filter;
#if !defined(__flexcc2__) && defined(TEQ_CA9_NO_OPTIM_VERSION)
    t_filter              first_filter_no_optim;
    t_filter              second_filter_no_optim;
#endif /* !__flexcc2__ && TEQ_CA9_NO_OPTIM_VERSION */
} TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T;


extern void transducer_equalizer_set_filters_pointer(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct);

extern void transducer_equalizer_filter(int                                 nb_samples,
                                        MMshort                             *p_input,
                                        MMshort                             *p_output,
                                        TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct);


#ifdef  __cplusplus
}
#endif

#endif //_transducer_equalizer_h_

/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   transducer_equalizer_src_CA9.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "transducer_equalizer_CA9.h"
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TEQ_CA9_NO_OPTIM_VERSION

static __inline MMlong wL_imul(MMshort a, MMshort b)
{
    return(((MMlong) a) * ((MMlong) b));
}


static __inline MMlonglong wX_fmul(MMshort a, MMshort b)
{
    if((a == MIN_SHORT) && (b == MIN_SHORT))
    {
        return MAX_LONG;
    }
    else
    {
        return((((MMlonglong) a) * ((MMlonglong) b)) << 1);
    }
}


static __inline MMlonglong msl64(MMlonglong a, MMshort b)
{
    MMlonglong tmp;

    if(b > 63)
    {
        b = 63;
    }
    tmp = (a << b);
    if((tmp >> b) == a)
    {
        return tmp;
    }
    else if(a > 0)
    {
        return MAX_LONG;
    }
    else
    {
        return MIN_LONG;
    }
}

static __inline MMlonglong msr64(MMlonglong a, MMshort b)
{
    if(b > 63)
    {
        b = 63;
    }
    if(b > 0)
    {
        return ((a + (((MMlonglong) 1) << (b - 1))) >> b);
    }
    else
    {
        return a;
    }
}

static __inline MMlonglong wX_msl(MMlonglong a, MMshort b)
{
    if(a == 0)
    {
        return 0;
    }
    else
    {
        if(b == 0)
        {
            return a;
        }
        else if(b < 0)
        {
            return msr64(a, -b);
        }
        else
        {
            return msl64(a, b);
        }
    }
}

static __inline MMshort wround_X(MMlonglong a)
{
    if(a > MAX_LONG - (((MMlonglong) 1) << 31))
    {
        return MAX_SHORT;
    }
    else
    {
        return (MMshort) ((a + (((MMlonglong) 1) << 31)) >> 32);
    }
}


static MMshort transducer_equalizer_biquad_filter_mono_sample_no_optim(MMshort       in,
                                                                       MMshort       biquad_gain_exp,
                                                                       MMshort       biquad_gain_mant,
                                                                       int           nb_biquad_cells,
                                                                       t_biquad_cell *p_biquad_coef,
                                                                       MMshort       *p_mem_biquad)
{
    MMlonglong accu_0, accu_1;
    int        i;


    if(nb_biquad_cells > 0)
    {
        for(i = nb_biquad_cells; i > 0; i--)
        {
            accu_0          = wL_imul(p_biquad_coef->b0, in);
            accu_0         += wL_imul(p_biquad_coef->b1, p_mem_biquad[0]);
            accu_0         += wL_imul(p_biquad_coef->b2, p_mem_biquad[1]);
            if(p_biquad_coef->b_exp >= 0)
            {
                accu_0    <<= p_biquad_coef->b_exp;
            }
            else
            {
                accu_0    >>= -p_biquad_coef->b_exp;
            }
            p_mem_biquad[1] = p_mem_biquad[0];
            p_mem_biquad[0] = in;
            p_mem_biquad   += 2;
            accu_1          = wL_imul(p_biquad_coef->a1, p_mem_biquad[0]);
            accu_1         += accu_1;
            accu_1         += wL_imul(p_biquad_coef->a2, p_mem_biquad[1]);
            accu_0         -= accu_1;
            in              = wround_X(wX_msl(accu_0, 1));
            p_biquad_coef++;
        }
        p_mem_biquad[1] = p_mem_biquad[0];
        p_mem_biquad[0] = in;
    }

    return wround_X(wX_msl(wX_fmul(in, biquad_gain_mant), biquad_gain_exp));
}


static void transducer_equalizer_same_biquad_filter_stereo_sample_no_optim(MMshort       *p_in,
                                                                           MMshort       *p_out,
                                                                           MMshort       biquad_gain_exp,
                                                                           MMshort       biquad_gain_mant,
                                                                           int           nb_biquad_cells,
                                                                           t_biquad_cell *p_biquad_coef,
                                                                           MMshort       *p_mem_biquad)
{
    MMlonglong accu_0_left, accu_0_right, accu_1_left, accu_1_right;
    MMshort    in_left  = p_in[0];
    MMshort    in_right = p_in[1];
    int        i;


    if(nb_biquad_cells > 0)
    {
        for(i = nb_biquad_cells; i > 0; i--)
        {
            accu_0_left     = wL_imul(p_biquad_coef->b0, in_left);
            accu_0_right    = wL_imul(p_biquad_coef->b0, in_right);
            accu_0_left    += wL_imul(p_biquad_coef->b1, p_mem_biquad[0]);
            accu_0_right   += wL_imul(p_biquad_coef->b1, p_mem_biquad[1]);
            accu_0_left    += wL_imul(p_biquad_coef->b2, p_mem_biquad[2]);
            accu_0_right   += wL_imul(p_biquad_coef->b2, p_mem_biquad[3]);
            if(p_biquad_coef->b_exp >= 0)
            {
                accu_0_left  <<= p_biquad_coef->b_exp;
                accu_0_right <<= p_biquad_coef->b_exp;
            }
            else
            {
                accu_0_left  >>= -p_biquad_coef->b_exp;
                accu_0_right >>= -p_biquad_coef->b_exp;
            }
            p_mem_biquad[2] = p_mem_biquad[0];
            p_mem_biquad[0] = in_left;
            p_mem_biquad[3] = p_mem_biquad[1];
            p_mem_biquad[1] = in_right;
            p_mem_biquad   += 4;
            accu_1_left     = wL_imul(p_biquad_coef->a1, p_mem_biquad[0]);
            accu_1_right    = wL_imul(p_biquad_coef->a1, p_mem_biquad[1]);
            accu_1_left    += accu_1_left;
            accu_1_right   += accu_1_right;
            accu_1_left    += wL_imul(p_biquad_coef->a2, p_mem_biquad[2]);
            accu_1_right   += wL_imul(p_biquad_coef->a2, p_mem_biquad[3]);
            accu_0_left    -= accu_1_left;
            accu_0_right   -= accu_1_right;
            in_left         = wround_X(wX_msl(accu_0_left,  1));
            in_right        = wround_X(wX_msl(accu_0_right, 1));
            p_biquad_coef++;
        }
        p_mem_biquad[2] = p_mem_biquad[0];
        p_mem_biquad[0] = in_left;
        p_mem_biquad[3] = p_mem_biquad[1];
        p_mem_biquad[1] = in_right;
    }

    p_out[0] = wround_X(wX_msl(wX_fmul(in_left,  biquad_gain_mant), biquad_gain_exp));
    p_out[1] = wround_X(wX_msl(wX_fmul(in_right, biquad_gain_mant), biquad_gain_exp));
}


static void transducer_equalizer_different_biquad_filter_stereo_sample_no_optim(MMshort          *p_in,
                                                                                MMshort          *p_out,
                                                                                MMshort          biquad_gain_exp_l,
                                                                                MMshort          biquad_gain_mant_l,
                                                                                MMshort          biquad_gain_exp_r,
                                                                                MMshort          biquad_gain_mant_r,
                                                                                int              nb_biquad_cells,
                                                                                t_biquad_cell_st *p_biquad_coef,
                                                                                MMshort          *p_mem_biquad)
{
    MMlonglong accu_0_left, accu_0_right, accu_1_left, accu_1_right;
    MMshort    in_left  = p_in[0];
    MMshort    in_right = p_in[1];
    int        i;


    if(nb_biquad_cells > 0)
    {
        for(i = nb_biquad_cells; i > 0; i--)
        {
            accu_0_left     = wL_imul(p_biquad_coef->b0_l, in_left);
            accu_0_right    = wL_imul(p_biquad_coef->b0_r, in_right);
            accu_0_left    += wL_imul(p_biquad_coef->b1_l, p_mem_biquad[0]);
            accu_0_right   += wL_imul(p_biquad_coef->b1_r, p_mem_biquad[1]);
            accu_0_left    += wL_imul(p_biquad_coef->b2_l, p_mem_biquad[2]);
            accu_0_right   += wL_imul(p_biquad_coef->b2_r, p_mem_biquad[3]);
            if(p_biquad_coef->b_exp_l >= 0)
            {
                accu_0_left <<= p_biquad_coef->b_exp_l;
            }
            else
            {
                accu_0_left >>= -p_biquad_coef->b_exp_l;
            }
            if(p_biquad_coef->b_exp_r >= 0)
            {
                accu_0_right <<= p_biquad_coef->b_exp_r;
            }
            else
            {
                accu_0_right >>= -p_biquad_coef->b_exp_r;
            }
            p_mem_biquad[2] = p_mem_biquad[0];
            p_mem_biquad[0] = in_left;
            p_mem_biquad[3] = p_mem_biquad[1];
            p_mem_biquad[1] = in_right;
            p_mem_biquad   += 4;
            accu_1_left     = wL_imul(p_biquad_coef->a1_l, p_mem_biquad[0]);
            accu_1_right    = wL_imul(p_biquad_coef->a1_r, p_mem_biquad[1]);
            accu_1_left    += accu_1_left;
            accu_1_right   += accu_1_right;
            accu_1_left    += wL_imul(p_biquad_coef->a2_l, p_mem_biquad[2]);
            accu_1_right   += wL_imul(p_biquad_coef->a2_r, p_mem_biquad[3]);
            accu_0_left    -= accu_1_left;
            accu_0_right   -= accu_1_right;
            in_left         = wround_X(wX_msl(accu_0_left,  1));
            in_right        = wround_X(wX_msl(accu_0_right, 1));
            p_biquad_coef++;
        }
        p_mem_biquad[2] = p_mem_biquad[0];
        p_mem_biquad[0] = in_left;
        p_mem_biquad[3] = p_mem_biquad[1];
        p_mem_biquad[1] = in_right;
    }

    p_out[0] = wround_X(wX_msl(wX_fmul(in_left,  biquad_gain_mant_l), biquad_gain_exp_l));
    p_out[1] = wround_X(wX_msl(wX_fmul(in_right, biquad_gain_mant_r), biquad_gain_exp_r));
}


static MMshort transducer_equalizer_FIR_filter_mono_sample_no_optim(MMshort in,
                                                                    MMshort FIR_gain_exp,
                                                                    MMshort FIR_gain_mant,
                                                                    int     nb_FIR_coefs,
                                                                    MMshort *p_FIR_coef,
                                                                    MMshort *p_FIR_mem)
{
    MMlonglong accu;
    int        i;


    if(nb_FIR_coefs > 0)
    {
        for(i = nb_FIR_coefs - 1; i > 0; i--)
        {
            p_FIR_mem[i] = p_FIR_mem[i - 1];
        }
        p_FIR_mem[0] = in;
        accu         = 0;
        for(i = nb_FIR_coefs; i > 0; i--)
        {
            accu += wL_imul(*p_FIR_coef++, *p_FIR_mem++);
        }
        accu = wX_msl(accu, 1);

        return wround_X(wX_msl(wX_fmul(wround_X(accu), FIR_gain_mant), FIR_gain_exp));
    }
    else
    {
        return wround_X(wX_msl(wX_fmul(in, FIR_gain_mant), FIR_gain_exp));
    }
}


static void transducer_equalizer_same_FIR_filter_stereo_sample_no_optim(MMshort *p_in,
                                                                        MMshort *p_out,
                                                                        MMshort FIR_gain_exp,
                                                                        MMshort FIR_gain_mant,
                                                                        int     nb_FIR_coefs,
                                                                        MMshort *p_FIR_coef,
                                                                        MMshort *p_FIR_mem)
{
    MMlonglong accu_left, accu_right;
    MMshort    coef;
    int        i;


    if(nb_FIR_coefs > 0)
    {
        for(i = nb_FIR_coefs - 1; i > 0; i--)
        {
            p_FIR_mem[2 * i + 1] = p_FIR_mem[2 * i - 1];
            p_FIR_mem[2 * i + 0] = p_FIR_mem[2 * i - 2];
        }
        p_FIR_mem[0] = p_in[0];
        p_FIR_mem[1] = p_in[1];
        accu_left    = 0;
        accu_right   = 0;
        for(i = nb_FIR_coefs; i > 0; i--)
        {
            coef       = *p_FIR_coef++;
            accu_left  += wL_imul(coef, *p_FIR_mem++);
            accu_right += wL_imul(coef, *p_FIR_mem++);
        }
        accu_left  = wX_msl(accu_left,  1);
        accu_right = wX_msl(accu_right, 1);

        p_out[0] = wround_X(wX_msl(wX_fmul(wround_X(accu_left),  FIR_gain_mant), FIR_gain_exp));
        p_out[1] = wround_X(wX_msl(wX_fmul(wround_X(accu_right), FIR_gain_mant), FIR_gain_exp));
    }
    else
    {
        p_out[0] = wround_X(wX_msl(wX_fmul(p_in[0], FIR_gain_mant), FIR_gain_exp));
        p_out[1] = wround_X(wX_msl(wX_fmul(p_in[1], FIR_gain_mant), FIR_gain_exp));
    }
}


static void transducer_equalizer_different_FIR_filter_stereo_sample_no_optim(MMshort *p_in,
                                                                             MMshort *p_out,
                                                                             MMshort FIR_gain_exp_l,
                                                                             MMshort FIR_gain_mant_l,
                                                                             MMshort FIR_gain_exp_r,
                                                                             MMshort FIR_gain_mant_r,
                                                                             int     nb_FIR_coefs,
                                                                             MMshort *p_FIR_coef,
                                                                             MMshort *p_FIR_mem)
{
    MMlonglong accu_left, accu_right;
    int        i;


    if(nb_FIR_coefs > 0)
    {
        for(i = nb_FIR_coefs - 1; i > 0; i--)
        {
            p_FIR_mem[2 * i + 1] = p_FIR_mem[2 * i - 1];
            p_FIR_mem[2 * i + 0] = p_FIR_mem[2 * i - 2];
        }
        p_FIR_mem[0] = p_in[0];
        p_FIR_mem[1] = p_in[1];
        accu_left    = 0;
        accu_right   = 0;
        for(i = nb_FIR_coefs; i > 0; i--)
        {
            accu_left  += wL_imul(*p_FIR_coef++, *p_FIR_mem++);
            accu_right += wL_imul(*p_FIR_coef++, *p_FIR_mem++);
        }
        accu_left  = wX_msl(accu_left,  1);
        accu_right = wX_msl(accu_right, 1);

        p_out[0] = wround_X(wX_msl(wX_fmul(wround_X(accu_left),  FIR_gain_mant_l), FIR_gain_exp_l));
        p_out[1] = wround_X(wX_msl(wX_fmul(wround_X(accu_right), FIR_gain_mant_r), FIR_gain_exp_r));
    }
    else
    {
        p_out[0] = wround_X(wX_msl(wX_fmul(p_in[0], FIR_gain_mant_l), FIR_gain_exp_l));
        p_out[1] = wround_X(wX_msl(wX_fmul(p_in[1], FIR_gain_mant_r), FIR_gain_exp_r));
    }
}


static void biquad_filter_stereo_different_filter_l_r_no_optim(int     nb_samples,
                                                               MMshort *p_input,
                                                               MMshort *p_output,
                                                               void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell_st                    *p_biquad_coef_st              = p_transducer_equalizer_struct->p_biquad_coef_st;
    MMshort                             *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    MMshort                             biquad_gain_exp_r              = p_transducer_equalizer_struct->biquad_gain_exp_r;
    MMshort                             biquad_gain_mant_r             = p_transducer_equalizer_struct->biquad_gain_mant_r;
    int                                 i;
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


    for(i = 0; i < nb_samples; i += 2)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
        transducer_equalizer_different_biquad_filter_stereo_sample_no_optim(p_input  + i,
                                                                            p_output + i,
                                                                            biquad_gain_exp_l,
                                                                            biquad_gain_mant_l,
                                                                            biquad_gain_exp_r,
                                                                            biquad_gain_mant_r,
                                                                            nb_biquad_cells,
                                                                            p_biquad_coef_st,
                                                                            p_biquad_mem);
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 10)  // remove first 10 values (cache reason)
        {
            Tr_EQ_add_stat(&Tr_EQ_stat_biquad_stereo2[nb_biquad_cells - NB_BIQUAD_MIN], cycles);
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_different_biquad_filter_stereo_sample (%d biquads) NEON CYCLES = %d\n", nb_biquad_cells, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}


static void biquad_filter_stereo_same_filter_l_r_no_optim(int     nb_samples,
                                                          MMshort *p_input,
                                                          MMshort *p_output,
                                                          void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell                       *p_biquad_coef                 = p_transducer_equalizer_struct->p_biquad_coef;
    MMshort                             *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    int                                 i;
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


    for(i = 0; i < nb_samples; i += 2)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
        transducer_equalizer_same_biquad_filter_stereo_sample_no_optim(p_input  + i,
                                                                       p_output + i,
                                                                       biquad_gain_exp_l,
                                                                       biquad_gain_mant_l,
                                                                       nb_biquad_cells,
                                                                       p_biquad_coef,
                                                                       p_biquad_mem);
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 10)  // remove first 10 values (cache reason)
        {
            Tr_EQ_add_stat(&Tr_EQ_stat_biquad_stereo1[nb_biquad_cells - NB_BIQUAD_MIN], cycles);
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_same_biquad_filter_stereo_sample (%d biquads) NEON CYCLES = %d\n", nb_biquad_cells, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}


static void biquad_filter_dual_mono_different_filter_l_r_no_optim(int     nb_samples,
                                                                  MMshort *p_input,
                                                                  MMshort *p_output,
                                                                  void    *p_void_transducer_equalizer_struct)
{
    assert(0);
}


static void biquad_filter_dual_mono_same_filter_l_r_no_optim(int     nb_samples,
                                                             MMshort *p_input,
                                                             MMshort *p_output,
                                                             void    *p_void_transducer_equalizer_struct)
{
    assert(0);
}


static void biquad_filter_mono_no_optim(int     nb_samples,
                                        MMshort *p_input,
                                        MMshort *p_output,
                                        void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell                       *p_biquad_coef                 = p_transducer_equalizer_struct->p_biquad_coef;
    MMshort                             *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    int                                 i;
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


    for(i = 0; i < nb_samples; i++)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
        p_output[i] = transducer_equalizer_biquad_filter_mono_sample_no_optim(p_input[i],
                                                                              biquad_gain_exp_l,
                                                                              biquad_gain_mant_l,
                                                                              nb_biquad_cells,
                                                                              p_biquad_coef,
                                                                              p_biquad_mem);
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 5)   // remove first 5 values (cache reason)
        {
            Tr_EQ_add_stat(&Tr_EQ_stat_biquad_mono[nb_biquad_cells - NB_BIQUAD_MIN], cycles);
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_biquad_filter_mono_sample (%d biquads) NEON CYCLES = %d\n", nb_biquad_cells, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}


static void FIR_filter_stereo_different_filter_l_r_no_optim(int     nb_samples,
                                                            MMshort *p_input,
                                                            MMshort *p_output,
                                                            void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    MMshort                             *p_FIR_mem                     = p_transducer_equalizer_struct->p_FIR_mem;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    MMshort                             FIR_gain_exp_r                 = p_transducer_equalizer_struct->FIR_gain_exp_r;
    MMshort                             FIR_gain_mant_r                = p_transducer_equalizer_struct->FIR_gain_mant_r;
    int                                 i;
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


    for(i = 0; i < nb_samples; i += 2)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
        transducer_equalizer_different_FIR_filter_stereo_sample_no_optim(p_input  + i,
                                                                         p_output + i,
                                                                         FIR_gain_exp_l,
                                                                         FIR_gain_mant_l,
                                                                         FIR_gain_exp_r,
                                                                         FIR_gain_mant_r,
                                                                         nb_FIR_coefs,
                                                                         p_FIR_coef,
                                                                         p_FIR_mem);
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 10)  // remove first 5 values (cache reason)
        {
            Tr_EQ_add_stat(&Tr_EQ_stat_FIR_mem_shift_stereo2[nb_FIR_coefs - NB_FIR_COEF_MIN], cycles);
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_different_FIR_filter_stereo_sample (%d coefficients) NEON CYCLES = %d\n", nb_FIR_coefs, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}


static void FIR_filter_stereo_same_filter_l_r_no_optim(int     nb_samples,
                                                       MMshort *p_input,
                                                       MMshort *p_output,
                                                       void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    MMshort                             *p_FIR_mem                     = p_transducer_equalizer_struct->p_FIR_mem;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    int                                 i;
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


    for(i = 0; i < nb_samples; i += 2)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
        transducer_equalizer_same_FIR_filter_stereo_sample_no_optim(p_input  + i,
                                                                    p_output + i,
                                                                    FIR_gain_exp_l,
                                                                    FIR_gain_mant_l,
                                                                    nb_FIR_coefs,
                                                                    p_FIR_coef,
                                                                    p_FIR_mem);
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 10)  // remove first 5 values (cache reason)
        {
            Tr_EQ_add_stat(&Tr_EQ_stat_FIR_mem_shift_stereo1[nb_FIR_coefs - NB_FIR_COEF_MIN], cycles);
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_same_FIR_filter_stereo_sample (%d coefficients) NEON CYCLES = %d\n", nb_FIR_coefs, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}


static void FIR_filter_dual_mono_different_filter_l_r_no_optim(int     nb_samples,
                                                               MMshort *p_input,
                                                               MMshort *p_output,
                                                               void    *p_void_transducer_equalizer_struct)
{
    assert(0);
}


static void FIR_filter_dual_mono_same_filter_l_r_no_optim(int     nb_samples,
                                                          MMshort *p_input,
                                                          MMshort *p_output,
                                                          void    *p_void_transducer_equalizer_struct)
{
    assert(0);
}


static void FIR_filter_mono_no_optim(int     nb_samples,
                                     MMshort *p_input,
                                     MMshort *p_output,
                                     void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    MMshort                             *p_FIR_mem                     = p_transducer_equalizer_struct->p_FIR_mem;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    int                                 i;
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


    for(i = 0; i < nb_samples; i++)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
        p_output[i] = transducer_equalizer_FIR_filter_mono_sample_no_optim(p_input[i],
                                                                           FIR_gain_exp_l,
                                                                           FIR_gain_mant_l,
                                                                           nb_FIR_coefs,
                                                                           p_FIR_coef,
                                                                           p_FIR_mem);
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 5)   // remove first 5 values (cache reason)
        {
            Tr_EQ_add_stat(&Tr_EQ_stat_FIR_mem_shift_mono[nb_FIR_coefs - NB_FIR_COEF_MIN], cycles);
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_FIR_filter_mono_sample (%d coefficients) NEON CYCLES = %d\n", nb_FIR_coefs, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}

#endif /* TEQ_CA9_NO_OPTIM_VERSION */


#ifdef TEQ_CA9_OPTIM_VERSION

static MMshort transducer_equalizer_biquad_filter_mono_sample(MMshort       in,
                                                              MMshort       biquad_gain_exp,
                                                              MMshort       biquad_gain_mant,
                                                              int           nb_biquad_cells,
                                                              t_biquad_cell *p_biquad_coef,
                                                              MMshort       *p_mem_biquad)
{
    int32_t      *p_load_mem_int32, *p_store_mem_int32, *p_coef_int32;
    int32x2_t    in_32x2, mem_0_32x2, mem_1_32x2;
    int32x4_t    mem_0_32x4, mem_1_32x4;
    int32x4_t    coef_0_32x4, coef_1_32x4;
    int64x2_t    accu_0_64x2, accu_1_64x2;
    int64x1_t    accu_0_64x1, accu_1_64x1, zero_64x1;
    unsigned int i;


    zero_64x1 = vdup_n_s64(0);
    in_32x2   = vdup_n_s32(0);
    in_32x2   = vset_lane_s32(in, in_32x2, 1);                                                                              // 0, in=x(n)
    if(nb_biquad_cells > 0)
    {
        p_coef_int32      = (int32_t *) p_biquad_coef;
        p_load_mem_int32  = (int32_t *) p_mem_biquad;
        p_store_mem_int32 = p_load_mem_int32;
        mem_0_32x4        = vld1q_s32(p_load_mem_int32);                                                                    // x(n-1), x(n-2), y(n-1), y(n-2)
        p_load_mem_int32 += 4;
        for(i = (unsigned int) (nb_biquad_cells >> 1); i > 0; i--)
        {
            // even biquad
            coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b_exp, b0, b1, b2
            p_coef_int32      += 4;
            coef_1_32x4        = vld1q_s32(p_coef_int32);                                                                   // a1, a2, next b_ext, next b0
            p_coef_int32      += 4;

            accu_0_64x2        = vmull_s32(             in_32x2,                   vget_low_s32 (coef_0_32x4));             // 0, b0*x(n)
            accu_0_64x2        = vmlal_s32(accu_0_64x2, vget_low_s32 (mem_0_32x4), vget_high_s32(coef_0_32x4));             // b1*x(n-1), b0*x(n)+b2*x(n-2)
            accu_1_64x2        = vmull_s32(             vget_high_s32(mem_0_32x4), vget_low_s32 (coef_1_32x4));             // a1*y(n-1), a2*y(n-2)
            accu_0_64x1        = vadd_s64(vget_low_s64(accu_0_64x2), vget_high_s64(accu_0_64x2));                           // b1*x(n-1)+b0*x(n)+b2*x(n-2)
            accu_1_64x1        = vadd_s64(vget_low_s64(accu_1_64x2), vget_low_s64 (accu_1_64x2));                           // 2*a1*y(n-1)
            accu_1_64x1        = vadd_s64(accu_1_64x1,               vget_high_s64(accu_1_64x2));                           // 2*a1*y(n-1)+a2*y(n-2)
            // should be vget_low_s64(vmovl_s32(vget_low_s32(...))) but in fact vreinterpret_s64_s32(vget_low_s32(...))
            // works fine (because MSB are ignored in left shift) and take 1 instruction less
            //accu_0_64x1        = vrshl_s64(accu_0_64x1, vget_low_s64(vmovl_s32(vget_low_s32(coef_0_32x4))));                // (b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
            accu_0_64x1        = vrshl_s64(accu_0_64x1, vreinterpret_s64_s32(vget_low_s32(coef_0_32x4)));                   // (b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
            accu_0_64x1        = vsub_s64(accu_0_64x1, accu_1_64x1);                                                        // ((b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

            mem_0_32x2         = vext_s32(in_32x2, vget_low_s32(mem_0_32x4), 1);                                            // x(n), x(n-1)

            in_32x2            = vqrshrn_n_s64(vcombine_s64(zero_64x1, accu_0_64x1), 31);

            // odd biquad
            mem_1_32x4         = vld1q_s32(p_load_mem_int32);                                                               // y(n-1), y(n-2), ..., ...
            p_load_mem_int32  += 4;
            coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b1, b2, a1, a2
            p_coef_int32      += 4;

            accu_0_64x2        = vmull_s32(             in_32x2,                   vget_high_s32(coef_1_32x4));             // 0, b0*x(n)
            accu_0_64x2        = vmlal_s32(accu_0_64x2, vget_high_s32(mem_0_32x4), vget_low_s32(coef_0_32x4));              // b1*x(n-1), b0*x(n)+b2*x(n-2)
            accu_1_64x2        = vmull_s32(             vget_low_s32 (mem_1_32x4), vget_high_s32(coef_0_32x4));             // a1*y(n-1), a2*y(n-2)
            accu_0_64x1        = vadd_s64(vget_low_s64(accu_0_64x2), vget_high_s64(accu_0_64x2));                           // b1*x(n-1)+b0*x(n)+b2*x(n-2)
            accu_1_64x1        = vadd_s64(vget_low_s64(accu_1_64x2), vget_low_s64 (accu_1_64x2));                           // 2*a1*y(n-1)
            accu_1_64x1        = vadd_s64(accu_1_64x1,               vget_high_s64(accu_1_64x2));                           // 2*a1*y(n-1)+a2*y(n-2)
            // should be vget_low_s64(vmovl_s32(vget_high_s32(...))) but in fact vreinterpret_s64_s32(vget_high_s32(...))
            // works fine (because MSB are ignored in left shift) and take 1 instruction less
            //accu_0_64x1        = vrshl_s64(accu_0_64x1, vget_low_s64(vmovl_s32(vget_high_s32(coef_1_32x4))));               // (b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
            accu_0_64x1        = vrshl_s64(accu_0_64x1, vreinterpret_s64_s32(vget_high_s32(coef_1_32x4)));                  // (b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
            accu_0_64x1        = vsub_s64(accu_0_64x1, accu_1_64x1);                                                        // ((b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

            mem_1_32x2         = vext_s32(in_32x2, vget_high_s32(mem_0_32x4), 1);                                           // x(n), x(n-1)
            vst1q_s32(p_store_mem_int32, vcombine_s32(mem_0_32x2, mem_1_32x2));
            p_store_mem_int32 += 4;

            in_32x2            = vqrshrn_n_s64(vcombine_s64(zero_64x1, accu_0_64x1), 31);

            mem_0_32x4         = mem_1_32x4;
        }

        if(nb_biquad_cells & 1)
        {
            int32x2_t coef_32x2;

            // last biquad if odd number of biquads
            coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b_exp, b0, b1, b2
            coef_32x2          = vld1_s32(p_coef_int32 + 4);                                                                // a1, a2

            accu_0_64x2        = vmull_s32(             in_32x2, vget_low_s32(coef_0_32x4));                                             // 0, b0*x(n)
            accu_0_64x2        = vmlal_s32(accu_0_64x2, vget_low_s32(mem_0_32x4), vget_high_s32(coef_0_32x4));              // b1*x(n-1), b0*x(n)+b2*x(n-2)
            accu_1_64x2        = vmull_s32(vget_high_s32(mem_0_32x4), coef_32x2);                                           // a1*y(n-1), a2*y(n-2)
            accu_0_64x1        = vadd_s64(vget_low_s64(accu_0_64x2), vget_high_s64(accu_0_64x2));                           // b1*x(n-1)+b0*x(n)+b2*x(n-2)
            accu_1_64x1        = vadd_s64(vget_low_s64(accu_1_64x2), vget_low_s64(accu_1_64x2));                            // 2*a1*y(n-1)
            accu_1_64x1        = vadd_s64(accu_1_64x1, vget_high_s64(accu_1_64x2));                                         // 2*a1*y(n-1)+a2*y(n-2)
            // should be vget_low_s64(vmovl_s32(vget_low_s32(...))) but in fact vreinterpret_s64_s32(vget_low_s32(...))
            // works fine (because MSB are ignored in left shift) and take 1 instruction less
            //accu_0_64x1        = vrshl_s64(accu_0_64x1, vget_low_s64(vmovl_s32(vget_low_s32(coef_0_32x4))));                // (b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
            accu_0_64x1        = vrshl_s64(accu_0_64x1, vreinterpret_s64_s32(vget_low_s32(coef_0_32x4)));                   // (b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
            accu_0_64x1        = vsub_s64(accu_0_64x1, accu_1_64x1);                                                        // ((b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

            mem_0_32x2         = vext_s32(in_32x2, vget_low_s32(mem_0_32x4), 1);                                            // x(n), x(n-1)

            in_32x2            = vqrshrn_n_s64(vcombine_s64(zero_64x1, accu_0_64x1), 31);

            mem_1_32x2         = vext_s32(in_32x2, vget_high_s32(mem_0_32x4), 1);                                           // x(n), x(n-1)
            vst1q_s32(p_store_mem_int32, vcombine_s32(mem_0_32x2, mem_1_32x2));
        }
        else
        {
            mem_0_32x2         = vext_s32(in_32x2, vget_low_s32(mem_0_32x4), 1);                                            // x(n), x(n-1)
            vst1_s32(p_store_mem_int32, mem_0_32x2);
        }
    }

    accu_0_64x2 = vqdmull_s32(in_32x2, vdup_n_s32(biquad_gain_mant));                                                       // 0, in*biquad_gain_mant
    accu_0_64x2 = vqrshlq_s64(accu_0_64x2, vdupq_n_s64(biquad_gain_exp));                                                   // 0, (in*biquad_gain_mant)<<biquad_gain_exp
    in_32x2     = vqrshrn_n_s64(accu_0_64x2, 32);

    return vget_lane_s32(in_32x2, 1);
}


static void transducer_equalizer_same_biquad_filter_stereo_sample(MMshort       *p_in,
                                                                  MMshort       *p_out,
                                                                  MMshort       biquad_gain_exp,
                                                                  MMshort       biquad_gain_mant,
                                                                  int           nb_biquad_cells,
                                                                  t_biquad_cell *p_biquad_coef,
                                                                  MMshort       *p_mem_biquad)
{
    int32_t      *p_load_mem_int32, *p_store_mem_int32, *p_coef_int32;
    int32x2_t    in_32x2;
    int32x4_t    mem_0_32x4, mem_1_32x4;
    int32x2_t    b_exp_b0_32x2;
    int32x4_t    b1_b2_a1_a2_32x4;
    int64x2_t    accu_0_64x2, accu_1_64x2;
    unsigned int i;


    in_32x2 = vld1_s32((int32_t *) p_in);                                                                                       // x_l(n), x_r(n)
    if(nb_biquad_cells > 0)
    {
        p_coef_int32      = (int32_t *) p_biquad_coef;
        p_load_mem_int32  = (int32_t *) p_mem_biquad;
        p_store_mem_int32 = p_load_mem_int32;
        mem_0_32x4        = vld1q_s32(p_load_mem_int32);                                                                        // x_l(n-1), x_r(n-1), x_l(n-2), x_r(n-2)
        p_load_mem_int32 += 4;

        for(i = (unsigned int) nb_biquad_cells; i > 0; i--)
        {
            mem_1_32x4         = vld1q_s32(p_load_mem_int32);                                                                   // next x_l(n-1), x_r(n-1), x_l(n-2), x_r(n-2)
            p_load_mem_int32  += 4;
            b_exp_b0_32x2      = vld1_s32(p_coef_int32);                                                                        // b_exp, b0
            p_coef_int32      += 2;
            b1_b2_a1_a2_32x4   = vld1q_s32(p_coef_int32);                                                                       // b1, b2, a1, a2
            p_coef_int32      += 4;

            accu_0_64x2        = vmull_lane_s32(             in_32x2,                   b_exp_b0_32x2,                  1);     // b0*x_l(n), b0*x_r(n)
            accu_0_64x2        = vmlal_lane_s32(accu_0_64x2, vget_low_s32 (mem_0_32x4), vget_low_s32(b1_b2_a1_a2_32x4), 0);     // b0*x_l(n)+b1*x_l(n-1), b0*x_r(n)+b1*x_r(n-1)
            accu_0_64x2        = vmlal_lane_s32(accu_0_64x2, vget_high_s32(mem_0_32x4), vget_low_s32(b1_b2_a1_a2_32x4), 1);     // b0*x_l(n)+b1*x_l(n-1)+b2*x_l(n-2), b0*x_r(n)+b1*x_r(n-1)+b2*x_r(n-2)
            accu_0_64x2        = vshlq_s64(accu_0_64x2, vdupq_lane_s64(vreinterpret_s64_s32(b_exp_b0_32x2), 0));                // (b0*x_l(n)+b1*x_l(n-1)+b2*x_l(n-2))<<b_exp, (b0*x_r(n)+b1*x_r(n-1)+b2*x_r(n-2))<<b_exp

            mem_0_32x4         = vcombine_s32(in_32x2, vget_low_s32(mem_0_32x4));                                               // x_l(n), x_r(n), x_l(n-1), x_r(n-1)
            vst1q_s32(p_store_mem_int32, mem_0_32x4);
            p_store_mem_int32 += 4;

            accu_1_64x2        = vmull_lane_s32(        vget_low_s32 (mem_1_32x4), vget_high_s32(b1_b2_a1_a2_32x4), 0);         // a1*y_l(n-1), a1*y_r(n-1)
            accu_1_64x2        = vaddq_s64(accu_1_64x2, accu_1_64x2);                                                           // 2*a1*y_l(n-1), 2*a1*y_r(n-1)
            accu_1_64x2        = vmlal_lane_s32(accu_1_64x2, vget_high_s32(mem_1_32x4), vget_high_s32(b1_b2_a1_a2_32x4), 1);    // 2*a1*y_l(n-1)+a2*y_l(n-2), 2*a1*y_r(n-1)+a2*y_r(n-2)
            accu_0_64x2        = vsubq_s64(accu_0_64x2, accu_1_64x2);                                                           // ((b0*x_l(n)+b1*x_l(n-1)+b2*x_l(n-2))<<b_exp)-2*a1*y_l(n-1)-a2*y_l(n-2), ((b0*x_r(n)+b1*x_r(n-1)+b2*x_r(n-2))<<b_exp)-2*a1*y_r(n-1)-a2*y_r(n-2)
            in_32x2            = vqrshrn_n_s64(accu_0_64x2, 31);                                                                // ((b0*x_l(n)+b1*x_l(n-1)+b2*x_l(n-2))<<b_exp)-2*a1*y_l(n-1)-a2*y_l(n-2), ((b0*x_r(n)+b1*x_r(n-1)+b2*x_r(n-2))<<b_exp)-2*a1*y_r(n-1)-a2*y_r(n-2)

            mem_0_32x4         = mem_1_32x4;
        }
        mem_0_32x4 = vcombine_s32(in_32x2, vget_low_s32(mem_0_32x4));                                                           // y_l(n), y_r(n), y_l(n-1), y_r(n-1)
        vst1q_s32(p_store_mem_int32, mem_0_32x4);
    }

    accu_0_64x2 = vqdmull_s32(in_32x2, vdup_n_s32(biquad_gain_mant));                                                           // in*biquad_gain_mant
    accu_0_64x2 = vqrshlq_s64(accu_0_64x2, vdupq_n_s64(biquad_gain_exp));                                                       // (in*biquad_gain_mant)<<biquad_gain_exp
    in_32x2     = vqrshrn_n_s64(accu_0_64x2, 32);
    vst1_s32((int32_t *) p_out, in_32x2);
}


static void transducer_equalizer_different_biquad_filter_stereo_sample(MMshort          *p_in,
                                                                       MMshort          *p_out,
                                                                       int32x2_t        biquad_gain_exp_32x2,
                                                                       int32x2_t        biquad_gain_mant_32x2,
                                                                       int              nb_biquad_cells,
                                                                       t_biquad_cell_st *p_biquad_coef,
                                                                       MMshort          *p_mem_biquad)
{
    int32_t      *p_load_mem_int32, *p_store_mem_int32, *p_coef_int32;
    int32x2_t    in_32x2;
    int32x4_t    mem_0_32x4, mem_1_32x4;
    int32x4_t    b_exp_b0_32x4, b1_b2_32x4, a1_a2_32x4;
    int64x2_t    accu_0_64x2, accu_1_64x2;
    unsigned int i;


    in_32x2 = vld1_s32((int32_t *) p_in);                                                                                       // x_l(n), x_r(n)
    if(nb_biquad_cells > 0)
    {
        p_coef_int32      = (int32_t *) p_biquad_coef;
        p_load_mem_int32  = (int32_t *) p_mem_biquad;
        p_store_mem_int32 = p_load_mem_int32;
        mem_0_32x4        = vld1q_s32(p_load_mem_int32);                                                                        // x_l(n-1), x_r(n-1), x_l(n-2), x_r(n-2)
        p_load_mem_int32 += 4;

        for(i = (unsigned int) nb_biquad_cells; i > 0; i--)
        {
            mem_1_32x4         = vld1q_s32(p_load_mem_int32);                                                                   // next x_l(n-1), x_r(n-1), x_l(n-2), x_r(n-2)
            p_load_mem_int32  += 4;
            b_exp_b0_32x4      = vld1q_s32(p_coef_int32);                                                                       // b_exp_l, b_exp_r, b0_l, b0_r
            p_coef_int32      += 4;
            b1_b2_32x4         = vld1q_s32(p_coef_int32);                                                                       // b1_l, b1_r, b2_l, b2_r
            p_coef_int32      += 4;
            a1_a2_32x4         = vld1q_s32(p_coef_int32);                                                                       // a1_l, a1_r, a2_l, a2_r
            p_coef_int32      += 4;

            accu_0_64x2        = vmull_s32(             in_32x2,                   vget_high_s32(b_exp_b0_32x4));               // b0_l*x_l(n), b0_r*x_r(n)
            accu_0_64x2        = vmlal_s32(accu_0_64x2, vget_low_s32 (mem_0_32x4), vget_low_s32 (b1_b2_32x4));                  // b0_l*x_l(n)+b1_l*x_l(n-1), b0_r*x_r(n)+b1_r*x_r(n-1)
            accu_0_64x2        = vmlal_s32(accu_0_64x2, vget_high_s32(mem_0_32x4), vget_high_s32(b1_b2_32x4));                  // b0_l*x_l(n)+b1_l*x_l(n-1)+b2_l*x_l(n-2), b0_r*x_r(n)+b1_r*x_r(n-1)+b2_r*x_r(n-2)
            accu_0_64x2        = vshlq_s64(accu_0_64x2, vmovl_s32(vget_low_s32(b_exp_b0_32x4)));                                // (b0_l*x_l(n)+b1_l*x_l(n-1)+b2_l*x_l(n-2))<<b_exp_l, (b0_r*x_r(n)+b1_r*x_r(n-1)+b2_r*x_r(n-2))<<b_exp_r

            mem_0_32x4         = vcombine_s32(in_32x2, vget_low_s32(mem_0_32x4));                                               // x_l(n), x_r(n), x_l(n-1), x_r(n-1)
            vst1q_s32(p_store_mem_int32, mem_0_32x4);
            p_store_mem_int32 += 4;

            accu_1_64x2        = vmull_s32(             vget_low_s32 (mem_1_32x4), vget_low_s32 (a1_a2_32x4));                  // a1_l*y_l(n-1), a1_r*y_r(n-1)
            accu_1_64x2        = vaddq_s64(accu_1_64x2, accu_1_64x2);                                                           // 2*a1_l*y_l(n-1), 2*a1_r*y_r(n-1)
            accu_1_64x2        = vmlal_s32(accu_1_64x2, vget_high_s32(mem_1_32x4), vget_high_s32(a1_a2_32x4));                  // 2*a1_l*y_l(n-1)+a2_l*y_l(n-2), 2*a1_r*y_r(n-1)+a2_r*y_r(n-2)
            accu_0_64x2        = vsubq_s64(accu_0_64x2, accu_1_64x2);                                                           // ((b0_l*x_l(n)+b1_l*x_l(n-1)+b2_l*x_l(n-2))<<b_exp_l)-2*a1_l*y_l(n-1)-a2_l*y_l(n-2), ((b0_r*x_r(n)+b1_r*x_r(n-1)+b2_r*x_r(n-2))<<b_exp_r)-2*a1_r*y_r(n-1)-a2_r*y_r(n-2)
            in_32x2            = vqrshrn_n_s64(accu_0_64x2, 31);                                                                // ((b0_l*x_l(n)+b1_l*x_l(n-1)+b2_l*x_l(n-2))<<b_exp_l)-2*a1_l*y_l(n-1)-a2_l*y_l(n-2), ((b0_r*x_r(n)+b1_r*x_r(n-1)+b2_r*x_r(n-2))<<b_exp_r)-2*a1_r*y_r(n-1)-a2_r*y_r(n-2)

            mem_0_32x4         = mem_1_32x4;
        }
        mem_0_32x4 = vcombine_s32(in_32x2, vget_low_s32(mem_0_32x4));                                                           // y_l(n), y_r(n), y_l(n-1), y_r(n-1)
        vst1q_s32(p_store_mem_int32, mem_0_32x4);
    }

    accu_0_64x2 = vqdmull_s32(in_32x2, biquad_gain_mant_32x2);                                                                  // in_l*biquad_gain_mant_l, in_r*biquad_gain_mant_r
    accu_0_64x2 = vqrshlq_s64(accu_0_64x2, vmovl_s32(biquad_gain_exp_32x2));                                                    // (in_l*biquad_gain_mant_l)<<biquad_gain_exp_l, (in_r*biquad_gain_mant_r)<<biquad_gain_exp_r
    in_32x2     = vqrshrn_n_s64(accu_0_64x2, 32);
    vst1_s32((int32_t *) p_out, in_32x2);
}


static __forceinline void FIR_mem_copy(MMshort *p_dst, MMshort *p_src, int size)
{
    int32_t      *p_load_mem_int32, *p_store_mem_int32;
    int32x4_t    mem_32x4;
    int32x2_t    mem_32x2;
    unsigned int i;

    p_load_mem_int32  = (int32_t *) p_src;
    p_store_mem_int32 = (int32_t *) p_dst;
    for(i = (unsigned int) (size >> 2); i > 0; i--)
    {
        mem_32x4 = vld1q_s32(p_load_mem_int32);
        p_load_mem_int32  += 4;
        vst1q_s32(p_store_mem_int32, mem_32x4);
        p_store_mem_int32 += 4;
    }
    if(size & 2)
    {
        mem_32x2 = vld1_s32(p_load_mem_int32);
        vst1_s32(p_store_mem_int32, mem_32x2);
    }
    if(size & 1)
    {
        p_dst[size - 1] = p_src[size - 1];
    }
}

/*
static __forceinline void FIR_mem_move(MMshort *p_dst, MMshort *p_src, int size)
{
    int32_t      *p_load_mem_int32, *p_store_mem_int32;
    int32x4_t    mem_32x4;
    int32x2_t    mem_32x2;
    unsigned int i;

    p_load_mem_int32  = (int32_t *) (p_src + size - 4);
    p_store_mem_int32 = (int32_t *) (p_dst + size - 4);
    for(i = (unsigned int) (size >> 2); i > 0; i--)
    {
        mem_32x4 = vld1q_s32(p_load_mem_int32);
        p_load_mem_int32  -= 4;
        vst1q_s32(p_store_mem_int32, mem_32x4);
        p_store_mem_int32 -= 4;
    }
    if(size & 2)
    {
        mem_32x2 = vld1_s32(p_load_mem_int32 + 2);
        vst1_s32(p_store_mem_int32 + 2, mem_32x2);
    }
    if(size & 1)
    {
        p_dst[0] = p_src[0];
    }
}
*/

static __forceinline void FIR_mem_shift(MMshort *p_dst, MMshort *p_src, int size)
{
    int offset = p_dst - p_src;


    if(offset <= 0)
    {
        FIR_mem_copy(p_dst, p_src, size);
        return;
    }

    do
    {
        if(offset >= size)
        {
            FIR_mem_copy(p_dst, p_src, size);
            return;
        }
        else
        {
            size -= offset;
            FIR_mem_copy(p_dst + size, p_src + size, offset);
        }
    }
    while(size > 0);
}


static MMshort transducer_equalizer_FIR_filter_mono_sample(MMshort in,
                                                           MMshort FIR_gain_exp,
                                                           MMshort FIR_gain_mant,
                                                           int     nb_FIR_coefs,
                                                           MMshort *p_FIR_coef,
                                                           MMshort *p_FIR_mem,
                                                           int     *p_FIR_index)
{
    unsigned int i;
    int32x4_t    mem_32x4, coef_32x4;
    int32x2_t    mem_32x2, coef_32x2;
    int64x2_t    accu_64x2;
    int64x1_t    accu_64x1;
    int32_t      *p_mem_int32, *p_coef_int32;


    if(nb_FIR_coefs > 0)
    {
        (*p_FIR_index)--;
        if(*p_FIR_index < 0)
        {
            FIR_mem_shift(p_FIR_mem + FIR_MEM_OVERHEAD, p_FIR_mem, nb_FIR_coefs - 1);
            *p_FIR_index = FIR_MEM_OVERHEAD - 1;
            p_FIR_mem   += FIR_MEM_OVERHEAD - 1;
        }
        else
        {
            p_FIR_mem   += *p_FIR_index;
        }
        p_FIR_mem[0] = in;
        p_coef_int32 = (int32_t *) p_FIR_coef;
        p_mem_int32  = (int32_t *) p_FIR_mem;
        accu_64x2    = vdupq_n_s64(0);
        for(i = (unsigned int) (nb_FIR_coefs >> 2); i > 0; i--)
        {
            coef_32x4     = vld1q_s32(p_coef_int32);
            p_coef_int32 += 4;
            mem_32x4      = vld1q_s32(p_mem_int32);
            p_mem_int32  += 4;
            accu_64x2     = vmlal_s32(accu_64x2, vget_low_s32 (mem_32x4), vget_low_s32 (coef_32x4));
            accu_64x2     = vmlal_s32(accu_64x2, vget_high_s32(mem_32x4), vget_high_s32(coef_32x4));
        }
        switch(nb_FIR_coefs & 3)
        {
            default:
                break;

            case 1:
                coef_32x2 = vld1_s32      (p_coef_int32);
                mem_32x2  = vld1_s32      (p_mem_int32);
                coef_32x2 = vset_lane_s32 (0, coef_32x2, 1);
                accu_64x2 = vmlal_s32     (accu_64x2, mem_32x2, coef_32x2);
                break;

            case 2:
                coef_32x2 = vld1_s32      (p_coef_int32);
                mem_32x2  = vld1_s32      (p_mem_int32);
                accu_64x2 = vmlal_s32     (accu_64x2, mem_32x2, coef_32x2);
                break;

            case 3:
                coef_32x4 = vld1q_s32     (p_coef_int32);
                mem_32x4  = vld1q_s32     (p_mem_int32);
                coef_32x4 = vsetq_lane_s32(0, coef_32x4, 3);
                accu_64x2 = vmlal_s32     (accu_64x2, vget_low_s32 (mem_32x4), vget_low_s32 (coef_32x4));
                accu_64x2 = vmlal_s32     (accu_64x2, vget_high_s32(mem_32x4), vget_high_s32(coef_32x4));
                break;
        }
        accu_64x1 = vadd_s64(vget_low_s64(accu_64x2), vget_high_s64(accu_64x2));
        mem_32x2  = vqrshrn_n_s64(vcombine_s64(vdup_n_s64(0), accu_64x1), 31);
    }
    else
    {
        mem_32x2  = vdup_n_s32(0);
        mem_32x2  = vset_lane_s32(in, mem_32x2, 1);
    }
    accu_64x2 = vqdmull_s32(mem_32x2, vdup_n_s32(FIR_gain_mant));
    accu_64x2 = vqrshlq_s64(accu_64x2, vdupq_n_s64(FIR_gain_exp));
    mem_32x2  = vqrshrn_n_s64(accu_64x2, 32);

    return vget_lane_s32(mem_32x2, 1);
}


static void transducer_equalizer_same_FIR_filter_stereo_sample(MMshort *p_in,
                                                               MMshort *p_out,
                                                               MMshort FIR_gain_exp,
                                                               MMshort FIR_gain_mant,
                                                               int     nb_FIR_coefs,
                                                               MMshort *p_FIR_coef,
                                                               MMshort *p_FIR_mem,
                                                               int     *p_FIR_index)
{
    unsigned int i;
    int32x4_t    mem_0_32x4, mem_1_32x4, coef_32x4;
    int32x2_t    mem_32x2, coef_32x2;
    int64x2_t    accu_64x2;
    int32_t      *p_mem_int32, *p_coef_int32;


    if(nb_FIR_coefs > 0)
    {
        (*p_FIR_index) -= 2;
        if(*p_FIR_index < 0)
        {
            FIR_mem_shift(p_FIR_mem + 2 * FIR_MEM_OVERHEAD, p_FIR_mem, 2 * (nb_FIR_coefs - 1));
            *p_FIR_index = 2 * (FIR_MEM_OVERHEAD - 1);
            p_FIR_mem   += 2 * (FIR_MEM_OVERHEAD - 1);
        }
        else
        {
            p_FIR_mem   += *p_FIR_index;
        }
        p_FIR_mem[0] = p_in[0];
        p_FIR_mem[1] = p_in[1];
        p_coef_int32 = (int32_t *) p_FIR_coef;
        p_mem_int32  = (int32_t *) p_FIR_mem;
        accu_64x2    = vdupq_n_s64(0);
        for(i = (unsigned int) (nb_FIR_coefs >> 2); i > 0; i--)
        {
            coef_32x4     = vld1q_s32     (p_coef_int32);
            p_coef_int32 += 4;
            mem_0_32x4    = vld1q_s32     (p_mem_int32);
            p_mem_int32  += 4;
            mem_1_32x4    = vld1q_s32     (p_mem_int32);
            p_mem_int32  += 4;
            accu_64x2     = vmlal_lane_s32(accu_64x2, vget_low_s32 (mem_0_32x4), vget_low_s32 (coef_32x4), 0);
            accu_64x2     = vmlal_lane_s32(accu_64x2, vget_high_s32(mem_0_32x4), vget_low_s32 (coef_32x4), 1);
            accu_64x2     = vmlal_lane_s32(accu_64x2, vget_low_s32 (mem_1_32x4), vget_high_s32(coef_32x4), 0);
            accu_64x2     = vmlal_lane_s32(accu_64x2, vget_high_s32(mem_1_32x4), vget_high_s32(coef_32x4), 1);
        }
        switch(nb_FIR_coefs & 3)
        {
            default:
                break;

            case 1:
                coef_32x2    = vld1_s32      (p_coef_int32);
                mem_32x2     = vld1_s32      (p_mem_int32);
                accu_64x2    = vmlal_lane_s32(accu_64x2, mem_32x2, coef_32x2, 0);
                break;

            case 2:
                coef_32x2    = vld1_s32      (p_coef_int32);
                mem_0_32x4   = vld1q_s32     (p_mem_int32);
                accu_64x2    = vmlal_lane_s32(accu_64x2, vget_low_s32 (mem_0_32x4), coef_32x2, 0);
                accu_64x2    = vmlal_lane_s32(accu_64x2, vget_high_s32(mem_0_32x4), coef_32x2, 1);
                break;

            case 3:
                coef_32x4    = vld1q_s32     (p_coef_int32);
                mem_0_32x4   = vld1q_s32     (p_mem_int32);
                p_mem_int32 += 4;
                mem_32x2     = vld1_s32      (p_mem_int32);
                accu_64x2    = vmlal_lane_s32(accu_64x2, vget_low_s32 (mem_0_32x4), vget_low_s32 (coef_32x4), 0);
                accu_64x2    = vmlal_lane_s32(accu_64x2, vget_high_s32(mem_0_32x4), vget_low_s32 (coef_32x4), 1);
                accu_64x2    = vmlal_lane_s32(accu_64x2, mem_32x2,                  vget_high_s32(coef_32x4), 0);
                break;
        }
        mem_32x2 = vqrshrn_n_s64(accu_64x2, 31);
    }
    else
    {
        mem_32x2 = vld1_s32((int32_t *) p_in);
    }
    accu_64x2 = vqdmull_s32(mem_32x2, vdup_n_s32(FIR_gain_mant));
    accu_64x2 = vqrshlq_s64(accu_64x2, vdupq_n_s64(FIR_gain_exp));
    mem_32x2  = vqrshrn_n_s64(accu_64x2, 32);
    vst1_s32((int32_t *) p_out, mem_32x2);
}


static void transducer_equalizer_different_FIR_filter_stereo_sample(MMshort   *p_in,
                                                                    MMshort   *p_out,
                                                                    int32x2_t FIR_gain_exp_32x2,
                                                                    int32x2_t FIR_gain_mant_32x2,
                                                                    int       nb_FIR_coefs,
                                                                    MMshort   *p_FIR_coef,
                                                                    MMshort   *p_FIR_mem,
                                                                    int       *p_FIR_index)
{
    unsigned int i;
    int32x4_t    mem_32x4, coef_32x4;
    int32x2_t    mem_32x2, coef_32x2;
    int64x2_t    accu_64x2;
    int32_t      *p_mem_int32, *p_coef_int32;


    if(nb_FIR_coefs > 0)
    {
        (*p_FIR_index) -= 2;
        if(*p_FIR_index < 0)
        {
            FIR_mem_shift(p_FIR_mem + 2 * FIR_MEM_OVERHEAD, p_FIR_mem, 2 * (nb_FIR_coefs - 1));
            *p_FIR_index = 2 * (FIR_MEM_OVERHEAD - 1);
            p_FIR_mem   += 2 * (FIR_MEM_OVERHEAD - 1);
        }
        else
        {
            p_FIR_mem   += *p_FIR_index;
        }
        p_FIR_mem[0] = p_in[0];
        p_FIR_mem[1] = p_in[1];
        p_coef_int32 = (int32_t *) p_FIR_coef;
        p_mem_int32  = (int32_t *) p_FIR_mem;
        accu_64x2    = vdupq_n_s64(0);
        for(i = (unsigned int) (nb_FIR_coefs >> 1); i > 0; i--)
        {
            coef_32x4     = vld1q_s32(p_coef_int32);
            p_coef_int32 += 4;
            mem_32x4      = vld1q_s32(p_mem_int32);
            p_mem_int32  += 4;
            accu_64x2     = vmlal_s32(accu_64x2, vget_low_s32 (coef_32x4), vget_low_s32 (mem_32x4));
            accu_64x2     = vmlal_s32(accu_64x2, vget_high_s32(coef_32x4), vget_high_s32(mem_32x4));
        }
        if(nb_FIR_coefs & 1)
        {
            coef_32x2 = vld1_s32 (p_coef_int32);
            mem_32x2  = vld1_s32 (p_mem_int32);
            accu_64x2 = vmlal_s32(accu_64x2, coef_32x2, mem_32x2);
        }
        mem_32x2 = vqrshrn_n_s64(accu_64x2, 31);
    }
    else
    {
        mem_32x2 = vld1_s32((int32_t *) p_in);
    }
    accu_64x2 = vqdmull_s32(mem_32x2, FIR_gain_mant_32x2);
    accu_64x2 = vqrshlq_s64(accu_64x2, vmovl_s32(FIR_gain_exp_32x2));
    mem_32x2  = vqrshrn_n_s64(accu_64x2, 32);
    vst1_s32((int32_t *) p_out, mem_32x2);
}


#endif /* TEQ_CA9_OPTIM_VERSION */



static void biquad_filter_stereo_different_filter_l_r(int     nb_samples,
                                                      MMshort *p_input,
                                                      MMshort *p_output,
                                                      void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell_st                    *p_biquad_coef_st              = p_transducer_equalizer_struct->p_biquad_coef_st;
    MMshort                             *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    MMshort                             biquad_gain_exp_r              = p_transducer_equalizer_struct->biquad_gain_exp_r;
    MMshort                             biquad_gain_mant_r             = p_transducer_equalizer_struct->biquad_gain_mant_r;
#ifdef TEQ_CA9_OPTIM_VERSION
    MMshort                             buf_tmp[2];
    int32x2_t                           biquad_gain_exp_32x2, biquad_gain_mant_32x2;
#endif /* TEQ_CA9_OPTIM_VERSION */
    int                                 i;
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


#ifdef TEQ_CA9_OPTIM_VERSION
    buf_tmp[0]            = biquad_gain_exp_l;
    buf_tmp[1]            = biquad_gain_exp_r;
    biquad_gain_exp_32x2  = vld1_s32((int32_t *) buf_tmp);
    buf_tmp[0]            = biquad_gain_mant_l;
    buf_tmp[1]            = biquad_gain_mant_r;
    biquad_gain_mant_32x2 = vld1_s32((int32_t *) buf_tmp);
#endif /* TEQ_CA9_OPTIM_VERSION */

    for(i = 0; i < nb_samples; i += 2)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
#ifdef TEQ_CA9_OPTIM_VERSION
        transducer_equalizer_different_biquad_filter_stereo_sample         (p_input  + i,
                                                                            p_output + i,
                                                                            biquad_gain_exp_32x2,
                                                                            biquad_gain_mant_32x2,
                                                                            nb_biquad_cells,
                                                                            p_biquad_coef_st,
                                                                            p_biquad_mem);
#else /* TEQ_CA9_OPTIM_VERSION */
        transducer_equalizer_different_biquad_filter_stereo_sample_no_optim(p_input  + i,
                                                                            p_output + i,
                                                                            biquad_gain_exp_l,
                                                                            biquad_gain_mant_l,
                                                                            biquad_gain_exp_r,
                                                                            biquad_gain_mant_r,
                                                                            nb_biquad_cells,
                                                                            p_biquad_coef_st,
                                                                            p_biquad_mem);
#endif /* TEQ_CA9_OPTIM_VERSION */
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 10)  // remove first 10 values (cache reason)
        {
            Tr_EQ_add_stat(&Tr_EQ_stat_biquad_stereo2[nb_biquad_cells - NB_BIQUAD_MIN], cycles);
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_different_biquad_filter_stereo_sample (%d biquads) NEON CYCLES = %d\n", nb_biquad_cells, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}


static void biquad_filter_stereo_same_filter_l_r(int     nb_samples,
                                                 MMshort *p_input,
                                                 MMshort *p_output,
                                                 void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell                       *p_biquad_coef                 = p_transducer_equalizer_struct->p_biquad_coef;
    MMshort                             *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    int                                 i;
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


    for(i = 0; i < nb_samples; i += 2)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
#ifdef TEQ_CA9_OPTIM_VERSION
        transducer_equalizer_same_biquad_filter_stereo_sample         (p_input  + i,
                                                                       p_output + i,
                                                                       biquad_gain_exp_l,
                                                                       biquad_gain_mant_l,
                                                                       nb_biquad_cells,
                                                                       p_biquad_coef,
                                                                       p_biquad_mem);
#else /* TEQ_CA9_OPTIM_VERSION */
        transducer_equalizer_same_biquad_filter_stereo_sample_no_optim(p_input  + i,
                                                                       p_output + i,
                                                                       biquad_gain_exp_l,
                                                                       biquad_gain_mant_l,
                                                                       nb_biquad_cells,
                                                                       p_biquad_coef,
                                                                       p_biquad_mem);
#endif /* TEQ_CA9_OPTIM_VERSION */
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 10)  // remove first 10 values (cache reason)
        {
            Tr_EQ_add_stat(&Tr_EQ_stat_biquad_stereo1[nb_biquad_cells - NB_BIQUAD_MIN], cycles);
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_same_biquad_filter_stereo_sample (%d biquads) NEON CYCLES = %d\n", nb_biquad_cells, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}


static void biquad_filter_dual_mono_different_filter_l_r(int     nb_samples,
                                                         MMshort *p_input,
                                                         MMshort *p_output,
                                                         void    *p_void_transducer_equalizer_struct)
{
    assert(0);
}


static void biquad_filter_dual_mono_same_filter_l_r(int     nb_samples,
                                                    MMshort *p_input,
                                                    MMshort *p_output,
                                                    void    *p_void_transducer_equalizer_struct)
{
    assert(0);
}


static void biquad_filter_mono(int     nb_samples,
                               MMshort *p_input,
                               MMshort *p_output,
                               void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell                       *p_biquad_coef                 = p_transducer_equalizer_struct->p_biquad_coef;
    MMshort                             *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    int                                 i;
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


    for(i = 0; i < nb_samples; i++)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
#ifdef TEQ_CA9_OPTIM_VERSION
        p_output[i] = transducer_equalizer_biquad_filter_mono_sample         (p_input[i],
                                                                              biquad_gain_exp_l,
                                                                              biquad_gain_mant_l,
                                                                              nb_biquad_cells,
                                                                              p_biquad_coef,
                                                                              p_biquad_mem);
#else /* TEQ_CA9_OPTIM_VERSION */
        p_output[i] = transducer_equalizer_biquad_filter_mono_sample_no_optim(p_input[i],
                                                                              biquad_gain_exp_l,
                                                                              biquad_gain_mant_l,
                                                                              nb_biquad_cells,
                                                                              p_biquad_coef,
                                                                              p_biquad_mem);
#endif /* TEQ_CA9_OPTIM_VERSION */
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 5)   // remove first 5 values (cache reason)
        {
            Tr_EQ_add_stat(&Tr_EQ_stat_biquad_mono[nb_biquad_cells - NB_BIQUAD_MIN], cycles);
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_biquad_filter_mono_sample (%d biquads) NEON CYCLES = %d\n", nb_biquad_cells, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}


static void FIR_filter_stereo_different_filter_l_r(int     nb_samples,
                                                   MMshort *p_input,
                                                   MMshort *p_output,
                                                   void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    MMshort                             *p_FIR_mem                     = p_transducer_equalizer_struct->p_FIR_mem;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    MMshort                             FIR_gain_exp_r                 = p_transducer_equalizer_struct->FIR_gain_exp_r;
    MMshort                             FIR_gain_mant_r                = p_transducer_equalizer_struct->FIR_gain_mant_r;
#ifdef TEQ_CA9_OPTIM_VERSION
    int                                 *p_FIR_index                   = &p_transducer_equalizer_struct->FIR_index;
    MMshort                             buf_tmp[2];
    int32x2_t                           FIR_gain_exp_32x2, FIR_gain_mant_32x2;
#endif /* TEQ_CA9_OPTIM_VERSION */
    int                                 i;
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


#ifdef TEQ_CA9_OPTIM_VERSION
    buf_tmp[0]         = FIR_gain_exp_l;
    buf_tmp[1]         = FIR_gain_exp_r;
    FIR_gain_exp_32x2  = vld1_s32((int32_t *) buf_tmp);
    buf_tmp[0]         = FIR_gain_mant_l;
    buf_tmp[1]         = FIR_gain_mant_r;
    FIR_gain_mant_32x2 = vld1_s32((int32_t *) buf_tmp);
#endif /* TEQ_CA9_OPTIM_VERSION */

    for(i = 0; i < nb_samples; i += 2)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
#ifdef TEQ_CA9_OPTIM_VERSION
        transducer_equalizer_different_FIR_filter_stereo_sample         (p_input  + i,
                                                                         p_output + i,
                                                                         FIR_gain_exp_32x2,
                                                                         FIR_gain_mant_32x2,
                                                                         nb_FIR_coefs,
                                                                         p_FIR_coef,
                                                                         p_FIR_mem,
                                                                         p_FIR_index);
#else /* TEQ_CA9_OPTIM_VERSION */
        transducer_equalizer_different_FIR_filter_stereo_sample_no_optim(p_input  + i,
                                                                         p_output + i,
                                                                         FIR_gain_exp_l,
                                                                         FIR_gain_mant_l,
                                                                         FIR_gain_exp_r,
                                                                         FIR_gain_mant_r,
                                                                         nb_FIR_coefs,
                                                                         p_FIR_coef,
                                                                         p_FIR_mem);
#endif /* TEQ_CA9_OPTIM_VERSION */
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 10)  // remove first 5 values (cache reason)
        {
#ifdef TEQ_CA9_OPTIM_VERSION
            if((i == 0) || (*p_FIR_index < 2 * (FIR_MEM_OVERHEAD - 1)))
            {
                Tr_EQ_add_stat(&Tr_EQ_stat_FIR_no_mem_shift_stereo2[nb_FIR_coefs - NB_FIR_COEF_MIN], cycles);
            }
            else
#endif /* TEQ_CA9_OPTIM_VERSION */
            {
                Tr_EQ_add_stat(&Tr_EQ_stat_FIR_mem_shift_stereo2[nb_FIR_coefs - NB_FIR_COEF_MIN], cycles);
            }
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_different_FIR_filter_stereo_sample (%d coefficients) NEON CYCLES = %d\n", nb_FIR_coefs, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}


static void FIR_filter_stereo_same_filter_l_r(int     nb_samples,
                                              MMshort *p_input,
                                              MMshort *p_output,
                                              void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    MMshort                             *p_FIR_mem                     = p_transducer_equalizer_struct->p_FIR_mem;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    int                                 i;
#ifdef TEQ_CA9_OPTIM_VERSION
    int                                 *p_FIR_index                   = &p_transducer_equalizer_struct->FIR_index;
#endif /* TEQ_CA9_OPTIM_VERSION */
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


    for(i = 0; i < nb_samples; i += 2)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
#ifdef TEQ_CA9_OPTIM_VERSION
        transducer_equalizer_same_FIR_filter_stereo_sample         (p_input  + i,
                                                                    p_output + i,
                                                                    FIR_gain_exp_l,
                                                                    FIR_gain_mant_l,
                                                                    nb_FIR_coefs,
                                                                    p_FIR_coef,
                                                                    p_FIR_mem,
                                                                    p_FIR_index);
#else /* TEQ_CA9_OPTIM_VERSION */
        transducer_equalizer_same_FIR_filter_stereo_sample_no_optim(p_input  + i,
                                                                    p_output + i,
                                                                    FIR_gain_exp_l,
                                                                    FIR_gain_mant_l,
                                                                    nb_FIR_coefs,
                                                                    p_FIR_coef,
                                                                    p_FIR_mem);
#endif /* TEQ_CA9_OPTIM_VERSION */
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 10)  // remove first 5 values (cache reason)
        {
#ifdef TEQ_CA9_OPTIM_VERSION
            if((i == 0) || (*p_FIR_index < 2 * (FIR_MEM_OVERHEAD - 1)))
            {
                Tr_EQ_add_stat(&Tr_EQ_stat_FIR_no_mem_shift_stereo1[nb_FIR_coefs - NB_FIR_COEF_MIN], cycles);
            }
            else
#endif /* TEQ_CA9_OPTIM_VERSION */
            {
                Tr_EQ_add_stat(&Tr_EQ_stat_FIR_mem_shift_stereo1[nb_FIR_coefs - NB_FIR_COEF_MIN], cycles);
            }
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_same_FIR_filter_stereo_sample (%d coefficients) NEON CYCLES = %d\n", nb_FIR_coefs, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}


static void FIR_filter_dual_mono_different_filter_l_r(int     nb_samples,
                                                      MMshort *p_input,
                                                      MMshort *p_output,
                                                      void    *p_void_transducer_equalizer_struct)
{
    assert(0);
}


static void FIR_filter_dual_mono_same_filter_l_r(int     nb_samples,
                                                 MMshort *p_input,
                                                 MMshort *p_output,
                                                 void    *p_void_transducer_equalizer_struct)
{
    assert(0);
}


static void FIR_filter_mono(int     nb_samples,
                            MMshort *p_input,
                            MMshort *p_output,
                            void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    MMshort                             *p_FIR_mem                     = p_transducer_equalizer_struct->p_FIR_mem;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    int                                 i;
#ifdef TEQ_CA9_OPTIM_VERSION
    int                                 *p_FIR_index                   = &p_transducer_equalizer_struct->FIR_index;
#endif /* TEQ_CA9_OPTIM_VERSION */
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    int                                 cycles;
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */


    for(i = 0; i < nb_samples; i++)
    {
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        PerfReset(1);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
#ifdef TEQ_CA9_OPTIM_VERSION
        p_output[i] = transducer_equalizer_FIR_filter_mono_sample         (p_input[i],
                                                                           FIR_gain_exp_l,
                                                                           FIR_gain_mant_l,
                                                                           nb_FIR_coefs,
                                                                           p_FIR_coef,
                                                                           p_FIR_mem,
                                                                           p_FIR_index);
#else /* TEQ_CA9_OPTIM_VERSION */
        p_output[i] = transducer_equalizer_FIR_filter_mono_sample_no_optim(p_input[i],
                                                                           FIR_gain_exp_l,
                                                                           FIR_gain_mant_l,
                                                                           nb_FIR_coefs,
                                                                           p_FIR_coef,
                                                                           p_FIR_mem);
#endif /* TEQ_CA9_OPTIM_VERSION */
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        cycles = PerfGetCount(0xff);
        if(i > 5)   // remove first 5 values (cache reason)
        {
#ifdef TEQ_CA9_OPTIM_VERSION
            if((i == 0) || (*p_FIR_index < FIR_MEM_OVERHEAD - 1))
            {
                Tr_EQ_add_stat(&Tr_EQ_stat_FIR_no_mem_shift_mono[nb_FIR_coefs - NB_FIR_COEF_MIN], cycles);
            }
            else
#endif /* TEQ_CA9_OPTIM_VERSION */
            {
                Tr_EQ_add_stat(&Tr_EQ_stat_FIR_mem_shift_mono[nb_FIR_coefs - NB_FIR_COEF_MIN], cycles);
            }
        }
#ifdef TEQ_CA9_PRINT_ALL_NEON_CYCLES
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "transducer_equalizer_FIR_filter_mono_sample (%d coefficients) NEON CYCLES = %d\n", nb_FIR_coefs, cycles);
        Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_NEON_CYCLES */
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
}



static void dummy_filter(int     nb_samples,
                         MMshort *p_input,
                         MMshort *p_output,
                         void    *p_void_transducer_equalizer_struct)
{
}


static void copy_filter(int     nb_samples,
                        MMshort *p_input,
                        MMshort *p_output,
                        void    *p_void_transducer_equalizer_struct)
{
#ifdef TEQ_CA9_OPTIM_VERSION

    FIR_mem_copy(p_output, p_input, nb_samples);

#else /* TEQ_CA9_OPTIM_VERSION */

    unsigned int i;

    for(i = (unsigned int) nb_samples; i > 0; i--)
    {
        *p_output++ = *p_input++;
    }

#endif /* TEQ_CA9_OPTIM_VERSION */
}



static int test_gain_0dB(MMshort gain_mant, MMshort gain_exp)
{
    return (gain_exp >= 0 ? ((((MMlonglong) gain_mant) <<   gain_exp)  == (1LL << 31))
                          : ((((MMlonglong) gain_mant) >> (-gain_exp)) == (1LL << 31)));
}



void transducer_equalizer_set_filters_pointer(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct)
{
    int      biquad_on, FIR_on;
    t_filter biquad_filter, FIR_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
    t_filter biquad_filter_no_optim, FIR_filter_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */


    biquad_on =    (p_transducer_equalizer_struct->nb_biquad_cells_per_channel > 0)
                || (!test_gain_0dB(p_transducer_equalizer_struct->biquad_gain_mant_l, p_transducer_equalizer_struct->biquad_gain_exp_l))
                || ((!p_transducer_equalizer_struct->same_biquad_l_r) && (!test_gain_0dB(p_transducer_equalizer_struct->biquad_gain_mant_r, p_transducer_equalizer_struct->biquad_gain_exp_r)));

    FIR_on    =    (p_transducer_equalizer_struct->nb_FIR_coefs_per_channel > 0)
                || (!test_gain_0dB(p_transducer_equalizer_struct->FIR_gain_mant_l, p_transducer_equalizer_struct->FIR_gain_exp_l))
                || ((!p_transducer_equalizer_struct->same_FIR_l_r) && (!test_gain_0dB(p_transducer_equalizer_struct->FIR_gain_mant_r, p_transducer_equalizer_struct->FIR_gain_exp_r)));

    if(p_transducer_equalizer_struct->stereo_process)
    {
        if(p_transducer_equalizer_struct->same_biquad_l_r)
        {
            biquad_filter = biquad_filter_stereo_same_filter_l_r;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            biquad_filter_no_optim = biquad_filter_stereo_same_filter_l_r_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
        }
        else
        {
            biquad_filter = biquad_filter_stereo_different_filter_l_r;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            biquad_filter_no_optim = biquad_filter_stereo_different_filter_l_r_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
        }
        if(p_transducer_equalizer_struct->same_FIR_l_r)
        {
            FIR_filter    = FIR_filter_stereo_same_filter_l_r;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            FIR_filter_no_optim    = FIR_filter_stereo_same_filter_l_r_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
        }
        else
        {
            FIR_filter    = FIR_filter_stereo_different_filter_l_r;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            FIR_filter_no_optim    = FIR_filter_stereo_different_filter_l_r_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
        }
    }
    else if(p_transducer_equalizer_struct->offset == 2)
    {
        if(p_transducer_equalizer_struct->same_biquad_l_r)
        {
            biquad_filter = biquad_filter_dual_mono_same_filter_l_r;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            biquad_filter_no_optim = biquad_filter_dual_mono_same_filter_l_r_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
        }
        else
        {
            biquad_filter = biquad_filter_dual_mono_different_filter_l_r;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            biquad_filter_no_optim = biquad_filter_dual_mono_different_filter_l_r_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
        }
        if(p_transducer_equalizer_struct->same_FIR_l_r)
        {
            FIR_filter    = FIR_filter_dual_mono_same_filter_l_r;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            FIR_filter_no_optim    = FIR_filter_dual_mono_same_filter_l_r_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
        }
        else
        {
            FIR_filter    = FIR_filter_dual_mono_different_filter_l_r;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            FIR_filter_no_optim    = FIR_filter_dual_mono_different_filter_l_r_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
        }
    }
    else
    {
        biquad_filter = biquad_filter_mono;
        FIR_filter    = FIR_filter_mono;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
        biquad_filter_no_optim = biquad_filter_mono_no_optim;
        FIR_filter_no_optim    = FIR_filter_mono_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
    }

    if(p_transducer_equalizer_struct->biquad_first)
    {
        if(biquad_on)
        {
            p_transducer_equalizer_struct->first_filter = biquad_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            p_transducer_equalizer_struct->first_filter_no_optim = biquad_filter_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
            if(FIR_on)
            {
                p_transducer_equalizer_struct->second_filter = FIR_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
                p_transducer_equalizer_struct->second_filter_no_optim = FIR_filter_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
            }
            else
            {
                p_transducer_equalizer_struct->second_filter = dummy_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
                p_transducer_equalizer_struct->second_filter_no_optim = dummy_filter;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
            }
        }
        else
        {
            if(FIR_on)
            {
                p_transducer_equalizer_struct->first_filter = FIR_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
                p_transducer_equalizer_struct->first_filter_no_optim = FIR_filter_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
            }
            else
            {
                p_transducer_equalizer_struct->first_filter = copy_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
                p_transducer_equalizer_struct->first_filter_no_optim = copy_filter;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
            }
            p_transducer_equalizer_struct->second_filter = dummy_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            p_transducer_equalizer_struct->second_filter_no_optim = dummy_filter;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
        }
    }
    else
    {
        if(FIR_on)
        {
            p_transducer_equalizer_struct->first_filter = FIR_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            p_transducer_equalizer_struct->first_filter_no_optim = FIR_filter_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
            if(biquad_on)
            {
                p_transducer_equalizer_struct->second_filter = biquad_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
                p_transducer_equalizer_struct->second_filter_no_optim = biquad_filter_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
            }
            else
            {
                p_transducer_equalizer_struct->second_filter = dummy_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
                p_transducer_equalizer_struct->second_filter_no_optim = dummy_filter;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
            }
        }
        else
        {
            if(biquad_on)
            {
                p_transducer_equalizer_struct->first_filter = biquad_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
                p_transducer_equalizer_struct->first_filter_no_optim = biquad_filter_no_optim;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
            }
            else
            {
                p_transducer_equalizer_struct->first_filter = copy_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
                p_transducer_equalizer_struct->first_filter_no_optim = copy_filter;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
            }
            p_transducer_equalizer_struct->second_filter = dummy_filter;
#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
            p_transducer_equalizer_struct->second_filter_no_optim = dummy_filter;
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */
        }
    }
}


void transducer_equalizer_filter(int                                 nb_samples,
                                 MMshort                             *p_input,
                                 MMshort                             *p_output,
                                 TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct)
{
    if(nb_samples > 0)
    {
        (*p_transducer_equalizer_struct->first_filter)  (nb_samples, p_input,  p_output, p_transducer_equalizer_struct);
        (*p_transducer_equalizer_struct->second_filter) (nb_samples, p_output, p_output, p_transducer_equalizer_struct);
    }
}


#if defined(__arm) && defined(TEQ_CA9_NO_OPTIM_VERSION)
void transducer_equalizer_filter_no_optim(int                                 nb_samples,
                                          MMshort                             *p_input,
                                          MMshort                             *p_output,
                                          TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct)
{
    if(nb_samples > 0)
    {
        (*p_transducer_equalizer_struct->first_filter_no_optim)  (nb_samples, p_input,  p_output, p_transducer_equalizer_struct);
        (*p_transducer_equalizer_struct->second_filter_no_optim) (nb_samples, p_output, p_output, p_transducer_equalizer_struct);
    }
}
#endif /* __arm && TEQ_CA9_NO_OPTIM_VERSION */




#ifdef TEQ_CA9_OPTIM_VERSION
void smooth_transition1_stereo(MMshort *p_input,
                               MMshort *p_output,
                               int     nb_samples,
                               int     coef_num_smooth_input,
                               int     coef_num_smooth_output,
                               int     shift_div_transition,
                               MMshort coef_div_transition,
                               MMshort transition_gain_mant_l,
                               int     transition_gain_exp_l,
                               MMshort transition_gain_mant_r,
                               int     transition_gain_exp_r)
{
    unsigned int i;
    int32_t      *p_input_int32, *p_output_int32;
    int32x2_t    input_32x2, output_32x2, coef_num_smooth_input_32x2, coef_num_smooth_output_32x2, transition_gain_mant_32x2;
    int32x2_t    coef_div_transition_32x2, tmp_low_32x2, tmp_high_32x2, one_32x2;
    int64x2_t    transition_gain_exp_64x2, shift_low_div_transition_64x2, shift_high_div_transition_64x2, tmp_64x2;


    coef_num_smooth_input_32x2     = vdup_n_s32(coef_num_smooth_input);
    coef_num_smooth_output_32x2    = vdup_n_s32(coef_num_smooth_output);
    transition_gain_mant_32x2      = vdup_n_s32(transition_gain_mant_l);
    transition_gain_mant_32x2      = vset_lane_s32(transition_gain_mant_r, transition_gain_mant_32x2, 1);
    transition_gain_exp_64x2       = vdupq_n_s64((int64_t) transition_gain_exp_l);
    transition_gain_exp_64x2       = vsetq_lane_s64((int64_t) transition_gain_exp_r, transition_gain_exp_64x2, 1);
    coef_div_transition_32x2       = vdup_n_s32(coef_div_transition);
    shift_low_div_transition_64x2  = vdupq_n_s64((int64_t) shift_div_transition);
    shift_high_div_transition_64x2 = vdupq_n_s64((int64_t) (shift_div_transition + 32));
    one_32x2                       = vdup_n_s32(1);
    p_input_int32                  = p_input;
    p_output_int32                 = p_output;
    for(i = (unsigned int) nb_samples; i > 0; i--)
    {
        input_32x2                  = vld1_s32(p_input_int32);
        p_input_int32              += 2;
        output_32x2                 = vld1_s32(p_output_int32);
        tmp_64x2                    = vqshlq_s64(vmull_s32(vqrdmulh_s32(coef_num_smooth_input_32x2, transition_gain_mant_32x2), input_32x2), transition_gain_exp_64x2);
        tmp_64x2                    = vmlal_s32(tmp_64x2, coef_num_smooth_output_32x2, output_32x2);
        coef_num_smooth_input_32x2  = vadd_s32(coef_num_smooth_input_32x2,  one_32x2);
        coef_num_smooth_output_32x2 = vsub_s32(coef_num_smooth_output_32x2, one_32x2);
        tmp_low_32x2                = vmovn_s64(tmp_64x2);
        tmp_high_32x2               = vqrshrn_n_s64(tmp_64x2, 32);
        tmp_64x2                    =                      vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_low_32x2),  shift_low_div_transition_64x2);
        tmp_64x2                    = vqaddq_s64(tmp_64x2, vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_high_32x2), shift_high_div_transition_64x2));
        output_32x2                 = vqrshrn_n_s64(tmp_64x2, 32);
        vst1_s32(p_output_int32, output_32x2);
        p_output_int32             += 2;
    }
}


void smooth_transition1_mono(MMshort *p_input,
                             MMshort *p_output,
                             int     nb_samples,
                             int     coef_num_smooth_input,
                             int     coef_num_smooth_output,
                             int     shift_div_transition,
                             MMshort coef_div_transition,
                             MMshort transition_gain_mant,
                             int     transition_gain_exp)
{
    unsigned int i;
    int32_t      *p_input_int32, *p_output_int32;
    int32x2_t    input_32x2, output_32x2, coef_num_smooth_input_32x2, coef_num_smooth_output_32x2, transition_gain_mant_32x2;
    int32x2_t    coef_div_transition_32x2, tmp_low_32x2, tmp_high_32x2, two_32x2;
    int64x2_t    transition_gain_exp_64x2, shift_low_div_transition_64x2, shift_high_div_transition_64x2, tmp_64x2;


    coef_num_smooth_input_32x2     = vdup_n_s32(coef_num_smooth_input);
    coef_num_smooth_input_32x2     = vset_lane_s32(coef_num_smooth_input  + 1, coef_num_smooth_input_32x2,  1);
    coef_num_smooth_output_32x2    = vdup_n_s32(coef_num_smooth_output);
    coef_num_smooth_output_32x2    = vset_lane_s32(coef_num_smooth_output - 1, coef_num_smooth_output_32x2, 1);
    transition_gain_mant_32x2      = vdup_n_s32(transition_gain_mant);
    transition_gain_exp_64x2       = vdupq_n_s64((int64_t) transition_gain_exp);
    coef_div_transition_32x2       = vdup_n_s32(coef_div_transition);
    shift_low_div_transition_64x2  = vdupq_n_s64((int64_t) shift_div_transition);
    shift_high_div_transition_64x2 = vdupq_n_s64((int64_t) (shift_div_transition + 32));
    two_32x2                       = vdup_n_s32(2);
    p_input_int32                  = p_input;
    p_output_int32                 = p_output;
    for(i = (unsigned int) (nb_samples >> 1); i > 0; i--)
    {
        input_32x2                  = vld1_s32(p_input_int32);
        p_input_int32              += 2;
        output_32x2                 = vld1_s32(p_output_int32);
        tmp_64x2                    = vqshlq_s64(vmull_s32(vqrdmulh_s32(coef_num_smooth_input_32x2, transition_gain_mant_32x2), input_32x2), transition_gain_exp_64x2);
        tmp_64x2                    = vmlal_s32(tmp_64x2, coef_num_smooth_output_32x2, output_32x2);
        coef_num_smooth_input_32x2  = vadd_s32(coef_num_smooth_input_32x2,  two_32x2);
        coef_num_smooth_output_32x2 = vsub_s32(coef_num_smooth_output_32x2, two_32x2);
        tmp_low_32x2                = vmovn_s64(tmp_64x2);
        tmp_high_32x2               = vqrshrn_n_s64(tmp_64x2, 32);
        tmp_64x2                    =                      vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_low_32x2),  shift_low_div_transition_64x2);
        tmp_64x2                    = vqaddq_s64(tmp_64x2, vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_high_32x2), shift_high_div_transition_64x2));
        output_32x2                 = vqrshrn_n_s64(tmp_64x2, 32);
        vst1_s32(p_output_int32, output_32x2);
        p_output_int32             += 2;
    }
    if(nb_samples & 1)
    {
        input_32x2                  = vld1_s32(p_input_int32);
        output_32x2                 = vld1_s32(p_output_int32);
        tmp_64x2                    = vqshlq_s64(vmull_s32(vqrdmulh_s32(coef_num_smooth_input_32x2, transition_gain_mant_32x2), input_32x2), transition_gain_exp_64x2);
        tmp_64x2                    = vmlal_s32(tmp_64x2, coef_num_smooth_output_32x2, output_32x2);
        tmp_low_32x2                = vmovn_s64(tmp_64x2);
        tmp_high_32x2               = vqrshrn_n_s64(tmp_64x2, 32);
        tmp_64x2                    =                      vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_low_32x2),  shift_low_div_transition_64x2);
        tmp_64x2                    = vqaddq_s64(tmp_64x2, vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_high_32x2), shift_high_div_transition_64x2));
        output_32x2                 = vqrshrn_n_s64(tmp_64x2, 32);
        vst1_lane_s32(p_output_int32, output_32x2, 0);
    }
}


void smooth_transition2_stereo(MMshort *p_input,
                               MMshort *p_output,
                               int     nb_samples,
                               int     coef_num_smooth_input,
                               int     coef_num_smooth_output,
                               int     shift_div_transition,
                               MMshort coef_div_transition,
                               MMshort transition_gain_mant_l,
                               int     transition_gain_exp_l,
                               MMshort transition_gain_mant_r,
                               int     transition_gain_exp_r)
{
    unsigned int i;
    int32_t      *p_input_int32, *p_output_int32;
    int32x2_t    input_32x2, output_32x2, coef_num_smooth_input_32x2, coef_num_smooth_output_32x2, transition_gain_mant_32x2;
    int32x2_t    coef_div_transition_32x2, tmp_low_32x2, tmp_high_32x2, one_32x2;
    int64x2_t    transition_gain_exp_64x2, shift_low_div_transition_64x2, shift_high_div_transition_64x2, tmp_64x2;


    coef_num_smooth_input_32x2     = vdup_n_s32(coef_num_smooth_input);
    coef_num_smooth_output_32x2    = vdup_n_s32(coef_num_smooth_output);
    transition_gain_mant_32x2      = vdup_n_s32(transition_gain_mant_l);
    transition_gain_mant_32x2      = vset_lane_s32(transition_gain_mant_r, transition_gain_mant_32x2, 1);
    transition_gain_exp_64x2       = vdupq_n_s64((int64_t) transition_gain_exp_l);
    transition_gain_exp_64x2       = vsetq_lane_s64((int64_t) transition_gain_exp_r, transition_gain_exp_64x2, 1);
    coef_div_transition_32x2       = vdup_n_s32(coef_div_transition);
    shift_low_div_transition_64x2  = vdupq_n_s64((int64_t) shift_div_transition);
    shift_high_div_transition_64x2 = vdupq_n_s64((int64_t) (shift_div_transition + 32));
    one_32x2                       = vdup_n_s32(1);
    p_input_int32                  = p_input;
    p_output_int32                 = p_output;
    for(i = (unsigned int) nb_samples; i > 0; i--)
    {
        input_32x2                  = vld1_s32(p_input_int32);
        p_input_int32              += 2;
        output_32x2                 = vld1_s32(p_output_int32);
        tmp_64x2                    = vqshlq_s64(vmull_s32(vqrdmulh_s32(coef_num_smooth_input_32x2, transition_gain_mant_32x2), input_32x2), transition_gain_exp_64x2);
        tmp_64x2                    = vmlal_s32(tmp_64x2, coef_num_smooth_output_32x2, output_32x2);
        coef_num_smooth_input_32x2  = vsub_s32(coef_num_smooth_input_32x2,  one_32x2);
        coef_num_smooth_output_32x2 = vadd_s32(coef_num_smooth_output_32x2, one_32x2);
        tmp_low_32x2                = vmovn_s64(tmp_64x2);
        tmp_high_32x2               = vqrshrn_n_s64(tmp_64x2, 32);
        tmp_64x2                    =                      vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_low_32x2),  shift_low_div_transition_64x2);
        tmp_64x2                    = vqaddq_s64(tmp_64x2, vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_high_32x2), shift_high_div_transition_64x2));
        output_32x2                 = vqrshrn_n_s64(tmp_64x2, 32);
        vst1_s32(p_output_int32, output_32x2);
        p_output_int32             += 2;
    }
}


void smooth_transition2_mono(MMshort *p_input,
                             MMshort *p_output,
                             int     nb_samples,
                             int     coef_num_smooth_input,
                             int     coef_num_smooth_output,
                             int     shift_div_transition,
                             MMshort coef_div_transition,
                             MMshort transition_gain_mant,
                             int     transition_gain_exp)
{
    unsigned int i;
    int32_t      *p_input_int32, *p_output_int32;
    int32x2_t    input_32x2, output_32x2, coef_num_smooth_input_32x2, coef_num_smooth_output_32x2, transition_gain_mant_32x2;
    int32x2_t    coef_div_transition_32x2, tmp_low_32x2, tmp_high_32x2, two_32x2;
    int64x2_t    transition_gain_exp_64x2, shift_low_div_transition_64x2, shift_high_div_transition_64x2, tmp_64x2;


    coef_num_smooth_input_32x2     = vdup_n_s32(coef_num_smooth_input);
    coef_num_smooth_input_32x2     = vset_lane_s32(coef_num_smooth_input  - 1, coef_num_smooth_input_32x2,  1);
    coef_num_smooth_output_32x2    = vdup_n_s32(coef_num_smooth_output);
    coef_num_smooth_output_32x2    = vset_lane_s32(coef_num_smooth_output + 1, coef_num_smooth_output_32x2, 1);
    transition_gain_mant_32x2      = vdup_n_s32(transition_gain_mant);
    transition_gain_exp_64x2       = vdupq_n_s64((int64_t) transition_gain_exp);
    coef_div_transition_32x2       = vdup_n_s32(coef_div_transition);
    shift_low_div_transition_64x2  = vdupq_n_s64((int64_t) shift_div_transition);
    shift_high_div_transition_64x2 = vdupq_n_s64((int64_t) (shift_div_transition + 32));
    two_32x2                       = vdup_n_s32(2);
    p_input_int32                  = p_input;
    p_output_int32                 = p_output;
    for(i = (unsigned int) (nb_samples >> 1); i > 0; i--)
    {
        input_32x2                  = vld1_s32(p_input_int32);
        p_input_int32              += 2;
        output_32x2                 = vld1_s32(p_output_int32);
        tmp_64x2                    = vqshlq_s64(vmull_s32(vqrdmulh_s32(coef_num_smooth_input_32x2, transition_gain_mant_32x2), input_32x2), transition_gain_exp_64x2);
        tmp_64x2                    = vmlal_s32(tmp_64x2, coef_num_smooth_output_32x2, output_32x2);
        coef_num_smooth_input_32x2  = vsub_s32(coef_num_smooth_input_32x2,  two_32x2);
        coef_num_smooth_output_32x2 = vadd_s32(coef_num_smooth_output_32x2, two_32x2);
        tmp_low_32x2                = vmovn_s64(tmp_64x2);
        tmp_high_32x2               = vqrshrn_n_s64(tmp_64x2, 32);
        tmp_64x2                    =                      vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_low_32x2),  shift_low_div_transition_64x2);
        tmp_64x2                    = vqaddq_s64(tmp_64x2, vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_high_32x2), shift_high_div_transition_64x2));
        output_32x2                 = vqrshrn_n_s64(tmp_64x2, 32);
        vst1_s32(p_output_int32, output_32x2);
        p_output_int32             += 2;
    }
    if(nb_samples & 1)
    {
        input_32x2                  = vld1_s32(p_input_int32);
        output_32x2                 = vld1_s32(p_output_int32);
        tmp_64x2                    = vqshlq_s64(vmull_s32(vqrdmulh_s32(coef_num_smooth_input_32x2, transition_gain_mant_32x2), input_32x2), transition_gain_exp_64x2);
        tmp_64x2                    = vmlal_s32(tmp_64x2, coef_num_smooth_output_32x2, output_32x2);
        tmp_low_32x2                = vmovn_s64(tmp_64x2);
        tmp_high_32x2               = vqrshrn_n_s64(tmp_64x2, 32);
        tmp_64x2                    =                      vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_low_32x2),  shift_low_div_transition_64x2);
        tmp_64x2                    = vqaddq_s64(tmp_64x2, vqshlq_s64(vqdmull_s32(coef_div_transition_32x2, tmp_high_32x2), shift_high_div_transition_64x2));
        output_32x2                 = vqrshrn_n_s64(tmp_64x2, 32);
        vst1_lane_s32(p_output_int32, output_32x2, 0);
    }
}
#endif /* TEQ_CA9_OPTIM_VERSION */

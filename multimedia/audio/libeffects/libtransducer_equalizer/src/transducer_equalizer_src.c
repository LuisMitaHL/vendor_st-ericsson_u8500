/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   transducer_equalizer_src.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "transducer_equalizer_include.h"


#ifdef __flexcc2__
#pragma inline
#endif /* __flexcc2__ */
static MMshort transducer_equalizer_biquad_filter_mono_sample(MMshort       in,
                                                              MMshort       biquad_gain_exp,
                                                              MMshort       biquad_gain_mant,
                                                              int           nb_biquad_cells,
                                                              t_biquad_cell *p_biquad_coef,
                                                              MMshort YMEM  *p_mem_biquad)
{
    MMlonglong accu;
    int        i;


    if(nb_biquad_cells > 0)
    {
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = 0; i < nb_biquad_cells; i++)
        {
            accu            =                     wX_fmul(p_biquad_coef->b0, in);
            accu            = wX_add(accu,        wX_fmul(p_biquad_coef->b1, p_mem_biquad[0]));
            accu            = wX_add(accu,        wX_fmul(p_biquad_coef->b2, p_mem_biquad[1]));
            accu            = wX_msl(accu,        p_biquad_coef->b_exp);
            p_mem_biquad[1] = p_mem_biquad[0];
            p_mem_biquad[0] = in;
            p_mem_biquad   += 2;
            accu            = wX_sub(accu, wX_msl(wX_fmul(p_biquad_coef->a1, p_mem_biquad[0]), 1));
            in              = wsubr (accu,        wX_fmul(p_biquad_coef->a2, p_mem_biquad[1]));
            p_biquad_coef++;
        }
        p_mem_biquad[1] = p_mem_biquad[0];
        p_mem_biquad[0] = in;
    }

    return wround_X(wX_msl(wX_fmul(in, biquad_gain_mant), biquad_gain_exp));
}


#ifdef __flexcc2__
#pragma inline
#endif /* __flexcc2__ */
static void transducer_equalizer_same_biquad_filter_stereo_sample(MMshort       *p_in,
                                                                  MMshort       *p_out,
                                                                  MMshort       biquad_gain_exp,
                                                                  MMshort       biquad_gain_mant,
                                                                  int           nb_biquad_cells,
                                                                  t_biquad_cell *p_biquad_coef,
                                                                  MMshort YMEM  *p_mem_biquad)
{
    MMlonglong accu_left, accu_right;
    MMshort    in_left  = p_in[0];
    MMshort    in_right = p_in[1];
    int        i;


    if(nb_biquad_cells > 0)
    {
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = 0; i < nb_biquad_cells; i++)
        {
            accu_left       =                           wX_fmul(p_biquad_coef->b0, in_left);
            accu_right      =                           wX_fmul(p_biquad_coef->b0, in_right);
            accu_left       = wX_add(accu_left,         wX_fmul(p_biquad_coef->b1, p_mem_biquad[0]));
            accu_right      = wX_add(accu_right,        wX_fmul(p_biquad_coef->b1, p_mem_biquad[1]));
            accu_left       = wX_add(accu_left,         wX_fmul(p_biquad_coef->b2, p_mem_biquad[2]));
            accu_right      = wX_add(accu_right,        wX_fmul(p_biquad_coef->b2, p_mem_biquad[3]));
            accu_left       = wX_msl(accu_left,         p_biquad_coef->b_exp);
            accu_right      = wX_msl(accu_right,        p_biquad_coef->b_exp);
            p_mem_biquad[2] = p_mem_biquad[0];
            p_mem_biquad[0] = in_left;
            p_mem_biquad[3] = p_mem_biquad[1];
            p_mem_biquad[1] = in_right;
            p_mem_biquad   += 4;
            accu_left       = wX_sub(accu_left,  wX_msl(wX_fmul(p_biquad_coef->a1, p_mem_biquad[0]), 1));
            accu_right      = wX_sub(accu_right, wX_msl(wX_fmul(p_biquad_coef->a1, p_mem_biquad[1]), 1));
            in_left         = wsubr (accu_left,         wX_fmul(p_biquad_coef->a2, p_mem_biquad[2]));
            in_right        = wsubr (accu_right,        wX_fmul(p_biquad_coef->a2, p_mem_biquad[3]));
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


#ifdef __flexcc2__
#pragma inline
#endif /* __flexcc2__ */
static void transducer_equalizer_same_biquad_filter_left_sample(MMshort       *p_in,
                                                                MMshort       *p_out,
                                                                MMshort       biquad_gain_exp,
                                                                MMshort       biquad_gain_mant,
                                                                int           nb_biquad_cells,
                                                                t_biquad_cell *p_biquad_coef,
                                                                MMshort YMEM  *p_mem_biquad)
{
    MMlonglong accu;
    MMshort    in = p_in[0];
    int        i;


    if(nb_biquad_cells > 0)
    {
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = 0; i < nb_biquad_cells; i++)
        {
            accu            =                     wX_fmul(p_biquad_coef->b0, in);
            accu            = wX_add(accu,        wX_fmul(p_biquad_coef->b1, p_mem_biquad[0]));
            accu            = wX_add(accu,        wX_fmul(p_biquad_coef->b2, p_mem_biquad[2]));
            accu            = wX_msl(accu,        p_biquad_coef->b_exp);
            p_mem_biquad[2] = p_mem_biquad[0];
            p_mem_biquad[0] = in;
            p_mem_biquad[3] = p_mem_biquad[1];
            p_mem_biquad[1] = 0;
            p_mem_biquad   += 4;
            accu            = wX_sub(accu, wX_msl(wX_fmul(p_biquad_coef->a1, p_mem_biquad[0]), 1));
            in              = wsubr (accu,        wX_fmul(p_biquad_coef->a2, p_mem_biquad[2]));
            p_biquad_coef++;
        }
        p_mem_biquad[2] = p_mem_biquad[0];
        p_mem_biquad[0] = in;
        p_mem_biquad[3] = p_mem_biquad[1];
        p_mem_biquad[1] = 0;
    }

    p_out[0] = wround_X(wX_msl(wX_fmul(in,  biquad_gain_mant), biquad_gain_exp));
    p_out[1] = 0;
}


#ifdef __flexcc2__
#pragma inline
#endif /* __flexcc2__ */
static void transducer_equalizer_different_biquad_filter_stereo_sample(MMshort          *p_in,
                                                                       MMshort          *p_out,
                                                                       MMshort          biquad_gain_exp_l,
                                                                       MMshort          biquad_gain_mant_l,
                                                                       MMshort          biquad_gain_exp_r,
                                                                       MMshort          biquad_gain_mant_r,
                                                                       int              nb_biquad_cells,
                                                                       t_biquad_cell_st *p_biquad_coef_st,
                                                                       MMshort YMEM     *p_mem_biquad)
{
    MMlonglong accu_left, accu_right;
    MMshort    in_left  = p_in[0];
    MMshort    in_right = p_in[1];
    int        i;


    if(nb_biquad_cells > 0)
    {
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = 0; i < nb_biquad_cells; i++)
        {
            accu_left       =                           wX_fmul(p_biquad_coef_st->b0_l, in_left);
            accu_right      =                           wX_fmul(p_biquad_coef_st->b0_r, in_right);
            accu_left       = wX_add(accu_left,         wX_fmul(p_biquad_coef_st->b1_l, p_mem_biquad[0]));
            accu_right      = wX_add(accu_right,        wX_fmul(p_biquad_coef_st->b1_r, p_mem_biquad[1]));
            accu_left       = wX_add(accu_left,         wX_fmul(p_biquad_coef_st->b2_l, p_mem_biquad[2]));
            accu_right      = wX_add(accu_right,        wX_fmul(p_biquad_coef_st->b2_r, p_mem_biquad[3]));
            accu_left       = wX_msl(accu_left,         p_biquad_coef_st->b_exp_l);
            accu_right      = wX_msl(accu_right,        p_biquad_coef_st->b_exp_r);
            p_mem_biquad[2] = p_mem_biquad[0];
            p_mem_biquad[0] = in_left;
            p_mem_biquad[3] = p_mem_biquad[1];
            p_mem_biquad[1] = in_right;
            p_mem_biquad   += 4;
            accu_left       = wX_sub(accu_left,  wX_msl(wX_fmul(p_biquad_coef_st->a1_l, p_mem_biquad[0]), 1));
            accu_right      = wX_sub(accu_right, wX_msl(wX_fmul(p_biquad_coef_st->a1_r, p_mem_biquad[1]), 1));
            in_left         = wsubr (accu_left,         wX_fmul(p_biquad_coef_st->a2_l, p_mem_biquad[2]));
            in_right        = wsubr (accu_right,        wX_fmul(p_biquad_coef_st->a2_r, p_mem_biquad[3]));
            p_biquad_coef_st++;
        }
        p_mem_biquad[2] = p_mem_biquad[0];
        p_mem_biquad[0] = in_left;
        p_mem_biquad[3] = p_mem_biquad[1];
        p_mem_biquad[1] = in_right;
    }

    p_out[0] = wround_X(wX_msl(wX_fmul(in_left,  biquad_gain_mant_l), biquad_gain_exp_l));
    p_out[1] = wround_X(wX_msl(wX_fmul(in_right, biquad_gain_mant_r), biquad_gain_exp_r));
}


#ifdef __flexcc2__
#pragma inline
#endif /* __flexcc2__ */
static void transducer_equalizer_different_biquad_filter_left_sample(MMshort          *p_in,
                                                                     MMshort          *p_out,
                                                                     MMshort          biquad_gain_exp,
                                                                     MMshort          biquad_gain_mant,
                                                                     int              nb_biquad_cells,
                                                                     t_biquad_cell_st *p_biquad_coef_st,
                                                                     MMshort YMEM     *p_mem_biquad)
{
    MMlonglong accu;
    MMshort    in = p_in[0];
    int        i;


    if(nb_biquad_cells > 0)
    {
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = 0; i < nb_biquad_cells; i++)
        {
            accu            =                     wX_fmul(p_biquad_coef_st->b0_l, in);
            accu            = wX_add(accu,        wX_fmul(p_biquad_coef_st->b1_l, p_mem_biquad[0]));
            accu            = wX_add(accu,        wX_fmul(p_biquad_coef_st->b2_l, p_mem_biquad[2]));
            accu            = wX_msl(accu,        p_biquad_coef_st->b_exp_l);
            p_mem_biquad[2] = p_mem_biquad[0];
            p_mem_biquad[0] = in;
            p_mem_biquad[3] = p_mem_biquad[1];
            p_mem_biquad[1] = 0;
            p_mem_biquad   += 4;
            accu            = wX_sub(accu, wX_msl(wX_fmul(p_biquad_coef_st->a1_l, p_mem_biquad[0]), 1));
            in              = wsubr (accu,        wX_fmul(p_biquad_coef_st->a2_l, p_mem_biquad[2]));
            p_biquad_coef_st++;
        }
        p_mem_biquad[2] = p_mem_biquad[0];
        p_mem_biquad[0] = in;
        p_mem_biquad[3] = p_mem_biquad[1];
        p_mem_biquad[1] = 0;
    }

    p_out[0] = wround_X(wX_msl(wX_fmul(in,  biquad_gain_mant), biquad_gain_exp));
    p_out[1] = 0;
}


#ifdef __flexcc2__
#pragma inline
static MMshort transducer_equalizer_FIR_filter_mono_sample(MMshort             in,
                                                           MMshort             FIR_gain_exp,
                                                           MMshort             FIR_gain_mant,
                                                           int                 nb_FIR_coefs,
                                                           MMshort             *p_FIR_coef,
                                                           MMshort __circ YMEM **p_p_mem_FIR)
{
    MMshort __circ YMEM *p_mem_FIR;
    MMlonglong          accu;
    int                 i;


    if(nb_FIR_coefs > 0)
    {
        (*p_p_mem_FIR)--;
        p_mem_FIR    = *p_p_mem_FIR;
        p_mem_FIR[0] = in;
        accu         = 0;
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
        for(i = 0; i < nb_FIR_coefs; i++)
        {
            accu = wX_add(accu, wX_fmul(*p_FIR_coef++, *p_mem_FIR++));
        }

        return wround_X(wX_msl(wX_fmul(wround_X(accu), FIR_gain_mant), FIR_gain_exp));
    }
    else
    {
        return wround_X(wX_msl(wX_fmul(in, FIR_gain_mant), FIR_gain_exp));
    }
}
#else /* __flexcc2__ */
static MMshort transducer_equalizer_FIR_filter_mono_sample(MMshort in,
                                                           MMshort FIR_gain_exp,
                                                           MMshort FIR_gain_mant,
                                                           int     nb_FIR_coefs,
                                                           MMshort *p_FIR_coef,
                                                           MMshort **p_p_mem_FIR)
{
    MMshort    *p_mem_FIR;
    MMlonglong accu;
    int        i;


    if(nb_FIR_coefs > 0)
    {
        p_mem_FIR  = *p_p_mem_FIR;
        for(i = nb_FIR_coefs - 1; i >= 1; i--)
        {
            p_mem_FIR[i] = p_mem_FIR[i - 1];
        }
        p_mem_FIR[0] = in;
        accu         = 0;
        for(i = 0; i < nb_FIR_coefs; i++)
        {
            accu = wX_add(accu, wX_fmul(*p_FIR_coef++, *p_mem_FIR++));
        }

        return wround_X(wX_msl(wX_fmul(wround_X(accu), FIR_gain_mant), FIR_gain_exp));
    }
    else
    {
        return wround_X(wX_msl(wX_fmul(in, FIR_gain_mant), FIR_gain_exp));
    }
}
#endif /* __flexcc2__ */


#ifdef __flexcc2__
#pragma inline
static void transducer_equalizer_same_FIR_filter_stereo_sample(MMshort             *p_in,
                                                               MMshort             *p_out,
                                                               MMshort             FIR_gain_exp,
                                                               MMshort             FIR_gain_mant,
                                                               int                 nb_FIR_coefs,
                                                               MMshort             *p_FIR_coef,
                                                               MMshort __circ YMEM **p_p_mem_FIR)
{
    MMshort __circ YMEM *p_mem_FIR;
    MMlonglong          accu_left, accu_right;
    MMshort             coef;
    int                 i;


    if(nb_FIR_coefs > 0)
    {
        *p_p_mem_FIR -= 2;
        p_mem_FIR    = *p_p_mem_FIR;
        p_mem_FIR[0] = p_in[0];
        p_mem_FIR[1] = p_in[1];
        accu_left    = 0;
        accu_right   = 0;
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
        for(i = 0; i < nb_FIR_coefs; i++)
        {
            coef       = *p_FIR_coef++;
            accu_left  = wX_add(accu_left,  wX_fmul(coef, *p_mem_FIR++));
            accu_right = wX_add(accu_right, wX_fmul(coef, *p_mem_FIR++));
        }

        p_out[0] = wround_X(wX_msl(wX_fmul(wround_X(accu_left),  FIR_gain_mant), FIR_gain_exp));
        p_out[1] = wround_X(wX_msl(wX_fmul(wround_X(accu_right), FIR_gain_mant), FIR_gain_exp));
    }
    else
    {
        p_out[0] = wround_X(wX_msl(wX_fmul(p_in[0], FIR_gain_mant), FIR_gain_exp));
        p_out[1] = wround_X(wX_msl(wX_fmul(p_in[1], FIR_gain_mant), FIR_gain_exp));
    }
}
#else /* __flexcc2__ */
static void transducer_equalizer_same_FIR_filter_stereo_sample(MMshort *p_in,
                                                               MMshort *p_out,
                                                               MMshort FIR_gain_exp,
                                                               MMshort FIR_gain_mant,
                                                               int     nb_FIR_coefs,
                                                               MMshort *p_FIR_coef,
                                                               MMshort **p_p_mem_FIR)
{
    MMshort    *p_mem_FIR;
    MMlonglong accu_left, accu_right;
    MMshort    coef;
    int        i;


    if(nb_FIR_coefs > 0)
    {
        p_mem_FIR  = *p_p_mem_FIR;
        for(i = 2 * nb_FIR_coefs - 1; i >= 2; i--)
        {
            p_mem_FIR[i] = p_mem_FIR[i - 2];
        }
        p_mem_FIR[0] = p_in[0];
        p_mem_FIR[1] = p_in[1];
        accu_left    = 0;
        accu_right   = 0;
        for(i = 0; i < nb_FIR_coefs; i++)
        {
            coef       = *p_FIR_coef++;
            accu_left  = wX_add(accu_left,  wX_fmul(coef, *p_mem_FIR++));
            accu_right = wX_add(accu_right, wX_fmul(coef, *p_mem_FIR++));
        }

        p_out[0] = wround_X(wX_msl(wX_fmul(wround_X(accu_left),  FIR_gain_mant), FIR_gain_exp));
        p_out[1] = wround_X(wX_msl(wX_fmul(wround_X(accu_right), FIR_gain_mant), FIR_gain_exp));
    }
    else
    {
        p_out[0] = wround_X(wX_msl(wX_fmul(p_in[0], FIR_gain_mant), FIR_gain_exp));
        p_out[1] = wround_X(wX_msl(wX_fmul(p_in[1], FIR_gain_mant), FIR_gain_exp));
    }
}
#endif /* __flexcc2__ */


#ifdef __flexcc2__
#pragma inline
static void transducer_equalizer_same_FIR_filter_left_sample(MMshort             *p_in,
                                                             MMshort             *p_out,
                                                             MMshort             FIR_gain_exp,
                                                             MMshort             FIR_gain_mant,
                                                             int                 nb_FIR_coefs,
                                                             MMshort             *p_FIR_coef,
                                                             MMshort __circ YMEM **p_p_mem_FIR)
{
    MMshort __circ YMEM *p_mem_FIR;
    MMlonglong          accu;
    int                 i;


    if(nb_FIR_coefs > 0)
    {
        *p_p_mem_FIR -= 2;
        p_mem_FIR    = *p_p_mem_FIR;
        p_mem_FIR[0] = p_in[0];
        p_mem_FIR[1] = 0;
        accu         = 0;
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
        for(i = 0; i < nb_FIR_coefs; i++)
        {
            accu = wX_add(accu, wX_fmul(*p_FIR_coef++, *p_mem_FIR++));
        }

        p_out[0] = wround_X(wX_msl(wX_fmul(wround_X(accu), FIR_gain_mant), FIR_gain_exp));
        p_out[1] = 0;
    }
    else
    {
        p_out[0] = wround_X(wX_msl(wX_fmul(p_in[0], FIR_gain_mant), FIR_gain_exp));
        p_out[1] = 0;
    }
}
#else /* __flexcc2__ */
static void transducer_equalizer_same_FIR_filter_left_sample(MMshort *p_in,
                                                             MMshort *p_out,
                                                             MMshort FIR_gain_exp,
                                                             MMshort FIR_gain_mant,
                                                             int     nb_FIR_coefs,
                                                             MMshort *p_FIR_coef,
                                                             MMshort **p_p_mem_FIR)
{
    MMshort    *p_mem_FIR;
    MMlonglong accu;
    int        i;


    if(nb_FIR_coefs > 0)
    {
        p_mem_FIR  = *p_p_mem_FIR;
        for(i = 2 * nb_FIR_coefs - 1; i >= 2; i--)
        {
            p_mem_FIR[i] = p_mem_FIR[i - 2];
        }
        p_mem_FIR[0] = p_in[0];
        p_mem_FIR[1] = 0;
        accu         = 0;
        for(i = 0; i < nb_FIR_coefs; i++)
        {
            accu = wX_add(accu, wX_fmul(*p_FIR_coef++, *p_mem_FIR++));
        }

        p_out[0] = wround_X(wX_msl(wX_fmul(wround_X(accu), FIR_gain_mant), FIR_gain_exp));
        p_out[1] = 0;
    }
    else
    {
        p_out[0] = wround_X(wX_msl(wX_fmul(p_in[0], FIR_gain_mant), FIR_gain_exp));
        p_out[1] = 0;
    }
}
#endif /* __flexcc2__ */


#ifdef __flexcc2__
#pragma inline
static void transducer_equalizer_different_FIR_filter_stereo_sample(MMshort             *p_in,
                                                                    MMshort             *p_out,
                                                                    MMshort             FIR_gain_exp_l,
                                                                    MMshort             FIR_gain_mant_l,
                                                                    MMshort             FIR_gain_exp_r,
                                                                    MMshort             FIR_gain_mant_r,
                                                                    int                 nb_FIR_coefs,
                                                                    MMshort             *p_FIR_coef,
                                                                    MMshort __circ YMEM **p_p_mem_FIR)
{
    MMshort __circ YMEM *p_mem_FIR;
    MMlonglong          accu_left, accu_right;
    int                 i;


    if(nb_FIR_coefs > 0)
    {
        *p_p_mem_FIR -= 2;
        p_mem_FIR    = *p_p_mem_FIR;

        //p_mem_FIR[0] = p_in[0];   // MMDSP compiler generates 4 cycles for the loop above
        //p_mem_FIR[1] = p_in[1];   // with these initialisations

        *p_mem_FIR++ = *p_in++;     // MMDSP compiler generates 2 cycles for the loop above
        *p_mem_FIR-- = *p_in--;     // with these initialisations

        accu_left    = 0;
        accu_right   = 0;
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
        for(i = 0; i < nb_FIR_coefs; i++)
        {
            accu_left  = wX_add(accu_left,  wX_fmul(*p_FIR_coef++, *p_mem_FIR++));
            accu_right = wX_add(accu_right, wX_fmul(*p_FIR_coef++, *p_mem_FIR++));
        }

        p_out[0] = wround_X(wX_msl(wX_fmul(wround_X(accu_left),  FIR_gain_mant_l), FIR_gain_exp_l));
        p_out[1] = wround_X(wX_msl(wX_fmul(wround_X(accu_right), FIR_gain_mant_r), FIR_gain_exp_r));
    }
    else
    {
        p_out[0] = wround_X(wX_msl(wX_fmul(p_in[0], FIR_gain_mant_l), FIR_gain_exp_l));
        p_out[1] = wround_X(wX_msl(wX_fmul(p_in[1], FIR_gain_mant_r), FIR_gain_exp_r));
    }
}
#else /* __flexcc2__ */
static void transducer_equalizer_different_FIR_filter_stereo_sample(MMshort *p_in,
                                                                    MMshort *p_out,
                                                                    MMshort FIR_gain_exp_l,
                                                                    MMshort FIR_gain_mant_l,
                                                                    MMshort FIR_gain_exp_r,
                                                                    MMshort FIR_gain_mant_r,
                                                                    int     nb_FIR_coefs,
                                                                    MMshort *p_FIR_coef,
                                                                    MMshort **p_p_mem_FIR)
{
    MMshort    *p_mem_FIR;
    MMlonglong accu_left, accu_right;
    int        i;


    if(nb_FIR_coefs > 0)
    {
        p_mem_FIR  = *p_p_mem_FIR;
        for(i = 2 * nb_FIR_coefs - 1; i >= 2; i--)
        {
            p_mem_FIR[i] = p_mem_FIR[i - 2];
        }
        p_mem_FIR[0] = p_in[0];
        p_mem_FIR[1] = p_in[1];
        accu_left    = 0;
        accu_right   = 0;
        for(i = 0; i < nb_FIR_coefs; i++)
        {
            accu_left  = wX_add(accu_left,  wX_fmul(*p_FIR_coef++, *p_mem_FIR++));
            accu_right = wX_add(accu_right, wX_fmul(*p_FIR_coef++, *p_mem_FIR++));
        }

        p_out[0] = wround_X(wX_msl(wX_fmul(wround_X(accu_left),  FIR_gain_mant_l), FIR_gain_exp_l));
        p_out[1] = wround_X(wX_msl(wX_fmul(wround_X(accu_right), FIR_gain_mant_r), FIR_gain_exp_r));
    }
    else
    {
        p_out[0] = wround_X(wX_msl(wX_fmul(p_in[0], FIR_gain_mant_l), FIR_gain_exp_l));
        p_out[1] = wround_X(wX_msl(wX_fmul(p_in[1], FIR_gain_mant_r), FIR_gain_exp_r));
    }
}
#endif /* __flexcc2__ */


#ifdef __flexcc2__
#pragma inline
static void transducer_equalizer_different_FIR_filter_left_sample(MMshort             *p_in,
                                                                  MMshort             *p_out,
                                                                  MMshort             FIR_gain_exp,
                                                                  MMshort             FIR_gain_mant,
                                                                  int                 nb_FIR_coefs,
                                                                  MMshort             *p_FIR_coef,
                                                                  MMshort __circ YMEM **p_p_mem_FIR)
{
    MMshort __circ YMEM *p_mem_FIR;
    MMlonglong          accu;
    int                 i;


    if(nb_FIR_coefs > 0)
    {
        *p_p_mem_FIR -= 2;
        p_mem_FIR    = *p_p_mem_FIR;
        p_mem_FIR[0] = p_in[0];
        p_mem_FIR[1] = 0;
        accu         = 0;
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
        for(i = 0; i < nb_FIR_coefs; i++)
        {
            accu = wX_add(accu, wX_fmul(*p_FIR_coef, *p_mem_FIR));
            p_FIR_coef += 2;
            p_mem_FIR  += 2;
        }

        p_out[0] = wround_X(wX_msl(wX_fmul(wround_X(accu), FIR_gain_mant), FIR_gain_exp));
        p_out[1] = 0;
    }
    else
    {
        p_out[0] = wround_X(wX_msl(wX_fmul(p_in[0], FIR_gain_mant), FIR_gain_exp));
        p_out[1] = 0;
    }
}
#else /* __flexcc2__ */
static void transducer_equalizer_different_FIR_filter_left_sample(MMshort *p_in,
                                                                  MMshort *p_out,
                                                                  MMshort FIR_gain_exp,
                                                                  MMshort FIR_gain_mant,
                                                                  int     nb_FIR_coefs,
                                                                  MMshort *p_FIR_coef,
                                                                  MMshort **p_p_mem_FIR)
{
    MMshort    *p_mem_FIR;
    MMlonglong accu;
    int        i;


    if(nb_FIR_coefs > 0)
    {
        p_mem_FIR  = *p_p_mem_FIR;
        for(i = 2 * nb_FIR_coefs - 1; i >= 2; i--)
        {
            p_mem_FIR[i] = p_mem_FIR[i - 2];
        }
        p_mem_FIR[0] = p_in[0];
        p_mem_FIR[1] = 0;
        accu         = 0;
        for(i = 0; i < nb_FIR_coefs; i++)
        {
            accu = wX_add(accu, wX_fmul(*p_FIR_coef, *p_mem_FIR));
            p_FIR_coef += 2;
            p_mem_FIR  += 2;
        }

        p_out[0] = wround_X(wX_msl(wX_fmul(wround_X(accu), FIR_gain_mant), FIR_gain_exp));
        p_out[1] = 0;
    }
    else
    {
        p_out[0] = wround_X(wX_msl(wX_fmul(p_in[0], FIR_gain_mant), FIR_gain_exp));
        p_out[1] = 0;
    }
}
#endif /* __flexcc2__ */


static void biquad_filter_stereo_same_filter_l_r(int     nb_samples,
                                                 MMshort *p_input,
                                                 MMshort *p_output,
                                                 void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell                       *p_biquad_coef                 = p_transducer_equalizer_struct->p_biquad_coef;
    MMshort YMEM                        *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    int                                 i;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i += 2)
    {
        transducer_equalizer_same_biquad_filter_stereo_sample(p_input  + i,
                                                              p_output + i,
                                                              biquad_gain_exp_l,
                                                              biquad_gain_mant_l,
                                                              nb_biquad_cells,
                                                              p_biquad_coef,
                                                              p_biquad_mem);
    }
}


static void biquad_filter_stereo_different_filter_l_r(int     nb_samples,
                                                      MMshort *p_input,
                                                      MMshort *p_output,
                                                      void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell_st                    *p_biquad_coef_st              = p_transducer_equalizer_struct->p_biquad_coef_st;
    MMshort YMEM                        *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    MMshort                             biquad_gain_exp_r              = p_transducer_equalizer_struct->biquad_gain_exp_r;
    MMshort                             biquad_gain_mant_r             = p_transducer_equalizer_struct->biquad_gain_mant_r;
    int                                 i;


#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i += 2)
    {
        transducer_equalizer_different_biquad_filter_stereo_sample(p_input,
                                                                   p_output,
                                                                   biquad_gain_exp_l,
                                                                   biquad_gain_mant_l,
                                                                   biquad_gain_exp_r,
                                                                   biquad_gain_mant_r,
                                                                   nb_biquad_cells,
                                                                   p_biquad_coef_st,
                                                                   p_biquad_mem);
        p_input  += 2;
        p_output += 2;
    }
}


static void biquad_filter_dual_mono_same_filter_l_r(int     nb_samples,
                                                    MMshort *p_input,
                                                    MMshort *p_output,
                                                    void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell                       *p_biquad_coef                 = p_transducer_equalizer_struct->p_biquad_coef;
    MMshort YMEM                        *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    int                                 i;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i += 2)
    {
        transducer_equalizer_same_biquad_filter_left_sample(p_input  + i,
                                                            p_output + i,
                                                            biquad_gain_exp_l,
                                                            biquad_gain_mant_l,
                                                            nb_biquad_cells,
                                                            p_biquad_coef,
                                                            p_biquad_mem);
    }
}


static void biquad_filter_dual_mono_different_filter_l_r(int     nb_samples,
                                                         MMshort *p_input,
                                                         MMshort *p_output,
                                                         void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell_st                    *p_biquad_coef_st              = p_transducer_equalizer_struct->p_biquad_coef_st;
    MMshort YMEM                        *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    int                                 i;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i += 2)
    {
        transducer_equalizer_different_biquad_filter_left_sample(p_input  + i,
                                                                 p_output + i,
                                                                 biquad_gain_exp_l,
                                                                 biquad_gain_mant_l,
                                                                 nb_biquad_cells,
                                                                 p_biquad_coef_st,
                                                                 p_biquad_mem);
    }
}


static void biquad_filter_mono(int     nb_samples,
                               MMshort *p_input,
                               MMshort *p_output,
                               void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    t_biquad_cell                       *p_biquad_coef                 = p_transducer_equalizer_struct->p_biquad_coef;
    MMshort YMEM                        *p_biquad_mem                  = p_transducer_equalizer_struct->p_biquad_mem;
    int                                 nb_biquad_cells                = p_transducer_equalizer_struct->nb_biquad_cells_per_channel;
    MMshort                             biquad_gain_exp_l              = p_transducer_equalizer_struct->biquad_gain_exp_l;
    MMshort                             biquad_gain_mant_l             = p_transducer_equalizer_struct->biquad_gain_mant_l;
    int                                 i;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i++)
    {
        p_output[i] = transducer_equalizer_biquad_filter_mono_sample(p_input[i],
                                                                     biquad_gain_exp_l,
                                                                     biquad_gain_mant_l,
                                                                     nb_biquad_cells,
                                                                     p_biquad_coef,
                                                                     p_biquad_mem);
    }
}


static void FIR_filter_stereo_same_filter_l_r(int     nb_samples,
                                              MMshort *p_input,
                                              MMshort *p_output,
                                              void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    int                                 i;


#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i += 2)
    {
        transducer_equalizer_same_FIR_filter_stereo_sample(p_input  + i,
                                                           p_output + i,
                                                           FIR_gain_exp_l,
                                                           FIR_gain_mant_l,
                                                           nb_FIR_coefs,
                                                           p_FIR_coef,
                                                           &p_transducer_equalizer_struct->p_FIR_mem);
    }
}


static void FIR_filter_stereo_different_filter_l_r(int     nb_samples,
                                                   MMshort *p_input,
                                                   MMshort *p_output,
                                                   void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    MMshort                             FIR_gain_exp_r                 = p_transducer_equalizer_struct->FIR_gain_exp_r;
    MMshort                             FIR_gain_mant_r                = p_transducer_equalizer_struct->FIR_gain_mant_r;
    int                                 i;


#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i += 2)
    {
        transducer_equalizer_different_FIR_filter_stereo_sample(p_input  + i,
                                                                p_output + i,
                                                                FIR_gain_exp_l,
                                                                FIR_gain_mant_l,
                                                                FIR_gain_exp_r,
                                                                FIR_gain_mant_r,
                                                                nb_FIR_coefs,
                                                                p_FIR_coef,
                                                                &p_transducer_equalizer_struct->p_FIR_mem);
    }
}


static void FIR_filter_dual_mono_same_filter_l_r(int     nb_samples,
                                                 MMshort *p_input,
                                                 MMshort *p_output,
                                                 void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    int                                 i;


#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i += 2)
    {
        transducer_equalizer_same_FIR_filter_left_sample(p_input  + i,
                                                         p_output + i,
                                                         FIR_gain_exp_l,
                                                         FIR_gain_mant_l,
                                                         nb_FIR_coefs,
                                                         p_FIR_coef,
                                                         &p_transducer_equalizer_struct->p_FIR_mem);
    }
}


static void FIR_filter_dual_mono_different_filter_l_r(int     nb_samples,
                                                      MMshort *p_input,
                                                      MMshort *p_output,
                                                      void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    int                                 i;


#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i += 2)
    {
        transducer_equalizer_different_FIR_filter_left_sample(p_input  + i,
                                                              p_output + i,
                                                              FIR_gain_exp_l,
                                                              FIR_gain_mant_l,
                                                              nb_FIR_coefs,
                                                              p_FIR_coef,
                                                              &p_transducer_equalizer_struct->p_FIR_mem);
    }
}


static void FIR_filter_mono(int     nb_samples,
                            MMshort *p_input,
                            MMshort *p_output,
                            void    *p_void_transducer_equalizer_struct)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) p_void_transducer_equalizer_struct;
    MMshort                             *p_FIR_coef                    = p_transducer_equalizer_struct->p_FIR_coef;
    int                                 nb_FIR_coefs                   = p_transducer_equalizer_struct->nb_FIR_coefs_per_channel;
    MMshort                             FIR_gain_exp_l                 = p_transducer_equalizer_struct->FIR_gain_exp_l;
    MMshort                             FIR_gain_mant_l                = p_transducer_equalizer_struct->FIR_gain_mant_l;
    int                                 i;


#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i++)
    {
        p_output[i] = transducer_equalizer_FIR_filter_mono_sample(p_input[i],
                                                                  FIR_gain_exp_l,
                                                                  FIR_gain_mant_l,
                                                                  nb_FIR_coefs,
                                                                  p_FIR_coef,
                                                                  &p_transducer_equalizer_struct->p_FIR_mem);
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
    int i;


#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_samples; i++)
    {
        p_output[i] = p_input[i];
    }
}



static int test_gain_0dB(MMshort gain_mant, MMshort gain_exp)
{
    return (wX_msl(wX_depmsp(gain_mant), gain_exp - 24) == (1LL << 23));
}



void transducer_equalizer_set_filters_pointer(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_transducer_equalizer_struct)
{
    int      biquad_on, FIR_on;
    t_filter biquad_filter, FIR_filter;


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
        }
        else
        {
            biquad_filter = biquad_filter_stereo_different_filter_l_r;
        }
        if(p_transducer_equalizer_struct->same_FIR_l_r)
        {
            FIR_filter    = FIR_filter_stereo_same_filter_l_r;
        }
        else
        {
            FIR_filter    = FIR_filter_stereo_different_filter_l_r;
        }
    }
    else if(p_transducer_equalizer_struct->offset == 2)
    {
        if(p_transducer_equalizer_struct->same_biquad_l_r)
        {
            biquad_filter = biquad_filter_dual_mono_same_filter_l_r;
        }
        else
        {
            biquad_filter = biquad_filter_dual_mono_different_filter_l_r;
        }
        if(p_transducer_equalizer_struct->same_FIR_l_r)
        {
            FIR_filter    = FIR_filter_dual_mono_same_filter_l_r;
        }
        else
        {
            FIR_filter    = FIR_filter_dual_mono_different_filter_l_r;
        }
    }
    else
    {
        biquad_filter     = biquad_filter_mono;
        FIR_filter        = FIR_filter_mono;
    }


    if(p_transducer_equalizer_struct->biquad_first)
    {
        if(biquad_on)
        {
            p_transducer_equalizer_struct->first_filter      = biquad_filter;
            if(FIR_on)
            {
                p_transducer_equalizer_struct->second_filter = FIR_filter;
            }
            else
            {
                p_transducer_equalizer_struct->second_filter = dummy_filter;
            }
        }
        else
        {
            if(FIR_on)
            {
                p_transducer_equalizer_struct->first_filter  = FIR_filter;
            }
            else
            {
                p_transducer_equalizer_struct->first_filter  = copy_filter;
            }
            p_transducer_equalizer_struct->second_filter     = dummy_filter;
        }
    }
    else
    {
        if(FIR_on)
        {
            p_transducer_equalizer_struct->first_filter      = FIR_filter;
            if(biquad_on)
            {
                p_transducer_equalizer_struct->second_filter = biquad_filter;
            }
            else
            {
                p_transducer_equalizer_struct->second_filter = dummy_filter;
            }
        }
        else
        {
            if(biquad_on)
            {
                p_transducer_equalizer_struct->first_filter  = biquad_filter;
            }
            else
            {
                p_transducer_equalizer_struct->first_filter  = copy_filter;
            }
            p_transducer_equalizer_struct->second_filter     = dummy_filter;
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

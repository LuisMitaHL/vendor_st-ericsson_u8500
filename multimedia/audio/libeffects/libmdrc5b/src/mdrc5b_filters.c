/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: none.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**********************************************************************/
/**
  \file mdrc5b_filters.c
  \brief FIR and IIR filtering part of the multi-band DRC
  \authors Menu Michel, Champsaur Ludovic
  \email michel.menu@stericsson.com, ludovic.champsaur@stericsson.com
  \date DEC 2,  2011
*/

#ifdef _NMF_MPC_
#include "libeffects/libmdrc5b.nmf"
#define free(ptr)    // free doesn't exist with NMF
#endif

#include "mdrc5b.h"

#ifdef __arm__
#define inline __inline
//#define inline __forceinline
#endif // __arm__


static void           get_biquad_cell                          (t_biquad_cell_int16   *p_biquad_cell_int16, t_biquad_cell        YMEM *p_biquad_cell);
static void           get_biquad_cell_Butter                   (t_biquad_cell_int16   *p_biquad_cell_int16, t_biquad_cell_Butter YMEM *p_biquad_cell);
static int            get_FIR                                  (t_FIR_int16           *p_FIR_int16,         t_FIR                     *p_FIR);

static void           mdrc_sub_bands_mono_biquad_filtering     (t_mdrc_filters *p_mdrc_filters, MMshort *p_input[MDRC_CHANNELS_MAX], MMshort *p_output[MDRC_CHANNELS_MAX][MDRC_BANDS_MAX], int nb_samples);
static void           mdrc_sub_bands_stereo_biquad_filtering   (t_mdrc_filters *p_mdrc_filters, MMshort *p_input[MDRC_CHANNELS_MAX], MMshort *p_output[MDRC_CHANNELS_MAX][MDRC_BANDS_MAX], int nb_samples);
static void           mdrc_sub_bands_mono_FIR_filtering        (t_mdrc_filters *p_mdrc_filters, MMshort *p_input[MDRC_CHANNELS_MAX], MMshort *p_output[MDRC_CHANNELS_MAX][MDRC_BANDS_MAX], int nb_samples);
static void           mdrc_sub_bands_stereo_FIR_filtering      (t_mdrc_filters *p_mdrc_filters, MMshort *p_input[MDRC_CHANNELS_MAX], MMshort *p_output[MDRC_CHANNELS_MAX][MDRC_BANDS_MAX], int nb_samples);

static MMshort inline mono_biquad_filtering                    (MMshort in, t_biquad_processing_ctx *p_biquad_processing_ctx);
static MMshort inline mono_biquad_filtering_ButterLP           (MMshort in, t_biquad_processing_ctx *p_biquad_processing_ctx);
static MMshort inline mono_biquad_filtering_ButterHP           (MMshort in, t_biquad_processing_ctx *p_biquad_processing_ctx);

static void    inline stereo_biquad_filtering                  (MMshort *p_in_left, MMshort *p_in_right, t_biquad_processing_ctx *p_biquad_processing_ctx);
static void    inline stereo_biquad_filtering_ButterLP         (MMshort *p_in_left, MMshort *p_in_right, t_biquad_processing_ctx *p_biquad_processing_ctx);
static void    inline stereo_biquad_filtering_ButterHP         (MMshort *p_in_left, MMshort *p_in_right, t_biquad_processing_ctx *p_biquad_processing_ctx);

static MMshort inline mono_FIR_filtering                       (t_FIR *p_FIR, MMshort YMEM __circ *p_mem_FIR);
static MMshort inline mono_symmetric_FIR_filtering             (t_FIR *p_FIR, MMshort YMEM __circ *p_mem_FIR);

static void    inline stereo_FIR_filtering                     (t_FIR *p_FIR, MMshort YMEM __circ *p_mem_FIR_left, MMshort YMEM __circ *p_mem_FIR_right, MMshort *p_output_left, MMshort *p_output_right);
static void    inline stereo_symmetric_FIR_filtering           (t_FIR *p_FIR, MMshort YMEM __circ *p_mem_FIR_left, MMshort YMEM __circ *p_mem_FIR_right, MMshort *p_output_left, MMshort *p_output_right);




static MMshort convert_lowhigh2MMshort(t_uint16 low, t_uint16 high)
{
#ifdef __arm__

    return  ((MMshort) (low & 0xFFFF)) | (((MMshort) high) << 16);

#else // __arm__

    MMlong tmp;

    tmp  = ((MMlong) wX_deplsp(low  & 0xFFFF)) << 16;
    tmp |= ((MMlong) wX_deplsp(high & 0xFFFF)) << 32;
#ifndef __flexcc2__
    tmp  = (tmp << 16) >> 16;
#endif /* !__flexcc2__ */

    return wround_L(tmp);

#endif // __arm__
}


static void get_biquad_cell(t_biquad_cell_int16 *p_biquad_cell_int16, t_biquad_cell YMEM *p_biquad_cell)
{
#ifdef __flexcc2__
    p_biquad_cell->b_exp = ((((MMshort) p_biquad_cell_int16->b_exp) << 8) >> 8);
#else /* __flexcc2__ */
    p_biquad_cell->b_exp = ((((MMshort) p_biquad_cell_int16->b_exp) << 16) >> 16);
#endif /* __flexcc2__ */
    p_biquad_cell->b0    = convert_lowhigh2MMshort(p_biquad_cell_int16->b0_low, p_biquad_cell_int16->b0_high);
    p_biquad_cell->b1    = convert_lowhigh2MMshort(p_biquad_cell_int16->b1_low, p_biquad_cell_int16->b1_high);
    p_biquad_cell->b2    = convert_lowhigh2MMshort(p_biquad_cell_int16->b2_low, p_biquad_cell_int16->b2_high);
    p_biquad_cell->a1    = convert_lowhigh2MMshort(p_biquad_cell_int16->a1_low, p_biquad_cell_int16->a1_high);
    p_biquad_cell->a2    = convert_lowhigh2MMshort(p_biquad_cell_int16->a2_low, p_biquad_cell_int16->a2_high);
#ifndef SAMPLES_24_BITS
    // in 32 bits/samples mode : 1 guard bit per biquad cell
    p_biquad_cell->b0    = (p_biquad_cell->b0 < 0x7FFFFFFF ? p_biquad_cell->b0 + 1 : p_biquad_cell->b0) >> 1;
    p_biquad_cell->b1    = (p_biquad_cell->b1 < 0x7FFFFFFF ? p_biquad_cell->b1 + 1 : p_biquad_cell->b1) >> 1;
    p_biquad_cell->b2    = (p_biquad_cell->b2 < 0x7FFFFFFF ? p_biquad_cell->b2 + 1 : p_biquad_cell->b2) >> 1;
#endif // !SAMPLES_24_BITS
}


static void get_biquad_cell_Butter(t_biquad_cell_int16 *p_biquad_cell_int16, t_biquad_cell_Butter YMEM *p_biquad_cell)
{
#ifdef __flexcc2__
    p_biquad_cell->b_exp = ((((MMshort) p_biquad_cell_int16->b_exp) << 8) >> 8);
#else /* __flexcc2__ */
    p_biquad_cell->b_exp = ((((MMshort) p_biquad_cell_int16->b_exp) << 16) >> 16);
#endif /* __flexcc2__ */
    p_biquad_cell->b0    = convert_lowhigh2MMshort(p_biquad_cell_int16->b0_low, p_biquad_cell_int16->b0_high);
    p_biquad_cell->a1    = convert_lowhigh2MMshort(p_biquad_cell_int16->a1_low, p_biquad_cell_int16->a1_high);
    p_biquad_cell->a2    = convert_lowhigh2MMshort(p_biquad_cell_int16->a2_low, p_biquad_cell_int16->a2_high);
#ifndef SAMPLES_24_BITS
    // in 32 bits/samples mode : 1 guard bit per biquad cell
    p_biquad_cell->b0    = (p_biquad_cell->b0 < 0x7FFFFFFF ? p_biquad_cell->b0 + 1 : p_biquad_cell->b0) >> 1;
#endif // !SAMPLES_24_BITS
}


static int get_FIR(t_FIR_int16 *p_FIR_int16, t_FIR *p_FIR)
{
    int i;

    p_FIR->order    = (MMshort)    p_FIR_int16->order;
#ifdef __flexcc2__
    p_FIR->gain_exp = ((((MMshort) p_FIR_int16->gain_exp) << 8) >> 8);
#else /* __flexcc2__ */
    p_FIR->gain_exp = ((((MMshort) p_FIR_int16->gain_exp) << 16) >> 16);
#ifndef SAMPLES_24_BITS
    // in 32 bits/samples mode : 4 guard bits for FIR
    p_FIR->gain_exp += 4;
#endif // !SAMPLES_24_BITS
#endif /* __flexcc2__ */

    for(i = 0; i < p_FIR->order; i++)
    {
        p_FIR->coef[i] = convert_lowhigh2MMshort(p_FIR_int16->coef[i].low, p_FIR_int16->coef[i].high);
#ifndef SAMPLES_24_BITS
        // in 32 bits/samples mode : 4 guard bits for FIR
        p_FIR->coef[i] = (p_FIR->coef[i]< 0x7FFFFFF8 ? p_FIR->coef[i] + (1 << 3) : p_FIR->coef[i]) >> 4;
#endif // !SAMPLES_24_BITS
    }

    return 0;
}



#ifdef __arm__

static MMshort inline mono_biquad_filtering_arm(MMshort in, unsigned int nb_biquad_cells, MMshort *p_mem_biquad, t_biquad_cell YMEM *p_biquad_cell)
{
    int32_t      *p_load_mem_int32, *p_store_mem_int32, *p_coef_int32;
    int32x2_t    in_32x2, mem_0_32x2, mem_1_32x2;
    int32x4_t    mem_0_32x4, mem_1_32x4;
    int32x4_t    coef_0_32x4, coef_1_32x4;
    int64x2_t    accu_0_64x2, accu_1_64x2;
    int64x1_t    accu_0_64x1, accu_1_64x1, zero_64x1;
    unsigned int i;


    //         (b0 + 2.b1.z^-1 + b2.z^-2).2^gain_exp
    // H(z) = ---------------------------------------
    //           1 + 2.a1.z^-1 + a2.z^-2
    zero_64x1         = vdup_n_s64(0);
    in_32x2           = vdup_n_s32(0);
    in_32x2           = vset_lane_s32(in, in_32x2, 1);                                                                  // 0, in=x(n)
    p_coef_int32      = (int32_t *) p_biquad_cell;
    p_load_mem_int32  = (int32_t *) p_mem_biquad;
    p_store_mem_int32 = p_load_mem_int32;
    mem_0_32x4        = vld1q_s32(p_load_mem_int32);                                                                    // x(n-1), x(n-2), y(n-1), y(n-2)
    p_load_mem_int32 += 4;
    for(i = nb_biquad_cells / 2; i > 0; i--)
    {
        // even biquad
        coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b_exp, b0, b1, b2
        p_coef_int32      += 4;
        coef_1_32x4        = vld1q_s32(p_coef_int32);                                                                   // a1, a2, next b_ext, next b0
        p_coef_int32      += 4;

        accu_0_64x2        = vmull_s32(             in_32x2,                   vget_low_s32 (coef_0_32x4));             // 0, b0*x(n)
        accu_0_64x2        = vmlal_s32(accu_0_64x2, vget_low_s32 (mem_0_32x4), vget_high_s32(coef_0_32x4));             // b1*x(n-1), b0*x(n)+b2*x(n-2)
        accu_1_64x2        = vmull_s32(             vget_high_s32(mem_0_32x4), vget_low_s32 (coef_1_32x4));             // a1*y(n-1), a2*y(n-2)
        accu_0_64x1        = vadd_s64 (             vget_low_s64(accu_0_64x2), vget_low_s64 (accu_0_64x2));             // 2*b1*x(n-1)
        accu_0_64x1        = vadd_s64 (             accu_0_64x1,               vget_high_s64(accu_0_64x2));             // 2*b1*x(n-1)+b0*x(n)+b2*x(n-2)
        accu_1_64x1        = vadd_s64 (             vget_low_s64(accu_1_64x2), vget_low_s64 (accu_1_64x2));             // 2*a1*y(n-1)
        accu_1_64x1        = vadd_s64 (             accu_1_64x1,               vget_high_s64(accu_1_64x2));             // 2*a1*y(n-1)+a2*y(n-2)
        // should be vget_low_s64(vmovl_s32(vget_low_s32(...))) but in fact vreinterpret_s64_s32(vget_low_s32(...))
        // works fine (because MSB are ignored in left shift) and take 1 instruction less
        //accu_0_64x1        = vrshl_s64(accu_0_64x1, vget_low_s64(vmovl_s32(vget_low_s32(coef_0_32x4))));                // (2*b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
        accu_0_64x1        = vrshl_s64(accu_0_64x1, vreinterpret_s64_s32(vget_low_s32(coef_0_32x4)));                   // (2*b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
        accu_0_64x1        = vqsub_s64(accu_0_64x1, accu_1_64x1);                                                       // ((2*b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

        mem_0_32x2         = vext_s32(in_32x2, vget_low_s32(mem_0_32x4), 1);                                            // x(n), x(n-1)

        in_32x2            = vqrshrn_n_s64(vcombine_s64(zero_64x1, accu_0_64x1), 31);

        // odd biquad
        mem_1_32x4         = vld1q_s32(p_load_mem_int32);                                                               // y(n-1), y(n-2), ..., ...
        p_load_mem_int32  += 4;
        coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b1, b2, a1, a2
        p_coef_int32      += 4;

        accu_0_64x2        = vmull_s32(             in_32x2,                   vget_high_s32(coef_1_32x4));             // 0, b0*x(n)
        accu_0_64x2        = vmlal_s32(accu_0_64x2, vget_high_s32(mem_0_32x4), vget_low_s32 (coef_0_32x4));             // b1*x(n-1), b0*x(n)+b2*x(n-2)
        accu_1_64x2        = vmull_s32(             vget_low_s32 (mem_1_32x4), vget_high_s32(coef_0_32x4));             // a1*y(n-1), a2*y(n-2)
        accu_0_64x1        = vadd_s64 (             vget_low_s64(accu_0_64x2), vget_low_s64 (accu_0_64x2));             // 2*b1*x(n-1)
        accu_0_64x1        = vadd_s64 (             accu_0_64x1,               vget_high_s64(accu_0_64x2));             // 2*b1*x(n-1)+b0*x(n)+b2*x(n-2)
        accu_1_64x1        = vadd_s64 (             vget_low_s64(accu_1_64x2), vget_low_s64 (accu_1_64x2));             // 2*a1*y(n-1)
        accu_1_64x1        = vadd_s64 (             accu_1_64x1,               vget_high_s64(accu_1_64x2));             // 2*a1*y(n-1)+a2*y(n-2)
        // should be vget_low_s64(vmovl_s32(vget_high_s32(...))) but in fact vreinterpret_s64_s32(vget_high_s32(...))
        // works fine (because MSB are ignored in left shift) and take 1 instruction less
        //accu_0_64x1        = vrshl_s64(accu_0_64x1, vget_low_s64(vmovl_s32(vget_high_s32(coef_1_32x4))));               // (2*b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
        accu_0_64x1        = vrshl_s64(accu_0_64x1, vreinterpret_s64_s32(vget_high_s32(coef_1_32x4)));                  // (2*b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
        accu_0_64x1        = vqsub_s64(accu_0_64x1, accu_1_64x1);                                                       // ((2*b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

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

        accu_0_64x2        = vmull_s32(             in_32x2,                   vget_low_s32(coef_0_32x4));              // 0, b0*x(n)
        accu_0_64x2        = vmlal_s32(accu_0_64x2, vget_low_s32(mem_0_32x4),  vget_high_s32(coef_0_32x4));             // b1*x(n-1), b0*x(n)+b2*x(n-2)
        accu_1_64x2        = vmull_s32(             vget_high_s32(mem_0_32x4), coef_32x2);                              // a1*y(n-1), a2*y(n-2)
        accu_0_64x1        = vadd_s64 (             vget_low_s64(accu_0_64x2), vget_low_s64 (accu_0_64x2));             // 2*b1*x(n-1)
        accu_0_64x1        = vadd_s64 (             accu_0_64x1,               vget_high_s64(accu_0_64x2));             // 2*b1*x(n-1)+b0*x(n)+b2*x(n-2)
        accu_1_64x1        = vadd_s64 (             vget_low_s64(accu_1_64x2), vget_low_s64 (accu_1_64x2));             // 2*a1*y(n-1)
        accu_1_64x1        = vadd_s64 (             accu_1_64x1,               vget_high_s64(accu_1_64x2));             // 2*a1*y(n-1)+a2*y(n-2)
        // should be vget_low_s64(vmovl_s32(vget_low_s32(...))) but in fact vreinterpret_s64_s32(vget_low_s32(...))
        // works fine (because MSB are ignored in left shift) and take 1 instruction less
        //accu_0_64x1        = vrshl_s64(accu_0_64x1, vget_low_s64(vmovl_s32(vget_low_s32(coef_0_32x4))));                // (2*b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
        accu_0_64x1        = vrshl_s64(accu_0_64x1, vreinterpret_s64_s32(vget_low_s32(coef_0_32x4)));                   // (2*b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
        accu_0_64x1        = vqsub_s64(accu_0_64x1, accu_1_64x1);                                                       // ((2*b1*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

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

#ifndef SAMPLES_24_BITS
    // in 32 bits/samples mode : 1 guard bit per biquad cell
    in_32x2 = vqshl_s32(in_32x2, vdup_n_s32(nb_biquad_cells));
#endif // !SAMPLES_24_BITS

    return vget_lane_s32(in_32x2, 1);
}

#endif // __arm__


static MMshort inline mono_biquad_filtering(MMshort in, t_biquad_processing_ctx *p_biquad_processing_ctx)
{
    int                nb_biquad_cells = p_biquad_processing_ctx->biquad_order;
    MMshort            *p_mem_biquad   = p_biquad_processing_ctx->p_biquad_mem[0];
    t_biquad_cell YMEM *p_biquad_cell  = (t_biquad_cell YMEM *) p_biquad_processing_ctx->p_biquad_cell_void;


#ifdef __arm__

    return mono_biquad_filtering_arm(in, (unsigned int) nb_biquad_cells, p_mem_biquad, p_biquad_cell);

#else // __arm__

    MMlonglong accu;
    int        i;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_BIQUAD_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_biquad_cells; i++)
    {
        //         (b0 + 2.b1.z^-1 + b2.z^-2).2^gain_exp
        // H(z) = ---------------------------------------
        //           1 + 2.a1.z^-1 + a2.z^-2
        accu            =                     wX_fmul(p_biquad_cell->b0, in);
        accu            = wX_add(accu, wX_msl(wX_fmul(p_biquad_cell->b1, p_mem_biquad[0]), 1));
        accu            = wX_add(accu,        wX_fmul(p_biquad_cell->b2, p_mem_biquad[1]));
        accu            = wX_msl(accu,        p_biquad_cell->b_exp);
        p_mem_biquad[1] = p_mem_biquad[0];
        p_mem_biquad[0] = in;
        p_mem_biquad   += 2;
        accu            = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->a1, p_mem_biquad[0]), 1));
        in              = wsubr (accu,        wX_fmul(p_biquad_cell->a2, p_mem_biquad[1]));
        p_biquad_cell++;
    }
    p_mem_biquad[1] = p_mem_biquad[0];
    p_mem_biquad[0] = in;

    return in;

#endif // __arm__
}


static void inline stereo_biquad_filtering(MMshort *p_in_left, MMshort *p_in_right, t_biquad_processing_ctx *p_biquad_processing_ctx)
{
    int                nb_biquad_cells     = p_biquad_processing_ctx->biquad_order;
    MMshort            *p_mem_biquad_left  = p_biquad_processing_ctx->p_biquad_mem[0];
    MMshort            *p_mem_biquad_right = p_biquad_processing_ctx->p_biquad_mem[1];
    t_biquad_cell YMEM *p_biquad_cell      = (t_biquad_cell YMEM *) p_biquad_processing_ctx->p_biquad_cell_void;
    MMshort            in_left             = *p_in_left;
    MMshort            in_right            = *p_in_right;


#ifdef __arm__

    *p_in_left  = mono_biquad_filtering_arm(in_left,  (unsigned int) nb_biquad_cells, p_mem_biquad_left,  p_biquad_cell);
    *p_in_right = mono_biquad_filtering_arm(in_right, (unsigned int) nb_biquad_cells, p_mem_biquad_right, p_biquad_cell);

#else // __arm__

    MMlonglong accu;
    int        i;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_BIQUAD_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_biquad_cells; i++)
    {
        //         (b0 + 2.b1.z^-1 + b2.z^-2).2^gain_exp
        // H(z) = ---------------------------------------
        //           1 + 2.a1.z^-1 + a2.z^-2
        accu                  =                     wX_fmul(p_biquad_cell->b0, in_left);
        accu                  = wX_add(accu, wX_msl(wX_fmul(p_biquad_cell->b1, p_mem_biquad_left[0]), 1));
        accu                  = wX_add(accu,        wX_fmul(p_biquad_cell->b2, p_mem_biquad_left[1]));
        accu                  = wX_msl(accu,        p_biquad_cell->b_exp);
        p_mem_biquad_left[1]  = p_mem_biquad_left[0];
        p_mem_biquad_left[0]  = in_left;
        p_mem_biquad_left    += 2;
        accu                  = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->a1, p_mem_biquad_left[0]), 1));
        in_left               = wsubr (accu,        wX_fmul(p_biquad_cell->a2, p_mem_biquad_left[1]));

        accu                  =                     wX_fmul(p_biquad_cell->b0, in_right);
        accu                  = wX_add(accu, wX_msl(wX_fmul(p_biquad_cell->b1, p_mem_biquad_right[0]), 1));
        accu                  = wX_add(accu,        wX_fmul(p_biquad_cell->b2, p_mem_biquad_right[1]));
        accu                  = wX_msl(accu,        p_biquad_cell->b_exp);
        p_mem_biquad_right[1] = p_mem_biquad_right[0];
        p_mem_biquad_right[0] = in_right;
        p_mem_biquad_right   += 2;
        accu                  = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->a1, p_mem_biquad_right[0]), 1));
        in_right              = wsubr (accu,        wX_fmul(p_biquad_cell->a2, p_mem_biquad_right[1]));

        p_biquad_cell++;
    }
    p_mem_biquad_left [1] = p_mem_biquad_left [0];
    p_mem_biquad_left [0] = in_left;
    p_mem_biquad_right[1] = p_mem_biquad_right[0];
    p_mem_biquad_right[0] = in_right;

    *p_in_left  = in_left;
    *p_in_right = in_right;

#endif // __arm__
}


#ifdef __arm__

static MMshort inline mono_biquad_filtering_ButterLP_arm(MMshort in, unsigned int nb_biquad_cells, MMshort *p_mem_biquad, t_biquad_cell_Butter YMEM *p_biquad_cell)
{
    int32_t      *p_load_mem_int32, *p_store_mem_int32, *p_coef_int32;
    int32x2_t    in_32x2, mem_0_32x2, mem_1_32x2;
    int32x4_t    mem_0_32x4, mem_1_32x4;
    int32x4_t    coef_0_32x4;
    int64x2_t    accu_0_64x2, accu_1_64x2;
    int64x1_t    accu_0_64x1, accu_1_64x1, zero_64x1;
    unsigned int i;


    //         (b0 + 2.b0.z^-1 + b0.z^-2).2^gain_exp
    // H(z) = ---------------------------------------
    //           1 + 2.a1.z^-1 + a2.z^-2
    zero_64x1         = vdup_n_s64(0);
    in_32x2           = vdup_n_s32(0);
    in_32x2           = vset_lane_s32(in, in_32x2, 1);                                                                  // 0, in=x(n)
    p_coef_int32      = (int32_t *) p_biquad_cell;
    p_load_mem_int32  = (int32_t *) p_mem_biquad;
    p_store_mem_int32 = p_load_mem_int32;
    mem_0_32x4        = vld1q_s32(p_load_mem_int32);                                                                    // x(n-1), x(n-2), y(n-1), y(n-2)
    p_load_mem_int32 += 4;
    for(i = nb_biquad_cells / 2; i > 0; i--)
    {
        // even biquad
        coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b_exp, b0, a1, a2
        p_coef_int32      += 4;

        accu_0_64x2        = vmull_s32     (             in_32x2,                    vget_low_s32 (coef_0_32x4));       // 0, b0*x(n)
        accu_0_64x2        = vmlal_lane_s32(accu_0_64x2, vget_low_s32 (mem_0_32x4),  vget_low_s32 (coef_0_32x4), 1);    // b0*x(n-1), b0*x(n)+b0*x(n-2)
        accu_1_64x2        = vmull_s32     (             vget_high_s32(mem_0_32x4),  vget_high_s32(coef_0_32x4));       // a1*y(n-1), a2*y(n-2)
        accu_0_64x1        = vadd_s64      (             vget_low_s64 (accu_0_64x2), vget_low_s64 (accu_0_64x2));       // 2*b0*x(n-1)
        accu_0_64x1        = vadd_s64      (             accu_0_64x1,                vget_high_s64(accu_0_64x2));       // 2*b0*x(n-1)+b0*x(n)+b0*x(n-2)
        accu_1_64x1        = vadd_s64      (             vget_low_s64 (accu_1_64x2), vget_low_s64 (accu_1_64x2));       // 2*a1*y(n-1)
        accu_1_64x1        = vadd_s64      (             accu_1_64x1,                vget_high_s64(accu_1_64x2));       // 2*a1*y(n-1)+a2*y(n-2)
        // should be vget_low_s64(vmovl_s32(vget_low_s32(...))) but in fact vreinterpret_s64_s32(vget_low_s32(...))
        // works fine (because MSB are ignored in left shift) and take 1 instruction less
        //accu_0_64x1        = vrshl_s64     (accu_0_64x1, vget_low_s64(vmovl_s32(vget_low_s32(coef_0_32x4))));           // (2*b0*x(n-1)+b0*x(n)+b0*x(n-2))<<b_exp
        accu_0_64x1        = vrshl_s64     (accu_0_64x1, vreinterpret_s64_s32(vget_low_s32(coef_0_32x4)));              // (2*b0*x(n-1)+b0*x(n)+b0*x(n-2))<<b_exp
        accu_0_64x1        = vqsub_s64     (accu_0_64x1, accu_1_64x1);                                                  // ((2*b0*x(n-1)+b0*x(n)+b0*x(n-2))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

        mem_0_32x2         = vext_s32      (in_32x2, vget_low_s32(mem_0_32x4), 1);                                      // x(n), x(n-1)

        in_32x2            = vqrshrn_n_s64 (vcombine_s64(zero_64x1, accu_0_64x1), 31);

        // odd biquad
        mem_1_32x4         = vld1q_s32(p_load_mem_int32);                                                               // y(n-1), y(n-2), ..., ...
        p_load_mem_int32  += 4;
        coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b1, b2, a1, a2
        p_coef_int32      += 4;

        accu_0_64x2        = vmull_s32     (             in_32x2,                    vget_low_s32 (coef_0_32x4));       // 0, b0*x(n)
        accu_0_64x2        = vmlal_lane_s32(accu_0_64x2, vget_high_s32(mem_0_32x4),  vget_low_s32 (coef_0_32x4), 1);    // b0*x(n-1), b0*x(n)+b2*x(n-2)
        accu_1_64x2        = vmull_s32     (             vget_low_s32 (mem_1_32x4),  vget_high_s32(coef_0_32x4));       // a1*y(n-1), a2*y(n-2)
        accu_0_64x1        = vadd_s64      (             vget_low_s64 (accu_0_64x2), vget_low_s64 (accu_0_64x2));       // 2*b0*x(n-1)
        accu_0_64x1        = vadd_s64      (             accu_0_64x1,                vget_high_s64(accu_0_64x2));       // 2*b0*x(n-1)+b0*x(n)+b2*x(n-2)
        accu_1_64x1        = vadd_s64      (             vget_low_s64 (accu_1_64x2), vget_low_s64 (accu_1_64x2));       // 2*a1*y(n-1)
        accu_1_64x1        = vadd_s64      (             accu_1_64x1,                vget_high_s64(accu_1_64x2));       // 2*a1*y(n-1)+a2*y(n-2)
        // should be vget_low_s64(vmovl_s32(vget_high_s32(...))) but in fact vreinterpret_s64_s32(vget_high_s32(...))
        // works fine (because MSB are ignored in left shift) and take 1 instruction less
        //accu_0_64x1        = vrshl_s64     (accu_0_64x1, vget_low_s64(vmovl_s32(vget_high_s32(coef_0_32x4))));          // (2*b0*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
        accu_0_64x1        = vrshl_s64     (accu_0_64x1, vreinterpret_s64_s32(vget_low_s32(coef_0_32x4)));              // (2*b0*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp
        accu_0_64x1        = vqsub_s64     (accu_0_64x1, accu_1_64x1);                                                  // ((2*b0*x(n-1)+b0*x(n)+b2*x(n-2))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

        mem_1_32x2         = vext_s32(in_32x2, vget_high_s32(mem_0_32x4), 1);                                           // x(n), x(n-1)
        vst1q_s32(p_store_mem_int32, vcombine_s32(mem_0_32x2, mem_1_32x2));
        p_store_mem_int32 += 4;

        in_32x2            = vqrshrn_n_s64(vcombine_s64(zero_64x1, accu_0_64x1), 31);

        mem_0_32x4         = mem_1_32x4;
    }

    if(nb_biquad_cells & 1)
    {
        // last biquad if odd number of biquads
        coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b_exp, b0, a1, a2

        accu_0_64x2        = vmull_s32     (             in_32x2,                    vget_low_s32 (coef_0_32x4));       // 0, b0*x(n)
        accu_0_64x2        = vmlal_lane_s32(accu_0_64x2, vget_low_s32 (mem_0_32x4),  vget_low_s32 (coef_0_32x4), 1);    // b0*x(n-1), b0*x(n)+b0*x(n-2)
        accu_1_64x2        = vmull_s32     (             vget_high_s32(mem_0_32x4),  vget_high_s32(coef_0_32x4));       // a1*y(n-1), a2*y(n-2)
        accu_0_64x1        = vadd_s64      (             vget_low_s64 (accu_0_64x2), vget_low_s64 (accu_0_64x2));       // 2*b0*x(n-1)
        accu_0_64x1        = vadd_s64      (             accu_0_64x1,                vget_high_s64(accu_0_64x2));       // 2*b0*x(n-1)+b0*x(n)+b0*x(n-2)
        accu_1_64x1        = vadd_s64      (             vget_low_s64 (accu_1_64x2), vget_low_s64 (accu_1_64x2));       // 2*a1*y(n-1)
        accu_1_64x1        = vadd_s64      (             accu_1_64x1,                vget_high_s64(accu_1_64x2));       // 2*a1*y(n-1)+a2*y(n-2)
        // should be vget_low_s64(vmovl_s32(vget_low_s32(...))) but in fact vreinterpret_s64_s32(vget_low_s32(...))
        // works fine (because MSB are ignored in left shift) and take 1 instruction less
        //accu_0_64x1        = vrshl_s64     (accu_0_64x1, vget_low_s64(vmovl_s32(vget_low_s32(coef_0_32x4))));           // (2*b0*x(n-1)+b0*x(n)+b0*x(n-2))<<b_exp
        accu_0_64x1        = vrshl_s64     (accu_0_64x1, vreinterpret_s64_s32(vget_low_s32(coef_0_32x4)));              // (2*b0*x(n-1)+b0*x(n)+b0*x(n-2))<<b_exp
        accu_0_64x1        = vqsub_s64     (accu_0_64x1, accu_1_64x1);                                                  // ((2*b0*x(n-1)+b0*x(n)+b0*x(n-2))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

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

#ifndef SAMPLES_24_BITS
    // in 32 bits/samples mode : 1 guard bit per biquad cell
    in_32x2 = vqshl_s32(in_32x2, vdup_n_s32(nb_biquad_cells));
#endif // !SAMPLES_24_BITS

    return vget_lane_s32(in_32x2, 1);
}

#endif // __arm__


static MMshort inline mono_biquad_filtering_ButterLP(MMshort in, t_biquad_processing_ctx *p_biquad_processing_ctx)
{
    int                       nb_biquad_cells = p_biquad_processing_ctx->biquad_order;
    MMshort                   *p_mem_biquad   = p_biquad_processing_ctx->p_biquad_mem[0];
    t_biquad_cell_Butter YMEM *p_biquad_cell  = (t_biquad_cell_Butter YMEM *) p_biquad_processing_ctx->p_biquad_cell_void;


#ifdef __arm__

    return mono_biquad_filtering_ButterLP_arm(in, (unsigned int) nb_biquad_cells, p_mem_biquad, p_biquad_cell);

#else // __arm__

    MMlonglong accu;
    int        i;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_BIQUAD_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_biquad_cells; i++)
    {
        //         (b0 + 2.b0.z^-1 + b0.z^-2).2^gain_exp
        // H(z) = ---------------------------------------
        //           1 + 2.a1.z^-1 + a2.z^-2
        accu            =                     wX_fmul(p_biquad_cell->b0, in);
        accu            = wX_add(accu, wX_msl(wX_fmul(p_biquad_cell->b0, p_mem_biquad[0]), 1));
        accu            = wX_add(accu,        wX_fmul(p_biquad_cell->b0, p_mem_biquad[1]));
        accu            = wX_msl(accu,        p_biquad_cell->b_exp);
        p_mem_biquad[1] = p_mem_biquad[0];
        p_mem_biquad[0] = in;
        p_mem_biquad   += 2;
        accu            = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->a1, p_mem_biquad[0]), 1));
        in              = wsubr (accu,        wX_fmul(p_biquad_cell->a2, p_mem_biquad[1]));
        p_biquad_cell++;
    }
    p_mem_biquad[1] = p_mem_biquad[0];
    p_mem_biquad[0] = in;

    return in;

#endif // __arm__
}


static void inline stereo_biquad_filtering_ButterLP(MMshort *p_in_left, MMshort *p_in_right, t_biquad_processing_ctx *p_biquad_processing_ctx)
{
    int                       nb_biquad_cells     = p_biquad_processing_ctx->biquad_order;
    MMshort                   *p_mem_biquad_left  = p_biquad_processing_ctx->p_biquad_mem[0];
    MMshort                   *p_mem_biquad_right = p_biquad_processing_ctx->p_biquad_mem[1];
    t_biquad_cell_Butter YMEM *p_biquad_cell      = (t_biquad_cell_Butter YMEM *) p_biquad_processing_ctx->p_biquad_cell_void;
    MMshort                   in_left             = *p_in_left;
    MMshort                   in_right            = *p_in_right;


#ifdef __arm__

    *p_in_left  = mono_biquad_filtering_ButterLP_arm(in_left,  (unsigned int) nb_biquad_cells, p_mem_biquad_left,  p_biquad_cell);
    *p_in_right = mono_biquad_filtering_ButterLP_arm(in_right, (unsigned int) nb_biquad_cells, p_mem_biquad_right, p_biquad_cell);

#else // __arm__

    MMlonglong accu;
    int        i;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_BIQUAD_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_biquad_cells; i++)
    {
        //         (b0 + 2.b0.z^-1 + b0.z^-2).2^gain_exp
        // H(z) = ---------------------------------------
        //           1 + 2.a1.z^-1 + a2.z^-2
        accu                  =                     wX_fmul(p_biquad_cell->b0, in_left);
        accu                  = wX_add(accu, wX_msl(wX_fmul(p_biquad_cell->b0, p_mem_biquad_left[0]), 1));
        accu                  = wX_add(accu,        wX_fmul(p_biquad_cell->b0, p_mem_biquad_left[1]));
        accu                  = wX_msl(accu,        p_biquad_cell->b_exp);
        p_mem_biquad_left[1]  = p_mem_biquad_left[0];
        p_mem_biquad_left[0]  = in_left;
        p_mem_biquad_left    += 2;
        accu                  = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->a1, p_mem_biquad_left[0]), 1));
        in_left               = wsubr (accu,        wX_fmul(p_biquad_cell->a2, p_mem_biquad_left[1]));

        accu                  =                     wX_fmul(p_biquad_cell->b0, in_right);
        accu                  = wX_add(accu, wX_msl(wX_fmul(p_biquad_cell->b0, p_mem_biquad_right[0]), 1));
        accu                  = wX_add(accu,        wX_fmul(p_biquad_cell->b0, p_mem_biquad_right[1]));
        accu                  = wX_msl(accu,        p_biquad_cell->b_exp);
        p_mem_biquad_right[1] = p_mem_biquad_right[0];
        p_mem_biquad_right[0] = in_right;
        p_mem_biquad_right   += 2;
        accu                  = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->a1, p_mem_biquad_right[0]), 1));
        in_right              = wsubr (accu,        wX_fmul(p_biquad_cell->a2, p_mem_biquad_right[1]));

        p_biquad_cell++;
    }
    p_mem_biquad_left [1] = p_mem_biquad_left [0];
    p_mem_biquad_left [0] = in_left;
    p_mem_biquad_right[1] = p_mem_biquad_right[0];
    p_mem_biquad_right[0] = in_right;

    *p_in_left  = in_left;
    *p_in_right = in_right;

#endif // __arm__
}


#ifdef __arm__

static MMshort inline mono_biquad_filtering_ButterHP_arm(MMshort in, unsigned int nb_biquad_cells, MMshort *p_mem_biquad, t_biquad_cell_Butter YMEM *p_biquad_cell)
{
    int32_t      *p_load_mem_int32, *p_store_mem_int32, *p_coef_int32;
    int32x2_t    in_32x2, mem_0_32x2, mem_1_32x2;
    int32x4_t    mem_0_32x4, mem_1_32x4;
    int32x4_t    coef_0_32x4;
    int64x2_t    accu_0_64x2, accu_1_64x2;
    int64x1_t    accu_0_64x1, accu_1_64x1, zero_64x1;
    unsigned int i;


    //         (b0 - 2.b0.z^-1 + b0.z^-2).2^gain_exp
    // H(z) = ---------------------------------------
    //           1 + 2.a1.z^-1 + a2.z^-2
    zero_64x1         = vdup_n_s64(0);
    in_32x2           = vdup_n_s32(0);
    in_32x2           = vset_lane_s32(in, in_32x2, 1);                                                                  // 0, in=x(n)
    p_coef_int32      = (int32_t *) p_biquad_cell;
    p_load_mem_int32  = (int32_t *) p_mem_biquad;
    p_store_mem_int32 = p_load_mem_int32;
    mem_0_32x4        = vld1q_s32(p_load_mem_int32);                                                                    // x(n-1), x(n-2), y(n-1), y(n-2)
    p_load_mem_int32 += 4;
    for(i = nb_biquad_cells / 2; i > 0; i--)
    {
        // even biquad
        coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b_exp, b0, a1, a2
        p_coef_int32      += 4;

        accu_0_64x2        = vmull_s32     (             in_32x2,                    vget_low_s32 (coef_0_32x4));       // 0, b0*x(n)
        accu_0_64x2        = vmlal_lane_s32(accu_0_64x2, vget_low_s32 (mem_0_32x4),  vget_low_s32 (coef_0_32x4), 1);    // b0*x(n-1), b0*x(n)+b0*x(n-2)
        accu_1_64x2        = vmull_s32     (             vget_high_s32(mem_0_32x4),  vget_high_s32(coef_0_32x4));       // a1*y(n-1), a2*y(n-2)
        accu_0_64x1        = vadd_s64      (             vget_low_s64 (accu_0_64x2), vget_low_s64 (accu_0_64x2));       // 2*b0*x(n-1)
        accu_0_64x1        = vsub_s64      (             vget_high_s64(accu_0_64x2), accu_0_64x1);                      // b0*x(n)+b0*x(n-2)-2*b0*x(n-1)
        accu_1_64x1        = vadd_s64      (             vget_low_s64 (accu_1_64x2), vget_low_s64 (accu_1_64x2));       // 2*a1*y(n-1)
        accu_1_64x1        = vadd_s64      (             accu_1_64x1,                vget_high_s64(accu_1_64x2));       // 2*a1*y(n-1)+a2*y(n-2)
        // should be vget_low_s64(vmovl_s32(vget_low_s32(...))) but in fact vreinterpret_s64_s32(vget_low_s32(...))
        // works fine (because MSB are ignored in left shift) and take 1 instruction less
        //accu_0_64x1        = vrshl_s64     (accu_0_64x1, vget_low_s64(vmovl_s32(vget_low_s32(coef_0_32x4))));           // (b0*x(n)+b0*x(n-2)-2*b0*x(n-1))<<b_exp
        accu_0_64x1        = vrshl_s64     (accu_0_64x1, vreinterpret_s64_s32(vget_low_s32(coef_0_32x4)));              // (b0*x(n)+b0*x(n-2)-2*b0*x(n-1))<<b_exp
        accu_0_64x1        = vqsub_s64     (accu_0_64x1, accu_1_64x1);                                                  // ((b0*x(n)+b0*x(n-2)-2*b0*x(n-1))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

        mem_0_32x2         = vext_s32      (in_32x2, vget_low_s32(mem_0_32x4), 1);                                      // x(n), x(n-1)

        in_32x2            = vqrshrn_n_s64 (vcombine_s64(zero_64x1, accu_0_64x1), 31);

        // odd biquad
        mem_1_32x4         = vld1q_s32(p_load_mem_int32);                                                               // y(n-1), y(n-2), ..., ...
        p_load_mem_int32  += 4;
        coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b1, b2, a1, a2
        p_coef_int32      += 4;

        accu_0_64x2        = vmull_s32     (             in_32x2,                    vget_low_s32 (coef_0_32x4));       // 0, b0*x(n)
        accu_0_64x2        = vmlal_lane_s32(accu_0_64x2, vget_high_s32(mem_0_32x4),  vget_low_s32 (coef_0_32x4), 1);    // b0*x(n-1), b0*x(n)+b2*x(n-2)
        accu_1_64x2        = vmull_s32     (             vget_low_s32 (mem_1_32x4),  vget_high_s32(coef_0_32x4));       // a1*y(n-1), a2*y(n-2)
        accu_0_64x1        = vadd_s64      (             vget_low_s64 (accu_0_64x2), vget_low_s64 (accu_0_64x2));       // 2*b0*x(n-1)
        accu_0_64x1        = vsub_s64      (             vget_high_s64(accu_0_64x2), accu_0_64x1);                      // b0*x(n)+b0*x(n-2)-2*b0*x(n-1)
        accu_1_64x1        = vadd_s64      (             vget_low_s64 (accu_1_64x2), vget_low_s64 (accu_1_64x2));       // 2*a1*y(n-1)
        accu_1_64x1        = vadd_s64      (             accu_1_64x1,                vget_high_s64(accu_1_64x2));       // 2*a1*y(n-1)+a2*y(n-2)
        // should be vget_low_s64(vmovl_s32(vget_high_s32(...))) but in fact vreinterpret_s64_s32(vget_high_s32(...))
        // works fine (because MSB are ignored in left shift) and take 1 instruction less
        //accu_0_64x1        = vrshl_s64     (accu_0_64x1, vget_low_s64(vmovl_s32(vget_high_s32(coef_0_32x4))));          // (b0*x(n)+b2*x(n-2)-2*b0*x(n-1))<<b_exp
        accu_0_64x1        = vrshl_s64     (accu_0_64x1, vreinterpret_s64_s32(vget_low_s32(coef_0_32x4)));             // (b0*x(n)+b2*x(n-2)-2*b0*x(n-1))<<b_exp
        accu_0_64x1        = vqsub_s64     (accu_0_64x1, accu_1_64x1);                                                  // ((b0*x(n)+b2*x(n-2)-2*b0*x(n-1))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

        mem_1_32x2         = vext_s32(in_32x2, vget_high_s32(mem_0_32x4), 1);                                           // x(n), x(n-1)
        vst1q_s32(p_store_mem_int32, vcombine_s32(mem_0_32x2, mem_1_32x2));
        p_store_mem_int32 += 4;

        in_32x2            = vqrshrn_n_s64(vcombine_s64(zero_64x1, accu_0_64x1), 31);

        mem_0_32x4         = mem_1_32x4;
    }

    if(nb_biquad_cells & 1)
    {
        // last biquad if odd number of biquads
        coef_0_32x4        = vld1q_s32(p_coef_int32);                                                                   // b_exp, b0, a1, a2

        accu_0_64x2        = vmull_s32     (             in_32x2,                    vget_low_s32 (coef_0_32x4));       // 0, b0*x(n)
        accu_0_64x2        = vmlal_lane_s32(accu_0_64x2, vget_low_s32 (mem_0_32x4),  vget_low_s32 (coef_0_32x4), 1);    // b0*x(n-1), b0*x(n)+b0*x(n-2)
        accu_1_64x2        = vmull_s32     (             vget_high_s32(mem_0_32x4),  vget_high_s32(coef_0_32x4));       // a1*y(n-1), a2*y(n-2)
        accu_0_64x1        = vadd_s64      (             vget_low_s64 (accu_0_64x2), vget_low_s64 (accu_0_64x2));       // 2*b0*x(n-1)
        accu_0_64x1        = vsub_s64      (             vget_high_s64(accu_0_64x2), accu_0_64x1);                      // b0*x(n)+b0*x(n-2)-2*b0*x(n-1)
        accu_1_64x1        = vadd_s64      (             vget_low_s64 (accu_1_64x2), vget_low_s64 (accu_1_64x2));       // 2*a1*y(n-1)
        accu_1_64x1        = vadd_s64      (             accu_1_64x1,                vget_high_s64(accu_1_64x2));       // 2*a1*y(n-1)+a2*y(n-2)
        // should be vget_low_s64(vmovl_s32(vget_low_s32(...))) but in fact vreinterpret_s64_s32(vget_low_s32(...))
        // works fine (because MSB are ignored in left shift) and take 1 instruction less
        //accu_0_64x1        = vrshl_s64     (accu_0_64x1, vget_low_s64(vmovl_s32(vget_low_s32(coef_0_32x4))));           // (b0*x(n)+b0*x(n-2)-2*b0*x(n-1))<<b_exp
        accu_0_64x1        = vrshl_s64     (accu_0_64x1, vreinterpret_s64_s32(vget_low_s32(coef_0_32x4)));              // (b0*x(n)+b0*x(n-2)-2*b0*x(n-1))<<b_exp
        accu_0_64x1        = vqsub_s64     (accu_0_64x1, accu_1_64x1);                                                  // ((b0*x(n)+b0*x(n-2)-2*b0*x(n-1))<<b_exp)-2*a1*y(n-1)-a2*y(n-2)

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

#ifndef SAMPLES_24_BITS
    // in 32 bits/samples mode : 1 guard bit per biquad cell
    in_32x2 = vqshl_s32(in_32x2, vdup_n_s32(nb_biquad_cells));
#endif // !SAMPLES_24_BITS

    return vget_lane_s32(in_32x2, 1);
}

#endif // __arm__


static MMshort inline mono_biquad_filtering_ButterHP(MMshort in, t_biquad_processing_ctx *p_biquad_processing_ctx)
{
    int                       nb_biquad_cells = p_biquad_processing_ctx->biquad_order;
    MMshort                   *p_mem_biquad   = p_biquad_processing_ctx->p_biquad_mem[0];
    t_biquad_cell_Butter YMEM *p_biquad_cell  = (t_biquad_cell_Butter YMEM *) p_biquad_processing_ctx->p_biquad_cell_void;


#ifdef __arm__

    return mono_biquad_filtering_ButterHP_arm(in, (unsigned int) nb_biquad_cells, p_mem_biquad, p_biquad_cell);

#else // __arm__

    MMlonglong accu;
    int        i;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_BIQUAD_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_biquad_cells; i++)
    {
        //         (b0 - 2.b0.z^-1 + b0.z^-2).2^gain_exp
        // H(z) = ---------------------------------------
        //           1 + 2.a1.z^-1 + a2.z^-2
        accu            =                     wX_fmul(p_biquad_cell->b0, in);
        accu            = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->b0, p_mem_biquad[0]), 1));
        accu            = wX_add(accu,        wX_fmul(p_biquad_cell->b0, p_mem_biquad[1]));
        accu            = wX_msl(accu,        p_biquad_cell->b_exp);
        p_mem_biquad[1] = p_mem_biquad[0];
        p_mem_biquad[0] = in;
        p_mem_biquad   += 2;
        accu            = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->a1, p_mem_biquad[0]), 1));
        in              = wsubr (accu,        wX_fmul(p_biquad_cell->a2, p_mem_biquad[1]));
        p_biquad_cell++;
    }
    p_mem_biquad[1] = p_mem_biquad[0];
    p_mem_biquad[0] = in;

    return in;

#endif // __arm__
}


static void inline stereo_biquad_filtering_ButterHP(MMshort *p_in_left, MMshort *p_in_right, t_biquad_processing_ctx *p_biquad_processing_ctx)
{
    int                       nb_biquad_cells     = p_biquad_processing_ctx->biquad_order;
    MMshort                   *p_mem_biquad_left  = p_biquad_processing_ctx->p_biquad_mem[0];
    MMshort                   *p_mem_biquad_right = p_biquad_processing_ctx->p_biquad_mem[1];
    t_biquad_cell_Butter YMEM *p_biquad_cell      = (t_biquad_cell_Butter YMEM *) p_biquad_processing_ctx->p_biquad_cell_void;
    MMshort                   in_left             = *p_in_left;
    MMshort                   in_right            = *p_in_right;


#ifdef __arm__

    *p_in_left  = mono_biquad_filtering_ButterHP_arm(in_left,  (unsigned int) nb_biquad_cells, p_mem_biquad_left,  p_biquad_cell);
    *p_in_right = mono_biquad_filtering_ButterHP_arm(in_right, (unsigned int) nb_biquad_cells, p_mem_biquad_right, p_biquad_cell);

#else // __arm__

    MMlonglong accu;
    int        i;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_BIQUAD_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < nb_biquad_cells; i++)
    {
        //         (b0 + 2.b0.z^-1 + b0.z^-2).2^gain_exp
        // H(z) = ---------------------------------------
        //           1 + 2.a1.z^-1 + a2.z^-2
        accu                  =                     wX_fmul(p_biquad_cell->b0, in_left);
        accu                  = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->b0, p_mem_biquad_left[0]), 1));
        accu                  = wX_add(accu,        wX_fmul(p_biquad_cell->b0, p_mem_biquad_left[1]));
        accu                  = wX_msl(accu,        p_biquad_cell->b_exp);
        p_mem_biquad_left[1]  = p_mem_biquad_left[0];
        p_mem_biquad_left[0]  = in_left;
        p_mem_biquad_left    += 2;
        accu                  = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->a1, p_mem_biquad_left[0]), 1));
        in_left               = wsubr (accu,        wX_fmul(p_biquad_cell->a2, p_mem_biquad_left[1]));

        accu                  =                     wX_fmul(p_biquad_cell->b0, in_right);
        accu                  = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->b0, p_mem_biquad_right[0]), 1));
        accu                  = wX_add(accu,        wX_fmul(p_biquad_cell->b0, p_mem_biquad_right[1]));
        accu                  = wX_msl(accu,        p_biquad_cell->b_exp);
        p_mem_biquad_right[1] = p_mem_biquad_right[0];
        p_mem_biquad_right[0] = in_right;
        p_mem_biquad_right   += 2;
        accu                  = wX_sub(accu, wX_msl(wX_fmul(p_biquad_cell->a1, p_mem_biquad_right[0]), 1));
        in_right              = wsubr (accu,        wX_fmul(p_biquad_cell->a2, p_mem_biquad_right[1]));

        p_biquad_cell++;
    }
    p_mem_biquad_left [1] = p_mem_biquad_left [0];
    p_mem_biquad_left [0] = in_left;
    p_mem_biquad_right[1] = p_mem_biquad_right[0];
    p_mem_biquad_right[0] = in_right;

    *p_in_left  = in_left;
    *p_in_right = in_right;

#endif // __arm__
}


#ifdef __arm__

static MMshort inline mono_FIR_filtering_arm(unsigned int nb_FIR_coefs, MMshort YMEM __circ *p_FIR_mem, MMshort *p_FIR_coef, int FIR_gain_exp)
{
    int32x4_t    mem_32x4, coef_32x4;
    int32x2_t    mem_32x2, coef_32x2;
    int64x2_t    accu_64x2;
    int64x1_t    accu_64x1;
    int32_t      *p_mem_int32, *p_coef_int32;
    unsigned int i;

    // H(z) = a(0).z^0 + a(1).z^-1 + ... + a(p-1).z^-(p-1)
    p_coef_int32 = (int32_t *) p_FIR_coef;
    p_mem_int32  = (int32_t *) p_FIR_mem;
    accu_64x2    = vdupq_n_s64(0);
    for(i = nb_FIR_coefs / 4; i > 0; i--)
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
    accu_64x1 = vqrshl_s64(accu_64x1, vdup_n_s64(FIR_gain_exp));
    mem_32x2  = vqrshrn_n_s64(vcombine_s64(vdup_n_s64(0), accu_64x1), 31);

    return vget_lane_s32(mem_32x2, 1);
}

#endif // __arm__


static MMshort inline mono_FIR_filtering(t_FIR               *p_FIR,
                                         MMshort YMEM __circ *p_mem_FIR)
{
#ifdef __arm__

    return mono_FIR_filtering_arm((unsigned int) p_FIR->order, p_mem_FIR, p_FIR->coef, p_FIR->gain_exp);

#else // __arm__

    MMshort    *p_FIR_coef;
    MMlonglong accu;
    int        i;

    // H(z) = a(0).z^0 + a(1).z^-1 + ... + a(p-1).z^-(p-1)
    p_FIR_coef = p_FIR->coef;
    accu       = 0;
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_FIR_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < p_FIR->order; i++)
    {
        accu = wX_add(accu, wX_fmul(*p_FIR_coef++, *p_mem_FIR++));
    }

    return wround_X(wX_msl(accu, p_FIR->gain_exp));

#endif // __arm__
}


static void inline stereo_FIR_filtering(t_FIR               *p_FIR,
                                        MMshort YMEM __circ *p_mem_FIR_left,
                                        MMshort YMEM __circ *p_mem_FIR_right,
                                        MMshort             *p_output_left,
                                        MMshort             *p_output_right)
{
#ifdef __arm__

    *p_output_left  = mono_FIR_filtering_arm((unsigned int) p_FIR->order, p_mem_FIR_left,  p_FIR->coef, p_FIR->gain_exp);
    *p_output_right = mono_FIR_filtering_arm((unsigned int) p_FIR->order, p_mem_FIR_right, p_FIR->coef, p_FIR->gain_exp);

#else // __arm__

    MMshort    *p_FIR_coef;
    MMlonglong accu;
    int        i;

    // H(z) = a(0).z^0 + a(1).z^-1 + ... + a(p-1).z^-(p-1)
    p_FIR_coef = p_FIR->coef;
    accu       = 0;
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_FIR_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < p_FIR->order; i++)
    {
        accu = wX_add(accu, wX_fmul(*p_FIR_coef++, *p_mem_FIR_left++));
    }
    *p_output_left = wround_X(wX_msl(accu, p_FIR->gain_exp));

    p_FIR_coef = p_FIR->coef;
    accu       = 0;
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_FIR_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 0; i < p_FIR->order; i++)
    {
        accu = wX_add(accu, wX_fmul(*p_FIR_coef++, *p_mem_FIR_right++));
    }
    *p_output_right = wround_X(wX_msl(accu, p_FIR->gain_exp));

#endif // __arm__
}


#ifdef __arm__

static MMshort inline mono_symmetric_FIR_filtering_arm(unsigned int nb_FIR_coefs, MMshort YMEM __circ *p_FIR_mem, MMshort *p_FIR_coef, int FIR_gain_exp)
{
    int32x4_t    mem_32x4, coef_32x4;
    int32x2_t    mem_32x2, coef_32x2;
    int64x2_t    accu_64x2;
    int64x1_t    accu_64x1;
    int32_t      *p_mem1_int32, *p_mem2_int32, *p_coef_int32;
    unsigned int i;

    // H(z) = a(0).z^-(p-1) + a(1).[z^-(p-1-1) + z^-(p-1+1)] + a(2).[z^-(p-1-2) + z^-(p-1+2)] + ... + a(p-1).[z^0 + z^-(2.p-2)]
    nb_FIR_coefs--;
    p_FIR_mem   += nb_FIR_coefs;
    p_coef_int32 = (int32_t *) p_FIR_coef;
    p_mem1_int32 = (int32_t *) p_FIR_mem;
    p_mem2_int32 = p_mem1_int32 - 4;
    coef_32x2    = vld1_s32(p_coef_int32++);
    mem_32x2     = vld1_s32(p_mem1_int32++);
    coef_32x2    = vset_lane_s32(0, coef_32x2, 1);
    accu_64x2    = vmull_s32(mem_32x2, coef_32x2);
    for(i = nb_FIR_coefs / 4; i > 0; i--)
    {
        coef_32x4     = vld1q_s32(p_coef_int32);
        p_coef_int32 += 4;
        mem_32x4      = vld1q_s32(p_mem1_int32);
        p_mem1_int32 += 4;
        accu_64x2     = vmlal_s32(accu_64x2, vget_low_s32 (mem_32x4), vget_low_s32 (coef_32x4));
        accu_64x2     = vmlal_s32(accu_64x2, vget_high_s32(mem_32x4), vget_high_s32(coef_32x4));
        mem_32x4      = vld1q_s32(p_mem2_int32);
        p_mem2_int32 -= 4;
        mem_32x4      = vrev64q_s32(mem_32x4);
        accu_64x2     = vmlal_s32(accu_64x2, vget_low_s32 (mem_32x4), vget_high_s32(coef_32x4));
        accu_64x2     = vmlal_s32(accu_64x2, vget_high_s32(mem_32x4), vget_low_s32 (coef_32x4));
    }
    switch(nb_FIR_coefs & 3)
    {
        default:
            break;

        case 1:
            coef_32x2 = vld1_s32      (p_coef_int32);
            mem_32x2  = vext_s32      (vld1_s32(p_mem1_int32), vld1_s32(p_mem2_int32 + 2), 1);
            accu_64x2 = vmlal_lane_s32(accu_64x2, mem_32x2, coef_32x2, 0);
            break;

        case 2:
            coef_32x2 = vld1_s32      (p_coef_int32);
            mem_32x2  = vld1_s32      (p_mem1_int32);
            accu_64x2 = vmlal_s32     (accu_64x2, mem_32x2, coef_32x2);
            mem_32x2  = vld1_s32      (p_mem2_int32 + 2);
            mem_32x2  = vrev64_s32    (mem_32x2);
            accu_64x2 = vmlal_s32     (accu_64x2, mem_32x2, coef_32x2);
            break;

        case 3:
            coef_32x4 = vld1q_s32     (p_coef_int32);
            mem_32x4  = vld1q_s32     (p_mem1_int32);
            coef_32x4 = vsetq_lane_s32(0, coef_32x4, 3);
            accu_64x2 = vmlal_s32     (accu_64x2, vget_low_s32 (mem_32x4), vget_low_s32 (coef_32x4));
            accu_64x2 = vmlal_s32     (accu_64x2, vget_high_s32(mem_32x4), vget_high_s32(coef_32x4));
            mem_32x4  = vld1q_s32     (p_mem2_int32);
            mem_32x4  = vrev64q_s32   (mem_32x4);
            accu_64x2 = vmlal_s32     (accu_64x2, vget_low_s32 (mem_32x4), vget_high_s32(coef_32x4));
            accu_64x2 = vmlal_s32     (accu_64x2, vget_high_s32(mem_32x4), vget_low_s32 (coef_32x4));
            break;
    }
    accu_64x1 = vadd_s64(vget_low_s64(accu_64x2), vget_high_s64(accu_64x2));
    accu_64x1 = vqrshl_s64(accu_64x1, vdup_n_s64(FIR_gain_exp));
    mem_32x2  = vqrshrn_n_s64(vcombine_s64(vdup_n_s64(0), accu_64x1), 31);

    return vget_lane_s32(mem_32x2, 1);
}

#endif // __arm__


static MMshort inline mono_symmetric_FIR_filtering(t_FIR               *p_FIR,
                                                   MMshort YMEM __circ *p_mem_FIR)
{
#ifdef __arm__

    return mono_symmetric_FIR_filtering_arm((unsigned int) p_FIR->order, p_mem_FIR, p_FIR->coef, p_FIR->gain_exp);

#else // __arm__

    MMshort YMEM __circ *p_mem_FIR1;
    MMshort YMEM __circ *p_mem_FIR2;
    MMshort             *p_FIR_coef;
    MMshort             coef;
    MMlonglong          accu;
    int                 i;

    // H(z) = a(0).z^-(p-1) + a(1).[z^-(p-1-1) + z^-(p-1+1)] + a(2).[z^-(p-1-2) + z^-(p-1+2)] + ... + a(p-1).[z^0 + z^-(2.p-2)]
    p_mem_FIR1 = p_mem_FIR + p_FIR->order - 1;
    p_mem_FIR2 = p_mem_FIR1 - 1;
    p_FIR_coef = p_FIR->coef;
    accu       = wX_fmul(*p_FIR_coef++, *p_mem_FIR1++);
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_FIR_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 1; i < p_FIR->order; i++)
    {
        coef = *p_FIR_coef++;
        accu = wX_add(accu, wX_fmul(coef, *p_mem_FIR1++));
        accu = wX_add(accu, wX_fmul(coef, *p_mem_FIR2--));
    }

    return wround_X(wX_msl(accu, p_FIR->gain_exp));

#endif // __arm__
}



static void inline stereo_symmetric_FIR_filtering(t_FIR               *p_FIR,
                                                  MMshort YMEM __circ *p_mem_FIR_left,
                                                  MMshort YMEM __circ *p_mem_FIR_right,
                                                  MMshort             *p_output_left,
                                                  MMshort             *p_output_right)
{
#ifdef __arm__

    *p_output_left  = mono_symmetric_FIR_filtering_arm((unsigned int) p_FIR->order, p_mem_FIR_left,  p_FIR->coef, p_FIR->gain_exp);
    *p_output_right = mono_symmetric_FIR_filtering_arm((unsigned int) p_FIR->order, p_mem_FIR_right, p_FIR->coef, p_FIR->gain_exp);

#else // __arm__

    MMshort YMEM __circ *p_mem_FIR1_left;
    MMshort YMEM __circ *p_mem_FIR2_left;
    MMshort YMEM __circ *p_mem_FIR1_right;
    MMshort YMEM __circ *p_mem_FIR2_right;
    MMshort             *p_FIR_coef;
    MMshort             coef;
    MMlonglong          accu;
    int                 i;

    // H(z) = a(0).z^-(p-1) + a(1).[z^-(p-1-1) + z^-(p-1+1)] + a(2).[z^-(p-1-2) + z^-(p-1+2)] + ... + a(p-1).[z^0 + z^-(2.p-2)]
    p_mem_FIR1_left = p_mem_FIR_left + p_FIR->order - 1;
    p_mem_FIR2_left = p_mem_FIR1_left - 1;
    p_FIR_coef      = p_FIR->coef;
    accu            = wX_fmul(*p_FIR_coef++, *p_mem_FIR1_left++);
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_FIR_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 1; i < p_FIR->order; i++)
    {
        coef = *p_FIR_coef++;
        accu = wX_add(accu, wX_fmul(coef, *p_mem_FIR1_left++));
        accu = wX_add(accu, wX_fmul(coef, *p_mem_FIR2_left--));
    }
    *p_output_left = wround_X(wX_msl(accu, p_FIR->gain_exp));

    p_mem_FIR1_right = p_mem_FIR_right + p_FIR->order - 1;
    p_mem_FIR2_right = p_mem_FIR1_right - 1;
    p_FIR_coef       = p_FIR->coef;
    accu             = wX_fmul(*p_FIR_coef++, *p_mem_FIR1_right++);
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_FIR_ORDER_MAX)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
    for(i = 1; i < p_FIR->order; i++)
    {
        coef = *p_FIR_coef++;
        accu = wX_add(accu, wX_fmul(coef, *p_mem_FIR1_right++));
        accu = wX_add(accu, wX_fmul(coef, *p_mem_FIR2_right--));
    }
    *p_output_right = wround_X(wX_msl(accu, p_FIR->gain_exp));

#endif // __arm__
}



static void mdrc_sub_bands_mono_biquad_filtering(t_mdrc_filters *p_mdrc_filters, MMshort *p_input[MDRC_CHANNELS_MAX], MMshort *p_output[MDRC_CHANNELS_MAX][MDRC_BANDS_MAX], int nb_samples)
{
    t_mdrc_biquads          *p_mdrc_biquads = p_mdrc_filters->mdrc_filters.mdrc_biquads;
    int                     nb_bands        = p_mdrc_filters->nb_bands;
    t_biquad_processing_ctx *p_biquadLP_processing_ctx;
    t_biquad_processing_ctx *p_biquadHP_processing_ctx;
    MMshort                 *p_band_input;
    MMshort                 *p_band_output;
    int                     i, n;
    MMshort                 sample;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_BANDS_MAX)
#pragma loop force_hwloop
#endif
    for(i = 0; i < nb_bands; i++)
    {
        p_biquadLP_processing_ctx = &(p_mdrc_biquads->biquadLP_processing_ctx);
        p_biquadHP_processing_ctx = &(p_mdrc_biquads->biquadHP_processing_ctx);
        p_band_input              = p_input [0];
        p_band_output             = p_output[0][i];

        switch(p_mdrc_biquads->biquad_LP_HP_combination)
        {
            case DUMMY_BIQUAD_LP_DUMMY_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample           = *p_band_input;
                    p_band_input    += 2;   // because input buffer is MMlong
                    *p_band_output++ = sample;
                }
                break;

            case DUMMY_BIQUAD_LP_BUTTERWORTH_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample           = *p_band_input;
                    p_band_input    += 2;   // because input buffer is MMlong
                    sample           = mono_biquad_filtering_ButterHP(sample, p_biquadHP_processing_ctx);
                    *p_band_output++ = sample;
                }
                break;

            case DUMMY_BIQUAD_LP_OTHER_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample           = *p_band_input;
                    p_band_input    += 2;   // because input buffer is MMlong
                    sample           = mono_biquad_filtering(sample, p_biquadHP_processing_ctx);
                    *p_band_output++ = sample;
                }
                break;

            case BUTTERWORTH_BIQUAD_LP_DUMMY_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample           = *p_band_input;
                    p_band_input    += 2;   // because input buffer is MMlong
                    sample           = mono_biquad_filtering_ButterLP(sample, p_biquadLP_processing_ctx);
                    *p_band_output++ = sample;
                }
                break;

            case BUTTERWORTH_BIQUAD_LP_BUTTERWORTH_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample           = *p_band_input;
                    p_band_input    += 2;   // because input buffer is MMlong
                    sample           = mono_biquad_filtering_ButterLP(sample, p_biquadLP_processing_ctx);
                    sample           = mono_biquad_filtering_ButterHP(sample, p_biquadHP_processing_ctx);
                    *p_band_output++ = sample;
                }
                break;

            case BUTTERWORTH_BIQUAD_LP_OTHER_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample           = *p_band_input;
                    p_band_input    += 2;   // because input buffer is MMlong
                    sample           = mono_biquad_filtering_ButterLP(sample, p_biquadLP_processing_ctx);
                    sample           = mono_biquad_filtering         (sample, p_biquadHP_processing_ctx);
                    *p_band_output++ = sample;
                }
                break;

            case OTHER_BIQUAD_LP_DUMMY_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample           = *p_band_input;
                    p_band_input    += 2;   // because input buffer is MMlong
                    sample           = mono_biquad_filtering(sample, p_biquadLP_processing_ctx);
                    *p_band_output++ = sample;
                }
                break;

            case OTHER_BIQUAD_LP_BUTTERWORTH_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample           = *p_band_input;
                    p_band_input    += 2;   // because input buffer is MMlong
                    sample           = mono_biquad_filtering         (sample, p_biquadLP_processing_ctx);
                    sample           = mono_biquad_filtering_ButterHP(sample, p_biquadHP_processing_ctx);
                    *p_band_output++ = sample;
                }
                break;

            case OTHER_BIQUAD_LP_OTHER_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample           = *p_band_input;
                    p_band_input    += 2;   // because input buffer is MMlong
                    sample           = mono_biquad_filtering(sample, p_biquadLP_processing_ctx);
                    sample           = mono_biquad_filtering(sample, p_biquadHP_processing_ctx);
                    *p_band_output++ = sample;
                }
                break;

            default:
                break;
        }

        p_mdrc_biquads++;
    }
}


static void mdrc_sub_bands_stereo_biquad_filtering(t_mdrc_filters *p_mdrc_filters, MMshort *p_input[MDRC_CHANNELS_MAX], MMshort *p_output[MDRC_CHANNELS_MAX][MDRC_BANDS_MAX], int nb_samples)
{
    t_mdrc_biquads          *p_mdrc_biquads = p_mdrc_filters->mdrc_filters.mdrc_biquads;
    int                     nb_bands        = p_mdrc_filters->nb_bands;
    t_biquad_processing_ctx *p_biquadLP_processing_ctx;
    t_biquad_processing_ctx *p_biquadHP_processing_ctx;
    MMshort                 *p_band_input_left;
    MMshort                 *p_band_input_right;
    MMshort                 *p_band_output_left;
    MMshort                 *p_band_output_right;
    int                     i, n;
    MMshort                 sample_left, sample_right;

#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_BANDS_MAX)
#pragma loop force_hwloop
#endif
    for(i = 0; i < nb_bands; i++)
    {
        p_biquadLP_processing_ctx = &(p_mdrc_biquads->biquadLP_processing_ctx);
        p_biquadHP_processing_ctx = &(p_mdrc_biquads->biquadHP_processing_ctx);
        p_band_input_left         = p_input [0];
        p_band_input_right        = p_input [1];
        p_band_output_left        = p_output[0][i];
        p_band_output_right       = p_output[1][i];

        switch(p_mdrc_biquads->biquad_LP_HP_combination)
        {
            case DUMMY_BIQUAD_LP_DUMMY_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample_left            = *p_band_input_left;
                    sample_right           = *p_band_input_right;
                    p_band_input_left     += 2;   // because input buffer is MMlong
                    p_band_input_right    += 2;   // because input buffer is MMlong
                    *p_band_output_left++  = sample_left;
                    *p_band_output_right++ = sample_right;
                }
                break;

            case DUMMY_BIQUAD_LP_BUTTERWORTH_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample_left            = *p_band_input_left;
                    sample_right           = *p_band_input_right;
                    p_band_input_left     += 2;   // because input buffer is MMlong
                    p_band_input_right    += 2;   // because input buffer is MMlong
                    stereo_biquad_filtering_ButterHP(&sample_left, &sample_right, p_biquadHP_processing_ctx);
                    *p_band_output_left++  = sample_left;
                    *p_band_output_right++ = sample_right;
                }
                break;

            case DUMMY_BIQUAD_LP_OTHER_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample_left            = *p_band_input_left;
                    sample_right           = *p_band_input_right;
                    p_band_input_left     += 2;   // because input buffer is MMlong
                    p_band_input_right    += 2;   // because input buffer is MMlong
                    stereo_biquad_filtering(&sample_left, &sample_right, p_biquadHP_processing_ctx);
                    *p_band_output_left++  = sample_left;
                    *p_band_output_right++ = sample_right;
                }
                break;

            case BUTTERWORTH_BIQUAD_LP_DUMMY_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample_left            = *p_band_input_left;
                    sample_right           = *p_band_input_right;
                    p_band_input_left     += 2;   // because input buffer is MMlong
                    p_band_input_right    += 2;   // because input buffer is MMlong
                    stereo_biquad_filtering_ButterLP(&sample_left, &sample_right, p_biquadLP_processing_ctx);
                    *p_band_output_left++  = sample_left;
                    *p_band_output_right++ = sample_right;
                }
                break;

            case BUTTERWORTH_BIQUAD_LP_BUTTERWORTH_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample_left            = *p_band_input_left;
                    sample_right           = *p_band_input_right;
                    p_band_input_left     += 2;   // because input buffer is MMlong
                    p_band_input_right    += 2;   // because input buffer is MMlong
                    stereo_biquad_filtering_ButterLP(&sample_left, &sample_right, p_biquadLP_processing_ctx);
                    stereo_biquad_filtering_ButterHP(&sample_left, &sample_right, p_biquadHP_processing_ctx);
                    *p_band_output_left++  = sample_left;
                    *p_band_output_right++ = sample_right;
                }
                break;

            case BUTTERWORTH_BIQUAD_LP_OTHER_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample_left            = *p_band_input_left;
                    sample_right           = *p_band_input_right;
                    p_band_input_left     += 2;   // because input buffer is MMlong
                    p_band_input_right    += 2;   // because input buffer is MMlong
                    stereo_biquad_filtering_ButterLP(&sample_left, &sample_right, p_biquadLP_processing_ctx);
                    stereo_biquad_filtering         (&sample_left, &sample_right, p_biquadHP_processing_ctx);
                    *p_band_output_left++  = sample_left;
                    *p_band_output_right++ = sample_right;
                }
                break;

            case OTHER_BIQUAD_LP_DUMMY_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample_left            = *p_band_input_left;
                    sample_right           = *p_band_input_right;
                    p_band_input_left     += 2;   // because input buffer is MMlong
                    p_band_input_right    += 2;   // because input buffer is MMlong
                    stereo_biquad_filtering(&sample_left, &sample_right, p_biquadLP_processing_ctx);
                    *p_band_output_left++  = sample_left;
                    *p_band_output_right++ = sample_right;
                }
                break;

            case OTHER_BIQUAD_LP_BUTTERWORTH_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample_left            = *p_band_input_left;
                    sample_right           = *p_band_input_right;
                    p_band_input_left     += 2;   // because input buffer is MMlong
                    p_band_input_right    += 2;   // because input buffer is MMlong
                    stereo_biquad_filtering         (&sample_left, &sample_right, p_biquadLP_processing_ctx);
                    stereo_biquad_filtering_ButterHP(&sample_left, &sample_right, p_biquadHP_processing_ctx);
                    *p_band_output_left++  = sample_left;
                    *p_band_output_right++ = sample_right;
                }
                break;

            case OTHER_BIQUAD_LP_OTHER_BIQUAD_HP:
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                for(n = nb_samples; n > 0; n--)
                {
                    sample_left            = *p_band_input_left;
                    sample_right           = *p_band_input_right;
                    p_band_input_left     += 2;   // because input buffer is MMlong
                    p_band_input_right    += 2;   // because input buffer is MMlong
                    stereo_biquad_filtering(&sample_left, &sample_right, p_biquadLP_processing_ctx);
                    stereo_biquad_filtering(&sample_left, &sample_right, p_biquadHP_processing_ctx);
                    *p_band_output_left++  = sample_left;
                    *p_band_output_right++ = sample_right;
                }
                break;

            default:
                break;
        }

        p_mdrc_biquads++;
    }
}


static void mdrc_sub_bands_mono_FIR_filtering(t_mdrc_filters *p_mdrc_filters, MMshort *p_input[MDRC_CHANNELS_MAX], MMshort *p_output[MDRC_CHANNELS_MAX][MDRC_BANDS_MAX], int nb_samples)
{
    t_mdrc_FIR          *p_mdrc_FIR = p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR;
    MMshort YMEM __circ *p_FIR_mem;
    MMshort YMEM __circ *save_p_FIR_mem;
    t_FIR               *p_FIR;
    MMshort             *p_band_input;
    MMshort             *p_band_output;
    int                 i, n;


    // use same FIR filter memory for each band
    p_FIR_mem    = p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[0];
    p_band_input = p_input[0];
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
    for(n = nb_samples; n > 0; n--)
    {
        *p_FIR_mem--  = *p_band_input;
        p_band_input += 2;   // because input buffer is MMlong
    }
    p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[0] = p_FIR_mem;
#else /* __flexcc2__ */
    p_FIR_mem += MDRC5B_BLOCK_SIZE + MDRC_FIR_ORDER_MAX - 1;
    for(n = MDRC5B_BLOCK_SIZE + MDRC_FIR_ORDER_MAX - nb_samples; n > 0; n--)
    {
        *p_FIR_mem = *(p_FIR_mem - nb_samples);
        p_FIR_mem--;
    }
    for(n = nb_samples; n > 0; n--)
    {
        *p_FIR_mem--  = *p_band_input;
        p_band_input += 2;   // because input buffer is MMlong
    }
#endif /* __flexcc2__ */

    save_p_FIR_mem = p_FIR_mem + nb_samples;
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_BANDS_MAX)
#pragma loop force_hwloop
#endif
    for(i = 0; i < p_mdrc_filters->nb_bands; i++)
    {
        p_FIR         = &(p_mdrc_FIR->FIR);
        p_FIR_mem     =   save_p_FIR_mem;
        p_band_output =   p_output[0][i];

        if(p_mdrc_FIR->FIR_kind == SYMMETRIC_FIR)
        {
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
            for(n = nb_samples; n > 0; n--)
            {
                *p_band_output++ = mono_symmetric_FIR_filtering(p_FIR, p_FIR_mem--);
            }
        }
        else
        {
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
            for(n = nb_samples; n > 0; n--)
            {
                *p_band_output++ = mono_FIR_filtering(p_FIR, p_FIR_mem--);
            }
        }

        p_mdrc_FIR++;
    }
}


static void mdrc_sub_bands_stereo_FIR_filtering(t_mdrc_filters *p_mdrc_filters, MMshort *p_input[MDRC_CHANNELS_MAX], MMshort *p_output[MDRC_CHANNELS_MAX][MDRC_BANDS_MAX], int nb_samples)
{
    t_mdrc_FIR          *p_mdrc_FIR = p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR;
    MMshort YMEM __circ *p_FIR_mem_left;
    MMshort YMEM __circ *p_FIR_mem_right;
    MMshort YMEM __circ *save_p_FIR_mem_left;
    MMshort YMEM __circ *save_p_FIR_mem_right;
    t_FIR               *p_FIR;
    MMshort             *p_band_input;
    MMshort             *p_band_output_left;
    MMshort             *p_band_output_right;
    int                 i, n;


    // use same FIR filter memory for each band
    p_FIR_mem_left = p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[0];
    p_band_input   = p_input[0];
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
    for(n = nb_samples; n > 0; n--)
    {
        *p_FIR_mem_left-- = *p_band_input;
        p_band_input     += 2;   // because input buffer is MMlong
    }
    p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[0] = p_FIR_mem_left;
#else /* __flexcc2__ */
    p_FIR_mem_left += MDRC5B_BLOCK_SIZE + MDRC_FIR_ORDER_MAX - 1;
    for(n = MDRC5B_BLOCK_SIZE + MDRC_FIR_ORDER_MAX - nb_samples; n > 0; n--)
    {
        *p_FIR_mem_left = *(p_FIR_mem_left - nb_samples);
        p_FIR_mem_left--;
    }
    for(n = nb_samples; n > 0; n--)
    {
        *p_FIR_mem_left-- = *p_band_input;
        p_band_input     += 2;   // because input buffer is MMlong
    }
#endif /* __flexcc2__ */
    p_FIR_mem_right = p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[1];
    p_band_input    = p_input[1];
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
    for(n = nb_samples; n > 0; n--)
    {
        *p_FIR_mem_right-- = *p_band_input;
        p_band_input      += 2;   // because input buffer is MMlong
    }
    p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[1] = p_FIR_mem_right;
#else /* __flexcc2__ */
    p_FIR_mem_right += MDRC5B_BLOCK_SIZE + MDRC_FIR_ORDER_MAX - 1;
    for(n = MDRC5B_BLOCK_SIZE + MDRC_FIR_ORDER_MAX - nb_samples; n > 0; n--)
    {
        *p_FIR_mem_right = *(p_FIR_mem_right - nb_samples);
        p_FIR_mem_right--;
    }
    for(n = nb_samples; n > 0; n--)
    {
        *p_FIR_mem_right-- = *p_band_input;
        p_band_input      += 2;   // because input buffer is MMlong
    }
#endif /* __flexcc2__ */

    save_p_FIR_mem_left  = p_FIR_mem_left  + nb_samples;
    save_p_FIR_mem_right = p_FIR_mem_right + nb_samples;
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(MDRC_BANDS_MAX)
#pragma loop force_hwloop
#endif
    for(i = 0; i < p_mdrc_filters->nb_bands; i++)
    {
        p_FIR               = &(p_mdrc_FIR->FIR);
        p_FIR_mem_left      =   save_p_FIR_mem_left;
        p_FIR_mem_right     =   save_p_FIR_mem_right;
        p_band_output_left  =   p_output[0][i];
        p_band_output_right =   p_output[1][i];

        if(p_mdrc_FIR->FIR_kind == SYMMETRIC_FIR)
        {
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
            for(n = nb_samples; n > 0; n--)
            {
                stereo_symmetric_FIR_filtering(p_FIR, p_FIR_mem_left--, p_FIR_mem_right--, p_band_output_left++, p_band_output_right++);
            }
        }
        else
        {
#ifdef __flexcc2__
#pragma loop minitercount(1)
#pragma loop maxitercount(1023)
#pragma loop force_hwloop
#endif /* __flexcc2__ */
            for(n = nb_samples; n > 0; n--)
            {
                stereo_FIR_filtering(p_FIR, p_FIR_mem_left--, p_FIR_mem_right--, p_band_output_left++, p_band_output_right++);
            }
        }

        p_mdrc_FIR++;
    }
}




void mdrc_filter_default_config(t_mdrc_filters *p_mdrc_filters)
{
    p_mdrc_filters->mdrc_filter_kind                                                        = MDRC_BIQUAD_FILTER;
    p_mdrc_filters->nb_channels                                                             = 1;
    p_mdrc_filters->nb_bands                                                                = 1;
    p_mdrc_filters->p_sub_bands_filtering                                                   = mdrc_sub_bands_mono_biquad_filtering;
    p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquad_LP_HP_combination                   = DUMMY_BIQUAD_LP_DUMMY_BIQUAD_HP;
    p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.biquad_order       = 0;
    p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.biquad_order       = 0;
    p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.p_biquad_cell_void = NULL;
    p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.p_biquad_cell_void = NULL;
    p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.p_biquad_mem[0]    = NULL;
    p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.p_biquad_mem[1]    = NULL;
    p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.p_biquad_mem[0]    = NULL;
    p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.p_biquad_mem[1]    = NULL;
}


int get_mdrc_filter_config(t_mdrc_filters      *p_mdrc_filters,
                           MdrcFiltersConfig_t *p_mdrc_filters_config,
                           t_mdrc_filter_kind  param_mdrc_filter_kind,
                           t_biquad_kind       param_biquad_kind,
                           t_FIR_kind          param_FIR_kind)
{
    int i, j;


    p_mdrc_filters->mdrc_filter_kind = param_mdrc_filter_kind;
    p_mdrc_filters->nb_bands         = (int) p_mdrc_filters_config->nb_bands;
    switch(p_mdrc_filters->mdrc_filter_kind)
    {
        case MDRC_BIQUAD_FILTER:
            if(p_mdrc_filters->nb_channels == 1)
            {
                p_mdrc_filters->p_sub_bands_filtering = mdrc_sub_bands_mono_biquad_filtering;
            }
            else if(p_mdrc_filters->nb_channels == 2)
            {
                p_mdrc_filters->p_sub_bands_filtering = mdrc_sub_bands_stereo_biquad_filtering;
            }
            else
            {
                return -1;
            }
            for(i = 0; i < p_mdrc_filters->nb_bands; i++)
            {
                p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquad_LP_HP_combination = (t_biquad_LP_HP_combination) 0;
            }
            p_mdrc_filters->mdrc_filters.mdrc_biquads[                           0].biquad_LP_HP_combination            += DUMMY_BIQUAD_HP;
            p_mdrc_filters->mdrc_filters.mdrc_biquads[p_mdrc_filters->nb_bands - 1].biquad_LP_HP_combination            += DUMMY_BIQUAD_LP;
            p_mdrc_filters->mdrc_filters.mdrc_biquads[                           0].biquadHP_processing_ctx.biquad_order = 0;
            p_mdrc_filters->mdrc_filters.mdrc_biquads[p_mdrc_filters->nb_bands - 1].biquadLP_processing_ctx.biquad_order = 0;

            for(i = 0; i < p_mdrc_filters->nb_bands - 1; i++)
            {
                t_biquad_cell_int16       *p_biquad_cell_int16;
                t_biquad_cell_Butter YMEM *p_biquad_cell_Butter;
                t_biquad_cell        YMEM *p_biquad_cell;
                int                       biquad_order;

                biquad_order = (int) p_mdrc_filters_config->mdrc_biquad[i].biquadLP.biquad_order;
                if(biquad_order > MDRC_BIQUAD_ORDER_MAX)
                {
                    return -1;
                }
                p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.biquad_order = biquad_order;
                switch((t_biquad_kind) p_mdrc_filters_config->mdrc_biquad[i].biquadLP.biquad_kind)
                {
                    case BUTTERWORTH_BIQUAD:
                        p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquad_LP_HP_combination += BUTTERWORTH_BIQUAD_LP;

                        p_biquad_cell_Butter = (t_biquad_cell_Butter YMEM *) p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void;
                        p_biquad_cell_int16  = p_mdrc_filters_config->mdrc_biquad[i].biquadLP.biquad_cell_int16;
                        for(j = biquad_order; j > 0; j--)
                        {
                            get_biquad_cell_Butter(p_biquad_cell_int16++, p_biquad_cell_Butter++);
                        }
                        break;

                    case OTHER_BIQUAD:
                        if(param_biquad_kind == BUTTERWORTH_BIQUAD)
                        {
                            // allocation has been made with BUTTERWORTH_BIQUAD => not enough memory
                            return -1;
                        }

                        p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquad_LP_HP_combination += OTHER_BIQUAD_LP;

                        p_biquad_cell       = (t_biquad_cell YMEM *) p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void;
                        p_biquad_cell_int16 = p_mdrc_filters_config->mdrc_biquad[i].biquadLP.biquad_cell_int16;
                        for(j = biquad_order; j > 0; j--)
                        {
                            get_biquad_cell(p_biquad_cell_int16++, p_biquad_cell++);
                        }
                        break;

                    default:
                        break;
                }

                biquad_order = (int) p_mdrc_filters_config->mdrc_biquad[i].biquadHP.biquad_order;
                p_mdrc_filters->mdrc_filters.mdrc_biquads[i + 1].biquadHP_processing_ctx.biquad_order = biquad_order;
                switch((t_biquad_kind) p_mdrc_filters_config->mdrc_biquad[i].biquadHP.biquad_kind)
                {
                    case BUTTERWORTH_BIQUAD:
                        p_mdrc_filters->mdrc_filters.mdrc_biquads[i + 1].biquad_LP_HP_combination += BUTTERWORTH_BIQUAD_HP;

                        p_biquad_cell_Butter = (t_biquad_cell_Butter YMEM *) p_mdrc_filters->mdrc_filters.mdrc_biquads[i + 1].biquadHP_processing_ctx.p_biquad_cell_void;
                        p_biquad_cell_int16  = p_mdrc_filters_config->mdrc_biquad[i].biquadHP.biquad_cell_int16;
                        for(j = biquad_order; j > 0; j--)
                        {
                            get_biquad_cell_Butter(p_biquad_cell_int16++, p_biquad_cell_Butter++);
                        }
                        break;

                    case OTHER_BIQUAD:
                        if(param_biquad_kind == BUTTERWORTH_BIQUAD)
                        {
                            // allocation has been made with BUTTERWORTH_BIQUAD => not enough memory
                            return -1;
                        }

                        p_mdrc_filters->mdrc_filters.mdrc_biquads[i + 1].biquad_LP_HP_combination += OTHER_BIQUAD_HP;

                        p_biquad_cell       = (t_biquad_cell YMEM *) p_mdrc_filters->mdrc_filters.mdrc_biquads[i + 1].biquadHP_processing_ctx.p_biquad_cell_void;
                        p_biquad_cell_int16 = p_mdrc_filters_config->mdrc_biquad[i].biquadHP.biquad_cell_int16;
                        for(j = biquad_order; j > 0; j--)
                        {
                            get_biquad_cell(p_biquad_cell_int16++, p_biquad_cell++);
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case MDRC_FIR_FILTER:
            if(p_mdrc_filters->nb_channels == 1)
            {
                p_mdrc_filters->p_sub_bands_filtering = mdrc_sub_bands_mono_FIR_filtering;
            }
            else if(p_mdrc_filters->nb_channels == 2)
            {
                p_mdrc_filters->p_sub_bands_filtering = mdrc_sub_bands_stereo_FIR_filtering;
            }
            else
            {
                return -1;
            }

            for(i = 0; i < p_mdrc_filters->nb_bands; i++)
            {
                int FIR_order;

                FIR_order                                                    = (int) p_mdrc_filters_config->mdrc_FIR[i].FIR.order;
                p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.order = FIR_order;
                p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR_kind  = (t_FIR_kind) p_mdrc_filters_config->mdrc_FIR[i].FIR_kind;
                switch(p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR_kind)
                {
                    case SYMMETRIC_FIR:
                        FIR_order = 2 * FIR_order - 1;
                        if(FIR_order > MDRC_FIR_ORDER_MAX)
                        {
                            return -1;
                        }

                        if(get_FIR(&p_mdrc_filters_config->mdrc_FIR[i].FIR, &p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR) != 0)
                        {
                            free_mdrc_filter(p_mdrc_filters);
                            return -1;
                        }
                        break;

                    case OTHER_FIR:
                        if(param_FIR_kind == SYMMETRIC_FIR)
                        {
                            // allocation has been made with SYMMETRIC_FIR => not enough memory
                            return -1;
                        }

                        if(FIR_order > MDRC_FIR_ORDER_MAX)
                        {
                            return -1;
                        }

                        if(get_FIR(&p_mdrc_filters_config->mdrc_FIR[i].FIR, &p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR) != 0)
                        {
                            free_mdrc_filter(p_mdrc_filters);
                            return -1;
                        }
                        break;

                    default:
                        break;
                }
            }

            break;

        default:
            break;
    }

    return 0;
}


void reset_mdrc_filter(t_mdrc_filters *p_mdrc_filters)
{
    int i, j, k;

    switch(p_mdrc_filters->mdrc_filter_kind)
    {
        case MDRC_BIQUAD_FILTER:
            for(i = 0; i < p_mdrc_filters->nb_bands; i++)
            {
                for(j = 0; j < p_mdrc_filters->nb_channels; j++)
                {
                    MMshort *p_biquad_mem;

                    p_biquad_mem = p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[j];
                    if(p_biquad_mem != NULL)
                    {
                        for(k = 2 * MDRC_BIQUAD_ORDER_MAX + 2; k > 0; k--)
                        {
                            *p_biquad_mem++ = 0;
                        }
                    }

                    p_biquad_mem = p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[j];
                    if(p_biquad_mem != NULL)
                    {
                        for(k = 2 * MDRC_BIQUAD_ORDER_MAX + 2; k > 0; k--)
                        {
                            *p_biquad_mem++ = 0;
                        }
                    }
                }
            }
            break;

        case MDRC_FIR_FILTER:
            for(j = 0; j < p_mdrc_filters->nb_channels; j++)
            {
#ifdef __flexcc2__
                MMshort YMEM *p_FIR_mem = wget_circ_ymem_min(p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[j]);
#else // __flexcc2__
                MMshort YMEM *p_FIR_mem = p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[j];
#endif // __flexcc2__

                if(p_FIR_mem != NULL)
                {
                    for(k = MDRC_FIR_ORDER_MAX + MDRC5B_BLOCK_SIZE; k > 0; k--)
                    {
                        *p_FIR_mem++ = 0;
                    }
                }
            }
            break;

        default:
            break;
    }
}


void free_mdrc_filter(t_mdrc_filters *p_mdrc_filters)
{
    int i, j;

    switch(p_mdrc_filters->mdrc_filter_kind)
    {
        case MDRC_BIQUAD_FILTER:
            for(i = 0; i < p_mdrc_filters->nb_bands; i++)
            {
                if(p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void != NULL)
                {
                    free(p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void);
                    p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void = NULL;
                }
                if(p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_cell_void != NULL)
                {
                    free(p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_cell_void);
                    p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_cell_void = NULL;
                }
                for(j = 0; j < p_mdrc_filters->nb_channels; j++)
                {
                    if(p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[j] != NULL)
                    {
                        free(p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[j]);
                        p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[j] = NULL;
                    }
                    if(p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[j] != NULL)
                    {
                        free(p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[j]);
                        p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[j] = NULL;
                    }
                }
            }
            break;

        case MDRC_FIR_FILTER:
            for(i = 0; i < p_mdrc_filters->nb_bands; i++)
            {
                if(p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef != NULL)
                {
                    free(p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef);
                    p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef = NULL;
                }
            }
            for(j = 0; j < p_mdrc_filters->nb_channels; j++)
            {
#ifdef __flexcc2__
                MMshort YMEM *p_FIR_mem = wget_circ_ymem_min(p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[j]);
#else // __flexcc2__
                MMshort YMEM *p_FIR_mem = p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[j];
#endif // __flexcc2__

                if(p_FIR_mem != NULL)
                {
                    free(p_FIR_mem);
#ifdef __flexcc2__
                    p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[j] = winit_circ_ymem_ptr(NULL, NULL, NULL);
#else // __flexcc2__
                    p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[j] = NULL;
#endif // __flexcc2__
                }
            }
            if(p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem_optim != NULL)
            {
                free(p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem_optim);
                p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem_optim = NULL;
            }
            break;

        default:
            break;
    }
}

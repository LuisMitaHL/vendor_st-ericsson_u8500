/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   resample_ML_filter_block_fixin_fixout_bt.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_
#include <libeffects/libresampling.nmf>
#endif
#include "resample_local.h"
#include "vector.h"


#ifdef MMDSP
#if !defined(ARM) && !defined(NEON)
/********** COMMON CODE FOR UP AND DOWN SAMPLING ****************/
void resample_ML_filter_block_fixin_fixout(int               *in,
                                           unsigned int      istride,
                                           unsigned int      nbIn,
                                           int               *out,
                                           unsigned int      ostride,
                                           unsigned int      nbOutRequested,
                                           unsigned int      *nbOutAvail,
                                           ResampleDelayLine *delayLine,
                                           ResampleFilter    *filterInfo,
                                           unsigned int      loop_siz,
                                           Float             *CoefRam,
										   int squelch)
{
    unsigned int     M                = filterInfo->M;
    const EXTERN int *incr_offset_tab = filterInfo->incr_offset_tab;
    int              shift            = filterInfo->shift;
    unsigned int     actualLen;
    int              currIndex        = delayLine->currIndex;
    int              f_index;
    unsigned int     offset           = delayLine->offset;
    int              val;
    int              decimno          = filterInfo->decimno;
    WORD56           Acc;
    const YMEM Float *filter          = filterInfo->filter;
    const YMEM Float *p_coef, *p_coef1;
    int              i_coef;
    int              incr;
    int              nbin_to_consume;
    /* mapped data address variables */
    int              *baseAddress = &delayLine->baseAddress[0];
#ifndef __flexcc2__
    int              *p_val;
    int              *p_val1;
#else
    int __circ       *p_val;
    int __circ       *p_val1;
#endif
    int              first_dly = delayLine->len - 1;
    int              *max_adr  = &baseAddress[delayLine->len];
    int              *min_adr  = baseAddress;
    int              nzcoefs   = filterInfo->nzcoefs;
    int              dfindx    = nzcoefs - delayLine->len * M * decimno + filterInfo->ncfs;
    int              flush_mask;

    if (nbIn == 0)
        flush_mask = 0;
    else
        flush_mask = -1;
    if ((filterInfo->coef != 0) && (filterInfo->coef < MAXVAL - 1) && (nbIn > 0))
    {
        /* this is a pure downsampling filter, compensate for normalization of
        the filter coefficients */
        vector_scale_in_place(in, filterInfo->coef, nbIn, istride);
    }
#ifndef __flexcc2__
#ifndef MMDSP
    set_modulo_range1((void *) min_adr, (void *) max_adr);
    set_addressing_mode(MOD_AX1);
#endif
    // make unix here
    p_val  = baseAddress;
    p_val += currIndex;
    p_val1 = p_val;
#else
    p_val  = winit_circ_ptr(&baseAddress[currIndex],        /* dummy value, this will be initialized later */
                            min_adr,                        /* minimum value           */
                            max_adr);                       /* maximum value +1        */
    p_val1 = winit_circ_ptr(&baseAddress[currIndex],        /* dummy value, this will be initialized later */
                            min_adr,                        /* minimum value           */
                            max_adr);                       /* maximum value +1        */
#endif /* __flexcc2__ */
    actualLen = delayLine->len / 2;
    i_coef    = decimno * M;
    assert(actualLen > 1);
    nbin_to_consume = loop_siz - nbOutRequested;
    if ((delayLine->len & 1) != 0)
    {
        if (offset >= M)
        {
            incr   = wextract(incr_offset_tab[offset], 0x0808);
            offset = wextract(incr_offset_tab[offset], 0x0800);
            HWLOOP (incr)
            {
                p_val  = mod_add(p_val, -1, max_adr, min_adr);
                val    = *in & flush_mask;
                in    += istride;
                *p_val = val;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        }
#pragma loop minitercount(4)
        HWLOOP (nbOutRequested)
        {
#ifdef __flexcc2__
            p_val1 = wset_circ_addr(p_val1, wget_circ_addr(p_val));
#else
            p_val1 = p_val;
#endif
            p_val1  = mod_add(p_val1, first_dly, max_adr, min_adr);
            f_index = offset * decimno;
            p_coef1 = &filter[i_coef - f_index + nzcoefs - 1];
            p_coef  = &filter[dfindx + f_index];
            Acc     = 0;
#pragma loop minitercount(4)
            HWLOOP (actualLen)
            {
                Acc      = wX_add(Acc, wX_fmul(*p_val,  *p_coef));
                Acc      = wX_add(Acc, wX_fmul(*p_val1, *p_coef1));
                p_val    = mod_add(p_val,   1/*i_val*/,   max_adr, min_adr);
                p_val1   = mod_add(p_val1, -1/*i_val_m*/, max_adr, min_adr);
                p_coef1 += i_coef;
                p_coef  += i_coef;
            } END_HWLOOP;
            incr   = wextract(incr_offset_tab[offset], 0x0808);
            incr   = wmin(incr, nbin_to_consume);
            offset = wextract(incr_offset_tab[offset], 0x0800);
            Acc    = wX_add(Acc,wX_fmul(*p_val,*p_coef));
            p_val  = mod_add(p_val, -actualLen, max_adr, min_adr);
            /* compensate for normalization */
            Acc    = wX_msr(Acc, shift);
            //val = waddr(Acc, Acc0);
            val    = wround_X(Acc);
            *out   = val;
            out   += ostride;
            HWLOOP(incr)
            {
                /* put in delay line */
                p_val  = mod_add(p_val, -1/*i_val_m*/, max_adr, min_adr);
                val    = *in & flush_mask;
                in    += istride;
                *p_val = val;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        } END_HWLOOP;
    }
    else //if (((delayLine->len)&1)!=0)
    {
        if (offset >= M)
        {
            incr   = wextract(incr_offset_tab[offset], 0x0808);
            offset = wextract(incr_offset_tab[offset], 0x0800);
            HWLOOP (incr)
            {
                /* put in delay line */
                p_val  = mod_add(p_val, -1, max_adr, min_adr);
                val    = *in & flush_mask;
                in    += istride;
                *p_val = val;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        }
#pragma loop minitercount(4)
        HWLOOP(nbOutRequested)
        {
#ifdef __flexcc2__
            p_val1  = wset_circ_addr(p_val1, wget_circ_addr(p_val));
#else
            p_val1  = p_val;
#endif
            p_val1  = mod_add(p_val1, first_dly, max_adr, min_adr);
            f_index = decimno * offset;
            p_coef1 = &filter[i_coef - f_index + nzcoefs - 1];
            p_coef  = &filter[dfindx + f_index];
            Acc     = 0;
#pragma loop minitercount(4)
            HWLOOP (actualLen)
            {
                Acc      = wX_add(Acc, wX_fmul(*p_val,  *p_coef));
                Acc      = wX_add(Acc, wX_fmul(*p_val1, *p_coef1));
                p_val    = mod_add(p_val,   1/*i_val*/,   max_adr, min_adr);
                p_val1   = mod_add(p_val1, -1/*i_val_m*/, max_adr, min_adr);
                p_coef  += i_coef;
                p_coef1 += i_coef;
            } END_HWLOOP;
            incr   = wextract(incr_offset_tab[offset], 0x0808);
            incr   = wmin(incr, nbin_to_consume);
            offset = wextract(incr_offset_tab[offset], 0x0800);
            p_val  = mod_add(p_val, -actualLen, max_adr, min_adr);
            /* compensate for normalization */
            Acc    = wX_msr(Acc, shift);
            //val = waddr(Acc, Acc0);
            val    = wround_X(Acc);
            *out   = val;
            out   += ostride;
            HWLOOP(incr)
            {
                /* put in delay line */
                p_val  = mod_add(p_val, -1/*i_val_m*/, max_adr, min_adr);
                val    = *in & flush_mask;
                in    += istride;
                *p_val = val;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        } END_HWLOOP;
    } //if (((delayLine->len)&1)!=0)
    delayLine->currIndex = p_val - baseAddress;
    *nbOutAvail          = nbOutRequested;
}



/********** CODE FOR UP SAMPLING ONLY ***************************/
void resample_ML_load_block_fixin_fixout(int               *in,
                                         unsigned int      istride,
                                         unsigned int      nbIn,
                                         ResampleDelayLine *delayLine,
                                         ResampleFilter    *filterInfo,
                                         int               squelch/*, int low_latency*/)
{
    /* mapped data address variables */
    int        *baseAddress = &delayLine->baseAddress[0];
#ifndef __flexcc2__
    int        *p_val;
#else
    int __circ *p_val;
#endif
    int         i_val_m = -1;
    int         nbremain = delayLine->nbremain;
    int         *max_adr = &baseAddress[delayLine->totalen];
    int         *min_adr = baseAddress;
    Float       coef     = filterInfo->coef;

#ifndef __flexcc2__
#ifndef MMDSP
    set_modulo_range1((void *) min_adr, (void *) max_adr);
    set_addressing_mode(MOD_AX1);
#endif
    // make unix here
    p_val  = baseAddress;
    p_val += delayLine->writeIndex;
#else
    p_val = winit_circ_ptr(&baseAddress[delayLine->writeIndex],        /* dummy value, this will be initialized later */
                           min_adr,                                    /* minimum value           */
                           max_adr);                                   /* maximum value +1        */
#endif /* __flexcc2__ */
    if(squelch)
    {
        HWLOOP(nbIn)
        {
            p_val  = mod_add(p_val, i_val_m, max_adr, min_adr);
            *p_val = 0;
            nbremain++;
        }END_HWLOOP;
    }
    else // if (squelch)
    {
        HWLOOP(nbIn)
        {
            p_val  = mod_add(p_val, i_val_m, max_adr, min_adr);
            *p_val = wfmulr(*in, coef);
            in    += istride;
            nbremain++;
        }END_HWLOOP;
    } // if (squelch)
    delayLine->nbremain = nbremain;
    //FIXME0:
    delayLine->writeIndex = p_val - baseAddress;
}



void resample_ML_conv_block_fixin_fixout(int               *out,
                                         unsigned int      ostride,
                                         unsigned int      nbOutRequested,
                                         unsigned int      *nbOutAvail,
                                         ResampleDelayLine *delayLine,
                                         ResampleFilter    *filterInfo,
                                         unsigned int      loop_siz,
                                         Float             *CoefRam)
{
    unsigned int     M                = filterInfo->M;
    const EXTERN int *incr_offset_tab = filterInfo->incr_offset_tab;
    int              shift            = filterInfo->shift;
    unsigned int     actualLen;
    int              currIndex        = delayLine->currIndex;
    int              f_index;
    unsigned int     offset           = delayLine->offset;
    int              nbremain         = delayLine->nbremain;
    int              val;
    int              decimno          = filterInfo->decimno;
    WORD56           Acc;
    const YMEM Float *filter          = filterInfo->filter;
    const YMEM Float *p_coef, *p_coef1;
    int              i_coef;
    int              incr;
    int              nbin_to_consume;
    /* mapped data address variables */
    int              *baseAddress = &delayLine->baseAddress[0];
#ifndef __flexcc2__
    int              *p_val;
    int              *p_val1;
#else
    int __circ       *p_val;
    int __circ       *p_val1;
#endif
    int              first_dly = delayLine->len - 1;
    int              *max_adr  = &baseAddress[delayLine->totalen];
    int              *min_adr  = baseAddress;
    int              nzcoefs   = filterInfo->nzcoefs;
    int              dfindx    = nzcoefs - delayLine->len * M * decimno + filterInfo->ncfs;

#ifndef __flexcc2__
#ifndef MMDSP
    set_modulo_range1((void *) min_adr, (void *) max_adr);
    set_addressing_mode(MOD_AX1);
#endif
    // make unix here
    p_val  = baseAddress;
    p_val += currIndex;
    p_val1 = p_val;
#else
    p_val  = winit_circ_ptr(&baseAddress[currIndex],        /* dummy value, this will be initialized later */
                            min_adr,                        /* minimum value           */
                            max_adr);                       /* maximum value +1        */
    p_val1 = winit_circ_ptr(&baseAddress[currIndex],        /* dummy value, this will be initialized later */
                            min_adr,                        /* minimum value           */
                            max_adr);                       /* maximum value +1        */
#endif /* __flexcc2__ */
    actualLen = delayLine->len / 2;
    i_coef    = decimno * M;
    assert(actualLen > 1);
    nbin_to_consume = loop_siz - nbOutRequested;
    if ((delayLine->len & 1) != 0)
    {
        if (offset >= M)
        {
            incr   = wextract(incr_offset_tab[offset], 0x0808);
            offset = wextract(incr_offset_tab[offset], 0x0800);
            HWLOOP (incr)
            {
                p_val = mod_add(p_val, -1, max_adr, min_adr);
                nbremain--;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        }
#pragma loop minitercount(4)
        HWLOOP (nbOutRequested)
        {
#ifdef __flexcc2__
            p_val1  = wset_circ_addr(p_val1, wget_circ_addr(p_val));
#else
            p_val1  = p_val;
#endif
            p_val1  = mod_add(p_val1,first_dly,max_adr,min_adr);
            f_index = offset * decimno;
            p_coef1 = &filter[i_coef - f_index + nzcoefs - 1];
            p_coef  = &filter[dfindx + f_index];
            Acc     = 0;
#pragma loop minitercount(4)
            HWLOOP (actualLen)
            {
                Acc      = wX_add(Acc, wX_fmul(*p_val,  *p_coef));
                Acc      = wX_add(Acc, wX_fmul(*p_val1, *p_coef1));
                p_val    = mod_add(p_val,   1/*i_val*/,   max_adr, min_adr);
                p_val1   = mod_add(p_val1, -1/*i_val_m*/, max_adr, min_adr);
                p_coef1 += i_coef;
                p_coef  += i_coef;
            } END_HWLOOP;
            incr   = wextract(incr_offset_tab[offset], 0x0808);
            incr   = wmin(incr, nbin_to_consume);
            offset = wextract(incr_offset_tab[offset], 0x0800);
            Acc    = wX_add(Acc,wX_fmul(*p_val, *p_coef));
            p_val  = mod_add(p_val, -actualLen, max_adr, min_adr);
            /* compensate for normalization */
            Acc    = wX_msr(Acc, shift);
            //val = waddr(Acc, Acc0);
            val    = wround_X(Acc);
            *out   = val;
            out   += ostride;
            HWLOOP(incr)
            {
                /* put in delay line */
                p_val = mod_add(p_val, -1/*i_val_m*/, max_adr, min_adr);
                nbremain--;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        } END_HWLOOP;
    }
    else //if (((delayLine->len)&1)!=0)
    {
        if (offset >= M)
        {
            incr   = wextract(incr_offset_tab[offset], 0x0808);
            offset = wextract(incr_offset_tab[offset], 0x0800);
            HWLOOP (incr)
            {
                /* put in delay line */
                p_val = mod_add(p_val, -1, max_adr, min_adr);
                nbremain--;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        }
#pragma loop minitercount(4)
        HWLOOP(nbOutRequested)
        {
#ifdef __flexcc2__
            p_val1  = wset_circ_addr(p_val1, wget_circ_addr(p_val));
#else
            p_val1  = p_val;
#endif
            p_val1  = mod_add(p_val1, first_dly, max_adr, min_adr);
            f_index = decimno * offset;
            p_coef1 = &filter[i_coef - f_index + nzcoefs - 1];
            p_coef  = &filter[dfindx + f_index];
            Acc     = 0;
#pragma loop minitercount(4)
            HWLOOP (actualLen)
            {
                Acc      = wX_add(Acc, wX_fmul(*p_val,  *p_coef));
                Acc      = wX_add(Acc, wX_fmul(*p_val1, *p_coef1));
                p_val    = mod_add(p_val,   1/*i_val*/,   max_adr, min_adr);
                p_val1   = mod_add(p_val1, -1/*i_val_m*/, max_adr, min_adr);
                p_coef  += i_coef;
                p_coef1 += i_coef;
            } END_HWLOOP;
            incr   = wextract(incr_offset_tab[offset], 0x0808);
            incr   = wmin(incr,nbin_to_consume);
            offset = wextract(incr_offset_tab[offset], 0x0800);
            p_val  = mod_add(p_val, -actualLen, max_adr, min_adr);
            /* compensate for normalization */
            Acc    = wX_msr(Acc, shift);
            //val = waddr(Acc, Acc0);
            val    = wround_X(Acc);
            *out   = val;
            out   += ostride;
            HWLOOP(incr)
            {
                /* put in delay line */
                p_val = mod_add(p_val, -1/*i_val_m*/, max_adr, min_adr);
                nbremain--;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        } END_HWLOOP;
    } //if (((delayLine->len)&1)!=0)
    delayLine->nbremain  = nbremain;
    delayLine->currIndex = p_val - baseAddress;
    *nbOutAvail          = nbOutRequested;
}



/********** CODE FOR DOWN SAMPLING ONLY *****************/
void resample_ML_convout_block_fixin_fixout(int               *in,
                                            unsigned int      istride,
                                            unsigned int      nbIn,
                                            ResampleRemainbuf *resampleremainbuf,
                                            unsigned int      nbOutRequested,
                                            unsigned int      *nbOutAvail,
                                            ResampleDelayLine *delayLine,
                                            ResampleFilter    *filterInfo,
                                            unsigned int      loop_siz,
                                            unsigned int      block_siz,
                                            Float             *CoefRam)
{
    unsigned int     M                = filterInfo->M;
    const EXTERN int *incr_offset_tab = filterInfo->incr_offset_tab;
    int              shift            = filterInfo->shift;
    unsigned int     actualLen;
    int              currIndex        = delayLine->currIndex;
    int              f_index;
    unsigned int     offset           = delayLine->offset;
    int              val;
    int              decimno          = filterInfo->decimno;
    WORD56           Acc;
    const YMEM Float *filter          = filterInfo->filter;
    const YMEM Float *p_coef, *p_coef1;
    int              i_coef;
    int              incr;
    int              nbin_to_consume;
    /* mapped data address variables */
    int              *baseAddress        = &delayLine->baseAddress[0];
    int              *baseAddress_remain = resampleremainbuf->baseAddress;
#ifndef __flexcc2__
    int              *p_val;
    int              *p_val1;
    int              *p_remain;
#else
    int __circ       *p_val;
    int __circ       *p_val1;
    int __circ       *p_remain;
#endif
    int              first_dly       = delayLine->len - 1;
    int              *max_adr        = &baseAddress[delayLine->len];
    int              *min_adr        = baseAddress;
    int              *max_adr_remain = &baseAddress_remain[block_siz + 1];
    int              *min_adr_remain = &baseAddress_remain[0];
    int              nzcoefs         = filterInfo->nzcoefs;
    int              dfindx          = nzcoefs - delayLine->len * M * decimno + filterInfo->ncfs;
    int              flush_mask;

    if (nbIn == 0)
        flush_mask = 0;
    else
        flush_mask = -1;
    if ((filterInfo->coef != 0 ) && (filterInfo->coef < MAXVAL - 1) && (nbIn > 0))
    {
        /* this is a pure downsampling filter, compensate for normalization of
        the filter coefficients */
        vector_scale_in_place(in, filterInfo->coef, nbIn, istride);
    }
#ifndef __flexcc2__
#ifndef MMDSP
    set_modulo_range1((void *) min_adr, (void *) max_adr);
    set_addressing_mode(MOD_AX1);
#endif
    // make unix here
    p_val     = baseAddress;
    p_val    += currIndex;
    p_val1    = p_val;
    p_remain  = baseAddress_remain;
    p_remain += resampleremainbuf->writeptr;
#else
    p_val     = winit_circ_ptr(&baseAddress[currIndex],        /* dummy value, this will be initialized later */
                               min_adr,                        /* minimum value           */
                               max_adr);                       /* maximum value +1        */
    p_val1    = winit_circ_ptr(&baseAddress[currIndex],        /* dummy value, this will be initialized later */
                               min_adr,                        /* minimum value           */
                               max_adr);                       /* maximum value +1        */
    p_remain  = winit_circ_ptr(&baseAddress_remain[resampleremainbuf->writeptr],
                               min_adr_remain,
                               max_adr_remain);
#endif /* __flexcc2__ */
    actualLen = delayLine->len / 2;
    i_coef    = decimno * M;
    assert(actualLen > 1);
    nbin_to_consume=loop_siz-nbOutRequested;
    if ((delayLine->len & 1) != 0)
    {
        if (offset >= M)
        {
            incr   = wextract(incr_offset_tab[offset], 0x0808);
            offset = wextract(incr_offset_tab[offset], 0x0800);
            HWLOOP (incr)
            {
                p_val  = mod_add(p_val, -1, max_adr, min_adr);
                val    = *in & flush_mask;
                in    += istride;
                *p_val = val;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        }
#pragma loop minitercount(4)
        HWLOOP (nbOutRequested)
        {
#ifdef __flexcc2__
            p_val1 = wset_circ_addr(p_val1, wget_circ_addr(p_val));
#else
            p_val1 = p_val;
#endif
            p_val1  = mod_add(p_val1, first_dly, max_adr, min_adr);
            f_index = offset * decimno;
            p_coef1 = &filter[i_coef - f_index + nzcoefs - 1];
            p_coef  = &filter[dfindx + f_index];
            Acc     = 0;
#pragma loop minitercount(4)
            HWLOOP (actualLen)
            {
                Acc     = wX_add(Acc, wX_fmul(*p_val,  *p_coef));
                Acc     = wX_add(Acc, wX_fmul(*p_val1, *p_coef1));
                p_val   = mod_add(p_val,   1/*i_val*/,   max_adr, min_adr);
                p_val1  = mod_add(p_val1, -1/*i_val_m*/, max_adr, min_adr);
                p_coef1 += i_coef;
                p_coef  += i_coef;
            } END_HWLOOP;
            incr      = wextract(incr_offset_tab[offset], 0x0808);
            incr      = wmin(incr,nbin_to_consume);
            offset    = wextract(incr_offset_tab[offset], 0x0800);
            Acc       = wX_add(Acc, wX_fmul(*p_val, *p_coef));
            p_val     = mod_add(p_val, -actualLen, max_adr, min_adr);
            /* compensate for normalization */
            Acc       = wX_msr(Acc, shift);
            //val = waddr(Acc, Acc0);
            val       = wround_X(Acc);
            *p_remain = val;
            p_remain  = mod_add(p_remain, 1, max_adr_remain, min_adr_remain);
            HWLOOP(incr)
            {
                /* put in delay line */
                p_val  = mod_add(p_val, -1/*i_val_m*/, max_adr, min_adr);
                val    = *in & flush_mask;
                in    += istride;
                *p_val = val;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        } END_HWLOOP;
    }
    else //if (((delayLine->len)&1)!=0)
    {
        if (offset >= M)
        {
            incr   = wextract(incr_offset_tab[offset], 0x0808);
            offset = wextract(incr_offset_tab[offset], 0x0800);
            HWLOOP (incr)
            {
                /* put in delay line */
                p_val  = mod_add(p_val, -1, max_adr, min_adr);
                val    = *in & flush_mask;
                in    += istride;
                *p_val = val;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        }
#pragma loop minitercount(4)
        HWLOOP(nbOutRequested)
        {
#ifdef __flexcc2__
            p_val1  = wset_circ_addr(p_val1, wget_circ_addr(p_val));
#else
            p_val1  = p_val;
#endif
            p_val1  = mod_add(p_val1, first_dly, max_adr, min_adr);
            f_index = decimno*offset;
            p_coef1 = &filter[i_coef - f_index + nzcoefs - 1];
            p_coef  = &filter[dfindx + f_index];

            Acc     = 0;
#pragma loop minitercount(4)
            HWLOOP (actualLen)
            {
                Acc      = wX_add(Acc, wX_fmul(*p_val,  *p_coef));
                Acc      = wX_add(Acc, wX_fmul(*p_val1, *p_coef1));
                p_val    = mod_add(p_val,   1/*i_val*/,   max_adr, min_adr);
                p_val1   = mod_add(p_val1, -1/*i_val_m*/, max_adr, min_adr);
                p_coef  += i_coef;
                p_coef1 += i_coef;
            } END_HWLOOP;
            incr      = wextract(incr_offset_tab[offset], 0x0808);
            incr      = wmin(incr, nbin_to_consume);
            offset    = wextract(incr_offset_tab[offset], 0x0800);
            p_val     = mod_add(p_val, -actualLen, max_adr, min_adr);
            /* compensate for normalization */
            Acc       = wX_msr(Acc, shift);
            //val = waddr(Acc, Acc0);
            val       = wround_X(Acc);
            *p_remain = val;
            p_remain  = mod_add(p_remain, 1, max_adr_remain, min_adr_remain);
            HWLOOP(incr)
            {
                /* put in delay line */
                p_val  = mod_add(p_val, -1/*i_val_m*/, max_adr, min_adr);
                val    = *in & flush_mask;
                in    += istride;
                *p_val = val;
            } END_HWLOOP;
            nbin_to_consume -= incr;
        } END_HWLOOP;
    } //if (((delayLine->len)&1)!=0)
    delayLine->currIndex         = p_val - baseAddress;
    *nbOutAvail                  = nbOutRequested;
    resampleremainbuf->nbremain += nbOutRequested;
    resampleremainbuf->writeptr  = p_remain - baseAddress_remain;
}



void resample_ML_store_block_fixin_fixout(ResampleRemainbuf *resampleremainbuf,
                                          int               *out,
                                          unsigned int      ostride,
                                          unsigned int      nbOutRequested,
                                          unsigned int      block_siz,
										  unsigned int		ARM_Sample16)
{
    unsigned int iter;
#ifdef __flexcc2__
    int __circ *p_val;
#else
    int        *p_val;
#endif
    int        *baseAddress = resampleremainbuf->baseAddress;
    int        *max_adr     = &baseAddress[block_siz + 1];
    int        *min_adr     = &baseAddress[0];

#ifdef __flexcc2__
    p_val  = winit_circ_ptr(&baseAddress[resampleremainbuf->readptr], min_adr, max_adr);
#else
    p_val  = baseAddress;
    p_val += resampleremainbuf->readptr;
#endif
    HWLOOP(nbOutRequested)
    {
        *out  = *p_val;
        p_val = mod_add(p_val, 1, max_adr, min_adr);
        out  += ostride;
    } END_HWLOOP;
    resampleremainbuf->readptr   = p_val - baseAddress;
    resampleremainbuf->nbremain -= nbOutRequested;
}



void resample_ML_convout_block_fixin_fixout_ll(int               *in,
                                               unsigned int      istride,
                                               unsigned int      nbIn,
                                               ResampleRemainbuf *resampleremainbuf,
                                               unsigned int      nbOutRequested,
                                               unsigned int      *nbOutAvail,
                                               ResampleDelayLine *delayLine,
                                               ResampleFilter    *filterInfo,
                                               unsigned int      loop_siz,
                                               unsigned int      block_siz,
                                               Float             *CoefRam)
{
    unsigned int     M                = filterInfo->M;
    int              decimno          = filterInfo->decimno;
    const YMEM Float *filter          = filterInfo->filter;
    const EXTERN int *incr_offset_tab = filterInfo->incr_offset_tab;
    int              shift            = filterInfo->shift;
    int              currIndex        = delayLine->currIndex;
    unsigned int     offset           = delayLine->offset;
    int              f_index;
    WORD56           Acc;
    int              val;
    /* mapped filter address variables */
    const YMEM Float *p_coef;
    int              incr;
    int              nbin_to_consume;
    int              i_coef;
    /* mapped data address variables */
    int              *baseAddress        = &delayLine->baseAddress[0];
    int              *baseAddress_remain = resampleremainbuf->baseAddress;
#ifndef __flexcc2__
    int              *p_val;
    int              *p_remain;
#else
    int __circ       *p_val;
    int __circ       *p_remain;
#endif
    int              *max_adr        = &baseAddress[delayLine->len];
    int              *min_adr        = baseAddress;
    int              *max_adr_remain = &baseAddress_remain[block_siz + 1];
    int              *min_adr_remain = &baseAddress_remain[0];
    int              flush_mask;

    if (nbIn == 0)
        flush_mask = 0;
    else
        flush_mask = -1;
    if ((filterInfo->coef != 0) && (filterInfo->coef < MAXVAL - 1) && (nbIn > 0))
    {
        /* this is a pure downsampling filter, compensate for normalization of
           the filter coefficients */
        vector_scale_in_place(in, filterInfo->coef, nbIn, istride);
    }
    i_coef = decimno * M;
#ifndef __flexcc2__
#ifndef MMDSP
    set_modulo_range1((void *) min_adr, (void *) max_adr);
    set_addressing_mode(MOD_AX1);
#endif
    // make unix here
    p_val     = baseAddress;
    p_val    += currIndex;
    p_remain  = baseAddress_remain;
    p_remain += resampleremainbuf->writeptr;
#else

    p_val    = winit_circ_ptr(&baseAddress[currIndex],        /* dummy value, this will be initialized later */
                              min_adr,                        /* minimum value           */
                              max_adr);                       /* maximum value +1        */

    p_remain = winit_circ_ptr(&baseAddress_remain[resampleremainbuf->writeptr],
                              min_adr_remain,
                              max_adr_remain);

#endif /* __flexcc2__ */
    nbin_to_consume = loop_siz-nbOutRequested;
    /*update dlyline if needed*/
    if (offset >= M)
    {
        incr   = wextract(incr_offset_tab[offset], 0x0808);
        offset = wextract(incr_offset_tab[offset], 0x0800);
        HWLOOP (incr)
        {
            p_val  = mod_add(p_val, -1, max_adr, min_adr);
            val    = *in & flush_mask;
            in    += istride;
            *p_val = val;

        } END_HWLOOP;
        nbin_to_consume -= incr;
    }
    /*main loop*/
#pragma loop minitercount(2)
    HWLOOP (nbOutRequested)
    {
        /*convolution*/
        f_index = offset * decimno;
        p_coef  = &filter[f_index];
        Acc     = 0;
#pragma loop minitercount(4)
        HWLOOP (delayLine->len)
        {
            Acc     = wX_add(Acc, wX_fmul(*p_val, *p_coef));
            p_val   = mod_add(p_val, 1, max_adr, min_adr);
            p_coef += i_coef;
        } END_HWLOOP;
        incr      = wextract(incr_offset_tab[offset], 0x0808);
        offset    = wextract(incr_offset_tab[offset], 0x0800);
        incr      = wmin(incr, nbin_to_consume);
        /* compensate for normalization */
        Acc       = wX_msr(Acc, shift);
        val       = wround_X(Acc);
        *p_remain = val;
        p_remain  = mod_add(p_remain, 1, max_adr_remain, min_adr_remain);
        /* update dlyline  */
        HWLOOP (incr)
        {
            p_val  = mod_add(p_val, -1, max_adr, min_adr);
            val    = *in & flush_mask;
            in    += istride;
            *p_val = val;
        } END_HWLOOP;
        nbin_to_consume -= incr;
    } END_HWLOOP;
    delayLine->currIndex         = p_val - baseAddress;
    *nbOutAvail                  = nbOutRequested;
    resampleremainbuf->nbremain += nbOutRequested;
    resampleremainbuf->writeptr  = p_remain - baseAddress_remain;
}



void resample_ML_conv_block_fixin_fixout_ll(int               *out,
                                            unsigned int      ostride,
                                            unsigned int      nbOutRequested,
                                            unsigned int      *nbOutAvail,
                                            ResampleDelayLine *delayLine,
                                            ResampleFilter    *filterInfo,
                                            unsigned int      loop_siz,
                                            Float             *CoefRam)
{
    unsigned int     M         = filterInfo->M;
    int              decimno   = filterInfo->decimno;
    const YMEM Float *filter   = filterInfo->filter;
    int              shift     = filterInfo->shift;
    int              currIndex = delayLine->currIndex;
    unsigned int     offset    = delayLine->offset;
    int              nbremain  = delayLine->nbremain;
    int              f_index;
    WORD56           Acc;
    int              val;
    /* mapped filter address variables */
    const YMEM Float *p_coef;
    const EXTERN int *incr_offset_tab = filterInfo->incr_offset_tab;
    int              incr;
    int              nbin_to_consume;
    int              i_coef;
    /* mapped data address variables */
    int              *baseAddress = &delayLine->baseAddress[0];
#ifndef __flexcc2__
    int              *p_val;
#else
    int __circ       *p_val;
#endif
    int              *max_adr = &baseAddress[delayLine->totalen];
    int              *min_adr = baseAddress;

    i_coef = decimno * M;
#ifndef __flexcc2__
#ifndef MMDSP
    set_modulo_range1((void *) min_adr, (void *) max_adr);
    set_addressing_mode(MOD_AX1);
#endif
    // make unix here
    p_val  = baseAddress;
    p_val += currIndex;
#else
    p_val  = winit_circ_ptr(&baseAddress[currIndex],        /* dummy value, this will be initialized later */
                            min_adr,                        /* minimum value           */
                            max_adr);                       /* maximum value +1        */
#endif /* __flexcc2__ */
    nbin_to_consume = loop_siz - nbOutRequested;
    /*update dlyline if needed*/
    if (offset >= M)
    {
        incr   = wextract(incr_offset_tab[offset], 0x0808);
        offset = wextract(incr_offset_tab[offset], 0x0800);
        HWLOOP (incr)
        {
            p_val = mod_add(p_val, -1, max_adr, min_adr);
            nbremain--;
        } END_HWLOOP;
        nbin_to_consume -= incr;
    }
    /*main loop*/
#pragma loop minitercount(2)
    HWLOOP (nbOutRequested)
    {
        /*convolution*/
        f_index = decimno * offset;
        p_coef  = &filter[f_index];
        Acc     = 0;
#pragma loop minitercount(4)
        HWLOOP (delayLine->len)
        {
            Acc     = wX_add(Acc,wX_fmul(*p_val, *p_coef));
            p_val   = mod_add(p_val, 1, max_adr, min_adr);
            p_coef += i_coef;
        } END_HWLOOP;
        incr   = wextract(incr_offset_tab[offset], 0x0808);
        offset = wextract(incr_offset_tab[offset], 0x0800);
        incr   = wmin(incr,nbin_to_consume);
        p_val  = mod_add(p_val, -delayLine->len, max_adr, min_adr);
        /* compensate for normalization */
        Acc    = wX_msr(Acc, shift);
        val    = wround_X(Acc);
        *out   = val;
        out   += ostride;
        /* update dlyline  */
        HWLOOP (incr)
        {
            p_val = mod_add(p_val, -1, max_adr, min_adr);
            nbremain--;
        } END_HWLOOP;
        nbin_to_consume -= incr;
    } END_HWLOOP;
    delayLine->nbremain  = nbremain;
    delayLine->currIndex = p_val - baseAddress;
    *nbOutAvail          = nbOutRequested;
    delayLine->offset    = offset;
}



void resample_ML_filter_block_fixin_fixout_ll(int               *in,
                                              unsigned int      istride,
                                              unsigned int      nbIn,
                                              int               *out,
                                              unsigned int      ostride,
                                              unsigned int      nbOutRequested,
                                              unsigned int      *nbOutAvail,
                                              ResampleDelayLine *delayLine,
                                              ResampleFilter    *filterInfo,
                                              unsigned int      loop_siz,
                                              Float             *CoefRam,
                                              int               squelch)
{
    unsigned int     M                = filterInfo->M;
    int              decimno          = filterInfo->decimno;
    const YMEM Float *filter          = filterInfo->filter;
    const EXTERN int *incr_offset_tab = filterInfo->incr_offset_tab;
    int              shift            = filterInfo->shift;
    int              currIndex        = delayLine->currIndex;
    unsigned int     offset           = delayLine->offset;
    int              f_index;
    WORD56           Acc;
    int              val;
    /* mapped filter address variables */
    const YMEM Float *p_coef;
    int              incr;
    int              nbin_to_consume;
    int              i_coef;
    /* mapped data address variables */
    int              *baseAddress = &delayLine->baseAddress[0];
#ifndef __flexcc2__
    int              *p_val;
#else
    int __circ       *p_val;
#endif
    int              *max_adr = &baseAddress[delayLine->len];
    int              *min_adr = baseAddress;
    int              flush_mask;

    if ((nbIn == 0) || (squelch != 0))
        flush_mask = 0;
    else
        flush_mask = -1;
    if ((filterInfo->coef != 0) && (filterInfo->coef < MAXVAL - 1) && (nbIn > 0))
    {
        /* this is a pure downsampling filter, compensate for normalization of
           the filter coefficients */
        vector_scale_in_place(in, filterInfo->coef, nbIn, istride);
    }
    i_coef = decimno * M;
#ifndef __flexcc2__
#ifndef MMDSP
    set_modulo_range1((void *) min_adr, (void *) max_adr);
    set_addressing_mode(MOD_AX1);
#endif
    // make unix here
    p_val  = baseAddress;
    p_val += currIndex;
#else
    p_val  = winit_circ_ptr(&baseAddress[currIndex],        /* dummy value, this will be initialized later */
                            min_adr,                        /* minimum value           */
                            max_adr);                       /* maximum value +1        */
#endif /* __flexcc2__ */
    nbin_to_consume = loop_siz - nbOutRequested;
    if (offset >= M)
    {
        incr   = wextract(incr_offset_tab[offset], 0x0808);
        offset = wextract(incr_offset_tab[offset], 0x0800);
        HWLOOP (incr)
        {
            /* put in delay line */
            p_val  = mod_add(p_val, -1, max_adr, min_adr);
            val    = *in & flush_mask;
            in    += istride;
            *p_val = val;
        } END_HWLOOP;
        nbin_to_consume -= incr;
    }
#pragma loop minitercount(4)
    HWLOOP(nbOutRequested)
    {
        f_index = decimno * offset;
        p_coef  = &filter[f_index];
        Acc     = 0;
#pragma loop minitercount(4)
        HWLOOP (delayLine->len)
        {
            Acc     = wX_add(Acc, wX_fmul(*p_val, *p_coef));
            p_val   = mod_add(p_val, 1, max_adr, min_adr);
            p_coef += i_coef;
        } END_HWLOOP;
        incr   = wextract(incr_offset_tab[offset], 0x0808);
        offset = wextract(incr_offset_tab[offset], 0x0800);
        incr   = wmin(incr, nbin_to_consume);
        p_val  = mod_add(p_val, -delayLine->len, max_adr, min_adr);
        /* compensate for normalization */
        Acc    = wX_msr(Acc, shift);
        val    = wround_X(Acc);
        *out   = val;
        out   += ostride;
        HWLOOP(incr)
        {
            /* put in delay line */
            p_val  = mod_add(p_val, -1, max_adr, min_adr);
            val    = *in & flush_mask;
            in    += istride;
            *p_val = val;
        } END_HWLOOP;
        nbin_to_consume -= incr;
    } END_HWLOOP;
    delayLine->currIndex = p_val - baseAddress;
    *nbOutAvail          = nbOutRequested;
}
#endif // #if !defined(ARM) && !defined(NEON)
#endif //MMDSP

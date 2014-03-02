/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   effect.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <transducer_equalizer/nmfil/effect.nmf>
#include <inc/archi-wrapper.h>
#include <stdlib.h>
#include <stwdsp.h>
#include <malloc/include/heap.h>
#include "fwmalloc.h"
#include "dbc.h"
#include "libeffects/libtransducer_equalizer/include/transducer_equalizer.h"
#include "libeffects/libtransducer_equalizer/include/effect_common.h"

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////

static t_heap_info EXTMEM mHeapInfo[MEM_BANK_COUNT];

t_uint24 EXTMEM ATTR(XMemUsed)     = 0;
t_uint24 EXTMEM ATTR(YMemUsed)     = 0;
t_uint24 EXTMEM ATTR(DDR24MemUsed) = 0;
t_uint24 EXTMEM ATTR(DDR16MemUsed) = 0;
t_uint24 EXTMEM ATTR(ESR24MemUsed) = 0;
t_uint24 EXTMEM ATTR(ESR16MemUsed) = 0;

static        TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T  mTrEqStruct;
static EXTMEM TransducerEqualizerConfig_t          mTrEqConfig;

static EXTMEM transducer_equalizer_smooth_config_t smooth_config;
static EXTMEM MMshort                              samples_buf[960];


////////////////////////////////////////////////////////////
//          Common Mpc/Host code
////////////////////////////////////////////////////////////

#define TRANSDUCER_EQUALIZER_NMFIL_COMMON_SRC_CODE
#include "libeffects/libtransducer_equalizer/include/effect_common.h"


////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

static int isInitialized = FALSE;

void initialize(void)
{
    int i;


    if(isInitialized)
    {
        return;
    }
    else
    {
        isInitialized = TRUE;
    }

    for(i = 0; i < MEM_BANK_COUNT; i++)
    {
        mHeapInfo[i].Start            = NULL;
        mHeapInfo[i].SizeAvailable    = 0;
        mHeapInfo[i].SizeAllocated    = 0;
        mHeapInfo[i].SizeTotal        = 0;
    }

    common_start(&mTrEqStruct, &smooth_config);
}


void METH(setHeap)(t_memory_bank memory_bank, void *p_buf, t_uint32 size)
{
    int     i;
    t_sword *ptr;


    if(!isInitialized)
    {
        initialize();
    }

    PRECONDITION(!mTrEqStruct.alloc_done);
    PRECONDITION(memory_bank < MEM_BANK_COUNT);
    PRECONDITION(!(((int) p_buf) & 0x1));

    mHeapInfo[memory_bank].Start         = p_buf;
    mHeapInfo[memory_bank].SizeAvailable = size;
    mHeapInfo[memory_bank].SizeAllocated = 0;
    mHeapInfo[memory_bank].SizeTotal     = size;

    // Heap initialization
    ptr = (t_sword *) p_buf;
    for(i = 0; i < size; i++)
    {
        *ptr++ = 0;
    }
}


void METH(setParameter)(TransducerEqualizerParams_t StaticParams)
{
    if(!isInitialized)
    {
        PRECONDITION(0);
        initialize();
    }

    PRECONDITION((StaticParams.iMemoryPreset >= MEM_DEFAULT) && (StaticParams.iMemoryPreset<= MEM_MIX_ESRAM_OTHER_5));
    mTrEqStruct.memory_preset = StaticParams.iMemoryPreset;

    common_setParameter(&mTrEqStruct, &StaticParams);
}


void METH(setConfig)(t_uint32 DynamicParamsFifoDspAddress)
{
    TransducerEqualizerConfigStructFifo_t *p_TransducerEqualizerConfigStructFifo = (TransducerEqualizerConfigStructFifo_t *) DynamicParamsFifoDspAddress;


    if(!isInitialized)
    {
        initialize();
    }

    // flush data cache for making TransducerEqualizerConfigStructFifo readable by mpc
#ifdef __flexcc2__
    MMDSP_FLUSH_DCACHE();
#endif

    // copy config into local structure
    mTrEqConfig = p_TransducerEqualizerConfigStructFifo->config[p_TransducerEqualizerConfigStructFifo->read_index];

    // increment read_index
    if(p_TransducerEqualizerConfigStructFifo->read_index == TEQ_FIFO_CONFIG_SIZE - 1)
    {
        p_TransducerEqualizerConfigStructFifo->read_index = 0;
    }
    else
    {
        p_TransducerEqualizerConfigStructFifo->read_index++;
    }

    // flush data cache for making read_index readable by host
#ifdef __flexcc2__
    MMDSP_FLUSH_DCACHE();
#endif

    // real setConfig
    common_setConfig(&mTrEqStruct, &mTrEqConfig, &smooth_config);
}



#define TRANSDUCER_EQUALIZER_NB_MEM_PRESET 6

void METH(open)(void)
{
    /* first a table for the effect must be defined. This table gather the memory preset
       supported by the effect .
       In our case we support 6 memory preset.
    */

    static MEMORY_PRESET_T EXTMEM const transducer_equalizer_mem_preset_table[TRANSDUCER_EQUALIZER_NB_MEM_PRESET] =
    {
        MEM_ALL_TCM,
        MEM_ALL_DDR,
        MEM_ALL_ESRAM,
        MEM_MIX_ESRAM_TCM,
        MEM_MIX_DDR_TCM_1,
        MEM_MIX_ESRAM_DDR
    };

    /* For each zone allocated, a look up table must be defined to determine in
       which memory the zone must be allocated according the memory preset parameter
    */
    static MEMORY_TYPE_T EXTMEM const transducer_equalizer_malloc_biquad_coef[TRANSDUCER_EQUALIZER_NB_MEM_PRESET] =
    {
        MEM_XTCM,
        MEM_DDR24,
        MEM_ESR24,
        MEM_XTCM,
        MEM_XTCM,
        MEM_ESR24
    };

    static MEMORY_TYPE_T EXTMEM const transducer_equalizer_malloc_FIR_coef[TRANSDUCER_EQUALIZER_NB_MEM_PRESET] =
    {
        MEM_XTCM,
        MEM_DDR24,
        MEM_ESR24,
        MEM_XTCM,
        MEM_XTCM,
        MEM_ESR24
    };

    int index_mem_preset, i, nb_channels;


    PRECONDITION(!mTrEqStruct.alloc_done)

    for(index_mem_preset = TRANSDUCER_EQUALIZER_NB_MEM_PRESET - 1; index_mem_preset > 0; index_mem_preset--)
    {
        if(transducer_equalizer_mem_preset_table[index_mem_preset] == mTrEqStruct.memory_preset)
        {
            break;
        }
    }

    // WARNING : must be aligned with proxy/TEQNmfMpc.cpp RAM heap allocations
    nb_channels = (mTrEqStruct.stereo_config ? 2 : 1);
    if(mTrEqStruct.nb_alloc_biquad_cells_per_channel > 0)
    {
        mTrEqStruct.p_alloc_biquad_coef = (void         *) malloc_core  (&mHeapInfo[transducer_equalizer_malloc_biquad_coef[index_mem_preset]],
                                                                         nb_channels * mTrEqStruct.nb_alloc_biquad_cells_per_channel * sizeof(t_biquad_cell));

        mTrEqStruct.p_biquad_mem        = (MMshort YMEM *) malloc_core_y(&mHeapInfo[MEM_YTCM],
                                                                         nb_channels * 2 * (mTrEqStruct.nb_alloc_biquad_cells_per_channel + 1) * sizeof(MMshort));
        ASSERT(mTrEqStruct.p_alloc_biquad_coef != NULL);
        ASSERT(mTrEqStruct.p_biquad_mem        != NULL);
    }
    if(mTrEqStruct.nb_alloc_FIR_coefs_per_channel > 0)
    {
        MMshort YMEM *p_FIR_mem;

        mTrEqStruct.p_FIR_coef = (MMshort      *) malloc_core  (&mHeapInfo[transducer_equalizer_malloc_FIR_coef[index_mem_preset]],
                                                                nb_channels * mTrEqStruct.nb_alloc_FIR_coefs_per_channel * sizeof(MMshort));

        p_FIR_mem              = (MMshort YMEM *) malloc_core_y(&mHeapInfo[MEM_YTCM],
                                                                nb_channels * mTrEqStruct.nb_alloc_FIR_coefs_per_channel * sizeof(MMshort));
        ASSERT(mTrEqStruct.p_FIR_coef != NULL);
        ASSERT(p_FIR_mem              != NULL);
#ifdef __flexcc2__
        mTrEqStruct.p_FIR_mem = winit_circ_ymem_ptr(p_FIR_mem, p_FIR_mem, p_FIR_mem + nb_channels * mTrEqStruct.nb_alloc_FIR_coefs_per_channel);
#else /* __flexcc2__ */
        mTrEqStruct.p_FIR_mem = p_FIR_mem;
#endif /* __flexcc2__ */
    }

    mTrEqStruct.alloc_done = TRUE;

    // default filter settings (flat response)
    if(!mTrEqStruct.stereo_config)
    {
        mTrEqStruct.same_biquad_l_r = TRUE;
        mTrEqStruct.same_FIR_l_r    = TRUE;
    }
    if(mTrEqStruct.same_biquad_l_r)
    {
        mTrEqStruct.p_biquad_coef_st = (t_biquad_cell_st *) NULL;
        mTrEqStruct.p_biquad_coef    = (t_biquad_cell    *) mTrEqStruct.p_alloc_biquad_coef;
        for(i = 0; i < mTrEqStruct.nb_alloc_biquad_cells_per_channel; i++)
        {
            mTrEqStruct.p_biquad_coef[i].b_exp = 0;
            mTrEqStruct.p_biquad_coef[i].b0    = 0x7FFFFF;
            mTrEqStruct.p_biquad_coef[i].b1    = 0;
            mTrEqStruct.p_biquad_coef[i].b2    = 0;
            mTrEqStruct.p_biquad_coef[i].a1    = 0;
            mTrEqStruct.p_biquad_coef[i].a2    = 0;
        }
    }
    else
    {
        mTrEqStruct.p_biquad_coef_st = (t_biquad_cell_st *) mTrEqStruct.p_alloc_biquad_coef;
        mTrEqStruct.p_biquad_coef    = (t_biquad_cell    *) NULL;
        for(i = 0; i < mTrEqStruct.nb_alloc_biquad_cells_per_channel; i++)
        {
            mTrEqStruct.p_biquad_coef_st[i].b_exp_l = 0;
            mTrEqStruct.p_biquad_coef_st[i].b_exp_r = 0;
            mTrEqStruct.p_biquad_coef_st[i].b0_l    = 0x7FFFFF;
            mTrEqStruct.p_biquad_coef_st[i].b0_r    = 0x7FFFFF;
            mTrEqStruct.p_biquad_coef_st[i].b1_l    = 0;
            mTrEqStruct.p_biquad_coef_st[i].b1_r    = 0;
            mTrEqStruct.p_biquad_coef_st[i].b2_l    = 0;
            mTrEqStruct.p_biquad_coef_st[i].b2_r    = 0;
            mTrEqStruct.p_biquad_coef_st[i].a1_l    = 0;
            mTrEqStruct.p_biquad_coef_st[i].a1_r    = 0;
            mTrEqStruct.p_biquad_coef_st[i].a2_l    = 0;
            mTrEqStruct.p_biquad_coef_st[i].a2_r    = 0;
        }
    }
    if(mTrEqStruct.nb_alloc_FIR_coefs_per_channel > 0)
    {
        if(mTrEqStruct.same_FIR_l_r)
        {
            mTrEqStruct.p_FIR_coef[0] = 0x7FFFFF;
            for(i = 1; i < mTrEqStruct.nb_alloc_FIR_coefs_per_channel; i++)
            {
                mTrEqStruct.p_FIR_coef[i] = 0;
            }
        }
        else
        {
            mTrEqStruct.p_FIR_coef[0] = 0x7FFFFF;
            mTrEqStruct.p_FIR_coef[1] = 0x7FFFFF;
            for(i = 2; i < 2 * mTrEqStruct.nb_alloc_FIR_coefs_per_channel; i++)
            {
                mTrEqStruct.p_FIR_coef[i] = 0;
            }
        }
    }


    if(mTrEqStruct.config_received)
    {
        common_setConfig(&mTrEqStruct, &mTrEqConfig, &smooth_config);
    }
    else
    {
        common_reset(&mTrEqStruct);
        transducer_equalizer_set_filters_pointer(&mTrEqStruct);
    }


    ATTR(XMemUsed)     = mHeapInfo[MEM_XTCM] .SizeAllocated;
    ATTR(YMemUsed)     = mHeapInfo[MEM_YTCM] .SizeAllocated;
    ATTR(DDR24MemUsed) = mHeapInfo[MEM_DDR24].SizeAllocated;
    ATTR(DDR16MemUsed) = mHeapInfo[MEM_DDR16].SizeAllocated;
    ATTR(ESR24MemUsed) = mHeapInfo[MEM_ESR24].SizeAllocated;
    ATTR(ESR16MemUsed) = mHeapInfo[MEM_ESR16].SizeAllocated;
    if(disp_mem.displayMemPreset != NULL)
    {
        disp_mem.displayMemPreset(ATTR(XMemUsed),
                                  ATTR(YMemUsed),
                                  ATTR(DDR24MemUsed),
                                  ATTR(DDR16MemUsed),
                                  ATTR(ESR24MemUsed),
                                  ATTR(ESR16MemUsed));
    }
}


void METH(newFormat)(t_sample_freq *p_freq, t_uint16 *p_chans_nb, t_uint16 *p_sample_bitsize)
{
    if(!mTrEqStruct.alloc_done)
    {
        return;
    }

    if(mTrEqStruct.chan_nb == -1)
    {
        // no data has been processed : number of channels may change
        mTrEqStruct.new_chan_nb = *p_chans_nb;
    }
    else
    {
        // data has been processed : number of channels can't change
        ASSERT(mTrEqStruct.chan_nb == *p_chans_nb);
    }
    mTrEqStruct.sample_freq = *p_freq;
}


void METH(reset)(void)
{
    common_reset(&mTrEqStruct);
}


static void transition1_processBuffer(t_sword **p_p_inbuf, t_sword **p_p_outbuf, int nb_samples)
{
    t_sword *p_inbuf  = *p_p_inbuf;
    t_sword *p_outbuf = *p_p_outbuf;
    int     size, i, shift_div_transition;
    MMshort *p1, *p2;
    MMshort coef_num_smooth_input, coef_num_smooth_output, coef_div_transition;
    MMlong  tmp;


    size = (mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples);
    if(p_inbuf == p_outbuf)
    {
        ASSERT(size * sizeof(MMshort) <= sizeof(samples_buf));
        p1 = samples_buf;
        p2 = p_inbuf;
#ifdef __flexcc2__
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = size; i > 0; i--)
        {
            *p1++ = *p2++;
        }
        p1 = p_outbuf;
        p2 = samples_buf;
    }
    else
    {
        p1 = p_outbuf;
        p2 = p_inbuf;
    }

    transducer_equalizer_filter(size,
                                p_inbuf,
                                p_outbuf,
                                &mTrEqStruct);

    coef_num_smooth_input  = smooth_config.smooth_transition_duration1 - smooth_config.smooth_transition_counter1;
    coef_num_smooth_output = smooth_config.smooth_transition_counter1;
    shift_div_transition   = smooth_config.shift_div_transition1;
    coef_div_transition    = smooth_config.coef_div_transition1;
    if(mTrEqStruct.stereo_config)
    {
        MMshort transition_gain_mant_l = smooth_config.transition_gain_mant_l;
        MMshort transition_gain_exp_l  = smooth_config.transition_gain_exp_l;
        MMshort transition_gain_mant_r = smooth_config.transition_gain_mant_r;
        MMshort transition_gain_exp_r  = smooth_config.transition_gain_exp_r;

#ifdef __flexcc2__
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = nb_samples; i > 0; i--)
        {
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant_l), *p2++), transition_gain_exp_l);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant_r), *p2++), transition_gain_exp_r);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            coef_num_smooth_input++;
            coef_num_smooth_output--;
        }
    }
    else
    {
        MMshort transition_gain_mant = smooth_config.transition_gain_mant_l;
        MMshort transition_gain_exp  = smooth_config.transition_gain_exp_l;

#ifdef __flexcc2__
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = nb_samples; i > 0; i--)
        {
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant), *p2++), transition_gain_exp);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            coef_num_smooth_input++;
            coef_num_smooth_output--;
        }
    }
    smooth_config.smooth_transition_counter1 -= nb_samples;
    *p_p_inbuf                               += size;
    *p_p_outbuf                              += size;
}


static void transition2_processBuffer(t_sword **p_p_inbuf, t_sword **p_p_outbuf, int nb_samples)
{
    t_sword *p_inbuf  = *p_p_inbuf;
    t_sword *p_outbuf = *p_p_outbuf;
    int     size, i, shift_div_transition;
    MMshort *p1, *p2;
    MMshort coef_num_smooth_input, coef_num_smooth_output, coef_div_transition;
    MMlong  tmp;


    size = (mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples);
    if(p_inbuf == p_outbuf)
    {
        ASSERT(size * sizeof(MMshort) <= sizeof(samples_buf));
        p1 = samples_buf;
        p2 = p_inbuf;
#ifdef __flexcc2__
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = size; i > 0; i--)
        {
            *p1++ = *p2++;
        }
        p1 = p_outbuf;
        p2 = samples_buf;
    }
    else
    {
        p1 = p_outbuf;
        p2 = p_inbuf;
    }

    transducer_equalizer_filter(size,
                                p_inbuf,
                                p_outbuf,
                                &mTrEqStruct);

    coef_num_smooth_input  = smooth_config.smooth_transition_counter2;
    coef_num_smooth_output = smooth_config.smooth_transition_duration2 - smooth_config.smooth_transition_counter2;
    shift_div_transition   = smooth_config.shift_div_transition2;
    coef_div_transition    = smooth_config.coef_div_transition2;
    if(mTrEqStruct.stereo_config)
    {
        MMshort transition_gain_mant_l = smooth_config.transition_gain_mant_l;
        MMshort transition_gain_exp_l  = smooth_config.transition_gain_exp_l;
        MMshort transition_gain_mant_r = smooth_config.transition_gain_mant_r;
        MMshort transition_gain_exp_r  = smooth_config.transition_gain_exp_r;

#ifdef __flexcc2__
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = nb_samples; i > 0; i--)
        {
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant_l), *p2++), transition_gain_exp_l);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant_r), *p2++), transition_gain_exp_r);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            coef_num_smooth_input--;
            coef_num_smooth_output++;
        }
    }
    else
    {
        MMshort transition_gain_mant = smooth_config.transition_gain_mant_l;
        MMshort transition_gain_exp  = smooth_config.transition_gain_exp_l;

#ifdef __flexcc2__
#pragma loop force_hwloop
#endif /* __flexcc2__ */
        for(i = nb_samples; i > 0; i--)
        {
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant), *p2++), transition_gain_exp);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            coef_num_smooth_input--;
            coef_num_smooth_output++;
        }
    }
    smooth_config.smooth_transition_counter2 -= nb_samples;
    *p_p_inbuf                               += size;
    *p_p_outbuf                              += size;
}


void METH(processBuffer)(t_sword *p_inbuf, t_sword *p_outbuf, t_uint16 size)
{
    int     nb_samples, nb_channels_FIR, i;
    MMshort *p1, *p2, *p3;
    MMshort coef_smooth_FIR;


    PRECONDITION(mTrEqStruct.alloc_done == TRUE);

    if(mTrEqStruct.chan_nb == -1)
    {
        // no data has been processed : last chan_nb must be less or equal to allocated chan_nb
        // if so, set chan_nb (and offset) to this value and set new config
        if(mTrEqStruct.new_chan_nb == -1)
        {
            mTrEqStruct.new_chan_nb = mTrEqStruct.chan_nb = mTrEqStruct.offset = mTrEqStruct.alloc_chan_nb;
        }
        else
        {
            ASSERT(mTrEqStruct.new_chan_nb == mTrEqStruct.alloc_chan_nb);
        }
    }

    if(!mTrEqStruct.enable)
    {
        if(p_inbuf != p_outbuf)
        {
#ifdef __flexcc2__
#pragma loop force_hwloop
#endif /* __flexcc2__ */
            for(i = 0; i < size; i++)
            {
                p_outbuf[i] = p_inbuf[i];
            }
        }
    }
    else if(!mTrEqStruct.smooth_config_change)
    {
        // no configuration smoothing
        transducer_equalizer_filter(size,
                                    p_inbuf,
                                    p_outbuf,
                                    &mTrEqStruct);
    }
    else
    {
        if(mTrEqStruct.stereo_config)
        {
            nb_samples = (size >> 1);
        }
        else
        {
            nb_samples = size;
        }

        if(smooth_config.smooth_transition_counter1 > 0)
        {
            if(smooth_config.smooth_transition_counter1 > nb_samples)
            {
                // first transition
                transition1_processBuffer(&p_inbuf, &p_outbuf, nb_samples);
            }
            else
            {
                // end of first transition
                nb_samples -= smooth_config.smooth_transition_counter1;
                transition1_processBuffer(&p_inbuf, &p_outbuf, smooth_config.smooth_transition_counter1);

                common_copy_new_config(&mTrEqStruct, &smooth_config);
                common_reset(&mTrEqStruct);
                transducer_equalizer_set_filters_pointer(&mTrEqStruct);

                if(smooth_config.smooth_transition_counter2 == 0)
                {
                    // no second transition : means that transducer_equalizer is now disabled
                    if(p_inbuf != p_outbuf)
                    {
#ifdef __flexcc2__
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                        for(i = (mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples); i > 0; i--)
                        {
                            *p_outbuf++ = *p_inbuf++;
                        }
                    }
                    mTrEqStruct.smooth_config_change = FALSE;
                }
                else
                {
                    if(smooth_config.smooth_transition_counter2 > nb_samples)
                    {
                        // second transition
                        transition2_processBuffer(&p_inbuf, &p_outbuf, nb_samples);
                    }
                    else
                    {
                        // end of second transition
                        nb_samples -= smooth_config.smooth_transition_counter2;
                        transition2_processBuffer(&p_inbuf, &p_outbuf, smooth_config.smooth_transition_counter2);

                        transducer_equalizer_filter((mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples),
                                                    p_inbuf,
                                                    p_outbuf,
                                                    &mTrEqStruct);
                        mTrEqStruct.smooth_config_change = FALSE;
                    }
                }
            }
        }
        else if(smooth_config.smooth_transition_counter2 > 0)
        {
            if(smooth_config.smooth_transition_counter2 > nb_samples)
            {
                // second transition
                transition2_processBuffer(&p_inbuf, &p_outbuf, nb_samples);
            }
            else
            {
                // end of second transition
                nb_samples -= smooth_config.smooth_transition_counter2;
                transition2_processBuffer(&p_inbuf, &p_outbuf, smooth_config.smooth_transition_counter2);

                transducer_equalizer_filter((mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples),
                                            p_inbuf,
                                            p_outbuf,
                                            &mTrEqStruct);
                mTrEqStruct.smooth_config_change = FALSE;
            }
        }
        else
        {
            if(smooth_config.smooth_FIR_transition_counter > 0)
            {
                nb_channels_FIR = ((mTrEqStruct.stereo_config && !mTrEqStruct.same_FIR_l_r) ? 2 : 1);
                while((smooth_config.smooth_FIR_transition_counter > 0) && (nb_samples > 0))
                {
                    coef_smooth_FIR = fract_ratio(smooth_config.smooth_FIR_transition_counter, smooth_config.smooth_FIR_transition_duration);
                    p1              = mTrEqStruct.p_FIR_coef;
                    p2              = smooth_config.new_FIR_coef;
                    p3              = smooth_config.old_FIR_coef;
#ifdef __flexcc2__
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                    for(i = nb_channels_FIR * mTrEqStruct.nb_FIR_coefs_per_channel; i > 0; i--)
                    {
                        *p1 = waddsat(*p2, wfmulr(coef_smooth_FIR, wsubsat(*p3, *p2)));
                        p1++;
                        p2++;
                        p3++;
                    }

                    size = (smooth_config.smooth_FIR_transition_counter > smooth_config.FIR_smoothing_delay_granularity ? smooth_config.FIR_smoothing_delay_granularity : smooth_config.smooth_FIR_transition_counter);
                    size = (size > nb_samples ? nb_samples : size);
                    smooth_config.smooth_FIR_transition_counter -= size;
                    nb_samples                                  -= size;

                    size = (mTrEqStruct.stereo_config ? size * 2 : size);
                    transducer_equalizer_filter(size,
                                                p_inbuf,
                                                p_outbuf,
                                                &mTrEqStruct);
                    p_inbuf  += size;
                    p_outbuf += size;
                }
                if(smooth_config.smooth_FIR_transition_counter == 0)
                {
                    // end of smooth FIR transition
                    p1 = mTrEqStruct.p_FIR_coef;
                    p2 = smooth_config.save_new_FIR_coef;
#ifdef __flexcc2__
#pragma loop force_hwloop
#endif /* __flexcc2__ */
                    for(i = nb_channels_FIR * smooth_config.new_nb_FIR_coefs_per_channel; i > 0; i--)
                    {
                        *p1++ = *p2++;
                    }
                    mTrEqStruct.nb_FIR_coefs_per_channel = smooth_config.new_nb_FIR_coefs_per_channel;
                    mTrEqStruct.same_FIR_l_r             = smooth_config.new_same_FIR_l_r;
                    mTrEqStruct.FIR_gain_exp_l           = smooth_config.new_FIR_gain_exp_l;
                    mTrEqStruct.FIR_gain_mant_l          = smooth_config.new_FIR_gain_mant_l;
                    mTrEqStruct.FIR_gain_exp_r           = smooth_config.new_FIR_gain_exp_r;
                    mTrEqStruct.FIR_gain_mant_r          = smooth_config.new_FIR_gain_mant_r;

                    transducer_equalizer_set_filters_pointer(&mTrEqStruct);

                    if(nb_samples > 0)
                    {
                        size = (mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples);
                        transducer_equalizer_filter(size,
                                                    p_inbuf,
                                                    p_outbuf,
                                                    &mTrEqStruct);
                    }

                    mTrEqStruct.smooth_config_change = FALSE;
                }
            }
            else
            {
                transducer_equalizer_filter(size,
                                            p_inbuf,
                                            p_outbuf,
                                            &mTrEqStruct);

                mTrEqStruct.smooth_config_change = FALSE;
            }
        }

        if(!mTrEqStruct.smooth_config_change)
        {
            smooth_config.old_average_gain_exp_l  = smooth_config.new_average_gain_exp_l;
            smooth_config.old_average_gain_mant_l = smooth_config.new_average_gain_mant_l;
            smooth_config.old_average_gain_exp_r  = smooth_config.new_average_gain_exp_r;
            smooth_config.old_average_gain_mant_r = smooth_config.new_average_gain_mant_r;
        }
    }
}

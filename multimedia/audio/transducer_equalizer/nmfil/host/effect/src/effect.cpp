/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   effect.cpp
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <transducer_equalizer/nmfil/host/effect.nmf>
#include <armnmf_dbc.h>
#include "OMX_Core.h"
#include "bindings_optim.h"
#include "transducer_equalizer/effect/include/TEQ_SHAI.h"



////////////////////////////////////////////////////////////
//          Common Mpc/Host code
////////////////////////////////////////////////////////////

#define TRANSDUCER_EQUALIZER_NMFIL_COMMON_SRC_CODE
#include "libeffects/libtransducer_equalizer/include/effect_common.h"




METH(transducer_equalizer_nmfil_host_effect)(void)
{
    instance           = 0;
    p_buf1_sample32    = NULL;
    p_buf2_sample32    = NULL;
    buf1_sample32_size = 0;
    buf2_sample32_size = 0;
    METH(start)();
}


METH(~transducer_equalizer_nmfil_host_effect)(void)
{
    if(p_buf1_sample32 != NULL)
    {
        free(p_buf1_sample32);
        p_buf1_sample32    = NULL;
        buf1_sample32_size = 0;
    }
    if(p_buf2_sample32 != NULL)
    {
        free(p_buf2_sample32);
        p_buf2_sample32    = NULL;
        buf2_sample32_size = 0;
    }
    METH(stop)();
}



////////////////////////////////////////////////////////////
// Provided Interfaces
////////////////////////////////////////////////////////////

void METH(start)(void)
{
    teq_common_utils::common_start(&mTrEqStruct, &smooth_config);

    effect_process_type_cap = EFFECT_PROCESS_TYPE_INPLACE;
    //effect_process_type_cap = EFFECT_PROCESS_TYPE_NOT_INPLACE;
}


void METH(stop)(void)
{
    if(mTrEqStruct.alloc_done)
    {
        if(mTrEqStruct.nb_alloc_biquad_cells_per_channel > 0)
        {
            free(mTrEqStruct.p_alloc_biquad_coef);
            free(mTrEqStruct.p_biquad_mem);
        }
        if(mTrEqStruct.nb_alloc_FIR_coefs_per_channel > 0)
        {
            free(mTrEqStruct.p_FIR_coef);
            free(mTrEqStruct.p_FIR_mem);
        }

        // reset to start values
        METH(start)();
    }
}


void METH(close)(void)
{
    METH(stop)();
}


void METH(setParameter)(TransducerEqualizerParams_t &StaticParams, t_uint16 instance_uint16)
{
    instance = (int) instance_uint16;

    teq_common_utils::common_setParameter(&mTrEqStruct, &StaticParams);
}


void METH(setConfig)(TransducerEqualizerConfig_t &DynamicParams)
{
    mTrEqStruct.config_received = TRUE;
    mTrEqConfig                 = DynamicParams;

    teq_common_utils::common_setConfig(&mTrEqStruct, &mTrEqConfig, &smooth_config);
}


t_bool METH(open)(const t_host_effect_config *config, t_effect_caps *caps)
{
    int i, nb_channels;


    ARMNMF_DBC_PRECONDITION(!mTrEqStruct.alloc_done)

    host_effect_config = *config;

    ARMNMF_DBC_PRECONDITION(host_effect_config.block_size > 0);
    ARMNMF_DBC_PRECONDITION((host_effect_config.infmt.nof_channels > 0) && (host_effect_config.infmt.nof_channels == host_effect_config.outfmt.nof_channels));
    ARMNMF_DBC_PRECONDITION(((host_effect_config.infmt.nof_bits_per_sample == 16) || (host_effect_config.infmt.nof_bits_per_sample == 32)) && (host_effect_config.infmt.nof_bits_per_sample == host_effect_config.outfmt.nof_bits_per_sample));
    ARMNMF_DBC_PRECONDITION(host_effect_config.infmt.freq == host_effect_config.outfmt.freq);

    mTrEqStruct.offset = mTrEqStruct.chan_nb = mTrEqStruct.new_chan_nb = host_effect_config.infmt.nof_channels;

    nb_channels = (mTrEqStruct.stereo_config ? 2 : 1);
    if(mTrEqStruct.nb_alloc_biquad_cells_per_channel > 0)
    {
        mTrEqStruct.p_alloc_biquad_coef = (void    *) malloc(nb_channels * mTrEqStruct.nb_alloc_biquad_cells_per_channel * sizeof(t_biquad_cell));

        mTrEqStruct.p_biquad_mem        = (MMshort *) malloc(nb_channels * 2 * (mTrEqStruct.nb_alloc_biquad_cells_per_channel + 1) * sizeof(MMshort));
        ARMNMF_DBC_ASSERT(mTrEqStruct.p_alloc_biquad_coef != NULL);
        ARMNMF_DBC_ASSERT(mTrEqStruct.p_biquad_mem        != NULL);
    }
    if(mTrEqStruct.nb_alloc_FIR_coefs_per_channel > 0)
    {
        MMshort *p_FIR_mem;

        mTrEqStruct.p_FIR_coef = (MMshort *) malloc(nb_channels * mTrEqStruct.nb_alloc_FIR_coefs_per_channel * sizeof(MMshort));

        p_FIR_mem              = (MMshort *) malloc(nb_channels * (mTrEqStruct.nb_alloc_FIR_coefs_per_channel + FIR_MEM_OVERHEAD) * sizeof(MMshort));
        mTrEqStruct.FIR_index  = nb_channels * FIR_MEM_OVERHEAD;
        ARMNMF_DBC_ASSERT(mTrEqStruct.p_FIR_coef != NULL);
        ARMNMF_DBC_ASSERT(p_FIR_mem              != NULL);
        mTrEqStruct.p_FIR_mem = p_FIR_mem;
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
            mTrEqStruct.p_biquad_coef[i].b0    = 0x7FFFFFFF;
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
            mTrEqStruct.p_biquad_coef_st[i].b0_l    = 0x7FFFFFFF;
            mTrEqStruct.p_biquad_coef_st[i].b0_r    = 0x7FFFFFFF;
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
            mTrEqStruct.p_FIR_coef[0] = 0x7FFFFFFF;
            for(i = 1; i < mTrEqStruct.nb_alloc_FIR_coefs_per_channel; i++)
            {
                mTrEqStruct.p_FIR_coef[i] = 0;
            }
        }
        else
        {
            mTrEqStruct.p_FIR_coef[0] = 0x7FFFFFFF;
            mTrEqStruct.p_FIR_coef[1] = 0x7FFFFFFF;
            for(i = 2; i < 2 * mTrEqStruct.nb_alloc_FIR_coefs_per_channel; i++)
            {
                mTrEqStruct.p_FIR_coef[i] = 0;
            }
        }
    }


    if(mTrEqStruct.config_received)
    {
        teq_common_utils::common_setConfig(&mTrEqStruct, &mTrEqConfig, &smooth_config);
    }
    else
    {
        teq_common_utils::common_reset(&mTrEqStruct);
        transducer_equalizer_set_filters_pointer(&mTrEqStruct);
    }

    caps->proc_type = effect_process_type_cap;
    return true;
}


void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize, t_uint8 idx)
{
    if(!mTrEqStruct.alloc_done)
    {
        return;
    }

    if(mTrEqStruct.chan_nb == -1)
    {
        // no data has been processed : number of channels may change
        mTrEqStruct.new_chan_nb = chans_nb;
    }
    else
    {
        // data has been processed : number of channels can't change
        ARMNMF_DBC_ASSERT(mTrEqStruct.chan_nb == chans_nb);
    }
    mTrEqStruct.sample_freq = (t_saa_sample_freq) freq;
}


void METH(reset)(t_effect_reset_reason reason)
{
    teq_common_utils::common_reset(&mTrEqStruct);
}


static inline void alloc_buf_sample(int **p_p_buf_sample32, int *p_buf_sample32_size, int size)
{
    int *p_buf_sample32 = *p_p_buf_sample32;


    if((p_buf_sample32 != NULL) && (*p_buf_sample32_size < size))
    {
        free(p_buf_sample32);
        p_buf_sample32 = NULL;
    }
    if(p_buf_sample32 == NULL)
    {
        *p_buf_sample32_size = size;
        p_buf_sample32       = (int *) malloc(size * sizeof(int));
        ARMNMF_DBC_ASSERT(p_buf_sample32 != NULL);
    }
    *p_p_buf_sample32 = p_buf_sample32;
}


void transducer_equalizer_nmfil_host_effect::transition1_processBuffer(void **p_p_inbuf, void **p_p_outbuf, int nb_samples, int offset_shift)
{
    void    *p_inbuf  = *p_p_inbuf;
    void    *p_outbuf = *p_p_outbuf;
    MMshort *p1, *p2, *p3;
    int     sample_size;


    sample_size = (mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples);
    if(host_effect_config.infmt.nof_bits_per_sample == 16)
    {
        alloc_buf_sample(&p_buf1_sample32, &buf1_sample32_size, sample_size);
        copy_in16b_out32b_noswap_optim(p_inbuf, (void *) p_buf1_sample32, (unsigned int) sample_size);
        p1 = p_buf1_sample32;       // 32 bits input
        p2 = p_buf1_sample32;       // 32 bits output
    }
    else
    {
        p1 = (MMshort *) p_inbuf;   // 32 bits input
        p2 = (MMshort *) p_outbuf;  // 32 bits output
    }
    if(p1 == p2)
    {
        alloc_buf_sample(&p_buf2_sample32, &buf2_sample32_size, sample_size);
        copy_in32b_out32b_noswap_optim((void *) p1, (void *) p_buf2_sample32, (unsigned int) sample_size);
        p3 = p_buf2_sample32;       // 32 bits input
    }
    else
    {
        p3 = p1;                    // 32 bits input
    }

    // p1 = input
    // p2 = output
    // p3 = input if input!=output or copy of input if input==output

    transducer_equalizer_filter(sample_size, p1, p2, &mTrEqStruct);

    if(mTrEqStruct.stereo_config)
    {
        smooth_transition1_stereo(p3,
                                  p2,
                                  nb_samples,
                                  smooth_config.smooth_transition_duration1 - smooth_config.smooth_transition_counter1,
                                  smooth_config.smooth_transition_counter1,
                                  smooth_config.shift_div_transition1,
                                  smooth_config.coef_div_transition1,
                                  smooth_config.transition_gain_mant_l,
                                  smooth_config.transition_gain_exp_l,
                                  smooth_config.transition_gain_mant_r,
                                  smooth_config.transition_gain_exp_r);
    }
    else
    {
        smooth_transition1_mono  (p3,
                                  p2,
                                  nb_samples,
                                  smooth_config.smooth_transition_duration1 - smooth_config.smooth_transition_counter1,
                                  smooth_config.smooth_transition_counter1,
                                  smooth_config.shift_div_transition1,
                                  smooth_config.coef_div_transition1,
                                  smooth_config.transition_gain_mant_l,
                                  smooth_config.transition_gain_exp_l);
    }

    if(host_effect_config.infmt.nof_bits_per_sample == 16)
    {
        copy_in32b_out16b_noswap_optim((void *) p2, p_outbuf, (unsigned int) sample_size);
    }

    smooth_config.smooth_transition_counter1 -= nb_samples;
    *p_p_inbuf                                = (int *) (((char *) p_inbuf)  + (nb_samples << offset_shift));
    *p_p_outbuf                               = (int *) (((char *) p_outbuf) + (nb_samples << offset_shift));
}


void transducer_equalizer_nmfil_host_effect::transition2_processBuffer(void **p_p_inbuf, void **p_p_outbuf, int nb_samples, int offset_shift)
{
    void    *p_inbuf  = *p_p_inbuf;
    void    *p_outbuf = *p_p_outbuf;
    MMshort *p1, *p2, *p3;
    int     sample_size;


    sample_size = (mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples);
    if(host_effect_config.infmt.nof_bits_per_sample == 16)
    {
        alloc_buf_sample(&p_buf1_sample32, &buf1_sample32_size, sample_size);
        copy_in16b_out32b_noswap_optim(p_inbuf, (void *) p_buf1_sample32, (unsigned int) sample_size);
        p1 = p_buf1_sample32;       // 32 bits input
        p2 = p_buf1_sample32;       // 32 bits output
    }
    else
    {
        p1 = (MMshort *) p_inbuf;   // 32 bits input
        p2 = (MMshort *) p_outbuf;  // 32 bits output
    }
    if(p1 == p2)
    {
        alloc_buf_sample(&p_buf2_sample32, &buf2_sample32_size, sample_size);
        copy_in32b_out32b_noswap_optim((void *) p1, (void *) p_buf2_sample32, (unsigned int) sample_size);
        p3 = p_buf2_sample32;   // 32 bits input
    }
    else
    {
        p3 = p1;                // 32 bits input
    }

    // p1 = input
    // p2 = output
    // p3 = input if input!=output or copy of input if input==output

    transducer_equalizer_filter(sample_size, p1, p2, &mTrEqStruct);

    if(mTrEqStruct.stereo_config)
    {
        smooth_transition2_stereo(p3,
                                  p2,
                                  nb_samples,
                                  smooth_config.smooth_transition_counter2,
                                  smooth_config.smooth_transition_duration2 - smooth_config.smooth_transition_counter2,
                                  smooth_config.shift_div_transition2,
                                  smooth_config.coef_div_transition2,
                                  smooth_config.transition_gain_mant_l,
                                  smooth_config.transition_gain_exp_l,
                                  smooth_config.transition_gain_mant_r,
                                  smooth_config.transition_gain_exp_r);
    }
    else
    {
        smooth_transition2_mono  (p3,
                                  p2,
                                  nb_samples,
                                  smooth_config.smooth_transition_counter2,
                                  smooth_config.smooth_transition_duration2 - smooth_config.smooth_transition_counter2,
                                  smooth_config.shift_div_transition2,
                                  smooth_config.coef_div_transition2,
                                  smooth_config.transition_gain_mant_l,
                                  smooth_config.transition_gain_exp_l);
    }

    if(host_effect_config.infmt.nof_bits_per_sample == 16)
    {
        copy_in32b_out16b_noswap_optim((void *) p2, p_outbuf, (unsigned int) sample_size);
    }

    smooth_config.smooth_transition_counter2 -= nb_samples;
    *p_p_inbuf                                = (int *) (((char *) p_inbuf)  + (nb_samples << offset_shift));
    *p_p_outbuf                               = (int *) (((char *) p_outbuf) + (nb_samples << offset_shift));
}


static inline void transducer_equalizer_process(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct,
                                                int                                 *p_inbuf,
                                                int                                 *p_outbuf,
                                                int                                 sample_size,
                                                int                                 nof_bits_per_sample,
                                                int                                 **p_p_buf_sample32,
                                                int                                 *p_buf_sample32_size)
{
    switch(nof_bits_per_sample)
    {
        case 16:
            alloc_buf_sample(p_p_buf_sample32, p_buf_sample32_size, sample_size);
            copy_in16b_out32b_noswap_optim((void *) p_inbuf, (void *) *p_p_buf_sample32,  (unsigned int) sample_size);
            transducer_equalizer_filter(sample_size, *p_p_buf_sample32, *p_p_buf_sample32, pTrEqStruct);
            copy_in32b_out16b_noswap_optim((void *) *p_p_buf_sample32, (void *) p_outbuf, (unsigned int) sample_size);
            break;

        case 32:
            transducer_equalizer_filter(sample_size, p_inbuf, p_outbuf, pTrEqStruct);
            break;

        default:
            ARMNMF_DBC_ASSERT(FALSE);
            break;
    }
}


void METH(process)(t_effect_process_params *params)
{
    t_effect_process_inplace_params     *ip_params  = (t_effect_process_inplace_params     *) params;
    t_effect_process_not_inplace_params *nip_params = (t_effect_process_not_inplace_params *) params;
    OMX_BUFFERHEADERTYPE                *pInputBuf, *pOutputBuf;
    int                                 *p_inbuf, *p_outbuf, byte_size, sample_size, offset_shift;
    int                                 nb_samples, nb_samples_tmp, nb_channels_FIR, i;
    MMshort                             *p1, *p2, *p3;
    MMshort                             coef_smooth_FIR;


    ARMNMF_DBC_PRECONDITION(mTrEqStruct.alloc_done == TRUE);

    if(params->proc_type == EFFECT_PROCESS_TYPE_INPLACE)
    {
        pInputBuf = pOutputBuf = ip_params->buf_hdr;
    }
    else
    {
        pInputBuf  = nip_params->inbuf_hdr;
        pOutputBuf = nip_params->outbuf_hdr;
    }
    p_inbuf   = (int *) pInputBuf ->pBuffer;
    p_outbuf  = (int *) pOutputBuf->pBuffer;
    byte_size =         pInputBuf ->nFilledLen;

    ARMNMF_DBC_ASSERT(pInputBuf->nOffset == 0);
    ARMNMF_DBC_ASSERT(p_inbuf            != NULL);
    ARMNMF_DBC_ASSERT(p_outbuf           != NULL);

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
            ARMNMF_DBC_ASSERT(mTrEqStruct.new_chan_nb == mTrEqStruct.alloc_chan_nb);
        }
    }

    if(mTrEqStruct.enable)
    {
        sample_size = byte_size;
        switch(host_effect_config.infmt.nof_bits_per_sample)
        {
            case 16:
                sample_size >>= 1;
                break;

            case 32:
                sample_size >>= 2;
                break;

            default:
                ARMNMF_DBC_ASSERT(FALSE);
                break;
        }

        if(!mTrEqStruct.smooth_config_change)
        {
            // no configuration smoothing
            transducer_equalizer_process(&mTrEqStruct,
                                         p_inbuf,
                                         p_outbuf,
                                         sample_size,
                                         host_effect_config.infmt.nof_bits_per_sample,
                                         &p_buf1_sample32,
                                         &buf1_sample32_size);
        }
        else
        {
            offset_shift = (host_effect_config.infmt.nof_bits_per_sample == 16 ? 1 : 2);
            nb_samples   = sample_size;
            if(mTrEqStruct.stereo_config)
            {
                nb_samples >>= 1;
                offset_shift++;
            }

            if(smooth_config.smooth_transition_counter1 > 0)
            {
                if(smooth_config.smooth_transition_counter1 > nb_samples)
                {
                    // first transition
                    transition1_processBuffer((void **) &p_inbuf, (void **) &p_outbuf, nb_samples, offset_shift);
                }
                else
                {
                    // end of first transition
                    nb_samples -= smooth_config.smooth_transition_counter1;
                    transition1_processBuffer((void **) &p_inbuf, (void **) &p_outbuf, smooth_config.smooth_transition_counter1, offset_shift);

                    teq_common_utils::common_copy_new_config(&mTrEqStruct, &smooth_config);
                    teq_common_utils::common_reset(&mTrEqStruct);
                    transducer_equalizer_set_filters_pointer(&mTrEqStruct);

                    if(smooth_config.smooth_transition_counter2 == 0)
                    {
                        // no second transition : means that transducer_equalizer is now disabled
                        if(p_inbuf != p_outbuf)
                        {
                            // we can used copy_in16b_out16b_noswap_optim() routine
                            // with (nb_samples<<offset_shift)/2 number of 16 bits samples whatever real sample size
                            copy_in16b_out16b_noswap_optim((void *) p_inbuf, (void *) p_outbuf, (unsigned int) (nb_samples << (offset_shift - 1)));
                        }
                        mTrEqStruct.smooth_config_change = FALSE;
                    }
                    else
                    {
                        if(smooth_config.smooth_transition_counter2 > nb_samples)
                        {
                            // second transition
                            transition2_processBuffer((void **) &p_inbuf, (void **) &p_outbuf, nb_samples, offset_shift);
                        }
                        else
                        {
                            // end of second transition
                            nb_samples -= smooth_config.smooth_transition_counter2;
                            transition2_processBuffer((void **) &p_inbuf, (void **) &p_outbuf, smooth_config.smooth_transition_counter2, offset_shift);

                            if(nb_samples > 0)
                            {
                                sample_size = (mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples);
                                transducer_equalizer_process(&mTrEqStruct,
                                                             p_inbuf,
                                                             p_outbuf,
                                                             sample_size,
                                                             host_effect_config.infmt.nof_bits_per_sample,
                                                             &p_buf1_sample32,
                                                             &buf1_sample32_size);
                            }
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
                    transition2_processBuffer((void **) &p_inbuf, (void **) &p_outbuf, nb_samples, offset_shift);
                }
                else
                {
                    // end of second transition
                    nb_samples -= smooth_config.smooth_transition_counter2;
                    transition2_processBuffer((void **) &p_inbuf, (void **) &p_outbuf, smooth_config.smooth_transition_counter2, offset_shift);

                    if(nb_samples > 0)
                    {
                        sample_size = (mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples);
                        transducer_equalizer_process(&mTrEqStruct,
                                                     p_inbuf,
                                                     p_outbuf,
                                                     sample_size,
                                                     host_effect_config.infmt.nof_bits_per_sample,
                                                     &p_buf1_sample32,
                                                     &buf1_sample32_size);
                    }
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
                        coef_smooth_FIR = teq_common_utils::fract_ratio(smooth_config.smooth_FIR_transition_counter, smooth_config.smooth_FIR_transition_duration);
                        p1              = mTrEqStruct.p_FIR_coef;
                        p2              = smooth_config.new_FIR_coef;
                        p3              = smooth_config.old_FIR_coef;
                        for(i = nb_channels_FIR * mTrEqStruct.nb_FIR_coefs_per_channel; i > 0; i--)
                        {
                            *p1 = *p2 + (MMshort) ((((MMlong) coef_smooth_FIR) * ((MMlong) (*p3 - *p2))) >> Q_ARITHMETIC);
                            p1++;
                            p2++;
                            p3++;
                        }

                        nb_samples_tmp = (smooth_config.smooth_FIR_transition_counter > smooth_config.FIR_smoothing_delay_granularity ? smooth_config.FIR_smoothing_delay_granularity : smooth_config.smooth_FIR_transition_counter);
                        nb_samples_tmp = (nb_samples_tmp > nb_samples ? nb_samples : nb_samples_tmp);
                        smooth_config.smooth_FIR_transition_counter -= nb_samples_tmp;
                        nb_samples                                  -= nb_samples_tmp;

                        sample_size = (mTrEqStruct.stereo_config ? nb_samples_tmp * 2 : nb_samples_tmp);
                        transducer_equalizer_process(&mTrEqStruct,
                                                     p_inbuf,
                                                     p_outbuf,
                                                     sample_size,
                                                     host_effect_config.infmt.nof_bits_per_sample,
                                                     &p_buf1_sample32,
                                                     &buf1_sample32_size);
                        p_inbuf  = (int *) (((char *) p_inbuf)  + (nb_samples_tmp << offset_shift));
                        p_outbuf = (int *) (((char *) p_outbuf) + (nb_samples_tmp << offset_shift));
                    }

                    if(smooth_config.smooth_FIR_transition_counter == 0)
                    {
                        // end of smooth FIR transition
                        p1 = mTrEqStruct.p_FIR_coef;
                        p2 = smooth_config.save_new_FIR_coef;
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
                            sample_size = (mTrEqStruct.stereo_config ? nb_samples * 2 : nb_samples);
                            transducer_equalizer_process(&mTrEqStruct,
                                                         p_inbuf,
                                                         p_outbuf,
                                                         sample_size,
                                                         host_effect_config.infmt.nof_bits_per_sample,
                                                         &p_buf1_sample32,
                                                         &buf1_sample32_size);
                        }

                        mTrEqStruct.smooth_config_change = FALSE;
                    }
                }
                else
                {
                    transducer_equalizer_process(&mTrEqStruct,
                                                 p_inbuf,
                                                 p_outbuf,
                                                 sample_size,
                                                 host_effect_config.infmt.nof_bits_per_sample,
                                                 &p_buf1_sample32,
                                                 &buf1_sample32_size);

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

    if(params->proc_type == EFFECT_PROCESS_TYPE_NOT_INPLACE)
    {
        if(!mTrEqStruct.enable)
        {
            // byte_size unit is byte, so we can used copy_in16b_out16b_noswap_optim() routine
            // with byte_size/2 number of 16 bits samples whatever real sample size
            copy_in16b_out16b_noswap_optim((void *) p_inbuf, (void *) p_outbuf, (unsigned int) (byte_size >> 1));
        }
        pOutputBuf->nFilledLen    = pInputBuf->nFilledLen;
        pOutputBuf->nOffset       = pInputBuf->nOffset;
        pOutputBuf->nTimeStamp    = pInputBuf->nTimeStamp;
        pOutputBuf->nFlags        = pInputBuf->nFlags;
        nip_params->inbuf_emptied = true;
        nip_params->outbuf_filled = true;
    }
}

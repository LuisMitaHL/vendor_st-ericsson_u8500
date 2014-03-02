/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
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

#include <volctrl/nmfil/host/effect.nmf>

#include <string.h>
#include <armnmf_dbc.h>
#include "OMX_Core.h"

//#define VOLCTRL_TRACE_CONTROL
#ifdef VOLCTRL_TRACE_CONTROL
#include <stdio.h>
#define PRINT   printf("[VOLCTRL] "); printf
#else
#define PRINT(x,...)
#endif


t_sint16 METH(saturate)(int value)
{
    t_sint16 var_out;

    if (value > 0x00007fffL)
    {
        var_out = 0x7fff;
    }
    else if (value < (int) 0xffff8000L)
    {
        var_out = 0x8000;
    }
    else
    {
        var_out = (t_sint16)value;
    }

    return (var_out);
}



void METH(copyChannel)(int * inbuf, int * outbuf, int size, int stride)
{
    for (int i = 0; i < size; i+= stride) {
        outbuf[i] = inbuf[i];
    }
}



void METH(channel_mapping_config)(unsigned short nchannels, int *mapping, const t_channel_type channel_mapping[])
{
    int i;


    for(i=0; i < nchannels; i++)
    {
        switch(channel_mapping[i])
        {
            case CHANNEL_LF:
                mapping[CHANNEL_LF]= i;
                break;

            case CHANNEL_RF:
                mapping[CHANNEL_RF]= i;
                break;
 
            case CHANNEL_CF:
                mapping[CHANNEL_CF]= i;
                break;

            case CHANNEL_LFE:
                mapping[CHANNEL_LFE]= i;
                break;

            case CHANNEL_LS:
                mapping[CHANNEL_LS]= i;
                break;

            case CHANNEL_RS:
                mapping[CHANNEL_RS]= i;
                break;

            case CHANNEL_LR:
                mapping[CHANNEL_LR]= i;
                break;

            case CHANNEL_RR:
                mapping[CHANNEL_RR]= i;
                break;

            default:
                break;
        }
    }

}



METH(volctrl_nmfil_host_effect)()
{
    int i,j;
    
    mOpen   = 0;
    mConfig = 0;
    mMyProcess = 0;
    mInBuf  = NULL;
    mOutBuf = NULL;

    memset(&m_ramp, 0, sizeof(VolctrlRampConfig_t));
    memset(&m_config, 0, sizeof(t_host_effect_config));
    memset(&mVolData, 0, sizeof(VOLCTRL_LOCAL_STRUCT_T));
    memset(&mDownMix, 0, sizeof(UPDOWNMIX_LOCAL_STRUCT_T));


    // Prepare the algorithm with default values ...
    mVolData.db_ramp   = 1;   // Value set to dB

    for(i=0;i<VOLCTRL_MAX_CMD;i++){
        for(j=0;j<MAX_CH;j++){
            mVolData.gain_cmd[i][j]  = (int) (0);
        }
    }

    mInitialized = false;
    
}



METH(~volctrl_nmfil_host_effect)() {}



t_bool METH(open)(const t_host_effect_config* config, t_effect_caps* caps)
{
    PRINT("volctrl_nmfil_host_effect::open()\n");

    ARMNMF_DBC_PRECONDITION(config->infmt.freq == config->outfmt.freq);
    ARMNMF_DBC_PRECONDITION(config->infmt.nof_channels <= MAX_MCH);
    ARMNMF_DBC_PRECONDITION(config->infmt.nof_channels >= config->outfmt.nof_channels); // volctrl is capable of downmix
    ARMNMF_DBC_PRECONDITION((config->infmt.nof_bits_per_sample == 16) || (config->infmt.nof_bits_per_sample == 32));
    ARMNMF_DBC_PRECONDITION(config->infmt.nof_bits_per_sample == config->outfmt.nof_bits_per_sample);

    if (config->infmt.nof_bits_per_sample == 16)
    {
        mInBuf  = new int[config->block_size * config->infmt.nof_channels];
        mOutBuf = new int[config->block_size * config->infmt.nof_channels];

        if(mInBuf == 0 || mOutBuf == 0) return false;
    }
    else
    {
        mInBuf  = NULL;
        mOutBuf = NULL;
    }
	
    volctrl_init(&mVolData, config->infmt.freq);

    mVolData.input_mode = config->infmt.nof_channels;

    // Set default configuration for downmix if necessary (>= CHECK!!!)
    if(mVolData.downmix && mVolData.multichannel)
    {
        int *mapping_in;
        int *mapping_out;
        int channel_mapping_in[MAXCHANNELS_SUPPORT]  ={0};
        int channel_mapping_out[MAXCHANNELS_SUPPORT] ={0};

        mapping_in = &channel_mapping_in[0];
        channel_mapping_config(config->infmt.nof_channels, mapping_in, config->infmt.channel_mapping);

        mapping_out = &channel_mapping_out[0];
        channel_mapping_config(config->outfmt.nof_channels, mapping_out, config->outfmt.channel_mapping);

        init_updownmix(config->infmt.nof_channels, config->outfmt.nof_channels, &mDownMix, mapping_in, mapping_out);
    }

    // Set ramping according to the channel mapping if settings are from Load to Idle State
    if(mConfig)
    {
        int channel_position =0;

        if(m_ramp.iTerminate==0)
        {
            mVolData.new_config = VOLCTRL_IMMEDIAT_CMD_PENDING+1;

            for (int j = 0; j < config->infmt.nof_channels; j++) {
                if(config->infmt.channel_mapping[j] == (int)m_ramp.iChannel) {
                    channel_position = j;
                    break;
                }
            }

            mVolData.gain_cmd[1][START_IDX_MCH+channel_position] = m_ramp.iStartVolume - VOLCTRL_OFFSET_GAIN;
            mVolData.gain_cmd[0][START_IDX_MCH+channel_position] = m_ramp.iEndVolume - VOLCTRL_OFFSET_GAIN;
            mVolData.duration[START_IDX_MCH+channel_position] = m_ramp.ialpha;
            mVolData.configX[channel_position] = 1;

            for (int i = 0; i < MAX_MCH; i++)
            {
                if(i != channel_position) {
                    mVolData.gain_cmd[1][START_IDX_MCH+i] = mVolData.gain_cmd[0][START_IDX_MCH+i];
                    mVolData.gain_cmd[0][START_IDX_MCH+i] = mVolData.current_gaindB[START_IDX_MCH+i];
                }
            }
        }

        mVolData.terminate[START_IDX_MCH+channel_position] = m_ramp.iTerminate;
    }


    mOpen =1;
    m_config = *config;
    caps->proc_type = EFFECT_PROCESS_TYPE_INPLACE;

    return true;
}



void METH(reset)(t_effect_reset_reason reason)
{
    PRINT("volctrl_nmfil_host_effect::reset()\treason=%d\n", (int)reason);

    if(mMyProcess && reason == EFFECT_RESET_REASON_STOP)
    {
        mOpen    = 0;
        mConfig  = 0;
        mMyProcess = 0;
    }
}



void METH(process)(t_effect_process_params* params)
{
    int *inptr;
    int *outptr;
    unsigned int i;
    unsigned int ch;
    unsigned int sampleCount;

    mMyProcess = 1;
    t_effect_process_inplace_params* inplace_params = (t_effect_process_inplace_params*)params;
    OMX_BUFFERHEADERTYPE *pOmxBufHdr = (OMX_BUFFERHEADERTYPE*)inplace_params->buf_hdr;

    sampleCount = pOmxBufHdr->nFilledLen / (m_config.infmt.nof_bits_per_sample / 8);

  
    ARMNMF_DBC_PRECONDITION(inplace_params->base.proc_type == EFFECT_PROCESS_TYPE_INPLACE);
    ARMNMF_DBC_PRECONDITION(pOmxBufHdr->nOffset == 0);
  

    while (mVolData.new_config){
        volctrl_set_new_config(&mVolData);
    }


    if (m_config.infmt.nof_bits_per_sample == 16)
    {
        t_sint16* sampleBuf;
    
        for (ch = 0; ch < m_config.infmt.nof_channels; ch++)
        {
            sampleBuf = ((t_sint16*)pOmxBufHdr->pBuffer) + ch;

            for (i = 0; i < sampleCount; i += m_config.infmt.nof_channels) {
                mInBuf[ch+i]=(int)sampleBuf[i];
            }
        }

        inptr = mInBuf;

    } 
    else
    {
        inptr = (int*) pOmxBufHdr->pBuffer;
    }
 
 
    if (m_config.outfmt.nof_bits_per_sample == 16) {
        outptr = mOutBuf;
    } 
    else 
    {
        outptr = (int*) pOmxBufHdr->pBuffer;
    }


    if (mVolData.multichannel)
    {
        int gain_idx;
        int *multichannel_in  = inptr;
        int *multichannel_out = outptr;

        for (int i=0; i<mVolData.input_mode; i++)
        {
            gain_idx=i+START_IDX_MCH;

            if (mVolData.gain_mask & (1<<gain_idx)) {
                process_one_of_nch_no_cross_gain(multichannel_in, multichannel_out, i, sampleCount/mVolData.input_mode, &mVolData);
            }
        }
    }
    else
    {
        int *left_in   = inptr;
        int *right_in  = inptr + 1;
        int *left_out  = outptr;
        int *right_out = outptr + 1;


        if((m_config.infmt.nof_channels == 1) && (m_config.outfmt.nof_channels == 1))
        {
            process_one_channel_no_cross_gain(left_in, left_out, GLL, sampleCount, 1, &mVolData);
        } 
        else if(m_config.infmt.nof_channels == 2)
        {
            if(m_config.outfmt.nof_channels == 1)
            {
                // downmix
                process_one_channel_with_cross_gain(left_in, right_in, outptr, 1, sampleCount, GLL, GRL, &mVolData);

                // special downmix case: filled length is modified
                sampleCount >>= 1;
                pOmxBufHdr->nFilledLen >>= 1;

            }
            else if((mVolData.gain_mask & GRL_MASK) && (mVolData.gain_mask & GLR_MASK))
            {
                // both cross gains not zero
                process_two_channel_with_cross_gain(left_in, left_out, sampleCount, &mVolData);

            } 
            else if(mVolData.gain_mask & GRL_MASK)
            {
                // left channel from L & R
                process_one_channel_with_cross_gain(left_in, right_in, left_out, 2, sampleCount, GLL, GRL, &mVolData);
      
                if(mVolData.gain_mask & GRR_MASK)
                {
                    // right channel from R
                    process_one_channel_no_cross_gain(right_in, right_out, GRR, sampleCount, 2, &mVolData);
                } 
                else
                {
                    copyChannel(right_in, right_out, sampleCount, 2);
                }

            } 
            else if(mVolData.gain_mask & GLR_MASK)
            {
                if(mVolData.gain_mask & GRR_MASK)
                {
                    // right channel from L & R
                    process_one_channel_with_cross_gain(right_in, left_in, right_out, 2, sampleCount, GRR, GLR, &mVolData);
                } 
                else
                {
                    // right channel from L
                    process_one_channel_no_cross_gain(left_in, right_out, GLR, sampleCount, 2, &mVolData);
                }

                // left channel from L
                process_one_channel_no_cross_gain(left_in, left_out, GLL, sampleCount, 2, &mVolData);
            }
            else
            {
                // no cross gain

                // left channel from L
                process_one_channel_no_cross_gain(left_in, left_out, GLL, sampleCount, 2, &mVolData);
      
                // right channel from R
                process_one_channel_no_cross_gain(right_in, right_out, GRR, sampleCount, 2, &mVolData);
            }
        }
    }

    //Apply downmix for multichannel streams
    if(mVolData.multichannel && mVolData.downmix)
    {
        // special downmix case: filled length is modified
        sampleCount = m_config.block_size * m_config.outfmt.nof_channels;
        pOmxBufHdr->nFilledLen = (pOmxBufHdr->nFilledLen / m_config.infmt.nof_channels) * m_config.outfmt.nof_channels;

        process_updownmix(outptr, outptr, m_config.block_size, &mDownMix);
    }


    if(m_config.outfmt.nof_bits_per_sample == 16)
    {
        t_sint16* sampleBuf;

        for(ch = 0; ch < m_config.outfmt.nof_channels; ch++)
        {
            sampleBuf = ((t_sint16*)pOmxBufHdr->pBuffer) + ch;

            for(i = 0; i < sampleCount; i += m_config.outfmt.nof_channels) {
	            sampleBuf[i]=saturate(mOutBuf[ch+i]);
            }
        }
    }
}



void METH(close)(void)
{
    PRINT("volctrl_nmfil_host_effect::close()\n");

    if(m_config.infmt.nof_bits_per_sample == 16)
    {
        if (mOutBuf != 0) delete [] mOutBuf;
        if (mInBuf  != 0) delete [] mInBuf;
    }
}



void METH(setConfig)(VolctrlConfig_t config)
{
    PRINT("volctrl_nmfil_host_effect::setConfig()\n");

    int config_index;

    mVolData.multichannel = config.iMultichannel;
    mVolData.downmix = config.iDownMix;
    mVolData.db_ramp = config.iDBRamp;

    // save command gains
    if(config.ialpha == 0)
    {
        config_index = 1;
        mVolData.new_config = VOLCTRL_IMMEDIAT_CMD_PENDING;
    }
    else
    {
        config_index = 0;
        mVolData.new_config |= 1;  // do not reset VOLCTRL_IMMEDIAT_CMD_PENDING bit
    }

    if(!mVolData.multichannel)
    {
        // reset gain
        for (int i = 0; i < 4; i++) {
            mVolData.gain[i] = 0;
            mVolData.duration[i] = config.ialpha;
        }

        mVolData.gain_cmd[config_index][GLL] = config.igll - VOLCTRL_OFFSET_GAIN;
        mVolData.gain_cmd[config_index][GLR] = config.iglr - VOLCTRL_OFFSET_GAIN;
        mVolData.gain_cmd[config_index][GRL] = config.igrl - VOLCTRL_OFFSET_GAIN;
        mVolData.gain_cmd[config_index][GRR] = config.igrr - VOLCTRL_OFFSET_GAIN;
        mVolData.configL = 1;
        mVolData.configR = 1;
  
        mVolData.terminate[0]= 0;
        mVolData.terminate[1]= 0;
        mVolData.terminate[2]= 0;
        mVolData.terminate[3]= 0;
    }
    else
    {
        // reset gain & save command gains
        for(int i = 0; i < MAX_MCH; i++)
        {
            mVolData.configX[i] =1;
            mVolData.gain[START_IDX_MCH+i] = 0;
            mVolData.terminate[START_IDX_MCH+i] = 0;
            mVolData.duration[START_IDX_MCH+i]  = config.ialpha;
            mVolData.gain_cmd[config_index][START_IDX_MCH+i]  = config.igains[i] - VOLCTRL_OFFSET_GAIN;
        }
    }
}



void METH(setRampConfig)(VolctrlRampConfig_t config)
{
    mVolData.new_config = VOLCTRL_IMMEDIAT_CMD_PENDING+1;
    mVolData.multichannel = config.iMultichannel;

    if(!mVolData.multichannel)
    {
        // Case: Stereo, mono and downmix
        if(config.iChannel==1 || config.iChannel==3)
        {
            if(config.iTerminate==0)
            {
                mVolData.gain_cmd[1][GLL] = config.iStartVolume - VOLCTRL_OFFSET_GAIN;
                mVolData.gain_cmd[0][GLL] = config.iEndVolume - VOLCTRL_OFFSET_GAIN;

                mVolData.gain_cmd[1][GRR] = mVolData.gain_cmd[0][GRR];
                mVolData.gain_cmd[0][GRR] = mVolData.current_gaindB[GRR];

                mVolData.duration[GLL]    = config.ialpha;
                mVolData.configL = 1;
            }

            mVolData.terminate[GLL] = config.iTerminate;
        }

        //Case: Stereo
        if(config.iChannel==2)
        {
            if(config.iTerminate==0)
            {
                mVolData.gain_cmd[1][GRR] = config.iStartVolume - VOLCTRL_OFFSET_GAIN;
                mVolData.gain_cmd[0][GRR] = config.iEndVolume - VOLCTRL_OFFSET_GAIN;

                mVolData.gain_cmd[1][GLL] = mVolData.gain_cmd[0][GLL];
                mVolData.gain_cmd[0][GLL] = mVolData.current_gaindB[GLL];

                mVolData.duration[GRR]    = config.ialpha;
                mVolData.configR = 1;
            }

            mVolData.terminate[GRR] = config.iTerminate;
        }

        // Case: Ramp to all channels (stereo, mono, downmix)
        if(config.iChannel == (t_uint32)OMX_ALL)
        {
            if(config.iTerminate==0)
            {
                mVolData.gain_cmd[1][GLL] = config.iStartVolume - VOLCTRL_OFFSET_GAIN;
                mVolData.gain_cmd[0][GLL] = config.iEndVolume - VOLCTRL_OFFSET_GAIN;
                mVolData.gain_cmd[1][GRR] = config.iStartVolume - VOLCTRL_OFFSET_GAIN;
                mVolData.gain_cmd[0][GRR] = config.iEndVolume - VOLCTRL_OFFSET_GAIN;
                mVolData.duration[GRR]    = config.ialpha;
                mVolData.duration[GLL]    = config.ialpha;
                mVolData.configL = 1;
                mVolData.configR = 1;
            }

            mVolData.terminate[GLL] = config.iTerminate;
            mVolData.terminate[GRR] = config.iTerminate;
        }
    }
    else
    {
        if(config.iChannel == (t_uint32)OMX_ALL)
        {
            if(config.iTerminate==0)
            {
                for (int i = 0; i < MAX_MCH; i++)
                {
                    mVolData.gain_cmd[1][START_IDX_MCH+i] = config.iStartVolume - VOLCTRL_OFFSET_GAIN;
                    mVolData.gain_cmd[0][START_IDX_MCH+i] = config.iEndVolume - VOLCTRL_OFFSET_GAIN;
                    mVolData.duration[START_IDX_MCH+i]    = config.ialpha;
                    mVolData.configX[i] = 1;
                }
            }

            for (int j = 0; j < MAX_MCH; j++) {
                mVolData.terminate[START_IDX_MCH+j] = config.iTerminate;            
            }
        }
        else
        {
            if(mOpen)
            {
                int channel_position = 0;

                if(config.iTerminate==0)
                {
                    for (int j = 0; j < m_config.infmt.nof_channels; j++) {
                        if(m_config.infmt.channel_mapping[j] == (int)config.iChannel) {
                            channel_position = j;
                            break;
                        }
                    }

                    mVolData.gain_cmd[1][START_IDX_MCH+channel_position] = config.iStartVolume - VOLCTRL_OFFSET_GAIN;
                    mVolData.gain_cmd[0][START_IDX_MCH+channel_position] = config.iEndVolume - VOLCTRL_OFFSET_GAIN;
                    mVolData.duration[START_IDX_MCH+channel_position] = config.ialpha;
                    mVolData.configX[channel_position] = 1;

                    for (int i = 0; i < MAX_MCH; i++) 
                    {
                        if(i != channel_position) {
                            mVolData.gain_cmd[1][START_IDX_MCH+i] = mVolData.gain_cmd[0][START_IDX_MCH+i];
                            mVolData.gain_cmd[0][START_IDX_MCH+i] = mVolData.current_gaindB[START_IDX_MCH+i];
                        }
                    }
                }

                mVolData.terminate[START_IDX_MCH+channel_position] = config.iTerminate;
            }
            else
            {
                mConfig = 1;
                m_ramp  = config;
            }
        }
    }
}


void METH(getCurrentRamp)(VolCtrlCurrentRamp_t *volume, t_uint32 channel)
{
    if(channel == (t_uint32)OMX_ALL)
    {
        volume->volume = (mVolData.current_gaindB[((mVolData.multichannel == 0 ) ? 0: START_IDX_MCH)] * 100) >> 8;
        volume->timestamp = (mVolData.current_duration[0] * OMX_TICKS_PER_SECOND) / 100000;
    }
    else
    {
        volume->volume = (mVolData.current_gaindB[channel] * 100) >> 8;
        volume->timestamp = (mVolData.current_duration[channel] * OMX_TICKS_PER_SECOND) / 100000;
    }
}

////////////////////////////////////////////////////////////
//          new Interface
////////////////////////////////////////////////////////////


void
METH(processBufferDownmix)(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    MMshort *left_in  = inbuf;
    MMshort *right_in = inbuf + 1;
    MMshort *left_out = outbuf;
    

    process_one_channel_with_cross_gain(
        left_in, right_in, left_out, 1, size, GLL, GRL, &mVolData);
}

void
METH(processBufferUpmix)(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    int i;
    MMshort *left_in  = inbuf;
    MMshort *left_out = outbuf;
    
    
    process_one_channel_no_cross_gain(
        left_in, left_in, GLL, size, 1, &mVolData);

    // Upmix mode, scratch buffer is input buffer
    for(i=0;i<size;i++){
        outbuf[i] = inbuf[i];
    }
    //Mono - stereo conversion
    process_updownmix(outbuf, outbuf, size, &mDownMix);
    
}

void
METH(processBufferMono)(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    MMshort *left_in  = inbuf;
    MMshort *left_out = outbuf;

    process_one_channel_no_cross_gain(
        left_in, left_out, GLL, size, 1, &mVolData);
}

void
METH(processBufferStereo)(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    MMshort *left_in  = inbuf;
    MMshort *right_in  = inbuf +1;
    MMshort *left_out = outbuf;
    MMshort *right_out= outbuf + 1;

    process_one_channel_no_cross_gain(
        left_in, left_out, GLL, size, 2, &mVolData);
    process_one_channel_no_cross_gain(
        right_in, right_out, GRR, size, 2, &mVolData);
}

void
METH(processMultichannel)(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    int gain_idx;
    int *multichannel_in  = inbuf;
    int i;
    
    //int *multichannel_out = outbuf;

    for (int i=0; i<mVolData.input_mode; i++)
    {
        gain_idx=i+START_IDX_MCH;

        if (mVolData.gain_mask & (1<<gain_idx)) {
            // By default the volctrl is configured inplace
            process_one_of_nch_no_cross_gain(multichannel_in, multichannel_in, i,size/mChannelsIn, &mVolData);
        }
    }
    //Apply downmix for multichannel streams
    // Updownmix is compiled as inplace
    if(mVolData.downmix || mRemap){
        //Library updownmix is using the number of sample
        // Downmix mode, scratch buffer is ouput buffer
        process_updownmix(outbuf, outbuf, size/mChannelsIn, &mDownMix);
    }
    else if(mVolData.upmix)
    {
        // Upmix version
        // Upmix mode, scratch buffer is input buffer
        for(i=0;i<size;i++){
            outbuf[i] = inbuf[i];
        }
        process_updownmix(outbuf, outbuf, size/mChannelsIn, &mDownMix);
    }
}

void
METH(processBufferDownmix_16bits)( t_sint16* inbuf,  t_sint16* outbuf, t_uint16 size) {
    t_sint16*left_in  = inbuf;
    t_sint16*right_in = inbuf + 1;
    t_sint16*left_out = outbuf;
    

    process_one_channel_with_cross_gain_sample16(
        left_in, right_in, left_out, 1, size, GLL, GRL, &mVolData);
}

void
METH(processBufferUpmix_16bits)( t_sint16* inbuf,  t_sint16* outbuf, t_uint16 size) {
    t_sint16*left_in  = inbuf;
    t_sint16*left_out = outbuf;
    int i;
	    
	    
    process_one_channel_no_cross_gain_sample16(
        left_in, inbuf, GLL, size, 1, &mVolData);
	
    // Upmix mode, scratch buffer is input buffer
    for(i=0;i<size;i++){
        outbuf[i] = inbuf[i];
    }
    //Mono - stereo conversion
    process_updownmix_sample16(outbuf, outbuf, size, &mDownMix);
	    
}

void
METH(processBufferMono_16bits)( t_sint16* inbuf,  t_sint16* outbuf, t_uint16 size) {
    t_sint16*left_in  = inbuf;
    t_sint16*left_out = outbuf;

    process_one_channel_no_cross_gain_sample16(
        left_in, left_out, GLL, size, 1, &mVolData);
}

void
METH(processBufferStereo_16bits)( t_sint16* inbuf,  t_sint16* outbuf, t_uint16 size) {
    t_sint16 *left_in   = inbuf;
    t_sint16 *right_in  = inbuf +1;
    t_sint16 *left_out  = outbuf;
    t_sint16 *right_out = outbuf + 1;

    process_one_channel_no_cross_gain_sample16(
        left_in, left_out, GLL, size, 2, &mVolData);
    process_one_channel_no_cross_gain_sample16(
        right_in, right_out, GRR, size, 2, &mVolData);
}

void
METH(processMultichannel_16bits)(t_sint16 * inbuf, t_sint16 * outbuf, t_uint16 size) {
    int gain_idx;
    t_sint16  *multichannel_in  = inbuf;
    int i;
    
    //int *multichannel_out = outbuf;

    for (int i=0; i<mVolData.input_mode; i++)
    {
        gain_idx=i+START_IDX_MCH;

        if (mVolData.gain_mask & (1<<gain_idx)) {
            // By default the volctrl is configured inplace
            process_one_of_nch_no_cross_gain_sample16(multichannel_in, multichannel_in, i,size/mChannelsIn, &mVolData);
        }
    }
    //Apply downmix for multichannel streams
    // Updownmix is compiled as inplace
    if(mVolData.downmix || mRemap){
        //Library updownmix is using the number of sample
        // Downmix mode, scratch buffer is ouput buffer
        process_updownmix_sample16(outbuf, outbuf, size/mChannelsIn, &mDownMix);
    }
    else if(mVolData.upmix)
    {
        // Upmix version
        // Upmix mode, scratch buffer is input buffer
        for(i=0;i<size;i++){
            outbuf[i] = inbuf[i];
        }
        process_updownmix_sample16(outbuf, outbuf, size/mChannelsIn, &mDownMix);
    }
}

void
METH(fillConfigStruct)(VolctrlConfig_t *config) {
    t_sint16 leftVolumemdB, rightVolumemdB;
    t_sint16 commonVolume;
    double log_value=0;
    
    int i;
    
    if (mMute) {
        leftVolumemdB   = VOLCTRL_VOLUME_MUTE;
        rightVolumemdB  = VOLCTRL_VOLUME_MUTE;
    } else {
        if(mBalance == -100)
        {
            leftVolumemdB  = mVolume;
            rightVolumemdB = VOLCTRL_VOLUME_MIN;
        }
        else if(mBalance == 100)
        {
            leftVolumemdB  = VOLCTRL_VOLUME_MIN;
            rightVolumemdB = mVolume;
        }
        else if(mBalance == 0)
        {
            leftVolumemdB  = mVolume;
            rightVolumemdB = mVolume;
        }
        else 
        {
            if(mBalance > 0){    
                log_value = log((double)((double)mBalance/(double)VOLCTRL_BALANCE_ALLRIGHT));
            }
            leftVolumemdB  = mBalance <= 0 ? (mVolume) : (mVolume+2000*log_value);
            if(mBalance < 0){    
                log_value = log((double)((double)mBalance/(double)VOLCTRL_BALANCE_ALLLEFT));
            }
            rightVolumemdB = mBalance >= 0 ? (mVolume) : (mVolume+2000*log_value);
        }   
    }
    
    config->iDownMix      = mVolData.downmix; // downmix set at init time so dont change it
    config->iMultichannel = mVolData.multichannel; // multichannel set at init time so dont change it
    config->iDBRamp  = 1;
    config->igll     = (128<<8); //offset
    config->igll     += (leftVolumemdB<<8)/100; //care to take into account values < 1dB
    config->iglr     = 0;
    config->igrl     = 0;
    config->igrr     = (128<<8); //offset
    config->igrr     += (rightVolumemdB<<8)/100; //care to take into account values < 1dB
    config->ialpha   = 0;
    // In multichannlel mode all channel ahve the same gain
    if (mMute) {
        commonVolume = VOLCTRL_VOLUME_MUTE;
    }
    else{
        commonVolume = mVolume;
    }
    for(i=0;i<VOLCTRL_MULTICHANNEL_GAINS;i++)
    {
        config->igains[i] =  (128<<8); //offset
        config->igains[i] += (commonVolume<<8)/100; //care to take into account values < 1dB
    }
}

void METH(fillRampConfigStruct)(VolctrlRampConfig_t *config) {
    config->iMultichannel = mVolData.multichannel; // multichannel set at init time so dont change it
    config->iStartVolume    = (128 << 8);
    config->iStartVolume    += (mVolume << 8) / 100;
    config->iEndVolume      = (128 << 8);
    config->iEndVolume      += (mRampEndVolume << 8) / 100;
    config->ialpha          = mRampDuration;
    config->iDBRamp         = 1;

    if (mRampChannels == OMX_ALL) {
        config->iChannel       = 3;
    }
    else if (mRampChannels == OMX_AUDIO_ChannelCF 
             || mRampChannels == OMX_AUDIO_ChannelLF) {
        config->iChannel = 1;
    }
    else if (mRampChannels == OMX_AUDIO_ChannelRF) {
        config->iChannel = 2;
    }
    else { ARMNMF_DBC_ASSERT(0);}

    config->iTerminate      = mRampTerminate;
}

void METH(init) (t_uint16 nChannelsIn, t_uint16 nChannelsOut, t_sample_freq freq,t_channel_type channel_type_in[MAXCHANNELS_SUPPORT], t_channel_type channel_type_out[MAXCHANNELS_SUPPORT]) {
    //ARMNMF_DBC_PRECONDITION(nChannelsIn >= nChannelsOut); // volctrl is only capable of downmix/ no upmix
    
    // This function only set the field 
    // mVolData.sample_freq =  volctrl_freq[freq];
    volctrl_init(&mVolData, freq);
    mVolData.multichannel = 0;
    if( (nChannelsIn > 2) || (nChannelsOut > 2)){
        mVolData.multichannel = 1;
    }
    mVolData.downmix = 0;
    if(nChannelsIn>nChannelsOut){
        mVolData.downmix = 1;
    }
    mVolData.upmix = 0;
    if(nChannelsOut > nChannelsIn){
        mVolData.upmix = 1;
    }
    mRemap = false;
    // check if we need to re-map the channels
    if (nChannelsIn == nChannelsOut) {
        for (int i = 0; i < nChannelsIn; i++) {
            if (channel_type_in[i] !=  channel_type_out[i]) {
                mRemap = true;
                break;
            }
        }
    }
    mInitialized = true;
    
    if (mRampDuration != 0) {
        VolctrlRampConfig_t config;
        fillRampConfigStruct(&config);
        setRampConfig(config);
    } else {
        VolctrlConfig_t config;
        fillConfigStruct(&config);
        setConfig(config);
    }
    
    
    if (nChannelsIn == 1 && nChannelsOut == 1) {
        mVolData.input_mode = VOLCTRL_INPUT_MONO;
        
        mProcess16 = &METH(processBufferMono_16bits);
        mProcess = &METH(processBufferMono);
        
    }
    else if (nChannelsIn == 1 && nChannelsOut == 2) {
        mVolData.input_mode = VOLCTRL_INPUT_MONO;
        
        mProcess = &METH(processBufferUpmix);
        mProcess16 = &METH(processBufferUpmix_16bits);

        int *mapping_in;
        int *mapping_out;
        int channel_mapping_in[MAXCHANNELS_SUPPORT]  ={0};
        int channel_mapping_out[MAXCHANNELS_SUPPORT] ={0};
        
        
        memset(channel_mapping_in,  0, sizeof(channel_mapping_in));
        memset(channel_mapping_out, 0, sizeof(channel_mapping_out));
        
        mapping_in = &channel_mapping_in[0];
        channel_mapping_config(nChannelsIn, mapping_in, channel_type_in);
        
        mapping_out = &channel_mapping_out[0];
        channel_mapping_config(nChannelsOut, mapping_out, channel_type_out);
        
        init_updownmix(nChannelsIn, nChannelsOut, &mDownMix, mapping_in, mapping_out);
    }
    else if (nChannelsIn == 2 && nChannelsOut == 1) {
        mVolData.input_mode = VOLCTRL_INPUT_STEREO;
        mVolData.downmix   = 1;
        
        mProcess16 =  &METH(processBufferDownmix_16bits);
        mProcess =  &METH(processBufferDownmix);
        
    }
    else if (nChannelsIn == 2 && nChannelsOut == 2) {
        mVolData.input_mode = VOLCTRL_INPUT_STEREO;
        mProcess16 =  &METH(processBufferStereo_16bits);
        mProcess =  &METH(processBufferStereo);
    }
    else {
        mVolData.input_mode = nChannelsIn;
        if(mVolData.downmix || mRemap || mVolData.upmix)
        {
            int *mapping_in;
            int *mapping_out;
            int channel_mapping_in[MAXCHANNELS_SUPPORT]  ={0};
            int channel_mapping_out[MAXCHANNELS_SUPPORT] ={0};

            
            memset(channel_mapping_in,  0, sizeof(channel_mapping_in));
            memset(channel_mapping_out, 0, sizeof(channel_mapping_out));

            mapping_in = &channel_mapping_in[0];
            channel_mapping_config(nChannelsIn, mapping_in, channel_type_in);

            mapping_out = &channel_mapping_out[0];
            channel_mapping_config(nChannelsOut, mapping_out, channel_type_out);

            init_updownmix(nChannelsIn, nChannelsOut, &mDownMix, mapping_in, mapping_out);
        }
        
        mProcess16 =  &METH(processMultichannel_16bits);
        mProcess =  &METH(processMultichannel);
    }
    mChannelsIn = nChannelsIn;
    mChannelsOut = nChannelsOut;
    
    mVolData.new_config |= VOLCTRL_IMMEDIAT_CMD_PENDING;
}

void METH(setMute)(BOOL bMute) {
    mMute = bMute;
    if (mInitialized) {
        VolctrlConfig_t config;
        fillConfigStruct(&config);
        setConfig(config);
    }
}

void METH(setBalance)(t_sint16 nBalance) {
    mBalance = nBalance;
    if (mInitialized) {
        VolctrlConfig_t config;
        fillConfigStruct(&config);
        setConfig(config);
    }
}

void METH(setVolume)(t_sint16 nVolume) {
    mVolume = nVolume;
    if (mInitialized) {
        VolctrlConfig_t config;
        fillConfigStruct(&config);
        setConfig(config);
    }
}

void METH(setVolumeRamp)(
    t_sint16    nStartVolume,
    t_sint16    nEndVolume,
    t_uint16    nRampChannels,
    t_uint24    nRampDuration,
    BOOL        bRampTerminate
                         ) 
{
    mVolume             = nStartVolume;
    mRampEndVolume      = nEndVolume;
    mRampChannels       = nRampChannels;
    mRampDuration       = nRampDuration;
    mRampTerminate      = bRampTerminate;
    
    if (mInitialized) {
        VolctrlRampConfig_t config;
        fillRampConfigStruct(&config);
        setRampConfig(config);
    }
}

void METH(process)(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    //   int i; 

    while (mVolData.new_config) {
        volctrl_set_new_config(&mVolData);
    }

    (*this.*mProcess)(inbuf, outbuf, size);
#if 0
    for(i=0;i<MAX_CH;i++){
        if(mRampActivated[i]){
            if(mVolData.nb_smooth_iter[i] == 0){
                // Ramp terminated
                mRampActivated[i] = 0;
                // Communication with proxy
                proxy.eventHandler(OMX_DSP_EventIndexSettingChanged,i, 0x1);
            }
        }
    }
#endif
}

void METH(process_16bits)(t_sint16 *inbuf,t_sint16  *outbuf,t_uint16 size) {
    //   int i; 

    while (mVolData.new_config) {
        volctrl_set_new_config(&mVolData);
    }

    (*this.*mProcess16)(inbuf, outbuf, size);
}

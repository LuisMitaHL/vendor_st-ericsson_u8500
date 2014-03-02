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
#include <volctrl/nmfil/effect.nmf>
#include "dbc.h"
#include <stdbool.h>
#include "libeffects/libvolctrl/include/volctrl.h"

// ******** VOLUME MIN/MAX
// volctrl min -114dB
#define VOLCTRL_VOLUME_MIN  -11400
// max value kept to 0dB: do not update to 12dB; attenuation only for the timebeing..+FIXME define used for inits..TODO(pht)
#define VOLCTRL_VOLUME_MAX  0
#define VOLCTRL_VOLUME_MUTE VOLCTRL_VOLUME_MIN

// ******** BALANCE
#define VOLCTRL_BALANCE_ALLLEFT  -100
#define VOLCTRL_BALANCE_CENTER   0
#define VOLCTRL_BALANCE_ALLRIGHT +100

// ******** RAMP DURATION in micro secs
#define VOLCTRL_RAMPDURATION_MIN 1000
#define VOLCTRL_RAMPDURATION_MAX 10000000

#define OMX_ALL                 0xFFFF
#define OMX_AUDIO_ChannelLF     0x1
#define OMX_AUDIO_ChannelRF     0x2
#define OMX_AUDIO_ChannelCF     0x3


t_uint24 EXTMEM ATTR(gGLL) = 0;
t_uint24 EXTMEM ATTR(gGRR) = 0;
 
t_uint24 EXTMEM ATTR(gTimeLeft) = 0;
t_uint24 EXTMEM ATTR(gTimeRight) = 0;
                         
////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////
static t_uint16     mChannels;
static t_uint16     mRampActivated[MAX_CH];
static VOLCTRL_LOCAL_STRUCT_T   mVolData;

////////////////////////////////////////////////////////////

static void
VolCtrl_copyChannel(t_sword * inbuf, t_sword * outbuf, t_uint16 size, t_uint16 stride) {
    int i;
    for (i = 0; i < size; i+= stride) {
        outbuf[i] = inbuf[i];
    }
}

void METH(open)()
{
    mVolData.gain_mask = (GLL_MASK | GRR_MASK);
    mVolData.downmix   = 0;
    mVolData.db_ramp   = 1;

    /* reset gll and grr to one and glr grl to zero */
    mVolData.gain[GLL]          = wL_msl(FORMAT_FLOAT(0.5,MAXVAL), (1 - GUARD_BIT + 24));
    mVolData.gain[GRR]          = wL_msl(FORMAT_FLOAT(0.5,MAXVAL), (1 - GUARD_BIT + 24));
    mVolData.gain_smoothed[GLL] = mVolData.gain[GLL];
    mVolData.gain_smoothed[GRR] = mVolData.gain[GRR];
    mVolData.gain_cmd[0][GLL]   = 0;
    mVolData.gain_cmd[0][GRR]   = 0;
    mVolData.gain_cmd[1][GLL]   = 0;
    mVolData.gain_cmd[1][GRR]   = 0;
//    mVolData.gain_cmd[2][GLL]   = 0;  //start gains
//    mVolData.gain_cmd[2][GRR]   = 0;

    mVolData.gain[GLR]          = 0;
    mVolData.gain_smoothed[GLR] = mVolData.gain[GLR];
    mVolData.gain[GRL]          = 0;
    mVolData.gain_smoothed[GRL] = mVolData.gain[GRL];
    mVolData.gain_cmd[0][GLR]   = VOLCTRL_MINIMUM_GAIN;
    mVolData.gain_cmd[0][GRL]   = VOLCTRL_MINIMUM_GAIN;
    mVolData.gain_cmd[1][GLR]   = VOLCTRL_MINIMUM_GAIN;
    mVolData.gain_cmd[1][GRL]   = VOLCTRL_MINIMUM_GAIN;
//    mVolData.gain_cmd[2][GLR]   = VOLCTRL_MINIMUM_GAIN; //start gains
//    mVolData.gain_cmd[2][GRL]   = VOLCTRL_MINIMUM_GAIN;
//	mVolData.start_ramp_from_current_gains = 0;
	mVolData.configL=1;
	mVolData.configR=1;
	mVolData.terminate[0]=mVolData.terminate[1]=mVolData.terminate[2]=mVolData.terminate[3]=0;

    mVolData.input_mode = VOLCTRL_INPUT_UNDEFINED;
    mVolData.new_config = 0;

    mRampActivated[GLL] = 0;
    mRampActivated[GRR] = 0;
    mRampActivated[GLR] = 0;
    mRampActivated[GRL] = 0;
}

void METH(reset)()
{
    
    
}

void METH(processBuffer)(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    MMshort *left_in  = inbuf;
    MMshort *right_in = inbuf + 1;
    MMshort *left_out = outbuf;
    MMshort *right_out= outbuf + 1;
    MMshort i;
    
    
    while (mVolData.new_config) {
        volctrl_set_new_config(&mVolData);
    }

    switch (mChannels) {
        case 1:
            // mono mode
            process_one_channel_no_cross_gain(
                    left_in, left_out, GLL, size, 1, &mVolData);
            break;

        case 2:
            // stereo mode
            if (mVolData.downmix) {
                process_one_channel_with_cross_gain(
                        left_in, right_in, left_out, 1, size, GLL, GRL, &mVolData);
            }
            else if ((mVolData.gain_mask & GRL_MASK) && (mVolData.gain_mask & GLR_MASK)) {
                // both cross gains not zero
                process_two_channel_with_cross_gain(
                        inbuf, outbuf, size, &mVolData);
            } 
            else if (mVolData.gain_mask & GRL_MASK) {
                // left channel from L & R
                process_one_channel_with_cross_gain(
                        left_in, right_in, left_out, 2, size, GLL, GRL, &mVolData);

                if (mVolData.gain_mask & GRR_MASK) {
                    // right channel from R
                    process_one_channel_no_cross_gain(
                            right_in, right_out, GRR, size, 2, &mVolData);
                } else {
                    VolCtrl_copyChannel(right_in, right_out, size, 2);
                }
            } 
            else if (mVolData.gain_mask & GLR_MASK) {
                if (mVolData.gain_mask & GRR_MASK) {
                    // right channel from L & R
                    process_one_channel_with_cross_gain(
                            right_in, left_in, right_out, 2, size, GRR, GLR, &mVolData);
                } else {
                    // right channel from L
                    process_one_channel_no_cross_gain(
                            left_in, right_out, GLR, size, 2, &mVolData);
                }

                // left channel from L
                process_one_channel_no_cross_gain(
                        left_in, left_out, GLL, size, 2, &mVolData);
            } else {
                // no cross gain

                // left channel from L
                process_one_channel_no_cross_gain(
                        left_in, left_out, GLL, size, 2, &mVolData);

                if (mVolData.gain_mask & GRR_MASK) {
                    // right channel from R
                    process_one_channel_no_cross_gain(
                            right_in, right_out, GRR, size, 2, &mVolData);
                } else {
                    VolCtrl_copyChannel(right_in, right_out, size, 2);
                }
            }
            break;

        default:
            ASSERT(0);
    }

    // Update attributes for getConfig OMX command
    ATTR(gGLL)= mVolData.current_gaindB[GLL]+VOLCTRL_OFFSET_GAIN;
    ATTR(gGRR)= mVolData.current_gaindB[GRR]+VOLCTRL_OFFSET_GAIN;

    ATTR(gTimeLeft) =  mVolData.current_duration[GLL];
    ATTR(gTimeRight) = mVolData.current_duration[GRR];

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
}


////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void METH(setConfig)(VolctrlConfig_t config)
{
    int i, config_index;

    /* reset gain */
    for (i = 0; i < 4; i++) {
        mVolData.gain[i] = 0;
    	mVolData.duration[i] = config.ialpha;
    }

    mVolData.downmix = config.iDownMix;
    mVolData.db_ramp = config.iDBRamp; 

    /* Save command gain */
    if (config.ialpha == 0) {
        config_index = 1;
        mVolData.new_config = VOLCTRL_IMMEDIAT_CMD_PENDING;
    } else {
        config_index = 0;
        mVolData.new_config |= 1; // do not reset VOLCTRL_IMMEDIAT_CMD_PENDING bit
    }

    mVolData.gain_cmd[config_index][GLL] = config.igll - VOLCTRL_OFFSET_GAIN;
    mVolData.gain_cmd[config_index][GLR] = config.iglr - VOLCTRL_OFFSET_GAIN;
    mVolData.gain_cmd[config_index][GRL] = config.igrl - VOLCTRL_OFFSET_GAIN;
    mVolData.gain_cmd[config_index][GRR] = config.igrr - VOLCTRL_OFFSET_GAIN;
    mVolData.configL=mVolData.configR=1;
    mVolData.terminate[0]=mVolData.terminate[1]=mVolData.terminate[2]=mVolData.terminate[3]=0;

    // Update attributes for getConfig OMX command
    // dont use current_gaindB => update based on current_gaindB will be done upon process
    // use gain_cmd, in case no process runs (idle for instance) in order to have coherent getconfig after setconfig
    ATTR(gGLL)= config.igll;
    ATTR(gGRR)= config.igrr;
}

void METH(setRampConfig)(VolctrlRampConfig_t config)
{
    if ((config.iChannel&1)!=0) //left channel setRampConfig
    {
        if (config.iTerminate==0)
        {
            mVolData.new_config = VOLCTRL_IMMEDIAT_CMD_PENDING+1;
            mVolData.gain_cmd[1][GLL] 	= config.iStartVolume - VOLCTRL_OFFSET_GAIN;
            mVolData.gain_cmd[0][GLL] 	= config.iEndVolume - VOLCTRL_OFFSET_GAIN;
            mVolData.duration[GLL]		= config.ialpha;
            mVolData.configL=1;
            if(mRampActivated[GLL]==1){
                // Communication with proxy to indicate that the previous ramp is ending
                proxy.eventHandler(OMX_DSP_EventIndexSettingChanged,GLL, 0x1);
            }
            mRampActivated[GLL]         = 1;
        }
        mVolData.terminate[GLL]		= config.iTerminate;

        // Update attributes for getConfig OMX command
        ATTR(gGLL)= config.iStartVolume;
    }

    if ((config.iChannel&2)!=0) //right channel setRampConfig
    {
        if (config.iTerminate==0)
        {
            mVolData.new_config = VOLCTRL_IMMEDIAT_CMD_PENDING+1;
            mVolData.gain_cmd[1][GRR] 	= config.iStartVolume - VOLCTRL_OFFSET_GAIN;
            mVolData.gain_cmd[0][GRR] 	= config.iEndVolume - VOLCTRL_OFFSET_GAIN;
            mVolData.duration[GRR]		= config.ialpha;
            mVolData.configR=1;
            if(mRampActivated[GRR]==1){
                // Communication with proxy to indicate that the previous ramp is ending
                proxy.eventHandler(OMX_DSP_EventIndexSettingChanged,GRR, 0x1);
            }
            mRampActivated[GRR]         = 1;
        }
        mVolData.terminate[GRR]		= config.iTerminate;

        // Update attributes for getConfig OMX command
        ATTR(gGRR)= config.iStartVolume;
    }

}


void METH(newFormat) (
        t_sample_freq *freq,
        t_uint16 *chans_nb,
        t_uint16 *sample_bitsize)
{
    mChannels = *chans_nb;

    volctrl_init(&mVolData, *freq);

    switch (mChannels) {
        case 1:
            mVolData.input_mode = VOLCTRL_INPUT_MONO;
            break;

        case 2:
            mVolData.input_mode = VOLCTRL_INPUT_STEREO;
            break;

        default:
            ASSERT(0);
    }

    if (mVolData.downmix) {
        ASSERT(*chans_nb == 2);
        *chans_nb = 1;
    }

    mVolData.new_config |= VOLCTRL_IMMEDIAT_CMD_PENDING;
}


////////////////////////////////////////////////////////////
//          new Interface
////////////////////////////////////////////////////////////

static void (*mProcess)(t_sword * inbuf, t_sword * outbuf, t_uint16 size);
static BOOL             mMute;
static BOOL             mInitialized = false;
static t_sint16         mBalance;
static t_sint16         mVolume;
static t_sint16         mRampEndVolume;
static t_sint16         mRampChannels;
static t_uint24         mRampDuration;
static BOOL             mRampTerminate;


static void
processBufferDownmix(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    MMshort *left_in  = inbuf;
    MMshort *right_in = inbuf + 1;
    MMshort *left_out = outbuf;
    MMshort *right_out= outbuf + 1;
    MMshort i;

    process_one_channel_with_cross_gain(
            left_in, right_in, left_out, 1, size, GLL, GRL, &mVolData);
}

static void
processBufferMono(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    MMshort *left_in  = inbuf;
    MMshort *right_in = inbuf + 1;
    MMshort *left_out = outbuf;
    MMshort *right_out= outbuf + 1;
    MMshort i;

    process_one_channel_no_cross_gain(
            left_in, left_out, GLL, size, 1, &mVolData);
}

static void
processBufferStereo(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    MMshort *left_in  = inbuf;
    MMshort *right_in = inbuf + 1;
    MMshort *left_out = outbuf;
    MMshort *right_out= outbuf + 1;
    MMshort i;

    process_one_channel_no_cross_gain(
            left_in, left_out, GLL, size, 2, &mVolData);
    process_one_channel_no_cross_gain(
            right_in, right_out, GRR, size, 2, &mVolData);
}

static void
fillConfigStruct(VolctrlConfig_t *config) {
    t_sint16 leftVolumemdB, rightVolumemdB;

    if (mMute) {
        leftVolumemdB   = VOLCTRL_VOLUME_MUTE;
        rightVolumemdB  = VOLCTRL_VOLUME_MUTE;
    } else {
        leftVolumemdB  = mBalance <= 0 ?
                (mVolume) :
                (mVolume + mBalance*(VOLCTRL_VOLUME_MIN - mVolume)/VOLCTRL_BALANCE_ALLRIGHT);
        rightVolumemdB = mBalance <= 0 ?
                (mVolume + mBalance*(VOLCTRL_VOLUME_MIN - mVolume)/VOLCTRL_BALANCE_ALLLEFT) :
                (mVolume);
    }
    
    config->iDownMix = mVolData.downmix; // downmix set at init time so dont change it
    config->iDBRamp  = 1;
    config->igll     = (128<<8); //offset
    config->igll     += (leftVolumemdB<<8)/100; //care to take into account values < 1dB
    config->iglr     = 0;
    config->igrl     = 0;
    config->igrr     = (128<<8); //offset
    config->igrr     += (rightVolumemdB<<8)/100; //care to take into account values < 1dB
    config->ialpha   = 0;
}

static void
fillRampConfigStruct(VolctrlRampConfig_t *config) {

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
    else { ASSERT(0);}

    config->iTerminate      = mRampTerminate;
}
void METH(init) (t_uint16 nChannelsIn, t_uint16 nChannelsOut, t_sample_freq freq) {
    
    METH(open)();
    volctrl_init(&mVolData, freq);
    mInitialized = true;
    
    if (mRampDuration != 0) {
        VolctrlRampConfig_t config;
        fillRampConfigStruct(&config);
        METH(setRampConfig)(config);
    } else {
        VolctrlConfig_t config;
        fillConfigStruct(&config);
        METH(setConfig)(config);
    }

    if (nChannelsIn == 1 && nChannelsOut == 1) {
        mVolData.input_mode = VOLCTRL_INPUT_MONO;
        mProcess = processBufferMono;
    }
    else if (nChannelsIn == 2 && nChannelsOut == 1) {
        mVolData.input_mode = VOLCTRL_INPUT_STEREO;
        mVolData.downmix   = 1;
        mProcess = processBufferDownmix;
    }
    else if (nChannelsIn == 2 && nChannelsOut == 2) {
        mVolData.input_mode = VOLCTRL_INPUT_STEREO;
        mProcess = processBufferStereo;
    }
    else {
        ASSERT(0);
    }
    mVolData.new_config |= VOLCTRL_IMMEDIAT_CMD_PENDING;
}

void METH(setMute)(BOOL bMute) {
    mMute = bMute;
    if (mInitialized) {
        VolctrlConfig_t config;
        fillConfigStruct(&config);
        METH(setConfig)(config);
    }
}

void METH(setBalance)(t_sint16 nBalance) {
    mBalance = nBalance;
    if (mInitialized) {
        VolctrlConfig_t config;
        fillConfigStruct(&config);
        METH(setConfig)(config);
    }
}

void METH(setVolume)(t_sint16 nVolume) {
    mVolume = nVolume;
    if (mInitialized) {
        VolctrlConfig_t config;
        fillConfigStruct(&config);
        METH(setConfig)(config);
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
        METH(setRampConfig)(config);
    }
}

void METH(process)(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
    int i; 

    while (mVolData.new_config) {
        volctrl_set_new_config(&mVolData);
    }

    ASSERT(mProcess != 0);

    mProcess(inbuf, outbuf, size);

    // Update attributes for getConfig OMX command
    ATTR(gGLL)= mVolData.current_gaindB[GLL]+VOLCTRL_OFFSET_GAIN;
    ATTR(gGRR)= mVolData.current_gaindB[GRR]+VOLCTRL_OFFSET_GAIN;

    ATTR(gTimeLeft) =  mVolData.current_duration[GLL];
    ATTR(gTimeRight) = mVolData.current_duration[GRR];

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
}

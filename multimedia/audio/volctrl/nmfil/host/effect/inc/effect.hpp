/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/**
 * \file   effect.hpp
 * \brief
 * \author ST-Ericsson
 */

#ifndef _volctrl_nmfil_host_effect_hpp_
#define _volctrl_nmfil_host_effect_hpp_

extern "C"{
#include "libeffects/libvolctrl/include/volctrl.h"
#include "updownmix.h"
}

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

//#define OMX_ALL                 0xFFFF
#define OMX_AUDIO_ChannelLF     0x1
#define OMX_AUDIO_ChannelRF     0x2
#define OMX_AUDIO_ChannelCF     0x3

class volctrl_nmfil_host_effect : public volctrl_nmfil_host_effectTemplate
{
public:

    volctrl_nmfil_host_effect();
    virtual ~volctrl_nmfil_host_effect();

    // Effect interface...
    virtual t_bool open(const t_host_effect_config* config, t_effect_caps* caps);
    virtual void reset(t_effect_reset_reason reason);
    virtual void process(t_effect_process_params* params);
    virtual void close(void);

    // Volume config interface...
    virtual void setConfig(VolctrlConfig_t config);
    virtual void setRampConfig(VolctrlRampConfig_t config);
    virtual void getCurrentRamp(VolCtrlCurrentRamp_t *volume, t_uint32 channel);

    // New interfaces with mixer
    virtual void init (t_uint16 nChannelsIn, t_uint16 nChannelsOut, t_sample_freq freq,t_channel_type channel_type_in[MAXCHANNELS_SUPPORT], t_channel_type channel_type_out[MAXCHANNELS_SUPPORT]);
    virtual void process(t_sword * inbuf, t_sword * outbuf, t_uint16 size);
    virtual void process_16bits(t_sint16 * inbuf,t_sint16  * outbuf, t_uint16 size);
    virtual void setMute(BOOL bMute);
    virtual void setBalance(t_sint16 nBalance);
    virtual void setVolume(t_sint16 nVolume);
    virtual void setVolumeRamp(t_sint16 nStartVolume, t_sint16 nEndVolume, t_uint16 nRampChannels, t_uint24 nDuration, BOOL bTerminate);

    //Local functions
    void processBufferDownmix(t_sword * inbuf, t_sword * outbuf, t_uint16 size);
    void processBufferUpmix(t_sword * inbuf, t_sword * outbuf, t_uint16 size);
    void processBufferMono(t_sword * inbuf, t_sword * outbuf, t_uint16 size);
    void processBufferStereo(t_sword * inbuf, t_sword * outbuf, t_uint16 size);
    void processMultichannel(t_sword * inbuf, t_sword * outbuf, t_uint16 size);
    void processBufferDownmix_16bits(t_sint16 *inbuf,t_sint16  *outbuf, t_uint16 size);
    void processBufferUpmix_16bits(t_sint16 *inbuf,t_sint16  *outbuf, t_uint16 size);
    void processBufferMono_16bits(t_sint16 *inbuf,t_sint16  *outbuf, t_uint16 size);
    void processBufferStereo_16bits(t_sint16 *inbuf,t_sint16  *outbuf, t_uint16 size);
    void processMultichannel_16bits(t_sint16 *inbuf,t_sint16 *outbuf, t_uint16 size);
    void fillConfigStruct(VolctrlConfig_t *config);
    void fillRampConfigStruct(VolctrlRampConfig_t *config);


private:

    static t_sint16 saturate(int value);
    static void copyChannel(int * inbuf, int * outbuf, int size, int stride);
    void channel_mapping_config(unsigned short nchannels, int *mapping, const t_channel_type channel_mapping[]);

    int mOpen;
    int mConfig;
    int mMyProcess;
    int *mInBuf;
    int *mOutBuf;

    VolctrlRampConfig_t  m_ramp;
    t_host_effect_config m_config;
    VOLCTRL_LOCAL_STRUCT_T   mVolData;
    UPDOWNMIX_LOCAL_STRUCT_T mDownMix;

    void (volctrl_nmfil_host_effect::*mProcess)(t_sword * inbuf, t_sword * outbuf, t_uint16 size);
    void (volctrl_nmfil_host_effect::*mProcess16)(t_sint16 *inbuf,t_sint16  *outbuf, t_uint16 size);
    BOOL             mMute;
    BOOL             mInitialized;
    BOOL             mRemap;
    t_sint16         mBalance;
    t_sint16         mVolume;
    t_sint16         mRampEndVolume;
    t_sint16         mRampChannels;
    t_uint32         mRampDuration;
    BOOL             mRampTerminate;
    t_uint16         mChannelsIn;
    t_uint16         mChannelsOut;

};

#endif // _volctrl_nmfil_host_effect_hpp_


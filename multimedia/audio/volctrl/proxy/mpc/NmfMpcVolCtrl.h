/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcVolCtrl.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _NMFMPCVOLCTRL_H_
#define _NMFMPCVOLCTRL_H_

#include "NmfMpcPcmProcessing.h"
#include "audio_hw_control_chipset_api.h"
#include "audio_chipset_api_index.h"
#include "OMX_Symbian_AudioExt_Ste.h"

#include "host/volctrl/nmfil/effect/configure.hpp"

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

/// Concrete class implementing a VolCtrl component
/// Derived from NmfMpcPcmProcessing
class NmfMpcVolCtrl: public NmfMpcPcmProcessing {
public:
    NmfMpcVolCtrl(OMX_BOOL bDownmix,
                  t_pcmprocessing_config config,
                  OMX_U32 portIndex, OMX_BOOL usemultichannelmode = OMX_FALSE);

    virtual ~NmfMpcVolCtrl(void);

    virtual OMX_ERRORTYPE instantiateAlgo(void);
    virtual OMX_ERRORTYPE startAlgo(void);
    virtual OMX_ERRORTYPE stopAlgo(void);
    virtual OMX_ERRORTYPE configureAlgo(void);
    virtual OMX_ERRORTYPE deInstantiateAlgo(void);

    OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex,
                              OMX_PTR pComponentConfigStructure);

    OMX_ERRORTYPE retrieveConfig(OMX_INDEXTYPE nConfigIndex,
                              OMX_PTR pComponentConfigStructure);

private:
    t_cm_instance_handle mNmfEffectsLib;
    t_cm_instance_handle mOstTrace;

    Ivolctrl_nmfil_effect_configure mIconfigure;

    // config
    OMX_BOOL mDownmix;
    OMX_U32  mNbChannel;

    void registerStubsAndSkels(void);
    void unregisterStubsAndSkels(void);

    OMX_U32  mPortIndex;

    OMX_BOOL mIsRampRunning;
    OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE mConfigVolumeRamp;

    // volctrl can either be used with volume or volume channel API: exclusive usage decided at ctor time
    // if multichannel API mode chosen, then volume application will be based on that only => discarding balance config
    OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * mpConfigVolumeChannel;
    OMX_AUDIO_CONFIG_VOLUMETYPE     mConfigVolume;

    OMX_AUDIO_CONFIG_MUTETYPE       mConfigMute;
    OMX_AUDIO_CONFIG_BALANCETYPE    mConfigBalance;
};

#endif //_NMFMPCVOLCTRL_H_

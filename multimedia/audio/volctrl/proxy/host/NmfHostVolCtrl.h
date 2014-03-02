/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfHostVolCtrl.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _NMFHOSTVOLCTRL_H_
#define _NMFHOSTVOLCTRL_H_

#include "NmfHostPcmProcessing.h"
#include "audio_chipset_api_index.h"
#include "OMX_Symbian_AudioExt_Ste.h"
#include "audio_hw_control_chipset_api.h"

#include "volctrl/nmfil/host/effect/configure.hpp"
#include "hst/pcmprocessings/effect.hpp"
#include "volctrl/nmfil/host/effectWrapped.hpp"


// ******** VOLUME MIN/MAX
#define VOLCTRL_VOLUME_MIN      -11400  // -114dB
#define VOLCTRL_VOLUME_MAX       0      //    0dB
#define VOLCTRL_VOLUME_MUTE      VOLCTRL_VOLUME_MIN

// ******** MULTICHANNEL
#define VOLCTRL_START_INDEX_MULTICHANNEL_GAINS   4
#define VOLCTRL_MAX_VOLUME_GAINS                 8

// ******** BALANCE
#define VOLCTRL_BALANCE_ALLLEFT   -100
#define VOLCTRL_BALANCE_CENTER     0
#define VOLCTRL_BALANCE_ALLRIGHT  +100

// ******** RAMP DURATION in micro secs
#define VOLCTRL_RAMPDURATION_MIN 1000
#define VOLCTRL_RAMPDURATION_MAX 10000000

/// Concrete class implementing a VolCtrl component
/// Derived from NmfHostPcmProcessing
class NmfHostVolCtrl: public NmfHostPcmProcessing
{
    public:
        NmfHostVolCtrl(OMX_BOOL bDownmix,OMX_BOOL bMultichannel, t_host_pcmprocessing_config config, OMX_U32 portIndex);

        virtual ~NmfHostVolCtrl(void);

        virtual OMX_ERRORTYPE instantiateAlgo(void);
        virtual OMX_ERRORTYPE startAlgo(void);
        virtual OMX_ERRORTYPE stopAlgo(void);
        virtual OMX_ERRORTYPE configureAlgo(void);
        virtual OMX_ERRORTYPE deInstantiateAlgo(void);

        OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
    
    private:
        void applyVolumeConfig(void);

        Ivolctrl_nmfil_host_effect_configure     mIconfigure;

        OMX_U32     mPortIndex;
        OMX_BOOL    mDownmix;
        OMX_BOOL    mMultichannel;
        OMX_BOOL    mIsRampRunning;
        OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE mConfigVolumeRamp;
        OMX_AUDIO_CONFIG_VOLUMETYPE     mConfigVolume;
        OMX_AUDIO_CONFIG_MUTETYPE       mConfigMute;
        OMX_AUDIO_CONFIG_BALANCETYPE    mConfigBalance;
};

#endif //_NMFHOSTVOLCTRL_H_

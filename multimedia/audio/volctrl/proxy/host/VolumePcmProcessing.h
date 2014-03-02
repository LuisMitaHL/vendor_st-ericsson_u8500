/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Volume pcm processor proxy
 * \author  ST-Ericsson
 */
/*****************************************************************************/
#ifndef _VOLUME_PCM_PROCESSING_H_
#define _VOLUME_PCM_PROCESSING_H_

#include "AFM_PcmProcessing.h"
#include "audio_chipset_api_index.h"
#include "audio_render_chipset_api.h"

#define VOLCTRL_VOLUME_MIN          -10000  // -100dB
#define VOLCTRL_VOLUME_MAX          0       //    0dB
#define VOLCTRL_VOLUME_MUTE         VOLCTRL_VOLUME_MIN

#define VOLCTRL_BALANCE_ALLLEFT     -100
#define VOLCTRL_BALANCE_CENTER      0
#define VOLCTRL_BALANCE_ALLRIGHT    +100

OMX_ERRORTYPE volPcmProcFactoryMethod(ENS_Component_p *ppENSComponent);


class VolumePcmProcessing : public AFM_PcmProcessing
{
    public:

        // Constructor
        OMX_ERRORTYPE  construct(void);
        virtual ~VolumePcmProcessing() {};

        virtual OMX_ERRORTYPE setConfig( OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
        virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const;

        virtual OMX_AUDIO_CONFIG_VOLUMERAMPTYPE  *getConfigVolumeRamp() {return &mConfigVolumeRamp;}
        virtual OMX_AUDIO_CONFIG_VOLUMETYPE *getConfigVolume() {return &mConfigVolume;}
        virtual OMX_AUDIO_CONFIG_MUTETYPE *getConfigMute() {return &mConfigMute;}
        virtual OMX_AUDIO_CONFIG_BALANCETYPE *getConfigBalance() {return &mConfigBalance;}
        virtual OMX_BOOL getRampRunning() const {return mRampRunning;}
        virtual void setRampRunning(OMX_BOOL Ramp) {mRampRunning = Ramp;}

        virtual OMX_U32 getSampleBitSize(void) const {return 16;} 

    private:

        //OMX volume configurations
        OMX_BOOL                        mRampRunning;
        OMX_AUDIO_CONFIG_VOLUMERAMPTYPE mConfigVolumeRamp;
        OMX_AUDIO_CONFIG_VOLUMETYPE     mConfigVolume;
        OMX_AUDIO_CONFIG_MUTETYPE       mConfigMute;
        OMX_AUDIO_CONFIG_BALANCETYPE    mConfigBalance; 
};

#endif // _VOLUME_PCM_PROCESSING_H_

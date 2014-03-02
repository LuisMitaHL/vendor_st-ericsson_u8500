/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Volume Control NMF Host Processing class headers
 * \author  ST-Ericsson
 */
/*****************************************************************************/

#ifndef _VOLUME_NMF_HOST_PCM_PROCESSING_H_
#define _VOLUME_NMF_HOST_PCM_PROCESSING_H_

#include "audio_chipset_api_index.h"
#include "audio_render_chipset_api.h"
#include "audio_hw_control_chipset_api.h"

#include "AFMNmfHost_PcmProcessing.h"
#include "volctrl/nmfil/host/effect/configure.hpp"
#include "hst/pcmprocessings/effect.hpp"
#include "volctrl/nmfil/host/effectWrapped.hpp"


#define VOLCTRL_START_INDEX_MULTICHANNEL_GAINS   4
#define VOLCTRL_MAX_VOLUME_GAINS                 8


// Concrete class implementing volume processing component
class VolumeNmfHost_PcmProcessing: public AFMNmfHost_PcmProcessing
{
    public:

        VolumeNmfHost_PcmProcessing(ENS_Component &enscomp): AFMNmfHost_PcmProcessing(enscomp), mIsRampRunning(OMX_FALSE),
                                                             mConfigVolumeRamp(0), mConfigVolume(0), mConfigMute(0), mConfigBalance(0) {}

        virtual OMX_ERRORTYPE construct(void);
        virtual OMX_ERRORTYPE destroy(void);

        virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

    protected:

        virtual OMX_ERRORTYPE instantiateAlgo(void);
        virtual OMX_ERRORTYPE startAlgo(void);
        virtual OMX_ERRORTYPE stopAlgo(void);
        virtual OMX_ERRORTYPE configureAlgo(void);
        virtual OMX_ERRORTYPE deInstantiateAlgo(void);
        virtual OMX_U32       nbBitPerSampleProcessed(){return 32;}; // Should be 16 or 32

    private:

        void applyVolumeConfig(void);

        Ivolctrl_nmfil_host_effect_configure  mIConfig;

        // config
        OMX_BOOL    mIsRampRunning;
        OMX_AUDIO_CONFIG_VOLUMERAMPTYPE *mConfigVolumeRamp;
        OMX_AUDIO_CONFIG_VOLUMETYPE     *mConfigVolume;
        OMX_AUDIO_CONFIG_MUTETYPE       *mConfigMute;
        OMX_AUDIO_CONFIG_BALANCETYPE    *mConfigBalance;

};

#endif // _VOLUME_NMF_HOST_PCM_PROCESSING_H_

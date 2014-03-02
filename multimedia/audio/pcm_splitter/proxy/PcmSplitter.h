/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _PCMSPLITTER_H_
#define _PCMSPLITTER_H_

#include "AFM_Component.h"

#include "audio_chipset_api_index.h"
#include "audio_codecs_chipset_api.h"
#include "AFM_Symbian_AudioExt.h"

#define NB_OUTPUT_PORTS  2
#define NB_INPUT_PORTS   1
#define INPUT_PORT_IDX   0

#define MS_GRANULARITY 5

/// PcmSplitter factory method used when registering the PcmSplitter
/// component to the ENS Core
OMX_ERRORTYPE pcmsplitterFactoryMethod(ENS_Component_p * ppENSComponent);
OMX_ERRORTYPE pcmsplitterhostFactoryMethod(ENS_Component_p * ppENSComponent);

/// Concrete class implementing a PcmSplitter component
/// Derived from AFM_Component
class PcmSplitter: public AFM_Component {
    public:
        /// Constructor
        PcmSplitter(OMX_BOOL isHost):mIsHost(isHost) { };
        OMX_ERRORTYPE  construct(void);

        virtual ~PcmSplitter(void);

        virtual OMX_ERRORTYPE getParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure) const;

        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);

        virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure) const;

        virtual OMX_ERRORTYPE getExtensionIndex(
                OMX_STRING cParameterName,
                OMX_INDEXTYPE* pIndexType) const;

        OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE  *getConfigVolumeRamp(OMX_U32 portIdx)  {
            return &mConfigVolumeRamp[portIdx];
        }
        OMX_AUDIO_CONFIG_VOLUMETYPE *getConfigVolume(OMX_U32 portIdx)  {
            return &mConfigVolume[portIdx];
        }
        OMX_AUDIO_CONFIG_MUTETYPE *getConfigMute(OMX_U32 portIdx)  {
            return &mConfigMute[portIdx];
        }
        OMX_AUDIO_CONFIG_BALANCETYPE *getConfigBalance(OMX_U32 portIdx)  {
            return &mConfigBalance[portIdx];
        }

        virtual OMX_U32 getUidTopDictionnary(void);

        virtual bool prioritiesSetForAllPorts() const {return false;};

    private:
        OMX_U32       mBlockSize;
        void setDefaultSettings();
        OMX_BOOL      mIsHost;

        //OMX volume configurations
        OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE mConfigVolumeRamp[NB_OUTPUT_PORTS+1];
        OMX_AUDIO_CONFIG_VOLUMETYPE     mConfigVolume[NB_OUTPUT_PORTS+1];
        OMX_AUDIO_CONFIG_MUTETYPE       mConfigMute[NB_OUTPUT_PORTS+1];
        OMX_AUDIO_CONFIG_BALANCETYPE    mConfigBalance[NB_OUTPUT_PORTS+1];

        bool lowLatencySrc(OMX_U32 sampleRateIn, OMX_U32 sampleRateOut) const;
};

#endif //_PCMSPLITTER_H_

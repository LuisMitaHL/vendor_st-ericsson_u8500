/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerPort.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MIXERPORT_H_
#define _MIXERPORT_H_

#include "AFM_PcmPort.h"
#include "ENS_List.h"
#include "AudioEffect.h"

#define MAX_NB_EFFECT_CONFIGS   20

typedef struct {
    OMX_U32   nConfigIndex;
    OMX_U32   nEffectSpecificIndex;
    unsigned int    nPosition;
} ConfigInfo_t;

class MixerPort: public AFM_PcmPort {
    public:
        MixerPort(OMX_U32 nIndex, OMX_DIRTYPE eDir, ENS_Component &enscomp)
            : AFM_PcmPort(nIndex, eDir, defaultNbChannels, defaultSamplingRate, defaultBitPerSample, enscomp),
            mNbConfigsToCommit(0) {};
        virtual ~MixerPort(void);

        virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
        virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const;

        OMX_ERRORTYPE setEffectConfig(OMX_U32 nPositionIndex, OMX_INDEXTYPE nEffectSpecificIndex, OMX_PTR pEffectConfigStructure);
        OMX_ERRORTYPE getEffectConfig(OMX_U32 nPositionIndex, OMX_INDEXTYPE nEffectSpecificIndex, OMX_PTR pEffectConfigStructure);

        OMX_U32 getNbConfigsToCommit(void) {return mNbConfigsToCommit;}
        void resetNbConfigsToCommit(void) {mNbConfigsToCommit=0;}
        ConfigInfo_t * getConfigToCommitInfo(OMX_U32 configTableIndex) {return &mConfigsToCommitTable[configTableIndex];}

        ENS_List * getEffectNodeList(void) {return &mEffectNodeList;}

		AudioEffect * getEffectNode(OMX_U32 nPosition);

        inline OMX_BOOL isLoadedState(void) {
            OMX_STATETYPE state;
            ENS_Component& myENSComponent=getENSComponent();
            DBC_ASSERT(getENSComponent().GetState(&myENSComponent,&state) == OMX_ErrorNone);
            return ((state == OMX_StateLoaded)?OMX_TRUE:OMX_FALSE);
        }

    private:
        OMX_ERRORTYPE checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const;

        void removeEffectNode(OMX_U32 nPosition);

        void destroyAllEffectsInList(ENS_List *list);

        static const OMX_U32    defaultSamplingRate   = 48000;
        static const OMX_U32    defaultNbChannels     = 2;
        static const OMX_U32    defaultBitPerSample   = 16;

        //list items are EffectNode types.
        ENS_List                mEffectNodeList;

        //Configuration to apply on effect nodes are stored here till Commit is not done.
        //Only used when port is enabled and component is not in loaded state.
        ConfigInfo_t            mConfigsToCommitTable[MAX_NB_EFFECT_CONFIGS];
        OMX_U32                 mNbConfigsToCommit;

        inline void storeConfigInCommitStructTable(OMX_INDEXTYPE nConfigIndex, OMX_INDEXTYPE nEffectSpecificIndex, OMX_U32 nPositionIndex) {
            DBC_ASSERT(mNbConfigsToCommit < MAX_NB_EFFECT_CONFIGS);
            mConfigsToCommitTable[mNbConfigsToCommit].nConfigIndex         = nConfigIndex;
            mConfigsToCommitTable[mNbConfigsToCommit].nEffectSpecificIndex = nEffectSpecificIndex;
            mConfigsToCommitTable[mNbConfigsToCommit].nPosition            = nPositionIndex;
            mNbConfigsToCommit++;
        }
};

#endif // _MIXERPORT_H_

/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerPort.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "MixerPort.h"
#include "OMX_STE_AudioEffectsExt.h"
#include <string.h>
#include "AFM_Utils.h"

#include "OstTraceDefinitions_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mixer_proxy_MixerPortTraces.h"
#endif
#include "OpenSystemTrace_ste.h"

#define LOG_TAG "MixerPort"
#include "linux_utils.h"

/////////////////////////////////////////////////////////////////////////////////////////////

MixerPort::~MixerPort(void) {
    destroyAllEffectsInList(&mEffectNodeList);
}

OMX_ERRORTYPE MixerPort::checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings)const
{
    if (pcmSettings.nChannels == 6) {
        bool hasChannelLF = false;
        bool hasChannelRF = false;
        bool hasChannelCF = false;
        bool hasChannelLS = false;
        bool hasChannelRS = false;
        bool hasChannelLFE = false;

        for (int i = 0; i < 6; i++) {
            switch (pcmSettings.eChannelMapping[i]) {
                case OMX_AUDIO_ChannelLF:
                    hasChannelLF = true;
                    break;
                case OMX_AUDIO_ChannelRF:
                    hasChannelRF = true;
                    break;
                case OMX_AUDIO_ChannelCF:
                    hasChannelCF = true;
                    break;
                case OMX_AUDIO_ChannelLS:
                    hasChannelLS = true;
                    break;
                case OMX_AUDIO_ChannelRS:
                    hasChannelRS = true;
                    break;
                case OMX_AUDIO_ChannelLFE:
                    hasChannelLFE = true;
                    break;
                default:
                    break;
            }
        }
        if (!hasChannelLF || !hasChannelRF || !hasChannelCF ||
            !hasChannelLS || !hasChannelRS || !hasChannelLFE) {
            return OMX_ErrorUnsupportedSetting;
        }
    }
    else if (pcmSettings.nChannels > 2) {
        return OMX_ErrorUnsupportedSetting;
    }

    switch (pcmSettings.nSamplingRate) {
        case 8000:
        case 11025:
        case 12000:
        case 16000:
        case 22050:
        case 24000:
        case 32000:
        case 44100:
        case 48000:
            break;
        default:
            return OMX_ErrorUnsupportedSetting;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerPort::setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioEffectAddPosition:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_EFFECTADDPOSITIONTYPE);
                OMX_AUDIO_CONFIG_EFFECTADDPOSITIONTYPE * pConfig = (OMX_AUDIO_CONFIG_EFFECTADDPOSITIONTYPE *) pComponentConfigStructure;
                OMX_ERRORTYPE error;

                OstTraceFiltInst2(TRACE_DEBUG,"MixerPort::setConfig (OMX_IndexConfigAudioEffectAddPosition) position %d on port Id %d", pConfig->nPositionIndex, getPortIndex());
                OstTraceFiltInstData(TRACE_OMX_API, "MixerPort::setConfig (OMX_IndexConfigAudioEffectAddPosition) EffectName = %{int8[]}", (OMX_U8 *)pConfig->cEffectName, strlen(pConfig->cEffectName));

                if(pConfig->nPositionIndex == 0) {
                    OstTraceFiltInst0(TRACE_ERROR,"MixerPort::setConfig (OMX_IndexConfigAudioEffectAddPosition) effect can not be added at position 0");
                    return OMX_ErrorUnsupportedSetting;
                }

                //Check if there is already an effect Node at the requested position
                if(getEffectNode(pConfig->nPositionIndex) != NULL) {
                    OstTraceFiltInst0(TRACE_DEBUG,"MixerPort::one effect exists at this position -> we must remove it before adding new effect");

                    //Remove effect in mEffectNodeList
                    removeEffectNode(pConfig->nPositionIndex);

                    if( isEnabled() && (!isLoadedState()) ) {
                        //We must store config in mConfigsToCommitTable only if port is enabled and component is not in OMX_StateLoaded
                        storeConfigInCommitStructTable((OMX_INDEXTYPE)OMX_IndexConfigAudioEffectRemovePosition, (OMX_INDEXTYPE)0, pConfig->nPositionIndex);
                    }
                }

                //Create new effect Node
                AudioEffectsRegister *pEffectRegister = AudioEffectsRegister::getInstance();
                AudioEffect *pNewEffectNode = NULL;
                error = pEffectRegister->createEffect(&pNewEffectNode,
                        getENSComponent(),
                        AudioEffectParams(getPortIndex(),
                            pConfig->nPositionIndex,
                            pConfig->cEffectName));
                if(error != OMX_ErrorNone) {
                    OstTraceFiltInst2(TRACE_ERROR,"MixerPort::setConfig (OMX_IndexConfigAudioEffectAddPosition) can not create effect at position %d on port Id %d", pConfig->nPositionIndex, getPortIndex());
                    LOGE("MixerPort::setConfig : OMX_IndexConfigAudioEffectAddPosition : can not create effect %s at position %d on port Id %d\n", pConfig->cEffectName, (OMX_U16)pConfig->nPositionIndex, (OMX_U16)getPortIndex());
                    return error;
                }

                mEffectNodeList.pushBack(pNewEffectNode);

                if( isEnabled() && (!isLoadedState()) ) {
                    //We must store config in mConfigsToCommitTable only if port is enabled and component is not in OMX_StateLoaded
                    storeConfigInCommitStructTable((OMX_INDEXTYPE)OMX_IndexConfigAudioEffectAddPosition, (OMX_INDEXTYPE)0, pConfig->nPositionIndex);
                }

                return OMX_ErrorNone;
            }
        case OMX_IndexConfigAudioEffectRemovePosition:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_EFFECTREMOVEPOSITIONTYPE);
                OMX_AUDIO_CONFIG_EFFECTREMOVEPOSITIONTYPE * pConfig = (OMX_AUDIO_CONFIG_EFFECTREMOVEPOSITIONTYPE *) pComponentConfigStructure;

                if(pConfig->nPositionIndex == OMX_ALL) {

                    if(mEffectNodeList.isEmpty()) {
                        OstTraceFiltInst1(TRACE_DEBUG,"MixerPort::setConfig (OMX_IndexConfigAudioEffectRemovePosition) nothing to remove (no effect was added on port Id %d)", getPortIndex());
                        return OMX_ErrorNone;
                    }

                    if( isEnabled() && (!isLoadedState()) ) {
                        ENS_ListItem *pEffectItem = mEffectNodeList.getFirst();
                        while (pEffectItem != NULL) {
                            AudioEffect *pAudioEffect = static_cast<AudioEffect *>(pEffectItem->getData());
                            storeConfigInCommitStructTable((OMX_INDEXTYPE)OMX_IndexConfigAudioEffectRemovePosition, (OMX_INDEXTYPE)0, pAudioEffect->getPosition());
                            pEffectItem = pEffectItem->getNext();
                        }
                    }

                    destroyAllEffectsInList(&mEffectNodeList);

                } else {

                    //Check that one has been previously added at this position
                    AudioEffect *pAudioEffect = getEffectNode(pConfig->nPositionIndex);
                    if(pAudioEffect == NULL) {
                        OstTraceFiltInst2(TRACE_ERROR,"MixerPort::setConfig return OMX_ErrorUnsupportedSettings (no effect present at position %d on port Id %d)", pConfig->nPositionIndex, getPortIndex());
                        return OMX_ErrorUnsupportedSetting; //effect list is empty : no effects can be remove
                    }

                    if( isEnabled() && (!isLoadedState()) ) {
                        //We must store config in mConfigsToCommitTable only if port is enabled and component is not in OMX_StateLoaded
                        storeConfigInCommitStructTable((OMX_INDEXTYPE)OMX_IndexConfigAudioEffectRemovePosition, (OMX_INDEXTYPE)0, pConfig->nPositionIndex);
                    }

                    removeEffectNode(pConfig->nPositionIndex);

                }

                return OMX_ErrorNone;
            }
        case OMX_IndexConfigAudioEffectCommit:
            {
                OstTraceFiltInst2(TRACE_DEBUG,"MixerPort::setConfig (OMX_IndexConfigAudioEffectCommit) %d configs must be commited on port %d", mNbConfigsToCommit, getPortIndex());
                return OMX_ErrorNone;
            }
        default:
            return OMX_ErrorUnsupportedIndex;
    }
}

OMX_ERRORTYPE MixerPort::getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const {
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioEffectAddPosition:
        {
            return OMX_ErrorNone; //TODO : to be implemented!
        }
        default:
            return OMX_ErrorUnsupportedIndex;
    }
}

OMX_ERRORTYPE MixerPort::setEffectConfig(OMX_U32 nPositionIndex, OMX_INDEXTYPE nEffectSpecificIndex, OMX_PTR pEffectConfigStructure) {
    OMX_ERRORTYPE error;

    AudioEffect *pEffectNode = getEffectNode(nPositionIndex);
    if(pEffectNode == NULL) {
        OstTraceFiltInst2(TRACE_ERROR,"MixerPort::setEffectConfig : can not find effect at position %d on port Id %d", nPositionIndex, getPortIndex());
        return OMX_ErrorUnsupportedSetting;
    }

    if(nEffectSpecificIndex == (OMX_INDEXTYPE)AFM_IndexConfigPcmProbe) {

        AFM_AUDIO_CONFIG_PCM_PROBE *pNewProbeConfig = static_cast<AFM_AUDIO_CONFIG_PCM_PROBE *>(pEffectConfigStructure);
        AFM_AUDIO_CONFIG_PCM_PROBE *pCurrentProbeConfig = pEffectNode->getProbeConfig();

        if(pCurrentProbeConfig->bEnable == pNewProbeConfig->bEnable) {
            // it seems we're required not to change anything, so do not change...
            OstTraceFiltInst0(TRACE_WARNING, "MixerPort: setEffectConfig (AFM_IndexConfigPcmProbe) -> no change : early exit");
            return OMX_ErrorNone;
        }

        error = pEffectNode->setProbeConfig(pNewProbeConfig);
        if(error != OMX_ErrorNone) return error;

    } else {

        error = pEffectNode->setConfig(nEffectSpecificIndex, pEffectConfigStructure);
        if(error != OMX_ErrorNone) return error;
    }

    if( isEnabled() && (!isLoadedState()) ) {
        //We must store config in mConfigsToCommitTable only if port is enabled and component is not in OMX_StateLoaded
        storeConfigInCommitStructTable((OMX_INDEXTYPE)OMX_IndexConfigAudioEffectConfigure, nEffectSpecificIndex, nPositionIndex);
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerPort::getEffectConfig(OMX_U32 nPositionIndex, OMX_INDEXTYPE nEffectSpecificIndex, OMX_PTR pEffectConfigStructure) {

    AudioEffect *pEffectNode = getEffectNode(nPositionIndex);
    if(pEffectNode == NULL) return OMX_ErrorUnsupportedSetting;

    if(nEffectSpecificIndex == (OMX_INDEXTYPE)AFM_IndexConfigPcmProbe) {
        AFM_AUDIO_CONFIG_PCM_PROBE *pProbeConfig = static_cast<AFM_AUDIO_CONFIG_PCM_PROBE *>(pEffectConfigStructure);
        pProbeConfig->bEnable = pEffectNode->getProbeConfig()->bEnable;
        pProbeConfig->nProbeIdx = pEffectNode->getProbeConfig()->nProbeIdx;
        return OMX_ErrorNone;
    } else {
        return pEffectNode->getConfig(nEffectSpecificIndex, pEffectConfigStructure);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

AudioEffect * MixerPort::getEffectNode(OMX_U32 nPosition) {
    if(!mEffectNodeList.isEmpty()) {
        ENS_ListItem *pEffectItem = mEffectNodeList.getFirst();
        while (pEffectItem != NULL) {
            if (nPosition == static_cast<AudioEffect *>(pEffectItem->getData())->getPosition()) {
                return (static_cast<AudioEffect *>(pEffectItem->getData()));
            }
            pEffectItem = pEffectItem->getNext();
        }
    }
    return NULL;
}

void MixerPort::removeEffectNode(OMX_U32 nPosition) {

    ENS_ListItem *pEffectItem = mEffectNodeList.getFirst();
    while (pEffectItem != NULL) {
        if (nPosition == static_cast<AudioEffect *>(pEffectItem->getData())->getPosition()) {
            break;
        }
        pEffectItem = pEffectItem->getNext();
    }

    DBC_ASSERT(pEffectItem != NULL);

    AudioEffect *pEffectNode = static_cast<AudioEffect *>(pEffectItem->getData());
    OstTraceFiltInst3(TRACE_DEBUG,"MixerPort::removeEffectNode portidx=%d position=%d pEffectNode=0x%x", getPortIndex(), nPosition, (unsigned int)pEffectNode);
    delete pEffectNode;

    if(pEffectItem == mEffectNodeList.getFirst()) {
        mEffectNodeList.popFront();
    }else if (pEffectItem == mEffectNodeList.getLast()) {
        mEffectNodeList.popBack();
    }else {
        ENS_ListItem *pPrevEffectItem = pEffectItem->getPrev();
        ENS_ListItem *pNextEffectItem = pEffectItem->getNext();
        //Remove pEffectItem from the linked list
        pNextEffectItem->setPrev(pPrevEffectItem);
        pPrevEffectItem->setNext(pNextEffectItem);
        //Delete pEffectItem
        delete pEffectItem;
    }

}

void MixerPort::destroyAllEffectsInList(ENS_List *list) {
    if(list->isEmpty()) return;

    ENS_ListItem *pEffectItem = list->getFirst();
    while (pEffectItem != NULL) {
        AudioEffect *pAudioEffect = static_cast<AudioEffect *>(pEffectItem->getData());
        delete pAudioEffect;
        pEffectItem = pEffectItem->getNext();
    }

    list->clear();
}



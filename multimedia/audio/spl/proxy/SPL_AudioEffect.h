/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   SPL_AudioEffect.h
* \brief  proxy part of SPL effect included in Mixer OMX component
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SPL_AUDIO_EFFECT_H_
#define _SPL_AUDIO_EFFECT_H_

#include "AudioEffect.h"
#include "AFM_Types.h"
#include "audio_transducer_chipset_api.h"

/// Concrete class implementing a SPL effect used in OMX Mixer/Splitter component
/// Derived from AudioEffect concrete class
class SPL_AudioEffect: public AudioEffect
{
    public:
        SPL_AudioEffect(const AudioEffectParams &sParams, ENS_Component &enscomp);

        virtual OMX_ERRORTYPE setConfig(OMX_U32 nConfigIndex, OMX_PTR pComponentConfigStructure);
        virtual OMX_ERRORTYPE getConfig(OMX_U32 nConfigIndex, OMX_PTR pComponentConfigStructure) const;

        inline AFM_AUDIO_PARAM_SPLTYPE *getSplParams(void) const {
            return (AFM_AUDIO_PARAM_SPLTYPE *) &mSPLParam;
        }

        inline OMX_AUDIO_CONFIG_SPLLIMITTYPE *getSplTypeConfig(void) const {
            return (OMX_AUDIO_CONFIG_SPLLIMITTYPE *) &mSPLTypeConfig;
        }

        inline OMX_AUDIO_CONFIG_VOLUMETYPE *getSplVolConfig(void) const {
            return (OMX_AUDIO_CONFIG_VOLUMETYPE *) &mSPLVolConfig;
        }

    private:
        AFM_AUDIO_PARAM_SPLTYPE       mSPLParam;
        OMX_AUDIO_CONFIG_SPLLIMITTYPE mSPLTypeConfig;
        OMX_AUDIO_CONFIG_VOLUMETYPE   mSPLVolConfig;
};

#endif //  _SPL_AUDIO_EFFECT_H_

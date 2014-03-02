/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   TEQ_AudioEffect.h
* \brief  proxy part of transducer equalizer effect included in
* Mixer OMX component
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _TEQ_AUDIO_EFFECT_H_
#define _TEQ_AUDIO_EFFECT_H_

#include "AudioEffect.h"

#define TREQ_DEFAULT_NB_CHANNEL         2

/// Concrete class implementing a TEQ effect used in OMX Mixer/Splitter component
/// Derived from AudioEffect concrete class
class TEQ_AudioEffect: public AudioEffect
{
    public:
        TEQ_AudioEffect(const AudioEffectParams &sParams, ENS_Component &enscomp);

        /// Destructor
        virtual ~TEQ_AudioEffect(void);

        virtual OMX_ERRORTYPE setConfig(OMX_U32 nConfigIndex, OMX_PTR pComponentConfigStructure);
        virtual OMX_ERRORTYPE getConfig(OMX_U32 nConfigIndex, OMX_PTR pComponentConfigStructure) const;

        inline AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE * getTeqParams(void) const
        {
            return (AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE *) &mTransducerEqualizerParams;
        }

        inline AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE * getTeqConfig(void) const
        {
            return (AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE *) &mTransducerEqualizerConfig;
        }

        inline int getInstance(void) const
        {
            return instance;
        }

    private:
        static int                               instance;
        AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE  mTransducerEqualizerParams;
        AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE mTransducerEqualizerConfig;
};

#endif //  _TEQ_AUDIO_EFFECT_H_

/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   MDRC_AudioEffect.h
* \brief  proxy part of MDRC effect included in Mixer OMX component
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _MDRC_AUDIO_EFFECT_H_
#define _MDRC_AUDIO_EFFECT_H_

#include "AudioEffect.h"
#include "OMX_Symbian_AudioExt_Ste.h"
#include <host/libeffects/libmdrc5b/include/config.idt.h>

/// Concrete class implementing a MDRC effect used in OMX Mixer/Splitter component
/// Derived from AudioEffect concrete class
class MDRC_AudioEffect: public AudioEffect
{
    public:
        MDRC_AudioEffect(const AudioEffectParams &sParams, ENS_Component &enscomp);

        virtual OMX_ERRORTYPE setConfig(OMX_U32 nConfigIndex, OMX_PTR pComponentConfigStructure);
        virtual OMX_ERRORTYPE getConfig(OMX_U32 nConfigIndex, OMX_PTR pComponentConfigStructure) const;

        inline AFM_AUDIO_PARAM_MDRC * getMdrcParams(void) const{
            return (AFM_AUDIO_PARAM_MDRC *) &mMdrcParams;
        }

        inline AFM_AUDIO_GLOBAL_CONFIG_MDRC * getMdrcGlobalConfig(void) const{
            return (AFM_AUDIO_GLOBAL_CONFIG_MDRC *) &mMdrcGlobalConfig;
        }

        inline OMX_SYMBIAN_AUDIO_CONFIG_MDRCTYPE * getMdrcBandConfig(OMX_U32 countBand) const{
            return (OMX_SYMBIAN_AUDIO_CONFIG_MDRCTYPE *) &mMdrcConfig[countBand];
        }

        inline bool isNewMdrcConfigToApply(void) const { return mNewMdrcConfigToApply; };
        inline void resetNewMdrcConfigToApply(void) { mNewMdrcConfigToApply = false; };

    private:
        AFM_AUDIO_PARAM_MDRC                    mMdrcParams;
        AFM_AUDIO_GLOBAL_CONFIG_MDRC            mMdrcGlobalConfig;
        OMX_SYMBIAN_AUDIO_CONFIG_MDRCTYPE       mMdrcConfig[MDRC_BANDS_MAX];

        //mNewMdrcConfigToApply is necessary when OMX component is in OMX_StateLoaded because proprietary implementation
        //does not support OMX API (AFM_AUDIO_GLOBAL_CONFIG_MDRC/OMX_SYMBIAN_AUDIO_CONFIG_MDRCTYPE) but proprietary API (MdrcConfig_t).
        //At OMX level, several OMX_SetConfig are necessary in order to configure all MDRC bands.
        //Whereas at proprietary implementation level, only 1 setConfig is necessary to configure all MDRC bands.
        bool                                    mNewMdrcConfigToApply;
};

#endif //  _MDRC_AUDIO_EFFECT_H_

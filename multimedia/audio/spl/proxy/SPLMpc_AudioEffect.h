/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   SPLMpc_AudioEffect.h
* \brief  Specific DSP part of spl effect included in
* OMX Mixer/Splitter components
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SPL_MPC_AUDIO_EFFECT_H_
#define _SPL_MPC_AUDIO_EFFECT_H_

#include "AudioEffectMpc.h"

class SPLMpc_AudioEffect : public AudioEffectMpc
{
    public:
        SPLMpc_AudioEffect(AudioEffect &audioEffect, ENS_Component &enscomp): AudioEffectMpc(audioEffect, enscomp) {};

        virtual OMX_ERRORTYPE allocAndFillDspConfigStruct(OMX_U32 nConfigIndex, ARMSpecificEffectConfig_t *pEffectConfig);
};
/// @}
#endif //  _SPL_MPC_AUDIO_EFFECT_H_

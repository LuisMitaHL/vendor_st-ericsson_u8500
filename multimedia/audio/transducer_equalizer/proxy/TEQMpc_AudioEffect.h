/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   TEQMpc_AudioEffect.h
* \brief  Specific DSP part of teq effect included in
* OMX Mixer/Splitter components
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _TEQ_MPC_AUDIO_EFFECT_H_
#define _TEQ_MPC_AUDIO_EFFECT_H_

#include "AudioEffectMpc.h"
#include "TEQMpcConfigs.h"

class TEQMpc_AudioEffect : public AudioEffectMpc
{
    public:
        TEQMpc_AudioEffect(AudioEffect &audioEffect, ENS_Component &enscomp): AudioEffectMpc(audioEffect, enscomp) {};

        virtual OMX_ERRORTYPE allocAndFillDspConfigStruct(OMX_U32 nConfigIndex, ARMSpecificEffectConfig_t *pEffectConfig);

    private:
        OMX_ERRORTYPE fillDSPTEQConfig(ARMTransducerEqualizerConfig_t *pConfigMpc);
};
/// @}
#endif //  _TEQ_MPC_AUDIO_EFFECT_H_

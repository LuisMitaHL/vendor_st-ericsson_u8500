/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   MDRCMpc_AudioEffect.h
* \brief  Specific DSP part of MDRC effect included in OMX Mixer/Splitter components
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _MDRC_MPC_AUDIO_EFFECT_H_
#define _MDRC_MPC_AUDIO_EFFECT_H_

#include "AudioEffectMpc.h"
#include "libeffects/libmdrc5b/include/mdrc5b_filters_design.h"

//#define DEBUG_MDRC_CONFIG_PASSING_FROM_HST_TO_MPC
//#define DEBUG_MDRC_ARM_CONFIG

class MDRCMpc_AudioEffect : public AudioEffectMpc
{
    public:
        MDRCMpc_AudioEffect(AudioEffect &audioEffect, ENS_Component &enscomp): AudioEffectMpc(audioEffect, enscomp) {};

        virtual OMX_ERRORTYPE allocAndFillDspConfigStruct(OMX_U32 nConfigIndex, ARMSpecificEffectConfig_t *pEffectConfig);

    private:
        t_cm_memory_handle allocDspDDR16Memory(OMX_U32 domainId, OMX_U32 sizeBytes, OMX_U32 *configStructArmAddress, OMX_U32 *configStructDspAddress);
        OMX_ERRORTYPE fillDSPMDRCConfig(MdrcConfig_t *pConfigMpc);
#if defined(DEBUG_MDRC_CONFIG_PASSING_FROM_HST_TO_MPC) || defined(DEBUG_MDRC_ARM_CONFIG)
        OMX_ERRORTYPE printMdrcConfigs(MdrcConfig_t *pConfigMpc);
#endif //#if defined(DEBUG_MDRC_CONFIG_PASSING_FROM_HST_TO_MPC) || defined(DEBUG_MDRC_ARM_CONFIG)
};
/// @}
#endif //  _MDRC_MPC_AUDIO_EFFECT_H_

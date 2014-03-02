/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerEffectsRegister.cpp
 * \brief  register OMX Mixer internal effects (spl, treq...)
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include "SPL_AudioEffectCreateMethod.h"
#include "TEQ_AudioEffectCreateMethod.h"
#include "MDRC_AudioEffectCreateMethod.h"

void MixerEffects_Init(void) {
	OMX_ERRORTYPE err;

    AudioEffectsRegister *pAudioEffectsRegister = AudioEffectsRegister::createInstance();

	err = pAudioEffectsRegister->registerEffect(OMX_STRING("audio_processor.pcm.spl"), splCreateMethod);
    DBC_ASSERT(err == OMX_ErrorNone);

	err = pAudioEffectsRegister->registerEffect(OMX_STRING("audio_processor.pcm.trans_equalizer"), teqCreateMethod);
    DBC_ASSERT(err == OMX_ErrorNone);

	err = pAudioEffectsRegister->registerEffect(OMX_STRING("audio_processor.pcm.mdrc"), mdrcCreateMethod);
    DBC_ASSERT(err == OMX_ErrorNone);
}

void MixerEffects_DeInit(void) {
    AudioEffectsRegister::destroyInstance();
}


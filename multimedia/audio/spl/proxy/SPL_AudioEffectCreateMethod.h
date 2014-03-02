/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   SPL_AudioEffectCreateMethod.h
* \brief  createMethod of spl effect included in
* Mixer OMX Component
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SPL_AUDIO_EFFECT_CREATE_METHOD_H_
#define _SPL_AUDIO_EFFECT_CREATE_METHOD_H_

#include "ENS_IOMX.h"
#include "AudioEffect.h"

/// SPL Create method used when registering the SPL AudioEffect to AudioEffectRegister
OMX_ERRORTYPE splCreateMethod(AudioEffect **ppAudioEffect, ENS_Component &enscomp, const AudioEffectParams &sParams);

#endif //  _SPL_AUDIO_EFFECT_CREATE_METHOD_H_

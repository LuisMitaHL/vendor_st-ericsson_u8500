/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   TEQ_AudioEffectCreateMethod.h
* \brief  createMethod of transducer equalizer effect included in
* Mixer OMX component
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _TEQ_AUDIO_EFFECT_CREATE_METHOD_H_
#define _TEQ_AUDIO_EFFECT_CREATE_METHOD_H_

#include "ENS_IOMX.h"
#include "AudioEffect.h"

/// TEQ Create method used when registering the TEQ AudioEffect to AudioEffectRegister
OMX_ERRORTYPE teqCreateMethod(AudioEffect **ppAudioEffect, ENS_Component &enscomp, const AudioEffectParams &sParams);

#endif //  _TEQ_AUDIO_EFFECT_CREATE_METHOD_H_

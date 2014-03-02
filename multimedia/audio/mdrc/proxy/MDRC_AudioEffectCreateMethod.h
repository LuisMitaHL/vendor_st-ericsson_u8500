/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   MDRC_AudioEffectCreateMethod.h
* \brief  createMethod of MDRC effect included in Mixer OMX component
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _MDRC_AUDIO_EFFECT_CREATE_METHOD_H_
#define _MDRC_AUDIO_EFFECT_CREATE_METHOD_H_

#include "ENS_IOMX.h"
#include "AudioEffect.h"

/// MDRC Create method used when registering the MDRC AudioEffect to AudioEffectRegister
OMX_ERRORTYPE mdrcCreateMethod(AudioEffect **ppAudioEffect, ENS_Component &enscomp, const AudioEffectParams &sParams);

#endif //  _MDRC_AUDIO_EFFECT_CREATE_METHOD_H_

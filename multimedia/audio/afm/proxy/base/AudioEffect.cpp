/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AudioEffect.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include "AudioEffect.h"
#include "AudioEffectMpc.h"
#include "ENS_DBC.h"
#include "ENS_String.h"
#include <string.h>
#include <inc/typedef.h>
#include "AFM_PcmPort.h"

#include "OstTraceDefinitions_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_base_AudioEffectTraces.h"
#endif
#include "OpenSystemTrace_ste.h"

/////////////////////////////////////////////////////////////////////////////////////////////
AudioEffectsRegister * AudioEffectsRegister::instance = 0;
int AudioEffectsRegister::nbInstance = 0;

AudioEffectsRegister::AudioEffectsRegister(void) {
    mRegisteredCount = 0;

    for (int i = 0; i < MAX_EFFECTS_REGISTERED; i++) {
        mEntries[i].mName = 0;
        mEntries[i].mCreateMethod = 0;
    }
}

AudioEffectsRegister::~AudioEffectsRegister(void) {
}

AudioEffectsRegister * AudioEffectsRegister::createInstance(void) {
    if (nbInstance == 0) {
        instance = new AudioEffectsRegister;
    }
    nbInstance ++;
    return instance;
}

void AudioEffectsRegister::destroyInstance(void) {
    nbInstance --;
    DBC_ASSERT(nbInstance >= 0);
    if (nbInstance == 0) {
        delete instance;
        instance = 0;
    }
}

AudioEffectsRegister * AudioEffectsRegister::getInstance(void) {
    return instance;
}

OMX_ERRORTYPE AudioEffectsRegister::registerEffect(
        OMX_STRING cEffectName,
        AUDIO_EFFECT_CREATE_METHODTYPE pFactoryMethod) {

    AUDIO_EFFECT_CREATE_METHODTYPE pDummyCreateMethod;
    OMX_ERRORTYPE error;

    DBC_ASSERT(cEffectName!=0);
    DBC_ASSERT(mRegisteredCount<MAX_EFFECTS_REGISTERED);
    DBC_ASSERT(strlen(cEffectName)<OMX_MAX_STRINGNAME_SIZE);

    error = lookupEffect(cEffectName, &pDummyCreateMethod);
    switch (error) {
        case OMX_ErrorNone:
            // component already there ==> no need to re-register
            return OMX_ErrorNone;
        case OMX_ErrorComponentNotFound:
            // component is not found ==> we must register it, so no return now
            break;
        default:
            DBC_ASSERT(0); // an error has occured!
    }

    mEntries[mRegisteredCount].mName = cEffectName;
    mEntries[mRegisteredCount].mCreateMethod = pFactoryMethod;
    mRegisteredCount++;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AudioEffectsRegister::lookupEffect(
        OMX_STRING cEffectName,
        AUDIO_EFFECT_CREATE_METHODTYPE *ppCreateMethod)
{
    DBC_ASSERT(ppCreateMethod!=0);
    DBC_ASSERT(cEffectName!=0);
    DBC_ASSERT(strlen(cEffectName)<OMX_MAX_STRINGNAME_SIZE);

    for (int i = 0; i < mRegisteredCount; i++) {
        if (ENS_String<OMX_MAX_STRINGNAME_SIZE>(cEffectName)
                == (char*)mEntries[i].mName) {
            *ppCreateMethod = mEntries[i].mCreateMethod;
            return OMX_ErrorNone;
        }
    }

    *ppCreateMethod = 0;

    return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE AudioEffectsRegister::createEffect(
        AudioEffect **ppAudioEffect,
        ENS_Component &enscomp,
        const AudioEffectParams &sParams)
{
    AUDIO_EFFECT_CREATE_METHODTYPE pCreateMethod;
    OMX_ERRORTYPE error;

    error = lookupEffect(sParams.cEffectName, &pCreateMethod);
    if(error != OMX_ErrorNone) {
        return error;
    }

    return pCreateMethod(ppAudioEffect, enscomp, sParams);
}

/////////////////////////////////////////////////////////////////////////////////////

AudioEffectParams::AudioEffectParams(
        OMX_U32 nPortIndex,
        OMX_U32 nPosition,
        OMX_STRING cEffectName)
    :
    mPortIndex(nPortIndex),
    mPosition(nPosition),
    cEffectName(cEffectName)
{
}

/////////////////////////////////////////////////////////////////////////////////////

AudioEffect::AudioEffect(const AudioEffectParams &sParams, ENS_Component &enscomp)
    : mENSComponent(enscomp), mNbConfigs(0)
{
    mPortIdx        = sParams.mPortIndex;
    mPosition       = sParams.mPosition;
    DBC_ASSERT(strlen(sParams.cEffectName)<OMX_MAX_STRINGNAME_SIZE);
    strcpy(mName, sParams.cEffectName);

    mCoreSpecificImpl = NULL;
    mConfigIndexTable = NULL;
    mProbeConfig.bEnable = OMX_FALSE; //By default, pcmprobe is disabled

    setTraceInfo(enscomp.getSharedTraceInfoPtr(),mPortIdx);
}

AudioEffect::~AudioEffect(void) {
    if (mCoreSpecificImpl) {
        delete mCoreSpecificImpl;
    }
    if (mConfigIndexTable) {
        delete [] mConfigIndexTable;
    }
}

OMX_ERRORTYPE AudioEffect::setProbeConfig(AFM_AUDIO_CONFIG_PCM_PROBE *pConfig) {

    if(pConfig->nPortIndex != AUDIO_EFFECT_MMPROBE_INDEX) {
        OstTraceFiltInst2(TRACE_ERROR,"AudioEffect::setProbeConfig nPortIndex (%d) not supported, only %d is supported", pConfig->nPortIndex, AUDIO_EFFECT_MMPROBE_INDEX);
        return OMX_ErrorBadParameter;
    }

    mProbeConfig = *pConfig;

    return OMX_ErrorNone;
}

OMX_U32 AudioEffect::getNbChannels(void) const
{
    AFM_PcmPort *pPcmPort  = static_cast<AFM_PcmPort *>(mENSComponent.getPort(mPortIdx));
    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings  = pPcmPort ->getPcmSettings();

    return pcmSettings.nChannels;
}

OMX_U32 AudioEffect::getSamplingRate(void) const
{
    AFM_PcmPort *pPcmPort  = static_cast<AFM_PcmPort *>(mENSComponent.getPort(mPortIdx));
    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings  = pPcmPort ->getPcmSettings();

    return pcmSettings.nSamplingRate;
}


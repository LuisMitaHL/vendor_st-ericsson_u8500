/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerInputPort.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "MixerInputPort.h"
#include "audio_chipset_api_index.h"
#include "libeffects/include/volctrl_defines.h"


MixerInputPort::MixerInputPort(
        OMX_U32 nIndex,
        ENS_Component &enscomp)
    : MixerPort(nIndex, OMX_DirInput, enscomp), mIsPaused(OMX_FALSE)
{
    mSrcMode            = AFM_SRC_MODE_NORMAL;

    setDefaultSettings();
}

void MixerInputPort::setDefaultSettings(void)
{
    // Mute configuration
    mConfigMute.nSize = sizeof(OMX_AUDIO_CONFIG_MUTETYPE);
    getOmxIlSpecVersion(&mConfigMute.nVersion);
    mConfigMute.nPortIndex = getPortIndex();
    mConfigMute.bMute = OMX_FALSE;

    // Balance Configuration
    mConfigBalance.nSize = sizeof(OMX_AUDIO_CONFIG_BALANCETYPE);
    getOmxIlSpecVersion(&mConfigBalance.nVersion);
    mConfigBalance.nPortIndex = getPortIndex();
    mConfigBalance.nBalance = VOLCTRL_BALANCE_CENTER;

    // Volume Configuration
    mConfigVolume.nSize = sizeof(OMX_AUDIO_CONFIG_VOLUMETYPE);
    getOmxIlSpecVersion(&mConfigVolume.nVersion);
    mConfigVolume.nPortIndex = getPortIndex();
    mConfigVolume.bLinear  = OMX_FALSE;
    mConfigVolume.sVolume.nValue = VOLCTRL_VOLUME_MAX;
    mConfigVolume.sVolume.nMin   = VOLCTRL_VOLUME_MIN;
    mConfigVolume.sVolume.nMax   = VOLCTRL_VOLUME_MAX;

    // VolumeRamp Configuration
    mConfigVolumeRamp.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE);
    getOmxIlSpecVersion(&mConfigVolumeRamp.nVersion);
    mConfigVolumeRamp.nPortIndex = getPortIndex();
    mConfigVolumeRamp.nChannel = 2; //Default is stereo
    mConfigVolumeRamp.bLinear  = OMX_FALSE;

    mConfigVolumeRamp.sStartVolume.nValue = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.sStartVolume.nMin   = VOLCTRL_VOLUME_MIN;
    mConfigVolumeRamp.sStartVolume.nMax   = VOLCTRL_VOLUME_MAX;

    mConfigVolumeRamp.sEndVolume.nValue   = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.sEndVolume.nMin     = VOLCTRL_VOLUME_MIN;
    mConfigVolumeRamp.sEndVolume.nMax     = VOLCTRL_VOLUME_MAX;

    mConfigVolumeRamp.sCurrentVolume.nValue = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.sCurrentVolume.nMin   = VOLCTRL_VOLUME_MIN;
    mConfigVolumeRamp.sCurrentVolume.nMax   = VOLCTRL_VOLUME_MAX;

    mConfigVolumeRamp.nVolumeStep      = 0; //TODO ?

    mConfigVolumeRamp.nRampCurrentTime = 0;
    mConfigVolumeRamp.nRampDuration    = 0;
    mConfigVolumeRamp.nRampMinDuration = VOLCTRL_RAMPDURATION_MIN;
    mConfigVolumeRamp.nRampMaxDuration = VOLCTRL_RAMPDURATION_MAX;

    mConfigVolumeRamp.bRampTerminate   = OMX_FALSE;

    mIsRampVolume = OMX_FALSE;
}

OMX_ERRORTYPE MixerInputPort::setParameter(
	OMX_INDEXTYPE nParamIndex,
	OMX_PTR pComponentParameterStructure)
{
	switch (nParamIndex) {
        case AFM_IndexParamSrcMode:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,AFM_AUDIO_PARAM_SRC_MODE);
            AFM_AUDIO_PARAM_SRC_MODE *srcMode = (AFM_AUDIO_PARAM_SRC_MODE *)pComponentParameterStructure;

            mSrcMode= srcMode->nSrcMode;

            return OMX_ErrorNone;
        }
        default:
            return AFM_PcmPort::setParameter(
                       nParamIndex, pComponentParameterStructure);
	}
}

OMX_ERRORTYPE MixerInputPort::getParameter(
	OMX_INDEXTYPE nParamIndex,
	OMX_PTR pComponentParameterStructure) const
{
	switch (nParamIndex) {
          case AFM_IndexParamSrcMode:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,AFM_AUDIO_PARAM_SRC_MODE);
            AFM_AUDIO_PARAM_SRC_MODE *srcMode = (AFM_AUDIO_PARAM_SRC_MODE *)pComponentParameterStructure;

            srcMode->nSrcMode = mSrcMode;

            return OMX_ErrorNone;
        }
        default:
            return AFM_PcmPort::getParameter(
                nParamIndex, pComponentParameterStructure);
	}
}

OMX_ERRORTYPE MixerInputPort::setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) {
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioVolume:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_VOLUMETYPE);
                OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume = (OMX_AUDIO_CONFIG_VOLUMETYPE *) pComponentConfigStructure;

                if (pVolume->bLinear != OMX_FALSE) {
                    return OMX_ErrorBadParameter;
                }

                // check range; if out of range take min / max value (dont fail)
                if (pVolume->sVolume.nValue < mConfigVolume.sVolume.nMin) {
                    pVolume->sVolume.nValue = mConfigVolume.sVolume.nMin;
                } else if (pVolume->sVolume.nValue > mConfigVolume.sVolume.nMax) {
                    pVolume->sVolume.nValue = mConfigVolume.sVolume.nMax;
                }

                // update value of interest
                mConfigVolume.sVolume.nValue = pVolume->sVolume.nValue;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioMute:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_MUTETYPE);
                OMX_AUDIO_CONFIG_MUTETYPE * pMute = (OMX_AUDIO_CONFIG_MUTETYPE *) pComponentConfigStructure;

                // update value of interest
                mConfigMute.bMute = pMute->bMute;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioBalance:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_BALANCETYPE);
                OMX_AUDIO_CONFIG_BALANCETYPE * pBalance = (OMX_AUDIO_CONFIG_BALANCETYPE *) pComponentConfigStructure;

                // check range; if out of range take left / right value (dont fail)
                if (pBalance->nBalance < VOLCTRL_BALANCE_ALLLEFT) {
                    pBalance->nBalance = VOLCTRL_BALANCE_ALLLEFT;
                } else if (pBalance->nBalance > VOLCTRL_BALANCE_ALLRIGHT) {
                    pBalance->nBalance = VOLCTRL_BALANCE_ALLRIGHT;
                }

                // update value of interest
                mConfigBalance.nBalance = pBalance->nBalance;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioVolumeRamp:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE);
                OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume =
                    (OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *)pComponentConfigStructure;

                if (pRampVolume->bLinear != OMX_FALSE) {
                    return OMX_ErrorBadParameter;
                }

                // check range; if out of range take min / max value (dont fail)
                // TODO(pht) OST traces ?
                if(pRampVolume->sStartVolume.nValue > mConfigVolumeRamp.sStartVolume.nMax){
                    pRampVolume->sStartVolume.nValue = mConfigVolumeRamp.sStartVolume.nMax;
                }
                if(pRampVolume->sStartVolume.nValue < mConfigVolumeRamp.sStartVolume.nMin){
                    pRampVolume->sStartVolume.nValue = mConfigVolumeRamp.sStartVolume.nMin;
                }

                if(pRampVolume->sEndVolume.nValue > mConfigVolumeRamp.sEndVolume.nMax){
                    pRampVolume->sEndVolume.nValue = mConfigVolumeRamp.sEndVolume.nMax;
                }
                if(pRampVolume->sEndVolume.nValue < mConfigVolumeRamp.sEndVolume.nMin){
                    pRampVolume->sEndVolume.nValue = mConfigVolumeRamp.sEndVolume.nMin;
                }

                if((pRampVolume->nRampDuration == 0 && (pRampVolume->bRampTerminate!=OMX_TRUE) )) {
                    // TODO(pht) as per spec this indicates immediate volume application; check if need to support this
                    return OMX_ErrorNotImplemented;
                } else {
                    if(pRampVolume->nRampDuration > mConfigVolumeRamp.nRampMaxDuration){
                        pRampVolume->nRampDuration = mConfigVolumeRamp.nRampMaxDuration;
                    }
                    if(pRampVolume->nRampDuration < mConfigVolumeRamp.nRampMinDuration){
                        pRampVolume->nRampDuration = mConfigVolumeRamp.nRampMinDuration;
                    }
                }

                // update values of interest
                mConfigVolumeRamp.nChannel              = pRampVolume->nChannel;
                mConfigVolumeRamp.bLinear               = pRampVolume->bLinear;
                mConfigVolumeRamp.sStartVolume.nValue   = pRampVolume->sStartVolume.nValue;
                mConfigVolumeRamp.sEndVolume.nValue     = pRampVolume->sEndVolume.nValue;
                mConfigVolumeRamp.nRampDuration         = pRampVolume->nRampDuration;
                mConfigVolumeRamp.bRampTerminate        = pRampVolume->bRampTerminate;

                mIsRampVolume = OMX_TRUE;

                return OMX_ErrorNone;
            }

        default:
            return MixerPort::setConfig(nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE MixerInputPort::getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const {
    switch (nConfigIndex) {

        case OMX_IndexConfigAudioVolume:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_VOLUMETYPE);
                OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume = (OMX_AUDIO_CONFIG_VOLUMETYPE *) pComponentConfigStructure;

                if (pVolume->bLinear != OMX_FALSE) {
                    return OMX_ErrorBadParameter;
                }

                pVolume->sVolume = mConfigVolume.sVolume;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioMute:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_MUTETYPE);
                OMX_AUDIO_CONFIG_MUTETYPE * pMute = (OMX_AUDIO_CONFIG_MUTETYPE *) pComponentConfigStructure;

                pMute->bMute = mConfigMute.bMute;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioBalance:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_BALANCETYPE);
                OMX_AUDIO_CONFIG_BALANCETYPE * pBalance = (OMX_AUDIO_CONFIG_BALANCETYPE *) pComponentConfigStructure;

                pBalance->nBalance = mConfigBalance.nBalance;

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioVolumeRamp:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE);
                OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE*pRampVolume = (OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE*)pComponentConfigStructure;

                // We force the client to use non linear mode
                pRampVolume->bLinear = OMX_FALSE;

                pRampVolume->nChannel = mConfigVolumeRamp.nChannel;

                pRampVolume->sStartVolume.nValue = mConfigVolumeRamp.sStartVolume.nValue;
                pRampVolume->sStartVolume.nMin = mConfigVolumeRamp.sStartVolume.nMin;
                pRampVolume->sStartVolume.nMax = mConfigVolumeRamp.sStartVolume.nMax;

                pRampVolume->sEndVolume.nValue = mConfigVolumeRamp.sEndVolume.nValue;
                pRampVolume->sEndVolume.nMin = mConfigVolumeRamp.sEndVolume.nMin;
                pRampVolume->sEndVolume.nMax = mConfigVolumeRamp.sEndVolume.nMax;

                pRampVolume->sCurrentVolume.nValue = mConfigVolumeRamp.sCurrentVolume.nValue;
                pRampVolume->sCurrentVolume.nMin = mConfigVolumeRamp.sCurrentVolume.nMin;
                pRampVolume->sCurrentVolume.nMax = mConfigVolumeRamp.sCurrentVolume.nMax;

                pRampVolume->nVolumeStep = mConfigVolumeRamp.nVolumeStep;

                pRampVolume->nRampCurrentTime = mConfigVolumeRamp.nRampCurrentTime;
                pRampVolume->nRampDuration = mConfigVolumeRamp.nRampDuration;
                pRampVolume->nRampMinDuration = mConfigVolumeRamp.nRampMinDuration;
                pRampVolume->nRampMaxDuration = mConfigVolumeRamp.nRampMaxDuration;

                pRampVolume->bRampTerminate = mConfigVolumeRamp.bRampTerminate;

                return OMX_ErrorNone;
            }

        default:
            return MixerPort::getConfig(nConfigIndex, pComponentConfigStructure);
    }
}


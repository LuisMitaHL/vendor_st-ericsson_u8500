/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   Volume pcm processor proxy
 * \author  ST-Ericsson
 */
/*****************************************************************************/
#include "Volume_PcmPort.h"
#include "VolumePcmProcessing.h"
#include "NmfHostVolCtrl_PcmProcessing.h"


OMX_ERRORTYPE volPcmProcFactoryMethod(ENS_Component_p * ppENSComponent)
{
    OMX_ERRORTYPE error  = OMX_ErrorNone;

    VolumePcmProcessing *pcmProc = new VolumePcmProcessing();
    if (pcmProc == 0) return OMX_ErrorInsufficientResources;

    error = pcmProc->construct();

    if (error != OMX_ErrorNone) {
        delete pcmProc;
        *ppENSComponent = 0;
        return error;
    }

    VolumeNmfHost_PcmProcessing *pcmProcProcessingComp = new VolumeNmfHost_PcmProcessing(*pcmProc);

    if (pcmProcProcessingComp == 0) {
        delete pcmProc;
        return OMX_ErrorInsufficientResources;
    }

    pcmProc->setProcessingComponent(pcmProcProcessingComp);

    *ppENSComponent = pcmProc;

    return error;
}


OMX_ERRORTYPE  VolumePcmProcessing::construct()
{
    OMX_ERRORTYPE error;
    OMX_AUDIO_PARAM_PCMMODETYPE defaultPcmSettingsIn;
    OMX_AUDIO_PARAM_PCMMODETYPE defaultPcmSettingsOut;

    defaultPcmSettingsIn.nPortIndex     = 0;
    defaultPcmSettingsIn.eNumData       = OMX_NumericalDataSigned;
    defaultPcmSettingsIn.eEndian        = OMX_EndianLittle;
    defaultPcmSettingsIn.bInterleaved   = OMX_TRUE;
    defaultPcmSettingsIn.ePCMMode       = OMX_AUDIO_PCMModeLinear;
    defaultPcmSettingsIn.nBitPerSample  = 16;
    defaultPcmSettingsIn.nChannels      = 8;
    defaultPcmSettingsIn.nSamplingRate  = 48000;
    defaultPcmSettingsIn.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
    defaultPcmSettingsIn.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
    defaultPcmSettingsIn.eChannelMapping[2] = OMX_AUDIO_ChannelCF;
    defaultPcmSettingsIn.eChannelMapping[3] = OMX_AUDIO_ChannelLFE;
    defaultPcmSettingsIn.eChannelMapping[4] = OMX_AUDIO_ChannelLR;
    defaultPcmSettingsIn.eChannelMapping[5] = OMX_AUDIO_ChannelRR;
    defaultPcmSettingsIn.eChannelMapping[6] = OMX_AUDIO_ChannelLS;
    defaultPcmSettingsIn.eChannelMapping[7] = OMX_AUDIO_ChannelRS;

    defaultPcmSettingsOut.nPortIndex     = 1;
    defaultPcmSettingsOut.eNumData       = OMX_NumericalDataSigned;
    defaultPcmSettingsOut.eEndian        = OMX_EndianLittle;
    defaultPcmSettingsOut.bInterleaved   = OMX_TRUE;
    defaultPcmSettingsOut.ePCMMode       = OMX_AUDIO_PCMModeLinear;
    defaultPcmSettingsOut.nBitPerSample  = 16;
    defaultPcmSettingsOut.nChannels      = 8;
    defaultPcmSettingsOut.nSamplingRate  = 48000;
    defaultPcmSettingsOut.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
    defaultPcmSettingsOut.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
    defaultPcmSettingsOut.eChannelMapping[2] = OMX_AUDIO_ChannelCF;
    defaultPcmSettingsOut.eChannelMapping[3] = OMX_AUDIO_ChannelLFE;
    defaultPcmSettingsOut.eChannelMapping[4] = OMX_AUDIO_ChannelLR;
    defaultPcmSettingsOut.eChannelMapping[5] = OMX_AUDIO_ChannelRR;
    defaultPcmSettingsOut.eChannelMapping[6] = OMX_AUDIO_ChannelLS;
    defaultPcmSettingsOut.eChannelMapping[7] = OMX_AUDIO_ChannelRS;


    error = AFM_PcmProcessing::construct();
    if (error != OMX_ErrorNone) return error;

    //Create i/p PCM port
    //Input port needs to be specific because of slaving behavior of PCM settings (ex. similar sample rate, number of channels)
    ENS_Port * port = new Volume_PcmPort( 0, getBlockSize() * defaultPcmSettingsIn.nChannels * defaultPcmSettingsIn.nBitPerSample/8, defaultPcmSettingsIn, *this);
    if (port == 0) return OMX_ErrorInsufficientResources;

    addPort(port);

    //create o/p PCM port
    error = createPcmPort(1, OMX_DirOutput, getBlockSize() * defaultPcmSettingsOut.nChannels * defaultPcmSettingsOut.nBitPerSample/8, defaultPcmSettingsOut);
    if (error != OMX_ErrorNone) return error;

    // Set standard tunneling...
    getPort(0)->forceStandardTunneling();
    getPort(1)->forceStandardTunneling();

    // Set volume default values...

    // Mute configuration
    mConfigMute.nSize = sizeof(OMX_AUDIO_CONFIG_MUTETYPE);
    getOmxIlSpecVersion(&mConfigMute.nVersion);
    mConfigMute.nPortIndex = 0;
    mConfigMute.bMute = OMX_FALSE;

    // Balance Configuration
    mConfigBalance.nSize = sizeof(OMX_AUDIO_CONFIG_BALANCETYPE);
    getOmxIlSpecVersion(&mConfigBalance.nVersion);
    mConfigBalance.nPortIndex = 0;
    mConfigBalance.nBalance = VOLCTRL_BALANCE_CENTER;

    // Volume Configuration
    mConfigVolume.nSize = sizeof(OMX_AUDIO_CONFIG_VOLUMETYPE);
    getOmxIlSpecVersion(&mConfigVolume.nVersion);
    mConfigVolume.nPortIndex = 0;
    mConfigVolume.bLinear  = OMX_FALSE;
    mConfigVolume.sVolume.nValue = VOLCTRL_VOLUME_MAX;
    mConfigVolume.sVolume.nMin   = VOLCTRL_VOLUME_MIN;
    mConfigVolume.sVolume.nMax   = VOLCTRL_VOLUME_MAX;

    // VolumeRamp Configuration
    mConfigVolumeRamp.nSize = sizeof(OMX_AUDIO_CONFIG_VOLUMERAMPTYPE);
    getOmxIlSpecVersion(&mConfigVolumeRamp.nVersion);
    mConfigVolumeRamp.nPortIndex            = 0;
    mConfigVolumeRamp.nChannel              = 2; //Default is stereo
    mConfigVolumeRamp.bLinear               = OMX_FALSE;
    mConfigVolumeRamp.sStartVolume.nValue   = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.sStartVolume.nMin     = VOLCTRL_VOLUME_MIN;
    mConfigVolumeRamp.sStartVolume.nMax     = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.sEndVolume.nValue     = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.sEndVolume.nMin       = VOLCTRL_VOLUME_MIN;
    mConfigVolumeRamp.sEndVolume.nMax       = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.nRampDuration.nValue  = 0;
    mConfigVolumeRamp.nRampDuration.nMin    = 1000;
    mConfigVolumeRamp.nRampDuration.nMax    = 1000000;
    mConfigVolumeRamp.bRampTerminate        = OMX_FALSE;
    mConfigVolumeRamp.sCurrentVolume.nValue = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.sCurrentVolume.nMin   = VOLCTRL_VOLUME_MIN;
    mConfigVolumeRamp.sCurrentVolume.nMax   = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.nRampCurrentTime      = 0;
    
    return OMX_ErrorNone;
}


OMX_ERRORTYPE VolumePcmProcessing::setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex)
    {
        case OMX_IndexConfigAudioVolume:
        {
            OMX_AUDIO_CONFIG_VOLUMETYPE *pVolume = (OMX_AUDIO_CONFIG_VOLUMETYPE *)pComponentConfigStructure;

            if(pVolume->nPortIndex > getPortCount()) {
                return OMX_ErrorBadPortIndex;
            }

            if(pVolume->bLinear != OMX_FALSE) {
                return OMX_ErrorBadParameter;
            }

            if(pVolume->sVolume.nValue < VOLCTRL_VOLUME_MIN || pVolume->sVolume.nValue > VOLCTRL_VOLUME_MAX) {
                return OMX_ErrorBadParameter;
            }

            mConfigVolume = *pVolume;

            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioMute:
        {
            OMX_AUDIO_CONFIG_MUTETYPE *pMute = (OMX_AUDIO_CONFIG_MUTETYPE *)pComponentConfigStructure;

            if (pMute->nPortIndex > getPortCount()) {
                return OMX_ErrorBadPortIndex;
            }

            mConfigMute = *pMute;

            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioBalance:
        {
            OMX_AUDIO_CONFIG_BALANCETYPE *pBalance = (OMX_AUDIO_CONFIG_BALANCETYPE *)pComponentConfigStructure;

            if (pBalance->nPortIndex > getPortCount()) {
                return OMX_ErrorBadPortIndex;
            }

            if (pBalance->nBalance < VOLCTRL_BALANCE_ALLLEFT || pBalance->nBalance > VOLCTRL_BALANCE_ALLRIGHT) {
                return OMX_ErrorBadParameter;
            }

            if (pBalance->nBalance < VOLCTRL_BALANCE_ALLLEFT || pBalance->nBalance > VOLCTRL_BALANCE_ALLRIGHT) {
                return OMX_ErrorBadParameter;
            }

            mConfigBalance = *pBalance;

            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioVolumeRamp:
        {
            OMX_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume = (OMX_AUDIO_CONFIG_VOLUMERAMPTYPE*)pComponentConfigStructure;

            if (pRampVolume->nPortIndex > getPortCount()) {
                return OMX_ErrorBadPortIndex;
            }       

            if(pRampVolume->sStartVolume.nValue < VOLCTRL_VOLUME_MIN || pRampVolume->sStartVolume.nValue > VOLCTRL_VOLUME_MAX) {
                return OMX_ErrorBadParameter;
            }

            if(pRampVolume->sEndVolume.nValue < VOLCTRL_VOLUME_MIN || pRampVolume->sEndVolume.nValue > VOLCTRL_VOLUME_MAX) {
                return OMX_ErrorBadParameter;
            }

            if(pRampVolume->nRampDuration.nValue < 1000 || pRampVolume->nRampDuration.nValue > 1000000) {
                return OMX_ErrorBadParameter;
            }

            mRampRunning = OMX_TRUE;
            mConfigVolumeRamp.nChannel = pRampVolume->nChannel;
            mConfigVolumeRamp.bLinear  = pRampVolume->bLinear;
            mConfigVolumeRamp.sStartVolume.nValue  = pRampVolume->sStartVolume.nValue;
            mConfigVolumeRamp.sEndVolume.nValue    = pRampVolume->sEndVolume.nValue;
            mConfigVolumeRamp.nRampDuration.nValue = pRampVolume->nRampDuration.nValue;
            mConfigVolumeRamp.bRampTerminate       = pRampVolume->bRampTerminate;

            return OMX_ErrorNone;
        }

        default:
            return AFM_PcmProcessing::setConfig(nConfigIndex, pComponentConfigStructure);
    }
}


OMX_ERRORTYPE VolumePcmProcessing::getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const
{
    switch (nConfigIndex)
    {
        case OMX_IndexConfigAudioVolume:
        {
            OMX_AUDIO_CONFIG_VOLUMETYPE *pVolume = (OMX_AUDIO_CONFIG_VOLUMETYPE *)pComponentConfigStructure;

            if(pVolume->nPortIndex > getPortCount()) {
                return OMX_ErrorBadPortIndex;
            }

            *pVolume = mConfigVolume;

            return OMX_ErrorNone;
        }
        
        case OMX_IndexConfigAudioMute:
        {
            OMX_AUDIO_CONFIG_MUTETYPE *pMute = (OMX_AUDIO_CONFIG_MUTETYPE *)pComponentConfigStructure;

            if(pMute->nPortIndex > getPortCount()) {
                return OMX_ErrorBadPortIndex;
            }

            *pMute = mConfigMute;

            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioBalance:
        {
            OMX_AUDIO_CONFIG_BALANCETYPE *pBalance = (OMX_AUDIO_CONFIG_BALANCETYPE *)pComponentConfigStructure;

            if (pBalance->nPortIndex > getPortCount()) {
                return OMX_ErrorBadPortIndex;
            }

            *pBalance = mConfigBalance;

            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioVolumeRamp:
        {
            OMX_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume = (OMX_AUDIO_CONFIG_VOLUMERAMPTYPE*)pComponentConfigStructure;

            if (pRampVolume->nPortIndex > getPortCount()) {
                return OMX_ErrorBadPortIndex;
            }

            *pRampVolume = mConfigVolumeRamp;

            return OMX_ErrorNone; 
        }

        default:
            return AFM_PcmProcessing::getConfig(nConfigIndex, pComponentConfigStructure);
    }
}

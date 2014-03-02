/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   VolCtrl Host  nmf processing class
 * \author  ST-Ericsson
 */
/*****************************************************************************/
#include "AFM_PcmPort.h"
#include "VolumePcmProcessing.h"
#include "NmfHostVolCtrl_PcmProcessing.h"



inline OMX_ERRORTYPE VolumeNmfHost_PcmProcessing::construct(void) {
    return (AFMNmfHost_PcmProcessing::construct());
}


inline OMX_ERRORTYPE VolumeNmfHost_PcmProcessing::destroy(void) {
    return (AFMNmfHost_PcmProcessing::destroy());
}


OMX_ERRORTYPE VolumeNmfHost_PcmProcessing::instantiateAlgo(void)
{
    t_nmf_error   nmf_error;

    mNmfil = (NMF::Composite*)volctrl_nmfil_host_effectWrappedCreate();
    if (((NMF::Composite*)mNmfil) == NULL) { return OMX_ErrorInsufficientResources; };
    (static_cast<volctrl_nmfil_host_effectWrapped*>(mNmfil))->priority=getPriorityLevel();
    if (((NMF::Composite*)mNmfil)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = ((NMF::Composite*)mNmfil)->bindFromUser("configure", 1, &mIConfig);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE VolumeNmfHost_PcmProcessing::deInstantiateAlgo(void) 
{

    t_nmf_error   nmf_error;
  
    nmf_error = ((NMF::Composite*)mNmfil)->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
    
    if (((NMF::Composite*)mNmfil)->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    volctrl_nmfil_host_effectWrappedDestroy((volctrl_nmfil_host_effectWrapped*&)mNmfil);

    return OMX_ErrorNone;
}


inline OMX_ERRORTYPE VolumeNmfHost_PcmProcessing::startAlgo(void) {
    ((NMF::Composite*)mNmfil)->start();
    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE VolumeNmfHost_PcmProcessing::stopAlgo(void) {
    ((NMF::Composite*)mNmfil)->stop();
    return OMX_ErrorNone;
}


OMX_ERRORTYPE VolumeNmfHost_PcmProcessing::configureAlgo(void) {
  
  applyVolumeConfig();
  return OMX_ErrorNone;
}


OMX_ERRORTYPE VolumeNmfHost_PcmProcessing::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) 
{ 

    switch (nConfigIndex)
    {
        case OMX_IndexConfigAudioVolume:
        {
            /*OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume = static_cast<OMX_AUDIO_CONFIG_VOLUMETYPE *>(pComponentConfigStructure);

            if(pVolume->nPortIndex != mPortIndex){
                return OMX_ErrorBadParameter;
                
            }*/
            //mIsRampRunning = OMX_FALSE; // Check SHAI spec!!!!
            //mConfigVolume  = *pVolume;
            break;
        }
    
        case OMX_IndexConfigAudioMute:
        {
            /*OMX_AUDIO_CONFIG_MUTETYPE * pMute = static_cast<OMX_AUDIO_CONFIG_MUTETYPE *>(pComponentConfigStructure);

            if(pMute->nPortIndex != mPortIndex){
                return OMX_ErrorBadParameter;
                
            }
            mConfigMute = *pMute;*/
            break;
        }

        case OMX_IndexConfigAudioBalance:
        {
            /*OMX_AUDIO_CONFIG_BALANCETYPE * pBalance = static_cast<OMX_AUDIO_CONFIG_BALANCETYPE *>(pComponentConfigStructure);

            if(pBalance->nPortIndex != mPortIndex){
                return OMX_ErrorBadParameter;
                
            }
            mConfigBalance = *pBalance;*/
            break;
        }

        case OMX_IndexConfigAudioVolumeRamp:
        {
            /*OMX_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume = static_cast<OMX_AUDIO_CONFIG_VOLUMERAMPTYPE *>(pComponentConfigStructure);

            if(pRampVolume->nPortIndex != mPortIndex){
                return OMX_ErrorBadParameter;
                
            }*/
            mIsRampRunning    = OMX_TRUE;
            //mConfigVolumeRamp = *pRampVolume;
            break;
        }

        default:
            return OMX_ErrorBadParameter;

    }

    applyVolumeConfig();

    return OMX_ErrorNone; 
}


void VolumeNmfHost_PcmProcessing::applyVolumeConfig(void)
{
    VolctrlConfig_t      volctrlConfig = {0};
    VolctrlRampConfig_t  volctrlRampConfig = {0};

    VolumePcmProcessing *proxy = static_cast<VolumePcmProcessing*>(&mENSComponent);

    AFM_PcmPort *portOut = static_cast<AFM_PcmPort *>(mENSComponent.getPort(1));
    AFM_PcmPort *portIn  = static_cast<AFM_PcmPort *>(mENSComponent.getPort(0));

    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettingsOut = portOut->getPcmSettings();
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettingsIn  = portIn->getPcmSettings();

    OMX_BOOL downmix      = OMX_FALSE;
    OMX_BOOL multichannel = OMX_FALSE;

    if (pcmSettingsIn.nChannels > pcmSettingsOut.nChannels) {
        downmix = OMX_TRUE;
    }

    if (pcmSettingsIn.nChannels > 2) {
        multichannel = OMX_TRUE;
    }

    mIsRampRunning    = proxy->getRampRunning();
    mConfigVolume     = proxy->getConfigVolume();
    mConfigMute       = proxy->getConfigMute();
    mConfigBalance    = proxy->getConfigBalance();
    mConfigVolumeRamp = proxy->getConfigVolumeRamp();


    if(multichannel == OMX_TRUE)
    {
        for (int i=0; i < VOLCTRL_MAX_VOLUME_GAINS; i++) {
            // update direct gains from master volume (volume is in millibel, gains are in decibel)
            // deliver to the algorithm in Q8 format to the algorithm (dB)
            volctrlConfig.igains[i] = (128 + ((mConfigMute->bMute == OMX_TRUE ? (VOLCTRL_VOLUME_MUTE) : (mConfigVolume->sVolume.nValue) ) ) / 100 ) << 8;
        }
    }
    else
    {
        // update direct gains from master volume and balance (volume is in millibel, gains are in decibel)
        OMX_S32 leftVolumedB  = (mConfigBalance->nBalance <= 0 ? (mConfigVolume->sVolume.nValue) : (mConfigVolume->sVolume.nValue + mConfigBalance->nBalance*(VOLCTRL_VOLUME_MIN - mConfigVolume->sVolume.nValue)/VOLCTRL_BALANCE_ALLRIGHT)) / 100;
        OMX_S32 rightVolumedB = (mConfigBalance->nBalance <= 0 ? (mConfigVolume->sVolume.nValue + mConfigBalance->nBalance*(VOLCTRL_VOLUME_MIN - mConfigVolume->sVolume.nValue)/VOLCTRL_BALANCE_ALLLEFT) : (mConfigVolume->sVolume.nValue)) / 100;

        if(mConfigMute->bMute == OMX_TRUE)
        {
            leftVolumedB  = VOLCTRL_VOLUME_MUTE/100;
            rightVolumedB = VOLCTRL_VOLUME_MUTE/100;
        }

        volctrlConfig.igll = (128 + leftVolumedB) << 8;
        volctrlConfig.iglr =  0;
        volctrlConfig.igrl =  0;
        volctrlConfig.igrr = (128 + rightVolumedB) << 8;
    }

    volctrlConfig.iDBRamp = 1;  // Ramp in dB
    volctrlConfig.ialpha  = 0;  // Ramp duration

    volctrlConfig.iDownMix      = downmix;
    volctrlConfig.iMultichannel = multichannel;

    mIConfig.setConfig(volctrlConfig);

    if (mIsRampRunning == OMX_TRUE)
    {
        // Gains in the volume ramp are delivered in Q8 format to the algorithm
        // Ramp Duration is given by 100*msec = 100 * x[us] /1000 [us]
        volctrlRampConfig.iStartVolume = (128 + mConfigVolumeRamp->sStartVolume.nValue) << 8;
        volctrlRampConfig.iEndVolume   = (128 + mConfigVolumeRamp->sEndVolume.nValue) << 8;
        volctrlRampConfig.ialpha       = mConfigVolumeRamp->nRampDuration.nValue / 10;
        volctrlRampConfig.iDBRamp      = 1;
        volctrlRampConfig.iChannel     = mConfigVolumeRamp->nChannel;
        volctrlRampConfig.iTerminate   = mConfigVolumeRamp->bRampTerminate;

        proxy->setRampRunning(OMX_FALSE);

        mIConfig.setRampConfig(volctrlRampConfig);
    }
}




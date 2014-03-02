/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfHostVolCtrl.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "NmfHostVolCtrl.h"



NmfHostVolCtrl::NmfHostVolCtrl(OMX_BOOL bDownmix,
                               OMX_BOOL bMultichannel,
                               //coverity[pass_by_value]
                               t_host_pcmprocessing_config config,
                               OMX_U32 portIndex) :NmfHostPcmProcessing(config)
{
    mPortIndex = portIndex;

    mDownmix       = bDownmix;
    mMultichannel  = bMultichannel;
    mIsRampRunning = OMX_FALSE;

    // Mute configuration
    mConfigMute.nSize = sizeof(OMX_AUDIO_CONFIG_MUTETYPE);
    //getOmxIlSpecVersion(&mConfigMute.nVersion);
    mConfigMute.nPortIndex = portIndex;
    mConfigMute.bMute = OMX_FALSE;
    
    // Balance Configuration
    mConfigBalance.nSize = sizeof(OMX_AUDIO_CONFIG_BALANCETYPE);
    //getOmxIlSpecVersion(&mConfigBalance.nVersion);
    mConfigBalance.nPortIndex = portIndex;
    mConfigBalance.nBalance   = VOLCTRL_BALANCE_CENTER;
    
    // Volume Configuration
    mConfigVolume.nSize = sizeof(OMX_AUDIO_CONFIG_VOLUMETYPE);
    //getOmxIlSpecVersion(&mConfigVolume.nVersion);
    mConfigVolume.nPortIndex     = portIndex;
    mConfigVolume.bLinear        = OMX_FALSE;
    mConfigVolume.sVolume.nValue = VOLCTRL_VOLUME_MAX > 0 ? 0 : VOLCTRL_VOLUME_MAX; // no amplification by default;;
    mConfigVolume.sVolume.nMin   = VOLCTRL_VOLUME_MIN;
    mConfigVolume.sVolume.nMax   = VOLCTRL_VOLUME_MAX;
    
    // VolumeRamp Configuration
    mConfigVolumeRamp.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE);
    //getOmxIlSpecVersion(&mConfigVolumeRamp.nVersion);
    mConfigVolumeRamp.nPortIndex            = portIndex;
    mConfigVolumeRamp.nChannel              = 2; //Default is stereo
    mConfigVolumeRamp.bLinear               = OMX_FALSE;
    mConfigVolumeRamp.sStartVolume.nValue   = VOLCTRL_VOLUME_MAX > 0 ? 0 : VOLCTRL_VOLUME_MAX; // no amplification by default;;
    mConfigVolumeRamp.sStartVolume.nMin     = VOLCTRL_VOLUME_MIN;
    mConfigVolumeRamp.sStartVolume.nMax     = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.sEndVolume.nValue     = VOLCTRL_VOLUME_MAX > 0 ? 0 : VOLCTRL_VOLUME_MAX; // no amplification by default;;
    mConfigVolumeRamp.sEndVolume.nMin       = VOLCTRL_VOLUME_MIN;
    mConfigVolumeRamp.sEndVolume.nMax       = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.nRampDuration         = 0;
    mConfigVolumeRamp.bRampTerminate        = OMX_FALSE;
    mConfigVolumeRamp.sCurrentVolume.nValue = VOLCTRL_VOLUME_MAX > 0 ? 0 : VOLCTRL_VOLUME_MAX; // no amplification by default;;
    mConfigVolumeRamp.sCurrentVolume.nMin   = VOLCTRL_VOLUME_MIN;
    mConfigVolumeRamp.sCurrentVolume.nMax   = VOLCTRL_VOLUME_MAX;
    mConfigVolumeRamp.nRampCurrentTime      = 0;
    mConfigVolumeRamp.nRampMinDuration = VOLCTRL_RAMPDURATION_MIN;
    mConfigVolumeRamp.nRampMaxDuration = VOLCTRL_RAMPDURATION_MAX;
    mConfigVolumeRamp.nVolumeStep      = 0; //TODO ?
}


NmfHostVolCtrl::~NmfHostVolCtrl(void) {}


OMX_ERRORTYPE NmfHostVolCtrl::instantiateAlgo(void)
{
    t_nmf_error   nmf_error;

    mNmfAlgo = volctrl_nmfil_host_effectWrappedCreate();
    if (mNmfAlgo == NULL) { return OMX_ErrorInsufficientResources; };
    (static_cast<volctrl_nmfil_host_effectWrapped*>(mNmfAlgo))->priority=mMyPriority;
    if (mNmfAlgo->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
	
    nmf_error = mNmfAlgo->bindFromUser("configure", 8, &mIconfigure);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfHostVolCtrl::deInstantiateAlgo(void)
{
    t_nmf_error   nmf_error;

    nmf_error = mNmfAlgo->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    if (mNmfAlgo->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    volctrl_nmfil_host_effectWrappedDestroy((volctrl_nmfil_host_effectWrapped*&)mNmfAlgo);

    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfHostVolCtrl::startAlgo(void) {
    mNmfAlgo->start();
    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfHostVolCtrl::stopAlgo(void) {
    mNmfAlgo->stop();
    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfHostVolCtrl::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex)
    {
        case OMX_IndexConfigAudioVolume:
        {
            OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume = static_cast<OMX_AUDIO_CONFIG_VOLUMETYPE *>(pComponentConfigStructure);

            if(pVolume->nPortIndex != mPortIndex) {
                return OMX_ErrorBadParameter;
            }

            //non linear mode only
            if (pVolume->bLinear != OMX_FALSE) {
              return OMX_ErrorBadParameter;
            }

            // use only volume value: dont take into account min and max (so no need to be set by caller)
            OMX_S32 volume = pVolume->sVolume.nValue;
            if (volume > mConfigVolume.sVolume.nMax) {
              volume = mConfigVolume.sVolume.nMax;
            } else if (volume < mConfigVolume.sVolume.nMin) {
              volume = mConfigVolume.sVolume.nMin;
            }
            mConfigVolume.sVolume.nValue = volume;

            break;
        }
    
        case OMX_IndexConfigAudioMute:
        {
            OMX_AUDIO_CONFIG_MUTETYPE * pMute = static_cast<OMX_AUDIO_CONFIG_MUTETYPE *>(pComponentConfigStructure);

            if(pMute->nPortIndex != mPortIndex) {
                return OMX_ErrorBadParameter;
            }
            mConfigMute.bMute = pMute->bMute;
            break;
        }

        case OMX_IndexConfigAudioBalance:
        {
            OMX_AUDIO_CONFIG_BALANCETYPE * pBalance = static_cast<OMX_AUDIO_CONFIG_BALANCETYPE *>(pComponentConfigStructure);

            if(pBalance->nPortIndex != mPortIndex) {
                return OMX_ErrorBadParameter;
            }
            mConfigBalance.nBalance = pBalance->nBalance;
            break;
        }

        case OMX_IndexConfigAudioVolumeRamp:
        {
            OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume = static_cast<OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE*>(pComponentConfigStructure);

            if(pRampVolume->nPortIndex != mPortIndex){
                return OMX_ErrorBadParameter;
                
            }

            //non linear mode only
            if (pRampVolume->bLinear != OMX_FALSE) {
              return OMX_ErrorBadParameter;
            }

            mIsRampRunning = OMX_TRUE;

            // update fields of interest; dont take into account min and max (so no need to be set by caller)

            mConfigVolumeRamp.nChannel = pRampVolume->nChannel; // TODO(pht) check channel validity

            OMX_S32 volumeStartValue = pRampVolume->sStartVolume.nValue;
            if (volumeStartValue > mConfigVolumeRamp.sStartVolume.nMax) {
              volumeStartValue = mConfigVolumeRamp.sStartVolume.nMax;
            } else if (volumeStartValue < mConfigVolumeRamp.sStartVolume.nMin) {
              volumeStartValue = mConfigVolumeRamp.sStartVolume.nMin;
            }
            mConfigVolumeRamp.sStartVolume.nValue = volumeStartValue;

            OMX_S32 volumeEndValue = pRampVolume->sEndVolume.nValue;
            if (volumeEndValue > mConfigVolumeRamp.sEndVolume.nMax) {
              volumeEndValue = mConfigVolumeRamp.sEndVolume.nMax;
            } else if (volumeEndValue < mConfigVolumeRamp.sEndVolume.nMin) {
              volumeEndValue = mConfigVolumeRamp.sEndVolume.nMin;
            }
            mConfigVolumeRamp.sEndVolume.nValue = volumeEndValue;

            OMX_U32 rampduration = pRampVolume->nRampDuration;
            if (rampduration == 0) {
              // TODO(pht) as per spec, indicates immediate volume application; check if need to support this
              return OMX_ErrorNotImplemented;
            } else {
              if (rampduration > mConfigVolumeRamp.nRampMaxDuration) {
                rampduration = mConfigVolumeRamp.nRampMaxDuration;
              } else if (rampduration < mConfigVolumeRamp.nRampMinDuration) {
                rampduration = mConfigVolumeRamp.nRampMinDuration;
              }
            }
            mConfigVolumeRamp.nRampDuration = rampduration;

            mConfigVolumeRamp.bRampTerminate = pRampVolume->bRampTerminate;

            break;
        }

        default:
            return OMX_ErrorBadParameter;
    }

    applyVolumeConfig();

    return OMX_ErrorNone; 

}

// FIXME TODO(pht) retrieveConfig, especially for RAMP

OMX_ERRORTYPE NmfHostVolCtrl::configureAlgo(void) {

    applyVolumeConfig();
    return OMX_ErrorNone;
}


void NmfHostVolCtrl::applyVolumeConfig(void)
{
    VolctrlConfig_t      volctrlConfig = {0};
    VolctrlRampConfig_t  volctrlRampConfig = {0};

    // TODO(pht) following formulas do not allow sub dB values: cf MPC code for fix

    if (mMultichannel == OMX_TRUE)
    {
        for(int i=0; i < VOLCTRL_MAX_VOLUME_GAINS; i++) {
            // update direct gains from master volume (volume is in millibel, gains are in decibel)
            // deliver to the algorithm in Q8 format in dB to the algorithm
            volctrlConfig.igains[i] = (128 + ((mConfigMute.bMute == OMX_TRUE ? (VOLCTRL_VOLUME_MUTE) : (mConfigVolume.sVolume.nValue) ) ) / 100 ) << 8;
        }
    }
    else
    {
        // update direct gains from master volume and balance (volume is in millibel, gains are in decibel)
        OMX_S32 leftVolumedB  = (mConfigBalance.nBalance <= 0 ? (mConfigVolume.sVolume.nValue) : (mConfigVolume.sVolume.nValue + mConfigBalance.nBalance*(VOLCTRL_VOLUME_MIN - mConfigVolume.sVolume.nValue)/VOLCTRL_BALANCE_ALLRIGHT)) / 100;
        OMX_S32 rightVolumedB = (mConfigBalance.nBalance <= 0 ? (mConfigVolume.sVolume.nValue + mConfigBalance.nBalance*(VOLCTRL_VOLUME_MIN - mConfigVolume.sVolume.nValue)/VOLCTRL_BALANCE_ALLLEFT) : (mConfigVolume.sVolume.nValue)) / 100;

        if(mConfigMute.bMute == OMX_TRUE)
        {
            leftVolumedB  = VOLCTRL_VOLUME_MUTE/100;
            rightVolumedB = VOLCTRL_VOLUME_MUTE/100;
        }

        volctrlConfig.igll = (128 + leftVolumedB) << 8;
        volctrlConfig.iglr =  0;
        volctrlConfig.igrl =  0;
        volctrlConfig.igrr = (128 + rightVolumedB) << 8;
    }

    volctrlConfig.iDBRamp       = 1;  // Ramp in dB
    volctrlConfig.ialpha        = 0;  // Ramp duration
    volctrlConfig.iDownMix      = mDownmix;
    volctrlConfig.iMultichannel = mMultichannel;

    mIconfigure.setConfig(volctrlConfig);

    if (mIsRampRunning == OMX_TRUE)
    {
        // Gains in the volume ramp are delivered in Q8 format to the algorithm
        // Ramp Duration is given by 100*msec = 100 * x[us] /1000 [us]
        mIsRampRunning                 = OMX_FALSE;
        volctrlRampConfig.iStartVolume = (128 + (mConfigVolumeRamp.sStartVolume.nValue/100)) << 8;
        volctrlRampConfig.iEndVolume   = (128 + (mConfigVolumeRamp.sEndVolume.nValue/100)) << 8;
        volctrlRampConfig.ialpha       = mConfigVolumeRamp.nRampDuration / 10;
        volctrlRampConfig.iDBRamp      = 1;
        volctrlRampConfig.iChannel     = mConfigVolumeRamp.nChannel;
        volctrlRampConfig.iTerminate   = mConfigVolumeRamp.bRampTerminate;

        mIconfigure.setRampConfig(volctrlRampConfig);
    }
}

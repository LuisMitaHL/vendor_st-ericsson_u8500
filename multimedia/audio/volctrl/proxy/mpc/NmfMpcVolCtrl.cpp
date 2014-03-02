/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcVolCtrl.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "ENS_Nmf.h"
#include "NmfMpcVolCtrl.h"

NmfMpcVolCtrl::NmfMpcVolCtrl(
        OMX_BOOL bDownmix,
        t_pcmprocessing_config config,
        OMX_U32 portIndex,
        OMX_BOOL usemultichannelmode)
    :
      NmfMpcPcmProcessing(config),
      mNmfEffectsLib(0),
      mOstTrace(0),
      mIconfigure(0),
      mDownmix(bDownmix),
      mNbChannel(0),
      mPortIndex(portIndex),
      mIsRampRunning(OMX_FALSE),
      mConfigVolumeRamp(),
      mpConfigVolumeChannel(0),
      mConfigVolume(),
      mConfigMute(),
      mConfigBalance()
{
  // VolumeRamp Configuration
  mConfigVolumeRamp.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE);
  //getOmxIlSpecVersion(&mConfigVolumeRamp.nVersion);
  mConfigVolumeRamp.nPortIndex            = portIndex;
  mConfigVolumeRamp.nChannel              = 2; //Default is stereo
  mConfigVolumeRamp.bLinear               = OMX_FALSE;
  mConfigVolumeRamp.sStartVolume.nValue   = VOLCTRL_VOLUME_MAX;
  mConfigVolumeRamp.sStartVolume.nMin     = VOLCTRL_VOLUME_MIN;
  mConfigVolumeRamp.sStartVolume.nMax     = VOLCTRL_VOLUME_MAX;
  mConfigVolumeRamp.sEndVolume.nValue     = VOLCTRL_VOLUME_MAX;
  mConfigVolumeRamp.sEndVolume.nMin       = VOLCTRL_VOLUME_MIN;
  mConfigVolumeRamp.sEndVolume.nMax       = VOLCTRL_VOLUME_MAX;
  mConfigVolumeRamp.nRampDuration         = 0;
  mConfigVolumeRamp.bRampTerminate        = OMX_FALSE;
  mConfigVolumeRamp.sCurrentVolume.nValue = VOLCTRL_VOLUME_MAX;
  mConfigVolumeRamp.sCurrentVolume.nMin   = VOLCTRL_VOLUME_MIN;
  mConfigVolumeRamp.sCurrentVolume.nMax   = VOLCTRL_VOLUME_MAX;
  mConfigVolumeRamp.nRampCurrentTime      = 0;
  mConfigVolumeRamp.nRampMinDuration = VOLCTRL_RAMPDURATION_MIN;
  mConfigVolumeRamp.nRampMaxDuration = VOLCTRL_RAMPDURATION_MAX;
  mConfigVolumeRamp.nVolumeStep      = 0; //TODO ?

  // Channel Volume Configuration
  if (usemultichannelmode == OMX_TRUE) {
    // volume config
    mpConfigVolumeChannel = new OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE[config.effectConfig.infmt.nof_channels];
    if (mpConfigVolumeChannel) {
      mNbChannel = config.effectConfig.infmt.nof_channels;
      for (unsigned int i = 0; i < mNbChannel; i ++) {
        mpConfigVolumeChannel[i].nSize = sizeof(OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE);
        // getOmxIlSpecVersion(&mpConfigVolumeChannel[i].nVersion);
        mpConfigVolumeChannel[i].nPortIndex = portIndex;
        mpConfigVolumeChannel[i].nChannel = i;
        mpConfigVolumeChannel[i].bLinear = OMX_FALSE;
        mpConfigVolumeChannel[i].sVolume.nValue = VOLCTRL_VOLUME_MAX > 0 ? 0 : VOLCTRL_VOLUME_MAX; // no amplification by default;
        mpConfigVolumeChannel[i].sVolume.nMin = VOLCTRL_VOLUME_MIN;
        mpConfigVolumeChannel[i].sVolume.nMax = VOLCTRL_VOLUME_MAX;
        mpConfigVolumeChannel[i].bIsMIDI = OMX_FALSE;  // not supported
      }
    } else {
      mNbChannel = 0;
    }
  }

  // Volume Configuration
  mConfigVolume.nSize = sizeof(OMX_AUDIO_CONFIG_VOLUMETYPE);
  //getOmxIlSpecVersion(&mConfigVolume.nVersion);
  mConfigVolume.nPortIndex     = portIndex;
  mConfigVolume.bLinear        = OMX_FALSE;
  mConfigVolume.sVolume.nValue = VOLCTRL_VOLUME_MAX > 0 ? 0 : VOLCTRL_VOLUME_MAX; // no amplification by default;
  mConfigVolume.sVolume.nMin   = VOLCTRL_VOLUME_MIN;
  mConfigVolume.sVolume.nMax   = VOLCTRL_VOLUME_MAX;

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

  registerStubsAndSkels();
}

NmfMpcVolCtrl::~NmfMpcVolCtrl(void) {
    unregisterStubsAndSkels();

    if(mpConfigVolumeChannel){
        delete mpConfigVolumeChannel;
    }
}

void NmfMpcVolCtrl::registerStubsAndSkels(void)
{
    CM_REGISTER_STUBS(volctrl_cpp);
}


void NmfMpcVolCtrl::unregisterStubsAndSkels(void)
{
    CM_UNREGISTER_STUBS(volctrl_cpp);
}

OMX_ERRORTYPE NmfMpcVolCtrl::instantiateAlgo(void) {
    OMX_ERRORTYPE error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "libeffects.mpc.libeffects",
            "libeffects", &mNmfEffectsLib, mMyPriority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "osttrace.mmdsp",
            "OSTTRACE", &mOstTrace, mMyPriority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "volctrl.nmfil.effect",
            "Volctrl", &mNmfAlgo, mMyPriority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfEffectsLib, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfAlgo, "libvolctrl", mNmfEffectsLib, "libvolctrl");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(
            mNmfAlgo, "configure",  &mIconfigure, 20);
    if (error != OMX_ErrorNone) return error;

	return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfMpcVolCtrl::startAlgo(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfEffectsLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mNmfAlgo);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfMpcVolCtrl::stopAlgo(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfEffectsLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent(mNmfAlgo);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcVolCtrl::applyConfig(OMX_INDEXTYPE nConfigIndex,
                                         OMX_PTR pComponentConfigStructure)
{
  if (!pComponentConfigStructure) {
    return OMX_ErrorBadParameter;
  }
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioVolume:
        {
            OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume =
            static_cast<OMX_AUDIO_CONFIG_VOLUMETYPE *>(pComponentConfigStructure);
            if(pVolume->nPortIndex != mPortIndex){
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

            if (!mpConfigVolumeChannel) {
              mConfigVolume.sVolume.nValue = volume;
            } else {
              for (unsigned int i = 0; i < mNbChannel; i ++) {
                mpConfigVolumeChannel[i].sVolume.nValue = volume;
              }
            }

            break;
        }

        case OMX_IndexConfigAudioChannelVolume:
        {
            OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * pChannelVolume =
            static_cast<OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE *>(pComponentConfigStructure);
            if(pChannelVolume->nPortIndex != mPortIndex){
                return OMX_ErrorBadParameter;
            }
            if (!mpConfigVolumeChannel) {
              return OMX_ErrorUnsupportedIndex;
            }

            //non linear mode only
            if (pChannelVolume->bLinear != OMX_FALSE) {
              return OMX_ErrorBadParameter;
            }

            // use only volume value: dont take into account min and max (so no need to be set by caller)
            OMX_S32 volume = pChannelVolume->sVolume.nValue;
            // assume each channel has same max and min volumes
            if (mNbChannel > 0) {
              if (volume > mpConfigVolumeChannel[0].sVolume.nMax) {
                volume = mpConfigVolumeChannel[0].sVolume.nMax;
              } else if (volume < mpConfigVolumeChannel[0].sVolume.nMin) {
                volume = mpConfigVolumeChannel[0].sVolume.nMin;
              }
            }

            if (pChannelVolume->nChannel == OMX_ALL) {
              for (unsigned int i = 0; i < mNbChannel; i ++) {
                mpConfigVolumeChannel[i].sVolume.nValue = volume;
              }
            } else {
              if (pChannelVolume->nChannel >= mNbChannel) {
                return OMX_ErrorBadParameter;
              }
              mpConfigVolumeChannel[pChannelVolume->nChannel].sVolume.nValue = volume;
            }

            break;
        }

        case OMX_IndexConfigAudioMute:
        {
            OMX_AUDIO_CONFIG_MUTETYPE * pMute =
            static_cast<OMX_AUDIO_CONFIG_MUTETYPE *>(pComponentConfigStructure);
            if(pMute->nPortIndex != mPortIndex){
                return OMX_ErrorBadParameter;
            }
            mConfigMute.bMute = pMute->bMute;
            break;
        }

        case OMX_IndexConfigAudioBalance:
        {
            OMX_AUDIO_CONFIG_BALANCETYPE * pBalance =
            static_cast<OMX_AUDIO_CONFIG_BALANCETYPE *>(pComponentConfigStructure);
            if(pBalance->nPortIndex != mPortIndex){
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
        default:{
            return OMX_ErrorBadParameter;
        }
    }
    return OMX_ErrorNone;

}

OMX_ERRORTYPE NmfMpcVolCtrl::retrieveConfig(OMX_INDEXTYPE nConfigIndex,
                                         OMX_PTR pComponentConfigStructure)
{
  if (!pComponentConfigStructure) {
    return OMX_ErrorBadParameter;
  }
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioVolume:
        {
            OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume =
            static_cast<OMX_AUDIO_CONFIG_VOLUMETYPE *>(pComponentConfigStructure);
            if(pVolume->nPortIndex != mPortIndex){
                return OMX_ErrorBadParameter;
            }

            //non linear mode only
            if (pVolume->bLinear != OMX_FALSE) {
              return OMX_ErrorBadParameter;
            }

            // use left gainread value
            // current volctrl: no multichannel api
            OMX_U32 gainread;
            if( (ENS::OMX_Error(CM_ReadComponentAttribute(mNmfAlgo,"gGLL",&gainread))) != OMX_ErrorNone){
              return OMX_ErrorUndefined;
            }

            // FIXME in idle, volctrl gains do not reflect gain values applied through mIconfigure.setConfig..
            // and report 0 (not a valid value):
            // workaround: in that case, do not update Volume value..
            OMX_S32 volback = (((OMX_S32)gainread * 100) >> 8) - 12800;
            // check vs absolute ranges (needed in order to cope with rounded values at min/max levels)
            if (volback < VOLCTRL_VOLUME_MIN) {
              volback = VOLCTRL_VOLUME_MIN;
            } else if (volback > VOLCTRL_VOLUME_MAX) {
              volback = VOLCTRL_VOLUME_MAX;
            }
            if (!mpConfigVolumeChannel) {
              if (gainread) {
                mConfigVolume.sVolume.nValue = volback;
              }
              pVolume->sVolume.nValue = mConfigVolume.sVolume.nValue;
              pVolume->sVolume.nMin = mConfigVolume.sVolume.nMin;
              pVolume->sVolume.nMax = mConfigVolume.sVolume.nMax;
            } else {
              // use channel 0 in that case
              if (gainread) {
                mpConfigVolumeChannel[0].sVolume.nValue = volback;
              }
              pVolume->sVolume.nValue = mpConfigVolumeChannel[0].sVolume.nValue;
              pVolume->sVolume.nMin = mpConfigVolumeChannel[0].sVolume.nMin;
              pVolume->sVolume.nMax = mpConfigVolumeChannel[0].sVolume.nMax;
            }
            break;
        }
        case OMX_IndexConfigAudioChannelVolume:
        {
            OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * pChannelVolume =
            static_cast<OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE *>(pComponentConfigStructure);
            if(pChannelVolume->nPortIndex != mPortIndex){
                return OMX_ErrorBadParameter;
            }
            if (!mpConfigVolumeChannel) {
              return OMX_ErrorUnsupportedIndex;
            }

            //non linear mode only
            if (pChannelVolume->bLinear != OMX_FALSE) {
              return OMX_ErrorBadParameter;
            }

            OMX_U32 channel;
            if (pChannelVolume->nChannel == OMX_ALL) {
              //use channel 0, dont check other channels
              channel = 0;
            } else {
              if (pChannelVolume->nChannel >= mNbChannel) {
                return OMX_ErrorBadParameter;
              }
              channel = pChannelVolume->nChannel;
            }

            // current volctrl: no multichannel api, so use either channel 0 or 1
            OMX_U32 gainread;
            if (channel == 0) {
              if( (ENS::OMX_Error(CM_ReadComponentAttribute(mNmfAlgo,"gGLL",&gainread))) != OMX_ErrorNone){
                return OMX_ErrorUndefined;
              }
            } else {
              if( (ENS::OMX_Error(CM_ReadComponentAttribute(mNmfAlgo,"gGRR",&gainread))) != OMX_ErrorNone){
                return OMX_ErrorUndefined;
              }
            }

            // FIXME in idle, volctrl gains do not reflect gain values applied through mIconfigure.setConfig..
            // and report 0 (not a valid value):
            // workaround: in that case, do not update Volume value..
            if (gainread) {
              OMX_S32 volback = (((OMX_S32)gainread * 100) >> 8) - 12800;
              // check vs absolute ranges (needed in order to cope with rounded values at min/max levels)
              if (volback < VOLCTRL_VOLUME_MIN) {
                volback = VOLCTRL_VOLUME_MIN;
              } else if (volback > VOLCTRL_VOLUME_MAX) {
                volback = VOLCTRL_VOLUME_MAX;
              }
              mpConfigVolumeChannel[channel].sVolume.nValue = volback;
            }
            pChannelVolume->sVolume.nValue = mpConfigVolumeChannel[channel].sVolume.nValue;
            pChannelVolume->sVolume.nMin = mpConfigVolumeChannel[channel].sVolume.nMin;
            pChannelVolume->sVolume.nMax = mpConfigVolumeChannel[channel].sVolume.nMax;

            break;
        }

        case OMX_IndexConfigAudioVolumeRamp:
        {
            OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume = static_cast<OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE*>(pComponentConfigStructure);

            //non linear mode only
            if (pRampVolume->bLinear != OMX_FALSE) {
              return OMX_ErrorBadParameter;
            }

            OMX_U32 gainread;
            OMX_U32 currentTime;
            if((pRampVolume->nChannel == OMX_ALL) ||
                (pRampVolume->nChannel == OMX_AUDIO_ChannelCF) ||
                (pRampVolume->nChannel == OMX_AUDIO_ChannelLF)) {
                if( (ENS::OMX_Error(CM_ReadComponentAttribute(mNmfAlgo,"gGLL",&gainread))) != OMX_ErrorNone){
                    return OMX_ErrorUndefined;
                }
                if( (ENS::OMX_Error(CM_ReadComponentAttribute(mNmfAlgo,"gTimeLeft",&currentTime))) != OMX_ErrorNone){
                    return OMX_ErrorUndefined;
                }
            } else if(pRampVolume->nChannel == OMX_AUDIO_ChannelRF) {
                if( (ENS::OMX_Error(CM_ReadComponentAttribute(mNmfAlgo,"gGRR",&gainread))) != OMX_ErrorNone){
                    return OMX_ErrorUndefined;
                }
                if( (ENS::OMX_Error(CM_ReadComponentAttribute(mNmfAlgo,"gTimeRight",&currentTime))) != OMX_ErrorNone){
                    return OMX_ErrorUndefined;
                }
            } else {
                return OMX_ErrorBadParameter;
            }

            // FIXME in idle, volctrl gains do not reflect gain values applied through mIconfigure.setRampConfig..
            // and report 0 (not a valid value):
            // workaround: in that case, do not update Volume value..
            if (gainread) {
              OMX_S32 volback = (((OMX_S32)gainread * 100) >> 8) - 12800;
              // check vs absolute ranges (needed in order to cope with rounded values at min/max levels)
              if (volback < VOLCTRL_VOLUME_MIN) {
                volback = VOLCTRL_VOLUME_MIN;
              } else if (volback > VOLCTRL_VOLUME_MAX) {
                volback = VOLCTRL_VOLUME_MAX;
              }
              mConfigVolumeRamp.sCurrentVolume.nValue = volback;
            }

            pRampVolume->sCurrentVolume.nValue = mConfigVolumeRamp.sCurrentVolume.nValue;
            pRampVolume->nRampCurrentTime = (mConfigVolumeRamp.nRampDuration-currentTime*10);

            // updates other static values
            pRampVolume->sStartVolume.nValue = mConfigVolumeRamp.sStartVolume.nValue;
            pRampVolume->sStartVolume.nMin   = mConfigVolumeRamp.sStartVolume.nMin;
            pRampVolume->sStartVolume.nMax   = mConfigVolumeRamp.sStartVolume.nMax;
            pRampVolume->sEndVolume.nValue = mConfigVolumeRamp.sEndVolume.nValue;
            pRampVolume->sEndVolume.nMin   = mConfigVolumeRamp.sEndVolume.nMin;
            pRampVolume->sEndVolume.nMax   = mConfigVolumeRamp.sEndVolume.nMax;

            pRampVolume->nRampDuration = mConfigVolumeRamp.nRampDuration;
            pRampVolume->bRampTerminate = mConfigVolumeRamp.bRampTerminate;

            pRampVolume->sCurrentVolume.nMin = mConfigVolumeRamp.sCurrentVolume.nMin;
            pRampVolume->sCurrentVolume.nMax = mConfigVolumeRamp.sCurrentVolume.nMax;

            pRampVolume->nRampMinDuration = mConfigVolumeRamp.nRampMinDuration;
            pRampVolume->nRampMaxDuration = mConfigVolumeRamp.nRampMaxDuration;
            pRampVolume->nVolumeStep = mConfigVolumeRamp.nVolumeStep;

            break;
        }
        default:
        {
          return OMX_ErrorBadParameter;
        }
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcVolCtrl::configureAlgo(void) {
  // Ramp volume or channel volume or simple volume or mute
  if (mIsRampRunning == OMX_TRUE) {
    VolctrlRampConfig_t  volctrlRampConfig = {0};

    // Gains in the volume ramp are delivered in Q8 format to the algorithm
    // Ramp Duration is given by 100*msec = 100 * x[us] /1000 [us]
    mIsRampRunning                 = OMX_FALSE;
    volctrlRampConfig.iStartVolume = 128 << 8;
    volctrlRampConfig.iStartVolume += static_cast<OMX_S32>((mConfigVolumeRamp.sStartVolume.nValue << 8)/100);
    volctrlRampConfig.iEndVolume   = 128 << 8;
    volctrlRampConfig.iEndVolume   += static_cast<OMX_S32>((mConfigVolumeRamp.sEndVolume.nValue << 8)/100);
    volctrlRampConfig.ialpha       = mConfigVolumeRamp.nRampDuration / 10;
    volctrlRampConfig.iDBRamp      = 1;
    if(mConfigVolumeRamp.nChannel != OMX_ALL){
      switch(mConfigVolumeRamp.nChannel)
      {
        case OMX_AUDIO_ChannelCF:
        case OMX_AUDIO_ChannelLF:
          volctrlRampConfig.iChannel = 1;
          break;
        case OMX_AUDIO_ChannelRF:
          volctrlRampConfig.iChannel = 2;
          break;
        default:
          volctrlRampConfig.iChannel = 3;
      }
    } else {
      volctrlRampConfig.iChannel = 3; //3 means all channels (Left+Right)
    }

    volctrlRampConfig.iTerminate   = mConfigVolumeRamp.bRampTerminate;

    mIconfigure.setRampConfig(volctrlRampConfig);
  } else {
    OMX_S32 leftVolumemdB;
    OMX_S32 rightVolumemdB;

    if (mConfigMute.bMute == OMX_TRUE) {
      leftVolumemdB  = VOLCTRL_VOLUME_MUTE;
      rightVolumemdB = VOLCTRL_VOLUME_MUTE;
    } else if (!mpConfigVolumeChannel) {
      // update direct gains from master volume and balance (volume is in millibel, gains are in decibel)
      leftVolumemdB  = (mConfigBalance.nBalance <= 0 ?
          (mConfigVolume.sVolume.nValue) :
          (mConfigVolume.sVolume.nValue + mConfigBalance.nBalance*(mConfigVolume.sVolume.nMin - mConfigVolume.sVolume.nValue)/VOLCTRL_BALANCE_ALLRIGHT));
      rightVolumemdB = (mConfigBalance.nBalance <= 0 ?
          (mConfigVolume.sVolume.nValue + mConfigBalance.nBalance*(mConfigVolume.sVolume.nMin - mConfigVolume.sVolume.nValue)/VOLCTRL_BALANCE_ALLLEFT) :
          (mConfigVolume.sVolume.nValue));
    } else {
      // multichannel variant; do not use balance config
      // current volctrl: no direct multichannel api (different from host volctrl)
      leftVolumemdB = mpConfigVolumeChannel[0].sVolume.nValue;
      rightVolumemdB = (mNbChannel >= 2) ? mpConfigVolumeChannel[1].sVolume.nValue : 0;
    }

    VolctrlConfig_t volctrlConfig = {0};
    volctrlConfig.iDownMix = mDownmix;
    volctrlConfig.iDBRamp  = 1;
    volctrlConfig.igll     = (128<<8); //offset
    volctrlConfig.igll     += static_cast<OMX_S32>((leftVolumemdB<<8)/100); //care to take into account values < 1dB
    volctrlConfig.iglr     = 0;
    volctrlConfig.igrl     = 0;
    volctrlConfig.igrr     = (128<<8); //offset
    volctrlConfig.igrr     += static_cast<OMX_S32>((rightVolumemdB<<8)/100); //care to take into account values < 1dB
    volctrlConfig.ialpha   = 0;

    mIconfigure.setConfig(volctrlConfig);
  }

  return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfMpcVolCtrl::deInstantiateAlgo(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponent(mNmfAlgo, "libvolctrl");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost( &mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfEffectsLib, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfEffectsLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfAlgo);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


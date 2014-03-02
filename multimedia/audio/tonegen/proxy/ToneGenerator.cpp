/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Tone generator proxy
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "ToneGenerator.hpp"
#include "ToneGeneratorNmfHost.hpp"
#include "tonegen/nmfil/host/composite/tonegen.hpp"
#include "OMX_Types.h"
#include "audio_chipset_api_index.h"
#include "audio_sourcesink_chipset_api.h"  // OMX_AUDIO_CONFIG_TONEGENERATORTYPE
#include "audio_render_chipset_api.h"  // OMX_AUDIO_CONFIG_VOLUMERAMPTYPE

//#define PRINT_DEBUG

#ifdef PRINT_DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF(x,...)
#endif


OMX_ERRORTYPE tonegeneratorFactoryMethod(ENS_Component_p * ppENSComponent)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    *ppENSComponent = 0;

    PRINTF("tonegeneratorFactoryMethod()\n");

    ToneGenerator *toneGen = new ToneGenerator();
    if (toneGen == 0) return OMX_ErrorInsufficientResources;

    error = toneGen->construct();

    if (error != OMX_ErrorNone) {
        delete toneGen;
        return error;
    }

    ToneGeneratorNmfHost *toneGenNmfHost = new ToneGeneratorNmfHost(*toneGen);
    if (toneGenNmfHost == 0) {
        delete toneGen;
        return OMX_ErrorInsufficientResources;
    }

    toneGen->setProcessingComponent(toneGenNmfHost);

    *ppENSComponent = toneGen;
    return error;
}


OMX_ERRORTYPE ToneGenerator::construct()
{
    OMX_ERRORTYPE error;
    OMX_AUDIO_PARAM_PCMMODETYPE defaultPcmSettingsOut;

    PRINTF("ToneGenerator::construct()\n");

    defaultPcmSettingsOut.nPortIndex     = 0;
    defaultPcmSettingsOut.eNumData       = OMX_NumericalDataSigned;
    defaultPcmSettingsOut.eEndian        = OMX_EndianLittle;
    defaultPcmSettingsOut.bInterleaved   = OMX_TRUE;
    defaultPcmSettingsOut.ePCMMode       = OMX_AUDIO_PCMModeLinear;
    defaultPcmSettingsOut.nBitPerSample  = 16;
    defaultPcmSettingsOut.nChannels      = 2;
    defaultPcmSettingsOut.nSamplingRate  = 8000;
    defaultPcmSettingsOut.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
    defaultPcmSettingsOut.eChannelMapping[1] = OMX_AUDIO_ChannelRF;

    error = AFM_Component::construct(1);  // one port
    if (error != OMX_ErrorNone) return error;

    //create o/p PCM port
    error = createPcmPort(0,
                          OMX_DirOutput,
                          getBlockSize() *
                            defaultPcmSettingsOut.nChannels *
                            defaultPcmSettingsOut.nBitPerSample / 8,
                          defaultPcmSettingsOut);
    if (error != OMX_ErrorNone) return error;

    AFM_PcmPort * outPort  = static_cast<AFM_PcmPort*>(getPort(0));
    outPort->setSupportedProprietaryComm(AFM_HOST_PROPRIETARY_COMMUNICATION);

    // Set tone generator default values...
    mEnable = TONEGENERATOR_ENABLE_DEFAULT;
    mTonetype = TONEGENERATOR_TONETYPE_DEFAULT;
    mFrequency[0] = TONEGENERATOR_FREQ1_DEFAULT;
    mFrequency[1] = TONEGENERATOR_FREQ2_DEFAULT;
    mDigit = TONEGENERATOR_DIGIT_DEFAULT;

    mLeftVolume[0] = TONEGENERATOR_VOLUME_DEFAULT;
    mLeftVolume[1] = TONEGENERATOR_VOLUME_DEFAULT;
    mRightVolume[0] = TONEGENERATOR_VOLUME_DEFAULT;
    mRightVolume[1] = TONEGENERATOR_VOLUME_DEFAULT;
    mLeftMute = TONEGENERATOR_MUTE_DEFAULT;
    mRightMute = TONEGENERATOR_MUTE_DEFAULT;

    return OMX_ErrorNone;
}


OMX_U32 ToneGenerator::getChannels(void) const
{
  PRINTF("ToneGenerator::getChannels()\n");

  OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings =
      ((AFM_PcmPort *) getPort(0))->getPcmSettings();

  return pcmSettings.nChannels;
}


void ToneGenerator::getVolumeLeft(
        t_sint16* startVolume,
        t_sint16* endVolume,
        t_bool* muteState) const
{
  PRINTF("ToneGenerator::getVolumeLeft()\n");

  *startVolume = mLeftVolume[0];
  *endVolume = mLeftVolume[1];
  *muteState = mLeftMute;
}


void ToneGenerator::getVolumeRight(
        t_sint16* startVolume,
        t_sint16* endVolume,
        t_bool* muteState) const
{
  PRINTF("ToneGenerator::getVolumeRight()\n");

  *startVolume = mRightVolume[0];
  *endVolume = mRightVolume[1];
  *muteState = mRightMute;
}


void ToneGenerator::getTone(Tone_t* tone) const
{
  PRINTF("ToneGenerator::getTone()\n");

  tone->enable = mEnable;
  if (mTonetype == OMX_TONEGEN_NormalTone)
    tone->tone_type = TONEGEN_TONE_TYPE_NORMAL;
  else
    tone->tone_type = TONEGEN_TONE_TYPE_DTMF;
  tone->frequencies[0] = mFrequency[0];
  tone->frequencies[1] = mFrequency[1];
  tone->digit = mDigit;
}


OMX_ERRORTYPE ToneGenerator::getParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) const
{
  PRINTF("ToneGenerator::getParameter()\n");

  switch (nParamIndex) {
    case AFM_IndexParamPcmLayout:
    {
      AFM_PARAM_PCMLAYOUTTYPE *pcmlayout =
            (AFM_PARAM_PCMLAYOUTTYPE *) pComponentParameterStructure;

      PRINTF("\tOMX_INDEXTYPE = AFM_IndexParamPcmLayout (%d, 0x%x)\n",
          nParamIndex, nParamIndex);

      if (pcmlayout->nPortIndex != 0) {
        return OMX_ErrorBadPortIndex;
      }

      pcmlayout->nBlockSize      = getBlockSize();
      pcmlayout->nChannels       = getChannels();
      pcmlayout->nMaxChannels    = 2;
      pcmlayout->nBitsPerSample  = getSampleBitSize();
      pcmlayout->nNbBuffers      = 1;

      break;
    }

    default:
      PRINTF("\tOMX_INDEXTYPE = unknown (%d, 0x%x)\n", nParamIndex, nParamIndex);
      return AFM_Component::getParameter(
        nParamIndex, pComponentParameterStructure);
  }
  return OMX_ErrorNone;
}


OMX_ERRORTYPE ToneGenerator::setConfig(
        OMX_INDEXTYPE nConfigIndex,
        OMX_PTR pComponentConfigStructure)
{
  PRINTF("ToneGenerator::setConfig()\n");
  OMX_U32 channels = getChannels();

  switch (nConfigIndex) {
    case OMX_IndexConfigAudioToneGenerator:
    {
      OMX_AUDIO_CONFIG_TONEGENERATORTYPE * pToneGen =
        (OMX_AUDIO_CONFIG_TONEGENERATORTYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioToneGenerator (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      if (pToneGen->nPortIndex > getPortCount()) {
        return OMX_ErrorBadPortIndex;
      }

      switch (pToneGen->eTonetype) {
        case OMX_TONEGEN_NormalTone:
        case OMX_TONEGEN_DtmfTone:
          break;
        default:
          return OMX_ErrorNotImplemented;
      }

      if (pToneGen->eTonetype == OMX_TONEGEN_DtmfTone) {
        switch (pToneGen->nDigit) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
          case '#':
          case '*':
            break;
          default:
            return OMX_ErrorBadParameter;
        }
      }

      mEnable = pToneGen->bEnable;
      mTonetype = pToneGen->eTonetype;
      mFrequency[0] = pToneGen->nFrequency[0];
      mFrequency[1] = pToneGen->nFrequency[1];
      mDigit = pToneGen->nDigit;

      break;
    }

    case OMX_IndexConfigAudioChannelVolume:
    {
      int volume;
      OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * pVol =
        (OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioChannelVolume (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      if (pVol->nPortIndex > getPortCount()) {
        return OMX_ErrorBadPortIndex;
      }

      volume = pVol->sVolume.nValue;

      if (pVol->bLinear == OMX_FALSE) {
        if (volume > TONEGENERATOR_VOLUME_MAX_MB) {
          return OMX_ErrorBadParameter;
        }

        switch (pVol->nChannel) {
          case 0:
            CONV_MB_TO_Q(mLeftVolume[0], volume);
            mLeftVolume[1] = mLeftVolume[0];
            break;
          case 1:
            if (channels != 2)
              return OMX_ErrorBadParameter;
            CONV_MB_TO_Q(mRightVolume[0], volume);
            mRightVolume[1] = mRightVolume[0];
            break;
          case OMX_ALL:
            CONV_MB_TO_Q(mLeftVolume[0],  volume);
            mLeftVolume[1] = mLeftVolume[0];
            mRightVolume[0] = mLeftVolume[0];
            mRightVolume[1] = mLeftVolume[0];
            break;
          default:
            return OMX_ErrorBadParameter;
        }
      }
      else {  // pVol->bLinear == OMX_TRUE
        if (volume > TONEGENERATOR_VOLUME_MAX_PERCENT ||
            volume < TONEGENERATOR_VOLUME_MIN_PERCENT) {
          return OMX_ErrorBadParameter;
        }

        switch (pVol->nChannel) {
          case 0:
            CONV_PERCENT_TO_Q(mLeftVolume[0], volume);
            mLeftVolume[1] = mLeftVolume[0];
            break;
          case 1:
            if (channels != 2)
              return OMX_ErrorBadParameter;
            CONV_PERCENT_TO_Q(mRightVolume[0], volume);
            mRightVolume[1] = mRightVolume[0];
            break;
          case OMX_ALL:
            CONV_PERCENT_TO_Q(mLeftVolume[0],  volume);
            mLeftVolume[1] = mLeftVolume[0];
            mRightVolume[0] = mLeftVolume[0];
            mRightVolume[1] = mLeftVolume[0];
            break;
          default:
            return OMX_ErrorBadParameter;
        }
      }
      break;
    }

    case OMX_IndexConfigAudioChannelMute:
    {
      OMX_AUDIO_CONFIG_CHANNELMUTETYPE * pMute =
        (OMX_AUDIO_CONFIG_CHANNELMUTETYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioChannelMute (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      if (pMute->nPortIndex > getPortCount()) {
        return OMX_ErrorBadPortIndex;
      }

      switch (pMute->nChannel) {
        case 0:
          mLeftMute = pMute->bMute;
          mLeftVolume[0] = mLeftVolume[1];
          break;
        case 1:
          if (channels != 2)
            return OMX_ErrorBadParameter;
          mRightMute = pMute->bMute;
          mRightVolume[0] = mRightVolume[1];
          break;
        case OMX_ALL:
          mLeftMute = pMute->bMute;
          mRightMute = mLeftMute;
          mLeftVolume[0] = mLeftVolume[1];
          mRightVolume[0] = mRightVolume[1];
          break;
        default:
          return OMX_ErrorBadParameter;
      }
      break;
    }

    case OMX_IndexConfigAudioVolumeRamp:
    {
      OMX_AUDIO_CONFIG_VOLUMERAMPTYPE * pVolRamp =
        (OMX_AUDIO_CONFIG_VOLUMERAMPTYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioVolumeRamp (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      if (pVolRamp->nPortIndex > getPortCount()) {
        return OMX_ErrorBadPortIndex;
      }

      if (pVolRamp->nRampDuration.nValue != 0)
        return OMX_ErrorBadParameter;

      if (pVolRamp->bLinear == OMX_FALSE) {
        if (pVolRamp->sStartVolume.nValue > TONEGENERATOR_VOLUME_MAX_MB ||
            pVolRamp->sEndVolume.nValue   > TONEGENERATOR_VOLUME_MAX_MB) {
          return OMX_ErrorBadParameter;
        }

        switch (pVolRamp->nChannel) {
          case 0:
            CONV_MB_TO_Q(mLeftVolume[1], pVolRamp->sEndVolume.nValue);
            if (mLeftMute)
              mLeftVolume[0] = mLeftVolume[1];
            else
              CONV_MB_TO_Q(mLeftVolume[0], pVolRamp->sStartVolume.nValue);
            break;
          case 1:
            if (channels != 2)
              return OMX_ErrorBadParameter;
            CONV_MB_TO_Q(mRightVolume[1], pVolRamp->sEndVolume.nValue);
            if (mRightMute)
              mRightVolume[0] = mRightVolume[1];
            else
              CONV_MB_TO_Q(mRightVolume[0], pVolRamp->sStartVolume.nValue);
            break;
          case OMX_ALL:
            CONV_MB_TO_Q(mLeftVolume[1], pVolRamp->sEndVolume.nValue);
            mRightVolume[1] = mLeftVolume[1];

            if (mLeftMute)
              mLeftVolume[0] = mLeftVolume[1];
            else
              CONV_MB_TO_Q(mLeftVolume[0], pVolRamp->sStartVolume.nValue);

            if (mRightMute)
              mRightVolume[0] = mRightVolume[1];
            else
              CONV_MB_TO_Q(mRightVolume[0], pVolRamp->sStartVolume.nValue);

            break;
          default:
            return OMX_ErrorBadParameter;
        }
      }
      else {  // pVolRamp->bLinear == OMX_TRUE
        if (pVolRamp->sStartVolume.nValue > TONEGENERATOR_VOLUME_MAX_PERCENT ||
            pVolRamp->sStartVolume.nValue < TONEGENERATOR_VOLUME_MIN_PERCENT ||
            pVolRamp->sEndVolume.nValue   > TONEGENERATOR_VOLUME_MAX_PERCENT ||
            pVolRamp->sEndVolume.nValue   < TONEGENERATOR_VOLUME_MIN_PERCENT) {
          return OMX_ErrorBadParameter;
        }

        switch (pVolRamp->nChannel) {
          case 0:
            CONV_PERCENT_TO_Q(mLeftVolume[1], pVolRamp->sEndVolume.nValue);
            if (mLeftMute)
              mLeftVolume[0] = mLeftVolume[1];
            else
              CONV_PERCENT_TO_Q(mLeftVolume[0], pVolRamp->sStartVolume.nValue);
            break;
          case 1:
            if (channels != 2)
              return OMX_ErrorBadParameter;
            CONV_PERCENT_TO_Q(mRightVolume[1], pVolRamp->sEndVolume.nValue);
            if (mRightMute)
              mRightVolume[0] = mRightVolume[1];
            else
              CONV_PERCENT_TO_Q(mRightVolume[0], pVolRamp->sStartVolume.nValue);
            break;
          case OMX_ALL:
            CONV_PERCENT_TO_Q(mLeftVolume[1], pVolRamp->sEndVolume.nValue);
            mRightVolume[1] = mLeftVolume[1];

            if (mLeftMute)
              mLeftVolume[0] = mLeftVolume[1];
            else
              CONV_PERCENT_TO_Q(mLeftVolume[0], pVolRamp->sStartVolume.nValue);

            if (mRightMute)
              mRightVolume[0] = mRightVolume[1];
            else
              CONV_PERCENT_TO_Q(mRightVolume[0], pVolRamp->sStartVolume.nValue);

            break;
          default:
            return OMX_ErrorBadParameter;
        }
      }
      break;
    }

    default:
      PRINTF("\tOMX_INDEXTYPE = unknown (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);
      return AFM_Component::setConfig(
        nConfigIndex, pComponentConfigStructure);
  }

  return OMX_ErrorNone;
}

OMX_ERRORTYPE ToneGenerator::getConfig(
        OMX_INDEXTYPE nConfigIndex,
        OMX_PTR pComponentConfigStructure) const
{
  PRINTF("ToneGenerator::getConfig()\n");
  OMX_U32 channels = getChannels();

  switch (nConfigIndex) {
    case OMX_IndexConfigAudioToneGenerator:
    {
      OMX_AUDIO_CONFIG_TONEGENERATORTYPE * pToneGen =
        (OMX_AUDIO_CONFIG_TONEGENERATORTYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioToneGenerator (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      if (pToneGen->nPortIndex > getPortCount()) {
        return OMX_ErrorBadPortIndex;
      }

      pToneGen->bEnable = mEnable;
      pToneGen->eTonetype = mTonetype;
      if (mTonetype == OMX_TONEGEN_NormalTone) {
        pToneGen->nFrequency[0] = mFrequency[0];
        pToneGen->nFrequency[1] = mFrequency[1];
      }
      else {
        pToneGen->nDigit = mDigit;
      }
      break;
    }

    case OMX_IndexConfigAudioChannelVolume:
    {
      int volume;
      OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * pVol =
        (OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioChannelVolume (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      if (pVol->nPortIndex > getPortCount()) {
        return OMX_ErrorBadPortIndex;
      }

      switch (pVol->nChannel) {
        case 0:
          volume = mLeftVolume[1];
          break;
        case 1:
          if (channels != 2)
            return OMX_ErrorBadParameter;
          volume = mRightVolume[1];
          break;
        case OMX_ALL:
          // This is the correct behavior for OMX_IndexConfigAudioVolumeRamp.
          // Use the same behavior here because it is unclear.
          if (mLeftVolume[1] != mRightVolume[1] && channels == 2)
            return OMX_ErrorBadParameter;
          volume = mLeftVolume[1];
          break;
        default:
          return OMX_ErrorBadParameter;
      }

      if (pVol->bLinear == OMX_FALSE) {
        pVol->sVolume.nMin = TONEGENERATOR_VOLUME_MIN_MB;
        pVol->sVolume.nMax = TONEGENERATOR_VOLUME_MAX_MB;
        CONV_Q_TO_MB(pVol->sVolume.nValue, volume);
      }
      else {
        pVol->sVolume.nMin = TONEGENERATOR_VOLUME_MIN_PERCENT;
        pVol->sVolume.nMax = TONEGENERATOR_VOLUME_MAX_PERCENT;
        CONV_Q_TO_PERCENT(pVol->sVolume.nValue, volume);
      }
      pVol->bIsMIDI = OMX_FALSE;
      break;
    }

    case OMX_IndexConfigAudioChannelMute:
    {
      OMX_AUDIO_CONFIG_CHANNELMUTETYPE * pMute =
        (OMX_AUDIO_CONFIG_CHANNELMUTETYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioChannelMute (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      if (pMute->nPortIndex > getPortCount()) {
        return OMX_ErrorBadPortIndex;
      }

      switch (pMute->nChannel) {
        case 0:
          pMute->bMute = mLeftMute;
          break;
        case 1:
          if (channels != 2)
            return OMX_ErrorBadParameter;
          pMute->bMute = mRightMute;
          break;
        case OMX_ALL:
          // This is the correct behavior for OMX_IndexConfigAudioVolumeRamp.
          // Use the same behavior here because it is unclear.
          if (mLeftMute != mRightMute && channels == 2)
            return OMX_ErrorBadParameter;
          pMute->bMute = mLeftMute;
          break;
        default:
          return OMX_ErrorBadParameter;
      }
      pMute->bIsMIDI = OMX_FALSE;
      break;
    }

    case OMX_IndexConfigAudioVolumeRamp:
    {
      int volume;
      OMX_AUDIO_CONFIG_VOLUMERAMPTYPE * pVolRamp =
        (OMX_AUDIO_CONFIG_VOLUMERAMPTYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioVolumeRamp (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      if (pVolRamp->nPortIndex > getPortCount()) {
        return OMX_ErrorBadPortIndex;
      }

      switch (pVolRamp->nChannel) {
        case 0:
          volume = mLeftVolume[1];
          break;
        case 1:
          if (channels != 2)
            return OMX_ErrorBadParameter;
          volume = mRightVolume[1];
          break;
        case OMX_ALL:
          if (mLeftVolume[1] != mRightVolume[1] && channels == 2)
            return OMX_ErrorBadParameter;
          volume = mLeftVolume[1];
          break;
        default:
          return OMX_ErrorBadParameter;
      }

      // Only nRampDuration = 0 is supported, thus all value is static
      // except for current volume.

      if (pVolRamp->bLinear == OMX_FALSE) {
        pVolRamp->sStartVolume.nMin = TONEGENERATOR_VOLUME_MIN_MB;
        pVolRamp->sStartVolume.nMax = TONEGENERATOR_VOLUME_MAX_MB;
        pVolRamp->sStartVolume.nValue = 0;

        pVolRamp->sEndVolume.nMin = TONEGENERATOR_VOLUME_MIN_MB;
        pVolRamp->sEndVolume.nMax = TONEGENERATOR_VOLUME_MAX_MB;
        pVolRamp->sEndVolume.nValue = 0;

        pVolRamp->nRampDuration.nMin = 0;
        pVolRamp->nRampDuration.nMax = 0;
        pVolRamp->nRampDuration.nValue = 0;

        pVolRamp->bRampTerminate = OMX_FALSE;

        pVolRamp->sCurrentVolume.nMin = TONEGENERATOR_VOLUME_MIN_MB;
        pVolRamp->sCurrentVolume.nMax = TONEGENERATOR_VOLUME_MAX_MB;
        CONV_Q_TO_MB(pVolRamp->sCurrentVolume.nValue, volume);

        pVolRamp->nRampCurrentTime = 0;
      }
      else {
        pVolRamp->sStartVolume.nMin = TONEGENERATOR_VOLUME_MIN_PERCENT;
        pVolRamp->sStartVolume.nMax = TONEGENERATOR_VOLUME_MAX_PERCENT;
        pVolRamp->sStartVolume.nValue = 0;

        pVolRamp->sEndVolume.nMin = TONEGENERATOR_VOLUME_MIN_PERCENT;
        pVolRamp->sEndVolume.nMax = TONEGENERATOR_VOLUME_MAX_PERCENT;
        pVolRamp->sEndVolume.nValue = 0;

        pVolRamp->nRampDuration.nMin = 0;
        pVolRamp->nRampDuration.nMax = 0;
        pVolRamp->nRampDuration.nValue = 0;

        pVolRamp->bRampTerminate = OMX_FALSE;

        pVolRamp->sCurrentVolume.nMin = TONEGENERATOR_VOLUME_MIN_PERCENT;
        pVolRamp->sCurrentVolume.nMax = TONEGENERATOR_VOLUME_MAX_PERCENT;
        CONV_Q_TO_PERCENT(pVolRamp->sCurrentVolume.nValue, volume);

        pVolRamp->nRampCurrentTime = 0;
      }
      PRINTF("\tpVolRamp->sCurrentVolume.nValue = %d (Volume = %d)\n",
             pVolRamp->sCurrentVolume.nValue, volume);
      break;
    }

    default:
      PRINTF("\tOMX_INDEXTYPE = unknown (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);
      return AFM_Component::getConfig(
        nConfigIndex, pComponentConfigStructure);
  }

  return OMX_ErrorNone;
}

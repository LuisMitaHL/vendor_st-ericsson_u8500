/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Tone generator NMF Host class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "ToneGeneratorNmfHost.hpp"
#include "ToneGenerator.hpp"
#include "tonegen/nmfil/host/composite/wrapper.hpp"
#include "tonegen/nmfil/host/composite/tonegen.hpp"
#include "AFM_PcmProcessing.h"
#include "AFM_Utils.h"
#include "AFM_PcmPort.h"
#include "audio_chipset_api_index.h"
#include "audio_sourcesink_chipset_api.h"  // OMX_AUDIO_CONFIG_TONEGENERATORTYPE
#include "audio_render_chipset_api.h"  // OMX_AUDIO_CONFIG_VOLUMERAMPTYPE
#include <string.h>

//#define PRINT_DEBUG

#ifdef PRINT_DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF(x,...)
#endif


OMX_ERRORTYPE
ToneGeneratorNmfHost::construct(void)
{
    mNmfil = 0;
    return (AFMNmfHost_ProcessingComp::construct());
}


OMX_ERRORTYPE
ToneGeneratorNmfHost::instantiateMain(void)
{
  t_nmf_error nmf_error;
  OMX_ERRORTYPE error;

  PRINTF("ToneGeneratorNmfHost::instantiateMain()\n");

  error = instantiateAlgo();
  if (error != OMX_ErrorNone) return error;

  mNmfMain = tonegen_nmfil_host_composite_wrapperCreate();
  if (mNmfMain == NULL) return OMX_ErrorInsufficientResources;
  ((tonegen_nmfil_host_composite_wrapper *)(mNmfil))->priority=getPriorityLevel();
  if (mNmfMain->construct() != NMF_OK) return OMX_ErrorInsufficientResources;

  nmf_error = mNmfMain->bindFromUser("configure", 1, &mIMainConfig);
  if (nmf_error != NMF_OK) {
    NMF_LOG("Error: unable to bind main configure!...[%d]\n", nmf_error);
    return OMX_ErrorInsufficientResources;
  }

  nmf_error = mNmfMain->bindComponent("source", mNmfil, "source");
  if (nmf_error != NMF_OK) {
    NMF_LOG("Error: unable to bind main source!...[%d]\n", nmf_error);
    return OMX_ErrorInsufficientResources;
  }

  return OMX_ErrorNone;
}


OMX_ERRORTYPE
ToneGeneratorNmfHost::deInstantiateMain(void)
{
  OMX_ERRORTYPE error;
  t_nmf_error nmf_error;

  PRINTF("ToneGeneratorNmfHost::deInstantiateMain()\n");

  nmf_error = mNmfMain->unbindComponent("source", mNmfil, "source");
  if (nmf_error != NMF_OK) {
    NMF_LOG("Error: unable to unbind main source!...[%d]\n", nmf_error);
    return OMX_ErrorInsufficientResources;
  }

  nmf_error = mNmfMain->unbindFromUser("configure");
  if (nmf_error != NMF_OK) {
    NMF_LOG("Error: unable to unbind main configure!...[%d]\n", nmf_error);
    return OMX_ErrorInsufficientResources;
  }

  error = deInstantiateAlgo();
  if (error != OMX_ErrorNone) return error;

  if (mNmfMain->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;

  tonegen_nmfil_host_composite_wrapperDestroy(
      (tonegen_nmfil_host_composite_wrapper*&)mNmfMain);

  return OMX_ErrorNone;
}


OMX_ERRORTYPE
ToneGeneratorNmfHost::startMain(void)
{
  PRINTF("ToneGeneratorNmfHost::startMain()\n");

  mNmfMain->start();
  return OMX_ErrorNone;
}


OMX_ERRORTYPE
ToneGeneratorNmfHost::stopMain(void)
{
  PRINTF("ToneGeneratorNmfHost::stopMain()\n");

  mNmfMain->stop();
  return OMX_ErrorNone;
}


OMX_ERRORTYPE
ToneGeneratorNmfHost::configureMain(void)
{
  OMX_ERRORTYPE error;

  PRINTF("ToneGeneratorNmfHost::configureMain()\n");

  error = configureAlgo();
  if (error != OMX_ErrorNone) return error;

  AFM_PcmProcessing* proxy = static_cast<AFM_PcmProcessing*>(&mENSComponent);
  AFM_PcmPort* outPort = (AFM_PcmPort*) proxy->getPort(0);
  OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings = outPort->getPcmSettings();

  // Set PCM  config...
  t_tonegen_wrapper_config pcmConfig;
  memset(&pcmConfig, 0, sizeof(t_tonegen_wrapper_config));
  pcmConfig.freq = AFM::sampleFreq(pcmSettings.nSamplingRate);
  pcmConfig.nof_channels = pcmSettings.nChannels;
  pcmConfig.nof_bits_per_sample = pcmSettings.nBitPerSample;
  mIMainConfig.setParameter(pcmConfig);

  return OMX_ErrorNone;
}


OMX_ERRORTYPE
ToneGeneratorNmfHost::applyConfig(
                    OMX_INDEXTYPE nConfigIndex,
                    OMX_PTR pComponentConfigStructure)
{
  PRINTF("ToneGeneratorNmfHost::applyConfig()\n");
  switch (nConfigIndex) {
    case OMX_IndexConfigAudioToneGenerator:
    {
      OMX_AUDIO_CONFIG_TONEGENERATORTYPE * pToneGen =
        (OMX_AUDIO_CONFIG_TONEGENERATORTYPE *) pComponentConfigStructure;
      Tone_t toneCfg;
      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioToneGenerator (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      memset(&toneCfg, 0, sizeof(toneCfg));
      toneCfg.enable = pToneGen->bEnable;
      if (pToneGen->eTonetype == OMX_TONEGEN_NormalTone) {
        toneCfg.tone_type = TONEGEN_TONE_TYPE_NORMAL;
        toneCfg.frequencies[0] = pToneGen->nFrequency[0];
        toneCfg.frequencies[1] = pToneGen->nFrequency[1];
      }
      else {
        toneCfg.tone_type = TONEGEN_TONE_TYPE_DTMF;
        toneCfg.digit = pToneGen->nDigit;
      }
      mIAlgoConfig.setTone(toneCfg);
      break;
    }

    case OMX_IndexConfigAudioChannelVolume:
    {
      t_sint16 volume;
      OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE * pVol =
        (OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioChannelVolume (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      if (pVol->bLinear == OMX_FALSE) {
        CONV_MB_TO_Q(volume, pVol->sVolume.nValue);
      }
      else {
        CONV_PERCENT_TO_Q(volume, pVol->sVolume.nValue);
      }

      mIAlgoConfig.setVolume(volume, pVol->nChannel);
      break;
    }

    case OMX_IndexConfigAudioChannelMute:
    {
      OMX_AUDIO_CONFIG_CHANNELMUTETYPE * pMute =
        (OMX_AUDIO_CONFIG_CHANNELMUTETYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioChannelMute (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      mIAlgoConfig.setMute(pMute->bMute, pMute->nChannel);
      break;
    }

    case OMX_IndexConfigAudioVolumeRamp:
    {
      int startVolume;
      int endVolume;
      OMX_AUDIO_CONFIG_VOLUMERAMPTYPE * pVolRamp =
        (OMX_AUDIO_CONFIG_VOLUMERAMPTYPE *) pComponentConfigStructure;

      PRINTF("\tOMX_INDEXTYPE = OMX_IndexConfigAudioVolumeRamp (%d, 0x%x)\n",
          nConfigIndex, nConfigIndex);

      if (pVolRamp->bLinear == OMX_FALSE) {
        CONV_MB_TO_Q(startVolume, pVolRamp->sStartVolume.nValue);
        CONV_MB_TO_Q(endVolume,   pVolRamp->sEndVolume.nValue);
      }
      else {
        CONV_PERCENT_TO_Q(startVolume, pVolRamp->sStartVolume.nValue);
        CONV_PERCENT_TO_Q(endVolume,   pVolRamp->sEndVolume.nValue);
      }

      mIAlgoConfig.setVolumeRamp(startVolume, endVolume, pVolRamp->nChannel);
      break;
    }

    default:
      PRINTF("\tOMX_INDEXTYPE = unknown (%d, 0x%x)\n", nConfigIndex, nConfigIndex);
      return AFMNmfHost_ProcessingComp::applyConfig(nConfigIndex, pComponentConfigStructure);
  }
  return OMX_ErrorNone;
}

OMX_ERRORTYPE
ToneGeneratorNmfHost::instantiateAlgo(void)
{
    t_nmf_error nmf_error;
    PRINTF("ToneGeneratorNmfHost::instantiateAlgo()\n");

    mNmfil = tonegen_nmfil_host_composite_tonegenCreate();
    if (mNmfil == NULL) return OMX_ErrorInsufficientResources;
    ((tonegen_nmfil_host_composite_tonegen *)(mNmfil))->priority=getPriorityLevel();
    if (mNmfil->construct() != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfil->bindFromUser("configure", 1, &mIAlgoConfig);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
ToneGeneratorNmfHost::deInstantiateAlgo(void)
{
    t_nmf_error nmf_error;

    PRINTF("ToneGeneratorNmfHost::deInstantiateAlgo()\n");

    nmf_error = mNmfil->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
    if (mNmfil->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;

    tonegen_nmfil_host_composite_tonegenDestroy((tonegen_nmfil_host_composite_tonegen*&)mNmfil);

    return OMX_ErrorNone;
}


OMX_ERRORTYPE
ToneGeneratorNmfHost::configureAlgo(void)
{
  PRINTF("ToneGeneratorNmfHost::configureAlgo()\n");

  ToneGenerator *proxy = static_cast<ToneGenerator *>(&mENSComponent);
  Config_t toneGenCfg;
  t_sint16 startVolume, endVolume;
  t_bool muteState;

  proxy->getTone(&toneGenCfg.tone);

  proxy->getVolumeLeft(&startVolume, &endVolume, &muteState);
  toneGenCfg.leftVolume[0] = startVolume;
  toneGenCfg.leftVolume[1] = endVolume;
  toneGenCfg.muteState[0] = muteState;

  proxy->getVolumeRight(&startVolume, &endVolume, &muteState);
  toneGenCfg.rightVolume[0] = startVolume;
  toneGenCfg.rightVolume[1] = endVolume;
  toneGenCfg.muteState[1] = muteState;
  mIAlgoConfig.setConfig(toneGenCfg);

  return OMX_ErrorNone;
}

/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <tonegen/nmfil/host/tonegen.nmf>
#include <armnmf_dbc.h>
#include "OMX_Core.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "r_tonegen.h"
#include "t_tonegen.h"
#ifdef __cplusplus
}
#endif

// #define PRINT_DEBUG

#ifdef PRINT_DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF(x,...)
#endif


tonegen_nmfil_host_tonegen::tonegen_nmfil_host_tonegen()
{
  PRINTF("tonegen_nmfil_host_tonegen::tonegen()\n");

  mHandle = NULL;
  mPCM_Config = NULL;
  mReset = true;

  // Set some initialization values.
  mEnable = false;
  mTone_type = TONEGEN_TONE_TYPE_DTMF;
  mFrequencies[0] = 0;
  mFrequencies[1] = 0;
  mDigit = TG_TONE_KEY_0;
  mVolume[0] = 0;
  mVolume[1] = 0;
  mMute[0] = false;
  mMute[1] = false;
  memset(&mConfig, 0, sizeof(Config_t));
}


t_bool METH(open)(const t_source_config* config)
{
  int i;
  PRINTF("tonegen_nmfil_host_tonegen::open()\n");

  mHandle = TG_Create();
  if (mHandle == NULL) {
    return false;
  }

  mPCM_Config = (TG_PCM_Config_t*)calloc(1, sizeof(TG_PCM_Config_t));
  if (mPCM_Config == NULL) {
    TG_Destroy(&mHandle);
    return false;
  }

  switch (config->freq) {
    case FREQ_192KHZ:
      mPCM_Config->Freq = 192000;
      break;
    case FREQ_176_4KHZ:
      mPCM_Config->Freq = 176400;
      break;
    case FREQ_128KHZ:
      mPCM_Config->Freq = 128000;
      break;
    case FREQ_96KHZ:
      mPCM_Config->Freq = 96000;
      break;
    case FREQ_88_2KHZ:
      mPCM_Config->Freq = 88200;
      break;
    case FREQ_64KHZ:
      mPCM_Config->Freq = 64000;
      break;
    case FREQ_48KHZ:
      mPCM_Config->Freq = 48000;
      break;
    case FREQ_44_1KHZ:
      mPCM_Config->Freq = 44100;
      break;
    case FREQ_32KHZ:
      mPCM_Config->Freq = 32000;
      break;
    case FREQ_24KHZ:
      mPCM_Config->Freq = 24000;
      break;
    case FREQ_22_05KHZ:
      mPCM_Config->Freq = 22050;
      break;
    case FREQ_16KHZ:
      mPCM_Config->Freq = 16000;
      break;
    case FREQ_12KHZ:
      mPCM_Config->Freq = 12000;
      break;
    case FREQ_11_025KHZ:
      mPCM_Config->Freq = 11025;
      break;
    case FREQ_8KHZ:
      mPCM_Config->Freq = 8000;
      break;
    case FREQ_7_2KHZ:
      mPCM_Config->Freq = 7200;
      break;
    default:
      mPCM_Config->Freq = 48000;
      PRINTF("Unknown sample frequency = %d\n", config->freq);
      break;
  }

  mPCM_Config->Resolution = config->nof_bits_per_sample;
  mPCM_Config->NoOfChannels = config->nof_channels;
  mPCM_Config->Interval = mPCM_Config->NoOfChannels;
  for (i = 0; i < mPCM_Config->NoOfChannels; i++) {
    mPCM_Config->Channels[i].Offset = i;
  }

  // set buffered setting
  setTone(mConfig.tone);
  setMute(mConfig.muteState[0], 0);
  setMute(mConfig.muteState[1], 1);
  if (mConfig.leftVolume[0] == mConfig.leftVolume[1])
    setVolume(mConfig.leftVolume[1], 0);
  else
    setVolumeRamp(mConfig.leftVolume[0], mConfig.leftVolume[1], 0);

  if (mConfig.rightVolume[0] == mConfig.rightVolume[1])
    setVolume(mConfig.rightVolume[1], 1);
  else
    setVolumeRamp(mConfig.rightVolume[0], mConfig.rightVolume[1], 1);

  return true;
}


void METH(setConfig)(Config_t config)
{
  PRINTF("tonegen_nmfil_host_tonegen::setConfig()\n");
  memcpy(&mConfig, &config, sizeof(Config_t));
}


void METH(reset)(t_source_reset_reason reason)
{
  PRINTF("tonegen_nmfil_host_tonegen::reset()\n");

  mReset = true;
  TG_Reset(mHandle, mPCM_Config);
}


void METH(close)(void)
{
  PRINTF("tonegen_nmfil_host_tonegen::close()\n");

  TG_Destroy(&mHandle);
  free(mPCM_Config);
}


void METH(process)(t_source_process_params* params)
{
  OMX_BUFFERHEADERTYPE* omxBufHdr;

  mReset = false;
  omxBufHdr = (OMX_BUFFERHEADERTYPE*) params->buf_hdr;
  if (mEnable) {
    TG_NextFrame(mHandle, mPCM_Config, omxBufHdr);
  }
  else {
    memset(omxBufHdr->pBuffer, 0, omxBufHdr->nAllocLen);
  }
}


void METH(setTone)(Tone_t tone)
{
  TG_JobDescriptor_t* job = NULL;
  PRINTF("tonegen_nmfil_host_tonegen::setTone()\n");

  // do a reset before going back to enable
  if (tone.enable == TRUE && mEnable == FALSE) {
    reset(SOURCE_RESET_REASON_STOP);
  }

  mEnable = tone.enable;
  mTone_type = tone.tone_type;
  mFrequencies[0] = tone.frequencies[0];
  mFrequencies[1] = tone.frequencies[1];
  switch (tone.digit) {
    case '0':
      mDigit = TG_TONE_KEY_0;
      break;
    case '1':
      mDigit = TG_TONE_KEY_1;
      break;
    case '2':
      mDigit = TG_TONE_KEY_2;
      break;
    case '3':
      mDigit = TG_TONE_KEY_3;
      break;
    case '4':
      mDigit = TG_TONE_KEY_4;
      break;
    case '5':
      mDigit = TG_TONE_KEY_5;
      break;
    case '6':
      mDigit = TG_TONE_KEY_6;
      break;
    case '7':
      mDigit = TG_TONE_KEY_7;
      break;
    case '8':
      mDigit = TG_TONE_KEY_8;
      break;
    case '9':
      mDigit = TG_TONE_KEY_9;
      break;
    case '*':
      mDigit = TG_TONE_KEY_ASTERISK;
      break;
    case '#':
      mDigit = TG_TONE_KEY_NUMBERSIGN;
      break;
    default:
      mDigit = TG_TONE_TONE_OFF;
  }

  job = (TG_JobDescriptor_t*)calloc(1, sizeof(TG_JobDescriptor_t));
  if (job == NULL) {
    PRINTF("\tOut of memory\n");
    return;
  }

  switch (mTone_type) {
    case TONEGEN_TONE_TYPE_NORMAL:
      job->JobId = TG_JOB_ID_OWN_TONE;
      job->Descriptor.Own.FirstFrequency = mFrequencies[0];
      job->Descriptor.Own.SecondFrequency = mFrequencies[1];
      job->Descriptor.Own.ThirdFrequency = 0;
      break;
    case TONEGEN_TONE_TYPE_DTMF:
      job->JobId = TG_JOB_ID_DTMF_TONE;
      job->Descriptor.DTMF.Tone = mDigit;
      break;
  }

  (void)TG_SetTone(mHandle, mPCM_Config, job);
}


void METH(setVolume)(t_sint16 volume, t_uint32 nChannel)
{
  int time;
  PRINTF("tonegen_nmfil_host_tonegen::setVolume()\n");

  if (mReset)
    time = 0;
  else
    time = 20;

  switch (nChannel) {
    case 0:
      mVolume[0] = volume;
      if (!mMute[0]) {
        // Set volume on ch 0
        TG_SetVolume(mHandle, mPCM_Config, volume, nChannel, time);
      }
      break;
    case 1:
      mVolume[1] = volume;
      if (!mMute[1]) {
        // Set volume on ch 1
        TG_SetVolume(mHandle, mPCM_Config, volume, nChannel, time);
      }
      break;
    case OMX_ALL:
      mVolume[0] = volume;
      mVolume[1] = volume;
      if (!mMute[0]) {
        // Set volume on ch 0
        TG_SetVolume(mHandle, mPCM_Config, volume, 0, time);
      }
      if (!mMute[1] && mPCM_Config->NoOfChannels == 2) {
        // Set volume on ch 1
        TG_SetVolume(mHandle, mPCM_Config, volume, 1, time);
      }
      break;
  }
}


void METH(setVolumeRamp)(t_sint16 startVolume, t_sint16 endVolume, t_uint32 nChannel)
{
  PRINTF("tonegen_nmfil_host_tonegen::setVolumeRamp()\n");

  switch (nChannel) {
    case 0:
      mVolume[0] = endVolume;
      if (!mMute[0]) {
        // Set ramp on ch 0
        TG_RampVolume(mHandle, mPCM_Config, startVolume, endVolume, nChannel, 20);
      }
      break;
    case 1:
      mVolume[1] = endVolume;
      if (!mMute[1]) {
        // Set ramp on ch 1
        TG_RampVolume(mHandle, mPCM_Config, startVolume, endVolume, nChannel, 20);
      }
      break;
    case OMX_ALL:
      mVolume[0] = endVolume;
      mVolume[1] = endVolume;
      if (!mMute[0]) {
        // Set ramp on ch 0
        TG_RampVolume(mHandle, mPCM_Config, startVolume, endVolume, 0, 20);
      }
      if (!mMute[1] && mPCM_Config->NoOfChannels == 2) {
        // Set ramp on ch 1
        TG_RampVolume(mHandle, mPCM_Config, startVolume, endVolume, 1, 20);
      }
      break;
  }
}


void METH(setMute)(t_bool muteState, t_uint32 nChannel)
{
  int time;
  PRINTF("tonegen_nmfil_host_tonegen::setMute()\n");

  if (mReset)
    time = 0;
  else
    time = 20;

  switch (nChannel) {
    case 0:
      if (muteState) {
        // Mute on ch 0
        TG_SetVolume(mHandle, mPCM_Config, 0, nChannel, time);
      }
      else {
        // Mute off ch 0, restore volume
        TG_SetVolume(mHandle, mPCM_Config, mVolume[0], nChannel, time);
      }
      mMute[0] = muteState;
      break;
    case 1:
      if (muteState) {
        // Mute on ch 1
        TG_SetVolume(mHandle, mPCM_Config, 0, nChannel, time);
      }
      else {
        // Mute off ch 1, restore volume
        TG_SetVolume(mHandle, mPCM_Config, mVolume[1], nChannel, time);
      }
      mMute[1] = muteState;
      break;
    case OMX_ALL:
      if (muteState) {
        // Mute on ch 0
        TG_SetVolume(mHandle, mPCM_Config, 0, 0, time);
      }
      else {
        // Mute off ch 0, restore volume
        TG_SetVolume(mHandle, mPCM_Config, mVolume[0], 0, time);
      }
      if (mPCM_Config->NoOfChannels == 2) {
        if (muteState) {
          // Mute on ch 1
          TG_SetVolume(mHandle, mPCM_Config, 0, 1, time);
        }
        else {
          // Mute off ch 1, restore volume
          TG_SetVolume(mHandle, mPCM_Config, mVolume[1], 1, time);
        }
      }
      mMute[0] = muteState;
      mMute[1] = muteState;
      break;
  }
}

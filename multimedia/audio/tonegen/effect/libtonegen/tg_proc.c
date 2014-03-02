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
#include <stdlib.h>
#include "OMX_Core.h"
#include "t_tonegen.h"
#include "tg_types.h"
#include "tg_tones.h"

#define TONEGEN_FULL_VOLUME 0x7FFF


// #define LOG_DEBUG

#ifdef LOG_DEBUG
#include <stdio.h>
#define LOGD printf("[Tonegen] "); printf
#else
#define LOGD(x,...)
#endif



TG_Handle_t* TG_Create(void)
{
  TG_Process_t *TG_p = NULL;
  LOGD("TG_Create()\n");

  TG_p = (TG_Process_t*)calloc(1, sizeof(TG_Process_t));
  if (TG_p == NULL) {
    LOGD("Create: MEMORY ALLOCATION ERROR - A ToneGen struct could not be allocated.\n");
    return NULL;
  }

  TG_p->State = TG_STATE_CREATED;
  TG_p->FadingState = TG_FADING_STATE_NONE;
  TG_p->channelVolume[0].Volume = TONEGEN_FULL_VOLUME;
  TG_p->channelVolume[1].Volume = TONEGEN_FULL_VOLUME;

  return (TG_Handle_t*)TG_p;
}


void TG_SetVolume(TG_Handle_t* TG_Handle_p,
                  const TG_PCM_Config_t* const PCM_Config_p,
                  sint16 Volume,
                  uint32 Channel,
                  uint32 Time)
{
  TG_Process_t* TG_p = (TG_Process_t*)TG_Handle_p;
  boolean Fading[2];
  LOGD("TG_SetVolume()\n");

  switch (TG_p->FadingState) {
    case TG_FADING_STATE_FADING_LEFT:
      Fading[0] = TRUE;
      Fading[1] = FALSE;
      break;
    case TG_FADING_STATE_FADING_RIGHT:
      Fading[0] = FALSE;
      Fading[1] = TRUE;
      break;
    case TG_FADING_STATE_FADING_BOTH:
      Fading[0] = TRUE;
      Fading[1] = TRUE;
      break;
    default:
      Fading[0] = FALSE;
      Fading[1] = FALSE;
      break;
  }

  switch (Channel) {
    case 0:
      if (Time == 0) {
        // Do an instant volume change
        TG_p->channelVolume[0].Volume = Volume;
        Fading[0] = FALSE;
      }
      else {
        if (Fading[0]) {
          // A ramp is already ongoing
          TG_p->channelVolume[0].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[0].FadeSampleCounter = 0;
          TG_p->channelVolume[0].FadingCoeff = (Volume - TG_p->channelVolume[0].FadingOffset) /
                                                 TG_p->channelVolume[0].NumberOfSamplesToFade;
          TG_p->channelVolume[0].Volume = Volume;
        }
        else if (Volume != TG_p->channelVolume[0].Volume) {
          // A ramp is not ongoing
          TG_p->channelVolume[0].FadingOffset = TG_p->channelVolume[0].Volume;
          TG_p->channelVolume[0].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[0].FadeSampleCounter = 0;
          TG_p->channelVolume[0].FadingCoeff = (Volume - TG_p->channelVolume[0].FadingOffset) /
                                                TG_p->channelVolume[0].NumberOfSamplesToFade;
          TG_p->channelVolume[0].Volume = Volume;
          Fading[0] = TRUE;
        }
      }
      break;
    case 1:
      if (Time == 0) {
        // Do a instant volume change
        TG_p->channelVolume[1].Volume = Volume;
        Fading[1] = FALSE;
      }
      else {
        if (Fading[1]) {
          // A ramp is already ongoing
          TG_p->channelVolume[1].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[1].FadeSampleCounter = 0;
          TG_p->channelVolume[1].FadingCoeff = (Volume - TG_p->channelVolume[1].FadingOffset) /
                                                TG_p->channelVolume[1].NumberOfSamplesToFade;
          TG_p->channelVolume[1].Volume = Volume;
        }
        else if (Volume != TG_p->channelVolume[1].Volume) {
          // A ramp is not ongoing
          TG_p->channelVolume[1].FadingOffset = TG_p->channelVolume[1].Volume;
          TG_p->channelVolume[1].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[1].FadeSampleCounter = 0;
          TG_p->channelVolume[1].FadingCoeff = (Volume - TG_p->channelVolume[1].FadingOffset) /
                                                TG_p->channelVolume[1].NumberOfSamplesToFade;
          TG_p->channelVolume[1].Volume = Volume;
          Fading[1] = TRUE;
        }
      }
      break;
    case OMX_ALL:
      if (Time == 0) {
        // Do a instant volume change
        TG_p->channelVolume[0].Volume = Volume;
        TG_p->channelVolume[1].Volume = Volume;
        Fading[0] = FALSE;
        Fading[1] = FALSE;
      }
      else {
        // set volume for ch0
        if (Fading[0]) {
          // A ramp is already ongoing
          TG_p->channelVolume[0].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[0].FadeSampleCounter = 0;
          TG_p->channelVolume[0].FadingCoeff = (Volume - TG_p->channelVolume[0].FadingOffset) /
                                                TG_p->channelVolume[0].NumberOfSamplesToFade;
          TG_p->channelVolume[0].Volume = Volume;
        }
        else if (Volume != TG_p->channelVolume[0].Volume) {
          // A ramp is not ongoing
          TG_p->channelVolume[0].FadingOffset = TG_p->channelVolume[0].Volume;
          TG_p->channelVolume[0].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[0].FadeSampleCounter = 0;
          TG_p->channelVolume[0].FadingCoeff = (Volume - TG_p->channelVolume[0].FadingOffset) /
                                                TG_p->channelVolume[0].NumberOfSamplesToFade;
          TG_p->channelVolume[0].Volume = Volume;
          Fading[0] = TRUE;
        }

        // set volume for ch1
        if (Fading[1]) {
          // A ramp is already ongoing
          TG_p->channelVolume[1].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[1].FadeSampleCounter = 0;
          TG_p->channelVolume[1].FadingCoeff = (Volume - TG_p->channelVolume[1].FadingOffset) /
                                                TG_p->channelVolume[1].NumberOfSamplesToFade;
          TG_p->channelVolume[1].Volume = Volume;
        }
        else if (Volume != TG_p->channelVolume[1].Volume) {
          // A ramp is not ongoing
          TG_p->channelVolume[1].FadingOffset = TG_p->channelVolume[1].Volume;
          TG_p->channelVolume[1].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[1].FadeSampleCounter = 0;
          TG_p->channelVolume[1].FadingCoeff = (Volume - TG_p->channelVolume[1].FadingOffset) /
                                                 TG_p->channelVolume[1].NumberOfSamplesToFade;
          TG_p->channelVolume[1].Volume = Volume;
          Fading[1] = TRUE;
        }
      }
      break;
    default:
      LOGD("\tERROR: Incorrect channel\n");
      break;
  }

  // set fading state
  if (Fading[0] && Fading[1])
    TG_p->FadingState = TG_FADING_STATE_FADING_BOTH;
  else if (Fading[0])
    TG_p->FadingState = TG_FADING_STATE_FADING_LEFT;
  else if (Fading[1])
    TG_p->FadingState = TG_FADING_STATE_FADING_RIGHT;
  else {
    if ((PCM_Config_p->NoOfChannels == 1 && TG_p->channelVolume[0].Volume == 0) ||
        (TG_p->channelVolume[0].Volume == 0 && TG_p->channelVolume[1].Volume == 0))
      TG_p->FadingState = TG_FADING_STATE_FADED;
    else
      TG_p->FadingState = TG_FADING_STATE_NONE;
  }
}


void TG_RampVolume(TG_Handle_t* TG_Handle_p,
                   const TG_PCM_Config_t* const PCM_Config_p,
                   sint16 StartVolume,
                   sint16 EndVolume,
                   uint32 Channel,
                   uint32 Time)
{
  TG_Process_t* TG_p = (TG_Process_t*)TG_Handle_p;
  boolean Fading[2];
  LOGD("TG_RampVolume()\n");

  switch (TG_p->FadingState) {
    case TG_FADING_STATE_FADING_LEFT:
      Fading[0] = TRUE;
      Fading[1] = FALSE;
      break;
    case TG_FADING_STATE_FADING_RIGHT:
      Fading[0] = FALSE;
      Fading[1] = TRUE;
      break;
    case TG_FADING_STATE_FADING_BOTH:
      Fading[0] = TRUE;
      Fading[1] = TRUE;
      break;
    default:
      Fading[0] = FALSE;
      Fading[1] = FALSE;
      break;
  }

  switch (Channel) {
    case 0:
      if (Time == 0) {
        // Do a instant volume change
        TG_p->channelVolume[0].Volume = EndVolume;
        Fading[0] = FALSE;
      }
      else {
        if (Fading[0] &&
            TG_p->channelVolume[0].Volume == StartVolume) {
          // A ramp is already ongoing and a smooth transition is expecte
          TG_p->channelVolume[0].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[0].FadeSampleCounter = 0;
          TG_p->channelVolume[0].FadingCoeff = (EndVolume - TG_p->channelVolume[0].FadingOffset) /
                                                TG_p->channelVolume[0].NumberOfSamplesToFade;
          TG_p->channelVolume[0].Volume = EndVolume;
        }
        else {
          // A ramp is not ongoing or out of order start value
          TG_p->channelVolume[0].FadingOffset = StartVolume;
          TG_p->channelVolume[0].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[0].FadeSampleCounter = 0;
          TG_p->channelVolume[0].FadingCoeff = (EndVolume - StartVolume) /
                                                TG_p->channelVolume[0].NumberOfSamplesToFade;
          TG_p->channelVolume[0].Volume = EndVolume;
          Fading[0] = TRUE;
        }
      }
      break;
    case 1:
      if (Time == 0) {
        // Do a instant volume change
        TG_p->channelVolume[1].Volume = EndVolume;
        Fading[1] = FALSE;
      }
      else {
        if (Fading[1] &&
            TG_p->channelVolume[1].Volume == StartVolume) {
          // A ramp is already ongoing and a smooth transition is expecte
          TG_p->channelVolume[1].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[1].FadeSampleCounter = 0;
          TG_p->channelVolume[1].FadingCoeff = (EndVolume - TG_p->channelVolume[1].FadingOffset) /
                                                TG_p->channelVolume[1].NumberOfSamplesToFade;
          TG_p->channelVolume[1].Volume = EndVolume;
        }
        else {
          // A ramp is not ongoing or out of order start value
          TG_p->channelVolume[1].FadingOffset = StartVolume;
          TG_p->channelVolume[1].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[1].FadeSampleCounter = 0;
          TG_p->channelVolume[1].FadingCoeff = (EndVolume - StartVolume) /
                                                TG_p->channelVolume[1].NumberOfSamplesToFade;
          TG_p->channelVolume[1].Volume = EndVolume;
          Fading[1] = TRUE;
        }
      }
      break;
    case OMX_ALL:
      if (Time == 0) {
        // Do a instant volume change
        TG_p->channelVolume[0].Volume = EndVolume;
        TG_p->channelVolume[1].Volume = EndVolume;
        Fading[0] = FALSE;
        Fading[1] = FALSE;
      }
      else {
        // set volume for ch0
        if (Fading[0] &&
            TG_p->channelVolume[0].Volume == StartVolume) {
          // A ramp is already ongoing and a smooth transition is expecte
          TG_p->channelVolume[0].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[0].FadeSampleCounter = 0;
          TG_p->channelVolume[0].FadingCoeff = (EndVolume - TG_p->channelVolume[0].FadingOffset) /
                                                TG_p->channelVolume[0].NumberOfSamplesToFade;
          TG_p->channelVolume[0].Volume = EndVolume;
        }
        else {
          // A ramp is not ongoing or out of order start value
          TG_p->channelVolume[0].FadingOffset = StartVolume;
          TG_p->channelVolume[0].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[0].FadeSampleCounter = 0;
          TG_p->channelVolume[0].FadingCoeff = (EndVolume - StartVolume) /
                                                TG_p->channelVolume[0].NumberOfSamplesToFade;
          TG_p->channelVolume[0].Volume = EndVolume;
          Fading[0] = TRUE;
        }

        // set volume for ch1
        if (Fading[1] &&
            TG_p->channelVolume[1].Volume == StartVolume) {
          // A ramp is already ongoing and a smooth transition is expecte
          TG_p->channelVolume[1].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[1].FadeSampleCounter = 0;
          TG_p->channelVolume[1].FadingCoeff = (EndVolume - TG_p->channelVolume[1].FadingOffset) /
                                                TG_p->channelVolume[1].NumberOfSamplesToFade;
          TG_p->channelVolume[1].Volume = EndVolume;
        }
        else {
          // A ramp is not ongoing or out of order start value
          TG_p->channelVolume[1].FadingOffset = StartVolume;
          TG_p->channelVolume[1].NumberOfSamplesToFade = (uint32)((Time * PCM_Config_p->Freq) / 1000);
          TG_p->channelVolume[1].FadeSampleCounter = 0;
          TG_p->channelVolume[1].FadingCoeff = (EndVolume - StartVolume) /
                                                TG_p->channelVolume[1].NumberOfSamplesToFade;
          TG_p->channelVolume[1].Volume = EndVolume;
          Fading[1] = TRUE;
        }
      }
      break;
    default:
      LOGD("\tERROR: Incorrect channel\n");
      break;
  }

  // set fading state
  if (Fading[0] && Fading[1])
    TG_p->FadingState = TG_FADING_STATE_FADING_BOTH;
  else if (Fading[0])
    TG_p->FadingState = TG_FADING_STATE_FADING_LEFT;
  else if (Fading[1])
    TG_p->FadingState = TG_FADING_STATE_FADING_RIGHT;
  else {
    if ((PCM_Config_p->NoOfChannels == 1 && TG_p->channelVolume[0].Volume == 0) ||
        (TG_p->channelVolume[0].Volume == 0 && TG_p->channelVolume[1].Volume == 0))
      TG_p->FadingState = TG_FADING_STATE_FADED;
    else
      TG_p->FadingState = TG_FADING_STATE_NONE;
  }
}


TG_Result_t TG_SetTone(TG_Handle_t* TG_Handle_p,
                       const TG_PCM_Config_t* const PCM_Config_p,
                       const TG_JobDescriptor_t* const Job_p)
{
  TG_Process_t* TG_p = (TG_Process_t*)TG_Handle_p;

  LOGD("TG_SetTone()\n");

  if (TG_p->CurJob != NULL) {
    free(TG_p->CurJob);
  }
  TG_p->CurJob = Job_p;

  switch (Job_p->JobId) {
    //--------------
    case TG_JOB_ID_SIMPLE_TONE:
    case TG_JOB_ID_DTMF_TONE:
    case TG_JOB_ID_OWN_TONE:
      TG_Tones_Init(TG_p, PCM_Config_p, Job_p);
      TG_p->State=TG_STATE_RUNNING;
      break;

    // Not implemented
    case TG_JOB_ID_CUSTOM_TONE:
    case TG_JOB_ID_COMFORT_TONE:
    case TG_JOB_ID_KEYCLICK:
    default:
      return TG_RESULT_NOT_IMPLEMENTED;
  }

  return TG_RESULT_OK;
}


void TG_NextFrame(TG_Handle_t* TG_Handle_p,
                  TG_PCM_Config_t* PCM_Config_p,
                  OMX_BUFFERHEADERTYPE* BufHdr_p)
{
  TG_Process_t* TG_p = (TG_Process_t*)TG_Handle_p;
  sint16* SampleBuf_p;
  uint32 NoOfSamples;

  // Setup buffer information (sample oriented)...
  SampleBuf_p = (sint16*)BufHdr_p->pBuffer;
  if ((uint32)PCM_Config_p->Interval > PCM_Config_p->NoOfChannels) {
    // In this case we have holes in the buffer...
    NoOfSamples = BufHdr_p->nAllocLen / (PCM_Config_p->Interval * sizeof(sint16));
  }
  else {
    NoOfSamples = BufHdr_p->nAllocLen / (PCM_Config_p->NoOfChannels * sizeof(sint16));
  }

  // Check state
  switch (TG_p->State) {
    case TG_STATE_RUNNING:
      // Check type of generation
      switch (TG_p->CurJob->JobId) {
        case TG_JOB_ID_SIMPLE_TONE:
        case TG_JOB_ID_DTMF_TONE:
        case TG_JOB_ID_OWN_TONE:
          TG_Tones_NextFrame(TG_p, PCM_Config_p, SampleBuf_p, 0, NoOfSamples);
          break;

        // Not implemented
        case TG_JOB_ID_KEYCLICK:
        case TG_JOB_ID_COMFORT_TONE:
        case TG_JOB_ID_CUSTOM_TONE:

        default:
          // unknown type
          break;
      } // switch
      break;

    case TG_STATE_IDLE:
    case TG_STATE_STOPPING:
    case TG_STATE_CREATED:
    case TG_STATE_DESTROYED:

    default:
      // Do nothing
      LOGD("TG_NextFrame() called in wrong state\n");
      break;
  } // switch
}


void TG_Reset(TG_Handle_t* TG_Handle_p, const TG_PCM_Config_t* const PCM_Config_p)
{
  TG_Process_t* TG_p = (TG_Process_t*)TG_Handle_p;

  LOGD("TG_Reset()\n");

  // end all on-going ramps
 if ((PCM_Config_p->NoOfChannels == 1 && TG_p->channelVolume[0].Volume == 0) ||
     (TG_p->channelVolume[0].Volume == 0 && TG_p->channelVolume[1].Volume == 0))
   TG_p->FadingState = TG_FADING_STATE_FADED;
 else
   TG_p->FadingState = TG_FADING_STATE_NONE;
}


void TG_Destroy(TG_Handle_t** TG_Handle_pp)
{
  LOGD("TG_Destroy()\n");

  TG_Process_t* TG_p = (TG_Process_t*)*TG_Handle_pp;
  if (TG_p->CurJob != NULL) {
    free(TG_p->CurJob);
  }
  free(*TG_Handle_pp);
  *TG_Handle_pp = NULL;
}

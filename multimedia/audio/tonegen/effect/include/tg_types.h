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
#ifndef INCLUSION_GUARD_TG_TYPES_H
#define INCLUSION_GUARD_TG_TYPES_H

#include "t_basicdefinitions.h"
#include "t_tonegen.h"
#include "tg_defs.h"

/**
 * ToneGen process state machine
 */
TYPEDEF_ENUM {
  TG_STATE_CREATED,
  TG_STATE_IDLE,
  TG_STATE_RUNNING,
  TG_STATE_STOPPING,
  TG_STATE_DESTROYED
} SIGNED_ENUM8(TG_State_t);

/**
 * ToneGen fading state machine
 */
TYPEDEF_ENUM {
  TG_FADING_STATE_NONE,
  TG_FADING_STATE_FADING_LEFT,
  TG_FADING_STATE_FADING_RIGHT,
  TG_FADING_STATE_FADING_BOTH,
  TG_FADING_STATE_FADED
} SIGNED_ENUM8(TG_FadingState_t);


typedef struct
{
  sint16 Volume;
  // Sample counter for the current tone this counter is reset when fade-out is started
  uint32 FadeSampleCounter;
  // Number of samples to fade
  sint32 NumberOfSamplesToFade;
  // Offset used for fading
  sint16 FadingOffset;
  // Rate for fading
  sint16 FadingCoeff;
} TG_ChannelVolume_t;


/**
 * Definition of the private ToneGen struct
 *
 * @param State    Current state of the tone generation state machine
 * @param CurJob   Current Job
 */
typedef struct {
  TG_State_t State;
  TG_JobHandle_t CurJob;

//  boolean DelayedStop;

#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE
  // Sound specific
  uint32  Amplitude[TG_NBR_SIMULTANEOUS_TONES];
#endif
  // Buffer specific
  uint32  NoOfSamplesPerPeriod[TG_NBR_SIMULTANEOUS_TONES];
  uint32  PeriodBufIdx[TG_NBR_SIMULTANEOUS_TONES];

  // Fix point offset add
  uint32  PeriodBufIdxAdd[TG_NBR_SIMULTANEOUS_TONES];
#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE
  // Fix point offset add, number of bits for integer part
  uint8   PeriodBufIdxAddFracBits[TG_NBR_SIMULTANEOUS_TONES];
#endif
  sint16* PeriodBuf_p[TG_NBR_SIMULTANEOUS_TONES];

  // Comfort Tone Description
  // CEPT, ANSI or Japanese
//  TG_ComfortToneType_t ComfortToneType;
  // BUSY, RING etc
//  TG_ComfortTone_t ComfortTone;
  // Pointer to scheme
//  const ComfortScheme_t* ComfortSchemeMatrix;
  // Nof repeats
//  uint16 ComfortNoOfRepeats;
  // Scheme indesx
//  uint32 ComfortSchemeIndex;
  // Sample counter
//  uint32 ComfortSchemeSampleCount;
  // Flag if new scheme data should be loaded
//  uint32 ComfortSchemeLoad;
  // Current duration (can also be infinite)
//  uint16 ComfortDuration;

  // Keeps track of what state the fade is in
  TG_FadingState_t FadingState;

  // Automatic start of fade-out is to be used
//  boolean ComfortToneFade;
  // When the fade-out should start (no. of samples)
//  uint32 StartFadeOut;

  TG_ChannelVolume_t channelVolume[2];

} TG_Process_t;

#endif //INCLUSION_GUARD_TG_TYPES_H

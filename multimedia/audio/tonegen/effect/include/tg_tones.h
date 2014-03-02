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
#ifndef INCLUSION_GUARD_TG_TONES_H
#define INCLUSION_GUARD_TG_TONES_H

#include "t_basicdefinitions.h"
#include "tg_types.h"

#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE

// Limit to use min 512 table entries = 1kByte
#define TG_MIN_NBR_BITS_FOR_TONE_WAVE_TABLE   9
// Limit to use max 1k table entries = 2kByte
#define TG_MAX_NBR_BITS_FOR_TONE_WAVE_TABLE  10

#else

#define TG_FIXED_WAVETABLE_INT_BITS 10
extern const uint16 TG_FixedWaveTable1024[1 << TG_FIXED_WAVETABLE_INT_BITS];
#define TG_FIXED_WAVETABLE_FRAC_BITS (32 - TG_FIXED_WAVETABLE_INT_BITS)

#endif //TG_USE_DYNAMIC_TONE_WAVETABLE


#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE
// static const float
#define TG_PI2 (2 * 3.1415926535897932384626433832795f)
#endif

// The fade in/out duration (ms)
#define TG_FADING_DURATION 20

#ifdef TG_USE_DYNAMIC_TONE_WAVETABLE
/*************************************************************
* TG_GetMSB
**************************************************************/
uint8 TG_GetMSB(uint32 Val);
#endif

/**
 * Init Tones
 */
void TG_Tones_Init(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, const TG_JobDescriptor_t* const Job_p);

/**
 * Called when reset a new tone (used internally and by comfort tone)
 */
void TG_Tones_Set(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, sint32 NbrTones, const uint32* const Frq);

/**
 * Fill next frame with PCM output
 */
void TG_Tones_NextFrame(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, sint16* SampleBuf_p, uint32 Offset, uint32 NoOfSamples);

/**
 * Fill next frame with zero
 */
void TG_Tones_ClearFrame(TG_Process_t *TG_p, const TG_PCM_Config_t* const PCM_Config_p, sint16* SampleBuf_p, uint32 Offset, uint32 NoOfSamples);

/**
 * Check if Tone is valid
 */
boolean ToneGen_IsValidTone(TG_Tone_t Tone);

#endif //INCLUSION_GUARD_TG_TONES_H

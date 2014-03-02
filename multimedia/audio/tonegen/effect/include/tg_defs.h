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
#ifndef INCLUSION_GUARD_TG_DEFS_H
#define INCLUSION_GUARD_TG_DEFS_H

// Number of tones that can be generated simulataneously
#define TG_NBR_SIMULTANEOUS_TONES  4

// Max amplitude for PCM sample
#define TG_PCM_SAMPLE_MAX_AMPLITUDE          0x7fff
// Quarter max, use this if adding 3 (or 4) waveforms.
#define TG_PCM_SAMPLE_QUARTER_MAX_AMPLITUDE  (TG_PCM_SAMPLE_MAX_AMPLITUDE / 4)

// Limit to use max 256 table entries for keyclick = 512 bytes
#define TG_MAX_NBR_BITS_FOR_KEYCLICK_WAVE_TABLE  8

// If tone wave table should be allocated and calculated in runtime
//#define TG_USE_DYNAMIC_TONE_WAVETABLE

#endif //INCLUSION_GUARD_TG_DEFS_H

#ifndef _OMX_STE_NOISE_REDUCTION_H_
#define _OMX_STE_NOISE_REDUCTION_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     omx_ste_speech_proc.h
 * \brief    OMX definition of parameter and config definition supported by the
 *           Noise Reduction
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include <OMX_Types.h>
#include <OMX_Core.h>
#include "AFM_Index.h"

typedef enum OMX_STE_NOISE_REDUCTION_INDEX {
	OMX_STE_IndexConfigAudioNoiseReductionFirstIndex = AFM_IndexStartComponentSpecificExtensions + 736,
	OMX_STE_IndexConfigAudioNoiseReduction,  /**< reference: OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE */
	OMX_STE_IndexConfigAudioNoiseReductionEndIndex
} OMX_STE_NOISE_REDUCTION_INDEX;

typedef struct OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE {
  OMX_U32         nSize;             
  OMX_VERSIONTYPE nVersion;  
  OMX_U32         nPortIndex;    
  OMX_BOOL        bEnable;

  OMX_BOOL        bUseSeparateSettings;

  OMX_S16         nAdaptiveHighPassFilter;
  OMX_S16         nDesiredNoiseReduction;
  OMX_S16         nNrCutOffFrequency;
  OMX_S16         nNoiseReductionDuringSpeech;
  OMX_S16         nNoiseReductionTradeoff;
  OMX_S16         nNoiseFloorPower;
  OMX_S16         nHighPassFilterSnrThreshold;
  OMX_S16         nHighPassCutOffMargin;

  OMX_S16         nAdaptiveHighPassFilterChannel2;
  OMX_S16         nDesiredNoiseReductionChannel2;
  OMX_S16         nNrCutOffFrequencyChannel2;
  OMX_S16         nNoiseReductionDuringSpeechChannel2;
  OMX_S16         nNoiseReductionTradeoffChannel2;
  OMX_S16         nNoiseFloorPowerChannel2;
  OMX_S16         nHighPassFilterSnrThresholdChannel2;
  OMX_S16         nHighPassCutOffMarginChannel2;

} OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE;

#endif // _OMX_STE_NOISE_REDUCTION_H_












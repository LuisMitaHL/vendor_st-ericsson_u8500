#ifndef _OMX_STE_SPEECH_PROC_H_
#define _OMX_STE_SPEECH_PROC_H_
#include "OMX_Types.h"
#include "OMX_Core.h"

enum OMX_STE_SPEECH_PROC_INDEX {
    OMX_STE_IndexConfigAudioFirstIndex = 11000,
    OMX_STE_IndexConfigAudioTxSpeechEnhancement,  /**< reference: OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE */
    OMX_STE_IndexConfigAudioRxSpeechEnhancement, /**< reference: OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE */
    OMX_STE_IndexConfigAudioTxDRC, /**< reference: OMX_STE_CONFIG_AUDIO_DRCTYPE */
    OMX_STE_IndexConfigAudioRxDRC, /**< reference: OMX_STE_CONFIG_AUDIO_DRCTYPE */
    OMX_STE_IndexConfigAudioTxCTM, /**< reference: OMX_STE_CONFIG_AUDIO_CTMTYPE */
    OMX_STE_IndexConfigAudioRxCTM, /**< reference: OMX_STE_CONFIG_AUDIO_CTMTYPE */
    OMX_STE_IndexConfigAudioTransducerEqualizer, /**< reference: OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE */
    OMX_STE_IndexConfigAudioEndIndex
};

typedef struct STE_ConfigAudioEchoControl_t {
  // General
  OMX_S16  nEnabled;
  OMX_S16  nLinearEchoCancellation;
  OMX_S16  nEchoDetection;
  OMX_S16  nHighPassFilter;
  OMX_S16  nResidualEchoControl;
  OMX_S16  nComfortNoise;
  OMX_S16  nPureDelay;
  // Linear modelling
  OMX_S16  nFilterLength;
  OMX_S16  nShadowFilterLength;
  OMX_S16  nMaxEchoPathGain;
  // General residual echo control
  OMX_S16  nAdaptiveResidualLoss;
  OMX_S16  nRxResidualLossRegressor[32];
  OMX_S16  nNonlinearSpeakerModelling;
  OMX_S16  nResidualEchoMargin;
  // Reverberation
  OMX_S16  nEchoDecayTime;
  // Echo reduction
  OMX_S16  nMaxResidualLoss[32];
  // Loudspeaker nonlineararities
  OMX_S16  nNonLinearEchoDecayTime;
  OMX_S16  nHarmonicGains[6];
  OMX_S16  nFundamentalGains[32];
  OMX_S16  nHarmonicActivationLevel;
  OMX_S16  nSpectralLeakageGains[32];
  OMX_S16  nLeakageActivationLevel;
  // Transparency of residual echo control
  OMX_S16  nSpectralMaskingMargin;
  OMX_S16  nTemporalMaskingTime;
  OMX_S16  nResidualEchoReleaserRate;
} STE_ConfigAudioEchoControl_t;

typedef struct STE_ConfigTxNoiseReduction_t {
  OMX_S16  nEnabled;
  OMX_S16  nAdaptiveHighPassFilter;
  OMX_S16  nReduceFarField;
  OMX_S16  nReduceWindNoise;
  OMX_S16  nDesiredNoiseReduction;
  OMX_S16  nNoiseReductionDuringSpeech;
  OMX_S16  nNoiseReductionTradeoff;
  OMX_S16  nNoiseFloorPower;
  OMX_S16  nReferenceMicrophoneChannel;
  OMX_S16  nHighPassFilterSnrThreshold;
  OMX_S16  nCutoffMargin;
} STE_ConfigTxNoiseReduction_t;

typedef struct SE_ConfigTxLevelControl_t {
  OMX_S16 nEnabled;
  OMX_S16 nAdaptiveSignalGain;
  OMX_S16 nNoiseAdaptiveGain;
  OMX_S16 nNoiseAdaptiveGainSlope;
  OMX_S16 nNoiseAdaptiveGainThreshold;
  OMX_S16 nTargetLevel;
  OMX_S16 nStaticGain;
} SE_ConfigTxLevelControl_t;

typedef struct OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE {
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
  OMX_BOOL bEnable;
  STE_ConfigAudioEchoControl_t sConfigEchoControl;
  STE_ConfigTxNoiseReduction_t sConfigTxNoiseReduction;
  SE_ConfigTxLevelControl_t sConfigTxLevelControl;
} OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE;


typedef struct STE_ConfigRxNoiseReduction_t {
  OMX_S16  nEnabled;
  OMX_S16  nAdaptiveHighPassFilter;
  OMX_S16  nDesiredNoiseReduction;
  OMX_S16  nNoiseReductionDuringSpeech;
  OMX_S16  nNoiseReductionTradeoff;
  OMX_S16  nNoiseFloorPower;
  OMX_S16  nHighPassFilterSnrThreshold;
  OMX_S16  nCutoffMargin;
} STE_ConfigRxNoiseReduction_t;


typedef struct STE_ConfigRxEqualizer_t {
  OMX_S16  nEnabled;
  OMX_S16  nSpeechEnhance;
  OMX_S16  nCalibrationMode;
  OMX_S16  nComplexityLevel;
  OMX_S16  nMinNoiseSpeechEnhance;
  OMX_S16  nSaturationMargin;
  OMX_S16  nMinimumTargetSNR;
  OMX_S16  nMaxGainTargetSNR;
  OMX_S16  nThresholdSNREnergyRedist;
  OMX_S16  nMaxGainEnergyRedistHf;
  OMX_S16  nMaxLossEnergyRedistLf;
  OMX_S16  nTxAcomp[40];
  OMX_S16  nRxAcomp[40];
} STE_ConfigRxEqualizer_t;

typedef struct OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bEnable;
    STE_ConfigRxNoiseReduction_t sConfigRxNoiseReduction;
    STE_ConfigRxEqualizer_t sConfigRxEqualizer;
} OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE;


typedef enum OMX_STE_AUDIO_DRCACCEPTANCELEVELTYPE {
   OMX_STE_AUDIO_DRCAcceptanceLevel90Percent = 1,
   OMX_STE_AUDIO_DRCAcceptanceLevel99Percent,
   OMX_STE_AUDIO_DRCAcceptanceLevel999Percent
} OMX_STE_AUDIO_DRCACCEPTANCELEVELTYPE;

typedef struct STE_ConfigAudioDRC_s {
    OMX_S16  nStaticCurvePointX[8];
    OMX_S16  nStaticCurvePointY[8];
    OMX_S16  nInputGain;
    OMX_S16  nOffsetGain;
    OMX_U16  nLevelDetectorAttackTime;
    OMX_U16  nLevelDetectorReleaseTime;
    OMX_U16  nGainProcessorAttackTime;
    OMX_U16  nGainProcessorReleaseTime;
    OMX_STE_AUDIO_DRCACCEPTANCELEVELTYPE  eAcceptanceLevel;
} STE_ConfigAudioDRC_t;

typedef struct OMX_STE_CONFIG_AUDIO_DRCTYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bEnable;
    STE_ConfigAudioDRC_t sConfig;
} OMX_STE_CONFIG_AUDIO_DRCTYPE;

typedef struct OMX_STE_CONFIG_AUDIO_CTMTYPE {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bEnable;
} OMX_STE_CONFIG_AUDIO_CTMTYPE;


typedef struct STE_ConfigAudioTransducerEqualizer_s {
  OMX_BOOL        same_FIR_l_r;
  OMX_BOOL        biquad_first;
  OMX_U16         biquad_gain_exp_l;
  OMX_U32         biquad_gain_mant_l;
  OMX_U16         biquad_gain_exp_r;
  OMX_U32         biquad_gain_mant_r;
  OMX_U16         FIR_gain_exp_l;
  OMX_U32         FIR_gain_mant_l;
  OMX_U16         FIR_gain_exp_r;
  OMX_U32         FIR_gain_mant_r;
  OMX_U16         nb_biquad_cells_per_channel;
  OMX_U16         nb_FIR_coefs_per_channel;
  struct
  {
     OMX_U16 b_exp;
     OMX_U32 b0;
     OMX_U32 b1;
     OMX_U32 b2;
     OMX_U32 a1;
     OMX_U32 a2;
  } biquad_cell[40];
    OMX_U32 FIR_coef[400];
} STE_ConfigAudioTransducerEqualizer_t;

typedef struct OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE {
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
  OMX_BOOL        bEnable;
  STE_ConfigAudioTransducerEqualizer_t sConfig;
} OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE;

#endif // _OMX_STE_SPEECH_PROC_H_












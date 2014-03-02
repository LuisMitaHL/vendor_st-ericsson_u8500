/*                                                                              
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.                      
 * This code is ST-Ericsson proprietary and confidential.                       
 * Any use of the code for whatever purpose is subject to                       
 * specific written permission of ST-Ericsson SA.                               
 */
#ifndef INCLUSION_GUARD_SPEECH_ENHANCEMENT_H
#define INCLUSION_GUARD_SPEECH_ENHANCEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************
 * COPYRIGHT (C)   ERICSSON AB 2010
 *
 * The copyright to the document(s) herein is the property of
 * Ericsson AB, Sweden.
 *
 * The document(s) may be used and/or copied only with the
 * written permission from Ericsson AB
 * or in accordance with the terms and conditions stipulated
 * in the agreement or contract under which the document(s)
 * have been supplied.
 ***********************************************************************
 * DESCRIPTION
 *
 * Public interface of Speech Enhancement module
 *
 ***********************************************************************/

#define SE_INTERFACE_REVISION  "Interface $Revision: 1213 $"

#define SE_STEREO_CHANNEL

#ifdef SE_STEREO_CHANNEL
    #define SE_MAX_NUMBER_OF_TX_CHANNELS      2
    #define SE_MAX_NUMBER_OF_RX_CHANNELS      2
    #define SE_MAX_NUMBER_OF_CROSS_CHANNELS   4 
    #define SE_MAX_NUMBER_OF_CHANNELS         2 
    #define SE_CHANNEL_STRING ". Stereo "
  #else
#ifdef SE_MULTI_CHANNEL
    #define SE_MAX_NUMBER_OF_TX_CHANNELS      2
    #define SE_MAX_NUMBER_OF_RX_CHANNELS      8
    #define SE_MAX_NUMBER_OF_CROSS_CHANNELS  16 
    #define SE_MAX_NUMBER_OF_CHANNELS         8 
    #define SE_CHANNEL_STRING ". Multi-channel "
#else
/* Confuguration for mono is the default */
#define SE_MAX_NUMBER_OF_TX_CHANNELS      1
#define SE_MAX_NUMBER_OF_RX_CHANNELS      1
#define SE_MAX_NUMBER_OF_CROSS_CHANNELS   1 
#define SE_MAX_NUMBER_OF_CHANNELS         1 
#define SE_CHANNEL_STRING ". Mono "
  #endif
#endif
/* Maximum number of channels in input parameter struct */
#define SE_PARAMETER_MAX_TX_CHANNELS      2
/***********************************************************************
 *
 * Enums data types
 *
 ***********************************************************************/
typedef enum {
   SE_SAMPLE_RATE_8000 = 0,
   SE_SAMPLE_RATE_16000,
   SE_SAMPLE_RATE_32000,
   SE_SAMPLE_RATE_48000
} SE_SampleRate_t;


typedef enum {
   SE_MONO = 1,
   SE_STEREO = 2,
   SE_THREE_CHANNELS = 3,
   SE_FOUR_CHANNELS = 4,
   SE_FIVE_CHANNELS = 5,
   SE_SIX_CHANNELS = 6,
   SE_SEVEN_CHANNELS = 7,
   SE_EIGHT_CHANNELS = 8
} SE_NumberOfChannels_t;

 
typedef enum {
   SE_NO_ERROR                         = (0),
   SE_PARAM_BELOW_LIMIT                = (1<<0),
   SE_PARAM_ABOVE_LIMIT                = (1<<1),
   SE_UNSUPPORTED_SAMPLE_RATE          = (1<<2),
   SE_UNSUPPORTED_NTXCH                = (1<<3),
   SE_UNSUPPORTED_NRXCH                = (1<<4),
   SE_INSUFFICIENT_MIC_CHANNELS        = (1<<5),
   SE_CONFLICTING_TX_CHANNEL_MAPPING   = (1<<6),
   SE_CONFLICTING_REF_CHANNEL_MAPPING  = (1<<7),
   SE_RESOLVED_INCONSISTENT_PARAMETERS = (1<<8),
   SE_NULL_POINTER_TO_OUTPUT           = (1<<9),
   SE_WRONG_NUMBER_OF_PARAMETERS       = (1<<10)
} SE_Return_t;


typedef enum {
  SE_MM_PROBE_ECHOFILTER_LEFTTX_LEFTRX,
  SE_MM_PROBE_ECHOFILTER_RIGHTTX_LEFTRX,
  SE_MM_PROBE_ECHOFILTER_LEFTTX_RIGHTRX,
  SE_MM_PROBE_ECHOFILTER_RIGHTTX_RIGHTRX,
  SE_MM_PROBE_RXEQ_TX_NOISESPECTRUM,
  SE_MM_PROBE_RXEQ_RX_SPEECHSPECTRUM
} SE_MM_Probe_t;


/***********************************************************************
 *
 * Public data structures for RX and TX speech enhancement compoennts
 *
 ***********************************************************************/
struct SE_TxComponent_t;
struct SE_RxComponent_t;

struct SE_RxExportContainer_t;
struct SE_TxExportContainer_t;

typedef struct {
  short echoEstimationEnabled;
  short echoSubtractionEnabled;
  short noiseReductionEnabled;
  short residualEchoControlEnabled;
  short signalLevelMeterEnabled;
  short txLevelControlEnabled;
  short nTxFrequencyResponses;
} SETX_Status_t;
typedef struct {
  short noiseReductionEnabled;
  short rxEqualizeEnabled;
  short adaptiveRxGainEnabled;
  short nRxFrequencyResponses;
} SERX_Status_t;

/***********************************************************************
 *
 * Externally controlled parameters for all functionalities
 *
 ***********************************************************************/

/* Echo Control */

typedef struct {
  // General
  short   enabled;
  short   linearEchoCancellation;
  short   echoDetection;
  //short   floatingShadowFilter;
  short   highPassFilter;
  short   residualEchoControl;
  short   comfortNoise;
  short   pureDelay;

  // Linear modelling
  short   filterLength;
  short   shadowFilterLength;
  short   maxEchoPathGain;
  //short   systemVarianceBootstrap;

  // General residual echo control
  short   adaptiveResidualLoss;
  short   rxResidualLossRegressor_p[40];
  //short   masterResidualGain_p[40];
  short   nonlinearSpeakerModelling;
  short   residualEchoMargin;

  // Reverberation
  short   echoDecayTime;

  // Echo reduction
  short   maxResidualLoss_p[40];

  // Loudspeaker nonlinearities
  short   nonLinearEchoDecayTime;
  short   harmonicGains_p[6];
  short   fundamentalGains_p[32];
  short   harmonicActivationLevel;
  short   spectralLeakageGains_p[40];
  short   leakageActivationLevel;

  // Transparency of residual echo control
  short   spectralMaskingMargin;
  //short   nearEndMaskingMargin;
  short   temporalMaskingTime;
  short   residualEchoReleaseRate;
} SE_EcParameters_t;


/* Noise Reduction */
typedef struct {
  short   enabled;
  short   adaptiveHighPassFilter;
  short   desiredNoiseReduction;
  short   nrCutOffFrequency;
  short   noiseReductionDuringSpeech;
  short   noiseReductionTradeoff;
  short   noiseFloorPower;
  short   highPassFilterSnrThreshold;
  short   highPassCutOffMargin;
} SE_NrParameters_t;


/* Tx Level Control */
typedef struct {
  short   enabled;
  short   adaptiveSignalGain;

  short   noiseAdaptiveGain;
  short   noiseAdaptiveGainSlope;
  short   noiseAdaptiveGainThreshold;

  short   targetLevel;
  short   staticGain;
} SE_TxLcParameters_t;


/* Multi microphone processing */
typedef struct {
  short   enabled;
  short   txChannelMapping_p[SE_PARAMETER_MAX_TX_CHANNELS];
  short   reduceFarField;
  short   reduceWindNoise;
  short   referenceMicrophoneChannel;
} SE_MmicParameters_t;


/* Rx Speech Enhancement */
typedef struct {
   short  enabled;
   short  speechEnhance;
   short  calibrationMode;
   short  complexityLevel;
   short  minNoiseSpeechEnhance;
   short  saturationMargin;
   short  minimumTargetSNR;
   short  maxGainTargetSNR;
   short  thresholdSNREnergyRedist;
   short  maxGainEnergyRedistHf;
   short  maxLossEnergyRedistLf;
   short  txAcomp_p[40];
   short  rxAcomp_p[40];
} SE_RxEqParameters_t;


/* Adaptive Rx Gain */
typedef struct {
  short   enabled;
  short   activateInNonSpeech;
  short   maxRxLoss;
  short   rxLossAttackTime;
  short   rxLossReleaseTime;
} SE_ArxgParameters_t;
/* Rx Peak Limiter */
typedef struct {
  short   enabled;
  short   limit;
  short   preLimitGain;
  short   postLimitGain;
  short   releaseTime;
} SE_PlimParameters_t;

/* Linear Variable Filter */
typedef struct {
  //short   enabled;
  short   lowDelay;
  //short   staticCompensation;
  //short   staticSpectrumGain_p[64];
  //short   staticGain;
} SE_LvfParameters_t;
// Aggregated parameters for properties in TX path
typedef struct {
  SE_EcParameters_t    EchoControl;
  SE_MmicParameters_t  MultiMicrophone;
  SE_TxLcParameters_t  TxLevelControl;
  SE_NrParameters_t    TxNoiseReduction;
  SE_LvfParameters_t   TxFilter;
} SE_TxParameters_t;

// Aggregated parameters for properties in RX path
typedef struct {
  SE_NrParameters_t    RxNoiseReduction;
  SE_RxEqParameters_t  RxEqualize;
  SE_LvfParameters_t   RxFilter;
  //SE_ArxgParameters_t  AdaptiveRxGain;
} SE_RxParameters_t;

// Aggregated parameters for SE
typedef struct {
  SE_RxParameters_t    Rx;
  SE_TxParameters_t    Tx;
} SE_Parameters_t;

/***********************************************************************
 *
 * Default parameters
 *
 ***********************************************************************/
extern const SE_EcParameters_t   SE_EcDefaultParameters;
extern const SE_LvfParameters_t  SE_RxfDefaultParameters;
extern const SE_LvfParameters_t  SE_TxfDefaultParameters;
extern const SE_NrParameters_t   SE_RxNrDefaultParameters;
extern const SE_NrParameters_t   SE_TxNrDefaultParameters;
extern const SE_TxLcParameters_t SE_TxLcDefaultParameters;
extern const SE_MmicParameters_t SE_MmicDefaultParameters;
extern const SE_RxEqParameters_t SE_RxEqDefaultParameters;
extern const SE_ArxgParameters_t  SE_ArxgDefaultParameters;


/***********************************************************************
 *
 * Public interfaces
 *
 ***********************************************************************/
/* RX */
struct SE_RxComponent_t* SERX_Construct(void);

SE_Return_t SERX_Configure(struct SE_RxComponent_t*     const RxComponent_p,
                            const SE_RxParameters_t*    const SeRxParameters_p,
                            const SE_SampleRate_t             sampleRate,
                            const SE_NumberOfChannels_t       nRxCh,
                            const SE_NumberOfChannels_t       nTxCh);

SE_Return_t SERX_DefaultParameters(SE_RxParameters_t*  const SeRxParameters_p);

SE_Return_t SERX_ValidateParameters(const SE_RxParameters_t*  const SeRxParameters_p,
                                    const SE_SampleRate_t           sampleRate,
                                    const SE_NumberOfChannels_t           nRxCh,
                                    const SE_NumberOfChannels_t           nTxCh);

void SERX_Destruct(struct SE_RxComponent_t* const RxComponent_p);

void SERX_Execute( const short**           const RxInPCM_pp,
                                short**           const RxOutPCM_pp,
                         struct SE_RxComponent_t* const RxComponent_p);

/* TX */
struct SE_TxComponent_t* SETX_Construct(void);

SE_Return_t SETX_Configure(struct SE_TxComponent_t*     const TxComponent_p,
                            const SE_TxParameters_t*    const EcParameters_p,
                            const SE_SampleRate_t             sampleRate,
                            const SE_NumberOfChannels_t       nRxCh,
                            const SE_NumberOfChannels_t       nTxCh,
                            const short                       nMicCh);

SE_Return_t SETX_DefaultParameters(SE_TxParameters_t*  const SeTxParameters_p);

SE_Return_t SETX_ValidateParameters(const SE_TxParameters_t*  const SeTxParameters_p,
                                    const SE_SampleRate_t           sampleRate,
                                    const SE_NumberOfChannels_t           nRxCh,
                                    const SE_NumberOfChannels_t           nTxCh,
                                    const short                     nMicCh);

void SETX_Destruct(struct SE_TxComponent_t* const TxComponent_p);

void SETX_Execute( const short**            const RxRefPcm_pp,
                          const short**            const MicPcm_pp,
                                short**            const SxOutPcm_pp,
                                short**            const LinOutPcm_pp,
                         struct SE_TxComponent_t*  const TxComponent_p);


/* Data sharing from RX to TX */
struct SE_RxExportContainer_t* SERX_ExportContainer_Construct(void);

void SERX_ExportContainer_Copy(      
                               struct SE_RxExportContainer_t*  const OutputRxExport_p,
                         const struct SE_RxExportContainer_t*  const InputRxExport_p);

void SERX_ExportContainer_Fill(
                              struct SE_RxExportContainer_t*  const RxExport_p,
                        const struct SE_RxComponent_t*        const RxComponent_p);

void SERX_ExportContainer_Empty(      
                              struct SE_RxExportContainer_t*  const RxExport_p,
                              struct SE_TxComponent_t*        const TxComponent_p);

void SERX_ExportContainer_Destruct(struct SE_RxExportContainer_t* const RxExport_p);

/* Data sharing from TX to RX */
struct SE_TxExportContainer_t* SETX_ExportContainer_Construct(void);

void SETX_ExportContainer_Copy(      
                               struct SE_TxExportContainer_t*  const OutputTxExport_p,
                         const struct SE_TxExportContainer_t*  const InputTxExport_p);

void SETX_ExportContainer_Fill(
                              struct SE_TxExportContainer_t*  const TxExport_p,
                        const struct SE_TxComponent_t*        const TxComponent_p);

void SETX_ExportContainer_Empty(      
                              struct SE_TxExportContainer_t*  const TxExport_p,
                              struct SE_RxComponent_t*        const RxComponent_p);

void SETX_ExportContainer_Destruct(struct SE_TxExportContainer_t* const TxExport_p);


/***********************************************************************
 *
 * Interfaces to internal data in SE
 *
 ***********************************************************************/
void SETX_GetShadowFilterOffset(
               short*                   shadowFilterOffset_p,   
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the echo path delay in milli seconds including pureDelay */
void SETX_GetEchoPathDelay(
               short*                    echoPathDelay_p,   
  const struct SE_TxComponent_t*  const TxComponent_p);
  
  /* Get the echo path gain centi Bell (range +/-100 dB) */
void SETX_GetEchoPathGain(
               short*                    echoPathGain_p,   
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the long and short term speech level in cBovl */
void SETX_GetTxSpeechLevel(
               short*                    longTimeSpeechLevel_p,
               short*                    shortTimeSpeechLevel_p,
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the long and short term RX speech level in cBovl */
void SETX_GetRxSpeechLevel(
               short*                    longTimeSpeechLevel_p,
               short*                    shortTimeSpeechLevel_p,
  const struct SE_RxComponent_t*  const RxComponent_p);
/* Get the spectrum of the linear residual */
short SETX_GetLinearResidualSpectrum(
        short**                          linearResidualSpectrum_pp,
  const short                           spectrumLength,
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the spectrum of the residual echo */
short SETX_GetResidualEchoSpectrum(
        short**                          residualEchoSpectrum_pp,
  const short                           spectrumLength,
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the spectrum of the near-end */
short SETX_GetNearEndSpectrum(
        short**                          nearEndSpectrum_pp,
  const short                           spectrumLength,
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the REC masking threshold in cBovl */
short SETX_GetRecMaskingThreshold(
        short**                          maskingThreshold_pp,
  const short                           spectrumLength,
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the REC masking spectrum in cBovl */
short SETX_GetRecMaskingSpectrum(
        short**                          maskingSpectrum_pp,
  const short                           spectrumLength,
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the REC frequency response in centiBell (range +/-100 dB) */
short SETX_GetRecFrequencyResponse(
        short**                          frequencyResponse_pp,
  const short                           spectrumLength,
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the TX LVF FIR filter in Q15 */
short SETX_GetLvfFilter(
        short*                          timeDomainFilter_p,
  const short                           filterLength,
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the RX LVF FIR filter in Q15 */
short SERX_GetLvfFilter(
        short*                          timeDomainFilter_p,
  const short                           filterLength,
  const struct SE_RxComponent_t*  const RxComponent_p);

/* Get the status of the SETX module */
void SETX_GetStatus(
        SETX_Status_t*            const SeStatus_p,
  const struct SE_TxComponent_t*  const TxComponent_p);

/* Get the status of the SERX module */
void SERX_GetStatus(
        SERX_Status_t*            const SeStatus_p,
  const struct SE_RxComponent_t*  const RxComponent_p);


/***********************************************************************
 *
 * Interfaces to information on SE
 *
 ***********************************************************************/
/* Get the revision of the parameter specification */
int SE_GetParameterRevision(
        char*           const parameterRevisionStr_p,
  const int                   stringLength);

/* Get the revision of the interface */
int SE_GetRevision(
        char*           const revisionStr_p,
  const int                   stringLength);

/* Print parameters to text string */
void SE_PrintParameters(      char*               const outTextString_p,
                        const int                       stringLength,
                        const SE_Parameters_t*    const SeParameters_p);

/* Parse text string to parameter structure */ 
void SE_ParseConfiguration(const char*             const configurationText_p,
                                 unsigned int            stringLength,
                                 SE_Parameters_t*  const SeParameters_p);
								 
/***********************************************************************
 *
 * int SE_MM_Probe
 *
 * Return value: number of element written to Buffer_p.
 * If Buffer_p == NULL return value gives needed data size.
 * In case of failure function returns negative value.
 * Buffer is filled with short integers using Q14.
 *
 ***********************************************************************/
int SE_MM_Probe(      short*                   const Buffer_p,
                const struct SE_TxComponent_t* const TxComponent_p,
                const struct SE_RxComponent_t* const RxComponent_p,
                const SE_MM_Probe_t                  ProbeType);


#ifdef __cplusplus
}
#endif

#endif

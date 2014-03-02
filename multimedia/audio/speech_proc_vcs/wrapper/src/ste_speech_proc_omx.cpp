
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     ste_speech_proc_omx.cpp
 * \brief    STE speech processing library
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "ste_speech_proc_omx.h"

#include <string.h>


// happily copied from DRC proxy
#define AFM_DRC_STATIC_CURVE_POINT_MAX         0
#define AFM_DRC_STATIC_CURVE_POINT_MIN        (-128 << 8)
#define AFM_DRC_ATTACK_RELEASE_TIME_MAX       (16000 << 1)
#define AFM_DRC_LEVEL_DETECTOR_TIME_DEFAULT    16
#define AFM_DRC_GAIN_PROCESSOR_TIME_DEFAULT    400
#define AFM_DRC_NUMBER_OF_STATIC_CURVE_POINTS  8

STE_SpeechProcLibrary::STE_SpeechProcLibrary()
{
  defaultTxSpeechEnhancement(&mTxSpe, OUTPUT_UPLINK_PORT);
  defaultRxSpeechEnhancement(&mRxSpe, OUTPUT_DOWNLINK_PORT);
  defaultDRC(&mTxDrc, OUTPUT_UPLINK_PORT);
  defaultDRC(&mRxDrc, OUTPUT_DOWNLINK_PORT);
  defaultCTM(&mRxCTM, OUTPUT_DOWNLINK_PORT);
  defaultCTM(&mTxCTM, OUTPUT_UPLINK_PORT);
  defaultTrEq(&mTxTrEq, OUTPUT_UPLINK_PORT);
  defaultComfortnoise(&mTxComfortnoise, OUTPUT_UPLINK_PORT);
  defaultComfortnoise(&mRxComfortnoise, OUTPUT_DOWNLINK_PORT);
  defaultCustAlg(&mRxCustemerType1, OUTPUT_DOWNLINK_PORT);
  defaultCustAlg(&mRxCustemerType2, OUTPUT_DOWNLINK_PORT);
  defaultCustAlg(&mTxCustemerType2, OUTPUT_UPLINK_PORT);
}

STE_SpeechProcLibrary::~STE_SpeechProcLibrary()
{
}

OMX_U32 STE_SpeechProcLibrary::getNbParamAndConfig(speech_proc_algo_type_t algo)
{
  switch(algo) {
	
  case UPLINK_ALGORITHMS:
	return OMX_STE_IndexConfigNbOfUplinkIndex;;
	break;
  case DOWNLINK_ALGORITHMS:
	return OMX_STE_IndexConfigNbOfUplinkIndex;;
	break;
  }
  return 0;
}
bool STE_SpeechProcLibrary::isBypass(speech_proc_algo_type_t algo)
{
  return false;
}

OMX_ERRORTYPE STE_SpeechProcLibrary::checkParameter(OMX_U32 index, OMX_PTR param)
{
  // no paramater supported
  return OMX_ErrorUnsupportedIndex;
}

OMX_ERRORTYPE STE_SpeechProcLibrary::checkConfig(OMX_U32 index, OMX_PTR config)
{
  OMX_ERRORTYPE error;
  switch(index) {
  case OMX_STE_IndexConfigAudioTxSpeechEnhancement:
	{
	  OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE *tx =
		(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE*) config;
	  if (tx->nSize == sizeof(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE) &&
		  tx->nPortIndex == OUTPUT_UPLINK_PORT) {
		error = OMX_ErrorNone;
		mTxSpe = *tx;
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
		  
	}
	break;
  case OMX_STE_IndexConfigAudioRxSpeechEnhancement:
	{
	  OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE *rx =
		(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE*) config;
	  if (rx->nSize == sizeof(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE) &&
		  rx->nPortIndex == OUTPUT_DOWNLINK_PORT) {
		error = OMX_ErrorNone;
		mRxSpe = *rx;
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	  
	}
	break;
  case OMX_STE_IndexConfigAudioTxCTM:
	{
	  OMX_STE_CONFIG_AUDIO_CTMTYPE *ctm =
		( OMX_STE_CONFIG_AUDIO_CTMTYPE*) config;
	  if (ctm->nSize == sizeof(OMX_STE_CONFIG_AUDIO_CTMTYPE) &&
		  ctm->nPortIndex == OUTPUT_UPLINK_PORT) {
		error = OMX_ErrorNone;
		mTxCTM = *ctm;
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;
  case OMX_STE_IndexConfigAudioTxDRC:
	{
	  OMX_STE_CONFIG_AUDIO_DRCTYPE *drc =
		( OMX_STE_CONFIG_AUDIO_DRCTYPE*) config;
	  if (drc->nSize == sizeof(OMX_STE_CONFIG_AUDIO_DRCTYPE) &&
		  drc->nPortIndex == OUTPUT_UPLINK_PORT) {
          error = checkDrc(drc);
          if (error == OMX_ErrorNone) {
		   mTxDrc = *drc;
           setDrcMinMax(mTxDrc);
         }
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;
  case OMX_STE_IndexConfigAudioRxCTM:
	{
	  OMX_STE_CONFIG_AUDIO_CTMTYPE *ctm =
		( OMX_STE_CONFIG_AUDIO_CTMTYPE*) config;
	  if (ctm->nSize == sizeof(OMX_STE_CONFIG_AUDIO_CTMTYPE) &&
		  ctm->nPortIndex == OUTPUT_DOWNLINK_PORT) {
		error = OMX_ErrorNone;
		mRxCTM = *ctm;
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;
  case OMX_STE_IndexConfigAudioRxDRC:
	{
      OMX_STE_CONFIG_AUDIO_DRCTYPE *drc =
		( OMX_STE_CONFIG_AUDIO_DRCTYPE*) config;
	  if (drc->nSize == sizeof(OMX_STE_CONFIG_AUDIO_DRCTYPE) &&
		  drc->nPortIndex == OUTPUT_DOWNLINK_PORT) {
        error = checkDrc(drc);
        if (error == OMX_ErrorNone) {
		  mRxDrc = *drc;
          setDrcMinMax(mRxDrc);
        }
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;
	
  case OMX_STE_IndexConfigAudioTransducerEqualizer:
	{
	  OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE *treq =
		(OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE*) config;
	  if (treq->nSize == sizeof(OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE) &&
		  treq->nPortIndex == OUTPUT_UPLINK_PORT)  {
		error = OMX_ErrorNone;
		mTxTrEq = *treq;
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;
  case OMX_STE_IndexConfigAudioRxComfortNoise:
	{
	  OMX_STE_AUDIO_CONFIG_COMFORT_NOISE *noise =
		( OMX_STE_AUDIO_CONFIG_COMFORT_NOISE*) config;

	  if (noise->nSize == sizeof(OMX_STE_AUDIO_CONFIG_COMFORT_NOISE) &&
		  noise->nPortIndex == OUTPUT_DOWNLINK_PORT)  {
		 error = OMX_ErrorNone;
		 mRxComfortnoise= *noise;
	  }

	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;
  case OMX_STE_IndexConfigAudioTxComfortNoise:
	{
	  OMX_STE_AUDIO_CONFIG_COMFORT_NOISE *noise =
		( OMX_STE_AUDIO_CONFIG_COMFORT_NOISE*) config;

	  if (noise->nSize == sizeof(OMX_STE_AUDIO_CONFIG_COMFORT_NOISE) &&
		  noise->nPortIndex == OUTPUT_UPLINK_PORT)  {
		 error = OMX_ErrorNone;
		 mTxComfortnoise= *noise;
	  }

      else error = OMX_ErrorUnsupportedSetting;
	}
	break;
  case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType1:
	{
	  OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE *tmp =
		(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE*) config;
	  if (tmp->nSize == sizeof(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE) &&
		  tmp->nPortIndex == OUTPUT_DOWNLINK_PORT)  {
		error = OMX_ErrorNone;
		mRxCustemerType1 = *tmp;
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
    break;
   case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType2:
	{
	  OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE *tmp =
		(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE*) config;
	  if (tmp->nSize == sizeof(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE) &&
		  tmp->nPortIndex == OUTPUT_DOWNLINK_PORT)  {
		error = OMX_ErrorNone;
		mRxCustemerType2 = *tmp;
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
    break;
    case OMX_STE_IndexConfigAudioTxSpeechProcCustomerType2:
	{
	  OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE *tmp =
		(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE*) config;
	  if (tmp->nSize == sizeof(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE) &&
		  tmp->nPortIndex == OUTPUT_UPLINK_PORT)  {
		error = OMX_ErrorNone;
		mTxCustemerType2 = *tmp;
	  }
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;
  default:
	error = OMX_ErrorUnsupportedIndex;
  }
  return error;
}


OMX_PTR STE_SpeechProcLibrary::getNextParameter(speech_proc_algo_type_t algo,
												OMX_U32 previous_index,
												OMX_U32 *current_index)
{
  // no parameter supported
  *current_index = previous_index + 1;
  return 0;
}

OMX_U32 STE_SpeechProcLibrary::nextIndex(speech_proc_algo_type_t algo,
										 OMX_U32 previous_index)
{
  int next_index;
  if (previous_index <= OMX_STE_IndexConfigAudioFirstIndex) {
	if (algo == UPLINK_ALGORITHMS) {
	  next_index = OMX_STE_IndexConfigAudioTxSpeechEnhancement;
	} else  {
	  next_index = OMX_STE_IndexConfigAudioRxSpeechEnhancement;
	}
  } else {
	if (algo == UPLINK_ALGORITHMS) {
	  switch (previous_index) {
	  case OMX_STE_IndexConfigAudioTxSpeechEnhancement:
		next_index = OMX_STE_IndexConfigAudioTxDRC;
		break;
	 
	  case OMX_STE_IndexConfigAudioTxDRC:
		next_index = OMX_STE_IndexConfigAudioTxCTM;
		break;
	 
	  case OMX_STE_IndexConfigAudioTxCTM:
		next_index =  OMX_STE_IndexConfigAudioTransducerEqualizer;
		break;
	 
	  case OMX_STE_IndexConfigAudioTransducerEqualizer:
		next_index = OMX_STE_IndexConfigAudioTxComfortNoise;
		break;
		
	  case OMX_STE_IndexConfigAudioTxComfortNoise:
		next_index = OMX_STE_IndexConfigAudioTxSpeechProcCustomerType2;
		break;
		
      case OMX_STE_IndexConfigAudioTxSpeechProcCustomerType2:
		next_index = OMX_STE_IndexConfigAudioEndIndex;
		break;
      
	  default:
		next_index = OMX_STE_IndexConfigAudioEndIndex;
	  }
	} else { // downlink
	  switch (previous_index) {
	  case OMX_STE_IndexConfigAudioRxSpeechEnhancement:
		next_index = OMX_STE_IndexConfigAudioRxDRC;
		break;
		
	  case OMX_STE_IndexConfigAudioRxDRC:
		next_index = OMX_STE_IndexConfigAudioRxCTM;
		break;
		
	  case OMX_STE_IndexConfigAudioRxCTM:
		next_index = OMX_STE_IndexConfigAudioRxComfortNoise;
		break;
		
      case OMX_STE_IndexConfigAudioRxComfortNoise:
		next_index = OMX_STE_IndexConfigAudioRxSpeechProcCustomerType1;
		break;
		
       case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType1:
		next_index = OMX_STE_IndexConfigAudioRxSpeechProcCustomerType2;
		break;
		
       case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType2:
		next_index = OMX_STE_IndexConfigAudioEndIndex;
		break;
		
	  default:
		next_index = OMX_STE_IndexConfigAudioEndIndex;
	  }
	}
  }
  return next_index;
}
  

OMX_PTR STE_SpeechProcLibrary::getNextConfig(speech_proc_algo_type_t algo,
											 OMX_U32 previous_index,
											 OMX_U32 *current_index)
{
  OMX_PTR result;
  OMX_U32 next =  nextIndex(algo, previous_index);
 
  switch(next) {
  case OMX_STE_IndexConfigAudioTxSpeechEnhancement:
	result = &mTxSpe;
	break;
  case OMX_STE_IndexConfigAudioRxSpeechEnhancement:
	result = &mRxSpe;
	break;
  case OMX_STE_IndexConfigAudioTxCTM:
	result = &mTxCTM;
	break;
  case OMX_STE_IndexConfigAudioRxCTM:
	result = &mRxCTM;
	break;
  case OMX_STE_IndexConfigAudioTxDRC:
	result = &mTxDrc;
	break;
  case OMX_STE_IndexConfigAudioRxDRC:
	result = &mRxDrc;
	break;
  case OMX_STE_IndexConfigAudioTransducerEqualizer:
	result = &mTxTrEq;
	break;
  case OMX_STE_IndexConfigAudioTxComfortNoise:
	result = &mTxComfortnoise;
	break;
  case OMX_STE_IndexConfigAudioRxComfortNoise:
	result = &mRxComfortnoise;
	break;
  case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType1:
	result = &mRxCustemerType1;
	break;
  case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType2:
	result = &mRxCustemerType2;
	break;
  case OMX_STE_IndexConfigAudioTxSpeechProcCustomerType2:
	result = &mTxCustemerType2;
	break;
  default:
	result = 0;
  };
 
  *current_index = next;
  return result;
}

OMX_ERRORTYPE STE_SpeechProcLibrary::getConfig(speech_proc_algo_type_t algo,
											   OMX_U32 index,
											   OMX_PTR* config_table,
											   OMX_U32* index_table,
											   OMX_U32 *nb_config)
{
  OMX_ERRORTYPE error = OMX_ErrorNone;
  if (algo == UPLINK_ALGORITHMS) {
	switch(index) {
	case OMX_STE_IndexConfigAudioTxSpeechEnhancement:
	  {
		config_table[0] = (OMX_PTR)&mTxSpe;
		index_table[0] = index;
	  }
	  break;
	case OMX_STE_IndexConfigAudioTxCTM:
	  {
		config_table[0] = (OMX_PTR)&mTxCTM;
		index_table[0] = index;
	  }
	  break;
	case OMX_STE_IndexConfigAudioTxDRC:
	  {
		config_table[0] = (OMX_PTR) &mTxDrc;
		index_table[0] = index;
	  }
	  break;
	case OMX_STE_IndexConfigAudioTransducerEqualizer:
	  {
		config_table[0] = (OMX_PTR) &mTxTrEq;
		index_table[0] = index;
	  }
	  break;
    case OMX_STE_IndexConfigAudioTxComfortNoise:
      {
        config_table[0] = (OMX_PTR) &mTxComfortnoise;
		index_table[0] = index;
      }
    break;
   
    case OMX_STE_IndexConfigAudioTxSpeechProcCustomerType2:
	  {
		config_table[0] = (OMX_PTR) &mTxCustemerType2;
		index_table[0] = index;
	  }
	  break; 

	default:
	  error = OMX_ErrorUnsupportedIndex;
	}
  } else {
	switch(index) {  
	case OMX_STE_IndexConfigAudioRxSpeechEnhancement:
	  {
		config_table[0] = (OMX_PTR)&mRxSpe;
		index_table[0] = index;
	  }
	  break;
	  
	case OMX_STE_IndexConfigAudioRxCTM:
	  {
		config_table[0] = (OMX_PTR) &mRxCTM;
		index_table[0] = index;
	  }
	  break;
	case OMX_STE_IndexConfigAudioRxDRC:
	  {
		config_table[0] = (OMX_PTR) &mRxDrc;
		index_table[0] = index;
	  }
	  break;
	case OMX_STE_IndexConfigAudioRxComfortNoise:
      {
        config_table[0] = (OMX_PTR) &mRxComfortnoise;
        index_table[0] = index;
      }
    break; 
    case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType1:
	  {
		config_table[0] = (OMX_PTR) &mRxCustemerType1;
		index_table[0] = index;
	  }
	  break;
    case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType2:
	  {
		config_table[0] = (OMX_PTR) &mRxCustemerType2;
		index_table[0] = index;
	  }
	  break;
  
	default:
	  error = OMX_ErrorUnsupportedIndex;
	}
  }
  if (error == OMX_ErrorNone) *nb_config = 1;
 
  return error;
}
OMX_ERRORTYPE STE_SpeechProcLibrary::getOMXParamOrConfig(OMX_U32 index,
														 OMX_PTR config)
{
  OMX_ERRORTYPE error;
  switch(index) {
  case OMX_STE_IndexConfigAudioTxSpeechEnhancement:
	{
	  OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE *tx =
		(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE*) config;
	  if (tx->nSize == sizeof(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE) ||
		  tx->nPortIndex == OUTPUT_UPLINK_PORT) {
		error = OMX_ErrorNone;
		memcpy(tx, &mTxSpe, tx->nSize);
	  }
	  else error = OMX_ErrorUnsupportedSetting;
		  
	}
	break;
	
  case OMX_STE_IndexConfigAudioRxSpeechEnhancement:
	{
	  OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE *rx =
		(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE*) config;
	  if (rx->nSize == sizeof(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE) &&
		  rx->nPortIndex == OUTPUT_DOWNLINK_PORT) {
		error = OMX_ErrorNone;
		memcpy(rx, &mRxSpe, rx->nSize);
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	  
	}
	break;
	
  case OMX_STE_IndexConfigAudioTxCTM:
	{
	  OMX_STE_CONFIG_AUDIO_CTMTYPE *ctm =
		( OMX_STE_CONFIG_AUDIO_CTMTYPE*) config;
	  if (ctm->nSize == sizeof(OMX_STE_CONFIG_AUDIO_CTMTYPE) &&
		  ctm->nPortIndex == OUTPUT_UPLINK_PORT){
		error = OMX_ErrorNone;
		memcpy(ctm, &mTxCTM,ctm->nSize);
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;

  case OMX_STE_IndexConfigAudioTxDRC:
	{
	  OMX_STE_CONFIG_AUDIO_DRCTYPE *drc =
		( OMX_STE_CONFIG_AUDIO_DRCTYPE*) config;
	  if (drc->nSize == sizeof(OMX_STE_CONFIG_AUDIO_DRCTYPE) &&
		  drc->nPortIndex == OUTPUT_UPLINK_PORT){
		error = OMX_ErrorNone;
		memcpy(drc, &mTxDrc,drc->nSize);
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;

  case OMX_STE_IndexConfigAudioRxCTM:
	{
	  OMX_STE_CONFIG_AUDIO_CTMTYPE *ctm =
		( OMX_STE_CONFIG_AUDIO_CTMTYPE*) config;
	  if (ctm->nSize == sizeof(OMX_STE_CONFIG_AUDIO_CTMTYPE) &&
		  ctm->nPortIndex == OUTPUT_DOWNLINK_PORT) {
		error = OMX_ErrorNone;
		memcpy(ctm, &mRxCTM,ctm->nSize);
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;

  case OMX_STE_IndexConfigAudioRxDRC:
	{
	  OMX_STE_CONFIG_AUDIO_DRCTYPE *drc =
		( OMX_STE_CONFIG_AUDIO_DRCTYPE*) config;
	  if (drc->nSize == sizeof(OMX_STE_CONFIG_AUDIO_DRCTYPE) &&
		  drc->nPortIndex == OUTPUT_DOWNLINK_PORT) {
		error = OMX_ErrorNone;
		memcpy(drc, &mRxDrc,drc->nSize);
	  }
	  
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;
	
  case OMX_STE_IndexConfigAudioTransducerEqualizer:
	{
	  OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE *txtreq =
		(OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE*) config;
	  if (txtreq->nSize == sizeof(OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE) &&
		  txtreq->nPortIndex == OUTPUT_UPLINK_PORT) {
		error = OMX_ErrorNone;
		memcpy(txtreq, &mTxTrEq, txtreq->nSize);
	  }
	  else error = OMX_ErrorUnsupportedSetting;
	}
	break;

   case OMX_STE_IndexConfigAudioTxComfortNoise:
    {
        OMX_STE_AUDIO_CONFIG_COMFORT_NOISE* noise =
		(OMX_STE_AUDIO_CONFIG_COMFORT_NOISE*) config;
        if (noise->nSize == sizeof(OMX_STE_AUDIO_CONFIG_COMFORT_NOISE) &&
		   noise->nPortIndex == OUTPUT_UPLINK_PORT) {
           error = OMX_ErrorNone;
		   memcpy(noise, &mTxComfortnoise, noise->nSize);
        } else error = OMX_ErrorUnsupportedSetting;
    }
    break;

    case OMX_STE_IndexConfigAudioRxComfortNoise:
    {
       OMX_STE_AUDIO_CONFIG_COMFORT_NOISE* noise =
	   (OMX_STE_AUDIO_CONFIG_COMFORT_NOISE*) config;
        if (noise->nSize == sizeof(OMX_STE_AUDIO_CONFIG_COMFORT_NOISE) &&
		   noise->nPortIndex == OUTPUT_DOWNLINK_PORT) {
             error = OMX_ErrorNone;
		     memcpy(noise, &mRxComfortnoise, noise->nSize);
        } else error = OMX_ErrorUnsupportedSetting;
    }
    break;

  case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType1:
	{
	  OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE *tmp =
		(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE*) config;
	  if (tmp->nSize == sizeof(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE) &&
		  tmp->nPortIndex == OUTPUT_DOWNLINK_PORT) {
		error = OMX_ErrorNone;
		memcpy(tmp, &mRxCustemerType1, tmp->nSize);
	  }
	  else error = OMX_ErrorUnsupportedSetting;
	}
    break;

   case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType2:
	{
	  OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE *tmp =
		(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE*) config;
	  if (tmp->nSize == sizeof(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE) &&
		  tmp->nPortIndex == OUTPUT_DOWNLINK_PORT) {
		error = OMX_ErrorNone;
		memcpy(tmp, &mRxCustemerType2, tmp->nSize);
	  }
	  else error = OMX_ErrorUnsupportedSetting;
	}
    break;

    case OMX_STE_IndexConfigAudioTxSpeechProcCustomerType2:
	{
	  OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE *tmp =
		(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE*) config;
	  if (tmp->nSize == sizeof(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE) &&
		  tmp->nPortIndex == OUTPUT_DOWNLINK_PORT) {
		error = OMX_ErrorNone;
		memcpy(tmp, &mTxCustemerType2, tmp->nSize);
	  }
	  else error = OMX_ErrorUnsupportedSetting;
	}
    break;

  default:
	error = OMX_ErrorUnsupportedIndex;
  }
  return error;
}

OMX_ERRORTYPE
STE_SpeechProcLibrary::checkPortSettings(OMX_AUDIO_PARAM_PCMMODETYPE*
										 portsettings[SPEECH_PROC_NB_AUDIO_PORT],
										 OMX_U32 portenabled)
{
  int i;
  unsigned int j;
  OMX_ERRORTYPE error = OMX_ErrorNone; // be optimistic
  for (i = 0; i < SPEECH_PROC_NB_AUDIO_PORT; i++) {
	// portsettings[i]->eEndian well we support native whatever it is. Assumes we get it in native;
	
	if (portsettings[i]->nBitPerSample != 16) error = OMX_ErrorBadParameter;
	if (portsettings[i]->ePCMMode != OMX_AUDIO_PCMModeLinear) error = OMX_ErrorBadParameter;
	switch(portsettings[i]->nPortIndex) {
	case INPUT_UPLINK_PORT:
	  {
		if (portsettings[i]->nChannels < 1 ||
			portsettings[i]->nChannels > 3) error = OMX_ErrorBadParameter; 
	  }
	  break;
	case INPUT_DOWNLINK_PORT:
	  {
		if (portsettings[i]->nChannels != 1) error = OMX_ErrorBadParameter;
	  }
	  break;
	case INPUT_REFERENCE_PORT:
	  {
		if (portsettings[i]->nChannels < 1 ||
			portsettings[i]->nChannels > 2) error = OMX_ErrorBadParameter; 
	  }
	  break;
	case OUTPUT_UPLINK_PORT:
	  {
		if (portsettings[i]->nChannels != 1) error = OMX_ErrorBadParameter;
	  }
	  break;
	case OUTPUT_DOWNLINK_PORT:
	  {
		if (portsettings[i]->nChannels != 1) error = OMX_ErrorBadParameter;
	  }
	  break;
	default:
	  error = OMX_ErrorBadParameter;
	  break;
	}
	for (j = 0; j < portsettings[i]->nChannels; j++) {
	  // not sure I understand this
	}
  }
  return error;
}
OMX_ERRORTYPE STE_SpeechProcLibrary::getExtensionIndex(OMX_STRING extension, OMX_U32* index)
{
  OMX_ERRORTYPE error = OMX_ErrorNone;
  if (strcmp("OMX_STE_IndexConfigAudioTxSpeechEnhancement", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioTxSpeechEnhancement;
  } else if (strcmp("OMX_STE_IndexConfigAudioTxCTM", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioTxCTM;
  } else if (strcmp("OMX_STE_IndexConfigAudioTxDRC", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioTxDRC;
  } else if (strcmp("OMX_STE_IndexConfigAudioTransducerEqualizer", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioTransducerEqualizer;
  } else if (strcmp("OMX_STE_IndexConfigAudioRxSpeechEnhancement", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioRxSpeechEnhancement;
  } else if (strcmp("OMX_STE_IndexConfigAudioRxCTM", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioRxCTM;
  } else if (strcmp("OMX_STE_IndexConfigAudioRxDRC", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioRxDRC;
  } else if (strcmp("OMX_STE_IndexConfigAudioTxComfortNoise", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioTxComfortNoise;
  } else if (strcmp("OMX_STE_IndexConfigAudioRxComfortNoise", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioRxComfortNoise;
  } else if (strcmp("OMX_STE_IndexConfigAudioRxSpeechProcCustomerType1", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioRxSpeechProcCustomerType1;
  } else if (strcmp("OMX_STE_IndexConfigAudioRxSpeechProcCustomerType2", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioRxSpeechProcCustomerType2;
  } else if (strcmp("OMX_STE_IndexConfigAudioTxSpeechProcCustomerType2", extension) == 0) {
	*index = OMX_STE_IndexConfigAudioTxSpeechProcCustomerType2;
  } else {
	error = OMX_ErrorUnsupportedIndex;
  }
  return error;
}


void STE_SpeechProcLibrary::defaultTxSpeechEnhancement(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE* config, OMX_U32 port)
{
  memset(config, 0, sizeof(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE));
  config->nSize = sizeof(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE);
  config->nPortIndex =port;
  config->nVersion.nVersion = OMX_VERSION;
}

void STE_SpeechProcLibrary::defaultRxSpeechEnhancement(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE* config, OMX_U32 port)
{
  memset(config, 0, sizeof(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE));
  config->nSize = sizeof(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE);
  config->nPortIndex = port;
  config->nVersion.nVersion = OMX_VERSION;
}

void STE_SpeechProcLibrary::defaultDRC(OMX_STE_CONFIG_AUDIO_DRCTYPE* config, OMX_U32 port)
{
  memset(config, 0, sizeof(OMX_STE_CONFIG_AUDIO_DRCTYPE));
  config->nSize = sizeof(OMX_STE_CONFIG_AUDIO_DRCTYPE);
  config->nPortIndex = port;
  config->nVersion.nVersion = OMX_VERSION;
}

void STE_SpeechProcLibrary::defaultCTM(OMX_STE_CONFIG_AUDIO_CTMTYPE* config, OMX_U32 port)
{
  memset(config, 0, sizeof(OMX_STE_CONFIG_AUDIO_CTMTYPE));
  config->nSize = sizeof(OMX_STE_CONFIG_AUDIO_CTMTYPE);
  config->nPortIndex = port;
  config->nVersion.nVersion = OMX_VERSION;
}

void STE_SpeechProcLibrary::defaultTrEq(OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE* config, OMX_U32 port)
{
  memset(config, 0, sizeof(OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE));
  config->nSize = sizeof(OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE);
  config->nPortIndex = port;
  config->nVersion.nVersion = OMX_VERSION;
}


void STE_SpeechProcLibrary::defaultComfortnoise(OMX_STE_AUDIO_CONFIG_COMFORT_NOISE* config, OMX_U32 port)
{
  memset(config, 0, sizeof(OMX_STE_AUDIO_CONFIG_COMFORT_NOISE));
  config->nSize = sizeof(OMX_STE_AUDIO_CONFIG_COMFORT_NOISE);
  config->nPortIndex = port;
  config->nVersion.nVersion = OMX_VERSION;
}

void STE_SpeechProcLibrary::defaultCustAlg(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE* config, OMX_U32 port)
{
  memset(config, 0, sizeof(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE));
  config->nSize = sizeof(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE);
  config->nPortIndex = port;
  config->nVersion.nVersion = OMX_VERSION;
}

void STE_SpeechProcLibrary::setDrcMinMax(OMX_STE_CONFIG_AUDIO_DRCTYPE& drc) 
{
    drc.sLookAheadTime.nMin = 0;
    drc.sLookAheadTime.nMax = 0;
    drc.sInputGain.nMin  = (-128 << 8);
    drc.sOffsetGain.nMin = ( -48 << 8);
    drc.sInputGain.nMax  = (128 << 8);
    drc.sOffsetGain.nMax = ( 48 << 8);
    drc.sLookAheadTime.nMin    = 0;
    drc.sLookAheadTime.nMax    = 0; // do not try to set anything than 0
    drc.sLevelDetectorAttackTime.nMin     = 0;
    drc.sLevelDetectorReleaseTime.nMin    = 0;
    drc.sGainProcessorAttackTime.nMin     = 0;
    drc.sGainProcessorReleaseTime.nMin    = 0;
    drc.sLevelDetectorAttackTime.nMax     = AFM_DRC_ATTACK_RELEASE_TIME_MAX;
    drc.sLevelDetectorReleaseTime.nMax    = AFM_DRC_ATTACK_RELEASE_TIME_MAX;
    drc.sGainProcessorAttackTime.nMax     = AFM_DRC_ATTACK_RELEASE_TIME_MAX;
    drc.sGainProcessorReleaseTime.nMax    = AFM_DRC_ATTACK_RELEASE_TIME_MAX;

    for (int i=0; i < AFM_DRC_NUMBER_OF_STATIC_CURVE_POINTS; i++ )
    {
       drc.sStaticCurvePointX[i].nMin = AFM_DRC_STATIC_CURVE_POINT_MIN;
       drc.sStaticCurvePointY[i].nMin = AFM_DRC_STATIC_CURVE_POINT_MIN;
       drc.sStaticCurvePointX[i].nMax = AFM_DRC_STATIC_CURVE_POINT_MAX;
       drc.sStaticCurvePointY[i].nMax = AFM_DRC_STATIC_CURVE_POINT_MAX;
    }
}

OMX_ERRORTYPE STE_SpeechProcLibrary::checkDrc(const OMX_STE_CONFIG_AUDIO_DRCTYPE* drc) 
{
    if(drc->sLookAheadTime.nValue > 0) {
     return OMX_ErrorBadParameter;
    }

    if((drc->sInputGain.nValue < (OMX_S32)((-128) << 8)) || (drc->sInputGain.nValue >= (OMX_S32)(128 << 8))) {
     return OMX_ErrorBadParameter;
    }

    if((drc->sOffsetGain.nValue < (OMX_S16)((-48) << 8)) || (drc->sOffsetGain.nValue > ((OMX_S16)48 << 8))) {
     return OMX_ErrorBadParameter;
    }

    if(drc->sLevelDetectorAttackTime.nValue > AFM_DRC_ATTACK_RELEASE_TIME_MAX) {
     return OMX_ErrorBadParameter;
    }

    if(drc->sLevelDetectorReleaseTime.nValue > AFM_DRC_ATTACK_RELEASE_TIME_MAX) {
     return OMX_ErrorBadParameter;
    }

    if(drc->sGainProcessorAttackTime.nValue > AFM_DRC_ATTACK_RELEASE_TIME_MAX) {
     return OMX_ErrorBadParameter;
    }

    if(drc->sGainProcessorReleaseTime.nValue > AFM_DRC_ATTACK_RELEASE_TIME_MAX) {
     return OMX_ErrorBadParameter;
    }

    if((drc->eAcceptanceLevel < OMX_AUDIO_DRCAcceptanceLevel90Percent) || (drc->eAcceptanceLevel > OMX_AUDIO_DRCAcceptanceLevel999Percent)) {
     return OMX_ErrorBadParameter;
    }

    for (int i=0; i < AFM_DRC_NUMBER_OF_STATIC_CURVE_POINTS; i++ )
    {
     if ((drc->sStaticCurvePointX[i].nValue > 0) || (drc->sStaticCurvePointY[i].nValue > 0)) {
        return OMX_ErrorBadParameter;
     }
    }
    return OMX_ErrorNone;
}

SpeechProcLibrary *speech_proc_getOMXInterface(void)
{
  STE_SpeechProcLibrary*lib = new STE_SpeechProcLibrary();
  return lib;
}


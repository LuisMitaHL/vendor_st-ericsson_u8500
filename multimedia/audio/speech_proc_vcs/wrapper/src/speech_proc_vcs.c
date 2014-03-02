/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     speech_proc_vcs.h
 * \brief    STE implementation of speech_proc_interface.h
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "speech_proc_interface.h"
#include "vcs_rx.h"
#include "vcs_tx.h"

#include "vcs_algo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "omx_ste_speech_proc.h"
#include "vcs_setx.h"
#include "vcs_serx.h"
#include "vcs_drc.h"
#include "vcs_algorithm_gen.h"
#ifdef VCS_ANDROID
#define LOG_TAG "VCS"
#define LOG_NDEBUG 0
#include <cutils/log.h>
#endif
#if defined(VCS_MMPROBE)
#include "t_mm_probe.h"
#include "r_mm_probe.h"
#endif


#define VCSBSIZE 128*10
typedef struct my_speech_log_s
{
  vcs_log_t fn;
  char buf[VCSBSIZE];
  unsigned long probecounter;
} speech_proc_log_t;

typedef struct {
  vcs_ctx_t* ctx;
  vcs_portsdef_t inport;
  vcs_portsdef_t outport;
  vcs_mode_t mode;
  speech_proc_log_t mylog;
} speech_proc_vcs_t;


static void my_log(struct vcs_log_s* hndl,
				   vcs_log_kind_t reason,
				   const char* s, ...)
{
#if defined(VCS_ANDROID) || defined(VCS_LOG)
  speech_proc_log_t* mylog = (speech_proc_log_t*) hndl;
  static const char *reasontxt[] = {"ERR", "ERR", "WARN", "INFO", "DEBUG", "V"};
  va_list args;
  va_start(args, s);
  (void) vsnprintf(mylog->buf, VCSBSIZE, s, args);
  va_end(args);
#ifdef VCS_ANDROID
  switch(reason) {
  case VCS_FATAL:
  case VCS_ERROR:
	ALOGE(mylog->buf);
	break;
  case VCS_WARNING:
	ALOGW(mylog->buf);
	break;
  case VCS_INFO:
	ALOGI(mylog->buf);
	break;
  case VCS_DEBUG:
	ALOGD(mylog->buf);
	break;
  case VCS_VERBOSE:
	ALOGV(mylog->buf);
	break;
  default:
	ALOGD("mising case in log");
  }
#else

  printf("VCS %s %s\n", reasontxt[reason], mylog->buf);
#endif

#endif  // defined(VCS_ANDROID) || defined(VCS_LOG)
}

static int my_probe_is_active(struct vcs_log_s* hndl,
					           vcs_probe_points_t point)
{
   int result = 0;
#if defined(VCS_MMPROBE)
   MM_ProbeID_t id = 255;
   switch (point) {
   case VCS_PROBE_TX_INPUT_LEFT:
   // we currently only support MONO with one additional NR ref, so no channel == 2 means mono + one NR.
	id = MM_PROBE_EC_MIC_L_INPUT;
	break;
  case VCS_PROBE_TX_INPUT_RIGHT:
	id =  MM_PROBE_NR_TX_REF_INPUT;
	break;
  case VCS_PROBE_TX_INPUT_MIC_REF1:
	id = MM_PROBE_NR_TX_REF_INPUT; // not totally correct but will anyway not be used until some future support for 3 channels
	break;
  case VCS_PROBE_RX_INPUT:
	id = MM_PROBE_RX_SPEECH_PROC_INPUT;
	break;
  case 	VCS_PROBE_TX_EC_FEEDBACK_LEFT:
	id = MM_PROBE_EC_REF_L_INPUT;
	break;
  case VCS_PROBE_TX_EC_FEEDBACK_RIGHT:
	id =  MM_PROBE_EC_REF_R_INPUT;
	break;
  case 	VCS_PROBE_TX_EC_FILTERTAP_LEFT:
	id = MM_PROBE_EC_FILTER_L;
	break;
  case VCS_PROBE_TX_EC_FILTERTAP_RIGHT:
	id =  MM_PROBE_EC_FILTER_R;
	break;
  case VCS_PROBE_RX_SE:
	id = MM_PROBE_SE_RX_OUTPUT;
	break;
  case VCS_PROBE_TX_SE_NONLINEAR_LEFT:
	id = MM_PROBE_SE_TX_OUTPUT_L;
	break;
   case VCS_PROBE_TX_SE_NONLINEAR_RIGHT:
	id = MM_PROBE_SE_TX_OUTPUT_R;
	break;
  case VCS_PROBE_TX_SE_LINEAR_LEFT:
	id = MM_PROBE_EC_LIN_OUTPUT_L;
	break;
  case VCS_PROBE_TX_SE_LINEAR_RIGHT:
	id   = MM_PROBE_EC_LIN_OUTPUT_R;
	break;
  case VCS_PROBE_TX_SE_OUT:
	id = MM_PROBE_SE_TX_OUTPUT;
	break;
  case VCS_PROBE_TX_DRC:
	id = MM_PROBE_TX_DRC_OUTPUT;
	break;
  case VCS_PROBE_RX_DRC:
	id = MM_PROBE_RX_DRC_OUTPUT;
	break;
  case VCS_PROBE_TX_COMFORTNOISE:
    id = MM_PROBE_TX_COMF_NOISE_OUTPUT;
    break;
  case VCS_PROBE_RX_COMFORTNOISE:
    id = MM_PROBE_RX_COMF_NOISE_OUTPUT;
    break;
  case VCS_PROBE_RX_CTM:
	id =  MM_PROBE_RX_CTM_OUTPUT;
	break;
  case VCS_PROBE_TX_CTM:
	id =  MM_PROBE_TX_CTM_OUTPUT;
	break;
  case VCS_PROBE_TX_CUSTOMER_EFFECT:
    id = MM_PROBE_TX_CUSTOMER_EFFECT_OUTPUT;
    break;
  case VCS_PROBE_RX_CUSTOMER_EFFECT:
    id = MM_PROBE_RX_CUSTOMER_EFFECT_OUTPUT;
    break;
  case VCS_PROBE_ADEQ_TX_CALIBRATION_DATA:
    id =  MM_PROBE_ADEQ_TX_CALIBRATION_DATA;
    break;
  case VCS_PROBE_ADEQ_RX_CALIBRATION_DATA:
    id =  MM_PROBE_ADEQ_RX_CALIBRATION_DATA;
    break;
  case VCS_PROBE_TX_DRC_CONFIG:
  case VCS_PROBE_RX_DRC_CONFIG:
	id = MM_PROBE_PLAIN_TEXT;
	break;
  case VCS_PROBE_TX_SE_CONFIG:
  case VCS_PROBE_RX_SE_CONFIG:
	id = MM_PROBE_PLAIN_TEXT;
	break;
  case VCS_PROBE_RX_CTM_CONFIG:
  case VCS_PROBE_TX_CTM_CONFIG:
	id = MM_PROBE_PLAIN_TEXT;
    break;
  case VCS_PROBE_RX_COMFORTNOISE_CONFIG:
  case VCS_PROBE_TX_COMFORTNOISE_CONFIG:
	id = MM_PROBE_PLAIN_TEXT;
    break;
  default:
    id = 255;
  }
  if (id != 255) result = mmprobe_status(id);
  else result = 0;
#endif
  return result;
}
static void my_probe(struct vcs_log_s* hndl,
					 vcs_probe_points_t point,
					 const void* data,
					 int len)
{
#if defined(VCS_MMPROBE)
  MM_ProbeID_t id = 255;
  speech_proc_log_t* mylog = (speech_proc_log_t*) hndl;
  switch (point) {
  case VCS_PROBE_TX_INPUT_LEFT:
   // we currently only support MONO with one additional NR ref, so no channel == 2 means mono + one NR.
	id = MM_PROBE_EC_MIC_L_INPUT;
	break;
  case VCS_PROBE_TX_INPUT_RIGHT:
	id =  MM_PROBE_NR_TX_REF_INPUT;
	break;
  case VCS_PROBE_TX_INPUT_MIC_REF1:
	id = MM_PROBE_NR_TX_REF_INPUT; // not totally correct but will anyway not be used until some future support for 3 channels
	break;
  case VCS_PROBE_RX_INPUT:
	id = MM_PROBE_RX_SPEECH_PROC_INPUT;
	break;
  case 	VCS_PROBE_TX_EC_FEEDBACK_LEFT:
	id = MM_PROBE_EC_REF_L_INPUT;
	break;
  case VCS_PROBE_TX_EC_FEEDBACK_RIGHT:
	id =  MM_PROBE_EC_REF_R_INPUT;
	break;
  case 	VCS_PROBE_TX_EC_FILTERTAP_LEFT:
	id = MM_PROBE_EC_FILTER_L;
	break;
  case VCS_PROBE_TX_EC_FILTERTAP_RIGHT:
	id = MM_PROBE_EC_FILTER_R;
	break;
  case VCS_PROBE_RX_SE:
	id = MM_PROBE_SE_RX_OUTPUT;
	break;
  case VCS_PROBE_TX_SE_NONLINEAR_LEFT:
	id = MM_PROBE_SE_TX_OUTPUT_L;
	break;
   case VCS_PROBE_TX_SE_NONLINEAR_RIGHT:
	id = MM_PROBE_SE_TX_OUTPUT_R;
	break;
  case VCS_PROBE_TX_SE_LINEAR_LEFT:
	id = MM_PROBE_EC_LIN_OUTPUT_L;
	break;
  case VCS_PROBE_TX_SE_LINEAR_RIGHT:
	id = MM_PROBE_EC_LIN_OUTPUT_R;
	break;
  case VCS_PROBE_TX_SE_OUT:
	id = MM_PROBE_SE_TX_OUTPUT;
	break;
  case VCS_PROBE_TX_DRC:
	id = MM_PROBE_TX_DRC_OUTPUT;
	break;
  case VCS_PROBE_RX_DRC:
	id = MM_PROBE_RX_DRC_OUTPUT;
	break;
  case VCS_PROBE_TX_COMFORTNOISE:
    id = MM_PROBE_TX_COMF_NOISE_OUTPUT;
    break;
  case VCS_PROBE_RX_COMFORTNOISE:
    id = MM_PROBE_RX_COMF_NOISE_OUTPUT;
    break;
  case VCS_PROBE_RX_CTM:
	id =  MM_PROBE_RX_CTM_OUTPUT;
	break;
  case VCS_PROBE_TX_CTM:
	id =  MM_PROBE_TX_CTM_OUTPUT;
	break;
  case VCS_PROBE_TX_CUSTOMER_EFFECT:
    id = MM_PROBE_TX_CUSTOMER_EFFECT_OUTPUT;
    break;
  case VCS_PROBE_RX_CUSTOMER_EFFECT:
    id = MM_PROBE_RX_CUSTOMER_EFFECT_OUTPUT;
    break;
  case VCS_PROBE_ADEQ_TX_CALIBRATION_DATA:
    id =  MM_PROBE_ADEQ_TX_CALIBRATION_DATA;
    break;
  case VCS_PROBE_ADEQ_RX_CALIBRATION_DATA:
    id =  MM_PROBE_ADEQ_RX_CALIBRATION_DATA;
    break;
  case VCS_PROBE_TX_DRC_CONFIG:
  case VCS_PROBE_RX_DRC_CONFIG:
	id = MM_PROBE_PLAIN_TEXT;
	break;
  case VCS_PROBE_TX_SE_CONFIG:
  case VCS_PROBE_RX_SE_CONFIG:
	id = MM_PROBE_PLAIN_TEXT;
	break;
  case VCS_PROBE_RX_CTM_CONFIG:
  case VCS_PROBE_TX_CTM_CONFIG:
	id = MM_PROBE_PLAIN_TEXT;
	break;
  case VCS_PROBE_RX_COMFORTNOISE_CONFIG:
  case VCS_PROBE_TX_COMFORTNOISE_CONFIG:
	id = MM_PROBE_PLAIN_TEXT;
    break;

  case VCS_PROBE_RX_CUSTOMER_FIRST_CONFIG:
  case VCS_PROBE_RX_CUSTOMER_LAST_CONFIG:
  case VCS_PROBE_TX_CUSTOMER_LAST_CONFIG:
    id = MM_PROBE_PLAIN_TEXT;
	break;

  default:
	id = 255;
  }
  if (id != 255) {
	if (id != MM_PROBE_PLAIN_TEXT) {
	  (void) mmprobe_probe(id, (void*)data, len);
	 
	} else if (mmprobe_status(id)) {

	  static const char *dirtext[] = {"TX_SE", "TX_TREQ", "TX_CTM", "TX_DRC","TX_COMFORTNOISE", "RX_CTM",
									  "RX_SE","RX_DRC", "RX_COMFORTNOISE", "RX_CUST_FIRST", "RX_CUST_LAST", "TX_CUST_LAST", "UNDEF"};
	  int len = sprintf(mylog->buf, "%s:", dirtext[point - VCS_PROBE_TX_SE_CONFIG]);

	  if (point == VCS_PROBE_TX_SE_CONFIG) {
		vcs_setx_config_tostring(mylog->buf + len,
								 VCSBSIZE - len,
								 (vcs_setx_config_t*) data);
	  } else if (point == VCS_PROBE_RX_SE_CONFIG) {
		vcs_serx_config_tostring(mylog->buf + len,
								 VCSBSIZE - len,
								 (vcs_serx_config_t*) data);
	  } else if (point == VCS_PROBE_RX_DRC_CONFIG || point == VCS_PROBE_TX_DRC_CONFIG ) {
		vcs_drc_config_tostring(mylog->buf + len,
								VCSBSIZE - len,
								(vcs_drc_config_t*) data);
	  } else if (point == VCS_PROBE_RX_CTM_CONFIG || point == VCS_PROBE_TX_CTM_CONFIG) {
		sprintf(mylog->buf+len, "%d",  ((vcs_ctm_config_t*) data)->enabled);
	  } else if (point == VCS_PROBE_RX_COMFORTNOISE_CONFIG || point == VCS_PROBE_TX_COMFORTNOISE_CONFIG) {
        len += sprintf(mylog->buf+len, " %d,",  ((vcs_comfortnoise_config_t*) data)->enabled);
        sprintf(mylog->buf+len, "%d",  ((vcs_comfortnoise_config_t*) data)->level);
	  } else if (point == VCS_PROBE_RX_CUSTOMER_FIRST_CONFIG || point == VCS_PROBE_RX_CUSTOMER_LAST_CONFIG || point == VCS_PROBE_TX_CUSTOMER_LAST_CONFIG ) {
		sprintf(mylog->buf+len, "%d",  ((vcs_algorithm_config_t*) data)->enabled);
	  }
	  len = strlen(mylog->buf);
	  if (len % 2 != 0) {
		mylog->buf[len] = ';';
		mylog->buf[++len] = '\0';
	  }

	  mmprobe_probe(id, mylog->buf, len + 1);

	}
  }
#endif
  
}

enum {VCSCONFIG_INVALIDINDEX = -1, VCSCONFIG_INVALIDSIZE = -2, VCSCONFIG_OUTOFMEMORY = -3};


static int extract_vcs_customerconfig(vcs_algorithm_config_t** config,
                                       OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE* omx)
{
   vcs_algorithm_config_t* tmp = 0;
   int result = 0;
   tmp = malloc(sizeof(vcs_algorithm_config_t));
   if (tmp != 0) {
      int i;
      tmp->enabled = omx->bEnable;
      tmp->len =  sizeof(tmp->data)/sizeof(unsigned short);
      for (i = 0; i < tmp->len; i++) {
         tmp->data[i] = omx->nData[i];
      }
   } else {
     result = VCSCONFIG_OUTOFMEMORY;
   }
   *config = tmp;
   return result;
}

static void extract_vcs_drc(vcs_drc_config_t* drc,
							OMX_STE_CONFIG_AUDIO_DRCTYPE* omx)
{
  int i;
  drc->enabled = omx->bEnable;
#ifdef DRC
  drc->config_param.use_peak_measure = omx->bUsePeakMeasure;
  drc->config_param.look_ahead_time = omx->sLookAheadTime.nValue;
  drc->config_param.static_curve.input_gain = omx->sInputGain.nValue;
  drc->config_param.static_curve.offset_gain = omx->sOffsetGain.nValue;
  for ( i= 0; i < DRC_NUMBER_OF_STATIC_CURVE_POINTS; i++) {
	drc->config_param.static_curve.static_curve_point[i].x =
	  omx->sStaticCurvePointX[i].nValue;
	drc->config_param.static_curve.static_curve_point[i].y =
	  omx->sStaticCurvePointY[i].nValue;
  }
  drc->config_param.time_specifiers.level_detector_attack_time =
	omx->sLevelDetectorAttackTime.nValue;
  drc->config_param.time_specifiers.level_detector_release_time =
	omx->sLevelDetectorReleaseTime.nValue;
  drc->config_param.time_specifiers.gain_processor_attack_time =
	omx->sGainProcessorAttackTime.nValue;
  drc->config_param.time_specifiers.gain_processor_release_time =
	omx->sGainProcessorReleaseTime.nValue;
  drc->config_param.time_specifiers.acceptance_level =
	omx->eAcceptanceLevel;
#endif
}

static void extract_vcs_setx(vcs_setx_config_t *tx,
							 const OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE* omx)
{
  tx->enabled = omx->bEnable;
#ifdef VCSSE
  if (sizeof(STE_ConfigAudioEchoControl_t) == sizeof(SE_EcParameters_t)) {
	memcpy(&tx->txparam.EchoControl, &omx->sConfigEchoControl, sizeof(SE_EcParameters_t));
  } else {
	memset(&tx->txparam.EchoControl, 0,sizeof(SE_EcParameters_t));
  }
  if (sizeof(STE_ConfigNoiseReduction_t) == sizeof(SE_NrParameters_t)) {
	memcpy(&tx->txparam.TxNoiseReduction, &omx->sConfigNoiseReduction, sizeof(SE_NrParameters_t));
  } else {
	memset(&tx->txparam.TxNoiseReduction, 0, sizeof(SE_NrParameters_t));
  }
  if (sizeof(SE_ConfigTxLevelControl_t) == sizeof(SE_TxLcParameters_t)) {
	memcpy(&tx->txparam.TxLevelControl, &omx->sConfigTxLevelControl, sizeof(SE_TxLcParameters_t));
  } else {
	memset(&tx->txparam.TxLevelControl, 0,sizeof(SE_TxLcParameters_t));
  }
  if (sizeof(SE_ConfigMMicParameter_t) == sizeof(SE_MmicParameters_t)) {
     memcpy(&tx->txparam.MultiMicrophone, &omx->sConfigMMic, sizeof(SE_MmicParameters_t));
  } else {
	memset(&tx->txparam.MultiMicrophone, 0,sizeof(SE_MmicParameters_t));
  }
#endif
}



static void extract_vcs_serx(vcs_serx_config_t *rx,
							 const OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE* omx)
{
  rx->enabled = omx->bEnable;
#ifdef VCSSE

  if (sizeof(STE_ConfigNoiseReduction_t) == sizeof(SE_NrParameters_t)) {
	memcpy(&rx->rxparam.RxNoiseReduction, &omx->sConfigNoiseReduction, sizeof(SE_NrParameters_t));
  } else {
	memset(&rx->rxparam.RxNoiseReduction, 0,sizeof(SE_NrParameters_t));
  }
  if (sizeof(STE_ConfigRxEqualizer_t) == sizeof(SE_RxEqParameters_t)) {
	memcpy(&rx->rxparam.RxEqualize, &omx->sConfigRxEqualizer, sizeof(SE_RxEqParameters_t));
  } else {
	memset(&rx->rxparam.RxEqualize, 0,sizeof(SE_RxEqParameters_t));
  }

#endif
}

static void extract_vcs_ctm(vcs_ctm_config_t *ctm,
							const OMX_STE_CONFIG_AUDIO_CTMTYPE* omx)
{
  ctm->enabled = omx->bEnable;
  // ctm only contains enable/disable
}

static void extract_vcs_comfortnoise(vcs_comfortnoise_config_t* noise,
							        OMX_STE_AUDIO_CONFIG_COMFORT_NOISE* omx)
{
  noise->enabled = omx->bEnable;
  noise->level = omx->sLevel.nValue;
}

static void extract_vcs_treq(vcs_treq_config_t* treq,
							 const  OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE * omx)
{
  treq->enabled = omx->bEnable;
#ifdef VCSTREQ
  // here the rest will go once treq is implmemented for M7400 FAT modem. Eventually also U5500
#endif
}


static int extract_vcs_config(speech_proc_vcs_t* my,
							  int index,
							  void *omx_config,
							  int* vcs_index,
							  void** vcs_config)
{

  int result = VCSCONFIG_INVALIDINDEX;
  switch(index) {
  case OMX_STE_IndexConfigAudioTxDRC:
  case OMX_STE_IndexConfigAudioRxDRC:
  {
	OMX_STE_CONFIG_AUDIO_DRCTYPE* omx = (OMX_STE_CONFIG_AUDIO_DRCTYPE*) omx_config;
	vcs_drc_config_t* drc;
	if (omx->nSize == sizeof(OMX_STE_CONFIG_AUDIO_DRCTYPE)) {
	  drc = malloc(sizeof(vcs_drc_config_t));
	  if (drc != 0) {
		extract_vcs_drc(drc, omx);
		*vcs_config = drc;
		*vcs_index = VCS_DRC;
		result = 0;
	  } else result = VCSCONFIG_OUTOFMEMORY;
	} else result = VCSCONFIG_INVALIDSIZE;
  }
  break;
  case OMX_STE_IndexConfigAudioTxSpeechEnhancement:
	{
	  OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE* omx =
		(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE*)omx_config;
	  vcs_setx_config_t *tx;
	  if (omx->nSize == sizeof(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE)) {
		tx = malloc(sizeof(vcs_setx_config_t));
		if (tx != 0) {
		  memset(tx, 0, sizeof(vcs_setx_config_t));
		  extract_vcs_setx(tx, omx);
		  *vcs_config = tx;
		  *vcs_index = VCS_TXSPEECHENHANCEMENT;
		  result = 0;
		} else result = VCSCONFIG_OUTOFMEMORY;
	  }  else result = VCSCONFIG_INVALIDSIZE;
	}
	break;
  case OMX_STE_IndexConfigAudioRxSpeechEnhancement:
	{
	   OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE* omx =
		(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE*)omx_config;
	  vcs_serx_config_t *rx;
	  if (omx->nSize == sizeof(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE)) {
		rx = malloc(sizeof(vcs_serx_config_t));
		if (rx != 0) {
		  memset(rx, 0, sizeof(vcs_serx_config_t));
		  extract_vcs_serx(rx, omx);
		  *vcs_config = rx;
		  *vcs_index = VCS_RXSPEECHENHANCEMENT;
		  result = 0;
		} else result = VCSCONFIG_OUTOFMEMORY;
	  } else result = VCSCONFIG_INVALIDSIZE;
	}
	break;
  case OMX_STE_IndexConfigAudioTxCTM:
  case OMX_STE_IndexConfigAudioRxCTM:
	{
	  OMX_STE_CONFIG_AUDIO_CTMTYPE* omx =
		(OMX_STE_CONFIG_AUDIO_CTMTYPE*) omx_config;
	  vcs_ctm_config_t *ctm;
	  if (omx->nSize == sizeof(OMX_STE_CONFIG_AUDIO_CTMTYPE)) {
		ctm = malloc(sizeof(vcs_ctm_config_t));
		if (ctm != 0) {
		  extract_vcs_ctm(ctm, omx);
		  *vcs_config = ctm;
		  *vcs_index = VCS_CTM;
		  result = 0;
		} else result = VCSCONFIG_OUTOFMEMORY;
	  } else result = VCSCONFIG_INVALIDSIZE;

	}
	break;
  case OMX_STE_IndexConfigAudioTransducerEqualizer:
	{
	  OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE* omx =
		(OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE*) omx_config;
	  vcs_treq_config_t *treq;
	  if (omx->nSize == sizeof(OMX_STE_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE)) {
		treq = malloc(sizeof(vcs_treq_config_t));
		if (treq != 0) {
		  extract_vcs_treq(treq, omx);
		  *vcs_config = treq;
		  *vcs_index = VCS_TREQ;
		  result = 0;
		} else result = VCSCONFIG_OUTOFMEMORY;
	  } else result = VCSCONFIG_INVALIDSIZE;
	}
	break;
  case OMX_STE_IndexConfigAudioTxComfortNoise:
  case OMX_STE_IndexConfigAudioRxComfortNoise:
    {
      OMX_STE_AUDIO_CONFIG_COMFORT_NOISE* omx = (OMX_STE_AUDIO_CONFIG_COMFORT_NOISE*) omx_config;
      vcs_comfortnoise_config_t *noise;
      if (omx->nSize == sizeof(OMX_STE_AUDIO_CONFIG_COMFORT_NOISE)) {
         noise = malloc(sizeof(vcs_comfortnoise_config_t));
         if (noise != 0) {
            extract_vcs_comfortnoise(noise, omx);
            *vcs_config = noise;
		    *vcs_index = VCS_COMFORTNOISE;
		    result = 0;
         } else result = VCSCONFIG_OUTOFMEMORY;
      } else result = VCSCONFIG_INVALIDSIZE;
    
    }
    break;
  case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType1:
  case OMX_STE_IndexConfigAudioRxSpeechProcCustomerType2:
  case OMX_STE_IndexConfigAudioTxSpeechProcCustomerType2:
	{
       OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE* omx =
		(OMX_STE_AUDIO_CONFIG_SPEECHPROC_CUSTOMERTYPE*) omx_config;
       vcs_algorithm_config_t *config;
       result = extract_vcs_customerconfig(&config, omx);
       if (config != 0) {
          *vcs_config = config;
          if (index == OMX_STE_IndexConfigAudioRxSpeechProcCustomerType1) {
             *vcs_index = VCS_CUSTOMER_PLUGIN_FIRST;
          } else {
             *vcs_index = VCS_CUSTOMER_PLUGIN_LAST;
          }
          result = 0;
        } else result = VCSCONFIG_OUTOFMEMORY;
     }
     break;
  default:
   break;
  }
  if (result != 0) {
	if (result == VCSCONFIG_OUTOFMEMORY) {
	  my->mylog.fn.log(&my->mylog.fn, VCS_WARNING,
					   "Out of memory OMX index %i", index);
	} else if (result == VCSCONFIG_INVALIDSIZE) {
	   my->mylog.fn.log(&my->mylog.fn, VCS_WARNING,
					   "Invalid size of data OMX index %i", index);
	} else if (result == VCSCONFIG_INVALIDINDEX){
	   my->mylog.fn.log(&my->mylog.fn, VCS_INFO,
					   "Unknowned index OMX index%i", index);
	}
  }
  return result;
}

static int extract_vcs_param(int index,
							 void *omx_param,
							 int* vcs_index,
							 void** vcs_param)
{
  int result = -1;
  switch(index) {

  default:
	break;
  }
  return result;

}


speech_proc_error_t speech_proc_init(speech_proc_algo_t algo,
									 void **ctx,
									 unsigned int *version)
{
  // allocate context
  speech_proc_error_t error = SP_ERROR_NONE;
  speech_proc_vcs_t *vcs = (speech_proc_vcs_t*) malloc(sizeof(speech_proc_vcs_t));
  if ( version != NULL) {
    *version = 1;
  }
  if (vcs == 0) {
	*ctx = 0;
	return SP_ERROR_NO_MORE_MEMORY;
  }
  memset(vcs, 0, sizeof(speech_proc_vcs_t));
  vcs->mylog.fn.log = my_log;
  vcs->mylog.fn.probe = my_probe;
  vcs->mylog.fn.probe_is_active = my_probe_is_active;
  if (algo == SP_UPLINK_ALGO || algo == SP_UPLINK_NO_ALGO) {
	vcs->ctx = vcs_tx_create(&vcs->mylog.fn);

  } else if (algo == SP_DOWNLINK_ALGO || algo == SP_DOWNLINK_NO_ALGO) {
	vcs->ctx = vcs_rx_create(&vcs->mylog.fn);
  } else {
	free(vcs);
	*ctx = 0;
	return SP_ERROR;
  }
  if (vcs->ctx == 0) {
	free(vcs);
	*ctx = 0;
	return SP_ERROR_NO_MORE_MEMORY;
  }
  *ctx= (void*)vcs;

  vcs->mylog.fn.log(&vcs->mylog.fn, VCS_DEBUG, "Created VCS %s",
				   algo == SP_DOWNLINK_ALGO ? "RX": "TX");

  vcs_portsdef_init(&vcs->inport,0, 1, 8000, 0);
  if (algo == SP_UPLINK_ALGO) vcs_portsdef_init(&vcs->inport, 1, 1, 8000, 0);
  vcs_portsdef_init(&vcs->outport,0, 1, 8000, 0);
  return error;

}


speech_proc_error_t speech_proc_close(void *ctx)
{
  // close and deallocate context
  speech_proc_error_t error = SP_ERROR_NONE;
  speech_proc_vcs_t *my = (speech_proc_vcs_t*) ctx;
  vcs_processing_kind_t algo = my->ctx->get_kind(my->ctx);
  int result =  my->ctx->destroy(my->ctx);
  if (result != 0) {
    error = result + SP_ERROR_SPECIFIC_START;
  } else my->mylog.fn.log(&my->mylog.fn, VCS_DEBUG, "Destroyed VCS %s",
				   algo == VCS_DOWNLINK ? "RX": "TX");
  free(my);
  return error;
}

speech_proc_error_t speech_proc_set_parameter(void *ctx,
											  int index,
											  void *config_struct)
{

  speech_proc_error_t error = SP_ERROR_NONE;
  speech_proc_vcs_t *my = (speech_proc_vcs_t*) ctx;
  int vcs_index;
  void* vcs_param;
  if (extract_vcs_param(index, config_struct, &vcs_index, &vcs_param) == 0) {
	int result =
	  my->ctx->set_param(my->ctx,
						 index,
						 (unsigned char*) config_struct,
						 0);
	free(vcs_param);
	if (result != VCS_OK) error = result + SP_ERROR_SPECIFIC_START;
  }
  //return error; once we not longer might get nokia stuff
  return  SP_ERROR_NONE;
}

speech_proc_error_t speech_proc_set_config(void *ctx,
										   int index,
										   void *config_struct)
{
  speech_proc_error_t error = SP_ERROR_NONE;
  speech_proc_vcs_t *my = (speech_proc_vcs_t*) ctx;
  int vcs_index;
  void* vcs_config;
  if (extract_vcs_config(my, index, config_struct, &vcs_index, &vcs_config) == 0) {
	int result =
	  my->ctx->set_config(my->ctx,
						  vcs_index,
						  (unsigned char*) vcs_config,
						  0);
	free(vcs_config);
	if (result != VCS_OK) {
	  my->mylog.fn.log(&my->mylog.fn, VCS_INFO, "Invalid parameter %i %i", index, result);
	  error = result + SP_ERROR_SPECIFIC_START;
	}
  } else  my->mylog.fn.log(&my->mylog.fn, VCS_INFO, "Invalid parameter %d", index);
  //return error; once we no longer get nokia stuff
   return  error;
}

speech_proc_error_t speech_proc_set_mode(void *ctx,
										 const speech_proc_settings_t* mode,
										 const speech_proc_port_settings_t* inports,
										 const speech_proc_port_settings_t* outports)
{
  // setup 8 and 16 kHz and number of channels
  speech_proc_error_t error = SP_ERROR_NONE;
  speech_proc_vcs_t *my = (speech_proc_vcs_t*) ctx;
  int result = VCS_OK;
  unsigned int i;
  unsigned int samplerate = 0;
  my->inport.nb_ports = 0;
  for (i = 0; i < inports->nb_ports; i++) {
	vcs_portsdef_init(&my->inport, i,
					  inports->port[i].nb_channels,
					  inports->port[i].samplerate,
					  inports->port[i].interleaved);
	if (i == 0) samplerate = inports->port[i].samplerate;
	else if (samplerate != inports->port[i].samplerate) result  = VCS_ERROR_PARAM;
  }
  my->outport.nb_ports = 0;
  for (i = 0; i < outports->nb_ports; i++) {
	vcs_portsdef_init(&my->outport, i,
					   outports->port[i].nb_channels,
					   outports->port[i].samplerate,
					   outports->port[i].interleaved);
	 if (samplerate != outports->port[i].samplerate) result  = VCS_ERROR_PARAM;
  }
  my->mode.resolution = mode->resolution;
  my->mode.samplerate = samplerate;
  my->mode.framesize = mode->framesize * samplerate/1000;
  if (result == VCS_OK) {
	result =
	  my->ctx->set_mode(my->ctx,
						&my->mode,
						&my->inport,
						&my->outport);
  }
  if (result != VCS_OK) error = result + SP_ERROR_SPECIFIC_START;
  return error;
}

speech_proc_error_t speech_proc_open(void *ctx)
{
  // if init does not cover all initialization then do it here
  speech_proc_error_t error = SP_ERROR_NONE;
  return error;
}

speech_proc_error_t speech_proc_process(void *ctx,
										short** input, int nb_input,
										short** output, int nb_output,
                                        const speech_proc_frame_info_t *frame_info)
{
  // execute one pcm block using input and output buffers
  speech_proc_error_t error = SP_ERROR_NONE;
  int result;
  speech_proc_vcs_t *my = (speech_proc_vcs_t*) ctx;
  result = my->ctx->process(my->ctx, input, nb_input, output, nb_output);
  if (result != VCS_OK) error = result + SP_ERROR_SPECIFIC_START;
  return error;
}

void * speech_proc_get_processing_info(void *ctx)
{
  // get all information needed (uplink or downlink depending on ctx)
  // that the other side needs. Will be set with speech_proc_set_processing_info
  speech_proc_vcs_t *my = (speech_proc_vcs_t*) ctx;
  struct vcs_export_s *data = 0;
  my->ctx->exportdata(my->ctx, &data);
  return data;
}

speech_proc_error_t speech_proc_set_processing_info(void *ctx,
													void *info)
{
  // sets all information needed (uplink or downlink depending on ctx)
  // that is retreived from other side through with speech_proc_get_processing_info
  speech_proc_error_t error = SP_ERROR_NONE;
  speech_proc_vcs_t *my = (speech_proc_vcs_t*) ctx;
  struct vcs_export_s *data = (struct vcs_export_s*) info;;
  my->ctx->importdata(my->ctx, &data);
  return error;
}

speech_proc_error_t speech_proc_get_sidetone_gain(void *ctx,
												  int *gain,
												  int *updated)
{
  return SP_ERROR_UNSUPPORTED;
}

speech_proc_error_t speech_proc_get_config(void *ctx, int index, void *config_struct)
{
  return SP_ERROR_UNSUPPORTED;
}








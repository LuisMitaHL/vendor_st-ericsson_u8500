/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_serx.c
 * \brief    Speech enhancement downlink (RX)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs_serx.h"
#include <string.h>

#include <stdio.h>

#ifdef VCSSE
# include "vcs_queue.h"
#endif

struct vcs_serx_s 
{
  int enabled;
  int running;
#ifdef VCSSE
  struct SE_RxComponent_t *instance;
  SE_RxParameters_t       config;
  vcs_log_t                *mylog;
  vcs_queue_t              *queue;
#endif
};

#ifdef VCSSE
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static void my_queue_free(void* data)
{
  struct SE_TxExportContainer_t* d = (struct SE_TxExportContainer_t*) data;
  SETX_ExportContainer_Destruct(d);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int vcs_serx_enable2(vcs_serx_t* rx, const vcs_algo_mode_t *mode)
{
  int result = VCS_OK;
  rx->enabled = 1;

  if (!rx->running)
  {
    if (rx->instance == NULL)
    {
      rx->instance = SERX_Construct();
      if (rx->instance == NULL) 
      {
        rx->mylog->log(rx->mylog, VCS_ERROR, "Cannot allocate memory for SERX\n");
        result = VCS_ERROR_MEMORY;
        return result;
      }
    }

    {
      SE_Return_t result;
      (void) SERX_Configure(rx->instance, &rx->config,
							  mode->samplerate == 8000 ? SE_SAMPLE_RATE_8000 : SE_SAMPLE_RATE_16000,
							  mode->nb_channels == 2 ? SE_STEREO : SE_MONO,
							  mode->nb_channels == 2 ? SE_STEREO : SE_MONO);
	  
      result = SE_NO_ERROR;
      rx->mylog->log(rx->mylog, VCS_DEBUG, "SERX enabled\n");
      rx->running = 1;
    }
  }
  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_serx_t* vcs_serx_create(vcs_log_t* log)
{
  vcs_serx_t* rx = (vcs_serx_t*) calloc(1, sizeof(vcs_serx_t));

  if (rx != 0)
  {
    rx->mylog = log;
    rx->queue = vcs_queue_alloc(5);
    if (!rx->queue)
    {
      free(rx);
      rx = 0;
    }
  }
  return rx;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_destroy(vcs_serx_t *rx)
{
  vcs_serx_disable(rx);
  vcs_queue_destroy(rx->queue, my_queue_free);
  free(rx);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_serx_enable(vcs_serx_t * rx, const vcs_algo_mode_t *mode)
{
  return vcs_serx_enable2(rx, mode);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_serx_reset(vcs_serx_t *rx, const vcs_algo_mode_t *mode)
{
  if (rx->running)
  {
    (void)SERX_Configure(rx->instance, &rx->config,
							mode->samplerate == 8000 ? SE_SAMPLE_RATE_8000 : SE_SAMPLE_RATE_16000,
							mode->nb_channels == 2 ? SE_STEREO : SE_MONO,
							mode->nb_channels == 2 ? SE_STEREO : SE_MONO);
    rx->mylog->log(rx->mylog, VCS_DEBUG, "SERX reset\n");
    rx->running = 1;
  }
  return VCS_OK;
}
 
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_serx_configure(vcs_serx_t *rx, const vcs_algo_mode_t *mode, const vcs_serx_config_t* param)
{
  int result = VCS_OK;

  if (param->enabled) 
  {
    rx->config = param->rxparam;
	
    if (!rx->running)
    {
      vcs_serx_enable2(rx, mode);
    } 
    else
    {
      (void) SERX_Configure(rx->instance, &rx->config,
							  mode->samplerate == 8000 ? SE_SAMPLE_RATE_8000 : SE_SAMPLE_RATE_16000,
							  mode->nb_channels == 2 ? SE_STEREO : SE_MONO,
							  mode->nb_channels == 2 ? SE_STEREO : SE_MONO);
	  
      rx->mylog->log(rx->mylog, VCS_DEBUG, "SERX reconfigured (enabled)\n");
      rx->running = 1;
    }
  }
  else
  {
    vcs_serx_disable(rx);
  }
  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_disable(vcs_serx_t *rx)
{
  vcs_queue_empty(rx->queue, my_queue_free);
  SERX_Destruct(rx->instance);

  rx->instance = NULL;
  rx->enabled  = 0;
  rx->running  = 0;
  rx->mylog->log(rx->mylog, VCS_DEBUG, "SERX disabled\n");
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_serx_is_enabled(vcs_serx_t *rx)
{
  return rx->enabled;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_serx_is_running(vcs_serx_t *rx)
{
  return rx->running;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_execute(vcs_serx_t *rx, short** input, int nb_input, short** output, int nb_output)
{
  SERX_Execute( (const short**) input, output, rx->instance);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_export(vcs_serx_t *rx, vcs_se_export_t **data)
{
  if (!vcs_queue_isempty(rx->queue))
  {
    *data = (vcs_se_export_t*)vcs_queue_dequeue(rx->queue);
  } 
  else 
    *data = 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_import(vcs_serx_t *rx, vcs_se_export_t **data)
{
  if (*data)
  {
    if (rx->running)
    {
      SETX_ExportContainer_Empty(*data, rx->instance);
      if (vcs_queue_add(rx->queue, *data) == 0)
      {
        SETX_ExportContainer_Destruct(*data);
      }
    } 
    else
      SETX_ExportContainer_Destruct(*data);
  }
  *data = 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_config_tostring(char* buffer, unsigned int len, const vcs_serx_config_t* param)
{
  int pos = 0;
  int i;

  pos += snprintf(buffer +pos, len-pos, "%d,",    param->enabled);
  pos += snprintf(buffer +pos, len-pos, "nr{%d,", param->rxparam.RxNoiseReduction.enabled);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxNoiseReduction.adaptiveHighPassFilter);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxNoiseReduction.desiredNoiseReduction);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxNoiseReduction.nrCutOffFrequency);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxNoiseReduction.noiseReductionDuringSpeech);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxNoiseReduction.noiseReductionTradeoff);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxNoiseReduction.noiseFloorPower);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxNoiseReduction.highPassFilterSnrThreshold);
  pos += snprintf(buffer +pos, len-pos, "%d},",   param->rxparam.RxNoiseReduction.highPassCutOffMargin);
  pos += snprintf(buffer +pos, len-pos, "eq{%d,", param->rxparam.RxEqualize.enabled);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxEqualize.speechEnhance);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxEqualize.calibrationMode);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxEqualize.complexityLevel);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxEqualize.minNoiseSpeechEnhance);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxEqualize.saturationMargin);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxEqualize.minimumTargetSNR);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxEqualize.maxGainTargetSNR);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxEqualize.thresholdSNREnergyRedist);
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->rxparam.RxEqualize.maxGainEnergyRedistHf);
  pos += snprintf(buffer +pos, len-pos, "%d,{",   param->rxparam.RxEqualize.maxLossEnergyRedistLf);
 
#define RXEQ_MAX_NUM_BANDS (40) 
  for (i = 0; i < RXEQ_MAX_NUM_BANDS; i++)
  {
    if (i < RXEQ_MAX_NUM_BANDS-1)
      pos += snprintf(buffer +pos, len-pos, "%d,", param->rxparam.RxEqualize.txAcomp_p[i]);
    else
      pos += snprintf(buffer +pos, len-pos, "%d},{", param->rxparam.RxEqualize.txAcomp_p[i]);
  }
  
  for (i = 0; i < RXEQ_MAX_NUM_BANDS; i++)
  {
    if (i < RXEQ_MAX_NUM_BANDS-1)
      pos += snprintf(buffer +pos, len-pos, "%d,", param->rxparam.RxEqualize.rxAcomp_p[i]);
    else
      pos += snprintf(buffer +pos, len-pos, "%d}}", param->rxparam.RxEqualize.rxAcomp_p[i]);
   }
}
 
int vcs_serx_probe_is_supported(const vcs_serx_t *tx, vcs_probe_points_t kind)
{
  switch (kind) {
  case VCS_PROBE_ADEQ_TX_CALIBRATION_DATA:
  case VCS_PROBE_ADEQ_RX_CALIBRATION_DATA:
    return 1;
  default:
	return 0;
  }
}
vcs_se_probedata_t* vcs_serx_probe_data_alloc(vcs_serx_t *rx, vcs_probe_points_t kind)   
{
  vcs_se_probedata_t * probedata = 0;
  int size = 0;
  if (rx->running == 0) return 0;
  if (kind == VCS_PROBE_ADEQ_TX_CALIBRATION_DATA) {
    size = SE_MM_Probe(0, 0, rx->instance, SE_MM_PROBE_RXEQ_TX_NOISESPECTRUM);
  } else if (kind == VCS_PROBE_ADEQ_RX_CALIBRATION_DATA) {
    size = SE_MM_Probe(0, 0, rx->instance, SE_MM_PROBE_RXEQ_RX_SPEECHSPECTRUM);
  }
  if (size > 0) {
	  size_t totsize = sizeof(vcs_se_probedata_t)+(size-1)*sizeof(short);
	  probedata = (vcs_se_probedata_t*) malloc(totsize);
	  if (probedata != 0) {
	    memset(probedata, 0, totsize);
		probedata->kind = kind;
		probedata->datalen = size;
		probedata->alloclen = size;
	  }
  }
  return probedata;
}
   
void vcs_serx_probe_data_free(vcs_se_probedata_t* data)
{
  if (data) free(data);
}
  
int vcs_serx_probe_data_get(vcs_serx_t *rx, vcs_se_probedata_t* data)
{
  int result;
  if (data->kind == VCS_PROBE_ADEQ_TX_CALIBRATION_DATA) {
    int outsize = SE_MM_Probe(data->data, 0, rx->instance, SE_MM_PROBE_RXEQ_TX_NOISESPECTRUM);
    if (outsize != data->datalen)  {
	  rx->mylog->log(rx->mylog, VCS_ERROR, 
	                "SERX SE_MM_PROBE_RXEQ_TX_NOISESPECTRUM buf len %d differs from fill len %d", 
	                data->datalen, outsize);
	  if (outsize < data->alloclen) {
	    data->datalen = outsize; 
		result = 1;
	  } else result = 0;
	} else result = 1;
  } else if (data->kind == VCS_PROBE_ADEQ_RX_CALIBRATION_DATA) {
    int outsize = SE_MM_Probe(data->data, 0, rx->instance, SE_MM_PROBE_RXEQ_RX_SPEECHSPECTRUM);
    if (outsize != data->datalen)  {
	  rx->mylog->log(rx->mylog, VCS_ERROR, 
	                 "SERX SE_MM_PROBE_RXEQ_RX_SPEECHSPECTRUM buf len %d differs from fill len %d", 
	                 data->datalen, outsize);
      if (outsize < data->alloclen) {
        data->datalen = outsize; 
        result = 1;
	  } else result = 0;
	} else result = 1;
  } else result = 1;
  return result;
}

#else

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_serx_t* vcs_serx_create(vcs_log_t* log)
{
  vcs_serx_t* rx = (vcs_serx_t*) calloc(1, sizeof(vcs_serx_t));

  if (rx != NULL) 
  {
    rx->mylog = log;
  }
  return rx;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_destroy(vcs_serx_t *rx)
{
  free(rx);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_serx_reset(vcs_serx_t *rx, const vcs_algo_mode_t *mode)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_serx_configure(vcs_serx_t *rx,const vcs_algo_mode_t *mode, const vcs_serx_config_t* param)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_disable(vcs_serx_t *rx)
{
}


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_serx_is_enabled(vcs_serx_t *rx)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_serx_is_running(vcs_serx_t *rx)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_execute(vcs_serx_t *rx, short** input, int nb_input, short** output, int nb_output)
{
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_export(vcs_serx_t *rx, vcs_se_export_t **data)
{
  *data = 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_import(vcs_serx_t *rx, vcs_se_export_t **data)
{
  *data = 0;
}


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_serx_config_tostring(char* buffer, unsigned int len, const vcs_serx_config_t* param)
{
  buffer[0] = '\0';
}

int vcs_serx_probe_is_supported(vcs_se_probe_type_t kind) 
{
  return 0;
}

vcs_se_probedata_t* vcs_serx_probe_data_alloc(vcs_se_probe_type_t kind)
{
  return 0;
} 
   
void vcs_serx_probe_data_free(vcs_se_probe_type_t kind, vcs_se_probedata_t* data)
{
} 
  
int vcs_serx_probe_data_get(vcs_se_probe_type_t kind, vcs_se_probedata_t* data)
{
  return 0;
}

#endif // VCSSE


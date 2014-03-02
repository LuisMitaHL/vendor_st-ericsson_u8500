/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_setx.c
 * \brief    Speech enhancement uplink
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs_setx.h"
#include <string.h>
#include <stdio.h>

#ifdef VCSSE
# include "vcs_queue.h"
#endif

struct vcs_setx_s
{
  int       enabled;
  int       running;
  vcs_log_t *mylog;
#ifdef VCSSE
  struct SE_TxComponent_t *instance;
  SE_TxParameters_t       config;
  vcs_queue_t             *queue;
  vcs_queue_t             *sampleDelay;
#endif
};


#ifdef VCSSE
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static void my_queue_free(void* data)
{
  struct SE_TxExportContainer_t*  d = (struct SE_TxExportContainer_t*) data;
  SETX_ExportContainer_Destruct(d);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int vcs_setx_enable2(vcs_setx_t * tx, const vcs_algo_mode_t *mode)
{
  int result = VCS_OK;
  tx->enabled = 1;

  if (!tx->running)
  {
    if (tx->instance == NULL)
    {
      tx->instance = SETX_Construct();
    
      if (tx->instance == NULL)
      {
        tx->mylog->log(tx->mylog, VCS_ERROR, "Cannot allocate memory for SETX");
        result = VCS_ERROR_MEMORY;
        return result;
      }
    }

    {
      SE_Return_t result;
      (void) SETX_Configure(tx->instance, &tx->config,
						    mode->samplerate == 8000 ? SE_SAMPLE_RATE_8000 : SE_SAMPLE_RATE_16000,
						 // set MONO on both RX and TX input. Any additional channel is used for NR
							SE_MONO /* RxIn mode->nb_channels == 2 ? SE_STEREO : SE_MONO */,
							SE_MONO, /* TxIn mode->nb_channels == 2 ? SE_STEREO : SE_MONO */
							mode->nb_channels /* number of mic*/);
      result = SE_NO_ERROR;
      tx->mylog->log(tx->mylog, VCS_DEBUG, "SETX enabled\n");
      tx->running = 1;
    }
  }
  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_setx_t* vcs_setx_create(vcs_log_t* log)
{
  vcs_setx_t* tx = (vcs_setx_t*) calloc(1, sizeof(vcs_setx_t) );

  if (tx != NULL)
  {
    tx->mylog = log;
    tx->queue = vcs_queue_alloc(5);
    if (!tx->queue)
    {
      free(tx);
      tx = 0;
    }
  }
  
  return tx;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_destroy(vcs_setx_t *tx)
{
  vcs_setx_disable(tx);
  vcs_queue_destroy(tx->queue, my_queue_free);
  free(tx);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_setx_enable(vcs_setx_t * tx, const vcs_algo_mode_t *mode)
{
  return vcs_setx_enable2(tx, mode);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_setx_reset(vcs_setx_t *tx, const vcs_algo_mode_t *mode)
{
  if (tx->running)
  {
    (void) SETX_Configure(tx->instance, &tx->config,
							mode->samplerate == 8000 ? SE_SAMPLE_RATE_8000 : SE_SAMPLE_RATE_16000,
							// set MONO on both RX and TX input. Any additional channel is used for NR
							SE_MONO /* RxIn mode->nb_channels == 2 ? SE_STEREO : SE_MONO */,
							SE_MONO, /* TxIn mode->nb_channels == 2 ? SE_STEREO : SE_MONO */
							mode->nb_channels /* number of mic*/);
    tx->mylog->log(tx->mylog, VCS_DEBUG, "SETX reset\n");
    tx->running = 1;
  }
  return VCS_OK;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_setx_configure(vcs_setx_t *tx, const vcs_algo_mode_t *mode, const vcs_setx_config_t* param)
{
  int result = VCS_OK;

  if (param->enabled)
  {
    tx->config = param->txparam;
    
    if (!tx->running)
    {
      vcs_setx_enable2(tx, mode);
    }
    else
    {
      (void) SETX_Configure(tx->instance, &tx->config,
							  mode->samplerate == 8000 ? SE_SAMPLE_RATE_8000 : SE_SAMPLE_RATE_16000,
							  SE_MONO /* RxIn mode->nb_channels == 2 ? SE_STEREO : SE_MONO */,
							  SE_MONO /* TxIn mode->nb_channels == 2 ? SE_STEREO : SE_MONO */,
							  mode->nb_channels /* number of mic*/);

      tx->mylog->log(tx->mylog, VCS_DEBUG, "SETX reconfigured\n");
      tx->running = 1;
    }
  } 
  else
  {
    vcs_setx_disable(tx);
  }
  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_disable(vcs_setx_t *tx)
{
  vcs_queue_empty(tx->queue, my_queue_free);
  SETX_Destruct(tx->instance);
  tx->instance = NULL;
  tx->enabled  = 0;
  tx->running  = 0;

  tx->mylog->log(tx->mylog, VCS_DEBUG, "SETX disabled\n");
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_setx_is_enabled(vcs_setx_t *tx)
{
  return tx->enabled;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_setx_is_running(vcs_setx_t *tx)
{
  return tx->running;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_execute(vcs_setx_t *tx, short** input, int nb_input, short** ecref, int nb_ecref, short** output, int nb_output,
                      short** linoutput, int nb_linoutput)
{
  SETX_Execute((const short**)ecref, (const short**)input, output, linoutput, tx->instance);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_export(vcs_setx_t *tx, vcs_se_export_t  **data)
{
  struct SE_TxExportContainer_t* container = 0;
  
  if (tx->running)
  {
    if (vcs_queue_isempty(tx->queue))
    {
      container = SETX_ExportContainer_Construct();
    }
    else
    {
      container = (struct SE_TxExportContainer_t*) vcs_queue_dequeue(tx->queue);
    }
    if (container)
    {
      SETX_ExportContainer_Fill(container, tx->instance);
    }
  }
  *data = container;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_import(vcs_setx_t *tx, vcs_se_export_t  **data)
{
  if (*data != 0)
  {
    if (tx->running)
    {
      if (vcs_queue_isfull(tx->queue))
      {
        SETX_ExportContainer_Destruct(*data);
      }
      else
      {
        vcs_queue_add(tx->queue, *data);
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
void vcs_setx_config_tostring(char* buffer, unsigned int len, const vcs_setx_config_t* param)
{
  int pos = 0;
  int i;
  pos += snprintf(buffer +pos, len-pos, "%d,",    param->enabled);
  pos += snprintf(buffer +pos, len-pos, "ec{%d,", (int)param->txparam.EchoControl.enabled);
  pos += snprintf(buffer +pos, len-pos, "%d,",    (int)param->txparam.EchoControl.linearEchoCancellation);
  pos += snprintf(buffer +pos, len-pos, "%d,",    (int)param->txparam.EchoControl.echoDetection);
  pos += snprintf(buffer +pos, len-pos, "%d,",    (int)param->txparam.EchoControl.highPassFilter);
  pos += snprintf(buffer +pos, len-pos, "%d,",    (int)param->txparam.EchoControl.residualEchoControl);
  pos += snprintf(buffer +pos, len-pos, "%d,",    (int)param->txparam.EchoControl.comfortNoise);
  pos += snprintf(buffer +pos, len-pos, "%d,",    (int)param->txparam.EchoControl.pureDelay);
  pos += snprintf(buffer +pos, len-pos, "%d,",    (int)param->txparam.EchoControl.filterLength);
  pos += snprintf(buffer +pos, len-pos, "%d,",    (int)param->txparam.EchoControl.shadowFilterLength);
  pos += snprintf(buffer +pos, len-pos, "%d,",    (int)param->txparam.EchoControl.maxEchoPathGain);
  pos += snprintf(buffer +pos, len-pos, "%d,{",   (int)param->txparam.EchoControl.adaptiveResidualLoss);

  for (i = 0; i < 40; i++)
  {
    if (i < 39)
      pos += snprintf(buffer +pos, len-pos, "%d,",  (int)param->txparam.EchoControl.rxResidualLossRegressor_p[i]);
    else
      pos += snprintf(buffer +pos, len-pos, "%d},", (int)param->txparam.EchoControl.rxResidualLossRegressor_p[i]);
  }
  pos += snprintf(buffer +pos, len-pos, "%d,",  (int)param->txparam.EchoControl.nonlinearSpeakerModelling);
  pos += snprintf(buffer +pos, len-pos, "%d,",  (int)param->txparam.EchoControl.residualEchoMargin);
  pos += snprintf(buffer +pos, len-pos, "%d,{", (int)param->txparam.EchoControl.echoDecayTime);
   
  for (i = 0; i < 40; i++)
  {
    if (i < 39)
      pos += snprintf(buffer +pos, len-pos, "%d,",  (int)param->txparam.EchoControl.maxResidualLoss_p[i]);
    else
      pos += snprintf(buffer +pos, len-pos, "%d},", (int)param->txparam.EchoControl.maxResidualLoss_p[i]);
  }
  pos += snprintf(buffer +pos, len-pos, "%d,{",     (int)param->txparam.EchoControl.nonLinearEchoDecayTime);

  for (i = 0; i < 6; i++)
  {
    if (i < 5)
      pos += snprintf(buffer +pos, len-pos, "%d,",  (int)param->txparam.EchoControl.harmonicGains_p[i]);
    else
      pos += snprintf(buffer +pos, len-pos, "%d},{", (int)param->txparam.EchoControl.harmonicGains_p[i]);
  }
  for (i = 0; i < 32; i++)
  {
    if (i < 31)
      pos += snprintf(buffer +pos, len-pos, "%d,",  (int)param->txparam.EchoControl.fundamentalGains_p[i]);
    else
      pos += snprintf(buffer +pos, len-pos, "%d},", (int)param->txparam.EchoControl.fundamentalGains_p[i]);
  }
  pos += snprintf(buffer +pos, len-pos, "%d,{",     (int)param->txparam.EchoControl.harmonicActivationLevel);
  
  for (i = 0; i < 40; i++)
  {
    if (i < 39)
      pos += snprintf(buffer +pos, len-pos, "%d,",  (int)param->txparam.EchoControl.spectralLeakageGains_p[i]);
    else
      pos += snprintf(buffer +pos, len-pos, "%d},", (int)param->txparam.EchoControl.spectralLeakageGains_p[i]);
  }
  
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.EchoControl.leakageActivationLevel);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.EchoControl.spectralMaskingMargin);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.EchoControl.temporalMaskingTime);
  pos += snprintf(buffer +pos, len-pos, "%d},",    (int)param->txparam.EchoControl.residualEchoReleaseRate);
  
  pos += snprintf(buffer +pos, len-pos, " mmic{%d,", (int)param->txparam.MultiMicrophone.enabled);
  pos += snprintf(buffer +pos, len-pos, "{%d,", (int)param->txparam.MultiMicrophone.txChannelMapping_p[0]);
  pos += snprintf(buffer +pos, len-pos, "%d},", (int)param->txparam.MultiMicrophone.txChannelMapping_p[1]);
  pos += snprintf(buffer +pos, len-pos, "%d,", (int)param->txparam.MultiMicrophone.reduceFarField);
  pos += snprintf(buffer +pos, len-pos, "%d,", (int)param->txparam.MultiMicrophone.reduceWindNoise);
  pos += snprintf(buffer +pos, len-pos, "%d},", (int)param->txparam.MultiMicrophone.referenceMicrophoneChannel);
  
  pos += snprintf(buffer +pos, len-pos, "nr{%d,", (int)param->txparam.TxNoiseReduction.enabled);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxNoiseReduction.adaptiveHighPassFilter);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxNoiseReduction.desiredNoiseReduction);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxNoiseReduction.nrCutOffFrequency);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxNoiseReduction.noiseReductionDuringSpeech);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxNoiseReduction.noiseReductionTradeoff);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxNoiseReduction.noiseFloorPower);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxNoiseReduction.highPassFilterSnrThreshold);
  pos += snprintf(buffer +pos, len-pos, "%d},",    (int)param->txparam.TxNoiseReduction.highPassCutOffMargin);
  pos += snprintf(buffer +pos, len-pos, "lc{%d,",  (int)param->txparam.TxLevelControl.enabled);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxLevelControl.adaptiveSignalGain);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxLevelControl.noiseAdaptiveGain);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxLevelControl.noiseAdaptiveGainSlope);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxLevelControl.noiseAdaptiveGainThreshold);
  pos += snprintf(buffer +pos, len-pos, "%d,",     (int)param->txparam.TxLevelControl.targetLevel);
  pos += snprintf(buffer +pos, len-pos, "%d}",     (int)param->txparam.TxLevelControl.staticGain);
}

int vcs_setx_probe_is_supported(const vcs_setx_t *tx, vcs_probe_points_t kind) 
{
 if (tx->running == 0) return 0;
 switch (kind) {
  case VCS_PROBE_TX_EC_FILTERTAP_LEFT:
    return 1;
  default:
	return 0;
  } 
}

vcs_se_probedata_t* vcs_setx_probe_data_alloc(vcs_setx_t *tx, vcs_probe_points_t kind)
{
  vcs_se_probedata_t * probedata = 0;
  if (tx->running == 0) return 0;
  if (kind == VCS_PROBE_TX_EC_FILTERTAP_LEFT) {
    int size = SE_MM_Probe(0, tx->instance, 0, SE_MM_PROBE_ECHOFILTER_LEFTTX_LEFTRX);
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
  }
  return probedata;
} 
   
void vcs_setx_probe_data_free(vcs_se_probedata_t* data)
{
  if (data) free(data);
} 
  
int vcs_setx_probe_data_get(vcs_setx_t *tx, vcs_se_probedata_t* data)
{
    int result;
    if (data->kind == VCS_PROBE_TX_EC_FILTERTAP_LEFT) {
        int outsize = SE_MM_Probe(0, tx->instance, 0, SE_MM_PROBE_ECHOFILTER_LEFTTX_LEFTRX);
        if (outsize != data->datalen)  {
            tx->mylog->log(tx->mylog, VCS_ERROR, "SETX vcs_setx_probe_data_get allocated len %d not same as required %d", data->datalen, outsize);
            result = 0;
        } else {
            (void) SE_MM_Probe(data->data, tx->instance, 0, SE_MM_PROBE_ECHOFILTER_LEFTTX_LEFTRX);
            result = 1;
        }
    } else result = 0;

  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
#else
vcs_setx_t* vcs_setx_create(vcs_log_t* log)
{
  vcs_setx_t* tx = (vcs_setx_t*) calloc(1, sizeof(vcs_setx_t));

  if (tx != NULL)
  {
    tx->mylog = log;
  }
  return tx;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_destroy(vcs_setx_t *tx)
{
  free(tx);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_setx_reset(vcs_setx_t *tx, const vcs_algo_mode_t *mode)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_setx_configure(vcs_setx_t *tx, const vcs_algo_mode_t *mode, const vcs_setx_config_t* param)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_disable(vcs_setx_t *tx)
{
}


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_setx_is_enabled(vcs_setx_t *tx)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_setx_is_running(vcs_setx_t *tx)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_execute(vcs_setx_t *tx, short** input, int nb_input, short** ecref, int nb_ecref, short** output, int nb_output)
{
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_config_tostring(char* buffer, unsigned int len, const vcs_setx_config_t* param)
{
  buffer[0] = '\0';
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_export(vcs_setx_t *tx, vcs_se_export_t  **data)
{
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_setx_import(vcs_setx_t *tx, vcs_se_export_t  **data)
{
}

int vcs_setx_probe_is_supported(const vcs_setx_t *tx, vcs_se_probe_type_t kind) 
{
  return 0;
}

vcs_se_probedata_t* vcs_setx_probe_data_alloc(vcs_setx_t *tx, vcs_se_probe_type_t kind)
{
  return 0;
} 
   
void vcs_setx_probe_data_free(vcs_setx_t *tx, vcs_se_probe_type_t kind, vcs_se_probedata_t* data)
{
} 
  
int vcs_setx_probe_data_get(vcs_setx_t *tx, vcs_se_probe_type_t kind, vcs_se_probedata_t* data)
{
  return 0;
}

#endif // VCSSE


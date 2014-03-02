/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_rx.c
 * \brief    Voice shell downlink (RX)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs_tx.h"
#include "string.h"

#include "vcs_drc.h"
#include "vcs_serx.h"
#include "vcs_ctmrx.h"
#include "vcs_comfortnoise.h"
#include "vcs_queue.h"
#include "vcs_plugin.h"
#ifdef VCS_SINE_GENERATION
#include "vcs_sine.h"
#endif

#define VCS_NB_BUFFERS          2
#define VCS_RX_IN
#define VCS_RX_MAX_OUT_CHANNELS 1
#define VCS_RX_MAX_IN_CHANNELS  1

typedef struct
{
  vcs_ctx_t      ctx;
  vcs_log_t      *mylog;
  vcs_mode_t     mode;
  vcs_portsdef_t input;
  vcs_portsdef_t output;
  vcs_drc_t      *drc;
  vcs_serx_t     *serx;
  vcs_ctmrx_t    *ctmrx;
  vcs_comfortnoise_t *comfortnoise;
  vcs_plugin_t   *plugin;
  vcs_algorithm_t* pluginrxfirst;
  vcs_algorithm_t* pluginrxlast;
  vcs_queue_t    *queue;
  vcs_se_probedata_t *eq_tx_noisespectrum;
  vcs_se_probedata_t *eq_rx_speechspectrum;
#ifdef VCS_SINE_GENERATION
  vcs_sine_t     sine;
#endif
} vcs_rx_data_t;

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int vcs_rx_init(vcs_rx_data_t *rx);

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int is_valid_output (vcs_rx_data_t *const rx, const vcs_mode_t *const mode, const vcs_portsdef_t *const ports) 
{
  if (ports->nb_ports != 1)
  {
    rx->mylog->log(rx->mylog, VCS_DEBUG, "VCS RX output invalid port number: %i", ports->nb_ports);
    return 0;
  }

  rx->mylog->log(rx->mylog, VCS_DEBUG, "VCX RX output samplerate:%i, nb_channels:%i interleaved:%i",
                 ports->port[0].samplerate,
                 ports->port[0].nb_channels,
                 ports->port[0].interleaved);

  // same sample rate and only one channel (mono) supported
  return ports->port[0].samplerate == mode->samplerate && ports->port[0].nb_channels >= 1 && ports->port[0].nb_channels <= VCS_RX_MAX_OUT_CHANNELS;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int is_valid_input (vcs_rx_data_t *const rx, const vcs_mode_t *const mode, const vcs_portsdef_t *const ports)
{
  if (ports->nb_ports != 1)
  {
    rx->mylog->log(rx->mylog, VCS_INFO, "VCS RX input invalid port number:%i", ports->nb_ports);
    return 0;
  }

  rx->mylog->log(rx->mylog, VCS_DEBUG, "VCX RX input samplerate:%i, nb_channels:%i interleaved:%i",
                 ports->port[0].samplerate,
	         ports->port[0].nb_channels,
                 ports->port[0].interleaved);
			 	
  // same sample rate and only one channel (mono) supported
  return ports->port[0].samplerate == mode->samplerate && ports->port[0].nb_channels >= 1 && ports->port[0].nb_channels <= VCS_RX_MAX_IN_CHANNELS;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static void my_rx_reset(vcs_rx_data_t *const rx)
{
  vcs_algo_mode_t amode;
  vcs_algorith_mode_t pluginmode;
  
  vcs_serx_probe_data_free(rx->eq_tx_noisespectrum);
  rx->eq_tx_noisespectrum = 0;
  vcs_serx_probe_data_free(rx->eq_rx_speechspectrum);
  rx->eq_rx_speechspectrum = 0;
  // DRC 
  vcs_to_algo_mode(&amode, &rx->mode, rx->input.port[0].nb_channels, 0); 
  vcs_to_algorithm_mode(&pluginmode, &rx->mode, rx->input.port[0].nb_channels, 0); 
  
  vcs_drc_reset(rx->drc, &amode);

  // CTM
  vcs_ctmrx_reset(rx->ctmrx, &amode);

  // SE RX 
  vcs_serx_reset(rx->serx, &amode);

  // comfort noise
  vcs_comfortnoise_reset(rx->comfortnoise, &amode);
  
  // customer plugin 
  rx->pluginrxfirst->reset(rx->pluginrxfirst, &pluginmode);
  rx->pluginrxlast->reset(rx->pluginrxlast, &pluginmode);

}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_set_mode(struct vcs_ctx_s* self, const vcs_mode_t* mode, const vcs_portsdef_t *const input, const vcs_portsdef_t *const output)
{
  vcs_rx_data_t *rx = (vcs_rx_data_t*) self;
  int result = VCS_ERROR_PARAM;

  rx->mylog->log(rx->mylog, VCS_DEBUG, "VCS RX resolution:%i samplerate:%i framesize:%i", mode->resolution, mode->samplerate, mode->framesize);
			 
  if ((mode->resolution == 16) && ((mode->samplerate == 8000) || (mode->samplerate == 16000)) &&
	  is_valid_input(rx, mode, input) && is_valid_output(rx, mode, output))
  {
    rx->mode   = *mode;
    rx->input  = *input;
    rx->output = *output;
    result     = VCS_OK;
    my_rx_reset(rx);
  } 
  else 
    rx->mylog->log(rx->mylog, VCS_INFO, "VCS RX unsupported mode and port def");

  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_set_param(struct vcs_ctx_s *const self, const int param, const void *const data, const int len)
{
  return VCS_ERROR_NOT_IMPL;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_set_config(struct vcs_ctx_s *const self, const int param, const void *const data, const int len)
{
  int result = VCS_OK;
  vcs_rx_data_t *rx = (vcs_rx_data_t*) self;
  
  switch(param)
  {
    case VCS_DRC:
    {
      vcs_drc_config_t *config = (vcs_drc_config_t*) data;
      vcs_algo_mode_t amode;
      vcs_to_algo_mode(&amode, &rx->mode, rx->input.port[0].nb_channels, 0); // 0->nb input channels and not interleaved

      result = vcs_drc_configure(rx->drc, &amode, config);

      rx->mylog->probe(rx->mylog, VCS_PROBE_RX_DRC_CONFIG, config,  sizeof(vcs_drc_config_t));  
    }
    break;

    case VCS_RXSPEECHENHANCEMENT:
    {
      vcs_serx_config_t *config = (vcs_serx_config_t*) data;
      vcs_algo_mode_t amode;
      vcs_to_algo_mode(&amode, &rx->mode, rx->input.port[0].nb_channels, 0); // 0->nb input channels and not interleaved

      result = vcs_serx_configure(rx->serx, &amode, config);
  
      rx->mylog->probe(rx->mylog, VCS_PROBE_RX_SE_CONFIG, config,  sizeof(vcs_serx_config_t));
    }
    break;
	
    case VCS_CTM:
    {
      vcs_ctm_config_t *config = (vcs_ctm_config_t*) data;
      vcs_algo_mode_t amode;
      vcs_to_algo_mode(&amode, &rx->mode,1 , 0); // input channels, not interleaved
	  
      result = vcs_ctmrx_configure(rx->ctmrx, &amode, config);

      rx->mylog->probe(rx->mylog, VCS_PROBE_RX_CTM_CONFIG, config, sizeof(vcs_drc_config_t));
    }
    break;
    case VCS_COMFORTNOISE:
    {
      vcs_comfortnoise_config_t *config = (vcs_comfortnoise_config_t*) data;
      vcs_algo_mode_t amode;
      vcs_to_algo_mode(&amode, &rx->mode,1 , 0); // input channels, not interleaved
      result = vcs_comfortnoise_config(rx->comfortnoise, &amode, config);

      rx->mylog->probe(rx->mylog, VCS_PROBE_RX_COMFORTNOISE_CONFIG, config, sizeof(vcs_comfortnoise_config_t));
    }
    break;
    case VCS_CUSTOMER_PLUGIN_FIRST:
    {
      vcs_algorith_mode_t pluginmode;
      vcs_algorithm_config_t *config = (vcs_algorithm_config_t*) data;
      vcs_to_algorithm_mode(&pluginmode, &rx->mode, rx->input.port[0].nb_channels, 0); 
      rx->pluginrxfirst->set_config(rx->pluginrxfirst, &pluginmode, config);
      rx->mylog->probe(rx->mylog, VCS_PROBE_RX_CUSTOMER_FIRST_CONFIG, config, sizeof(vcs_algorithm_config_t));
    }
    break;
    case VCS_CUSTOMER_PLUGIN_LAST:
    {
      vcs_algorith_mode_t pluginmode;
      vcs_algorithm_config_t *config = (vcs_algorithm_config_t*) data;
      vcs_to_algorithm_mode(&pluginmode, &rx->mode, rx->input.port[0].nb_channels, 0); 
      rx->pluginrxlast->set_config(rx->pluginrxlast, &pluginmode, config);
       rx->mylog->probe(rx->mylog, VCS_PROBE_RX_CUSTOMER_LAST_CONFIG, config, sizeof(vcs_algorithm_config_t));
    }
    break;

    default:
      result = VCS_ERROR_PARAM;
    break;
  }

  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_get_param(struct vcs_ctx_s *const self, const int param, void *const data, int *const len)
{
 
  return VCS_ERROR_NOT_IMPL;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_get_config(struct vcs_ctx_s *const self, int param, void *const data, int *const len)
{
  return VCS_ERROR_NOT_IMPL;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
inline static void vcs_swap(short** in, short** out, const int len)
{
  int i;

  for (i = 0; i < len; i++)
  {
    short *tmp = in[i];
    in[i] = out[i];
    out[i] = tmp;
  }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static void setup_inoutput(short** to, const int maxchannels, const int validchannels, short** from, const int nb_input)
{
  int i;

  for (i = 0; i < maxchannels; i++)
  {
    if (i < validchannels && i < nb_input)
    {
      to[i] = from[i];
    }
    else
      to[i] = 0; // NULL for no channel
  }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_process(struct vcs_ctx_s *const self, short** input, const int nb_input, short** output, const int nb_output)
{
  vcs_rx_data_t *rx = (vcs_rx_data_t*) self;
  short *indata[VCS_RX_MAX_IN_CHANNELS];
  short *outdata[VCS_RX_MAX_OUT_CHANNELS];
  
  /* since some algorithms does not support inplace, that is we can not use
	 same buffers as both input and output we have to swap between two
	 buffers. Here we assumes both input and output has same size  */

  if (self->mykind != VCS_DOWNLINK)
    return VCS_ERROR_OTHER;

  /* tx->input.port[0] is input speech decoder
	* and tx->output.port[0] is output to loudspeaker
	* both should be mono */
  setup_inoutput(indata,  VCS_RX_MAX_IN_CHANNELS,  rx->input.port[0].nb_channels,  input,  nb_input);
  setup_inoutput(outdata, VCS_RX_MAX_OUT_CHANNELS, rx->output.port[0].nb_channels, output, nb_output);

  rx->mylog->probe(rx->mylog, VCS_PROBE_RX_INPUT, indata[0], rx->mode.framesize * sizeof(short));
  /* execute first customer plugin */
  if (rx->pluginrxfirst->is_running(rx->pluginrxfirst)) {
    rx->pluginrxfirst->execute(rx->pluginrxfirst, indata, 1, outdata, 1);
    vcs_swap(indata, outdata, 1);
  }
  
  /*****************************/
  /* Execute CTM RX            */
  if (vcs_ctmrx_is_running(rx->ctmrx))
  {
    vcs_ctmrx_execute(rx->ctmrx, indata, 1, outdata, 1);
    vcs_swap(indata, outdata, 1);
  }
  rx->mylog->probe(rx->mylog, VCS_PROBE_RX_CTM, indata[0], rx->mode.framesize * sizeof(short));

  /*****************************/
  /* Execute SE RX             */
  if (vcs_serx_is_running(rx->serx))
  {
    vcs_serx_execute(rx->serx, indata, 1, outdata, 1);
    if (rx->mylog->probe_is_active(rx->mylog, VCS_PROBE_ADEQ_TX_CALIBRATION_DATA)) {
      if (rx->eq_tx_noisespectrum == 0) {
	    rx->eq_tx_noisespectrum = vcs_serx_probe_data_alloc(rx->serx, VCS_PROBE_ADEQ_TX_CALIBRATION_DATA);
	    if (rx->eq_tx_noisespectrum != 0) {
		   rx->mylog->log(rx->mylog, VCS_DEBUG, "EQ TX NOISESPECTRUM PROBE CREATED LEN %d", 
		               rx->eq_tx_noisespectrum->datalen);
		}
	  }
	  if (rx->eq_tx_noisespectrum != 0 && vcs_serx_probe_data_get(rx->serx, rx->eq_tx_noisespectrum)) {
	    rx->mylog->probe(rx->mylog, VCS_PROBE_ADEQ_TX_CALIBRATION_DATA, 
		                 rx->eq_tx_noisespectrum->data, rx->eq_tx_noisespectrum->datalen*sizeof(short));
	  }
    }
	if (rx->mylog->probe_is_active(rx->mylog, VCS_PROBE_ADEQ_RX_CALIBRATION_DATA)) {
      if (rx->eq_rx_speechspectrum == 0) {
	    rx->eq_rx_speechspectrum = vcs_serx_probe_data_alloc(rx->serx, VCS_PROBE_ADEQ_RX_CALIBRATION_DATA);
	    if (rx->eq_rx_speechspectrum != 0) {
		  rx->mylog->log(rx->mylog, VCS_DEBUG, "EQ RX SPEECHSPECTRUM PROBE CREATED LEN %d", 
		                                        rx->eq_rx_speechspectrum->datalen);
		}
	  }
	  if (rx->eq_rx_speechspectrum != 0 && vcs_serx_probe_data_get(rx->serx, rx->eq_rx_speechspectrum)) {
	    rx->mylog->probe(rx->mylog, VCS_PROBE_ADEQ_RX_CALIBRATION_DATA, 
		                 rx->eq_rx_speechspectrum->data, rx->eq_rx_speechspectrum->datalen*sizeof(short));
	  }
    }
    vcs_swap(indata, outdata, 1);
  }
  rx->mylog->probe(rx->mylog, VCS_PROBE_RX_SE, indata[0], rx->mode.framesize * sizeof(short));

  /*****************************/
  /* Execute DRC RX             */
  if (vcs_drc_is_running(rx->drc))
  {
    vcs_drc_execute(rx->drc, indata, 1, outdata, 1);
    vcs_swap(indata, outdata, 1);
  }
  rx->mylog->probe(rx->mylog, VCS_PROBE_RX_DRC, output[0],  rx->mode.framesize * sizeof(short));
  
  /* execute last customer plugin */
  if (rx->pluginrxlast->is_running(rx->pluginrxlast)) {
    rx->pluginrxlast->execute(rx->pluginrxlast, indata, 1, outdata, 1);
    vcs_swap(indata, outdata, 1);
  }  
  rx->mylog->probe(rx->mylog, VCS_PROBE_RX_CUSTOMER_EFFECT, output[0],  rx->mode.framesize * sizeof(short));

  /* Algorithm people say this should be last*/
  /* Noise generation can not be introduced. It might */
  /* affect the algorithm output */
  if (vcs_comfortnoise_is_running(rx->comfortnoise))
  {
    vcs_comfortnoise_execute(rx->comfortnoise, indata, 1, outdata, 1);
	vcs_swap(indata, outdata, 1);
  }

  /*
	Each time we swap indata indata will contain the output from last step. It will odd time
	point on output and even time on input. If pointing at output it is even and we have to
	do a final memory copy
   */
  if (indata[0] != output[0])
  {
    memcpy(output[0], input[0], sizeof(short) * rx->mode.framesize);
  }
#ifdef VCS_SINE_GENERATION
  vcs_sine_fill(&rx->sine, output[0], rx->mode.framesize);
#endif
  // Comfort Noise seems to be last probe so we add it absolutly at the end
  rx->mylog->probe(rx->mylog, VCS_PROBE_RX_COMFORTNOISE, output[0],  rx->mode.framesize * sizeof(short));
  
  return VCS_OK;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static vcs_processing_kind_t my_get_kind(const struct vcs_ctx_s *const self)
{
  return self->mykind;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_destroy(struct vcs_ctx_s *const self)
{
  vcs_rx_data_t *rx = (vcs_rx_data_t*) self;
  
  if (self->mykind != VCS_DOWNLINK)
    return VCS_ERROR_OTHER;
  vcs_serx_probe_data_free(rx->eq_tx_noisespectrum);
  vcs_serx_probe_data_free(rx->eq_rx_speechspectrum);
  vcs_queue_destroy(rx->queue, free);
  vcs_serx_destroy(rx->serx);
  vcs_drc_destroy(rx->drc);
  vcs_ctmrx_destroy(rx->ctmrx);
  vcs_comfortnoise_destroy(rx->comfortnoise);
  rx->pluginrxfirst->destroy(rx->pluginrxfirst);
  rx->pluginrxlast->destroy(rx->pluginrxlast);
  vcs_plugin_destroy(rx->plugin);
  free(self);

  return VCS_OK;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_export(struct vcs_ctx_s*self, vcs_export_t**data)
{
  vcs_rx_data_t *rx = (vcs_rx_data_t*) self;
   
  vcs_export_t* d = 0;

  if (vcs_queue_isempty(rx->queue))
  {
    d = (vcs_export_t*) malloc(sizeof(vcs_export_t));
  } 
  else
  {
    d = vcs_queue_dequeue(rx->queue);
  }

  if (d)
  {
    d->torx.se = 0;
    vcs_serx_export(rx->serx, &d->totx.se);
    vcs_ctmrx_export(rx->ctmrx, &d->totx.ctmrx);
  }
  *data = d;

  return VCS_OK;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_import(struct vcs_ctx_s*self, vcs_export_t**data)
{
  vcs_rx_data_t *rx = (vcs_rx_data_t*) self;

  if (*data)
  {
    vcs_export_t* d = *data;
    vcs_serx_import(rx->serx, &d->torx.se);

    if (!vcs_queue_add(rx->queue, d))
    {
      free(d);
    }
  }

  *data = 0;

  return VCS_OK;
}


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_ctx_t* vcs_rx_create(vcs_log_t* log)
{
  vcs_rx_data_t* rx = calloc(1, sizeof( vcs_rx_data_t));

  if (rx == NULL)
    return 0;

  rx->ctx.mykind     = VCS_DOWNLINK;
  rx->ctx.set_mode   = my_set_mode;
  rx->ctx.process    = my_process;
  rx->ctx.set_param  = my_set_param;
  rx->ctx.set_config = my_set_config;
  rx->ctx.get_param  = my_get_param;
  rx->ctx.get_config = my_get_config;
  rx->ctx.get_kind   = my_get_kind;
  rx->ctx.destroy    = my_destroy;
  rx->ctx.exportdata = my_export;
  rx->ctx.importdata = my_import;
  rx->mylog          = log;
  
  if (vcs_rx_init(rx) == 0)
  {
    log->log(log, VCS_ERROR, "Failed initialization");
    my_destroy ((vcs_ctx_t*)rx);
    rx = 0;
  }

  return (vcs_ctx_t*) rx;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
// local method 
static int vcs_rx_init(vcs_rx_data_t *rx)
{
  rx->drc   = vcs_drc_create(rx->mylog);
  rx->ctmrx = vcs_ctmrx_create(rx->mylog);
  rx->serx  = vcs_serx_create(rx->mylog);
  rx->comfortnoise = vcs_comfortnoise_create(rx->mylog);

  
  // plugin part
  rx->plugin = vcs_plugin_init(rx->mylog);
  rx->pluginrxfirst = vcs_plugin_get(rx->plugin, VCS_PLUGIN_RX_FIRST);
  rx->pluginrxlast = vcs_plugin_get(rx->plugin, VCS_PLUGIN_RX_LAST);
  
  // queue must be at least as big as is allocated in both rx and tx
  rx->queue = vcs_queue_alloc(5);

  // finally set default mode since algorithm configuration might come before mode is set
  vcs_portsdef_init(&rx->input, 0, 1, 8000, 0);

  rx->mode.resolution = 16;
  rx->mode.samplerate = 8000;
  rx->mode.framesize  = 80;
#ifdef VCS_SINE_GENERATION
  vcs_sine_init(&rx->sine);
#endif  

  return rx->queue != 0 && rx->drc != 0 && rx->ctmrx != 0 && rx->serx != 0 && rx->comfortnoise != 0 && 
         rx->plugin != 0 && rx->pluginrxfirst != 0 && rx->pluginrxlast != 0;

}





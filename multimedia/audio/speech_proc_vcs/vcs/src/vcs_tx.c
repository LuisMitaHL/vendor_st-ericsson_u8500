/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_tx.c
 * \brief    Implementation of voice shell uplink (TX)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include <string.h>

#include "vcs_tx.h"
#include "vcs_drc.h"
#include "vcs_setx.h"
#include "vcs_ctmtx.h"
#include "vcs_comfortnoise.h"
#include "vcs_queue.h"
#include "vcs_circbuff.h"
#include "vcs_plugin.h"
#ifdef VCS_SINE_GENERATION
#include "vcs_sine.h"
#endif

#define VCS_STEREO_CHANNEL 2
typedef struct
{
  vcs_ctx_t      ctx;
  vcs_log_t      *mylog;
  vcs_mode_t     mode;
  vcs_portsdef_t input;
  vcs_portsdef_t output;
  short          *outbuf;
  vcs_drc_t      *drc;
  vcs_setx_t     *setx;
  vcs_ctmtx_t    *ctmtx;
  vcs_comfortnoise_t *comfortnoise;
  vcs_plugin_t   *plugin;
  vcs_algorithm_t* plugintxlast;
  vcs_queue_t    *queue;
  short          *linout[VCS_STEREO_CHANNEL];
  struct vcs_circbuff_s *circbuff; 
  vcs_se_probedata_t *ecfiltertap;
#ifdef VCS_SINE_GENERATION
  vcs_sine_t     sine;
#endif
} vcs_tx_data_t;

#define VCS_TX_MAX_IN_PORTS     8
#define VCS_TX_MAX_IN_CHANNELS  3
#define VCS_TX_MAX_OUT_CHANNELS 1
#define VCS_TX_MAX_IN_ECREF     2

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int is_valid_input(vcs_tx_data_t *const tx, const vcs_mode_t *const mode, const vcs_portsdef_t *const ports)
{
  int result = 1;

  if (ports->nb_ports < 1 || ports->nb_ports > 2)
  {
    tx->mylog->log(tx->mylog, VCS_INFO, "VCS TX input invalid port number:%i", ports->nb_ports);
    return 0;
  }

  // mic input must have same sample rate as mode and
  // 1...VCS_TX_MAX_IN_CHANNELS channels
  if (ports->port[0].samplerate  != mode->samplerate ||
      ports->port[0].nb_channels <  1 ||
      ports->port[0].nb_channels >  VCS_TX_MAX_IN_CHANNELS) 
  {
    result = 0;
  }

  // interleaved is not supported
  else if (ports->port[0].nb_channels > 1 && ports->port[0].interleaved != 0) 
  {
    result = 0;
  }

  tx->mylog->log(tx->mylog, VCS_DEBUG, "VCX TX input samplerate:%i, nb_channels:%i interleaved:%i", 
		 ports->port[0].samplerate,
		 ports->port[0].nb_channels,
		 ports->port[0].interleaved);

  if (ports->nb_ports == 2) 
  {
    // echo canceller ref channel must have same sample rate as mode and
    // 1...2 channels (mono or stereo)
    if (ports->port[1].samplerate != mode->samplerate ||
	ports->port[1].nb_channels < 1 ||
	ports->port[1].nb_channels > VCS_TX_MAX_IN_ECREF) 
      result = 0;
	
    // interleaved is not supported
    else if (ports->port[1].nb_channels > 1 && ports->port[1].interleaved != 0)
    {
      result = 0;
    }

    tx->mylog->log(tx->mylog, VCS_DEBUG, "VCX TX EC REF samplerate:%i, nb_channels:%i interleaved:%i",
                   ports->port[1].samplerate,
	           ports->port[1].nb_channels,
	           ports->port[1].interleaved);
  }
  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/*********************************************************************************/
static int is_valid_output (vcs_tx_data_t* tx, const vcs_mode_t* mode, const vcs_portsdef_t* ports)
{
  int retval;
  
  if (ports->nb_ports != 1)
  {
    tx->mylog->log(tx->mylog, VCS_INFO, "VCS TX output invalid port number:%i", ports->nb_ports);
    return 0;
  }

  tx->mylog->log(tx->mylog, VCS_DEBUG, "VCX TX output samplerate:%i, nb_channels:%i interleaved:%i",
                 ports->port[0].samplerate,
		 ports->port[0].nb_channels,
		 ports->port[0].interleaved);

  // same sample rate and only one channel (mono) supported
  // since number of channels is 1 interleaved or not is not importent
  retval = ports->port[0].samplerate == mode->samplerate && ports->port[0].nb_channels == VCS_TX_MAX_OUT_CHANNELS;
  return retval;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static void my_tx_reset(vcs_tx_data_t *const tx)
{
  vcs_algo_mode_t amode;
  vcs_algorith_mode_t pluginmode;
  // drc and CTM only mono
  vcs_to_algo_mode(&amode, &tx->mode, 1, 0); 
  vcs_drc_reset(tx->drc, &amode);
  vcs_ctmtx_reset(tx->ctmtx, &amode);
  vcs_comfortnoise_reset(tx->comfortnoise, &amode);
  // mode based in input port
  vcs_to_algo_mode2(&amode, &tx->mode, tx->input.port[0].nb_channels, tx->input.port[1].nb_channels, 0); 
  // 0->nb input channels and not interleaved
 
  vcs_setx_reset(tx->setx, &amode);
  
  vcs_to_algorithm_mode(&pluginmode, &tx->mode, 1, 0); 
  tx->plugintxlast->reset(tx->plugintxlast, &pluginmode);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_set_mode(struct vcs_ctx_s     *const self, 
                       const vcs_mode_t     *const mode,
                       const vcs_portsdef_t *const input,
		       const vcs_portsdef_t *const output)
{
  vcs_tx_data_t *tx = (vcs_tx_data_t*) self;
  int result = VCS_ERROR_PARAM;

  tx->mylog->log(tx->mylog, VCS_DEBUG, "VCS TX resolution:%i samplerate:%i framesize:%i", mode->resolution, mode->samplerate, mode->framesize);

  if (self->mykind != VCS_UPLINK) return VCS_ERROR_OTHER;

  if ( (mode->resolution == 16) &&
      ((mode->samplerate == 8000) || (mode->samplerate == 16000)) &&
      is_valid_input(tx, mode, input) &&
      is_valid_output(tx, mode, output) )
  {
    int i;
    tx->mode = *mode;
    tx->input = *input;
    tx->output = *output;
    result = VCS_OK;
	/*
	 * If input port is stereo and output is mono we have to allocate a temporary that can be used
	 * for speech enhancement that assumes same number of channel for input as for output
	*/
    if (tx->input.port[0].nb_channels > 0 && tx->output.port[0].nb_channels < 2) 
    {
      if (tx->outbuf) free(tx->outbuf);
      tx->outbuf = calloc(tx->mode.framesize*2, sizeof(short));
    }
	for (i = 0; i < VCS_STEREO_CHANNEL; i++) {
	  if (tx->linout[i] != 0) {
	    free(tx->linout[i]);
	    tx->linout[i] = 0;
	  }
	}
	
    my_tx_reset(tx);

    if (tx->ecfiltertap != 0) {
        vcs_setx_probe_data_free(tx->ecfiltertap);
        tx->ecfiltertap = 0;
    }

  } 
  else 
    tx->mylog->log(tx->mylog, VCS_INFO, "VCS TX usnported mode and port def");

  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_set_param(struct vcs_ctx_s *const self, const int param, const void *const data, const int len)
{
  int result = VCS_OK;
  
  switch (param) 
  {
    case VCS_DRC:
    break;

    case VCS_TXSPEECHENHANCEMENT:
    break;
  
    case VCS_CTM:
    break;

    default:
      result = VCS_ERROR_ALG_NOT_SUPPORTED;
    break;
  }
  return result;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_set_config(struct vcs_ctx_s *const self, const int param, const void *const data, const int len)
{
  int result = VCS_OK;
  vcs_tx_data_t *tx = (vcs_tx_data_t*) self;

  switch(param) 
  {
    case VCS_DRC:
    {
      vcs_drc_config_t *config = (vcs_drc_config_t*) data;
      vcs_algo_mode_t amode;
      vcs_to_algo_mode(&amode, &tx->mode,1 , 0); // input channels, not interleaved
	  
      result = vcs_drc_configure(tx->drc, &amode, config);

      tx->mylog->probe(tx->mylog, VCS_PROBE_TX_DRC_CONFIG, config,  sizeof(vcs_drc_config_t));
    }
    break;

    case VCS_TXSPEECHENHANCEMENT:
    {
      vcs_setx_config_t *config = (vcs_setx_config_t*) data;
      vcs_algo_mode_t amode;
      vcs_to_algo_mode2(&amode, &tx->mode, tx->input.port[0].nb_channels, tx->input.port[1].nb_channels, 0); // 0->nb input channels and not interleaved
	  
      result = vcs_setx_configure(tx->setx, &amode, config);
	  
      tx->mylog->probe(tx->mylog, VCS_PROBE_TX_SE_CONFIG, config,  sizeof(vcs_setx_config_t));

      if (tx->ecfiltertap != 0) {
          vcs_setx_probe_data_free(tx->ecfiltertap);
          tx->ecfiltertap = 0;
      }

    }
    break;

    case VCS_CTM:
    {
      vcs_ctm_config_t *config = (vcs_ctm_config_t*) data;
      vcs_algo_mode_t amode;
      vcs_to_algo_mode(&amode, &tx->mode, 1, 0); // input channels, not interleaved
	  
      result = vcs_ctmtx_configure(tx->ctmtx, &amode, config);

      tx->mylog->probe(tx->mylog, VCS_PROBE_TX_CTM_CONFIG, config, sizeof(vcs_drc_config_t));
    }
    break;
    case VCS_COMFORTNOISE:
    {
      vcs_comfortnoise_config_t *config = (vcs_comfortnoise_config_t*) data;
      vcs_algo_mode_t amode;
      vcs_to_algo_mode(&amode, &tx->mode,1 , 0); // input channels, not interleaved
	  
      result = vcs_comfortnoise_config(tx->comfortnoise, &amode, config);

      tx->mylog->probe(tx->mylog, VCS_PROBE_TX_COMFORTNOISE_CONFIG, config, sizeof(vcs_comfortnoise_config_t));
    }
    break;
    case VCS_TREQ:
    {
    }
    break;
    case VCS_CUSTOMER_PLUGIN_LAST:
    {
      vcs_algorith_mode_t pluginmode;
      vcs_algorithm_config_t *config = (vcs_algorithm_config_t*) data;
      vcs_to_algorithm_mode(&pluginmode, &tx->mode, 1, 0); 
      tx->plugintxlast->set_config(tx->plugintxlast, &pluginmode, config);
      tx->mylog->probe(tx->mylog, VCS_PROBE_TX_CUSTOMER_LAST_CONFIG, config, sizeof(vcs_algorithm_config_t));
    }
    break;
    default:
      result = VCS_ERROR_ALG_NOT_SUPPORTED;
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
static int my_get_config(struct vcs_ctx_s *const self, const int param, void *const data, int *const len)
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
    short*tmp = in[i];
    in[i] = out[i];
    out[i] = tmp;
  }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static void vcs_stereo_to_mono(vcs_tx_data_t *tx, short**in, int nb_channels)
{
  if (nb_channels > 1) 
  {
    unsigned int i;
    
    for (i = 0; i < tx->mode.framesize; i++) 
    {
      // for the moment only merge left and right
      long tmp = in[0][i] + in[1][i];
      in[0][i] =(short) tmp / 2;
    }
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
    {
      to[i] = 0; // NULL for no channel
    }
  }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_process(struct vcs_ctx_s *const self, short** input, int nb_input, short** output, int nb_output)
{
  vcs_tx_data_t *tx = (vcs_tx_data_t*) self;
  short *indata[VCS_TX_MAX_IN_CHANNELS];
  short *ecref[VCS_TX_MAX_IN_ECREF];
  short *outdata[2]; // internally echo canceller might output stereo.
 
  if (self->mykind != VCS_UPLINK) 
    return VCS_ERROR_OTHER;

  /* first tx->input.port[0].nb_channels is input from microphone
   * remaining is echo canceller feedback.
   * Output is simpler, it only contains output channels but since
   * echo canceller might be in stereo output is 2 channels.
   */
  setup_inoutput(indata, VCS_TX_MAX_IN_CHANNELS, tx->input.port[0].nb_channels, input, nb_input);
  setup_inoutput(ecref, VCS_TX_MAX_IN_ECREF, tx->input.port[1].nb_channels, input + tx->input.port[0].nb_channels, nb_input - tx->input.port[0].nb_channels);
  setup_inoutput(outdata, 2, tx->output.port[0].nb_channels, output, nb_output);

#ifdef VCS_TX_BUFFERING
#define VCS_TX_BUFFERING_SIZE (1)
  if (tx->circbuff == 0) {
    tx->circbuff = vcs_circbuff_alloc(tx->mode.framesize, VCS_TX_BUFFERING_SIZE);
	if (tx->circbuff != 0) tx->mylog->log(tx->mylog, VCS_DEBUG, "VCS_TX_BUFFERING_SIZE %d", VCS_TX_BUFFERING_SIZE);
	else tx->mylog->log(tx->mylog, VCS_ERROR, "FAILURE ALLOCATED VCS_TX_BUFFERING_SIZE %d", VCS_TX_BUFFERING_SIZE);
  }
  if (tx->circbuff != 0) {
    vcs_circbuff_put(tx->circbuff, indata[0]);
    short* d = vcs_circbuff_getcurrent(tx->circbuff);
    memcpy(indata[0], d, tx->mode.framesize*sizeof(short));
  }
#endif // VCS_TX_BUFFERING
  if ( tx->output.port[0].nb_channels == 1)
  {
	outdata[1] = tx->outbuf;
  }
  tx->mylog->probe(tx->mylog, VCS_PROBE_TX_INPUT_LEFT, indata[0], tx->mode.framesize*sizeof(short));
  if (indata[1] != 0) {
    tx->mylog->probe(tx->mylog, VCS_PROBE_TX_INPUT_MIC_REF1, indata[1], tx->mode.framesize*sizeof(short));
  }

  if (ecref[0] != 0) 
  {
    tx->mylog->probe(tx->mylog, VCS_PROBE_TX_EC_FEEDBACK_LEFT, ecref[0], tx->mode.framesize*sizeof(short));
	if (tx->input.port[1].nb_channels == 2) tx->mylog->probe(tx->mylog, VCS_PROBE_TX_EC_FEEDBACK_RIGHT, ecref[1], tx->mode.framesize*sizeof(short));
  }
  
  /*****************************/
  /* Execute SE TX             */
  if (vcs_setx_is_running(tx->setx) && ecref[0] != 0)
  {
    if (tx->mylog->probe_is_active(tx->mylog, VCS_PROBE_TX_SE_LINEAR_LEFT)) {
	  if (tx->linout[0] == 0) {
	     tx->linout[0] = calloc(tx->mode.framesize, sizeof(short));
		 tx->linout[1] = calloc(tx->mode.framesize, sizeof(short));
		 tx->mylog->log(tx->mylog, VCS_DEBUG, "PROBE LINEAR LEFT CREATED LEN %d", 
		                tx->mode.framesize*sizeof(short));
	  }
	} else if (tx->linout[0] != 0) {
	  int i;
	  for (i = 0; i < VCS_STEREO_CHANNEL; i++) {
	     if (tx->linout[i] != 0) {
	       free(tx->linout[i]);
	       tx->linout[i] = 0;
	     }
      }
	}
    // to execute speech enhancement echo ref must also be valid
    vcs_setx_execute(tx->setx, indata, 2, ecref, VCS_TX_MAX_IN_ECREF, outdata, 2, tx->linout, 2);
    vcs_swap(indata, outdata, 2);
    if (tx->mylog->probe_is_active(tx->mylog, VCS_PROBE_TX_EC_FILTERTAP_LEFT)) {
      if (tx->ecfiltertap == 0) {
	    tx->ecfiltertap = vcs_setx_probe_data_alloc(tx->setx, VCS_PROBE_TX_EC_FILTERTAP_LEFT);
	    tx->mylog->log(tx->mylog, VCS_DEBUG, "FILTER TAP PROBE CREATED LEN %d", tx->ecfiltertap->datalen);
	  }
	  if (tx->ecfiltertap != 0 && vcs_setx_probe_data_get(tx->setx, tx->ecfiltertap)) {
	    tx->mylog->probe(tx->mylog, VCS_PROBE_TX_EC_FILTERTAP_LEFT, tx->ecfiltertap->data, tx->ecfiltertap->datalen*sizeof(short));
	  }
    } 
	if (tx->linout[0] != 0) {
	   tx->mylog->probe(tx->mylog, VCS_PROBE_TX_SE_LINEAR_LEFT, tx->linout[0], tx->mode.framesize*sizeof(short));
	}
  }
  
  tx->mylog->probe(tx->mylog, VCS_PROBE_TX_SE_NONLINEAR_LEFT, indata[0],  tx->mode.framesize*sizeof(short));
  
  // Note! Only MOMO, any additional mic channel is used for noise reduction so do not mix. How to handle stereo need investigation since
  // we can not use tx->input.port[0].nb_channels directly
  /*if (tx->input.port[0].nb_channels > 1 && nb_input > 1) 
  {
    vcs_stereo_to_mono(tx, indata, tx->input.port[0].nb_channels);
  } */
  tx->mylog->probe(tx->mylog, VCS_PROBE_TX_SE_OUT, indata[0], tx->mode.framesize*sizeof(short));
  
  /*****************************/
  /* Execute CTM TX            */
  if (vcs_ctmtx_is_running(tx->ctmtx) ) 
  {
    vcs_ctmtx_execute(tx->ctmtx, indata, 1, outdata, 1);
    vcs_swap(indata, outdata, 1);
  }
  tx->mylog->probe(tx->mylog, VCS_PROBE_TX_CTM,  indata[0],  tx->mode.framesize*sizeof(short));

  /*****************************/
  /* Execute DRC TX            */
  if (vcs_drc_is_running(tx->drc) )
  {
    vcs_drc_execute(tx->drc, indata, 1, outdata, 1);
    vcs_swap(indata, outdata, 1);
  }
  tx->mylog->probe(tx->mylog, VCS_PROBE_TX_DRC, output[0], tx->mode.framesize*sizeof(short));

  /* execute last customer plugin */
  if (tx->plugintxlast->is_running(tx->plugintxlast)) {
    tx->plugintxlast->execute(tx->plugintxlast, indata, 1, outdata, 1);
    vcs_swap(indata, outdata, 1);
  }
  tx->mylog->probe(tx->mylog, VCS_PROBE_TX_CUSTOMER_EFFECT, output[0], tx->mode.framesize*sizeof(short));

  /* Algorithm people say this should be last*/
  /* Noise generation can not be introduced. It might */
  /* affect the algorithm output */
  if (vcs_comfortnoise_is_running(tx->comfortnoise))
  {
    vcs_comfortnoise_execute(tx->comfortnoise, indata, 1, outdata, 1);
    vcs_swap(indata, outdata, 1);
  }

  /*
	Each time we swap indata indata will contain the output from last step.
	It will odd time point on output and even time on input.
	If pointing at output it is even and we have todo a final memory copy
   */
  if (indata[0] != output[0])
  {
    memcpy(output[0], input[0], sizeof(short) * tx->mode.framesize);
  }
#ifdef VCS_SINE_GENERATION
  vcs_sine_fill(&tx->sine, output[0], tx->mode.framesize);
#endif
  // comfort noise seem to be last probe so we add it absolutly at the end
  tx->mylog->probe(tx->mylog, VCS_PROBE_TX_COMFORTNOISE, output[0], tx->mode.framesize*sizeof(short));

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
  vcs_tx_data_t *tx = (vcs_tx_data_t*) self;
  int i;
  if (self->mykind != VCS_UPLINK)
    return VCS_ERROR_OTHER;
  if (tx->ecfiltertap != 0) {
    vcs_setx_probe_data_free(tx->ecfiltertap);
  }
  vcs_drc_destroy(tx->drc);
  vcs_setx_destroy(tx->setx);
  vcs_ctmtx_destroy(tx->ctmtx);
  tx->plugintxlast->destroy(tx->plugintxlast);
  vcs_plugin_destroy(tx->plugin);
  vcs_queue_destroy(tx->queue, free);
  vcs_circbuff_delete(tx->circbuff); 
  vcs_comfortnoise_destroy(tx->comfortnoise);
  // finally free all internal buffer that might have been allocated
  if (tx->outbuf)
    free(tx->outbuf);
  for (i = 0; i < VCS_STEREO_CHANNEL; i++) {
	if (tx->linout[i] != 0) {
	  free(tx->linout[i]);
	  tx->linout[i] = 0;
	}
  }

  free(self);
  return VCS_OK;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_export(struct vcs_ctx_s *const self, vcs_export_t**data)
{
  vcs_tx_data_t *tx = (vcs_tx_data_t*) self;
  vcs_export_t* d = 0;
  
  if (vcs_queue_isempty(tx->queue))
  {
    d = (vcs_export_t*) malloc(sizeof(vcs_export_t));
  } 
  else
  {
    d = vcs_queue_dequeue(tx->queue);
  }

  if (d)
  {
    d->totx.se = 0;
    vcs_setx_export(tx->setx, &d->torx.se);
  }
  
  *data = d;
  return VCS_OK;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int my_import(struct vcs_ctx_s *const self, vcs_export_t**data)
{
  vcs_tx_data_t *tx = (vcs_tx_data_t*) self;
  
  if (*data)
  {
    vcs_export_t* d = *data;
    vcs_setx_import(tx->setx, &d->totx.se);
    vcs_ctmtx_import(tx->ctmtx, &d->totx.ctmrx);
    
    if (!vcs_queue_add(tx->queue, d))
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
static int vcs_tx_init(vcs_tx_data_t *const tx)
{
  tx->drc = vcs_drc_create(tx->mylog);
  tx->setx = vcs_setx_create(tx->mylog);
  tx->ctmtx = vcs_ctmtx_create(tx->mylog);
  tx->comfortnoise = vcs_comfortnoise_create(tx->mylog);

  
  // plugin part
  tx->plugin = vcs_plugin_init(tx->mylog);
  tx->plugintxlast = vcs_plugin_get(tx->plugin, VCS_PLUGIN_TX_LAST);

  // queue must be at least as big as is allocated in both rx and tx
  tx->queue = vcs_queue_alloc(5);
  
  // finally set default mode since algorithm configuration might come before mode is set
  vcs_portsdef_init(&tx->input, 0, 1, 8000, 0);
  vcs_portsdef_init(&tx->input, 1, 1, 8000, 0);

  tx->mode.resolution = 16;
  tx->mode.samplerate = 8000;
  tx->mode.framesize  = 80;
  tx->circbuff = 0; // vcs_circbuff_alloc(tx->mode.framesize, 4);
#ifdef VCS_SINE_GENERATION
  vcs_sine_init(&tx->sine);
#endif   

  return tx->drc != 0 && tx->setx != 0 && tx->ctmtx != 0 && tx->queue != 0 &&  tx->comfortnoise != 0 &&
         tx->plugin != 0 && tx->plugintxlast != 0;

}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_ctx_t* vcs_tx_create(vcs_log_t* log)
{
  vcs_tx_data_t* tx = calloc(1, sizeof( vcs_tx_data_t));

  if (tx == NULL) 
    return 0;

  tx->ctx.mykind     = VCS_UPLINK;
  tx->ctx.set_mode   = my_set_mode;
  tx->ctx.process    = my_process;
  tx->ctx.set_param  = my_set_param;
  tx->ctx.set_config = my_set_config;
  tx->ctx.get_param  = my_get_param;
  tx->ctx.get_config = my_get_config;
  tx->ctx.get_kind   = my_get_kind;
  tx->ctx.destroy    = my_destroy;
  tx->ctx.exportdata = my_export;
  tx->ctx.importdata = my_import;
  tx->mylog          = log;

  if (vcs_tx_init(tx) == 0)
  {
    log->log(log, VCS_ERROR, "Failed initialization");
    my_destroy((vcs_ctx_t*) tx);
    tx = 0;
  }

  return (vcs_ctx_t*) tx;
}


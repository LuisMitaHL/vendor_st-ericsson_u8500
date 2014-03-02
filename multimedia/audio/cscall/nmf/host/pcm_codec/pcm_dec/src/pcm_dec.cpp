/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file   pcm_dec.cpp
 * \brief  PCM Decoder used by the decoder wrapper.
 * \       This decoder is mainly doing a copy from 'stream' structure to the
 * \       'sample' structure.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/pcm_codec/pcm_dec.nmf>
#include <cscall/nmf/host/pcm_codec/common/include/pcm_codec.hpp>
#include <armnmf_dbc.h>
#include <string.h>

#if 0
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_nmf_host_pcm_codec_pcm_dec_src_pcm_decTraces.h"
#endif
#endif

// MMProbe framwork
#ifdef MMPROBE_ENABLED
#include "t_mm_probe.h"
#include "r_mm_probe.h"
#endif


/*
 * constructor
 *
*/
 cscall_nmf_host_pcm_codec_pcm_dec::cscall_nmf_host_pcm_codec_pcm_dec (void)
 {
	 pcmdec_sampling_rate = PCM_SamplingRate_Unknown;
	 big_endian_flag = true;
 }

t_uint16
METH(checkSynchro)(t_uint24 msp, t_uint24 lsp, t_sint32 *framesize_p) {
    return 0;
}

t_uint16
METH(decodeFrame)(void *interface) {

	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	unsigned char               *indata              = NULL;
	unsigned char               *outdata             = NULL;
	RETURN_STATUS_LEVEL_T 		status				 = RETURN_STATUS_OK;

    SAMPLE_STRUCT_T             *sample_struct_p     = &(interface_fe->sample_struct);
    STREAM_STRUCT_T             *stream_struct_p     = &(interface_fe->stream_struct);

    indata = (unsigned char*) stream_struct_p->bits_struct.buf_add;
    outdata = (unsigned char*) sample_struct_p->buf_add;

    if (pcmdec_sampling_rate == PCM_SamplingRate_8k)
    {
    	// Note : For the MAI protocol, the data are stored in 'big endian' format.
    	if (big_endian_flag == true)
    		copyAndSwapEndianess(outdata,indata,PCM8K_BITSTREAM_SIZE_IN_BYTES);
    	else
    		memcpy(outdata, indata, PCM8K_BITSTREAM_SIZE_IN_BYTES);

    	sample_struct_p->chans_nb = 1;/* nb of possible channel */
     	sample_struct_p->sample_freq = ESAA_FREQ_8KHZ;
    	sample_struct_p->headroom = 0;
       	sample_struct_p->block_len = PCM8K_BITSTREAM_LENGTH;
    }
    else if (pcmdec_sampling_rate == PCM_SamplingRate_16k)
    {
    	// Note : For the MAI protocol, the data are stored in 'big endian' format.
    	if (big_endian_flag == true)
    		copyAndSwapEndianess(outdata,indata,PCM16K_BITSTREAM_SIZE_IN_BYTES);
    	else
    		memcpy(outdata, indata, PCM16K_BITSTREAM_SIZE_IN_BYTES);

    	sample_struct_p->chans_nb = 1;/* nb of possible channel */
    	sample_struct_p->sample_freq = ESAA_FREQ_16KHZ;
    	sample_struct_p->headroom = 0;
       	sample_struct_p->block_len = PCM16K_BITSTREAM_LENGTH;
    }
    else // The sampling rate is 'unknown'...
    {
       // OstTraceFiltInst0 (TRACE_WARNING, "pcm decoder:: WARNING!! Unknown sampling rate!");
    	status = RETURN_STATUS_ERROR;
    }

    interface_fe->codec_state.output_enable = 1;
  	interface_fe->codec_state.remaining_blocks = 0;
	interface_fe->codec_state.mode = PCM;

  	return status;
}

void
METH(setConfig)(PcmCodecConfig_t config) {

	pcmdec_sampling_rate = config.sampling_rate;
	big_endian_flag = config.big_endian_flag;
}

void
METH(open)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	PCM_CODEC_STRUCT_T *pcmcodec_dec_inst_p = NULL;
	SAMPLE_STRUCT_T             *sample_struct_p     = &(interface_fe->sample_struct);

	if (interface == NULL)
	{
    //    OstTraceFiltInst0 (TRACE_WARNING, "pcm decoder:: WARNING!! Opening with NULL interface!");
	}
	else
	{
		interface_fe->codec_config_local_struct = NULL;
		interface_fe->codec_config_local_struct = &pcmdec_sampling_rate;
		interface_fe->codec_info_local_struct = NULL;
		interface_fe->codec_local_struct = malloc(sizeof(PCM_CODEC_STRUCT_T));
		if (interface_fe->codec_local_struct == NULL)
		{
		//	OstTraceFiltInst0 (TRACE_WARNING, "pcm decoder:: WARNING!! Malloc failed during the opening!");
		}
		else
		{
			pcmcodec_dec_inst_p = (PCM_CODEC_STRUCT_T *) interface_fe->codec_local_struct;
			pcmcodec_dec_inst_p->in_buf = NULL;
			pcmcodec_dec_inst_p->out_buf = NULL;
		}

		/* Init of static variables
		 * By default, we configure the system in 16K.
		 */
		sample_struct_p     = &(interface_fe->sample_struct);

		sample_struct_p->sample_freq = ESAA_FREQ_16KHZ;
		sample_struct_p->sample_size = 16;
		sample_struct_p->headroom = 0;
		sample_struct_p->chans_nb = 1;
		sample_struct_p->block_len = PCM16K_BITSTREAM_LENGTH;

	}
}

t_uint32
METH(close)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;

	if (interface_fe != NULL)
	{
		if (interface_fe->codec_local_struct != NULL)
		{
			free(interface_fe->codec_local_struct);
			interface_fe->codec_local_struct = NULL;
		}
	}
	return 0;
}

void
METH(reset)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	PCMCODEC_Dec_Inst_t          *pcmcodec_dec_inst_p = NULL;

	if (interface_fe != NULL)
	{
		pcmcodec_dec_inst_p = (PCMCODEC_Dec_Inst_t *) interface_fe->codec_local_struct;
		if (pcmcodec_dec_inst_p != NULL)
		{
			pcmcodec_dec_inst_p->in_buf = NULL;
			pcmcodec_dec_inst_p->out_buf = NULL;
		}
	}
}

void
METH(setOutputBuf)(void *interface, void *buf) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;

	PCM_CODEC_STRUCT_T *pcmcodec_dec_inst_p = (PCM_CODEC_STRUCT_T *) interface_fe->codec_local_struct;
    pcmcodec_dec_inst_p->out_buf = (Word16*)buf;
}

t_sint32
METH(getMaxFrameSize)(void) {

	return PCM_MAX_BITSTREAM_SIZE_IN_BITS;
}

void
METH(getOutputGrain)(t_uint16 *blocksize, t_uint16 *maxchans) {
	if (pcmdec_sampling_rate == PCM_SamplingRate_8k)
		*blocksize  = PCM8K_BITSTREAM_LENGTH;
	else
		*blocksize  = PCM16K_BITSTREAM_LENGTH;

	*maxchans   = 1;
}

t_uint16
METH(getHeaderType)(void) {

	return NO_HEADER_VARYING_SIZE;
}

t_uint16
METH(getSampleBitSize)(void) {

	// Behaves like all the other codecs...
	return PCM_BITS_PER_SAMPLE;

}

t_sint32
METH(getBlockSize)(void) {

    // Behaves like all the other codecs......
	return 0;
}


void cscall_nmf_host_pcm_codec_pcm_dec:: copyAndSwapEndianess(unsigned char * buffer_out,
															  const unsigned char * buffer_in,
															  t_uint16 size)
{
	t_uint16 i;

	for(i = 0 ; i < size ;i+=2)
	{
		buffer_out[i]   = buffer_in[i+1];
		buffer_out[i+1] = buffer_in[i];
	}
}


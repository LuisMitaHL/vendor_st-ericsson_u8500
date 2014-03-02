/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file   pcm_enc.cpp
 * \brief  PCM Encoder used by the encoder structure.This encoder is mainly
 * \       doing a copy from the 'sample' structure to the 'stream' structure.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/pcm_codec/pcm_enc.nmf>
#include <cscall/nmf/host/pcm_codec/common/include/pcm_codec.hpp>
#include <armnmf_dbc.h>
#include <string.h>

#if 0
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_nmf_host_pcm_codec_pcm_enc_src_pcm_encTraces.h"
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
 cscall_nmf_host_pcm_codec_pcm_enc::cscall_nmf_host_pcm_codec_pcm_enc (void)
 {
	 pcmdec_sampling_rate = PCM_SamplingRate_Unknown;
	 big_endian_flag = true;
 }

t_uint16
METH(encodeFrame)(void *interface) {

	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	unsigned char               *indata              = NULL;
	unsigned char               *outdata             = NULL;
	RETURN_STATUS_LEVEL_T 		status				 = RETURN_STATUS_OK;

    indata = (unsigned char*) interface_fe->sample_struct.buf_add;
    outdata = (unsigned char*) interface_fe->stream_struct.bits_struct.buf_add;

    if (pcmdec_sampling_rate == PCM_SamplingRate_8k)
    {
    	// Note : For the MAI protocol, the data are stored in 'big endian' format.
    	if (big_endian_flag == true)
       		copyAndSwapEndianess(outdata,indata,PCM8K_BITSTREAM_SIZE_IN_BYTES);
        else
        	memcpy(outdata, indata, PCM8K_BITSTREAM_SIZE_IN_BYTES);

        interface_fe->stream_struct.real_size_frame_in_bit = (PCM8K_BITSTREAM_SIZE_IN_BYTES << 3);
    }
    else if (pcmdec_sampling_rate == PCM_SamplingRate_16k)
    {
    	// Note : For the MAI protocol, the data are stored in 'big endian' format.
    	if (big_endian_flag == true)
       		copyAndSwapEndianess(outdata,indata,PCM16K_BITSTREAM_SIZE_IN_BYTES);
        else
        	memcpy(outdata, indata, PCM16K_BITSTREAM_SIZE_IN_BYTES);

        interface_fe->stream_struct.real_size_frame_in_bit = (PCM16K_BITSTREAM_SIZE_IN_BYTES << 3);
    }
    else // The sampling rate is 'unknown'...
    {
    //	OstTraceFiltInst0 (TRACE_WARNING, "pcm encoder:: WARNING!! Unknown sampling rate!");
    	status = RETURN_STATUS_ERROR;
    }

    interface_fe->codec_state.output_enable = 1;
    interface_fe->codec_state.remaining_blocks = 0;

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

	if (interface == NULL)
	{
	  // 	OstTraceFiltInst0 (TRACE_WARNING, "pcm encoder:: WARNING!! Opening is failling..!");
	}
	else
	{
		interface_fe->codec_config_local_struct = NULL;
		interface_fe->codec_config_local_struct = &pcmdec_sampling_rate;
		interface_fe->codec_info_local_struct = NULL;
		interface_fe->codec_local_struct = malloc(sizeof(PCM_CODEC_STRUCT_T));
		if (interface_fe->codec_local_struct == NULL)
		{
		//   	OstTraceFiltInst0 (TRACE_WARNING, "pcm encoder:: WARNING!! Malloc is failling at opening..!");
		}
		else
		{
			pcmcodec_dec_inst_p = (PCM_CODEC_STRUCT_T *) interface_fe->codec_local_struct;
			pcmcodec_dec_inst_p->in_buf = NULL;
			pcmcodec_dec_inst_p->out_buf = NULL;
		}
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

t_uint16
METH(getMaxFrameSize)(void) {
	return PCM_MAX_BITSTREAM_SIZE_IN_BITS;
}

t_uint16
METH(getMaxSamples)(void) {
	return PCM_SAMPLE_FRAME_SIZE_IN_WORDS;
}

t_uint16
METH(getSampleBitSize)(void) {
	return PCM_BITS_PER_SAMPLE;

}

void cscall_nmf_host_pcm_codec_pcm_enc:: copyAndSwapEndianess(unsigned char * buffer_out,
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


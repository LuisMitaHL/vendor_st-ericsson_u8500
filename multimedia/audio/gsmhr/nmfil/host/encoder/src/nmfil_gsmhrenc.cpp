/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <gsmhr/nmfil/host/encoder.nmf>

#include <armnmf_dbc.h>   

t_uint16
METH(encodeFrame)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	return gsmhr_encode_frame(interface_fe);
}

void 
METH(setParameter)(GsmhrEncParams_t params) {
	gsmhrenc_config_struct.memory_preset  = 0;
	}
void
METH(setConfig)(GsmhrEncConfig_t config){
	   gsmhrenc_config_struct.DTXon             = config.bDtx;
	   gsmhrenc_config_struct.payload_format		= (unsigned short)config.epayload_format;
}

void
METH(open)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	int status;
	
	interface_fe->codec_config_local_struct = &gsmhrenc_config_struct;

	status = gsmhr_encode_open(interface_fe);
	if (status!= INIT_OK) {
	  NMF_LOG("ERROR: GSMHR INIT FAILED");
	}

}

void
METH(reset)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	gsmhr_encode_reset(interface_fe);
}

t_uint16
METH(getMaxFrameSize)(void) {
	if (gsmhrenc_config_struct.payload_format == 1)//MB modem
	  return GSMHR_MAX_BITSTREAM_SIZE;           // 352 bits
	if (gsmhrenc_config_struct.payload_format == 2) //FB modem
	  return 120; 
	else 
	  return GSMHR_MAX_BITSTREAM_SIZE;           // 352 bits
}

t_uint16
METH(getMaxSamples)(void) {
	return GSMHR_SAMPLE_FRAME_SIZE;           // 160 samples
}

t_uint16
METH(getSampleBitSize)(void) {
	return GSMHR_DEC_SAMPLE_SIZE;             // 16 bits
}

t_uint32 
METH(close)(void *interface){ 
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
  gsmhr_encode_close(interface_fe);
  return 0;	
}

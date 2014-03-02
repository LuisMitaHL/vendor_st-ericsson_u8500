/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <gsmfr/nmfil/host/encoder.nmf>
#include <armnmf_dbc.h>

t_uint16 
METH(encodeFrame)(void *interface) {
	
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *)interface;
	
	if (gsmfrenc_config_struct.mNewConfigAvailable == 1){
	   gsmfr_dtx_reinit(interface_fe);
     gsmfrenc_config_struct.mNewConfigAvailable = 0;
	}
	return gsmfr_process_encode(interface_fe);
}

void METH(setParameter)(NmfGsmfrEncParams_t Params) {
		gsmfrenc_config_struct.memory_preset   = 0;
}

void
METH(setConfig)(NmfGsmfrEncConfig_t config){
	
	if (gsmfrenc_config_struct.has_dtx != config.bDtx){
	    gsmfrenc_config_struct.has_dtx = config.bDtx;
	    gsmfrenc_config_struct.mNewConfigAvailable = 1; //set mNewConfigAvailable as true
	}        
	  
	gsmfrenc_config_struct.payload_format		= (unsigned short)config.epayload_format;
	return;
}

void
METH(open)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	interface_fe->codec_config_local_struct = &gsmfrenc_config_struct;

  if (gsmfr_encode_open(interface_fe)!= INIT_OK) {
	  NMF_LOG("ERROR: GSMFR INIT FAILED");
	}
	
}

void 
METH(reset)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	gsmfr_encode_reset(interface_fe);
}

t_uint16 
METH(getMaxFrameSize)(void) {
	if(gsmfrenc_config_struct.payload_format == 1)
		return GSMFR_MAX_BITSTREAM_SIZE_PAYLOAD_FMT; // 304 bits
	else if(gsmfrenc_config_struct.payload_format == 2)
	  return 272;	//FB modem frame 
	else
	  return GSMFR_MAX_BITSTREAM_SIZE_PAYLOAD_FMT; 
}

t_uint16 
METH(getMaxSamples)(void) {
	return GSMFR_SAMPLE_FRAME_SIZE; // 160 samples (words)
}

t_uint16
METH(getSampleBitSize)(void) {
	return GSMFR_DEC_SAMPLE_SIZE; // 16 bits
}

t_uint32 
METH(close)(void *interface){ 
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
  gsmfr_encode_close(interface_fe);
	return 0;
}

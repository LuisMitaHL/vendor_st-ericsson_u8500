/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <gsmfr/nmfil/host/decoder.nmf>


t_uint16
METH(checkSynchro)(t_uint24 msp, t_uint24 lsp, t_sint32 *framesize_p) {
  return gsmfr_syncword(msp, lsp, (int*)framesize_p);
}

t_uint16
METH(decodeFrame)(void *interface) {
  return gsmfr_decode_frame((CODEC_INTERFACE_T *)interface);
}

void
METH(setParameter)(NmfGsmfrDecParams_t Params) {
		gsmfrdec_config_struct.memory_preset  = 0;
}

void
METH(setConfig)(NmfGsmfrDecConfig_t config) {
	gsmfrdec_config_struct.has_dtx        = (unsigned short)config.bDtx;
	gsmfrdec_config_struct.payload_format = (unsigned short)config.epayload_format;	
}

void
METH(open)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	interface_fe->codec_config_local_struct = &gsmfrdec_config_struct;

	if (gsmfr_decode_open(interface_fe)!= INIT_OK) {
	  NMF_LOG("ERROR: GSMFR INIT FAILED");
	}
	
}

void 
METH(reset)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	gsmfr_decode_reset(interface_fe);
}

void
METH(setOutputBuf)(void *interface, void *buf) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	
    gsmfr_decode_setBuffer((CODEC_INTERFACE_T *)interface_fe, buf);
}

t_sint32
METH(getMaxFrameSize)(void) {
	return GSMFR_MAX_BITSTREAM_SIZE_PAYLOAD_FMT; // 304 bits
}

void
METH(getOutputGrain)(t_uint16 *blocksize, t_uint16 *maxchans) {
	*blocksize  = GSMFR_SAMPLE_FRAME_SIZE; //160 samples
	*maxchans   = 1;
}

t_uint16
METH(getHeaderType)(void) {
	return NO_HEADER_VARYING_SIZE;
}

t_uint16
METH(getSampleBitSize)(void) {
	return GSMFR_DEC_SAMPLE_SIZE; // 16 bits
}

t_sint32
METH(getBlockSize)(void) {
	return 0;
}

t_uint32
METH(close) (void *interface)
{
    CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
    gsmfr_decode_close(interface_fe);
    return 0;
    
}

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <amrwb/nmfil/host/decoder.nmf>

t_uint16
METH(checkSynchro)(t_uint24 msp, t_uint24 lsp, t_sint32 *framesize_p) {
  return amrwb_syncword(msp, lsp, (int*)framesize_p);
}

t_uint16
METH(decodeFrame)(void *interface) {
	t_uint16 status = amrwb_decode_frame((CODEC_INTERFACE_T *)interface);
	return (status>1)?status:0;
}

void
METH(setParameter)(AmrwbDecParams_t Params) {
	amrwbdec_config_struct.noHeader       = (unsigned short)Params.bNoHeader;
	amrwbdec_config_struct.concealment_on = (unsigned short)Params.bErrorConcealment;
	amrwbdec_config_struct.memory_preset  = 0; 
    }

void
METH(setConfig)(AmrwbDecConfig_t config) {
	amrwbdec_config_struct.Payload_Format = (unsigned short)config.ePayloadFormat;
}
void
METH(open)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;

	interface_fe->codec_config_local_struct = &amrwbdec_config_struct;
	interface_fe->codec_info_local_struct   = &amrwbdec_info_struct;

	if (amrwb_init_decode_malloc(interface_fe)!= INIT_OK) {
	  NMF_LOG("ERROR: AMRWB INIT FAILED");
	}
}

void 
METH(reset)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	amrwb_decode_reset(interface_fe);
}

void
METH(setOutputBuf)(void *interface, void *buf) {

    amrwb_decode_setBuffer((CODEC_INTERFACE_T *)interface, buf);
}

t_sint32
METH(getMaxFrameSize)(void) {
	return AMRWB_MAX_BITSTREAM_SIZE_IN_BIT;           // 560 bits
}

void
METH(getOutputGrain)(t_uint16 *blocksize, t_uint16 *maxchans) {
	*blocksize  = AMRWB_SAMPLE_FRAME_SIZE_IN_WORD;    // 320 samples
	*maxchans   = 1;
}

t_uint16
METH(getHeaderType)(void) {
	return NO_HEADER_VARYING_SIZE;
}

t_uint16
METH(getSampleBitSize)(void) {
	return AMRWB_BITS_PER_SAMPLE;                     // 16 bits
}

t_sint32
METH(getBlockSize)(void) {  
	return 0;
}

t_uint32 
METH(close)(void *interface)
{
   CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
   amrwb_close_decode_malloc(interface_fe);
    return 0;
}

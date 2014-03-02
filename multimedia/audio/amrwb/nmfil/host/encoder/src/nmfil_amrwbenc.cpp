/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <amrwb/nmfil/host/encoder.nmf>
#include <armnmf_dbc.h>

/////////////////////////////////////////////////////////////////////

t_uint16
METH(encodeFrame)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	int nbBytes;
	return amrwb_encode(interface_fe,
					  &interface_fe->stream_struct.bits_struct, &nbBytes);
}

void 
METH(setParameter)(AmrwbEncParam_t params) {
	amrwbenc_config_struct.noHeader       = params.bNoHeader;
	amrwbenc_config_struct.memory_preset  = 0; 
            }

void 
METH(setConfig)(AmrwbEncConfig_t config) {
	amrwbenc_config_struct.mode   = config.nBitRate;
	amrwbenc_config_struct.dtx    = config.bDtxEnable;

	amrwbenc_config_struct.Payload_Format = config.ePayloadFormat;
}

void
METH(open)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	int status;
	
	interface_fe->codec_config_local_struct = &amrwbenc_config_struct;
	interface_fe->codec_info_local_struct   = &amrwbenc_info_struct;

	status = amrwb_encode_open_malloc(interface_fe);
	ARMNMF_DBC_POSTCONDITION(status == INIT_OK);
}

void
METH(reset)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	amrwb_encode_reset(interface_fe);
}

t_uint16
METH(getMaxFrameSize)(void) {
	return AMRWB_MAX_BITSTREAM_SIZE_IN_BIT;           // 560 bits
}

t_uint16
METH(getMaxSamples)(void) {
	return AMRWB_SAMPLE_FRAME_SIZE_IN_WORD;           // 320 samples
}

t_uint16
METH(getSampleBitSize)(void) {
	return AMRWB_BITS_PER_SAMPLE;                     // 16 bits
}

t_uint32 
METH(close)(void *interface)
{
   CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
   amrwb_encode_close(interface_fe);
    return 0;
}

/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   nmfil_amrenc.cpp
* \brief  AMR Decoder NMF_ARM Component
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <amr/nmfil/host/encoder.nmf>

#include <armnmf_dbc.h>

/////////////////////////////////////////////////////////////////////

amr_nmfil_host_encoder::amr_nmfil_host_encoder() {
    unsigned int i;

    char* tmp = (char *)&amrenc_config_struct;
    for (i=0;i<sizeof(amrenc_config_struct);tmp[i++]=0);

    tmp=(char *)&amrenc_info_struct;
    for (i=0;i<sizeof(amrenc_info_struct);tmp[i++]=0);
}

t_uint16
METH(encodeFrame)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	int nbBytes;
	return amr_encode(interface_fe,
					  &interface_fe->stream_struct.bits_struct, &nbBytes);
}

void 
METH(setParameter)(AmrEncParam_t params) {
	amrenc_config_struct.noHeader       = params.bNoHeader;
	amrenc_config_struct.memory_preset  = params.memory_preset;
            }

void 
METH(setConfig)(AmrEncConfig_t config) {
	amrenc_config_struct.mode           = config.nBitRate;
	amrenc_config_struct.dtx            = config.bDtxEnable;
	amrenc_config_struct.Payload_Format = config.ePayloadFormat;
    amrenc_config_struct.Efr_on	        = config.bEfr_on;
}

void
METH(open)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	int status;
	
	interface_fe->codec_config_local_struct = &amrenc_config_struct;
	interface_fe->codec_info_local_struct   = &amrenc_info_struct;

	status = amr_encode_open_malloc(interface_fe);
	ARMNMF_DBC_POSTCONDITION(status == INIT_OK);
}

t_uint32 
METH(close)(void *interface) {
    amr_encode_close(static_cast<CODEC_INTERFACE_T *>(interface));
    return 0;
}

void
METH(reset)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	amr_encode_reset(interface_fe);
}

t_uint16
METH(getMaxFrameSize)(void) {
	return AMR_MAX_BITSTREAM_SIZE_IN_BIT;           // 304 bits
}

t_uint16
METH(getMaxSamples)(void) {
	return AMR_SAMPLE_FRAME_SIZE_IN_WORD;           // 160 samples
}

t_uint16
METH(getSampleBitSize)(void) {
	return AMR_BITS_PER_SAMPLE;                     // 16 bits
}



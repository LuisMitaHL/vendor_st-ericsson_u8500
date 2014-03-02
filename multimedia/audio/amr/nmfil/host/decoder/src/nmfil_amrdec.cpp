/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   nmfil_amrdec.cpp
* \brief  AMR Decoder NMF Component
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <amr/nmfil/host/decoder.nmf>

amr_nmfil_host_decoder::amr_nmfil_host_decoder() {
    unsigned int i;
    for(i=0;i<sizeof(amrdec_info_struct);i++) {
        ((char *) &amrdec_info_struct)[i]=0;
    }
    for(i=0;i<sizeof(amrdec_config_struct);i++) {
        ((char *) &amrdec_config_struct)[i]=0;
    }
}

t_uint16
METH(checkSynchro)(t_uint24 msp, t_uint24 lsp, t_sint32 *framesize_p) {
  return amr_syncword(msp, lsp, (int*)framesize_p);
}

t_uint16
METH(decodeFrame)(void *interface) {
	return amr_decode_frame((CODEC_INTERFACE_T *)interface);
}

void
METH(setParameter)(AmrDecParams_t Params) {
	amrdec_config_struct.noHeader       = (unsigned short)Params.bNoHeader;
	amrdec_config_struct.concealment_on = (unsigned short)Params.bErrorConcealment;
	amrdec_config_struct.memory_preset  = (unsigned short)Params.memory_preset;
}

void
METH(setConfig)(AmrDecConfig_t config) {
	amrdec_config_struct.Payload_Format = (unsigned short)config.ePayloadFormat;
    amrdec_config_struct.Efr_on         = (unsigned short)config.bEfr_on;
}

void
METH(open)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;

	interface_fe->codec_config_local_struct = &amrdec_config_struct;
	interface_fe->codec_info_local_struct   = &amrdec_info_struct;

	if (amr_init_decode_malloc(interface_fe)!= INIT_OK) {
	  NMF_LOG("ERROR: AMR INIT FAILED");
	}
}

t_uint32
METH(close)(void *interface) {
    amr_close_decode_malloc(static_cast<CODEC_INTERFACE_T *>(interface));
    return 0;
}

void 
METH(reset)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	amr_decode_reset(interface_fe);
}

void
METH(setOutputBuf)(void *interface, void *buf) {
      amr_decode_setBuffer((CODEC_INTERFACE_T *)interface, buf);
}

t_sint32
METH(getMaxFrameSize)(void) {
	return AMR_MAX_BITSTREAM_SIZE_IN_BIT;           // 304 bits
}

void
METH(getOutputGrain)(t_uint16 *blocksize, t_uint16 *maxchans) {
	*blocksize  = AMR_SAMPLE_FRAME_SIZE_IN_WORD;    // 160 samples
	*maxchans   = 1;
}

t_uint16
METH(getHeaderType)(void) {
	return NO_HEADER_VARYING_SIZE;
}

t_uint16
METH(getSampleBitSize)(void) {
	return AMR_BITS_PER_SAMPLE;                     // 16 bits
}

t_sint32
METH(getBlockSize)(void) {
	return 0;
}

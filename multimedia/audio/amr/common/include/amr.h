/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/
#ifndef _amr_h_
#define _amr_h_

#include "driver_readbuffer.h"
#include "bitstream_lib_proto.h"
#include "audiolibs_common.h"
#include "common_interface.h"

#define AMR_MAX_BITSTREAM_SIZE_IN_BIT     (304)   // 48 bits magic number + 8bit header + 248 bits
#define AMR_SAMPLE_FRAME_SIZE_IN_WORD     160
#define AMR_BITS_PER_SAMPLE               16

enum {
	AMR_ERROR_CONCEALMENT_OFF = 0,
    AMR_ERROR_CONCEALMENT_RX_NO_DATA
};


/*************************** DECODER PART *******************************************/
/* decoder */
//extern const int amr_decoder_heap_size;
//extern const int amr_decoder_struct_size;

typedef struct {
	// Config									//  DEFAULT CONF
    unsigned short noHeader;					// depending on AMR support format - STATIC
    unsigned short concealment_on;				// enable error concealment
    unsigned short memory_preset;               		// defines memory allocation zone. Value taken from AMR_ALLOC_TYPE_ENUM_T
    unsigned short Payload_Format;              		// 0 (rfc3267), 1 (IF2), 2(GSM MODEM PAYLOAD), 0x82 (3G MODEM PAYLOAD)
    unsigned short Efr_on;                                          // 1 activates Alternative EFR mode
} AMR_DECODER_CONFIG_STRUCT_T;

typedef struct {
    // Config									//  DEFAULT CONF
    unsigned short data_valid;					// 0 as long as codec info not available, 1 otherwise
    unsigned short mode;
    unsigned short bitrate;						// value in bps/10
} AMR_DECODER_INFO_STRUCT_T;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

AUDIO_API_IMPORT int 
amr_syncword(int hi, int lo, int *length);

AUDIO_API_IMPORT CODEC_INIT_T
amr_init_decode(CODEC_INTERFACE_T *itf_cfg);

AUDIO_API_IMPORT CODEC_INIT_T
amr_init_decode_malloc(CODEC_INTERFACE_T *itf_cfg);

AUDIO_API_IMPORT void
amr_close_decode_malloc(CODEC_INTERFACE_T *itf_cfg);

AUDIO_API_IMPORT RETURN_STATUS_LEVEL_T
amr_decode_frame(CODEC_INTERFACE_T *interface_fe);

AUDIO_API_IMPORT void
amr_decode_reset(CODEC_INTERFACE_T *interface_fe);

AUDIO_API_IMPORT void
amr_decode_setBuffer(CODEC_INTERFACE_T *interface_fe, void *buf);

#ifdef __cplusplus
}
#endif // __cplusplus


/*************************** ENCODER PART *******************************************/
/* encoder */
//extern const int amr_encoder_heap_size;
//extern const int amr_encoder_struct_size;

typedef struct {
	// Config									//  DEFAULT CONF
	unsigned short mode;                        //  MR122 == 7      DYNAMIC
	unsigned short dtx;                         //  1, dtx enabled  DYNAMIC
    	unsigned short noHeader;					// depending on AMR support format - STATIC
    	unsigned short memory_preset;               // defines memory allocation zone. Value taken from AMR_ALLOC_TYPE_ENUM_T
    	unsigned short Payload_Format;              // 0 (rfc3267), 1 (IF2), 2(GSM MODEM PAYLOAD), 0x82 (3G MODEM PAYLOAD)
        unsigned short Efr_on;                      // 1 activates Alternative EFR mode
} AMR_ENCODER_CONFIG_STRUCT_T;

typedef struct {
	// Config									//  DEFAULT CONF
	unsigned short used_mode;                   //  MR122 == 7     
} AMR_ENCODER_INFO_STRUCT_T;


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*AUDIO_API_IMPORT CODEC_INIT_T
amr_encode_open(CODEC_INTERFACE_T *config);*/

AUDIO_API_IMPORT void
amr_encode_reset(CODEC_INTERFACE_T *itf);

AUDIO_API_IMPORT CODEC_INIT_T
amr_encode_open_malloc(CODEC_INTERFACE_T *config);

AUDIO_API_IMPORT void
amr_encode_close(CODEC_INTERFACE_T *config);

AUDIO_API_IMPORT RETURN_STATUS_LEVEL_T
amr_encode(CODEC_INTERFACE_T *config,
		   BS_STRUCT_T *OutputSt,
		   int *nbBytes);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* Do not edit below this line */

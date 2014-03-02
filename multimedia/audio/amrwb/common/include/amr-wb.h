/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _amrwb_h_
#define _amrwb_h_

#include "audiolibs_common.h"
#include "bitstream_lib_proto.h"
#include "common_interface.h"

#define AMRWB_MAX_BITSTREAM_SIZE_IN_BIT 560
// 560 : 72 bits magic number + 8bit header + 480 bits

#define AMRWB_SAMPLE_FRAME_SIZE_IN_WORD 320
#define AMRWB_BITS_PER_SAMPLE 16

/*************************** ENCODER STRUCT *******************************************/
typedef struct {
    // Config									//  DEFAULT CONF
    unsigned short mode;                        //  23.85 == 8
    unsigned short dtx;                         //  1, dtx enabled
    unsigned short noHeader;					// depending on AMRWB support format
    unsigned short memory_preset;               // defines memory allocation zone. (ex: MEM_ALL_DDR ...)
    unsigned short Payload_Format;
}  AMRWB_ENCODER_CONFIG_STRUCT_T;

typedef struct {
    // Config									//  DEFAULT CONF
    unsigned short used_mode;                        //  23.85 == 8	
}  AMRWB_ENCODER_INFO_STRUCT_T;

/*************************** DECODER STRUCT *******************************************/

enum {
    AMRWB_ERROR_CONCEALMENT_OFF,
    AMRWB_ERROR_CONCEALMENT_RX_NO_DATA
};


typedef struct {
    // Config									//  DEFAULT CONF
    unsigned short noHeader;					// depending on AMRWB support format
    unsigned short memory_preset;               // defines memory allocation zone. (ex: MEM_ALL_DDR ...)
    unsigned short concealment_on;				// enable error concealment
    unsigned short Payload_Format;
}  AMRWB_DECODER_CONFIG_STRUCT_T;

typedef struct {
    // Config									//  DEFAULT CONF
    unsigned short used_mode;					// depending on AMRWB support format
    
}  AMRWB_DECODER_INFO_STRUCT_T;


extern const EXTERN int amrwb_decoder_heap_size   ;
extern const EXTERN int amrwb_decoder_struct_size ;





#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

AUDIO_API_IMPORT int
amrwb_syncword(int hi, int lo, int *length);

AUDIO_API_IMPORT extern CODEC_INIT_T
amrwb_init_decode(CODEC_INTERFACE_T *itf_cfg);

AUDIO_API_IMPORT extern CODEC_INIT_T
amrwb_init_decode_malloc(CODEC_INTERFACE_T *itf_cfg);

AUDIO_API_IMPORT extern RETURN_STATUS_LEVEL_T
amrwb_decode_frame(CODEC_INTERFACE_T *itf_cfg);

/* encoder */
extern const EXTERN int amrwb_encoder_heap_size;
extern const EXTERN int amrwb_encoder_struct_size;

AUDIO_API_IMPORT extern CODEC_INIT_T
amrwb_encode_open(CODEC_INTERFACE_T *config);

AUDIO_API_IMPORT extern CODEC_INIT_T
amrwb_encode_open_malloc(CODEC_INTERFACE_T *config);

AUDIO_API_IMPORT extern void
amrwb_encode_reset(CODEC_INTERFACE_T *interface_fe);

AUDIO_API_IMPORT extern void
amrwb_decode_reset(CODEC_INTERFACE_T *interface_fe);

AUDIO_API_IMPORT extern RETURN_STATUS_LEVEL_T
amrwb_encode(CODEC_INTERFACE_T *config,
			 BS_STRUCT_T *OutputSt,
			 int *nbBytes);

AUDIO_API_IMPORT extern void
amrwb_close_decode_malloc(CODEC_INTERFACE_T *interface_fe);

AUDIO_API_IMPORT extern void
amrwb_encode_close(CODEC_INTERFACE_T *interface_fe);

AUDIO_API_IMPORT void
amrwb_decode_setBuffer(CODEC_INTERFACE_T *interface_fe, void *buf);

#ifdef __cplusplus
}
#endif // __cplusplus
			 
#endif /* Do not edit below this line */

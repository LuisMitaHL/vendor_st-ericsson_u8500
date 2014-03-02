/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * <function prototype declaration for enc/dec>
 * 
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __GSMFR_H
#define __GSMFR_H

#include "bitstream_lib_proto.h"
#include "audiolibs_common.h"
#include "common_interface.h"

#define GSMFR_MAX_BITSTREAM_SIZE             1216
#define GSMFR_MAX_BITSTREAM_SIZE_PAYLOAD_FMT 304
#define GSMFR_SAMPLE_FRAME_SIZE              160
#define GSMFR_DEC_SAMPLE_SIZE                16
#define GSMFR_ENC_NB_PRESETS                 5
#define GSMFR_DEC_NB_PRESETS                 5

//#ifndef USE_24BITS_LIBBITSTREAM
//#define bs_read_bit bs_read_bit_32b
//#define bs_write_byte bs_write_byte_32b
//#define bs_fast_move_bit bs_fast_move_bit_32b
//#endif

typedef struct{
    unsigned short has_dtx;
    unsigned short payload_format;
    unsigned short memory_preset;
    unsigned short VC_TX_SP;       //this field is sent to the bitstream for reformatted payload
    unsigned short mNewConfigAvailable; //sets when DTX value is 
}GSMFR_ENCODER_CONFIG_STRUCT_T;

typedef struct{
    unsigned short has_dtx;
    unsigned short payload_format;
    unsigned short memory_preset;
    unsigned short VC_RX_SP;       //this and parameters below are read from the bitstream for reformatted payload
    unsigned short VC_RX_SID;
    unsigned short VC_RX_TAF;
    unsigned short VC_RX_BFI;
}GSMFR_DECODER_CONFIG_STRUCT_T;

/* Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

AUDIO_API_IMPORT extern int gsmfr_syncword(int hi, int lo, int *length);
AUDIO_API_IMPORT extern CODEC_INIT_T gsmfr_decode_open(CODEC_INTERFACE_T *gsmfr_dec_itf);
AUDIO_API_IMPORT RETURN_STATUS_LEVEL_T gsmfr_decode_frame(CODEC_INTERFACE_T *gsmfr_dec_itf);
AUDIO_API_IMPORT extern void   gsmfr_decode_reset(CODEC_INTERFACE_T *gsmfr_dec_itf);
AUDIO_API_IMPORT extern void gsmfr_decode_close(CODEC_INTERFACE_T *gsmfr_dec_itf);
AUDIO_API_IMPORT void gsmfr_decode_setBuffer(CODEC_INTERFACE_T *gsmfr_dec_itf, void *buf);

AUDIO_API_IMPORT extern CODEC_INIT_T gsmfr_encode_open(CODEC_INTERFACE_T *gsmfr_enc_itf);
AUDIO_API_IMPORT RETURN_STATUS_LEVEL_T gsmfr_process_encode(CODEC_INTERFACE_T   *gsmfr_enc_itf);
AUDIO_API_IMPORT void gsmfr_encode_reset(CODEC_INTERFACE_T *gsmfr_enc_itf);
AUDIO_API_IMPORT void gsmfr_encode_close(CODEC_INTERFACE_T *gsmfr_enc_itf);
AUDIO_API_IMPORT void gsmfr_dtx_reinit(CODEC_INTERFACE_T *gsmfr_enc_itf);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * <function prototype declaration for enc/dec>
 * 
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __GSMHR_H__
#define __GSMHR_H__


#include "driver_readbuffer.h"
#include "bitstream_lib_proto.h"
#include "audiolibs_common.h"
#include "common_interface.h"

#define GSMHR_MAX_BITSTREAM_SIZE 352
#define GSMHR_SAMPLE_FRAME_SIZE  160
#define GSMHR_DEC_SAMPLE_SIZE     16
#define GSMHR_ENC_NB_PRESETS       5
#define GSMHR_DEC_NB_PRESETS       5
#define SCRATCH_BUFF             2000

/**************************************************************/

/* CONFIG STRUCTURE */
typedef struct{
    int payload_format;
    unsigned int memory_preset;
    unsigned short VC_RX_SP;       //this and parameters below is sent by modem
    unsigned short VC_RX_SID;
    unsigned short VC_RX_TAF;
    unsigned short VC_RX_UFI;
    unsigned short VC_RX_BFI;
    int etsi_testing;   /*This parameter takes into account if the codec gives the o/p in (ETSI format = 1) or in a (new payload format = 0)*/

} GSMHR_DECODER_CONFIG_STRUCT_T;


typedef struct{
    int DTXon;
    int payload_format;
    int res_error;
    unsigned int memory_preset;
    unsigned short VC_TX_VAD;
    unsigned short VC_TX_SP;       //this field is sent to modem
    int etsi_testing;   /*This parameter takes into account if the codec gives the o/p in (ETSI format = 1) or in a (new payload format = 0)*/

} GSMHR_ENCODER_CONFIG_STRUCT_T;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* Function Prototypes */
AUDIO_API_IMPORT extern int gsmhr_syncword(int hi, int lo, int *length);
AUDIO_API_IMPORT CODEC_INIT_T gsmhr_decode_open(CODEC_INTERFACE_T *gsmhr_dec_itf);
AUDIO_API_IMPORT RETURN_STATUS_LEVEL_T  gsmhr_decode_frame(CODEC_INTERFACE_T *gsmhr_dec_itf);
AUDIO_API_IMPORT void gsmhr_decode_reset(CODEC_INTERFACE_T *gsmhr_dec_itf);
AUDIO_API_IMPORT void gsmhr_decode_close(CODEC_INTERFACE_T *gsmhr_dec_itf);
AUDIO_API_IMPORT void gsmhr_decode_setBuffer(CODEC_INTERFACE_T *gsmhr_dec_itf, void *buf);

AUDIO_API_IMPORT CODEC_INIT_T gsmhr_encode_open(CODEC_INTERFACE_T *gsmhr_enc_itf);
AUDIO_API_IMPORT RETURN_STATUS_LEVEL_T  gsmhr_encode_frame(CODEC_INTERFACE_T *gsmhr_enc_itf);
AUDIO_API_IMPORT void gsmhr_encode_reset(CODEC_INTERFACE_T *gsmhr_enc_itf);
AUDIO_API_IMPORT void gsmhr_encode_close(CODEC_INTERFACE_T *gsmhr_enc_itf);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__GSMHR_H__

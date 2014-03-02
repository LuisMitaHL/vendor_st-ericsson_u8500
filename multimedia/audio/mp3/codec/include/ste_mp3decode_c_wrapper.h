

#ifndef _STE_MP3_DECODE_C_WRAPPER_H_
#define _STE_MP3_DECODE_C_WRAPPER_H_

#include "mp3.h"


#ifdef __cplusplus
extern "C" {
#endif
    CODEC_INIT_T          STE_MP3Decode_mp3_decode_init_malloc(CODEC_INTERFACE_T *itf);
    RETURN_STATUS_LEVEL_T STE_MP3Decode_mp3_decode_frame(CODEC_INTERFACE_T *interface_fe);
    void                  STE_MP3Decode_mp3_reset(CODEC_INTERFACE_T *itf);
    void                  STE_MP3Decode_mp3_close(CODEC_INTERFACE_T *itf);
	MP3_DEC_ERROR_ENUM_T  STE_mp3_2_5_parse_header(CODEC_INTERFACE_T *itf);
#ifdef __cplusplus
}
#endif

#endif /* _STE_MP3_DECODE_C_WRAPPER_H_ */

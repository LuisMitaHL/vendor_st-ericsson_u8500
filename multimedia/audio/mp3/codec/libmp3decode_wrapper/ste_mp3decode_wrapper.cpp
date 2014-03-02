

#include "ste_mp3decode_c_wrapper.h"

extern "C"
{

    CODEC_INIT_T          STE_MP3Decode_mp3_decode_init_malloc(CODEC_INTERFACE_T *itf)
    {
        return mp3_decode_init_malloc(itf);
    }

    RETURN_STATUS_LEVEL_T STE_MP3Decode_mp3_decode_frame(CODEC_INTERFACE_T *interface_fe)
    {
        return mp3_decode_frame(interface_fe);
    }

    void                  STE_MP3Decode_mp3_reset(CODEC_INTERFACE_T *itf)
    {
        mp3_reset(itf);
    }


    void                  STE_MP3Decode_mp3_close(CODEC_INTERFACE_T *itf)
    {
        mp3_close(itf);
    }

    MP3_DEC_ERROR_ENUM_T                  STE_mp3_2_5_parse_header(CODEC_INTERFACE_T *itf)
    {
     return               mp3_2_5_parse_header(itf);
    }


}

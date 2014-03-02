/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <omxilosalservices.h>
#include "omxilosal_debug.h"
using namespace omxilosalservices;

#define LOG_NDEBUG 0
#define DBGT_LAYER 0
#define LOG_TAG "OmxILOsalProperty"
#include "linux_utils.h"

#ifdef ANDROID
  #define MY_GET_PROPERTY(key, value, default_value) \
    property_get(key, value, default_value);
#else

static void MY_GET_PROPERTY(const char *key, char *value, const char *default_value) {
    char *tmp = getenv(key);
    if (tmp == NULL) {
        strncpy(value, default_value, OMXIL_OSAL_PROPERY_VALUE_MAX);
    } else {
        strncpy(value, tmp, OMXIL_OSAL_PROPERY_VALUE_MAX);
    }
}
#endif

#ifdef ANDROID
  #define PROPERTY_KEY_VIDEO_DECODER_MAX_RESOLUTION "ste.video.decoder.max.res"
  #define PROPERTY_KEY_VIDEO_DECODER_H264_MAX_LEVEL "ste.video.decoder.h264.max.lev"
  #define PROPERTY_KEY_VIDEO_ENCODER_OUTPUT_BUFFER_COUNT "ste.video.enc.out.buffercnt"
  #define PROPERTY_KEY_VIDEO_DECODER_MPEG4_INPUT_SIZE "ste.video.dec.mpeg4.in.size"
  #define PROPERTY_KEY_VIDEO_DECODER_RECYCLING_DELAY "ste.video.dec.recycle.delay"
  #define PROPERTY_KEY_VIDEO_DECODER_DISPLAY_ORDER "ste.video.dec.out.reorder"
  #define PROPERTY_KEY_VIDEO_DECODER_DEFAULT_LEVEL "ste.video.decoder.h264.def.lev"
#else
  #define PROPERTY_KEY_VIDEO_DECODER_MAX_RESOLUTION "ste_video_decoder_max_res"
  #define PROPERTY_KEY_VIDEO_DECODER_H264_MAX_LEVEL "ste_video_decoder_h264_max_lev"
  #define PROPERTY_KEY_VIDEO_ENCODER_OUTPUT_BUFFER_COUNT "ste_video_enc_out_buffercnt"
  #define PROPERTY_KEY_VIDEO_DECODER_MPEG4_INPUT_SIZE "ste_video_dec_mpeg4_in_size"
  #define PROPERTY_KEY_VIDEO_DECODER_RECYCLING_DELAY "ste_video_dec_recycle_delay"
  #define PROPERTY_KEY_VIDEO_DECODER_DISPLAY_ORDER "ste_video_dec_out_reorder"
  #define PROPERTY_KEY_VIDEO_DECODER_DEFAULT_LEVEL "ste_video_decoder_h264_def_lev"
#endif


OMX_ERRORTYPE OmxILOsalProperties::GetProperty(OMXIL_OSAL_PROPERTY_KEY key, char *value) {

    switch (key) {
        case EPropertyKeyVideoDecoderMaxResolution:
            MY_GET_PROPERTY(PROPERTY_KEY_VIDEO_DECODER_MAX_RESOLUTION, value, "");
            DBGT_PTRACE("GetProperty " PROPERTY_KEY_VIDEO_DECODER_MAX_RESOLUTION " => %s", value);
            return OMX_ErrorNone;

        case EPropertyKeyVideoDecoderH264MaxLevel:
            MY_GET_PROPERTY(PROPERTY_KEY_VIDEO_DECODER_H264_MAX_LEVEL, value, "");
            DBGT_PTRACE("GetProperty " PROPERTY_KEY_VIDEO_DECODER_H264_MAX_LEVEL " => %s", value);
            return OMX_ErrorNone;

        case EPropertyKeyVideoEncoderOutputBufferCount:
            MY_GET_PROPERTY(PROPERTY_KEY_VIDEO_ENCODER_OUTPUT_BUFFER_COUNT, value, "");
            DBGT_PTRACE("GetProperty " PROPERTY_KEY_VIDEO_ENCODER_OUTPUT_BUFFER_COUNT " => %s", value);
            return OMX_ErrorNone;

        case EPropertyKeyVideoDecoderMPEG4InputSize:
            MY_GET_PROPERTY(PROPERTY_KEY_VIDEO_DECODER_MPEG4_INPUT_SIZE, value, "");
            DBGT_PTRACE("GetProperty " PROPERTY_KEY_VIDEO_DECODER_MPEG4_INPUT_SIZE " => %s", value);
            return OMX_ErrorNone;

        case EPropertyKeyVideoDecoderRecyclingDelay:
            MY_GET_PROPERTY(PROPERTY_KEY_VIDEO_DECODER_RECYCLING_DELAY, value, "");
            DBGT_PTRACE("GetProperty " PROPERTY_KEY_VIDEO_DECODER_RECYCLING_DELAY " => %s", value);
            return OMX_ErrorNone;

        case EPropertyKeyVideoDecoderDisplayOrder:
            MY_GET_PROPERTY(PROPERTY_KEY_VIDEO_DECODER_DISPLAY_ORDER, value, "");
            DBGT_PTRACE("GetProperty " PROPERTY_KEY_VIDEO_DECODER_DISPLAY_ORDER " => %s", value);
            return OMX_ErrorNone;

        case EPropertyKeyVideoDecoderH264DefaultLevel:
            MY_GET_PROPERTY(PROPERTY_KEY_VIDEO_DECODER_DEFAULT_LEVEL, value, "");
            DBGT_PTRACE("GetProperty " PROPERTY_KEY_VIDEO_DECODER_DEFAULT_LEVEL " => %s", value);
            return OMX_ErrorNone;

        default:
            DBGT_ERROR("GetProperty request on uknown keyId=%d", (int)key);
            return OMX_ErrorBadParameter;
    }
}

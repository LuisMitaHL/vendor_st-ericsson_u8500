/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * STEJpegEnc.h : external Isp Jpeg encoder part
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STEJPEGENC_H
#define STEJPEGENC_H

#include <mmhwbuffer_ext.h>

namespace android {

class SteJpegEnc {
        public:
            sem_t mStateSem;
            sem_t mSetPortCamSem;
            sem_t mJpegEncSem;

            nsecs_t getConstructorDuration() { return timestamp_init; }
            nsecs_t getEncodeDuration() { return timestamp_encode; }

            SteJpegEnc( int width,
                        int height,
                        int quality,
                        int inBufferSize,
                        OMX_IMAGE_CODINGTYPE codingType,
                        OMX_COLOR_FORMATTYPE format );

            ~SteJpegEnc();

            int encode(  char* inBuffer,
                         char* &outBuffer,
                         int &outSize );

        private:
            OMX_HANDLETYPE  mJpegEnc;
            OMX_CALLBACKTYPE mJpegCallback;
            OmxUtils mOmxUtils;
            int mWidth;
            int mHeight;
            nsecs_t timestamp_begin, timestamp_end;
            nsecs_t timestamp_init;
            nsecs_t timestamp_encode;
            OMX_BUFFERHEADERTYPE *pJpegInBuffer;
            OMX_BUFFERHEADERTYPE *pJpegOutBuffer;
            OMX_PARAM_PORTDEFINITIONTYPE paramJPEGinput;
            OMX_PARAM_PORTDEFINITIONTYPE paramJPEGoutput;

            OMX_COLOR_FORMATTYPE mformat;
            bool mIsExifInJpeg;
    };
}; // namespace android

#endif





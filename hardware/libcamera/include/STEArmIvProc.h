/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
*
 * STEJpegEnc.h : external Isp Jpeg encoder part
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STEARMIVPROC_H
#define STEARMIVPROC_H

namespace android {

    class SteArmIvProc {

    public:
        sem_t mStateSem;
        sem_t mSetPortCamSem;
        sem_t mArmIvProcSem;

        nsecs_t getConstructorDuration() { return timestamp_init; }
        nsecs_t getEncodeDuration() { return timestamp_encode; }

        SteArmIvProc(
                int inputWidth,
                int inputHeight,
                OMX_COLOR_FORMATTYPE inputColorFormat,
                int outputWidth,
                int outputHeight,
                OMX_COLOR_FORMATTYPE outputColorFormat);

        ~SteArmIvProc();

        int transform(
                char* inBuffer,
                char* &outBuffer);

    private:
        OMX_HANDLETYPE  mArmIvProc;
        OMX_CALLBACKTYPE mArmIvProcCallback;
        OmxUtils mOmxUtils;
        int mInputWidth;
        int mInputHeight;
        int mInputBufSize;
        OMX_COLOR_FORMATTYPE mInputColorFormat;
        int mOutputWidth;
        int mOutputHeight;
        int mOutputBufSize;
        OMX_COLOR_FORMATTYPE mOutputColorFormat;
        nsecs_t timestamp_begin, timestamp_end;
        nsecs_t timestamp_init;
        nsecs_t timestamp_encode;

        OMX_BUFFERHEADERTYPE *pArmIvProcInBuffer;
        OMX_BUFFERHEADERTYPE *pArmIvProcOutBuffer;
        OMX_PARAM_PORTDEFINITIONTYPE paramArmIvProcInput;
        OMX_PARAM_PORTDEFINITIONTYPE paramArmIvProcOutput;
    };
}; // namespace android

#endif

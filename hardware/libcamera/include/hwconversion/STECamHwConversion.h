/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
*
 * STECamHwConversion.h : interface of class SteHwConv
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef SteHwConv_H
#define SteHwConv_H

namespace android {

    class SteHwConv {

    public:
        nsecs_t getConstructorDuration() { return timestamp_init; }
        nsecs_t getEncodeDuration() { return timestamp_encode; }

        SteHwConv(
                int rotation,
                int inputWidth,
                int inputHeight,
                OMX_COLOR_FORMATTYPE inputColorFormat,
                int outputWidth,
                int outputHeight,
                OMX_COLOR_FORMATTYPE outputColorFormat);

        ~SteHwConv();

        int transform(
        sp<MemoryBase> inMem,
        sp<MemoryBase> outMem,
        int inMemFd,
        int outMemFd );

        int transform(
                int inMemFd,
                int inOffset,
                int inSize,
                int outMemFd,
                int outOffset,
                int outSize
            );

        int setCropSrc(int left, int top, int width, int height);
        int setCropDst(int left, int top, int width, int height);

    private:
        int mRotation;
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
        int mSrcRectX, mSrcRectY, mSrcRectWidth, mSrcRectHeight;
        int mDstRectX, mDstRectY, mDstRectWidth, mDstRectHeight;
        nsecs_t timestamp_encode;
        int mBlt;
    };
}; // namespace android

#endif

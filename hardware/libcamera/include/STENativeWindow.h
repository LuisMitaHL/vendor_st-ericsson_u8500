 /*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 *
 * Classes to manage interaction with the android native window
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef STE_NATIVE_WINDOW_H
#define STE_NATIVE_WINDOW_H

#include <unistd.h>
#include <hardware/camera.h>
#include <ui/GraphicBuffer.h>

namespace android {

class STENativeWindow {
public:
    STENativeWindow(preview_stream_ops_t *window)
        : mPreviewWindow(window),
        mInitialized(false)
    {}
    ~STENativeWindow() { }
    preview_stream_ops_t *getNativeWindow()
    {
        return mPreviewWindow;
    }

    // a return of -1 indicates failure in the following APIs
    int init(int width, int height, int pixelFormat, int usage);
    int setCrop(int left, int top, int right, int bottom);
    int getMinUndequeuedBufferCount()
    {
        return mInitialized ? mMinUndequeuedCount : -1;
    }
    int setBufferCount(int count);
    // pass pStride as NULL if not required
    int dequeueBuffer(buffer_handle_t **ppBuffer, int *pStride);
    int enqueueBuffer(buffer_handle_t &pBuffer);
    int cancelBuffer(buffer_handle_t &pBuffer);
    int lockBuffer(buffer_handle_t &pBuffer);

private:
    preview_stream_ops_t *mPreviewWindow;
    int mMinUndequeuedCount;
    bool mInitialized;
};

}; //  end namespace android
#endif //STE_NATIVE_WINDOW_H

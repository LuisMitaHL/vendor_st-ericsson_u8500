 /*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 *
 * Classes to manage interaction with the android native window
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "STENativeWindow.h"
#include "STECamTrace.h"

namespace android {

int STENativeWindow::init(
        int width,
        int height,
        int pixelFormat,
        int usage)
{
    DBGT_PROLOG("");
    if (mPreviewWindow == NULL) {
        DBGT_CRITICAL("mPreviewWindow = %p", mPreviewWindow);
        DBGT_EPILOG("");
        return -1;
    }
    if (mInitialized)
    {
        DBGT_EPILOG("");
        return 0;
    }

    status_t error
        = mPreviewWindow->set_buffers_geometry(
                mPreviewWindow,
                width,
                height,
                pixelFormat);
    if (OK != error) {
        DBGT_CRITICAL("error = %d", error);
        DBGT_EPILOG("");
        return -1;
    }

    error = mPreviewWindow->get_min_undequeued_buffer_count(
                mPreviewWindow, &mMinUndequeuedCount);
    if (OK != error) {
        DBGT_CRITICAL("error = %d", error);
        DBGT_EPILOG("");
        return -1;
    }

    error = mPreviewWindow->set_usage(
                mPreviewWindow,
                usage);
    if (OK != error) {
        DBGT_CRITICAL("error = %d", error);
        DBGT_EPILOG("");
        return -1;
    }

    mInitialized = true;
    DBGT_EPILOG("");
    return 0;
}

int STENativeWindow::setCrop(int left, int top, int right, int bottom)
{
    DBGT_PROLOG("");
    if (!mInitialized) {
        DBGT_CRITICAL("mInitialized = %d", mInitialized);
        DBGT_EPILOG("");
        return -1;
    }
    status_t error = mPreviewWindow->set_crop(
                        mPreviewWindow,
                        left, top,
                        right, bottom);
    if (OK != error) {
        DBGT_CRITICAL("error = %d", error);
        DBGT_EPILOG("");
        return -1;
    }
    
    DBGT_EPILOG("");
    return 0;
}

int STENativeWindow::setBufferCount(int count)
{
    DBGT_PROLOG("");
    if (!mInitialized || count < mMinUndequeuedCount) {
        DBGT_CRITICAL("mInitialized = %d, count = %d", mInitialized, count);
        DBGT_EPILOG("");
        return -1;
    }

    status_t error = mPreviewWindow->set_buffer_count(
                        mPreviewWindow, count);
    if (OK != error) {
        DBGT_CRITICAL("error = %d", error);
        DBGT_EPILOG("");
        return -1;
    }

    DBGT_EPILOG("");
    return 0;
}

int STENativeWindow::dequeueBuffer(buffer_handle_t **ppBuffer, int *pStride)
{
    DBGT_PROLOG("");
    if (!mInitialized) {
        DBGT_CRITICAL("mInitialized = %d", mInitialized);
        DBGT_EPILOG("");
        return -1;
    }

    int stride;
    status_t error = mPreviewWindow->dequeue_buffer(
                        mPreviewWindow,
                        ppBuffer,
                        &stride);
    if (OK != error) {
        DBGT_CRITICAL("error = %d", error);
        DBGT_EPILOG("");
        return -1;
    }
    if (pStride) {
        *pStride = stride;
    }

    DBGT_EPILOG("");
    return 0;
}

int STENativeWindow::enqueueBuffer(buffer_handle_t &pBuffer)
{
    DBGT_PROLOG("");
    if (!mInitialized) {
        DBGT_CRITICAL("mInitialized = %d", mInitialized);
        DBGT_EPILOG("");
        return -1;
    }

    status_t error = mPreviewWindow->enqueue_buffer(
                        mPreviewWindow,
                        &pBuffer);
    if (OK != error) {
        DBGT_CRITICAL("error = %d", error);
        DBGT_EPILOG("");
        return -1;
    }

    DBGT_EPILOG("");
    return 0;
}

int STENativeWindow::cancelBuffer(buffer_handle_t &pBuffer)
{
    DBGT_PROLOG("");
    if (!mInitialized) {
        DBGT_CRITICAL("mInitialized = %d", mInitialized);
        DBGT_EPILOG("");
        return -1;
    }

    status_t error = mPreviewWindow->cancel_buffer(
                        mPreviewWindow,
                        &pBuffer);
    if (OK != error) {
        DBGT_CRITICAL("error = %d", error);
        DBGT_EPILOG("");
        return -1;
    }

    DBGT_EPILOG("");
    return 0;
}

int STENativeWindow::lockBuffer(buffer_handle_t &pBuffer)
{
    DBGT_PROLOG("");
    if (!mInitialized) {
        DBGT_CRITICAL("mInitialized = %d", mInitialized);
        DBGT_EPILOG("");
        return -1;
    }

    status_t error = mPreviewWindow->lock_buffer(
                        mPreviewWindow,
                        &pBuffer);
    if (OK != error) {
        DBGT_CRITICAL("error = %d", error);
        DBGT_EPILOG("");
        return -1;
    }

    DBGT_EPILOG("");
    return 0;
}

}; // end namespace android

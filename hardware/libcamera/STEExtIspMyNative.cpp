 /*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 *
 * Class to manage interaction with the native window
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#undef CAM_LOG_TAG
#define CAM_LOG_TAG DBGT_TAG
#define DBGT_LAYER 1
#define DBGT_PREFIX "NatWin"


#include <binder/MemoryBase.h>
#include <utils/threads.h>

//System includes
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/android_pmem.h>
#include <linux/time.h>
#include <fcntl.h>
#include <mmhwbuffer.h>
#include <mmhwbuffer_ext.h>
#include <mmhwbuffer_c_wrapper.h>

//Internal includes
#include "STECamTrace.h"
#include "STECamUtils.h"
#include "STECamOmxUtils.h"
#include "STENativeWindow.h"
#include "STEExtIspMyNative.h"

namespace android {

#undef _CNAME_
#define _CNAME_ MyNativeWindow

MyNativeWindow::MyNativeWindow(PFMMCreateNativeBuffer aCreate, PFMMDestroyNativeBuffer aDestroy)
    :mIsInitialized(true),
     mSTENativeWindow(NULL),
     mTotalNumOfNativeBuff(0),
     mNBuffers(NULL),
     mCreate(aCreate),
     mDestroy(aDestroy),
     mRequestMemoryFunc(NULL),
     mWindowWidth(-1),
     mWindowHeight(-1)
{
    DBGT_PTRACE("MyNativeWindow IN");
}

status_t MyNativeWindow::init(
        preview_stream_ops_t* aWindow,
        camera_request_memory aRequestMemoryFunc,
        int width, int height,
        int pixelFormat, int usage)
{
    DBGT_ASSERT((NULL != aWindow), "No preview window set");
    Mutex::Autolock autoLock(mLock);
    if (mIsInitialized) {
        if (aWindow == getNativeWindow_l()) {
            return OK;
        } else {
            deinit_l();
        }
    }

    mSTENativeWindow = new STENativeWindow(aWindow);
    if(mSTENativeWindow == NULL) {
        return NO_MEMORY;
    }

    status_t err = mSTENativeWindow->init(
                       width,
                       height,
                       pixelFormat,
                       usage);
    if(OK != err) {
        DBGT_CRITICAL("STENativeWindow::init() failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mWindowWidth = width;
    mWindowHeight = height;
    mPreviewColorFormat = pixelFormat;
    err = fetchNBufferFromNativeWindow_l();
    if (OK != err) {
        DBGT_CRITICAL("getNativeBuffFromNativeWindow failed !!");
        deinit_l();
        return err;
    }
    mRequestMemoryFunc = aRequestMemoryFunc;
    err = setupNBuffersForPreview_l();
    if (OK != err) {
        DBGT_CRITICAL("setupNBuffersForPreview_l failed !!");
        deinit_l();
        return err;
    }
    mIsInitialized = true;

    return err;
}



status_t MyNativeWindow::fetchNBufferFromNativeWindow_l(void)
{
    DBGT_PROLOG("");

    mTotalNumOfNativeBuff = kPreviewBufferCount + mSTENativeWindow->getMinUndequeuedBufferCount();
    status_t err = mSTENativeWindow->setBufferCount(mTotalNumOfNativeBuff);
    if(OK != err) {
        DBGT_CRITICAL("MyNativeWindow::setBufferCount() failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

    mNBuffers = new NBuffer*[mTotalNumOfNativeBuff];
    if (NULL == mNBuffers) {
        DBGT_CRITICAL("Not enough memory to create mNBuffer array");
        return NO_MEMORY;
    }

    // Now dequeue buffers from Native window
    int i;
    for (i = 0; i < mTotalNumOfNativeBuff; i++) {

        buffer_handle_t* buffHandle;
        err = mSTENativeWindow->dequeueBuffer(
                  &buffHandle, NULL);
        if(err != OK) {
            goto CANCEL_BUFFERS;
        }
        mNBuffers[i] = new NBuffer(*buffHandle);
        if(NULL == mNBuffers[i]) {
            goto CANCEL_BUFFERS;
        }
    }

    DBGT_PTRACE("Native handles:");
    for (i = 0; i < mTotalNumOfNativeBuff; i++) {
        DBGT_PTRACE("0x%x ",(unsigned int)(mNBuffers[i]->getBufferHandle()));
    }

    DBGT_EPILOG("");

    return err; //err should be OK

CANCEL_BUFFERS:
    //  Since we get error while dequeue ith buffer
    //  that means (i-1) buffer are already dequeued,so return
    //  them back to native window

    int cancelEnd = i;
    for (i = 0; i < cancelEnd; i++) { // return back (i-1) buffers, see condition 
        err = returnBuffer_l(mNBuffers[i]->getBufferHandle());
        if(err != OK) {
            DBGT_CRITICAL("Cancelbuffer error on buff num (%d), but still we continue...",i);
        }
    }
    return UNKNOWN_ERROR;  //unable to get required native buffers from native window
}


status_t MyNativeWindow::setupNBuffersForPreview_l()
{

    buffer_handle_t buf;
    sp<IMemoryHeap>  heap;
    sp<MemoryBase> memory;
    MMNativeBuffer* nativeBuffer = NULL;

    for (int i = 0; i<mTotalNumOfNativeBuff; i++) {

        heap.clear();
        memory.clear();
        buf = mNBuffers[i]->getBufferHandle();
        nativeBuffer = (MMNativeBuffer*)mCreate((void*)buf);
        nativeBuffer->init();

        mNBuffers[i]->setMMNativeBuffer(nativeBuffer);
        camera_memory_t* camMem = mRequestMemoryFunc(
                                          mNBuffers[i]->getMMNativeBuffer()->getHandle(),
                                          getPreviewFrameSize(),
                                          1,
                                          this);
        mNBuffers[i]->setClientMemory(camMem);
    }

    //return all buffers to Native window
    returnAllNBuffers_l();
    return NO_ERROR;
}

}; // end namespace android


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */
#define _CNAME_ CamServiceBuffer

#define DBGT_LAYER 2
#define DBGT_PREFIX "BufSrv"

//Internal includes
#include "STECamTrace.h"
#include "STECamServiceBuffer.h"

namespace android {

CamServiceBuffer::CamServiceBuffer(camera_request_memory &aRequestMemory,
        void **aCallbackCookie)
        : mCamHandle(0),
        mRequestMemory(aRequestMemory),
        mCallbackCookie(aCallbackCookie)
{
    DBGT_PROLOG("");
    DBGT_EPILOG("");
}

CamServiceBuffer::~CamServiceBuffer()
{
    DBGT_PROLOG("");
    if (mCamHandle) {
        mCamHandle->release(mCamHandle);
    }

    DBGT_EPILOG("");
}

status_t CamServiceBuffer::alloc(const sp<IMemoryHeap>& aHeap, ssize_t aOffset, size_t aSize)
{
    DBGT_PINFO("Offset: %ld Size: %zu", aOffset, aSize);

    DBGT_ASSERT(0 != aSize, "Size is 0");

    status_t err = NO_ERROR;

    mCamHandle = mRequestMemory(-1, aSize, 1, (void *)mCallbackCookie);
    if (mCamHandle == 0) {
        DBGT_EPILOG("");
        return (int)NO_MEMORY;
    }

    //No need to set mMemoryBase for CamServiceBuffer.
    //mMemoryBase= new MemoryBase(camHeap->mHeap, 0, aSize);

    DBGT_EPILOG("");
    return err;
}

void CamServiceBuffer::dealloc()
{
    DBGT_PROLOG("");

    if (mCamHandle){
        mCamHandle->release(mCamHandle);
    }
    DBGT_EPILOG("");
    return;
}

void* CamServiceBuffer::base()
{
    DBGT_PROLOG("");

    DBGT_ASSERT(NULL != mCamHandle->data);

    DBGT_EPILOG("");
    return mCamHandle->data;
}

} //namespace android

/* End of STECamServiceBuffer.cpp */

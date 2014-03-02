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
#define _CNAME_ CamBuffer

#define DBGT_LAYER 2
#define DBGT_PREFIX "Buf   "

//Internal includes
#include "STECamTrace.h"
#include "STECamBuffer.h"

namespace android {

CamBuffer::CamBuffer() : mMemoryBase(0)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

CamBuffer::~CamBuffer()
{
    DBGT_PROLOG("");

    mMemoryBase.clear();

    DBGT_EPILOG("");
}

status_t CamBuffer::realloc(const sp<IMemoryHeap>& aHeap, ssize_t aOffset, size_t aSize)
{
    DBGT_PROLOG("Offset: %ld Size: %zu", aOffset, aSize);

    DBGT_ASSERT(0 != aSize, "Size is 0");

    status_t err = NO_ERROR;

    //clear previous buffer
    clear();

    //perform allocation
    err = alloc(aHeap, aOffset, aSize);
    if(NO_ERROR != err) {
        DBGT_CRITICAL("alloc failed err = %d", (int)err);
        DBGT_EPILOG("");
        return err;
    }

    DBGT_EPILOG("");
    return err;
}

void CamBuffer::clear()
{
    DBGT_PROLOG("");

    dealloc();
    mMemoryBase.clear();

    DBGT_EPILOG("");
    return;
}

/* virtual */ void CamBuffer::dealloc()
{
    DBGT_PROLOG("");

    //do nothing

    DBGT_EPILOG("");
}

/* virtual */ void* CamBuffer::base()
{
    DBGT_PROLOG("");

    ssize_t offset;
    size_t size;
    sp<IMemoryHeap> heap = memoryBase()->getMemory(&offset, &size);

    DBGT_ASSERT(NULL != heap->base());

    DBGT_EPILOG("");
    return (void*)heap->base();
}

} //namespace android

/* End of STECamBuffer.cpp */

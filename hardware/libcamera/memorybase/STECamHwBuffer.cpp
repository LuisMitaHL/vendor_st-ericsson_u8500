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
#define _CNAME_ CamHwBuffer

//Internal includes
#define CAM_LOG_TAG "STECamera"
#include "STECamTrace.h"
#include "STECamHwBuffer.h"

namespace android {

CamHwBuffer::CamHwBuffer()
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

CamHwBuffer::~CamHwBuffer()
{
}

/* virtual */ status_t CamHwBuffer::alloc(const sp<IMemoryHeap>& aHeap, ssize_t aOffset, size_t aSize)
{
    DBGT_PROLOG("Offset: %ld Size: %zu", aOffset, aSize);

    DBGT_ASSERT(0 != aSize, "Size is 0");

    status_t err = NO_ERROR;

    //alloc memory base
    mMemoryBase= new MemoryBase(aHeap, aOffset, aSize);
    if(mMemoryBase == 0) {
        DBGT_EPILOG("");
        return (int)NO_MEMORY;
    }

    DBGT_EPILOG("");
    return err;
}

} //namespace android

/* End of STECamHwBuffer.cpp */

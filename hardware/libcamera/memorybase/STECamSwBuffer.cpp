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
#define _CNAME_ CamSwBuffer

#define DBGT_LAYER 2
#define DBGT_PREFIX "BufSw "

//Internal includes
#include "STECamTrace.h"
#include "STECamSwBuffer.h"

namespace android {

CamSwBuffer::CamSwBuffer() : mMemoryHeapBase(0)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

CamSwBuffer::~CamSwBuffer()
{
    DBGT_PROLOG("");

    mMemoryHeapBase.clear();

    DBGT_EPILOG("");
}

/* virtual */ status_t CamSwBuffer::alloc(const sp<IMemoryHeap>& aHeap, ssize_t aOffset, size_t aSize)
{
    DBGT_PROLOG("Offset: %ld Size: %zu", aOffset, aSize);

    DBGT_ASSERT(0 != aSize, "Size is 0");

    status_t err = NO_ERROR;

    //alloc heap base
    mMemoryHeapBase = new MemoryHeapBase(aSize);
    if(mMemoryHeapBase == 0) {
        DBGT_EPILOG("");
        return (int)NO_MEMORY;
    }

    //alloc memory base. Ignore offset and heap, used only by HWBuffer
    mMemoryBase= new MemoryBase(mMemoryHeapBase, 0, aSize);
    if(mMemoryBase == 0) {
        DBGT_EPILOG("");
        return (int)NO_MEMORY;
    }

    DBGT_EPILOG("");
    return (int)err;
}

/*virtual */ void CamSwBuffer::dealloc()
{
    DBGT_PROLOG("");

    mMemoryHeapBase.clear();

    DBGT_EPILOG("");
    return;
}

} //namespace android

/* End of STECamSwBuffer.cpp */

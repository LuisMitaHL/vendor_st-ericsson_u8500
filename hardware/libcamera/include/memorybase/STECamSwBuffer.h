/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMSWBUFFER_H
#define STECAMSWBUFFER_H

//Internal includes
#include "STECamBuffer.h"

namespace android {

class CamSwBuffer : public CamBuffer
{
public:
    /* CTOR */
    CamSwBuffer();

    /* DTOR */
    ~CamSwBuffer();

private:
    /* Alloc, from CamBuffer */
    virtual status_t alloc(const sp<IMemoryHeap>& aHeap, ssize_t aOffset, size_t aSize);

    /* De-alloc, from CamBuffer */
    virtual void dealloc();
private:
    sp<MemoryHeapBase> mMemoryHeapBase; //Sw HeapBase
};

}

#endif // STECAMSWBUFFER_H


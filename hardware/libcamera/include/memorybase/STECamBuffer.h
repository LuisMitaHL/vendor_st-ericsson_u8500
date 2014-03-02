/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMBUFFER_H
#define STECAMBUFFER_H

//System include
#include <stdint.h>
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>

namespace android {

class CamBuffer
{
public:
    /* CTOR */
    CamBuffer();

    /* DTOR */
    virtual ~CamBuffer();

    /* Re-Alloc */
    status_t realloc(const sp<IMemoryHeap>& aHeap, ssize_t aOffset, size_t aSize);

    /* Clear */
    void clear();

    /* base */
    virtual void* base();

public:
    inline sp<MemoryBase>& memoryBase();

private:
    /* Allocate */
    virtual status_t alloc(const sp<IMemoryHeap>& aHeap, ssize_t aOffset, size_t aSize) =0;

    /* De-allocate */
    virtual void dealloc();

protected:
  sp<MemoryBase> mMemoryBase; //Memory base
};

#include "STECamBuffer.inl"

}

#endif // STECAMBUFFER_H


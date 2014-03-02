/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMSERVICEBUFFER_H
#define STECAMSERVICEBUFFER_H

//Internal includes
#include "STECamBuffer.h"
#include <hardware/camera.h>

namespace android {

class CamServiceBuffer : public CamBuffer
{
public:
    /* CTOR */
    CamServiceBuffer(camera_request_memory &aRequestMemory, void **aCallbackCookie);

    /* DTOR */
    ~CamServiceBuffer();

    /* Alloc, from CamBuffer */
    virtual status_t alloc(const sp<IMemoryHeap>& aHeap, ssize_t aOffset, size_t aSize);

    /* De-alloc, from CamBuffer */
    virtual void dealloc();

    /* base, from CamBuffer */
    virtual void* base();

public:
  camera_memory_t *mCamHandle;
  camera_request_memory &mRequestMemory;
  void **mCallbackCookie;
};

}

#endif // STECAMSERVICEBUFFER_H


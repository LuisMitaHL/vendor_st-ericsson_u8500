/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMHWBUFFER_H
#define STECAMHWBUFFER_H

//Internal includes
#include "STECamBuffer.h"

namespace android {

class CamHwBuffer : public CamBuffer
{
public:
    /* CTOR */
    CamHwBuffer();

    /* DTOR */
    ~CamHwBuffer();

    /* Alloc, from CamBuffer */
    virtual status_t alloc(const sp<IMemoryHeap>& aHeap, ssize_t aOffset, size_t aSize);

};

}

#endif // STECAMHWBUFFER_H


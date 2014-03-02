/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMMEMORYBASE_H
#define STECAMMEMORYBASE_H

//System include
#include <stdint.h>
#include <binder/MemoryBase.h>

//Internal includes
#include "STECamTrace.h"

namespace android {

class CamMemoryBase : public MemoryBase
{
public:
    /* CTOR */
    CamMemoryBase(const sp<IMemoryHeap>& aHeap, ssize_t aOffset, size_t aSize)
      : MemoryBase(aHeap, aOffset, aSize) {};

    /* Setup buffer header */
    void setOmxBufferHeaderType(OMX_BUFFERHEADERTYPE* const aOmxBufferHeaderType)
    {
        DBGT_PROLOG("OmxBufferHeaderType: %p", aOmxBufferHeaderType);
        if(NULL == aOmxBufferHeaderType){
            DBGT_WARNING("OmxBufferHeaderType is NULL");
        }else{
            mOmxBufferHeaderType = aOmxBufferHeaderType;
        }
        DBGT_EPILOG("");
    };

    OMX_BUFFERHEADERTYPE* omxBufferHeaderType()
    {
        DBGT_PROLOG("");
        if(NULL == mOmxBufferHeaderType){
            DBGT_WARNING("OmxBufferHeaderType is NULL");
        }
        DBGT_EPILOG("mOmxBufferHeaderType - %p", mOmxBufferHeaderType);
        return mOmxBufferHeaderType;
    }

private:
    OMX_BUFFERHEADERTYPE* mOmxBufferHeaderType; /**< OMX Buffer header type */

};

}

#endif // STECAMMEMORYBASE_H


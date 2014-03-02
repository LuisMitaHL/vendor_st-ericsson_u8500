/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMSWPROCESSINGTHREAD_H
#define STECAMSWPROCESSINGTHREAD_H

//Internal include
#include "STECamThreadHandler.h"

namespace android {

typedef sp<MemoryBase>& (STECamera::*requestHandler)(
                    const OMX_BUFFERHEADERTYPE* const aOmxBuffHdr,
                    /*  MMHwBuffer* const aMMHwBuffer, */
                    int aPrevWidth, int aPrevHeight);

struct SwProcessingThreadData {
    OMX_BUFFERHEADERTYPE* mOmxBufferHeader;
    /*  MMHwBuffer* mMMHwBuffer; */
    sp<MemoryBase>* mMemoryBase;
    requestHandler mReqHandler; /**< Handler */
    int mPrevWidth;
    int mPrevHeight;
};

//Forwar declaration
class STECamera;

//SwProcessing thread
template<typename T>
class SwProcessingThread : public ThreadHandler<T> {
};

template<>
class SwProcessingThread<SwProcessingThreadData> :
        public ThreadHandler<SwProcessingThreadData> {
public:
    /* CTOR */
    inline SwProcessingThread(STECamera* aParent);

    /* Handle request, from ThreadHandler */
    virtual status_t handleRequest(SwProcessingThreadData& aData);

private:
    STECamera* mParent;
};

#include "STECamSwProcessingThread.inl"

}

#endif // STECAMSWPROCESSINGTHREAD_H


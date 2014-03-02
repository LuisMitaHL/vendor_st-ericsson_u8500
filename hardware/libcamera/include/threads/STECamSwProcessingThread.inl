/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMSWPROCESSINGTHREAD_INL
#define STECAMSWPROCESSINGTHREAD_INL

inline SwProcessingThread<SwProcessingThreadData>::SwProcessingThread(STECamera* aParent) :
    mParent(aParent)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

status_t SwProcessingThread<SwProcessingThreadData>::handleRequest(SwProcessingThreadData& aData)
{
    DBGT_PROLOG("OmxBufferHeader: %p", aData.mOmxBufferHeader /* , aData.mMMHwBuffer */);
    DBGT_ASSERT( NULL != aData.mOmxBufferHeader, "OmxBufferHeader is NULL");
   // DBGT_ASSERT( NULL != aData.mMMHwBuffer, "MMHwBuffer is NULL");

    aData.mMemoryBase = &(*mParent.*aData.mReqHandler)(aData.mOmxBufferHeader /*, aData.mMMHwBuffer */, aData.mPrevWidth, aData.mPrevHeight);

    DBGT_PTRACE("MemoryBase: %p", aData.mMemoryBase);

	DBGT_EPILOG("");
	return (int)NO_ERROR;
}

#endif // STECAMSWPROCESSINGTHREAD_INL

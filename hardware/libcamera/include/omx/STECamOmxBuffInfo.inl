/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMOMXBUFFINO_INL
#define STECAMOMXBUFFINO_INL

inline OmxBuffInfo::OmxBuffInfo() : mCamMemoryBase(NULL),
                                    mProcessingBuffer(NULL),
                                    mSwRotation(NULL),
                                    mSwConversion(NULL),
                                    mFlags(ENone),
                                    mUserData(NULL),
                                    m_pMMHwBuffer(NULL),
                                    mHeapOffset(0),
                                    mBufferOwner(false)
{
    memset(&mMMHwBufferInfo, 0, sizeof(mMMHwBufferInfo));
    memset(&mMMHwChunkMetaData, 0, sizeof(mMMHwChunkMetaData));
};

inline OmxBuffInfo::OmxBuffInfo(CamMemoryBase* const aCamMemoryBase) :
    mCamMemoryBase(aCamMemoryBase), mSwRotation(NULL), mSwConversion(NULL), mFlags(ENone),
    mUserData(NULL), m_pMMHwBuffer(NULL), mHeapOffset(0), mBufferOwner(false)
{
    memset(&mMMHwBufferInfo, 0, sizeof(mMMHwBufferInfo));
    memset(&mMMHwChunkMetaData, 0, sizeof(mMMHwChunkMetaData));
};

inline OmxBuffInfo::~OmxBuffInfo()
{
    //check buffer in correct state
    //DBGT_ASSERT(ENone == mFlags, "Buffer unknown state: 0xx%08x", mFlags);

    clear();

    mUserData = NULL;

    delete mProcessingBuffer;
}

inline void OmxBuffInfo::clear()
{
    mCamMemoryBase.clear();

    mSwRotation = NULL;
    mSwConversion = NULL;
}

inline void OmxBuffInfo::setUserData(void* aUserData)
{
    DBGT_PROLOG("UserData: %p", aUserData);

    DBGT_ASSERT(NULL != aUserData, "User Data is NULL");

    mUserData = aUserData;

    DBGT_EPILOG("");
    return;
}

inline void* OmxBuffInfo::userData()
{
    DBGT_PROLOG("");

    DBGT_ASSERT(NULL != mUserData, "User Data is NULL");

	DBGT_EPILOG("mUserData - %p", mUserData);
	return mUserData;
}

inline void* OmxBuffInfo::userData() const
{
    DBGT_PROLOG("");

    DBGT_ASSERT(NULL != mUserData, "User Data is NULL");

	DBGT_EPILOG("mUserData - %p", mUserData);
	return mUserData;
}

/*static*/ inline OMX_ERRORTYPE OmxBuffInfo::fillBuffer(OMX_HANDLETYPE aHandle,
                                                        OMX_BUFFERHEADERTYPE* const aOmxBufferHeader)
{
    DBGT_PROLOG("Handle: %p OmxBufferHeader: %p", aHandle, aOmxBufferHeader);
    DBGT_ASSERT(NULL != aHandle, "Handle is NULL");

    OMX_ERRORTYPE err = pushBuffer(aHandle, aOmxBufferHeader, ((OMX_COMPONENTTYPE*)aHandle)->FillThisBuffer);

	DBGT_EPILOG("");
	return err;
}

/*static*/ inline OMX_ERRORTYPE OmxBuffInfo::emptyBuffer(OMX_HANDLETYPE aHandle,
                                                        OMX_BUFFERHEADERTYPE* const aOmxBufferHeader)
{
    DBGT_PROLOG("Handle: %p OmxBufferHeader: %p", aHandle, aOmxBufferHeader);
    DBGT_ASSERT(NULL != aHandle, "Handle is NULL");

    OMX_ERRORTYPE err = pushBuffer(aHandle, aOmxBufferHeader, ((OMX_COMPONENTTYPE*)aHandle)->EmptyThisBuffer);

	DBGT_EPILOG("");
	return err;
}

/*static*/ inline OMX_ERRORTYPE OmxBuffInfo::pushBuffer(OMX_HANDLETYPE aHandle,
                                                         OMX_BUFFERHEADERTYPE* const aOmxBufferHeader,
                                                         OMX_ERRORTYPE (*aFunc)(OMX_HANDLETYPE aHandle, OMX_BUFFERHEADERTYPE* aOmxBufferHeader))
{
    DBGT_PROLOG("Handle: %p OmxBufferHeader: %p PortIndex: %d", aHandle, aOmxBufferHeader, (int)aOmxBufferHeader->nOutputPortIndex);

    DBGT_ASSERT(NULL != aHandle, "Handle is NULL");
    DBGT_ASSERT(NULL != aOmxBufferHeader, "Handle is NULL");
    DBGT_ASSERT(NULL != aFunc, "Func is NULL");

    OmxBuffInfo* buffInfo = static_cast<OmxBuffInfo*>(aOmxBufferHeader->pAppPrivate);
    DBGT_ASSERT(NULL != buffInfo, "Buffinfo is NULL");

    //check buffer not already pushed
    DBGT_ASSERT( !(buffInfo->mFlags & ESentToComp),
               "Handle: %p Buffer: %p already pushed flags: 0x%08x",
               aHandle, aOmxBufferHeader, buffInfo->mFlags);

    //Enable flag
    buffInfo->mFlags |= ESentToComp;

    //push buffer
    OMX_ERRORTYPE err = (*aFunc)(aHandle, aOmxBufferHeader);

    DBGT_PTRACE("mflags = %d  err =%d", buffInfo->mFlags, (int)err);
    //Disable flag in case of error
    if(OMX_ErrorNone != err)
        buffInfo->mFlags &= ~ESentToComp;

	DBGT_EPILOG("");
	return err;
}

#endif // STECAMOMXBUFFINO_INL

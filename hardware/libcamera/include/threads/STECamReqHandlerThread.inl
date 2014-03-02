/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMREQHANDLERTHREAD_INL
#define STECAMREQHANDLERTHREAD_INL

inline ReqHandlerThreadData::ReqHandlerThreadData() :
    mReqHandler(NULL), mArg(NULL),
    mMsg(0), mInfo1(0), mInfo2(0),
    mLock("RequestStatus"), mWaitForRequestToStart(false),
    mFree(true)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

inline void ReqHandlerThreadData::waitForRequestToStart()
{
    DBGT_PROLOG("");

    mRequestStatus.wait(mLock);

    DBGT_EPILOG("");
}

inline void ReqHandlerThreadData::signalRequestStarted()
{
    DBGT_PROLOG("");

    Mutex::Autolock lock(mLock);
    mRequestStatus.signal();

    DBGT_EPILOG("");
}

inline bool ReqHandlerThreadData::updateIfFree()
{
    DBGT_PROLOG("");

    Mutex::Autolock lock(mLock);

    bool free = true;

    //if free, update state to used
    if(mFree)
        mFree = false;
    else
        free = false;

	DBGT_EPILOG("free - %d", free);
	return free;
}

inline ReqHandlerThread<ReqHandlerThreadData>::ReqHandlerThread(STECamera* aParent) :
    mParent(aParent)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

void ReqHandlerThread<ReqHandlerThreadData>::request(ReqHandlerThreadData& aData,
                                                     bool aCheckRequestQueueFull/* = false*/)
{
    DBGT_PROLOG("CheckRequestQueueFull: %d", aCheckRequestQueueFull);

    //Check slot not free, valid request
    DBGT_ASSERT(false == aData.mFree, "Invalid slot requested: %d", aData.mFree);

    //Delegate to base class
    ThreadHandler<ReqHandlerThreadData>::request(aData, aCheckRequestQueueFull);

    //wait for request to start
    if(aData.mWaitForRequestToStart)
        aData.waitForRequestToStart();

    DBGT_EPILOG("");
    return;
}

status_t ReqHandlerThread<ReqHandlerThreadData>::handleRequest(ReqHandlerThreadData& aData)
{
    DBGT_PROLOG("Arg: %p", aData.mArg);

    DBGT_PTRACE("Msg: %d(0x%08x) Info1: %d Info2: %d",
        aData.mMsg, aData.mMsg,
        aData.mInfo1, aData.mInfo2);

    //signal request started
    if(aData.mWaitForRequestToStart)
        aData.signalRequestStarted();

    /* If request handler present, call it and reset.
     * Else dispatch to callback */
    if(aData.mReqHandler){

        //call req handler
        OMX_ERRORTYPE err = (*mParent.*aData.mReqHandler)(aData.mArg);

        //check for error
        if(OMX_ErrorNone != err) {
            aData.mMsg = CAMERA_MSG_ERROR;
            aData.mInfo1 = err;
            //dispatch error
            mParent->doCallbackProcessing(aData);
        }

        //reset handler
        aData.mReqHandler = NULL;
        aData.mArg = NULL;
    }
    else
        mParent->doCallbackProcessing(aData);

    //Slot free now
    aData.mLock.lock();
    aData.mFree = true;
    aData.mWaitForRequestToStart = false;
    aData.mLock.unlock();

	DBGT_EPILOG("");
	return (int)NO_ERROR;
}

#endif // STECAMREQHANDLERTHREAD_INL

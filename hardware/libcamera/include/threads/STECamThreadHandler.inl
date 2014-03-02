/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMTHREADHANDLER_INL
#define STECAMTHREADHANDLER_INL

template <typename T>
inline ThreadHandler<T>::ThreadHandler():
                    mLastRequest(NO_ERROR),
                    mIsThreadActive(false),
                    mCurrentData(NULL),
                    mLock("ThreadHandler::RemoveRequest"),
                    mFlushLock("ThreadHandler::Flush"),
                    mFlushPending(false)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

template<typename T>
ThreadHandler<T>::~ThreadHandler()
{
    DBGT_PROLOG("");

    /* stop in case not done earlier */
    selfDestroy();

    DBGT_EPILOG("");
}

template<typename T>
inline status_t ThreadHandler<T>::init(uint32_t aMaxEvents, bool aSignalCompletion, const char* aName,
                                       int32_t aPriority, size_t aStack)
{
    DBGT_PROLOG("Signal completion: %d", aSignalCompletion);

    DBGT_PTRACE("MaxEvents: %u Name: %s Priority: %d Stack: %zd",
         aMaxEvents, aName, aPriority, aStack);

    //check
    DBGT_ASSERT(0 != aMaxEvents, "Max events is 0");

    status_t err = NO_ERROR;

    err = mQueue.init(aMaxEvents);
	if(NO_ERROR != err) {
		DBGT_CRITICAL("Queue init failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

    /* Semaphore count can be more then queue size in case of flush.
     * Flush sends a dummy event and hence value can be 1 more then
     * queue size.
     */
    err = mRequestSem.init(aMaxEvents+1);
	if(-1 == err) {
		DBGT_CRITICAL("RequestSem init failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

    err = mCompletionSem.init(aMaxEvents);
	if(-1 == err) {
		DBGT_CRITICAL("CompletionSem init failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

    //out standing flush request can only be 1
    err = mFlushSem.init(1);
	if(-1 == err) {
		DBGT_CRITICAL("FlushSem init failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

    //save
    mSignalCompletion = aSignalCompletion;

    //start thread loop
    err = run(aName, aPriority, aStack);
	if(NO_ERROR != err) {
		DBGT_CRITICAL("Thread run failed err = %d", err);
		DBGT_EPILOG("");
		return err;
	}

    //thread active now
    mIsThreadActive = true;
    mLastRequest = NO_ERROR;

	DBGT_EPILOG("");
	return err;
}

template<typename T>
inline void ThreadHandler<T>::selfDestroy()
{
    DBGT_PROLOG("");

    //if thread not active return
    if (!mIsThreadActive) {
        DBGT_EPILOG("");
        return;
    }

    //flush pending events
    flush();

    //request exit
    requestExit();
    mRequestSem.signal();
    requestExitAndWait();

    //thread no more active
    mIsThreadActive = false;

    DBGT_EPILOG("");
    return;
}

template<typename T>
inline status_t ThreadHandler<T>::flush()
{
    DBGT_PROLOG("");

    status_t err = NO_ERROR;

    //check thread active
    DBGT_ASSERT(mIsThreadActive, "Thread not active");

    /* Check no previous flush on-going.
     *
     * This can occur only if threadLoop did not get chance to run
     * and another flush request occurs. Steal all previous signals
     * and perform flush again
     */
    mFlushLock.lock();
    if(mFlushPending){
        //steal previous flush event
        mRequestSem.wait();

        //steal previous flush signal
        mFlushSem.wait();
    }

    //flush on-going
    mFlushPending = true;
    mFlushLock.unlock();

    //signal thread for flush event
    mRequestSem.signal();

    Mutex::Autolock _l(mLock);

    //flush remaining buffers
    while(!mQueue.isEmpty()){

        //wait for event
        mRequestSem.wait();
        //get data
        T* data = mQueue.remove();

        DBGT_PTRACE("Sending data: %p for flush", data);
        mLastRequest = handleFlush(*data);
    }

    //signal flush complete
    mFlushSem.signal();

	DBGT_EPILOG("");
	return (int)mLastRequest;
}

template<typename T>
inline void ThreadHandler<T>::request(T& aData, bool aCheckRequestQueueFull/* = false*/)
{
    DBGT_PROLOG("CheckRequestQueueFull: %d", aCheckRequestQueueFull);

    //if queue full, flush
    if(aCheckRequestQueueFull && mQueue.isFull())
        flush();

    //add request to queue
    mQueue.add(&aData);

    //signal thread
    mRequestSem.signal();

	DBGT_EPILOG("");
    return;
}

template<typename T>
inline status_t ThreadHandler<T>::waitForCompletion()
{
    DBGT_PROLOG("");

    //wait
    mCompletionSem.wait();

    status_t err = mLastRequest;

	DBGT_EPILOG("");
	return (int)mLastRequest;
}

template<typename T>
bool ThreadHandler<T>::threadLoop()
{
    DBGT_PROLOG("ExitPending: %d", exitPending());

    bool runAlways = true;

    //Remove data under lock
    mLock.lock();
    //wait for new reuest
    mRequestSem.wait();

    DBGT_PTRACE("Flushpending: %d", mFlushPending);

    //flush event, ignore
    mFlushLock.lock();
    if(mFlushPending){
        mFlushPending = false;
        mLock.unlock();
        mFlushSem.wait();
        mFlushLock.unlock();
		DBGT_EPILOG("");
		return runAlways;
    }
    mFlushLock.unlock();

    //exit if pending
    if(exitPending()){
        mLock.unlock();
		DBGT_EPILOG("");
		return false;
    }

    //get data
    T* data = mQueue.remove();
    //Request on-going
    mCurrentData = data;

    //lock should be released when request is ongoing
    mLock.unlock();

    //handle request
    mLastRequest = handleRequest(*data);

    //request complete
    mCurrentData = NULL;

    //signal completion
    if(mSignalCompletion)
        mCompletionSem.signal();

	DBGT_EPILOG("");
	return runAlways;
}

template<typename T>
inline status_t ThreadHandler<T>::handleFlush(T& aData)
{
    DBGT_PROLOG("");

	DBGT_EPILOG("");
	return (int)NO_ERROR;
}

template<typename T>
inline volatile T* ThreadHandler<T>::isRequestPending()
{
    DBGT_PROLOG("");

	DBGT_EPILOG("");
	return mCurrentData;
}

#endif // STECAMTHREADHANDLER_INL

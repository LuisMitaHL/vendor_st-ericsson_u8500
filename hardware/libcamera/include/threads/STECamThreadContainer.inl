/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMTHREADCONTAINER_INL
#define STECAMTHREADCONTAINER_INL

template <typename T>
inline ThreadContainer<T>::ThreadContainer(ThreadHandler<T>* aThreadHandler):
                          mThreadHandler(aThreadHandler)
{
    DBGT_PROLOG("");

    DBGT_ASSERT(mThreadHandler != 0, "ThreadHandler is NULL");

    DBGT_EPILOG("");
}

template<typename T>
ThreadContainer<T>::~ThreadContainer()
{
    DBGT_PROLOG("");

    sp<ThreadHandler<T> > threadHandler;
    threadHandler = mThreadHandler;
    threadHandler->selfDestroy();
    mThreadHandler.clear();

    DBGT_EPILOG("");
}

template<typename T>
inline ThreadHandler<T>* ThreadContainer<T>::operator-> () const
{
    DBGT_ASSERT(mThreadHandler != 0, "ThreadHandler is NULL");
    return mThreadHandler.get();
}

template<typename T>
inline ThreadHandler<T>* ThreadContainer<T>::get () const
{
    DBGT_ASSERT(mThreadHandler != 0, "ThreadHandler is NULL");
    return mThreadHandler.get();
}

#endif // STECAMTHREADCONTAINER_INL

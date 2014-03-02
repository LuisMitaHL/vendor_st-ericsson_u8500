/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMQUEUE_INL
#define STECAMQUEUE_INL

template <typename T>
inline Queue<T>::Queue() : mData(NULL),
                           mLock("Queue::Lock"),
                           mCount(0),
                           mMaxCount(0),
                           mInsertIndex(-1),
                           mRemoveIndex(-1)
{
    DBGT_PROLOG("");
    DBGT_EPILOG("");

}

template<typename T>
inline Queue<T>::~Queue()
{
    DBGT_PROLOG("");
    delete [] mData;
    DBGT_EPILOG("");
}

template<typename T>
inline bool Queue<T>::isEmpty()
{
    DBGT_PDEBUG("Count: %u", mCount);
    Mutex::Autolock lock(mLock);
    return (0 == mCount);
}

template<typename T>
inline bool Queue<T>::isFull()
{
    Mutex::Autolock lock(mLock);
    if (mMaxCount == mCount){
        DBGT_PDEBUG("Queue is full");
    }

    return (mMaxCount == mCount);
}

template<typename T>
status_t Queue<T>::init(uint32_t aLength)
{
    DBGT_PDEBUG("Length: %u", aLength);

    status_t err = NO_ERROR;

    DBGT_ASSERT(0 != aLength, "Length is 0");

    //delete previous if any
    delete [] mData;

    //alocate memory
    mData = new T*[aLength];
    if(NULL == mData)
    {
        err = NO_MEMORY;
        if(err != NO_ERROR){
            DBGT_WARNING("err %x",err);
        }
        return err;
    }

    //reset all varaibles
    mCount = 0;
    mMaxCount = aLength;
    mInsertIndex = -1;
    mRemoveIndex = -1;
    for(uint32_t loop = 0; loop < mMaxCount; loop++)
        mData[loop] = NULL;

    if(err != NO_ERROR){
        DBGT_WARNING("err %x",err);
    }
    return err;
}

template<typename T>
void Queue<T>::add(T* const aElem)
{
    DBGT_PDEBUG("Elem: %p", aElem);

    DBGT_ASSERT(NULL != aElem, "Elem is NULL");
    DBGT_ASSERT(mMaxCount > 0, "MaxCount is invalid: %u", mMaxCount);

    Mutex::Autolock lock(mLock);

    //increment count
    mCount++;

    //update index in circular fashion
    mInsertIndex = (mInsertIndex+1)%mMaxCount;

    //check index is less then max
    DBGT_ASSERT((int32_t)mMaxCount > mInsertIndex,
               "InsertIndex: %d is more then Maxcount: %u",
               mInsertIndex, mMaxCount);
    //check data not already present
    DBGT_ASSERT(NULL == mData[mInsertIndex],
               "Data: %p already present at: %d",
               mData[mInsertIndex], mInsertIndex);
    //check max limit reached
    DBGT_ASSERT(mMaxCount >=  mCount,
               "Count: %u more then MaxCount: %u",
               mCount, mMaxCount);

    //put data
    mData[mInsertIndex] = aElem;
}

template<typename T>
T* Queue<T>::remove()
{
    Mutex::Autolock lock(mLock);

    //decrement count
    mCount--;

    //update index in circular fashion
    mRemoveIndex = (mRemoveIndex+1)%mMaxCount;

    //check index is less then max
    DBGT_ASSERT((int32_t)mMaxCount > mRemoveIndex,
               "RemoveIndex: %d is more then Maxcount: %u",
               mRemoveIndex, mMaxCount);
    //check data present
    DBGT_ASSERT(NULL != mData[mRemoveIndex],
               "Data not present at: %d", mRemoveIndex);
    //check count valid
    DBGT_ASSERT(mMaxCount >= mCount,
               "MaxCount: %u less then Count: %u", mMaxCount, mCount);

    //get data
    T* data = mData[mRemoveIndex];
    mData[mRemoveIndex] = NULL;

    //check data not null
    DBGT_ASSERT(NULL != data, "Data is NULL");

    DBGT_PDEBUG("Elem: %p", data);
    return data;
}

#endif // STECAMQUEUE_INL

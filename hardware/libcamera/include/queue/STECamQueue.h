/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMQUEUE_H
#define STECAMQUEUE_H

//System include
#include <stdint.h>
#include <utils/threads.h>

namespace android {

//Thread-safe queue
template <typename T>
class Queue
    {
    public:
        /* CTOR */
        inline Queue();

        /* DTOR */
        inline ~Queue();

        /* Check if empty */
        inline bool isEmpty();

        /* Check if queue full */
        inline bool isFull();
        /* Init */
        inline status_t init(uint32_t aLength);

        /* Add */
        inline void add(T* const aElem);

        /* Remove */
        inline T* remove();

    private:
        T** mData; /**< Data */
        Mutex mLock; /**< Lock */
        uint32_t mCount; /**< Count */
        uint32_t mMaxCount; /**< Max Count */
        int32_t mInsertIndex; /**< Insert index */
        int32_t mRemoveIndex; /**< Remove index */
    };

#include "STECamQueue.inl"

}

#endif // STECAMQUEUE_H


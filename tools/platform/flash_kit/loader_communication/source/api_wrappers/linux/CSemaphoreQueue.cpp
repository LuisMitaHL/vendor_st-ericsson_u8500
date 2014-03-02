/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include <pthread.h>
#include <assert.h>
#include "Types.h"
#include "CEventObject.h"
#include "CSemaphore.h"
#include "CSemaphoreQueue.h"

CSemaphoreQueue::CSemaphoreQueue(unsigned int MaxCount) : m_MaximumCount(MaxCount)
{
    m_pEventObject = new CEventObject();
#if defined(__APPLE__)
    m_pSemaphore = new CSemaphore(0);
#elif defined(__linux__)
    m_pSemaphore = new CSemaphore();
#endif

    m_ObjectCollection.Add(m_pEventObject);
    m_ObjectCollection.Add(m_pSemaphore);

    m_Queue = new void*[MaxCount];
    m_CurrentHead = 0;
    m_CurrentTail = 0;
    m_CurrentCount = 0;
}

CSemaphoreQueue::~CSemaphoreQueue()
{
    delete m_pEventObject;
    delete m_pSemaphore;
    delete[] m_Queue;
}

bool CSemaphoreQueue::AddTail(void *pObject)
{
    bool result = false;
    CLockCS LocalCSLock(m_CSLock);
    AddToQueueTail(pObject);
    result = m_pSemaphore->Release(1);

    if (!result) {
        // Error : use ::GetLastError for cause of error
        RemoveFromQueueTail(); // Not really necessary but keep,
        // everything's gone pear-shaped anyway
    }

    return result;
}

RemoveResult CSemaphoreQueue::RemoveHead(void **ppObject, size_t mSecTimeout)
{
    CWaitableObject *pWaitableObject = m_ObjectCollection.Wait(mSecTimeout);

    if (pWaitableObject == m_pEventObject) {
        return REMOVE_WAIT_FOR_SEMAPHORE;
    } else if (pWaitableObject == m_pSemaphore) {
        CLockCS LocalCSLock(m_CSLock);
        *ppObject = RemoveFromQueueHead(); // Remove pObject from pObjectQueue head
        return REMOVE_SUCCESS;
    } else if (NULL == pWaitableObject) {
        return REMOVE_TIMEOUT;;
    } else {
        // Should never occur
        assert(false);

        return REMOVE_CANCEL;;
    }
}

void CSemaphoreQueue::SignalEvent()
{
    CEventObject *pEvent = m_pEventObject;
    pEvent->SetEvent();
}

void CSemaphoreQueue::IncrementHead()
{
    ++m_CurrentHead;

    if (m_CurrentHead == m_MaximumCount) {
        m_CurrentHead = 0;
    }
}

void CSemaphoreQueue::IncrementTail()
{
    ++m_CurrentTail;

    if (m_CurrentTail == m_MaximumCount) {
        m_CurrentTail = 0;
    }
}

void CSemaphoreQueue::AddToQueueTail(void *pObject)
{
    m_Queue[m_CurrentTail] = pObject;
    IncrementTail();
}

void *CSemaphoreQueue::RemoveFromQueueHead()
{
    void *Object;

    Object = m_Queue[m_CurrentHead];
    IncrementHead();
    return Object;
}

// Next 2 functions not really necessary - for error case only
void CSemaphoreQueue::DecrementTail()
{
    if (m_CurrentTail == 0) {
        m_CurrentTail = m_MaximumCount - 1;
    } else {
        --m_CurrentTail;
    }
}

void CSemaphoreQueue::RemoveFromQueueTail()
{
    DecrementTail();
    m_Queue[m_CurrentTail] = 0;
}

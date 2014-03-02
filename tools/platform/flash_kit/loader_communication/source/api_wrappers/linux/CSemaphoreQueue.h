/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _CSEMAPHOREQUEUE_H
#define _CSEMAPHOREQUEUE_H
#include "Types.h"
#include "CEventObject.h"
#include "CSemaphore.h"
#include "pthread.h"
#include "CCriticalSection.h"
#include "CWaitableObject.h"
#include "CWaitableObjectCollection.h"

class CSemaphore;

enum RemoveResult {
    REMOVE_SUCCESS,
    REMOVE_TIMEOUT,
    REMOVE_CANCEL,
    REMOVE_WAIT_FOR_SEMAPHORE
};

// An object queue implementation using WinAPI's Semaphore.
// For multi-threaded multi-processor applications with multiple producers and multiple consumers
class CSemaphoreQueue
{
public:
    CSemaphoreQueue(unsigned int MaxCount);
    ~CSemaphoreQueue();
    bool AddTail(void *pObject);
    RemoveResult RemoveHead(void **ppRequest, size_t mSecTimeout);
    void SignalEvent();
private:
    CEventObject *m_pEventObject;
    CSemaphore *m_pSemaphore;
    CCriticalSectionObject m_CSLock;
    void **m_Queue;
    const unsigned int m_MaximumCount;
    unsigned int m_CurrentCount;
    unsigned int m_CurrentHead;
    unsigned int m_CurrentTail;
    void IncrementHead();
    void IncrementTail();
    void AddToQueueTail(void *pObject);
    void *RemoveFromQueueHead();
    unsigned int GetIndex(CWaitableObject *Object) const;
    // Next 2 functions not really necessary - for error case only
    void DecrementTail();
    void RemoveFromQueueTail();
    CWaitableObjectCollection m_ObjectCollection;
};

class CPollQueue : public CSemaphoreQueue
{
public:
    CPollQueue() : CSemaphoreQueue(256) {}
    RemoveResult RemovePollRequest(void **pTO, int mSecTimeout = INFINITE) {
        return RemoveHead(pTO, mSecTimeout);
    }
    bool AddPollRequest(void *pObject) {
        return AddTail(pObject);
    }
};

#endif /* _CSEMAPHOREQUEUE_H */


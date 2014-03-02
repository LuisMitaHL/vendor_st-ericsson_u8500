/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/*
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup queue
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "Queue.h"
#include "LCDriverMethods.h"
#include <cstdlib>
#include <cstring>

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

typedef struct Queue_s {
    /* Head index of the buffer. This is where items are inserted.*/
    uint32 Head;
    /* Tail index of the buffer. This is where items are removed.*/
    uint32 Tail;
    /* Size of the buffer.*/
    uint32 Size;
    /* Buffer pointer.*/
    void **Buffer_pp;
    /* Empty buffer callback pointer. */
    void (*EmptyCallback)(const void *const Queue_p, void *Param_p);
    /* The parameter is used for the empty buffer transition callback.*/
    void *EmptyParam_p;
    /* NonEmpty buffer callback pointer. */
    void (*NonEmptyCallback)(const void *const Queue_p, void *Param_p);
    /* The parameter used for the non-empty buffer transition callback.*/
    void *NonEmptyParam_p;
    /* Pointer to a function for deallocating any resources that are reserved for
     * the element. This function is provided as an input parameter when the queue
     * is created.
     */
    void (*DestroyElement)(void *Value_p);
} Queue_t;

Queue::Queue(void)
{
    logger_ = NULL;
}

Queue::~Queue(void)
{
}

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
void Queue::Create(void **const Queue_pp, const uint32 MaxLength, void (*DestroyElement)(void *Element_p))
{
    Queue_t *Queue_p = new Queue_t;

    if (Queue_p != 0) {
        Queue_p->Size = MaxLength;
        Queue_p->Head = 0;
        Queue_p->Tail = 0;
        Queue_p->Buffer_pp = new void*[MaxLength + 1];
        Queue_p->DestroyElement = DestroyElement;
        Queue_p->NonEmptyCallback = 0;
        Queue_p->NonEmptyParam_p = 0;
        Queue_p->EmptyCallback = 0;
        Queue_p->EmptyParam_p = 0;

        *Queue_pp = Queue_p;

#ifdef _QUEUEDEBUG
        PrintF("Queue: Create - Queue 0x%p", Queue_p);
#endif
    } else {
#ifdef _QUEUEDEBUG
        PrintF("Queue: Create - Not enough memory Queue 0x%p", 0);
#endif
    }
}

void Queue::Destroy(void **const Queue_pp)
{
    Queue_t *Queue_p = (Queue_t *) * Queue_pp;
    void *Object_p = 0;

    if (NULL != Queue_p->DestroyElement) {
        Object_p = Dequeue(Queue_p);

        while (0 != Object_p) {
            Queue_p->DestroyElement(Object_p);
            Object_p = Dequeue(Queue_p);
        }
    }

#ifdef _QUEUEDEBUG
    PrintF("Queue: Destroy - Queue 0x%p", Queue_p);
#endif

    delete[] Queue_p->Buffer_pp;
    delete Queue_p;
    *Queue_pp = NULL;
}

ErrorCode_e Queue::Enqueue(void *const Queue_p, void *const Object_p)
{
    Queue_t *TempQueue_p = (Queue_t *)Queue_p;
    uint32 Head = (TempQueue_p->Head + 1) % TempQueue_p->Size;
    boolean NonEmptyCall = (TempQueue_p->Head == TempQueue_p->Tail);

    if (Head != TempQueue_p->Tail) {
#ifdef _QUEUEDEBUG
        PrintF("Queue: Enqueue - Queue 0x%p", Queue_p);
        PrintF("Queue: Enqueue - Object 0x%p", Object_p);
#endif
        TempQueue_p->Buffer_pp[Head] = Object_p;
        TempQueue_p->Head = Head;

        if (Head == TempQueue_p->Tail) {
#ifdef _QUEUEDEBUG
            PrintF("Queue: Enqueue - return E_FAILED_TO_STORE_IN_FIFO, Object 0x%p", Object_p);
#endif
            return E_FAILED_TO_STORE_IN_FIFO;
        }

        if (NonEmptyCall && NULL != TempQueue_p->NonEmptyCallback) {
#ifdef _QUEUEDEBUG
            PrintF("Queue: Enqueue - Executing NonEmptyCallback Queue 0x%p", Queue_p);
#endif
            TempQueue_p->NonEmptyCallback(TempQueue_p, TempQueue_p->NonEmptyParam_p);
        }

        return E_SUCCESS;
    } else {
#ifdef _QUEUEDEBUG
        PrintF("Queue: Enqueue - return E_FAILED_TO_STORE_IN_FIFO, Object 0x%p", Object_p);
#endif
        return E_FAILED_TO_STORE_IN_FIFO;
    }
}

void *Queue::Dequeue(void *const Queue_p)
{
    Queue_t *TempQueue_p = (Queue_t *)Queue_p;
    uint32 Tail = TempQueue_p->Tail;
    void *Object_p = NULL;

    if (Tail != TempQueue_p->Head) {
#ifdef _QUEUEDEBUG
        PrintF("Queue: Dequeue - Queue 0x%p", Queue_p);
#endif
        Tail = (Tail + 1) % TempQueue_p->Size;
        Object_p = TempQueue_p->Buffer_pp[Tail];
        TempQueue_p->Tail = Tail;

        if (Tail == TempQueue_p->Head && NULL != TempQueue_p->EmptyCallback) {
#ifdef _QUEUEDEBUG
            PrintF("Queue: Dequeue - Executing EmptyCallback Queue 0x%p", Queue_p);
#endif
            TempQueue_p->EmptyCallback(TempQueue_p, TempQueue_p->EmptyParam_p);
        }

#ifdef _QUEUEDEBUG
        PrintF("Queue: Dequeue - Object 0x%p", Object_p);
#endif
    }

    return Object_p;
}

QueueCallback_fn Queue::SetCallback(void *const Queue_p,
                                    const QueueCallbackType_e Type,
                                    const QueueCallback_fn Callback,
                                    void *const Param_p)
{
    Queue_t *TempQueue_p = (Queue_t *)Queue_p;
    QueueCallback_fn OldCallback;

    if (Type == QUEUE_EMPTY) {
        OldCallback = TempQueue_p->EmptyCallback;
        TempQueue_p->EmptyCallback = Callback;
        TempQueue_p->EmptyParam_p = Param_p;
    } else {
        OldCallback = TempQueue_p->NonEmptyCallback;
        TempQueue_p->NonEmptyCallback = Callback;
        TempQueue_p->NonEmptyParam_p = Param_p;
    }

    return OldCallback;
}

boolean Queue::IsEmpty(const void *const Queue_p)
{
    Queue_t *TempQueue_p = (Queue_t *)Queue_p;

    if (TempQueue_p->Tail == TempQueue_p->Head) {
        return TRUE;
    } else {
        return FALSE;
    }
}

boolean Queue::IsMember(const void *const Queue_p, void *Value_p, boolean(*Match)(void *Value1_p, void *Value2_p))
{
    Queue_t *TempQueue_p = (Queue_t *)Queue_p;

    for (size_t i = TempQueue_p->Head; i < TempQueue_p->Tail; i++) {
        if (Match(Value_p, TempQueue_p->Buffer_pp[i])) {
            return TRUE;
        }
    }

    return FALSE;
}

int Queue::GetNrOfElements(const void *const Queue_p)
{
    Queue_t *TempQueue_p = (Queue_t *)Queue_p;
    int Return = 0;

    if (TempQueue_p->Head >= TempQueue_p->Tail) {
        Return = TempQueue_p->Head - TempQueue_p->Tail;
    } else {
        Return = TempQueue_p->Size - (TempQueue_p->Tail - TempQueue_p->Head);
    }

    return Return;
}

void Queue::RCreate(void **const Queue_pp, const uint32 MaxLength, void (*DestroyElement)(void *))
{
    CLockCS QueueLock(QueueCSO);
    Create(Queue_pp, MaxLength, DestroyElement);
}

void Queue::RDestroy(void **const Queue_pp)
{
    CLockCS QueueLock(QueueCSO);
    Destroy(Queue_pp);
}

ErrorCode_e Queue::REnqueue(void *const Queue_p, void *const Value_p)
{
    CLockCS QueueLock(QueueCSO);
    return Enqueue(Queue_p, Value_p);
}

void *Queue::RDequeue(void *const Queue_p)
{
    CLockCS QueueLock(QueueCSO);
    return Dequeue(Queue_p);
}

QueueCallback_fn Queue::RSetCallback(void *const Queue_p,
                                     const QueueCallbackType_e Type,
                                     const QueueCallback_fn Callback,
                                     void *const Param_p)
{
    CLockCS QueueLock(QueueCSO);
    return SetCallback(Queue_p, Type, Callback, Param_p);
}

boolean Queue::RIsEmpty(const void *const Queue_p)
{
    CLockCS QueueLock(QueueCSO);
    return IsEmpty(Queue_p);
}

boolean Queue::RIsMember(const void *const Queue_p, void *Value_p, boolean(*Match)(void *Value1_p, void *Value2_p))
{
    CLockCS QueueLock(QueueCSO);
    return IsMember(Queue_p, Value_p, Match);
}

int Queue::RGetNrOfElements(const void *const Queue_p)
{
    CLockCS QueueLock(QueueCSO);
    return GetNrOfElements(Queue_p);
}

void Queue::PrintF(const char *text, void *pVoid)
{
    if (NULL != logger_) {
        logger_->log(text, pVoid);
    }
}

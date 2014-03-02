/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
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
#include "r_queue.h"
#include <string.h>
#include <stdlib.h>
#include "r_debug_macro.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

typedef struct Queue_s {
    /** Head index of the buffer. This is where items are inserted. */
    uint32 Head;
    /** Tail index of the buffer. This is where items are removed. */
    uint32 Tail;
    /** Size of the buffer. */
    uint32 Size;
    /** Buffer pointer.v*/
    void **Buffer_pp;
    /** Empty buffer callback pointer. */
    void (*EmptyCallback)(const void *const Queue_p, void *Param_p);
    /** The parameter is used for the empty buffer transition callback. */
    void *EmptyParam_p;
    /** NonEmpty buffer callback pointer. */
    void (*NonEmptyCallback)(const void *const Queue_p, void *Param_p);
    /** The parameter used for the non-empty buffer transition callback. */
    void *NonEmptyParam_p;
    /**
     * Pointer to a function for deallocating any resources that are reserved for
     * the element. This function is provided as an input parameter when the queue
     * is created.
     */
    void (*DestroyElement)(void *Value_p);
} Queue_t;

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

void Do_Fifo_Create(void *Object_p, void **const Queue_pp, const uint32 MaxLength, void (*DestroyElement)(void *Element_p))
{
    *Queue_pp = (Queue_t *) malloc(sizeof(Queue_t));
    ASSERT(NULL != (*Queue_pp));
    memset(*Queue_pp, 0, sizeof(Queue_t));
    ((Queue_t *)*Queue_pp)->Size = MaxLength;
    ((Queue_t *)*Queue_pp)->Buffer_pp = (void **)malloc(MaxLength * sizeof(void *) + 1);
    ASSERT(NULL != (((Queue_t *)*Queue_pp)->Buffer_pp));
    ((Queue_t *)*Queue_pp)->DestroyElement = DestroyElement;
}


void Do_Fifo_Destroy(void *Object_p, void **const Queue_pp)
{
    Queue_t *Queue_p = (Queue_t *) * Queue_pp;
    uint32 i = 0;

    if (NULL != Queue_p->DestroyElement) {
        if (Queue_p->Head >= Queue_p->Tail) {
            for (i = Queue_p->Head; i < Queue_p->Tail; i++) {
                Queue_p->DestroyElement(Queue_p->Buffer_pp[i]);
            }
        } else {
            for (i = Queue_p->Tail; i < Queue_p->Size; i++) {
                Queue_p->DestroyElement(Queue_p->Buffer_pp[i]);
            }

            for (i = 0; i < Queue_p->Head; i++) {
                Queue_p->DestroyElement(Queue_p->Buffer_pp[i]);
            }
        }
    }

    BUFFER_FREE(Queue_p->Buffer_pp);
    BUFFER_FREE(*Queue_pp);
}


ErrorCode_e Do_Fifo_Enqueue(void *Object_p, void *const Queue_p, void *const Value_p)
{
    /* Note that the ordering of statements are important. The compiler
     * must guarantee the assignments to be atomic (which is the case
     * with properly aligned 32 bit values).
     */
    Queue_t *TempQueue_p = (Queue_t *)Queue_p;

    uint32 Head = (TempQueue_p->Head + 1) % TempQueue_p->Size;

    boolean NonEmptyCall = (TempQueue_p->Head == TempQueue_p->Tail);

    if (Head != TempQueue_p->Tail) {
        TempQueue_p->Buffer_pp[Head] = Value_p;
        TempQueue_p->Head = Head;

        if (NonEmptyCall && NULL != TempQueue_p->NonEmptyCallback) {
            TempQueue_p->NonEmptyCallback(TempQueue_p, TempQueue_p->NonEmptyParam_p);
        }

        return E_SUCCESS;
    } else {
        return E_FAILED_TO_STORE_IN_FIFO;
    }
}


void *Do_Fifo_Dequeue(void *Object_p, void *const Queue_p)
{
    Queue_t *TempQueue_p = (Queue_t *) Queue_p;
    uint32 Tail = TempQueue_p->Tail;
    void *Value_p = NULL;

    if (Tail != TempQueue_p->Head) {
        Tail = (Tail + 1) % TempQueue_p->Size;
        Value_p = TempQueue_p->Buffer_pp[Tail];
        TempQueue_p->Tail = Tail;

        if (Tail == TempQueue_p->Head && NULL != TempQueue_p->EmptyCallback) {
            TempQueue_p->EmptyCallback(TempQueue_p, TempQueue_p->EmptyParam_p);
        }
    }

    return Value_p;
}


QueueCallback_fn Do_Fifo_SetCallback(void *Object_p, void *const Queue_p,
                                     const QueueCallbackType_e Type,
                                     const QueueCallback_fn Callback,
                                     void *const Param_p)
{
    Queue_t *TempQueue_p = (Queue_t *) Queue_p;
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


boolean Do_Fifo_IsEmpty(void *Object_p, const void *const Queue_p)
{
    Queue_t *TempQueue_p = (Queue_t *) Queue_p;

    if (TempQueue_p->Tail == TempQueue_p->Head) {
        return TRUE;
    } else {
        return FALSE;
    }
}


boolean Do_Fifo_IsMember(void *Object_p, const void *const Queue_p, void *Value_p, boolean(*Match)(void *Value1_p, void *Value2_p))
{
    Queue_t *TempQueue_p = (Queue_t *) Queue_p;
    uint32 i = 0;

    for (i = TempQueue_p->Head; i < TempQueue_p->Tail; i++) {
        if (Match(Value_p, TempQueue_p->Buffer_pp[i])) {
            return TRUE;
        }
    }

    return FALSE;
}

int Do_Fifo_GetNrOfElements(void *Object_p, const void *const Queue_p)
{
    Queue_t *TempQueue_p = (Queue_t *)Queue_p;

    if (TempQueue_p->Head >= TempQueue_p->Tail) {
        return (TempQueue_p->Head - TempQueue_p->Tail);
    }

    return (TempQueue_p->Size - (TempQueue_p->Tail - TempQueue_p->Head));
}

void Do_RFifo_Create(void *Object_p, void **const Queue_pp, const uint32 MaxLength, void (*DestroyElement)(void *Element_p))
{
    //DisableInterrupts();
    Do_Fifo_Create(Object_p, Queue_pp, MaxLength, DestroyElement);
    //EnableInterupts();
}

void Do_RFifo_Destroy(void *Object_p, void **const Queue_pp)
{
    //DisableInterrupts();
    Do_Fifo_Destroy(Object_p, Queue_pp);
    //EnableInterrupts();
}

ErrorCode_e Do_RFifo_Enqueue(void *Object_p, void *const Queue_p, void *const Value_p)
{
    //DisableInterrupts();
    return Do_Fifo_Enqueue(Object_p, Queue_p, Value_p);
    //EnableInterrupts();
}

void *Do_RFifo_Dequeue(void *Object_p, void *const Queue_p)
{
    //DisableInterrupts();
    return Do_Fifo_Dequeue(Object_p, Queue_p);
    //EnableInterrupts();
}

QueueCallback_fn Do_RFifo_SetCallback(void *Object_p, void *const Queue_p,
                                      const QueueCallbackType_e Type,
                                      const QueueCallback_fn Callback,
                                      void *const Param_p)
{
    //DisableInterrupts();
    return Do_Fifo_SetCallback(Object_p, Queue_p, Type, Callback, Param_p);
    //EnableInterrupts();
}

boolean Do_RFifo_IsEmpty(void *Object_p, const void *const Queue_p)
{
    //DisableInterrupts();
    return Do_Fifo_IsEmpty(Object_p, Queue_p);
    //EnableInterrupts();
}

boolean Do_RFifo_IsMember(void *Object_p, const void *const Queue_p, void *Value_p, boolean(*Match)(void *Value1_p, void *Value2_p))
{
    //DisableInterrupts();
    return Do_Fifo_IsMember(Object_p, Queue_p, Value_p, Match);
    //EnableInterrupts();
}

int Do_RFifo_GetNrOfElements(void *Object_p, const void *const Queue_p)
{
    //DisableInterrupts();
    return Do_Fifo_GetNrOfElements(Object_p, Queue_p);
    //EnableInterrupts();
}

/* @} */
/* @} */

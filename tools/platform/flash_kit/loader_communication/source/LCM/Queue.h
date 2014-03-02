/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "t_queue.h"
#include "error_codes.h"
#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include <stdio.h>
#include "LinuxApiWrappers.h"
#endif
#include "Logger.h"

class Queue
{
public:
    Queue(void);
    ~Queue(void);

    void Create(void **const Queue_pp, const uint32 MaxLength, void (*DestroyElement)(void *Element_p));
    void Destroy(void **const Queue_pp);
    ErrorCode_e Enqueue(void *const Queue_p, void *const Value_p);
    void *Dequeue(void *const Queue_p);
    QueueCallback_fn SetCallback(void *const Queue_p, const QueueCallbackType_e Type, const QueueCallback_fn Callback, void *const Param_p);
    boolean IsEmpty(const void *const Queue_p);
    boolean IsMember(const void *const Queue_p, void *Value_p, boolean(*Match)(void *Value1_p, void *Value2_p));
    int GetNrOfElements(const void *const Queue_p);

    //Reentrant
    void RCreate(void **const Queue_pp, const uint32 MaxLength, void (*DestroyElement)(void *Element_p));
    void RDestroy(void **const Queue_pp);
    ErrorCode_e REnqueue(void *const Queue_p, void *const Value_p);
    void *RDequeue(void *const Queue_p);
    QueueCallback_fn RSetCallback(void *const Queue_p, const QueueCallbackType_e Type, const QueueCallback_fn Callback, void *const Param_p);
    boolean RIsEmpty(const void *const Queue_p);
    boolean RIsMember(const void *const Queue_p, void *Value_p, boolean(*Match)(void *Value1_p, void *Value2_p));
    int RGetNrOfElements(const void *const Queue_p);

    void SetLogger(Logger *logger) {
        logger_ = logger;
    }
private:
    CCriticalSectionObject QueueCSO;
    Logger *logger_;
    void PrintF(const char *text, void *pVoid);
};

#endif // _QUEUE_H_

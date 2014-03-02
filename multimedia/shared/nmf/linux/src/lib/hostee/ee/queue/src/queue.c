/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <ee/queue.nmf>

typedef t_bool (*nmfQueueMatchingFunction)(t_queue_link *pRes, void *pArgs);
typedef void (*nmfQueueExecuteFunction)(t_queue_link *pRes);
typedef void (*nmfQueueExecuteFunctionWithArgs)(t_queue_link *pRes, void *pArgs);

typedef struct {
    hMutex mutex;
    t_queue_link *pFIRST;
    t_queue_link *pLAST;
} t_queue_description;

/* implement ee.api.queue.itf */
EXPORT_SHARED t_queue nmfQueueCreate()
{
    t_queue_description *pRes;

    pRes = (t_queue_description *) allocator.alloc(sizeof(t_queue_description));
    if (pRes != 0)
    {
        pRes->mutex = eeMutexCreate();
        if (pRes->mutex == 0) {
            allocator.free(pRes);
            pRes = 0;
        } else {
            pRes->pFIRST = 0;
            pRes->pLAST = 0;
        }
    }

    return (t_queue)pRes;
}

EXPORT_SHARED void nmfQueueDestroy(t_queue _pQueue)
{
    t_queue_description* pQueue = (t_queue_description*)_pQueue;

    eeMutexDestroy(pQueue->mutex);
    allocator.free((void *) pQueue);
}

EXPORT_SHARED void nmfQueuePush(t_queue _pQueue, t_queue_link *pElem)
{
    nmfQueuePushAndExecute(_pQueue, pElem, 0);
}

void nmfQueuePushAndExecute(t_queue _pQueue, t_queue_link *pElem, void *pExecuteFunction)
{
    t_queue_description* pQueue = (t_queue_description*)_pQueue;

    eeMutexLock(pQueue->mutex);
    METH(pushAndExecute)(_pQueue, pElem, pExecuteFunction);
    eeMutexUnlock(pQueue->mutex);
}

EXPORT_SHARED t_queue_link *nmfQueuePop(t_queue _pQueue)
{
    t_queue_description* pQueue = (t_queue_description*)_pQueue;
    t_queue_link *pRes;

    eeMutexLock(pQueue->mutex);
    pRes = pQueue->pFIRST;
    if (pRes != 0)
    {
        pQueue->pFIRST = pRes->pNext;
        if (pQueue->pFIRST == 0) {pQueue->pLAST = 0;}
        pRes->pNext = 0;//not necessary but easier to detect error
    }
    eeMutexUnlock(pQueue->mutex);

    return pRes;
}

t_queue_link *nmfQueuePopMatching(t_queue _pQueue, void* pMatchingFunction, void* pMatchingFunctionArgs)
{
    return nmfQueuePopMatchingAndExecute(_pQueue, pMatchingFunction, pMatchingFunctionArgs, 0, 0);
}

t_queue_link *nmfQueuePopMatchingAndExecute(t_queue _pQueue, void* pMatchingFunction, void* pMatchingFunctionArgs, void *pExecuteFunction, void *pExecuteFunctionArgs)
{
    t_queue_description* pQueue = (t_queue_description*)_pQueue;
    t_queue_link *pRes;

    eeMutexLock(pQueue->mutex);
    pRes = METH(popMatchingAndExecute)(_pQueue, pMatchingFunction, pMatchingFunctionArgs, pExecuteFunction, pExecuteFunctionArgs);
    eeMutexUnlock(pQueue->mutex);

    return pRes;
}

void nmfQueueLock(t_queue _pQueue)
{
    t_queue_description* pQueue = (t_queue_description*)_pQueue;
    
    eeMutexLock(pQueue->mutex);
}

void nmfQueueUnlock(t_queue _pQueue)
{
    t_queue_description* pQueue = (t_queue_description*)_pQueue;
    
    eeMutexUnlock(pQueue->mutex);
}

/* implement ee.api.queue.nolock.itf */
t_queue METH(create)()
{
    t_queue_description *pRes;

    pRes = (t_queue_description *) allocator.alloc(sizeof(t_queue_description));
    if (pRes != 0)
    {
        pRes->mutex = 0;
		pRes->pFIRST = 0;
		pRes->pLAST = 0;
    }

    return (t_queue)pRes;
}

void METH(destroy)(t_queue _pQueue)
{
	t_queue_description* pQueue = (t_queue_description*)_pQueue;

	allocator.free((void *) pQueue);
}

void METH(pushAndExecute)(t_queue _pQueue, t_queue_link *pElem, void *pExecuteFunction)
{
    t_queue_description* pQueue = (t_queue_description*)_pQueue;
    nmfQueueExecuteFunction executeFunction = (nmfQueueExecuteFunction) pExecuteFunction;

    if (pQueue->pFIRST == 0) {pQueue->pFIRST = pElem;}
    else {pQueue->pLAST->pNext = pElem;}
    pElem->pNext = 0;
    pQueue->pLAST = pElem;
    if (executeFunction != 0) {(*executeFunction)(pElem);}
}

t_queue_link *METH(popMatchingAndExecute)(t_queue _pQueue, void* pMatchingFunction, void* pMatchingFunctionArgs, void *pExecuteFunction, void *pExecuteFunctionArgs)
{
    t_queue_description* pQueue = (t_queue_description*)_pQueue;
    t_queue_link *pRes;
    t_queue_link *pPrev = NULL;
    nmfQueueMatchingFunction matchingFunction = (nmfQueueMatchingFunction) pMatchingFunction;
    nmfQueueExecuteFunctionWithArgs executeFunction = (nmfQueueExecuteFunctionWithArgs) pExecuteFunction;

    pRes = pQueue->pFIRST;
    /* find matching elem */
    while(pRes != NULL)
    {
        if ((*matchingFunction)(pRes, pMatchingFunctionArgs))
        {
            /* remove it */
            if (pPrev == NULL) {pQueue->pFIRST = pRes->pNext;} // First one
            else if (pRes->pNext == NULL) {pPrev->pNext = 0; pQueue->pLAST = pPrev;} // Last one
            else {pPrev->pNext = pRes->pNext;}
            pRes->pNext = 0;//not necessary but easier to detect error

            break;
        }
        pPrev = pRes;
        pRes = pRes->pNext;
    }

    if (executeFunction != 0) {(*executeFunction)(pRes, pExecuteFunctionArgs);}

    return pRes;
}



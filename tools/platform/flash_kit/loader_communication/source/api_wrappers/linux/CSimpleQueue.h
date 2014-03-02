/******************************************************************************
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
******************************************************************************/

#ifndef _CSIMPLEQUEUE_H_
#define _CSIMPLEQUEUE_H_

#include "Types.h"
#include "CSemaphoreQueue.h"

class CSimpleQueue : public CSemaphoreQueue
{
public:
    CSimpleQueue(): CSemaphoreQueue(256) {}
    RemoveResult RemoveRequest(void **ppRequest, int mSecTimeout = INFINITE) {
        return RemoveHead(ppRequest, mSecTimeout);
    }
    bool AddRequest(void *pRequest) {
        return AddTail(pRequest);
    }
};

#endif // _CSIMPLEQUEUE_H_

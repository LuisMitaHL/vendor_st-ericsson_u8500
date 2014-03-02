/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <share/inc/macros.h>
#include "ENS_Queue.h"
#include "ENS_DBC.h"

#define INCREMENT(val, size) ((val + 1) % size)

ENS_Queue::ENS_Queue(int queue_size)
{
    head = tail = mutex = 0;
    DBC_ASSERT(queue_size >= 0);
    size = queue_size + 1;
    queue = new void * [size];
    mutex = LOS_MutexCreate();
}

ENS_Queue::~ENS_Queue()
{
    delete [] queue;
    LOS_MutexDestroy(mutex);
}

int ENS_Queue::nbElem()
{
    if(head >= tail)
	return head-tail;
    else
	return (head+size-tail);
}

int ENS_Queue::Push(void * elem)
{
    int result = 0;
    LOS_MutexLock(mutex);
    if(INCREMENT(head, size) == tail) {
	// queue full !!!!
	result = 1;
    } else {
	queue[head] = elem;
	head = INCREMENT(head, size);
    }
    LOS_MutexUnlock(mutex);
    return result;
}

void * ENS_Queue::Pop()
{
    void * result;
    LOS_MutexLock(mutex);
    if(head == tail) {
	result = NULL;
    } else {
	result = queue[tail];
	tail = INCREMENT(tail, size);
    }
    LOS_MutexUnlock(mutex);
    return result;
}

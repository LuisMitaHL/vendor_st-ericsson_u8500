/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#include <stdlib.h>
#include "osal_mutex.h"
#include "Img_EnsWrapper_Shared.h"
#include "Img_EnsQueue.h"

#define INCREMENT(val, size) ((val + 1) % size)

Img_EnsQueue::Img_EnsQueue(int queue_size)
{
	head = tail = 0;
	mutex = OSAL_MutexCreate();
	IMG_DBC_ASSERT(queue_size >= 0);
	size = queue_size + 1;
	queue = new void * [size];
}

Img_EnsQueue::~Img_EnsQueue()
{
	delete [] queue;
	head = 0;
	tail = 0;
	size = 0;
	OSAL_MutexDestroy(mutex);
}

int Img_EnsQueue::nbElem() const
{
	if(head >= tail)
		return head-tail;
	else
		return (head+size-tail);
}

void Img_EnsQueue::Reset()
{ /** Reset all entries */
	OSAL_MutexLock(mutex);
	head = tail = 0;
	OSAL_MutexUnlock(mutex);
}


int Img_EnsQueue::Push(void * elem)
{
	int result = 0;
	OSAL_MutexLock(mutex);
	if(INCREMENT(head, size) == tail)
	{	// queue full !!!!
		result = 1;
	}
	else
	{
		queue[head] = elem;
		head        = INCREMENT(head, size);
	}
	OSAL_MutexUnlock(mutex);
	return result;
}

void * Img_EnsQueue::Pop()
{
	void * Elem;
	OSAL_MutexLock(mutex);
	if(head == tail)
	{
		Elem = NULL;
	}
	else
	{
		Elem = queue[tail];
		tail = INCREMENT(tail, size);
	}
	OSAL_MutexUnlock(mutex);
	return Elem;
}

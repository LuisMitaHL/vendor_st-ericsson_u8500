/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SEMC_QUEUE_H_
#define _SEMC_QUEUE_H_
#include <stdlib.h>
#include <pthread.h>
struct QueueNode {
  void* item;
  QueueNode* pNext;
};
class Queue {
public:
  Queue()
  {
    nNode = 0;
    pRoot = NULL;
    pLast = NULL;
    pthread_mutex_init(&mutex, NULL);
  };
  void *dequeue()
  {
    void* ret = NULL;
    QueueNode* delNode;
    pthread_mutex_lock(&mutex);
    delNode = pRoot;
    if (delNode)
    {
      ret = delNode->item;
      if (delNode->pNext)
      {
        pRoot = delNode->pNext;
      }
      else
      {
        pRoot = NULL;
        pLast = NULL;
      }
      delete delNode;
      nNode --;
    }
    pthread_mutex_unlock(&mutex);
    return ret;
  };
  void enqueue(void* item)
  {
    QueueNode* addNode = new QueueNode;
    if (addNode)
    {
      addNode->item  = item;
      addNode->pNext = NULL;
      pthread_mutex_lock(&mutex);
      if (pLast)
      {
        pLast->pNext = addNode;
        pLast = addNode;
      }
      else
      {
        pLast = addNode;
        pRoot = addNode;
      }
      nNode++;
      pthread_mutex_unlock(&mutex);
    }
  };
private:
  int nNode;
  QueueNode* pRoot;
  QueueNode* pLast;
  pthread_mutex_t mutex;
};
#endif

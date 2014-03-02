/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMTHREADHANDLER_H
#define STECAMTHREADHANDLER_H

//System include
#include <utils/threads.h>

//Internal includes
#include "STECamQueue.h"
#include "STECamSemaphoreContainer.h"

namespace android {

//Thread Handler
template <typename T>
class ThreadHandler : public Thread
{
 template<typename U>
 friend class ThreadContainer;

 public:
  /* CTOR */
  inline ThreadHandler();

  /* DTOR */
  virtual ~ThreadHandler();

  /* start */
  inline status_t init(uint32_t aMaxEvents, bool aSignalCompletion, const char* aName,
                       int32_t aPriority, size_t aStack);

  /* Flush */
  inline status_t flush();

  /* Request */
  virtual void request(T& aData, bool aCheckRequestQueueFull = false);

  /* Wait for request to complete */
  inline status_t waitForCompletion();

  /* Request Pending */
  inline volatile T* isRequestPending();

 public:
  /* Handle new request */
  virtual status_t handleRequest(T& aData) = 0;

  /* Handle flush request */
  virtual status_t handleFlush(T& aData);

 protected:
  /* Self Destroy */
  inline void selfDestroy();

 protected:
  /* threadLoop, from Thread */
  virtual bool threadLoop();

 private:
  Queue<T> mQueue; /**< Queue */
  status_t mLastRequest; /**< Last Request */
  bool mIsThreadActive; /**< If thread is alive */
  bool mSignalCompletion; /**< Signal completion */
  SemaphoreContainer mRequestSem; /**< Request semaphore */
  SemaphoreContainer mCompletionSem; /**< Completion semaphore */
  SemaphoreContainer mFlushSem; /**< Flush semaphore */
  volatile T* mCurrentData; /**< Current data */
  Mutex mLock; /**< Lock for removing event */
  Mutex mFlushLock; /**< Flush Lock */
  volatile bool mFlushPending; /**< Flush pending */
};

#include "STECamThreadHandler.inl"

}

#endif // STECAMTHREADHANDLER_H


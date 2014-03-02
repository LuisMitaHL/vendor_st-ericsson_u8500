/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMREQHANDLERTHREAD_H
#define STECAMREQHANDLERTHREAD_H

//Internal include
#include "STECamThreadHandler.h"

namespace android {

//Forward declaration
class STECamera;

class ReqHandlerThreadData
{
 template<typename T>
 friend class ReqHandlerThread;

 public:
  inline ReqHandlerThreadData();

  inline void waitForRequestToStart();
  inline void signalRequestStarted();
  inline bool updateIfFree();

 public:
    reqHandler mReqHandler; /**< Handler */
    void* mArg; /**< Args */
    int32_t mMsg; /**< Message */
    int32_t mInfo1; /**< Message Info */
    int32_t mInfo2; /**< Message Info */
    Mutex   mLock; /**< Lock */
    bool    mWaitForRequestToStart; /*< Wait for request to start */

 private:
    Condition mRequestStatus; /**< Condition */
    bool      mFree; /**< Message is free */
};

//Request Handler thread
template<typename T>
class ReqHandlerThread : public ThreadHandler<T>
{
};

template<>
class ReqHandlerThread<ReqHandlerThreadData> : public ThreadHandler<ReqHandlerThreadData>
{
 public:
  /* CTOR */
  inline ReqHandlerThread(STECamera* aParent);

  /* Request, from ThreadHandler */
  virtual void request(ReqHandlerThreadData& aData, bool aCheckRequestQueueFull = false);

  /* Handle request, from ThreadHandler */
  virtual status_t handleRequest(ReqHandlerThreadData& aData);

 private:
  STECamera* mParent;
};

#include "STECamReqHandlerThread.inl"

}

#endif // STECAMREQHANDLERTHREAD_H


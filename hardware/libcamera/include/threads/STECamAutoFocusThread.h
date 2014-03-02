/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMAUTOFOCUSTHREAD_H
#define STECAMAUTOFOCUSTHREAD_H

//Internal include
#include "STECamThreadHandler.h"
#include "STECamAutoFocusThreadData.h"
namespace android {

//Forward declaration
class STECamera;

//AutoFocus thread
template<typename T>
class AutoFocusThread : public ThreadHandler<T>
{
};

template<>
class AutoFocusThread<AutoFocusThreadData> : public ThreadHandler<AutoFocusThreadData>
{
 public:
  /* CTOR */
  inline AutoFocusThread(STECamera* aParent);

  /* Handle request, from ThreadHandler */
  virtual status_t handleRequest(AutoFocusThreadData& aData);

 private:
  STECamera* mParent;
};

#include "STECamAutoFocusThread.inl"

}

#endif // STECAMAUTOFOCUSTHREAD_H


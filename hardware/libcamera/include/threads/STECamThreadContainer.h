/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMTHREADCONTAINER_H
#define STECAMTHREADCONTAINER_H

//Internal includes
#include "STECamThreadHandler.h"

namespace android {

//Thread Container
template <typename T>
class ThreadContainer
{
 public:
  /* CTOR */
  inline ThreadContainer(ThreadHandler<T>* aThreadHandler);

  /* DTOR */
  inline ~ThreadContainer();

  /* Accessors */
  inline ThreadHandler<T>* operator-> () const;
  inline ThreadHandler<T>* get() const;

 private:
  sp<ThreadHandler<T> > mThreadHandler; /**< Thread handler */
};

#include "STECamThreadContainer.inl"

}

#endif // STECAMTHREADCONTAINER_H


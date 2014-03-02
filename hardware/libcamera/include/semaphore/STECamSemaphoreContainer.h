/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMSEMAPHORECONTAINER_H
#define STECAMSEMAPHORECONTAINER_H

//System include
#include <semaphore.h>

namespace android {

//Thread Handler
class SemaphoreContainer
{
 public:
  /* CTOR */
  inline SemaphoreContainer();

  /* DTOR */
  inline ~SemaphoreContainer();

  /* start */
  inline status_t init(uint32_t aMaxOutstandingEvents);

  /* Wait */
  inline void wait();

  /* Signal */
  inline void signal();

 private:
  /* Check semaphore status */
  inline void check();

 private:
  sem_t mSem; /**< Semaphore */
  uint32_t mMaxOutstandingEvents; /**< Max outstanding events */
};

#include "STECamSemaphoreContainer.inl"

}

#endif // STECAMSEMAPHORECONTAINER_H


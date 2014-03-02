/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file uosal.cpp
 * utility osals stubs
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

// this code is only for RME unit development & tests: NOT for production code
#ifdef __RMEUNITARYDEV__

#include <pthread.h>
#include <sys/time.h>
#include <los/api/los_api.h>

#include "uosal.h"

////////////////////// local OSAL mutex class implementation using Posix
namespace omxilosalservices {

// OmxILOsalMutex implementation class
class OmxILOsalMutex_impl : public OmxILOsalMutex {
 public:
  OmxILOsalMutex_impl() : mpPosixMutex(new pthread_mutex_t) {
    if (mpPosixMutex)
      pthread_mutex_init(mpPosixMutex, NULL);
  };
  virtual ~OmxILOsalMutex_impl() {
    if (mpPosixMutex) {
      pthread_mutex_destroy(mpPosixMutex);
      delete mpPosixMutex;
    }
  };

  void MutexLock() {
    if (mpPosixMutex)
      pthread_mutex_lock(mpPosixMutex);
  };
  TError MutexTryLock() {
    if (mpPosixMutex) {
      if (pthread_mutex_trylock(mpPosixMutex) == 0)
        return ENone;
      else
        return EFailed;  // would prefer some EBusy
    } else {
      return EFailed;
    }
  };
  void MutexUnlock() {
    if (mpPosixMutex)
      pthread_mutex_unlock(mpPosixMutex);
  };

 private:
  pthread_mutex_t* mpPosixMutex;

  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(OmxILOsalMutex_impl);
};

// mutex create
TError OmxILOsalMutex::MutexCreate(OmxILOsalMutex* &aOmxILOsalMutex) {
  aOmxILOsalMutex = new OmxILOsalMutex_impl();
  if (aOmxILOsalMutex)
    return ENone;
  else
    return EFailed;
}

// mutex free
void OmxILOsalMutex::MutexFree(OmxILOsalMutex* &aOmxILOsalMutex) {
  delete reinterpret_cast<OmxILOsalMutex_impl*>(aOmxILOsalMutex);
  aOmxILOsalMutex = NULL;
}

// OmxILOsalTimer implementation
// Returns the system time in microseconds
OMX_U64 OmxILOsalTimer::GetSystemTime() {
  struct timeval tv;
  OMX_U64 systime = 0L;
  if (gettimeofday(&tv, NULL) == 0) {
    systime = tv.tv_sec * 1000000 + tv.tv_usec;
  }
  return systime;
}

}  // namespace omxilosalservices

extern "C" {
t_los_process_id LOS_ThreadCreate( void (*start_routine) (void *), void* arg, int stacksize, t_los_priority priority, const char* name ) {
    pthread_t ddt;
    if (name && priority && stacksize) { } // warning removal
    pthread_create(&ddt, NULL, (void*(*)(void*))start_routine, arg);
    return 0;
}
}

#endif


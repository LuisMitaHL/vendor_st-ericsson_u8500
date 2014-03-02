/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMTHREADINFO_H
#define STECAMTHREADINFO_H

//System include
#include <utils/threads.h>

//Internal includes
#include "STECamConfig.h"

namespace android {

//Thread Info
class ThreadInfo
{
 public:
  enum Type{
      ESwProcessing, /**< Sw Processing */
      EPreview, /**< Preview */
      ERecord, /**< Record */
      EAutoFocus, /**< AutoFocus */
      EReqHandler, /**< Request Handler */
      EMax /**< Watermark */
  };

 public:
  inline uint32_t maxRequests(Type aType) const;
  inline bool waitForCompletion(Type aType) const;
  inline const char* name(Type aType) const;
  inline int32_t priority(Type aType) const;
  inline size_t stackSize(Type aType) const;
};

#include "STECamThreadInfo.inl"

}; //namespace android

#endif // STECAMTHREADINFO_H


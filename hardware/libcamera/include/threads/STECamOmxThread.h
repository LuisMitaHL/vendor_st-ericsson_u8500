/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMOMXTHREAD_H
#define STECAMOMXTHREAD_H

//Internal include
#include "STECamThreadHandler.h"
#include "STECamOmxBuffInfo.h"

namespace android {

//Omx thread
template<typename T>
class OmxThread : public ThreadHandler<T>
{
};

template<>
class OmxThread<OmxBuffInfo> : public ThreadHandler<OmxBuffInfo>
{
 public:
  /* Handle request, from ThreadHandler */
  virtual status_t handleRequest(OmxBuffInfo& aData);

  /* Handle flush request, from ThreadHandler */
  virtual status_t handleFlush(OmxBuffInfo& aData);

 public:
  /* Handle buffer */
  virtual status_t handleBuffer(OmxBuffInfo& aData, OMX_BOOL& aUpdateFlags) =0;

 protected:
  inline void checkBufferValid(OmxBuffInfo& aData);
  inline void updateFlags(OmxBuffInfo& aData);
};

#include "STECamOmxThread.inl"

}

#endif // STECAMOMXTHREAD_H


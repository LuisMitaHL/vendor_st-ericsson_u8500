/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMRECORDTHREAD_H
#define STECAMRECORDTHREAD_H

//Internal include
#include "STECamOmxThread.h"

namespace android {

class STECamera;

//Record thread
template<typename T>
class RecordThread : public OmxThread<T>
{
};

template<>
class RecordThread<OmxBuffInfo> : public OmxThread<OmxBuffInfo>
{
 public:
  /* CTOR */
  inline RecordThread(STECamera* aParent);

  /* Handle buffer, from OmxThread */
  virtual status_t handleBuffer(OmxBuffInfo& aData, OMX_BOOL& aUpdateFlags);

 private:
  STECamera* mParent;
};

#include "STECamRecordThread.inl"

}

#endif // STECAMRECORDTHREAD_H


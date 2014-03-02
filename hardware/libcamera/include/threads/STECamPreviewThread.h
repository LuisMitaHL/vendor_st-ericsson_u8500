/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMPREVIEWTHREAD_H
#define STECAMPREVIEWTHREAD_H

//Internal include
#include "STECamOmxThread.h"

namespace android {

class STECamera;

//Preview thread
template<typename T>
class PreviewThread : public OmxThread<T>
{
};

template<>
class PreviewThread<OmxBuffInfo> : public OmxThread<OmxBuffInfo>
{
 public:
  /* CTOR */
  inline PreviewThread(STECamera* aParent);

  /* Handle buffer, from OmxThread */
  virtual status_t handleBuffer(OmxBuffInfo& aData, OMX_BOOL& aUpdateFlags);

 private:
  STECamera* mParent;
};

#include "STECamPreviewThread.inl"

}

#endif // STECAMPREVIEWTHREAD_H


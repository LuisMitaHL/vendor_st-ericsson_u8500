/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMOMXREQCBHANDLER_H
#define STECAMOMXREQCBHANDLER_H

//System Include
#include <OMX_CoreExt.h>
#include <OMX_IndexExt.h>

//internal include
#include "STECamTrace.h"
#include "STECamOmxUtils.h"

namespace android {

class OmxReqCbHandler
{
 public:
  enum Type
      {
          EAutoFocusStatus,  /**< AutoFocus Status*/
          EAutoFocusLock,  /**< AutoFocus Lock*/
          EExposureInitiated,  /**< ExposureInitiated*/
          EPreCaptureExposureTime,   /**< ShakeDet Lock*/
          ESmoothZoomStatus, /**< SmoothZoom Status*/
          EMeteringLock, /**<Lock for 3A*/
          EExtFocusStatus, /**<Continuous focus status*/
          EMaxType /**< WaterMark */
      };

 public:
  /* CTOR */
  OmxReqCbHandler();

  /* Init index */
  void init(Type aType, OMX_HANDLETYPE aHandle, OMX_U32 aPort, OMX_INDEXTYPE aIndex);

 public:
  /* Enable */
  inline OMX_ERRORTYPE enable(Type aType);

  /* Disable */
  inline OMX_ERRORTYPE disable(Type aType);

  /* Check if enabled */
  inline bool isEnabled(Type aType) const;

 private:
  /* Update request */
  inline OMX_ERRORTYPE update(Type aType, OMX_BOOL aEnable);

 private:
  OmxUtils::StructContainer<OMX_CONFIG_CALLBACKREQUESTTYPE> mReq[EMaxType]; /**< Request struct */
  OMX_HANDLETYPE mHandle[EMaxType]; /**< Handle */
  bool           mEnabled[EMaxType]; /**< Enabled */
};

#include "STECamOmxReqCbHandler.inl"

}

#endif // STECAMOMXREQCBHANDLER_H

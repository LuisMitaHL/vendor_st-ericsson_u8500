/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMOMXBUFFINFO_H
#define STECAMOMXBUFFINFO_H

//System include
#include <OMX_Core.h>
#include <hardware/camera.h>

//internal includes
#include "STECamTrace.h"
#include "STECamMemoryBase.h"
#include "STECamBuffer.h"
#include "STECamSwRoutines.h"

namespace android {

class OmxBuffInfo
{
 public:
  enum Type
  {
      ENone = 0, /**< None */
      ECameraHal=1, /*<buffinfo is with Camera HAL */
      ESentToComp = ECameraHal << 1, /**< Sent to component */
      ESentToHandlerThread = ESentToComp << 1, /**< Sent to HandlerThread */
      ESentToClient = ESentToHandlerThread << 1, /**< Sent to client */
      EEnableClientCallback = ESentToClient << 1, /**< Enable callbacks */
      ESentToNativeWindow = EEnableClientCallback << 1, /** <buffinfo is with native window */
  };

 public:
  /* CTOR */
  inline OmxBuffInfo();
  inline OmxBuffInfo(CamMemoryBase* const aCamMemoryBase);

  /* DTOR */
  inline ~OmxBuffInfo();

  /* Clear */
  inline void clear();

  /* Set user data */
  inline void setUserData(void* aUserData);

  /* User data */
  inline void* userData();
  inline void* userData() const;

  /* Fill Buffer */
  static inline OMX_ERRORTYPE fillBuffer(OMX_HANDLETYPE aHandle,
                                         OMX_BUFFERHEADERTYPE* const aOmxBufferHeader);

  /* Empty Buffer */
  static inline OMX_ERRORTYPE emptyBuffer(OMX_HANDLETYPE aHandle,
                                          OMX_BUFFERHEADERTYPE* const aOmxBufferHeader);

 private:
  /* Push buffer to component */
  static inline OMX_ERRORTYPE pushBuffer(OMX_HANDLETYPE aHandle,
                                         OMX_BUFFERHEADERTYPE* const aOmxBufferHeader,
                                         OMX_ERRORTYPE (*aFunc)(OMX_HANDLETYPE aHandle, OMX_BUFFERHEADERTYPE* aOmxBufferHeader));

 public:
  sp<CamMemoryBase> mCamMemoryBase; /**< Cam Memory Base We have keep it for backward
                                                                                       compatiable with GB*/
  camera_memory_t *mCamHandle; /** < Same as mCamMemoryBase.This is passed in callback */

  CamBuffer* mProcessingBuffer; /**< Processing buffer */
  swRoutines mSwRotation; /**< SW rotation */
  swRoutines mSwConversion; /**< SW conversion */
  uint32_t mFlags; /**< Flags */
  void* mUserData; /**< User data */
  int nativebuffIndex;
  MMHwBuffer* m_pMMHwBuffer;
  MMHwBuffer::TBufferInfo mMMHwBufferInfo;
  OMX_OSI_CONFIG_SHARED_CHUNK_METADATA mMMHwChunkMetaData;
  unsigned int mHeapOffset;
  bool mBufferOwner;
};

#include "STECamOmxBuffInfo.inl"

}

#endif // STECAMOMXBUFFINFO_H

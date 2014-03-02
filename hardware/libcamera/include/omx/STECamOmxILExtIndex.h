/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMOMXILEXTINDEX_H
#define STECAMOMXILEXTINDEX_H

//System Include
#include <OMX_Core.h>

//internal include
#include "STECamTrace.h"

namespace android {

class OmxILExtIndex
{
 public:
  enum Type
      {
          ECapturing, /**< Capturing */
          EDigitalZoom, /**< Digital Zoom */
          EFocusRange, /**< Focus Range */
          EFocusStatus,  /**< Focus status */
          EFocusLock,  /**< Focus lock */
          EFlashControl, /**< Flash Control */
          EFlickerRemoval, /**< Flicker Removal */
          ESceneMode, /**< Scene Mode */
          ESharpness, /**< Sharpness Mode */
          EExposureInitiated, /**< Exposure initiated */
          EPreCaptureExposureTime, /**< Shake Detection Lock*/
          EFocusRegion, /**< Focus Region*/
          ERegionOI, /**< ROI for Region Control*/
          EMakeModel, /**< Make and Model*/
          EOrientationScene, /**< Orientation Scene*/
//          EZoomMode, /**Zoom mode eg. smooth zoom*/
          ELensParams, /**Lens Params e.g. focal Length*/
          EExtradata, /**Exif fields in Extradata */
          ESoftware, /*Exif field for Software Version*/
//          EMeteringLock, /**Lock for 3A */
          EExtCaptureMode, /**< Ext Capture Mode for ZSL*/
//          EExifTimeStamp, /**<Exif Time Stamp*/
//          EMaxZoomSupportedRes,/*<Max zoom resolution supported by sensor*/
          EExtFocusStatus,  /**<Get continuous focus status*/
          EMaxType /**< WaterMark */
      };

 public:
  /* CTOR */
  OmxILExtIndex(const OMX_HANDLETYPE aCamHandle);

  /* Init index */
  OMX_ERRORTYPE init();

 public:
  /* Get index */
  inline OMX_INDEXTYPE getIndex(Type aType) const;

 private:
  static const OMX_STRING mExtIndexName[]; /*< Extension strings */
  OMX_INDEXTYPE mIndex[EMaxType]; /**< Index */
  OMX_HANDLETYPE mCamHandle; /**< Camera Handle */
};

#include "STECamOmxILExtIndex.inl"

}

#endif // STECAMOMXILEXTINDEX_H

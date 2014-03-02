/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "framerate_ext.h"
const float gFramerateTable[OMX_3A_SENSOR_NAME_MAX][OMX_3A_SENSORMODE_USAGE_MAX] =
{
  //.[OMX_3A_SENSOR_NAME_IMX072] =
  {
    30.0, // .[OMX_3A_SENSORMODE_USAGE_VF]                   //
    30.0, // .[OMX_3A_SENSORMODE_USAGE_AF]                   // don't use by semc
    15.0, // .[OMX_3A_SENSORMODE_USAGE_STILL_CAPTURE]        //
    15.0, // .[OMX_3A_SENSORMODE_USAGE_NIGHT_STILL_CAPTURE]  // don't use by semc
    15.0, // .[OMX_3A_SENSORMODE_USAGE_SEQ_STILL_CAPTURE]    // don't use by semc
    30.0, // .[OMX_3A_SENSORMODE_USAGE_VIDEO_CAPTURE]        //
    30.0, // .[OMX_3A_SENSORMODE_USAGE_NIGHT_VIDEO_CAPTURE]  // don't use by semc
    30.0, // .[OMX_3A_SENSORMODE_USAGE_HQ_VIDEO_CAPTURE]     //
    120.0,// .[OMX_3A_SENSORMODE_USAGE_HS_VIDEO_CAPTURE]     //
  },
  //.[OMX_3A_SENSOR_NAME_IMX111] =
  {
    // TODO: use 20 fps for M1 alpha , should use 45.0 after support sensor usage selecting
#if 1
    45.0, // .[OMX_3A_SENSORMODE_USAGE_VF]                   //
    45.0, // .[OMX_3A_SENSORMODE_USAGE_AF]                   // don't use by semc
    15.0, // .[OMX_3A_SENSORMODE_USAGE_STILL_CAPTURE]        //
    15.0, // .[OMX_3A_SENSORMODE_USAGE_NIGHT_STILL_CAPTURE]  // don't use by semc
    15.0, // .[OMX_3A_SENSORMODE_USAGE_SEQ_STILL_CAPTURE]    // don't use by semc
    45.0, // .[OMX_3A_SENSORMODE_USAGE_VIDEO_CAPTURE]        //
    45.0, // .[OMX_3A_SENSORMODE_USAGE_NIGHT_VIDEO_CAPTURE]  // don't use by semc
    30.0, // .[OMX_3A_SENSORMODE_USAGE_HQ_VIDEO_CAPTURE]     //
    120.0,// .[OMX_3A_SENSORMODE_USAGE_HS_VIDEO_CAPTURE]     //
#else
    30.0, // .[OMX_3A_SENSORMODE_USAGE_VF]                   //
    30.0, // .[OMX_3A_SENSORMODE_USAGE_AF]                   // don't use by semc
    15.0, // .[OMX_3A_SENSORMODE_USAGE_STILL_CAPTURE]        //
    15.0, // .[OMX_3A_SENSORMODE_USAGE_NIGHT_STILL_CAPTURE]  // don't use by semc
    15.0, // .[OMX_3A_SENSORMODE_USAGE_SEQ_STILL_CAPTURE]    // don't use by semc
    30.0, // .[OMX_3A_SENSORMODE_USAGE_VIDEO_CAPTURE]        //
    30.0, // .[OMX_3A_SENSORMODE_USAGE_NIGHT_VIDEO_CAPTURE]  // don't use by semc
    30.0, // .[OMX_3A_SENSORMODE_USAGE_HQ_VIDEO_CAPTURE]     //
    30.0, // .[OMX_3A_SENSORMODE_USAGE_HS_VIDEO_CAPTURE]     //
#endif
  },
};
CFramerateExt::CFramerateExt(OMX_U32 defaultFramerate_x100)
  : CFramerate(defaultFramerate_x100)
{
  sensorName = OMX_3A_SENSOR_NAME_UNKNOWN;
  usageMode = OMX_3A_SENSORMODE_USAGE_UNDEFINED;
}

float CFramerateExt::getSensorMaxFrameRate()
{
  float fMaxFrameRate;
  if (sensorName == OMX_3A_SENSOR_NAME_UNKNOWN || usageMode == OMX_3A_SENSORMODE_USAGE_UNDEFINED)
  {
    fMaxFrameRate = (float)15; // we don't have this case , but just in case.
  }
  else
  {
    fMaxFrameRate = gFramerateTable[sensorName][usageMode];
  }
  return fMaxFrameRate;
}

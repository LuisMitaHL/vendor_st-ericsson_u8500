/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _FRAMERATE_EXTENSION_H_
#define _FRAMERATE_EXTENSION_H_
#include "framerate.h"
#include "OMX_3A_CameraExt.h"

/*
 *
 * The component determined sensor maximum framerate to use before running ISP.
 * The framerate is only supported variable framerate , the fix framerate can be fixed on AE.
 * After running we will directory set the FrameRateControl_f_UserMaximumFrameRate_Hz_Byte0 from 3A manager
 * 3A manager can only decrease the framerate .
 *
 */
class CFramerateExt : public CFramerate
{
public:
  CFramerateExt(OMX_U32 defaultFramerate_x100);
  virtual void setSensorName(OMX_3A_SENSOR_NAME_TYPE name)
  {
    sensorName = name;
  };
  virtual void setSensorUsageMode(OMX_3A_SENSORMODE_USAGE_TYPE sensorUsageMode)
  {
    usageMode = sensorUsageMode;
  };
  virtual float getSensorMaxFrameRate();
  virtual OMX_3A_SENSORMODE_USAGE_TYPE getSensorUsageMode()
  {
    return usageMode;
  };
protected:
  OMX_3A_SENSORMODE_USAGE_TYPE usageMode;
  OMX_3A_SENSOR_NAME_TYPE sensorName;
};
#endif

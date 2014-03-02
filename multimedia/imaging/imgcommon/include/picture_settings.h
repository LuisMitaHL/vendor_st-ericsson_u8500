/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _PICTURE_SETTINGS_
#define _PICTURE_SETTINGS_

#include <inc/typedef.h>
#include "tuning.h"
#include <OMX_Types.h>
#include "TraceObject.h"

typedef enum
{
   PICTURE_SETTING_OK,
   PICTURE_SETTING_INVALID_ARGUMENT,
   PICTURE_SETTING_TUNING_ERROR,
} t_picture_setting_error_code;

class COmxSettingMapping
{
   public:
     COmxSettingMapping(TraceObject *traceobj, t_uint32 aGainParamAddr, t_uint32 aOffsetParamAddr);
     t_picture_setting_error_code Configure(CTuning* pTuning);

   protected:
     t_uint32 mGainParamAddr;
     t_uint32 mOffsetParamAddr;
     float    mGainValue;
     float    mOffsetValue;
     TraceObject*          mTraceObject;
};

class COmxBrightnessMapping: public COmxSettingMapping
{
   public:
      COmxBrightnessMapping(TraceObject *traceobj);
      t_sint16 ComputeLumaOffsetPE(OMX_U32 aOmxBrightness);
};

class COmxSaturationMapping: public COmxSettingMapping
{
   public:
      COmxSaturationMapping(TraceObject *traceobj);
      t_uint8 ComputeColourSaturationPE(OMX_S32 aOmxSaturation);
};

class COmxContrastMapping: public COmxSettingMapping
{
   public:
      COmxContrastMapping(TraceObject *traceobj);
      t_uint8 ComputeContrastPE(OMX_S32 aOmxContrast);
};

class CPictureSettings
{
   public:
     CPictureSettings(TraceObject *);
     t_picture_setting_error_code Configure(CTuning* pTuning);
     COmxBrightnessMapping iBrightness;
     COmxSaturationMapping iSaturation;
     COmxContrastMapping   iContrast;
     TraceObject*          mTraceObject;
};

#endif // _PICTURE_SETTINGS_

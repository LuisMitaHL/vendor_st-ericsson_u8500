/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "PICTURE_SETTINGS"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include "osi_trace.h"
#include "picture_settings.h"
#include "tuning_params.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_picture_settingsTraces.h"
#endif

/*
 * Constructor
 */
COmxSettingMapping::COmxSettingMapping(TraceObject *traceobj, t_uint32 aGainParamAddr, t_uint32 aOffsetParamAddr): mTraceObject(traceobj)
{
   mGainParamAddr = aGainParamAddr;
   mOffsetParamAddr = aOffsetParamAddr;
   mGainValue = 1.0;
   mOffsetValue = 0.0;
}

/*
 * This function configures the OMX setting mapping instance from tuning configuration
 */
t_picture_setting_error_code COmxSettingMapping::Configure(CTuning* pTuning)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry COmxSettingMapping::Configure", (mTraceObject));
   t_tuning_error_code err = TUNING_OK;

   if(pTuning==NULL) {
      DBGT_ERROR("Invalid argument: pTuning==NULL\n");
      OstTraceFiltStatic0(TRACE_ERROR, "Invalid argument: pTuning==NULL", (mTraceObject));
      OUTR(" ", PICTURE_SETTING_INVALID_ARGUMENT);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit COmxSettingMapping::Configure (%d)", (mTraceObject), PICTURE_SETTING_INVALID_ARGUMENT);
      return PICTURE_SETTING_INVALID_ARGUMENT;
   }

   err = pTuning->GetParam(IQSET_PICTURE_SETTINGS_MAPPING, IQSET_USER_DRIVER, mGainParamAddr, &mGainValue);
   if(err != TUNING_OK) {
      DBGT_ERROR("Failed to get tuning param 0x%lX, err=%d (%s)\n", mGainParamAddr, err, CTuning::ErrorCode2String(err));
      OstTraceFiltStatic2(TRACE_ERROR, "Failed to get tuning param 0x%lX, err=%d", (mTraceObject), mGainParamAddr, err);
      OUTR(" ", PICTURE_SETTING_TUNING_ERROR);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit COmxSettingMapping::Configure (%d)", (mTraceObject), PICTURE_SETTING_TUNING_ERROR);
      return PICTURE_SETTING_TUNING_ERROR;
   }

   err = pTuning->GetParam(IQSET_PICTURE_SETTINGS_MAPPING, IQSET_USER_DRIVER, mOffsetParamAddr, &mOffsetValue);
   if(err != TUNING_OK) {
      DBGT_ERROR("Failed to get tuning param 0x%lX, err=%d (%s)\n", mOffsetParamAddr, err, CTuning::ErrorCode2String(err));
      OstTraceFiltStatic2(TRACE_ERROR, "Failed to get tuning param 0x%lX, err=%d", (mTraceObject), mOffsetParamAddr, err);
      OUTR(" ", PICTURE_SETTING_TUNING_ERROR);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit COmxSettingMapping::Configure (%d)", (mTraceObject), PICTURE_SETTING_TUNING_ERROR);
      return PICTURE_SETTING_TUNING_ERROR;
   }

   OUTR(" ", PICTURE_SETTING_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit COmxSettingMapping::Configure (%d)", (mTraceObject), PICTURE_SETTING_OK);
   return PICTURE_SETTING_OK;
}

/*
 * Brithness related functions
 */
COmxBrightnessMapping::COmxBrightnessMapping(TraceObject *traceobj):COmxSettingMapping(traceobj, f_BrightnessGain, f_BrightnessOffset)
{
}

t_sint16 COmxBrightnessMapping::ComputeLumaOffsetPE(OMX_U32 aOmxBrightness)
{
    float fLumaOffset = ((float)aOmxBrightness * mGainValue) + mOffsetValue + 0.5;
    if( fLumaOffset < -255.0 )
        fLumaOffset = -255.0;
    else if( fLumaOffset > 255.0 )
        fLumaOffset = 255.0;
    return (t_sint16)fLumaOffset;
}

/*
 * Saturation related functions
 */
COmxSaturationMapping::COmxSaturationMapping(TraceObject *traceobj):COmxSettingMapping(traceobj, f_SaturationGain, f_SaturationOffset)
{
}

t_uint8 COmxSaturationMapping::ComputeColourSaturationPE(OMX_S32 aOmxSaturation)
{
    float fSaturation = ((float)aOmxSaturation * mGainValue) + mOffsetValue;
    if( fSaturation < 1.0)
       fSaturation = 1.0;
    else if( fSaturation > 255.0)
       fSaturation = 255.0;
    return (t_uint8)fSaturation;
}

/*
 * Contrast related functions
 */
COmxContrastMapping::COmxContrastMapping(TraceObject *traceobj):COmxSettingMapping(traceobj, f_ContrastGain, f_ContrastOffset)
{
}

t_uint8 COmxContrastMapping::ComputeContrastPE(OMX_S32 aOmxContrast)
{
    float fContrast = ((float)aOmxContrast * mGainValue) + mOffsetValue;
    if( fContrast < 1.0)
       fContrast = 1.0;
    else if( fContrast > 255.0)
       fContrast = 255.0;
    return (t_uint8)fContrast;
}

/*
 * CPictureSettings Constructor
 */
CPictureSettings::CPictureSettings(TraceObject *traceobj):
		iBrightness(traceobj)
		,iSaturation(traceobj)
		,iContrast(traceobj)
		,mTraceObject(traceobj)
{
}

/*
 * This function configures the picture settings from tuning configuration
 */
t_picture_setting_error_code CPictureSettings::Configure(CTuning* pTuning)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CPictureSettings::Configure", (mTraceObject));
   t_picture_setting_error_code err = PICTURE_SETTING_OK;

   err = iBrightness.Configure(pTuning);
   if(err != PICTURE_SETTING_OK) {
      DBGT_ERROR("Failed to configure OMX Brightness setting, err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to configure OMX Brightness setting, err=%d", (mTraceObject), err);
      OUTR(" ", err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CPictureSettings::Configure (%d)", (mTraceObject), err);
      return err;
   }

   err = iSaturation.Configure(pTuning);
   if(err != PICTURE_SETTING_OK) {
      DBGT_ERROR("Failed to configure OMX Saturation setting, err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to configure OMX Saturation setting, err=%d", (mTraceObject), err);
      OUTR(" ", err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CPictureSettings::Configure (%d)", (mTraceObject), err);
      return err;
   }

   err = iContrast.Configure(pTuning);
   if(err != PICTURE_SETTING_OK) {
      DBGT_ERROR("Failed to configure OMX Contrast setting, err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to configure OMX Contrast setting, err=%d", (mTraceObject), err);
      OUTR(" ", err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CPictureSettings::Configure (%d)", (mTraceObject), err);
      return err;
   }

   OUTR(" ", PICTURE_SETTING_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CPictureSettings::Configure (%d)", (mTraceObject), PICTURE_SETTING_OK);
   return PICTURE_SETTING_OK;
}

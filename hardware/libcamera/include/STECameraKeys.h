/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Extension keys defined for STE Camera
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMERAKEYS_H
#define STECAMERAKEYS_H

namespace android {

//cam-mode = 0(STILL)/1(VIDEO) for mode
static const char KEY_CAM_MODE[] = "cam-mode";
static const char KEY_F_NUMBER[] = "fnumber";

//Record Keys
static const char KEY_RECORD_SIZE[]            = "record-size";
static const char KEY_SUPPORTED_RECORD_SIZES[] = "record-size-values";
static const char KEY_RECORD_BUFFER_COUNT[]    = "record-buffer-count";
static const char KEY_RECORD_BUFFER_SIZE[]     = "record-buffer-size";
static const char KEY_RECORD_MEM_HANDLE[]      = "record-mem-handle";
static const char KEY_RECORD_MEM_HEAP_HANDLE[] = "record-mem-heap-handle";

//Preview keys
static const char KEY_PREVIEW_NSTRIDE[]   = "preview-nStride";
static const char KEY_PREVIEW_NSLICE[]    = "preview-nSliceHeight";

//Extra keys
static const char KEY_CONTRAST[]         = "contrast";
static const char KEY_BRIGHTNESS[]       = "brightness";
static const char KEY_SATURATION[]       = "saturation";
static const char KEY_SHARPNESS[]        = "sharpness";

static const char KEY_SUPPORTED_CONTRAST[]          = "contrast-values";
static const char KEY_SUPPORTED_BRIGHTNESS[]        = "brightness-values";
static const char KEY_SUPPORTED_SATURATION[]        = "saturation-values";

static const char KEY_SUPPORTED_METERING[]          = "metering-values";
static const char KEY_SUPPORTED_ISO[]               = "iso-values";

static const char KEY_MAX_CONTRAST[]     = "contrast-max";
static const char KEY_MIN_CONTRAST[]     = "contrast-min";
static const char KEY_CONTRAST_STEP[]    = "contrast-step";
static const char KEY_MAX_BRIGHTNESS[]   = "brightness-max";
static const char KEY_MIN_BRIGHTNESS[]   = "brightness-min";
static const char KEY_BRIGHTNESS_STEP[]  = "brightness-step";
static const char KEY_MAX_SHARPNESS[]    = "sharpness-max";
static const char KEY_MIN_SHARPNESS[]    = "sharpness-min";
static const char KEY_SHARPNESS_STEP[]   = "sharpness-step";
static const char KEY_MAX_SATURATION[]   = "saturation-max";
static const char KEY_MIN_SATURATION[]   = "saturation-min";
static const char KEY_SATURATION_STEP[]  = "saturation-step";

static const char KEY_METERING_EXPOSURE[]           = "metering";
static const char METERING_EXPOSURE_MATRIX[]        = "matrix";
static const char METERING_EXPOSURE_SPOT[]          = "spot";
static const char METERING_EXPOSURE_AVERAGE[]       = "average";
static const char METERING_EXPOSURE_CENTRE_WEIGHTED[] = "center";
static const char SCENE_MODE_BACKLIGHT[]          = "backlight";
static const char SCENE_MODE_CLOSEUP[]            = "macro";
static const char SCENE_MODE_DOCUMENT[]           = "document";
static const char EFFECT_MONOTONE_NEGATIVE[] = "mono-negative";
static const char KEY_AUTO_CONTRAST_MODE[]  = "wdr";

static const char KEY_ISO[]     = "iso";
static const char ISO_AUTO[]    = "auto";
static const char ISO_100[]     = "100";
static const char ISO_200[]     = "200";
static const char ISO_400[]     = "400";
static const char ISO_800[]     = "800";
static const char ISO_1600[]    = "1600";

static const char KEY_REGION_CONTROL[]     = "region-control";
static const char KEY_SUPPORTED_REGION_CONTROL[]     = "supported-region-control";
static const char REGION_CENTER[]          = "region-center";
static const char REGION_MULTI[]           = "region-multi";
static const char REGION_USER[]            = "region-user";
static const char REGION_TOUCH[]           = "region-touch";
static const char REGION_OBJECT[]          = "region-object";
static const char REGION_FACE[]            = "region-face";
static const char REGION_CONTROL_OFF[]     = "region-control-off";
static const char REGION_ROI_NUMBER[]      = "region-roi-number";
static const char REGION_COORDINATES[]     = "region-coordinates";

static const char KEY_ZERO_SHUTTER_LAG_MODE[]     = "zero-shutter-lag-mode";
static const char KEY_HIRES_VIEWFINDER[] = "hires-viewfinder";

//Overlay key
static const char KEY_OVERLAY_FORMAT[]   = "overlay-format";
}; // namespace android

#endif

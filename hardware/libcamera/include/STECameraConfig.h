/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMERACONFIG_H
#define STECAMERACONFIG_H

//*************************************************************************
//*************************************************************************
//**  PRIMARY camera
//*************************************************************************
//*************************************************************************

// Define below lines for default preview params
#define DEFAULT_PRIMARY_PREVIEW_WIDTH 640
#define DEFAULT_PRIMARY_PREVIEW_HEIGHT 480
#define DEFAULT_PRIMARY_PREVIEW_NSTRIDE 640
#define DEFAULT_PRIMARY_PREVIEW_SLICE_HEIGHT 16

#define DEFAULT_PRIMARY_PREVIEW_FRAMERATE 30
#define DEFAULT_PRIMARY_FPS_RANGE "5000,30000"
#define SUPPORTED_PRIMARY_PREVIEW_SIZES "640x480"
#define SUPPORTED_PRIMARY_FRAMERATES "15,30"
#define SUPPORTED_PRIMARY_FPS_RANGES "(5000,30000)"

#define SUPPORTED_PRIMARY_RECORD_SIZES "640x480,1920x1080,1280x720,320x240,176x144"
/* +CR340479 */
#define DEFAULT_PRIMARY_RECORD_SIZE "0"
 
#define DEFAULT_PRIMARY_VIDEO_RECORD_WIDTH 640
#define DEFAULT_PRIMARY_VIDEO_RECORD_HEIGHT 480

#define DEFAULT_PRIMARY_FOCAL_LENGTH "5"

// Define below lines for default capture params
#define DEFAULT_PRIMARY_PICTURE_WIDTH 640
#define DEFAULT_PRIMARY_PICTURE_HEIGHT 480
#define DEFAULT_PRIMARY_JPEG_QUALITY 90
#define DEFAULT_PRIMARY_THUMBNAIL_WIDTH 160
#define DEFAULT_PRIMARY_THUMBNAIL_HEIGHT 120
#define DEFAULT_PRIMARY_JPEG_THUMBNAIL_QUALITY 70

#if defined(STE_SENSOR_MT9P111)
#define SUPPORTED_PRIMARY_PICTURE_SIZES "2560x1920,2560x1440,2048x1536,2048x1152,1600x1200,1280x720,1024x768,640x480,320x240"
#else
#define SUPPORTED_PRIMARY_PICTURE_SIZES "2592x1944,2048x1536,1920x1080,1600x1200,1280x720,1024x768,720x480,640x480,352x288,320x240,176x144"
#endif

#define SUPPORTED_PRIMARY_THUMB_SIZES "320x240,160x120,0x0"

#define DEFAULT_PRIMARY_WHITE_BALANCE "auto"
#define SUPPORTED_PRIMARY_WHITE_BALANCES "auto,incandescent,fluorescent,daylight,cloudy-daylight"

#define DEFAULT_PRIMARY_FOCUS CameraParameters::FOCUS_MODE_AUTO
#define SUPPORTED_PRIMARY_FOCUS "auto,infinity"

#define DEFAULT_PRIMARY_FLASH CameraParameters::FLASH_MODE_OFF
#define SUPPORTED_PRIMARY_FLASH "off,on,torch" //"on,torch,off"

#define DEFAULT_PRIMARY_ANTIBANDING CameraParameters::ANTIBANDING_OFF
#define SUPPORTED_PRIMARY_ANTIBANDING "off"

#define DEFAULT_PRIMARY_SCENE_MODE CameraParameters::SCENE_MODE_AUTO
#define SUPPORTED_PRIMARY_SCENE_MODES "auto"

#define DEFAULT_PRIMARY_EFFECT CameraParameters::EFFECT_NONE
#define SUPPORTED_PRIMARY_EFFECTS "none,mono,negative,solarize,sepia"

#define DEFAULT_PRIMARY_AUTO_CONTRAST_MODE 0
#define DEFAULT_PRIMARY_CONTRAST 0
#define DEFAULT_PRIMARY_BRIGHTNESS 0
#define DEFAULT_PRIMARY_SHARPNESS 0
#define DEFAULT_PRIMARY_SATURATION 0
#define DEFAULT_PRIMARY_EXPOSURE_COMPENSATION 0

#define DEFAULT_PRIMARY_ISO ISO_AUTO
#define DEFAULT_PRIMARY_METERING_EXPOSURE METERING_EXPOSURE_MATRIX

// here the definitions of min, max, step for contrast, exposure, saturation, brightness & sharpness
#define CONTRAST_PRIMARY_MIN -3
#define CONTRAST_PRIMARY_MAX  3
#define CONTRAST_PRIMARY_STEP 1

#ifdef  STE_SENSOR_MT9P111
#define EXPOSURE_PRIMARY_MIN -2
#define EXPOSURE_PRIMARY_MAX  2
#define EXPOSURE_PRIMARY_STEP 1
#else
#define EXPOSURE_PRIMARY_MIN -3
#define EXPOSURE_PRIMARY_MAX  3
#define EXPOSURE_PRIMARY_STEP 1
#endif

#define SATURATION_PRIMARY_MIN -2
#define SATURATION_PRIMARY_MAX  2
#define SATURATION_PRIMARY_STEP 1

#define BRIGHTNESS_PRIMARY_MIN -2
#define BRIGHTNESS_PRIMARY_MAX  2
#define BRIGHTNESS_PRIMARY_STEP 1

#define SHARPNESS_PRIMARY_MIN -2
#define SHARPNESS_PRIMARY_MAX  2
#define SHARPNESS_PRIMARY_STEP 1

//*************************************************************************
//*************************************************************************
//**  SECONDARY camera
//*************************************************************************
//*************************************************************************

// Define below lines for default preview params
#define DEFAULT_SECONDARY_PREVIEW_WIDTH 640
#define DEFAULT_SECONDARY_PREVIEW_HEIGHT 480
#define DEFAULT_SECONDARY_PREVIEW_FRAMERATE 30
#define DEFAULT_SECONDARY_PREVIEW_NSTRIDE 640
#define DEFAULT_SECONDARY_PREVIEW_SLICE_HEIGHT 16

#ifdef  STE_SENSOR_OV5640
#define SUPPORTED_SECONDARY_PREVIEW_SIZES "720x480,640x480,352x288,320x240,320x200,200x320,176x144"
#else
#define SUPPORTED_SECONDARY_PREVIEW_SIZES "640x480,320x240"
#endif

#define SUPPORTED_SECONDARY_FRAMERATES "15,30"
#define SUPPORTED_SECONDARY_FPS_RANGES "(5000,30000)"
#define SUPPORTED_SECONDARY_FPS_RANGE "5000,30000"
#define SUPPORTED_MIN_FPS_RANGE 5000
#define SUPPORTED_MAX_FPS_RANGE 30000

#define DEFAULT_SECONDARY_VIDEO_RECORD_WIDTH 640
#define DEFAULT_SECONDARY_VIDEO_RECORD_HEIGHT 480
#define SUPPORTED_SECONDARY_RECORD_SIZES "640x480,640x384,352x288,320x240,176x144"
#define PREFERRED_SECONDARY_PREVIEW_SIZE "640x480"
#define DEFAULT_SECONDARY_FOCAL_LENGTH "5"
//#define DEFAULT_SECONDARY_RECORD_SIZE "0"
//#define SUPPORTED_SECONDARY_FPS_RANGES "(10000,10000),(15000,15000),(20000,20000),(30000,30000)"

// Define below lines for default capture params
#define DEFAULT_SECONDARY_PICTURE_WIDTH 640
#define DEFAULT_SECONDARY_PICTURE_HEIGHT 480
#define DEFAULT_SECONDARY_JPEG_QUALITY 90
#define DEFAULT_SECONDARY_THUMBNAIL_WIDTH 160
#define DEFAULT_SECONDARY_THUMBNAIL_HEIGHT 120
#define DEFAULT_SECONDARY_JPEG_THUMBNAIL_QUALITY 70
#ifdef  STE_SENSOR_OV5640
#define SUPPORTED_SECONDARY_PICTURE_SIZES "720x480,640x480,352x288,320x240,320x200,200x320,176x144"
#else
#define SUPPORTED_SECONDARY_PICTURE_SIZES "640x480,352x288,320x240,176x144"
#endif
#define SUPPORTED_SECONDARY_THUMB_SIZES "0x0,160x120"

#define DEFAULT_SECONDARY_WHITE_BALANCE "auto"
#define SUPPORTED_SECONDARY_WHITE_BALANCES "auto,incandescent,fluorescent,daylight,cloudy-daylight"

#define DEFAULT_SECONDARY_FOCUS CameraParameters::FOCUS_MODE_FIXED
#define SUPPORTED_SECONDARY_FOCUS CameraParameters::FOCUS_MODE_FIXED

#define DEFAULT_SECONDARY_FLASH CameraParameters::FLASH_MODE_OFF
#define SUPPORTED_SECONDARY_FLASH CameraParameters::FLASH_MODE_OFF

#define DEFAULT_SECONDARY_ANTIBANDING CameraParameters::ANTIBANDING_50HZ
#define SUPPORTED_SECONDARY_ANTIBANDING "auto,50hz,60hz,off"

#define DEFAULT_SECONDARY_SCENE_MODE CameraParameters::SCENE_MODE_AUTO

// suppress some unused scene modes
#define SUPPORTED_SECONDARY_SCENE_MODES "auto"
#define DEFAULT_SECONDARY_EFFECT CameraParameters::EFFECT_NONE
#define SUPPORTED_SECONDARY_EFFECTS "none,mono,negative,solarize,sepia,posterize,aqua"

#define DEFAULT_SECONDARY_CONTRAST 0
#define DEFAULT_SECONDARY_BRIGHTNESS 0
#define DEFAULT_SECONDARY_SHARPNESS 0
#define DEFAULT_SECONDARY_SATURATION 0
#define DEFAULT_SECONDARY_EXPOSURE_COMPENSATION 0
#define DEFAULT_SECONDARY_ISO ISO_AUTO
#define DEFAULT_SECONDARY_METERING_EXPOSURE METERING_EXPOSURE_MATRIX

// here the definitions of min, max, step for exposure, saturation, brightness & sharpness
#define CONTRAST_SECONDARY_MIN -3
#define CONTRAST_SECONDARY_MAX  3
#define CONTRAST_SECONDARY_STEP 1

#define EXPOSURE_SECONDARY_MIN -3
#define EXPOSURE_SECONDARY_MAX  3
#define EXPOSURE_SECONDARY_STEP 1

#define SATURATION_SECONDARY_MIN -2
#define SATURATION_SECONDARY_MAX  2
#define SATURATION_SECONDARY_STEP 1

#define BRIGHTNESS_SECONDARY_MIN -2
#define BRIGHTNESS_SECONDARY_MAX  2
#define BRIGHTNESS_SECONDARY_STEP 1

#define SHARPNESS_SECONDARY_MIN -2
#define SHARPNESS_SECONDARY_MAX  2
#define SHARPNESS_SECONDARY_STEP 1

//*************************************************************************
//*************************************************************************
//**  COMMON SETTINGS
//*************************************************************************
//*************************************************************************

#define FOCUS_STATUS_WAIT 10 //in ms
#ifdef  STE_SENSOR_MT9P111
#define FOCUS_TIMEOUT_COUNTER 200 //increased the value to 200 for MT9P111
#else
#define FOCUS_TIMEOUT_COUNTER 100 //increased the value to 100 for OV5640
#endif
#define CONTRAST_OFFSET 2
#define EXPOSURE_OFFSET 3
#define SATURATION_OFFSET 2
#define BRIGHTNESS_OFFSET 2
#define SHARPNESS_OFFSET 2

// limit the preview to WVGA to reduce memory bandwith
#define LIMIT_PREVIEW_WIDTH 864
#define LIMIT_PREVIEW_HEIGHT 480

//*************************************************************************
//*************************************************************************
//**  EXIF DEFAULT SETTINGS SETTINGS
//*************************************************************************
//*************************************************************************
#define EXIF_DEFAULT_MAKE "St-Ericsson"
#define EXIF_DEFAULT_MODEL "U8500"
#define EXIF_DEFAULT_SOFTWARE "Android 4.0"
#define EXIF_DEFAULT_EXPOSURE_TIME 0
#define EXIF_DEFAULT_APERTURE_FNUMBER 0
#define EXIF_DEFAULT_ISO_EQUIVALENT 100
#define EXIF_DEFAULT_METERING_MODE "2" //default = center_weighted
#define EXIF_DEFAULT_ZOOM_RATIO 1.0
#define EXIF_DEFAULT_SCENE_TYPE "0" //default = standard
#define EXIF_DEFAULT_LIGHT_SOURCE "0" //default = unknown
#define EXIF_DEFAULT_USER_COMMENTS "User comments"
#define EXIF_DEFAULT_FLASH_USED "0" //default = Flash did not fire
#define EXIF_DEFAULT_COLOR_SPACE "1" //default = sRGB
#define EXIF_DEFAULT_EXPOSURE_BIAS 0
#define EXIF_DEFAULT_SHUTTER_SPEED 0
#define EXIF_DEFAULT_FOCAL_LENGTH_NUM 10
#define EXIF_DEFAULT_FOCAL_LENGTH_DENOM 3
#define EXIF_DEFAULT_MAX_APERTURE_NUM 10
#define EXIF_DEFAULT_MAX_APERTURE_DENOM 3
#define EXIF_DEFAULT_CUSTOM_RENDERED "0" //default = Normal process
#define EXIF_DEFAULT_EXPOSURE_MODE "0" //default = Auto Exposure
#define EXIF_DEFAULT_EXPOSURE_PROGRAM "3" //default = Aperture priority
#define EXIF_DEFAULT_DISTANCE_RANGE "0" //default = unknown
#define EXIF_DEFAULT_COMPRESSION "6"
#define EXIF_DEFAULT_XYRESOL "72/1"
#define EXIF_DEFAULT_RESO_UNIT "2" //default = inch
#define EXIF_DEFAULT_YCBCR_POSITIONING "2" //default = co-sited
#define EXIF_DEFAULT_EXIF_VERSION "0220"
#define EXIF_DEFAULT_FLASH_PIX_VERSION "0100"
#define EXIF_DEFAULT_COMPONENTS_CONFIG "1230"
#define EXIF_DEFAULT_INTEROP_INDEX "R98"
#define EXIF_DEFAULT_INTEROP_VERSION ('0'<<24) + ('0'<<16) + ('1'<<8) + ('0'<<0) //default = "0100"
#define EXIF_DEFAULT_GPS_VERSION "2200" //default = "2200"
#define EXIF_DEFAULT_GPS_MAPDATUM "WGS 84"

#endif

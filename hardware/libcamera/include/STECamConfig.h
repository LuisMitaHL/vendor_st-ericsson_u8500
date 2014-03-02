/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMCONFIG_H
#define STECAMCONFIG_H

//System Include
#include <camera/CameraParameters.h>
#include <OMX_Core.h>

//Multimedia includes
#include <OMX_Symbian_CameraExt_Ste.h>

//For Android pixel format
#include <hardware/hardware.h>

// Possible video rotation modes
#define CAM_VIDEO_ROTATION_LANDSCAPE 1
#define CAM_VIDEO_ROTATION_PORTRAIT 2

// Possible image rotation modes
#define CAM_IMAGE_ROTATION_DEVICE 1
#define CAM_IMAGE_ROTATION_EXIF 2

//Supported Preview sizes
#ifdef STE_SENSOR_8MP
#define CAM_SUPPORTED_PREVIEW_SIZES "1024x768,864x480,960x540,720x480,720x408,640x480,352x288,320x240,176x144"
#else //!STE_SENSOR_8MP
#define CAM_SUPPORTED_PREVIEW_SIZES "1024x768,864x480,720x480,720x408,640x480,352x288,320x240,176x144"
#endif //STE_SENSOR_8MP

/* Extended Preview and Record sizes
 * They cant be part of Supported Previe size because of current duap-pipe implmentation
 * and CTS requirments mis-match
 */
#ifdef ENABLE_HIGH_RESOLUTION_VF
#define CAM_EXTENDED_PREVIEW_SIZES "1600x1200,1920x1080,1280x720"
#else //!ENABLE_HIGH_RESOLUTION_VF
#define CAM_EXTENDED_PREVIEW_SIZES "1920x1080,1280x720"
#endif //ENABLE_HIGH_RESOLUTION_VF

namespace android {

/* Format Info */
struct CameraFormatInfo
{
    /* CTOR */
    CameraFormatInfo() : mStr(NULL),
                         mPreviewOmxColorFormat(OMX_COLOR_FormatUnused),
                         mAndroidPixelColorFormat(0),   //PIXEL_FORMAT_UNKNOWN
                         mRecordOmxColorFormat(OMX_COLOR_FormatUnused),
                         mPreviewStrideFactor(0),
                         mWidthAlignment(0),
                         mHeightAlignment(0) {};

    CameraFormatInfo(const char* aStr,
                     OMX_COLOR_FORMATTYPE aPreviewOmxColorFormat,
                     int aAndroidPixelColorFormat,
                     OMX_COLOR_FORMATTYPE aRecordOmxColorFormat,
                     uint32_t aPreviewStrideFactor,
                     int aWidthAlignment,
                     int aHeightAlignment):
                     mStr(aStr),
                     mPreviewOmxColorFormat(aPreviewOmxColorFormat),
                     mAndroidPixelColorFormat(aAndroidPixelColorFormat),
                     mRecordOmxColorFormat(aRecordOmxColorFormat),
                     mPreviewStrideFactor(aPreviewStrideFactor),
                     mWidthAlignment(aWidthAlignment),
                     mHeightAlignment(aHeightAlignment) {};

    const char* mStr; /**< String */
    OMX_COLOR_FORMATTYPE mPreviewOmxColorFormat; /**< OMX color format */
    int mAndroidPixelColorFormat;   /** < Color format as required by native buffer */
    OMX_COLOR_FORMATTYPE mRecordOmxColorFormat; /**< OMX color format */
    uint32_t mPreviewStrideFactor; /**< Stride factor */
    int mWidthAlignment; /**< Width alignment for OMXCamera */
    int mHeightAlignment; /**< Height Alignment for OMXCamera */
};

#ifdef ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
#define PREVIEW_STRIDE_FACTOR_YUV420PLANAR_FORMATS 1
#else // !ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
#define PREVIEW_STRIDE_FACTOR_YUV420PLANAR_FORMATS 2
#endif //ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS

#if SWCONVERSION_INPUT_FORMAT==_OMX_COLOR_FORMAT16BITRGB565
#define CAM_PREVIEW_FORMAT_FOR_UNSUPPORTED_HWFORMATS OMX_COLOR_Format16bitRGB565
#define CAM_ANDROID_PIXEL_FORMAT    HAL_PIXEL_FORMAT_RGB_565
#else
#define CAM_PREVIEW_FORMAT_FOR_UNSUPPORTED_HWFORMATS OMX_COLOR_FormatCbYCrY
#define CAM_ANDROID_PIXEL_FORMAT    HAL_PIXEL_FORMAT_YCbCr_422_I
#endif //SWCONVERSION_INPUT_FORMAT

static const CameraFormatInfo g_CameraFormatInfo[] = {
    //420SPNV21
    CameraFormatInfo(CameraParameters::PIXEL_FORMAT_YUV420SP,
                     CAM_PREVIEW_FORMAT_FOR_UNSUPPORTED_HWFORMATS,
                     CAM_ANDROID_PIXEL_FORMAT,
                     (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar,
                     PREVIEW_STRIDE_FACTOR_YUV420PLANAR_FORMATS,
                     8,
                     1),
    //420P
    CameraFormatInfo(CameraParameters::PIXEL_FORMAT_YUV420P,
                     CAM_PREVIEW_FORMAT_FOR_UNSUPPORTED_HWFORMATS,
                     CAM_ANDROID_PIXEL_FORMAT,
                     OMX_COLOR_FormatYUV420PackedPlanar,
                     PREVIEW_STRIDE_FACTOR_YUV420PLANAR_FORMATS,
                     8,
                     1),

#if SWCONVERSION_INPUT_FORMAT==_OMX_COLOR_FORMAT16BITRGB565
    //RGB565
    CameraFormatInfo(CameraParameters::PIXEL_FORMAT_RGB565,
                     OMX_COLOR_Format16bitRGB565,
                     HAL_PIXEL_FORMAT_RGB_565,
                     (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar,
                     2,
                     8,
                     1),
#else
    //422I
    CameraFormatInfo(CameraParameters::PIXEL_FORMAT_YUV422I,
                     OMX_COLOR_FormatCbYCrY,
                     HAL_PIXEL_FORMAT_CbYCrY_422_I,
                     (OMX_COLOR_FORMATTYPE)OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar,
                     2,
                     8,
                     1),
#endif
    CameraFormatInfo()
};

struct ThreadData
{
    ThreadData(uint32_t aMaxRequests,
               bool aWaitForCompletion,
               const char* aName,
               int32_t aPriority=PRIORITY_DEFAULT,
               size_t aStack=0) :
               mMaxRequests(aMaxRequests),
               mWaitForCompletion(aWaitForCompletion),
               mName(aName),
               mPriority(aPriority),
               mStack(aStack) {};

    uint32_t mMaxRequests; /**< Max requests that can be queued */
    bool mWaitForCompletion; /**< Wait for completion of each request */
    const char* mName; /**< Thread name */
    int32_t mPriority; /**< Priority */
    size_t mStack; /* Stack size */
};

//Max Requests
static const uint32_t kMaxRequests = 6;

//Thread info
static const ThreadData g_ThreadInfo[] = {
    ThreadData(CAMERA_PREVIEW_BUFFER_COUNT, true, "CameraSwProcessing", PRIORITY_URGENT_DISPLAY, 0), //ESwProcessing
    ThreadData(CAMERA_PREVIEW_BUFFER_COUNT, false, "CameraPreview"), //EPreview
    ThreadData(CAMERA_RECORD_BUFFER_COUNT, false, "CameraRecord", PRIORITY_HIGHEST, 0), //ERecord
    ThreadData(1, false, "CameraAutoFocus", PRIORITY_URGENT_DISPLAY, 0), //EAutoFocus
    ThreadData(kMaxRequests, false, "CameraReqHandler", PRIORITY_HIGHEST, 0), //EReqHandler
};

}

#endif //STECAMCONFIG_H

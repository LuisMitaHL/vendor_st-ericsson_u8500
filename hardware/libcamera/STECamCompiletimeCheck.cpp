/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 *  This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//Internal includes
#define CAM_LOG_TAG "dummy"
#include "STECamTrace.h"
#include "STECamUtils.h"
#include "STECamThreadInfo.h"
#include "STECameraMsg.h"

//System Include
#include <camera/Camera.h>
#include <system/camera.h>

/* Compile time assert fail */
#define CT_FAIL() CAM_ASSERT_COMPILE(0)

/* Only SWISPPROC or HWISPPROC should be defined */
#ifdef HWISPPROC
#ifdef SWISPPROC
#warning "HWISPPROC and SWISPPROC can't be enabled at sametime"
CT_FAIL();
#endif // SWISPPROC
#endif // HWISPPROC

/* JPEGENC can be used alongwith either HWISPPROC or SWISPPROC */
#ifdef JPEGENC
#if !defined(HWISPPROC) && !defined(SWISPPROC)
#warning "JPEGENC can't be used standalone without HWISPPROC or SWISPPROC"
CT_FAIL();
#endif // !HWISPPROC || !SWISPPROC
#endif // JPEGENC

/* HWISPPROC or SWISPPROC can be used only alongwith JPEGENC */
#if defined(HWISPPROC) || defined(SWISPPROC)
#ifndef JPEGENC
#ifdef HWISPPROC
#warning "HWISPPROC can be used only with JPEGENC"
#else // HWISPPROC
#warning "SWISPPROC can be used only with JPEGENC"
#endif // SWISPPROC
CT_FAIL();
#endif // !JPEGENC
#endif // HWISPPROC || HWISPPROC


/* SEND_PREVIEW_EXTRADATA is required only with
 * ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS */
#ifdef SEND_PREVIEW_EXTRADATA
#ifndef ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
#warning "SEND_PREVIEW_EXTRADATA is supported only with ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS"
CT_FAIL();
#endif // !ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
#endif // SEND_PREVIEW_EXTRADATA

//Current rotation routine in landscape re-uses buffer allocated for comversion
#if (ENABLE_VIDEO_ROTATION == CAM_VIDEO_ROTATION_LANDSCAPE)
#ifndef ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
#warning "ENABLE_VIDEO_ROTATION:CAM_VIDEO_ROTATION_LANDSCAPE is supported only with ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS"
CT_FAIL();
#endif // !ENABLE_SW_CONVERSION_OF_YUV420PLANAR_PREVIEW_FORMATS
#endif // ENABLE_VIDEO_ROTATION == CAM_VIDEO_ROTATION_LANDSCAPE

//check all thread info present
CAM_ASSERT_COMPILE(android::ThreadInfo::EMax == ARRAYCOUNT(android::g_ThreadInfo));

//check msg is valid
CAM_ASSERT_COMPILE((int)android::CAMERA_MSG_SHAKE_DETECTED > CAMERA_MSG_ALL_MSGS);


//check video rotation mode is correct
#ifdef ENABLE_VIDEO_ROTATION
CAM_ASSERT_COMPILE((CAM_VIDEO_ROTATION_LANDSCAPE == ENABLE_VIDEO_ROTATION) || (CAM_VIDEO_ROTATION_PORTRAIT == ENABLE_VIDEO_ROTATION));
#endif //ENABLE_VIDEO_ROTATION

/* DEFAULT_CAMERA_MODE shoudl always be defined */
#ifndef DEFAULT_CAMERA_MODE
#warning "DEFAULT_CAMERA_MODE should always be defined"
CT_FAIL();
#endif // !DEFAULT_CAMERA_MODE


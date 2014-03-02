/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//Multimedia includes
#include <OMX_Component.h>
#include <OMX_Symbian_ExtensionNames_Ste.h>

//Internal includes
#define CAM_LOG_TAG "STECamera"
#include "STECamUtils.h"
#include "STECamOmxILExtIndex.h"
#include <IFM_Index.h>
#include "STECamTrace.h"

namespace android {

/* static */ const OMX_STRING OmxILExtIndex::mExtIndexName[] = {
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTCAPTURING_NAME, //ECapturing
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTDIGITALZOOM_NAME, //EDigitalZoom
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_FOCUSRANGE_NAME, // EFocusRange
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTFOCUSSTATUS_NAME, //EFocusStatus
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_FOCUSLOCK_NAME, //EFocusLock
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_FLASHCONTROL_NAME, //EFlashControl
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_FLICKERREMOVAL_NAME, //EFlickerRemoval
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_SCENEMODE_NAME, //ESceneMode
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_SHARPNESS_NAME, //ESharpness
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXPOSUREINITIATED_NAME, //EExposureInitiated
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_PRECAPTUREEXPOSURETIME_NAME, //EPreCaptureExposureTime
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_FOCUSREGION_NAME, //EFocusRegion
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_ROI_NAME, //ERegionOI
    (OMX_STRING)OMX_SYMBIAN_INDEX_PARAM_COMMON_DEVICEMAKEMODEL_NAME, //EMakeModel
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_ORIENTATIONSCENE_NAME, //EOrientationScene
//    (OMX_STRING)OMX_STE_INDEX_CONFIG_COMMON_ZOOMMODE_NAME, //EZoomMode
    (OMX_STRING)OMX_STE_INDEX_CONFIG_COMMON_LENSPARAMETER, //ELensParams
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTRADATA_NAME, //EExtradata
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_DEVICESOFTWARE_NAME, //ESoftware
//    (OMX_STRING)OMX_STE_INDEX_CONFIG_METERINGON_NAME, //EMeteringLock
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTCAPTUREMODE_NAME, //EExtCaptureMode
//    (OMX_STRING)OMX_STE_INDEX_CONFIG_EXIF_TIMESTAMP_NAME, //EExifTimeStamp
//    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_ZOOM_SUPPORTED_RESOLUTIONS, //EMaxZoomSupportedRes
    (OMX_STRING)OMX_SYMBIAN_INDEX_CONFIG_COMMON_EXTFOCUSSTATUS_NAME //EExtFocusStatus
};

#undef _CNAME_
#define _CNAME_ OmxILExtIndex

OmxILExtIndex::OmxILExtIndex(const OMX_HANDLETYPE aCamHandle) :
    mCamHandle(aCamHandle)
{
    DBGT_PROLOG("");

    //check all name exist
    CAM_ASSERT_COMPILE(EMaxType == ARRAYCOUNT(OmxILExtIndex::mExtIndexName));
    //check handle valid
    DBGT_ASSERT(NULL != aCamHandle, "Camera handle null");

    //init all indexes as invalid
    for(uint32_t loop = 0; loop < EMaxType; loop++)
        mIndex[static_cast<Type>(loop)] = OMX_IndexMax;

    DBGT_EPILOG("");
}

OMX_ERRORTYPE OmxILExtIndex::init()
{
    DBGT_PROLOG("");

    //check handle valid
    DBGT_ASSERT(NULL != mCamHandle, "Camera handle null");

    OMX_ERRORTYPE err = OMX_ErrorNone;

    //get all indexes
    for(uint32_t loop = 0; loop < EMaxType; loop++)
    {
        DBGT_CRITICAL("Type: %d String: %s", loop, OmxILExtIndex::mExtIndexName[loop]);
        err = OMX_GetExtensionIndex(mCamHandle, OmxILExtIndex::mExtIndexName[loop], &mIndex[loop]);
        //OMX_SYMBIAN_INDEX_CONFIG_ZOOM_SUPPORTED_RESOLUTIONS is introduced for OVT constrain
        //so it shoud not impact raw bayer camera
        if (!strcmp(OmxILExtIndex::mExtIndexName[loop],OMX_SYMBIAN_INDEX_CONFIG_ZOOM_SUPPORTED_RESOLUTIONS)){
	DBGT_CRITICAL(" err = %#x, OMX_ErrorNotImplemented = %#x", err, OMX_ErrorNotImplemented);
            if(err == OMX_ErrorNotImplemented) err = OMX_ErrorNone;
        }

#ifdef SMOOTH_ZOOM_ENABLED
    if(OMX_ErrorNone != err && OMX_ErrorNotImplemented != err) {
        DBGT_CRITICAL("OMX_GetExtensionIndex failed err = %d", err);
        DBGT_EPILOG("");
        return err;
    }

        // Manage "Not implemented" error as a warning , not as a critical error
        if(err == OMX_ErrorNotImplemented) err = OMX_ErrorNone;
#else
    if(OMX_ErrorNone != err) {
        DBGT_CRITICAL("OMX_GetExtensionIndex failed err = %#x", err);
        DBGT_EPILOG("");
        return err;
    }
#endif //SMOOTH_ZOOM_ENABLED

        DBGT_PTRACE("Index: 0x%08x", mIndex[loop]);
    }

    DBGT_EPILOG("");
    return err;
}

}

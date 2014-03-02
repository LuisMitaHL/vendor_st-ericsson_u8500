/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 *  This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define DBGT_LAYER 1
#define DBGT_PREFIX "DynSet"

//System includes
#include <string.h>

//Internal includes
#define CAM_LOG_TAG "STECamera"
#include "STECamTrace.h"
#include "STECamUtils.h"
#include "STECamDynSetting.h"

//Keys
/* static */ char* const DynSetting::mKey[] = {
    (char*)TRACE_LOGGER_KEY, //ETraceLogger
    (char*)PERF_LOGGER_KEY, //EPerfLogger
    (char*)VF_FPS_LOGGER_KEY, //EVfFpsLogger
    (char*)VIDEO_FPS_LOGGER_KEY, //EVideoFpsLogger
    (char*)OVERLAY_FPS_LOGGER_KEY, //EOverlayFpsLogger
    (char*)XP70_LOGENABLE_KEY, //EXP70LogEnable
    (char*)XP70_LOGLEVEL_KEY, //EXP70LogLevel
    (char*)XP70_LOGMECHANISM_KEY, //EXP70LogMechanism
    (char*)PREVIEW_DUMP_KEY, //EPreviewDump
    (char*)RECORDING_DUMP_KEY, //ERecordingDump
    (char*)PREVIEW_REPLAY_KEY, //EPreviewReplay
    (char*)RECORDING_REPLAY_KEY, //ERecordingReplay
    (char*)"gsm.operator.numeric", //ECountryCode
    (char*)"ro.product.manufacturer", //EManufacturer
    (char*)"ro.product.model", //EModel
    (char*)"ste.cam.encode.fmt", // ERecordPixFmt
    (char*)"ste.cam.ext.encode.fmt", // EExtIspRecordPixFmt
    (char*)"ro.product.software"  //ESoftware
    };

char DynSetting::mValue[EMax][PROPERTY_VALUE_MAX]; /* Value */

DynSetting::DynSetting()
    {
    //Init all values NULL
    for(uint32_t loop = 0; loop < EMax; loop++)
        mValue[loop][0] = 0;

    //init keys
    initKeys();

    //init values
    initValues();

#ifdef CAM_DEBUG
    char value[PROPERTY_VALUE_MAX];
    for(uint32_t loop = 0; loop < EMax; loop++){
        get(static_cast<Type>(loop), value);
        DBGT_PINFO("Key: %s Value: %s", DynSetting::mKey[loop], value);
    }
#endif //CAM_DEBUG

    }

void DynSetting::initKeys()
    {
    //check all keys present
    CAM_ASSERT_COMPILE(EMax == ARRAYCOUNT(DynSetting::mKey));

    //check all keys are correct
    for(uint32_t loop = 0; loop < EMax; loop++){
        DBGT_ASSERT(PROPERTY_KEY_MAX > strlen(DynSetting::mKey[loop]),
                   "Strlen of %s is more then limit: %d",
                   DynSetting::mKey[loop], PROPERTY_KEY_MAX);
    }

    }

void DynSetting::initValues()
    {

#define INIT_VALUE(key, value) set(key, value)

    //init TraceLogger
#ifdef TRACE_LOGGER_VALUE
    INIT_VALUE(ETraceLogger, TRACE_LOGGER_VALUE);
#else // !TRACE_LOGGER_VALUE
    //default is to dump no traces
    INIT_VALUE(ETraceLogger, 8);
#endif // TRACE_LOGGER_VALUE

#ifdef PERF_LOGGER_VALUE
    INIT_VALUE(EPerfLogger, PERF_LOGGER_VALUE);
#endif //PERF_LOGGER_VALUE

#ifdef VF_FPS_LOGGER_VALUE
    INIT_VALUE(EVfFpsLogger, VF_FPS_LOGGER_VALUE);
#endif //VF_FPS_LOGGER_VALUE

#ifdef VIDEO_FPS_LOGGER_VALUE
    INIT_VALUE(EVideoFpsLogger, VIDEO_FPS_LOGGER_VALUE);
#endif //VIDEO_FPS_LOGGER_VALUE

#ifdef OVERLAY_FPS_LOGGER_VALUE
    INIT_VALUE(EOverlayFpsLogger, OVERLAY_FPS_LOGGER_VALUE);
#endif //OVERLAY_FPS_LOGGER_VALUE

#ifdef XP70_LOGENABLE_VALUE
    INIT_VALUE(EXP70LogEnable, XP70_LOGENABLE_VALUE);
#endif //XP70_LOGENABLE_VALUE

#ifdef XP70_LOGLEVEL_VALUE
    INIT_VALUE(EXP70LogLevel, XP70_LOGLEVEL_VALUE);
#endif //XP70_LOGLEVEL_VALUE

#ifdef XP70_LOGMECHANISM_VALUE
    INIT_VALUE(EXP70LogMechanism, XP70_LOGMECHANISM_VALUE);
#endif //XP70_LOGMECHANISM_VALUE

#ifdef DUMP_PREVIEW_FRAMES_VALUE
    INIT_VALUE(EPreviewDump, DUMP_PREVIEW_FRAMES_VALUE);
#endif //DUMP_PREVIEW_FRAMES_VALUE

#ifdef DUMP_RECORD_FRAMES_VALUE
    INIT_VALUE(ERecordingDump, DUMP_RECORD_FRAMES_VALUE);
#endif //DUMP_RECORD_FRAMES_VALUE

#ifdef REPLAY_PREVIEW_FRAMES_VALUE
    INIT_VALUE(EPreviewReplay, REPLAY_PREVIEW_FRAMES_VALUE);
#endif //REPLAY_PREVIEW_FRAMES_VALUE

#ifdef REPLAY_RECORD_FRAMES_VALUE
    INIT_VALUE(ERecordingReplay, REPLAY_RECORD_FRAMES_VALUE);
#endif //REPLAY_RECORD_FRAMES_VALUE

    // intialize the default pixel format for record path
    INIT_VALUE(ERecordPixFmt, "yuv420mb");
    INIT_VALUE(EExtIspRecordPixFmt, "yuv420mb");

    }

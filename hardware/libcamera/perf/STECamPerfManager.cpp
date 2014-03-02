/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
* Defines
*/
#define _CNAME_ PerfManager

//Internal includes
#define CAM_LOG_TAG "STECamera"
#include "STECamTrace.h"
#include "STECamPerfManager.h"

namespace android {

PerfManager::PerfManager() : mCount(0), mInitType(EMaxType)
    {

    }

PerfManager::~PerfManager()
    {
    DBGT_PROLOG("");

    //close all resources
    close();

    DBGT_EPILOG("");
    }

status_t PerfManager::connect()
    {
    DBGT_PROLOG("");

    int rc = NO_ERROR;

    //create timer
    createTimer();

    DBGT_EPILOG("");
    return rc;
    }

void PerfManager::close()
    {
    DBGT_PROLOG("");

    DBGT_EPILOG("");
    }

void PerfManager::createTimer()
    {
    DBGT_PROLOG("");

#ifdef HAVE_POSIX_CLOCKS
    mPerfTimer = static_cast<MPerfTimer*>(&mSystemTimePerfTimer);
#else // !HAVE_POSIX_CLOCKS
    mPerfTimer = static_cast<MPerfTimer*>(&mDurationTimerPerfTimer);
#endif // HAVE_POSIX_CLOCKS

    DBGT_EPILOG("");
    }

const char* PerfManager::name(TType aParam)
    {

#define PERF_TOKEN  "CamPerf"
#define PERF_STRINGIFY_PARAM(aParam) case aParam: return PERF_TOKEN#aParam


    switch ( aParam )
        {

        PERF_STRINGIFY_PARAM(EFirstCameraInstance);
        PERF_STRINGIFY_PARAM(ETakePictureRequested);
        PERF_STRINGIFY_PARAM(ECameraEOS);
        PERF_STRINGIFY_PARAM(EShutterCallback);
        PERF_STRINGIFY_PARAM(EIspProcLREOS);
        PERF_STRINGIFY_PARAM(EIspProcHREOS);
        PERF_STRINGIFY_PARAM(ERawImageCallback);
        PERF_STRINGIFY_PARAM(EArmIVEOS);
        PERF_STRINGIFY_PARAM(ESwJpegEOS);
        PERF_STRINGIFY_PARAM(EJpegFBD);
        PERF_STRINGIFY_PARAM(EJpegEOS);
        PERF_STRINGIFY_PARAM(EBeforeJpegCallback);
        PERF_STRINGIFY_PARAM(EAfterJpegCallback);
        PERF_STRINGIFY_PARAM(EBeforeExifCallback);
        PERF_STRINGIFY_PARAM(EAfterExifCallback);
        PERF_STRINGIFY_PARAM(EExifMixerEOS);
        PERF_STRINGIFY_PARAM(EFirstFrame);
        PERF_STRINGIFY_PARAM(EViewFinderRequested);
        PERF_STRINGIFY_PARAM(EViewFinderStarted);
        PERF_STRINGIFY_PARAM(EViewFinderStopped);
        PERF_STRINGIFY_PARAM(EVideoRequested);
        PERF_STRINGIFY_PARAM(EVideoStarted);
        PERF_STRINGIFY_PARAM(EVideoStopped);
        PERF_STRINGIFY_PARAM(EAutoFocusRequested);
        PERF_STRINGIFY_PARAM(EAutoFocusEvent);
        PERF_STRINGIFY_PARAM(EAutoFocusReached);
        PERF_STRINGIFY_PARAM(EExposureInitiatedEvent);
        PERF_STRINGIFY_PARAM(EShakeDetectionRequested);
        PERF_STRINGIFY_PARAM(EPreCaptureExposureTimeEvent);
        PERF_STRINGIFY_PARAM(EShakeDetected);
        PERF_STRINGIFY_PARAM(EMeteringEvent);
        PERF_STRINGIFY_PARAM(EDummy);
        default:
            DBGT_CRITICAL("Cant Happen");
            return "unknown";
        }
#undef PERF_TOKEN
#undef PERF_STRINGIFY_PARAM

    }

} //namespace android

/* End of STECamPerfManager.cpp */

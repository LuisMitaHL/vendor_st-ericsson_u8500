/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STECAMPERFMANAGER_H_
#define _STECAMPERFMANAGER_H_

/*
 * Includes
 */
#include "STECamPerfTimer.h"
#include "STECamDynSetting.h"

namespace android {

class PerfManager
    {
    public:
        /* Perf Trace type */
        enum TType
            {
            EFirstCameraInstance,
            ETakePictureRequested,
            ECameraEOS,
            EShutterCallback,
            EIspProcLREOS,
            EIspProcHREOS,
            ERawImageCallback,
            EArmIVEOS,
            ESwJpegEOS,
            EJpegFBD,
            EJpegEOS,
            EBeforeJpegCallback,
            EAfterJpegCallback,
            EBeforeExifCallback,
            EAfterExifCallback,
            EExifMixerEOS,
            EFirstFrame,
            EViewFinderRequested,
            EViewFinderStarted,
            EViewFinderStopped,
            EVideoRequested,
            EVideoStarted,
            EVideoStopped,
            EAutoFocusRequested,
            EAutoFocusEvent,
            EAutoFocusReached,
            EShakeDetectionRequested,
            EPreCaptureExposureTimeEvent,
            EShakeDetected,
            EExposureInitiatedEvent,
            EDummy,
            EMeteringEvent,
            EMaxType
            };

    public:
        /**< Constructor */
        PerfManager();

        /**< Destructor */
        ~PerfManager();

        /**< Allocate all resources */
        status_t connect();

        /**< Close all resources */
        void close();

    public:
        /**< Init */
        inline void init(const TType aType);

        /**< Log and dump */
        inline void logAndDump(const TType aType);

    private:
        /**< CreateTimer */
        void createTimer();

        /**< To get type name */
        const char* name(TType aParam);

    private:
        uint32_t                mCount; /**< Count of traces */
        TType                   mInitType; /**< Init type */
        MPerfTimer*             mPerfTimer; /**< Perf timer */
        MPerfTimer::Time        mLastTime; /**< Last time */
        DurationTimerPerfTimer  mDurationTimerPerfTimer; /**< Duratiom Timer based perf timer */
        SystemTimePerfTimer     mSystemTimePerfTimer; /**< Duratiom Timer based perf timer */
        char                    mCurTimeStr[MPerfTimer::ETimeStrLen]; /**< Curent time str */
        char                    mDiffTimeStr[MPerfTimer::ETimeStrLen]; /**< Curent time str */
    };

#include "STECamPerfManager.inl"

} //namespace android

#endif // _STECAMPERFMANAGER_H_

/* End of STECamPerfManager.h */

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMDYNSETTING_H
#define STECAMDYNSETTING_H

//System includes
#include <cutils/properties.h>
#include <stdlib.h>
#include <stdio.h>

class DynSetting
    {
    public:
        enum Type
            {
            ETraceLogger, /**< Trace logging */
            EPerfLogger, /**< Perf logging */
            EVfFpsLogger, /**< VF Fps */
            EVideoFpsLogger, /**< Video Fps */
            EOverlayFpsLogger, /**< Overlay Fps */
            EXP70LogEnable, /**< XP70 log enable */
            EXP70LogLevel, /**< XP70 log level */
            EXP70LogMechanism, /**< XP70 log mechnism */
            EPreviewDump, /**< Preview Frames dumping*/
            ERecordingDump, /**< Recording Frames dumping*/
            EPreviewReplay, /**< Preview Frames replay*/
            ERecordingReplay, /**< Recording Frames replay*/
            ECountryCode,  /**< CountryCode for Flicker Update>*/
            EManufacturer, /**< Manufacturer used in EXIF tags */
            EModel, /**< Model used in EXIF tags */
            ERecordPixFmt, /**< pixel format of record path */
            EExtIspRecordPixFmt, /**< pixel format of record path (ext isp) */
            ESoftware, /**< Software version used in EXIF tags */
            EMax /**< WaterMark */
            };

        /* Constructor */
        DynSetting();

        /* Get int value */
        static inline int get(Type type);

        /* Get char value */
        static inline void get(Type type, char *value);

        /* Set int value */
        static inline void set(Type type, int value);

        /* Set char value */
        static inline void set(Type type, const char *value);

    private:
        /* Init keys */
        void initKeys();

        /* Init values */
        void initValues();

    private:
        static char* const  mKey[]; /* Keys */
        static char mValue[EMax][PROPERTY_VALUE_MAX]; /* Value */
    };

#include "STECamDynSetting.inl"

#endif // STECAMDYNSETTING_H


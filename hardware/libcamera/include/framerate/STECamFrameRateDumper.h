/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMFRAMERATEDUMPER_H
#define STECAMFRAMERATEDUMPER_H

//System includes
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

//Internla Includes
#include "STECamDynSetting.h"

class FrameRateDumper
    {
    public:
        /**< Default constructor */
        FrameRateDumper();

        /**< Constructor with info */
        FrameRateDumper(const char* dumpString,
                        DynSetting::Type aType);

        /**< Log and dump framerate */
        void logAndDump();

    public:
        /**< Set DumpString */
        inline void setDumpString(const char* aDumpString);

        /**< Set DumpCount */
        inline void setDumpCount(const uint32_t aDumpCount);

        /**< Set DumpCount from DynSetting */
        inline void setDumpCount();

        /**< Start */
        inline void start();

        /**< Reset */
        inline void reset();

        /**< Dump framerate */
        inline void dump();

    private:
        enum 
            {
            EMaxLengthDumpString = 32
            };

        struct timeval              mOldTime; /**< Old time */
        struct timeval              mNewTime; /**< New time */
        uint32_t                    mCount; /**< Current count */
        uint32_t                    mDumpCount; /**< Dump count */
        char                        mDumpString[EMaxLengthDumpString]; /**< Dump string */
        DynSetting::Type            mType; /**< Type */
    };

#include "STECamFrameRateDumper.inl"

#endif // STECAMFRAMERATEDUMPER_H


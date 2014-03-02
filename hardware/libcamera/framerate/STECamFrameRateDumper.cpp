/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Code adapted for usage of OMX components. All functions
 * using OMX functionality are under copyright of ST-Ericsson
 *
 *  This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define DBGT_LAYER 2
#define DBGT_PREFIX "FpsDmp"

//Internal includes
#include "STECamTrace.h"
#include "STECamFrameRateDumper.h"

#undef _CNAME_
#define _CNAME_ FrameRateDumper

FrameRateDumper::FrameRateDumper() :
    mDumpCount(0), mType(DynSetting::EMax)
    {

    //do nothing

    }

FrameRateDumper::FrameRateDumper(const char* dumpString,
                DynSetting::Type aType) : mType(aType)
    {

    //set string
    setDumpString(dumpString);

    //set dump count
    setDumpCount();
    }

void FrameRateDumper::logAndDump()
    {
    DBGT_PROLOG("");

    //get new time diff
    gettimeofday(&mNewTime, NULL);

    //increment count
    mCount++;

    DBGT_PTRACE("Count: %u Duration sec: %ld  usecs: %ld", mCount, mNewTime.tv_sec, mNewTime.tv_usec);

    //dump if count reached
    if ( mCount == mDumpCount )
        dump();

    DBGT_EPILOG("");
    return;
    }

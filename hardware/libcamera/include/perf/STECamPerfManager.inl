/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STECAMPERFMANAGER_INL_
#define _STECAMPERFMANAGER_INL_

inline void PerfManager::init(const TType aType)
    {
#ifdef DUMP_PERF_TRACES
        if(DynSetting::get(DynSetting::EPerfLogger) > 0) {

            DBGT_ASSERT(aType < EMaxType, "Invalid type: %d", aType);
            DBGT_ASSERT(NULL != mPerfTimer, "mPerfTimer NULL");

            //init timer
            mPerfTimer->init();

            //init params
            mCount = 0;
            mInitType = aType;
            mLastTime = 0;

            MPerfTimer::getStr(mLastTime, mCurTimeStr);
            DBGT_PINFO("%s%s", mCurTimeStr, name(aType));
        }

#endif //DUMP_PERF_TRACES
    }

inline void PerfManager::logAndDump(const TType aType)
    {
#ifdef DUMP_PERF_TRACES
        if(DynSetting::get(DynSetting::EPerfLogger) > 0) {

            DBGT_ASSERT(aType < EMaxType, "Invalid type: %d", aType);
            DBGT_ASSERT(NULL != mPerfTimer, "mPerfTimer NULL");

            //increment count
            mCount++;

            //get current time
            MPerfTimer::Time curTime = mPerfTimer->elapsedTime();

            //get diff
            MPerfTimer::Time diffTime = curTime - mLastTime;

            //update last
            mLastTime = curTime;

            //get strings
            MPerfTimer::getStr(curTime, mCurTimeStr);
            MPerfTimer::getStr(diffTime, mDiffTimeStr);

            DBGT_PINFO("%s%s%s", mCurTimeStr, mDiffTimeStr, name(aType));
        }
#endif //DUMP_PERF_TRACES
    }

#endif // end of file STECampPerfManager.inl

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMFRAMERATEDUMPER_INL
#define STECAMFRAMERATEDUMPER_INL

inline void FrameRateDumper::setDumpString(const char* dumpString)
    {
    //dump string valid
    DBGT_ASSERT( NULL != dumpString, "DumpString null");
    //length valid
    DBGT_ASSERT( strlen(dumpString) <= EMaxLengthDumpString, "Sting len: %d more then : %d",
          strlen(dumpString), EMaxLengthDumpString);

    //copy string
    strcpy(mDumpString, dumpString);
    }

inline void FrameRateDumper::setDumpCount(const uint32_t dumpCount)
    {
    //check count more then zero
    DBGT_ASSERT(dumpCount > 0, "Dumpcount zero");
    DBGT_ASSERT(mType < DynSetting::EMax, "Type: %d is invalid", mType);

    //update dump count
    mDumpCount = dumpCount;
    DynSetting::set(mType, mDumpCount);
    }

inline void FrameRateDumper::setDumpCount()
    {
    DBGT_ASSERT(mType < DynSetting::EMax, "Type: %d is invalid", mType);
    mDumpCount = DynSetting::get(mType);
    }

inline void FrameRateDumper::start()
    {
    reset();

    //setup new dump count
    setDumpCount();
    }

inline void FrameRateDumper::reset()
    {
    //Reset count
    mCount=0;
    //Reset count
    mNewTime.tv_sec = 0;
    mNewTime.tv_usec = 0;
    //init old time
    gettimeofday(&mOldTime, NULL);
    }

inline void FrameRateDumper::dump()
    {
    //check dump string exists
    DBGT_ASSERT(0 != strlen(mDumpString), "String length zero");

    long long newTime  = ((long long) mNewTime.tv_sec) * 1000000LL +
        ((long long) mNewTime.tv_usec);
    long long oldTime = ((long long) mOldTime.tv_sec) * 1000000LL +
        ((long long) mOldTime.tv_usec);

    float averageTime =  ( (newTime - oldTime) / 1000.0f ) / mCount;

    DBGT_PINFO("%s: %f ms/frame (%f fps)",mDumpString, averageTime,
            1000.0f / averageTime );

    reset();
    }

#endif // STECAMFRAMERATEDUMPER_INL

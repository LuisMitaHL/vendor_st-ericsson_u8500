/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STECAMPERFTIMER_INL_
#define _STECAMPERFTIMER_INL_

/* static */ inline void MPerfTimer::getStr(const Time& aTime, char* aStr)
{
    DBGT_ASSERT(NULL != aStr, "aStr NULL");

    int num = snprintf(aStr, ETimeStrLen, "[%0*lldus]", ETimePrintWidth, aTime);
    DBGT_ASSERT(num < ETimeStrLen, "Num of characters : %d is more then TimeStrLen: %d", num, ETimeStrLen);
}

#endif // end of file STECampPerfManager.inl

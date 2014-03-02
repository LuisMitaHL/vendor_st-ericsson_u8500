/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMTHREADINFO_INL
#define STECAMTHREADINFO_INL

inline uint32_t ThreadInfo::maxRequests(Type aType) const
{
    DBGT_PROLOG("Type: %d", aType);
    DBGT_ASSERT(aType < EMax, "Type: %d invalid", aType);

	DBGT_EPILOG("%d", g_ThreadInfo[aType].mMaxRequests);
	return g_ThreadInfo[aType].mMaxRequests;
}

inline bool ThreadInfo::waitForCompletion(Type aType) const
{
    DBGT_PROLOG("Type: %d", aType);
    DBGT_ASSERT(aType < EMax, "Type: %d invalid", aType);

	DBGT_EPILOG("%d", g_ThreadInfo[aType].mWaitForCompletion);
    return g_ThreadInfo[aType].mWaitForCompletion;
}

inline const char* ThreadInfo::name(Type aType) const
{
    DBGT_PROLOG("Type: %d", aType);
    DBGT_ASSERT(aType < EMax, "Type: %d invalid", aType);

    DBGT_EPILOG("");
    return  g_ThreadInfo[aType].mName;
}

inline int32_t ThreadInfo::priority(Type aType) const
{
    DBGT_PROLOG("Type: %d", aType);
    DBGT_ASSERT(aType < EMax, "Type: %d invalid", aType);

	DBGT_EPILOG("%d", g_ThreadInfo[aType].mPriority);
    return g_ThreadInfo[aType].mPriority;
}

inline size_t ThreadInfo::stackSize(Type aType) const
{
    DBGT_PROLOG("Type: %d", aType);
    DBGT_ASSERT(aType < EMax, "Type: %d invalid", aType);

	DBGT_EPILOG("%d", g_ThreadInfo[aType].mPriority);
    return g_ThreadInfo[aType].mPriority;
}

#endif // STECAMTHREADINFO_INL

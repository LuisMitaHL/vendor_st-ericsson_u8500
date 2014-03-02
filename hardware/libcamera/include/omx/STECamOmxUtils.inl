/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMOMXUTILS_INL
#define STECAMOMXUTILS_INL
#include "STECamTrace.h"

inline OMXCoreInterface* OmxUtils::interface()
{
    //NULL checking
    DBGT_ASSERT(NULL != mDlHandle, "NULL DlHandle");
    DBGT_ASSERT(NULL != mInterface, "NULL Interface");
    return mInterface;
}

template <class TStruct>
inline /*static */ void OmxUtils::StructWrapper<TStruct>::init(TStruct& aStruct,
                                                               OMX_BOOL aMemset /*= OMX_TRUE*/,
                                                               OMX_BOOL aInitWithZero /*= OMX_FALSE*/)
{
    //reset all variable
    if(aMemset){
        char fillChar = kFillStructDebugChar;

        if(aInitWithZero)
            fillChar = 0;

        memset(&aStruct, fillChar, sizeof(aStruct));
    }

    //fill version/size
    aStruct.nVersion.nVersion = OmxUtils::kVersion;
    aStruct.nSize = sizeof(aStruct);
}

template <class TStruct>
inline OmxUtils::StructContainer<TStruct>::StructContainer(OMX_BOOL aInitWithZero /*= OMX_FALSE*/)
{
    StructWrapper<TStruct>::init(mStruct, OMX_TRUE, aInitWithZero);
}

template <class TStruct>
inline void OmxUtils::StructContainer<TStruct>::reInit(OMX_BOOL aInitWithZero /*= OMX_FALSE*/)
{
    StructWrapper<TStruct>::init(mStruct, OMX_FALSE, aInitWithZero);
}

template <class TStruct>
inline TStruct& OmxUtils::StructContainer<TStruct>::ref()
{
    return mStruct;
}

template <class TStruct>
inline TStruct* OmxUtils::StructContainer<TStruct>::ptr()
{
    return &mStruct;
}

#endif // STECAMOMXUTILS_INL

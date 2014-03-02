/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMOMXILEXTINDEX_INL
#define STECAMOMXILEXTINDEX_INL

inline OMX_INDEXTYPE OmxILExtIndex::getIndex(Type aType) const
{
    DBGT_ASSERT(EMaxType > aType,
               "Index: %d out of range, max index: %d", aType, EMaxType);
    DBGT_ASSERT(OMX_IndexMax != mIndex[aType],
               "Index not initialized for type: %d", aType);

    return mIndex[aType];
}

#endif // STECAMOMXILEXTINDEX_INL

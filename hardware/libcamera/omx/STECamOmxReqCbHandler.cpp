/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//Internal includes
#define CAM_LOG_TAG "STECamera"
#include "STECamOmxReqCbHandler.h"

namespace android {

#undef _CNAME_
#define _CNAME_ OmxReqCbHandler

OmxReqCbHandler::OmxReqCbHandler()
{
    DBGT_PROLOG("");

    //init all indexes as invalid
    for(uint32_t loop = 0; loop < EMaxType; loop++)
        mHandle[static_cast<Type>(loop)] = NULL;

    DBGT_EPILOG("");
}

void OmxReqCbHandler::init(Type aType, OMX_HANDLETYPE aHandle, OMX_U32 aPort, OMX_INDEXTYPE aIndex)
{
    DBGT_PROLOG("Type: %d Handle: %p Port: %lu Index: %d", aType, aHandle, aPort, aIndex);

    //check handle valid
    DBGT_ASSERT(EMaxType > aType,
               "Index: %d out of range, max index: %d", aType, EMaxType);
    DBGT_ASSERT(NULL != mHandle, "Handle null");
    DBGT_ASSERT(OMX_IndexMax > aIndex, "Invalid index: 0x%08x", aIndex);

    //save handle
    mHandle[aType] = aHandle;
    mEnabled[aType] = false;

    //save req params
    mReq[aType].ptr()->nPortIndex = aPort;
    mReq[aType].ptr()->nIndex = aIndex;

    DBGT_EPILOG("");
    return;
}

}

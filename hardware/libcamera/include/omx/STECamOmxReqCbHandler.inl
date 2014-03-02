/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMOMXREQCBHANDLER_INL
#define STECAMOMXREQCBHANDLER_INL

inline OMX_ERRORTYPE OmxReqCbHandler::enable(Type aType)
{

    return update(aType, OMX_TRUE);
}

inline OMX_ERRORTYPE OmxReqCbHandler::disable(Type aType)
{

    return update(aType, OMX_FALSE);
}

inline bool OmxReqCbHandler::isEnabled(Type aType) const
{
    DBGT_ASSERT(EMaxType > aType,
               "Index: %d out of range, max index: %d", aType, EMaxType);

    return mEnabled[aType];
}

inline OMX_ERRORTYPE OmxReqCbHandler::update(Type aType, OMX_BOOL aEnable)
{
    DBGT_ASSERT(EMaxType > aType,
               "Index: %d out of range, max index: %d", aType, EMaxType);

    DBGT_ASSERT(NULL != mHandle[aType], "Handle NULL");

    mEnabled[aType] = aEnable;
    mReq[aType].ptr()->bEnable = aEnable;
    OMX_ERRORTYPE err = OMX_SetConfig(mHandle[aType],
                                      static_cast<OMX_INDEXTYPE>(OMX_IndexConfigCallbackRequest),
                                      mReq[aType].ptr());

    return err;
}

#endif // STECAMOMXREQCBHANDLER_INL

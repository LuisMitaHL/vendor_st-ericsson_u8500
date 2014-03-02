/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMOMXTHREAD_INL
#define STECAMOMXTHREAD_INL

status_t OmxThread<OmxBuffInfo>::handleRequest(OmxBuffInfo& aData)
{
    DBGT_PROLOG("Buffinfo: %p", &aData);

    //check valid buffer
    checkBufferValid(aData);

    OMX_BOOL needFlagUpdation = OMX_TRUE;

    status_t err = handleBuffer(aData, needFlagUpdation);

    //update flags
    if(needFlagUpdation)
        updateFlags(aData);

	DBGT_EPILOG("");
	return (int)err;
}

status_t OmxThread<OmxBuffInfo>::handleFlush(OmxBuffInfo& aData)
{
    DBGT_PROLOG("Buffinfo: %p", &aData);

    //check valid buffer
    checkBufferValid(aData);

    //update flags
    updateFlags(aData);
	
	DBGT_EPILOG("");
	return (int)NO_ERROR;
}

inline void OmxThread<OmxBuffInfo>::checkBufferValid(OmxBuffInfo& aData)
{
    //check correct buffer coming
    DBGT_ASSERT( (aData.mFlags & OmxBuffInfo::ESentToHandlerThread),
               "Invlid buffer got Flags: 0x%08x", aData.mFlags);

    DBGT_ASSERT( (aData.mFlags & OmxBuffInfo::ESentToComp),
               "Invlid buffer got, not sent to comp Flags: 0x%08x", aData.mFlags);
    //reset flag
    aData.mFlags &= ~OmxBuffInfo::ESentToComp;
}

inline void OmxThread<OmxBuffInfo>::updateFlags(OmxBuffInfo& aData)
{
    //reset flag
    aData.mFlags &= ~OmxBuffInfo::ESentToHandlerThread;
}

#endif // STECAMOMXTHREAD_INL

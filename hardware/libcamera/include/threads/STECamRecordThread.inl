/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMRECORDTHREAD_INL
#define STECAMRECORDTHREAD_INL

inline RecordThread<OmxBuffInfo>::RecordThread(STECamera* aParent) :
    mParent(aParent)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

status_t RecordThread<OmxBuffInfo>::handleBuffer(OmxBuffInfo& aData, OMX_BOOL& aUpdateFlags)
{
    DBGT_PROLOG("UpdateFlags: %d", aUpdateFlags);

    mParent->doRecordProcessing(aData);

	DBGT_EPILOG("");
	return (int)NO_ERROR;
}

#endif // STECAMRECORDTHREAD_INL

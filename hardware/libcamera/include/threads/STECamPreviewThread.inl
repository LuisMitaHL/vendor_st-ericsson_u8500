/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMPREVIEWTHREAD_INL
#define STECAMPREVIEWTHREAD_INL

inline PreviewThread<OmxBuffInfo>::PreviewThread(STECamera* aParent) :
    mParent(aParent)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

status_t PreviewThread<OmxBuffInfo>::handleBuffer(OmxBuffInfo& aData, OMX_BOOL& aUpdateFlags)
{
    DBGT_PROLOG("UpdateFlags: %d", aUpdateFlags);

    mParent->doPreviewProcessing(aData, aUpdateFlags);

	DBGT_EPILOG("");
	return (int)NO_ERROR;
}

#endif // STECAMPREVIEWTHREAD_INL

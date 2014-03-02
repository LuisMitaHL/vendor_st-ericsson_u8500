/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMAUTOFOCUSTHREAD_INL
#define STECAMAUTOFOCUSTHREAD_INL

inline AutoFocusThread<AutoFocusThreadData>::AutoFocusThread(STECamera* aParent) :
    mParent(aParent)
{
    DBGT_PROLOG("");

    DBGT_EPILOG("");
}

status_t AutoFocusThread<AutoFocusThreadData>::handleRequest(AutoFocusThreadData& aData)
{
    DBGT_PROLOG("Enabled: %d", aData.mEnabled);

    mParent->doAutoFocusProcessing(aData);

	DBGT_EPILOG("");
	return (int)NO_ERROR;
}

#endif // STECAMAUTOFOCUSTHREAD_INL

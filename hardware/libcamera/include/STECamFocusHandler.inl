/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STECAMFOCUSHANDLER_INL_
#define _STECAMFOCUSHANDLER_INL_

inline float FocusHandler::getFocusDistance() 
{
    DBGT_PROLOG("");

    if (0 < mExtFocusStatus.mFocusStatus.sAFROIs[0].xFocusDistance) {
        DBGT_PTRACE("FocusDistance = %f", (float)mExtFocusStatus.mFocusStatus.sAFROIs[0].xFocusDistance/0x10000);
		DBGT_EPILOG("");
		return (float)mExtFocusStatus.mFocusStatus.sAFROIs[0].xFocusDistance/0x10000;
	}

	DBGT_EPILOG("FocusDistance = %f", 0.0);
	return 0.0;
}
#endif // _STECAMFOCUSHANDLER_INL_

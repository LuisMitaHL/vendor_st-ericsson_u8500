/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMAUTOFOCUSTHREADATA_H
#define STECAMAUTOFOCUSTHREADATA_H

namespace android {
enum continuousFocusState {
    ETrackingState = 0, /* TRACKING means sw3a is running focus, lens movement is still going on */
    EFocusReached = 1,
    EFocusUnableToReach = 2,
    EStopped = 3
};
struct AutoFocusThreadData
{
    volatile OMX_BOOL mEnabled;
    volatile int mContinuousFocusState;
};

};

#endif //STECAMAUTOFOCUSTHREADATA_H
/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_hostnmf_cb.h"
#include "ext_camera.h"

void  CHostNmf_CB::emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer)
{
    mCam->fillBufferDone((OMX_BUFFERHEADERTYPE *) buffer);
}

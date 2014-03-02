/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "hostnmf_cb.h"
#include "camera.h"

ENS_API_EXPORT void CHostNmf_CB::emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) 	{

    mCam->fillBufferDone((OMX_BUFFERHEADERTYPE *) buffer);
}

/*
 * Copyright (C) ST-Ericsson SA 2011 All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _EXT_CAPTURE_CONTEXT_H_
#define _EXT_CAPTURE_CONTEXT_H_

#include "OMX_Core.h"
#include "IFM_Types.h"

class CCapture_context
{
public :
    CCapture_context();
    OMX_BUFFERHEADERTYPE* ppOmxBufHdr[16]; /* pointer to fillBufferDoneVPB1 OMX buffer */
    OMX_U32 nfillBufferDoneCount;
};
#endif /* _EXT_CAPTURE_CONTEXT_H_ */

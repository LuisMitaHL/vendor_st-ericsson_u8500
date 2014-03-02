/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _CAPTURE_CONTEXT_H_
#define _CAPTURE_CONTEXT_H_

#include "OMX_Core.h"
#include "IFM_Types.h"
			
#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CCapture_context);
#endif
class CCapture_context
{
    public :
        CCapture_context();
        
        OMX_ERRORTYPE pushOmxBufHdr(OMX_BUFFERHEADERTYPE* pOmxBuf);
        OMX_ERRORTYPE popOmxBufHdr(OMX_BUFFERHEADERTYPE** pOmxBuf);

        IFM_BMS_CAPTURE_CONTEXT nBMS_capture_context;
        
    private :
        #define MAX_ALLOCATED_BUFFER_NB 10
        OMX_BUFFERHEADERTYPE* ppOmxBufHdr[MAX_ALLOCATED_BUFFER_NB]; /* pointer to fillBufferDoneVPB1 OMX buffer */
        OMX_U32 nOmxBufHdrReadCount;
        OMX_U32 nOmxBufHdrWriteCount;
};
#endif /**/

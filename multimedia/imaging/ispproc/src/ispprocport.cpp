/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "ispprocport.h"
#include "osi_trace.h"
#include "grabctlcommon.h"
#include "IFM_Types.h"
#include "extradata.h"


void ispprocport::setDefaultFormatInPortDefinition(OMX_PARAM_PORTDEFINITIONTYPE * defaultDef)
{
/*
typedef struct OMX_VIDEO_PORTDEFINITIONTYPE {
    OMX_STRING cMIMEType;
    OMX_NATIVE_DEVICETYPE pNativeRender;
    OMX_U32 nFrameWidth;
    OMX_U32 nFrameHeight;
    OMX_S32 nStride;
    OMX_U32 nSliceHeight;
    OMX_U32 nBitrate;
    OMX_U32 xFramerate;
    OMX_BOOL bFlagErrorConcealment;
    OMX_VIDEO_CODINGTYPE eCompressionFormat;
    OMX_COLOR_FORMATTYPE eColorFormat;
    OMX_NATIVE_WINDOWTYPE pNativeWindow;
} OMX_VIDEO_PORTDEFINITIONTYPE;
*/	
	
	mParamPortDefinition.format.video.cMIMEType =NULL;
	mParamPortDefinition.format.video.pNativeRender = 0 ;// not used
	mParamPortDefinition.format.video.nFrameWidth = defaultDef->format.video.nFrameWidth;
	mParamPortDefinition.format.video.nFrameHeight = defaultDef->format.video.nFrameHeight;	
	mParamPortDefinition.format.video.nSliceHeight = defaultDef->format.video.nFrameHeight;
	mParamPortDefinition.format.video.nBitrate = 0;	
	mParamPortDefinition.format.video.xFramerate = defaultDef->format.video.xFramerate;
	mParamPortDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
	mParamPortDefinition.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE)OMX_IMAGE_CodingUnused;
	mParamPortDefinition.format.video.eColorFormat = defaultDef->format.video.eColorFormat;
	mParamPortDefinition.format.video.pNativeWindow = 0;
	mParamPortDefinition.bBuffersContiguous = OMX_TRUE;

	mInternalFrameWidth = defaultDef->format.video.nFrameWidth;
	mInternalFrameHeight = defaultDef->format.video.nFrameHeight;
	
	mBytesPerPixel = getPixelDepth(mParamPortDefinition.format.video.eColorFormat);
		
    mParamPortDefinition.format.video.nStride = getStride(mParamPortDefinition.format.video.eColorFormat, mParamPortDefinition.format.video.nFrameWidth, NULL);

    OMX_U32 bufferSize = getBufferSize(mParamPortDefinition.format.video.eColorFormat, mParamPortDefinition.format.video.nStride,
                                                             mParamPortDefinition.format.video.nFrameWidth, mParamPortDefinition.format.video.nFrameHeight);
	bufferSize += Extradata::GetExtradataSize(Component_ISPProc, defaultDef->nPortIndex);
	if (bufferSize == 0) {
		DBC_ASSERT(0);
	}
    
	mParamPortDefinition.nBufferSize = (bufferSize+0x1000) & 0xFFFFF000; // round to next 4K    

}


OMX_ERRORTYPE ispprocport::checkFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
{
		/* Check Format */
		if(isSupportedFmt(portdef.nPortIndex, portdef.format.video.eColorFormat) != OMX_TRUE) return OMX_ErrorBadParameter;
		return OMX_ErrorNone;
}


OMX_ERRORTYPE ispprocport::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
{
    IN0("\n");

    t_uint16 mStride = 1; 
    t_uint16 mWidth = 0;
    t_uint16 mHeight = 0;
    OMX_S32 stride = 0;
    OMX_U32 bufferSize = 0;

    OMX_ERRORTYPE error = checkFormatInPortDefinition(portdef);
    if(error != OMX_ErrorNone) return error;

    if((portdef.nPortIndex==ISPPROC_PORT_OUT0)||(portdef.nPortIndex==ISPPROC_PORT_OUT1))
    {
        /* check hw constraints */
        getHwConstraints(portdef.nPortIndex, portdef.format.video.eColorFormat,&mWidth, &mHeight, &mStride);

        /* the stride and sliceHeight overridden by client must respect HW constraints */
        if(portdef.format.video.nStride%mStride !=0) return OMX_ErrorBadParameter;
        if(portdef.format.video.nSliceHeight%mHeight !=0) return OMX_ErrorBadParameter;

        /* if width/height set by client is not compliant to HW constraints, handle those internally.
        Note that port configuration seen by client (getParameter) is based on the (potentially unaligned) 
        values set by the client ! */  
        if (portdef.format.video.nFrameWidth%mWidth !=0) {
            mInternalFrameWidth = portdef.format.video.nFrameWidth + (mWidth-(portdef.format.video.nFrameWidth%mWidth));
        }
        else {
            mInternalFrameWidth = portdef.format.video.nFrameWidth;
        }

        if (portdef.format.video.nFrameHeight%mHeight !=0) {
            mInternalFrameHeight = portdef.format.video.nFrameHeight + (mHeight-(portdef.format.video.nFrameHeight%mHeight));
        }
        else {
            mInternalFrameHeight = portdef.format.video.nFrameHeight;
    }
    }
    else
    {
        mInternalFrameWidth = portdef.format.video.nFrameWidth;
        mInternalFrameHeight = portdef.format.video.nFrameHeight;   
    }

    mParamPortDefinition.format.video.xFramerate = portdef.format.video.xFramerate;
    mParamPortDefinition.format.video.nFrameWidth = portdef.format.video.nFrameWidth;
    mParamPortDefinition.format.video.nFrameHeight = portdef.format.video.nFrameHeight;
    mParamPortDefinition.format.video.pNativeRender = portdef.format.video.pNativeRender ;// not used

    mParamPortDefinition.format.video.bFlagErrorConcealment = portdef.format.video.bFlagErrorConcealment;
    mParamPortDefinition.format.video.eCompressionFormat = portdef.format.video.eCompressionFormat;
    mParamPortDefinition.format.video.eColorFormat = portdef.format.video.eColorFormat;
    mParamPortDefinition.format.video.pNativeWindow = portdef.format.video.pNativeWindow;
    mBytesPerPixel = getPixelDepth(mParamPortDefinition.format.video.eColorFormat);

    /* Calculate stride */
    stride = getStride(portdef.format.video.eColorFormat, mInternalFrameWidth, NULL);
    /* check the sanity of the client-provided stride value */
    if(portdef.format.video.nStride > stride) {
        mParamPortDefinition.format.video.nStride = portdef.format.video.nStride;
    }
    else {
        mParamPortDefinition.format.video.nStride = stride;
	 /* make sure it complies with HW constraints */
	 if((mParamPortDefinition.format.video.nStride % mStride) != 0) {
            mParamPortDefinition.format.video.nStride += (mStride-(mParamPortDefinition.format.video.nStride%mStride));
        }
    }

    /* check the sanity of the client-provided nSliceHeight value */
    if (portdef.format.video.nSliceHeight > mInternalFrameHeight) {
        mParamPortDefinition.format.video.nSliceHeight = portdef.format.video.nSliceHeight;
    } else {
        mParamPortDefinition.format.video.nSliceHeight = mInternalFrameHeight;
    }

    bufferSize = getBufferSize(mParamPortDefinition.format.video.eColorFormat, mParamPortDefinition.format.video.nStride,
		                             mParamPortDefinition.format.video.nFrameWidth, mParamPortDefinition.format.video.nSliceHeight);
    bufferSize += Extradata::GetExtradataSize(Component_ISPProc, portdef.nPortIndex);

    if (bufferSize == 0) {
        DBC_ASSERT(0); //frameWidth and height have been overwritten, assert
    }

    mParamPortDefinition.nBufferSize = (bufferSize+0x1000) & 0xFFFFF000; // round to next 4K

    portSettingsChanged = OMX_TRUE;

    OUTR(" ",OMX_ErrorNone);
    return OMX_ErrorNone;
}




void ispprocport::getHwConstraints(t_uint16 portId,OMX_COLOR_FORMATTYPE omxformat, t_uint16 * p_multiple_width, t_uint16 * p_multiple_height, t_uint16 * p_multiple_stride) 
{

	/* table coming from hardware specification */
	/* return value is multiple of pixels */
	switch(portId) {
		case ISPPROC_PORT_OUT0: //LR Pipe
		{
			switch((t_uint32)omxformat) {
				case OMX_COLOR_FormatCbYCrY:
					*p_multiple_width = 8; 
					*p_multiple_height = 1;
					*p_multiple_stride = 2;
					break;
				case OMX_COLOR_Format16bitRGB565:
					*p_multiple_width = 8;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				case OMX_COLOR_Format16bitARGB4444:
					*p_multiple_width = 8;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				case OMX_COLOR_Format16bitARGB1555:
					*p_multiple_width = 8;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				case OMX_COLOR_Format24bitRGB888:
					*p_multiple_width = 8;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				case OMX_COLOR_Format32bitARGB8888:
					*p_multiple_width = 8;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;
				default:
					DBC_ASSERT(0); //this should not be called for other omxformat
					break;
			}
			
			
			
			break;
		}
		case ISPPROC_PORT_OUT1: //HR pipe
		{		
		
			switch((t_uint32)omxformat) {
				case OMX_COLOR_FormatYUV420PackedPlanar:
					*p_multiple_width = 16;
					*p_multiple_height = 2;
					*p_multiple_stride = 16;
					break;
				case OMX_COLOR_FormatYUV420Planar: // for backward compat
				case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
				case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:	
					*p_multiple_width = 8;
					*p_multiple_height = 16;
					*p_multiple_stride = 16;
					break;
				case OMX_COLOR_FormatCbYCrY:
					*p_multiple_width = 4;
					*p_multiple_height = 1;
					*p_multiple_stride = 8;
					break;					
				default:
					DBC_ASSERT(0); //this should not be called for other omxformat
					break;	
			}
		
		
			break;
		}	
		default:
		{	
			DBC_ASSERT(0); //this should not be called for other pipeId
			break;
		}	
	
	}

       //*p_multiple_stride = (*p_multiple_stride) *  getPixelDepth(omxformat);
	bool isInterLeavedFormat;
	t_uint16 stride = getStride(omxformat, (OMX_U32)1, &isInterLeavedFormat);  //calling to get value for isInterLeavedFormat
	if(isInterLeavedFormat) {
	   *p_multiple_stride = (*p_multiple_stride) *  getPixelDepth(omxformat);
    }
}



/*
 * Supported Fmt table
 */
OMX_BOOL ispprocport::isSupportedFmt(t_uint16 portID, OMX_COLOR_FORMATTYPE omxformat) 
{

	switch(portID) 
	{
	case ISPPROC_PORT_OUT0:
	{
		switch ((t_uint32)omxformat) {
			case OMX_COLOR_Format16bitRGB565:
			case OMX_COLOR_Format16bitARGB4444:
			case OMX_COLOR_Format16bitARGB1555:
			case OMX_COLOR_Format24bitRGB888:
			case OMX_COLOR_Format32bitARGB8888:
			case OMX_COLOR_FormatCbYCrY:
				return OMX_TRUE;
			default:
				return OMX_FALSE;
		}
		
	}
	case ISPPROC_PORT_OUT1:
	{
		switch ((t_uint32)omxformat) {
			case OMX_COLOR_FormatCbYCrY:
			case OMX_COLOR_FormatYUV420Planar://backward compat.
			case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar:
			case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:				
			case OMX_COLOR_FormatYUV420PackedPlanar:
				return OMX_TRUE;
			default:
				return OMX_FALSE;
		}
	
		
	}
	case ISPPROC_PORT_IN0:
	{
		switch ((t_uint32)omxformat) {
			case OMX_COLOR_FormatRawBayer8bit:
			case OMX_COLOR_FormatRawBayer10bit://backward compat
			case OMX_SYMBIAN_COLOR_FormatRawBayer12bit:
				return OMX_TRUE;
			default:
				return OMX_FALSE;
		}	
		
	}
	default:
		return OMX_FALSE;
	}
}


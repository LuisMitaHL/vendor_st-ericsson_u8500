/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ENS_Component.h"
#include "ENS_Nmf.h"
#include "IFM_Types.h"
#include "ifmport.h"
#define OMXCOMPONENT "IFMPORT"
#include "osi_trace.h"
#include "IFM_Types.h"

#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x2



extern "C"{
#include <cm/inc/cm_macros.h>
}

/** Return pixel depth in byte for specified format*/
float ifmport::getPixelDepth(OMX_U32 format)
{
    IN0("\n");

    float pixelDepth;

    switch(format)
    {
        case OMX_COLOR_FormatUnused :
            MSG0("Format unused, stride = 0. This should not occur !\n");
            pixelDepth = 0;
            break;

        /* Monochrome & 8 bit RGB Formats */
        case OMX_COLOR_FormatMonochrome :
        case OMX_COLOR_Format8bitRGB332 :
            pixelDepth = 1;
            break;

        /* 8 Bit RawBayer Format */
        case OMX_COLOR_FormatRawBayer8bit :
        case OMX_COLOR_FormatRawBayer8bitcompressed:
            pixelDepth = 1;
            break;

        /* 10/12 Bit RawBayer Formats */
        case OMX_COLOR_FormatRawBayer10bit : // is used for raw12!!!
        case OMX_SYMBIAN_COLOR_FormatRawBayer12bit:
            pixelDepth = 1.5;
            break;

        /* 16 bit Interleaved RGB Formats */
        case OMX_COLOR_Format12bitRGB444 :
        case OMX_COLOR_Format16bitARGB4444 :
        case OMX_COLOR_Format16bitARGB1555 :
        case OMX_COLOR_Format16bitRGB565 :
        case OMX_COLOR_Format16bitBGR565 :
            pixelDepth = 2;
            break;

        /* Iterleaved YUV Formats */
        case OMX_COLOR_FormatYCbYCr :
        case OMX_COLOR_FormatYCrYCb :
        case OMX_COLOR_FormatCbYCrY :
        case OMX_COLOR_FormatCrYCbY :
            pixelDepth = 2;
            break;

        /* 3 Bytes Iterleaved RGB Formats */
        case OMX_COLOR_Format18bitRGB666 :
        case OMX_COLOR_Format18bitARGB1665 :
        case OMX_COLOR_Format19bitARGB1666 :
        case OMX_COLOR_Format24bitRGB888 :
        case OMX_COLOR_Format24bitBGR888 :
        case OMX_COLOR_Format24bitARGB1887 :
        case OMX_COLOR_Format18BitBGR666 :
        case OMX_COLOR_Format24BitARGB6666 :
        case OMX_COLOR_Format24BitABGR6666 :
            pixelDepth = 3;
            break;

        /* 4 Bytes Iterleaved RGB Formats */
        case OMX_COLOR_Format25bitARGB1888 :
        case OMX_COLOR_Format32bitBGRA8888 :
        case OMX_COLOR_Format32bitARGB8888 :
            pixelDepth = 4;
            break;

        /* YUV 411/422 Planar and SemiPlanar Formats */
        case OMX_COLOR_FormatYUV411Planar :
        case OMX_COLOR_FormatYUV411PackedPlanar :
        case OMX_COLOR_FormatYUV422Planar :
        case OMX_COLOR_FormatYUV422PackedPlanar :
        case OMX_COLOR_FormatYUV422SemiPlanar :
        case OMX_COLOR_FormatYUV422PackedSemiPlanar :
            pixelDepth = 1.5;
            break;

        /* YUV Interleaved Format */
        case OMX_COLOR_FormatYUV444Interleaved :
            pixelDepth = 1.5;
            break;

        /* YUV 420 Planar and SemiPlanar Formats */
        case OMX_COLOR_FormatYUV420PackedSemiPlanar :
        case OMX_COLOR_FormatYUV420Planar :
        case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV21:
        case OMX_STE_COLOR_FormatYUV420PackedSemiPlanar_NV12:
        case OMX_COLOR_FormatYUV420PackedPlanar :
        case OMX_COLOR_FormatYUV420SemiPlanar :
            pixelDepth = 1.5;
            break;

        /* YUV MBTiled Format */
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar :
        case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:
            pixelDepth = 1.5;
            break;

        case OMX_COLOR_FormatL2 :
        case OMX_COLOR_FormatL4 :
        case OMX_COLOR_FormatL8 :
        case OMX_COLOR_FormatL16 :
        case OMX_COLOR_FormatL24 :
        case OMX_COLOR_FormatL32 :
        default :
            MSG1("Could not evaluate stride for format 0x%lx", format);
            pixelDepth = 4;
    }
    
    OUTR(" ", (int)pixelDepth); //typecasting to int to remove warning
    return pixelDepth;
}

OMX_ERRORTYPE ifmport::checkCompatibility(
                OMX_HANDLETYPE hTunneledComponent,
                OMX_U32 nTunneledPort) const {
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ifmport::setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef)
{
	IN0("\n");


	OMX_ERRORTYPE error = checkFormatInPortDefinition(portdef);
	if(error != OMX_ErrorNone) return error;


	mParamPortDefinition.format.video.xFramerate = portdef.format.video.xFramerate;
	mParamPortDefinition.format.video.nFrameWidth = portdef.format.video.nFrameWidth;
	mParamPortDefinition.format.video.nFrameHeight = portdef.format.video.nFrameHeight;
	mParamPortDefinition.format.video.pNativeRender = portdef.format.video.pNativeRender ;// not used
	mParamPortDefinition.format.video.nStride = portdef.format.video.nStride;
	mParamPortDefinition.format.video.nSliceHeight = portdef.format.video.nSliceHeight;
	mParamPortDefinition.format.video.bFlagErrorConcealment = portdef.format.video.bFlagErrorConcealment;
	mParamPortDefinition.format.video.eCompressionFormat = portdef.format.video.eCompressionFormat;
	mParamPortDefinition.format.video.eColorFormat = portdef.format.video.eColorFormat;
	mParamPortDefinition.format.video.pNativeWindow = portdef.format.video.pNativeWindow;

	mBytesPerPixel = getPixelDepth(mParamPortDefinition.format.video.eColorFormat);
	OMX_U32 bufferSize = (OMX_U32) ((double) (portdef.format.video.nFrameWidth * portdef.format.video.nFrameHeight) * mBytesPerPixel);
	//OMX_U32 bufferSize = getBufferSize(portdef.format.video.eColorFormat, portdef.format.video.nStride, portdef.format.video.nFrameWidth, portdef.format.video.nFrameHeight);

	mParamPortDefinition.nBufferSize = bufferSize;


	OUTR(" ",OMX_ErrorNone);
	return OMX_ErrorNone;

}


OMX_S32 ifmport::getStride(OMX_U32 format, OMX_U32 width, bool *isInterleavedFormat) 
{
    IN0("\n");
    
    OMX_S32 stride;
    OMX_U32 pictureWidth = 0;
    float pixelDepth = getPixelDepth(format);
    bool isInterleaved = true;

    switch(format)
    {
        case OMX_COLOR_FormatUnused :
            MSG0("Format unused, stride = 0. This should not occur !\n");
            stride = 0;
            isInterleaved = true;
            break;

        /* Monochrome & 8 bit RGB Formats */
        case OMX_COLOR_FormatMonochrome :
        case OMX_COLOR_Format8bitRGB332 :
            stride = (OMX_S32)((float)width * pixelDepth);
            isInterleaved = true;
            break;

        /* 8 Bit RawBayer Format */
        case OMX_COLOR_FormatRawBayer8bit :
        case OMX_COLOR_FormatRawBayer8bitcompressed:
            pictureWidth = ((((191 + width * 8)/192)*192)/8);
            stride = (OMX_S32)((float)pictureWidth * pixelDepth);
            isInterleaved = true;
            break;

        /* 10/12 Bit RawBayer Formats */
        case OMX_COLOR_FormatRawBayer10bit : // is used for raw12!!!
        case OMX_SYMBIAN_COLOR_FormatRawBayer12bit:
            pictureWidth = ((((191 + width * 12)/192)*192)/12);
            stride = (OMX_S32)((float)pictureWidth * pixelDepth);
            isInterleaved = true;
            break;

        /* 16 bit Interleaved RGB Formats */
        case OMX_COLOR_Format12bitRGB444 :
        case OMX_COLOR_Format16bitARGB4444 :
        case OMX_COLOR_Format16bitARGB1555 :
        case OMX_COLOR_Format16bitRGB565 :
        case OMX_COLOR_Format16bitBGR565 :
            stride = (OMX_S32)((float)width * pixelDepth);
            isInterleaved = true;
            break;

        /* Iterleaved YUV Formats */
        case OMX_COLOR_FormatYCbYCr :
        case OMX_COLOR_FormatYCrYCb :
        case OMX_COLOR_FormatCbYCrY :
        case OMX_COLOR_FormatCrYCbY :
            stride = (OMX_S32)((float)width * pixelDepth);
            isInterleaved = true;
            break;

        /* 3 Bytes Iterleaved RGB Formats */
        case OMX_COLOR_Format18bitRGB666 :
        case OMX_COLOR_Format18bitARGB1665 :
        case OMX_COLOR_Format19bitARGB1666 :
        case OMX_COLOR_Format24bitRGB888 :
        case OMX_COLOR_Format24bitBGR888 :
        case OMX_COLOR_Format24bitARGB1887 :
        case OMX_COLOR_Format18BitBGR666 :
        case OMX_COLOR_Format24BitARGB6666 :
        case OMX_COLOR_Format24BitABGR6666 :
            stride = (OMX_S32)((float)width * pixelDepth);
            isInterleaved = true;
            break;

        /* 4 Bytes Iterleaved RGB Formats */
        case OMX_COLOR_Format25bitARGB1888 :
        case OMX_COLOR_Format32bitBGRA8888 :
        case OMX_COLOR_Format32bitARGB8888 :
            stride = (OMX_S32)((float)width * pixelDepth);
            isInterleaved = true;
            break;

        /* YUV 411/422 Planar and SemiPlanar Formats */
        case OMX_COLOR_FormatYUV411Planar :
        case OMX_COLOR_FormatYUV411PackedPlanar :
        case OMX_COLOR_FormatYUV422Planar :
        case OMX_COLOR_FormatYUV422PackedPlanar :
        case OMX_COLOR_FormatYUV422SemiPlanar :
        case OMX_COLOR_FormatYUV422PackedSemiPlanar :
            stride = (OMX_S32)width;
            isInterleaved = false;
            break;

        /* YUV Interleaved Format */
        case OMX_COLOR_FormatYUV444Interleaved :
            stride = (OMX_S32)((float)width * pixelDepth);
            isInterleaved = true;
            break;

        /* YUV 420 Planar and SemiPlanar Formats */
        case OMX_COLOR_FormatYUV420PackedSemiPlanar :
        case OMX_COLOR_FormatYUV420Planar :
        case OMX_COLOR_FormatYUV420PackedPlanar :
        case OMX_COLOR_FormatYUV420SemiPlanar :
            stride = (OMX_S32)width;
            isInterleaved = false;			
            break;

        /* YUV MBTiled Format */
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar :
        case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:
	     stride = (OMX_S32)((float)width * pixelDepth);
            isInterleaved = true;
            break;

        case OMX_COLOR_FormatL2 :
        case OMX_COLOR_FormatL4 :
        case OMX_COLOR_FormatL8 :
        case OMX_COLOR_FormatL16 :
        case OMX_COLOR_FormatL24 :
        case OMX_COLOR_FormatL32 :
        default :
            MSG1("Could not evaluate stride for format 0x%lx", format);
            stride = (OMX_S32)((float)width * pixelDepth);
            isInterleaved = true;
    }

    if(isInterleavedFormat != NULL) {
		*isInterleavedFormat = isInterleaved;
    }
    
    OUTR(" ", (int)stride); //typecasting to int to remove warning
    return stride;
}


OMX_U32 ifmport::getBufferSize(OMX_U32 format, OMX_S32 stride, OMX_U32 width, OMX_U32 height)
{
    IN0("\n");
    
    float pixelDepth = getPixelDepth(format);    
    OMX_U32 bufferSize = 0;

    switch(format)
    {
        case OMX_COLOR_FormatUnused :
            MSG0("Format unused, bufferSize = 0. This should not occur !\n");
            bufferSize = 0;
            break;

        /* Monochrome & 8 bit RGB Formats */
        case OMX_COLOR_FormatMonochrome :
        case OMX_COLOR_Format8bitRGB332 :
            bufferSize = (OMX_U32)((OMX_U32)stride * height);
            break;

        /* 8 Bit RawBayer Format */
        case OMX_COLOR_FormatRawBayer8bit :
        case OMX_COLOR_FormatRawBayer8bitcompressed:
            bufferSize = (OMX_U32)((OMX_U32)stride * height);
            break;

        /* 10/12 Bit RawBayer Formats */
        case OMX_COLOR_FormatRawBayer10bit : // is used for raw12!!!
        case OMX_SYMBIAN_COLOR_FormatRawBayer12bit:
            bufferSize = (OMX_U32)((OMX_U32)stride * height);
            break;

        /* 16 bit Interleaved RGB Formats */
        case OMX_COLOR_Format12bitRGB444 :
        case OMX_COLOR_Format16bitARGB4444 :
        case OMX_COLOR_Format16bitARGB1555 :
        case OMX_COLOR_Format16bitRGB565 :
        case OMX_COLOR_Format16bitBGR565 :
            bufferSize = (OMX_U32)((OMX_U32)stride * height);
            break;

        /* Iterleaved YUV Formats */
        case OMX_COLOR_FormatYCbYCr :
        case OMX_COLOR_FormatYCrYCb :
        case OMX_COLOR_FormatCbYCrY :
        case OMX_COLOR_FormatCrYCbY :
            bufferSize = (OMX_U32)((OMX_U32)stride * height);
            break;

        /* 3 Bytes Iterleaved RGB Formats */
        case OMX_COLOR_Format18bitRGB666 :
        case OMX_COLOR_Format18bitARGB1665 :
        case OMX_COLOR_Format19bitARGB1666 :
        case OMX_COLOR_Format24bitRGB888 :
        case OMX_COLOR_Format24bitBGR888 :
        case OMX_COLOR_Format24bitARGB1887 :
        case OMX_COLOR_Format18BitBGR666 :
        case OMX_COLOR_Format24BitARGB6666 :
        case OMX_COLOR_Format24BitABGR6666 :
            bufferSize = (OMX_U32)((OMX_U32)stride * height);
            break;

        /* 4 Bytes Iterleaved RGB Formats */
        case OMX_COLOR_Format25bitARGB1888 :
        case OMX_COLOR_Format32bitBGRA8888 :
        case OMX_COLOR_Format32bitARGB8888 :
            bufferSize = (OMX_U32)((OMX_U32)stride * height);
            break;

        /* YUV 411/422 Planar and SemiPlanar Formats */
        case OMX_COLOR_FormatYUV411Planar :
        case OMX_COLOR_FormatYUV411PackedPlanar :
        case OMX_COLOR_FormatYUV422Planar :
        case OMX_COLOR_FormatYUV422PackedPlanar :
        case OMX_COLOR_FormatYUV422SemiPlanar :
        case OMX_COLOR_FormatYUV422PackedSemiPlanar :
            bufferSize = (OMX_U32)((float)(width * height) * pixelDepth);
            break;

        /* YUV Interleaved Format */
        case OMX_COLOR_FormatYUV444Interleaved :
            bufferSize = (OMX_U32)((OMX_U32)stride * height);
            break;

        /* YUV 420 Planar and SemiPlanar Formats */
        case OMX_COLOR_FormatYUV420PackedSemiPlanar :
        case OMX_COLOR_FormatYUV420Planar :        
        case OMX_COLOR_FormatYUV420PackedPlanar :
        case OMX_COLOR_FormatYUV420SemiPlanar :
            bufferSize = (OMX_U32)((float)(width * height) * pixelDepth);
            break;

        /* YUV MBTiled Format */
        case OMX_SYMBIAN_COLOR_FormatYUV420MBPackedSemiPlanar :
        case OMX_STE_COLOR_FormatYUV420MBSwap64bPackedSemiPlanar:
            bufferSize = (OMX_U32)((OMX_U32)stride * height);
            break;
            
        case OMX_COLOR_FormatL2 :
        case OMX_COLOR_FormatL4 :
        case OMX_COLOR_FormatL8 :
        case OMX_COLOR_FormatL16 :
        case OMX_COLOR_FormatL24 :
        case OMX_COLOR_FormatL32 :
        default :
            MSG1("Could not evaluate stride for format 0x%lx", format);
            bufferSize = (OMX_U32)((OMX_U32)stride * height);
    }
    
    OUTR(" ", (int)bufferSize); //typecasting to int to remove warning
    return bufferSize;
}


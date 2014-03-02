/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//#include "cm/inc/cm_macros.h"
#include "IFM_NmfProcessingComp.h"
#include "ENS_Nmf.h"
#include "mmhwbuffer.h"
#include "IFM_Types.h"


#define OMXCOMPONENT "IFM_NMFPROCESSINGCOMP"
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x2
#include "osi_trace.h"
#include "host/eventhandler.hpp"
#include "register.h"
#include "IFM_Types.h"



float IFM_NmfProcessingComp::getPixelDepth(OMX_U32 format)
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


OMX_ERRORTYPE
IFM_NmfProcessingComp::construct(void)
{
    OMX_ERRORTYPE error;

  
  registerStubsAndSkels();

    error = NmfMpc_ProcessingComponent::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
IFM_NmfProcessingComp::destroy(void)
{
    OMX_ERRORTYPE error;

    error = NmfMpc_ProcessingComponent::destroy();
    if (error != OMX_ErrorNone) return error;

    unregisterStubsAndSkels();

    return OMX_ErrorNone;
}




char * IFM_NmfProcessingComp::getNmfSuffix(void)const{
    return (char*)"";
}

OMX_ERRORTYPE
IFM_NmfProcessingComp::instantiate(void) {
    return OMX_ErrorNone;
}

OMX_ERRORTYPE
IFM_NmfProcessingComp::deInstantiate(void) {
    return OMX_ErrorNone;
}

OMX_ERRORTYPE
IFM_NmfProcessingComp::configure(void) {
    return OMX_ErrorNone;
}

OMX_ERRORTYPE
IFM_NmfProcessingComp::start(void) {
    return OMX_ErrorNone;
}

OMX_ERRORTYPE
IFM_NmfProcessingComp::stop(void) {
    return OMX_ErrorNone;
}


OMX_U32 IFM_NmfProcessingComp::getBufferPhysicalAddress(void * pBufferMetaData, OMX_U8* aLogicalAddr, OMX_U32 aSize) {
	MSG0("Entering ifmport getBufferPhysicalAddress\n");
	//return HwBuffer::GetBufferPhysicalAddress(pBufferMetaData, aLogicalAddr, aSize);
	
		OMX_U32 bufPhysicalAddr; //= getBufferPhysicalAddress(bufferAllocInfo, pBuffer, pBufferHdr->nAllocLen);

		OMX_ERRORTYPE error = ((MMHwBuffer *)pBufferMetaData)->PhysAddress((OMX_U32)aLogicalAddr, aSize,bufPhysicalAddr);
		if (error != OMX_ErrorNone) DBC_ASSERT(0);
		
		return bufPhysicalAddr;
		
	
	
}


OMX_ERRORTYPE IFM_NmfProcessingComp::doBufferAllocation(OMX_U32 nPortIndex,OMX_U32 nBufferIndex,OMX_U32 nSizeBytes, \
OMX_U8 **ppData, void **ppBufferMetaData){
	IN0("\n");
	OMX_ERRORTYPE error = OMX_ErrorNone;
    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

	OMX_VERSIONTYPE version = {{0, 0, 0, 0}};
    getOmxIlSpecVersion(&version);
    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    portdef.nVersion = version;
	ENS_Port * port = mENSComponent.getPort(nPortIndex);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);

	MSG1("Allocating Buffer  : 0x%lx bytes\n",nSizeBytes);


	if (portdef.eDomain == OMX_PortDomainVideo) {


    MMHwBuffer *sharedChunk=0;
    OMX_ERRORTYPE error;
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    if (nBufferIndex == 0) {

        MMHwBuffer::TBufferPoolCreationAttributes poolAttrs;
        poolAttrs.iBuffers = port->getBufferCountActual();      // Number of buffers in the pool
        poolAttrs.iDeviceType = MMHwBuffer::ESystemMemory;      // Memory type
        poolAttrs.iDomainID = 0;                                // MPC Domain ID (only requested for MPC memory type)
        poolAttrs.iSize = nSizeBytes;                           // Size (in byte) of a buffer
        poolAttrs.iAlignment = 256;                             // Alignment applied to the base address of each buffer in the pool
                                                                //   1,2,4,8,16,32,64 byte or -1 for MMU pageword size (default)
        poolAttrs.iCacheAttr = MMHwBuffer::ENormalCached;
        sharedChunk = NULL; 
        error = MMHwBuffer::Create(poolAttrs, mENSComponent.getOMXHandle(), sharedChunk);
        if (error != OMX_ErrorNone) return OMX_ErrorInsufficientResources;
        port->setSharedChunk(sharedChunk);
    }

	// Let's now retrieve the "current" buffer
    sharedChunk = port->getSharedChunk();
    MMHwBuffer::TBufferInfo bufferInfo;
    error = sharedChunk->BufferInfo(nBufferIndex, bufferInfo);
	if (error != OMX_ErrorNone) return OMX_ErrorInsufficientResources;
    
    *ppData = (OMX_U8 *)bufferInfo.iLogAddr;

    *ppBufferMetaData = sharedChunk;		
		
		
		
		
		
	// Initialize extradata segment.
	int i = (int) (portdef.format.video.nFrameWidth * portdef.format.video.nFrameHeight * getPixelDepth(portdef.format.video.eColorFormat));
	if (i > 0 && i < (int)nSizeBytes)
	memset((void *)&((*ppData)[i]), 0, (int)nSizeBytes - i);

	}
	else /* other : clock domain */
	{
		  OMX_U8 * ptr = (OMX_U8 *) new char[nSizeBytes];

		  if (ptr ==NULL) {
			return OMX_ErrorInsufficientResources;
		  }

		  *ppData = ptr;
		  ptr = (OMX_U8 *) new char[sizeof(OMX_U32)];
		  *((OMX_U32 **) ppBufferMetaData) = (OMX_U32 *) ptr;
		  *(OMX_U32 *) ptr = (OMX_U32) *ppData;
		  


	}

	MSG1("pBuffer = %p\n",*ppData);
	MSG1("pBufferMetaData = %p\n",*ppBufferMetaData);

	OUTR(" ",error);
	return error;
}


OMX_ERRORTYPE IFM_NmfProcessingComp::doBufferDeAllocation(OMX_U32 nPortIndex,OMX_U32 nBufferIndex, void * pBufferMetaData){
	IN0("\n");

	OMX_ERRORTYPE error = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE   portdef;
	OMX_VERSIONTYPE version = {{0, 0, 0, 0}};
    getOmxIlSpecVersion(&version);
    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    portdef.nVersion = version;
	ENS_Port * port = mENSComponent.getPort(nPortIndex);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);


	if ((portdef.eDomain == OMX_PortDomainVideo)&&(nBufferIndex == 0)) {
	    ENS_Port *port = mENSComponent.getPort(nPortIndex);
		MMHwBuffer *sharedChunk = port->getSharedChunk();
		error = MMHwBuffer::Destroy(sharedChunk);
		if (error != OMX_ErrorNone)	{
			OUTR(" ",error);
			return error;
		}
	}
	else if (portdef.eDomain != OMX_PortDomainVideo)
	{
		delete [] (char *) pBufferMetaData;
	}



	OUTR(" ",OMX_ErrorNone);
	return OMX_ErrorNone;
}

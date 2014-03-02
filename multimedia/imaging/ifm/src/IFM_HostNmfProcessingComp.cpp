/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//pp#include "cm/inc/cm_macros.h"
#include "IFM_HostNmfProcessingComp.h"
#include "ENS_Nmf.h"
#include "mmhwbuffer.h"
#include "SharedBuffer.h"
#include "IFM_Types.h"


#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x2
#include "osi_trace.h"
#include "host/eventhandler.hpp"
#include "register.h"
#include "IFM_Types.h"

float IFM_HostNmfProcessingComp::getPixelDepth(OMX_U32 format)
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
IFM_HostNmfProcessingComp::construct(void)
{
    OMX_ERRORTYPE error;

    registerStubsAndSkels();


    error = NmfHost_ProcessingComponent::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
IFM_HostNmfProcessingComp::destroy(void)
{
    OMX_ERRORTYPE error;

    error = NmfHost_ProcessingComponent::destroy();
    if (error != OMX_ErrorNone) return error;

    unregisterStubsAndSkels();

    return OMX_ErrorNone;
}



char * IFM_HostNmfProcessingComp::getNmfSuffix(void)const{
    return (char*)"";
}

OMX_ERRORTYPE IFM_HostNmfProcessingComp::instantiate(void) {
    return OMX_ErrorNone;
}

OMX_ERRORTYPE IFM_HostNmfProcessingComp::deInstantiate(void) {
    return OMX_ErrorNone;
}

OMX_ERRORTYPE IFM_HostNmfProcessingComp::configure(void) {
    return OMX_ErrorNone;
}

OMX_ERRORTYPE IFM_HostNmfProcessingComp::start(void) {
    return OMX_ErrorNone;
}

OMX_ERRORTYPE IFM_HostNmfProcessingComp::stop(void) {
    return OMX_ErrorNone;
}




OMX_ERRORTYPE IFM_HostNmfProcessingComp::allocateBuffer(
		OMX_U32 nPortIndex
		,OMX_U32 nBufferIndex
		,OMX_U32 nSizeBytes
		, OMX_U8 **ppData
		, void **ppBufferMetaData
		, void **portPrivateInfo){
	IN0("\n");
	OMX_ERRORTYPE error = OMX_ErrorNone;

    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

	OMX_VERSIONTYPE version = {{0, 0, 0, 0}};
    getOmxIlSpecVersion(&version);
    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    portdef.nVersion = version;
	ENS_Port * port = mENSComponent.getPort(nPortIndex);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);

	if (portdef.eDomain == OMX_PortDomainVideo) {

		error = allocateBufferVisual(nPortIndex, nBufferIndex, nSizeBytes, ppData, NULL, portPrivateInfo, OMX_FALSE);
		if (error != OMX_ErrorNone) return error;

		/* add a SharedBuffer tied to allocated buffers, because for HSMCamera we need to exchange Buffers
		 * with DSP. TODO: other IFM-based components don't all need this, I should make this optional
		 */



		 OMX_U32 bufPhysicalAddr; 
		error = ((MMHwBuffer *)(*portPrivateInfo))->PhysAddress((OMX_U32)*ppData,nSizeBytes,bufPhysicalAddr);
		if (error != OMX_ErrorNone) return error;

		SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(),
                                                   nSizeBytes, *ppData, bufPhysicalAddr, 0, NULL, error);
		if (sharedBuf == 0) {
            freeBufferVisual(nPortIndex,nBufferIndex,OMX_TRUE,NULL,*portPrivateInfo);
            return OMX_ErrorInsufficientResources;
        }
        if (error != OMX_ErrorNone) {
            freeBufferVisual(nPortIndex,nBufferIndex,OMX_TRUE,NULL,*portPrivateInfo);
            return error;
        }
        /* backup portPrivateInfo (actually an MMHwBuffer *) to ppBufferMetaData, which is stored in 
        OMX_BUFFERHEADERTYPE.pPlatformPrivate by caller (NmfHost_ProcessingComponent->allocateBuffer). This is a hack
        to store the (MMHwBuffer *) somewhere easily accessible by NMF components, such as B2R2lib. Longer term solution
        would be to store it in sharedBuf. */
        *ppBufferMetaData = *portPrivateInfo;
         
		*portPrivateInfo = (void *)sharedBuf;

		// Initialize extradata segment.

		int i = (int) (portdef.format.video.nFrameWidth * portdef.format.video.nFrameHeight * getPixelDepth(portdef.format.video.eColorFormat));
		if (i > 0 && i < (int)nSizeBytes)
		memset((void *)&((*ppData)[i]), 0, (int)nSizeBytes - i);

	}
	else /* clock port*/
	{

		error = allocateBufferHeap(nPortIndex,nBufferIndex,nSizeBytes,ppData,ppBufferMetaData,portPrivateInfo);
		if (error != OMX_ErrorNone) return error;

	}


	MSG1("pBuffer = %p\n",*ppData);
	MSG1("pBufferMetaData = %p\n",*ppBufferMetaData);

	OUTR(" ",error);
	return error;
}


OMX_ERRORTYPE
IFM_HostNmfProcessingComp::useBufferHeader(
		OMX_DIRTYPE dir,
		OMX_BUFFERHEADERTYPE* pBufferHdr) {

    OMX_PARAM_PORTDEFINITIONTYPE   portdef;

	OMX_VERSIONTYPE version = {{0, 0, 0, 0}};
    getOmxIlSpecVersion(&version);
    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    portdef.nVersion = version;
    ENS_Port * port;

    if(NULL == pBufferHdr) return OMX_ErrorBadParameter;

	if(dir==OMX_DirOutput) port = mENSComponent.getPort(pBufferHdr->nOutputPortIndex);
	else port = mENSComponent.getPort(pBufferHdr->nInputPortIndex);
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);

	if (portdef.eDomain == OMX_PortDomainVideo) {

		SharedBuffer *sharedBuf = 0;

		if(dir == OMX_DirInput) {
			sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pInputPortPrivate);
		} else {
			sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pOutputPortPrivate);
		}
		if(!sharedBuf) return OMX_ErrorBadParameter;

		return sharedBuf->setOMXHeader(pBufferHdr);


	}else return OMX_ErrorNone;
}

OMX_ERRORTYPE
IFM_HostNmfProcessingComp::freeBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_BOOL bBufferAllocated,
        void *bufferAllocInfo,
        void *portPrivateInfo){
        IN0("\n");
	ENS_Port * port = mENSComponent.getPort(nPortIndex);

	OMX_PARAM_PORTDEFINITIONTYPE   portdef;
	OMX_VERSIONTYPE version = {{0, 0, 0, 0}};
    getOmxIlSpecVersion(&version);
    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    portdef.nVersion = version;
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);


    if(portdef.eDomain == OMX_PortDomainVideo){

    	//will free depending on bBufferAllocated
    	//freeBufferVisual(nPortIndex,nBufferIndex,bBufferAllocated,NULL,portPrivateInfo);

		OMX_ERRORTYPE error;
		ENS_Port *port = mENSComponent.getPort(nPortIndex);
		MMHwBuffer *sharedChunk = (MMHwBuffer *)port->getSharedChunk();
		if (sharedChunk) {
			if (nBufferIndex == port->getBufferCountActual()-1) {
				if (bBufferAllocated) {
					error = MMHwBuffer::Destroy(sharedChunk);
					DBC_ASSERT(error == OMX_ErrorNone);
				} else {
					error = MMHwBuffer::Close(sharedChunk);
					DBC_ASSERT(error == OMX_ErrorNone);
				}
				port->setSharedChunk(0);
			}
		}//else DBC_ASSERT(0);
	
    	/* whatever : delete the sharedBuffer as in both case (allocation or use) it is allocated*/
    	delete (SharedBuffer *)portPrivateInfo;
    }
    else /* clock domain */
    {
    	 freeBufferHeap(nPortIndex,nBufferIndex,bBufferAllocated,bufferAllocInfo,portPrivateInfo);
    }
	OUTR(" ",OMX_ErrorNone);
	return OMX_ErrorNone;
}



OMX_ERRORTYPE
IFM_HostNmfProcessingComp::useBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_BUFFERHEADERTYPE* pBufferHdr,
        void **portPrivateInfo)
{

	/* do not call useBufferVisual as it is implemented for Video which does not do the same than imaging */


	ENS_Port * port = mENSComponent.getPort(nPortIndex);

	OMX_PARAM_PORTDEFINITIONTYPE   portdef;
	OMX_VERSIONTYPE version = {{0, 0, 0, 0}};
    getOmxIlSpecVersion(&version);
    portdef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    portdef.nVersion = version;
    port->getParameter(OMX_IndexParamPortDefinition, &portdef);

    if(portdef.eDomain == OMX_PortDomainVideo){

		OMX_ERRORTYPE error;
		void *bufferAllocInfo = 0;
		OMX_U8 *pBuffer;

		bufferAllocInfo = (void *)(port->getSharedChunk()); // bufferAllocINfo is now a MMHWBUffer
		if(!bufferAllocInfo) {
            return (OMX_ErrorInsufficientResources);
        }

        error = ((MMHwBuffer *)bufferAllocInfo)->AddBufferInfo(nBufferIndex, (OMX_U32)pBufferHdr->pBuffer, pBufferHdr->nAllocLen);
        if (error != OMX_ErrorNone) {
            return error;
        }
		
		pBuffer = pBufferHdr->pBuffer;
		/* it is freed when freeBuffer is called*/
		//OMX_U32 bufPhysicalAddr = HwBuffer::GetBufferPhysicalAddress(bufferAllocInfo, pBuffer, pBufferHdr->nAllocLen);
		OMX_U32 bufPhysicalAddr; //= getBufferPhysicalAddress(bufferAllocInfo, pBuffer, pBufferHdr->nAllocLen);

		error = ((MMHwBuffer *)bufferAllocInfo)->PhysAddress((OMX_U32)pBuffer, pBufferHdr->nAllocLen,bufPhysicalAddr);
		if (error != OMX_ErrorNone) return error;
	
		SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(),
				pBufferHdr->nAllocLen, pBuffer, bufPhysicalAddr, 0, NULL, error);
		// actually we dont care about bufferAllocInfo in case where we are buffer supplier but we do care in case where we are not buffer supplier, thus keep it anyway in both cases.
		if (sharedBuf == 0) return OMX_ErrorInsufficientResources;
		if (error != OMX_ErrorNone) return error;

		sharedBuf->setOMXHeader(pBufferHdr);
		*portPrivateInfo = sharedBuf;
    }
    else
    {
		/* do nothing: useBufferHeap is empty*/
    }

    return OMX_ErrorNone;
}

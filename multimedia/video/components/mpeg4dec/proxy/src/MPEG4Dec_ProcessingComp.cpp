/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_mpeg4dec
#include "osi_trace.h"
#include "MPEG4Dec_Proxy.h"
#include "SharedBuffer.h"
#include "VFM_Memory.h"
#include "MPEG4Dec_ProcessingComp.h"
#include "VFM_DDepUtility.h"
#include "video_generic_chipset_api.h"
#include <string.h>
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4dec_proxy_src_MPEG4Dec_ProcessingCompTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE
#define DEBUG_MEM_SIZE (128*1024 - 1)
#define BUFFER_LINKLIST_SIZE 1024
#define LINE_BUFFER_INDEX 0
#define COEFF_BUFFER_INDEX 0
#define EXT_BIT       1
#define FIFO_SIZE 32
#define MAX_MB_WIDTH 120
#define MAX_MB_HEIGHT 68

volatile ts_ddep_buffer_descriptor g_mDebugBufferDescmpeg4Dec;
volatile ts_ddep_perf_param_out* g_pMPEG4DecOut_perf_parameters = 0;

MPEG4Dec_ProcessingComp::MPEG4Dec_ProcessingComp(ENS_Component &enscomp):
									VFM_NmfHost_ProcessingComponent(enscomp)
{
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::MPEG4Dec_ProcessingComp() constructor");

    pProxyComponent = (MPEG4Dec_Proxy *)&enscomp;
	firstFrame = OMX_TRUE;
	allocate_internal_memory=OMX_FALSE;
	memory_once_allocated = OMX_FALSE;
	error_reporting_enable=0;
	deblocking_enable=OMX_FALSE;
	//mSupportedExtension=0;
	//+ER344943
	//+ER352805
	//>if(!pProxyComponent->isMPCobject)
	//-ER352805
	{
		#ifndef __MPEG4DEC_SOFT_DECODER
			OstTraceInt0(TRACE_FLOW, "MPEG4Dec_ProcessingComp(): Deblocking enabled by default");
			deblocking_enable=OMX_TRUE;
		#else
			OstTraceInt0(TRACE_FLOW, "MPEG4Dec_ProcessingComp(): Deblocking disabled by default");
			deblocking_enable=OMX_FALSE;
		#endif
	}
	OstTraceInt1(TRACE_FLOW, "MPEG4Dec_ProcessingComp(): deblocking_enable is %d", deblocking_enable);

	//-ER344943
	mParamBufferDesc.nSize = 0;
	mParamBufferDesc.nMpcAddress = 0;
	mParamBufferDesc.nLogicalAddress = 0;
	mParamBufferDesc.nPhysicalAddress = 0;

	mLinkListBufferDesc.nSize = 0;
	mLinkListBufferDesc.nMpcAddress = 0;
	mLinkListBufferDesc.nLogicalAddress = 0;
	mLinkListBufferDesc.nPhysicalAddress = 0;

	mDebugBufferDesc.nSize = 0;
	mDebugBufferDesc.nMpcAddress = 0;
	mDebugBufferDesc.nLogicalAddress = 0;
	mDebugBufferDesc.nPhysicalAddress = 0;

	for(OMX_U32 i=0;i<4;i++)
	{
			   nLogicalAddress[i]=0;
	}
	for(OMX_U32 i=0;i<5;i++)
	{
			   references_bufaddress[i]=0;
	}
	for(OMX_U32 i=0;i<4;i++)
	{
			   deblock_param_address[i]=0;
	}
	for(OMX_U32 i=0;i<228;i++)
	{
			   error_map_copy[i]=0;
	}
	vfm_mem_ctxt = 0;
	fullDVFSSet = OMX_FALSE;
	fullDDRSet = OMX_FALSE; //for ER 345421
	AspectRatio=0;	//default value
	nHorizontal=0; 	//default value
	nVertical=0;   	//default value
	ColorPrimary=0;  //OMX_ColorPrimaryUnknown
	m_nSvaMcps=50;

	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::MPEG4Dec_ProcessingComp() constructor");
}

MPEG4Dec_ProcessingComp::~MPEG4Dec_ProcessingComp()
{
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::~MPEG4Dec_ProcessingComp() destructor");

    firstFrame = OMX_TRUE;
	allocate_internal_memory=OMX_FALSE;
    memory_once_allocated = OMX_FALSE;
	if(vfm_mem_ctxt)
		VFM_CloseMemoryList(vfm_mem_ctxt);
	vfm_mem_ctxt = 0;
	mParamBufferDesc.nSize = 0;
	//mSupportedExtension=0;
	mParamBufferDesc.nLogicalAddress = 0;
	mParamBufferDesc.nPhysicalAddress = 0;
	pProxyComponent->pFwPerfDataPtr=(OMX_PTR)0;
	mDebugBufferDesc.nSize = 0;
	mDebugBufferDesc.nLogicalAddress = 0;
	mDebugBufferDesc.nPhysicalAddress = 0;
	mLinkListBufferDesc.nSize = 0;
	mLinkListBufferDesc.nLogicalAddress = 0;
	mLinkListBufferDesc.nPhysicalAddress = 0;
	AspectRatio=0;	//default value
	nHorizontal=0; 	//default value
	nVertical=0;   	//default value
	ColorPrimary=0;  //OMX_ColorPrimaryUnknown

	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::~MPEG4Dec_ProcessingComp() destructor", pProxyComponent);
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceInt4(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::emptyThisBuffer(): pBuffer = 0x%x,  nFilledLen = %d, nFlags = 0x%x, pBuffer->nTimeStamp = %d", (unsigned int)pBuffer, pBuffer->nFilledLen, pBuffer->nFlags, pBuffer->nTimeStamp);
	OstTraceInt2(TRACE_FLOW, "... emptyThisBuffer(): pBuffer->pBuffer = 0x%x, pBuffer->nAllocLen = %d", (unsigned int)pBuffer->pBuffer, pBuffer->nAllocLen);

    VFM_Port    *pt_port_out,*pt_port_in;
	pt_port_out = (VFM_Port*)pProxyComponent->getPort(VPB+1);
	pt_port_in = (VFM_Port*)pProxyComponent->getPort(VPB);
	OMX_U32 width = pt_port_out->getFrameWidth();
	OMX_U32 height = pt_port_out->getFrameHeight();
    if(firstFrame == OMX_TRUE)
	{
		if(pt_port_in->getCompressionFormat()==12)
		{
			pProxyComponent->sorenson_flag=OMX_TRUE;
		}
		if(pProxyComponent->thumbnail)
		error_reporting_enable |= 4;
		iPortSettingsInfoSignal.sendportSettings(width,height,OMX_TRUE,pProxyComponent->sorenson_flag,AspectRatio,ColorPrimary,error_reporting_enable);
		firstFrame = OMX_FALSE;
	}

	OstTraceInt1(TRACE_FLOW, "MPEG4Dec_ProcessingComp::emptyThisBuffer(): getImmediateRelease() = %d", pProxyComponent->getParamAndConfig()->getImmediateRelease());
	if(pProxyComponent->getParamAndConfig()->getImmediateRelease())
	{
		OstTraceFiltInst0(TRACE_FLOW, "emptyThisBuffer(): Sending Immediate I flag to parser");
		iPortSettingsInfoSignal.sendImmediateIFlag(pProxyComponent->getParamAndConfig()->getImmediateRelease());
		resetDecodeImmediateRelease();
	}
	//+ER429711
	DisplayOrderType *pDisplayOrder =pProxyComponent->pParam->getDisplayOrder();
	pDisplayOrder->set(OMX_TRUE);
	//-ER429711
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::emptyThisBuffer()");

	return VFM_NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);
}

/* + Changes for CR 399075 */
void MPEG4Dec_ProcessingComp::resetDecodeImmediateRelease()
{
	pProxyComponent->getParamAndConfig()->resetDecodeImmediateFlag();
}
/* - Changes for CR 399075 */

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceInt2(TRACE_FLOW, "<=> MPEG4Dec_ProcessingComp::fillThisBuffer() pBuffer = 0x%x, pBuffer->nAllocLen = %d", (unsigned int)pBuffer, pBuffer->nAllocLen);

	return VFM_NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::construct(void)
{
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::construct()");

    RETURN_OMX_ERROR_IF_ERROR(allocateRequiredMemory());

	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::construct()");

    return NmfHost_ProcessingComponent::construct();
}
OMX_ERRORTYPE MPEG4Dec_ProcessingComp::destroy(void)
{
    //+ER345950
	destroy_internal();
	//-ER345950
	if(pProxyComponent->isMPCobject)
	{
		for(OMX_U32 i=0;i<4;i++)
		{
			if (nLogicalAddress[i])
			{
				VFM_Free(vfm_mem_ctxt, (t_uint8*)nLogicalAddress[i]);
				nLogicalAddress[i] = 0;
			}
		}
	}
	else
	{
		if (nLogicalAddress[0])
		{
			VFM_Free(vfm_mem_ctxt, (t_uint8*)nLogicalAddress[0]);
			nLogicalAddress[0] = 0;
		}
	}
    NmfHost_ProcessingComponent::destroy();
    return OMX_ErrorNone;

}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::allocateBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_U32 nSizeBytes,
                OMX_U8 **ppData,
                void **bufferAllocInfo,
                void **portPrivateInfo)
{
    OMX_ERRORTYPE error;
	 OstTraceFiltStatic1(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::allocateBuffer() nSizeBytes = %d", pProxyComponent, nSizeBytes);
	//if(pProxyComponent->isMPCobject)
#ifndef __MPEG4DEC_SOFT_DECODER
	{

#ifdef __CACHE_OPTI
		error = allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo,OMX_TRUE);
#else
		error = allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo,OMX_FALSE);
#endif
		if (error != OMX_ErrorNone) return error;
//#ifndef __MPEG4DEC_SOFT_DECODER
		if (NULL == bufferAllocInfo)
			return OMX_ErrorUndefined;
        *bufferAllocInfo = *portPrivateInfo;
	    MMHwBuffer::TBufferInfo bufferInfo;
	    ((MMHwBuffer *)*bufferAllocInfo)->BufferInfo(nBufferIndex,bufferInfo);
	    OMX_U32 bufPhysicalAddr = (OMX_U32)bufferInfo.iPhyAddr;

		OMX_U32 bufMpcAddress =0;
#ifdef __CACHE_OPTI
	if(!vfm_mem_ctxt)
	{
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
		VFM_MemoryInit(vfm_mem_ctxt,(OMX_PTR)(mENSComponent.getOMXHandle()));
	}
	VFM_AddMemoryExternalSource(vfm_mem_ctxt,(*ppData), (t_uint8 *)bufPhysicalAddr,nSizeBytes,(*portPrivateInfo));
#endif
	    SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(),
	      	  nSizeBytes, *ppData, bufPhysicalAddr, bufMpcAddress, *bufferAllocInfo, error);
	    if (sharedBuf == 0) return OMX_ErrorInsufficientResources;
	    if (error != OMX_ErrorNone) return error;

	    *portPrivateInfo = sharedBuf;
	}
#else
	{
		error = allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo,OMX_TRUE);
		if (error != OMX_ErrorNone) return error;
	#ifdef __CACHE_OPTI
		if(!vfm_mem_ctxt)
		{
			OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
			OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
			vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,1/*pass 1 as it is  full software*/);
		}
		VFM_AddMemoryExternalSource(vfm_mem_ctxt,(*ppData), (t_uint8 *)NULL,nSizeBytes,(*portPrivateInfo));
	#endif
	}
#endif
	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::allocateBuffer()", pProxyComponent);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::useBufferHeader(
                OMX_DIRTYPE dir,
                OMX_BUFFERHEADERTYPE* pBufferHdr)
{
	OstTraceFiltStatic0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::useBufferHeader()", pProxyComponent);

	//if(pProxyComponent->isMPCobject)
#ifndef __MPEG4DEC_SOFT_DECODER
	{

		SharedBuffer *sharedBuf = 0;

		if(!pBufferHdr) return OMX_ErrorBadParameter;

		if(dir == OMX_DirInput)
		{
			sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pInputPortPrivate);
		}
		else
		{
			sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pOutputPortPrivate);
		}
		if(!sharedBuf) return OMX_ErrorBadParameter;
        return sharedBuf->setOMXHeader(pBufferHdr);
	}
#else
	{
        return VFM_NmfHost_ProcessingComponent::useBufferHeader(dir,pBufferHdr);
	}
#endif
	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::useBufferHeader()", pProxyComponent);
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::useBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_BUFFERHEADERTYPE* pBufferHdr,
                void **portPrivateInfo)
{
	OstTraceFiltStatic0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::useBuffer()", pProxyComponent);

	//if(pProxyComponent->isMPCobject)
#ifndef __MPEG4DEC_SOFT_DECODER
	{

		OMX_ERRORTYPE error;
		void *bufferAllocInfo = 0;
		OMX_U8 *pBuffer;

	   ENS_Port *port = mENSComponent.getPort(nPortIndex);
	   bufferAllocInfo = port->getSharedChunk();
	   pBuffer = pBufferHdr->pBuffer;
	   pBufferHdr->pPlatformPrivate = port->getSharedChunk();

		/* +Change start for CR332521 IOMX UseBuffer */
                VFM_Component *pComponent = (VFM_Component *)(&mENSComponent);

                iOMXType *piOMX = pComponent->getParamAndConfig()->getiOMX();
                if (OMX_ErrorNone != piOMX->allocate(pComponent, (MMHwBuffer *)bufferAllocInfo, nPortIndex, nBufferIndex, pBufferHdr->nAllocLen, pBufferHdr)) {
                    return OMX_ErrorInsufficientResources;
                }

                piOMX->getBuffer((MMHwBuffer **)(&bufferAllocInfo), &pBuffer, nPortIndex, nBufferIndex);

		 //OMX_U32 bufPhysicalAddr = HwBuffer::GetBufferPhysicalAddress(bufferAllocInfo, pBuffer, pBufferHdr->nAllocLen);
		 MMHwBuffer::TBufferInfo bufferInfo;
                 if (pBuffer==pBufferHdr->pBuffer) {
                    ((MMHwBuffer *)bufferAllocInfo)->BufferInfo(nBufferIndex,bufferInfo);
                 } else {
                    ((MMHwBuffer *)bufferAllocInfo)->BufferInfo(0,bufferInfo);
                 }
				 /* -Change end for CR332521 IOMX UseBuffer */

		//OMX_U32 bufPhysicalAddr = HwBuffer::GetBufferPhysicalAddress(bufferAllocInfo, pBuffer, pBufferHdr->nAllocLen);
		// MMHwBuffer::TBufferInfo bufferInfo;
		//((MMHwBuffer *)bufferAllocInfo)->BufferInfo(nBufferIndex,bufferInfo);
		OMX_U32 bufPhysicalAddr = (OMX_U32)bufferInfo.iPhyAddr;

		OMX_U32 bufMpcAddress = 0;
#ifdef __CACHE_OPTI
	if(!vfm_mem_ctxt)
	{
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
		VFM_MemoryInit(vfm_mem_ctxt,(OMX_PTR)(mENSComponent.getOMXHandle()));
	}
	VFM_AddMemoryExternalSource(vfm_mem_ctxt,(pBuffer), (t_uint8 *)bufPhysicalAddr,(pBufferHdr->nAllocLen),bufferAllocInfo);

#endif
		SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(),
				pBufferHdr->nAllocLen, pBuffer, bufPhysicalAddr, bufMpcAddress, bufferAllocInfo, error);
		if (sharedBuf == 0) return OMX_ErrorInsufficientResources;
		if (error != OMX_ErrorNone) return error;

		sharedBuf->setOMXHeader(pBufferHdr);

		*portPrivateInfo = sharedBuf;
	}
#else
	{
		ENS_Port *port = mENSComponent.getPort(nPortIndex);
		MMHwBuffer *sharedChunk = port->getSharedChunk();
//+ER344873
#ifdef __CACHE_OPTI
		if(!vfm_mem_ctxt)
		{
			OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
			OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
			vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,1/*pass 1 as it is  full software*/);
		}
		VFM_AddMemoryExternalSource(vfm_mem_ctxt,(pBufferHdr->pBuffer), (t_uint8 *)NULL,(pBufferHdr->nAllocLen),sharedChunk);
#endif
//-ER344873
		if (sharedChunk)
		{
			sharedChunk->AddBufferInfo(nBufferIndex, (OMX_U32)pBufferHdr->pBuffer, pBufferHdr->nAllocLen);
			*portPrivateInfo = sharedChunk;
//+ER344873
			pBufferHdr->pPlatformPrivate = port->getSharedChunk();
//-ER344873
		}
		else
		{
			*portPrivateInfo = 0;
		}
     }
#endif
	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::useBuffer()", pProxyComponent);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::freeBuffer(
				OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_BOOL bBufferAllocated,
                void *bufferAllocInfo,
                void *portPrivateInfo)
{
    OMX_ERRORTYPE error;
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::freeBuffer()");

#ifndef __MPEG4DEC_SOFT_DECODER
		SharedBuffer *sharedBuf = static_cast<SharedBuffer *>(portPrivateInfo);
		portPrivateInfo = (void *)sharedBuf->getBufferAllocInfo();
#endif
		error = VFM_NmfHost_ProcessingComponent::freeBuffer(nPortIndex,nBufferIndex,bBufferAllocated,
														  bufferAllocInfo,portPrivateInfo);
		if (error != OMX_ErrorNone)
		{
			OstTraceInt0(TRACE_ERROR, "MPEG4Dec_ProcessingComp::freeBuffer() returned ERROR from VFM::freeBuffer");
			return error;
		}

#ifndef __MPEG4DEC_SOFT_DECODER
		delete sharedBuf;
#endif

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::freeBuffer()");
    return OMX_ErrorNone;
}


OMX_ERRORTYPE MPEG4Dec_ProcessingComp::allocateRequiredMemory()
{
    t_uint32 size;
	OstTraceFiltStatic0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::allocateRequiredMemory()", pProxyComponent);

	//jitender:: fix this check align mask
	int fakeBufferSize = 256;

	//jitender:: fix this remove literal 8 and make some #define and remove align mask
    size = sizeof(ts_ddep_vdc_mpeg4_param_desc)
			+ FIFO_SIZE* sizeof(ts_ddep_vdc_mpeg4_header_desc)
			+ fakeBufferSize;//check whether itis being passed to firmware

	{
		if(!vfm_mem_ctxt)
		{
				OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
				OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
				//if(pProxyComponent->isMPCobject)
#ifndef __MPEG4DEC_SOFT_DECODER
				{
					vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
				}
#else
				{
//+ER344873
					vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,1/*pass 1 as it is  full software*/);
//-ER344873
		}
#endif
		}
		VFM_MemoryInit(vfm_mem_ctxt,(OMX_PTR)(mENSComponent.getOMXHandle()));
		if(!memory_once_allocated)
		{
					t_uint8* param_buf_logical_addr = 0;
					t_uint8* linklist_buf_logical_addr = 0;
					t_uint8* debug_buf_logical_addr = 0;
					OstTraceFiltStatic0(TRACE_FLOW, "MPEG4Dec_ProcessingComp :allocateRequiredMemory  once_allocated\n",pProxyComponent);
                    if(pProxyComponent->isMPCobject)
					{
						param_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, size, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					}
					else
					{
						param_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, size, VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					}
					if(!param_buf_logical_addr)
					{
						VFM_CloseMemoryList(vfm_mem_ctxt);
						vfm_mem_ctxt = 0;
                        return OMX_ErrorInsufficientResources;
					}

					if(pProxyComponent->isMPCobject)
					{
						linklist_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, BUFFER_LINKLIST_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					}
					else
					{
						linklist_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, BUFFER_LINKLIST_SIZE, VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					}
					if(!linklist_buf_logical_addr)
					{
						VFM_CloseMemoryList(vfm_mem_ctxt);
						vfm_mem_ctxt = 0;
						return OMX_ErrorInsufficientResources;
					}
                    if(pProxyComponent->isMPCobject)
					{
						debug_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, DEBUG_MEM_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					}
					else
					{
						debug_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, DEBUG_MEM_SIZE, VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					}
					if(!debug_buf_logical_addr)
					{
						VFM_CloseMemoryList(vfm_mem_ctxt);
						vfm_mem_ctxt = 0;
						return OMX_ErrorInsufficientResources;
					}

					mParamBufferDesc.nSize = size;
					mParamBufferDesc.nLogicalAddress = (t_uint32)param_buf_logical_addr;
					mParamBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, param_buf_logical_addr));
					mParamBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(vfm_mem_ctxt, param_buf_logical_addr));

					mLinkListBufferDesc.nSize = BUFFER_LINKLIST_SIZE;
					mLinkListBufferDesc.nLogicalAddress = (t_uint32)linklist_buf_logical_addr;
					mLinkListBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, linklist_buf_logical_addr));
					mLinkListBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(vfm_mem_ctxt, linklist_buf_logical_addr));

					mDebugBufferDesc.nSize = DEBUG_MEM_SIZE;
					mDebugBufferDesc.nLogicalAddress = (t_uint32)debug_buf_logical_addr;
					mDebugBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, debug_buf_logical_addr));
					mDebugBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(vfm_mem_ctxt, debug_buf_logical_addr));

					g_mDebugBufferDescmpeg4Dec.nSize =  mDebugBufferDesc.nSize;
					g_mDebugBufferDescmpeg4Dec.nLogicalAddress =  mDebugBufferDesc.nLogicalAddress;
					g_mDebugBufferDescmpeg4Dec.nPhysicalAddress =  mDebugBufferDesc.nPhysicalAddress;
					g_mDebugBufferDescmpeg4Dec.nMpcAddress =  mDebugBufferDesc.nMpcAddress;
					memory_once_allocated=OMX_TRUE;
		}
	}

	//for multi-instance
        ts_t1xhv_vdc_internal_buf *pInternal_buf;
        pInternal_buf = (ts_t1xhv_vdc_internal_buf *)(mParamBufferDesc.nLogicalAddress
                                                    + sizeof(ts_t1xhv_vdc_frame_buf_in)
													+ sizeof(ts_t1xhv_vdc_frame_buf_out));

	if(pProxyComponent->isMPCobject)
	{
					OstTraceFiltStatic0(TRACE_FLOW, "MPEG4Dec_ProcessingComp :allocateRequiredMemory  x,y,mv,nc for mpc\n",pProxyComponent);
				//addr_mv_type_buffer
                  nLogicalAddress[0] = (t_uint32)VFM_Alloc(vfm_mem_ctxt,( MAX_MB_WIDTH * MAX_MB_HEIGHT * 2), VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
				  if(!nLogicalAddress[0])
				  {
					  VFM_CloseMemoryList(vfm_mem_ctxt);
					  vfm_mem_ctxt = 0;
					  return OMX_ErrorInsufficientResources;
				  }
				  memset ( (void *)((nLogicalAddress[0]) + 0), 0x00, ( MAX_MB_WIDTH * MAX_MB_HEIGHT * 2));

				  //addr_mb_not_coded
				  nLogicalAddress[1] = (t_uint32)VFM_Alloc(vfm_mem_ctxt,( MAX_MB_WIDTH * MAX_MB_HEIGHT * 2), VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
                  if(!nLogicalAddress[1])
				  {
					  VFM_CloseMemoryList(vfm_mem_ctxt);
					  vfm_mem_ctxt = 0;
					  return OMX_ErrorInsufficientResources;
				  }
                  memset ( (void *)((nLogicalAddress[1]) + 0), 0x00, ( MAX_MB_WIDTH * MAX_MB_HEIGHT * 2));

				  //addr_x_err_res_buffer
				  nLogicalAddress[2] = (t_uint32)VFM_Alloc(vfm_mem_ctxt,( MAX_MB_WIDTH * MAX_MB_HEIGHT * 8), VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
                  if(!nLogicalAddress[2])
				  {
					  VFM_CloseMemoryList(vfm_mem_ctxt);
					  vfm_mem_ctxt = 0;
					  return OMX_ErrorInsufficientResources;
				  }
				  memset ( (void *)((nLogicalAddress[2]) + 0), 0x00, ( MAX_MB_WIDTH * MAX_MB_HEIGHT * 8));

				  //addr_y_err_res_buffer
				  nLogicalAddress[3] = (t_uint32)VFM_Alloc(vfm_mem_ctxt,( MAX_MB_WIDTH * MAX_MB_HEIGHT * 8), VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
                  if(!nLogicalAddress[3])
				  {
					  VFM_CloseMemoryList(vfm_mem_ctxt);
					  vfm_mem_ctxt = 0;
					  return OMX_ErrorInsufficientResources;
				  }
				  memset ( (void *)((nLogicalAddress[3]) + 0), 0x00, ( MAX_MB_WIDTH * MAX_MB_HEIGHT * 8));

                  pInternal_buf->addr_mv_type_buffer = 		(t_uint32)ENDIANESS_CONVERSION((VFM_GetMpc(vfm_mem_ctxt,(t_uint8 *) nLogicalAddress[0])));
				  pInternal_buf->addr_mb_not_coded = 		(t_uint32)ENDIANESS_CONVERSION((VFM_GetMpc(vfm_mem_ctxt,(t_uint8 *) nLogicalAddress[1])));
				  pInternal_buf->addr_x_err_res_buffer = 	(t_uint32)ENDIANESS_CONVERSION((VFM_GetMpc(vfm_mem_ctxt,(t_uint8 *) nLogicalAddress[2])));
				  pInternal_buf->addr_y_err_res_buffer = 	(t_uint32)ENDIANESS_CONVERSION((VFM_GetMpc(vfm_mem_ctxt,(t_uint8 *) nLogicalAddress[3])));

    }
	else
	{
		VFM_Port    *pt_port_out;
		pt_port_out = (VFM_Port*)pProxyComponent->getPort(VPB+1);
		OMX_U32 width = pt_port_out->getFrameWidth();
		width +=(16-(width%16))%16;
		//+ER329503
		if(!width)
		{
			width=16;
		}
		//-ER329503
		OMX_U32 MB_width=width/16;
		OMX_U32 height = pt_port_out->getFrameHeight();
		height +=(16-(height%16))%16;
		//+ER329503
		if(!height)
		{
			height=16;
		}
		//-ER329503
		OMX_U32 MB_height=height/16;
		OstTraceFiltStatic0(TRACE_FLOW, "MPEG4Dec_ProcessingComp :allocateRequiredMemory  mv for HOST \n",pProxyComponent);
		//addr_mv_type_buffer
		nLogicalAddress[0]  = (t_uint32)VFM_Alloc(vfm_mem_ctxt, ( MB_width * MB_height * 92), VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
		if(!nLogicalAddress[0])
		{
			VFM_CloseMemoryList(vfm_mem_ctxt);
			vfm_mem_ctxt = 0;
			return OMX_ErrorInsufficientResources;
		}
		memset ( (void *)((nLogicalAddress[0]) + 0), 0x00, ( MB_width * MB_height * 92));
        pInternal_buf->addr_mv_type_buffer= ((t_uint32)(nLogicalAddress[0]));

        pInternal_buf->addr_mb_not_coded= 0;
		pInternal_buf->addr_x_err_res_buffer=0;
		pInternal_buf->addr_y_err_res_buffer= 0;

	}
	set_perf_parameters();

	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::allocateRequiredMemory()", pProxyComponent);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::codecConfigure()
{
	OstTraceFiltStatic0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::codecConfigure()", pProxyComponent);

	//+ER345950
	codecConfigure_internal();
	allocate_internal_memory=OMX_TRUE;
	//-ER345950

	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::codecConfigure()", pProxyComponent);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::doSpecificEventHandle_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2)
{
	OstTraceInt3(TRACE_FLOW, "<=> MPEG4Dec_ProcessingComp::doSpecificEventHandle_cb(): event = %d, nData1 = %d, nData2 = %d", event, nData1, nData2);
	return OMX_ErrorNone;
}

void MPEG4Dec_ProcessingComp::registerStubsAndSkels()
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::registerStubsAndSkels()");
#ifndef __MPEG4DEC_SOFT_DECODER
	if(pProxyComponent->isMPCobject)
    {
        CM_REGISTER_STUBS_SKELS(video_mpeg4dec_cpp);
    }
    else
    {
        CM_REGISTER_STUBS_SKELS(video_mpe4dec_sw_cpp);
    }
#endif

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::registerStubsAndSkels()");
}

void MPEG4Dec_ProcessingComp::unregisterStubsAndSkels()
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::unregisterStubsAndSkels()");
#ifndef __MPEG4DEC_SOFT_DECODER
	if(pProxyComponent->isMPCobject)
    {
        CM_UNREGISTER_STUBS_SKELS(video_mpeg4dec_cpp);
    }
    else
    {
        CM_UNREGISTER_STUBS_SKELS(video_mpe4dec_sw_cpp);
    }
#endif

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::unregisterStubsAndSkels()");
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::codecStart()
{
	OstTraceFiltInst0(TRACE_FLOW, "<=> MPEG4Dec_ProcessingComp::codecStart()");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::codecStop()
{
	OstTraceFiltInst0(TRACE_FLOW, "<=> MPEG4Dec_ProcessingComp::codecStop()");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::codecInstantiate()
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::codecInstantiate()");

    OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();
    RETURN_XXX_IF_WRONG((EnsWrapper_bindToUser(OMXHandle, mCodec, "iChangePortSettingsSignal",(mpeg4dec_arm_nmf_api_portSettingsDescriptor*)this, 4)==NMF_OK), OMX_ErrorInsufficientResources);
    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(0),
												   "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(1),
												   "fillthisbuffer", "outputport"));
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("iMemoryRequirements", 8, &(iProvideMemory))==NMF_OK), OMX_ErrorInsufficientResources);
    RETURN_XXX_IF_WRONG((mCodec->bindFromUser("portSettingsInfoSignal", 8,  &(iPortSettingsInfoSignal))==NMF_OK), OMX_ErrorInsufficientResources);

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::codecInstantiate()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::codecDeInstantiate()
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::codecDeInstantiate()");

    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(0),
												   "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(1),
												   "fillthisbuffer", "outputport"));
	RETURN_XXX_IF_WRONG((EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mCodec,"iChangePortSettingsSignal")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("portSettingsInfoSignal")==NMF_OK), OMX_ErrorUndefined);
    RETURN_XXX_IF_WRONG((mCodec->unbindFromUser("iMemoryRequirements")==NMF_OK), OMX_ErrorUndefined);
	firstFrame = OMX_TRUE;

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::codecDeInstantiate()");

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::codecCreate(OMX_U32 domainId)
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::codecCreate()");

	if(pProxyComponent->isMPCobject)
	{
#ifndef __MPEG4DEC_SOFT_DECODER
			mCodec = mpeg4dec_arm_nmf_mpeg4deccomparmCreate();
			if(mCodec==0)
			{
				OstTraceFiltInst1(TRACE_ERROR, "ERROR: codecCreate() returning OMX_ErrorUndefined at line %d", __LINE__);
				return OMX_ErrorUndefined;
			}
		    ((mpeg4dec_arm_nmf_mpeg4deccomparm *)mCodec)->domainSVACodec = static_cast<t_cm_domain_id>(domainId);
#endif
	}
	else
	{
			mCodec = mpeg4dec_arm_nmf_mpeg4dec_swCreate();
			if(mCodec==0)
            {
				OstTraceFiltInst1(TRACE_ERROR, "ERROR: codecCreate() returning OMX_ErrorUndefined at line %d", __LINE__);
                return OMX_ErrorUndefined;
            }
#ifndef __MPEG4DEC_SOFT_DECODER
		    ((mpeg4dec_arm_nmf_mpeg4dec_sw *)mCodec)->domainSVACodec = static_cast<t_cm_domain_id>(domainId);
#endif
	}

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::codecCreate()");

	return OMX_ErrorNone;
}

void MPEG4Dec_ProcessingComp::codecDestroy(void)
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::codecDestroy()");

	if(pProxyComponent->isMPCobject)
	{
#ifndef __MPEG4DEC_SOFT_DECODER
		mpeg4dec_arm_nmf_mpeg4deccomparmDestroy((mpeg4dec_arm_nmf_mpeg4deccomparm *&)mCodec);
#endif
	}
	else
	{
		mpeg4dec_arm_nmf_mpeg4dec_swDestroy((mpeg4dec_arm_nmf_mpeg4dec_sw *&)mCodec);
	}

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::codecDestroy()");
}

void MPEG4Dec_ProcessingComp::sendConfigToCodec()
{
	OstTraceFiltInst0(TRACE_FLOW, "<=> MPEG4Dec_ProcessingComp::sendConfigToCodec()");
}

OMX_BOOL MPEG4Dec_ProcessingComp::GetDeblockingFlag() const
{
	OstTraceFiltStatic1(TRACE_FLOW, "<=> MPEG4Dec_ProcessingComp::GetDeblockingFlag() deblocking_enable = %d", pProxyComponent, deblocking_enable);
	return deblocking_enable;
}

OMX_BOOL MPEG4Dec_ProcessingComp::GetIndex_macroblock_error_reporting() const
{
    OstTraceFiltInst1(TRACE_FLOW, "<=> MPEG4Dec_ProcessingComp::GetIndex_macroblock_error_reporting() error_reporting_enable = %d \n", error_reporting_enable);
	if(error_reporting_enable & 1)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

void MPEG4Dec_ProcessingComp::SetIndex_macroblock_error_reporting(OMX_BOOL flag)
{
    VFM_Port    *pt_port_out;
	pt_port_out = (VFM_Port*)pProxyComponent->getPort(VPB+1);
    OMX_U32 width = pt_port_out->getFrameWidth();
	OMX_U32 height = pt_port_out->getFrameHeight();
	if(flag)
	error_reporting_enable |= 1;
	if(pProxyComponent->thumbnail)
		error_reporting_enable |= 4;

	OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec_ProcessingComp::SetIndex_macroblock_error_reporting error_reporting_enable %d \n",error_reporting_enable);

	iPortSettingsInfoSignal.sendportSettings(width,height,OMX_TRUE,pProxyComponent->sorenson_flag,AspectRatio,ColorPrimary,error_reporting_enable);
}
void MPEG4Dec_ProcessingComp::GetIndex_macroblock_error_map(OMX_PTR pt) const
{
    OMX_CONFIG_MACROBLOCKERRORMAPTYPE *pt_video = (OMX_CONFIG_MACROBLOCKERRORMAPTYPE*)pt;
	OMX_U32 *ptr = (OMX_U32*)pt_video->ErrMap;
	memcpy((void *)ptr,(void *)error_map_copy,pt_video->nErrMapSize);
    memset((void *)error_map_copy, 0, 225*sizeof(t_uint16));

	OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::GetIndex_macroblock_error_map\n");
}
void MPEG4Dec_ProcessingComp::GetIndexParamVideoFastUpdate(OMX_PTR pt) const
{
    OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE *pt_video = (OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE *)pt;
	pt_video->nFirstGOB = error_map_copy[225];
    pt_video->nFirstMB =  error_map_copy[226];
    pt_video->nNumMBs =   error_map_copy[227];
    memset((void *)(error_map_copy + 225), 0,3*sizeof(t_uint16));
	OstTraceFiltStatic0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::GetIndexParamVideoFastUpdate\n",pProxyComponent);

}
void MPEG4Dec_ProcessingComp::SetIndexParamVideoFastUpdate(OMX_BOOL flag)
{
    VFM_Port    *pt_port_out;
	pt_port_out = (VFM_Port*)pProxyComponent->getPort(VPB+1);
    OMX_U32 width = pt_port_out->getFrameWidth();
	OMX_U32 height = pt_port_out->getFrameHeight();
	if(flag)
	error_reporting_enable |= 2;
	if(pProxyComponent->thumbnail)
		error_reporting_enable |= 4;
	OstTraceFiltStatic0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::SetIndexParamVideoFastUpdate\n",pProxyComponent);
    iPortSettingsInfoSignal.sendportSettings(width,height,OMX_TRUE,pProxyComponent->sorenson_flag,AspectRatio,ColorPrimary,error_reporting_enable);
}
OMX_ERRORTYPE MPEG4Dec_ProcessingComp::getIndexParamPixelAspectRatio(OMX_PTR pt) const
{
    OMX_PARAM_PIXELASPECTRATIO *p_pixel_aspect_ratio = (OMX_PARAM_PIXELASPECTRATIO*)pt;
	if(AspectRatio==KExtendedPar)
	{
		p_pixel_aspect_ratio->nHorizontal = nHorizontal;
		p_pixel_aspect_ratio->nVertical   = nVertical;
	}
	else if(AspectRatio==1)
	{
		p_pixel_aspect_ratio->nHorizontal = 1;
		p_pixel_aspect_ratio->nVertical   = 1;
	}
	else if(AspectRatio==2)
	{
		p_pixel_aspect_ratio->nHorizontal = 12;
		p_pixel_aspect_ratio->nVertical   = 11;
	}
	else if(AspectRatio==3)
	{
		p_pixel_aspect_ratio->nHorizontal = 10;
		p_pixel_aspect_ratio->nVertical   = 11;
	}
	else if(AspectRatio==4)
	{
		p_pixel_aspect_ratio->nHorizontal = 16;
		p_pixel_aspect_ratio->nVertical   = 11;
	}
	else if(AspectRatio==5)
	{
		p_pixel_aspect_ratio->nHorizontal = 40;
		p_pixel_aspect_ratio->nVertical   = 33;
	}
	else
	{
		p_pixel_aspect_ratio->nHorizontal = 0;
		p_pixel_aspect_ratio->nVertical   = 0;
	}
     OstTraceFiltStatic0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::getIndexParamPixelAspectRatio\n",pProxyComponent);
    return OMX_ErrorNone;
}
OMX_ERRORTYPE MPEG4Dec_ProcessingComp::getIndexParamColorPrimary(OMX_PTR pt) const
{
	 OMX_CONFIG_COLORPRIMARY *p_color_primary = (OMX_CONFIG_COLORPRIMARY*)pt;
     p_color_primary->eColorPrimary = (OMX_SYMBIAN_COLORPRIMARYTYPE)ColorPrimary;
	 OstTraceFiltStatic0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::getIndexParamColorPrimary\n",pProxyComponent);
	return OMX_ErrorNone;
}
OMX_ERRORTYPE MPEG4Dec_ProcessingComp::setIndexParamColorPrimary(OMX_PTR pt)
{
	OMX_CONFIG_COLORPRIMARY *p_color_primary = (OMX_CONFIG_COLORPRIMARY*)pt;
	OstTraceFiltStatic1(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::setIndexParamColorPrimary() eColorPrimary = 0x%x", pProxyComponent, p_color_primary->eColorPrimary);
	ColorPrimary=(t_uint8)p_color_primary->eColorPrimary;

	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::setIndexParamColorPrimary", pProxyComponent);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::setIndexParamPixelAspectRatio(OMX_PTR pt)
{
	OMX_PARAM_PIXELASPECTRATIO *p_pixel_aspect_ratio = (OMX_PARAM_PIXELASPECTRATIO*)pt;

	OstTraceFiltStatic2(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::setIndexParamPixelAspectRatio(): nHorizontal = %d, nVertical = %d", pProxyComponent, p_pixel_aspect_ratio->nHorizontal, p_pixel_aspect_ratio->nVertical);

	if(p_pixel_aspect_ratio->nHorizontal == 1 && p_pixel_aspect_ratio->nVertical   ==1)
		AspectRatio=1;
	else if(p_pixel_aspect_ratio->nHorizontal == 12 && p_pixel_aspect_ratio->nVertical   ==11)
		AspectRatio=2;
	else if(p_pixel_aspect_ratio->nHorizontal == 10 && p_pixel_aspect_ratio->nVertical   ==11)
		AspectRatio=3;
	else if(p_pixel_aspect_ratio->nHorizontal == 16 && p_pixel_aspect_ratio->nVertical   ==11)
		AspectRatio=4;
	else if(p_pixel_aspect_ratio->nHorizontal == 40 && p_pixel_aspect_ratio->nVertical   ==33)
		AspectRatio=5;
	else
		AspectRatio=KExtendedPar;

	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::setIndexParamPixelAspectRatio()", pProxyComponent);

    return OMX_ErrorNone;
}
void MPEG4Dec_ProcessingComp::portSettings(t_uint32 width,t_uint32 height,
										   t_uint8 aspect_ratio,t_uint8 nhorizontal,
									       t_uint8 nvertical,t_uint8 color_primary)
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::portSettings()");
	/* +Change for ER 426137 */
    MPEG4Dec_ParamAndConfig *pParam = ((MPEG4Dec_ParamAndConfig *)(pProxyComponent->getParamAndConfig()));
    /* -Change for ER 426137 */
    AspectRatio=aspect_ratio;
	if(AspectRatio==KExtendedPar)
	{
		nHorizontal=nhorizontal;
		nVertical=nvertical;
	}
	/* +Change for ER 426137 and ER 432847,435398 */
	//We check now whether the resolution that was set earlier, using the
	//	container info (pParam->CropWidth) is greater than the info found
	// in the elementary stream. In that case, we cannot consider the old
	// resolution as a crop resolution because crop vectors would go out-
	// side the frame. So we update the width/height with the new width/
	// height found in the elementary stream. 
	// Also, in the unlikely case that only one the width or height is 
	// invalid, then the following code also takes care of the fact that 
	// only that value will be overriden and not the other. 
	// So, effectively, we are setting here the cropping vectors to the 
	// smaller of the two values, for both width and height.
	if (pParam->CropWidth > width ) {
		pParam->CropWidth = width;
	}
	if(pParam->CropHeight > height) {
		pParam->CropHeight = height;
	}
	OstTraceFiltInst2(TRACE_FLOW, "MPEG4Dec_ProcessingComp::portSettings() Crop Width = %d, Width = %d", pParam->CropWidth, width);
	OstTraceFiltInst2(TRACE_FLOW, "MPEG4Dec_ProcessingComp::portSettings() Crop Height = %d, Height = %d", pParam->CropHeight, height);
	/* -Change for ER 426137 and ER 432847 */
	ColorPrimary=color_primary;
	pProxyComponent->updateOutputPortSettings((OMX_INDEXTYPE)1,width,height);

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::portSettings()");
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData)
{
    OstTraceInt2(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::sendCommand() eCmd = 0x%x, nData = 0x%x", eCmd, nData);

	VFM_Port    *pt_port_in;
	pt_port_in = (VFM_Port*)pProxyComponent->getPort(0);
	OMX_U32 width = pt_port_in->getFrameWidth();
	OMX_U32 height= pt_port_in->getFrameHeight();
	OMX_U32 thresholdValueDDR = (1280/16)*(720/16); //DDR to managed on 720p resolution
	OMX_U32 thresholdValueDVFS = (800/16)*(480/16); //DVFS and Latency managed on WVGA resolution
	OMX_U32 actualValue = (width/16)*(height/16);


         switch (eCmd)
         {
		case OMX_CommandStateSet:
			switch(nData)
			{
				case OMX_StateExecuting:
				case OMX_StatePause:
				case OMX_StateIdle:
							if((OMX_U32)OMX_StateExecuting==nData)
							{
								if (pProxyComponent->is_full_powermgt)
								{
									if (actualValue > thresholdValueDVFS) //for dvfs ER 345421
									{
										OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::setting full dvfs");
										VFM_PowerManagement::setFullDVFS(this);
                                	    VFM_PowerManagement::setLatency(this, VFM_PowerManagement::VFM_ResLatency_None);
										fullDVFSSet = OMX_TRUE;
									}
									if (actualValue > thresholdValueDDR) //for ddr ER 345421
									{
										OstTraceInt0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::setting full DDR");
										//+ ER335583
										VFM_PowerManagement::setFullDDR(this);
										//- ER335583
										fullDDRSet = OMX_TRUE;
									}
								}
								else
								{
									//always force it to 100%
										OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::forcing full dvfs");
										VFM_PowerManagement::setFullDVFS(this);
										//+ ER335583
										VFM_PowerManagement::setFullDDR(this);
										//- ER335583
                                	    VFM_PowerManagement::setLatency(this, VFM_PowerManagement::VFM_ResLatency_None);
										fullDVFSSet = OMX_TRUE;
										fullDDRSet = OMX_TRUE;
								}
							}
							else
							{
								if (fullDVFSSet)
								{

									VFM_PowerManagement::releaseFullDVFS(this);
                                    VFM_PowerManagement::resetLatency(this);
									fullDVFSSet = OMX_FALSE;
								}
								if (fullDDRSet) //for ER 345421
								{
									//+ ER335583
									VFM_PowerManagement::releaseFullDDR(this);
									//- ER335583
									fullDDRSet = OMX_FALSE;
								}
							}

					break;
				default:
					break;
			}
			break;//+ER345950
		 case OMX_CommandPortDisable:
			 if(allocate_internal_memory && (nData!=0))
			 {
				 allocate_internal_memory=OMX_FALSE;
				 destroy_internal();
			 }
			 break;
		 case OMX_CommandPortEnable:
			 if((!allocate_internal_memory) && (nData!=0))
			 {
				 allocate_internal_memory=OMX_TRUE;
				 codecConfigure_internal();
			 }
			 break;//-ER345950
                default:
                        break;
         }

    OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::sendCommand()");

	return NmfHost_ProcessingComponent::sendCommand(eCmd,nData);

}

void MPEG4Dec_ProcessingComp::set_perf_parameters()
{
	OstTraceFiltStatic0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::set_perf_parameters()", pProxyComponent);

    t_uint8 *ptr;
	ts_ddep_vdc_mpeg4_param_desc *ps_ddep_vdc_mpeg4_param_desc;
	ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
	ps_ddep_vdc_mpeg4_param_desc = (ts_ddep_vdc_mpeg4_param_desc *) ptr;
    pProxyComponent->pFwPerfDataPtr =(OMX_PTR)(&(ps_ddep_vdc_mpeg4_param_desc->s_out_perf_parameters));

	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::set_perf_parameters()", pProxyComponent);
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::setIndexParamResourceSvaMcps(OMX_PTR pPtr)
{
	OstTraceFiltStatic0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::setIndexParamResourceSvaMcps()", pProxyComponent);

    VFM_PARAM_RESOURCEESTIMATION_TYPE* pMcpsType = (VFM_PARAM_RESOURCEESTIMATION_TYPE*)pPtr;
    OMX_U32 mcps = pMcpsType->nSvaMips;
    if (mcps > 200) {
		OstTraceFiltStatic1(TRACE_ERROR, "ERROR: setIndexParamResourceSvaMcps() returning OMX_ErrorBadParameter at line %d", pProxyComponent, __LINE__);
        return OMX_ErrorBadParameter;
    }
    resourceSvaMcps(mcps);

	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::setIndexParamResourceSvaMcps()", pProxyComponent);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::getIndexParamResourceSvaMcps(OMX_PTR pPtr) const
{
	OstTraceFiltStatic0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::getIndexParamResourceSvaMcps()", pProxyComponent);

    VFM_PARAM_RESOURCEESTIMATION_TYPE* pMcpsType = (VFM_PARAM_RESOURCEESTIMATION_TYPE*)pPtr;
    pMcpsType->nSvaMips = resourceSvaMcps();

	OstTraceFiltStatic0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::getIndexParamResourceSvaMcps()", pProxyComponent);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::setIndexParamthumbnailgeneration(OMX_PTR pPtr)
{
    OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::setIndexParamthumbnailgeneration()");

    VFM_PARAM_THUMBNAIL_GENERATION_TYPE* ThumbType = (VFM_PARAM_THUMBNAIL_GENERATION_TYPE*)pPtr;
    pProxyComponent->thumbnail=ThumbType->bThumbnailGeneration;

    OstTraceFiltInst1(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::setIndexParamthumbnailgeneration = %d()", pProxyComponent->thumbnail);

    return OMX_ErrorNone;
}
/*OMX_ERRORTYPE MPEG4Dec_ProcessingComp::setIndexParamSupportedExtension(OMX_PTR pt_org)
{
    VFM_PARAM_SUPPORTEDEXTENSION_TYPE *pt = (VFM_PARAM_SUPPORTEDEXTENSION_TYPE *)pt_org;
    mSupportedExtension = pt->nMask;
	OstTraceInt0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::setIndexParamSupportedExtension\n");
    return OMX_ErrorNone;
}*/

/*OMX_ERRORTYPE MPEG4Dec_ProcessingComp::getIndexParamSupportedExtension(OMX_PTR pt_org) const
{
    VFM_PARAM_SUPPORTEDEXTENSION_TYPE *pt = (VFM_PARAM_SUPPORTEDEXTENSION_TYPE *)pt_org;
    pt->nMask = mSupportedExtension;
	OstTraceInt0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::getIndexParamSupportedExtension\n");
    return OMX_ErrorNone;
}*/

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::errorRecoveryDestroyAll()
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::errorRecoveryDestroyAll()");

	mCodec->stop();
	codecStop();

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::errorRecoveryDestroyAll()");

	return VFM_NmfHost_ProcessingComponent::deInstantiate();
}

//+ER345950
OMX_ERRORTYPE MPEG4Dec_ProcessingComp::codecConfigure_internal()
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::codecConfigure_internal()");

	VFM_Port    *pt_port_out,*pt_port_in;
	pt_port_in = (VFM_Port*)pProxyComponent->getPort(VPB+0);
	pt_port_out = (VFM_Port*)pProxyComponent->getPort(VPB+1);
	OMX_U32 width = pt_port_out->getFrameWidth();
	width +=(16-(width%16))%16;
	//+ER329503
	if(!width)
	{
		width=16;
	}
	//-ER329503
	OMX_U32 MB_width=width/16;
	OMX_U32 height = pt_port_out->getFrameHeight();
	height +=(16-(height%16))%16;
	//+ER329503
	if(!height)
	{
		height=16;
	}
	//-ER329503
	OMX_U32 MB_height=height/16;
	t_cm_system_address sysAddr;
	t_uint32 	deblocking_struct[10]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	t_uint32 buffer_i_frame[2] = {0x00, 0x00};

	//deblocking supported only till 720p as for higher resolution performance degrades
	/* +change for 346056 */
	if (((width*height) < (64*48)) || ((width*height) > (1280*720)))
	/* -change for 346056 */
    {
		OstTraceInt0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::codecConfigure_internal(): Deblocking disabled for > 720p");
		deblocking_enable=OMX_FALSE;
	}
	// buffer allocated for bitstream copy in case there is a need
	{
		  OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::codecConfigure_internal(): Internal Bitstream Buffer allocation");
		  if(pProxyComponent->isMPCobject)
		  {
			references_bufaddress[4]  = (t_uint32)VFM_Alloc(vfm_mem_ctxt, (width * height * 1.5 ), VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
		  }
		  else
		  {
             if(((width % 32) != 0) && ((height % 32) != 0))
             {
                references_bufaddress[4]  = (t_uint32)VFM_Alloc(vfm_mem_ctxt, (width * height * 1.5 + 256), VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
             }
             else
             {
                 references_bufaddress[4]  = (t_uint32)VFM_Alloc(vfm_mem_ctxt, (width * height * 1.5), VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
             }
          }
		  if(!references_bufaddress[4])
		  {
			  VFM_CloseMemoryList(vfm_mem_ctxt);
			  vfm_mem_ctxt = 0;
			  return OMX_ErrorInsufficientResources;
		  }
		  //+ER344944
		  memset ( (void *)((references_bufaddress[4]) + 0), 0x00, (width * height * 1.5 ));
		  //-ER344944
		  sysAddr.physical = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, (t_uint8*)references_bufaddress[4]));
		  sysAddr.physical = ((sysAddr.physical+0xf)&0xfffffff0);
		  if(pProxyComponent->isMPCobject)
			  deblocking_struct[8]=(sysAddr.physical);
		  else
			  deblocking_struct[8]=(references_bufaddress[4]);
		  deblocking_struct[9]=(references_bufaddress[4]);
    }
	//parameter buffers used for deblocking
	if(deblocking_enable)
	{
	//deblocking param buffer
	 for(OMX_U32 i=0;i<4;i++)
	 {
		 OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec_ProcessingComp::codecConfigure_internal(): Deblocking param buffer allocation");
		  if(pProxyComponent->isMPCobject)
		  {
			deblock_param_address[i]  = (t_uint32)VFM_Alloc(vfm_mem_ctxt, (8 * MB_width * MB_height), VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
		  }
		  else
		  {//+ER344943
			deblock_param_address[i]  = (t_uint32)VFM_Alloc(vfm_mem_ctxt, (16 * MB_width * MB_height), VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
		  }//-ER344943
		  if(!deblock_param_address[i])
		  {
			  VFM_CloseMemoryList(vfm_mem_ctxt);
			  vfm_mem_ctxt = 0;
			  return OMX_ErrorInsufficientResources;
		  }
		  //+ER344944
		  if(pProxyComponent->isMPCobject)
		  {
			  memset ( (void *)((deblock_param_address[i]) + 0), 0x00, (8 * MB_width * MB_height));
		  }
		  else
		  {
			  memset ( (void *)((deblock_param_address[i]) + 0), 0x00, (16 * MB_width * MB_height));
		  }
		  //-ER344944
		  sysAddr.physical = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, (t_uint8*)deblock_param_address[i]));
		  sysAddr.physical = ((sysAddr.physical+0xf)&0xfffffff0);
		  if(pProxyComponent->isMPCobject)
			  deblocking_struct[i]=(sysAddr.physical);
		  else
			  deblocking_struct[i]=(deblock_param_address[i]);
	 }
	 //buffers used internally for reference
	 for(OMX_U32 i=0;i<4;i++)
	 {
		 OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec_ProcessingComp:: internal reference buffer allocation with deblocking");
		 if(pProxyComponent->isMPCobject)
		  {
			references_bufaddress[i]  = (t_uint32)VFM_Alloc(vfm_mem_ctxt,(t_uint32)(width * height * 1.5), VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
		  }
		  else
		  {
			references_bufaddress[i]  = (t_uint32)VFM_Alloc(vfm_mem_ctxt,(t_uint32)(width * height * 1.5), VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
		  }
		 if(!references_bufaddress[i])
		  {
			  VFM_CloseMemoryList(vfm_mem_ctxt);
			  vfm_mem_ctxt = 0;
			  return OMX_ErrorInsufficientResources;
		  }
		  //+ER344944
		  //+ER354510
		  memset ( (void *)((references_bufaddress[i]) + 0), 0x00, (width * height * 1));
		  memset ( (void *)((references_bufaddress[i]) + (width*height)), 0x80, (width * height * 0.5));
		  //-ER354510
		  //-ER344944
		  sysAddr.physical = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, (t_uint8*)references_bufaddress[i]));
		  sysAddr.physical = ((sysAddr.physical+0xf)&0xfffffff0);
		  if(pProxyComponent->isMPCobject)
			  deblocking_struct[i+4]=(sysAddr.physical);
		  else
			  deblocking_struct[i+4]=(references_bufaddress[i]);
	 }
	}
	else
	{
		OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec_ProcessingComp:: Internal reference buffer allocation");

		if(pProxyComponent->isMPCobject)
		  {
			references_bufaddress[0]  = (t_uint32)VFM_Alloc(vfm_mem_ctxt,(t_uint32)(width * height * 1.5), VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
		  }
		  else
		  {
			references_bufaddress[0]  = (t_uint32)VFM_Alloc(vfm_mem_ctxt,(t_uint32)(width * height * 1.5), VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
		  }
		if(!references_bufaddress[0])
		{
			VFM_CloseMemoryList(vfm_mem_ctxt);
			vfm_mem_ctxt = 0;
			return OMX_ErrorInsufficientResources;
		}
		//+ER344944
		//+ER354510
		memset ( (void *)((references_bufaddress[0]) + 0), 0x00, (width * height * 1));
		memset ( (void *)((references_bufaddress[0]) + (width*height)), 0x80, (width * height * 0.5));
		//-ER354510
		//-ER344944
		sysAddr.physical = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, (t_uint8*)references_bufaddress[0]));
		sysAddr.physical = ((sysAddr.physical+0xf)&0xfffffff0);
		if(pProxyComponent->isMPCobject)
			deblocking_struct[4]=(sysAddr.physical);
		else
			deblocking_struct[4]=(references_bufaddress[0]);

	}
	if(pProxyComponent->thumbnail)
		error_reporting_enable |= 4;

#if 1
// Immediate I frame release
	if(pProxyComponent->isMPCobject)
	{
		OstTraceInt0(TRACE_FLOW, "MPEG4Dec_ProcessingComp(): Allocation for buffer copy for I frame");

		buffer_i_frame_logical = (t_uint32)VFM_Alloc(vfm_mem_ctxt, (width * height * 1.5 ), VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

		OstTraceInt1(TRACE_FLOW, "MPEG4Dec_ProcessingComp(): buffer_i_frame_logical = 0x%x", buffer_i_frame_logical);

		if(!buffer_i_frame_logical)
		{
			VFM_CloseMemoryList(vfm_mem_ctxt);
			vfm_mem_ctxt = 0;
			return OMX_ErrorInsufficientResources;
		}

		memset ( (void *)((buffer_i_frame_logical) + 0), 0x00, (width * height * 1.5 ));

		sysAddr.physical = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, (t_uint8*)buffer_i_frame_logical));
		sysAddr.physical = ((sysAddr.physical+0xf)&0xfffffff0);
		buffer_i_frame[0] = (sysAddr.physical);	//Physical Address
		buffer_i_frame[1] = (buffer_i_frame_logical);	//Logical Address


		OstTraceInt2(TRACE_FLOW, "codecConfigure_internal(): buffer_i_frame[0] (Physical) = 0x%x, buffer_i_frame[1] (Logical) = 0x%x", buffer_i_frame[0], buffer_i_frame[1]);

		iProvideMemory.provideBufferForI(buffer_i_frame);
	}
#endif

	iProvideMemory.memoryRequirements(deblocking_struct,mParamBufferDesc,mLinkListBufferDesc,mDebugBufferDesc,error_reporting_enable,error_map_copy,(t_uint8)pt_port_in->getBufferCountActual(),pProxyComponent->mIsARMLoadComputed,vfm_mem_ctxt);

	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::codecConfigure_internal()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Dec_ProcessingComp::destroy_internal(void)
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::destroy_internal()");

    if(deblocking_enable)
	{
		for(OMX_U32 i=0;i<5;i++)
		{
			if (references_bufaddress[i])
			{
				VFM_Free(vfm_mem_ctxt, (t_uint8*)references_bufaddress[i]);
				references_bufaddress[i] = 0;
			}
		}
		for(OMX_U32 i=0;i<4;i++)
		{
			if (deblock_param_address[i])
			{
				VFM_Free(vfm_mem_ctxt, (t_uint8*)deblock_param_address[i]);
				deblock_param_address[i] = 0;
			}
		}
		OstTraceInt0(TRACE_FLOW, "MPEG4Dec_ProcessingComp:: Destroy called with deblocking");

	}
	else
	{
		OstTraceInt0(TRACE_FLOW, "MPEG4Dec_ProcessingComp:: Destroy called without deblocking");
		if (references_bufaddress[0])
		{
			VFM_Free(vfm_mem_ctxt, (t_uint8*)references_bufaddress[0]);
			references_bufaddress[0] = 0;
		}
		if (references_bufaddress[4])
		{
			VFM_Free(vfm_mem_ctxt, (t_uint8*)references_bufaddress[4]);
			references_bufaddress[4] = 0;
		}
	}


	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::destroy_internal()");

	return OMX_ErrorNone;
}

//-ER345950
OMX_BOOL MPEG4Dec_ProcessingComp::isNMFPanicSource(t_panic_source ee_type, OMX_U32 faultingComponent)
{
	OstTraceFiltInst2(TRACE_FLOW, "=> MPEG4Dec_ProcessingComp::isNMFPanicSource() ee_type = 0x%x, faultingComponent = 0x%x", ee_type, faultingComponent);

	switch(ee_type)
	{
		case MPC_EE :
			if(mCodec->getMPCComponentHandle("cMpeg4decDdep")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cGenericFsm")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cComponentFsm")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cOSTTrace")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cSharedBufInMPC")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cSharedBufOutMPC")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cResource_manager")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cVpp")==faultingComponent	||
			   mCodec->getMPCComponentHandle("cMpeg4decAlgo")==faultingComponent
			   )
			{
                if(mCodec->getMPCComponentHandle("cMpeg4decDdep")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cMpeg4decDdep");
                }
                if(mCodec->getMPCComponentHandle("cGenericFsm")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cGenericFsm");
                }
                if(mCodec->getMPCComponentHandle("cComponentFsm")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cComponentFsm");
                }if(mCodec->getMPCComponentHandle("cOSTTrace")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cOSTTrace");
                }
                if(mCodec->getMPCComponentHandle("cSharedBufInMPC")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cSharedBufInMPC");
                }
                if(mCodec->getMPCComponentHandle("cSharedBufOutMPC")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cSharedBufOutMPC");
                }
                if(mCodec->getMPCComponentHandle("cResource_manager")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cResource_manager");
                }
                if(mCodec->getMPCComponentHandle("cVpp")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cVpp");
                }
                if(mCodec->getMPCComponentHandle("cMpeg4decAlgo")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cMpeg4decAlgo");
                }

				return OMX_TRUE;
			}

            break;
		case HOST_EE :
			if((void*)mCodec == (void*)faultingComponent){
                OstTraceInt0(TRACE_FLOW, "Panic::ARM Component");
                return OMX_TRUE;
            }
			else
				return OMX_FALSE;
			break;
		default:
			break;

	}

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec_ProcessingComp::isNMFPanicSource()");
	return OMX_FALSE;
}


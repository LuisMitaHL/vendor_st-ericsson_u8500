/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_jpegdec

#include "osi_trace.h"
#include "VFM_DDepUtility.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegdec_proxy_src_JPEGDec_NmfMpc_ProcessingComponentTraces.h"
#endif

#include "VFM_Port.h"
#include "JPEGDec_NmfMpc_ProcessingComponent.h"
#include "JPEGDec_Proxy.h"
#include "SharedBuffer.h"
//#include "hw_buffer_lib.h"
#ifdef __PERF_MEASUREMENT
#include "../test_arm/osi_perf.h"
#endif

#define RETURN_OMX_ERROR_IF_ERROR_OST(_x)    { OMX_ERRORTYPE _error; _error = (_x); if (_error!=OMX_ErrorNone) { jpegdec_nmfmpc_assert(_error, __LINE__, OMX_FALSE); return _error; } }


#define MAX_WIDTH 8176

#define DEBUG_MEM_SIZE (32*1024 - 1)
#define BUFFER_LINKLIST_SIZE 1024

volatile ts_ddep_buffer_descriptor g_mDebugBufferDescjpegDec;

void JPEGDec_NmfMpc_ProcessingComponent::jpegdec_nmfmpc_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
	OstTraceFiltStatic0(TRACE_API, "JPEGDECMPC jpegdec_nmfmpc_assert \n",pProxyComponent);
    if (OMX_ErrorNone != omxError)
    {
        OstTraceFiltStatic2(TRACE_ERROR, "JPEGDECMPC : errorType : 0x%x error line no %d\n",pProxyComponent,omxError,line);
        //NMF_LOG("JPEGDECMPC : errorType : 0x%x error line %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}


JPEGDec_NmfMpc_ProcessingComponent::JPEGDec_NmfMpc_ProcessingComponent(ENS_Component &enscomp)
						:VFM_NmfHost_ProcessingComponent(enscomp)
{

	pProxyComponent = (JPEGDec_Proxy *)&enscomp;
	memoryAllocated =0;
	mChannelId = 0;

	mDebugBufferDesc.nSize = 0;
	mDebugBufferDesc.nMpcAddress = 0;
	mDebugBufferDesc.nLogicalAddress = 0;
	mDebugBufferDesc.nPhysicalAddress = 0;

	//mDebugBufferHandle = (t_cm_memory_handle)0;

	mParamBufferDesc.nSize = 0;
	mParamBufferDesc.nMpcAddress = 0;
	mParamBufferDesc.nLogicalAddress = 0;
	mParamBufferDesc.nPhysicalAddress = 0;

	//mParamBufferHandle = (t_cm_memory_handle)0;

	mLinkListBufferDesc.nSize = 0;
	mLinkListBufferDesc.nMpcAddress = 0;
	mLinkListBufferDesc.nLogicalAddress = 0;
	mLinkListBufferDesc.nPhysicalAddress = 0;

	//mLinkListParamBufferHandle = (t_cm_memory_handle)0;

	//mLineBufferHandle = (t_cm_memory_handle)0;

	mLineBufferDesc.nSize = 0;
	mLineBufferDesc.nMpcAddress = 0;
	mLineBufferDesc.nLogicalAddress = 0;
	mLineBufferDesc.nPhysicalAddress = 0;
	mpc_vfm_mem_ctxt = 0;

	mHuffMemoryDesc.nLogicalAddress = 0;
	mHuffMemoryDesc.nMpcAddress = 0;
	mHuffMemoryDesc.nPhysicalAddress = 0;
	mHuffMemoryDesc.nSize = 0;

	isMemoryAllocated = OMX_FALSE;


	fullDVFSSet = OMX_FALSE;

}

JPEGDec_NmfMpc_ProcessingComponent::~JPEGDec_NmfMpc_ProcessingComponent()
{
	OstTraceFiltStatic0(TRACE_API, "JPEGDECMPC : ~JPEGDec_NmfMpc_ProcessingComponent \n",pProxyComponent);
	if (memoryAllocated)
	{
		OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC : memAllocated \n",pProxyComponent);
		if(mParamBufferDesc.nLogicalAddress)
		{
			OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC : parambufferDeAllocated \n",pProxyComponent);
			VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mParamBufferDesc.nLogicalAddress);
			mParamBufferDesc.nSize = 0;
			mParamBufferDesc.nLogicalAddress = 0;
			mParamBufferDesc.nPhysicalAddress = 0;
		}

		if(mHuffMemoryDesc.nLogicalAddress)
		{
			OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC : huffmemDEAllocated \n",pProxyComponent);
			VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mHuffMemoryDesc.nLogicalAddress);
			mHuffMemoryDesc.nSize = 0;
			mHuffMemoryDesc.nLogicalAddress = 0;
			mHuffMemoryDesc.nPhysicalAddress = 0;
		}

		if(mDebugBufferDesc.nLogicalAddress)
		{
			OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC : debugbufferDeAllocated \n",pProxyComponent);
			VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mDebugBufferDesc.nLogicalAddress);
			mDebugBufferDesc.nSize = 0;
			mDebugBufferDesc.nLogicalAddress = 0;
			mDebugBufferDesc.nPhysicalAddress = 0;
		}

		if(mLinkListBufferDesc.nLogicalAddress)
		{
			OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC : parambufferDeAllocated \n",pProxyComponent);
			VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mLinkListBufferDesc.nLogicalAddress);
			mLinkListBufferDesc.nSize = 0;
			mLinkListBufferDesc.nLogicalAddress = 0;
			mLinkListBufferDesc.nPhysicalAddress = 0;
		}

		if(mLineBufferDesc.nLogicalAddress)
		{
			OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC : linebufferDeAllocated \n",pProxyComponent);
			VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mLineBufferDesc.nLogicalAddress);
			mLineBufferDesc.nSize = 0;
			mLineBufferDesc.nLogicalAddress = 0;
			mLineBufferDesc.nPhysicalAddress = 0;
		}

		if(mpc_vfm_mem_ctxt)
		{
			OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC : mpc_vfm_mem_Allocated \n",pProxyComponent);
			VFM_CloseMemoryList(mpc_vfm_mem_ctxt);
		}

		mpc_vfm_mem_ctxt = 0;

	}
	isMemoryAllocated = OMX_FALSE;
}


OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::allocateRequiredMemory()
{

    t_uint32 size,linkBitstreamBufferSize, fakeBufferSize;
	OstTraceFiltStatic0(TRACE_API, "JPEGDECMPC : allocateRequiredMemory \n",pProxyComponent);

	linkBitstreamBufferSize = BUFFER_LINKLIST_SIZE;
    fakeBufferSize = (512) + VFM_ADDRESS_ALIGN_MASK;

	size = sizeof(ts_ddep_sdc_jpeg_param_desc)+ VFM_ADDRESS_ALIGN_MASK
           + sizeof(ts_ddep_sdc_jpeg_dynamic_configuration)+ VFM_ADDRESS_ALIGN_MASK
		   + 4*(sizeof(ts_ddep_sdc_jpeg_scan_desc)+ VFM_ADDRESS_ALIGN_MASK)
           + fakeBufferSize
           + 6*0xf ;//for alignment
	{
		OMX_U32 lineBufferSize;
		OMX_U8 *param_buf_logical_addr;
		OMX_U8 *link_buf_logical_addr;
		OMX_U8 *debug_buf_logical_addr;
		OMX_U8 *line_buf_logical_addr;
		OMX_U8 *huff_mem_logical_addr;

		param_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, size, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

		if(!param_buf_logical_addr)
		{
			jpegdec_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
			VFM_Free(mpc_vfm_mem_ctxt, param_buf_logical_addr);
			mpc_vfm_mem_ctxt = 0;
			OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : (!param_buf_logical_addr) hence return OMX_ErrorInsufficientResources\n",pProxyComponent);
			return OMX_ErrorInsufficientResources;
		}
		mParamBufferDesc.nSize = size;
		mParamBufferDesc.nLogicalAddress = (t_uint32)param_buf_logical_addr;
		mParamBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, param_buf_logical_addr));
		mParamBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, param_buf_logical_addr));
		OstTraceFiltStatic1(TRACE_FLOW, "JPEGDECMPC : Allocate memory for mParamBufferDesc : %d \n",pProxyComponent,mParamBufferDesc.nSize);


		huff_mem_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, sizeof(ts_t1xhv_vdc_sw_table), VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

		if(!huff_mem_logical_addr)
		{
			jpegdec_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
			VFM_Free(mpc_vfm_mem_ctxt, huff_mem_logical_addr);
			mpc_vfm_mem_ctxt = 0;
			OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : (!huff_mem_logical_addr) hence return OMX_ErrorInsufficientResources\n",pProxyComponent);
			return OMX_ErrorInsufficientResources;
		}
		mHuffMemoryDesc.nSize = sizeof(ts_t1xhv_vdc_sw_table);
		mHuffMemoryDesc.nLogicalAddress = (t_uint32)huff_mem_logical_addr;
		mHuffMemoryDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, huff_mem_logical_addr));
		mHuffMemoryDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, huff_mem_logical_addr));
		OstTraceFiltStatic1(TRACE_FLOW, "JPEGDECMPC : Allocate memory for mHuffMemoryDesc : %d \n",pProxyComponent,mHuffMemoryDesc.nSize);

		link_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, linkBitstreamBufferSize, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

		if(!link_buf_logical_addr)
		{
			jpegdec_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
			VFM_Free(mpc_vfm_mem_ctxt, link_buf_logical_addr);
			mpc_vfm_mem_ctxt = 0;
			OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : (!link_buf_logical_addr) hence return OMX_ErrorInsufficientResources\n",pProxyComponent);
			return OMX_ErrorInsufficientResources;
		}

		mLinkListBufferDesc.nSize = linkBitstreamBufferSize;
		mLinkListBufferDesc.nLogicalAddress = (t_uint32)link_buf_logical_addr;
		mLinkListBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, link_buf_logical_addr));
		mLinkListBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, link_buf_logical_addr));
		OstTraceFiltStatic1(TRACE_FLOW, "JPEGDECMPC : Allocate memory for mLinkListBufferDesc : %d \n",pProxyComponent,mLinkListBufferDesc.nSize);

		debug_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, DEBUG_MEM_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

		if(!debug_buf_logical_addr)
		{
			jpegdec_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
			VFM_Free(mpc_vfm_mem_ctxt, debug_buf_logical_addr);
			mpc_vfm_mem_ctxt = 0;
			OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : (!debug_buf_logical_addr) hence return OMX_ErrorInsufficientResources\n",pProxyComponent);
			return OMX_ErrorInsufficientResources;
		}

		mDebugBufferDesc.nSize = DEBUG_MEM_SIZE;
		mDebugBufferDesc.nLogicalAddress = (t_uint32)debug_buf_logical_addr;
		mDebugBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, debug_buf_logical_addr));
		mDebugBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, debug_buf_logical_addr));
		OstTraceFiltStatic1(TRACE_FLOW, "JPEGDECMPC : Allocate memory for mDebugBufferDesc : %d \n",pProxyComponent,mDebugBufferDesc.nSize);

		g_mDebugBufferDescjpegDec.nSize =  mDebugBufferDesc.nSize;
		g_mDebugBufferDescjpegDec.nLogicalAddress =  mDebugBufferDesc.nLogicalAddress;
		g_mDebugBufferDescjpegDec.nPhysicalAddress =  mDebugBufferDesc.nPhysicalAddress;
		g_mDebugBufferDescjpegDec.nMpcAddress =  mDebugBufferDesc.nMpcAddress;

		/* memory allocation for Line buffer */
		lineBufferSize = MAX_WIDTH/2;

		line_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, lineBufferSize, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

		if(!line_buf_logical_addr)
		{
			jpegdec_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
			VFM_Free(mpc_vfm_mem_ctxt, line_buf_logical_addr);
			mpc_vfm_mem_ctxt = 0;
			OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : (!line_buf_logical_addr) hence return OMX_ErrorInsufficientResources\n",pProxyComponent);
			return OMX_ErrorInsufficientResources;
		}
		mLineBufferDesc.nSize = lineBufferSize;
		mLineBufferDesc.nLogicalAddress = (t_uint32)line_buf_logical_addr;
		mLineBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, line_buf_logical_addr));
		mLineBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, line_buf_logical_addr));
		OstTraceFiltStatic1(TRACE_FLOW, "JPEGDECMPC : Allocate memory for mLineBufferDesc : %d \n",pProxyComponent,mLineBufferDesc.nSize);

        memset((t_uint8*)g_mDebugBufferDescjpegDec.nLogicalAddress,0x00,g_mDebugBufferDescjpegDec.nSize);
        memoryAllocated =1;
	}

    {
		//Zero initialize all param memory
		t_uint32 count;
		ts_ddep_sdc_jpeg_param_desc *ps_ddep_sdc_jpeg_param_desc;
		ps_ddep_sdc_jpeg_param_desc = (ts_ddep_sdc_jpeg_param_desc *)mParamBufferDesc.nLogicalAddress;
		for (count = 0; count<sizeof(ts_ddep_sdc_jpeg_param_desc);count++)
			((t_uint8*)ps_ddep_sdc_jpeg_param_desc)[count]=0;
    }


  return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer){
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nEnter ETB ProcComp.");
	#endif

	OstTraceFiltStatic3(TRACE_API, "JPEGDECMPC : In EmptyThisbuffer and pBuffer: %x nFilledLen : %d nFlags : %d \n",pProxyComponent,(OMX_U32)pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
	return NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer){
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nEnter FTB ProcComp.");
	#endif
	OstTraceFiltInst3(TRACE_API, "JPEGDECMPC : In fillThisBuffer and pBuffer: %x nFilledLen : %d nFlags : %d \n",(OMX_U32)pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
	return NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::construct(void)
{
	OstTraceFiltStatic0(TRACE_API, "JPEGDECMPC construct\n",pProxyComponent);
	if(!mpc_vfm_mem_ctxt)
	{
		OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC !mpc_vfm_mem_ctxt\n",pProxyComponent);
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		mpc_vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
	}

	if(!isMemoryAllocated) //if FALSE
	{
		OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC !isMemoryAllocated\n",pProxyComponent);
    	RETURN_OMX_ERROR_IF_ERROR_OST(allocateRequiredMemory());
    	isMemoryAllocated = OMX_TRUE;
	}

	return NmfHost_ProcessingComponent::construct();
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::destroy(void){
	return NmfHost_ProcessingComponent::destroy();
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::allocateBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_U32 nSizeBytes,
                OMX_U8 **ppData,
                void **bufferAllocInfo,
                void **portPrivateInfo)
{

	OMX_ERRORTYPE error;
	OstTraceFiltStatic0(TRACE_API, "JPEGDECMPC : In allocateBuffer ",pProxyComponent);
#ifdef __CACHE_OPTI
	VFM_Component   *vfm_component = (&(VFM_Component &)mENSComponent);
    OMX_S32 maxBuffers = vfm_component->getMaxBuffers(nPortIndex);
    if (maxBuffers>0) {
        if (((VFM_Port *)(vfm_component->getPort(nPortIndex)))->getParamPortDefinition()->nBufferCountActual
                > (OMX_U32)maxBuffers) {
            return OMX_ErrorBadParameter;
        }
    }

    error = allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo,OMX_TRUE);

#else
	error = VFM_NmfHost_ProcessingComponent::allocateBuffer(nPortIndex,nBufferIndex,nSizeBytes,ppData,
															bufferAllocInfo,portPrivateInfo);
#endif
    if (error != OMX_ErrorNone) 
	{
		OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : In allocateBuffer error!= none",pProxyComponent);
		return error;
	}
	///////CM_SetMode(CM_CMD_MPC_PRINT_VERBOSE, SVA_CORE_ID);

	if (NULL == bufferAllocInfo) 
	{
		OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : In allocateBuffer NULL == bufferAllocInfo error_undefined",pProxyComponent);
		return OMX_ErrorUndefined;
	}

    DBC_ASSERT(bufferAllocInfo != 0);
	//OMX_U32 bufPhysicalAddr = HwBuffer::GetBufferPhysicalAddress(*bufferAllocInfo, *ppData, nSizeBytes);
	*bufferAllocInfo = *portPrivateInfo;
	MMHwBuffer::TBufferInfo bufferInfo;
	((MMHwBuffer *)*bufferAllocInfo)->BufferInfo(nBufferIndex,bufferInfo);
	OMX_U32 bufPhysicalAddr = (OMX_U32)bufferInfo.iPhyAddr;
	//The implementation of this function can be changed in future.
	// Right now this returns 0 as address in sync with VFM_NmfMpc_ProcessingComponent class.
	//OMX_U32 bufMpcAddress = JPEGDec_getBufferMpcAddress(*bufferAllocInfo);

#ifdef __CACHE_OPTI
	if(!mpc_vfm_mem_ctxt)
	{
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		mpc_vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
	}
	VFM_AddMemoryExternalSource(mpc_vfm_mem_ctxt,(*ppData), (t_uint8 *)bufPhysicalAddr,nSizeBytes,(*portPrivateInfo));
#endif

	OMX_U32 bufMpcAddress =0;
    SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(),
            nSizeBytes, *ppData, bufPhysicalAddr, bufMpcAddress, *bufferAllocInfo, error);
    if (sharedBuf == 0) 
	{
		OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : sharedBuf == 0  ErrorInsufficientResources",pProxyComponent);
		return OMX_ErrorInsufficientResources;
	}
    if (error != OMX_ErrorNone) 
	{
		OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : error != OMX_ErrorNone Allocate buffer",pProxyComponent);
		return error;
	}

    *portPrivateInfo = sharedBuf;

	OstTraceFiltStatic1(TRACE_API, "JPEGDECMPC : In allocateBuffer size : %d \n",pProxyComponent,nSizeBytes);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::useBufferHeader(
                OMX_DIRTYPE dir,
                OMX_BUFFERHEADERTYPE* pBufferHdr)
{
	OstTraceFiltStatic0(TRACE_API, "JPEGDECMPC : In useBufferHeader \n",pProxyComponent);
	SharedBuffer *sharedBuf = 0;

    if(!pBufferHdr) 
	{
		OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : !pBufferHdr OMX_ErrorBadParameter",pProxyComponent);
		return OMX_ErrorBadParameter;
	}

    if(dir == OMX_DirInput) {
		OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC : dir == OMX_DirInput",pProxyComponent);
        sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pInputPortPrivate);
    } else {
		OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECMPC : dir == OMX_DirOutput",pProxyComponent);
        sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pOutputPortPrivate);
    }
    if(!sharedBuf) 
	{
		OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : !sharedBuf OMX_ErrorBadParameter",pProxyComponent);
		return OMX_ErrorBadParameter;
	}



    return sharedBuf->setOMXHeader(pBufferHdr);
}


OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::useBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_BUFFERHEADERTYPE* pBufferHdr,
                void **portPrivateInfo)
{

    OMX_ERRORTYPE error;
    void *bufferAllocInfo = 0;
    OMX_U8 *pBuffer;
	OstTraceFiltStatic0(TRACE_API, "JPEGDECMPC : In useBuffer \n",pProxyComponent);
    ENS_Port *port = mENSComponent.getPort(nPortIndex);
    bufferAllocInfo = port->getSharedChunk();
	pBuffer = pBufferHdr->pBuffer;

    MMHwBuffer::TBufferInfo bufferInfo;
	((MMHwBuffer *)bufferAllocInfo)->BufferInfo(nBufferIndex,bufferInfo);
	OMX_U32 bufPhysicalAddr = (OMX_U32)bufferInfo.iPhyAddr;
	OMX_U32 bufMpcAddress = 0;

#ifdef __CACHE_OPTI
	if(!mpc_vfm_mem_ctxt)
	{
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		mpc_vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
	}
	VFM_AddMemoryExternalSource(mpc_vfm_mem_ctxt,(pBuffer), (t_uint8 *)bufPhysicalAddr,(pBufferHdr->nAllocLen),bufferAllocInfo);

#endif
    SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(),
            pBufferHdr->nAllocLen, pBuffer, bufPhysicalAddr, bufMpcAddress, bufferAllocInfo, error);
    if (sharedBuf == 0) 
	{
		OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : In sharedBuf == 0  OMX_ErrorInsufficientResources\n",pProxyComponent);
		return OMX_ErrorInsufficientResources;
	}
    if (error != OMX_ErrorNone)
	{
		OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECMPC : error != OMX_ErrorNone USE_Buffer \n",pProxyComponent);
		return error;
	}

    sharedBuf->setOMXHeader(pBufferHdr);
    *portPrivateInfo = sharedBuf;



    return OMX_ErrorNone;
}



OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::freeBuffer(
				OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_BOOL bBufferAllocated,
                void *bufferAllocInfo,
                void *portPrivateInfo)
{
    OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : In freeBuffer \n");
    OMX_ERRORTYPE error;
    SharedBuffer *sharedBuf = static_cast<SharedBuffer *>(portPrivateInfo);
    portPrivateInfo = (void *)sharedBuf->getBufferAllocInfo(); // for VI 29465

    //portPrivateInfo = bufferAllocInfo;


#ifdef __CACHE_OPTI

	error = freeBufferVisual(nPortIndex,nBufferIndex,bBufferAllocated,bufferAllocInfo,portPrivateInfo);

#else
	error = VFM_NmfHost_ProcessingComponent::freeBuffer(nPortIndex,nBufferIndex,bBufferAllocated,
														bufferAllocInfo,portPrivateInfo);
#endif
	if (error != OMX_ErrorNone)
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In freeBuffer error != OMX_ErrorNone\n");
		return error;
	}

	delete sharedBuf;

	OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : In freeBuffer \n");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData){
	OstTraceFiltInst2(TRACE_API, "JPEGDECMPC sendCommand eCmd - %d , nData - %d \n",eCmd,nData);
	switch(eCmd) {
		case OMX_CommandStateSet:
			switch(nData)
			{
				case OMX_StateExecuting:
				case OMX_StatePause:
				case OMX_StateIdle:
							if((OMX_U32)OMX_StateExecuting==nData)
							{
								if (!fullDVFSSet)
								{
									VFM_PowerManagement::setFullDVFS(this);
									VFM_PowerManagement::setFullDDR(this);
									fullDVFSSet = OMX_TRUE;
								}
							}
							else
							{
								if (fullDVFSSet)
								{
									VFM_PowerManagement::releaseFullDVFS(this);
									VFM_PowerManagement::releaseFullDDR(this);
									fullDVFSSet = OMX_FALSE;
								}
							}

					break;
				default:
					break;
			}
			break;
		case OMX_CommandFlush:
		case OMX_CommandPortDisable:
			switch(nData) {
				case 0:
					pProxyComponent->mParam.initializeParamAndConfig();
					break;
				case 1:
					break;
			}
			break;
		default:
			break;
	}
	return NmfHost_ProcessingComponent::sendCommand(eCmd,nData);
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::configureAlgo(JPEGDec_Proxy * jpegdec){
	    return OMX_ErrorNone;
}

void JPEGDec_NmfMpc_ProcessingComponent::processingInit(){
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : In processingInit\n");
	memoryAllocated =0;
}

void JPEGDec_NmfMpc_ProcessingComponent::initiatePortSettingsChange(OMX_PARAM_PORTDEFINITIONTYPE *portDef){
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : In initiatePortSettingsChange\n");
	pProxyComponent->detectPortSettingsAndNotify(portDef);
}


OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::doSpecificEventHandle_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2){
	 return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::retrieveConfig(OMX_INDEXTYPE nConfigIndex,OMX_PTR pComponentConfigStructure){
	return OMX_ErrorNone;
}

void JPEGDec_NmfMpc_ProcessingComponent::registerStubsAndSkels(){
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : In registerStubsAndSkels\n");
	CM_REGISTER_STUBS_SKELS(video_jpegdec_cpp);
}

void JPEGDec_NmfMpc_ProcessingComponent::unregisterStubsAndSkels(){
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : In unregisterStubsAndSkels\n");
	CM_UNREGISTER_STUBS_SKELS(video_jpegdec_cpp);
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::codecStart(){
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::codecStop(){
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::codecInstantiate()
{
	t_nmf_error error;

	OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();
	VFM_Port *port = (VFM_Port *)mENSComponent.getPort(1);
	OMX_U32 fifo_size = port->getBufferCountActual();

	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : In codecInstantiate  \n");
    error = EnsWrapper_bindToUser(OMXHandle,mCodec,"iChangePortSettingsSignal",
						  (jpegdec_arm_nmf_api_portSettingsDescriptor*)this,4);
	if (error != NMF_OK) 
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecInstantiate error != NMF_OK OMX_ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

    RETURN_OMX_ERROR_IF_ERROR_OST(createPortInterface((VFM_Port *)mENSComponent.getPort(0),
												   "emptythisbuffer", "inputport",(fifo_size + 3)));
    RETURN_OMX_ERROR_IF_ERROR_OST(createPortInterface((VFM_Port *)mENSComponent.getPort(1),
												   "fillthisbuffer", "outputport",(fifo_size + 3)));

	error = mCodec->bindFromUser("iMemoryRequirements", 8, &(iProvideMemory));
	if (error != NMF_OK) 
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecInstantiate iMemoryRequirements OMX_ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

	error = mCodec->bindFromUser("iSendContextRequirements", 8, &(iSendContext));
	if (error != NMF_OK) 
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecInstantiate iSendContextRequirements OMX_ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}


	error = mCodec->bindFromUser("iSetConfig", 8, &(iSetConfig));
	if (error != NMF_OK)
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecInstantiate isetconfig OMX_ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

	error = mCodec->bindFromUser("icomputeARMLoad",8,&(icomputeARMLoad));
	if (error != NMF_OK) 
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecInstantiate icomputearmload OMX_ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

	if (pProxyComponent->isDualDecode)
	{
    	error = mCodec->getInterface("setParam",&IsetParamRef);
    	if (error != NMF_OK)
		{
			OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecInstantiate setparam OMX_ErrorUndefined \n");
			return OMX_ErrorUndefined ;
		}
	}



	return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::codecDeInstantiate()
{

	t_nmf_error error;
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : In codecDeInstantiate \n");
    RETURN_OMX_ERROR_IF_ERROR_OST(destroyPortInterface((VFM_Port *)mENSComponent.getPort(0),
												   "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR_OST(destroyPortInterface((VFM_Port *)mENSComponent.getPort(1),
											   "fillthisbuffer", "outputport"));
	error = EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(),mCodec, "iChangePortSettingsSignal");
	if (error != NMF_OK) 
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecDeInstantiate unbind changeportsettings OMX_ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

    error = mCodec->unbindFromUser("iSetConfig") ;
    if (error != NMF_OK) 
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecDeInstantiate unbind isetconfig OMX_ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

	error = mCodec->unbindFromUser("iMemoryRequirements") ;
	if (error != NMF_OK) 
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecDeInstantiate unbind memrequirements OMX_ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

    error = mCodec->unbindFromUser("icomputeARMLoad");
    if (error != NMF_OK) 
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecDeInstantiate unbind icomputearmload OMX_ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}
    IsetParamRef = 0;
    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::codecConfigure()
{
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : codecConfigure \n");
    icomputeARMLoad.ComputeARMLoad(pProxyComponent->mIsARMLoadComputed);
	//Calculate Depth of fifo
    iProvideMemory.memoryRequirements(mChannelId,mParamBufferDesc,mLinkListBufferDesc,mDebugBufferDesc,mLineBufferDesc);
    iSendContext.sendmemoryContext(mpc_vfm_mem_ctxt);
    //OstTraceInt1(TRACE_FLOW,"Value of mpc_vfm_mem_ctxt in ProcessingComp (0x%x)",(OMX_U32)mpc_vfm_mem_ctxt);
	iProvideMemory.huffMemory(mHuffMemoryDesc);
	return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::codecCreate(OMX_U32 domainId)
{
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : codecCreate \n");
	if (pProxyComponent->isDualDecode)
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : codecCreate isDualDecode\n");
		mCodec = jpegdec_arm_nmf_jpegdec_dualCreate();
	}
	else
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : codecCreate isnotDualDecode\n");
		mCodec = jpegdec_arm_nmf_jpegdeccomparmCreate();
	}

	if(mCodec==0)
	{
		jpegdec_nmfmpc_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECMPC : In CodecCreate mcode==0 OMX_ErrorUndefined \n");
		return OMX_ErrorUndefined;
	}

	if (pProxyComponent->isDualDecode)
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : codecCreate isDualDecode\n");
		((jpegdec_arm_nmf_jpegdec_dual *)mCodec)->domainSVACodec = static_cast<t_cm_domain_id>(domainId);
	}
	else
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : codecCreate isnotDualDecode\n");
		((jpegdec_arm_nmf_jpegdeccomparm *)mCodec)->domainSVACodec = static_cast<t_cm_domain_id>(domainId);
	}

	return OMX_ErrorNone;
}

void JPEGDec_NmfMpc_ProcessingComponent::codecDestroy(void){
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : codecDestroy \n");
	if (pProxyComponent->isDualDecode)
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : codecDestroy pProxyComponent->isDualDecode \n");
    	jpegdec_arm_nmf_jpegdec_dualDestroy((jpegdec_arm_nmf_jpegdec_dual *&)mCodec);
	}
	else
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : codecDestroy not pProxyComponent->isDualDecode \n");
         jpegdec_arm_nmf_jpegdeccomparmDestroy((jpegdec_arm_nmf_jpegdeccomparm *&)mCodec);
        }
}

void JPEGDec_NmfMpc_ProcessingComponent::sendConfigToCodec()
{
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : In sendConfigToCodec  \n");
	if(pProxyComponent->mParam.isCroppingEnabled && pProxyComponent->mParam.configCroppingChanged == OMX_TRUE){
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : In sendConfigToCodec configcroppingchanged  \n");
		ts_ddep_sdc_jpeg_dynamic_params config ;
        memset(&config, 0, sizeof(config));
		config.window_width = pProxyComponent->mParam.cropWindowWidth;
		config.window_height =pProxyComponent->mParam.cropWindowHeight;
		config.vertical_offset =pProxyComponent->mParam.vertical_offset ;
		config.horizontal_offset =pProxyComponent->mParam.horizontal_offset;
		iSetConfig.setConfig(OMX_IndexConfigCommonOutputCrop,config);
		pProxyComponent->mParam.configCroppingChanged = OMX_FALSE;
	}

	if (pProxyComponent->mParam.isDownsamplingEnabled && pProxyComponent->mParam.configScalingChanged == OMX_TRUE){
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : In sendConfigToCodec configscalingchanged  \n");
		ts_ddep_sdc_jpeg_dynamic_params config ;
        memset(&config, 0, sizeof(config));
		config.downsampling_factor =pProxyComponent->mParam.downsamplingFactor;
		iSetConfig.setConfig(OMX_IndexConfigCommonScale,config);
		pProxyComponent->mParam.configScalingChanged = OMX_FALSE;
	}

	if (pProxyComponent->isDualDecode)
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : In sendConfigToCodec isDualdecode  \n");
		OMX_IMAGE_PORTDEFINITIONTYPE* format1;
		OMX_IMAGE_PORTDEFINITIONTYPE* format2;
		format1 = ((VFM_Port*)(pProxyComponent->getPort(0)))->getImagePortDefinition();
		format2 = ((VFM_Port*)(pProxyComponent->getPort(1)))->getImagePortDefinition();
		IsetParamRef.setParam((void*)format1,(void*)format2);
	}


}

OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::applyConfig(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentConfigStructure)
{
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : In applyConfig  \n");
	ts_ddep_sdc_jpeg_dynamic_params config ;
   memset(&config, 0, sizeof(config));

	switch(nParamIndex){
		case OMX_IndexConfigCommonOutputCrop:
			OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : In applyConfig OMX_IndexConfigCommonOutputCrop \n");
			config.window_width = pProxyComponent->mParam.cropWindowWidth;
			config.window_height =pProxyComponent->mParam.cropWindowHeight;
			config.vertical_offset =pProxyComponent->mParam.vertical_offset ;
			config.horizontal_offset =pProxyComponent->mParam.horizontal_offset;
			if(pProxyComponent->mParam.configCroppingChanged)
				iSetConfig.setConfig(nParamIndex,config);
			pProxyComponent->mParam.configCroppingChanged = OMX_FALSE;
			break;
		case OMX_IndexConfigCommonScale:
			OstTraceFiltInst0(TRACE_FLOW, "JPEGDECMPC : In applyConfig OMX_IndexConfigCommonScale \n");
			config.downsampling_factor =pProxyComponent->mParam.downsamplingFactor;
			if(pProxyComponent->mParam.configScalingChanged)
				iSetConfig.setConfig(nParamIndex,config);
			pProxyComponent->mParam.configScalingChanged = OMX_FALSE;
			break;
		default:
			break;
	}

//	iSetConfig.setConfig(nParamIndex,config);


	return OMX_ErrorNone;
}

void JPEGDec_NmfMpc_ProcessingComponent::portSettings(void* portDef){
	OMX_PARAM_PORTDEFINITIONTYPE tmpPortDef = *((OMX_PARAM_PORTDEFINITIONTYPE*)portDef);

	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : In portSettings  \n");

	initiatePortSettingsChange(&tmpPortDef);
}


OMX_ERRORTYPE JPEGDec_NmfMpc_ProcessingComponent::errorRecoveryDestroyAll(){
	OstTraceFiltInst0(TRACE_API, "JPEGDECMPC : errorRecoveryDestroyAll \n");
	mCodec->stop();
	codecStop();
	return VFM_NmfHost_ProcessingComponent::deInstantiate();
}


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_mpeg2dec
#include "osi_trace.h"
#include "MPEG2Dec_Proxy.h"
#include "SharedBuffer.h"
#include "VFM_Memory.h"
#include "VFM_DDepUtility.h"
#include "stdio.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg2dec_proxy_src_MPEG2Dec_ProcessingCompTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE


#ifdef PACKET_VIDEO_SUPPORT
#define LOG_TAG "mpeg2dec"
#include<cutils/log.h>
#endif

#define RETURN_OMX_ERROR_IF_ERROR_OST(_x)    { OMX_ERRORTYPE _error; _error = (_x); if (_error!=OMX_ErrorNone) { mpeg2dec_processingcomp_assert(_error, __LINE__, OMX_FALSE); return _error; } }
#define RETURN_XXX_IF_WRONG_OST(_x, _error)  { if (!(_x)) { mpeg2dec_processingcomp_assert(_error, __LINE__, OMX_FALSE); return (_error); } }

#define DEBUG_MEM_SIZE (64*1024 - 1)
#define BUFFER_LINKLIST_SIZE 1024

volatile ts_ddep_buffer_descriptor g_mDebugBufferDescmpeg2Dec;
volatile ts_ddep_perf_param_out* g_pMPEG2DecOut_perf_parameters = 0;


void MPEG2Dec_ProcessingComp::mpeg2dec_processingcomp_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal){
    if (OMX_ErrorNone != omxError){
        OstTraceInt2(TRACE_ERROR, "MPEG2Dec_ProcessingComp : errorType : 0x%x error line no %d\n", omxError,line);
    if (isFatal){
            DBC_ASSERT(0==1);
        }
    }
}

MPEG2Dec_ProcessingComp::MPEG2Dec_ProcessingComp(ENS_Component &enscomp):
									VFM_NmfHost_ProcessingComponent(enscomp)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp_Constructor");
	pProxyComponent = (MPEG2Dec_Proxy *)&enscomp;
	firstFrame = OMX_TRUE;
	memory_once_allocated = OMX_FALSE;

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

	pParamBufferDesc=0;
	vfm_mem_ctxt =0;
	fullDVFSSet = OMX_FALSE;

	isErrorMapReportingEnable = OMX_TRUE;
	memset((void *)cumulativeErrorMap, 0, 225*sizeof(t_uint16));
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp_Constructor");
}

MPEG2Dec_ProcessingComp::~MPEG2Dec_ProcessingComp()
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp_Destructor");
	firstFrame = OMX_TRUE;
    memory_once_allocated = OMX_FALSE;

	if (vfm_mem_ctxt)
    {
           VFM_CloseMemoryList(vfm_mem_ctxt);
    }

	vfm_mem_ctxt = 0;

	mParamBufferDesc.nSize = 0;
	mParamBufferDesc.nLogicalAddress = 0;
	mParamBufferDesc.nPhysicalAddress = 0;

	//pProxyComponent->pFwPerfDataPtr=(OMX_PTR)0;
	mDebugBufferDesc.nSize = 0;
	mDebugBufferDesc.nLogicalAddress = 0;
	mDebugBufferDesc.nPhysicalAddress = 0;

	mLinkListBufferDesc.nSize = 0;
	mLinkListBufferDesc.nLogicalAddress = 0;
	mLinkListBufferDesc.nPhysicalAddress = 0;
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp_Destructor");
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::emptyThisBuffer");

	if(firstFrame == OMX_TRUE)  {
		VFM_Port *pt_port_out = (VFM_Port*)pProxyComponent->getPort(VPB+1);
		OMX_U32 width = pt_port_out->getFrameWidth();
		OMX_U32 height = pt_port_out->getFrameHeight();
		OMX_BOOL erc_flag = OMX_TRUE;

		iPortSettingsInfoSignal.sendportSettings(width,height,erc_flag);
		firstFrame = OMX_FALSE;
	}
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::emptyThisBuffer");
	return VFM_NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);
}


void MPEG2Dec_ProcessingComp::doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	VFM_NmfHost_ProcessingComponent::doSpecificFillBufferDone_cb(pBuffer);
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::fillThisBuffer");
	OstTraceInt0(TRACE_FLOW,"\nOutput Buffer recieved");
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::fillThisBuffer");
	return VFM_NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::construct(void)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::construct");
	if(!memory_once_allocated){
		OstTraceInt0(TRACE_FLOW,"MPEG2Dec_ProcessingComp::construct-Allocating Memrory.");
		RETURN_OMX_ERROR_IF_ERROR_OST(allocateRequiredMemory());
		memory_once_allocated=OMX_TRUE;
		OstTraceInt0(TRACE_FLOW,"MPEG2Dec_ProcessingComp::construct-Memory allocated.");
	}
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::construct");
	return NmfHost_ProcessingComponent::construct();
}
OMX_ERRORTYPE MPEG2Dec_ProcessingComp::destroy(void){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::destroy");
	NmfHost_ProcessingComponent::destroy();
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::destroy");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::allocateBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_U32 nSizeBytes,
                OMX_U8 **ppData,
                void **bufferAllocInfo,
                void **portPrivateInfo)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::allocateBuffer");
	OstTraceInt3(TRACE_FLOW,"nPortIndex:%d,nBufferIndex:%d,nSizeBytes:%d",nPortIndex,nBufferIndex,nSizeBytes);
	OMX_ERRORTYPE error;
#ifdef __CACHE_OPTI
		error = allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo,OMX_TRUE);
#else
		error = allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo,OMX_FALSE);
#endif
		if (error != OMX_ErrorNone) return error;

	if (error != OMX_ErrorNone){
		OstTraceInt1(TRACE_ERROR,"VFM_NmfHost_ProcessingComponent::allocateBuffer returned Error:%d",error);
		return error;
	}

    if(bufferAllocInfo == 0) return OMX_ErrorUndefined;
	//OMX_U32 bufPhysicalAddr = HwBuffer::GetBufferPhysicalAddress(*bufferAllocInfo, *ppData, nSizeBytes);
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
	OstTraceInt3(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::allocateBuffer with buffer Logical : 0x%x  Physical : 0x%x ,mpc : 0x%x",(OMX_U32)*ppData,(OMX_U32)bufPhysicalAddr,(OMX_U32)bufMpcAddress);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::useBufferHeader(
                OMX_DIRTYPE dir,
                OMX_BUFFERHEADERTYPE* pBufferHdr)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::useBufferHeader");
	SharedBuffer *sharedBuf = 0;

    if(!pBufferHdr) return OMX_ErrorBadParameter;

    if(dir == OMX_DirInput) {
        sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pInputPortPrivate);
    } else {
        sharedBuf = static_cast<SharedBuffer *>(pBufferHdr->pOutputPortPrivate);
    }
    if(!sharedBuf) return OMX_ErrorBadParameter;

	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::useBufferHeader");
    return sharedBuf->setOMXHeader(pBufferHdr);
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::useBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_BUFFERHEADERTYPE* pBufferHdr,
                void **portPrivateInfo)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::useBuffer");
	OstTraceInt2(TRACE_FLOW,"\nnPortIndex:%d,nBufferIndex:%d",nPortIndex,nBufferIndex);
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

	OstTraceInt3(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::useBuffer with buffer Logical : 0x%x  Physical : 0x%x ,mpc : 0x%x",(OMX_U32)pBuffer,(OMX_U32)bufPhysicalAddr,(OMX_U32)bufMpcAddress);
    return OMX_ErrorNone;

}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::freeBuffer(
				OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_BOOL bBufferAllocated,
                void *bufferAllocInfo,
                void *portPrivateInfo)
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::freeBuffer");
	OstTraceInt2(TRACE_FLOW,"nPortIndex:%d,nBufferIndex:%d",nPortIndex,nBufferIndex);
	OMX_ERRORTYPE error;
    SharedBuffer *sharedBuf = static_cast<SharedBuffer *>(portPrivateInfo);
	portPrivateInfo = (void *)sharedBuf->getBufferAllocInfo();
	error = VFM_NmfHost_ProcessingComponent::freeBuffer(nPortIndex,nBufferIndex,bBufferAllocated,
														bufferAllocInfo,portPrivateInfo);
	if (error != OMX_ErrorNone) return error;

	delete sharedBuf;

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG2Dec_ProcessingComp::freeBuffer()");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::sendCommand");
	OstTraceInt2(TRACE_FLOW,"Cmd:%d,nData:%d",eCmd,nData);
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::sendCommand");
	switch(eCmd)
	{
		case OMX_CommandStateSet:
			switch(nData)
			{
				case OMX_StateLoaded:
				case OMX_StateExecuting:
				case OMX_StatePause:
				case OMX_StateIdle:
					#ifdef __SYMBIAN32__
							if((OMX_U32)OMX_StateExecuting==nData)
							{
								if (!fullDVFSSet)
								{
									VFM_PowerManagement::setFullDVFS(this);
									VFM_PowerManagement::setLatency(this, VFM_PowerManagement::VFM_ResLatency_None);
									fullDVFSSet = OMX_TRUE;
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
							}
					#endif //__SYMBIAN32__

					break;
			}
			break;
		case OMX_CommandPortDisable:
		case OMX_CommandPortEnable:
		case OMX_CommandFlush:
			break;
		default:
			break;
	}

	return NmfHost_ProcessingComponent::sendCommand(eCmd,nData);
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::allocateRequiredMemory()
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::allocateRequiredMemory");
    t_uint32 size;

	int fakeBufferSize = (256) + VFM_ADDRESS_ALIGN_MASK;

    size = sizeof(ts_ddep_vdc_mpeg2_param_desc)
			+ COUNT_HEADER_BUFFERS* sizeof(ts_ddep_vdc_mpeg2_header_desc)
			+ fakeBufferSize  //check whether itis being passed to firmware
			+ VFM_IMAGE_BUFFER_ALIGN_MASK
			+ VFM_ADDRESS_ALIGN_MASK
			+ 5*0xf; //for alignment
	OstTraceInt1(TRACE_FLOW,"Required memory size:%d",size);
	{
		t_uint8* param_buf_logical_addr = 0;
		t_uint8* linklist_buf_logical_addr = 0;
		t_uint8* debug_buf_logical_addr = 0;

		if(!vfm_mem_ctxt){
			OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
			OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
			vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
		}

		param_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, size, VFM_MEM_CM_MPC_SDRAM16_ALLOC,
											CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/,
											__LINE__, (t_uint8 *)__FILE__);
		if(!param_buf_logical_addr){
			VFM_CloseMemoryList(vfm_mem_ctxt);
			vfm_mem_ctxt = 0;
			return OMX_ErrorInsufficientResources;
		}

		linklist_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, BUFFER_LINKLIST_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC,
											   CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/,
											   __LINE__, (t_uint8 *)__FILE__);
		if(!linklist_buf_logical_addr){
			VFM_CloseMemoryList(vfm_mem_ctxt);
			vfm_mem_ctxt = 0;
			return OMX_ErrorInsufficientResources;
		}
		debug_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, DEBUG_MEM_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC,
											CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/,
											__LINE__, (t_uint8 *)__FILE__);
		if(!debug_buf_logical_addr){
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

		g_mDebugBufferDescmpeg2Dec.nSize = mDebugBufferDesc.nSize;
		g_mDebugBufferDescmpeg2Dec.nLogicalAddress = mDebugBufferDesc.nLogicalAddress;
		g_mDebugBufferDescmpeg2Dec.nPhysicalAddress = mDebugBufferDesc.nPhysicalAddress;
		g_mDebugBufferDescmpeg2Dec.nMpcAddress = mDebugBufferDesc.nMpcAddress;
        memset((t_uint8*)g_mDebugBufferDescmpeg2Dec.nLogicalAddress,0x00,g_mDebugBufferDescmpeg2Dec.nSize);
	}

    {
		//Zero initialize all param memory
		t_uint32 count;
		ts_ddep_vdc_mpeg2_param_desc *ps_ddep_vdc_mpeg2dec_param_desc;
		ps_ddep_vdc_mpeg2dec_param_desc = (ts_ddep_vdc_mpeg2_param_desc *)mParamBufferDesc.nLogicalAddress;
		for (count = 0; count<sizeof(ts_ddep_vdc_mpeg2_param_desc);count++)
			((t_uint8*)ps_ddep_vdc_mpeg2dec_param_desc)[count]=0;
    }
  //CM_SetMode(CM_CMD_MPC_PRINT_VERBOSE, SVA_CORE_ID);
  OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::allocateRequiredMemory");
  return OMX_ErrorNone;

}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::doSpecificEventHandle_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::doSpecificEventHandle_cb");
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::doSpecificEventHandle_cb");
	return OMX_ErrorNone;
}

void MPEG2Dec_ProcessingComp::registerStubsAndSkels(){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::registerStubsAndSkels");
	CM_REGISTER_STUBS_SKELS(video_mpeg2dec_cpp);
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::registerStubsAndSkels");
}

void MPEG2Dec_ProcessingComp::unregisterStubsAndSkels(){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::unregisterStubsAndSkels");
	CM_UNREGISTER_STUBS_SKELS(video_mpeg2dec_cpp);
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::unregisterStubsAndSkels");
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::codecStart(){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::codecStart");
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::codecStart");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::codecStop(){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::codecStop");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::codecInstantiate(){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::codecInstantiate");
	OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();
    EnsWrapper_bindToUser(OMXHandle,mCodec,"iChangePortSettingsSignal",
						  (mpeg2dec_arm_nmf_api_portSettingsDescriptor*)this,4);

    RETURN_OMX_ERROR_IF_ERROR_OST(createPortInterface((VFM_Port *)mENSComponent.getPort(0),
												   "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR_OST(createPortInterface((VFM_Port *)mENSComponent.getPort(1),
												   "fillthisbuffer", "outputport"));

	mCodec->bindFromUser("iSetConfig", 8, &(iSetConfig));
	mCodec->bindFromUser("portSettingsInfoSignal", 8, &(iPortSettingsInfoSignal));
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::codecInstantiate");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::codecDeInstantiate(){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::codecDeInstantiate");
    RETURN_OMX_ERROR_IF_ERROR_OST(destroyPortInterface((VFM_Port *)mENSComponent.getPort(0),
												   "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR_OST(destroyPortInterface((VFM_Port *)mENSComponent.getPort(1),
												   "fillthisbuffer", "outputport"));
	EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(),mCodec, "iChangePortSettingsSignal");
    mCodec->unbindFromUser("portSettingsInfoSignal") ;
	mCodec->unbindFromUser("iMemoryRequirements") ;
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::codecDeInstantiate");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::codecConfigure()
{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::codecConfigure");
	ENS_Port *port = mENSComponent.getPort(1);
	iSetConfig.computeARMLoad(pProxyComponent->mIsARMLoadComputed);
	iSetConfig.memoryRequirements(0,mParamBufferDesc,mLinkListBufferDesc,mDebugBufferDesc,
									   port->getBufferCountActual(),isErrorMapReportingEnable,cumulativeErrorMap);
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::codecConfigure");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::codecCreate(OMX_U32 domainId){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::codecCreate");
	mCodec = mpeg2dec_arm_nmf_mpeg2deccomparmCreate();

	if(mCodec==0)
		return OMX_ErrorUndefined;

	((mpeg2dec_arm_nmf_mpeg2deccomparm *)mCodec)->domainSVACodec = static_cast<t_cm_domain_id>(domainId);

	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::codecCreate");
	return OMX_ErrorNone;
}

void MPEG2Dec_ProcessingComp::codecDestroy(void){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::codecDestroy");
    mpeg2dec_arm_nmf_mpeg2deccomparmDestroy((mpeg2dec_arm_nmf_mpeg2deccomparm *&)mCodec);
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::codecDestroy");}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::applyConfig(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentConfigStructure){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::applyConfig");
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::applyConfig");
	return OMX_ErrorNone;
}

void MPEG2Dec_ProcessingComp::sendConfigToCodec(){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::sendConfigToCodec");
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::sendConfigToCodec");
}

void MPEG2Dec_ProcessingComp::portSettings(t_uint32 width,t_uint32 height){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::portSettings");
	pProxyComponent->updateOutputPortSettings(width,height);
	pProxyComponent->mParam.CropWidth = 0;
	pProxyComponent->mParam.CropHeight = 0;
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::portSettings");
}

void MPEG2Dec_ProcessingComp::set_perf_parameters(){
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::set_perf_parameters");
	/*
	t_uint8 *ptr;
	ts_ddep_vdc_mpeg2_param_desc *ps_ddep_vdc_mpeg2_param_desc;
	ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
	ps_ddep_vdc_mpeg2_param_desc = (ts_ddep_vdc_mpeg2_param_desc *) ptr;
    //pProxyComponent->pFwPerfDataPtr =(OMX_PTR)(&(ps_ddep_vdc_mpeg2_param_desc->s_out_perf_parameters));*/
	OstTraceInt0(TRACE_FLOW,"Exit MPEG2Dec_ProcessingComp::set_perf_parameters");
}

void MPEG2Dec_ProcessingComp::GetIndex_macroblock_error_map(OMX_PTR pt) const{
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::GetIndex_macroblock_error_map");
	OMX_CONFIG_MACROBLOCKERRORMAPTYPE *pt_video = (OMX_CONFIG_MACROBLOCKERRORMAPTYPE*)pt;
	OMX_U32 *ptr = (OMX_U32*)pt_video->ErrMap;
	memcpy((void *)ptr[0],(void *)cumulativeErrorMap,225*sizeof(t_uint16));
	memset((void *)cumulativeErrorMap, 0, 225*sizeof(t_uint16));
	OstTraceInt0(TRACE_FLOW,"Enter MPEG2Dec_ProcessingComp::GetIndex_macroblock_error_map");
}

OMX_ERRORTYPE MPEG2Dec_ProcessingComp::errorRecoveryDestroyAll()
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG2Dec_ProcessingComp::errorRecoveryDestroyAll()");

	mCodec->stop();
	codecStop();

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG2Dec_ProcessingComp::errorRecoveryDestroyAll()");

	return VFM_NmfHost_ProcessingComponent::deInstantiate();
}


OMX_BOOL MPEG2Dec_ProcessingComp::isNMFPanicSource(t_panic_source ee_type, OMX_U32 faultingComponent)
{
	OstTraceFiltInst2(TRACE_FLOW, "=> MPEG2Dec_ProcessingComp::isNMFPanicSource() ee_type = 0x%x, faultingComponent = 0x%x", ee_type, faultingComponent);

	switch(ee_type)
	{
		case MPC_EE :
			if(mCodec->getMPCComponentHandle("cMpeg2decDdep")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cGenericFsm")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cComponentFsm")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cOSTTrace")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cSharedBufInMPC")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cSharedBufOutMPC")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cResource_manager")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cVpp")==faultingComponent ||
			   mCodec->getMPCComponentHandle("cMpeg2decAlgo")==faultingComponent
			   )
			{
                if(mCodec->getMPCComponentHandle("cMpeg2decDdep")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cMpeg2decDdep");
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
                if(mCodec->getMPCComponentHandle("cMpeg2decAlgo")==faultingComponent){
                     OstTraceInt0(TRACE_FLOW, "Panic::cMpeg2decAlgo");
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

	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG2Dec_ProcessingComp::isNMFPanicSource()");
	return OMX_FALSE;
}


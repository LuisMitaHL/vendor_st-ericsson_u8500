/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ JPEGDec_Proxy
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegdec_proxy_src_JPEGDec_ArmNmf_ProcessingComponentTraces.h"
#endif


#include "JPEGDec_Proxy.h"
#include "JPEGDec_ArmNmf_ProcessingComponent.h"

#define RETURN_OMX_ERROR_IF_ERROR_OST(_x)    { OMX_ERRORTYPE _error; _error = (_x); if (_error!=OMX_ErrorNone) { jpegdec_nmfarm_assert(_error, __LINE__, OMX_FALSE); return _error; } }


#define MAX_WIDTH 8176

#include <stdio.h>

void JPEGDec_ArmNmf_ProcessingComponent::jpegdec_nmfarm_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
	OstTraceFiltStatic0(TRACE_API, "JPEGDECARM : jpegdec_nmfarm_assert \n",pProxyComponent);
    if (OMX_ErrorNone != omxError)
    {
        OstTraceFiltStatic2(TRACE_ERROR, "JPEGDECARM : errorType : 0x%x error line no %d\n",pProxyComponent, omxError,line);
        NMF_LOG("JPEGDECARM : errorType : 0x%x error line %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}


JPEGDec_ArmNmf_ProcessingComponent::JPEGDec_ArmNmf_ProcessingComponent(ENS_Component &enscomp):VFM_NmfHost_ProcessingComponent(enscomp)
{
	pProxyComponent = (JPEGDec_Proxy *)&enscomp;
	mParamBufferDesc.nSize = 0;
	mParamBufferDesc.nMpcAddress = 0;
	mParamBufferDesc.nLogicalAddress = 0;
	mParamBufferDesc.nPhysicalAddress = 0;

	mLineBufferDesc.nSize = 0;
	mLineBufferDesc.nMpcAddress = 0;
	mLineBufferDesc.nLogicalAddress = 0;
	mLineBufferDesc.nPhysicalAddress = 0;
	arm_vfm_mem_ctxt = 0;

	mHuffMemoryDesc.nLogicalAddress = 0;
	mHuffMemoryDesc.nMpcAddress = 0;
	mHuffMemoryDesc.nPhysicalAddress = 0;
	mHuffMemoryDesc.nSize = 0;

	isMemoryAllocated = OMX_FALSE;
}


OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::codecInstantiate()
{
	t_nmf_error error;
	OstTraceFiltInst0(TRACE_API, "JPEGDECARM: Inside codeInstantiate \n");
	OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();
    error = EnsWrapper_bindToUser(OMXHandle,mCodec,"changePortSettingsSignal",(jpegdec_arm_nmf_api_portSettingsDescriptor*)this,4);
    if (error != NMF_OK)
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECARM: Inside codeInstantiate bind to user ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}


    RETURN_OMX_ERROR_IF_ERROR_OST(createPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR_OST(createPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport"));

	//set referenc to st Paramter
	error = mCodec->getInterface("setParam",&IsetParamRef);
	if (error != NMF_OK)
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECARM: Inside codeInstantiate get interface setparam ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

	//for setConfig interface
	error = mCodec->getInterface("setConfig",&IsetConfigRef);
	if (error != NMF_OK)
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECARM: Inside codeInstantiate get interface setConfig ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

	error = mCodec->getInterface("iMemoryRequirements",&iProvideMemory);
	if (error != NMF_OK)
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECARM: Inside codeInstantiate get interface memrequirements ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

	OstTraceFiltInst0(TRACE_API, "JPEGDECARM : codecInstantiate DONE \n");

    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::codecStart()
{
    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::codecStop()
{
    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::allocateRequiredMemory()
{

    t_uint32 size,fakeBufferSize;
	OstTraceFiltStatic0(TRACE_API, "JPEGDECARM : allocateRequiredMemory \n",pProxyComponent);

    fakeBufferSize = (512) + VFM_ADDRESS_ALIGN_MASK;

	size = sizeof(ts_ddep_sdc_jpeg_param_desc)+ VFM_ADDRESS_ALIGN_MASK
           + sizeof(ts_ddep_sdc_jpeg_dynamic_configuration)+ VFM_ADDRESS_ALIGN_MASK
		   + 4*(sizeof(ts_ddep_sdc_jpeg_scan_desc)+ VFM_ADDRESS_ALIGN_MASK)
           + fakeBufferSize
           + 6*0xf ;//for alignment
	{
		OMX_U32 lineBufferSize;
		OMX_U8 *param_buf_logical_addr;
		OMX_U8 *line_buf_logical_addr;
		OMX_U8 *huff_mem_logical_addr;

		param_buf_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, size, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

		if(!param_buf_logical_addr)
		{
			jpegdec_nmfarm_assert(OMX_ErrorInsufficientResources,__LINE__, OMX_TRUE);
			VFM_Free(arm_vfm_mem_ctxt, param_buf_logical_addr);
			arm_vfm_mem_ctxt = 0;
			OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECARM : (!param_buf_logical_addr) hence return OMX_ErrorInsufficientResources\n",pProxyComponent);
			return OMX_ErrorInsufficientResources;
		}
		mParamBufferDesc.nSize = size;
		mParamBufferDesc.nLogicalAddress = (t_uint32)param_buf_logical_addr;
		mParamBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(arm_vfm_mem_ctxt, param_buf_logical_addr));
		mParamBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(arm_vfm_mem_ctxt, param_buf_logical_addr));
		OstTraceFiltStatic1(TRACE_FLOW, "JPEGDECARM : Allocated memory for mParamBufferDesc size :%d \n",pProxyComponent,mParamBufferDesc.nSize);

		/* memory allocation for Line buffer */
		lineBufferSize = MAX_WIDTH/2;
		line_buf_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, lineBufferSize, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

		if(!line_buf_logical_addr)
		{
			jpegdec_nmfarm_assert(OMX_ErrorInsufficientResources,__LINE__, OMX_TRUE);
			VFM_Free(arm_vfm_mem_ctxt, line_buf_logical_addr);
			arm_vfm_mem_ctxt = 0;
			OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECARM : (!line_buf_logical_addr) hence return OMX_ErrorInsufficientResources\n",pProxyComponent);
			return OMX_ErrorInsufficientResources;
		}
		mLineBufferDesc.nSize = lineBufferSize;
		mLineBufferDesc.nLogicalAddress = (t_uint32)line_buf_logical_addr;
		mLineBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(arm_vfm_mem_ctxt, line_buf_logical_addr));
		mLineBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(arm_vfm_mem_ctxt, line_buf_logical_addr));
		OstTraceFiltStatic1(TRACE_FLOW, "JPEGDECARM : Allocated memory for mLineBufferDesc size :%d \n",pProxyComponent,mLineBufferDesc.nSize);

//		mLineBufferDesc.nSize = lineBufferSize;
//		mLineBufferDesc.nLogicalAddress = sysAddr.logical;
//		mLineBufferDesc.nPhysicalAddress = sysAddr.physical;

        //memoryAllocated =1;
		huff_mem_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, sizeof(ts_t1xhv_vdc_sw_table), VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

		if(!huff_mem_logical_addr)
		{
			jpegdec_nmfarm_assert(OMX_ErrorInsufficientResources,__LINE__, OMX_TRUE);
			VFM_Free(arm_vfm_mem_ctxt, huff_mem_logical_addr);
			arm_vfm_mem_ctxt = 0;
			OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECARM : (!huff_mem_logical_addr) hence return OMX_ErrorInsufficientResources\n",pProxyComponent);
			return OMX_ErrorInsufficientResources;
		}
		mHuffMemoryDesc.nSize = sizeof(ts_t1xhv_vdc_sw_table);
		mHuffMemoryDesc.nLogicalAddress = (t_uint32)huff_mem_logical_addr;
		mHuffMemoryDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(arm_vfm_mem_ctxt, huff_mem_logical_addr));
		mHuffMemoryDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(arm_vfm_mem_ctxt, huff_mem_logical_addr));
		OstTraceFiltStatic1(TRACE_FLOW, "JPEGDECARM : Allocated memory for mHuffMemoryDesc size :%d \n",pProxyComponent,mHuffMemoryDesc.nSize);
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



OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::codecConfigure()
{
    /* Restore the normal debug mode, i.e. DBG_MODE_NORMAL */
	OstTraceFiltInst0(TRACE_API, "JPEGDECARM : Inside codecConfigure\n");
	if(!arm_vfm_mem_ctxt)
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECARM : Inside codecConfigure !arm_vfm_mem_ctxt\n");
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		JPEGDec_Proxy   *proxy_component = (JPEGDec_Proxy *)(&mENSComponent);
		OMX_U32 ddrDomainId = proxy_component->getAllocDdrMemoryDomain();
		arm_vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
	}

    if(!isMemoryAllocated) //if FALSE
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECARM : Inside codecConfigure !isMemoryAllocated\n");
	   	RETURN_OMX_ERROR_IF_ERROR_OST(allocateRequiredMemory());
	   	isMemoryAllocated = OMX_TRUE;
	}

	iProvideMemory.memoryRequirements(0,mParamBufferDesc,mParamBufferDesc,mParamBufferDesc,mLineBufferDesc);
	iProvideMemory.huffMemory(mHuffMemoryDesc);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::codecDeInstantiate()
{
	t_nmf_error error;
	OstTraceFiltInst0(TRACE_API, "JPEGDECARM : inside codecDeInstantiate\n");
    RETURN_OMX_ERROR_IF_ERROR_OST(destroyPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR_OST(destroyPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport"));

	error = EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(),mCodec, "changePortSettingsSignal");
	if (error != NMF_OK)
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECARM: EnsWrapper_unbindToUser changeportsettings ErrorUndefined \n");
		return OMX_ErrorUndefined ;
	}

	IsetParamRef = 0;
	IsetConfigRef = 0;
	iProvideMemory = 0;


    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::allocateBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_U32 nSizeBytes,
                OMX_U8 **ppData,
                void **bufferAllocInfo,
                void **portPrivateInfo)
{
	OMX_ERRORTYPE error;
	OstTraceFiltStatic1(TRACE_API, "JPEGDECARM : allocateBuffer size : %d \n",pProxyComponent,nSizeBytes);
	error = allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo,OMX_TRUE);
    if (error != OMX_ErrorNone)
	{
		OstTraceFiltStatic0(TRACE_ERROR, "JPEGDECARM: ALLOCATEBUFFER error \n",pProxyComponent);
		return error;
	}

    return OMX_ErrorNone;
}


void JPEGDec_ArmNmf_ProcessingComponent::registerStubsAndSkels()
{

}

void JPEGDec_ArmNmf_ProcessingComponent::unregisterStubsAndSkels()
{
}

OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::codecCreate(OMX_U32 domainId)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	OstTraceFiltInst0(TRACE_API, "JPEGDECARM Inside codecCreate \n");
	//mCodec = jpegdec_swCreate();
	mCodec = jpegdec_arm_nmf_jpegdec_swCreate();
	if(!mCodec)
	{
		OstTraceFiltInst0(TRACE_ERROR, "JPEGDECARM: codecCreate ERRORUNDEFINEFED \n");
		error = OMX_ErrorUndefined;
	}

	return error;
}

void JPEGDec_ArmNmf_ProcessingComponent::codecDestroy(void)
{
    //jpegdec_swDestroy((jpegdec_sw*&)mCodec);
    jpegdec_arm_nmf_jpegdec_swDestroy((jpegdec_arm_nmf_jpegdec_sw*&)mCodec);
}

void JPEGDec_ArmNmf_ProcessingComponent::portSettings(void* portDef)
{

	//printf("\n portSettings from parser !");
	OMX_PARAM_PORTDEFINITIONTYPE tmpPortDef = *((OMX_PARAM_PORTDEFINITIONTYPE*)portDef);
	//printf("\nChange Port Settings to be done on processing componnt side.");
	JPEGDec_Proxy   *proxy_component = (JPEGDec_Proxy *)(&mENSComponent);
	//JPEGDec_ProcessingComp *pProcComp = (JPEGDec_ProcessingComp *)this;
	OstTraceFiltInst0(TRACE_API, "JPEGDECARM Inside portSettings \n");
	//printf("Calling detectPortSettingsAndNotify for Port : %d \n",tmpPortDef.nPortIndex);
    proxy_component->detectPortSettingsAndNotify(&tmpPortDef);

}


OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData)
{
	JPEGDec_Proxy   *proxy_component = (JPEGDec_Proxy *)(&mENSComponent);
	OstTraceFiltStatic2(TRACE_API, "JPEGDECARM : Inside send command eCmd - %d ,nData - %d\n",pProxyComponent,eCmd,nData);
	switch(eCmd)
	{
		case OMX_CommandFlush:
		case OMX_CommandPortDisable:
			switch(nData)
			{
				case 0:
					proxy_component->mParam.initializeParamAndConfig();
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


JPEGDec_ArmNmf_ProcessingComponent::~JPEGDec_ArmNmf_ProcessingComponent()
{
	OstTraceFiltStatic0(TRACE_API, "JPEGDECARM : Inside ~JPEGDec_ArmNmf_ProcessingComponent\n",pProxyComponent);
	if (isMemoryAllocated)
	{
		OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECARM : Inside isMemoryAllocated\n",pProxyComponent);
		if(mParamBufferDesc.nLogicalAddress)
		{
			OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECARM : Inside param buffer deallocated\n",pProxyComponent);
			VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mParamBufferDesc.nLogicalAddress);
			mParamBufferDesc.nSize = 0;
			mParamBufferDesc.nLogicalAddress = 0;
			mParamBufferDesc.nPhysicalAddress = 0;
		}
		if(mHuffMemoryDesc.nLogicalAddress)
		{
			OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECARM : Inside huff buffer deallocated\n",pProxyComponent);
			VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mHuffMemoryDesc.nLogicalAddress);
			mHuffMemoryDesc.nSize = 0;
			mHuffMemoryDesc.nLogicalAddress = 0;
			mHuffMemoryDesc.nPhysicalAddress = 0;
		}
		if(mLineBufferDesc.nLogicalAddress)
		{
			OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECARM : Inside line buffer deallocated\n",pProxyComponent);
			VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mLineBufferDesc.nLogicalAddress);
			mLineBufferDesc.nSize = 0;
			mLineBufferDesc.nLogicalAddress = 0;
			mLineBufferDesc.nPhysicalAddress = 0;
		}

		if(arm_vfm_mem_ctxt)
		{
			OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECARM : Inside arm_vfm_mem deallocated\n",pProxyComponent);
			VFM_CloseMemoryList(arm_vfm_mem_ctxt);
		}

		arm_vfm_mem_ctxt = 0;


	}
	isMemoryAllocated = OMX_FALSE;

}


void JPEGDec_ArmNmf_ProcessingComponent::sendConfigToCodec()
{
	OstTraceFiltInst0(TRACE_API, "JPEGDECARM : sendConfigToCodec \n");
	//printf("\n JPEGDec_ArmNmfProcessingComp::sendConfigToCodec");
	//send parameter structure to parser here !
	JPEGDec_Proxy   *proxy_component = (JPEGDec_Proxy *)(&mENSComponent);
	OMX_IMAGE_PORTDEFINITIONTYPE* format1;
	OMX_IMAGE_PORTDEFINITIONTYPE* format2;
	format1 = ((VFM_Port*)(proxy_component->getPort(0)))->getImagePortDefinition();
	format2 = ((VFM_Port*)(proxy_component->getPort(1)))->getImagePortDefinition();
	// (OMX_IndexParamPortDefinition,(OMX_PTR)&tmpPortDef)
	IsetParamRef.setParam((void*)format1,(void*)format2);

	if(proxy_component->mParam.isCroppingEnabled && proxy_component->mParam.configCroppingChanged == OMX_TRUE)
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECARM : sendConfigToCodec croppingenabled and changed\n");
		ts_ddep_sdc_jpeg_dynamic_params config;
        memset(&config, 0, sizeof(config));
		config.window_width = proxy_component->mParam.cropWindowWidth;
		config.window_height =proxy_component->mParam.cropWindowHeight;
		config.vertical_offset =proxy_component->mParam.vertical_offset ;
		config.horizontal_offset =proxy_component->mParam.horizontal_offset;
		IsetConfigRef.setConfig(OMX_IndexConfigCommonOutputCrop,config);
		proxy_component->mParam.configCroppingChanged = OMX_FALSE;
	}

	if (proxy_component->mParam.isDownsamplingEnabled && proxy_component->mParam.configScalingChanged == OMX_TRUE)
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGDECARM : sendConfigToCodec scaling enabled and config changed\n");
		ts_ddep_sdc_jpeg_dynamic_params config ;
        memset(&config, 0, sizeof(config));
		config.downsampling_factor =proxy_component->mParam.downsamplingFactor;
		IsetConfigRef.setConfig(OMX_IndexConfigCommonScale,config);
		proxy_component->mParam.configScalingChanged = OMX_FALSE;
	}


}


OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::applyConfig(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentConfigStructure)
{
	OstTraceFiltInst0(TRACE_API, "JPEGDECARM : applyConfig Done\n");
	ts_ddep_sdc_jpeg_dynamic_params config ;
    memset(&config, 0, sizeof(config));
	JPEGDec_Proxy   *pProxyComponent = (JPEGDec_Proxy *)(&mENSComponent);

	switch(nParamIndex){
		case OMX_IndexConfigCommonOutputCrop:
			OstTraceFiltInst0(TRACE_FLOW, "JPEGDECARM : applyConfig OMX_IndexConfigCommonOutputCrop\n");
			config.window_width = pProxyComponent->mParam.cropWindowWidth;
			config.window_height =pProxyComponent->mParam.cropWindowHeight;
			config.vertical_offset =pProxyComponent->mParam.vertical_offset ;
			config.horizontal_offset =pProxyComponent->mParam.horizontal_offset;
			if(pProxyComponent->mParam.configCroppingChanged)
				IsetConfigRef.setConfig(nParamIndex,config);
			pProxyComponent->mParam.configCroppingChanged = OMX_FALSE;
			break;
		case OMX_IndexConfigCommonScale:
			OstTraceFiltInst0(TRACE_FLOW, "JPEGDECARM : applyConfig OMX_IndexConfigCommonScale\n");
			config.downsampling_factor =pProxyComponent->mParam.downsamplingFactor;
			if(pProxyComponent->mParam.configScalingChanged)
				IsetConfigRef.setConfig(nParamIndex,config);
			pProxyComponent->mParam.configScalingChanged = OMX_FALSE;
			break;
		default:
			break;
	}


	return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::useBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_BUFFERHEADERTYPE* pBufferHdr,
                void **portPrivateInfo)
{
	OstTraceFiltStatic0(TRACE_API, "JPEGDECARM : useBuffer Done\n",pProxyComponent);
	ENS_Port *port = mENSComponent.getPort(nPortIndex);
    MMHwBuffer *sharedChunk = port->getSharedChunk();
    if (sharedChunk) {
		OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECARM : useBuffer sharedchunk\n",pProxyComponent);
        sharedChunk->AddBufferInfo(nBufferIndex, (OMX_U32)pBufferHdr->pBuffer, pBufferHdr->nAllocLen);
        /*if (error != OMX_ErrorNone) {
            return OMX_ErrorNone;
        }*/
        *portPrivateInfo = sharedChunk;
    } else {
		OstTraceFiltStatic0(TRACE_FLOW, "JPEGDECARM : useBuffer not in sharedchunk\n",pProxyComponent);
        *portPrivateInfo = 0;
    }



    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGDec_ArmNmf_ProcessingComponent::errorRecoveryDestroyAll(){
	OstTraceFiltInst0(TRACE_API, "JPEGDECARM : errorRecoveryDestroyAll\n");
	mCodec->stop();
	codecStop();
	return VFM_NmfHost_ProcessingComponent::deInstantiate();
}


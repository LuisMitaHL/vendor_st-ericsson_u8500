/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#define _CNAME_ JPEGEnc_Proxy
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegenc_proxy_src_JPEGEnc_ArmNmfProcessingComponentTraces.h"
	#endif //for OST_TRACE_COMPILER_IN_USE


#include "JPEGEnc_Proxy.h"
#include "JPEGEnc_ArmNmfProcessingComponent.h"

#include <stdio.h>
#include "VFM_Port.h"
#include "host/eventhandler.hpp"

#ifdef PACKET_VIDEO_SUPPORT
#ifndef LOG_TAG
#define LOG_TAG "jpegenc"
#endif
#include<cutils/log.h>
#endif

/// @ingroup JPEGEncoder
/// @brief Max size of the JPEG header

#ifdef HOST_MPC_COMPONENT_BOTH
extern OMX_U16 _nHeaderSize; // Coming from JPEGEnc_ParamAndConfig.cpp
extern OMX_U32 _nEXIFHeaderSize;
extern OMX_U8  _nJFIFappSegmentSize;
#else
OMX_U16 _nHeaderSize = ((OMX_U16) (0x1318/8) + 1);
OMX_U32 _nEXIFHeaderSize = ((OMX_U32) (64*1024) + ((OMX_U32) (0x1318/8) + 1)); //64KB
OMX_U8  _nJFIFappSegmentSize = ((OMX_U8)18);
#endif

#define EXIF_HEADER 64*1024
/// @ingroup JPEGEncoder
/// @brief If we want to optimize the computation of the header and quantization
/// table or not (if already computed)
int _statusOptimize=1; // Coming from JPEGEnc_ParamAndConfig.cpp

// some constants dedicated to huffman for the length and code
extern t_uint16 _nHuffmanLength;
extern t_uint16 _nHuffmanCode;


void JPEGEnc_ArmNmfProcessingComponent::jpegenc_armnmf_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
    if (OMX_ErrorNone != omxError)
    {
        OstTraceInt2(TRACE_ERROR, "JPEGENC_ARM_NMF : errorType : 0x%x error line no %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}


JPEGEnc_ArmNmfProcessingComponent::JPEGEnc_ArmNmfProcessingComponent(ENS_Component &enscomp): VFM_NmfHost_ProcessingComponent(enscomp)
{
		OstTraceInt1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In Constructor <line no %d> ",__LINE__);
	    pProxyComponent =0;
		nNumber_arm = 0;
        pProxyComponent = (JPEGEnc_Proxy *)0;
        mHeaderBufferDesc.nSize = 0;
	    mHeaderBufferDesc.nMpcAddress = 0;
	    mHeaderBufferDesc.nLogicalAddress = 0;
	    mHeaderBufferDesc.nPhysicalAddress = 0;
        mEXIFHeaderBufferDesc.nSize = 0;
	    mEXIFHeaderBufferDesc.nMpcAddress = 0;
	    mEXIFHeaderBufferDesc.nLogicalAddress = 0;
	    mEXIFHeaderBufferDesc.nPhysicalAddress = 0;

        mParamBufferDesc.nSize = 0;
	    mParamBufferDesc.nMpcAddress = 0;
	    mParamBufferDesc.nLogicalAddress = 0;
	    mParamBufferDesc.nPhysicalAddress = 0;

	    arm_vfm_mem_ctxt = 0;
		pProxyComponent = (JPEGEnc_Proxy *)(&mENSComponent);
		OMXHandle = (void *)0;
		//+ER 354962
		mNbPendingCommands = 0;
		//-ER 354962
#ifdef HVA_JPEGENC
		mHVATaskDecs.nSize = 0;
		mHVATaskDecs.nMpcAddress = 0;
		mHVATaskDecs.nLogicalAddress = 0;
		mHVATaskDecs.nPhysicalAddress = 0;

		mHVAInParamsDecs.nSize = 0;
		mHVAInParamsDecs.nMpcAddress = 0;
		mHVAInParamsDecs.nLogicalAddress = 0;
		mHVAInParamsDecs.nPhysicalAddress = 0;

		mHVAOutParamsDecs.nSize = 0;
		mHVAOutParamsDecs.nMpcAddress = 0;
		mHVAOutParamsDecs.nLogicalAddress = 0;
		mHVAOutParamsDecs.nPhysicalAddress = 0;

		mHVAQuantParamsDecs.nSize = 0;
		mHVAQuantParamsDecs.nMpcAddress = 0;
		mHVAQuantParamsDecs.nLogicalAddress = 0;
		mHVAQuantParamsDecs.nPhysicalAddress = 0;
#endif

}


void JPEGEnc_ArmNmfProcessingComponent::doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : in doSpecificEmptyBufferDone_cb pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
}



OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{

	OstTraceFiltInst3(TRACE_API, "JPEGEnc_ArmNmfProcessingComponent : In emptyThisBuffer pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);

	if ((pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingJPEG)&&(pBuffer->nFlags & OMX_BUFFERFLAG_EXTRADATA))
	{
		pBuffer->nFlags = pBuffer->nFlags & (~OMX_BUFFERFLAG_EXTRADATA);
	}

	if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingEXIF)
	{
		t_cm_system_address sysAddr1,sysAddr2;
		sysAddr1.logical = (t_uint32 )mHeaderBufferDesc.nLogicalAddress;
		DBC_ASSERT(sysAddr1.logical);

		sysAddr2.logical = (t_uint32 )mEXIFHeaderBufferDesc.nLogicalAddress;
		DBC_ASSERT(sysAddr2.logical);
		OstTraceFiltInst1(TRACE_FLOW, "In JPEGEnc_ArmNmfProcessingComponent : In emptyThisBuffer calling addEXIFHeader <line no %d> ",__LINE__);
		processingComp.addEXIFHeader(pProxyComponent,pBuffer,(OMX_U8 **)&(sysAddr1.logical),(OMX_U8 **)&(sysAddr2.logical),
		                             &(mEXIFHeaderBufferDesc.nSize),mHeaderBufferDesc.nSize);
	}

	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : emptyThisBuffer done <line no %d> ",__LINE__);
	return NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);
}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "JPEGEnc_ArmNmfProcessingComponent : In fillThisBuffer pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
	return NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);
}


void JPEGEnc_ArmNmfProcessingComponent::doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "JPEGEnc_ArmNmfProcessingComponent : In doSpecificFillBufferDone_cb pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
}

JPEGEnc_ArmNmfProcessingComponent::~JPEGEnc_ArmNmfProcessingComponent()
{
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In destructor  <line no %d> ",pProxyComponent,__LINE__);
	if (mParamBufferDesc.nSize)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Freeing memory for mParamBufferDesc <line no %d> ",__LINE__);
		VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mParamBufferDesc.nLogicalAddress);
		mParamBufferDesc.nSize = 0;
		mParamBufferDesc.nLogicalAddress = 0;
		mParamBufferDesc.nPhysicalAddress = 0;
	}

	if(mHeaderBufferDesc.nSize)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Freeing memory for mHeaderBufferDesc <line no %d> ",__LINE__);
		VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mHeaderBufferDesc.nLogicalAddress);
		mHeaderBufferDesc.nSize = 0;
		mHeaderBufferDesc.nLogicalAddress = 0;
		mHeaderBufferDesc.nPhysicalAddress = 0;
	}

	 if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingEXIF)
	 {
		if(mEXIFHeaderBufferDesc.nSize)
		{
			OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Freeing memory for mEXIFHeaderBufferDesc <line no %d> ",__LINE__);
			VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mEXIFHeaderBufferDesc.nLogicalAddress);
			mEXIFHeaderBufferDesc.nSize = 0;
			mEXIFHeaderBufferDesc.nLogicalAddress = 0;
			mEXIFHeaderBufferDesc.nPhysicalAddress = 0;
		}
	 }

#ifdef HVA_JPEGENC

	 if (pProxyComponent->isHVABased == OMX_TRUE)
	 {
		 if (mHVATaskDecs.nSize)
		 {
			OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Freeing memory for mHVATaskDecs <line no %d> ",__LINE__);
			VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mHVATaskDecs.nLogicalAddress);
			mHVATaskDecs.nSize = 0;
			mHVATaskDecs.nLogicalAddress = 0;
			mHVATaskDecs.nPhysicalAddress = 0;
		}

		if (mHVAInParamsDecs.nSize)
		 {
			OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Freeing memory for mHVAInParamsDecs <line no %d> ",__LINE__);
			VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mHVAInParamsDecs.nLogicalAddress);
			mHVAInParamsDecs.nSize = 0;
			mHVAInParamsDecs.nLogicalAddress = 0;
			mHVAInParamsDecs.nPhysicalAddress = 0;
		}

		if (mHVAOutParamsDecs.nSize)
		 {
			OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Freeing memory for mHVAOutParamsDecs <line no %d> ",__LINE__);
			VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mHVAOutParamsDecs.nLogicalAddress);
			mHVAOutParamsDecs.nSize = 0;
			mHVAOutParamsDecs.nLogicalAddress = 0;
			mHVAOutParamsDecs.nPhysicalAddress = 0;
		}

		if (mHVAQuantParamsDecs.nSize)
		 {
			OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Freeing memory for mHVAQuantParamsDecs <line no %d> ",__LINE__);
			VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mHVAQuantParamsDecs.nLogicalAddress);
			mHVAQuantParamsDecs.nSize = 0;
			mHVAQuantParamsDecs.nLogicalAddress = 0;
			mHVAQuantParamsDecs.nPhysicalAddress = 0;
		}
	}
#endif

	if(arm_vfm_mem_ctxt)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Closing Memory List  <line no %d> ",__LINE__);
		VFM_CloseMemoryList(arm_vfm_mem_ctxt);
	}

	arm_vfm_mem_ctxt = 0;
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : destruction done  <line no %d> ",pProxyComponent,__LINE__);
}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::allocateBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_U32 nSizeBytes,
                OMX_U8 **ppData,
                void **bufferAllocInfo,
                void **portPrivateInfo)
{
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In allocateBuffer   <line no %d> ",pProxyComponent,__LINE__);
	OMX_ERRORTYPE error;
        if (pProxyComponent->isHVABased)
        {
	        error = allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo,OMX_FALSE);
        }
        else
        {
	        error = allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo,OMX_TRUE);
        }
	if (error != OMX_ErrorNone)
	{
		OstTraceInt2(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error While allocating buffer errorType : 0x%x error line no %d\n",error,__LINE__);
		//jpegenc_armnmf_assert(error,__LINE__, OMX_TRUE);
		return error;
	}
	OstTraceFiltStatic3(TRACE_FLOW, "In JPEGEnc_ArmNmfProcessingComponent : In allocateBuffer nPortIndex : %d nSizeBytes : %d   <line no %d> ",pProxyComponent,nPortIndex,nSizeBytes,__LINE__);
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : allocateBuffer DONE  <line no %d> ",pProxyComponent,__LINE__);
    return OMX_ErrorNone;
}

void JPEGEnc_ArmNmfProcessingComponent::portSettings(OMX_U32 size)
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In portSettings <line no %d> ",__LINE__);
	JPEGEnc_Proxy   *proxy_component = (JPEGEnc_Proxy *)(&mENSComponent);
    proxy_component->detectPortSettingsAndNotify(size);
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : portSettings DONE <line no %d> ",__LINE__);
}

OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::codecInstantiate()
{
	//OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();
    t_nmf_error error ;
    OMXHandle = mENSComponent.getOMXHandle();

	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In codecInstantiate line no %d> \n",__LINE__);

    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport"));

	error = mCodec->bindFromUser("setParam",8,&setparamitf) ;
	if (error != NMF_OK)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error while binding setParam \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined ;
	}
	error = EnsWrapper_bindToUser(OMXHandle,mCodec,"iChangePortSettingsSignal",(jpegenc_arm_nmf_api_portSettingsDescriptor*)this,4);
	if (error != NMF_OK)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error while binding iChangePortSettingsSignal \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined ;
	}
	//+ER 354962
	error = EnsWrapper_bindToUser(OMXHandle,mCodec,"iCommandAck",(jpegenc_arm_nmf_api_cmd_ackDescriptor*)this,4);
	if (error != NMF_OK)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error while binding iCommandAck \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined ;
	}
	//-ER 354962
#ifdef HVA_JPEGENC
	if (pProxyComponent->isHVABased == OMX_TRUE)
	{
		error = mCodec->bindFromUser("setMemoryParam",8,&setMemoryitf) ;
		if (error != NMF_OK)
		{
			OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error while binding setMemoryParam \n");
			jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
			return OMX_ErrorUndefined;
		}
	}
#endif
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : codecInstantiate DONE line no %d> \n",__LINE__);
    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler)
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : doSpecificEventHandler_cb DONE <Event Type %d> ",event);
	if(event == OMX_EventCmdComplete && nData1 == OMX_CommandPortDisable)
	{
		/* ER 447646 */
		pProxyComponent->mSendParamToARMNMF.set();
		pProxyComponent->mParam.rotation_val = 0;
		/* ER 447646 */
	}
	deferEventHandler = OMX_FALSE;
	return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
	OstTraceFiltInst2(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : doSpecificSendCommand Command : %d Data : %d ",eCmd,nData);
	
	/* ER 447646 */
	if(eCmd == OMX_CommandPortEnable)
	{
		pProxyComponent->mSendParamToARMNMF.set();
		pProxyComponent->mParam.HeaderToGenerate.set();
		pProxyComponent->mParam.ConfigApplicable.set();
		/* SK: != Outport port(1). Reconfiguration should match settings at input port. Same as MPC side */
		if(nData == 0 || nData == OMX_ALL)
			configure();
	}
	if(eCmd == OMX_CommandStateSet && (nData == (OMX_U32)OMX_StateIdle))
	{
		pProxyComponent->mParam.rotation_val = 0;
	}
	/* ER 447646 */
	
	bDeferredCmd = OMX_FALSE;
	return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::codecStart()
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : codecStart DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::codecStop()
{
	//+ER 354962
	if(mNbPendingCommands!=0)
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_FALSE);
	//-ER 354962
	
	/* ER 447646 */
	pProxyComponent->mParam.rotation_val = 0;
	/* ER 447646 */
	
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : codecStop DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::configure()
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In configure <line no %d> ",__LINE__);
	OMX_ERRORTYPE error;

	if(!arm_vfm_mem_ctxt)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Creating Memory List <line no %d> ",__LINE__);
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		arm_vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
	}

    if(pProxyComponent->mSendParamToARMNMF.get())
		{
			//Allocating memory for Header
			if(!mHeaderBufferDesc.nSize)
			{
				OMX_U8 *header_buf_logical_addr;
				OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : configure() Allocating Memory to mHeaderBufferDesc <line no %d> ",__LINE__);
				if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingJPEG)
				{
					header_buf_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, (_nHeaderSize+_nJFIFappSegmentSize), VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Allocating Memory of Size : 0x%x to mHeaderBufferDesc <line no %d> ",_nHeaderSize+_nJFIFappSegmentSize,__LINE__);
				}
				else
				{
					header_buf_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, _nHeaderSize, VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Allocating Memory of Size : 0x%x to mHeaderBufferDesc <line no %d> ",_nHeaderSize,__LINE__);
				}

				if(!header_buf_logical_addr)
				{
					VFM_Free(arm_vfm_mem_ctxt, header_buf_logical_addr);
					arm_vfm_mem_ctxt = 0;
					OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error allocating memory to mHeaderBufferDesc \n");
					jpegenc_armnmf_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
					return OMX_ErrorInsufficientResources;
				}

				if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingJPEG)
				{
					mHeaderBufferDesc.nSize = (_nHeaderSize+_nJFIFappSegmentSize);
				}
				else
				{
					mHeaderBufferDesc.nSize = _nHeaderSize;
				}

				mHeaderBufferDesc.nLogicalAddress = (t_uint32)header_buf_logical_addr;
				mHeaderBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(arm_vfm_mem_ctxt, header_buf_logical_addr));
				mHeaderBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(arm_vfm_mem_ctxt, header_buf_logical_addr));
			}

			processingComp.compute_quantization_table(pProxyComponent);    // compute the quantization table
			JPEGheaderCreation(pProxyComponent);
			error = configureAlgo();
			if (OMX_ErrorNone != error)
			{
				OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent :  Error returned from configureAlgo \n");
				jpegenc_armnmf_assert(error, __LINE__, OMX_TRUE);
				return error;
			}
			//setparamitf.setParameter(ID_SEC_JPEG,mParamBufferDesc);
			pProxyComponent->mSendParamToARMNMF.reset();
		}
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : configure DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::codecConfigure()
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : codecConfigure DONE <line no %d> ",__LINE__);
    /* Restore the normal debug mode, i.e. DBG_MODE_NORMAL */
    IN0("");
    //iProvideHeaderBuffers.bufferRequirements(*pParamBufferDesc);
    OUT0("");
    return OMX_ErrorNone;
}



OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::codecDeInstantiate()
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In codecDeInstantiate line no %d> \n",__LINE__);
	t_nmf_error error;

    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport"));
	error = mCodec->unbindFromUser("setParam");
	if (error != NMF_OK)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error while un binding setParam \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined ;
	}

	//EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(),mCodec, "iChangePortSettingsSignal");
	error = EnsWrapper_unbindToUser(OMXHandle,mCodec, "iChangePortSettingsSignal");
	if (error != NMF_OK)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error while un binding iChangePortSettingsSignal \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined ;
	}
	//+ER 354962
	error = EnsWrapper_unbindToUser(OMXHandle,mCodec, "iCommandAck");
	if (error != NMF_OK)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error while un binding iCommandAck \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined ;
	}
	//-ER 354962
#ifdef HVA_JPEGENC
	if (pProxyComponent->isHVABased == OMX_TRUE)
	{
		error = mCodec->unbindFromUser("setMemoryParam");
		if (error != NMF_OK)
		{
			OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error while un binding setMemoryParam \n");
			jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
			return OMX_ErrorUndefined ;
		}
	}
#endif

	// for setting the parameters again incase another YUV is sent for encoding with same parameters
	pProxyComponent->mSendParamToARMNMF.set();
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : codecDeInstantiate DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}


void JPEGEnc_ArmNmfProcessingComponent::registerStubsAndSkels()
{

}

void JPEGEnc_ArmNmfProcessingComponent::unregisterStubsAndSkels()
{
}

OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::codecCreate(OMX_U32 domainId)
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In codecCreate  <line no %d> ",__LINE__);
	OMX_ERRORTYPE error = OMX_ErrorNone;

#ifdef HVA_JPEGENC
	if (pProxyComponent->isHVABased == OMX_TRUE)
	{
		mCodec = jpegenc_arm_nmf_hva_jpegencCreate();
	}
	else
#endif
	{
		mCodec = jpegenc_arm_nmf_jpegenc_swCreate();
	}

	if(!mCodec)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error in codecCreate \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		error = OMX_ErrorUndefined;
	}
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : codecCreate DONE <line no %d> ",__LINE__);
	return error;
}

void JPEGEnc_ArmNmfProcessingComponent::codecDestroy(void)
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In codecDestroy <line no %d> ",__LINE__);
#ifdef HVA_JPEGENC
	if (pProxyComponent->isHVABased == OMX_TRUE)
	{
		jpegenc_arm_nmf_hva_jpegencDestroy((jpegenc_arm_nmf_hva_jpegenc *&)mCodec);
	}
	else
#endif
	{
         jpegenc_arm_nmf_jpegenc_swDestroy((jpegenc_arm_nmf_jpegenc_sw *&)mCodec);
	}
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : codecDestroy DONE <line no %d> ",__LINE__);

}


void JPEGEnc_ArmNmfProcessingComponent::set_pJecOther_parameters(JPEGEnc_Proxy *jpegenc, ts_ddep_sec_jpeg_param_desc_ *ps_ddep_sec_jpeg_param_desc, t_uint32 header_size_in_bytes, t_uint8 *headerBufferAddress, t_uint8* thumbnailImageBufferAddress, t_uint8* runLevelBufferAddress)
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In set_pJecOther_parameters  <line no %d> ",__LINE__);
	t_cm_system_address sysAddr;
	sysAddr.logical = (t_uint32 )(t_uint32 )mHeaderBufferDesc.nLogicalAddress;;

	DBC_ASSERT(sysAddr.logical);

	ps_ddep_sec_jpeg_param_desc->s_header_buf.addr_header_buffer = (t_ahb_address)headerBufferAddress;
	nNumber_arm = pProxyComponent->mParam.nNumber_param;

	if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingJPEG)
	{
	    memcpy((t_uint8 *)&ps_ddep_sec_jpeg_param_desc->header_buffer, (t_uint8 *)sysAddr.logical, header_size_in_bytes);
		ps_ddep_sec_jpeg_param_desc->s_header_buf.header_size = header_size_in_bytes;
	}

	if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingEXIF)
	{
			ps_ddep_sec_jpeg_param_desc->s_header_buf.header_size = EXIF_HEADER + mHeaderBufferDesc.nSize; //size in bytes!
	}

	OstTraceFiltInst2(TRACE_FLOW, "In JPEGEnc_ArmNmfProcessingComponent : In set_pJecOther_parameters header Size : 0x%x <line no %d> ",ps_ddep_sec_jpeg_param_desc->s_header_buf.header_size,__LINE__);
	//ps_ddep_sec_jpeg_param_desc->s_header_buf.header_size = header_size_in_bytes;// Puneet *8; //size in bits!
	ps_ddep_sec_jpeg_param_desc->s_out_fram_buffer.addr_dest_buffer = (t_ahb_address)thumbnailImageBufferAddress;
	ps_ddep_sec_jpeg_param_desc->s_internal_buffer.addr_jpeg_run_level_buffer = (t_ahb_address)runLevelBufferAddress;
	ps_ddep_sec_jpeg_param_desc->s_in_out_frame_parameters.restart_mcu_count = jpegenc->mParam.getRestartInterval();
	ps_ddep_sec_jpeg_param_desc->s_ddep_in_param.nSliceHeight = (OMX_U16)jpegenc->getSliceHeight();
	ps_ddep_sec_jpeg_param_desc->s_ddep_in_param.nStride = (OMX_U16)jpegenc->getStride();
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : set_pJecOther_parameters DONE <line no %d> ",__LINE__);
}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::applyConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure)
{
			   OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In applyConfig <line no %d> ",__LINE__);
               if(pProxyComponent->mParam.ConfigApplicable.get())
			   {
				   OstTraceFiltInst1(TRACE_FLOW, "In JPEGEnc_ArmNmfProcessingComponent : In applyConfig Calling configureAlgo <line no %d> ",__LINE__);
				   OMX_ERRORTYPE error;
				   processingComp.compute_quantization_table(pProxyComponent);
				   JPEGheaderCreation(pProxyComponent);
				   error = configureAlgo();
				   if (OMX_ErrorNone != error)
				   {
					   OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error in applyConfig\n");
					   jpegenc_armnmf_assert(error, __LINE__, OMX_TRUE);
					   return error;
				   }
			   }
			   pProxyComponent->mParam.ConfigApplicable.reset();
			   OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : applyConfig DONE <line no %d> ",__LINE__);
               return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::configureAlgo()
{
	OMX_BOOL isNewParam = OMX_FALSE;
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In configureAlgo <line no %d> ",__LINE__);
    t_uint8 *ptr;
	t_bool zero_param_desc_dh=0;
    t_uint32 size;
    t_uint16 width, height;
    t_uint32 header_size_in_bytes;

    // misc. temporary buffers
    t_uint8 * thumbnailImageBufferAddress;//,*thumbnailImageBufferPhysicalAddress;
    t_uint8 * runLevelBufferAddress;//,*runLevelBufferPhysicalAddress;
    t_uint8 * headerBufferAddress;//,*headerBufferPhysicalAddress;
	t_uint32 headerBufferOffset;

	ts_ddep_sec_jpeg_param_desc_dh *ps_ddep_sec_jpeg_param_desc_dh=(ts_ddep_sec_jpeg_param_desc_dh *)0;
    headerBufferOffset = (t_uint32)&ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.header_buffer;

    header_size_in_bytes = mHeaderBufferDesc.nSize; //Puneet Gulati mHeaderBufferDesc.nSize;

    width = pProxyComponent->getFrameWidth(1);
    height = pProxyComponent->getFrameHeight(1);

    int thumbnailsize = ((((width/8 +15)/16)*16) * (((height/8 +15)/16)*16) * 2) + VFM_IMAGE_BUFFER_ALIGN_MASK;
    int runlevelsize = (width * 32 * 4 * 3) + VFM_IMAGE_BUFFER_ALIGN_MASK;

/*
	size = sizeof(ts_ddep_sec_jpeg_param_desc_dh)
			+ thumbnailsize
			+ runlevelsize
			+ VFM_IMAGE_BUFFER_ALIGN_MASK                  // header of JPEG
			+ linkBitstreamBufferSize
			+ VFM_ADDRESS_ALIGN_MASK
			+ 5*0xf; //for alignment
*/
	size = sizeof(ts_ddep_sec_jpeg_param_desc_dh)
			+ thumbnailsize
			+ runlevelsize
			+ VFM_IMAGE_BUFFER_ALIGN_MASK                  // header of JPEG
			+ VFM_ADDRESS_ALIGN_MASK
			+ 5*0xf; //for alignment


	// header buffer size not expected to cross 1024 bytes for current feature set
	DBC_ASSERT(header_size_in_bytes<sizeof(ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.header_buffer));
    {
#ifdef HVA_JPEGENC
		if (pProxyComponent->isHVABased == OMX_TRUE)
		{
			OMX_U8 *buf_logical_addr;
			OMX_U32 buff_size;
			buff_size = sizeof(ts_hva_task_descriptor);
			OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Allocating Memory of Size : 0x%x to mHVATaskDecs <line no %d> ",buff_size,__LINE__);
			buf_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, buff_size, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			if(!buf_logical_addr)
			{
				OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error allocating memory to mHVATaskDecs \n");
				VFM_Free(arm_vfm_mem_ctxt, buf_logical_addr);
				jpegenc_armnmf_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
				arm_vfm_mem_ctxt = 0;
				return OMX_ErrorInsufficientResources;
			}
			mHVATaskDecs.nSize = buff_size;
			mHVATaskDecs.nLogicalAddress = (t_uint32)buf_logical_addr;
			mHVATaskDecs.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(arm_vfm_mem_ctxt, buf_logical_addr));
			mHVATaskDecs.nMpcAddress = (t_uint32)(VFM_GetMpc(arm_vfm_mem_ctxt, buf_logical_addr));

			OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Allocating Memory of Size : 0x%x to mHVAInParamsDecs <line no %d> ",buff_size,__LINE__);
			buff_size = sizeof(ts_t1xhv_hva_input_parameters);
			buf_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, buff_size, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			if(!buf_logical_addr)
			{
				OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error allocating memory to mHVAInParamsDecs \n");
				VFM_Free(arm_vfm_mem_ctxt, buf_logical_addr);
				jpegenc_armnmf_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
				arm_vfm_mem_ctxt = 0;
				return OMX_ErrorInsufficientResources;
			}
			mHVAInParamsDecs.nSize = buff_size;
			mHVAInParamsDecs.nLogicalAddress = (t_uint32)buf_logical_addr;
			mHVAInParamsDecs.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(arm_vfm_mem_ctxt, buf_logical_addr));
			mHVAInParamsDecs.nMpcAddress = (t_uint32)(VFM_GetMpc(arm_vfm_mem_ctxt, buf_logical_addr));

			OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Allocating Memory of Size : 0x%x to mHVAOutParamsDecs <line no %d> ",buff_size,__LINE__);
			buff_size = sizeof(ts_t1xhv_hva_output_parameters);
			buf_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, buff_size, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			if(!buf_logical_addr)
			{
				VFM_Free(arm_vfm_mem_ctxt, buf_logical_addr);
				arm_vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error allocating memory to mHVAOutParamsDecs \n");
				jpegenc_armnmf_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
				return OMX_ErrorInsufficientResources;
			}
			mHVAOutParamsDecs.nSize = buff_size;
			mHVAOutParamsDecs.nLogicalAddress = (t_uint32)buf_logical_addr;
			mHVAOutParamsDecs.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(arm_vfm_mem_ctxt, buf_logical_addr));
			mHVAOutParamsDecs.nMpcAddress = (t_uint32)(VFM_GetMpc(arm_vfm_mem_ctxt, buf_logical_addr));

			OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Allocating Memory of Size : 0x%x to mHVAQuantParamsDecs <line no %d> ",buff_size,__LINE__);
			buff_size = sizeof(ts_t1xhv_hva_quant_parameters);
			buf_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, buff_size, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			if(!buf_logical_addr)
			{
				VFM_Free(arm_vfm_mem_ctxt, buf_logical_addr);
				arm_vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error allocating memory to mHVAQuantParamsDecs \n");
				jpegenc_armnmf_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
				return OMX_ErrorInsufficientResources;
			}
			mHVAQuantParamsDecs.nSize = buff_size;
			mHVAQuantParamsDecs.nLogicalAddress = (t_uint32)buf_logical_addr;
			mHVAQuantParamsDecs.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(arm_vfm_mem_ctxt, buf_logical_addr));
			mHVAQuantParamsDecs.nMpcAddress = (t_uint32)(VFM_GetMpc(arm_vfm_mem_ctxt, buf_logical_addr));

			/* copying the quang values */
			memcpy((OMX_U16*)mHVAQuantParamsDecs.nLogicalAddress, (OMX_U16*)pProxyComponent->mParam.QuantTable.QuantizationLuma,64);
			OMX_U16 *tempBuf = (OMX_U16*)mHVAQuantParamsDecs.nLogicalAddress;
			tempBuf = tempBuf+64;
			memcpy((OMX_U16*)tempBuf, (OMX_U16*)pProxyComponent->mParam.QuantTable.QuantizationChroma,64);
			tempBuf = tempBuf+64;
			memset((OMX_U16*)tempBuf, (OMX_U16)0,64);

			setMemoryitf.setMemoryParamHVAWrapper(mHVATaskDecs,mHVAInParamsDecs,
			                                      mHVAOutParamsDecs,mHVAQuantParamsDecs);
		}
#endif

		if (mParamBufferDesc.nSize && pProxyComponent->mSendParamToARMNMF.get())
		{
			VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mParamBufferDesc.nLogicalAddress);
			mParamBufferDesc.nSize = 0;
			mParamBufferDesc.nLogicalAddress = 0;
			mParamBufferDesc.nPhysicalAddress = 0;
		}

		if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingEXIF)
		{
			if (mEXIFHeaderBufferDesc.nSize && pProxyComponent->mSendParamToARMNMF.get())
			{
				VFM_Free(arm_vfm_mem_ctxt, (t_uint8*)mEXIFHeaderBufferDesc.nLogicalAddress);
				mEXIFHeaderBufferDesc.nSize = 0;
				mEXIFHeaderBufferDesc.nLogicalAddress = 0;
				mEXIFHeaderBufferDesc.nPhysicalAddress = 0;
			}
		}

		if((pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingEXIF) && (!mEXIFHeaderBufferDesc.nSize))
		{
			OMX_U8 *exif_buf_logical_addr;
			exif_buf_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, _nEXIFHeaderSize, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Allocating Memory of Size : 0x%x to mEXIFHeaderBufferDesc <line no %d> ",_nEXIFHeaderSize,__LINE__);
			if(!exif_buf_logical_addr)
			{
				VFM_Free(arm_vfm_mem_ctxt, exif_buf_logical_addr);
				arm_vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error allocating memory to mEXIFHeaderBufferDesc \n");
				jpegenc_armnmf_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
				return OMX_ErrorInsufficientResources;
			}
			mEXIFHeaderBufferDesc.nSize = _nEXIFHeaderSize;
			mEXIFHeaderBufferDesc.nLogicalAddress = (t_uint32)exif_buf_logical_addr;
			mEXIFHeaderBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(arm_vfm_mem_ctxt, exif_buf_logical_addr));
			mEXIFHeaderBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(arm_vfm_mem_ctxt, exif_buf_logical_addr));
		}


		if (!mParamBufferDesc.nSize)
		{
			isNewParam = OMX_TRUE;
			OMX_U8 *exif_buf_logical_addr;
			exif_buf_logical_addr  = VFM_Alloc(arm_vfm_mem_ctxt, size, VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_16BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Allocating Memory of Size : 0x%x to mParamBufferDesc <line no %d> ",size,__LINE__);
			if(!exif_buf_logical_addr)
			{
				VFM_Free(arm_vfm_mem_ctxt, exif_buf_logical_addr);
				arm_vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error allocating memory to mParamBufferDesc \n");
				jpegenc_armnmf_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
				return OMX_ErrorInsufficientResources;
			}
			mParamBufferDesc.nSize = size;
			mParamBufferDesc.nLogicalAddress = (t_uint32)exif_buf_logical_addr;
			mParamBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(arm_vfm_mem_ctxt, exif_buf_logical_addr));
			mParamBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(arm_vfm_mem_ctxt, exif_buf_logical_addr));

			zero_param_desc_dh = 1;
		}

		ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
        //physicalptr = (t_uint8*)mParamBufferDesc.nPhysicalAddress;
	}

	ps_ddep_sec_jpeg_param_desc_dh = (ts_ddep_sec_jpeg_param_desc_dh *) ptr;

	headerBufferAddress = ptr + headerBufferOffset;
	//headerBufferPhysicalAddress = physicalptr + headerBufferOffset;

	if(((t_uint32)headerBufferAddress)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error in configureAlgo \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
	//if(((t_uint32)headerBufferPhysicalAddress)&0xf){return OMX_ErrorUndefined;}

	ptr+=sizeof(ts_ddep_sec_jpeg_param_desc_dh);
	//physicalptr+=sizeof(ts_ddep_sec_jpeg_param_desc_dh);

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
    //physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error in configureAlgo \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
    thumbnailImageBufferAddress = ptr;
    ptr = ptr + thumbnailsize;
	//thumbnailImageBufferPhysicalAddress = physicalptr;                            physicalptr = physicalptr + thumbnailsize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
    //physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error in configureAlgo \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
    runLevelBufferAddress = ptr;
    ptr = ptr + runlevelsize;
    //runLevelBufferPhysicalAddress = physicalptr;                                physicalptr = physicalptr + runlevelsize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
    //physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error in configureAlgo \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
    headerBufferAddress = ptr;
    ptr = ptr + header_size_in_bytes + VFM_IMAGE_BUFFER_ALIGN_MASK;
    //headerBufferPhysicalAddress = physicalptr;                               physicalptr = physicalptr + header_size_in_bytes + VFM_IMAGE_BUFFER_ALIGN_MASK;

//	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
//    physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

//	if(((t_uint32)ptr)&0xf){return OMX_ErrorUndefined;}
    //linkBitstreamBufferAddress = ptr;                                          ptr = ptr + linkBitstreamBufferSize+VFM_ADDRESS_ALIGN_MASK;
	//linkBitstreamBufferAddress = linkBitstreamBufferAddress;
    //linkBitstreamBufferPhysicalAddress = physicalptr;                          physicalptr = physicalptr + linkBitstreamBufferSize+VFM_ADDRESS_ALIGN_MASK;
	//linkBitstreamBufferPhysicalAddress = linkBitstreamBufferPhysicalAddress;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
    //physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_ArmNmfProcessingComponent : Error in configureAlgo \n");
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}

    thumbnailImageBufferAddress = (t_uint8 *)VFM_ROUND_UPPER(thumbnailImageBufferAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
    //thumbnailImageBufferPhysicalAddress = (t_uint8 *)VFM_ROUND_UPPER(thumbnailImageBufferPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	runLevelBufferAddress = (t_uint8 *)VFM_ROUND_UPPER(runLevelBufferAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
	//runLevelBufferPhysicalAddress = (t_uint8 *)VFM_ROUND_UPPER(runLevelBufferPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	t_uint16 atomic_write;

	if (zero_param_desc_dh)
	{
		t_uint32 count;
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_ArmNmfProcessingComponent : Inside configureAlgo::if (zero_param_desc_dh) <line no %d> ",__LINE__);
		for (count = 0; count<sizeof(ts_ddep_sec_jpeg_param_desc_dh);count++)
			((t_uint8*)ps_ddep_sec_jpeg_param_desc_dh)[count]=0;

		// Set JPEG Parameter In: MPC PARAM SET (only once)
		processingComp.set_pJecIn_parameters(pProxyComponent, &ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.s_in_parameters);
		// Set JPEG Parameter Other and header
		set_pJecOther_parameters(pProxyComponent, &ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set, header_size_in_bytes, headerBufferAddress, thumbnailImageBufferAddress, runLevelBufferAddress);
		if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingEXIF)
		{
				processingComp.setValue((OMX_U32)&ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.header_buffer);//headerMPCPosition = (OMX_U32)&ps_ddep_sec_jpeg_param_desc->header_buffer;
				processingComp.setHeaderSizeValue((OMX_U32)&ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.s_header_buf.header_size);// = _nEXIFHeaderSize;
		}


		atomic_write = 0;
	}
	else atomic_write = ps_ddep_sec_jpeg_param_desc_dh->host_param_set_atomic_entry + 1;

    ps_ddep_sec_jpeg_param_desc_dh->host_param_set_atomic_entry = atomic_write;
    // Set JPEG Parameter In: HOST PARAM SET
    processingComp.set_pJecIn_parameters(pProxyComponent, &ps_ddep_sec_jpeg_param_desc_dh->host_param_set.s_in_parameters);
    // Set JPEG Parameter Other and header
	set_pJecOther_parameters(pProxyComponent, &ps_ddep_sec_jpeg_param_desc_dh->host_param_set, header_size_in_bytes, headerBufferAddress, thumbnailImageBufferAddress, runLevelBufferAddress);
	ps_ddep_sec_jpeg_param_desc_dh->host_param_set_atomic_exit = atomic_write;
	//+ER 354962
	if(isNewParam)
	{
		addPendingCommand();
		setparamitf.setParameter(ID_SEC_JPEG,mParamBufferDesc);
	}
	//need to check
	nNumber_arm = nNumber_arm << 1 ;
	nNumber_arm = (nNumber_arm | (((pProxyComponent->isHVABased) ? 1 : 0) &0x1));
	setparamitf.set_nNumber(nNumber_arm);
	//-ER 354962
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : configureAlgo DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}


void JPEGEnc_ArmNmfProcessingComponent::JPEGheaderCreation(JPEGEnc_Proxy *jpegenc)
{
	/* Start of Image
	   -------------- */
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In JPEGheaderCreation <line no %d> ",__LINE__);
	t_cm_system_address sysAddr;
	sysAddr.logical = (t_uint32 )mHeaderBufferDesc.nLogicalAddress;
	DBC_ASSERT(sysAddr.logical);
	processingComp.headerCreation(jpegenc,(OMX_U8 **)&(sysAddr.logical),&(mHeaderBufferDesc.nSize));
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : JPEGheaderCreation DONE<line no %d> ",__LINE__);

}


OMX_ERRORTYPE JPEGEnc_ArmNmfProcessingComponent::errorRecoveryDestroyAll(){
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : In errorRecoveryDestroyAll <line no %d> ",__LINE__);
	mCodec->stop();
	codecStop();
	return VFM_NmfHost_ProcessingComponent::deInstantiate();
}

//+ER 354962
void JPEGEnc_ArmNmfProcessingComponent::addPendingCommand()
{
    OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : in addPendingCommand <line no %d> ",__LINE__);
	OstTraceFiltInst1(TRACE_FLOW, "In JPEGEnc_ArmNmfProcessingComponent : in addPendingCommand : mNbPendingCommands : %d ",mNbPendingCommands);
    mNbPendingCommands++;
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent :  addPendingCommand DONE <line no %d> ",__LINE__);
}

void JPEGEnc_ArmNmfProcessingComponent::pendingCommandAck()
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent : in pendingCommandAck <line no %d> ",__LINE__);
	OstTraceFiltInst1(TRACE_FLOW, "In JPEGEnc_ArmNmfProcessingComponent : in pendingCommandAck : mNbPendingCommands : %d ",mNbPendingCommands);
	if(!(mNbPendingCommands>0))
		jpegenc_armnmf_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
    mNbPendingCommands--;
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_ArmNmfProcessingComponent :  pendingCommandAck DONE <line no %d> ",__LINE__);
}
//-ER 354962



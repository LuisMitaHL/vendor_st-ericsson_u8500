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

#define _CNAME_ Vfm_jpegenc
#include "osi_trace.h"
#include "VFM_DDepUtility.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegenc_proxy_src_JPEGEnc_NmfMpc_ProcessingComponentTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

#ifdef _CACHE_OPT_
	#include "VFM_Memory.h"
#endif
#include <stdio.h>
#include "VFM_Port.h"
#include "JPEGEnc_Proxy.h"
#include <cm/inc/cm_macros.h>
#include "host/eventhandler.hpp"
#include "SharedBuffer.h"
#include "JPEGEnc_NmfMpc_ProcessingComponent.h"

#ifdef PACKET_VIDEO_SUPPORT
#ifndef LOG_TAG
#define LOG_TAG "jpegenc"
#endif
#include<cutils/log.h>
#endif
#define EXIF_HEADER 64*1024
//volatile ts_ddep_buffer_descriptor * volatile g_DebugJpegEnc[2];
volatile ts_ddep_buffer_descriptor g_mDebugBufferDescjpegenc1;

#define RETURN_XXX_IF_WRONG_OST(_x, _error)  { if (!(_x)) { jpegenc_nmfmpc_assert(_error, __LINE__, OMX_FALSE); return (_error); } }


/// @ingroup JPEGEncoder
/// @brief Max size of the JPEG header
OMX_U16 _nHeaderSize = ((OMX_U16) (0x1318/8) + 1);
OMX_U32 _nEXIFHeaderSize = ((OMX_U32) (64*1024) + ((OMX_U32) (0x1318/8) + 1)); //64KB
OMX_U8  _nJFIFappSegmentSize = ((OMX_U8)18);

#define BUFFER_LINKLIST_SIZE (512)
#define DEBUG_MEM_SIZE (64*1024 - 1)
/// @ingroup JPEGEncoder
/// @brief If we want to optimize the computation of the header and quantization
/// table or not (if already computed)
extern int _statusOptimize; // Coming from JPEGEnc_ParamAndConfig.cpp

// some constants dedicated to huffman for the length and code
extern t_uint16 _nHuffmanLength;
extern t_uint16 _nHuffmanCode;
//extern "C" void PrintSvaMemoryStatus(void);
 /*------------------------------------------------------------------------
 * Private constants
 *----------------------------------------------------------------------*/

void JPEGEnc_NmfMpc_ProcessingComponent::jpegenc_nmfmpc_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
    if (OMX_ErrorNone != omxError)
    {
        OstTraceInt2(TRACE_ERROR, "JPEGENC_NMF_MPC : errorType : 0x%x error line no %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}


JPEGEnc_NmfMpc_ProcessingComponent::JPEGEnc_NmfMpc_ProcessingComponent(ENS_Component &enscomp): VFM_NmfMpc_ProcessingComponent(enscomp)
{
		OstTraceInt1(TRACE_FLOW, "In JPEGEnc_NmfMpc_ProcessingComponent : In Constructor <line no %d> ",__LINE__);
		 ddHandle=VFM_INVALID_CM_HANDLE;
		 mNmfAlgoHandle=VFM_INVALID_CM_HANDLE;
		 mNmfRmHandle=VFM_INVALID_CM_HANDLE;

		 cOSTTrace=VFM_INVALID_CM_HANDLE;

		 mChannelId=0;
         pProxyComponent = (JPEGEnc_Proxy *)(&mENSComponent);

        mpc_vfm_mem_ctxt = 0;

        mHeaderBufferDesc.nSize = 0;
	    mHeaderBufferDesc.nMpcAddress = 0;
	    mHeaderBufferDesc.nLogicalAddress = 0;
	    mHeaderBufferDesc.nPhysicalAddress = 0;

	    mTempBufferDesc.nSize = 0;
	    mTempBufferDesc.nMpcAddress = 0;
	    mTempBufferDesc.nLogicalAddress = 0;
	    mTempBufferDesc.nPhysicalAddress = 0;

        mEXIFHeaderBufferDesc.nSize = 0;
	    mEXIFHeaderBufferDesc.nMpcAddress = 0;
	    mEXIFHeaderBufferDesc.nLogicalAddress = 0;
	    mEXIFHeaderBufferDesc.nPhysicalAddress = 0;

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
		portSettingCallBack.parentClass = 0x0;
	   VPPHandle = (t_cm_memory_handle)0;

	   fullDVFSSet = OMX_FALSE;


}


//#define DUMP_TRACE_TO_FILE_BEFORE_EXIT

#ifdef DUMP_TRACE_TO_FILE_BEFORE_EXIT

#include "stdio.h"

typedef struct
{
	t_uint16 D1;
	t_uint16 D2;
	t_uint16 startoffset;
	t_uint16 currentWordToWrite;
	t_uint16 bytesAvailable;
	t_uint16 bytesAvailableH;
	t_uint16 bytesConsumed;
	t_uint16 bytesConsumedH;
	t_uint16 OverFlowOccured;
	t_uint16 isCircularModeEnable;
	t_uint16 D3;
	t_uint16 D4;
	t_uint16 D5;
	t_uint16 D6;
	t_uint16 D7;
	t_uint16 D8;
} t_mpc_trace_internal_tmp;


void printNMFTraceJpegEnc()
{
    t_mpc_trace_internal_tmp * p_mpc_trace_internal_tmp;
    extern volatile ts_ddep_buffer_descriptor g_mDebugBufferDescjpegenc1;
	volatile ts_ddep_buffer_descriptor *pS = &g_mDebugBufferDescjpegenc1;

    char *ptr =  (char *)pS->nLogicalAddress;
	p_mpc_trace_internal_tmp = (t_mpc_trace_internal_tmp *) ptr;
    int i;

	int bufferConsumed;

	FILE *fp = fopen("JpegEnc.txt", "w");

	if (fp == NULL)
	{
		printf("Error: opening tarce file to write");
		exit(-1);
	}

	//for (i = 0 ;i < 32 ; i++)
	//{
	//	printf("--%X--", ptr[i]);
	//}

    ptr +=32;

	bufferConsumed = p_mpc_trace_internal_tmp->bytesConsumed + (p_mpc_trace_internal_tmp->bytesConsumedH<<16);

	fprintf(fp, "Trace Size = %d bytes\n", bufferConsumed);


    for (i = 0 ;i < bufferConsumed; i++)
    {
        fputc(ptr[i], fp);
    }

	fclose(fp);

	//exit(-1);
}
#endif


JPEGEnc_NmfMpc_ProcessingComponent::~JPEGEnc_NmfMpc_ProcessingComponent()
{

#ifdef DUMP_TRACE_TO_FILE_BEFORE_EXIT
	printNMFTraceJpegEnc();
#endif
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In destructor  <line no %d> ",pProxyComponent,__LINE__);
	if (mParamBufferDesc.nSize)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Freeing memory for mParamBufferDesc <line no %d> ",__LINE__);
		VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mParamBufferDesc.nLogicalAddress);
		mParamBufferDesc.nSize = 0;
		mParamBufferDesc.nLogicalAddress = 0;
		mParamBufferDesc.nPhysicalAddress = 0;
		mParamBufferDesc.nMpcAddress = 0;
		//ENS::freeMpcMemory(mParamBufferHandle);
		pProxyComponent->pFwPerfDataPtr = (OMX_PTR)0x0;
		//mParamBufferHandle = 0;
	}

	if(mDebugBufferDesc.nSize)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Freeing memory for mDebugBufferDesc <line no %d> ",__LINE__);
		VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mDebugBufferDesc.nLogicalAddress);
		mDebugBufferDesc.nSize = 0;
		mDebugBufferDesc.nLogicalAddress = 0;
		mDebugBufferDesc.nPhysicalAddress = 0;
		mDebugBufferDesc.nMpcAddress = 0;
		//mDebugBufferHandle = 0;
	}

	if(mHeaderBufferDesc.nSize)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Freeing memory for mHeaderBufferDesc <line no %d> ",__LINE__);
		VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mHeaderBufferDesc.nLogicalAddress);
		mHeaderBufferDesc.nSize = 0;
		mHeaderBufferDesc.nLogicalAddress = 0;
		mHeaderBufferDesc.nPhysicalAddress = 0;
		mHeaderBufferDesc.nMpcAddress = 0;
		//mHeaderBufferHandle = 0x0;
	}

	 if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingEXIF)
	 {
	 	//HwBuffer::FreeBuffers(mEXIFHeaderBufferHandle);
		if(mEXIFHeaderBufferDesc.nSize)
		{
			OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Freeing memory for mEXIFHeaderBufferDesc <line no %d> ",__LINE__);
			VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mEXIFHeaderBufferDesc.nLogicalAddress);
			mEXIFHeaderBufferDesc.nSize = 0;
			mEXIFHeaderBufferDesc.nLogicalAddress = 0;
			mEXIFHeaderBufferDesc.nPhysicalAddress = 0;
			mEXIFHeaderBufferDesc.nMpcAddress = 0;
			//mEXIFHeaderBufferHandle = 0x0;
		}
	 }


	if(mLinkListBufferDesc.nSize)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Freeing memory for mLinkListBufferDesc <line no %d> ",__LINE__);
		VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mLinkListBufferDesc.nLogicalAddress);
		mLinkListBufferDesc.nSize = 0;
		mLinkListBufferDesc.nLogicalAddress = 0;
		mLinkListBufferDesc.nPhysicalAddress = 0;
		mLinkListBufferDesc.nMpcAddress = 0;
		//mLinkListParamBufferHandle = 0x0;
	}

	if(mpc_vfm_mem_ctxt)
	{
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Closing Memory List  <line no %d> ",__LINE__);
		VFM_CloseMemoryList(mpc_vfm_mem_ctxt);
	}

	mpc_vfm_mem_ctxt = 0;
OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent :  destruction done  <line no %d> ",pProxyComponent,__LINE__);
}

OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::allocateBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_U32 nSizeBytes,
        OMX_U8 **ppData,
        void **bufferAllocInfo,
        void **portPrivateInfo)
{
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In allocateBuffer   <line no %d> ",pProxyComponent,__LINE__);
	OMX_ERRORTYPE error;
	error =  NmfMpc_ProcessingComponent::allocateBuffer(nPortIndex,nBufferIndex,nSizeBytes,ppData,bufferAllocInfo,portPrivateInfo);
	if(error != OMX_ErrorNone)
		return error;
#ifdef _CACHE_OPT_
	OstTraceFiltStatic1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : allocateBuffer : Cacheable Buffers  <line no %d> ",pProxyComponent,__LINE__);
	OMX_U32 bufPhysicalAddr = VFM_NmfMpc_ProcessingComponent::getBufferPhysicalAddress(*bufferAllocInfo,*ppData,nSizeBytes);
	if(!mpc_vfm_mem_ctxt)
	{
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		mpc_vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
	}
	VFM_AddMemoryExternalSource(mpc_vfm_mem_ctxt,(*ppData), (t_uint8 *)bufPhysicalAddr,nSizeBytes,(*bufferAllocInfo));
#endif
	OstTraceFiltStatic3(TRACE_FLOW, "In JPEGEnc_NmfMpc_ProcessingComponent : In allocateBuffer nPortIndex : %d nSizeBytes : %d   <line no %d> ",pProxyComponent,nPortIndex,nSizeBytes,__LINE__);
	OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : allocateBuffer DONE  <line no %d> ",pProxyComponent,__LINE__);
	return OMX_ErrorNone;
}
OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::useBuffer(
        OMX_U32 nPortIndex,
        OMX_U32 nBufferIndex,
        OMX_BUFFERHEADERTYPE* pBufferHdr,
        void **portPrivateInfo)
{
		OstTraceFiltStatic2(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In useBuffer for Port No. : %d <line no %d> ",pProxyComponent,nPortIndex,__LINE__);
		OMX_ERRORTYPE error = NmfMpc_ProcessingComponent::useBuffer(nPortIndex,nBufferIndex,pBufferHdr,portPrivateInfo);
		if(error!=OMX_ErrorNone)
			return error;
#ifdef _CACHE_OPT_
		void *bufferAllocInfo = 0;
		OMX_U8 *pBuffer;
		OstTraceFiltStatic1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : useBuffer : Cacheable Buffers  <line no %d> ",pProxyComponent,__LINE__);
		ENS_Port *port = mENSComponent.getPort(nPortIndex);
        bufferAllocInfo = port->getSharedChunk();
		pBuffer = pBufferHdr->pBuffer;
		OMX_U32 bufPhysicalAddr = VFM_NmfMpc_ProcessingComponent::getBufferPhysicalAddress(bufferAllocInfo,pBuffer,pBufferHdr->nAllocLen);
		if(!mpc_vfm_mem_ctxt)
		{
			OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
			OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
			mpc_vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
		}
		VFM_AddMemoryExternalSource(mpc_vfm_mem_ctxt,pBuffer, (t_uint8 *)bufPhysicalAddr,pBufferHdr->nAllocLen,bufferAllocInfo);
#endif
		OstTraceFiltStatic1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : useBuffer DONE  <line no %d> ",pProxyComponent,__LINE__);
		return OMX_ErrorNone;
}
void JPEGEnc_NmfMpc_ProcessingComponent::doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "JPEGEnc_NmfMpc_ProcessingComponent :  doSpecificFillBufferDone_cb pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
#ifdef _CACHE_OPT_
	VFM_CacheInvalidate(mpc_vfm_mem_ctxt, pBuffer->pBuffer, pBuffer->nFilledLen);
#endif
}

OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler)
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In doSpecificEventHandler_cb <Event Type %d> ",event);
	if(event==OMX_EventCmdComplete&&nData1==OMX_CommandPortDisable)
	{
		/* ER 447646 */
		pProxyComponent->mParam.rotation_val = 0;
		/* ER 447646 */
		// for setting the parameters again incase another YUV is sent for encoding with same parameters
    	pProxyComponent->mSendParamToMpc.set();
		switch(nData2)
		{
			case 0:
					RETURN_XXX_IF_WRONG_OST((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
					RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(getNmfSharedBuf(0),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
    	            RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
					RETURN_XXX_IF_WRONG_OST((ENS::bindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
					RETURN_XXX_IF_WRONG_OST((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

					break;
			case 1:
					RETURN_XXX_IF_WRONG_OST((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
					RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(getNmfSharedBuf(1),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
    	            RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
					RETURN_XXX_IF_WRONG_OST((ENS::bindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
					RETURN_XXX_IF_WRONG_OST((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

					break;
    	    case OMX_ALL:
					RETURN_XXX_IF_WRONG_OST((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

					RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(getNmfSharedBuf(0),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
    	            RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
					RETURN_XXX_IF_WRONG_OST((ENS::bindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
					RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(getNmfSharedBuf(1),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
    	            RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
					RETURN_XXX_IF_WRONG_OST((ENS::bindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);

				    RETURN_XXX_IF_WRONG_OST((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
					break;

		}
	}
	deferEventHandler = OMX_FALSE;
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : doSpecificEventHandler_cb DONE <Event Type %d> ",event);return OMX_ErrorNone;
	return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
	OstTraceFiltInst2(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : doSpecificSendCommand Command : %d Data : %d ",eCmd,nData);
	switch(eCmd)
	{

		case OMX_CommandStateSet:
			switch(nData)
			{
				case OMX_StateExecuting:
				case OMX_StatePause:
				case OMX_StateIdle:

							if((OMX_U32)OMX_StateExecuting==nData)
							{
								if (pProxyComponent->is_full_powermgt) //should be OMX_FALSE (!!! NOT TO BE Enabled)
								{
									if (pProxyComponent->sva_mcps > 100)
									{
										OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc_ProcessingComp::setting full dvfs");
										VFM_PowerManagement::setFullDVFS(this);
										VFM_PowerManagement::setFullDDR(this);
                                	    VFM_PowerManagement::setLatency(this, VFM_PowerManagement::VFM_ResLatency_None);
										fullDVFSSet = OMX_TRUE;
									}
								}
								else
								{
									//always force it to 100%
										OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc_ProcessingComp::forcing full dvfs");
										VFM_PowerManagement::setFullDVFS(this);
										VFM_PowerManagement::setFullDDR(this);
                                	    VFM_PowerManagement::setLatency(this, VFM_PowerManagement::VFM_ResLatency_None);
										fullDVFSSet = OMX_TRUE;
								}
							}
							else
							{
								if (fullDVFSSet)
								{
									VFM_PowerManagement::releaseFullDVFS(this);
									VFM_PowerManagement::releaseFullDDR(this);
									VFM_PowerManagement::resetLatency(this);
									fullDVFSSet = OMX_FALSE;
								}
							}
							/* ER 447646 */
							if((OMX_U32)OMX_StateIdle == nData)
							{
								pProxyComponent->mParam.rotation_val = 0;
							}
							/* ER 447646 */
					break;
				default:
					break;
			}
			break;

		case OMX_CommandPortEnable:
		/* Update parameters again in case they are changed */
		/* ER 447646 */
		pProxyComponent->mSendParamToMpc.set();
		pProxyComponent->mParam.HeaderToGenerate.set();
		pProxyComponent->mParam.ConfigApplicable.set();
		/* ER 447646 */
		switch(nData)
		{
		case 0:
				RETURN_XXX_IF_WRONG_OST((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(getNmfSharedBuf(0), "mpc",ddHandle,"emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
                RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(ddHandle,"inputport", getNmfSharedBuf(0), "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG_OST((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
				/* ER 447646 */
               	configure();
				/* ER 447646 */
				break;
		case 1:
				RETURN_XXX_IF_WRONG_OST((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(getNmfSharedBuf(1),"mpc",ddHandle, "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
                RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(ddHandle,"outputport", getNmfSharedBuf(1), "emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG_OST((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
				/* ER 447646 */
                //configure();
				/* ER 447646 */
				break;
		case OMX_ALL:
				RETURN_XXX_IF_WRONG_OST((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(getNmfSharedBuf(0), "mpc",ddHandle,"emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
                RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);


				RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(ddHandle,"inputport", getNmfSharedBuf(0), "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(getNmfSharedBuf(1),"mpc",ddHandle, "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
                RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);

				RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(ddHandle,"outputport", getNmfSharedBuf(1), "emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG_OST((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
				/* ER 447646 */
               	configure();
				/* ER 447646 */
				break;
		}
			break;
		
		default:
			break;
	}

	bDeferredCmd = OMX_FALSE;
	OstTraceFiltInst2(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : doSpecificSendCommand  DONE Command : %d Data : %d ",eCmd,nData);
	return OMX_ErrorNone;
}

void JPEGEnc_NmfMpc_ProcessingComponent::doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : in doSpecificEmptyBufferDone_cb pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
}


OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::instantiate()
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In Instantiation line no : %d \n",__LINE__);
	pProxyComponent = (JPEGEnc_Proxy *)(&mENSComponent);
	t_uint32 fifo_size;		 // size of the FIFO for NMF commands NMFCALL()

    VFM_Port    *pt_port_in, *pt_port_out;
    pt_port_in = (VFM_Port*)pProxyComponent->getPort(VPB+0);
    pt_port_out = (VFM_Port*)pProxyComponent->getPort(VPB+1);

	fifo_size = (pt_port_in->getBufferCountActual()>pt_port_out->getBufferCountActual() ? pt_port_in->getBufferCountActual() : pt_port_out->getBufferCountActual()) + 1;

		registerStubsAndSkels();


    RETURN_XXX_IF_WRONG_OST((ENS::instantiateNMFComponent(pProxyComponent->getNMFDomainHandle(), "resource_manager", "resource_manager", &mNmfRmHandle, NMF_SCHED_URGENT) == OMX_ErrorNone),OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::instantiateNMFComponent(pProxyComponent->getNMFDomainHandle(), "jpegenc", "sec_jpeg_algo", &mNmfAlgoHandle,NMF_SCHED_NORMAL)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::instantiateNMFComponent(pProxyComponent->getNMFDomainHandle(), "osttrace.mmdsp", "sec_osttrace", &cOSTTrace,NMF_SCHED_NORMAL)==OMX_ErrorNone), OMX_ErrorUndefined);

        RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(mNmfAlgoHandle,"iBuffer",mNmfRmHandle,"iBuffer")== OMX_ErrorNone),OMX_ErrorUndefined);

        RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(mNmfAlgoHandle,"iMtf",mNmfRmHandle,"iMtf")== OMX_ErrorNone),OMX_ErrorUndefined);

		//PrintSvaMemoryStatus();
		RETURN_XXX_IF_WRONG_OST((ENS::instantiateNMFComponent(pProxyComponent->getNMFDomainHandle(), "vpp", "vpp", &VPPHandle, NMF_SCHED_NORMAL)==OMX_ErrorNone), OMX_ErrorUndefined);

		RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(VPPHandle,"iInterrupt",mNmfRmHandle,"iInterrupt")== OMX_ErrorNone),OMX_ErrorUndefined);
		// Data dep instantiation
		//PrintSvaMemoryStatus();
		RETURN_XXX_IF_WRONG_OST((ENS::instantiateNMFComponent(pProxyComponent->getNMFDomainHandle(), "jpegenc.mpc.ddep", "vec_jpeg_ddep", &ddHandle, NMF_SCHED_URGENT)==OMX_ErrorNone), OMX_ErrorUndefined);

		RETURN_XXX_IF_WRONG_OST((ENS::bindComponentFromHost(ddHandle, "ddep", &iDdep, (t_uint32)8 )==OMX_ErrorNone), OMX_ErrorUndefined);

		RETURN_XXX_IF_WRONG_OST((ENS::bindComponentFromHost(ddHandle, "sendcommand",  &mIsendCommand, fifo_size)==OMX_ErrorNone), OMX_ErrorUndefined);

		//RETURN_XXX_IF_WRONG_OST((ENS::bindComponentToHost(pProxyComponent->getOMXHandle(), ddHandle, "proxy",  &local_cb_eventhandler, fifo_size)==OMX_ErrorNone), OMX_ErrorUndefined);
		RETURN_XXX_IF_WRONG_OST((ENS::bindComponentToHost(pProxyComponent->getOMXHandle(), ddHandle, "proxy",  (NMF::InterfaceDescriptor *)this, fifo_size)==OMX_ErrorNone), OMX_ErrorUndefined);

		portSettingCallBack.parentClass = this;
		RETURN_XXX_IF_WRONG_OST((ENS::bindComponentToHost(pProxyComponent->getOMXHandle(), ddHandle, "iChangePortSettingsSignal",  (NMF::InterfaceDescriptor *)&portSettingCallBack, 4)==OMX_ErrorNone), OMX_ErrorUndefined);

        //RETURN_XXX_IF_WRONG_OST((ENS::bindComponentToHost(ddHandle, "ddep", &iPortSetting, 2)==OMX_ErrorNone), OMX_ErrorUndefined);

   if(pt_port_in->isEnabled() == OMX_TRUE)
	{
    // Input port bindings
		RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(getNmfSharedBuf(0), "mpc",ddHandle,"emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
		RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(ddHandle,"inputport",getNmfSharedBuf(0), "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
	}
	else
	{
		RETURN_XXX_IF_WRONG_OST((ENS::bindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
    }

	if(pt_port_out->isEnabled() == OMX_TRUE)
	{
		// Output port bindings
		RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(getNmfSharedBuf(1),"mpc",ddHandle, "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
		RETURN_XXX_IF_WRONG_OST((ENS::bindComponent(ddHandle, "outputport", getNmfSharedBuf(1),"emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
	}
	else
	{
	   RETURN_XXX_IF_WRONG_OST((ENS::bindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
	}
    //  Bind component
	RETURN_XXX_IF_WRONG_OST((ENS::bindComponentAsynchronous (ddHandle, "iResource", mNmfRmHandle, "iResource",4)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::bindComponentAsynchronous (ddHandle ,"codec_algo", mNmfAlgoHandle, "iAlgo",4)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::bindComponent (ddHandle ,"codec_update_algo"       , mNmfAlgoHandle, "iUpdateAlgo"       )==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::bindComponent (ddHandle ,"vpp_algo"       , VPPHandle, "iAlgo"       )==OMX_ErrorNone), OMX_ErrorUndefined);

    RETURN_XXX_IF_WRONG_OST((ENS::bindComponent (ddHandle ,"genericfsm"       , mNmfGenericFsmLib, "genericfsm"       )==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::bindComponent (ddHandle ,"componentfsm"       , mNmfComponentFsmLib, "componentfsm"       )==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::bindComponent (mNmfComponentFsmLib, "osttrace" , cOSTTrace,"osttrace"     )==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::bindComponent (ddHandle, "iSleep", mNmfRmHandle, "iSleep")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::bindComponent (ddHandle, "osttrace",cOSTTrace,"osttrace")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::bindComponentAsynchronous (ddHandle, "iSendLogEvent", ddHandle, "iGetLogEvent", 4)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::bindComponentAsynchronous (ddHandle, "me", ddHandle, "postevent", 2)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::bindComponentFromHost(ddHandle, "fsminit",   getNmfFsmInitItf(), 4)==OMX_ErrorNone), OMX_ErrorUndefined);

	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : Instantiation Complete line no : %d \n",__LINE__);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::start()
{

	RETURN_XXX_IF_WRONG_OST((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
	//check if resource manager and algo to be started
    OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : start Complete line no : %d \n",__LINE__);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::stop()
{

	RETURN_XXX_IF_WRONG_OST((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
	//check if resource manager and algo to be stopped
	/* ER 447646 */
	pProxyComponent->mParam.rotation_val = 0;
	/* ER 447646 */

    OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : stop Complete line no : %d \n",__LINE__);
    return OMX_ErrorNone;
}


OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::deInstantiate()
{
    OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In DeInstantiation line no : %d \n",__LINE__);
    VFM_Port    *pt_port_in, *pt_port_out;
    pt_port_in = (VFM_Port*)pProxyComponent->getPort(VPB+0);
    pt_port_out = (VFM_Port*)pProxyComponent->getPort(VPB+1);

	// for setting the parameters again incase another YUV is sent for encoding with same parameters
	/* ER 447646 */
    pProxyComponent->mSendParamToMpc.set();
	/* ER 447646 */

	if(mTempBufferDesc.nSize)
	{
		VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mTempBufferDesc.nLogicalAddress);
		mTempBufferDesc.nSize = 0;
		mTempBufferDesc.nLogicalAddress = 0;
		mTempBufferDesc.nPhysicalAddress = 0;
//		temp_bufhandle = 0;
	}

	//if(temp_bufhandle)
	//HwBuffer::FreeBuffers(temp_bufhandle);

    RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentFromHost( getNmfFsmInitItf())==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentAsynchronous (ddHandle, "iSendLogEvent")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentAsynchronous (ddHandle, "me")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent (ddHandle ,"componentfsm")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent (ddHandle ,"genericfsm")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentAsynchronous (ddHandle ,"codec_algo")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent (ddHandle ,"codec_update_algo")==OMX_ErrorNone), OMX_ErrorUndefined);

	// Bind it Asynchronously to avoid to nest execution of the  Datadep  "process" function
	RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentAsynchronous (ddHandle, "iResource")==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent (ddHandle, "iSleep")==OMX_ErrorNone), OMX_ErrorUndefined);


    RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(mNmfAlgoHandle,"iBuffer")== OMX_ErrorNone),OMX_ErrorUndefined);

    RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(mNmfAlgoHandle,"iMtf")== OMX_ErrorNone),OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(VPPHandle,"iInterrupt")== OMX_ErrorNone),OMX_ErrorUndefined);

    RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent (ddHandle ,"vpp_algo")==OMX_ErrorNone), OMX_ErrorUndefined);

    RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent (ddHandle, "osttrace")==OMX_ErrorNone), OMX_ErrorUndefined);

    RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent (mNmfComponentFsmLib, "osttrace")==OMX_ErrorNone), OMX_ErrorUndefined);

    RETURN_XXX_IF_WRONG_OST((ENS::destroyNMFComponent(VPPHandle) == OMX_ErrorNone),OMX_ErrorUndefined);

    RETURN_XXX_IF_WRONG_OST((ENS::destroyNMFComponent(mNmfAlgoHandle) == OMX_ErrorNone),OMX_ErrorUndefined);
	mNmfAlgoHandle = VFM_INVALID_CM_HANDLE;

	RETURN_XXX_IF_WRONG_OST((ENS::destroyNMFComponent(mNmfRmHandle) == OMX_ErrorNone),OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG_OST((ENS::destroyNMFComponent(cOSTTrace) == OMX_ErrorNone),OMX_ErrorUndefined);


	//  UnBind HOST to DD
	if(ddHandle != VFM_INVALID_CM_HANDLE){
		RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentFromHost(&iDdep)==OMX_ErrorNone), OMX_ErrorUndefined);

		RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentFromHost(&mIsendCommand)==OMX_ErrorNone), OMX_ErrorUndefined);

		if(pt_port_in->isEnabled()==OMX_TRUE)
		{
		   RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(getNmfSharedBuf(0),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
		   RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
		}
		else
		{
			RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
        }
		if(pt_port_out->isEnabled()==OMX_TRUE)
		{
			RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(getNmfSharedBuf(1),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
			RETURN_XXX_IF_WRONG_OST((ENS::unbindComponent(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
		}
		else
		{
		   RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
		}

		RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentToHost(pProxyComponent->getOMXHandle(), ddHandle, "proxy")==OMX_ErrorNone), OMX_ErrorUndefined);


		RETURN_XXX_IF_WRONG_OST((ENS::unbindComponentToHost(pProxyComponent->getOMXHandle(), ddHandle, "iChangePortSettingsSignal")==OMX_ErrorNone), OMX_ErrorUndefined);

		RETURN_XXX_IF_WRONG_OST((ENS::destroyNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
		ddHandle = VFM_INVALID_CM_HANDLE;
	}
    unregisterStubsAndSkels();

    OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : DeInstantiation Complete line no : %d \n",__LINE__);
    return OMX_ErrorNone;
}

void JPEGEnc_NmfMpc_ProcessingComponent::registerStubsAndSkels()
{

	CM_REGISTER_STUBS_SKELS(video_jpegenc_cpp);

}

void JPEGEnc_NmfMpc_ProcessingComponent::unregisterStubsAndSkels()
{
	CM_UNREGISTER_STUBS_SKELS(video_jpegenc_cpp);
}







void JPEGEnc_NmfMpc_ProcessingComponent::portSettings(OMX_U32 size)
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In portSettings <line no %d> ",__LINE__);
	JPEGEnc_Proxy   *proxy_component = (JPEGEnc_Proxy *)(&mENSComponent);
    proxy_component->detectPortSettingsAndNotify(size);
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : portSettings DONE <line no %d> ",__LINE__);
}


void JPEGEnc_portSetting::portSettings(OMX_U32 size)
{
	parentClass->portSettings(size);
}



OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::configure()
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In configure <line no %d> ",__LINE__);
	if(!mpc_vfm_mem_ctxt)
	{
		OstTraceFiltInst1(TRACE_FLOW, "In JPEGEnc_NmfMpc_ProcessingComponent : Creating Memory List <line no %d> ",__LINE__);
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		mpc_vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
	}


    if(pProxyComponent->mSendParamToMpc.get())
		{
			//OMX_ERRORTYPE error;
			
			//Allocating memory for Header
			if(!mHeaderBufferDesc.nSize)
			{
				OMX_U8 *header_buf_logical_addr;
				OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Allocating Memory to mHeaderBufferDesc <line no %d> ",__LINE__);
				if (pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingJPEG)
				{
					header_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, (_nHeaderSize+_nJFIFappSegmentSize), VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Allocating Memory of Size : 0x%x to mHeaderBufferDesc <line no %d> ",_nHeaderSize+_nJFIFappSegmentSize,__LINE__);
				}
				else
				{
					header_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, _nHeaderSize, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Allocating Memory of Size : 0x%x to mHeaderBufferDesc <line no %d> ",_nHeaderSize,__LINE__);
				}

				if(!header_buf_logical_addr)
				{
					VFM_Free(mpc_vfm_mem_ctxt, header_buf_logical_addr);
					mpc_vfm_mem_ctxt = 0;
					OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error allocating memory to mHeaderBufferDesc \n");
					jpegenc_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
					return OMX_ErrorInsufficientResources;
				}

				if (pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingJPEG)
				{
					mHeaderBufferDesc.nSize = (_nHeaderSize + _nJFIFappSegmentSize);
				}
				else
				{
					mHeaderBufferDesc.nSize = _nHeaderSize;
				}

				mHeaderBufferDesc.nLogicalAddress = (t_uint32)header_buf_logical_addr;
				mHeaderBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, header_buf_logical_addr));
				mHeaderBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, header_buf_logical_addr));
			}


			processingComp.compute_quantization_table(pProxyComponent);    // compute the quantization table
			JPEGheaderCreation(pProxyComponent);
			configureAlgo();

			iDdep.disableFWCodeexection(pProxyComponent->mIsARMLoadComputed);
			if(pProxyComponent->mParam.inputBufferFormat == (OMX_COLOR_FORMATTYPE)OMX_COLOR_FormatYUV420PackedPlanar)
			{
				if(!mTempBufferDesc.nSize)
				{
					OMX_U8 *temp_buf_logical_addr;
					OMX_U32 temp_buf_size = ((pProxyComponent->getFrameWidth(0) * pProxyComponent->getFrameHeight(0))*3)/2;
					temp_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, temp_buf_size, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
					OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Allocating Memory of Size : 0x%x to mTempBufferDesc <line no %d> ",temp_buf_size,__LINE__);
					if(!temp_buf_logical_addr)
					{
						VFM_Free(mpc_vfm_mem_ctxt, temp_buf_logical_addr);
						mpc_vfm_mem_ctxt = 0;
						OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error allocating memory to mTempBufferDesc \n");
						jpegenc_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
						return OMX_ErrorInsufficientResources;
					}
					mTempBufferDesc.nSize = temp_buf_size;
					mTempBufferDesc.nLogicalAddress = (t_uint32)temp_buf_logical_addr;
					mTempBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, temp_buf_logical_addr));
					mTempBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, temp_buf_logical_addr));
				}
				t_cm_system_address sysAddr;
                sysAddr.physical = ((mTempBufferDesc.nPhysicalAddress+0xf)&0xfffffff0);
				//sysAddr.physical=ENDIANESS_CONVERSION((t_uint8 *)sysAddr.physical);
				iDdep.setNeeds(mChannelId,mLinkListBufferDesc,mDebugBufferDesc,sysAddr.physical);
			}
			else
			{
				iDdep.setNeeds(mChannelId,mLinkListBufferDesc,mDebugBufferDesc,0);
			}
			OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : In configure setNeeds DONE <line no %d> ",__LINE__);
            //@todo Pass LinkList desc to Parser
			//Pass Param desc to DataDep
			//iDdep.setParameter(mChannelId,ID_SEC_JPEG,mParamBufferDesc);
			if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingEXIF) //allocate 64Kb of additional memory
			{

				if(!mEXIFHeaderBufferDesc.nSize)
				{
					OMX_U8 *exif_buf_logical_addr;
					OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Allocating Memory of Size : 0x%x to mEXIFHeaderBufferDesc <line no %d> ",_nEXIFHeaderSize,__LINE__);
					exif_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, _nEXIFHeaderSize, VFM_MEM_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

					if(!exif_buf_logical_addr)
					{
						VFM_Free(mpc_vfm_mem_ctxt, exif_buf_logical_addr);
						mpc_vfm_mem_ctxt = 0;
						OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error allocating memory to mEXIFHeaderBufferDesc \n");
						jpegenc_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
						return OMX_ErrorInsufficientResources;
					}
					mEXIFHeaderBufferDesc.nSize = _nEXIFHeaderSize;
					mEXIFHeaderBufferDesc.nLogicalAddress = (t_uint32)exif_buf_logical_addr;
					mEXIFHeaderBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, exif_buf_logical_addr));
					mEXIFHeaderBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, exif_buf_logical_addr));
				}

			}

			pProxyComponent->mSendParamToMpc.reset();
		}
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : configure DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::configureAlgo()
{
	OMX_BOOL isNewParam = OMX_FALSE;
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In configureAlgo <line no %d> ",__LINE__);
    t_uint8 *ptr;
    t_uint8 *physicalptr;

	t_bool zero_param_desc_dh=0;

    t_uint32 size;
    t_uint16 width, height;
    t_uint32 header_size_in_bytes;

    // misc. temporary buffers
    t_uint8 * thumbnailImageBufferAddress,*thumbnailImageBufferPhysicalAddress;
    t_uint8 * runLevelBufferAddress;
    t_uint8 * runLevelBufferPhysicalAddress;
    t_uint8 * headerBufferAddress,*headerBufferPhysicalAddress;
	t_uint32 headerBufferOffset;
    t_uint32  linkBitstreamBufferSize;


	ts_ddep_sec_jpeg_param_desc_dh *ps_ddep_sec_jpeg_param_desc_dh=(ts_ddep_sec_jpeg_param_desc_dh *)0;
    headerBufferOffset = (t_uint32)&ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.header_buffer;

    header_size_in_bytes = mHeaderBufferDesc.nSize;

    width = pProxyComponent->getFrameWidth(1);;
    height = pProxyComponent->getFrameHeight(1);;
    linkBitstreamBufferSize = sizeof(ts_t1xhv_bitstream_buf_link)+VFM_ADDRESS_ALIGN_MASK;
    int thumbnailsize = ((((width/8 +15)/16)*16) * (((height/8 +15)/16)*16) * 2) + VFM_IMAGE_BUFFER_ALIGN_MASK;
    int runlevelsize = (width * 32 * 4 * 3) + VFM_IMAGE_BUFFER_ALIGN_MASK;
    /*
	size =
        sizeof (ts_t1xhv_vec_frame_buf_in)      +
        sizeof (ts_t1xhv_vec_frame_buf_out)     +
        sizeof (ts_t1xhv_vec_internal_buf)      +
        sizeof (ts_t1xhv_vec_header_buf)        +
        sizeof (ts_t1xhv_bitstream_buf_pos)     +
        sizeof (ts_t1xhv_bitstream_buf_link)    +
        sizeof (ts_t1xhv_bitstream_buf_pos)     +
        sizeof (ts_t1xhv_vec_jpeg_param_in)     +
        sizeof (ts_t1xhv_vec_jpeg_param_out)    +
        sizeof (ts_t1xhv_vec_jpeg_param_inout)  +
        sizeof (ts_t1xhv_vec_jpeg_param_inout)  +
        thumbnailsize +
        runlevelsize +
        header_size_in_bytes+VFM_IMAGE_BUFFER_ALIGN_MASK+                  // header of JPEG
        linkBitstreamBufferSize+VFM_ADDRESS_ALIGN_MASK;                      // for linkBitstreamBufferAddressRounded
	*/
	size = sizeof(ts_ddep_sec_jpeg_param_desc_dh)
			+ thumbnailsize
			+ runlevelsize
			+ VFM_IMAGE_BUFFER_ALIGN_MASK                  // header of JPEG
			+ linkBitstreamBufferSize
			+ VFM_ADDRESS_ALIGN_MASK
			+ 5*0xf; //for alignment

	// header buffer size not expected to cross 1024 bytes for current feature set
	DBC_ASSERT(header_size_in_bytes<sizeof(ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.header_buffer));

    {
		t_uint8* param_buf_logical_addr = 0;
		t_uint8* linklist_buf_logical_addr = 0;
		t_uint8* debug_buf_logical_addr = 0;

		if (mParamBufferDesc.nSize && pProxyComponent->mSendParamToMpc.get())
		{
			VFM_Free(mpc_vfm_mem_ctxt, (t_uint8*)mParamBufferDesc.nLogicalAddress);
			mParamBufferDesc.nSize = 0;
			mParamBufferDesc.nLogicalAddress = 0;
			mParamBufferDesc.nPhysicalAddress = 0;
			pProxyComponent->pFwPerfDataPtr = (OMX_PTR)0x0;
		}

		if (!mParamBufferDesc.nSize)
		{
			isNewParam = OMX_TRUE;
			param_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, size, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Allocating Memory of Size : 0x%x to mParamBufferDesc <line no %d> ",size,__LINE__);
			if(!param_buf_logical_addr)
			{
				VFM_Free(mpc_vfm_mem_ctxt, param_buf_logical_addr);
				mpc_vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error allocating memory to mParamBufferDesc \n");
				jpegenc_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
				return OMX_ErrorInsufficientResources;
			}
			mParamBufferDesc.nSize = size;
			mParamBufferDesc.nLogicalAddress = (t_uint32)param_buf_logical_addr;
			mParamBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, param_buf_logical_addr));
			mParamBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, param_buf_logical_addr));

			zero_param_desc_dh = 1;
		}

		ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
        physicalptr = (t_uint8*)mParamBufferDesc.nPhysicalAddress;

		if (!mLinkListBufferDesc.nSize)
		{
			linklist_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, BUFFER_LINKLIST_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Allocating Memory of Size : 0x%x to mLinkListBufferDesc <line no %d> ",BUFFER_LINKLIST_SIZE,__LINE__);
			if(!linklist_buf_logical_addr)
			{
				//< not able to allocate linklist buffer, so dellocate previously allocated
				//< buffers for a clean graceful exit
				{
					VFM_Free(mpc_vfm_mem_ctxt, param_buf_logical_addr);
					mParamBufferDesc.nSize = 0;
					mParamBufferDesc.nLogicalAddress = 0;
					mParamBufferDesc.nPhysicalAddress = 0;
				}

				mpc_vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error allocating memory to mLinkListBufferDesc \n");
				jpegenc_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
				return OMX_ErrorInsufficientResources;
			}

			mLinkListBufferDesc.nSize = BUFFER_LINKLIST_SIZE;
			mLinkListBufferDesc.nLogicalAddress = (t_uint32)linklist_buf_logical_addr;
			mLinkListBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, linklist_buf_logical_addr));
			mLinkListBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, linklist_buf_logical_addr));

            //cm_error = ENS::allocMpcMemory(pProxyComponent->getNMFDomainHandle(),CM_MM_MPC_SDRAM16, 512, CM_MM_ALIGN_256BYTES, &mLinkListParamBufferHandle);
			//if (cm_error != CM_OK) return OMX_ErrorUndefined;
			//CM_GetMpcMemoryMpcAddress(mLinkListParamBufferHandle,&mLinkListBufferDesc.nMpcAddress);
			//CM_GetMpcMemorySystemAddress(mLinkListParamBufferHandle, &sysAddr);
			//mLinkListBufferDesc.nSize = 512;
			//mLinkListBufferDesc.nLogicalAddress = sysAddr.logical;
			//mLinkListBufferDesc.nPhysicalAddress = sysAddr.physical;
		}

		if (!mDebugBufferDesc.nSize)
		{
			debug_buf_logical_addr  = VFM_Alloc(mpc_vfm_mem_ctxt, DEBUG_MEM_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			OstTraceFiltInst2(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Allocating Memory of Size : 0x%x to mDebugBufferDesc <line no %d> ",DEBUG_MEM_SIZE,__LINE__);
			if(!debug_buf_logical_addr)
			{
				//< not able to allocate debug buffer, so dellocate previously allocated
				//< buffers for a clean graceful exit
				{
					VFM_Free(mpc_vfm_mem_ctxt, linklist_buf_logical_addr);
					mLinkListBufferDesc.nSize = 0;
					mLinkListBufferDesc.nLogicalAddress = 0;
					mLinkListBufferDesc.nPhysicalAddress = 0;
				}
				{
					VFM_Free(mpc_vfm_mem_ctxt, param_buf_logical_addr);
					mParamBufferDesc.nSize = 0;
					mParamBufferDesc.nLogicalAddress = 0;
					mParamBufferDesc.nPhysicalAddress = 0;
				}
				mpc_vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error allocating memory to mDebugBufferDesc \n");
				jpegenc_nmfmpc_assert(OMX_ErrorInsufficientResources, __LINE__, OMX_TRUE);
				return OMX_ErrorInsufficientResources;
			}

			mDebugBufferDesc.nSize = DEBUG_MEM_SIZE;
			mDebugBufferDesc.nLogicalAddress = (t_uint32)debug_buf_logical_addr;
			mDebugBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(mpc_vfm_mem_ctxt, debug_buf_logical_addr));
			mDebugBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(mpc_vfm_mem_ctxt, debug_buf_logical_addr));

            //cm_error = ENS::allocMpcMemory(pProxyComponent->getNMFDomainHandle(),CM_MM_MPC_SDRAM16,1024*64, CM_MM_ALIGN_256BYTES, &mDebugBufferHandle);
			//if (cm_error != CM_OK) return OMX_ErrorUndefined;
			//CM_GetMpcMemoryMpcAddress(mDebugBufferHandle,&mDebugBufferDesc.nMpcAddress);
			//CM_GetMpcMemorySystemAddress(mDebugBufferHandle, &sysAddr);
			//mDebugBufferDesc.nSize = 1024*64;
			//mDebugBufferDesc.nLogicalAddress = sysAddr.logical;
			//mDebugBufferDesc.nPhysicalAddress = sysAddr.physical;

			g_mDebugBufferDescjpegenc1.nSize =  mDebugBufferDesc.nSize;
			g_mDebugBufferDescjpegenc1.nLogicalAddress =  mDebugBufferDesc.nLogicalAddress;
			g_mDebugBufferDescjpegenc1.nPhysicalAddress =  mDebugBufferDesc.nPhysicalAddress;
			g_mDebugBufferDescjpegenc1.nMpcAddress =  mDebugBufferDesc.nMpcAddress;
		}
	}

	ps_ddep_sec_jpeg_param_desc_dh = (ts_ddep_sec_jpeg_param_desc_dh *) ptr;

	headerBufferAddress = ptr + headerBufferOffset;
	headerBufferPhysicalAddress = physicalptr + headerBufferOffset;

	if(((t_uint32)headerBufferAddress)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error in configureAlgo \n");
		jpegenc_nmfmpc_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
	if(((t_uint32)headerBufferPhysicalAddress)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error in configureAlgo \n");
		jpegenc_nmfmpc_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}

	ptr+=sizeof(ts_ddep_sec_jpeg_param_desc_dh);
	physicalptr+=sizeof(ts_ddep_sec_jpeg_param_desc_dh);

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
    physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error in configureAlgo \n");
		jpegenc_nmfmpc_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
    thumbnailImageBufferAddress = ptr;
	ptr = ptr + thumbnailsize;
	thumbnailImageBufferPhysicalAddress = physicalptr;                            physicalptr = physicalptr + thumbnailsize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
    physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error in configureAlgo \n");
		jpegenc_nmfmpc_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
    runLevelBufferAddress = ptr;
	ptr = ptr + runlevelsize;
    runLevelBufferPhysicalAddress = physicalptr;                                physicalptr = physicalptr + runlevelsize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
    physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error in configureAlgo \n");
		jpegenc_nmfmpc_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
    //headerBufferAddress = ptr;                                               ptr = ptr + header_size_in_bytes + VFM_IMAGE_BUFFER_ALIGN_MASK;
    //headerBufferPhysicalAddress = physicalptr;                               physicalptr = physicalptr + header_size_in_bytes + VFM_IMAGE_BUFFER_ALIGN_MASK;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
    physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error in configureAlgo \n");
		jpegenc_nmfmpc_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}
	ptr = ptr + linkBitstreamBufferSize+VFM_ADDRESS_ALIGN_MASK;
	physicalptr = physicalptr + linkBitstreamBufferSize+VFM_ADDRESS_ALIGN_MASK;
	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
    physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		OstTraceInt0(TRACE_ERROR, "In JPEGEnc_NmfMpc_ProcessingComponent : Error in configureAlgo \n");
		jpegenc_nmfmpc_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
		return OMX_ErrorUndefined;
	}

    thumbnailImageBufferAddress = (t_uint8 *)VFM_ROUND_UPPER(thumbnailImageBufferAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
    thumbnailImageBufferPhysicalAddress = (t_uint8 *)VFM_ROUND_UPPER(thumbnailImageBufferPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	runLevelBufferAddress = (t_uint8 *)VFM_ROUND_UPPER(runLevelBufferAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
	runLevelBufferPhysicalAddress = (t_uint8 *)VFM_ROUND_UPPER(runLevelBufferPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	t_uint16 atomic_write;

	if (zero_param_desc_dh)
	{
		t_uint32 count;
		//Zero memory
		//ts_ddep_sec_jpeg_param_desc_dh tmp={0};

		for (count = 0; count<sizeof(ts_ddep_sec_jpeg_param_desc_dh);count++)
			((t_uint8*)ps_ddep_sec_jpeg_param_desc_dh)[count]=0;

		//*ps_ddep_sec_jpeg_param_desc_dh = tmp;
		OstTraceFiltInst1(TRACE_DEBUG, "In JPEGEnc_NmfMpc_ProcessingComponent : Inside configureAlgo::if (zero_param_desc_dh) <line no %d> ",__LINE__);
		// Set JPEG Parameter In: MPC PARAM SET (only once)
		processingComp.set_pJecIn_parameters(pProxyComponent, &ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.s_in_parameters);
		// Set JPEG Parameter Other and header
		set_pJecOther_parameters(pProxyComponent, &ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set, header_size_in_bytes, headerBufferPhysicalAddress, thumbnailImageBufferAddress, runLevelBufferPhysicalAddress);

		if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingEXIF)
		{
				processingComp.setValue((OMX_U32)&ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.header_buffer);//headerMPCPosition = (OMX_U32)&ps_ddep_sec_jpeg_param_desc->header_buffer;
				processingComp.setHeaderSizeValue((OMX_U32)&ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.s_header_buf.header_size);// = _nEXIFHeaderSize;
		}


		pProxyComponent->pFwPerfDataPtr = (OMX_PTR)(&(ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set.s_out_perf_parameters));

		atomic_write = 0;
	}
	else atomic_write = ps_ddep_sec_jpeg_param_desc_dh->host_param_set_atomic_entry + 1;

    ps_ddep_sec_jpeg_param_desc_dh->host_param_set_atomic_entry = atomic_write;
    // Set JPEG Parameter In: HOST PARAM SET
    processingComp.set_pJecIn_parameters(pProxyComponent, &ps_ddep_sec_jpeg_param_desc_dh->host_param_set.s_in_parameters);
    // Set JPEG Parameter Other and header
	set_pJecOther_parameters(pProxyComponent, &ps_ddep_sec_jpeg_param_desc_dh->host_param_set, header_size_in_bytes, headerBufferPhysicalAddress, thumbnailImageBufferAddress, runLevelBufferPhysicalAddress);
	ps_ddep_sec_jpeg_param_desc_dh->host_param_set_atomic_exit = atomic_write;
	if(isNewParam) 
		iDdep.setParameter(mChannelId,ID_SEC_JPEG,mParamBufferDesc);//ER 334473 Resolution
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : configureAlgo DONE <line no %d> ",__LINE__);
    return OMX_ErrorNone;
}



OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::applyConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure)
{
	if(!mpc_vfm_mem_ctxt)
	{
		OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : Creating Memory List in applyConfig <line no %d> ",__LINE__);
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		mpc_vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
	}

               if(pProxyComponent->mParam.ConfigApplicable.get())
			   {
				   OstTraceFiltInst1(TRACE_FLOW, "In JPEGEnc_NmfMpc_ProcessingComponent : In applyConfig Calling configureAlgo <line no %d> ",__LINE__);
				   processingComp.compute_quantization_table(pProxyComponent);
				   JPEGheaderCreation(pProxyComponent);
				   configureAlgo();
			   }
			   pProxyComponent->mParam.ConfigApplicable.reset();
			   OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : applyConfig DONE <line no %d> ",__LINE__);
               return OMX_ErrorNone;
}

void JPEGEnc_NmfMpc_ProcessingComponent::JPEGheaderCreation(JPEGEnc_Proxy *jpegenc)
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In JPEGheaderCreation <line no %d> ",__LINE__);
	/* Start of Image
	   -------------- */
	t_cm_system_address sysAddr;
	sysAddr.logical = (t_uint32 )mHeaderBufferDesc.nLogicalAddress;
	DBC_ASSERT(sysAddr.logical);
	processingComp.headerCreation(jpegenc,(OMX_U8 **)&(sysAddr.logical),&(mHeaderBufferDesc.nSize));
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent :  JPEGheaderCreation DONE <line no %d> ",__LINE__);
}

void JPEGEnc_NmfMpc_ProcessingComponent::set_pJecOther_parameters(JPEGEnc_Proxy *jpegenc, ts_ddep_sec_jpeg_param_desc_ *ps_ddep_sec_jpeg_param_desc, t_uint32 header_size_in_bytes, t_uint8 *headerBufferAddress, t_uint8* thumbnailImageBufferAddress, t_uint8* runLevelBufferAddress)
{
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In set_pJecOther_parameters  <line no %d> ",__LINE__);
	t_cm_system_address sysAddr;
	sysAddr.logical = (t_uint32 )mHeaderBufferDesc.nLogicalAddress;
	DBC_ASSERT(sysAddr.logical);

	// copy the header already computed by the proxy in the VFM
	/* FIXME to remove memcpy */
	//ps_ddep_sec_jpeg_param_desc->s_header_buf.addr_header_buffer = ENDIANESS_CONVERSION((t_ahb_address )headerBufferAddress);

	if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingJPEG)
	{
	    memcpy((t_uint8 *)&ps_ddep_sec_jpeg_param_desc->header_buffer, (t_uint8 *)sysAddr.logical, header_size_in_bytes);
		ps_ddep_sec_jpeg_param_desc->s_header_buf.header_size = ENDIANESS_CONVERSION(header_size_in_bytes*8); //size in bits!
	}

	if(pProxyComponent->mParam.outputCompressionFormat == OMX_IMAGE_CodingEXIF)
	{
			ps_ddep_sec_jpeg_param_desc->s_header_buf.header_size = ENDIANESS_CONVERSION((mHeaderBufferDesc.nSize + EXIF_HEADER)*8); //size in bits!
	}
	OstTraceFiltInst2(TRACE_FLOW, "In JPEGEnc_NmfMpc_ProcessingComponent : In set_pJecOther_parameters header Size : 0x%x <line no %d> ",ps_ddep_sec_jpeg_param_desc->s_header_buf.header_size,__LINE__);
	ps_ddep_sec_jpeg_param_desc->s_header_buf.addr_header_buffer = ENDIANESS_CONVERSION((t_ahb_address )headerBufferAddress);
	//ps_ddep_sec_jpeg_param_desc->s_header_buf.header_size = ENDIANESS_CONVERSION(header_size_in_bytes*8); //size in bits!
	ps_ddep_sec_jpeg_param_desc->s_out_fram_buffer.addr_dest_buffer = ENDIANESS_CONVERSION((t_ahb_address)thumbnailImageBufferAddress);
	ps_ddep_sec_jpeg_param_desc->s_internal_buffer.addr_jpeg_run_level_buffer = ENDIANESS_CONVERSION((t_ahb_address)runLevelBufferAddress);
	ps_ddep_sec_jpeg_param_desc->s_in_out_frame_parameters.restart_mcu_count = jpegenc->mParam.getRestartInterval();
	ps_ddep_sec_jpeg_param_desc->s_ddep_in_param.nSliceHeight = ENDIANESS_CONVERSION(jpegenc->getSliceHeight());
	ps_ddep_sec_jpeg_param_desc->s_ddep_in_param.nStride = ENDIANESS_CONVERSION(jpegenc->getStride());
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : set_pJecOther_parameters DONE <line no %d> ",__LINE__);
}


OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "JPEGEnc_NmfMpc_ProcessingComponent : In emptyThisBuffer pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);

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
		OstTraceFiltInst1(TRACE_FLOW, "In JPEGEnc_NmfMpc_ProcessingComponent : In emptyThisBuffer calling addEXIFHeader <line no %d> ",__LINE__);
		processingComp.addEXIFHeader(pProxyComponent,pBuffer,(OMX_U8 **)&(sysAddr1.logical),(OMX_U8 **)&(sysAddr2.logical),
		                             &(mEXIFHeaderBufferDesc.nSize),mHeaderBufferDesc.nSize);
	}
#ifdef _CACHE_OPT_
	VFM_CacheClean(mpc_vfm_mem_ctxt, pBuffer->pBuffer, pBuffer->nFilledLen);
#endif
OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : emptyThisBuffer DONE <line no %d> ",__LINE__);
	return NmfMpc_ProcessingComponent::emptyThisBuffer(pBuffer);
}


OMX_ERRORTYPE JPEGEnc_NmfMpc_ProcessingComponent::errorRecoveryDestroyAll(){
	OstTraceFiltInst1(TRACE_API, "In JPEGEnc_NmfMpc_ProcessingComponent : In errorRecoveryDestroyAll <line no %d> ",__LINE__);
	stop();;
	return deInstantiate();
}


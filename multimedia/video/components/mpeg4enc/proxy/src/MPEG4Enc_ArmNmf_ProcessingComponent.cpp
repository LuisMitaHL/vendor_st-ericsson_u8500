/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_mpeg4enc
#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4enc_proxy_src_MPEG4Enc_ArmNmf_ProcessingComponentTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

#include "VFM_Port.h"
#include "VFM_Memory.h"
#include "VFM_DDepUtility.h"
#include "MPEG4Enc_Proxy.h"
#include "MPEG4Enc_Port.h"
#include <cm/inc/cm_macros.h>
#include "host/eventhandler.hpp"
#include "SharedBuffer.h"
#include <stdio.h>

#ifdef MMDSP_PERF
	#include "osi_perf.h"
#endif

#define DEBUG_MEM_SIZE (64*1024 - 1)
#define BUFFER_LINKLIST_SIZE (1024)
#define SEARCH_WINDOW_SIZE_IN_BYTES (0xa000)
#define HEADER_BUFFER_SIZE (0x38)

#define MAX_VBV_OCCUPANCY (63488)

/* +Change for 372717 */
#define ConvertMult16(x) ((x%16)?(x/16 + 1)*16 : x )
/* -Change for 372717 */

//>#define MV_FIELD_BUFFER_SIZE_IN_BYTES (0x6000)

//>volatile ts_ddep_buffer_descriptor g_mDebugBufferDescmpeg4enc1;

#ifndef CIF_WIDTH
	#define CIF_WIDTH 352
#endif
#ifndef CIF_HEIGHT
	#define CIF_HEIGHT 288
#endif

#ifndef QCIF_WIDTH
	#define QCIF_WIDTH 176
#endif
#ifndef QCIF_HEIGHT
	#define QCIF_HEIGHT 144
#endif

#ifndef VGA_WIDTH
	#define VGA_WIDTH 640
#endif
#ifndef VGA_HEIGHT
	#define VGA_HEIGHT 480
#endif


#define ONE_BIT 1
#define TWO_BITS 2
#define THREE_BITS 3
#define FOUR_BITS 4
#define EIGHT_BITS 8
#define ELEVEN_BITS 11
#define THIRTEEN_BITS 13
#define FIFTEEN_BITS 15
#define SIXTEEN_BITS 16
#define THIRTY_TWO_BITS 32


#define VOS_START_CODE 0x000001b0
#define VO_START_CODE 0x000001b5
#define VO_CODE 0x00000100
#define VOL_CODE 0x00000120
#define VOP_CODE 0x000001b6
#define VOS_END_CODE 0x000001b1

#define SVA_RTYPE_MODE_CONSTANT_ZERO 0
#define SVA_RTYPE_MODE_CONSTANT_ONE 1

typedef enum
{
	SVA_SPATIAL_QUALITY_NONE,
	SVA_SPATIAL_QUALITY_LOW,
	SVA_SPATIAL_QUALITY_MEDIUM,
	SVA_SPATIAL_QUALITY_HIGH
} t_sva_brc_spatial_quality;

typedef struct
{
	t_uint64 buffer;
	t_uint32 nbBitsValid;
	t_uint8 *currBuffer;/*destination buffer*/
	t_uint8 *endbuffer;
	t_uint32 totalBitsWritten;/*total bits written*/
	//FILE *fout;
}VFM_WRITESTREAM_HEADER;

MPEG4Enc_ArmNmf_ProcessingComponent::MPEG4Enc_ArmNmf_ProcessingComponent(ENS_Component &enscomp): VFM_NmfHost_ProcessingComponent(enscomp)
{
	OstTraceInt0(TRACE_API, "=> MPEG4Enc_ArmNmf_ProcessingComponent::MPEG4Enc_ArmNmf_ProcessingComponent() constructor");

	//iDdep.THIS = 0;
	//iDdep.setConfig = 0;
	//iDdep.setNeeds = 0;
	//iDdep.setParameter = 0;

	pProxyComponent = (MPEG4Enc_Proxy *)0;
	//> mChannelId = 0;
	//> ddHandle = VFM_INVALID_CM_HANDLE;
	//> brcHandle = VFM_INVALID_CM_HANDLE;
	//> mNmfAlgoHandle = VFM_INVALID_CM_HANDLE;
	//> mNmfRmHandle = VFM_INVALID_CM_HANDLE;

	//> mBufferingModel = SVA_BUFFERING_NONE;

	mParamBufferDesc.nSize = 0;
	mParamBufferDesc.nMpcAddress = 0;
	mParamBufferDesc.nLogicalAddress = 0;
	mParamBufferDesc.nPhysicalAddress = 0;

	mInternalBufferDesc.nSize = 0;
	mInternalBufferDesc.nMpcAddress = 0;
	mInternalBufferDesc.nLogicalAddress = 0;
	mInternalBufferDesc.nPhysicalAddress = 0;

	mLinkListBufferDesc.nSize = 0;
	mLinkListBufferDesc.nMpcAddress = 0;
	mLinkListBufferDesc.nLogicalAddress = 0;
	mLinkListBufferDesc.nPhysicalAddress = 0;

	//> mDebugBufferDesc.nSize = 0;
	//> mDebugBufferDesc.nMpcAddress = 0;
	//> mDebugBufferDesc.nLogicalAddress = 0;
	//> mDebugBufferDesc.nPhysicalAddress = 0;

	vfm_mem_ctxt = 0;

	//> mSwisBufSize = 0;
	//> mVbvBufferSize = 0;
	//> mVbvOccupancy = 0;

	OstTraceInt0(TRACE_API, "<= MPEG4Enc_ArmNmf_ProcessingComponent::MPEG4Enc_ArmNmf_ProcessingComponent()");
}

MPEG4Enc_ArmNmf_ProcessingComponent::~MPEG4Enc_ArmNmf_ProcessingComponent()
{
	OstTraceFiltStatic0(TRACE_FLOW, "MPEG4ENC_ARM_MPC : Inside destructor of MPEG4Enc_ArmNmf_ProcessingComponent ",pProxyComponent);

	if(mParamBufferDesc.nLogicalAddress)
	{
		OstTraceFiltInst0(TRACE_FLOW, "MPEG4ENC_ARM_MPC : Freeing memory for mParamBufferDesc \n");
		VFM_Free(vfm_mem_ctxt, (t_uint8*)mParamBufferDesc.nLogicalAddress);
		mParamBufferDesc.nSize = 0;
		mParamBufferDesc.nLogicalAddress = 0;
		mParamBufferDesc.nPhysicalAddress = 0;
		pProxyComponent->pFwPerfDataPtr = (OMX_PTR)0;
	}


	if(mInternalBufferDesc.nLogicalAddress)
	{
		OstTraceFiltInst0(TRACE_FLOW, "MPEG4ENC_ARM_MPC : Freeing memory for mInternalBufferDesc \n");
		VFM_Free(vfm_mem_ctxt, (t_uint8*)mInternalBufferDesc.nLogicalAddress);
		mInternalBufferDesc.nSize = 0;
		mInternalBufferDesc.nLogicalAddress = 0;
		mInternalBufferDesc.nPhysicalAddress = 0;
	}

/*	if(mDebugBufferDesc.nLogicalAddress)
	{
		VFM_Free(vfm_mem_ctxt, (t_uint8*)mDebugBufferDesc.nLogicalAddress);
		mDebugBufferDesc.nSize = 0;
		mDebugBufferDesc.nLogicalAddress = 0;
		mDebugBufferDesc.nPhysicalAddress = 0;
	}
*/
	if(mLinkListBufferDesc.nLogicalAddress)
	{
		OstTraceFiltInst0(TRACE_FLOW, "MPEG4ENC_ARM_MPC : Freeing memory for mLinkListBufferDesc \n");
		VFM_Free(vfm_mem_ctxt, (t_uint8*)mLinkListBufferDesc.nLogicalAddress);
		mLinkListBufferDesc.nSize = 0;
		mLinkListBufferDesc.nLogicalAddress = 0;
		mLinkListBufferDesc.nPhysicalAddress = 0;
	}

	if(vfm_mem_ctxt)
	{
		VFM_CloseMemoryList(vfm_mem_ctxt);
	}

	vfm_mem_ctxt = 0;
}


OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::allocateBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_U32 nSizeBytes,
                OMX_U8 **ppData,
                void **bufferAllocInfo,
                void **portPrivateInfo)
{
	//OstTraceFunctionEntryExt(ALLOCATEBUFFER_ENTRY, this);
	OMX_ERRORTYPE error;
	error = allocateBufferVisual(nPortIndex, nBufferIndex,  nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo,OMX_TRUE);
	if (error != OMX_ErrorNone)
	{
		OstTraceInt2(TRACE_ERROR, "MPEG4ENC_ARM_MPC : Reutning Error : 0x%x for Size : %d in allocateBuffer \n",error,nSizeBytes);
		return error;
	}

	//OstTraceFunctionExitExt(ALLOCATEBUFFER_EXIT, this, -1);
    return OMX_ErrorNone;
}


void MPEG4Enc_ArmNmf_ProcessingComponent::set_pMecOther_parameters(ts_ddep_vec_mpeg4_param_desc *ps_ddep_vec_mpeg4_param_desc,
																   t_uint32 searchwinsize,
																   t_uint32 mvfieldbuffersize,
																   t_uint8 *headerBufferAddress,
																   t_uint8* fwdRefBufferAddress,
																   t_uint8* destBufferAddress,
																   t_uint8* intraRefBufferAddress_in,
																   t_uint8* intraRefBufferAddress_out,
																   t_uint8* motionVectorBufferAddress,
																   t_uint8* searchWindowBufferAddress,
																   t_uint8* mvFieldBufferAddress
																  )
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ArmNmf_ProcessingComponent::set_pMecOther_parameters()");

	ps_ddep_vec_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer = (t_ahb_address)fwdRefBufferAddress;
	ps_ddep_vec_mpeg4_param_desc->s_in_frame_buffer.addr_grab_ref_buffer = (t_ahb_address)(0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_in_frame_buffer.addr_intra_refresh_buffer = ((t_ahb_address)intraRefBufferAddress_in);

	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer = ((t_ahb_address)destBufferAddress);
	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = ((t_ahb_address)(0x00000000));
	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_motion_vector_buffer = ((t_ahb_address)motionVectorBufferAddress);
	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_intra_refresh_buffer = ((t_ahb_address)intraRefBufferAddress_out);

	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_ime_mv_field_buffer = ((t_ahb_address)mvFieldBufferAddress);
	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_ime_mv_field_buffer_end = (((t_ahb_address)mvFieldBufferAddress) + mvfieldbuffersize);

	//< ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_search_window_buffer = ENDIANESS_CONVERSION((t_uint8 *)searchWindowBufferPhysicalAddress);
	//< ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_search_window_end = ENDIANESS_CONVERSION(((t_uint8 *)searchWindowBufferPhysicalAddress) + searchwinsize);

	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_search_window_buffer = (0x00000000);	//< FIXME
	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_search_window_end = (0x00000000);

	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_jpeg_run_level_buffer = (0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_h264e_H4D_buffer = (0x00000000);

	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_h264e_rec_local = (0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_h264e_metrics = (0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_h264e_cup_context =  (0x00000000);

	ps_ddep_vec_mpeg4_param_desc->s_header_buf.addr_header_buffer = ((t_ahb_address)headerBufferAddress);
	ps_ddep_vec_mpeg4_param_desc->s_header_buf.reserved_1 = (0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_header_buf.reserved_2 = (0x00000000);

/*
	ps_ddep_vec_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer = ENDIANESS_CONVERSION((t_uint8 *)fwdRefBufferPhysicalAddress);
	ps_ddep_vec_mpeg4_param_desc->s_in_frame_buffer.addr_grab_ref_buffer = ENDIANESS_CONVERSION(0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_in_frame_buffer.addr_intra_refresh_buffer = ENDIANESS_CONVERSION((t_uint8 *)intraRefBufferPhysicalAddress_in);

	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer = ENDIANESS_CONVERSION((t_uint8 *)destBufferPhysicalAddress);
	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = ENDIANESS_CONVERSION(0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_motion_vector_buffer = ENDIANESS_CONVERSION((t_uint8 *)motionVectorBufferPhysicalAddress);
	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_intra_refresh_buffer = ENDIANESS_CONVERSION((t_uint8 *)intraRefBufferPhysicalAddress_out);

	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_ime_mv_field_buffer = ENDIANESS_CONVERSION((t_uint8 *)mvFieldBufferPhysicalAddress);
	ps_ddep_vec_mpeg4_param_desc->s_out_fram_buffer.addr_ime_mv_field_buffer_end = ENDIANESS_CONVERSION(((t_uint8 *)mvFieldBufferPhysicalAddress) + mvfieldbuffersize);

	//< ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_search_window_buffer = ENDIANESS_CONVERSION((t_uint8 *)searchWindowBufferPhysicalAddress);
	//< ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_search_window_end = ENDIANESS_CONVERSION(((t_uint8 *)searchWindowBufferPhysicalAddress) + searchwinsize);

	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_search_window_buffer = ENDIANESS_CONVERSION(0x00000000);	//< FIXME
	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_search_window_end = ENDIANESS_CONVERSION(0x00000000);

	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_jpeg_run_level_buffer = ENDIANESS_CONVERSION(0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_h264e_H4D_buffer = ENDIANESS_CONVERSION(0x00000000);

	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_h264e_rec_local = ENDIANESS_CONVERSION(0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_h264e_metrics = ENDIANESS_CONVERSION(0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_internal_buffer.addr_h264e_cup_context =  ENDIANESS_CONVERSION(0x00000000);

	ps_ddep_vec_mpeg4_param_desc->s_header_buf.addr_header_buffer = ENDIANESS_CONVERSION((t_uint8 *)headerBufferPhysicalAddress);
	ps_ddep_vec_mpeg4_param_desc->s_header_buf.reserved_1 = ENDIANESS_CONVERSION(0x00000000);
	ps_ddep_vec_mpeg4_param_desc->s_header_buf.reserved_2 = ENDIANESS_CONVERSION(0x00000000);
*/
	//OstTraceFunctionExitExt(MPEG4ENC_NMF_SETMECOTHER_EXIT, this, -1);
}


OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler)
{
	deferEventHandler = OMX_FALSE;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
#if 0
	switch(eCmd)
	{
		case OMX_CommandStateSet:
			switch(nData)
			{
				case OMX_StateExecuting:
				case OMX_StatePause:
				case OMX_StateIdle:
					break;
				default:

					return OMX_ErrorNotImplemented;
			}
			break;
		case OMX_CommandPortDisable:
			switch(nData)
			{
				case 0:
					vos_written = NOT_WRITTEN;	// If port 0 is disabled, then VOS+VO+VOL headers must be resent
					pProxyComponent->mSendParamToArmNmf.set(); // The parameters must be set again
					break;
				case 1:
					//< vos_written = OMX_FALSE;
					break;
				default:
					return OMX_ErrorBadPortIndex;
			}
			break;
		case OMX_CommandPortEnable:
			switch(nData)
			{
				case 0:
				case 1:
				case OMX_ALL:
					break;
				default:
					return OMX_ErrorBadPortIndex;
			}
			break;
		case OMX_CommandFlush:
			switch(nData)
			{
				case 0:
				case 1:
					break;
				default:
					return OMX_ErrorBadPortIndex;
			}
			break;
		default:
			return OMX_ErrorNotImplemented;
	}

	if((eCmd == OMX_CommandPortEnable) && pProxyComponent->mSendParamToArmNmf.get())
	{
		configure();
	}
#endif
	bDeferredCmd = OMX_FALSE;
	return OMX_ErrorNone;
}

void MPEG4Enc_ArmNmf_ProcessingComponent::doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
#if 0
	OMX_U32 filledLen;
	OMX_U32 filledLen_bits;
	t_uint8* bufferptr;

#ifdef MMDSP_PERF
	write_ticks_to_buffer("PFBDI");
#endif

	filledLen = pBuffer->nFilledLen;

	OstTraceFiltInst3(TRACE_FLOW, "MPEG4ENC_ARM_MPC : In fillbufferDone pBuffer->pBuffer : 0x%x  pBuffer->nFilledLen : %d pBuffer->nFlags : %d \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
	if(((pProxyComponent->mParam.m_enc_param.bSVH) == OMX_FALSE) && (vos_written == NOT_WRITTEN))
	{
		vos_written = WRITTEN_ONCE;
		bufferptr = (t_uint8 *)(pBuffer->pBuffer+pBuffer->nOffset);
		filledLen_bits = processingComp.Write_VOS_VO_VOL(pProxyComponent, bufferptr);
		pBuffer->nFilledLen = filledLen_bits/8;
		filledLenLastVOL = pBuffer->nFilledLen;		//stores the filledLength to act as offset for the bitstream buffer for frame 1
		pBuffer->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;	//OMX 1.1.2 only, first buffer only has config data.
	}
	else if(((pProxyComponent->mParam.m_enc_param.bSVH) == OMX_FALSE) && (vos_written == WRITTEN_ONCE)) //when VOS header has been written again by Ddep
	{
		if( pBuffer->nFilledLen != 0)	//only if the frame has not been skipped
		{
			vos_written = WRITTEN_TWICE;
			//change offset and nFilledLen
			pBuffer->nOffset = filledLenLastVOL;
			pBuffer->nFilledLen = pBuffer->nFilledLen - filledLenLastVOL;
			filledLen = pBuffer->nFilledLen;	//checkme, is this really required?
		}
	}

	if(/*(pProxyComponent->mParam.m_vos_header.isSystemHeaderAddBeforeIntra == OMX_TRUE) && */(pBuffer->nFlags & OMX_BUFFERFLAG_EOS))
	{
		if(pProxyComponent->mParam.m_enc_param.bSVH == OMX_FALSE)
		{
			// if 4 bytes space is available in current buffer then, insert vos end code, else don't
			if(filledLen <= (pBuffer->nAllocLen - 4))
			{
				bufferptr = (t_uint8 *)(pBuffer->pBuffer+pBuffer->nOffset);
				bufferptr = bufferptr + filledLen;

				*bufferptr = 0x00; bufferptr++;	//SP end of stream code
				*bufferptr = 0x00; bufferptr++;
				*bufferptr = 0x01; bufferptr++;
				*bufferptr = 0xb1; bufferptr++;

				filledLen = filledLen + 4;
				pBuffer->nFilledLen = filledLen;
			}

		}
	}
#ifdef MMDSP_PERF
	write_ticks_to_buffer("PFBDO");
#endif

#endif

}

void MPEG4Enc_ArmNmf_ProcessingComponent::doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_FLOW, "MPEG4ENC_ARM_MPC : In doSpecificEmptyBufferDone_cb pBuffer->pBuffer : 0x%x  pBuffer->nFilledLen : %d pBuffer->nFlags : %d \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
	//OstTraceFunctionEntryExt(MPEG4ENC_NMF_EMPTYTHISBUFFER_ENTRY, this);
	//OstTtraraceExt2(TRACE_NORMAL,MPEG4ENC_NMF_EMPTYTHISBUFFER_FILLLEN_FLAGS, "EBD pBuffer->nFilledLen = %x, pBuffer->nFlags = %d\n", pBuffer->nFilledLen,  pBuffer->nFlags);
	//OstTraceFiltInst2(TRACE_FLOW, "MPEG4ENC_NMF_MPC : In fillbuffer done nFilledLen %d and nFlags %x \n", pBuffer->nFilledLen,pBuffer->nFlags);
	//OstTraceFunctionExitExt(MPEG4ENC_NMF_EMPTYTHISBUFFER_EXIT, this, -1);
	return VFM_NmfHost_ProcessingComponent::doSpecificEmptyBufferDone_cb(pBuffer);
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::codecInstantiate()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ArmNmf_ProcessingComponent::codecInstantiate()");

	t_nmf_error error ;
	//OMX_HANDLETYPE OMXHandle = mENSComponent.getOMXHandle();

    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR(createPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport"));

	pProxyComponent = (MPEG4Enc_Proxy *)(&mENSComponent);

	error = mCodec->bindFromUser("setParam",1,&setparamitf) ;
	if (error != NMF_OK)
	{
		OstTraceInt1(TRACE_ERROR, "MPEG4ENC_ARM_MPC : Error while binding setParameter interface at line no : %d \n",__LINE__);
		return OMX_ErrorUndefined ;
	}

	error = mCodec->bindFromUser("setCropForStab",1,&setcropstabitf) ;
	if (error != NMF_OK)
	{
		OstTraceInt1(TRACE_ERROR, "MPEG4ENC_ARM_MPC : Error while binding setcropstabitf interface at line no : %d \n",__LINE__);
		return OMX_ErrorUndefined ;
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ArmNmf_ProcessingComponent::codecInstantiate()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::codecStart()
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::codecStop()
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::codecConfigure()
{
	IN0("");

    OUT0("");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::codecCreate(OMX_U32 domainId)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;

	mCodec = mpeg4enc_arm_nmf_mpeg4enc_swCreate();
	if(!mCodec)
	{
		error = OMX_ErrorUndefined;
		OstTraceInt1(TRACE_ERROR, "MPEG4ENC_ARM_MPC : Error in CodecCreate at line no : %d \n",__LINE__);
	}

	return error;

}

void MPEG4Enc_ArmNmf_ProcessingComponent::codecDestroy()
{
	mpeg4enc_arm_nmf_mpeg4enc_swDestroy((mpeg4enc_arm_nmf_mpeg4enc_sw *&)mCodec);
}


OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::configure()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ArmNmf_ProcessingComponent::configure()");

	OMX_ERRORTYPE error;

	if(pProxyComponent->mSendParamToArmNmf.get())
	{
		error = configureAlgo();

		OstTraceFiltInst1(TRACE_FLOW, "MPEG4ENC_ARM_MPC : Return value of configureAlgo() API : 0x%x \n",error);

		DBC_ASSERT(error==OMX_ErrorNone);

		setparamitf.setParameter(ID_VEC_MPEG4,mParamBufferDesc);

		//Pass LinkList desc to DataDep
		//>iDdep.setNeeds(mChannelId,mLinkListBufferDesc,mDebugBufferDesc);
		//Pass Param desc to DataDep
		//>iDdep.setParameter(mChannelId,ID_VEC_MPEG4,mParamBufferDesc);

		pProxyComponent->mSendParamToArmNmf.reset();
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ArmNmf_ProcessingComponent::configure()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::configureAlgo()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ArmNmf_ProcessingComponent::configureAlgo()");

	t_uint8 *ptr, *physicalptr;

	t_bool zero_param_desc_dh=0;

	t_uint32 param_buffer_size=0, internal_buffer_size=0;
	t_uint16 width=0, height=0;

	t_uint8 *headerBufferAddress=NULL, *headerBufferPhysicalAddress=NULL;

	t_uint8 *fwdRefBufferAddress=NULL, *fwdRefBufferPhysicalAddress=NULL;
	t_uint8 *destBufferAddress=NULL, *destBufferPhysicalAddress=NULL;
	t_uint8 *intraRefBufferAddress_in=NULL, *intraRefBufferPhysicalAddress_in=NULL;
	t_uint8 *intraRefBufferAddress_out=NULL, *intraRefBufferPhysicalAddress_out=NULL;
	t_uint8 *motionVectorBufferAddress=NULL, *motionVectorBufferPhysicalAddress=NULL;
	t_uint8 *searchWindowBufferAddress=NULL, *searchWindowBufferPhysicalAddress=NULL;
	t_uint8 *mvFieldBufferAddress=NULL, *mvFieldBufferPhysicalAddress=NULL;


	ts_ddep_vec_mpeg4_param_desc_dh *ps_ddep_vec_mpeg4_param_desc_dh = (ts_ddep_vec_mpeg4_param_desc_dh *)0;

	width = pProxyComponent->getFrameWidth(1);
	height = pProxyComponent->getFrameHeight(1);

	t_uint32 fwdrefsize = ((((width*height)*3/2)/256+1)*256)+ VFM_IMAGE_BUFFER_ALIGN_MASK;
	t_uint32 destbufsize = ((((width*height)*3/2)/256+1)*256)+ VFM_IMAGE_BUFFER_ALIGN_MASK;
	t_uint32 intrefsize = ((((width/16)*(height/16)*2)/256+1)*256) + VFM_IMAGE_BUFFER_ALIGN_MASK;
	//> t_uint32 motionvecsize = ((((width/16+2)*(height/16+2)*4)/256+1)*256) + VFM_IMAGE_BUFFER_ALIGN_MASK;
	t_uint32 motionvecsize = ((((width/16+2)*(height/16+2)*2*sizeof(motion_vector))/256+1)*256) + VFM_IMAGE_BUFFER_ALIGN_MASK;
	t_uint32 searchwinsize = SEARCH_WINDOW_SIZE_IN_BYTES;

	t_uint32 mvfieldbuffersize = ((((width/16)*(height/16)*16)/256+1)*256) + VFM_IMAGE_BUFFER_ALIGN_MASK; //>MV_FIELD_BUFFER_SIZE_IN_BYTES;

	param_buffer_size = sizeof(ts_ddep_vec_mpeg4_param_desc_dh)
						+ HEADER_BUFFER_SIZE
						+ VFM_IMAGE_BUFFER_ALIGN_MASK
						+ VFM_ADDRESS_ALIGN_MASK
						+ 5*0xf; //for alignment

	internal_buffer_size = fwdrefsize
						   + destbufsize
						   + 2*intrefsize	  //allocated twice, one for buf_in one for buf_out
						   + motionvecsize
						   + mvfieldbuffersize
						   + searchwinsize
						   + VFM_IMAGE_BUFFER_ALIGN_MASK
						   + VFM_ADDRESS_ALIGN_MASK
						   + 5*0xf;	   //for alignment

#if 1 	//< experimental changes
	if(!vfm_mem_ctxt)
	{
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo, ddrDomainId, 0/*pass 0 as it is not full software*/);
	}
	t_uint8* param_buf_logical_addr = 0;
	t_uint8* internal_buf_logical_addr = 0;
	t_uint8* linklist_buf_logical_addr = 0;
	{
		if((mParamBufferDesc.nSize || mInternalBufferDesc.nSize/*internal_buffer_PoolId*/)&& pProxyComponent->mSendParamToArmNmf.get())
		{
			if(mParamBufferDesc.nSize)
			{
				param_buf_logical_addr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
				VFM_Free(vfm_mem_ctxt, param_buf_logical_addr);
				mParamBufferDesc.nSize = 0;
				pProxyComponent->pFwPerfDataPtr = (OMX_PTR)0;
			}
			if(mInternalBufferDesc.nSize /*internal_buffer_PoolId*/)
			{
				internal_buf_logical_addr = (t_uint8*)mInternalBufferDesc.nLogicalAddress;
				VFM_Free(vfm_mem_ctxt, internal_buf_logical_addr);
				mInternalBufferDesc.nSize = 0;
			}
		}

		if(!mParamBufferDesc.nSize)
		{
			param_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, param_buffer_size, VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

			OstTraceFiltInst2(TRACE_FLOW, "MPEG4ENC_ARM_MPC : Memory allocation for param_buf_logical_addr Addr : 0x%x and size : %d bytes\n",(OMX_U32)param_buf_logical_addr,param_buffer_size);
			//> param_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, param_buffer_size, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

			if(!param_buf_logical_addr)
			{
				//> VFM_Free(vfm_mem_ctxt, param_buf_logical_addr);
				vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "MPEG4ENC_ARM_MPC : OMX_ErrorInsufficientResources Memory allocation for linklist_buf_logical_addr \n");
				return OMX_ErrorInsufficientResources;
			}
			mParamBufferDesc.nSize = param_buffer_size;
			mParamBufferDesc.nLogicalAddress = (t_uint32)param_buf_logical_addr;
			mParamBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, param_buf_logical_addr));
			mParamBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(vfm_mem_ctxt, param_buf_logical_addr));

			zero_param_desc_dh = 1;
		}

		ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
		physicalptr = (t_uint8*)mParamBufferDesc.nPhysicalAddress;

		if(!mInternalBufferDesc.nSize)
		{
			internal_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, internal_buffer_size, VFM_MEM_CACHED_HWBUFFER_NONDSP, 256, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			//> internal_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, internal_buffer_size, VFM_MEM_CACHED_HWBUFFER_NONDSP, 256, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

			if(!internal_buf_logical_addr)
			{
				//< not able to allocate internal buffer, so dellocate previously allocated
				//< buffers for a clean graceful exit
				{
					VFM_Free(vfm_mem_ctxt, param_buf_logical_addr);
					mParamBufferDesc.nSize = 0;
					mParamBufferDesc.nLogicalAddress = 0;
					mParamBufferDesc.nPhysicalAddress = 0;
				}

				vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "MPEG4ENC_ARM_MPC : OMX_ErrorInsufficientResources Memory allocation for internal_buf_logical_addr \n");
				return OMX_ErrorInsufficientResources;
			}

			mInternalBufferDesc.nSize = internal_buffer_size;
			mInternalBufferDesc.nLogicalAddress = (t_uint32)internal_buf_logical_addr;
			mInternalBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, internal_buf_logical_addr));
			mInternalBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(vfm_mem_ctxt, internal_buf_logical_addr));

			//< it is necessary to set the allocated buffer to 0, as mvbuffer/search window buffer,
			//< which are part of the allocated internal buffer assume it that way
			memset((t_uint8 *)(internal_buf_logical_addr), 0x0, internal_buffer_size);
		}

		if(!mLinkListBufferDesc.nSize)
		{
			linklist_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, BUFFER_LINKLIST_SIZE, VFM_MEM_CACHED_HWBUFFER_NONDSP, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			OstTraceFiltInst2(TRACE_FLOW, "MPEG4ENC_ARM_MPC : Memory allocation for linklist_buf_logical_addr addr : 0x%x and size : %d bytes\n",(OMX_U32)linklist_buf_logical_addr,BUFFER_LINKLIST_SIZE);
			//> linklist_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, BUFFER_LINKLIST_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

			if(!linklist_buf_logical_addr)
			{
				//< not able to allocate linklist buffer, so dellocate previously allocated
				//< buffers for a clean graceful exit
				{
					VFM_Free(vfm_mem_ctxt, internal_buf_logical_addr);
					mInternalBufferDesc.nSize = 0;
					mInternalBufferDesc.nLogicalAddress = 0;
					mInternalBufferDesc.nPhysicalAddress = 0;
				}
				{
					VFM_Free(vfm_mem_ctxt, param_buf_logical_addr);
					mParamBufferDesc.nSize = 0;
					mParamBufferDesc.nLogicalAddress = 0;
					mParamBufferDesc.nPhysicalAddress = 0;
				}

				vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "MPEG4ENC_ARM_MPC : OMX_ErrorInsufficientResources Memory allocation for linklist_buf_logical_addr \n");
				return OMX_ErrorInsufficientResources;
			}

			mLinkListBufferDesc.nSize = BUFFER_LINKLIST_SIZE;
			mLinkListBufferDesc.nLogicalAddress = (t_uint32)linklist_buf_logical_addr;
			mLinkListBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, linklist_buf_logical_addr));
			mLinkListBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(vfm_mem_ctxt, linklist_buf_logical_addr));

		}
	#if 0

		if (!mDebugBufferDesc.nSize) {
			debug_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, DEBUG_MEM_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);


			if (!debug_buf_logical_addr) {
				//< not able to allocate debug buffer, so dellocate previously allocated
				//< buffers for a clean graceful exit
				{
					VFM_Free(vfm_mem_ctxt, linklist_buf_logical_addr);
					mLinkListBufferDesc.nSize = 0;
					mLinkListBufferDesc.nLogicalAddress = 0;
					mLinkListBufferDesc.nPhysicalAddress = 0;
				}
				{
					VFM_Free(vfm_mem_ctxt, internal_buf_logical_addr);
					mInternalBufferDesc.nSize = 0;
					mInternalBufferDesc.nLogicalAddress = 0;
					mInternalBufferDesc.nPhysicalAddress = 0;
				}
				{
					VFM_Free(vfm_mem_ctxt, param_buf_logical_addr);
					mParamBufferDesc.nSize = 0;
					mParamBufferDesc.nLogicalAddress = 0;
					mParamBufferDesc.nPhysicalAddress = 0;
				}
				vfm_mem_ctxt = 0;
				return OMX_ErrorInsufficientResources;
			}

			mDebugBufferDesc.nSize = DEBUG_MEM_SIZE;
			mDebugBufferDesc.nLogicalAddress = (t_uint32)debug_buf_logical_addr;
			mDebugBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, debug_buf_logical_addr));
			mDebugBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(vfm_mem_ctxt, debug_buf_logical_addr));

			g_mDebugBufferDescmpeg4enc1.nSize =  mDebugBufferDesc.nSize;
			g_mDebugBufferDescmpeg4enc1.nLogicalAddress =  mDebugBufferDesc.nLogicalAddress;
			g_mDebugBufferDescmpeg4enc1.nPhysicalAddress =  mDebugBufferDesc.nPhysicalAddress;
			g_mDebugBufferDescmpeg4enc1.nMpcAddress =  mDebugBufferDesc.nMpcAddress;

		}

	#endif

	}

#endif

	ps_ddep_vec_mpeg4_param_desc_dh = (ts_ddep_vec_mpeg4_param_desc_dh *) ptr;

	ptr+=sizeof(ts_ddep_vec_mpeg4_param_desc_dh);
	physicalptr+=sizeof(ts_ddep_vec_mpeg4_param_desc_dh);

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
	physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		return OMX_ErrorUndefined;
	}

	headerBufferAddress = ptr;
	ptr = ptr + HEADER_BUFFER_SIZE + VFM_IMAGE_BUFFER_ALIGN_MASK;
	headerBufferPhysicalAddress = physicalptr;
	physicalptr = physicalptr + HEADER_BUFFER_SIZE + VFM_IMAGE_BUFFER_ALIGN_MASK;

	headerBufferAddress = (t_uint8 *)VFM_ROUND_UPPER(headerBufferAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
	headerBufferPhysicalAddress = (t_uint8 *)VFM_ROUND_UPPER(headerBufferPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
	physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		return OMX_ErrorUndefined;
	}

	ptr = (t_uint8*)mInternalBufferDesc.nLogicalAddress;
	physicalptr = (t_uint8*)mInternalBufferDesc.nPhysicalAddress;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
	physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);
	if(((t_uint32)ptr)&0xf)
	{
		return OMX_ErrorUndefined;
	}

	fwdRefBufferAddress = ptr;
	ptr = ptr + fwdrefsize;
	fwdRefBufferPhysicalAddress = physicalptr;
	physicalptr = physicalptr + fwdrefsize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
	physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		return OMX_ErrorUndefined;
	}
	destBufferAddress = ptr;
	ptr = ptr + destbufsize;
	destBufferPhysicalAddress = physicalptr;
	physicalptr = physicalptr + destbufsize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
	physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		return OMX_ErrorUndefined;
	}
	intraRefBufferAddress_in = ptr;
	ptr = ptr + intrefsize;
	intraRefBufferPhysicalAddress_in = physicalptr;
	physicalptr = physicalptr + intrefsize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
	physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		return OMX_ErrorUndefined;
	}
	intraRefBufferAddress_out = ptr;
	ptr = ptr + intrefsize;
	intraRefBufferPhysicalAddress_out = physicalptr;
	physicalptr = physicalptr + intrefsize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
	physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		return OMX_ErrorUndefined;
	}
	motionVectorBufferAddress = ptr;
	ptr = ptr + motionvecsize;
	motionVectorBufferPhysicalAddress = physicalptr;
	physicalptr = physicalptr + motionvecsize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
	physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		return OMX_ErrorUndefined;
	}

	mvFieldBufferAddress = ptr;
	ptr = ptr + mvfieldbuffersize;
	mvFieldBufferPhysicalAddress = physicalptr;
	physicalptr = physicalptr + mvfieldbuffersize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
	physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		return OMX_ErrorUndefined;
	}

	searchWindowBufferAddress = ptr;
	ptr = ptr + searchwinsize;
	searchWindowBufferPhysicalAddress = physicalptr;
	physicalptr = physicalptr + searchwinsize;

	ptr = (t_uint8*)(((t_uint32)ptr+0xf)&0xfffffff0);
	physicalptr = (t_uint8*)(((t_uint32)physicalptr+0xf)&0xfffffff0);

	if(((t_uint32)ptr)&0xf)
	{
		return OMX_ErrorUndefined;
	}

	fwdRefBufferAddress = (t_uint8 *)VFM_ROUND_UPPER(fwdRefBufferAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
	fwdRefBufferPhysicalAddress = (t_uint8 *)VFM_ROUND_UPPER(fwdRefBufferPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
	intraRefBufferAddress_in = (t_uint8 *)VFM_ROUND_UPPER(intraRefBufferAddress_in, VFM_IMAGE_BUFFER_ALIGN_MASK);
	intraRefBufferPhysicalAddress_in = (t_uint8 *)VFM_ROUND_UPPER(intraRefBufferPhysicalAddress_in, VFM_IMAGE_BUFFER_ALIGN_MASK);

	motionVectorBufferAddress = (t_uint8 *)VFM_ROUND_UPPER(motionVectorBufferAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
	motionVectorBufferPhysicalAddress = (t_uint8 *)VFM_ROUND_UPPER(motionVectorBufferPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
	intraRefBufferAddress_out = (t_uint8 *)VFM_ROUND_UPPER(intraRefBufferAddress_out, VFM_IMAGE_BUFFER_ALIGN_MASK);
	intraRefBufferPhysicalAddress_out = (t_uint8 *)VFM_ROUND_UPPER(intraRefBufferPhysicalAddress_out, VFM_IMAGE_BUFFER_ALIGN_MASK);
	destBufferAddress = (t_uint8 *)VFM_ROUND_UPPER(destBufferAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
	destBufferPhysicalAddress = (t_uint8 *)VFM_ROUND_UPPER(destBufferPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	mvFieldBufferAddress = (t_uint8 *)VFM_ROUND_UPPER(mvFieldBufferAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
	mvFieldBufferPhysicalAddress = (t_uint8 *)VFM_ROUND_UPPER(mvFieldBufferPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	searchWindowBufferAddress = (t_uint8 *)VFM_ROUND_UPPER(searchWindowBufferAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);
	searchWindowBufferPhysicalAddress = (t_uint8 *)VFM_ROUND_UPPER(searchWindowBufferPhysicalAddress, VFM_IMAGE_BUFFER_ALIGN_MASK);

	t_uint16 atomic_write;

	if(zero_param_desc_dh)
	{
		//Zero memory
		memset((t_uint8 *)(ps_ddep_vec_mpeg4_param_desc_dh), 0x0, sizeof(ts_ddep_vec_mpeg4_param_desc_dh));

		// Set MPEG4 Parameter In: MPC PARAM SET (only once)
		processingComp.set_pMecIn_parameters(pProxyComponent, &ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set.s_in_parameters, &(ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set.s_in_custom_parameters));

		//< store the pointer to the struct containing the fw perf statistics.
		//< this pointer is used in MPEG4Enc_ArmNmf_ProcessingComponent::getPerfData to fill the
		//< user supplied struct with the fw perf stats, so that fw perf figures could be calculated

		pProxyComponent->pFwPerfDataPtr = (OMX_PTR)&(ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set.s_out_perf_parameters);
		// Set MPEG4 Parameter Other and header

		set_pMecOther_parameters(&ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set, searchwinsize, mvfieldbuffersize, headerBufferAddress, fwdRefBufferAddress, destBufferAddress, intraRefBufferAddress_in, intraRefBufferAddress_out, motionVectorBufferAddress, searchWindowBufferAddress, mvFieldBufferAddress);

		memcpy((t_uint8*)&ps_ddep_vec_mpeg4_param_desc_dh->host_param_set,
			   (t_uint8*)&ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set,
			   sizeof(ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set));

		atomic_write = 0;
	}
	else
		atomic_write = ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_entry + 1;

	ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_entry = atomic_write;
	// Set MPEG4 Parameter In: HOST PARAM SET
	processingComp.set_pMecIn_parameters(pProxyComponent, &ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters, &(ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_custom_parameters));

	set_pMecOther_parameters(&ps_ddep_vec_mpeg4_param_desc_dh->host_param_set, searchwinsize, mvfieldbuffersize, headerBufferAddress, fwdRefBufferAddress, destBufferAddress, intraRefBufferAddress_in, intraRefBufferAddress_out, motionVectorBufferAddress, searchWindowBufferAddress, mvFieldBufferAddress);

	// Set MPEG4 Parameter Other and header	//< FIXME
	ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_custom_parameters.atomic_count = atomic_write;
	ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_exit = atomic_write;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ArmNmf_ProcessingComponent::configureAlgo()");

	return OMX_ErrorNone;
}


OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::codecDeInstantiate()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_ArmNmf_ProcessingComponent::codecDeInstantiate()");

	t_nmf_error error;

    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(0), "emptythisbuffer", "inputport"));
    RETURN_OMX_ERROR_IF_ERROR(destroyPortInterface((VFM_Port *)mENSComponent.getPort(1), "fillthisbuffer", "outputport"));
	error = mCodec->unbindFromUser("setParam");
	if (error != NMF_OK)
	{
		OstTraceInt1(TRACE_ERROR, "MPEG4ENC_ARM_MPC : Error in codecDeInstantiate line no :%d \n",__LINE__);
		return OMX_ErrorUndefined ;
	}

	error = mCodec->unbindFromUser("setCropForStab");
	if (error != NMF_OK)
	{
		OstTraceInt1(TRACE_ERROR, "MPEG4ENC_ARM_MPC : Error in codecDeInstantiate line no :%d \n",__LINE__);
		return OMX_ErrorUndefined ;
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_ArmNmf_ProcessingComponent::codecDeInstantiate()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_FLOW, "MPEG4ENC_ARM_MPC : In emptyThisBuffer pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
	OMX_U32 cropTopOffset = 0, cropLeftOffset = 0, overscanWidth = 0, overscanHeight = 0;
	OMX_VIDEO_PORTDEFINITIONTYPE * mvideo_port;
	/* Checking for valid nFilledLen for the input buffer */
	MPEG4Enc_Port *pt_port_in;
	MPEG4Enc_Port *pt_port_out;
	pt_port_in = (MPEG4Enc_Port *)pProxyComponent->getPort(VPB+0);
	pt_port_out = (MPEG4Enc_Port *)pProxyComponent->getPort(VPB+1);
	mvideo_port = pt_port_in->getVideoPortDefinition();

	//Set the default values first.
	cropLeftOffset 	= pProxyComponent->mParam.m_crop_param.nLeft;
	cropTopOffset 	= pProxyComponent->mParam.m_crop_param.nTop;
	overscanWidth	= pProxyComponent->getFrameWidth(0);
	overscanHeight	= pProxyComponent->getFrameHeight(0);
	OMX_U32 computedStride = pt_port_in->getStride(mvideo_port->eColorFormat,mvideo_port->nFrameWidth);
	OstTraceFiltInst2(TRACE_FLOW,"MPEG4Enc_NmfMpc_ProcessingComponent:: Actual Stride : %d computedStride %d ",mvideo_port->nStride,computedStride);
	if((VFM_SocCapabilityMgt::getOutputBufferCount() &&
            ((unsigned)mvideo_port->nStride > computedStride )) ||
            (pBuffer->nFlags & OMX_BUFFERFLAG_EXTRADATA))
	{
		OMX_OTHER_EXTRADATATYPE *pExtraData;

		OstTraceFiltInst0(TRACE_FLOW, "MPEG4Enc_NmfMpc_ProcessingComponent::emptyThisBuffer(): OMX_BUFFERFLAG_EXTRADATA found");
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::emptythisbuffer() (mvideo_port->nStride) = %d",(mvideo_port->nStride));
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::emptythisbuffer() mvideo_port->nSliceHeight = %d",mvideo_port->nSliceHeight);
		pBuffer->nFilledLen = (mvideo_port->nStride)*(mvideo_port->nSliceHeight);
		//Now that there is some ExtraData, we go to the first Extra Data section (aligned at 4-byte boundary)
		pExtraData = (OMX_OTHER_EXTRADATATYPE *) (((OMX_U32)(pBuffer->pBuffer + pBuffer->nOffset + pBuffer->nFilledLen + 3)) & ~3);

		//We traverse all the ExtraData sections until we find the terminating ExtraData section
		// If there are multiple ExtraData sections with Stabilization info present, then
		// the data in the last Stabilization ExtraData section will be finally sent to Ddep.
		while(pExtraData->eType != OMX_ExtraDataNone)
		{
			if(pExtraData->eType == (OMX_EXTRADATATYPE)OMX_SYMBIAN_ExtraDataVideoStabilization)
			{
				OstTraceFiltInst0(TRACE_FLOW, "MPEG4Enc_NmfMpc_ProcessingComponent::OMX_SYMBIAN_ExtraDataVideoStabilization found");
				//Clearing the ExtraData Buffer Flag, to prevent it from propagating to the output buffer
				pBuffer->nFlags &= ~(OMX_BUFFERFLAG_EXTRADATA);

				OMX_VIDEO_DIGITALVIDEOSTABTYPE *pDVStab;
				pDVStab = (OMX_VIDEO_DIGITALVIDEOSTABTYPE *)&(pExtraData->data);
				OstTraceFiltInst2(TRACE_FLOW, "overscanWidth -%d overscanHeight-%d",overscanWidth,overscanHeight);
				OstTraceFiltInst1(TRACE_FLOW, "pDVStab->bState",pDVStab->bState);
				if(pDVStab->bState)
				{
					//Stabilization is enabled, so we overwrite the default values in the variables with this data
					cropLeftOffset 	= pDVStab->nTopLeftCropVectorX + pProxyComponent->mParam.m_crop_param.nLeft;
					cropTopOffset 	= pDVStab->nTopLeftCropVectorY + pProxyComponent->mParam.m_crop_param.nTop;
					overscanWidth	= pDVStab->nMaxOverscannedWidth;
					overscanHeight	= pDVStab->nMaxOverscannedHeight;
				}
				else
				{
					OstTraceFiltInst1(TRACE_FLOW, "pt_port_out->bCheck",pt_port_out->bCheck);
					if(pt_port_out->bCheck)
					{
						overscanWidth	= ((mvideo_port->nStride)/(pt_port_in->getPixelDepth(pt_port_in->getColorFormat() )	));
						overscanHeight	= mvideo_port->nSliceHeight;
						OstTraceFiltInst0(TRACE_FLOW, "bState is off but overscanned buffer ");
						OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::emptythisbuffer() (mvideo_port->nStride) = %d",(mvideo_port->nStride));
						OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::emptythisbuffer() mvideo_port->nSliceHeight = %d",mvideo_port->nSliceHeight);


					}
				}
				OstTraceFiltInst2(TRACE_FLOW, "cropLeftOffset -%d cropTopOffset -%d",cropLeftOffset,cropTopOffset);
			}

			//Go to the next ExtraData section
			pExtraData = (OMX_OTHER_EXTRADATATYPE *) (((OMX_U8 *)pExtraData) + pExtraData->nSize);
		}
	}
	else
	{
		OstTraceFiltInst1(TRACE_FLOW, "pt_port_out->bCheck",pt_port_out->bCheck);
			if(pt_port_out->bCheck)
			{
					overscanWidth	= ((mvideo_port->nStride)/(pt_port_in->getPixelDepth(pt_port_in->getColorFormat() )	));
					overscanHeight	= mvideo_port->nSliceHeight;
					OstTraceFiltInst0(TRACE_FLOW, "bState is off but overscanned buffer ");
					OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::emptythisbuffer() (mvideo_port->nStride) = %d",(mvideo_port->nStride));
					OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_Port::emptythisbuffer() mvideo_port->nSliceHeight = %d",mvideo_port->nSliceHeight);

			}
	}

     /* +Change for 372717 */
     overscanWidth = ConvertMult16(overscanWidth);
	 overscanHeight = ConvertMult16(overscanHeight);
     /* -Change for 372717 */

	OstTraceFiltInst2(TRACE_FLOW, "overscanWidth -%d overscanHeight-%d",overscanWidth,overscanHeight);
	setcropstabitf.setCropForStab(cropLeftOffset, cropTopOffset, overscanWidth, overscanHeight);
/* +Change start for ER 348538 I-frame request is not applied to correct frame */
	if(pProxyComponent->mParam.m_force_intra.IntraRefreshVOP == OMX_TRUE)
	{
		pBuffer->nFlags |= 0x100;
		pProxyComponent->mParam.m_force_intra.IntraRefreshVOP = OMX_FALSE;
	}
/* -Change end for ER 348538 I-frame request is not applied to correct frame */
	return VFM_NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_FLOW, "MPEG4ENC_ARM_MPC : In fillThisBuffer pBuffer->pBuffer (0x%x)  pBuffer->nFilledLen (%d)  pBuffer->nFlags (%d) \n",(OMX_U32)pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags);
	return NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);
}

void MPEG4Enc_ArmNmf_ProcessingComponent::registerStubsAndSkels()
{
	//> CM_REGISTER_STUBS(video_mpeg4enc_cpp);
}

void MPEG4Enc_ArmNmf_ProcessingComponent::unregisterStubsAndSkels()
{
	//CM_UNREGISTER_STUBS(video_mpeg4enc_cpp);
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::applyConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_ArmNmf_ProcessingComponent::errorRecoveryDestroyAll()
{
	mCodec->stop();
	codecStop();
	return VFM_NmfHost_ProcessingComponent::deInstantiate();
}


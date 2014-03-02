/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define _CNAME_ Vfm_mpeg4enc
#include "osi_trace.h"
#include "VFM_DDepUtility.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4enc_proxy_src_MPEG4Enc_NmfMpc_ProcessingComponentTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

#include "VFM_Port.h"
#include "VFM_ParamAndConfig.h"
#include "VFM_Memory.h"
#include "VFM_DDepUtility.h"
#include "MPEG4Enc_Proxy.h"
#include "MPEG4Enc_Port.h"
#include <cm/inc/cm_macros.h>
#include "host/eventhandler.hpp"
#include "SharedBuffer.h"

#ifdef MMDSP_PERF
	#include "osi_perf.h"
#endif

#define DEBUG_MEM_SIZE (64*1024 - 1)
#define BUFFER_LINKLIST_SIZE (1024)
#define SEARCH_WINDOW_SIZE_IN_BYTES (0xa000)
#define HEADER_BUFFER_SIZE (0x38)

#define MAX_VBV_OCCUPANCY (63488)

//>#define MV_FIELD_BUFFER_SIZE_IN_BYTES (0x6000)

/* +Change for 372717 */
#define ConvertMult16(x) ((x%16)?(x/16 + 1)*16 : x )
/* -Change for 372717 */

volatile ts_ddep_buffer_descriptor g_mDebugBufferDescmpeg4enc1;

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

#define PRINTPG


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

static OMX_U32 filledLenLastVOL = 0;

MPEG4Enc_NmfMpc_ProcessingComponent::MPEG4Enc_NmfMpc_ProcessingComponent(ENS_Component &enscomp): VFM_NmfMpc_ProcessingComponent(enscomp)
{
	OstTraceInt0(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::MPEG4Enc_NmfMpc_ProcessingComponent() constructor");

	//iDdep.THIS = 0;
	//iDdep.setConfig = 0;
	//iDdep.setNeeds = 0;
	//iDdep.setParameter = 0;

	//pProxyComponent = (MPEG4Enc_Proxy *)0;
	mChannelId = 0;
	ddHandle = VFM_INVALID_CM_HANDLE;
	brcHandle = VFM_INVALID_CM_HANDLE;
	mNmfAlgoHandle = VFM_INVALID_CM_HANDLE;
	mNmfRmHandle = VFM_INVALID_CM_HANDLE;

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

	mDebugBufferDesc.nSize = 0;
	mDebugBufferDesc.nMpcAddress = 0;
	mDebugBufferDesc.nLogicalAddress = 0;
	mDebugBufferDesc.nPhysicalAddress = 0;

	pProxyComponent = (MPEG4Enc_Proxy *)(&mENSComponent);

	cOSTTrace = VFM_INVALID_CM_HANDLE;

	vfm_mem_ctxt = 0;

	vos_written = NOT_WRITTEN;

#ifdef __SYMBIAN32__
	fullDVFSSet = OMX_FALSE;
#endif //__SYMBIAN32__

	OstTraceInt0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::MPEG4Enc_NmfMpc_ProcessingComponent()");
}

MPEG4Enc_NmfMpc_ProcessingComponent::~MPEG4Enc_NmfMpc_ProcessingComponent()
{
	OstTraceInt0(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::~MPEG4Enc_NmfMpc_ProcessingComponent() destructor");

	if(mParamBufferDesc.nLogicalAddress)
	{
		VFM_Free(vfm_mem_ctxt, (t_uint8*)mParamBufferDesc.nLogicalAddress);
		mParamBufferDesc.nSize = 0;
		mParamBufferDesc.nLogicalAddress = 0;
		mParamBufferDesc.nPhysicalAddress = 0;
		pProxyComponent->pFwPerfDataPtr = (OMX_PTR)0;
	}

	if(mInternalBufferDesc.nLogicalAddress)
	{
		VFM_Free(vfm_mem_ctxt, (t_uint8*)mInternalBufferDesc.nLogicalAddress);
		mInternalBufferDesc.nSize = 0;
		mInternalBufferDesc.nLogicalAddress = 0;
		mInternalBufferDesc.nPhysicalAddress = 0;
	}

	if(mDebugBufferDesc.nLogicalAddress)
	{
		VFM_Free(vfm_mem_ctxt, (t_uint8*)mDebugBufferDesc.nLogicalAddress);
		mDebugBufferDesc.nSize = 0;
		mDebugBufferDesc.nLogicalAddress = 0;
		mDebugBufferDesc.nPhysicalAddress = 0;
	}

	if(mLinkListBufferDesc.nLogicalAddress)
	{
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

	OstTraceInt0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::~MPEG4Enc_NmfMpc_ProcessingComponent()");
}

void MPEG4Enc_NmfMpc_ProcessingComponent::set_pMecOther_parameters(ts_ddep_vec_mpeg4_param_desc *ps_ddep_vec_mpeg4_param_desc,
																   t_uint32 searchwinsize,
																   t_uint32 mvfieldbuffersize,
																   t_uint8 *headerBufferPhysicalAddress,
																   t_uint8* fwdRefBufferPhysicalAddress,
																   t_uint8* destBufferPhysicalAddress,
																   t_uint8* intraRefBufferPhysicalAddress_in,
																   t_uint8* intraRefBufferPhysicalAddress_out,
																   t_uint8* motionVectorBufferPhysicalAddress,
																   t_uint8* searchWindowBufferPhysicalAddress,
																   t_uint8* mvFieldBufferPhysicalAddress
																  )
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::set_pMecOther_parameters()");

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

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::set_pMecOther_parameters()");
}


OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler)
{
	OstTraceFiltInst3(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificEventHandler_cb() event=0x%x, nData1=0x%x, nData2=0x%x", event, nData1, nData2);

	if(event==OMX_EventCmdComplete&&nData1==OMX_CommandPortDisable)
		switch(nData2)
		{
			case 0:
				RETURN_XXX_IF_WRONG((ENS::unbindComponent(getNmfSharedBuf(0),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::unbindComponent(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::bindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

				break;
			case 1:
				RETURN_XXX_IF_WRONG((ENS::unbindComponent(getNmfSharedBuf(1),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::unbindComponent(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::bindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

				break;
			case OMX_ALL:
				RETURN_XXX_IF_WRONG((ENS::unbindComponent(getNmfSharedBuf(0),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::unbindComponent(getNmfSharedBuf(1),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::unbindComponent(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::unbindComponent(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::bindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::bindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
				RETURN_XXX_IF_WRONG((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

		}

	deferEventHandler = OMX_FALSE;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificEventHandler_cb()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
	OstTraceFiltInst2(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificSendCommand() eCmd=0x%x, nData=0x%x", eCmd, nData);

	ENS_Port    *pt_port_in, *pt_port_out;
	pt_port_in = (VFM_Port*)pProxyComponent->getPort(VPB+0);
	pt_port_out = (VFM_Port*)pProxyComponent->getPort(VPB+1);

	//> OstTraceFunctionEntryExt(MPEG4ENC_NMF_SENDCOMMAND_ENTRY, this);

	//> OstTraceExt2(TRACE_NORMAL,MPEG4ENC_NMF_SENDCOEMMAND, "command = %d, data = %d\n", eCmd, nData);

	switch(eCmd)
	{
		case OMX_CommandStateSet:
			switch(nData)
			{
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
				default:
					//> OstTraceFunctionExitExt(MPEG4ENC_NMF_SENDCOMMAND_EXIT_1, this, OMX_ErrorNotImplemented);

					return OMX_ErrorNotImplemented;
			}
			break;
		case OMX_CommandPortDisable:
			switch(nData)
			{
				case 0:
					vos_written = NOT_WRITTEN;	// If port 0 is disabled, then VOS+VO+VOL headers must be resent
					pProxyComponent->mSendParamToMpc.set(); // The parameters must be set again
					break;
				case 1:
					//< vos_written = OMX_FALSE;
					break;
				default:
					//> OstTraceFunctionExitExt(MPEG4ENC_NMF_SENDCOMMAND_EXIT_2, this, OMX_ErrorBadPortIndex);
					return OMX_ErrorBadPortIndex;
			}
			break;
		case OMX_CommandPortEnable:
			switch(nData)
			{
				case 0:
					if(pt_port_in->isEnabled())
					{
						RETURN_XXX_IF_WRONG((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::bindComponent(getNmfSharedBuf(0), "mpc",ddHandle,"emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::unbindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);

						RETURN_XXX_IF_WRONG((ENS::bindComponent(ddHandle,"inputport", getNmfSharedBuf(0), "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
					}
					break;
				case 1:
					if(pt_port_out->isEnabled())
					{
						RETURN_XXX_IF_WRONG((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::bindComponent(getNmfSharedBuf(1),"mpc",ddHandle, "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::unbindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);

						RETURN_XXX_IF_WRONG((ENS::bindComponent(ddHandle,"outputport", getNmfSharedBuf(1), "emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
					}

					break;
				case OMX_ALL:
					if((pt_port_in->isEnabled()) && (pt_port_out->isEnabled()))
					{
						RETURN_XXX_IF_WRONG((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::bindComponent(getNmfSharedBuf(0), "mpc",ddHandle,"emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::unbindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);

						RETURN_XXX_IF_WRONG((ENS::bindComponent(getNmfSharedBuf(1),"mpc",ddHandle, "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::unbindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);

						RETURN_XXX_IF_WRONG((ENS::bindComponent(ddHandle,"inputport", getNmfSharedBuf(0), "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::bindComponent(ddHandle,"outputport", getNmfSharedBuf(1), "emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
						RETURN_XXX_IF_WRONG((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

					}

					break;
				default:
					//> OstTraceFunctionExitExt(MPEG4ENC_NMF_SENDCOMMAND_EXIT_3, this, OMX_ErrorBadPortIndex);

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
					//> OstTraceFunctionExitExt(MPEG4ENC_NMF_SENDCOMMAND_EXIT_4, this, OMX_ErrorBadPortIndex);

					return OMX_ErrorBadPortIndex;
			}
			break;
		default:
			//> OstTraceFunctionExitExt(MPEG4ENC_NMF_SENDCOMMAND_EXIT_5, this, OMX_ErrorNotImplemented);
			return OMX_ErrorNotImplemented;
	}

	if((eCmd == OMX_CommandPortEnable) && pProxyComponent->mSendParamToMpc.get())
	{
		configure();
	}

	bDeferredCmd = OMX_FALSE;

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificSendCommand()");

	return OMX_ErrorNone;
}

void MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst4(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificFillBufferDone_cb() pBuffer=0x%x, pBuffer->nTimeStamp=%d, pBuffer->nFilledLen=%d, pBuffer->nFlags=0x%x", (unsigned int)pBuffer, (unsigned int)pBuffer->nTimeStamp, pBuffer->nFilledLen, pBuffer->nFlags);

	OMX_U32 filledLen;
	OMX_U32 filledLen_bits;
	t_uint8* bufferptr;
	ts_ddep_vec_mpeg4_param_desc_dh *ps_ddep_vec_mpeg4_param_desc_dh;
	t_uint8 *ptr;
	t_uint16 atomic_write;

#ifdef MMDSP_PERF
	write_ticks_to_buffer("PFBDI");
#endif

#ifdef _CACHE_OPT_
	VFM_CacheInvalidate(vfm_mem_ctxt, pBuffer->pBuffer, pBuffer->nFilledLen);
#endif

	filledLen = pBuffer->nFilledLen;

	if(((pProxyComponent->mParam.m_enc_param.bSVH) == OMX_FALSE) && (vos_written == NOT_WRITTEN))
	{
		vos_written = WRITTEN_ONCE;
		bufferptr = (t_uint8 *)(pBuffer->pBuffer+pBuffer->nOffset);
		filledLen_bits = processingComp.Write_VOS_VO_VOL(pProxyComponent, bufferptr);
		ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
		ps_ddep_vec_mpeg4_param_desc_dh = (ts_ddep_vec_mpeg4_param_desc_dh *) ptr;
		atomic_write=ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_entry + 1;
		ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_entry = atomic_write;
        OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::1 - %x",ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_entry);
        /* +Change for 372717 */
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::1 frame_height- %d",ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.frame_height);
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::1 window_width- %d",ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.window_width);
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::1 window_height- %d",ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.window_height);
        OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::1 frame_width- %d",ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.frame_width);
        ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.frame_height = ConvertMult16(ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.frame_height);
		ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.window_width = ConvertMult16(ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.window_width);
		ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.window_height = ConvertMult16(ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.window_height);
		ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.frame_width = ConvertMult16(ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.frame_width);
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::2frame_height - %d",ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.frame_height);
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::2window_width - %d",ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.window_width);
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::2 window_height- %d",ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.window_height);
		OstTraceFiltInst1(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::2 frame_width- %d",ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_parameters.frame_width);
		/* -Change for 372717 */
		ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_exit=atomic_write;

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
	/*
	if((pProxyComponent->mParam.m_vos_header.isSystemHeaderAddBeforeIntra == OMX_TRUE) && (pBuffer->nFlags & OMX_BUFFERFLAG_EOS))
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
	}*/
#ifdef MMDSP_PERF
	write_ticks_to_buffer("PFBDO");
#endif

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificFillBufferDone_cb()");

}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::allocateBuffer(
		OMX_U32 nPortIndex,
		OMX_U32 nBufferIndex,
		OMX_U32 nSizeBytes,
		OMX_U8 **ppData,
		void **bufferAllocInfo,
		void **portPrivateInfo)
{
	OMX_ERRORTYPE error;
	OstTraceInt0(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::allocateBuffer enter");
	error =	NmfMpc_ProcessingComponent::allocateBuffer(nPortIndex, nBufferIndex, nSizeBytes, ppData, bufferAllocInfo, portPrivateInfo);

	if(error != OMX_ErrorNone) {
		return error;
	}

#ifdef _CACHE_OPT_
	OMX_U32 bufPhysicalAddr = VFM_NmfMpc_ProcessingComponent::getBufferPhysicalAddress(*bufferAllocInfo, *ppData, nSizeBytes);

	if(!vfm_mem_ctxt)
	{
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
	}
	VFM_AddMemoryExternalSource(vfm_mem_ctxt, (*ppData), (t_uint8 *)bufPhysicalAddr, nSizeBytes, (*bufferAllocInfo));
#endif

	return OMX_ErrorNone;

}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::useBuffer(
		OMX_U32 nPortIndex,
		OMX_U32 nBufferIndex,
		OMX_BUFFERHEADERTYPE* pBufferHdr,
		void **portPrivateInfo)
{
	OMX_ERRORTYPE error;

	/* +Change start from CR 399939 */
    OMX_BOOL isMetaDataUsed = pProxyComponent->mParam.getStoreMetadataInBuffers();
    PRINTPG("In mpeg4enc usebuffer header buffer addr : 0x%x \n",pBufferHdr);
    if (isMetaDataUsed && pProxyComponent->isEncoder() && (0 == nPortIndex))
    {
        pBufferHdr->nInputPortIndex = 0;
        *portPrivateInfo = 0;
		PRINTPG("In use buffer and for metaData making pBufferHdr-> to NULL, so DO NOTHING HERER \n");
		return OMX_ErrorNone;
	}
	/* -Change end from CR 399939 */

	error =	NmfMpc_ProcessingComponent::useBuffer(nPortIndex, nBufferIndex,	pBufferHdr, portPrivateInfo);

	if(error != OMX_ErrorNone) {
		return error;
	}

#ifdef _CACHE_OPT_
	void *bufferAllocInfo = 0;
	OMX_U8 *pBuffer;

	ENS_Port *port = mENSComponent.getPort(nPortIndex);
	bufferAllocInfo = port->getSharedChunk();
	pBuffer = pBufferHdr->pBuffer;

	OMX_U32 bufPhysicalAddr = VFM_NmfMpc_ProcessingComponent::getBufferPhysicalAddress(bufferAllocInfo, pBuffer, pBufferHdr->nAllocLen);

	if(!vfm_mem_ctxt)
	{
		OMX_U32 domainInfo = mENSComponent.getNMFDomainHandle(0); //PortIndex 0 as domain are same at all ports
		OMX_U32 ddrDomainId = pProxyComponent->getAllocDdrMemoryDomain();
		vfm_mem_ctxt = VFM_CreateMemoryList(domainInfo,ddrDomainId,0/*pass 0 as it is not full software*/);
	}
	VFM_AddMemoryExternalSource(vfm_mem_ctxt, pBuffer, (t_uint8 *)bufPhysicalAddr, pBufferHdr->nAllocLen, bufferAllocInfo);
#endif
    return error;
}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst4(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::emptyThisBuffer() pBuffer=0x%x, pBuffer->nTimeStamp=%d, pBuffer->nFilledLen=%d, pBuffer->nFlags=0x%x", (unsigned int)pBuffer, (unsigned int)pBuffer->nTimeStamp, pBuffer->nFilledLen, pBuffer->nFlags);
	OMX_U32 cropTopOffset = 0, cropLeftOffset = 0, overscanWidth = 0, overscanHeight = 0;
	OMX_VIDEO_PORTDEFINITIONTYPE * mvideo_port;
	/* Checking for valid nFilledLen for the input buffer */
	MPEG4Enc_Port *pt_port_in;
	MPEG4Enc_Port *pt_port_out;


	/* +Change start from CR 399938 */
	OMX_BOOL isMetaDataUsed =pProxyComponent->mParam.getStoreMetadataInBuffers();
	if (isMetaDataUsed)
	{
		OMX_ERRORTYPE error;
		OMX_U32 bufPhysicalAddr;
		pProxyComponent->mParam.extraProcessingInputBuffers(OMX_DirInput, &pBuffer); //now pBuffer->pBuffer is logical address
    	bufPhysicalAddr = pProxyComponent->mParam.getBufferPhysicalAddress((OMX_U32)pBuffer->pBuffer);
    	PRINTPG("In emptythisbuffer and physical buffer 0x%x \n",bufPhysicalAddr);

        if (pBuffer->pInputPortPrivate)
        {
			SharedBuffer *sharedBuf = (SharedBuffer *)pBuffer->pInputPortPrivate;
			PRINTPG("pInputPortPrivate already exists so deleting it first : 0x%x \n",(OMX_U32)sharedBuf);
			delete sharedBuf;
			pBuffer->pInputPortPrivate = 0;
		}
        PRINTPG("Abt to allocate Sharedbuffer memory \n");
	    SharedBuffer *sharedBuf = new SharedBuffer(mENSComponent.getNMFDomainHandle(),pBuffer->nAllocLen, pBuffer->pBuffer, bufPhysicalAddr, 0, 0, error);
	    if (sharedBuf == 0) return OMX_ErrorInsufficientResources;
        PRINTPG("Shared buffer allocated : 0x%x \n",sharedBuf);
	    sharedBuf->setOMXHeader(pBuffer);
	    pBuffer->pInputPortPrivate = sharedBuf;
	}

	PRINTPG("=> MPEG4Enc_NmfMpc_ProcessingComponent::emptyThisBuffer() AFTER pBuffer=0x%x, pBuffer->nTimeStamp=%d, pBuffer->nFilledLen=%d, pBuffer->nFlags=0x%x pBuffer->pBuffer=0x%x \n", (unsigned int)pBuffer, (unsigned int)pBuffer->nTimeStamp, pBuffer->nFilledLen, pBuffer->nFlags, (unsigned int)pBuffer->pBuffer);
	/* -Change start from CR 399938 */

/* +Change start for ER 348538 I-frame request is not applied to correct frame */
	if(pProxyComponent->mParam.m_force_intra.IntraRefreshVOP == OMX_TRUE)
	{
		pBuffer->nFlags |= 0x100;
		pProxyComponent->mParam.m_force_intra.IntraRefreshVOP = OMX_FALSE;
	}
/* -Change end for ER 348538 I-frame request is not applied to correct frame */
	pt_port_in = (MPEG4Enc_Port *)pProxyComponent->getPort(VPB+0);
	pt_port_out = (MPEG4Enc_Port *)pProxyComponent->getPort(VPB+1);
	mvideo_port = pt_port_in->getVideoPortDefinition();

	//Set the default values first.
	cropLeftOffset 	= pProxyComponent->mParam.m_crop_param.nLeft;
	cropTopOffset 	= pProxyComponent->mParam.m_crop_param.nTop;

	/* +Change start for CR343589 Rotation */
	switch(pProxyComponent->mParam.m_rotation.nRotation)
	{
		case 90   :
		case -90  :
		case 270  :
		case -270 :
			overscanWidth	= pProxyComponent->getFrameHeight(0);
			overscanHeight	= pProxyComponent->getFrameWidth(0);
			break;

		default:
			overscanWidth	= pProxyComponent->getFrameWidth(0);
			overscanHeight	= pProxyComponent->getFrameHeight(0);
			break;
	}
	/* -Change end for CR343589 Rotation */

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
	iDdep.setCropForStab(cropLeftOffset, cropTopOffset, overscanWidth, overscanHeight);

#ifdef _CACHE_OPT_
        if (!isMetaDataUsed) // Change for CR 399938 */
        {
			VFM_CacheClean(vfm_mem_ctxt, pBuffer->pBuffer, pBuffer->nFilledLen);
        }
#endif

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::emptyThisBuffer()");

	return NmfMpc_ProcessingComponent::emptyThisBuffer(pBuffer);
}


void MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst3(TRACE_API, "<=> MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificEmptyBufferDone_cb() pBuffer=0x%x, pBuffer->nFilledLen=0x%x, pBuffer->nFlags=0x%x", (unsigned int)pBuffer, pBuffer->nFilledLen, pBuffer->nFlags);

	/* +Change start from CR 399938 */
    OMX_BOOL isMetaDataUsed = pProxyComponent->mParam.getStoreMetadataInBuffers();
	if (isMetaDataUsed)
	{
        PRINTPG("In DELETEION LOOP but Not to delete \n");
		SharedBuffer *sharedBuf = (SharedBuffer *)pBuffer->pInputPortPrivate;
        PRINTPG("In DELETEION LOOP but Not to delete shared Buf 0x%x \n",sharedBuf);
	    pProxyComponent->mParam.extraProcessingInputBuffers(OMX_DirOutput, &pBuffer);
	}
	/* -Change start from CR 399938 */

	PRINTPG("<=> MPEG4Enc_NmfMpc_ProcessingComponent::doSpecificEmptyBufferDone_cb() AFTER pBuffer=0x%x, pBuffer->nFilledLen=0x%x, pBuffer->nFlags=0x%x pBuffer->pBuffer=0x%x pBuffer->nInputPortIndex 0x%x \n", (unsigned int)pBuffer, pBuffer->nFilledLen, pBuffer->nFlags,(unsigned int)pBuffer->pBuffer,pBuffer->nInputPortIndex);
}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::instantiate()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::instantiate()");

	pProxyComponent = (MPEG4Enc_Proxy *)(&mENSComponent);
	//CBeventhandler local_cb_eventhandler = {this, eventHandler_cb};
	t_uint32 fifo_size;		 // size of the FIFO for NMF commands NMFCALL()

	ENS_Port    *pt_port_in, *pt_port_out;
	pt_port_in = (VFM_Port*)pProxyComponent->getPort(VPB+0);
	pt_port_out = (VFM_Port*)pProxyComponent->getPort(VPB+1);

	fifo_size = (pt_port_in->getBufferCountActual()>pt_port_out->getBufferCountActual() ? pt_port_in->getBufferCountActual() : pt_port_out->getBufferCountActual()) + 1;

	/*Check for proper configuration */
	registerStubsAndSkels();

	// Get an instance handle for the resource manager component and a ChannelId.
	RETURN_XXX_IF_WRONG((ENS::instantiateNMFComponent(pProxyComponent->getNMFDomainHandle(RM_NMFD_PROCSVA), "resource_manager", "resource_manager", &mNmfRmHandle, NMF_SCHED_URGENT) == OMX_ErrorNone),OMX_ErrorUndefined);

	// First Mpeg4 encoder instantiate invocation : Instantiate the singleton.
	RETURN_XXX_IF_WRONG((ENS::instantiateNMFComponent(pProxyComponent->getNMFDomainHandle(RM_NMFD_PROCSVA), "mpeg4enc", "vec_mpeg4_algo", &mNmfAlgoHandle,NMF_SCHED_NORMAL)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::instantiateNMFComponent(pProxyComponent->getNMFDomainHandle(), "osttrace.mmdsp", "sec_osttrace", &cOSTTrace,NMF_SCHED_NORMAL)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::bindComponent(mNmfAlgoHandle, "iBuffer", mNmfRmHandle, "iBuffer")== OMX_ErrorNone),OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::bindComponent(mNmfAlgoHandle, "iMtf", mNmfRmHandle, "iMtf")== OMX_ErrorNone),OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::bindComponent(mNmfAlgoHandle, "iInterrupt", mNmfRmHandle, "iInterrupt")== OMX_ErrorNone),OMX_ErrorUndefined);

	// Data dep instantiation
	RETURN_XXX_IF_WRONG((ENS::instantiateNMFComponent(pProxyComponent->getNMFDomainHandle(RM_NMFD_PROCSVA), "mpeg4enc.mpc.ddep", "mpeg4enc.mpc.ddep", &ddHandle, NMF_SCHED_URGENT)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::instantiateNMFComponent(pProxyComponent->getNMFDomainHandle(RM_NMFD_PROCSVA), "mpeg4enc.mpc.brc", "mpeg4enc.mpc.brc", &brcHandle, NMF_SCHED_URGENT)==OMX_ErrorNone), OMX_ErrorUndefined);

	if(pt_port_in->isEnabled() == OMX_TRUE)
	{
		RETURN_XXX_IF_WRONG((ENS::bindComponent(getNmfSharedBuf(0), "mpc",ddHandle,"emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
		RETURN_XXX_IF_WRONG((ENS::bindComponent(ddHandle,"inputport",getNmfSharedBuf(0), "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
	}
	else
	{
		RETURN_XXX_IF_WRONG((ENS::bindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
	}
	// Bind Host and dd
	// ALgo use to configure
	RETURN_XXX_IF_WRONG((ENS::bindComponentFromHost(ddHandle, "ddep", &iDdep, 8)==OMX_ErrorNone), OMX_ErrorUndefined);
	// Send command will replace the algo interface
	RETURN_XXX_IF_WRONG((ENS::bindComponentFromHost(ddHandle, "sendcommand",  getNmfSendCommandItf(), fifo_size)==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG((ENS::bindComponentToHost(pProxyComponent->getOMXHandle(), ddHandle, "proxy",  (NMF::InterfaceDescriptor *)this, fifo_size)==OMX_ErrorNone), OMX_ErrorUndefined);

	// Input port bindings


	// Output port bindings

	if(pt_port_out->isEnabled() == OMX_TRUE)
	{
		RETURN_XXX_IF_WRONG((ENS::bindComponent(getNmfSharedBuf(1),"mpc",ddHandle, "fillthisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
		RETURN_XXX_IF_WRONG((ENS::bindComponent(ddHandle, "outputport", getNmfSharedBuf(1),"emptythisbuffer")==OMX_ErrorNone), OMX_ErrorUndefined);
	}
	else
	{
		RETURN_XXX_IF_WRONG((ENS::bindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
	}

	//event missing issue for camcorder UC
//	RETURN_XXX_IF_WRONG((ENS::bindComponentToHost(pProxyComponent->getOMXHandle(),ddHandle, "proxy",  &local_cb_eventhandler, fifo_size)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::bindComponentAsynchronous (ddHandle, "iResource", mNmfRmHandle, "iResource",4)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::bindComponent (mNmfComponentFsmLib, "osttrace" , cOSTTrace,"osttrace"     )==OMX_ErrorNone), OMX_ErrorUndefined);

	// Added for Power Management
	RETURN_XXX_IF_WRONG((ENS::bindComponent(ddHandle, "iSleep", mNmfRmHandle, "iSleep")== OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::bindComponent (ddHandle, "osttrace",cOSTTrace,"osttrace")==OMX_ErrorNone), OMX_ErrorUndefined);
	
	RETURN_XXX_IF_WRONG((ENS::bindComponent (brcHandle, "osttrace",cOSTTrace,"osttrace")==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG((ENS::bindComponentAsynchronous (ddHandle, "codec_algo", mNmfAlgoHandle, "iAlgo", 4)==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG((ENS::bindComponent (ddHandle, "codec_update_algo", mNmfAlgoHandle, "iUpdateAlgo")==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG((ENS::bindComponent (ddHandle, "iBrc", brcHandle, "brc")==OMX_ErrorNone), OMX_ErrorUndefined);

	// Added by Pascal
	RETURN_XXX_IF_WRONG((ENS::bindComponent (ddHandle ,"genericfsm" , mNmfGenericFsmLib, "genericfsm")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::bindComponent (ddHandle ,"componentfsm" , mNmfComponentFsmLib, "componentfsm")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::bindComponentAsynchronous (ddHandle, "iSendLogEvent", ddHandle, "iGetLogEvent", 4)==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG((ENS::bindComponentAsynchronous (ddHandle, "me", ddHandle, "postevent", 4)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::bindComponentFromHost(ddHandle, "fsminit",  getNmfFsmInitItf(), 2)==OMX_ErrorNone), OMX_ErrorUndefined);

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::instantiate()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::start()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::start()");

	RETURN_XXX_IF_WRONG((ENS::startNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::startNMFComponent(brcHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::start()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::stop()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::stop()");

	RETURN_XXX_IF_WRONG((ENS::stopNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::stopNMFComponent(brcHandle)==OMX_ErrorNone), OMX_ErrorUndefined);

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::stop()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::configure()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::configure()");

	if(pProxyComponent->mSendParamToMpc.get())
	{
		RETURN_OMX_ERROR_IF_ERROR(configureAlgo());

		//Inform Ddep whether MPC side encoding has to be disabled
		iDdep.disableFWCodeexection(pProxyComponent->mIsARMLoadComputed);

		//Pass LinkList desc to DataDep
		iDdep.setNeeds(mChannelId,mLinkListBufferDesc,mDebugBufferDesc);
		//Pass Param desc to DataDep
		iDdep.setParameter(mChannelId,ID_VEC_MPEG4,mParamBufferDesc);

		pProxyComponent->mSendParamToMpc.reset();
	}

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::configure()");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::configureAlgo()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::configureAlgo()");

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
	t_uint32 motionvecsize = ((((width/16+2)*(height/16+2)*4)/256+1)*256) + VFM_IMAGE_BUFFER_ALIGN_MASK;
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
	t_uint8* debug_buf_logical_addr = 0;

	{
		OstTraceFiltInst1(TRACE_FLOW, "configureAlgo(): mParamBufferDesc.nSize=%d", mParamBufferDesc.nSize);
		OstTraceFiltInst1(TRACE_FLOW, "configureAlgo(): mInternalBufferDesc.nSize=%d", mInternalBufferDesc.nSize);
		OstTraceFiltInst1(TRACE_FLOW, "configureAlgo(): pProxyComponent->mSendParamToMpc.get()=%d", pProxyComponent->mSendParamToMpc.get());
		if((mParamBufferDesc.nSize || mInternalBufferDesc.nSize/*internal_buffer_PoolId*/)&& pProxyComponent->mSendParamToMpc.get())
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
			param_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, param_buffer_size, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);

			OstTraceFiltInst2(TRACE_FLOW, "configureAlgo(): Memory allocation for Param Buffer, address=0x%x, size=%d", (OMX_U32)param_buf_logical_addr, param_buffer_size);

			if(!param_buf_logical_addr)
			{
				//> VFM_Free(vfm_mem_ctxt, param_buf_logical_addr);
				vfm_mem_ctxt = 0;
				OstTraceInt0(TRACE_ERROR, "configureAlgo(): Memory allocation for Param Buffer failed");
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
			internal_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, internal_buffer_size, VFM_MEM_HWBUFFER_NONDSP, 256, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			OstTraceFiltInst2(TRACE_FLOW, "configureAlgo(): Memory allocation for Internal Buffers, address=0x%x, size=%d", (OMX_U32)internal_buf_logical_addr, internal_buffer_size);

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
				OstTraceInt0(TRACE_ERROR, "configureAlgo(): Memory allocation for Internal Buffers failed");
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
			linklist_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, BUFFER_LINKLIST_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			OstTraceFiltInst2(TRACE_FLOW, "configureAlgo(): Memory allocation for LinkedList Buffer, address=0x%x, size=%d", (OMX_U32)linklist_buf_logical_addr, BUFFER_LINKLIST_SIZE);

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
				OstTraceInt0(TRACE_ERROR, "configureAlgo(): Memory allocation for LinkedList Buffer failed");
				return OMX_ErrorInsufficientResources;
			}

			mLinkListBufferDesc.nSize = BUFFER_LINKLIST_SIZE;
			mLinkListBufferDesc.nLogicalAddress = (t_uint32)linklist_buf_logical_addr;
			mLinkListBufferDesc.nPhysicalAddress = (t_uint32)(VFM_GetPhysical(vfm_mem_ctxt, linklist_buf_logical_addr));
			mLinkListBufferDesc.nMpcAddress = (t_uint32)(VFM_GetMpc(vfm_mem_ctxt, linklist_buf_logical_addr));

		}

		if(!mDebugBufferDesc.nSize)
		{
			debug_buf_logical_addr  = VFM_Alloc(vfm_mem_ctxt, DEBUG_MEM_SIZE, VFM_MEM_CM_MPC_SDRAM16_ALLOC, CM_MM_ALIGN_256BYTES, 0/*prop like cacheable etc*/, __LINE__, (t_uint8 *)__FILE__);
			OstTraceFiltInst2(TRACE_FLOW, "configureAlgo(): Memory allocation for Debug Buffer, address=0x%x, size=%d",(OMX_U32)debug_buf_logical_addr,DEBUG_MEM_SIZE);

			if(!debug_buf_logical_addr)
			{
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
				OstTraceInt0(TRACE_ERROR, "configureAlgo(): Memory allocation for Debug Buffer failed");
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

	OstTraceFiltInst1(TRACE_API, "configureAlgo(): BEFORE ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_entry=%d", ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_entry);
	OstTraceFiltInst1(TRACE_API, "configureAlgo(): BEFORE ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_exit=%d", ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_exit);
	OstTraceFiltInst1(TRACE_API, "configureAlgo(): BEFORE ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_custom_parameters.atomic_count=%d", ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_custom_parameters.atomic_count);
	if(zero_param_desc_dh)
	{
		//Zero memory
		memset((t_uint8 *)(ps_ddep_vec_mpeg4_param_desc_dh), 0x0, sizeof(ts_ddep_vec_mpeg4_param_desc_dh));

		// Set MPEG4 Parameter In: MPC PARAM SET (only once)
		processingComp.set_pMecIn_parameters(pProxyComponent, &ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set.s_in_parameters, &(ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set.s_in_custom_parameters));

		//< store the pointer to the struct containing the fw perf statistics.
		//< this pointer is used in MPEG4Enc_NmfMpc_ProcessingComponent::getPerfData to fill the
		//< user supplied struct with the fw perf stats, so that fw perf figures could be calculated

		pProxyComponent->pFwPerfDataPtr = (OMX_PTR)&(ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set.s_out_perf_parameters);
		// Set MPEG4 Parameter Other and header

		set_pMecOther_parameters(&ps_ddep_vec_mpeg4_param_desc_dh->mpc_param_set, searchwinsize, mvfieldbuffersize, headerBufferPhysicalAddress, fwdRefBufferPhysicalAddress, destBufferPhysicalAddress, intraRefBufferPhysicalAddress_in, intraRefBufferPhysicalAddress_out, motionVectorBufferPhysicalAddress, searchWindowBufferPhysicalAddress, mvFieldBufferPhysicalAddress);

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

	// Set MPEG4 Parameter Other and header	//< FIXME
	ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_custom_parameters.atomic_count = atomic_write;
	ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_exit = atomic_write;
	OstTraceFiltInst1(TRACE_API, "configureAlgo(): AFTER ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_entry=%d", ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_entry);
	OstTraceFiltInst1(TRACE_API, "configureAlgo(): AFTER ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_exit=%d", ps_ddep_vec_mpeg4_param_desc_dh->host_param_set_atomic_exit);
	OstTraceFiltInst1(TRACE_API, "configureAlgo(): AFTER ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_custom_parameters.atomic_count=%d", ps_ddep_vec_mpeg4_param_desc_dh->host_param_set.s_in_custom_parameters.atomic_count);

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::configureAlgo()");

	return OMX_ErrorNone;
}


OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::deInstantiate()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Enc_NmfMpc_ProcessingComponent::deInstantiate()");
        OMX_BOOL isMetaDataUsed =pProxyComponent->mParam.getStoreMetadataInBuffers();
	ENS_Port    *pt_port_in, *pt_port_out;
	pt_port_in = pProxyComponent->getPort(VPB+0);
	pt_port_out = pProxyComponent->getPort(VPB+1);

	vos_written = NOT_WRITTEN;

	/* +Change start from CR 399938 */
    if (isMetaDataUsed)
    {
		pProxyComponent->mParam.destroyMetaDataList();
    }
    /* -Change end from CR 399938 */

	RETURN_XXX_IF_WRONG((ENS::unbindComponentFromHost( getNmfFsmInitItf())==OMX_ErrorNone), OMX_ErrorUndefined);

	// Added by Pascal
	RETURN_XXX_IF_WRONG((ENS::unbindComponentAsynchronous (ddHandle, "me")==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG((ENS::unbindComponent (ddHandle ,"componentfsm")==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG((ENS::unbindComponent (ddHandle ,"genericfsm")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::unbindComponentAsynchronous (ddHandle ,"codec_algo")==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG((ENS::unbindComponent(ddHandle ,"codec_update_algo")==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG((ENS::unbindComponentAsynchronous (ddHandle ,"iSendLogEvent")==OMX_ErrorNone), OMX_ErrorUndefined);
	
	RETURN_XXX_IF_WRONG((ENS::unbindComponent (brcHandle, "osttrace")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::unbindComponent (ddHandle ,"iBrc")==OMX_ErrorNone), OMX_ErrorUndefined);
	RETURN_XXX_IF_WRONG((ENS::destroyNMFComponent(brcHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
	brcHandle = VFM_INVALID_CM_HANDLE;

	// Bind it Asynchronously to avoid to nest execution of the  Datadep  "process" function
	RETURN_XXX_IF_WRONG((ENS::unbindComponentAsynchronous (ddHandle, "iResource")==OMX_ErrorNone), OMX_ErrorUndefined);

	// Added for Power Management
	RETURN_XXX_IF_WRONG((ENS::unbindComponent (ddHandle, "iSleep")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::unbindComponent(mNmfAlgoHandle,"iBuffer")== OMX_ErrorNone),OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::unbindComponent(mNmfAlgoHandle,"iMtf")== OMX_ErrorNone),OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::unbindComponent(mNmfAlgoHandle,"iInterrupt")== OMX_ErrorNone),OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::unbindComponent (ddHandle, "osttrace")==OMX_ErrorNone), OMX_ErrorUndefined);
	

	RETURN_XXX_IF_WRONG((ENS::unbindComponent (mNmfComponentFsmLib, "osttrace")==OMX_ErrorNone), OMX_ErrorUndefined);

	RETURN_XXX_IF_WRONG((ENS::destroyNMFComponent(mNmfAlgoHandle) == OMX_ErrorNone),OMX_ErrorUndefined);
	mNmfAlgoHandle = VFM_INVALID_CM_HANDLE;

	RETURN_XXX_IF_WRONG((ENS::destroyNMFComponent(mNmfRmHandle) == OMX_ErrorNone),OMX_ErrorUndefined);
	mNmfRmHandle = VFM_INVALID_CM_HANDLE;

	RETURN_XXX_IF_WRONG((ENS::destroyNMFComponent(cOSTTrace) == OMX_ErrorNone),OMX_ErrorUndefined);


	//  UnBind HOST to DD
	if(ddHandle != VFM_INVALID_CM_HANDLE)
	{
		RETURN_XXX_IF_WRONG((ENS::unbindComponentFromHost(&iDdep)==OMX_ErrorNone), OMX_ErrorUndefined);
		RETURN_XXX_IF_WRONG((ENS::unbindComponentFromHost(getNmfSendCommandItf())==OMX_ErrorNone), OMX_ErrorUndefined);

		if(pt_port_in->isEnabled()==OMX_TRUE)
		{
			RETURN_XXX_IF_WRONG((ENS::unbindComponent(getNmfSharedBuf(0),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
			RETURN_XXX_IF_WRONG((ENS::unbindComponent(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
		}
		else
		{
			RETURN_XXX_IF_WRONG((ENS::unbindComponentToVoid(ddHandle,"inputport")==OMX_ErrorNone), OMX_ErrorUndefined);
		}

		if(pt_port_out->isEnabled()==OMX_TRUE)
		{
			RETURN_XXX_IF_WRONG((ENS::unbindComponent(getNmfSharedBuf(1),"mpc")==OMX_ErrorNone), OMX_ErrorUndefined);
			RETURN_XXX_IF_WRONG((ENS::unbindComponent(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
		}
		else
		{
			RETURN_XXX_IF_WRONG((ENS::unbindComponentToVoid(ddHandle,"outputport")==OMX_ErrorNone), OMX_ErrorUndefined);
		}

		RETURN_XXX_IF_WRONG((ENS::unbindComponentToHost(pProxyComponent->getOMXHandle(), ddHandle, "proxy")==OMX_ErrorNone), OMX_ErrorUndefined);

		RETURN_XXX_IF_WRONG((ENS::destroyNMFComponent(ddHandle)==OMX_ErrorNone), OMX_ErrorUndefined);
		ddHandle = VFM_INVALID_CM_HANDLE;
	}

	unregisterStubsAndSkels();

	OstTraceFiltInst0(TRACE_API, "<= MPEG4Enc_NmfMpc_ProcessingComponent::deInstantiate()");

	return OMX_ErrorNone;
}

void MPEG4Enc_NmfMpc_ProcessingComponent::registerStubsAndSkels()
{
	CM_REGISTER_STUBS(video_mpeg4enc_cpp);
}

void MPEG4Enc_NmfMpc_ProcessingComponent::unregisterStubsAndSkels()
{
	CM_UNREGISTER_STUBS(video_mpeg4enc_cpp);
}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::applyConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE MPEG4Enc_NmfMpc_ProcessingComponent::errorRecoveryDestroyAll()
{
	stop();
	return deInstantiate();
}


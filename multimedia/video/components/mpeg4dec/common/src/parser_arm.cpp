/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifndef SOFT_DECODER 
#include "mpeg4dec/arm_nmf/parser.nmf"
#include "SharedBuffer.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4dec_arm_nmf_parser_src_parserTraces.h"
#endif

#else
#include "mpeg4dec/arm_nmf/parser_arm.nmf"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg4dec_arm_nmf_parser_arm_src_parser_armTraces.h"
#endif
#endif

#ifdef __CACHE_OPTI
#include "VFM_Memory.h"
#endif

#include "string.h"
//#include "stdio.h"
unsigned int ARM_PARSER_NMF::msk[33]= {0x00000000, 0x00000001, 0x00000003, 0x00000007,
			0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
			0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
			0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
			0x0000ffffL,0x0001ffffL,0x0003ffffL,0x0007ffffL,
			0x000fffffL,0x001fffffL,0x003fffffL,0x007fffffL,
			0x00ffffffL,0x01ffffffL,0x03ffffffL,0x07ffffffL,
			0x0fffffffL,0x1fffffffL,0x3fffffffL,0x7fffffffL,0xffffffffUL};

#ifndef SOFT_DECODER
OMX_U32 parserFrameIndex = 0;
#endif

t_nmf_error METH(construct)()
{
	//printf("\nPARSER_MPEG4::construct");
	return NMF_OK;
}

void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
	int i=0;
	OstTraceInt0(TRACE_FLOW, "fsmInit setTunnelStatus");
	if (portIdx == -1)
    {
        for (i=0; i<2; i++)
        {
            if (isTunneled & (1<<i))
            {
                mPorts[i].setTunnelStatus(true);
			}
            else
            {
                mPorts[i].setTunnelStatus(false);
			}
        }
    }
    else
    {
        if (isTunneled & (1<<portIdx))
        {
			mPorts[portIdx].setTunnelStatus(true);
        }
        else
        {
			mPorts[portIdx].setTunnelStatus(false);
        }
    }
	//printf("\nPARSER_MPEG4::setTunnelStatus exit");
}
//void METH(fsmInit)(t_uint16 portsDisabled,t_uint16 portsTunneled)
void METH(fsmInit)(fsmInit_t fsm_init)
{
    OMX_BUFFERHEADERTYPE * mBufIn_list[VFM_HEADER_FIFO_COUNT] ;
	OMX_BUFFERHEADERTYPE * mBufOut_list[VFM_HEADER_FIFO_COUNT];
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec Parser: fsmInit()");

	for(int i=0;i<VFM_HEADER_FIFO_COUNT;i++)
	{
			mBufIn_list[i] = &mBufIn[i] ;  
			mBufOut_list[i] = &mBufOut[i] ;
	}
    if (fsm_init.traceInfoAddr)
	{
        setTraceInfo(fsm_init.traceInfoAddr, fsm_init.id1); 
	}  
    mPorts[0].init(InputPort,  false, true, 0, mBufIn_list, VFM_HEADER_FIFO_COUNT, &inputport, 0, (fsm_init.portsDisabled&1),(fsm_init.portsTunneled&1), this);
    mPorts[1].init(OutputPort, false, true, 0, mBufOut_list, VFM_HEADER_FIFO_COUNT, &outputport, 1, ((fsm_init.portsDisabled>>1)&1), ((fsm_init.portsTunneled>>1)&1),this);  //jitender to be checked

    init(2, mPorts, &proxy, &me, 0); 
	initParser();
	//Configure ports in non reset mode for buffer attributes
	mPorts[0].setBufferResetReqStatus(false);
	mPorts[1].setBufferResetReqStatus(false);

	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Parser: fsmInit()");
}

//+ER 354962
#ifdef SOFT_DECODER
void METH(start)()
{

}

void METH(stop)()
{
	OstTraceFiltInst0(TRACE_API, "=> MPEG4Dec Parser: stop()");
	mMutexPendingCommand->MutexLock();
	//>if(mNbPendingCommands!=0)
		//>mpeg4dec_armnmf_parser_assert(OMX_ErrorUndefined, __LINE__, OMX_FALSE);
	mMutexPendingCommand->MutexUnlock();
	OstTraceFiltInst0(TRACE_API, "<= MPEG4Dec Parser: stop()");
}

#if 0
void mpeg4dec_armnmf_parser_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal)
{
    if (OMX_ErrorNone != omxError)
    {
        OstTraceFiltInst2(TRACE_ERROR, "mpeg4dec_armnmf_parser_assert: errorType : 0x%x error line no %d\n", omxError,line);
        NMF_LOG("mpeg4dec_armnmf_parser_assert: errorType : 0x%x error line %d\n", omxError,line);
        if (isFatal)
        {
            DBC_ASSERT(0==1);
        }
    }
}
#endif

void mpeg4dec_arm_nmf_parser_arm::addPendingCommand()
{
	//No filtered traces here! Otherwise SegFault
	OstTraceFiltInst0(TRACE_API, "=> mpeg4dec_arm_nmf_parser_arm::addPendingCommand()");
	//>OstTraceInt1(TRACE_FLOW, "mNbPendingCommands BEFORE = %d", mNbPendingCommands);
	if (mNbPendingCommands == 0) {
		// lock the mutex as we now wait for feedback
		mMutexPendingCommand->MutexLock();
	}
	mNbPendingCommands++;
	OstTraceFiltInst1(TRACE_API, "<= mpeg4dec_arm_nmf_parser_arm::addPendingCommand() mNbPendingCommands = %d", mNbPendingCommands);
}

void mpeg4dec_arm_nmf_parser_arm::pendingCommandAck()
{
	OstTraceFiltInst0(TRACE_API, "=> mpeg4dec_arm_nmf_parser_arm::pendingCommandAck()");
	OstTraceFiltInst1(TRACE_FLOW, "mNbPendingCommands BEFORE = %d", mNbPendingCommands);
/*	if(!(mNbPendingCommands > 0))
		mpeg4dec_armnmf_parser_assert(OMX_ErrorUndefined, __LINE__, OMX_TRUE);
*/
	mNbPendingCommands--;
	if(mNbPendingCommands == 0) {
		// unlock the mutex as we do not wait for any other feedback
		mMutexPendingCommand->MutexUnlock();
	}
	OstTraceFiltInst1(TRACE_API, "<= mpeg4dec_arm_nmf_parser_arm::pendingCommandAck() mNbPendingCommands = %d", mNbPendingCommands);
}
#endif
//-ER 354962

void METH(reset)(){}  ;
void METH(disablePortIndication)(t_uint32 portIdx) 
{
	OstTraceInt0(TRACE_FLOW, "<=> MPEG4Dec Parser: disablePortIndication()");
	//Setting do_not_parse as OMX_FALSE");
	//+ER329550
	 do_not_parse = OMX_FALSE;
	 //-ER329550
	//+ER426245
	if(portIdx == 1) {
		port_output_was_disabled = 1;
	}
	//-ER426245
} ;
void METH(enablePortIndication)(t_uint32 portIdx) {
	OstTraceInt0(TRACE_FLOW, "<=> MPEG4Dec Parser: enablePortIndication()");
} ;
void METH(flushPortIndication)(t_uint32 portIdx) 
{
	//+ER329550
	OstTraceInt0(TRACE_FLOW, "MPEG4Dec Parser: flushPortIndication(): Setting do_not_parse as OMX_FALSE");
	do_not_parse = OMX_FALSE;
	//-ER329550
} ;
void METH(destroy)() {} ;

//called from MPEG4Dec_ProcessingComp::emptyThisBuffer
void METH(sendportSettings)(t_uint32 width,t_uint32 height,t_uint32 concealment_flag,t_uint32 sorenson_flag,t_uint8 aspect_ratio,t_uint8 color_primary,t_uint32 error_reporting_enable)
{	
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec Parser: sendportSettings()");
	OstTraceFiltInst2(TRACE_FLOW, "sendportSettings: width=%d height =%d", width, height);
	OstTraceFiltInst2(TRACE_FLOW, "sendportSettings: concealment_flag=%d aspect_ratio =%d", concealment_flag, aspect_ratio);
	OstTraceFiltInst2(TRACE_FLOW, "sendportSettings: color_primary=%d error_reporting_enable =%d", color_primary, error_reporting_enable);
	frame_info_width = width;
	frame_info_height = height;
    //+ER436807
    //We give the following parameters a default value based
    // on the values set by the user, so that if later no
    // width/height info is found in the header of the stream
    // then at least the default values are present.
    // In case of ER436807, the stream was a custom resolution
    // H263 P3 stream. When screen was powered off during playback
    // and then powered back ON, codec was re-constructed and
    // the first frame that was then passed to codec
    // was a P frame, which had UFEP = '000', so
    // no custom resolution could be found. As a result, the following
    // params took value 0 and a PortSettingsChanged event was raised!
    bit_stream->video_object_layer_width = width;
    bit_stream->video_object_layer_height = height;
    bit_stream->horizontal_size = width;
    bit_stream->vertical_size = height;
    //-ER436807
    AspectRatio=aspect_ratio;
    ColorPrimary=color_primary;
	config_concealmentflag = concealment_flag;
	bit_stream->flag_sorenson=sorenson_flag;
	error_reporting=error_reporting_enable;

//+ER 354962
#ifdef SOFT_DECODER
	addPendingCommand();
#endif
//-ER 354962
	iDdep.setConfig((t_uint16)error_reporting,1,0);
	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec Parser: sendportSettings()");
}
#ifdef SOFT_DECODER
void METH(fillThisHeader)(t_uint32 pHeader)
#else
void METH(fillThisHeader)(Buffer_p pHeader)
#endif
{
  OstTraceInt1(TRACE_FLOW, "=> MPEG4Dec Parser: fillThisHeader() pHeader = 0x%x", (unsigned int)pHeader);
  freeHeaderDesc((t_uint32*)pHeader);
  clearAllPendingRequestsToFree();
  OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Parser: fillThisHeader()");
}

void ARM_PARSER_NMF::freeHeaderDesc(t_uint32* pBuffer)
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec Parser: freeHeaderDesc()");
	OMX_U8 index;//headerDesc
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++)
    {
		if(headerDesc[index].mpcAddress == (OMX_U32) pBuffer)
        {
			if(error_reporting==1)  //error map
			{
               for(OMX_U8 i=0;i<225;i++)
				   *(errormap_ptr+i)= (*(errormap_ptr+i)) | headerDesc[index].pHeaderDesc->errormap[i];
			}
			if(error_reporting==2) //fast update  ::check whether it is cumulative or not
			{	
				(*(errormap_ptr+225))=(t_uint32)headerDesc[index].pHeaderDesc->mFirstGOB;
				(*(errormap_ptr+226))=(t_uint32)headerDesc[index].pHeaderDesc->mFirstMB ;
				(*(errormap_ptr+227))=(t_uint32)headerDesc[index].pHeaderDesc->mNumMBs ;
			}
			//printf("\nPARSER_MPEG4::freeHeaderDesc");
            headerDesc[index].isRequestedToFree = OMX_TRUE;
            break;
        }
    }
	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec Parser: freeHeaderDesc()");
}
void ARM_PARSER_NMF::clearAllPendingRequestsToFree()  //check whether needed or not
{
    OMX_U8 index;
	OstTraceInt0(TRACE_FLOW, "=> Parser: clearAllPendingRequestsToFree");

	for(index=0;index<VFM_HEADER_FIFO_COUNT;index++)
    {
        if(headerDesc[index].isRequestedToFree == OMX_TRUE)
        {
            headerDesc[index].isRequestedToFree = OMX_FALSE;
            headerDesc[index].isFree = OMX_TRUE;
        }
    }
}

void ARM_PARSER_NMF::fromProcComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
{

	OstTraceInt3(TRACE_FLOW, "=> MPEG4Dec Parser :: fromProcComp_emptyThisBuffer(): pBuffer = 0x%x, pBuffer->pBuffer = 0x%x, pBuffer->nTimeStamp = %d", (unsigned int)pBuffer, (unsigned int)pBuffer->pBuffer, pBuffer->nTimeStamp);
	OstTraceInt4(TRACE_FLOW, "... MPEG4Dec Parser :: fromProcComp_emptyThisBuffer(): pBuffer->nFilledLen = %d, pBuffer->nAllocLen = %d, pBuffer->nOffset = %d, pBuffer->nFlags = 0x%x", pBuffer->nFilledLen, pBuffer->nAllocLen, pBuffer->nOffset, pBuffer->nFlags);

//+ER362989
/*	 if(pBuffer->nFilledLen + 16 <= pBuffer->nAllocLen)
	 {
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 0), 0x00, 2);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 2), 0x01, 1);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 3), 0xB6, 1);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 4), 0x00, 2);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 6), 0x01, 1);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 7), 0xB6, 1);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 8), 0x00, 2);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 10), 0x00, 1);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 11), 0x20, 1);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 12), 0x00, 2);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 14), 0x00, 1);
		 memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 15), 0x20, 1);
	 }
*/
//-ER362989
	    OstTraceFiltInst0(TRACE_FLOW, "fromProcComp_emptyThisBuffer \n");
#ifdef __CACHE_OPTI
	if (save_memory_context)
	{
//+ER344873
#ifndef SOFT_DECODER 
        VFM_CacheClean(save_memory_context, pBuffer->pBuffer, pBuffer->nFilledLen);
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC VFM_CacheClean DONE ");
#endif
//-ER344873
		OstTraceInt0(TRACE_FLOW, "PARSER_MPC VFM_CacheClean DONE ");
	}
	else
	{
		OstTraceFiltInst0(TRACE_ERROR, "PARSER_MPC VFM_CacheClean FAIL as memorycontext is EMPTY ");
	}

	OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC after calling VFM_CacheClean");
#endif
        Component::deliverBuffer(0, pBuffer);
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Parser :: fromProcComp_emptyThisBuffer()");
}
void ARM_PARSER_NMF::fromProcComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
{
		OstTraceInt2(TRACE_FLOW, "=> MPEG4Dec Parser :: fromProcComp_fillThisBuffer(): pBuffer = 0x%x, pBuffer->pBuffer = 0x%x", (unsigned int)pBuffer, (unsigned int)pBuffer->pBuffer);
        pBuffer->nFlags = 0; //setting value of nFlags to zero
		pBuffer->nTimeStamp = 0; //Er 327354

#ifdef __CACHE_OPTI
        if (save_memory_context)
        {
	#ifndef SOFT_DECODER
		VFM_CacheClean(save_memory_context, pBuffer->pBuffer, pBuffer->nFilledLen);
	#endif
        }
#endif
        Component::deliverBuffer(1, pBuffer); 
		OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Parser :: fromProcComp_fillThisBuffer()");
}
void ARM_PARSER_NMF::fromSharedBufferComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	//Return Image buffer to proxy
	OstTraceInt2(TRACE_FLOW, "=> MPEG4Dec Parser :: fromSharedBufferComp_emptyThisBuffer(): pBuffer = 0x%x, pBuffer->pBuffer = 0x%x", (unsigned int)pBuffer, (unsigned int)pBuffer->pBuffer);
	OstTraceInt4(TRACE_FLOW, "... MPEG4Dec Parser :: fromSharedBufferComp_emptyThisBuffer(): pBuffer->nFilledLen = %d, pBuffer->nAllocLen = %d, pBuffer->nOffset = %d, pBuffer->nFlags = 0x%x", pBuffer->nFilledLen, pBuffer->nAllocLen, pBuffer->nOffset, pBuffer->nFlags);
	OMX_U32 flags = pBuffer->nFlags;
	OstTraceFiltInst0(TRACE_FLOW, "Return Image_buffer_to_proxy \n");
#ifdef __CACHE_OPTI
	OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC before calling CacheInvalidate");
	if (save_memory_context)
	{
#ifndef SOFT_DECODER 
		VFM_CacheInvalidate(save_memory_context, pBuffer->pBuffer, pBuffer->nFilledLen);
#else
//+ER344873
		VFM_CacheClean(save_memory_context, pBuffer->pBuffer, pBuffer->nFilledLen);
//-ER344873
#endif
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC VFM_CacheInvalidate DONE ");
	}
	else
	{
		OstTraceFiltInst0(TRACE_ERROR, "PARSER_MPC VFM_CacheInvalidate FAILED as memory context is EMPTY ");
	}
#endif
	
#ifndef SOFT_DECODER

	OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Parser: fromSharedBufferComp_emptyThisBuffer(): *pBuffer->pBuffer + 0 = 0x%x", *(pBuffer->pBuffer + 0));
	OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Parser: fromSharedBufferComp_emptyThisBuffer(): *pBuffer->pBuffer + 1 = 0x%x", *(pBuffer->pBuffer + 1));
	OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Parser: fromSharedBufferComp_emptyThisBuffer(): *pBuffer->pBuffer + 2 = 0x%x", *(pBuffer->pBuffer + 2));
	OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Parser: fromSharedBufferComp_emptyThisBuffer(): *pBuffer->pBuffer + 3 = 0x%x", *(pBuffer->pBuffer + 3));
	OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Parser: fromSharedBufferComp_emptyThisBuffer(): *pBuffer->pBuffer + 4 = 0x%x", *(pBuffer->pBuffer + 4));

	if(bImmediateIFlag)
	{
		bImmediateIFlag = 0;

		memcpy((t_uint8 *)pBufferForI, pBuffer->pBuffer, pBuffer->nFilledLen);
	
		OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Parser: fromSharedBufferComp_emptyThisBuffer(): pBufferForI + 0 = 0x%x", *((t_uint8 *)pBufferForI + 0));
		OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Parser: fromSharedBufferComp_emptyThisBuffer(): pBufferForI + 1 = 0x%x", *((t_uint8 *)pBufferForI + 1));
		OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Parser: fromSharedBufferComp_emptyThisBuffer(): pBufferForI + 2 = 0x%x", *((t_uint8 *)pBufferForI + 2));
		OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Parser: fromSharedBufferComp_emptyThisBuffer(): pBufferForI + 3 = 0x%x", *((t_uint8 *)pBufferForI + 3));
		OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Parser: fromSharedBufferComp_emptyThisBuffer(): pBufferForI + 4 = 0x%x", *((t_uint8 *)pBufferForI + 4));
	}

#endif	
	
	//printf("\nPARSER_MPEG4::Return Image_buffer_to_proxy enter %x nFlags =%x",pBuffer->pBuffer,pBuffer->nFlags);
    Component::returnBufferAsync(1,pBuffer);
	if(flags & OMX_BUFFERFLAG_EOS)
	{
		proxy.eventHandler(OMX_EventBufferFlag, 1, flags);
	}
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Parser :: fromSharedBufferComp_emptyThisBuffer()");
}

void ARM_PARSER_NMF::fromSharedBufferComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
{
	OstTraceInt2(TRACE_FLOW, "=> MPEG4Dec Parser :: fromSharedBufferComp_fillThisBuffer(): pBuffer = 0x%x, pBuffer->pBuffer = 0x%x", (unsigned int)pBuffer, (unsigned int)pBuffer->pBuffer);

	OstTraceFiltInst0(TRACE_FLOW, "Return_Bitstream_buffer_to_proxy \n");
	//Return Bitstream buffer to proxy
	DQIndex++;
	DQIndex %= VFM_HEADER_FIFO_COUNT;
    //printf("\nPARSER_MPEG4::Return_Bitstream_buffer_to_proxy enter %x",pBuffer->pBuffer);
    Component::returnBufferAsync(0,pBuffer);
	//printf("\nPARSER_MPEG4::Return_Bitstream_buffer_to_proxy exit");
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Parser :: fromSharedBufferComp_fillThisBuffer()");
}
//called from MPEG4Dec_ProcessingComp::codecConfigure
void METH(memoryRequirements)(t_uint32 deblocking_param_struct[10],ts_ddep_buffer_descriptor paramBufferDesc,
							  ts_ddep_buffer_descriptor linkListBufferDesc,ts_ddep_buffer_descriptor debugBufferDesc,
							  t_uint16 error_reporting_enable,t_uint16 *ptr,t_uint8 BufCountActual,t_bool disable,void *mpc_vfm_mem_ctxt)
{
	OMX_U8 index;
	OstTraceInt2(TRACE_FLOW, "memoryRequirements:: BufCountActual=%d error_reporting_enable =%d \n",BufCountActual,error_reporting_enable);
	OstTraceInt1(TRACE_FLOW, "memoryRequirements::  ARM_LOAD =%d \n",disable);
	//printf("\nPARSER_MPEG4::memoryRequirements enter error_reporting_enable =%d BufCountActual =%d",error_reporting_enable,BufCountActual);
	//printf("\nPARSER_MPEG4::memoryRequirements enter internal bufferadresss %x",deblocking_param_struct[9]);
	mParamBufferDesc = paramBufferDesc;
    mLinkListBufferDesc = linkListBufferDesc;
    mDebugBufferDesc = debugBufferDesc;
	save_memory_context = mpc_vfm_mem_ctxt;
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++)
    {
		//+ER426245
		//We check, in case the ports were enabled after disable,
		// whether the header buffer is already in use. In case it is,
		// we do not set the fields.
		OstTraceInt4(TRACE_FLOW, "memoryRequirements: port_output_was_disabled %d index %d headerDesc[index].isFree %d headerDesc[index].isRequestedToFree %d", port_output_was_disabled, index, headerDesc[index].isFree, headerDesc[index].isRequestedToFree);

		if(!port_output_was_disabled || (port_output_was_disabled &&  headerDesc[index].isFree != OMX_FALSE))
		{
			OstTraceInt0(TRACE_FLOW, "memoryRequirements: Setting (headerDesc[index].isFree = OMX_TRUE)");
			headerDesc[index].isFree = OMX_TRUE;
			headerDesc[index].isRequestedToFree = OMX_FALSE;
		}
		//-ER426245
        headerDesc[index].mpcAddress = (OMX_U32)(paramBufferDesc.ADD_RESS
                                                 + sizeof(ts_ddep_vdc_mpeg4_param_desc)/MULTIPLIER
                                                 + index* sizeof(ts_ddep_vdc_mpeg4_header_desc)/MULTIPLIER);
        headerDesc[index].pHeaderDesc= (ts_ddep_vdc_mpeg4_header_desc*)(paramBufferDesc.nLogicalAddress
                                                                         + sizeof(ts_ddep_vdc_mpeg4_param_desc)
                                                                         + index* sizeof(ts_ddep_vdc_mpeg4_header_desc));
    }  
	//+ER426245
	port_output_was_disabled = 0;
	//-ER426245

	pMpeg4ParamIn = (ts_t1xhv_vdc_mpeg4_param_in *)(paramBufferDesc.nLogicalAddress
                                                    + sizeof(ts_t1xhv_vdc_frame_buf_in)
													+ sizeof(ts_t1xhv_vdc_frame_buf_out)
													+ sizeof(ts_t1xhv_vdc_internal_buf)
													+ sizeof(ts_t1xhv_bitstream_buf_pos)
													+ sizeof(ts_t1xhv_bitstream_buf_pos));
	error_reporting=error_reporting_enable;
	errormap_ptr=ptr;
	InternalBufferAddress=deblocking_param_struct[9];
	//printf("\nPARSER_MPEG4::memoryRequirements enter InternalBufferAddress %x",InternalBufferAddress);

//+ER 354962
#ifdef SOFT_DECODER
	addPendingCommand();
#endif
//-ER 354962
    iDdep.setNeeds((t_uint16)error_reporting,BufCountActual,mLinkListBufferDesc,mDebugBufferDesc,(t_uint16)disable);

//+ER 354962
#ifdef SOFT_DECODER
	addPendingCommand();
#endif
//-ER 354962
	iDdep.setParameter(deblocking_param_struct,ID_VDC_MPEG4,mParamBufferDesc,mpc_vfm_mem_ctxt);
}
#if 1
#ifndef SOFT_DECODER
void METH(sendImmediateIFlag)(t_uint8 immediate_i_flag)
{
	OstTraceInt1(TRACE_FLOW, "=> MPEG4Dec Parser :: sendImmediateIFlag(): immediate_i_flag = %d", immediate_i_flag);

	bImmediateIFlag = immediate_i_flag;

	//iDdep.sendImmediateIFlagToMpc(immediate_i_flag);
    //+ER425364
    iDdep.setConfig(0,3,immediate_i_flag);
    //-ER425364
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Parser :: sendImmediateIFlag()");
}
void METH(provideBufferForI)(t_uint32 buffer_i_frame[2])
{
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec Parser: provideBufferForI()");

	OstTraceInt2(TRACE_FLOW, "provideBufferForI(): buffer_i_frame[0] (Physical) = 0x%x, buffer_i_frame[1] (Logical) = 0x%x", buffer_i_frame[0], buffer_i_frame[1]);

	pBufferForI = (t_uint32)(buffer_i_frame[1]);

	//iDdep.setBufferForI(buffer_i_frame[0]);
    //setconfig interface modified to be used for immdiate I frame release
    //+ER425364
    iDdep.setConfig(0,2,buffer_i_frame[0]);
    //-ER425364
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Parser: provideBufferForI()");
}
#else
void METH(sendImmediateIFlag)(t_uint8 immediate_i_flag) { }
void METH(provideBufferForI)(t_uint32 buffer_i_frame[2]) { }
#endif
#endif

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param)
{
	OstTraceFiltInst2(TRACE_FLOW, "=> MPEG4Dec Parser: sendCommand(): cmd = %d, param = %d", cmd, param);
	if((StartCodeCount) && ((cmd ==  OMX_CommandFlush) || (cmd == OMX_CommandPortDisable) || 
							((cmd == OMX_CommandStateSet) && (param == (t_uword)OMX_StateIdle))))
	{
		 StartCodeCount--;
		 //printf("\nPARSER_MPEG4::sendCommand free header StartCodeCount=%d ",StartCodeCount);
		 if(pDesc)
		 {
			  freeHeaderDesc((t_uint32*)(pDesc->mpcAddress));
			  clearAllPendingRequestsToFree();  
			  pDesc=NULL;
		 }
	} 
	//+ER329550
	if((cmd ==  OMX_CommandFlush) || (cmd == OMX_CommandPortDisable))
	{
		OstTraceInt0(TRACE_FLOW, "MPEG4Dec Parser: sendCommand(): Setting do_not_parse as OMX_TRUE");
		do_not_parse=OMX_TRUE;
	}
	//-ER329550
    Component::sendCommand(cmd, param) ;

//+ER 354962
#ifdef SOFT_DECODER
	addPendingCommand();
#endif
//-ER 354962
	iDdep.sendCommandX(cmd,param);
	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec Parser: sendCommand()");
}
OMX_BOOL ARM_PARSER_NMF::buffer_available_atinput()
{
    OstTraceFiltInst0(TRACE_FLOW, "buffer_available_atinput \n");
	if((pBufferIn == NULL) && (mPorts[0].queuedBufferCount()))
	{
		OstTraceInt0(TRACE_FLOW, "buffer_removed_atinput \n");
        pBufferIn = mPorts[0].dequeueBuffer();
		return OMX_TRUE;
	}
	else if(pBufferIn)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}
OMX_BOOL ARM_PARSER_NMF::buffer_available_atheaderport()
{
	OstTraceInt0(TRACE_FLOW, "=> Parser: buffer_available_atheaderport");
	if(pDesc==NULL)
	{
		//printf("\n nPARSER_MPEG4::buffer_available_atheaderport");
        pDesc = getFreeHeaderDesc();
		if(pDesc==(ts_vfm_vdc_mpeg4_header_desc *)NULL)
		{
			return OMX_FALSE;
		}
		pDesc->pHeaderDesc->InBuffCnt=0;
	}
    return OMX_TRUE;
}
OMX_BOOL ARM_PARSER_NMF::StartCodeFound()
{
	OMX_U32 temp =0;
	OMX_U32 bitCount = 0;
	OMX_BOOL scNotFound = OMX_TRUE;
	OMX_BOOL endOfStreamNotReached = OMX_TRUE;
	OMX_U32 bitShiftForResync = 0;
    if(pBufferIn->nFilledLen==0)
	{
		 OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::BUFFER_WITH_ZERO_FILLEDLENGTH\n");
		 return OMX_FALSE;
	}
	initParserBitstream(pBufferIn);
	if (bit_stream->flag_sorenson) {
		bit_stream->codingType=VFM_MPEG4_SORENSON;
		 scNotFound = OMX_FALSE;
	}
	if(bit_stream->codingType == VFM_MPEG4_SP)
		{
		bitShiftForResync = MP4_SC_LENGHT_IN_BITS - iVopHeader->iResyncMarkerSize;
		}
	else if(bit_stream->codingType == VFM_MPEG4_SH)
		{
		bitShiftForResync = MP4_SC_LENGHT_IN_BITS - iShortHeader->iResyncMarkerSize;
		}
	else
		{
		//check for sorenson
		}

	temp = getbits(32);
    while((scNotFound) && (endOfStreamNotReached))
	{
		bitCount = getbitscount();
        if( bitCount > (pBufferIn->nFilledLen<<3))
		{
				endOfStreamNotReached = OMX_FALSE;
				 if(!bit_stream->isVolHeaderFound)
				 {
					 OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::BUFFER_WITH_ZERO_FILLEDLENGTH_FORCED\n");
					 pBufferIn->nFilledLen=0;
				 }
                     return OMX_FALSE;
        }
		else
		{
		    //first check for MPEG4 SP headers
			/* visual object sequence */
			if ( ((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))
				&& (temp == VIS_OBJ_SEQ_CODE))
				{ /* first 32 bits */
                OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::VIS_OBJ_SEQ_CODE found\n");
                }
			//KVISUAL_OBJECT_SEQUENCE_END_CODE
			else if ( ((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))
				&& (temp == KVISUAL_OBJECT_SEQUENCE_END_CODE))
				{ /* first 32 bits */
				OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::KVISUAL_OBJECT_SEQUENCE_END_CODE found\n");
                }
			/* video object layer */
			else if (((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))
				&& (temp >= KVIDEO_OBJECT_LAYER_START_CODE_SMALLEST && temp <= KVIDEO_OBJECT_LAYER_START_CODE_GREATEST))
				{
				OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::VOL found\n");
				scNotFound = OMX_FALSE;
				bit_stream->codingType = VFM_MPEG4_SP;
                }
			else if (((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SH))&&
				( (temp >> (MP4_SC_LENGHT_IN_BITS - SH_SC_LENGTH_IN_BITS)) == SH_CODE)) // if not VOL then SH can be expected
				{
                OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::SH found\n");
				scNotFound = OMX_FALSE;
				bit_stream->codingType = VFM_MPEG4_SH;
				
				}
			else if( ((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))&&
				(temp == KVISUAL_OBJECT_START_CODE))
				{
                OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::KVISUAL_OBJECT_START_CODE found\n");
                }
			else if( ((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))&&
				(temp == KVOP_START_CODE))
				{
                OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::VOP found\n");
				scNotFound = OMX_FALSE;
				}
			else if(((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))&&
				(temp == KGROUP_OF_VOP_START_CODE))
				{
                OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::GOV found\n");
                }
			else if( (bit_stream->codingType == VFM_MPEG4_SP) &&
					(bit_stream->resync_marker_disable == 1) &&
					((temp >> bitShiftForResync) == KRESYNC_MARKER )
					)
				{
                OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::SP Resync marker found\n");
                }
			else if((bit_stream->codingType == VFM_MPEG4_SH) &&
					( (temp >> bitShiftForResync) == KRESYNC_MARKER)
					)
				{
                OstTraceFiltInst0(TRACE_FLOW, "StartCodeFound::SH Resync marker found\n");
                }



   			if (scNotFound == OMX_TRUE)
    	 	{
    	   		temp = temp << 8 | (OMX_U8)(getbits(8));
    	 	}
    	}
	}
    initParserBitstream(pBufferIn);
    return OMX_TRUE;
}
//$ check if mpeg4dec should also check for o/p buffers, only case is ceasing
void METH(process)()
{
	//+ER329550
	if(do_not_parse){
		OstTraceInt0(TRACE_FLOW, "MPEG4Dec Parser: process(): return because (do_not_parse==OMX_TRUE)");
		return;}//printf("\nPARSER_MPEG4::process enter");
	//-ER329550
    OMX_BUFFERHEADERTYPE *pBufferOut;
	while(mPorts[1].queuedBufferCount())
	{
        pBufferOut = mPorts[1].dequeueBuffer();
        fillthisbufferSharedBufOut.fillThisBuffer(pBufferOut);
	}
	while(buffer_available_atinput())
	{
		//+ER374321,+ER371424
		if((pBufferIn->nFilledLen < 4) && (!(pBufferIn->nFlags & OMX_BUFFERFLAG_EOS)))
		{
			//In case the input buffer is less than 4 bytes (including empty buffers, nFilledLen==0) return it back to user
			mPorts[0].returnBuffer(pBufferIn);
			pBufferIn = NULL;
		}
		else
		{
			break;
		}
		//-ER374321,-ER371424
	}
    if((pBufferIn) && (buffer_available_atheaderport()))
	{
		//printf("\nPARSER_MPEG4::process StartCodeCount =%X pBufferIn->nFlags =%X",StartCodeCount,pBufferIn->nFlags);
		if(( (StartCodeCount==0) && (pBufferIn->nFlags & OMX_BUFFERFLAG_ENDOFFRAME) ) || (StartCodeFound()))  
		{
//+ER362989
			if(pBufferIn->nFilledLen + 16 <= pBufferIn->nAllocLen)
			{
				if(bit_stream->codingType == VFM_MPEG4_SP || bit_stream->codingType == VFM_MPEG4_ASP || bit_stream->codingType == VFM_MPEG4_SP_ASP)
				{
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 0), 0x00, 2);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 2), 0x01, 1);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 3), 0xB6, 1);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 4), 0x00, 2);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 6), 0x01, 1);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 7), 0xB6, 1);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 8), 0x00, 2);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 10), 0x01, 1);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 11), 0xB6, 1);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 12), 0x00, 2);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 14), 0x01, 1);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 15), 0xB6, 1);
				}
				else if(bit_stream->codingType == VFM_MPEG4_SH || bit_stream->codingType == VFM_MPEG4_SORENSON)
				{
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 0), 0x00, 3);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 3), 0xFC, 1);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 4), 0x00, 3);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 7), 0xFC, 1);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 8), 0x00, 3);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 11), 0xFC, 1);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 12), 0x00, 3);
					memset ( (void *)((pBufferIn->pBuffer+pBufferIn->nOffset+pBufferIn->nFilledLen) + 15), 0xFC, 1);
				}
			}
//-ER362989
			StartCodeCount++;
				if(StartCodeCount == 2)
				{
					StartCodeCount--;
					//printf("\nPARSER_MPEG4::emptyThisHeader StartCodeCount =%X",StartCodeCount);
					if(pDesc)
#ifdef SOFT_DECODER
						//+ER 354962
						addPendingCommand();
						//-ER 354962
						emptythisheader.emptyThisHeader((t_uint32 )(pDesc->mpcAddress));
#else
						emptythisheader.emptyThisHeader((Buffer_p )(pDesc->mpcAddress));
#endif
					pDesc=NULL;
					buffer_available_atheaderport();
				}
                processBuffer();
		}
		else if(pBufferIn->nFilledLen==0 && (!(pBufferIn->nFlags & OMX_BUFFERFLAG_EOS)))
		{
			OstTraceFiltInst0(TRACE_FLOW, "process::return forced empty buffers directly to user\n");
            mPorts[0].returnBuffer(pBufferIn);
			pBufferIn=NULL;

		}
		else if(pBufferIn->nFlags & OMX_BUFFERFLAG_EOS)
		{	
			//printf("\nPARSER_MPEG4::process EOS StartCodeCount =%X length=%d ",StartCodeCount,pBufferIn->nFilledLen);
			if(!bit_stream->isVolHeaderFound)
				 {
					 OstTraceFiltInst0(TRACE_FLOW, "process::ALLOCATE_HEADER_FOR_EMPTY_BUFFER_TO_FINISH\n");
					 buffer_available_atheaderport();
				 }
			//case where EOS received and we have a SC
			if(StartCodeCount)
			{
				//printf("\nPARSER_MPEG4::emptyThisHeader StartCodeCount =%X",StartCodeCount);
				if(pDesc)
#ifdef SOFT_DECODER
						//+ER 354962
						addPendingCommand();
						//-ER 354962
						emptythisheader.emptyThisHeader((t_uint32 )(pDesc->mpcAddress));
#else
						emptythisheader.emptyThisHeader((Buffer_p )(pDesc->mpcAddress));
#endif
				pDesc=NULL;
			}
			if(pBufferIn->nFilledLen==0 && StartCodeCount)
			{
				buffer_available_atheaderport();
				updateMpeg4Params();
			   // if(pDesc)
					updateHeaderInfos(pDesc->pHeaderDesc,0);
			}
		}
        if(pBufferIn)
		{
		     QIndex %= VFM_HEADER_FIFO_COUNT;
		     pBufferInArray[QIndex++]=pBufferIn;
		     //printf("\nPARSER_MPEG4::process BufferInArray =%X QIndex =%d ",pBufferInArray[QIndex-1]->pBuffer,QIndex);
		     emptythisbufferSharedBufIn.emptyThisBuffer(pBufferIn);
		     if(pDesc==NULL)
		     	return;
		     pDesc->pHeaderDesc->InBuffCnt++;
		     //printf("\nPARSER_MPEG4::process FLAG= %X StartCodeCount =%d",pBufferIn->nFlags,StartCodeCount);
		     if((pBufferIn->nFlags & OMX_BUFFERFLAG_EOS )||( pBufferIn->nFlags & OMX_BUFFERFLAG_ENDOFFRAME))
		     {
		     	if(StartCodeCount)
		     	StartCodeCount--;
		     	//printf("\nPARSER_MPEG4::emptyThisHeader StartCodeCount =%X",StartCodeCount);
#ifdef SOFT_DECODER
						//+ER 354962
						addPendingCommand();
						//-ER 354962
						emptythisheader.emptyThisHeader((t_uint32 )(pDesc->mpcAddress));
#else
						emptythisheader.emptyThisHeader((Buffer_p )(pDesc->mpcAddress));
#endif
		     	pDesc=NULL;
		     }
		     pBufferIn=NULL;
		}
	}
	//printf("\nPARSER_MPEG4::process exit");
	//iSendLogEvent.eventProcess();
	if(mPorts[0].queuedBufferCount())
	{
		scheduleProcessEvent();
	}
	
}
void METH(InternalCopy)(t_uint16 InBuffCnt,t_uint32 offset)
{
    OstTraceFiltInst0(TRACE_FLOW, "InternalCopy \n");
	//printf("\n InternalCopy DQIndex= %d QIndex =%d",DQIndex,QIndex);
    for(t_uint8 i=0;i<InBuffCnt;i++)
	{
		//printf("\nPARSER_MPEG4::inputbuffers pBuffer =%X nFilledLen =%d offset= %d ",pBufferInArray[((DQIndex + i) % VFM_HEADER_FIFO_COUNT)]->pBuffer,pBufferInArray[((DQIndex + i) % VFM_HEADER_FIFO_COUNT)]->nFilledLen,offset);
	    memcpy((t_uint8 *)(InternalBufferAddress + offset),(t_uint8 *)(pBufferInArray[((DQIndex + i) % VFM_HEADER_FIFO_COUNT)]->pBuffer),(pBufferInArray[((DQIndex + i) % VFM_HEADER_FIFO_COUNT)]->nFilledLen));
		offset+=pBufferInArray[((DQIndex + i) % VFM_HEADER_FIFO_COUNT)]->nFilledLen;
    }
	//printf("\n PARSER_MPEG4::Copy exit InBuffCnt =%d offset=%d",InBuffCnt,offset);

//+ER 354962
#ifdef SOFT_DECODER
	addPendingCommand();
#endif
//-ER 354962
	iDdep.CopyComplete(offset,InBuffCnt);
}
void METH(eventProcess)()
{
	OstTraceFiltInst0(TRACE_FLOW, "eventProcess \n");
	//printf("\neventProcess entering");
	scheduleProcessEvent();
	//printf("\neventProcess exiting");
}
//called from process
ts_vfm_vdc_mpeg4_header_desc * ARM_PARSER_NMF::getFreeHeaderDesc()
{
    OMX_U8 index;
	OstTraceInt0(TRACE_FLOW, "=> Parser: getFreeHeaderDesc");
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++)
    {
        if(headerDesc[index].isFree == OMX_TRUE)
        {
            headerDesc[index].isFree = OMX_FALSE;
            return &headerDesc[index];
        }
    }

    return (ts_vfm_vdc_mpeg4_header_desc *)NULL;
}
//called from processBuffer
void ARM_PARSER_NMF::detectPortSettingsAndNotify(ts_ddep_vdc_mpeg4_header_desc *pDesc)
{
	OMX_BOOL has_changed = OMX_FALSE;
	OMX_U32 tempWidth,tempHeight;
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec Parser: detectPortSettingsAndNotify()");
//+ER426137

	tempWidth = bit_stream->video_object_layer_width;
	tempHeight = bit_stream->video_object_layer_height;
//LOGE("detectPortSettingsAndNotify New actual width (%d) New actual height (%d)",tempWidth,tempHeight);
//-ER426137
    bit_stream->video_object_layer_height = ((bit_stream->video_object_layer_height  + 0xF) & (~0xF));
    bit_stream->video_object_layer_width = ((bit_stream->video_object_layer_width  + 0xF) & (~0xF));

    if(frame_info_height != bit_stream->video_object_layer_height)
	{
        frame_info_height = bit_stream->video_object_layer_height;
		OstTraceFiltInst0(TRACE_FLOW, "port setting change event generated::frame_info_height\n");
		has_changed = OMX_TRUE;
	}
	OstTraceFiltInst2(TRACE_FLOW, "detectPortSettingsAndNotify frame_info_width (%d) bit_stream->video_object_layer_width (%d)",frame_info_width,bit_stream->video_object_layer_width);
	if(frame_info_width != bit_stream->video_object_layer_width)
	{
		frame_info_width = bit_stream->video_object_layer_width;
		OstTraceFiltInst0(TRACE_FLOW, "port setting change event generated::frame_info_width\n");
        has_changed = OMX_TRUE;
	}
	OstTraceFiltInst2(TRACE_FLOW, "detectPortSettingsAndNotify AspectRatio (%d) bit_stream->AspectRatio (%d)",AspectRatio,bit_stream->AspectRatio);
	if(AspectRatio != bit_stream->AspectRatio)
	{
		//printf("\nPARSER_MPEG4::bit_stream->AspectRatio");
        {
			AspectRatio = bit_stream->AspectRatio;
			OstTraceInt0(TRACE_FLOW, "port setting change event generated::AspectRatio\n");
         }
	}
	if(ColorPrimary != bit_stream->ColorPrimary)
	{
		//printf("\nPARSER_MPEG4::bit_stream->ColorPrimary");
        {
			ColorPrimary = bit_stream->ColorPrimary;
			OstTraceInt0(TRACE_FLOW, "port setting change event generated::ColorPrimary\n");
        }

	}
	if(has_changed)
	{
        has_changed = OMX_FALSE;
	//+ER 354962
	#ifdef SOFT_DECODER
		addPendingCommand();
	#endif
	//-ER 354962
        /* +Change start for ER335883 */
		iDdep.setConfig(8,1,0);  // readme::8 refers to CEASE only output port
        /* -Change end for ER335883 */

		//printf("\nPARSER_MPEG4::port setting change event generated");
		OstTraceInt0(TRACE_FLOW, "MPEG4Dec Parser: detectPortSettingsAndNotify: PortSettingsChanged event generated");
		//+ER401805
		OstTraceInt0(TRACE_FLOW, "MPEG4Dec Parser: detectPortSettingsAndNotify(): Setting do_not_parse as OMX_TRUE");
		do_not_parse = OMX_TRUE;
		//-ER401805
		//+ER426137
		OstTraceFiltInst2(TRACE_FLOW, "detectPortSettingsAndNotify, raising event, width (%d) height (%d)",tempWidth,tempHeight);
		iChangePortSettingsSignal.portSettings(tempWidth,tempHeight,bit_stream->AspectRatio,nHorizontal,nVertical,bit_stream->ColorPrimary);
		//-ER426137
	}
	OstTraceInt0(TRACE_FLOW, "<= Mpeg4dec_Parser: detectPortSettingsAndNotify()");
}




void ARM_PARSER_NMF::initParser()
{
    /* Initialize Parser parameters */
	OstTraceFiltInst0(TRACE_FLOW, "initParser \n");
    bit_stream = &ts_bitstream;
	iVolHeader = &ts_volheader;
    iVopHeader = &ts_vopheader;
    iShortHeader = &ts_ishortheader;
 
	bit_stream->codingType = VFM_MPEG4_NONE;
	bit_stream->profile = OMX_VIDEO_MPEG4ProfileSimple;
	bit_stream->level = OMX_VIDEO_MPEG4Level0;
	bit_stream->firstFrame = OMX_TRUE;
	bit_stream->updateParamsDone = OMX_FALSE;
	bit_stream->vop_time_increment_resolution = 0;
	//bit_stream->prev_horizontal_size =0;
	//bit_stream->prev_vertical_size =0;
	bit_stream->horizontal_size =0;
	bit_stream->vertical_size =0;
	bit_stream->fixed_vop_rate =0;
	bit_stream->fixed_vop_time_increment =0;
	bit_stream->video_object_layer_width =0;
	bit_stream->video_object_layer_height =0;
	bit_stream->reversible_vlc =0;
	bit_stream->data_partitioned =0;
	bit_stream->resync_marker_disable =1;
	bit_stream->complexity_estimation_disable =1;
	bit_stream->video_object_layer_verid = 0;
	bit_stream->lowDelay = 0;
	bit_stream->isInterlaced =0;
	bit_stream->quant_type =0;
	bit_stream->quarter_sample=0;
	//bit_stream->intraQuantizationMatrix[64];    //no initialization
	//bit_stream->nonIntraQuantizationMatrix[64]; //no initialization
	bit_stream->prev_temporal_ref = 0;
	bit_stream->flv_version=0;
	bit_stream->flag_sorenson=0;
	bit_stream->temporal_ref = 0;
    bit_stream->source_format=0; //might not be correct
    bit_stream->framenum=0; // //might not be correct
	bit_stream->pict_type = 0;
	bit_stream->pictType = OMX_VIDEO_PictureTypeI;
	bit_stream->modulo_time_base =0;
	bit_stream->vop_time_increment = 0;
	bit_stream->vop_coded = 0;
	bit_stream->vop_rounding_type = 0;
	bit_stream->intra_dc_vlc_thr = 0;
	bit_stream->quant  =0;
	bit_stream->vop_fcode_forward = 1;
	bit_stream->vop_fcode_backward = 1;
    bit_stream->isVolHeaderFound=OMX_FALSE;
    bit_stream->iShortHeader=OMX_FALSE;
	bit_stream->ColorPrimary=0;
	bit_stream->AspectRatio=0;
	bit_stream->sprite_enable=0;              
	bit_stream->no_of_sprite_warping_points=0;
	bit_stream->sprite_warping_accuracy=0;    
	bit_stream->sprite_brightness_change=0;   
	bit_stream->difftraj_x=0;                 
	bit_stream->difftraj_y=0;  
    bit_stream->h263_annexsupport=0;
	bit_stream->aic_mode=0;
	bit_stream->df_mode=0;
	bit_stream->ss_mode=0;
	bit_stream->mq_mode=0;
   bit_stream->Complexity_estimation_mp4_asp.estimation_method=0;
   bit_stream->Complexity_estimation_mp4_asp.opaque=0;
   bit_stream->Complexity_estimation_mp4_asp.transparent=0;
   bit_stream->Complexity_estimation_mp4_asp.intra_cae=0;
   bit_stream->Complexity_estimation_mp4_asp.inter_cae=0;
   bit_stream->Complexity_estimation_mp4_asp.no_update=0;
   bit_stream->Complexity_estimation_mp4_asp.upsampling=0;
   bit_stream->Complexity_estimation_mp4_asp.intra_blocks=0;
   bit_stream->Complexity_estimation_mp4_asp.inter_blocks=0;
   bit_stream->Complexity_estimation_mp4_asp.inter4v_blocks=0;
   bit_stream->Complexity_estimation_mp4_asp.not_coded_blocks=0;
   bit_stream->Complexity_estimation_mp4_asp.dct_coefs=0;
   bit_stream->Complexity_estimation_mp4_asp.dct_lines=0;
   bit_stream->Complexity_estimation_mp4_asp.vlc_symbols=0;
   bit_stream->Complexity_estimation_mp4_asp.vcl_bits=0;
   bit_stream->Complexity_estimation_mp4_asp.apm=0;
   bit_stream->Complexity_estimation_mp4_asp.npm=0;
   bit_stream->Complexity_estimation_mp4_asp.interpolate_mc_q=0;
   bit_stream->Complexity_estimation_mp4_asp.forw_back_mc_q=0;
   bit_stream->Complexity_estimation_mp4_asp.halfpel2=0;
   bit_stream->Complexity_estimation_mp4_asp.halfpel4=0;
   bit_stream->Complexity_estimation_mp4_asp.sadct=0;
   bit_stream->Complexity_estimation_mp4_asp.quarterpel=0; 

	//+ER372408
	iVolHeader->iQuantPrecision = 5;
	//-ER372408
}




void ARM_PARSER_NMF::updateMpeg4Params()
{
	OMX_U8 index;
	OstTraceFiltInst0(TRACE_FLOW, "updateMpeg4Params \n");
	pMpeg4ParamIn->frame_height = bit_stream->vertical_size;
	pMpeg4ParamIn->frame_width = bit_stream->horizontal_size;
	pMpeg4ParamIn->data_partitioned = bit_stream->data_partitioned;
	pMpeg4ParamIn->resync_marker_disable = bit_stream->resync_marker_disable;
	if (config_concealmentflag && ((bit_stream->vertical_size*bit_stream->horizontal_size) <= (1280*720))) /* change for ER 346056 */
	{
		
		if(bit_stream->reversible_vlc)
			pMpeg4ParamIn->error_concealment_config = 0x3D;	
		else 
			pMpeg4ParamIn->error_concealment_config = 0x1D;	
	}
	else 
	{	
		pMpeg4ParamIn->error_concealment_config = 0x0;	
	}
	pMpeg4ParamIn->flag_sorenson=0;
	pMpeg4ParamIn->flag_short_header = ((bit_stream->codingType == VFM_MPEG4_SH)||(bit_stream->codingType ==VFM_MPEG4_SORENSON)) ? 1 : 0;
    pMpeg4ParamIn->quant_type = bit_stream->quant_type;
	pMpeg4ParamIn->interlaced = bit_stream->isInterlaced;
	pMpeg4ParamIn->low_delay = bit_stream->lowDelay;
	pMpeg4ParamIn->reversible_vlc = bit_stream->reversible_vlc;
	pMpeg4ParamIn->rounding_type = bit_stream->vop_rounding_type;
	pMpeg4ParamIn->vop_time_increment_resolution = bit_stream->vop_time_increment_resolution;
    for(index = 0;index<64;index++){
		pMpeg4ParamIn->intra_quant_mat[index] = bit_stream->intraQuantizationMatrix[index];
	}
	for(index = 0;index<64;index++){
		pMpeg4ParamIn->nonintra_quant_mat[index] = bit_stream->nonIntraQuantizationMatrix[index];
	}
	pMpeg4ParamIn->flv_version=bit_stream->flv_version;
	pMpeg4ParamIn->flag_sorenson=bit_stream->flag_sorenson;
	pMpeg4ParamIn->quarter_sample=(OMX_U32)ENDIANESS_CONVERSION(bit_stream->quarter_sample);
	pMpeg4ParamIn->source_frame_width=bit_stream->horizontal_size;
	pMpeg4ParamIn->source_frame_height=bit_stream->vertical_size;
	pMpeg4ParamIn->output_format=0;
	pMpeg4ParamIn->sprite_enable=bit_stream->sprite_enable;              
	pMpeg4ParamIn->no_of_sprite_warping_points=bit_stream->no_of_sprite_warping_points;
	pMpeg4ParamIn->sprite_warping_accuracy=bit_stream->sprite_warping_accuracy;    
	pMpeg4ParamIn->sprite_brightness_change=bit_stream->sprite_brightness_change;   
	pMpeg4ParamIn->difftraj_x=bit_stream->difftraj_x;                 
	pMpeg4ParamIn->difftraj_y=bit_stream->difftraj_y;
	pMpeg4ParamIn->advanced_intra= bit_stream->aic_mode;
	pMpeg4ParamIn->deblocking_filter=bit_stream->df_mode;
	pMpeg4ParamIn->slice_structure=bit_stream->ss_mode;
	pMpeg4ParamIn->modified_quantizer=bit_stream->mq_mode;
	pMpeg4ParamIn->decode_flag=(OMX_U32)ENDIANESS_CONVERSION(1);
	/* +Change start for ER335883 */
	//GMC not supported above 720p
	if(( (pMpeg4ParamIn->data_partitioned || bit_stream->reversible_vlc  || (bit_stream->resync_marker_disable==0))
		 && ((bit_stream->vertical_size*bit_stream->horizontal_size) > (720*1280))) /* change for ER 346056 */
	   ||((bit_stream->codingType==VFM_MPEG4_SP)&&((bit_stream->vertical_size * bit_stream->horizontal_size) > (1088*1920))) /* change for ER 346056 */
#ifdef SOFT_DECODER //+ER349201
	   || ((bit_stream->vertical_size * bit_stream->horizontal_size) > (576*720)) /* change for ER 346056 */
#endif //-ER349201
	   )
	{
		//not supported from firmware:: send error to user and cease at ddep
		pDesc->pHeaderDesc->cease=1;//readme::cease both ports
		OstTraceFiltInst2(TRACE_FLOW, "OMX_ErrorFormatNotDetected resync_marker_disable=%d data_partitioned =%d \n",bit_stream->resync_marker_disable,pMpeg4ParamIn->data_partitioned);
        proxy.eventHandler(OMX_EventError, (t_uint32)OMX_ErrorFormatNotDetected,0);
	}
	/* -Change end for ER335883 */
}

//+ER401513
void ARM_PARSER_NMF::fillParamsSinceCorruptionIsMinor()
{
	OstTraceInt0(TRACE_API, "=> fillParamsSinceCorruptionIsMinor()");

	bit_stream->vertical_size 	= frame_info_height;
	bit_stream->horizontal_size = frame_info_width;
	bit_stream->data_partitioned 		= 0;
	bit_stream->resync_marker_disable 	= 1;

	//pMpeg4ParamIn->error_concealment_config will be filled based on width and height!

	bit_stream->quant_type 		= 0;
	bit_stream->isInterlaced 	= 0;
	bit_stream->lowDelay		= 0;
	bit_stream->reversible_vlc 	= 0;
	bit_stream->vop_rounding_type 	= 0;
	//CHECK THIS!!!
	bit_stream->vop_time_increment_resolution = 30000;

	for(OMX_U8 index = 0;index<64;index++){
		bit_stream->intraQuantizationMatrix[index] = 0;
	}
	for(OMX_U8 index = 0;index<64;index++){
		bit_stream->nonIntraQuantizationMatrix[index] = 0;
	}
	bit_stream->flv_version = 0;
	bit_stream->flag_sorenson = 0;
	bit_stream->quarter_sample = 0;
	//output format is 0
	bit_stream->sprite_enable = 0;
	bit_stream->no_of_sprite_warping_points = 0;
	bit_stream->sprite_warping_accuracy = 0;
	bit_stream->sprite_brightness_change = 0;
	bit_stream->difftraj_x = 0;
	bit_stream->difftraj_y = 0;
	bit_stream->aic_mode = 0;
	bit_stream->df_mode = 0;
	bit_stream->ss_mode = 0;
	bit_stream->mq_mode = 0;
	//decode flag is 1

	bit_stream->video_object_layer_height = frame_info_height;
	bit_stream->video_object_layer_width = frame_info_width;
	bit_stream->AspectRatio = AspectRatio;

	OstTraceInt0(TRACE_API, "<= fillParamsSinceCorruptionIsMinor()");
}
//-ER401513

void ARM_PARSER_NMF::processBuffer()
{
    OMX_U32 bitOffset=0;
    ts_ddep_vdc_mpeg4_header_desc *pHeaderDesc = (ts_ddep_vdc_mpeg4_header_desc*)NULL;
    OMX_U32 scValue;
    OMX_ERRORTYPE parseError;
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec Parser: processBuffer()");

	{
		#ifndef SOFT_DECODER
			parserFrameIndex++;
			OstTraceInt1(TRACE_FLOW, "MPEG4Dec Parser: processBuffer: parserFrameIndex = %d", parserFrameIndex);

			//>OstTraceInt3(TRACE_FLOW, "processBuffer: parserFrameIndex = %d bit_stream->vertical_size = %d at line %d", parserFrameIndex, bit_stream->vertical_size, __LINE__);
			//>OstTraceInt3(TRACE_FLOW, "processBuffer: parserFrameIndex = %d bit_stream->horizontal_size = %d at line %d", parserFrameIndex, bit_stream->horizontal_size, __LINE__);
			//>OstTraceInt2(TRACE_FLOW, "processBuffer: parserFrameIndex = %d  pDesc->pHeaderDesc->cease = %d", parserFrameIndex, pDesc->pHeaderDesc->cease);
		#endif

        initParserBitstream(pBufferIn);
        if(pBufferIn->nFilledLen >= 4 )  //why checked with 4
		{
            parseError = detectAndParseCodingType(scValue);
			OstTraceInt1(TRACE_FLOW, "processBuffer: scValue = 0x%x", scValue);
			while((parseError == OMX_ErrorNone) &&
					(scValue != SH_CODE) &&
					(scValue != KVOP_START_CODE))
				{
						OMX_U32 temp_stuff;
						temp_stuff = getbitscount();
						if(temp_stuff % 8)
						{
							getbits(8-(temp_stuff%8));
						}
                    parseError = detectAndParseCodingType(scValue);
					OstTraceInt1(TRACE_FLOW, "processBuffer: scValue = 0x%x", scValue);
				}
			bitOffset = getbitscount();
			OstTraceFiltInst1(TRACE_FLOW, "processBuffer():  parseError = 0x%x", parseError);
			if(pDesc==NULL)
				return;

            pHeaderDesc = pDesc->pHeaderDesc;
			
			if(pHeaderDesc==NULL)
				return;

			pHeaderDesc->cease=0;
			//printf("\nPARSER_MPEG4::parseError 0x =%x ",parseError);
			#ifndef SOFT_DECODER
				OstTraceInt2(TRACE_FLOW, "processBuffer(): parserFrameIndex = %d, bit_stream->isVolHeaderFound = %d", parserFrameIndex, bit_stream->isVolHeaderFound);
			#endif
			if(parseError != OMX_ErrorNone)
			{
				if(parseError == OMX_ErrorUndefined)
				{
					//+ER372408
					fillParamsSinceCorruptionIsMinor();
					//-ER372408
					//$ end of buffer without start code
					//$ this case is handled in 8815, where next buffer is pushed
					//return OMX_ErrorUndefined; //$ there can be buffer without relevant data
					//$ dont return errror and this will enter a condition that plength =0 and do nmfcall
				}
				else if(parseError == OMX_ErrorFormatNotDetected)
				{
					//$ commented for arm nmf component
					//printf("\nPARSER_MPEG4::OMX_ErrorFormatNotDetected");
					OstTraceFiltInst0(TRACE_FLOW, "processBuffer::OMX_ErrorFormatNotDetected\n");
					pHeaderDesc->cease=1;   //readme::cease both ports
					proxy.eventHandler(OMX_EventError, (t_uint32)OMX_ErrorFormatNotDetected,0);
					//+ER369024
					return;
					//-ER369024
				}
				else if(parseError == OMX_ErrorMbErrorsInFrame)
				{
					OstTraceFiltInst0(TRACE_FLOW, "processBuffer::FrameLevelError\n");
					mPorts[0].returnBuffer(pBufferIn);
					pBufferIn=NULL;
                    StartCodeCount--;
					pHeaderDesc->cease=1;   //readme::cease both ports
					if(pDesc)
					{
						 freeHeaderDesc((t_uint32*)(pDesc->mpcAddress));
						 clearAllPendingRequestsToFree();  
						 pDesc=NULL;
					}
                    return;
				}
				else if(parseError == OMX_ErrorOverflow)
				{
					if(bit_stream->firstFrame == OMX_FALSE)
					{
						//We don't do anything for firstFrame==TRUE because the first buffer may
						// have only configuration data and an overflow may most certainly occur.
						// Also, we do not returning any error to the client
						OstTraceInt0(TRACE_FLOW, "processBuffer(): (No start code found) Returning input buffer");
						mPorts[0].returnBuffer(pBufferIn);
						pBufferIn=NULL;
						StartCodeCount--;
						pHeaderDesc->cease=1;   //readme::cease both ports
						//+ER451628
						/* Releasing one output buffer from queue
						setConfig method of iDdep interface with value iDdep.setConfig(0,9,0)
						would release one output buffer from queue.
						*/
						iDdep.setConfig(0,9,0);//release buffer
						//-ER451628
						if(pDesc)
						{
							freeHeaderDesc((t_uint32*)(pDesc->mpcAddress));
							clearAllPendingRequestsToFree();
							pDesc=NULL;
						}
						return;
					}
				}
				else
				{
					//$ commented for arm nmf component
					//return OMX_ErrorOverflow;//$ wrong start code
				}

			}

			//printf("\nPARSER_MPEG4::processBuffer FirstFrameFlag =%d StartCode =%X",bit_stream->firstFrame,scValue);
			if(bit_stream->firstFrame)//$ not required for SH, check if required for SP/ASP
			{
				if((scValue == SH_CODE ||scValue ==KVOP_START_CODE))  
				{
					//+ER401513
					if(bit_stream->isVolHeaderFound == OMX_FALSE) {
						fillParamsSinceCorruptionIsMinor();
					}
					//-ER401513
                    detectPortSettingsAndNotify(pHeaderDesc);
					updateMpeg4Params();
					bit_stream->updateParamsDone = OMX_TRUE;
				}
				else 
				{
					/* Only VOS, VO available in buffer, Just pass this dummy buffer to MPC
					so that events are generated */
					//printf("\nPARSER_MPEG4::processBuffer Pass dummy Configbuffer to mpc");
					if(bit_stream->isVolHeaderFound)
					{	detectPortSettingsAndNotify(pHeaderDesc);
						updateMpeg4Params();
					}
					pBufferIn->nFilledLen = 0;
                    if(pBufferIn->nFlags & OMX_BUFFERFLAG_EOS)
					{
						//this is special case FIXME
					}
				}

				bit_stream->firstFrame = OMX_FALSE;

			}
			else 
			{
				if((scValue == SH_CODE) ||(scValue ==KVOP_START_CODE)){
					if(bit_stream->updateParamsDone == OMX_FALSE)
					{
						/* Detect and Notify if port settings have been changed */
						detectPortSettingsAndNotify(pHeaderDesc);
						updateMpeg4Params();
						bit_stream->updateParamsDone = OMX_TRUE;
                    }
				}
				else
				{
					if(bit_stream->isVolHeaderFound)
					{	detectPortSettingsAndNotify(pHeaderDesc);
						updateMpeg4Params();
					}
					pBufferIn->nFilledLen = 0;
                    if(pBufferIn->nFlags & OMX_BUFFERFLAG_EOS)
					{
						//this is special case FIXME
					}
				}

			}
			if(error_reporting==1)
			{
                if(bit_stream->pict_type==0) 
				{
					//printf("\nPARSER_MPEG4::processBuffer ResetErrorMap for I frame");
					memset(errormap_ptr,0x0,225*sizeof(OMX_U16)); //size of 720p
				}
			}
			//printf("\nPARSER_MPEG4::processBuffer bitOffset= %X ",bitOffset);
			updateHeaderInfos(pHeaderDesc,bitOffset);
        }
        //printf("\nPARSER_MPEG4::processBuffer exit bitOffset= %X ",bitOffset);

    }
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Parser: processBuffer()");
}

void ARM_PARSER_NMF::updateHeaderInfos(ts_ddep_vdc_mpeg4_header_desc *pHeaderDesc, OMX_U32 bitOffset)
{
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec Parser: updateHeaderInfos()");
	
	pHeaderDesc->bitOffset = (OMX_U32)ENDIANESS_CONVERSION(bitOffset);
	pHeaderDesc->pictureCodingType = bit_stream->pict_type;
	pHeaderDesc->quant = bit_stream->quant;
	pHeaderDesc->roundingType = bit_stream->vop_rounding_type;
	pHeaderDesc->intraDcVlcThr = bit_stream->intra_dc_vlc_thr;
	pHeaderDesc->vopFcodeForward = bit_stream->vop_fcode_forward;
	pHeaderDesc->vopFcodeBackward = bit_stream->vop_fcode_backward;
	pHeaderDesc->vopTimeIncrement = bit_stream->vop_time_increment;
	pHeaderDesc->moduloTimeBase = bit_stream->modulo_time_base;
	pHeaderDesc->vopCoded = ((bit_stream->codingType == VFM_MPEG4_SH)||(bit_stream->codingType ==VFM_MPEG4_SORENSON)) ? 1 : (bit_stream->vop_coded);
	pHeaderDesc->flv_version=bit_stream->flv_version;
	pHeaderDesc->flag_sorenson=bit_stream->flag_sorenson;
	pHeaderDesc->difftraj_x=bit_stream->difftraj_x;
	pHeaderDesc->difftraj_y=bit_stream->difftraj_y;
	pHeaderDesc->advanced_intra= bit_stream->aic_mode;
	pHeaderDesc->deblocking_filter=bit_stream->df_mode;
	pHeaderDesc->slice_structure=bit_stream->ss_mode;
	pHeaderDesc->modified_quantizer=bit_stream->mq_mode;

	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Parser: updateHeaderInfos()");
}

void ARM_PARSER_NMF::initParserBitstream(OMX_BUFFERHEADERTYPE* pBuffer)
{
	OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4dec_Parser: initParserBitstream");
	//printf("\nPARSER_MPEG4::initParserBitstream");
    bits_handle->ui32_rdbfr = (OMX_U32)pBuffer->pBuffer;
    bits_handle->rdbfr = (OMX_U8*)pBuffer->pBuffer;
    bits_handle->rdptr = (OMX_U8*)pBuffer->pBuffer;
    bits_handle->incnt = 0;
	bits_handle->incnt1 = 0;
	OstTraceInt0(TRACE_FLOW, "<= Mpeg4dec_Parser: initParserBitstream");
}

OMX_ERRORTYPE ARM_PARSER_NMF::detectAndParseCodingType(OMX_U32 &aScValue)
{

	OMX_U32 temp;
	OMX_U32 bitCount = 0;
	OMX_BOOL scNotFound = OMX_TRUE;
	OMX_BOOL endOfStreamNotReached = OMX_TRUE;
	OMX_U32 bitShiftForResync = 0;

	OMX_ERRORTYPE errorCode;

	OMX_U32 iStartCodeValue=0;
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec Parser: detectAndParseCodingType()");
	if (bit_stream->flag_sorenson) {
		bit_stream->codingType=VFM_MPEG4_SORENSON;
		 scNotFound = OMX_FALSE;
		 iStartCodeValue = SH_CODE;
	}
	//resync marker width has been calculated in advance in order to avoid rendundant calculations in each iteration of the following while loop
	if(bit_stream->codingType == VFM_MPEG4_SP)
		{
		bitShiftForResync = MP4_SC_LENGHT_IN_BITS - iVopHeader->iResyncMarkerSize;
		}
	else if(bit_stream->codingType == VFM_MPEG4_SH)
		{
		bitShiftForResync = MP4_SC_LENGHT_IN_BITS - iShortHeader->iResyncMarkerSize;
		}
	else
		{
		//nothing
		}

	temp = getbits(32);
	OstTraceInt3(TRACE_FLOW, "detectAndParseCodingType: temp = 0x%x, bit_stream->codingType = %d, pBufferIn->nFilledLen = %d, ", temp, bit_stream->codingType, pBufferIn->nFilledLen);
	//printf("\nPARSER_MPEG4::detectAndParseCodingType temp =%X",temp);
	while((scNotFound) && (endOfStreamNotReached))
	{
		bitCount = getbitscount();
		OstTraceInt1(TRACE_FLOW, "detectAndParseCodingType: bitCount = %d", bitCount);
		//if(((bitCount+7)&~7) > pBuffer->nFilledLen)
		if( bitCount > (pBufferIn->nFilledLen<<3))
		{
				endOfStreamNotReached = OMX_FALSE;
				OstTraceInt1(TRACE_FLOW, "<= detectAndParseCodingType() Return OMX_ErrorOverflow at line %d", __LINE__);
				//+ER372408
				return OMX_ErrorOverflow;// OMX_ErrorUndefined;
				//-ER372408
			//	return; FIXME.0 this should be error return OMX_Unsupported Port Format
			//	return OMX_
		}
		else
		{
		    //first check for MPEG4 SP headers
			/* visual object sequence */
			if ( ((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))
				&& (temp == VIS_OBJ_SEQ_CODE))
				{ /* first 32 bits */
				// //MSG0("VOS found");
				iStartCodeValue = VIS_OBJ_SEQ_CODE;
				scNotFound = OMX_FALSE;
				}
			//KVISUAL_OBJECT_SEQUENCE_END_CODE
			else if ( ((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))
				&& (temp == KVISUAL_OBJECT_SEQUENCE_END_CODE))
				{ /* first 32 bits */
				// //MSG0("VOS found");
				iStartCodeValue = KVISUAL_OBJECT_SEQUENCE_END_CODE;
				scNotFound = OMX_FALSE;
				}
			/* video object layer */
			else if (((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))
				&& (temp >= KVIDEO_OBJECT_LAYER_START_CODE_SMALLEST && temp <= KVIDEO_OBJECT_LAYER_START_CODE_GREATEST))
				{
				iStartCodeValue = KVIDEO_OBJECT_LAYER_START_CODE_SMALLEST; // just ignore the value, it does not matter
				// //MSG0("VOL found");
				scNotFound = OMX_FALSE;
				bit_stream->codingType = VFM_MPEG4_SP;
				bit_stream->codingType = VFM_MPEG4_SP;
				//iStartCodeLength = MP4_SC_LENGHT_IN_BYTES; leave unchanged
				}
			else if (((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SH) || (bit_stream->codingType == VFM_MPEG4_SP))&&
				( (temp >> (MP4_SC_LENGHT_IN_BITS - SH_SC_LENGTH_IN_BITS)) == SH_CODE)) // if not VOL then SH can be expected
				{
				iStartCodeValue = SH_CODE;
				//MSG0("SH found");
				scNotFound = OMX_FALSE;
				bit_stream->codingType = VFM_MPEG4_SH;
				//iStartCodeLength = MP4_SH_LENGHT_IN_BYTES;
				bit_stream->codingType = VFM_MPEG4_SH;
				}
			else if( ((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))&&
				(temp == KVISUAL_OBJECT_START_CODE))
				{
				iStartCodeValue = KVISUAL_OBJECT_START_CODE;
				//MSG0("Visual Object Start Code found");
				scNotFound = OMX_FALSE;
				}
			else if( ((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))&&
				(temp == KVOP_START_CODE))
				{
				iStartCodeValue = KVOP_START_CODE;
				//MSG0("VOP found");
				scNotFound = OMX_FALSE;
				}
			else if(((bit_stream->codingType == VFM_MPEG4_NONE) || (bit_stream->codingType == VFM_MPEG4_SP))&&
				(temp == KGROUP_OF_VOP_START_CODE))
				{
				iStartCodeValue = KGROUP_OF_VOP_START_CODE;
				//MSG0("GOV found");
				scNotFound = OMX_FALSE;
				}
			else if( (bit_stream->codingType == VFM_MPEG4_SP) &&
					(bit_stream->resync_marker_disable == 1) &&
					((temp >> bitShiftForResync) == KRESYNC_MARKER )
					)
				{
				iStartCodeValue = KSP_RESYNC_MARKER;
				//MSG0("SP Resync marker found");
				scNotFound = OMX_FALSE;
				}
			else if((bit_stream->codingType == VFM_MPEG4_SH) &&
					( (temp >> bitShiftForResync) == KRESYNC_MARKER)
					)
				{
				iStartCodeValue = KSH_RESYNC_MARKER;
				//MSG0("SH Resync marker found");
				scNotFound = OMX_FALSE;
				}



   			if (scNotFound == OMX_TRUE)
    	 	{
    	   		temp = temp << 8 | (OMX_U8)(getbits(8));
    	 	}
    	}
	}
	// +ER446263
	if(!((iStartCodeValue==VIS_OBJ_SEQ_CODE)||(iStartCodeValue==KVISUAL_OBJECT_START_CODE)||(iStartCodeValue==KVIDEO_OBJECT_LAYER_START_CODE_SMALLEST)))
	{
		initParserBitstream(pBufferIn);//$ added as before for initialized value
	}
	// -ER446263
    OstTraceInt1(TRACE_FLOW, "detectAndParseCodingType: iStartCodeValue = 0x%x", iStartCodeValue);
	//printf("\nPARSER_MPEG4::detectAndParseCodingType StartCode =%X",iStartCodeValue);
	aScValue = iStartCodeValue;
	switch(iStartCodeValue)
		{
	case VIS_OBJ_SEQ_CODE:
		bit_stream->codingType = VFM_MPEG4_SP; //Assuming Simple Profile
		errorCode =  GetVosHeaderDataL();

		break;
	case KVISUAL_OBJECT_START_CODE:
		errorCode = GetVOHeaderDataL();
		break;

	case KVISUAL_OBJECT_SEQUENCE_END_CODE:
		errorCode = GetVosEndHeaderDataL();	//$ do nothing

		break;

	case KVIDEO_OBJECT_LAYER_START_CODE_SMALLEST:
		bit_stream->codingType = VFM_MPEG4_SP; //Assuming Simple Profile
		// +ER446263
		errorCode = GetVolHeaderDataL(temp);
		// -ER446263
		//printf("\nPARSER_MPEG4::GetVolHeaderDataL errorCode =%X",errorCode);
		break;

	case SH_CODE:
		if(bit_stream->flag_sorenson)
			errorCode = GetSorensonDataL();
		else
			errorCode = GetShortHeaderDataL();
		OstTraceFiltInst1(TRACE_FLOW, "detectAndParseCodingType::  errorCode =%d \n",errorCode);
		break;
	case KGROUP_OF_VOP_START_CODE:
		errorCode = GetGovHeaderDataL();

		break;

	case KVOP_START_CODE:
		errorCode = GetVopHeaderDataL();

		break;

	case KSP_RESYNC_MARKER:
		errorCode = GetVopVpHeaderDataL();

		break;
	case KSH_RESYNC_MARKER:
		errorCode = GetShortHeaderGobDataL();

		break;

	default:
        errorCode = OMX_ErrorOverflow;//$should not occur
		}

	OstTraceInt1(TRACE_FLOW, "<= Mpeg4dec_Parser: detectAndParseCodingType() errorCode = 0x%x", errorCode);

	return errorCode;

}




OMX_ERRORTYPE ARM_PARSER_NMF::GetVosEndHeaderDataL()//$ visual object
{
	OMX_U32 temp;
	OstTraceFiltInst0(TRACE_FLOW, "GetVosEndHeaderDataL \n");
	temp = getbits(MP4_SC_LENGHT_IN_BITS);
	while(temp != KVISUAL_OBJECT_SEQUENCE_END_CODE)
	{
		temp = ((temp << 8) | (OMX_U32)((OMX_U8)getbits(8)));
	}
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ARM_PARSER_NMF::GetVOHeaderDataL()//$ visual object
{
	t_uint8 visual_object_type=0;
    //ER446263, To support repeated VO headers
    // +ER446263
    /*temp = getbits(MP4_SC_LENGHT_IN_BITS);
	OstTraceFiltInst0(TRACE_FLOW, "GetVOHeaderDataL \n");
	while(temp != KVISUAL_OBJECT_START_CODE)
	{
		temp = ((temp << 8) | (OMX_U32)((OMX_U8)getbits(8)));
	}*/
	// -ER446263
	//is_visual_object_identifier
	if(getbits(1))
	{
		//visual_object_verid
		getbits(4);
		//visual_object_priority
		getbits(3);
	}
	//visual_object_type
	visual_object_type=getbits(4);
	if(visual_object_type==1 || visual_object_type==2) //video ID  or still texture ID
	{
		//video_signal_type
		if(getbits(1))
		{
			//video_format
			getbits(3);
			//video_range
			getbits(1);
			//colour_description
			if(getbits(1))
			{
				//colour_primaries
				bit_stream->ColorPrimary=getbits(8);
				//printf("\n GetVOHeaderDataL::bit_stream->ColorPrimary = %d",bit_stream->ColorPrimary);
			}
		}

	}
    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ARM_PARSER_NMF::GetVosHeaderDataL()//$
{
	// +ER446263
	//temp = getbits(MP4_SC_LENGHT_IN_BITS);
	OstTraceFiltInst0(TRACE_FLOW, "GetVosHeaderDataL \n");
	/*while(temp != VIS_OBJ_SEQ_CODE)
	{
		temp = ((temp << 8) | (OMX_U32)((OMX_U8)getbits(8)));
	}*/
	// -ER446263
    bit_stream->profile = OMX_VIDEO_MPEG4ProfileSimple;
	bit_stream->level = (OMX_VIDEO_MPEG4LEVELTYPE)(1 << getbits(8));

	return OMX_ErrorNone;


}

OMX_ERRORTYPE ARM_PARSER_NMF::GetShortHeaderDataL()
{
    OstTraceInt0(TRACE_FLOW, "=> Mpeg4dec_Parser: GetShortHeaderDataL()");

	OMX_U32 tmp,ufep;
	tmp = getbits(22);
	if(tmp == 0x20)
	{
		//OstTraceInt0(TRACE_FLOW, "GetShortHeaderDataL found");
	}
	else
	{
		 tmp = ((tmp << 10) | (OMX_U32)((OMX_U8)getbits(10)));
		 while((tmp >> (MP4_SC_LENGHT_IN_BITS - SH_SC_LENGTH_IN_BITS)) != SH_CODE)
		 {
             tmp = ((tmp << 8) | (OMX_U32)((OMX_U8)getbits(8)));
		 }

	}
    OstTraceFiltInst1(TRACE_FLOW, "GetShortHeaderDataL::  tmp =%d \n",tmp);
   
	bit_stream->isVolHeaderFound = OMX_TRUE;
	//iShortHeader->iLastGobNumber = 0;//reset
	//iShortHeader->iGobFrameId = KInvalidGobFrameId;
    bit_stream->prev_temporal_ref = bit_stream->temporal_ref;//$ part of ref code
	bit_stream->temporal_ref = getbits(8);
	OstTraceFiltInst1(TRACE_FLOW, "GetShortHeaderDataL::  temporal_ref =%d \n",bit_stream->temporal_ref);
	if(bit_stream->prev_temporal_ref > bit_stream->temporal_ref)
	{
		bit_stream->prev_temporal_ref -= 256;
	}
	//marker bit
	OMX_U8 test = getbits(1);
    OstTraceFiltInst1(TRACE_FLOW, "GetShortHeaderDataL::  test =%d \n",test);
	if(test == OMX_FALSE)
	{
		return OMX_ErrorMbErrorsInFrame;
	}

	//Zero bit
	test = getbits(1);
    OstTraceFiltInst1(TRACE_FLOW, "GetShortHeaderDataL::  test =%d \n",test);
	if(test)
		{
		return OMX_ErrorMbErrorsInFrame;
		}
	//split screen indicator bit
	getbits(1);
	//document camera indicator bit
	getbits(1);
	//full picture freeze/release bit
	getbits(1);
	bit_stream->source_format = getbits(3);
	OstTraceFiltInst1(TRACE_FLOW, "GetShortHeaderDataL::  bit_stream->source_format =%d \n",bit_stream->source_format);
	switch (bit_stream->source_format)
    {
        case (SF_SQCIF):
			iShortHeader->iNumGobPerFrame = KNUM_GOB_SQCIF;
			if ((bit_stream->framenum) == 0) {
				bit_stream->horizontal_size = 128;
				bit_stream->vertical_size = 96;
                bit_stream->video_object_layer_width  = 128;
                bit_stream->video_object_layer_height = 96;
			}
			break;
        case (SF_QCIF):
			iShortHeader->iNumGobPerFrame = KNUM_GOB_QCIF;
			if ((bit_stream->framenum) == 0) {
				bit_stream->horizontal_size = 176;
				bit_stream->vertical_size = 144;
                 bit_stream->video_object_layer_width  = 176;
                bit_stream->video_object_layer_height = 144;
			}
			break;
        case (SF_CIF):
			iShortHeader->iNumGobPerFrame = KNUM_GOB_CIF;
			if ((bit_stream->framenum) == 0) {
			    bit_stream->horizontal_size = 352;
				bit_stream->vertical_size = 288;
                 bit_stream->video_object_layer_width  = 352;
                bit_stream->video_object_layer_height = 288;
			}
			break;
        case (SF_4CIF):
			iShortHeader->iNumGobPerFrame = KNUM_GOB_4CIF;
			if ((bit_stream->framenum) == 0) {
			    bit_stream->horizontal_size = 704;
				bit_stream->vertical_size = 576;
				 bit_stream->video_object_layer_width  =704;
                bit_stream->video_object_layer_height = 576;
			}
			break;
	case (SF_16CIF):
			OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::SF_16CIF \n");
			return OMX_ErrorFormatNotDetected;
			/*iShortHeader->iNumGobPerFrame = KNUM_GOB_16CIF;
			if ((bit_stream->framenum) == 0) {
			    bit_stream->horizontal_size = 1408;
				bit_stream->vertical_size = 1152;
				 bit_stream->video_object_layer_width  = 1408;
                bit_stream->video_object_layer_height = 1152;
			}
			break;*/
	case (SF_VGA):
			if ((bit_stream->framenum) == 0) {
			    bit_stream->horizontal_size = 640;
				bit_stream->vertical_size = 480;
				 bit_stream->video_object_layer_width  = 640;
                bit_stream->video_object_layer_height = 480;
			}
            break;
	case (WVGA):
#ifdef SOFT_DECODER
			bit_stream->h263_annexsupport  = 1;
			OstTraceInt0(TRACE_FLOW, "PLUSPTYPE is present for SWdecoder");
			break;
#else
			//We set h263_annexsupport = 1 here, even for HW decoder, in order to support
			// custom picture size (such as 144x176 - rotated QCIF) information present
			// in H263 streams. This information is present in the OPPTYPE+MPPTYPE parts
			// of the header, even though the fields for various P3 annexes may be 0.
			// Ideally, the name of this parameter (h263_annexsupport) must be something
			// like (plusptype_present) to be less confusing.
			bit_stream->h263_annexsupport  = 1;
			OstTraceInt0(TRACE_FLOW, "PLUSPTYPE is present for HW decoder");
			break;
#endif
            // break; <> this statement is unreachable
        default:
            //////////MSG1("Bitstream ERROR: SourceFormat unknown %d", iShortHeader->iSourceFormat);
			return OMX_ErrorMbErrorsInFrame;
    } /* end switch */

    if(bit_stream->h263_annexsupport == 0)
    {
			bit_stream->pict_type  = getbits(1);
			OstTraceFiltInst1(TRACE_FLOW, "GetShortHeaderDataL::  bit_stream->pict_type =%d \n",bit_stream->pict_type);
			//four reserved zero bits
			getbits(4);

			bit_stream->quant = getbits(5);
			//verify
			if (bit_stream->quant< 1)
			{
			//MSG0("Bitstream Warning: quantizer out of range: clipping");
			bit_stream->quant = 1;
			}
			 if (bit_stream->quant > 31)
			{
			//MSG0("Bitstream Warning: quantizer out of range: clipping");
			  				bit_stream->quant = 31;
			}
			//Zero bit
			test = getbits(1);
			OstTraceFiltInst1(TRACE_FLOW, "GetShortHeaderDataL::  test =%d \n",test);
			if(test)
			{
			  	  return OMX_ErrorMbErrorsInFrame;
			}
    }
    else
    {
			ufep = getbits(3);
			if(ufep == 1)
			{
			   // 3 bits for source format, 4 bits for optional PCF, Annex D,E,F
			   tmp = getbits(7);
			   bit_stream->source_format = (tmp>>4) & 7;
			   switch (bit_stream->source_format) 
			   {
				   case (SF_SQCIF):
						iShortHeader->iNumGobPerFrame = KNUM_GOB_SQCIF;
						if ((bit_stream->framenum) == 0) {
							bit_stream->horizontal_size = 128;
							bit_stream->vertical_size = 96;
							bit_stream->video_object_layer_width  = 128;
							bit_stream->video_object_layer_height = 96;
						}
						break;
				  case (SF_QCIF):
						iShortHeader->iNumGobPerFrame = KNUM_GOB_QCIF;
						if ((bit_stream->framenum) == 0) {
							bit_stream->horizontal_size = 176;
							bit_stream->vertical_size = 144;
							 bit_stream->video_object_layer_width  = 176;
							bit_stream->video_object_layer_height = 144;
						}
						break;
				  case (SF_CIF):
						 iShortHeader->iNumGobPerFrame = KNUM_GOB_CIF;
						 if ((bit_stream->framenum) == 0) {
							 bit_stream->horizontal_size = 352;
							 bit_stream->vertical_size = 288;
							  bit_stream->video_object_layer_width  = 352;
							 bit_stream->video_object_layer_height = 288;
						 }
						 break;
				   case (SF_4CIF):
						  iShortHeader->iNumGobPerFrame = KNUM_GOB_4CIF;
						  if ((bit_stream->framenum) == 0) {
							  bit_stream->horizontal_size = 704;
							  bit_stream->vertical_size = 576;
							   bit_stream->video_object_layer_width  =704;
							  bit_stream->video_object_layer_height = 576;
						  }
						  break;
			   case (SF_16CIF):
						OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::SF_16CIF \n");
						return OMX_ErrorFormatNotDetected;
						 /*iShortHeader->iNumGobPerFrame = KNUM_GOB_16CIF;
						 if ((bit_stream->framenum) == 0) {
							 bit_stream->horizontal_size = 1408;
							 bit_stream->vertical_size = 1152;
							  bit_stream->video_object_layer_width  = 1408;
							 bit_stream->video_object_layer_height = 1152;
						 }
						 break;*/
				   case (SF_VGA):
						   if ((bit_stream->framenum) == 0) {
							   bit_stream->horizontal_size = 640;
							   bit_stream->vertical_size = 480;
								bit_stream->video_object_layer_width  = 640;
							   bit_stream->video_object_layer_height = 480;
						   }
						   break;
				   case (WVGA):
						  bit_stream->h263_annexsupport  = 1;
						  break;
			   default:
               OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::default \n");
					return OMX_ErrorFormatNotDetected;
			    } // end switch

				//+ER353881
				bit_stream->custom_pcf = (((tmp>>3)&1)==1) ? 1 : 0;
				OstTraceInt1(TRACE_FLOW, "GetShortHeaderDataL(): bit_stream->custom_pcf = %d", bit_stream->custom_pcf);
				//-ER353881
				tmp = getbits(11);
				bit_stream->aic_mode = (tmp>>10) & 1;
				bit_stream->df_mode = (tmp>>9) & 1;
				bit_stream->ss_mode = (tmp>>8) & 1;
				bit_stream->mq_mode = (tmp>>4) & 1;

#ifndef SOFT_DECODER
				if(bit_stream->aic_mode || bit_stream->df_mode || bit_stream->ss_mode || bit_stream->mq_mode)
				{
					OstTraceInt1(TRACE_ERROR, "GetShortHeaderDataL(): Annexes  I, J, K, T not supported for HW decoder. Returning OMX_ErrorFormatNotDetected at line = %d", __LINE__);
					return OMX_ErrorFormatNotDetected;
				}
#endif
			}
			// 3 bits for picture type, 2 bits for Annex P, Q
			tmp = getbits(5);
			bit_stream->pict_type = (tmp >> 2) & 7;
			// 1 bit for rounding type, 3 reserved bits and 1 bit for CPM
			tmp = getbits(5);
			bit_stream->vop_rounding_type = (tmp >> 4) & 1;
			if(tmp&1) 
				OstTraceFiltInst0(TRACE_FLOW, "CPM???\n\n");
			if(ufep)
			{
				if(bit_stream->source_format ==  6)
				{
					getbits(4); //4 bits for PAR
					bit_stream->horizontal_size = (t_uint16) ((getbits(9) + 1) * 4);
					getbits(1); //1 bit stuffed
					bit_stream->vertical_size = (t_uint16) (getbits(9) * 4);

					//+ER417402
					//Aligning to multiple of 16
					bit_stream->horizontal_size	=	(bit_stream->horizontal_size + 0xF)&(~0xF);
					bit_stream->vertical_size	=	(bit_stream->vertical_size + 0xF)&(~0xF);
					//-ER417402

					bit_stream->video_object_layer_width = bit_stream->horizontal_size;
					bit_stream->video_object_layer_height = bit_stream->vertical_size;

					OstTraceInt1(TRACE_FLOW, "GetShortHeaderDataL: bit_stream->horizontal_size = %d", bit_stream->horizontal_size );
					OstTraceInt1(TRACE_FLOW, "GetShortHeaderDataL: bit_stream->vertical_size = %d", bit_stream->vertical_size );

				}

				//+ER353881
				if(bit_stream->custom_pcf == 1)
				{
					tmp =  getbits(8);	/* CPCFC */
				}

				/* Extended temporal reference */
				if(bit_stream->custom_pcf == 1)
				{
					tmp = getbits(2);
				}
				//-ER353881

				if(bit_stream->ss_mode)
					tmp = getbits(2); 
			}
			bit_stream->quant = getbits(5);
			if (( bit_stream->quant > 31) | ( bit_stream->quant < 1)) 
			{
                OstTraceFiltInst0(TRACE_FLOW, "Warning: quantizer out of range: clipping\n");
				//      Bitstream_mp4.quant = mmax(1,mmin(31, Bitstream_mp4.quant)); 
				if (bit_stream->quant < 1) bit_stream->quant = 1;
				if (bit_stream->quant > 31) bit_stream->quant = 31;
			}
	}
	//while(pei = getbits(1))//$ changed as per ref code. //16/06
	while(getbits(1))
	{
		/* extra info for possible future backward compatible additions */
        OstTraceFiltInst0(TRACE_FLOW, "enter here\n");
		getbits(8);  /* not used: psupp */
	}
    if(bit_stream->ss_mode)
	{
		t_uint32 MB_Nr = (bit_stream->video_object_layer_width /16) * (bit_stream->video_object_layer_height/16);
		tmp = getbits(1);//SEPB1
		if (MB_Nr <= 48)
		{
			// SUB-QCIF
			getbits(6);
		}
		else if (MB_Nr <= 99)
		{
			// QCIF
			getbits(7);
		}
		else if (MB_Nr <= 396)
		{
			// CIF
			getbits(9);
		}
		else if (MB_Nr <= 1584)
		{
			// 4CIF
			getbits(11);
		}
		else if (MB_Nr <= 6336)
		{
			// 16CIF
			getbits(13);
		}
		else
		{
			// 2048x1152
			getbits(14);
		}
		if (!getbits(1))
		{
            OstTraceFiltInst0(TRACE_FLOW, "Error Non-set SEPB3\n");
		}
	}

	OstTraceInt0(TRACE_FLOW, "<= Mpeg4dec_Parser: GetShortHeaderDataL()");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ARM_PARSER_NMF::GetSorensonDataL()
{
	OMX_U32 tmp;
	tmp = getbits(22);
	while(tmp != 0x20 && tmp != 0x21)
	{
		tmp = ((tmp << 8) | (OMX_U32)((OMX_U8)getbits(8)));
	}
	if(tmp==0x20)
	{
		bit_stream->flv_version=0;
	}
	else
	{
		bit_stream->flv_version=1;
	}
	OstTraceFiltInst0(TRACE_FLOW, "GetSorensonDataL \n");
	bit_stream->isVolHeaderFound = OMX_TRUE;
	//iShortHeader->iLastGobNumber = 0;//reset
	//iShortHeader->iGobFrameId = KInvalidGobFrameId;
    bit_stream->prev_temporal_ref = bit_stream->temporal_ref;//$ part of ref code
	bit_stream->temporal_ref = getbits(8);
	if(bit_stream->prev_temporal_ref > bit_stream->temporal_ref)
	{
		bit_stream->prev_temporal_ref -= 256;
	}
    bit_stream->source_format = getbits(3);
	OstTraceFiltInst1(TRACE_FLOW, "GetSorensonDataL bit_stream->source_format =%d\n",bit_stream->source_format);
	switch (bit_stream->source_format)
        {
        case (4):
			iShortHeader->iNumGobPerFrame = KNUM_GOB_SQCIF;
			if ((bit_stream->framenum) == 0) {
				bit_stream->horizontal_size = 128;
				bit_stream->vertical_size = 96;
                bit_stream->video_object_layer_width  = 128;
                bit_stream->video_object_layer_height = 96;
			}
			break;
        case (3):
			iShortHeader->iNumGobPerFrame = KNUM_GOB_QCIF;
			if ((bit_stream->framenum) == 0) {
				bit_stream->horizontal_size = 176;
				bit_stream->vertical_size = 144;
                 bit_stream->video_object_layer_width  = 176;
                bit_stream->video_object_layer_height = 144;
			}
			break;
        case (2):
			iShortHeader->iNumGobPerFrame = KNUM_GOB_CIF;
			if ((bit_stream->framenum) == 0) {
			    bit_stream->horizontal_size = 352;
				bit_stream->vertical_size = 288;
                 bit_stream->video_object_layer_width  = 352;
                bit_stream->video_object_layer_height = 288;
			}
			break;
        case (5):
			//iShortHeader->iNumGobPerFrame = KNUM_GOB_4CIF;
			if ((bit_stream->framenum) == 0) {
			    bit_stream->horizontal_size = 320;
				bit_stream->vertical_size = 240;
                 bit_stream->video_object_layer_width  = 320;
                bit_stream->video_object_layer_height = 240;
			}
			break;
        case (6):
			//iShortHeader->iNumGobPerFrame = KNUM_GOB_16CIF;
			if ((bit_stream->framenum) == 0) {
			    bit_stream->horizontal_size = 160;
				bit_stream->vertical_size = 128;
                 bit_stream->video_object_layer_width  = 160;
                bit_stream->video_object_layer_height = 120;
			}
			break;
	case (0):
			if ((bit_stream->framenum) == 0) {
				bit_stream->video_object_layer_width = getbits(8);
				bit_stream->horizontal_size =((bit_stream->video_object_layer_width + 15) >> 4)<<4;
				bit_stream->video_object_layer_height = getbits(8);
				bit_stream->vertical_size =((bit_stream->video_object_layer_height + 15) >> 4)<<4;
			}
            break;
	case (1):
			if ((bit_stream->framenum) == 0) {
	bit_stream->video_object_layer_width = getbits(16);
				bit_stream->horizontal_size =((bit_stream->video_object_layer_width + 15) >> 4)<<4;
				bit_stream->video_object_layer_height = getbits(16);
				bit_stream->vertical_size =((bit_stream->video_object_layer_height + 15) >> 4)<<4;
			}
            break;
        default:
            //////////MSG1("Bitstream ERROR: SourceFormat unknown %d", iShortHeader->iSourceFormat);
			return OMX_ErrorMbErrorsInFrame;
        } // end switch 


	OstTraceFiltInst1(TRACE_FLOW, "GetSorensonDataL bit_stream->horizontal_size =%d\n",bit_stream->horizontal_size);
	OstTraceFiltInst1(TRACE_FLOW, "GetSorensonDataL bit_stream->video_object_layer_width =%d\n",bit_stream->video_object_layer_width);
	OstTraceFiltInst1(TRACE_FLOW, "GetSorensonDataL bit_stream->vertical_size =%d\n",bit_stream->vertical_size);
	OstTraceFiltInst1(TRACE_FLOW, "GetSorensonDataL bit_stream->video_object_layer_height =%d\n",bit_stream->video_object_layer_height);
	bit_stream->pict_type  = getbits(2);
	//deblocking flag
	getbits(1);
	//quantizer
	bit_stream->quant = getbits(5);
	//verify
	if (bit_stream->quant< 1)
    {
	  //MSG0("Bitstream Warning: quantizer out of range: clipping");
	  bit_stream->quant = 1;
    }
		if (bit_stream->quant > 31)
    {
	  //MSG0("Bitstream Warning: quantizer out of range: clipping");
	  bit_stream->quant = 31;
    }
	//Zero bit
	while(getbits(1))
	{
		// extra info for possible future backward compatible additions 
		getbits(8);  // not used: psupp 
	}
	return OMX_ErrorNone;

}

OMX_ERRORTYPE ARM_PARSER_NMF::GetGovHeaderDataL()//$shubhrangam
{
	OMX_U32 tmp;//,temp_stuff;
    //Just check is a VOL was found earlier
	if(bit_stream->isVolHeaderFound == FALSE)
	{
		//MSG0("GOV before VOL??");
		return OMX_ErrorMbErrorsInFrame;
	}
	OstTraceFiltInst0(TRACE_FLOW, "GetGovHeaderDataL \n");
	//start parsing
	//skip GOV SC
	tmp =getbits(MP4_SC_LENGHT_IN_BITS);
	while(tmp != KGROUP_OF_VOP_START_CODE)
	{
		tmp = ((tmp << 8) | (OMX_U32)((OMX_U8)getbits(8)));
	}
	{
		//hours
		getbits(5);
		//minutes
		getbits(6);
		//marker bit
		OMX_S32 test = getbits(1);
		if(test == OMX_FALSE)
		{
			return OMX_ErrorMbErrorsInFrame;
		}
		//seconds
		getbits(6);
	}
	getbits(1);
	getbits(1);
	return OMX_ErrorNone;

}

OMX_S32 ARM_PARSER_NMF::VlcTrajPoint()
{
    OMX_S32 code;
    code = showbits(12);
	  if (code <= 1023) {
		getbits(2); 
		code = 0;
		return code;
	  }

	  if (code <= 1535) {
		getbits(3);
		code = getbits(1); 
		if (code < 1) code-=1;
		return code;
	  }
	  if (code <= 2047) {
		getbits(3);
		code = getbits(2); 
		if (code < 2) code-=3;
		return code;
	  }
	  if (code <= 2559) {
		getbits(3);
		code = getbits(3); 
		if (code < 4) code-=7;
		return code;
	  }  
	  if (code <= 3071) {
		getbits(3);
		code = getbits(4); 
		if (code < 8) code-=15;
		return code;
	  }
	  if (code <= 3583) {
		  getbits(3);
		code = getbits(5);
		if (code < 16) code-=31;
		return code;
	  }
	  if (code <= 3839) {
		  getbits(4);
		code = getbits(6);
		if (code < 32) code-=63;
		return code;
	  }
	  if (code <= 3967) {
		  getbits(5);
		code = getbits(7);
		if (code < 64) code-=127;
		return code;
	  }
	  if (code <= 4031) {
		getbits(6);
		code = getbits(8);
		if (code < 128) code-=255;
		return code;
	  }
	  if (code <= 4063) {
		  getbits(7);
		code = getbits(9); 
		if (code < 256) code-=511;
		return code;
	  }
	  if (code <= 4079) {
		getbits(8);
		code = getbits(10);
		if (code < 512) code-=1023;
		return code;
	  }
	  if (code <= 4087) {
		getbits(9);
		code = getbits(11);
		if (code < 1024) code-=2047;
		return code;
	  }
	  if (code <= 4091) {
		getbits(10);
		code = getbits(12);
		if (code < 2048) code-=4095;
		return code;
	  }
	  if (code <= 4093) {
		getbits(11);
		code = getbits(13);
		if (code < 4096) code-=8191;
		return code;
	  }
	  if (code == 4094) {
	     getbits(12);
		code = getbits(14);
		if (code < 8192) code-=16383;
		return code;
	  }  
      return(0);
}
OMX_ERRORTYPE ARM_PARSER_NMF::GetVopHeaderDataL()
{
	OMX_U32 temp,tmp;
	OstTraceInt0(TRACE_FLOW, "=> GetVopHeaderDataL()");
	//printf("\n ARM_PARSER_NMF::GetVopHeaderDataL ");
    //Just check if a VOL was found earlier
	if(bit_stream->isVolHeaderFound == FALSE)
	{
		//+ER401513
		OstTraceInt0(TRACE_FLOW, "GetVopHeaderDataL(): VOL Header was not found earlier! (bit_stream->isVolHeaderFound == FALSE)");
		//>return OMX_ErrorUndefined;
		//-ER401513
	}
	//start parsing
	//skip VOP SC
	temp = getbits(MP4_SC_LENGHT_IN_BITS);
	while(temp != KVOP_START_CODE)
	{
		temp = ((temp << 8) | (OMX_U32)((OMX_U8)getbits(8)));
	}
	bit_stream->pict_type = getbits(2);
	bit_stream->modulo_time_base = 0;
	OMX_U32 seconds = getbits(1);

	while(seconds >0)
	{
		{
			bit_stream->modulo_time_base++;
		}
		seconds = getbits(1);
	}
	//skip marker bit
	OMX_U8 test = getbits(1);
	if(test == 0)
	{
		OstTraceFiltInst0(TRACE_FLOW, "testbit 0 \n");
		return OMX_ErrorMbErrorsInFrame;
	}
	if((!(showbits(iVolHeader->iTimeIncementWidth+1)&1)) || (iVolHeader->iTimeIncementWidth==0))
	{
		t_uint32 nb_bits;
        for( nb_bits=1 ;nb_bits<16; nb_bits++)
	    {
            if (bit_stream->pict_type == 1 || (bit_stream->pict_type == 3 && bit_stream->sprite_enable == 2)) 
			{
				if(( showbits(nb_bits+6 ) & 0x37 ) == 0x30) 
					break;
			}
			else
			{
				if(( showbits(nb_bits+5 ) & 0x1F ) == 0x18) 
					break;
			}
        }
		OstTraceFiltInst1(TRACE_FLOW, "nb_bits =%d \n",nb_bits);
		bit_stream->vop_time_increment= getbits(nb_bits);
    }
	else
	{
	   bit_stream->vop_time_increment= getbits(iVolHeader->iTimeIncementWidth);
	}
    OstTraceFiltInst1(TRACE_FLOW, "bit_stream->vop_time_increment =%d \n",bit_stream->vop_time_increment);
	//marker bit
	test = getbits(1);
	if(test == OMX_FALSE)
	{
		OstTraceFiltInst0(TRACE_FLOW, "testbit 0 \n");
		return OMX_ErrorMbErrorsInFrame;
	}
	bit_stream->vop_coded = getbits(1);
	OstTraceFiltInst1(TRACE_FLOW, "vop_coded =%d \n",bit_stream->vop_coded);
	if(bit_stream->vop_coded)
	{
		bit_stream->vop_rounding_type = OMX_FALSE;
		if(bit_stream->pict_type == 0x01 || bit_stream->pict_type == 0x03)// != Intra
		{
			bit_stream->vop_rounding_type = getbits(1);
			OstTraceFiltInst1(TRACE_FLOW, "vop_rounding_type =%d \n",bit_stream->vop_rounding_type);
		}

	if ( ((bit_stream->complexity_estimation_disable) == 0) &&
         (((bit_stream->Complexity_estimation_mp4_asp.estimation_method) == 0) || ((bit_stream->Complexity_estimation_mp4_asp.estimation_method) == 1))) 
    {
     
        if ((bit_stream->pict_type) == 0)
        { /* I picture */ 
            if (bit_stream->Complexity_estimation_mp4_asp.opaque) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.transparent) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.intra_cae) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.inter_cae) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.no_update) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.upsampling) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.intra_blocks) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.not_coded_blocks) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.dct_coefs) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.dct_lines) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.vlc_symbols) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.vcl_bits) 
               getbits( 4);
			if (bit_stream->Complexity_estimation_mp4_asp.sadct) 
               getbits( 8);
        } /* end if I picture*/
		else if (((bit_stream->pict_type) == 1) || ((bit_stream->pict_type) == 3)) 
        { /* P picture */ 
            if (bit_stream->Complexity_estimation_mp4_asp.opaque)           
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.transparent)      
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.intra_cae)        
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.inter_cae)        
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.no_update)        
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.upsampling)       
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.intra_blocks)     
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.not_coded_blocks) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.dct_coefs)        
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.dct_lines)        
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.vlc_symbols)      
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.vcl_bits)         
               getbits( 4);
            if (bit_stream->Complexity_estimation_mp4_asp.inter_blocks)     
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.inter4v_blocks)   
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.apm)              
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.npm)              
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.forw_back_mc_q)   
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.halfpel2)         
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.halfpel4)         
               getbits( 8); 
			if (bit_stream->Complexity_estimation_mp4_asp.sadct)         
               getbits( 8);
			if (bit_stream->Complexity_estimation_mp4_asp.quarterpel)         
               getbits( 8);

        }else if ((bit_stream->pict_type) == 2) 
        { /* B picture */  
            if (bit_stream->Complexity_estimation_mp4_asp.opaque)           
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.transparent)      
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.intra_cae)        
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.inter_cae)        
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.no_update)        
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.upsampling)       
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.intra_blocks)     
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.not_coded_blocks) 
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.dct_coefs)        
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.dct_lines)        
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.vlc_symbols)      
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.vcl_bits)         
               getbits( 4);
            if (bit_stream->Complexity_estimation_mp4_asp.inter_blocks)     
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.inter4v_blocks)   
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.apm)              
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.npm)              
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.forw_back_mc_q)   
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.halfpel2)         
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.halfpel4)         
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.interpolate_mc_q)         
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.sadct)         
               getbits( 8);
            if (bit_stream->Complexity_estimation_mp4_asp.quarterpel)         
               getbits( 8);
        } /* end else */

    } 
		//ignore all the things in between (not valid for rectangular/ simple profile/001 etc)
		bit_stream->intra_dc_vlc_thr = getbits(3);
		if(bit_stream->sprite_enable && bit_stream->pict_type == 0x3) //for GMC
		{
			OstTraceFiltInst1(TRACE_FLOW, "no_of_sprite_warping_points =%d \n",bit_stream->no_of_sprite_warping_points);
			OstTraceFiltInst2(TRACE_FLOW, "GetVopHeaderDataL:: sprite_enable=%d pict_type =%d \n",bit_stream->sprite_enable,bit_stream->pict_type);
			if(bit_stream->no_of_sprite_warping_points)
			{
				bit_stream->difftraj_x = VlcTrajPoint();
				getbits(1);
				bit_stream->difftraj_y = VlcTrajPoint();
				getbits(1);
				OstTraceFiltInst2(TRACE_FLOW, "GetVopHeaderDataL:: difftraj_x=%d difftraj_y =%d \n",bit_stream->difftraj_x,bit_stream->difftraj_y);
			}
			if(bit_stream->sprite_brightness_change)
			{

			}

		}
		bit_stream->quant = getbits(iVolHeader->iQuantPrecision);
		//$ clipping in case of out of range (taken from ref code
		if(bit_stream->quant >31)
		{
			bit_stream->quant = 31;
		}
		else if(bit_stream->quant < 1)
		{
			bit_stream->quant = 1;
		}
		bit_stream->vop_fcode_forward = 0x01;
		if(bit_stream->pict_type != 0x00)//Not an I frame
		{
			bit_stream->vop_fcode_forward = getbits(3);
			if(bit_stream->vop_fcode_forward==0 || bit_stream->vop_fcode_forward>7)
			{
//				MSG1(" corrupt vop code forward value %d", iVopHeader->iVopCodeForward);
				return OMX_ErrorMbErrorsInFrame;
			}
		}
		/* frame B */
		tmp = (bit_stream->pict_type == 2) ? getbits(3) : 1;
		bit_stream->vop_fcode_backward = (tmp == 0) ? 1 : tmp;

		//update resync marker size in bits if resync is enables in VOL
		if(bit_stream->resync_marker_disable == OMX_FALSE)
		{
			iVopHeader->iResyncMarkerSize = KRESYNC_MARKER_SIZE + bit_stream->vop_fcode_forward;
		}
	}
	OstTraceInt0(TRACE_FLOW, "<= GetVopHeaderDataL()");
	return OMX_ErrorNone;

}


OMX_ERRORTYPE ARM_PARSER_NMF::GetVopVpHeaderDataL()//$shubhrangam
{
	OMX_U32 tmp;
	OMX_U8	iHecHeaderPresent;
	OMX_U8	iHecVopCodingType;
	OstTraceFiltInst0(TRACE_FLOW, "GetVopVpHeaderDataL \n");
	//start parsing
	//skip the RESYN_MARKER bits
	tmp = getbits(iVopHeader->iResyncMarkerSize);
	while(tmp != KSP_RESYNC_MARKER)
	{
		tmp = ((tmp << 1) | (OMX_U32)((OMX_U8)getbits(1)));//$ check if 1 is good value
	}
	//iVideoPacketHeader->iVpMbNumber = 
		getbits(iVolHeader->iMbNumberFieldWidth);

	//iVideoPacketHeader->iHecQuantScale = 
		getbits(iVolHeader->iQuantPrecision);

	iHecHeaderPresent = getbits(1);

	if(iHecHeaderPresent)
	{
		//iVideoPacketHeader->iHecModuloTimeBase = 0;
		OMX_U32 seconds = getbits(1);

		while(seconds >0)
		{
			//iVideoPacketHeader->iHecModuloTimeBase++;
			seconds = getbits(1);
		}

		//skip marker bit
		OMX_U8 test = getbits(1);
		if(test == 0)
		{
			return OMX_ErrorMbErrorsInFrame;
		}
		//iVideoPacketHeader->iHecVopTimeIncrement = 
			getbits(iVolHeader->iTimeIncementWidth);

		test = getbits(1);
		if(test == 0)
		{
			return OMX_ErrorMbErrorsInFrame;
		}

		iHecVopCodingType = getbits(2);

		//iVideoPacketHeader->iHecIntraDcVlcThr = getbits(3);

		//iVideoPacketHeader->iHecVopCodeForward = 0x01;
		if(iHecVopCodingType != 0x00)
		{
			//iVideoPacketHeader->iHecVopCodeForward = 
				getbits(3);
		}
	}
	return OMX_ErrorNone;

}
OMX_ERRORTYPE ARM_PARSER_NMF::GetShortHeaderGobDataL()//$shubhrangam
{
	OMX_U32 tmp;
	OstTraceInt0(TRACE_FLOW, "GetShortHeaderGobDataL \n");
	//start parsing
	//skip the RESYN_MARKER bits
	tmp = getbits(iShortHeader->iResyncMarkerSize);
	while(tmp != KSH_RESYNC_MARKER)
	{
		tmp = ((tmp << 1) | (OMX_U32)((OMX_U8)getbits(1)));//$ check if 1 is good value
	}

	if(bit_stream->iShortHeader != TRUE)
	{
		OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::iShortHeader \n");
		return OMX_ErrorFormatNotDetected;
	}

	//iShSliceHeader->iGobNumber
	getbits(5);
	//iShSliceHeader->iGobFrameId
	getbits(2);
	//iShSliceHeader->iVopQuant 
	getbits(5);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ARM_PARSER_NMF::GetVolHeaderDataL(OMX_U32 temp)
{
	OMX_U8 test;
	//OMX_U32 temp;
	OMX_U32 tmp, tmp2;
	int k;
	OstTraceFiltInst0(TRACE_FLOW, "GetVolHeaderDataL \n");
	// +ER446263
	//temp = getbits(MP4_SC_LENGHT_IN_BITS);
	//temp = 0x120;
	/*while(!(temp >= KVIDEO_OBJECT_LAYER_START_CODE_SMALLEST && temp <= KVIDEO_OBJECT_LAYER_START_CODE_GREATEST))
	{
		temp = ((temp << 8) | (OMX_U32)((OMX_U8)getbits(8)));
	}*/
	// -ER446263
    getbitscount();
	//printf("\n bitCount = %d",bitCount);
	bit_stream->isVolHeaderFound = OMX_TRUE;
	bit_stream->video_object_layer_verid = (temp & 0xF);//last 4 bits of the start code
	//iVolHeader->iRandomAccessible
	getbits(1);
	//printf("\n Address of iVolHeader 0x%x",iVolHeader);
	//printf("\n Address of bit_stream 0x%x",bit_stream);
	//validate
	if(getbits(8) != 0x1)
	{
		// //MSG0("Video Object type not supported");
		//return KErrNotSupported;
		//$return OMX_ErrorUnsupportedIndex; //$ this format is supported, finally remove this check
	} 
	if(getbits(1))
	{
		bit_stream->video_object_layer_verid = getbits(4);
		//iVolHeader->iVideoObjectLayerPriority
		getbits(3);
	}
	else
	{ // assume video_object_layer_verid='0001' (Version 1) assumed
			bit_stream->video_object_layer_verid = 1;
	}
    OMX_U8	iVolControlParams;
	OMX_U8	iVbvParams;
	OMX_U8	iVideoObjetLayerShape;
    bit_stream->AspectRatio = getbits(4);
	//printf("\n GetVolHeaderDataL::bit_stream->AspectRatio = %d",bit_stream->AspectRatio);
	//if(AspectRatio == KExtendedPar) //extended PAR
	if(bit_stream->AspectRatio == KExtendedPar) 
	{
		//skip next 16 bytes (8+8)
		//iExtendedParNum
	   // AspectRatio=bit_stream->AspectRatio;
		nHorizontal=getbits(8);
		//iExtendedParDen
		nVertical=getbits(8);
	}
	
	iVolControlParams = getbits(1);
	if(iVolControlParams)
	{
		//iVolHeader->iChromaFormat 
		getbits(2);
		bit_stream->lowDelay  = getbits(1);
		iVbvParams = getbits(1);

		if(iVbvParams)
		{
			//iVolHeader->iFirstHalfBitRate 
			getbits(15);
			//skip marker bit
			test = getbits(1);
			if(test == 0)
			{
				OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit \n");
				return OMX_ErrorFormatNotDetected;
			}
			//iVolHeader->iLatterHalfBitRate 
		    getbits(15);
			//skip marker bit
			test = getbits(1);
			if(test == 0)
			{
				OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 1 \n");
				return OMX_ErrorFormatNotDetected;
			}
			//iVolHeader->iFirstHalfVbvBufferSize 
			getbits(15);
			//skip marker bit
			test = getbits(1);
			if(test == 0)
			{
				OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 2\n");
				return OMX_ErrorFormatNotDetected;
			}
			//iVolHeader->iLatterHalfVbvBufferSize 
			 getbits(3);
			//iVolHeader->iFirstHalfVbvOccupancy 
		    getbits(11);
			//skip marker bit
			test = getbits(1);
			if(test == 0)
			{
				OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 3\n");
				return OMX_ErrorFormatNotDetected;
			}
			//iVolHeader->iLatterHalfVbvOccupancy 
		    getbits(15);
			//skip marker bit
			test = getbits(1);
			if(test == 0)
			{
				OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 4 \n");
				return OMX_ErrorFormatNotDetected;
			}
		}
	}
	iVideoObjetLayerShape = getbits(2);
	//validate
	if(iVideoObjetLayerShape != 00)
	{
		OstTraceInt0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 5\n");
		return OMX_ErrorUndefined;
	}
	//skip marker bit
	test = getbits(1);
	if(test == 0)
	{
		OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 6\n");
		return OMX_ErrorFormatNotDetected;
	}
  	OMX_U32    tmp_width;
	bit_stream->vop_time_increment_resolution = getbits(16);
	{
		OMX_U32 tmp2,high, low ,nbits;//$these should be 8 bit check for all ???
		tmp_width = bit_stream->vop_time_increment_resolution;
		tmp_width = (tmp_width>1) ? (tmp_width - 1) : 1;
		high   = (tmp_width >> 8) & 255;
		low    = tmp_width & 255;
		nbits = (high==0) ? 0 : 8;
		tmp2   = (high==0)    ?   low : high;
		nbits += (tmp2>0) +(tmp2>1) +(tmp2>3) +(tmp2>7)+
		         (tmp2>15)+(tmp2>31)+(tmp2>63)+(tmp2>127);

		iVolHeader->iTimeIncementWidth = nbits;
		OstTraceFiltInst1(TRACE_FLOW, "iVolHeader->iTimeIncementWidth =%d \n",iVolHeader->iTimeIncementWidth);
	}
	//skip marker bit
	test = getbits(1);
	if(test == 0)
	{
		OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 7\n");
		return OMX_ErrorFormatNotDetected;
	}
   /********************************************/
   /*** Fixed VOP rate              (1 bit)  ***/
   /*** and eventually:                      ***/
   /*** Fixed vop time increment  (1-16 bits)***/
   /********************************************/
	bit_stream->fixed_vop_time_increment = getbits(1);
   if(bit_stream->fixed_vop_time_increment) //jitender::bit_stream->fixed_vop_time_increment behaviour changes
   {
     OMX_U32 tmp2,high, low ,nbits;//$these should be 8 bit check for all ???
     tmp = bit_stream->vop_time_increment_resolution;
	 bit_stream->fixed_vop_rate = bit_stream->fixed_vop_time_increment; //$ Fix Me : Typo error should be fixedvoprate
     tmp = (tmp>1) ? (tmp - 1) : 1;
     high   = (tmp >> 8) & 255;
     low    = tmp & 255;
     nbits  = (high==0)    ?   0   : 8;
     tmp2   = (high==0)    ?   low : high;
     nbits += (tmp2>0) +(tmp2>1) +(tmp2>3) +(tmp2>7)+
              (tmp2>15)+(tmp2>31)+(tmp2>63)+(tmp2>127);
     bit_stream->fixed_vop_time_increment = getbits(nbits);
   }

   if(iVideoObjetLayerShape == 00) //is true for us(!!)
   {
		//skip marker bit
		test = getbits(1);
		if(test == 0)
		{
			OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 8\n");
			return OMX_ErrorFormatNotDetected;
		}
		bit_stream->video_object_layer_width = getbits(13);
		bit_stream->horizontal_size =((bit_stream->video_object_layer_width + 15) >> 4)<<4;
		//skip marker bit
		test = getbits(1);
		if(test == 0)
		{
			OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 9\n");
			return OMX_ErrorFormatNotDetected;
		}
		bit_stream->video_object_layer_height = getbits(13);
		bit_stream->vertical_size = ((bit_stream->video_object_layer_height + 15) >> 4)<<4;
		//printf("\nVOLHEADER horizontal_size = %d vertical_size = %d video_object_layer_width = %d horizontal_size = %d",bit_stream->horizontal_size,bit_stream->vertical_size,bit_stream->video_object_layer_width,bit_stream->video_object_layer_height);
        //skip marker bit
		test = getbits(1);
		if(test == 0)
		{
			OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 10\n");
			return OMX_ErrorFormatNotDetected;
		}
		//skip interlaced
		bit_stream->isInterlaced = getbits(1);
		//+patch for ER336159
		if(bit_stream->isInterlaced)
		{
			OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::Interlaced\n");
			return OMX_ErrorFormatNotDetected;
		}
		//-patch for ER336159
		//skip obmc_disable
		getbits(1);
		
		if(bit_stream->video_object_layer_verid == 1)
		{ 
            bit_stream->sprite_enable = getbits(1);
			if(bit_stream->sprite_enable)
			{
				OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 11\n");
				//static sprite not yet supported
				return OMX_ErrorFormatNotDetected;
			}
		}
		else
		{
			bit_stream->sprite_enable = getbits(2); 
			if(bit_stream->sprite_enable != 2 && bit_stream->sprite_enable != 0 )  //GMC and sprite not used
			{
				OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 12\n");
				//static sprite not yet supported
				return OMX_ErrorFormatNotDetected;
			}
		}
        if(bit_stream->sprite_enable == 2 || bit_stream->sprite_enable == 1)
		{ 
			if(bit_stream->sprite_enable != 2)
			{
				getbits(56);
			}
			bit_stream->no_of_sprite_warping_points=getbits(6);  //1 
			if(bit_stream->no_of_sprite_warping_points !=1)
			{
				OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 13\n");
				//warping_points =1 only supported yet
				return OMX_ErrorFormatNotDetected;

			}
			bit_stream->sprite_warping_accuracy=getbits(2);  
			if(bit_stream->sprite_warping_accuracy)
			{
				OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 14\n");
				//only zero accuracy supported
				return OMX_ErrorFormatNotDetected;

			}
			bit_stream->sprite_brightness_change=getbits(1);  //0
			if(bit_stream->sprite_brightness_change)
			{
				OstTraceFiltInst0(TRACE_FLOW, "OMX_ErrorFormatNotDetected ::skip marker bit 15\n");
				//only zero brightness_change supported
				return OMX_ErrorFormatNotDetected;
			}

            if(bit_stream->sprite_enable != 2)
			{
				getbits(1);
			}
		}
		if(getbits(1))
		{
			iVolHeader->iQuantPrecision = getbits(4);
			//iVolHeader->iBitsPerPixel
			getbits(4);
		}
		else//$set default values
		{
		 	iVolHeader->iQuantPrecision = 5;
		}
		OMX_U8 quantType = getbits(1);
		bit_stream->quant_type = quantType;
		if(quantType)
		{
			if(getbits(1))   /* load_intra_quant_mat */
			{
				k = 0;
				do
				{
					tmp2 = getbits(8);
					bit_stream->intraQuantizationMatrix[k] = (OMX_U16)tmp2;
					k++;
				} while ((k < 64) && (tmp2 != 0));
				if (tmp2 == 0)
				{
					k--;
					tmp2 = bit_stream->intraQuantizationMatrix[k - 1];
					while (k < 64)
					{
						bit_stream->intraQuantizationMatrix[k] = (OMX_U16)tmp2;
						k++;
					}
				}
			}
			else
			{
				for (k = 0; k < 64 ; k++)
				{
					bit_stream->intraQuantizationMatrix[k] = LUT_intra_quant_mat[k];
				}
			}

			if (getbits(1))   /* load_nonintra_quant_mat */
			{
				k = 0;
				do
				{
					tmp2 = getbits(8);
					bit_stream->nonIntraQuantizationMatrix[k] = (OMX_U16)tmp2;
					k++;
				} while ((k < 64) && (tmp2 != 0));
				if (tmp2 == 0)
				{
					k--;
					tmp2 = bit_stream->nonIntraQuantizationMatrix[k - 1];
					while (k < 64)
					{
						bit_stream->nonIntraQuantizationMatrix[k] = (OMX_U16)tmp2;
						k++;
					}
				}
			}
			else
			{
				for (k = 0; k < 64 ; k++)
				{
					bit_stream->nonIntraQuantizationMatrix[k] = LUT_nonintra_quant_mat[k];
				}
			}
		}
		if(bit_stream->video_object_layer_verid != 1)
		{
          bit_stream->quarter_sample=getbits(1);
		}
		//check complexity estimation
		test = bit_stream->complexity_estimation_disable = getbits(1);

		if(!bit_stream->complexity_estimation_disable)
		{
			  t_uint32 bits,count=0,read_bits_till_resync=1;
			  //tmp_bit_pos=bit_stream->Bitstream_buffer_asp.index_current_bit;
			  //tmp_byte_pos=bit_stream->Bitstream_buffer_asp.pt_current_byte;
			  bits = showbits(2);
			  count += 2;
			  bit_stream->Complexity_estimation_mp4_asp.estimation_method = bits;
			  OstTraceFiltInst1(TRACE_FLOW, "estimation_method =%d \n",bit_stream->Complexity_estimation_mp4_asp.estimation_method);
			  if (bits == 0 || bits == 1)  
			  {
			   /* shape complexity estimation */
				   count++;
				   if (!(showbits(count)>>(count-1))) 
				   {	
					   count++;
					   bit_stream->Complexity_estimation_mp4_asp.opaque           
						  = (showbits(count)>>(count-1));
					   count++;
					   bit_stream->Complexity_estimation_mp4_asp.transparent     
						  = (showbits(count)>>(count-1));
					   count++;
					   bit_stream->Complexity_estimation_mp4_asp.intra_cae        
						  = (showbits(count)>>(count-1));
					   count++;
					   bit_stream->Complexity_estimation_mp4_asp.inter_cae        
						  = (showbits(count)>>(count-1));
					   count++;
					   bit_stream->Complexity_estimation_mp4_asp.no_update        
						  = (showbits(count)>>(count-1));
					   count++;
					   bit_stream->Complexity_estimation_mp4_asp.upsampling       
						  = (showbits(count)>>(count-1));
					   OstTraceFiltInst2(TRACE_FLOW, "opaque=%d transparent =%d \n",bit_stream->Complexity_estimation_mp4_asp.opaque,bit_stream->Complexity_estimation_mp4_asp.transparent);
					   OstTraceFiltInst2(TRACE_FLOW, "intra_cae=%d inter_cae =%d \n",bit_stream->Complexity_estimation_mp4_asp.intra_cae,bit_stream->Complexity_estimation_mp4_asp.inter_cae);
					   OstTraceFiltInst2(TRACE_FLOW, "no_update=%d upsampling =%d \n",bit_stream->Complexity_estimation_mp4_asp.no_update,bit_stream->Complexity_estimation_mp4_asp.upsampling);
				   } /* end if shape complexity estimation */
	  
				   /* texture complexity estimation set 1 */
				   count++;
				   if (!showbits(1)) 
				   {
					   count++;
					  bit_stream->Complexity_estimation_mp4_asp.intra_blocks 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.inter_blocks 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.inter4v_blocks 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.not_coded_blocks 
					 = (showbits(count)>>(count-1));
					  OstTraceFiltInst2(TRACE_FLOW, "intra_blocks=%d inter_blocks =%d \n",bit_stream->Complexity_estimation_mp4_asp.opaque,bit_stream->Complexity_estimation_mp4_asp.inter_blocks);
					  OstTraceFiltInst2(TRACE_FLOW, "inter4v_blocks=%d not_coded_blocks =%d \n",bit_stream->Complexity_estimation_mp4_asp.inter4v_blocks,bit_stream->Complexity_estimation_mp4_asp.not_coded_blocks);
				   } /* end if texture complexity estimation set 1 */
				   count++;
				   if (!showbits(1))
				   {	
					  read_bits_till_resync=0;
					  bit_stream->complexity_estimation_disable =1;
					  OstTraceFiltInst0(TRACE_FLOW, "JUMP to resync marker \n");
					  count=0;
				   }
				  /* texture complexity estimation set 2 */
				   count++;
				  if (read_bits_till_resync && (!showbits(1))) 
				  {
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.dct_coefs 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.dct_lines 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.vlc_symbols 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.vcl_bits 
					 = (showbits(count)>>(count-1));
					  OstTraceFiltInst2(TRACE_FLOW, "dct_coefs=%d dct_lines =%d \n",bit_stream->Complexity_estimation_mp4_asp.dct_coefs,bit_stream->Complexity_estimation_mp4_asp.dct_lines);
					  OstTraceFiltInst2(TRACE_FLOW, "vlc_symbols=%d vcl_bits =%d \n",bit_stream->Complexity_estimation_mp4_asp.vlc_symbols,bit_stream->Complexity_estimation_mp4_asp.vcl_bits);
				  } /* end if texture complexity estimation set 2 */
		 
				  /* motion compensation complexity */
				  count++;
				  if (read_bits_till_resync && (!showbits(1))) 
				  {
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.apm 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.npm 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.interpolate_mc_q 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.forw_back_mc_q 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.halfpel2 
					 = (showbits(count)>>(count-1));
					  count++;
					  bit_stream->Complexity_estimation_mp4_asp.halfpel4 
					 = (showbits(count)>>(count-1));
					  OstTraceFiltInst2(TRACE_FLOW, "apm=%d npm =%d \n",bit_stream->Complexity_estimation_mp4_asp.apm,bit_stream->Complexity_estimation_mp4_asp.npm);
					  OstTraceFiltInst2(TRACE_FLOW, "interpolate_mc_q=%d forw_back_mc_q =%d \n",bit_stream->Complexity_estimation_mp4_asp.interpolate_mc_q,bit_stream->Complexity_estimation_mp4_asp.forw_back_mc_q);
					  OstTraceFiltInst2(TRACE_FLOW, "halfpel2=%d halfpel4 =%d \n",bit_stream->Complexity_estimation_mp4_asp.halfpel2,bit_stream->Complexity_estimation_mp4_asp.halfpel4);
				  } /* end if motion compensation complexity */
				  count++;
				  if (read_bits_till_resync && (!showbits(1)))
				  {	
					  bit_stream->complexity_estimation_disable =1;
					  read_bits_till_resync=0;
					  OstTraceFiltInst0(TRACE_FLOW, "JUMP to resync marker \n");
					  count=0;
				  }
				  if(read_bits_till_resync)
				  {
					  OstTraceFiltInst1(TRACE_FLOW, "read_bits_till_resync  count =%d\n",count);
					  getbits(count);
					  count=0;
				  }
				   /* if estimation method == '01' */
				   if(read_bits_till_resync && (bits == 1))
				   {
					   /* version2_complexity_estimation_disable */
					   if(!getbits(1))
					   {
						   bit_stream->Complexity_estimation_mp4_asp.sadct
						   = getbits(1);
						   /* quarterpel */
						   bit_stream->Complexity_estimation_mp4_asp.quarterpel 
						  = getbits(1);
						   OstTraceFiltInst2(TRACE_FLOW, "sadct=%d quarterpel =%d \n",bit_stream->Complexity_estimation_mp4_asp.sadct,bit_stream->Complexity_estimation_mp4_asp.quarterpel);
					   }
				   }
			  }
			  else
			  {
				  OstTraceFiltInst0(TRACE_FLOW, "estimation_method other that 0,1 \n");
				  count=0;
			  }
		} 

   }

    bit_stream->resync_marker_disable = getbits(1);
	bit_stream->data_partitioned = getbits(1);
	if(bit_stream->data_partitioned)
	{
		bit_stream->reversible_vlc = getbits(1);
	}
	else
	{
			bit_stream->reversible_vlc = 0;
	}
	if( bit_stream->video_object_layer_verid != 1 ) 
	{
		OMX_U8 newpred_enable;
		newpred_enable = getbits(1);
		if (newpred_enable)
		{
			getbits(3);

		}
		getbits(1);
	}
	//Now that we have width and height lets update iMbNumberFieldWidth
	OMX_U32 heightRounding = (bit_stream->video_object_layer_height + 15)/16;
	OMX_U32 widthRounding = (bit_stream->video_object_layer_width + 15)/16;
	//bit_stream->iMbNumberFieldWidth = TVolHeader::LogBase2(heightRounding * widthRounding);
	iVolHeader->iMbNumberFieldWidth = heightRounding * widthRounding;
    return OMX_ErrorNone;
}

unsigned int ARM_PARSER_NMF::getbits(int n)
{
  unsigned int l;
  unsigned int b;
  unsigned char *v;
  int c;
  int tmp;

  /* fill internal buffer if necessary */
  if (bits_handle->incnt<(unsigned int)n)
    fillbfr();

  tmp = (bits_handle->incnt);

  v = bits_handle->inbfr + ((96 - tmp)>>3);
  b = (v[0]<<24) | (v[1]<<16) | (v[2]<<8) | v[3];
  c = ((tmp-1) & 7) + 25;
  tmp = c-n;

  l = (tmp<0) ? ((b<<(-tmp)) & msk[n]) : ((b>>(tmp)) & msk[n]);

  flushbits(n);

  return l;

}
unsigned int ARM_PARSER_NMF::getbitscount()
{
	return (bits_handle->incnt1 - bits_handle->incnt);
}

unsigned int ARM_PARSER_NMF::showbits(int n)
{
  unsigned char *v;
  unsigned int b;
  int c;
  int tmp;

  /* fill internal buffer if necessary */
  if (bits_handle->incnt<(unsigned int)n)
    fillbfr();

  tmp = (bits_handle->incnt);

  v = bits_handle->inbfr + ((96 - tmp)>>3);
  b = (v[0]<<24) | (v[1]<<16) | (v[2]<<8) | v[3];
  c = ((tmp-1) & 7) + 25;
  tmp = c-n;

  return ((tmp<0) ? ((b<<(-tmp)) & msk[n]) : ((b>>(tmp)) & msk[n]));
}

int ARM_PARSER_NMF::next_start_code()
{
	  int exp_bits;
  int get_bits;
  int length_bits;
  int tmp;

  /* length_bits = (tmp = (bits_handle->incnt)%8) ? tmp : 8; */

  //length_bits = (tmp = ((bits_handle->incnt)&7)) ? tmp : 8;

  tmp = ((bits_handle->incnt)&7);
  length_bits = tmp ? tmp : 8;

  get_bits = getbits(length_bits);

  exp_bits = (1 << (length_bits - 1)) - 1;

  if(get_bits == exp_bits)
    return 0;

  return -1;
}


void ARM_PARSER_NMF::flushbits(int n)
{
  bits_handle->incnt-= (n);
  if ((OMX_S32)bits_handle->incnt < 0)
    fillbfr();
}

void ARM_PARSER_NMF::fillbfr(void)
{
  unsigned int *local_inbfr;
  unsigned int *local_rdptr;

  local_inbfr = (unsigned int *)(bits_handle->inbfr);
  local_rdptr = (unsigned int *)(bits_handle->rdptr);

  /* update inbfr buffer */
  local_inbfr[0] = local_inbfr[2];

  /* update inbfr buffer with the new 8 bytes */
  local_inbfr[1] = *(local_rdptr);
  local_inbfr[2] = *(local_rdptr+1);

  /* update rdptr pointer to the rdbfr buffer of 8 positions (8 bytes) and
  update inctn counter of 64 bits (8 bytes)                             */
  bits_handle->rdptr+= 8;
  bits_handle->incnt+= 64;
  bits_handle->incnt1+= 64;
}

unsigned int ARM_PARSER_NMF::nextbits_bytealigned(int n)
{
  unsigned char *v;
  unsigned int b;
  int c;

  int length_bits;
  int sum;
  int tmp;

  tmp = (bits_handle->incnt);

  /*
  if(!(length_bits = ((bits_handle->incnt)&7)))
  length_bits = (showbits(8) == 127) ? 8 : 0;
  */    /* 127 = 01111111 */

  length_bits = ((tmp)&7);
  length_bits = (length_bits) ? length_bits : 8;


  sum = length_bits + n;

  /* fill buffer if necessary */
  if (tmp<sum)
    fillbfr();

  tmp = (bits_handle->incnt);

  v = bits_handle->inbfr + ((96 - tmp)>>3);
  b = (v[0]<<24) | (v[1]<<16) | (v[2]<<8) | v[3];
  c = ((tmp-1) & 7) + 25;
  return (b>>(c-sum)) & msk[n];
}

ARM_PARSER_NMF::ARM_PARSER_NMF()
{
	for(int i=0;i<VFM_HEADER_FIFO_COUNT;i++)
	{
		headerDesc[i].mpcAddress=0;
	    headerDesc[i].isRequestedToFree=OMX_FALSE;
		headerDesc[i].isFree=OMX_FALSE;
		headerDesc[i].pHeaderDesc=0;
		pBufferInArray[i]=NULL;
	}
	OstTraceInt0(TRACE_FLOW, "ARM_PARSER_NMF \n");
    QIndex=0;
	DQIndex=0;
	pDesc = (ts_vfm_vdc_mpeg4_header_desc *)NULL;
	pBufferIn=NULL;
	config_concealmentflag=0;
	error_reporting=0;
	errormap_ptr=(t_uint16 *)NULL;
	mParamBufferDesc.nSize = 0;
    ColorPrimary=0;
	AspectRatio=0;
	nHorizontal=0;
	nVertical=0;
    StartCodeCount=0;
    InternalBufferAddress=0;
	bImmediateIFlag = 0;
	pBufferForI = 0;

	//+ER329550
	do_not_parse = OMX_FALSE;
	//-ER329550
	//+ER426245
	port_output_was_disabled = 0;
	//-ER426245

	mParamBufferDesc.nMpcAddress = 0;
	mParamBufferDesc.nLogicalAddress = 0;
	mParamBufferDesc.nPhysicalAddress = 0;

	mDebugBufferDesc.nSize = 0;
	mDebugBufferDesc.nMpcAddress = 0;
	mDebugBufferDesc.nLogicalAddress = 0;
	mDebugBufferDesc.nPhysicalAddress = 0;

	mLinkListBufferDesc.nSize = 0;
	mLinkListBufferDesc.nMpcAddress = 0;
	mLinkListBufferDesc.nLogicalAddress = 0;
	mLinkListBufferDesc.nPhysicalAddress = 0;
	pMpeg4ParamIn=0;
    
   save_memory_context=0;
   frame_info_width=0;
   frame_info_height=0;

   for(int i=0;i<VFM_HEADER_FIFO_COUNT;i++)
   {
		OMX_BUFFERHEADERTYPE tmp ;
        memset(&tmp,0,sizeof(tmp));
		mBufIn[i] = tmp;
		mBufOut[i] = tmp;
   }
   bits_handle = &base;
    //ts_bitstream initialization
		   ts_bitstream.codingType=0;                        
		   ts_bitstream.profile=OMX_VIDEO_MPEG4ProfileSimple;                           
		   ts_bitstream.level=OMX_VIDEO_MPEG4Level0;                              
		   ts_bitstream.firstFrame=0;                        
		   ts_bitstream.updateParamsDone=0;                  
		   ts_bitstream.vop_time_increment_resolution=0;                   
		   ts_bitstream.horizontal_size=0;                   
		   ts_bitstream.vertical_size=0;                     
		   ts_bitstream.fixed_vop_rate=0;                    
		   ts_bitstream.fixed_vop_time_increment=0;          
		   ts_bitstream.video_object_layer_width=0;          
		   ts_bitstream.video_object_layer_height=0;         
		   ts_bitstream.reversible_vlc=0;                    
		   ts_bitstream.data_partitioned=0;                  
		   ts_bitstream.resync_marker_disable=1;             
		   ts_bitstream.complexity_estimation_disable=1;     
		   ts_bitstream.video_object_layer_verid=0;          
		   ts_bitstream.lowDelay=0;                          
		   ts_bitstream.isInterlaced=0;                      
		   ts_bitstream.quant_type=0;                        
		   for(int i=0;i<64;i++)                              
		   {                                                  
			   ts_bitstream.intraQuantizationMatrix[i]=0;    
			   ts_bitstream.nonIntraQuantizationMatrix[i]=0; 
		   }       
		   ts_bitstream.flv_version=0;
		   ts_bitstream.flag_sorenson=0;
		   ts_bitstream.prev_temporal_ref=0;                 
		   ts_bitstream.temporal_ref=0;                      
		   ts_bitstream.source_format=0;                     
		   ts_bitstream.framenum=0;                          
		   ts_bitstream.pict_type=0;                         
		   ts_bitstream.pictType=OMX_VIDEO_PictureTypeI;                          
		   ts_bitstream.modulo_time_base=0;                  
		   ts_bitstream.vop_time_increment=0;                
		   ts_bitstream.vop_coded=0;                         
		   ts_bitstream.vop_rounding_type=0;                 
		   ts_bitstream.intra_dc_vlc_thr=0;                  
		   ts_bitstream.quant=0;                             
		   ts_bitstream.vop_fcode_forward=0;                 
		   ts_bitstream.vop_fcode_backward=0;                
		   ts_bitstream.isVolHeaderFound=OMX_FALSE;  
		   ts_bitstream.quarter_sample=0;                
		   ts_bitstream.iShortHeader=OMX_FALSE;    
		   ts_bitstream.ColorPrimary=0;
		   ts_bitstream.AspectRatio=0;
		   ts_bitstream.sprite_enable=0;              
		   ts_bitstream.no_of_sprite_warping_points=0;
		   ts_bitstream.sprite_warping_accuracy=0;    
		   ts_bitstream.sprite_brightness_change=0;   
		   ts_bitstream.difftraj_x=0;                 
		   ts_bitstream.difftraj_y=0; 
		   ts_bitstream.h263_annexsupport=0;
		   ts_bitstream.aic_mode=0;
	       ts_bitstream.df_mode =0;
	       ts_bitstream.ss_mode =0;
	       ts_bitstream.mq_mode =0;
		   ts_bitstream.Complexity_estimation_mp4_asp.estimation_method=0;
           ts_bitstream.Complexity_estimation_mp4_asp.opaque=0;
           ts_bitstream.Complexity_estimation_mp4_asp.transparent=0;
           ts_bitstream.Complexity_estimation_mp4_asp.intra_cae=0;
           ts_bitstream.Complexity_estimation_mp4_asp.inter_cae=0;
           ts_bitstream.Complexity_estimation_mp4_asp.no_update=0;
           ts_bitstream.Complexity_estimation_mp4_asp.upsampling=0;
           ts_bitstream.Complexity_estimation_mp4_asp.intra_blocks=0;
           ts_bitstream.Complexity_estimation_mp4_asp.inter_blocks=0;
           ts_bitstream.Complexity_estimation_mp4_asp.inter4v_blocks=0;
           ts_bitstream.Complexity_estimation_mp4_asp.not_coded_blocks=0;
           ts_bitstream.Complexity_estimation_mp4_asp.dct_coefs=0;
           ts_bitstream.Complexity_estimation_mp4_asp.dct_lines=0;
           ts_bitstream.Complexity_estimation_mp4_asp.vlc_symbols=0;
           ts_bitstream.Complexity_estimation_mp4_asp.vcl_bits=0;
           ts_bitstream.Complexity_estimation_mp4_asp.apm=0;
           ts_bitstream.Complexity_estimation_mp4_asp.npm=0;
           ts_bitstream.Complexity_estimation_mp4_asp.interpolate_mc_q=0;
           ts_bitstream.Complexity_estimation_mp4_asp.forw_back_mc_q=0;
           ts_bitstream.Complexity_estimation_mp4_asp.halfpel2=0;
           ts_bitstream.Complexity_estimation_mp4_asp.halfpel4=0;
           ts_bitstream.Complexity_estimation_mp4_asp.sadct=0;
           ts_bitstream.Complexity_estimation_mp4_asp.quarterpel=0; 

		   ts_vopheader.iResyncMarkerSize=0;	
		   

	//  initialization of  ts_volheader
		ts_volheader.iTimeIncementWidth=0;
        ts_volheader.iQuantPrecision=0;
        ts_volheader.iMbNumberFieldWidth=0;

    ts_ishortheader.iResyncMarkerSize=0; 
    ts_ishortheader.iNumGobPerFrame=0;

//+ER 354962
#ifdef SOFT_DECODER
	mNbPendingCommands = 0;
	omxilosalservices::OmxILOsalMutex::MutexCreate(mMutexPendingCommand);
#endif
//-ER 354962
}


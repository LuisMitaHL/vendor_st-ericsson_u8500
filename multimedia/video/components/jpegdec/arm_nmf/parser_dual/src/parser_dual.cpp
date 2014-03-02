/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "jpegdec/arm_nmf/parser_dual.nmf"
#include <string.h>
#include "SharedBuffer.h"
//#include "mpc/common/buffer.idt"
#ifdef __CACHE_OPTIOLD_
	#include "mmhwbuffer.h"
#endif

#ifdef __CACHE_OPTI
#include "VFM_Memory.h"
#endif


#ifdef __PERF_MEASUREMENT
#include "../../../proxy/test_arm/osi_perf.h"
#endif


#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegdec_arm_nmf_parser_dual_src_parser_dualTraces.h"
#endif

#define BUFFERFLAG_EOS OMX_BUFFERFLAG_EOS

#define NO_DOWNSAMPLING 1


const OMX_U16 huff_dc_code_y_hw_table[12] =
{
  0x0,0x2,0x3,0x4,0x5,0x6,0xe,0x1e,0x3e,0x7e,0xfe,0x1fe
};

/*const t_uint16 huff_dc_size_croma_hw_table[12] =
{
  2,2,2,3,4,5,6,7,8,9,10,11
};*/

const OMX_U16 huff_dc_code_croma_hw_table[12] =
{
  0x0,0x1,0x2,0x6,0xe,0x1e,0x3e,0x7e,0xfe,0x1fe,0x3fe,0x7fe
};



const OMX_U8 huff_ac_luma_val_hw_table[38] =
{
  0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,
  0x11,0x12,0x13,0x14,0x15,0x21,0x22,0x23,0x24,
  0x31,0x32,0x33,0x41,0x42,0x51,0x52,0x61,0x62,
  0x71,0x72,0x81,0x82,0x91,0xA1,0xB1,0xC1,0xD1,0xF0,0xFA
};

const OMX_U16 huff_ac_code_luma_code_hw_table[38] =
{
0x000a,0x0000,0x0001,0x0004,0x000b,0x001a,0x0078,0x00f8,0x3f6,
0x000c,0x001b,0x0079,0x01f6,0x07f6,0x001c,0x00f9,0x03f7,0x0ff4,
0x003a,0x01f7,0x0ff5,0x003b,0x03f8,0x007a,0x07f7,0x007b,0x0ff6,
0x00fa,0x0ff7,0x01f8,0x7fc0,0x01f9,0x01fa,0x03f9,0x03fa,0x07f8,0x07f9,0xfffeU
};



const OMX_U8 huff_ac_croma_val_hw_table[44] =
{
  0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA, 0x11,0x12,0x13,0x14,0x15,0x16,
  0x21,0x22,0x23,0x24,0x25,0x31,0x32,0x33,0x34,0x41,0x42,0x51,0x52,0x61,0x62,0x71,0x72,
  0x81,0x91,0xA1,0xB1,0xC1,0xD1,0xE1,0xF0,0xF1,0xFA
};


const OMX_U16 huff_ac_code_croma_code_hw_table[44] =
{
0x0000,0x0001,0x0004,0x000a,0x0018,0x0019,0x0038,0x0078,0x01f4,0x03f6,0x0ff4,0x000b,0x0039,0x00f6,0x01f5,0x07f6,0x0ff5,
0x001a,0x00f7,0x03f7,0x0ff6,0x7fc2,0x001b,0x00f8,0x03f8,0x0ff7,0x003a,0x01f6,0x003b,0x03f9,0x0079,0x07f7,0x007a,0x07f8,
0x00f9,0x01f7,0x01f8,0x01f9,0x01fa,0x07f9,0x3fe0,0x03fa,0x7fc3,0xfffeU
};


OMX_BOOL jpegdec_arm_nmf_parser_dual::compare_hw_huff_tables(ts_t1xhv_vdc_jpeg_param_in *pJdcIn_values)
{
 // t_bool comparison = TRUE;
  t_uint16 i;

  for(i=0;i<12;i++)
	{
	  if(huff_dc_code_y_hw_table[i]!=pJdcIn_values->huff_table.hw_huff_table.huffman_y_code_dc[i])
		return OMX_FALSE;
	  if(huff_dc_code_croma_hw_table[i]!=pJdcIn_values->huff_table.hw_huff_table.huffman_cb_code_dc[i])
		return OMX_FALSE;
	  if(huff_dc_code_croma_hw_table[i]!=pJdcIn_values->huff_table.hw_huff_table.huffman_cr_code_dc[i])
		return OMX_FALSE;
	}

   for(i=0;i<38;i++)
    {
    if(huff_ac_code_luma_code_hw_table[i]!=pJdcIn_values->huff_table.hw_huff_table.huffman_y_code_ac[(huff_ac_luma_val_hw_table[i])])
	  	return OMX_FALSE;
	}

    for(i=0;i<44;i++)
    {
    if(huff_ac_code_croma_code_hw_table[i]!=pJdcIn_values->huff_table.hw_huff_table.huffman_cb_code_ac[(huff_ac_croma_val_hw_table[i])])
	  	return OMX_FALSE;
	}
  /* TO BE DONE !!!!  ADD COMPARISON FOR AC AS WELL */   // done
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside compare_hw_huff_tables returning TRUE \n");
  return OMX_TRUE;
}



jpegdec_arm_nmf_parser_dual::jpegdec_arm_nmf_parser_dual(){
	PARSER_WAITING_FOR_BUFFERS=0;
	compState = OMX_StateMax;

	for(int i=0;i<4;i++){
		OMX_BUFFERHEADERTYPE tmp ;
        memset(&tmp,0,sizeof(tmp));
		mBufIn[i] = tmp;
		mBufOut[i] = tmp;
	}

	for(int i=0;i<VFM_HEADER_FIFO_COUNT;i++){
		mHeaderDesc[i].pHeader = 0;
		mHeaderDesc[i].isFree = OMX_FALSE;
		mHeaderDesc[i].isRequestedToFree = OMX_FALSE ;
		mHeaderDesc[i].sequenceNumber = 0;
	}

	mParamBufferDesc.nSize = 0;
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

	firstBufferWithBitstream    =OMX_TRUE;

	mLinkListBufferDesc.nPhysicalAddress = 0;
	bitstreamBufferProcessed = OMX_FALSE;

	m_ps_ddep_sdc_jpeg_dynamic_configuration = 0;

	mLineBufferDesc.nLogicalAddress = 0;
	mLineBufferDesc.nMpcAddress  = 0;
	mLineBufferDesc.nPhysicalAddress  = 0;
	//headerBuf = 0;
	mLineBufferDesc.nSize  = 0;
	bufferInput = 0x0;
	MPC_isInputBufferDequeued = OMX_FALSE;
	processActualExecution = OMX_FALSE;

	isSWDecoderReq = OMX_FALSE;
	subSamplingType = 0;
	colourMode = 0;
	bufferSizeReq = 0;

	inputBufferFifo.clear();
	inputHeaderFifo.clear();
	bufferLinkStart = 0;
    imageBufferLinkStart = 0;


	outputDependencyResolved    =OMX_FALSE;
	inputDependencyResolved    =OMX_FALSE;
	headerDependencyResolved	=OMX_FALSE;
    isOutputBufferDequeue = OMX_FALSE;
    isInputBufferDequeue = OMX_FALSE;
    bufferUpdateNeeded			= OMX_TRUE;
	downsamplingFactor = 1;
	windowHeight = 0;
	windowWidth = 0;

    allocatedBufferSize = 0;
	current_frame_width = 0;
	current_frame_height = 0;

	portDef1.bBuffersContiguous = OMX_FALSE;
	portDef1.bEnabled = OMX_FALSE;
	portDef1.bPopulated = OMX_FALSE;
	portDef1.eDir = (OMX_DIRTYPE)0;
	portDef1.eDomain = (OMX_PORTDOMAINTYPE)0;
	portDef1.nBufferAlignment = 0;
	portDef1.nBufferCountActual = 0;
	portDef1.nBufferCountMin = 0;
	portDef1.nBufferSize = 0;
	portDef1.nPortIndex = 0;
	portDef1.nSize = 0;

	portDef2.bBuffersContiguous = OMX_FALSE;
	portDef2.bEnabled = OMX_FALSE;
	portDef2.bPopulated = OMX_FALSE;
	portDef2.eDir = (OMX_DIRTYPE)0;
	portDef2.eDomain = (OMX_PORTDOMAINTYPE)0;
	portDef2.nBufferAlignment = 0;
	portDef2.nBufferCountActual = 0;
	portDef2.nBufferCountMin = 0;
	portDef2.nBufferSize = 0;
	portDef2.nPortIndex = 0;
	portDef2.nSize = 0;

	sdc_jpeg_scan_desc.bitOffset = 0;
	sdc_jpeg_scan_desc.buffer_p = 0;
	sdc_jpeg_scan_desc.encodedDataSize = 0;
	sdc_jpeg_scan_desc.headerBufferMpcAddress = 0;
	sdc_jpeg_scan_desc.physicalAddressParamIn = 0;

	formatType = OMX_COLOR_FormatYUV420Planar; // YUV 420

	isPortSetting = OMX_FALSE;
    isEOSTrue = OMX_FALSE;

	mHuffMemDesc.nLogicalAddress = 0;
	mHuffMemDesc.nMpcAddress = 0;
	mHuffMemDesc.nPhysicalAddress = 0;
	mHuffMemDesc.nSize = 0;

	save_memory_context = 0;

	memset(&sdc_jpeg_param_desc,0,sizeof(sdc_jpeg_param_desc)); //Fixme

	OstTraceInt0(TRACE_API, "PARSER_DUAL: Constructor of parser_dual called \n");

}


jpegdec_arm_nmf_parser_dual::~jpegdec_arm_nmf_parser_dual(){
}


t_nmf_error jpegdec_arm_nmf_parser_dual::construct(void)
{
	OstTraceInt0(TRACE_API, "PARSER_DUAL: Inside construct \n");
	mParser.initializeReferences(this,&mParam);
	return NMF_OK;
}

void jpegdec_arm_nmf_parser_dual::destroy(){

}

void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
	mPorts[0].setTunnelStatus((bool)isTunneled);
}

//void METH(fsmInit)(t_uint16 portsDisabled, t_uint16 portsTunneled)
void METH(fsmInit)(fsmInit_t initFSM)
{
    t_uint16 portsDisabled = initFSM.portsDisabled;
    t_uint16 portsTunneled = initFSM.portsTunneled;

	OMX_BUFFERHEADERTYPE * mBufIn_list[4] ;
	OMX_BUFFERHEADERTYPE * mBufOut_list[4];

    if (initFSM.traceInfoAddr)
    {
        setTraceInfo(initFSM.traceInfoAddr, initFSM.id1);
	}

	for(int i=0;i<4;i++){
		mBufIn_list[i] = &mBufIn[i] ;
		mBufOut_list[i] = &mBufOut[i] ;
	}

	//Input port can keep buffers till complete data is not available
    mPorts[0].init(InputPort,false,true,NULL,mBufIn_list,4,&inputport,0,(portsDisabled&1),(portsTunneled&1),this);
    mPorts[1].init(OutputPort,false,true,NULL,mBufOut_list,4,&outputport,1,(portsDisabled>>1)&1,(portsTunneled>>1)&1,this);

    init(2, mPorts, &proxy, &me, 0); //FIXME:invalid parameter is to be initilaized properly.
    OstTraceInt0(TRACE_API, "PARSER_DUAL: Inside fsminit DONE \n");
}

void METH(disablePortIndication)(t_uint32 portIdx){}
void METH(enablePortIndication)(t_uint32 portIdx){}
void METH(flushPortIndication)(t_uint32 portIdx){}
void METH(reset)(){}

void jpegdec_arm_nmf_parser_dual::processEvent(void){
	Component::processEvent() ;
}

void jpegdec_arm_nmf_parser_dual::fromProcComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nEnter ETB Parser.");
	#endif

	OstTraceFiltInst1(TRACE_API, "PARSER_DUAL: Inside fromProcComp_emptyThisBuffer with buffer : 0x%x \n",(OMX_U32)pBuffer);

	Component::deliverBuffer(0, pBuffer);

	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nExit ETB Parser.");
	#endif

	/*
	if(PARSER_WAITING_FOR_BUFFERS)
	{
		processBuffer(0,0);
		PARSER_WAITING_FOR_BUFFERS=0;
	}*/
}

void jpegdec_arm_nmf_parser_dual::fromProcComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nEnter FTB Parser");
	#endif

	OstTraceFiltInst1(TRACE_API, "PARSER_DUAL: Inside fromProcComp_fillThisBuffer with buffer : 0x%x \n",(OMX_U32)pBuffer);

	Component::deliverBuffer(1, pBuffer);
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nExit FTB Parser");
	#endif

}

void jpegdec_arm_nmf_parser_dual::fromSharedBufferComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	//Return Image buffer to proxy
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nEnter FBD Parser");
	#endif

	OstTraceFiltInst1(TRACE_API, "PARSER_DUAL: Inside fromSharedBufferComp_emptyThisBuffer with buffer : 0x%x \n",(OMX_U32)pBuffer);

#ifdef __CACHE_OPTI
	OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL before calling CacheInvalidate");
	if (save_memory_context)
	{
		OstTraceFiltInst3(TRACE_FLOW, "PARSER_DUAL before calling CacheInvalidate:: context (0x%x)  bBuffer (0x%x)   filledLen (%d)",(OMX_U32)save_memory_context,(OMX_U32)pBuffer->pBuffer, pBuffer->nFilledLen);
		VFM_CacheInvalidate(save_memory_context, pBuffer->pBuffer, pBuffer->nFilledLen);
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL:: VFM_CacheInvalidate DONE ");
	}
	else
	{
		OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL:: VFM_CacheInvalidate FAILED as memory context is EMPTY ");
	}
#endif


	if(isDispatching())
	{
		mPorts[1].returnBuffer(pBuffer);
	}
	else
	{
		//printf("return from Asyn - input \n");
	Component::returnBufferAsync(1,pBuffer);
	}
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nExit FBD Parser");
	#endif

}

void jpegdec_arm_nmf_parser_dual::fromSharedBufferComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	//Return Bitstream buffer to proxy
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nEnter EBD Parser");
	#endif

	OstTraceFiltInst1(TRACE_API, "PARSER_DUAL: Inside fromSharedBufferComp_fillThisBuffer with buffer : 0x%x \n",(OMX_U32)pBuffer);

	if(isDispatching())
	{
		mPorts[0].returnBuffer(pBuffer);
	}
	else
	{
		//printf("return from Asyn - input \n");
	Component::returnBufferAsync(0,pBuffer);
	}

	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nExit EBD Parser");
	#endif

}


void jpegdec_arm_nmf_parser_dual::sendmemoryContext(void *mpc_vfm_mem_ctxt)
{
	save_memory_context = mpc_vfm_mem_ctxt;
	OstTraceInt1(TRACE_FLOW,"PARSER_MPC: Inside sendmemoryContext <> memory context (0x%x)",(OMX_U32)save_memory_context);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword parameter){
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside send command \n");
	switch (cmd){
		case OMX_CommandStateSet:
		{
			switch (parameter){
				case OMX_StatePause:
				case OMX_StateExecuting:
						compState = (OMX_STATETYPE)parameter;
						break;
				case OMX_StateIdle:

							if((!processActualExecution) && (isSWDecoderReq))
							{
								OstTraceFiltInst0(TRACE_FLOW, "Releasing buffer from sendCommand \n");
								mParam.initializeParamAndConfig();
								mParser.parserReset();
								mParam.completeImageProcessed=OMX_FALSE;
								return_InputBuffers();
								inputDependencyResolved		=OMX_FALSE;
								headerDependencyResolved	=OMX_FALSE;
								return_OutputBuffers(0);
								outputDependencyResolved	=OMX_FALSE;
							}

							if (MPC_isInputBufferDequeued && bufferInput)
							{
								if(isDispatching())
								{
									mPorts[0].returnBuffer(bufferInput);
								}
								else
								{
									Component::returnBufferAsync(0,bufferInput);
								}
								MPC_isInputBufferDequeued = OMX_FALSE;
								bufferInput = 0x0;
							}
					compState = (OMX_STATETYPE)parameter;
							break;
			}
		}
		break;

		case OMX_CommandFlush:
		case OMX_CommandPortDisable:
		{
			switch (parameter){
				case 0:
				{
					mParam.initializeParamAndConfig();
					mParser.parserReset();
					mParam.completeImageProcessed=OMX_FALSE;
				}
				break;

				case 1:
					break;
			}
		}
		break;

		case OMX_CommandPortEnable:
			break;
		default:
			//To remove warning in linux
			break;

	}
    Component::sendCommand(cmd, parameter) ;
}


void METH(fillThisHeader)(Buffer_p pHeader){
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside fill header \n");
    freeHeaderDesc((t_uint32)pHeader);
	clearAllPendingRequestsToFreeHeader();
}

void METH(setConfig)(t_uint32 nParamIndex,ts_ddep_sdc_jpeg_dynamic_params config){
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside setConfig \n");
	switch(nParamIndex) {
		case OMX_IndexConfigCommonScale:
			updateParamAndConfigScale(&config);
			mParam.isDownScalingEnabled = 1;
			break;
		case OMX_IndexConfigCommonOutputCrop:
			updateParamAndConfigCrop(&config);
			mParam.isCroppingEnabled = 1;
			break;
		default:
			break;
	}

	updateDynamicConfigurationStructure();
}
void METH(huffMemory)(ts_ddep_buffer_descriptor HuffMemoryDesc)
{
	OstTraceInt0(TRACE_API, "PARSER_DUAL Inside HuffMemory \n");
    mHuffMemDesc = HuffMemoryDesc;
    return;
}
void METH(memoryRequirements)(t_uint16 mChannelId,ts_ddep_buffer_descriptor paramBufferDesc,
							  ts_ddep_buffer_descriptor linkListBufferDesc,ts_ddep_buffer_descriptor debugBufferDesc,
							  ts_ddep_buffer_descriptor lineBufferDesc)
{
	OMX_U8 index;
	//Store Memory Descriptors and initialize pointers
	mParamBufferDesc = paramBufferDesc;
    mLinkListBufferDesc = linkListBufferDesc;
    mDebugBufferDesc = debugBufferDesc;
    mLineBufferDesc = lineBufferDesc;

	OstTraceInt0(TRACE_API, "PARSER_DUAL Inside memoryRequirements \n");

    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++)
    {
        mHeaderDesc[index].isFree = OMX_TRUE;
        mHeaderDesc[index].isRequestedToFree = OMX_FALSE;
        mHeaderDesc[index].pHeader = (ts_ddep_sdc_jpeg_scan_desc*)(paramBufferDesc.nLogicalAddress
                                                             + sizeof(ts_ddep_sdc_jpeg_param_desc)
															 + sizeof(ts_ddep_sdc_jpeg_dynamic_configuration)
                                                             + index* sizeof(ts_ddep_sdc_jpeg_scan_desc));
		mHeaderDesc[index].pHeader->headerBufferMpcAddress = (OMX_U32)(mParamBufferDesc.nMpcAddress
															 + sizeof(ts_ddep_sdc_jpeg_param_desc)/2
															 + sizeof(ts_ddep_sdc_jpeg_dynamic_configuration)/2
															 + index* sizeof(ts_ddep_sdc_jpeg_scan_desc)/2);

		mHeaderDesc[index].pHeader->physicalAddressParamIn = ENDIANESS_CONVERSION((OMX_U32)(mParamBufferDesc.nPhysicalAddress
															 + sizeof(ts_ddep_sdc_jpeg_param_desc)
															 + sizeof(ts_ddep_sdc_jpeg_dynamic_configuration)
															 + index* sizeof(ts_ddep_sdc_jpeg_scan_desc)));
	}

	//Initilaize Dynamic configuration memory pointers
	m_ps_ddep_sdc_jpeg_dynamic_configuration = (ts_ddep_sdc_jpeg_dynamic_configuration*)(paramBufferDesc.nLogicalAddress
											   +sizeof(ts_ddep_sdc_jpeg_param_desc));
	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_entry=0;
	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit=0;

	//Provide memories to MPC
    iDdep.setNeeds(mChannelId,mLinkListBufferDesc,mDebugBufferDesc);
	iDdep.setParameter(mChannelId,ID_SDC_JPEG,mParamBufferDesc);

}

void METH(ComputeARMLoad)(t_bool value)
{
	OstTraceInt0(TRACE_API, "PARSER_DUAL Inside COMPUTE ARM LOAD \n");
	iDdep.disableFWCodeexection((t_bool)value);
}

void jpegdec_arm_nmf_parser_dual::return_InputBuffers()
{
	t_uint16 result = 0;
	OstTraceFiltInst1(TRACE_API, "PARSER_DUAL Inside return_InputBuffers with isInputBufferDequeue : %d \n",isInputBufferDequeue);

	if (isInputBufferDequeue)
	{
		bufferUpdateNeeded = OMX_TRUE;
		//Return Input Buffers
        //FIX ME: check for last buffer contents whether completely consumed or not
		if((bufferLinkStart->bufferLink.addr_next_buf_link ==0) && (bufferLinkStart->bufferLink.addr_next_buf_link == 0))
		{
			if(bufferLinkStart->pBuffer->nFlags & BUFFERFLAG_EOS)
			{
				OstTraceFiltInst0(TRACE_FLOW, "Inside return_InputBuffers and EOS is true \n");
				isEOSTrue = OMX_TRUE;
			}
			if(isDispatching())
			{
				mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
			}
			else
			{
				//printf("return from Asyn - input \n");
				Component::returnBufferAsync(0,bufferLinkStart->pBuffer);
			}

			OstTraceFiltInst1(TRACE_FLOW, "Input buffer returned : 0x%x\n",(OMX_U32)bufferLinkStart->pBuffer);

			isInputBufferDequeue = OMX_FALSE;
			//mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
			#ifdef DBG_DUALPRINT
				printf("Returning Input buffer \n");
			#endif
			result = removeBufferFromBtsLinkList(bufferLinkStart);
			if(result)
			{
				//FIX ME !
				//while(1);
				OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
			}
		}
		else
		{
			while(bufferLinkStart->bufferLink.addr_next_buf_link)
			{
				//check with firmware output fixme
				if(isDispatching())
				{
					mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
				}
				else
				{
					//printf("return from Asyn - input \n");
					Component::returnBufferAsync(0,bufferLinkStart->pBuffer);
				}
				OstTraceFiltInst1(TRACE_FLOW, "Input buffer returned : 0x%x\n",(OMX_U32)bufferLinkStart->pBuffer);

				isInputBufferDequeue = OMX_FALSE;
				//	mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
				#ifdef DBG_DUALPRINT
					printf("Returning Input buffer \n");
				#endif
				if(bufferLinkStart->pBuffer->nFlags & BUFFERFLAG_EOS)
				{
					OstTraceFiltInst0(TRACE_FLOW, "Inside return_InputBuffers and EOS is true \n");
					isEOSTrue = OMX_TRUE;
				}
				//bufferLinkStart = (ts_bitstream_buf_link_and_header *)bufferLinkStart->bufferLink.addr_next_buf_link;
				result = removeBufferFromBtsLinkList(bufferLinkStart);
				if(result)
				{
					//FIX ME !
					//while(1);
					OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
				}
			}
			//return the last buffer
			if(isDispatching())
			{
				mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
			}
			else
			{
				//printf("return from Asyn - input \n");
				Component::returnBufferAsync(0,bufferLinkStart->pBuffer);
			}
			isInputBufferDequeue = OMX_FALSE;
	 		//  mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
			#ifdef DBG_DUALPRINT
			    printf("Returning Input buffer \n");
			#endif

			OstTraceFiltInst1(TRACE_FLOW, "Inside Returning Input buffer : 0x%x \n",(OMX_U32)bufferLinkStart->pBuffer);

			if(bufferLinkStart->pBuffer->nFlags & BUFFERFLAG_EOS)
			{
				OstTraceFiltInst0(TRACE_FLOW, "Inside return_InputBuffers and EOS is true \n");
				isEOSTrue = OMX_TRUE;
			}
			result = removeBufferFromBtsLinkList(bufferLinkStart);
			if(result)
			{
				//FIX ME !
				//while(1);
				OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
			}
		}
	}
}


void jpegdec_arm_nmf_parser_dual::return_OutputBuffers(OMX_U32 output_buffSize)
{
	t_uint16 result = 0;

	OstTraceFiltInst1(TRACE_API, "PARSER_DUAL Inside return_OutputBuffers value of isOutputBufferDequeue : %d \n",isOutputBufferDequeue);

	if (isOutputBufferDequeue)
	{
		bufferUpdateNeeded = OMX_TRUE;
		//Return output Buffers
		if ((imageBufferLinkStart->next == 0) && (imageBufferLinkStart->prev ==0) )
		{
			if(isEOSTrue)
			{
				OstTraceFiltInst0(TRACE_FLOW, "Inside return_OutputBuffers EOS is being set \n");
				imageBufferLinkStart->pBuffer->nFlags |= BUFFERFLAG_EOS;
			}
			// FIX ME : correct nFilled length
			imageBufferLinkStart->pBuffer->nFilledLen = output_buffSize;
			if(isDispatching())
			{
				mPorts[1].returnBuffer(imageBufferLinkStart->pBuffer);
			}
			else
			{
				//printf("return from Asyn - input \n");
				Component::returnBufferAsync(1,imageBufferLinkStart->pBuffer);
			}

			OstTraceFiltInst1(TRACE_FLOW, "Inside return_OutputBuffers returning buffer : 0x%x \n",(OMX_U32)imageBufferLinkStart->pBuffer);

			//mPorts[1].returnBuffer(imageBufferLinkStart->pBuffer);
			isOutputBufferDequeue = OMX_FALSE;
			#ifdef DBG_DUALPRINT
				printf("Returning Ouput buffer \n");
			#endif
			result =  removeBufferfromImageLinkList(imageBufferLinkStart);
			if(result)
			{
				//FIX ME !
				//while(1);
				OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
			}
		}
		else
		{
			while(imageBufferLinkStart->next)
            {
			    if(isEOSTrue)
			   	{
			   		imageBufferLinkStart->pBuffer->nFlags |= BUFFERFLAG_EOS;
			   	}

				imageBufferLinkStart->pBuffer->nFilledLen = output_buffSize;
				if(isDispatching())
				{
					mPorts[1].returnBuffer(imageBufferLinkStart->pBuffer);
				}
				else
				{
					//printf("return from Asyn - input \n");
					Component::returnBufferAsync(1,imageBufferLinkStart->pBuffer);
				}

				OstTraceFiltInst1(TRACE_FLOW, "Inside return_OutputBuffers returning buffer : 0x%x \n",(OMX_U32)imageBufferLinkStart->pBuffer);

				//mPorts[1].returnBuffer(imageBufferLinkStart->pBuffer);
				isOutputBufferDequeue = OMX_FALSE;
				#ifdef DBG_DUALPRINT
					printf("Returning Ouput buffer \n");
				#endif
				//imageBufferLinkStart->next;
				result =  removeBufferfromImageLinkList(imageBufferLinkStart);
				if(result)
				{
					//FIX ME !
					//while(1);
					OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
				}
				// remove from the list
			}
		}
	}
}

void jpegdec_arm_nmf_parser_dual::endAlgo(t_t1xhv_status status, t_t1xhv_decoder_info info, t_uint32 durationInTicks)
{
	//NMF_LOG("Inside endAlgo API \n");

	t_uint16 result = 0;

	OstTraceFiltInst1(TRACE_API, "Inside endAlgo with status : 0x%x \n",status);

//	OMX_U32 filledlength;
	switch(status)
	{
		case STATUS_JOB_COMPLETE:
		                    //NMF_LOG("Job completed in Algo \n");
							return_InputBuffers();
							return_OutputBuffers(bufferSizeReq);
							OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: Inside endalgo returned the input and output buffers \n");
					        //check for EOS & Generate event if required
							if(isEOSTrue)
							{
								OstTraceFiltInst0(TRACE_FLOW, " PARSER_DUAL Inside endAlgo with isEOSTrue SET \n");
								proxy.eventHandler(OMX_EventBufferFlag, 1, BUFFERFLAG_EOS);
								isEOSTrue = OMX_FALSE;
							}

							//reset frame_width & frame_height for image mode
							//for payload management do not reset frame width or height
							// will be set by set Paramter or update sttings in case of port sttings
							//current_frame_width = 0;
							//current_frame_height = 0;

							inputDependencyResolved		=OMX_FALSE;
							outputDependencyResolved	=OMX_FALSE;
							headerDependencyResolved	=OMX_FALSE;

							//areFlagsAndTimestampSaved   = OMX_FALSE;
					        ResolveDependencies();
							break;

	   case STATUS_BUFFER_NEEDED:

							   //firstBufferWithBitstream  =OMX_TRUE;
								#ifdef DBG_DUALPRINT
									   printf("\n >>>>>>>>>>STATUS BUFFER NEEDED<<<<<<<<<<<");
								#endif
								OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: STATUS BUFFER NEEDED\n");
							   //return used up buffers
					            while(bufferLinkStart->bufferLink.addr_next_buf_link)
								{
									//check with firmware output fixme
									//printf("returning buffer bufferLinkStart->pBuffer: 0x%X",bufferLinkStart->pBuffer);
									mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
									#ifdef DBG_DUALPRINT
									printf("Returning Input buffer \n");
									#endif
									//bufferLinkStart = (ts_bitstream_buf_link_and_header *)bufferLinkStart->bufferLink.addr_next_buf_link;
									result = removeBufferFromBtsLinkList(bufferLinkStart);
									if(result)
									{
										//FIX ME !
										//while(1);
										OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
									}
								}
								//return the last buffer in the list
								#ifdef DBG_DUALPRINT
									printf("returning buffer bufferLinkStart->pBuffer: 0x%X",bufferLinkStart->pBuffer);
								#endif
		    					mPorts[0].returnBuffer(bufferLinkStart->pBuffer);
								#ifdef DBG_DUALPRINT
								    printf("Returning Input buffer \n");
								#endif
								result = removeBufferFromBtsLinkList(bufferLinkStart);
								if(result)
								{
									//FIX ME !
									//while(1);
									OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
								}

							   bufferUpdateNeeded = OMX_TRUE;
							   //> FIX ME
							   //ResolveDependencies();
							   //return the bitstream / Image buffer buffer FIX ME
							   break;
			default:
							    OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: Inside endalgo error in ALGO \n");
								break;
	}
	//fixme
}


void METH(process)()
{
	//NMF_LOG("Inside Process function \n");
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL Inside process \n");
	processBitstreamBuffer();
	//NMF_LOG("Back to Process value of MPC_isInputBufferDequeued : 0x%x\n",MPC_isInputBufferDequeued);
	if (MPC_isInputBufferDequeued)
	{
		//NMF_LOG("MPC to SW switch required : 0x%x\n",isSWDecoderReq);
		if (isSWDecoderReq)
		{
			ResolveDependencies();
		}
		else
		{
			if(mPorts[1].queuedBufferCount())
			{
				OMX_BUFFERHEADERTYPE *bufOut;
				bufOut = mPorts[1].dequeueBuffer();
				//NMF_LOG("Output buffer dequeue from Process 0x%x \n",bufOut);
				OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: sending output buffer to ddep \n");
				fillthisbufferSharedBufOut.fillThisBuffer(bufOut);
			}
		}
	}
	if ((bitstreamBufferProcessed) && (!isSWDecoderReq))
	{
		if(mPorts[1].queuedBufferCount())
		{
			OMX_BUFFERHEADERTYPE *bufOut;
			bufOut = mPorts[1].dequeueBuffer();
			OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: sending output buffer to ddep \n");
			fillthisbufferSharedBufOut.fillThisBuffer(bufOut);
		}
	}
	if ((bitstreamBufferProcessed) && (isSWDecoderReq))
	{
		ResolveDependencies();
	}
}


t_uint16 jpegdec_arm_nmf_parser_dual::addBufferToBtsLinkList(OMX_BUFFERHEADERTYPE *pBuffer,ts_bitstream_buf_link_and_header *currentBufferLink,t_ahb_address *return_addr)
{
#ifdef BTSLNKLST_DBG_PRNT
printf("\n addBufferToBtsLinkList\n");
#endif
			OstTraceFiltInst1(TRACE_API, "PARSER_DUAL: Inside addBufferToBtsLinkList for buffer : 0x%x \n",(OMX_U32)pBuffer);
			     if(bufferLinkStart == 0)
				 {
					ts_bitstream_buf_link_and_header *tmp = new ts_bitstream_buf_link_and_header;
					if(tmp == 0) return 1;

					bufferLinkStart = tmp;
					//bufferLinkStart->bufferLink = (tmp->bufferLink);
					tmp->bufferLink.addr_next_buf_link = 0;
					tmp->bufferLink.addr_prev_buf_link = 0;
					tmp->bufferLink.addr_buffer_start = (t_ahb_address)pBuffer->pBuffer + (t_ahb_address)pBuffer->nOffset;

					// <CAUTION> Important : ALGO requires extra WORD at end for processing so intentionally 64 bytes have been added
					tmp->bufferLink.addr_buffer_end = (t_ahb_address)pBuffer->pBuffer + (t_ahb_address)pBuffer->nOffset + (t_ahb_address)pBuffer->nFilledLen + 64;
					tmp->pBuffer = pBuffer;
                    *currentBufferLink = *tmp;
					*return_addr = (t_ahb_address)&tmp->bufferLink;
#ifdef BTSLNKLST_DBG_PRNT
printf("\n &tmp->bufferLink:0x%X \n tmp->bufferLink.addr_next_buf_link: 0x%X \n tmp->bufferLink.addr_prev_buf_link: 0x%X \n tmp->bufferLink.addr_buffer_start: 0x%X \n tmp->bufferLink.addr_buffer_end: 0x%X",
															&tmp->bufferLink,
															tmp->bufferLink.addr_next_buf_link,
															tmp->bufferLink.addr_prev_buf_link,
															tmp->bufferLink.addr_buffer_start,
															tmp->bufferLink.addr_buffer_end);
#endif

				}
				else
				{
					ts_bitstream_buf_link_and_header *lastElement = bufferLinkStart;
					ts_bitstream_buf_link_and_header *tmp = new ts_bitstream_buf_link_and_header;
#ifdef BTSLNKLST_DBG_PRNT
printf("\nAdded &tmp->bufferLink:0x%X in to the buffer struct\n",&tmp->bufferLink);
ts_bitstream_buf_link_and_header *tmp2;
#endif

					if(tmp == 0) return 1;

					//Add to the end of list
					while(lastElement->bufferLink.addr_next_buf_link)
					{
                    lastElement = (ts_bitstream_buf_link_and_header *)lastElement->bufferLink.addr_next_buf_link;
                    }

					lastElement->bufferLink.addr_next_buf_link = (t_ahb_address) &(tmp->bufferLink);
					tmp->bufferLink.addr_next_buf_link = (t_ahb_address)0;
					tmp->bufferLink.addr_prev_buf_link = (t_ahb_address)&(lastElement->bufferLink) ;

					tmp->bufferLink.addr_buffer_start = (t_ahb_address)pBuffer->pBuffer + (t_ahb_address)pBuffer->nOffset;
					tmp->bufferLink.addr_buffer_end = (t_ahb_address)pBuffer->pBuffer + (t_ahb_address)pBuffer->nOffset + (t_ahb_address)pBuffer->nFilledLen;
					tmp->pBuffer = pBuffer;
					*currentBufferLink = *tmp;
					*return_addr = (t_ahb_address)&tmp->bufferLink;
#ifdef BTSLNKLST_DBG_PRNT
//> debug prints
lastElement = bufferLinkStart;
                    //> print total (links - 1)
while(lastElement->bufferLink.addr_next_buf_link)
{
	tmp2=lastElement;
	printf("\n &tmp2->bufferLink:0x%X \n tmp2->bufferLink.addr_next_buf_link: 0x%X \n tmp2->bufferLink.addr_prev_buf_link: 0x%X \n tmp2->bufferLink.addr_buffer_start: 0x%X \n tmp2->bufferLink.addr_buffer_end: 0x%X",
																&tmp2->bufferLink,
																tmp2->bufferLink.addr_next_buf_link,
																tmp2->bufferLink.addr_prev_buf_link,
																tmp2->bufferLink.addr_buffer_start,
																tmp2->bufferLink.addr_buffer_end);

	lastElement = (ts_bitstream_buf_link_and_header *)lastElement->bufferLink.addr_next_buf_link;
}
					//print last element
	tmp2=lastElement;
	printf("\n &tmp2->bufferLink:0x%X \n tmp2->bufferLink.addr_next_buf_link: 0x%X \n tmp2->bufferLink.addr_prev_buf_link: 0x%X \n tmp2->bufferLink.addr_buffer_start: 0x%X \n tmp2->bufferLink.addr_buffer_end: 0x%X",
																&tmp2->bufferLink,
																tmp2->bufferLink.addr_next_buf_link,
																tmp2->bufferLink.addr_prev_buf_link,
																tmp2->bufferLink.addr_buffer_start,
																tmp2->bufferLink.addr_buffer_end);
#endif
				}

	return 0;
}

t_uint16 jpegdec_arm_nmf_parser_dual::removeBufferFromBtsLinkList(ts_bitstream_buf_link_and_header *BufferLinkToRemove)
{
#ifdef BTSLNKLST_DBG_PRNT
printf("\nremoveBufferFromBtsLinkList bufferLinkStart->pBuffer: 0x%X \n",bufferLinkStart->pBuffer);
ts_bitstream_buf_link_and_header *tmp2;
tmp2=bufferLinkStart;
while(tmp2->bufferLink.addr_next_buf_link)
	{
		printf("\n &tmp2->bufferLink:0x%X \n tmp2->bufferLink.addr_next_buf_link: 0x%X \n tmp2->bufferLink.addr_prev_buf_link: 0x%X \n tmp2->bufferLink.addr_buffer_start: 0x%X \n tmp2->bufferLink.addr_buffer_end: 0x%X",
																&tmp2->bufferLink,
																tmp2->bufferLink.addr_next_buf_link,
																tmp2->bufferLink.addr_prev_buf_link,
																tmp2->bufferLink.addr_buffer_start,
																tmp2->bufferLink.addr_buffer_end);

		tmp2 = (ts_bitstream_buf_link_and_header *)tmp2->bufferLink.addr_next_buf_link;
     }
	//print last element
    printf("\n &tmp2->bufferLink:0x%X \n tmp2->bufferLink.addr_next_buf_link: 0x%X \n tmp2->bufferLink.addr_prev_buf_link: 0x%X \n tmp2->bufferLink.addr_buffer_start: 0x%X \n tmp2->bufferLink.addr_buffer_end: 0x%X",
																&tmp2->bufferLink,
																tmp2->bufferLink.addr_next_buf_link,
																tmp2->bufferLink.addr_prev_buf_link,
																tmp2->bufferLink.addr_buffer_start,
																tmp2->bufferLink.addr_buffer_end);
#endif
	//remove the first element of list
	ts_bitstream_buf_link_and_header *element = bufferLinkStart;
	OstTraceFiltInst1(TRACE_FLOW, "PARSER_DUAL: Inside removeBufferToBtsLinkList for buffer : 0x%x \n",(OMX_U32)BufferLinkToRemove);
	if(bufferLinkStart!=BufferLinkToRemove)
		return 1;

	if(bufferLinkStart->bufferLink.addr_next_buf_link)
	{
		bufferLinkStart = (ts_bitstream_buf_link_and_header*)bufferLinkStart->bufferLink.addr_next_buf_link;
		bufferLinkStart->bufferLink.addr_prev_buf_link = 0;
	}
	else
	{
       //bufferLinkStart = (ts_bitstream_buf_link_and_header*);
		bufferLinkStart->bufferLink.addr_prev_buf_link = 0;
		bufferLinkStart->bufferLink.addr_next_buf_link = 0;
		bufferLinkStart = 0;
	}

	if(bufferLinkStart != 0)
    delete element;

	return 0;
}

t_uint16 jpegdec_arm_nmf_parser_dual::readLastBtsBufferfromLinkList(ts_bitstream_buf_link_and_header *currentBufferLink)
{
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside readLastBtsBufferfromLinkList\n");
	if(bufferLinkStart == 0)
		return 1;
	else
	{
		ts_bitstream_buf_link_and_header *tmp2;
		tmp2=bufferLinkStart;
		while(tmp2->bufferLink.addr_next_buf_link)
		{
			tmp2 = (ts_bitstream_buf_link_and_header *)tmp2->bufferLink.addr_next_buf_link;
		}


		*currentBufferLink = *tmp2;
	}
	return 0;
}

t_uint16 jpegdec_arm_nmf_parser_dual::addBufferToImageLinkList(OMX_BUFFERHEADERTYPE *pBuffer,ts_image_buf_link_and_header *currentBufferLink)
{
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside addBufferToImageLinkList\n");
	if(imageBufferLinkStart==0)
	{
		ts_image_buf_link_and_header *tmp = new ts_image_buf_link_and_header;
		imageBufferLinkStart = tmp;
		//maintian list
		tmp->next = 0;
		tmp->prev = 0;
		tmp->bufferLink.addr_dest_buffer = (t_ahb_address)pBuffer->pBuffer;
		*currentBufferLink = *tmp;
		tmp->pBuffer = pBuffer;
	}
	else
	{
		ts_image_buf_link_and_header *tmp = new ts_image_buf_link_and_header;
		ts_image_buf_link_and_header *LastElement = imageBufferLinkStart;
		//go to last element & add new element
		while(LastElement->next != 0 )
			LastElement = LastElement->next;

		LastElement->next = tmp;
		tmp->prev = LastElement;
		tmp->next = 0;
		tmp->bufferLink.addr_dest_buffer = (t_ahb_address)pBuffer->pBuffer;
		*currentBufferLink = *tmp;
		tmp->pBuffer = pBuffer;
	}

	return 0;
}

t_uint16 jpegdec_arm_nmf_parser_dual::removeBufferfromImageLinkList(ts_image_buf_link_and_header *BufferLinkToRemove)
{
	ts_image_buf_link_and_header *element = imageBufferLinkStart;
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside removeBufferToImageLinkList\n");
	if(BufferLinkToRemove !=imageBufferLinkStart)
		return 1;

	if(imageBufferLinkStart->next)
	{
		imageBufferLinkStart = imageBufferLinkStart->next;
		imageBufferLinkStart->prev = 0;
	}
	else
	{
		imageBufferLinkStart = 0;
	}

	delete element;
	return 0;
}

t_uint16 jpegdec_arm_nmf_parser_dual::readFirstImageBufferfromLinkList(ts_image_buf_link_and_header *currentBufferLink)
{
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside readFirstImageBufferfromLinkLis\n");
	if(imageBufferLinkStart == 0)
		return 1;
	else
	{
		*currentBufferLink = *imageBufferLinkStart;
	}
	return 0;
}



void jpegdec_arm_nmf_parser_dual::ResolveDependencies()
{
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside ResolveDependencies \n");
	isEOSTrue = OMX_FALSE;
	if(!outputDependencyResolved)
	{
        OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: ResolveDependencies !outputDependencyResolved\n");
		if(mPorts[1].queuedBufferCount())
		{
			outputDependencyResolved = OMX_TRUE;
            OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: ResolveDependencies outputDependencyResolved = true\n");
		}

	}

	if(!headerDependencyResolved)
	{
		//check for header buffer
        OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: ResolveDependencies !headerDependencyResolved\n");
		if(inputHeaderFifo.getSize())
		{
			headerDependencyResolved = OMX_TRUE;
            OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: ResolveDependencies headerDependencyResolved = true\n");
		}
	}//end of headerdepndency check

	if(inputBufferFifo.getSize() && headerDependencyResolved)
	{
		inputDependencyResolved = OMX_TRUE;
        OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: ResolveDependencies inputDependencyResolved = true\n");
		//taking out input buffer
		{
				OMX_BUFFERHEADERTYPE *pBuffer = (OMX_BUFFERHEADERTYPE *)inputBufferFifo.popFront();
				//Update link list with this buffer fixme
				while(pBuffer)
				{
                   //CHECK For EOS
					if(pBuffer->nFlags & BUFFERFLAG_EOS)
					{
						isEOSTrue = OMX_TRUE;
                        OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: ResolveDependencies EOS = true\n");
					}
					// check if filled length non zero
					if(pBuffer->nFilledLen == 0)
					{
						#ifdef BTSLNKLST_DBG_PRNT
						printf("\n Filled length is zero for pBuffer: 0x%X",pBuffer);
						#endif
						if(isEOSTrue)
						{
                        	#ifdef BTSLNKLST_DBG_PRNT
								printf("\n Filled length is zero & EOS FLAG set : 0x%X",pBuffer);
                        	#endif
							//> FIX ME: return all input & output buffers
////							endAlgo(STATUS_JOB_COMPLETE,VDC_ERT_END_OF_BITSREAM_REACHED,0);
						}
						else
						{
							mPorts[0].returnBuffer(pBuffer);
                            OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: ResolveDependencies returning inputbuffer\n");
							isInputBufferDequeue = OMX_FALSE;
							#ifdef DBG_DUALPRINT
							printf("Returning Input buffer \n");
							#endif
						}
					}
////////////////////////else
					{
						#ifdef BTSLNKLST_DBG_PRNT
						printf("\n ++++++++adding to bitstream list buffer 0x%X",pBuffer);
						#endif
                        OstTraceFiltInst1(TRACE_FLOW, "PARSER_DUAL:  ++++++++adding to bitstream list buffer 0x%X",(OMX_U32)pBuffer);
						ts_bitstream_buf_link_and_header currentBufferLink;
						t_ahb_address buf_struct_addr;
						t_uint16 result = addBufferToBtsLinkList(pBuffer,&currentBufferLink,&buf_struct_addr);
						if(result)
						{
								DBC_ASSERT(0);
								//while(1);
								OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
						}

						if(bufferUpdateNeeded == OMX_TRUE)
						{
							#ifdef BTSLNKLST_DBG_PRNT
							printf("\n||||||||||Programmed Buf struct = 0x%X\n",buf_struct_addr);
							#endif
							sdc_jpeg_param_desc.s_in_out_bitstream_buffer.addr_bitstream_buf_struct = (t_ahb_address)buf_struct_addr;//&currentBufferLink->bufferLink ;
							sdc_jpeg_param_desc.s_in_out_bitstream_buffer.addr_bitstream_start = (t_ahb_address)(currentBufferLink.pBuffer->pBuffer) + (t_ahb_address)(currentBufferLink.pBuffer->nOffset);
							bufferUpdateNeeded = OMX_FALSE;
                            OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL:bufferUpdateNeeded == OMX_TRUE");
						}
                   }//end of nFilled length check's else
				   pBuffer = (OMX_BUFFERHEADERTYPE *)inputBufferFifo.popFront();
				}//end of while
		}

    	//generate EventPortSettings changed if their is a change in width or height
		if (current_frame_width != sdc_jpeg_param_desc.s_in_parameters.frame_width || current_frame_height != sdc_jpeg_param_desc.s_in_parameters.frame_height)
    	{
			#ifdef DBG_DUALPRINT
				printf("\n DO th needful for Event Port0 Setting Changed for Width and Hieght\n");
			#endif
                OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL DO th needful for Event Port0 Setting Changed for Width and Hieght\n");
			portDef1.format.image.nFrameHeight = sdc_jpeg_param_desc.s_in_parameters.frame_height;
			portDef1.format.image.nFrameWidth = sdc_jpeg_param_desc.s_in_parameters.frame_width;
			//portDef1.format.image.eColorFormat = formatType;
			//changePortSettingsSignal.portSettings((void*)&portDef1);
			iChangePortSettingsSignal.portSettings((void*)&portDef1);
			isPortSetting = OMX_TRUE;
		}

		if (portDef2.format.image.eColorFormat != formatType)
    	{
			#ifdef DBG_DUALPRINT
				printf("\n DO th needful for Event Port Setting Changed at Port1 for FORMAT\n");
			#endif
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL DO th needful for Event Port1 Setting Changed for Width and Hieght\n");
			portDef2.format.image.nFrameHeight = sdc_jpeg_param_desc.s_in_parameters.frame_height;
			portDef2.format.image.nFrameWidth = sdc_jpeg_param_desc.s_in_parameters.frame_width;
			portDef2.nBufferSize = bufferSizeReq;
			portDef2.format.image.eColorFormat = formatType;
			iChangePortSettingsSignal.portSettings((void*)&portDef2);
	     }


	}

	if(inputDependencyResolved && outputDependencyResolved && headerDependencyResolved)
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: Inside ResolveDependencies all dependencies resolved\n");
		//for taking out output buffer

		    OMX_BUFFERHEADERTYPE *pBuffer;
			pBuffer = mPorts[1].dequeueBuffer();

			OstTraceFiltInst1(TRACE_FLOW, "PARSER_DUAL: Dequeuing Output buffer : 0x%x\n",(OMX_U32)pBuffer);

#ifdef DBG_DUALPRINT
			printf("Taking Output buffer \n");
#endif
			OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL Taking Output buffer \n");
			ts_image_buf_link_and_header currentBufferLink;
			t_uint16 result = addBufferToImageLinkList(pBuffer,&currentBufferLink);
			if(result)
			{
				 DBC_ASSERT(0);
				 //while(1);
				 OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
			}

            //program sdc_jpeg_param_desc
            sdc_jpeg_param_desc.s_out_frame_buffer.addr_dest_buffer = (t_ahb_address)currentBufferLink.bufferLink.addr_dest_buffer ;
			isOutputBufferDequeue = OMX_TRUE;
			allocatedBufferSize = pBuffer->nAllocLen;
		// for taking the header buffer

			//ts_sdc_jpeg_scan_desc *headerBuf = (ts_sdc_jpeg_scan_desc *)inputHeaderFifo.popFront();
			//PG modified for Dual
			ts_ddep_sdc_jpeg_scan_desc *headerBuf = (ts_ddep_sdc_jpeg_scan_desc *)inputHeaderFifo.popFront();
			OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL Taking out headerBuffer\n");
			//> TO DO: copy header data into Param structures or have two copy of param to optimize
            //headerDependencyResolved = OMX_TRUE;
            //copy buffer offset
            t_uint32 buffer_p_copy = 0;
            if (headerBuf)
            {
			buffer_p_copy = headerBuf->buffer_p;
			sdc_jpeg_param_desc.s_in_out_bitstream_buffer.bitstream_offset = headerBuf->bitOffset;

				freeHeaderDesc((t_uint32)(headerBuf->headerBufferMpcAddress));
				clearAllPendingRequestsToFreeHeader();
			}
      if(!buffer_p_copy)
	        {
                OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL buffer_p set to 0 in provideBitstreamBuffer for fakebuffers used to handle the case of filledlenth =0 \n");
                            return_InputBuffers();  /////freeing all the buffers
							return_OutputBuffers(bufferSizeReq);

					        //check for EOS & Generate event if required
							if(isEOSTrue)
							{
								proxy.eventHandler(OMX_EventBufferFlag, 1, BUFFERFLAG_EOS);
								isEOSTrue = OMX_FALSE;
							}

							//reset frame_width & frame_height for image mode
							//for payload management do not reset frame width or height
							// will be set by set Paramter or update sttings in case of port sttings
							//current_frame_width = 0;
							//current_frame_height = 0;

							inputDependencyResolved		=OMX_FALSE;
							outputDependencyResolved	=OMX_FALSE;
							headerDependencyResolved	=OMX_FALSE;
	        }
	   else
		{


			//store frame width & frame height to calculate nFilledLength & further checking for port settings
            current_frame_width = sdc_jpeg_param_desc.s_in_parameters.frame_width;
		    current_frame_height = sdc_jpeg_param_desc.s_in_parameters.frame_height;
	//end of header buffer FIFO size check

		isPortSetting = OMX_FALSE;


		if (allocatedBufferSize < bufferSizeReq || portDef2.format.image.eColorFormat != formatType)
    	{
			#ifdef DBG_DUALPRINT
				printf("\n DO th needful for Event Port Setting Changed at Port1\n");
			#endif
			OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL: DO th needful for Event Port Setting Changed at Port1 \n");
			portDef2.format.image.nFrameHeight = sdc_jpeg_param_desc.s_in_parameters.frame_height;
			portDef2.format.image.nFrameWidth = sdc_jpeg_param_desc.s_in_parameters.frame_width;
			portDef2.nBufferSize = bufferSizeReq;
			portDef2.format.image.eColorFormat = formatType;
			//changePortSettingsSignal.portSettings((void*)&portDef2);
			iChangePortSettingsSignal.portSettings((void*)&portDef2);
		}

		if (allocatedBufferSize < bufferSizeReq)
		{
			OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL: Setting Portsetting for bufferSize \n");
			//return the outputbuffer is allocated
			isPortSetting = OMX_TRUE;
		}

		if (isPortSetting)
		{
			isPortSetting = OMX_FALSE;
			if (isOutputBufferDequeue)
			{
				return_OutputBuffers(0);
				outputDependencyResolved    =OMX_FALSE;
			}
			return;
		}

		processActualExecution = OMX_TRUE;
		//Read from Image buffer List & Bts Buffer list and copy Buffer Flags
		{
			ts_bitstream_buf_link_and_header btsBuffer;
			ts_image_buf_link_and_header imageBuffer;

			memset(&btsBuffer,0x0,sizeof(ts_bitstream_buf_link_and_header)); //to fix coverity

			t_uint16 result =  readLastBtsBufferfromLinkList(&btsBuffer);
			if(result)
			{
				DBC_ASSERT(0);
				//while(1);
				OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
			}

			result = readFirstImageBufferfromLinkList(&imageBuffer);
			if(result)
			{
				DBC_ASSERT(0);
				//while(1);
				OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
			}
#ifdef DBG_DUALPRINT
			printf("\nbtsBuffer.pBuffer: 0x%X",btsBuffer.pBuffer);
#endif
			imageBuffer.pBuffer->nFlags = btsBuffer.pBuffer->nFlags;
			imageBuffer.pBuffer->nTimeStamp = btsBuffer.pBuffer->nTimeStamp;
		}

#ifdef DBG_DUALPRINT
		printf("Value of downsamplingFactor %d and colormode : %d \n",downsamplingFactor,colourMode);
#endif
		//configureAlgo
#ifdef DBG_DUALPRINT
        printf("All dependencies resolved!! Lets enjoy!! \n");
#endif
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: All dependencies resolved!! Calling ConfigureAlgo!! \n");
		if(firstBufferWithBitstream == OMX_TRUE)
		{
			OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: firstBufferWithBitstream is SET \n");
        iAlgo.configureAlgo((t_uint32)  &sdc_jpeg_param_desc.s_in_frame_buffer,
							(t_uint32)  &sdc_jpeg_param_desc.s_out_frame_buffer,
							(t_uint32)  &sdc_jpeg_param_desc.s_internal_buffer,
							(t_uint32)  &sdc_jpeg_param_desc.s_in_out_bitstream_buffer,
							(t_uint32)  &sdc_jpeg_param_desc.s_out_bitstream_buffer,
							(t_uint32)  &sdc_jpeg_param_desc.s_in_parameters,
							(t_uint32)  &sdc_jpeg_param_desc.s_out_parameters,
							(t_uint32)  &sdc_jpeg_param_desc.s_in_out_frame_parameters,
							(t_uint32)  &sdc_jpeg_param_desc.s_out_frame_parameters);

        //controlAlgo
#ifdef DBG_DUALPRINT
		printf("\n control algo: CMD_START\n");
#endif
		firstBufferWithBitstream = OMX_FALSE;
		iAlgo.controlAlgo(CMD_START,0);
		processActualExecution = OMX_FALSE;
        }
		else
		{
           //update buffer list if buffer is needed !
		   //falgs update
			ts_bitstream_buf_link_and_header btsBuffer;
			ts_image_buf_link_and_header imageBuffer;

			memset(&btsBuffer,0x0,sizeof(ts_bitstream_buf_link_and_header)); //to fix coverity

			t_uint16 result =  readLastBtsBufferfromLinkList(&btsBuffer);
			if(result)
			{
				DBC_ASSERT(0);
				//while(1);
				OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
			}

			result = readFirstImageBufferfromLinkList(&imageBuffer);
			if(result)
			{
				DBC_ASSERT(0);
				//while(1);
				OstTraceFiltInst1(TRACE_ERROR, "PARSER_DUAL: Incorrect result at line no : %d ",__LINE__);
			}
#ifdef DBG_DUALPRINT
			printf("\nbtsBuffer.pBuffer: 0x%X",btsBuffer.pBuffer);
#endif
			imageBuffer.pBuffer->nFlags = btsBuffer.pBuffer->nFlags;
			imageBuffer.pBuffer->nTimeStamp = btsBuffer.pBuffer->nTimeStamp;

			//call update.Algo
#ifdef DBG_DUALPRINT
			printf("\n control algo: CMD_UPDATE_BUFFER\n");
#endif
			OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL  update Algo\n");
			iAlgo.updateAlgo(CMD_UPDATE_BUFFER,0,0);
			processActualExecution = OMX_FALSE;
		}
	}
  }
}

void jpegdec_arm_nmf_parser_dual:: processBitstreamBuffer()
{
	t_djpegErrorCode btstState;
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL Inside processBitstreamBuffer \n");
	if(mPorts[0].queuedBufferCount())
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: Inside processBitstreamBuffer and started Processing \n");
		if(mParam.completeImageProcessed==OMX_TRUE)
		{
		  // processingInit();
		   mParam.initializeParamAndConfig();
		   //mParser.btparInit();
		   mParam.completeImageProcessed=OMX_FALSE;
		}
		#ifdef __PERF_MEASUREMENT
		write_ticks_to_buffer("\nStart Parsing.");
		#endif
		btstState = mParser.jpegParser();
		#ifdef __PERF_MEASUREMENT
		write_ticks_to_buffer("\nEEnd Parsing.");
		#endif

		switch(btstState)
		{
		case DJPEG_END_OF_DATA:
			PARSER_WAITING_FOR_BUFFERS=1;
			break;
		case DJPEG_UNSUPPORTED_MODE:
				OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL ProcessBitstream Unsupported mode \n");
			proxy.eventHandler(OMX_EventError,(OMX_U32)OMX_ErrorStreamCorrupt,0);
			//while(1);
			break;
		case DJPEG_NO_ERROR:
			PARSER_WAITING_FOR_BUFFERS=0;
			break;
		default:
			//To remove warning in linux
			break;


		}
	}
}

void jpegdec_arm_nmf_parser_dual::set_pJdcIn_parameters(ts_t1xhv_vdc_jpeg_param_in *pJdcIn_parameters){
	OMX_U8 cnt;
	OMX_BOOL isStandardHUFFTable;
	memset(pJdcIn_parameters,0x0,sizeof(ts_t1xhv_vdc_jpeg_param_in));

    pJdcIn_parameters->frame_width = mParam.frameWidth;
	pJdcIn_parameters->frame_height = mParam.frameHeight;

	OstTraceFiltInst1(TRACE_API, "PARSER_DUAL:  set_pJdcIn_parameters Width : %d \n",pJdcIn_parameters->frame_width);
	OstTraceFiltInst1(TRACE_API, "PARSER_DUAL:  set_pJdcIn_parameters Height : %d \n",pJdcIn_parameters->frame_height);

	if(mParam.isCroppingEnabled){
		pJdcIn_parameters->window_width = mParam.cropWindowWidth;
		pJdcIn_parameters->window_height= mParam.cropWindowHeight;

	}
	else{
		pJdcIn_parameters->window_width = mParam.frameWidth;
		pJdcIn_parameters->window_height= mParam.frameHeight;

	}

	OstTraceFiltInst1(TRACE_FLOW, "PARSER_DUAL:  set_pJdcIn_parameters window_width : %d \n",pJdcIn_parameters->window_width);
	OstTraceFiltInst1(TRACE_FLOW, "PARSER_DUAL:  set_pJdcIn_parameters window_height : %d \n",pJdcIn_parameters->window_height);

	pJdcIn_parameters->window_horizontal_offset    = mParam.horizontal_offset;
	pJdcIn_parameters->window_vertical_offset	   = mParam.vertical_offset;
    pJdcIn_parameters->downsampling_factor = mParam.downsamplingFactor;

    if (mParam.downsamplingFactor != NO_DOWNSAMPLING)
    {
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL:  isSWDecoderReq SET for Downsampling\n");
		isSWDecoderReq = OMX_TRUE;
	}

	// Data Dependency considers validity of Dynamic configuration in all cases(whether setconfig or not)
	// So if there is no or one time setconfig call is there following code takes care of that.

	//if(m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit==0){
	if(!mParam.isCroppingEnabled){
		m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_width = mParam.frameWidth;
		m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_height =mParam.frameHeight;

		m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.vertical_offset = 0;
		m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.horizontal_offset =0;
		//m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.downsampling_factor =mParam.downsamplingFactor;
	}

	if(!mParam.isDownScalingEnabled) {
		m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.downsampling_factor = mParam.downsamplingFactor;
	}
/*
	if(m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit==1){
		if(mParam.isCroppingEnabled){
			m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.downsampling_factor = mParam.downsamplingFactor;
		}
		else{
			m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_width = (mParam.frameWidth+0xf)& 0xfff0;
			m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_height =  (mParam.frameHeight+0xf)& 0xfff0;
			m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.vertical_offset  = mParam.vertical_offset;
			m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.horizontal_offset = mParam.horizontal_offset;
		}
	}
*/
	pJdcIn_parameters->nb_components = mParam.nbComponents;
	downsamplingFactor = mParam.downsamplingFactor;
	windowWidth = pJdcIn_parameters->window_width;
	windowHeight = pJdcIn_parameters->window_height;

	for(cnt =0; cnt<MAX_NUMBER_COMPONENTS;cnt++){
		if(mParam.components[cnt].componentId == mParam.IdY)
		{
			OMX_U8 DChuffmanDestSel= mParam.components[cnt].DCHuffmanTableDestinationSelector;
			OMX_U8 AChuffmanDestSel= mParam.components[cnt].ACHuffmanTableDestinationSelector;
			OMX_U8 quantDestSel= mParam.components[cnt].quantizationTableDestinationSelector;

			pJdcIn_parameters->h_sampling_factor_y = mParam.samplingFactors[cnt].hSamplingFactor;
			pJdcIn_parameters->v_sampling_factor_y = mParam.samplingFactors[cnt].vSamplingFactor;

			pJdcIn_parameters->component_selector_y = mParam.components[cnt].componentSelector;

			memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.huffman_bits_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.huffman_val_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanVal,
				   DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
                ////New Members
            memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.maxcode,
				   mParam.DCHuffmanTable[DChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.valoffset,
				   mParam.DCHuffmanTable[DChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
            memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.look_nbits,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_dc_table.look_sym,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
                /////AC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.huffman_bits_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.huffman_val_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanVal,
				   AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
                /////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.maxcode,
				   mParam.ACHuffmanTable[AChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.valoffset,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.look_nbits,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_y_ac_table.look_sym,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
	//////////hardware_hufftables
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_y_code_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanCode,
				   DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_y_size_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanSize,
				   DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_y_code_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanCode,
				   AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_y_size_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanSize,
				   AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));

			 memcpy(pJdcIn_parameters->quant_y,
				   mParam.quantizationTables[quantDestSel].QuantizationTable,
				   QUANT_TABLE_SIZE*sizeof(OMX_U16));
		}

		if(mParam.components[cnt].componentId == mParam.IdCb)
		{
			OMX_U8 DChuffmanDestSel= mParam.components[cnt].DCHuffmanTableDestinationSelector;
			OMX_U8 AChuffmanDestSel= mParam.components[cnt].ACHuffmanTableDestinationSelector;
			OMX_U8 quantDestSel= mParam.components[cnt].quantizationTableDestinationSelector;

			pJdcIn_parameters->h_sampling_factor_cb = mParam.samplingFactors[cnt].hSamplingFactor;
			pJdcIn_parameters->v_sampling_factor_cb = mParam.samplingFactors[cnt].vSamplingFactor;

			pJdcIn_parameters->component_selector_cb = mParam.components[cnt].componentSelector;

			/////DC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.huffman_bits_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.huffman_val_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanVal,
				   DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
             ////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.maxcode,
				   mParam.DCHuffmanTable[DChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.valoffset,
				   mParam.DCHuffmanTable[DChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.look_nbits,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_dc_table.look_sym,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
                /////AC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.huffman_bits_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.huffman_val_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanVal,
				   AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
             //////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.maxcode,
				   mParam.ACHuffmanTable[AChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.valoffset,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.look_nbits,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cb_ac_table.look_sym,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
			 //////////////hardware_hufftables
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_cb_code_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanCode,
				   DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_cb_size_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanSize,
				   DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_cb_code_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanCode,
				   AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_cb_size_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanSize,
				   AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));

			 memcpy(pJdcIn_parameters->quant_cb,
				   mParam.quantizationTables[quantDestSel].QuantizationTable,
				   QUANT_TABLE_SIZE*sizeof(OMX_U16));
		}

		if(mParam.components[cnt].componentId == mParam.IdCr)
		{
			OMX_U8 DChuffmanDestSel= mParam.components[cnt].DCHuffmanTableDestinationSelector;
			OMX_U8 AChuffmanDestSel= mParam.components[cnt].ACHuffmanTableDestinationSelector;
			OMX_U8 quantDestSel= mParam.components[cnt].quantizationTableDestinationSelector;

			pJdcIn_parameters->h_sampling_factor_cr = mParam.samplingFactors[cnt].hSamplingFactor;
			pJdcIn_parameters->v_sampling_factor_cr = mParam.samplingFactors[cnt].vSamplingFactor;

			pJdcIn_parameters->component_selector_cr = mParam.components[cnt].componentSelector;

			/////DC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.huffman_bits_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.huffman_val_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanVal,
				   DC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
             ////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.maxcode,
				   mParam.DCHuffmanTable[DChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.valoffset,
				   mParam.DCHuffmanTable[DChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.look_nbits,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_dc_table.look_sym,
				   mParam.DCHuffmanTable[DChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
                /////AC HUFFMAN TABLES
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.huffman_bits_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanBits,
				   HUFF_BITS_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.huffman_val_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanVal,
				   AC_HUFFVAL_TABLE_SIZE*sizeof(OMX_U16));
             //////New Members
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.maxcode,
				   mParam.ACHuffmanTable[AChuffmanDestSel].MaxCode,
				   18*sizeof(OMX_S32));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.valoffset,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ValOffset,
				   17*sizeof(OMX_S32));
             memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.look_nbits,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Nbits,
				   256*sizeof(OMX_S16));
			 memcpy( (( ts_t1xhv_vdc_sw_table *)(mHuffMemDesc.nLogicalAddress))->huffman_cr_ac_table.look_sym,
				   mParam.ACHuffmanTable[AChuffmanDestSel].Look_Sym,
				   256*sizeof(OMX_U16));
			 ///////////hardware_hufftables
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_cr_code_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanCode,
				   DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_cr_size_dc,
				   mParam.DCHuffmanTable[DChuffmanDestSel].DCHuffmanSize,
				   DC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_cr_code_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanCode,
				   AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));
			 memcpy(pJdcIn_parameters->huff_table.hw_huff_table.huffman_cr_size_ac,
				   mParam.ACHuffmanTable[AChuffmanDestSel].ACHuffmanSize,
				   AC_HUFF_TABLE_SIZE*sizeof(OMX_U16));

			 memcpy(pJdcIn_parameters->quant_cr,
				   mParam.quantizationTables[quantDestSel].QuantizationTable,
				   QUANT_TABLE_SIZE*sizeof(OMX_U16));
		}
	}


	isStandardHUFFTable = compare_hw_huff_tables(pJdcIn_parameters);

	if (isStandardHUFFTable == OMX_FALSE)
	{
		//NMF_LOG("Non standard huffman tables used \n");
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL:  isSWDecoderReq SET for Non Standard tables\n");
		isSWDecoderReq = OMX_TRUE;
	}
///////////Assigning the memory address of the software huffman table
    pJdcIn_parameters->huff_table.sw_huff_table = ((mHuffMemDesc.nLogicalAddress));

	pJdcIn_parameters->downsampling_factor = mParam.downsamplingFactor;
	pJdcIn_parameters->ace_enable = 0;
    pJdcIn_parameters->ace_strength = 0;
	pJdcIn_parameters->nb_scan_components = mParam.nbScanComponents;
	pJdcIn_parameters->progressive_mode =  mParam.mode;
	pJdcIn_parameters->successive_approx_position = mParam.successiveApproxPosition;
	pJdcIn_parameters->start_spectral_selection = mParam.startSpectralSelection;
	pJdcIn_parameters->end_spectral_selection = mParam.endSpectralSelection;
	pJdcIn_parameters->restart_interval = mParam.restartInterval;
	colourMode = mParam.nbComponents;

		//code segment for setting the values of color mode
		{
				//convert cropping and downsampling
				t_uint32 mb_width = 0;
				t_uint32 mb_height = 0;
				t_uint16 blocks_per_line =0,blocks_per_col=0;


				blocks_per_line=windowWidth/8;
				if((windowWidth%8)!=0) blocks_per_line++;

				blocks_per_col=windowHeight/8;
				if((windowHeight%8)!=0) blocks_per_col++;

				mb_width = (blocks_per_line*(8/downsamplingFactor)) / 16;
				if  ((blocks_per_line*(8/downsamplingFactor)) % 16 != 0) mb_width ++;

				mb_height = (blocks_per_col*(8/downsamplingFactor)) / 16;
				if  ((blocks_per_col*(8/downsamplingFactor)) % 16 != 0) mb_height ++;

				if(colourMode==1)
				{
					#ifdef DBG_DUALPRINT
						printf("\nMonochrome Mode.");
					#endif
					bufferSizeReq = (mb_width*16*mb_height*16*1);
				}
				else if(colourMode==3)
				{
					if (subSamplingType == 0)
					{
						#ifdef DBG_DUALPRINT
							printf("\nColor Mode and YUV 420.");
						#endif
						bufferSizeReq = ((mb_width*16*mb_height*16*3)/2);
					}
					if (subSamplingType == 1)
					{
						#ifdef DBG_DUALPRINT
							printf("\nColor Mode and YUV 422.");
						#endif
						isSWDecoderReq = OMX_TRUE;
						bufferSizeReq = ((mb_width*16*mb_height*16*2));
					}
					if (subSamplingType == 2)
					{
						#ifdef DBG_DUALPRINT
							printf("\nColor Mode and YUV 444.");
						#endif
						isSWDecoderReq = OMX_TRUE;
						bufferSizeReq = ((mb_width*16*mb_height*16*3));
					}


				}
				else
				{
					#ifdef DBG_DUALPRINT
						printf("\nErratic Color Mode.");
					#endif
				}
		}

		memcpy(&(sdc_jpeg_param_desc.s_in_parameters),pJdcIn_parameters,sizeof(ts_t1xhv_vdc_jpeg_param_in));
		if (isSWDecoderReq == OMX_TRUE)
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: SOFTWARE CHOOSEN");
	}

}

void jpegdec_arm_nmf_parser_dual::set_pJdcInOut_parameters(){
	t_uint8 *ptr;
	ts_t1xhv_vdc_jpeg_param_inout *pJdcInOut_parameters;
	ts_ddep_sdc_jpeg_param_desc *ps_ddep_sdc_jpeg_param_desc;

	ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
	ps_ddep_sdc_jpeg_param_desc = (ts_ddep_sdc_jpeg_param_desc *) ptr;
	pJdcInOut_parameters = &(ps_ddep_sdc_jpeg_param_desc->s_in_out_frame_parameters);

	pJdcInOut_parameters->mcu_index = 0;
	pJdcInOut_parameters->end_of_band_run = 0;
	pJdcInOut_parameters->dc_predictor_y = 0;
	pJdcInOut_parameters->dc_predictor_cb = 0;
	pJdcInOut_parameters->dc_predictor_cr = 0;
	pJdcInOut_parameters->ace_count0 = 0;
	pJdcInOut_parameters->ace_count1 = 0;
	pJdcInOut_parameters->ace_count2 = 0;
	pJdcInOut_parameters->ace_count3 = 0;
	pJdcInOut_parameters->crop_mcu_index = 0;
	pJdcInOut_parameters->crop_mcu_index_in_row = 0;
	pJdcInOut_parameters->slice_pos = 0;
	pJdcInOut_parameters->mcu_index_rst = 0;
	pJdcInOut_parameters->rst_value = 0;
	memcpy(&(sdc_jpeg_param_desc.s_in_out_frame_parameters),pJdcInOut_parameters,sizeof(ts_t1xhv_vdc_jpeg_param_inout));

	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside set_pJdcInOut_parameters DONE\n");
}

void jpegdec_arm_nmf_parser_dual::set_pJdcOut_parameters(){
	t_uint8 *ptr;
	ts_t1xhv_vdc_jpeg_param_out *pJdcOut_parameters;
	ts_ddep_sdc_jpeg_param_desc *ps_ddep_sdc_jpeg_param_desc;

	ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
	ps_ddep_sdc_jpeg_param_desc = (ts_ddep_sdc_jpeg_param_desc *) ptr;
	pJdcOut_parameters = &(ps_ddep_sdc_jpeg_param_desc->s_out_parameters);

	pJdcOut_parameters->error_type = 0;
	pJdcOut_parameters->ace_offset0 = 0;
	pJdcOut_parameters->ace_offset1 = 0;
	pJdcOut_parameters->ace_offset2 = 0;
	pJdcOut_parameters->ace_offset3 = 0;

	memcpy(&(sdc_jpeg_param_desc.s_out_parameters),pJdcOut_parameters,sizeof(ts_t1xhv_vdc_jpeg_param_out));
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside set_pJdcOut_parameters DONE\n");
}

void jpegdec_arm_nmf_parser_dual::setParam(void* portDef1, void* portDef2)
{
	OMX_IMAGE_PORTDEFINITIONTYPE *tmpFormat = (OMX_IMAGE_PORTDEFINITIONTYPE*)portDef1;

	// lets see what is necessary
	this->portDef1.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	this->portDef1.nVersion.s.nVersionMajor    = 1;
    this->portDef1.nVersion.s.nVersionMinor    = 1;
    this->portDef1.nVersion.s.nRevision	= 1;
    this->portDef1.nVersion.s.nStep		= 0;
	this->portDef1.nPortIndex = 0;

	//copy format
    this->portDef1.format.image.bFlagErrorConcealment = tmpFormat->bFlagErrorConcealment;
	this->portDef1.format.image.cMIMEType = tmpFormat->cMIMEType;
	this->portDef1.format.image.eColorFormat = tmpFormat->eColorFormat;
	this->portDef1.format.image.eCompressionFormat = tmpFormat->eCompressionFormat;
//	this->portDef1.format.image.nBitrate = tmpFormat->nBitrate;
	this->portDef1.format.image.nFrameHeight = tmpFormat->nFrameHeight;
	this->portDef1.format.image.nFrameWidth = tmpFormat->nFrameWidth;
	this->portDef1.format.image.nSliceHeight = tmpFormat->nSliceHeight;
	this->portDef1.format.image.nStride = tmpFormat->nStride;
	this->portDef1.format.image.pNativeRender = tmpFormat->pNativeRender;
	this->portDef1.format.image.pNativeWindow = tmpFormat->pNativeWindow;
//	this->portDef.format.image.xFramerate = tmpFormat->xFramerate;
	//set current_frame_width & height
	current_frame_height = tmpFormat->nFrameHeight;
	current_frame_width = tmpFormat->nFrameWidth;

	tmpFormat = (OMX_IMAGE_PORTDEFINITIONTYPE*)portDef2;

	// lets see what is necessary
	this->portDef2.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	this->portDef2.nVersion.s.nVersionMajor    = 1;
    this->portDef2.nVersion.s.nVersionMinor    = 1;
    this->portDef2.nVersion.s.nRevision	= 1;
    this->portDef2.nVersion.s.nStep		= 0;
	this->portDef2.nPortIndex = 1;

	//copy format
    this->portDef2.format.image.bFlagErrorConcealment = tmpFormat->bFlagErrorConcealment;
	this->portDef2.format.image.cMIMEType = tmpFormat->cMIMEType;
	this->portDef2.format.image.eColorFormat = tmpFormat->eColorFormat;
	this->portDef2.format.image.eCompressionFormat = tmpFormat->eCompressionFormat;
//	this->portDef2.format.image.nBitrate = tmpFormat->nBitrate;
	this->portDef2.format.image.nFrameHeight = tmpFormat->nFrameHeight;
	this->portDef2.format.image.nFrameWidth = tmpFormat->nFrameWidth;
	this->portDef2.format.image.nSliceHeight = tmpFormat->nSliceHeight;
	this->portDef2.format.image.nStride = tmpFormat->nStride;
	this->portDef2.format.image.pNativeRender = tmpFormat->pNativeRender;
	this->portDef2.format.image.pNativeWindow = tmpFormat->pNativeWindow;
//	this->portDef.format.image.xFramerate = tmpFormat->xFramerate;

	OstTraceInt0(TRACE_API, "PARSER_DUAL: Inside setParam Done \n");
}


void jpegdec_arm_nmf_parser_dual::setScanParams(ts_ddep_sdc_jpeg_scan_desc *pJdcScanParams){
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside setScanParams\n");
   if (isSWDecoderReq)
   {
   		pJdcScanParams->bitOffset = (mParam.scan.bitstreamOffset);
   		pJdcScanParams->encodedDataSize = (mParam.scan.encodedDataSize);
	}
	else
	{
   		pJdcScanParams->bitOffset = ENDIANESS_CONVERSION(mParam.scan.bitstreamOffset);
   		pJdcScanParams->encodedDataSize = ENDIANESS_CONVERSION(mParam.scan.encodedDataSize);
	}
}

//Header buffer to be used

void jpegdec_arm_nmf_parser_dual::initHeaderDescriptors(){
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL Inside initHeaderDescriptors \n");
	OMX_U8 index;
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
		mHeaderDesc[index].pHeader = NULL;
		mHeaderDesc[index].isFree = OMX_TRUE;
		mHeaderDesc[index].isRequestedToFree = OMX_FALSE;
    }
}

void jpegdec_arm_nmf_parser_dual::clearAllPendingRequestsToFreeHeader(){
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL Inside clearAllPendingRequestsToFreeHeader \n");
    OMX_U8 index;
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
        if(mHeaderDesc[index].isRequestedToFree == OMX_TRUE){
			mHeaderDesc[index].isRequestedToFree = OMX_FALSE;
			mHeaderDesc[index].isFree = OMX_TRUE;
        }
	}
}

ts_ddep_sdc_jpeg_scan_desc* jpegdec_arm_nmf_parser_dual::getFreeHeaderDesc(){
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL Inside getFreeHeaderDesc \n");
    OMX_U8 index;
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
        if(mHeaderDesc[index].isFree == OMX_TRUE){
            mHeaderDesc[index].isFree = OMX_FALSE;
            return (ts_ddep_sdc_jpeg_scan_desc*)mHeaderDesc[index].pHeader;
        }
    }
    return NULL;
}

void jpegdec_arm_nmf_parser_dual::freeHeaderDesc(t_uint32 pBuffer){
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL Inside freeHeaderDesc \n");
	OMX_U8 index;
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
        if(mHeaderDesc[index].pHeader->headerBufferMpcAddress == pBuffer){
			mHeaderDesc[index].isFree = OMX_TRUE;
            mHeaderDesc[index].isRequestedToFree = OMX_TRUE;
            break;
        }
    }
}

OMX_BUFFERHEADERTYPE* jpegdec_arm_nmf_parser_dual::provideBitstreamBuffer(){
	OMX_BUFFERHEADERTYPE *pBuffer;
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL Inside provideBitstreamBuffer \n");
	while(mPorts[0].queuedBufferCount())
	{
		pBuffer = mPorts[0].dequeueBuffer();

		OstTraceFiltInst1(TRACE_FLOW, "PARSER_DUAL: Inside provideBitstreamBuffer for buffer : 0x%x \n",(OMX_U32)pBuffer);

		if(!pBuffer->nFilledLen)
		{
			if(isSWDecoderReq)
			{
				OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: isSWDecoderReq TRUE and input buffer with size ZERO \n");
				isInputBufferDequeue = OMX_TRUE;
				inputBufferFifo.pushBack((void*)pBuffer);
				ts_ddep_sdc_jpeg_scan_desc *headerBuf= getFreeHeaderDesc();
				if(headerBuf)
				{
					headerBuf->buffer_p= 0; //setting to NULL
					//Temporarily set to configuration for a single frame
					headerBuf->lastHeader=1;
					headerBuf->isFakeHeader=1;
					inputHeaderFifo.pushBack((void*)headerBuf);
				}
				else
				{
					proxy.eventHandler(OMX_EventError,(OMX_U32)OMX_ErrorInsufficientResources,0);
				}
	               		ResolveDependencies();
			}
			else
			{
				OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: isSWDecoderReq FALSE and input buffer size ZERO \n");
				//sending the input buffer and fake header buffer to MPC
				emptythisbufferSharedBufIn.emptyThisBuffer(pBuffer);
				MPC_isInputBufferDequeued = OMX_FALSE;
				isInputBufferDequeue = OMX_FALSE;
				//providing header buffer to resolve dependencies at MPC
				ts_ddep_sdc_jpeg_scan_desc *headerBuf= getFreeHeaderDesc();
				if(headerBuf)
				{
					headerBuf->buffer_p= 0; //setting to NULL
					//Temporarily set to configuration for a single frame
					headerBuf->lastHeader=1;
					headerBuf->isFakeHeader=1;
					emptythisheader.emptyThisHeader((Buffer_p)(headerBuf->headerBufferMpcAddress));
				}
				else
				{
					OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL Inside provideBitstreamBuffer \n");
					proxy.eventHandler(OMX_EventError,(OMX_U32)OMX_ErrorInsufficientResources,0);
				}

			}
		}
		else
		{
			OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: With VALID input buffer \n");
			bufferInput = pBuffer;
			MPC_isInputBufferDequeued = OMX_TRUE;
			isInputBufferDequeue = OMX_TRUE;
			return pBuffer;
	        }
	}

   return NULL;
}

t_djpegErrorCode jpegdec_arm_nmf_parser_dual::fillHeader(OMX_BUFFERHEADERTYPE *pBuffer){
	ts_ddep_sdc_jpeg_scan_desc *headerBuf= getFreeHeaderDesc();
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL Inside fillHeader \n");
	if(headerBuf){
        if (!mParam.updateData){
			SharedBuffer *sharedBuf = (SharedBuffer *) pBuffer->pInputPortPrivate;
			Buffer_p buf;
			OMX_ERRORTYPE error = OMX_ErrorNone;
            buf = (Buffer_p)sharedBuf->getMPCHeader();
			//Check Whether parsed configuration is compatible with FW
			error = checkConfAndParams();
			if(error){
				freeHeaderDesc((t_uint32)headerBuf);
				proxy.eventHandler(OMX_EventError,(OMX_U32)error,0);
			}

			error = configureCoeffAndLineBuffer();
			if(error){
				freeHeaderDesc((t_uint32)headerBuf);
				proxy.eventHandler(OMX_EventError,(OMX_U32)error,0);
			}

			set_pJdcInOut_parameters();
			set_pJdcOut_parameters();


			headerBuf->buffer_p=ENDIANESS_CONVERSION(buf);
			set_pJdcIn_parameters(&(headerBuf->s_in_parameters));
			setScanParams(headerBuf);
			//Temporarily set to configuration for a single frame
			headerBuf->lastHeader=1;
			headerBuf->isFakeHeader=0;

			if (isSWDecoderReq)
			{
				OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: Inside fillHeader and  isSWDecoderReq SET \n");
				inputHeaderFifo.pushBack((void*)headerBuf);
			}
			else
			{
				OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: Inside fillHeader and  isSWDecoderReq FALSE \n");
				emptythisheader.emptyThisHeader((Buffer_p)(headerBuf->headerBufferMpcAddress));
			}
			mParam.updateData = 1;
        }
    }
	else
		return DJPEG_MALLOC_ERROR;

	return DJPEG_NO_ERROR;
}

void jpegdec_arm_nmf_parser_dual::releaseBtstBuffer(OMX_U8 btstContent,OMX_BUFFERHEADERTYPE *pBuffer){
	if(!((btstContent&0x10)!=0))
		pBuffer->nFilledLen=0;


	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nRelease Bitstream Buffer.");
	#endif
	#ifdef __CACHE_OPTIOLD_
	HwBuffer::TCacheOperation aOp = HwBuffer::ECacheCleanOp;
	OMX_PTR pBufferMetadata = (OMX_PTR)pBuffer->pPlatformPrivate;
	OMX_U8 *logicalAddress =  HwBuffer::GetBufferLogicalAddress(pBufferMetadata);
	OMX_ERRORTYPE error_type = HwBuffer::CacheOperation(pBufferMetadata,aOp,logicalAddress,pBuffer->nAllocLen);
	#endif


	bitstreamBufferProcessed = OMX_TRUE;
	if ((isSWDecoderReq) && (!(pBuffer->nFilledLen)))
	{
		OstTraceFiltInst2(TRACE_FLOW, "PARSER_DUAL: Inside releaseBtstBuffer for  isSWDecoderReq TRUE buffer : 0x%x nFilledLen %d\n",(OMX_U32)pBuffer,pBuffer->nFilledLen);
		if(isDispatching())
		{
			mPorts[0].returnBuffer(pBuffer);
		}
		else
		{
			//printf("return from Asyn - input \n");
			Component::returnBufferAsync(0,pBuffer);
		}
		isInputBufferDequeue = OMX_FALSE;
	}

	//FIXME: Filled Length should be multiple of 16
	pBuffer->nFilledLen = (pBuffer->nFilledLen + 0xf)&0xfffffff0;

	if (isSWDecoderReq)
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: Inside releaseBtstBuffer releasing bits buffer to soft algo\n");
		inputBufferFifo.pushBack((void*)pBuffer);
		//add this buffer to list of bitstream buffer
		ResolveDependencies();
	}
	else
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: Inside releaseBtstBuffer releasing bits buffer to ddep\n");

#ifdef __CACHE_OPTI
	if (save_memory_context)
	{
		OstTraceFiltInst3(TRACE_FLOW,"PARSER_DUAL Abt to call VFM_CacheClean with parameters memory_context (0x%x), buffer_id (0x%x) filledLen (%d) ",(OMX_U32)save_memory_context, (OMX_U32)pBuffer->pBuffer, pBuffer->nFilledLen);
		VFM_CacheClean(save_memory_context, pBuffer->pBuffer, pBuffer->nFilledLen);
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL  VFM_CacheClean DONE ");
	}
	else
	{
		OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL  VFM_CacheClean FAIL as memorycontext is EMPTY ");
	}

	OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL  after calling VFM_CacheClean");
#endif

		emptythisbufferSharedBufIn.emptyThisBuffer(pBuffer);
	}

	MPC_isInputBufferDequeued = OMX_FALSE;

	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nReleased Bitstream Buffer");
	#endif


	if (!isSWDecoderReq)
	{
		if(mPorts[1].queuedBufferCount())
		{
			//NMF_LOG("Sending output buffer from here \n");
			OMX_BUFFERHEADERTYPE *bufOut;
			bufOut = mPorts[1].dequeueBuffer();
			//NMF_LOG("Output buffer dequeue 0x%x \n",bufOut);
			fillthisbufferSharedBufOut.fillThisBuffer(bufOut);
		}
	}


}



void jpegdec_arm_nmf_parser_dual::updateDynamicConfigurationStructure(){
	t_uint16 atomic_write;
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside updateDynamicConfigurationStructure\n");
	atomic_write = m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_entry + 1;
	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_entry = atomic_write;

//	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_width = (mParam.cropWindowWidth+ 0xf)&0xfff0;
//	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_height = (mParam.cropWindowHeight+ 0xf)&0xfff0;

	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_width = mParam.cropWindowWidth;
	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_height = mParam.cropWindowHeight;

	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.vertical_offset = mParam.vertical_offset;
	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.horizontal_offset =mParam.horizontal_offset;
	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.downsampling_factor =mParam.downsamplingFactor;

	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit = atomic_write;
}

void jpegdec_arm_nmf_parser_dual::updateParamAndConfigCrop(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig){
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL: Inside updateParamAndConfigCrop\n");
	mParam.cropWindowWidth = pDynamicConfig->window_width;
	mParam.cropWindowHeight  = pDynamicConfig->window_height;
	mParam.horizontal_offset = pDynamicConfig->horizontal_offset;
	mParam.vertical_offset = pDynamicConfig->vertical_offset;
}

void jpegdec_arm_nmf_parser_dual::updateParamAndConfigScale(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig){
	mParam.downsamplingFactor = (JPEGDec_Proxy_Downsampling_Factor)pDynamicConfig->downsampling_factor;
}


OMX_ERRORTYPE jpegdec_arm_nmf_parser_dual::checkConfAndParams(){
	if(!(mParam.frameWidth>0 && mParam.frameWidth<=8176))
    {
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting (!(mParam.frameWidth>0 && mParam.frameWidth<=8176)) \n");
		return OMX_ErrorUnsupportedSetting;
	}
	if(!(mParam.frameHeight>0 && mParam.frameHeight<=8176))
    {
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting !(mParam.frameHeight>0 && mParam.frameHeight<=8176) \n");
		return OMX_ErrorUnsupportedSetting;
    }

    if(!(mParam.downsamplingFactor ==1
	   || mParam.downsamplingFactor ==2
	   || mParam.downsamplingFactor== 4
	   || mParam.downsamplingFactor == 8))
	{
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting unsupported downscale \n");
		 return OMX_ErrorUnsupportedSetting;
	}

	if(mParam.nbComponents>MAX_NUMBER_COMPONENTS)
    {
        OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported componetnss \n");
	   return OMX_ErrorUnsupportedSetting;
    }

   if(mParam.nbScanComponents>MAX_NUMBER_COMPONENTS)
   {
       OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported scan componetnss \n");
	   return OMX_ErrorUnsupportedSetting;
   }

   for(t_uint32 cnt =0;cnt<mParam.nbComponents;cnt++ )
   {
		t_uint32 h_factor, v_factor;
		h_factor = mParam.samplingFactors[cnt].hSamplingFactor;
		v_factor = mParam.samplingFactors[cnt].vSamplingFactor;

		if(h_factor<1 || h_factor >4 || h_factor==3){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported h_factor  \n");
			return OMX_ErrorUnsupportedSetting;
        }

		if(v_factor<1 || v_factor >4 || v_factor==3){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported v_factor  \n");
			return OMX_ErrorUnsupportedSetting;
        }
    }

   if(mParam.mode ==1){
       OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported mode  \n");
		return OMX_ErrorUnsupportedSetting;
   }

	if(mParam.mode==1){
        if(mParam.startSpectralSelection>63){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported startSpectralSelection  \n");
			return OMX_ErrorUnsupportedSetting;
        }
		if(mParam.endSpectralSelection!=63){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported endSpectralSelection  \n");
			return OMX_ErrorUnsupportedSetting;
        }
	}
	else{
		if(mParam.startSpectralSelection!=0){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported startSpectralSelection  \n");
			return OMX_ErrorUnsupportedSetting;
    }
		if(mParam.endSpectralSelection!=63){
            OstTraceFiltInst0(TRACE_ERROR, "PARSER_DUAL checkConfAndParams OMX_ErrorUnsupportedSetting Unsupported endSpectralSelection  \n");
			return OMX_ErrorUnsupportedSetting;
        }
	}
	return OMX_ErrorNone;
}


OMX_ERRORTYPE jpegdec_arm_nmf_parser_dual::configureCoeffAndLineBuffer(){
	t_uint8 *ptr;
	t_uint32 lineBufferSize=0,coeffBufferSize=0;
	ts_t1xhv_vdc_frame_buf_out *p_out_frame_buffer;
	ts_ddep_sdc_jpeg_param_desc *ps_ddep_sdc_jpeg_param_desc;
	t_uint16 width,height;
	OstTraceFiltInst0(TRACE_API, "PARSER_DUAL Inside configureCoeffAndLineBuffer \n");
	width =	(mParam.frameWidth+0xf)&0xfff0;
	height = (mParam.frameHeight+0xf)&0xfff0;
	ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
	ps_ddep_sdc_jpeg_param_desc = (ts_ddep_sdc_jpeg_param_desc *) ptr;
	p_out_frame_buffer = &(ps_ddep_sdc_jpeg_param_desc->s_out_frame_buffer);

	//Allocate Line Buffer
	if(mParam.samplingFactors[0].vSamplingFactor==1 &&mParam.samplingFactors[1].vSamplingFactor==1 &&
	   mParam.samplingFactors[2].vSamplingFactor==1 &&mParam.downsamplingFactor==DOWNSAMPLING_FACTOR_8){
		lineBufferSize = (width/2 + 0xf)&0xfffffff0;
		//lineBufferSize = (lineBufferSize>64)? lineBufferSize: 64;
	}
	else{
		lineBufferSize = 0;
	}

	if(lineBufferSize){
			ptr = (t_uint8*)mLineBufferDesc.nPhysicalAddress;
			p_out_frame_buffer->addr_jpeg_line_buffer = ENDIANESS_CONVERSION((OMX_U32)ptr);

		//TODO:allocate memory for line buffer.
		//return OMX_ErrorUndefined;
	}
	else{
		p_out_frame_buffer->addr_jpeg_line_buffer = 0;
	}

	//Allocate memory for coefficient buffer
	if(mParam.mode == PROGRESSIVE_JPEG){
		t_uint16 maxhSampFactor,maxvSampFactor;
		maxhSampFactor = mParam.samplingFactors[0].hSamplingFactor;
		maxhSampFactor = (maxhSampFactor > mParam.samplingFactors[1].hSamplingFactor) ? maxhSampFactor: mParam.samplingFactors[1].hSamplingFactor;
		maxhSampFactor = (maxhSampFactor > mParam.samplingFactors[2].hSamplingFactor) ? maxhSampFactor: mParam.samplingFactors[2].hSamplingFactor;

		maxvSampFactor = mParam.samplingFactors[0].vSamplingFactor;
		maxvSampFactor = (maxvSampFactor > mParam.samplingFactors[1].vSamplingFactor) ? maxvSampFactor: mParam.samplingFactors[1].vSamplingFactor;
		maxvSampFactor = (maxvSampFactor > mParam.samplingFactors[2].vSamplingFactor) ? maxvSampFactor: mParam.samplingFactors[2].vSamplingFactor;

		coeffBufferSize = ((height)*(width)*2*
						   (mParam.samplingFactors[0].hSamplingFactor * mParam.samplingFactors[0].vSamplingFactor +
							mParam.samplingFactors[1].hSamplingFactor * mParam.samplingFactors[1].vSamplingFactor +
							mParam.samplingFactors[2].hSamplingFactor * mParam.samplingFactors[2].vSamplingFactor))/
							(maxhSampFactor*maxvSampFactor);

		coeffBufferSize = (coeffBufferSize + 0xf)&0xfffffff0;
	  }
	  else{
		  coeffBufferSize =0;
	  }

	  if(coeffBufferSize){
		  //TODO:Allocate memory for coefficient buffer here
		  return OMX_ErrorUndefined;
	  }
	  else{
		  p_out_frame_buffer->addr_jpeg_coef_buffer = 0;
	  }

	if(!mParam.samplingFactors[1].hSamplingFactor)
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: formatType is OMX_COLOR_FormatMonochrome \n");
		formatType = OMX_COLOR_FormatMonochrome;
	}
	else if ((mParam.samplingFactors[0].hSamplingFactor==1) && (mParam.samplingFactors[0].vSamplingFactor==1) &&
		 	 (mParam.samplingFactors[1].hSamplingFactor==1) && (mParam.samplingFactors[1].vSamplingFactor==1) &&
			 (mParam.samplingFactors[2].hSamplingFactor==1) && (mParam.samplingFactors[2].vSamplingFactor==1))
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: formatType is OMX_COLOR_FormatYUV444Interleaved \n");
		formatType = OMX_COLOR_FormatYUV444Interleaved;
		subSamplingType = 2; // YUV 444
	}
	else if ((mParam.samplingFactors[0].hSamplingFactor==2) && (mParam.samplingFactors[0].vSamplingFactor==1) &&
			 (mParam.samplingFactors[1].hSamplingFactor==1) && (mParam.samplingFactors[1].vSamplingFactor==1) &&
			 (mParam.samplingFactors[2].hSamplingFactor==1) && (mParam.samplingFactors[2].vSamplingFactor==1))
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: formatType is OMX_COLOR_FormatYUV422Planar \n");
		formatType = OMX_COLOR_FormatYUV422Planar;
		subSamplingType = 1; // YUV 422
	}
	else if ((mParam.samplingFactors[0].hSamplingFactor==2) && (mParam.samplingFactors[0].vSamplingFactor==2) &&
			 (mParam.samplingFactors[1].hSamplingFactor==1) && (mParam.samplingFactors[1].vSamplingFactor==1) &&
			 (mParam.samplingFactors[2].hSamplingFactor==1) && (mParam.samplingFactors[2].vSamplingFactor==1))
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: formatType is OMX_COLOR_FormatYUV420Planar \n");
		formatType = OMX_COLOR_FormatYUV420Planar; // YUV 420
		subSamplingType = 0; // YUV 420
	}
	else
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_DUAL: formatType is OMX_COLOR_FormatYUV444Interleaved default \n");
		formatType = OMX_COLOR_FormatYUV444Interleaved; // YUV 420
		subSamplingType = 2; // YUV 444
	}


	return OMX_ErrorNone;
}


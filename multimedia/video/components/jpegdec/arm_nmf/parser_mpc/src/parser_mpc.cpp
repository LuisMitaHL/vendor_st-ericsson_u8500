/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "jpegdec/arm_nmf/parser_mpc.nmf"
#include <string.h>
#include "SharedBuffer.h"
//#include "mpc/common/buffer.idt"
#ifdef __CACHE_OPTIOLD
	#include "mmhwbuffer.h"
#endif

#ifdef __CACHE_OPTI
#include "VFM_Memory.h"
#endif

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_jpegdec_arm_nmf_parser_mpc_src_parser_mpcTraces.h"
#endif


#ifdef __PERF_MEASUREMENT
#include "../../../proxy/test_arm/osi_perf.h"
#endif

jpegdec_arm_nmf_parser_mpc::jpegdec_arm_nmf_parser_mpc(){
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
	mLinkListBufferDesc.nPhysicalAddress = 0;

	mHuffMemDesc.nLogicalAddress = 0;
	mHuffMemDesc.nMpcAddress = 0;
	mHuffMemDesc.nPhysicalAddress = 0;
	mHuffMemDesc.nSize = 0;


	m_ps_ddep_sdc_jpeg_dynamic_configuration = 0;

	mLineBufferDesc.nLogicalAddress = 0;
	mLineBufferDesc.nMpcAddress  = 0;
	mLineBufferDesc.nPhysicalAddress  = 0;
	mLineBufferDesc.nSize  = 0;
	bufferInput = 0x0;
	isInputBufferDequeued = OMX_FALSE;
	save_memory_context = 0x0;

}

jpegdec_arm_nmf_parser_mpc::~jpegdec_arm_nmf_parser_mpc(){
}

t_nmf_error jpegdec_arm_nmf_parser_mpc::construct(void)
{
	mParser.initializeReferences(this,&mParam);
	return NMF_OK;
}

void jpegdec_arm_nmf_parser_mpc::destroy(){

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
    mPorts[0].init(InputPort,false,true,NULL,mBufIn_list,16,&inputport,0,(portsDisabled&1),(portsTunneled&1),this);
    mPorts[1].init(OutputPort,false,true,NULL,mBufOut_list,16,&outputport,1,(portsDisabled>>1)&1,(portsTunneled>>1)&1,this);

    init(2, mPorts, &proxy, &me, 0); //FIXME:invalid parameter is to be initilaized properly.
    OstTraceInt0(TRACE_API, "PARSER_MPC: Inside fsminit DONE \n");
}

void METH(disablePortIndication)(t_uint32 portIdx){}
void METH(enablePortIndication)(t_uint32 portIdx){}
void METH(flushPortIndication)(t_uint32 portIdx){}
void METH(reset)(){}

void jpegdec_arm_nmf_parser_mpc::processEvent(void){
	Component::processEvent() ;
}

void jpegdec_arm_nmf_parser_mpc::fromProcComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nEnter ETB Parser.");
	#endif

	OstTraceFiltInst1(TRACE_API, "PARSER_MPC: fromProcComp_emptyThisBuffer buffer : 0x%x \n",(OMX_U32)pBuffer);

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

void jpegdec_arm_nmf_parser_mpc::fromProcComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nEnter FTB Parser");
	#endif
	OstTraceFiltInst1(TRACE_API, "PARSER_MPC: fromProcComp_fillThisBuffer buffer : 0x%x \n",(OMX_U32)pBuffer);

	Component::deliverBuffer(1, pBuffer);
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nExit FTB Parser");
	#endif

}

void jpegdec_arm_nmf_parser_mpc::fromSharedBufferComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){

	//Return Image buffer to proxy
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nEnter FBD Parser");
	#endif

	OstTraceFiltInst1(TRACE_API, "PARSER_MPC: fromSharedBufferComp_emptyThisBuffer buffer : 0x%x \n",(OMX_U32)pBuffer);

#ifdef __CACHE_OPTI
	OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC before calling CacheInvalidate");
	if (save_memory_context)
	{
		OstTraceFiltInst3(TRACE_FLOW, "PARSER_MPC before calling CacheInvalidate:: context (0x%x)  bBuffer (0x%x)   filledLen (%d)",(OMX_U32)save_memory_context,(OMX_U32)pBuffer->pBuffer, pBuffer->nFilledLen);
		VFM_CacheInvalidate(save_memory_context, pBuffer->pBuffer, pBuffer->nFilledLen);
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC VFM_CacheInvalidate DONE ");
	}
	else
	{
		OstTraceFiltInst0(TRACE_ERROR, "PARSER_MPC VFM_CacheInvalidate FAILED as memory context is EMPTY ");
	}
#endif

//	if(isDispatching())
//	{
//		mPorts[1].returnBuffer(pBuffer);
//	}
//	else
	{
		//printf("return from Asyn - input \n");
	Component::returnBufferAsync(1,pBuffer);
	}
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nExit FBD Parser");
	#endif

}

void jpegdec_arm_nmf_parser_mpc::fromSharedBufferComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	//Return Bitstream buffer to proxy
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nEnter EBD Parser");
	#endif

	OstTraceFiltInst1(TRACE_API, "PARSER_MPC: fromSharedBufferComp_fillThisBuffer buffer : 0x%x \n",(OMX_U32)pBuffer);

//	if(isDispatching())
//	{
//		mPorts[0].returnBuffer(pBuffer);
//	}
//	else
	{
		//printf("return from Asyn - input \n");
	Component::returnBufferAsync(0,pBuffer);
	}

	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nExit EBD Parser");
	#endif

}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword parameter){
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC sendCommand\n");
	switch (cmd){
		case OMX_CommandStateSet:
		{
			switch (parameter){
				case OMX_StatePause:
				case OMX_StateExecuting:
						compState = (OMX_STATETYPE)parameter;
						break;
				case OMX_StateIdle:
							if (isInputBufferDequeued && bufferInput)
							{
								if(isDispatching())
								{
									mPorts[0].returnBuffer(bufferInput);
								}
								else
								{
									Component::returnBufferAsync(0,bufferInput);
								}
								isInputBufferDequeued = OMX_FALSE;
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
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: fillthisheader \n");
    freeHeaderDesc((t_uint32)pHeader);
	clearAllPendingRequestsToFreeHeader();
}

void METH(setConfig)(t_uint32 nParamIndex,ts_ddep_sdc_jpeg_dynamic_params config){
	OstTraceInt0(TRACE_API, "PARSER_MPC: setConfig \n");
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
    /////not required here as mpc doesnt need sw huffman tables
    return;
}

void jpegdec_arm_nmf_parser_mpc::sendmemoryContext(void *mpc_vfm_mem_ctxt)
{
	save_memory_context = mpc_vfm_mem_ctxt;
	OstTraceInt1(TRACE_FLOW,"PARSER_MPC: Inside sendmemoryContext <> memory context (0x%x)",(OMX_U32)save_memory_context);
}

void METH(memoryRequirements)(t_uint16 mChannelId,ts_ddep_buffer_descriptor paramBufferDesc,
							  ts_ddep_buffer_descriptor linkListBufferDesc,ts_ddep_buffer_descriptor debugBufferDesc,
							  ts_ddep_buffer_descriptor lineBufferDesc)
{
	OMX_U8 index;
	//Store Memory Descriptors and initialize pointers

	OstTraceInt0(TRACE_API, "PARSER_MPC: Inside memoryRequirements \n");

	mParamBufferDesc = paramBufferDesc;
    mLinkListBufferDesc = linkListBufferDesc;
    mDebugBufferDesc = debugBufferDesc;
    mLineBufferDesc = lineBufferDesc;

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
	OstTraceInt0(TRACE_API, "PARSER_MPC: Inside memoryRequirements Provide memories to MPC\n");
    iDdep.setNeeds(mChannelId,mLinkListBufferDesc,mDebugBufferDesc);
	iDdep.setParameter(mChannelId,ID_SDC_JPEG,mParamBufferDesc);

}

void METH(ComputeARMLoad)(t_bool value)
{
	OstTraceInt1(TRACE_API, "PARSER_MPC: Inside ComputeARMLoad value : %d \n",value);
	iDdep.disableFWCodeexection((t_bool)value);
}


void METH(process)(){
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: Inside process \n");
	while(mPorts[1].queuedBufferCount()){
		OMX_BUFFERHEADERTYPE *bufOut;
		bufOut = mPorts[1].dequeueBuffer();
		OstTraceFiltInst1(TRACE_FLOW, "PARSER_MPC: Output buffer dequeued : 0x%x \n",(OMX_U32)bufOut);
		fillthisbufferSharedBufOut.fillThisBuffer(bufOut);
	}

	processBitstreamBuffer();
}

void jpegdec_arm_nmf_parser_mpc:: processBitstreamBuffer()
{
	t_djpegErrorCode btstState;

	if(mPorts[0].queuedBufferCount())
	{
		if(mParam.completeImageProcessed==OMX_TRUE)
		{
		  // processingInit();
		   mParam.initializeParamAndConfig();
		   //mParser.btparInit();
		   mParam.completeImageProcessed=OMX_FALSE;
		}
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: Input buffer dequeud and inside processBitstreamBuffer \n");
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
			proxy.eventHandler(OMX_EventError,(OMX_U32)OMX_ErrorStreamCorrupt,0);
			OstTraceFiltInst0(TRACE_ERROR, "PARSER_MPC: ProcessBitstream Unsupported mode \n");
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

void jpegdec_arm_nmf_parser_mpc::set_pJdcIn_parameters(ts_t1xhv_vdc_jpeg_param_in *pJdcIn_parameters){
	OMX_U8 cnt;
 /*   {
		//Initialize memory
		t_uint32 count;
        for (count = 0; count<sizeof(ts_t1xhv_vdc_jpeg_param_in);count++)
			((t_uint8*)pJdcIn_parameters)[count]=0;
    }
*/

	OstTraceFiltInst0(TRACE_API, "JPEGDEC PARSER_MPC Inside set_pJdcIn_parameters \n");

	memset(pJdcIn_parameters,0x0,sizeof(ts_t1xhv_vdc_jpeg_param_in));

//    pJdcIn_parameters->frame_width = (mParam.frameWidth+0xf)& 0xfff0;
//	pJdcIn_parameters->frame_height = (mParam.frameHeight+0xf) &0xfff0;
    pJdcIn_parameters->frame_width = mParam.frameWidth;
	pJdcIn_parameters->frame_height = mParam.frameHeight;

	if(mParam.isCroppingEnabled){
//		pJdcIn_parameters->window_width = (mParam.cropWindowWidth+ 0xf)&0xfff0;
//		pJdcIn_parameters->window_height= (mParam.cropWindowHeight + 0xf)&0xfff0;
		pJdcIn_parameters->window_width = mParam.cropWindowWidth;
		pJdcIn_parameters->window_height= mParam.cropWindowHeight;

	}
	else{
		//pJdcIn_parameters->window_width = (mParam.frameWidth+0xf)& 0xfff0;
		//pJdcIn_parameters->window_height= (mParam.frameHeight+0xf) &0xfff0;
		pJdcIn_parameters->window_width = mParam.frameWidth;
		pJdcIn_parameters->window_height= mParam.frameHeight;

	}

	pJdcIn_parameters->window_horizontal_offset    = mParam.horizontal_offset;
	pJdcIn_parameters->window_vertical_offset	   = mParam.vertical_offset;
    pJdcIn_parameters->downsampling_factor = mParam.downsamplingFactor;

	// Data Dependency considers validity of Dynamic configuration in all cases(whether setconfig or not)
	// So if there is no or one time setconfig call is there following code takes care of that.

	//if(m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit==0){
	if(!mParam.isCroppingEnabled){
		//m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_width = (mParam.frameWidth+0xf)& 0xfff0;
		//m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_height = (mParam.frameHeight+0xf)& 0xfff0;
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

	for(cnt =0; cnt<MAX_NUMBER_COMPONENTS;cnt++){
		if(mParam.components[cnt].componentId == mParam.IdY)
		{
			OMX_U8 DChuffmanDestSel= mParam.components[cnt].DCHuffmanTableDestinationSelector;
			OMX_U8 AChuffmanDestSel= mParam.components[cnt].ACHuffmanTableDestinationSelector;
			OMX_U8 quantDestSel= mParam.components[cnt].quantizationTableDestinationSelector;

			pJdcIn_parameters->h_sampling_factor_y = mParam.samplingFactors[cnt].hSamplingFactor;
			pJdcIn_parameters->v_sampling_factor_y = mParam.samplingFactors[cnt].vSamplingFactor;

			pJdcIn_parameters->component_selector_y = mParam.components[cnt].componentSelector;

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
///////////Assigning the memory address of the software huffman table but here its assigned NULL as not required
    pJdcIn_parameters->huff_table.sw_huff_table = (NULL);

	pJdcIn_parameters->downsampling_factor = mParam.downsamplingFactor;
	pJdcIn_parameters->ace_enable = 0;
    pJdcIn_parameters->ace_strength = 0;
	pJdcIn_parameters->nb_scan_components = mParam.nbScanComponents;
	pJdcIn_parameters->progressive_mode =  mParam.mode;
	pJdcIn_parameters->successive_approx_position = mParam.successiveApproxPosition;
	pJdcIn_parameters->start_spectral_selection = mParam.startSpectralSelection;
	pJdcIn_parameters->end_spectral_selection = mParam.endSpectralSelection;
	pJdcIn_parameters->restart_interval = mParam.restartInterval;
}

void jpegdec_arm_nmf_parser_mpc::set_pJdcInOut_parameters(){
	t_uint8 *ptr;
	ts_t1xhv_vdc_jpeg_param_inout *pJdcInOut_parameters;
	ts_ddep_sdc_jpeg_param_desc *ps_ddep_sdc_jpeg_param_desc;

	ptr = (t_uint8*)mParamBufferDesc.nLogicalAddress;
	ps_ddep_sdc_jpeg_param_desc = (ts_ddep_sdc_jpeg_param_desc *) ptr;
	pJdcInOut_parameters = &(ps_ddep_sdc_jpeg_param_desc->s_in_out_frame_parameters);
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: Inside set_pJdcInOut_parameters\n");
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
}

void jpegdec_arm_nmf_parser_mpc::set_pJdcOut_parameters(){
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

	OstTraceFiltInst0(TRACE_API, "PARSER_MPC Out parameters initialized \n");
}

void jpegdec_arm_nmf_parser_mpc::setScanParams(ts_ddep_sdc_jpeg_scan_desc *pJdcScanParams){
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: setScanParams \n");
   pJdcScanParams->bitOffset = ENDIANESS_CONVERSION(mParam.scan.bitstreamOffset);
   pJdcScanParams->encodedDataSize = ENDIANESS_CONVERSION(mParam.scan.encodedDataSize);
}

//Header buffer to be used

void jpegdec_arm_nmf_parser_mpc::initHeaderDescriptors(){
	OMX_U8 index;
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: Inside initHeaderDescriptors \n");
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
		mHeaderDesc[index].pHeader = NULL;
		mHeaderDesc[index].isFree = OMX_TRUE;
		mHeaderDesc[index].isRequestedToFree = OMX_FALSE;
    }
}

void jpegdec_arm_nmf_parser_mpc::clearAllPendingRequestsToFreeHeader(){
    OMX_U8 index;
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: Inside clearAllPendingRequestsToFreeHeader \n");
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
        if(mHeaderDesc[index].isRequestedToFree == OMX_TRUE){
			mHeaderDesc[index].isRequestedToFree = OMX_FALSE;
			mHeaderDesc[index].isFree = OMX_TRUE;
        }
	}
}

ts_ddep_sdc_jpeg_scan_desc* jpegdec_arm_nmf_parser_mpc::getFreeHeaderDesc(){
    OMX_U8 index;
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: Inside getFreeHeaderDesc \n");
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
        if(mHeaderDesc[index].isFree == OMX_TRUE){
            mHeaderDesc[index].isFree = OMX_FALSE;
            return (ts_ddep_sdc_jpeg_scan_desc*)mHeaderDesc[index].pHeader;
        }
    }
    return NULL;
}

void jpegdec_arm_nmf_parser_mpc::freeHeaderDesc(t_uint32 pBuffer){
	OMX_U8 index;
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: Inside freeHeaderDesc \n");
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
        if(mHeaderDesc[index].pHeader->headerBufferMpcAddress == pBuffer){
			mHeaderDesc[index].isFree = OMX_TRUE;
            mHeaderDesc[index].isRequestedToFree = OMX_TRUE;
            break;
        }
    }
}

OMX_BUFFERHEADERTYPE* jpegdec_arm_nmf_parser_mpc::provideBitstreamBuffer(){
	OMX_BUFFERHEADERTYPE *pBuffer;
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: Inside provideBitstreamBuffer \n");
	while(mPorts[0].queuedBufferCount())
	{
		pBuffer = mPorts[0].dequeueBuffer();

		//if nFilledLen for input buffer is Zero then two possibilities
		if (!pBuffer->nFilledLen)
		{
			OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: Input buffer with zero filledLen \n");
			//sending the input buffer and fake header buffer to MPC
			emptythisbufferSharedBufIn.emptyThisBuffer(pBuffer);
			isInputBufferDequeued = OMX_FALSE;

			//providing header buffer to resolve dependencies at MPC
			ts_ddep_sdc_jpeg_scan_desc *headerBuf= getFreeHeaderDesc();
			if(headerBuf)
			{
				headerBuf->buffer_p= 0; //setting to NULL
				//Temporarily set to configuration for a single frame
				headerBuf->lastHeader=1;
				headerBuf->isFakeHeader=1;
				emptythisheader.emptyThisHeader((Buffer_p)(headerBuf->headerBufferMpcAddress));
				OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: Fake header \n");
			}
			else
			{
				OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: InsufficeientResource for fake buffer\n");
				proxy.eventHandler(OMX_EventError,(OMX_U32)OMX_ErrorInsufficientResources,0);
			}
		}
		else
		{
			OstTraceFiltInst1(TRACE_FLOW, "PARSER_MPC: Input buffer with filled Size : %d \n",pBuffer->nFilledLen);
			bufferInput = pBuffer;
			isInputBufferDequeued = OMX_TRUE;
			return pBuffer;
	        }
	}

   return NULL;
}

t_djpegErrorCode jpegdec_arm_nmf_parser_mpc::fillHeader(OMX_BUFFERHEADERTYPE *pBuffer){
	ts_ddep_sdc_jpeg_scan_desc *headerBuf= getFreeHeaderDesc();

	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: Inside fillHeader \n");

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

			OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: Pushing header buffer to MPC \n");
			emptythisheader.emptyThisHeader((Buffer_p)(headerBuf->headerBufferMpcAddress));
			mParam.updateData = 1;
        }
    }
	else
		return DJPEG_MALLOC_ERROR;

	return DJPEG_NO_ERROR;
}

void jpegdec_arm_nmf_parser_mpc::releaseBtstBuffer(OMX_U8 btstContent,OMX_BUFFERHEADERTYPE *pBuffer){

	if(!((btstContent&0x10)!=0))
		pBuffer->nFilledLen=0;


	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nRelease Bitstream Buffer.");
	#endif


	OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: Pushing input buffer to MPC \n");

	//FIXME: Filled Length should be multiple of 16
	pBuffer->nFilledLen = (pBuffer->nFilledLen + 0xf)&0xfffffff0;
	OstTraceInt1(TRACE_FLOW, "PARSER_MPC sending buffer to ddep -%x \n",(OMX_U32)pBuffer);

#ifdef __CACHE_OPTI
	if (save_memory_context)
	{
		OstTraceFiltInst3(TRACE_FLOW,"PARSER_MPC Abt to call VFM_CacheClean with parameters memory_context (0x%x), buffer_id (0x%x) filledLen (%d) ",(OMX_U32)save_memory_context, (OMX_U32)pBuffer->pBuffer, pBuffer->nFilledLen);
		VFM_CacheClean(save_memory_context, pBuffer->pBuffer, pBuffer->nFilledLen);
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC VFM_CacheClean DONE ");
	}
	else
	{
		OstTraceFiltInst0(TRACE_ERROR, "PARSER_MPC VFM_CacheClean FAIL as memorycontext is EMPTY ");
	}

	OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC after calling VFM_CacheClean");
#endif

	OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: Abt to call emptythisbufferSharedBufIn.emptyThisBuffer");
	emptythisbufferSharedBufIn.emptyThisBuffer(pBuffer);
	isInputBufferDequeued = OMX_FALSE;
	#ifdef __PERF_MEASUREMENT
	write_ticks_to_buffer("\nReleased Bitstream Buffer");
	#endif

}

void jpegdec_arm_nmf_parser_mpc::updateDynamicConfigurationStructure(){
	t_uint16 atomic_write;
	atomic_write = m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_entry + 1;
	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_entry = atomic_write;
	OstTraceInt0(TRACE_API, "PARSER_MPC: updateDynamicConfigurationStructure( \n");
//	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_width = (mParam.cropWindowWidth+ 0xf)&0xfff0;
//	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_height = (mParam.cropWindowHeight+ 0xf)&0xfff0;

	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_width = mParam.cropWindowWidth;
	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.window_height = mParam.cropWindowHeight;

	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.vertical_offset = mParam.vertical_offset;
	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.horizontal_offset =mParam.horizontal_offset;
	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set.downsampling_factor =mParam.downsamplingFactor;

	m_ps_ddep_sdc_jpeg_dynamic_configuration->host_param_set_atomic_exit = atomic_write;
}

void jpegdec_arm_nmf_parser_mpc::updateParamAndConfigCrop(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig){
	mParam.cropWindowWidth = pDynamicConfig->window_width;
	mParam.cropWindowHeight  = pDynamicConfig->window_height;
	mParam.horizontal_offset = pDynamicConfig->horizontal_offset;
	mParam.vertical_offset = pDynamicConfig->vertical_offset;
	OstTraceInt0(TRACE_API, "PARSER_MPC: updateParamAndConfigCrop( \n");
}

void jpegdec_arm_nmf_parser_mpc::updateParamAndConfigScale(ts_ddep_sdc_jpeg_dynamic_params *pDynamicConfig){
	OstTraceInt0(TRACE_API, "PARSER_MPC: updateParamAndConfigScale\n");
	mParam.downsamplingFactor = (JPEGDec_Proxy_Downsampling_Factor)pDynamicConfig->downsampling_factor;
}


OMX_ERRORTYPE jpegdec_arm_nmf_parser_mpc::checkConfAndParams(){
	if(!(mParam.frameWidth>0 && mParam.frameWidth<=8176))
		return OMX_ErrorUnsupportedSetting;

	if(!(mParam.frameHeight>0 && mParam.frameHeight<=8176))
		return OMX_ErrorUnsupportedSetting;

    if(!(mParam.downsamplingFactor ==1
	   || mParam.downsamplingFactor ==2
	   || mParam.downsamplingFactor== 4
	   || mParam.downsamplingFactor == 8))
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: Returning OMX_ErrorUnsupportedSetting from checkConfAndParams \n");
		 return OMX_ErrorUnsupportedSetting;
	}

	if(mParam.nbComponents>MAX_NUMBER_COMPONENTS)
	{
		OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: Returning OMX_ErrorUnsupportedSetting from checkConfAndParams \n");
	   return OMX_ErrorUnsupportedSetting;
   }

   if(mParam.nbScanComponents>MAX_NUMBER_COMPONENTS)
   {
	   OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: Returning OMX_ErrorUnsupportedSetting from checkConfAndParams \n");
	   return OMX_ErrorUnsupportedSetting;
   }

   for(t_uint32 cnt =0;cnt<mParam.nbComponents;cnt++ ){
		t_uint32 h_factor, v_factor;
		h_factor = mParam.samplingFactors[cnt].hSamplingFactor;
		v_factor = mParam.samplingFactors[cnt].vSamplingFactor;

		if(h_factor<1 || h_factor >4 || h_factor==3)
		{
			OstTraceFiltInst1(TRACE_FLOW, "PARSER_MPC: Returning OMX_ErrorUnsupportedSetting from checkConfAndParams line no : %d \n",__LINE__);
			return OMX_ErrorUnsupportedSetting;
		}

		if(v_factor<1 || v_factor >4 || v_factor==3)
		{
			OstTraceFiltInst1(TRACE_FLOW, "PARSER_MPC: Returning OMX_ErrorUnsupportedSetting from checkConfAndParams line no : %d \n",__LINE__);
			return OMX_ErrorUnsupportedSetting;
		}
    }

   if(mParam.mode ==1)
   {
	   OstTraceFiltInst1(TRACE_FLOW, "PARSER_MPC: Returning OMX_ErrorUnsupportedSetting from checkConfAndParams line no : %d \n",__LINE__);
	   return OMX_ErrorUnsupportedSetting;
   }

	if(mParam.mode==1){
        if(mParam.startSpectralSelection>63){
			OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: checkConfAndParams unsupported start spectral selection\n");
			return OMX_ErrorUnsupportedSetting;
		}
		if(mParam.endSpectralSelection!=63){
			OstTraceFiltInst0(TRACE_FLOW, "PARSER_MPC: checkConfAndParams unsupported end spectral selection\n");
			return OMX_ErrorUnsupportedSetting;
		}
	}
	else{
		if(mParam.startSpectralSelection!=0)
			return OMX_ErrorUnsupportedSetting;
		if(mParam.endSpectralSelection!=63)
			return OMX_ErrorUnsupportedSetting;
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE jpegdec_arm_nmf_parser_mpc::configureCoeffAndLineBuffer(){
	t_uint8 *ptr;
	t_uint32 lineBufferSize=0,coeffBufferSize=0;
	ts_t1xhv_vdc_frame_buf_out *p_out_frame_buffer;
	ts_ddep_sdc_jpeg_param_desc *ps_ddep_sdc_jpeg_param_desc;
	t_uint16 width,height;
	OstTraceFiltInst0(TRACE_API, "PARSER_MPC: Inside configureCoeffAndLineBuffer\n");
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
		OstTraceFiltInst1(TRACE_FLOW, "PARSER_MPC: For PROGRESSIVE_JPEG line no : %d \n",__LINE__);

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

	return OMX_ErrorNone;
}

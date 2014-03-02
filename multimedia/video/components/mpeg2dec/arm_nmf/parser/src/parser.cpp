/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "mpeg2dec/arm_nmf/parser.nmf"
#include <string.h>
#include "SharedBuffer.h"
//#include "mpc/common/buffer.idt"
#include "host_m2d_parser.h"
//#include <stdio.h>
#ifdef __PERF_MEASUREMENT
#include "../../../proxy/test_arm/osi_perf.h"
#endif

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
	#ifdef OST_TRACE_COMPILER_IN_USE
	#include "video_components_mpeg2dec_arm_nmf_parser_src_parserTraces.h"
#endif //for OST_TRACE_COMPILER_IN_USE

mpeg2dec_arm_nmf_parser::mpeg2dec_arm_nmf_parser(){
	OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser_Constructor");
	compState = OMX_StateMax;

	for(int i=0;i<4;i++){
		OMX_BUFFERHEADERTYPE tmp;
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
	updateData = OMX_TRUE;

    errorMapReportingFlag = 1;
	mProxyErrorMapBuffer = 0;

	frame_info_width = 0;
	frame_info_height = 0;
	config_concealmentflag = 0;

	memset(&mpeg2StreamParams,0x0,sizeof(ts_mp2));
	OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser_Constructor");
}
        
mpeg2dec_arm_nmf_parser::~mpeg2dec_arm_nmf_parser(){
	OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser_Destructor");
	OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser_Destructor");
}

t_nmf_error METH(construct)(void){
	OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::Construct");
	OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::Construct");
	return NMF_OK;
}

void METH(destroy)(){
	OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::destroy");
	OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::destroy");
}

void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
	OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::setTunnelStatus");
	OstTraceInt2(TRACE_FLOW,"PortIndex:%d,isTunneled:%d",portIdx,isTunneled);
	int i=0;
	if (portIdx == -1){
        for (i=0;i<2;i++){
            if (isTunneled & (1<<i)){
                mPorts[i].setTunnelStatus(true);
			}
            else{
                mPorts[i].setTunnelStatus(false);
			}
        }
    }
    else
    {
        if (isTunneled & (1<<portIdx)){
			mPorts[portIdx].setTunnelStatus(true);
        }
        else{
			mPorts[portIdx].setTunnelStatus(false);
        }
    }
	OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::setTunnelStatus");
}

void METH(fsmInit)(fsmInit_t fsm_init){
	OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::fsmInit");
	OMX_BUFFERHEADERTYPE * mBufIn_list[INPUT_BUFFER_COUNT] ;
	OMX_BUFFERHEADERTYPE * mBufOut_list[OUTPUT_BUFFER_COUNT];

	for(int i=0;i<INPUT_BUFFER_COUNT;i++)
		mBufIn_list[i] = &mBufIn[i] ;

	for(int i=0;i<OUTPUT_BUFFER_COUNT;i++)
		mBufOut_list[i] = &mBufOut[i] ;
	
    if (fsm_init.traceInfoAddr){
        setTraceInfo(fsm_init.traceInfoAddr, fsm_init.id1); 
		 mParser.setTraceInfo(fsm_init.traceInfoAddr, fsm_init.id1); 
	}  

	//Input port can keep buffers till complete data is not available
    mPorts[0].init(InputPort,false,true,NULL,mBufIn_list,INPUT_BUFFER_COUNT,&inputport,0,(fsm_init.portsDisabled&1),(fsm_init.portsTunneled&1),this);
    mPorts[1].init(OutputPort,false,true,NULL,mBufOut_list,OUTPUT_BUFFER_COUNT,&outputport,1,((fsm_init.portsDisabled>>1)&1), (fsm_init.portsTunneled&1),this);

    init(2, mPorts, &proxy, &me, 0); //FIXME:invalid parameter is to be initilaized properly.
	OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::fsmInit");
}

void METH(disablePortIndication)(t_uint32 portIdx){
	OstTraceFiltInst1(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::disablePortIndication Port:%d",portIdx);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::disablePortIndication");
} 

void METH(enablePortIndication)(t_uint32 portIdx){
	OstTraceFiltInst1(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::enablePortIndication Port:%d",portIdx);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::destroy");
} 

void METH(flushPortIndication)(t_uint32 portIdx){
	OstTraceFiltInst1(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::flushPortIndication Port:%d",portIdx);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::flushPortIndication");
}
 
void METH(reset)(){
	OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::reset");
	OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::reset");
}  

void mpeg2dec_arm_nmf_parser::processEvent(void){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::processEvent");
	Component::processEvent() ;
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::processEvent");
}

void mpeg2dec_arm_nmf_parser::fromProcComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::fromProcComp_emptyThisBuffer");
	OstTraceFiltInst1(TRACE_FLOW,"Received input buffer from processing component:%x",(OMX_U32)pBuffer);
	Component::deliverBuffer(0, pBuffer);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::fromProcComp_emptyThisBuffer");
}

void mpeg2dec_arm_nmf_parser::fromProcComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){\
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::fromProcComp_fillThisBuffer");
	OstTraceFiltInst1(TRACE_FLOW,"Received output buffer from processing component:%x",(OMX_U32)pBuffer);
	Component::deliverBuffer(1, pBuffer);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::fromProcComp_fillThisBuffer");

}

void mpeg2dec_arm_nmf_parser::fromSharedBufferComp_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::fromSharedBufferComp_emptyThisBuffer");
	OstTraceFiltInst1(TRACE_FLOW,"Received output buffer from DDep:%x",(OMX_U32)pBuffer);
	OMX_U32 flags = pBuffer->nFlags;
	//Return Image buffer to proxy
	Component::returnBufferAsync(1,pBuffer);

	if(flags & OMX_BUFFERFLAG_EOS) {
		OstTraceFiltInst0(TRACE_FLOW,"Raising EOS event");
		proxy.eventHandler(OMX_EventBufferFlag, 1, flags);
	}
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::fromSharedBufferComp_emptyThisBuffer");

}

void mpeg2dec_arm_nmf_parser::fromSharedBufferComp_fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::fromSharedBufferComp_fillThisBuffer");
	OstTraceFiltInst1(TRACE_FLOW,"Received input buffer from DDep:%x",(OMX_U32)pBuffer);
	//Return Bitstream buffer to proxy
	Component::returnBufferAsync(0,pBuffer);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::fromSharedBufferComp_fillThisBuffer");
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword parameter){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::sendCommand");
	OstTraceFiltInst2(TRACE_FLOW,"Command:%d, Parameter:%d",cmd,parameter);
	Component::sendCommand(cmd, parameter) ;
	// This sendcommand is called here to keep it sequentially after parser's sendcommand call.
	// This prevents situations like flush in which ddep processes sendcommand first & parser send a buffer
	// to ddep. After sending buffer to ddep parser processes the sendcommand. Refer to mpeg4dec seek issue.
	iDdep.sendCommand_sequential(cmd,parameter);
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::sendCommand");
}


void METH(fillThisHeader)(Buffer_p pHeader){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::fillThisHeader");
	OstTraceFiltInst1(TRACE_FLOW,"Received header from DDep:%x",(OMX_U32)pHeader);
	freeHeaderDesc((t_uint32)pHeader);
	clearAllPendingRequestsToFreeHeader();
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::fillThisHeader");
}

void METH(sendportSettings)(t_uint32 width,t_uint32 height,t_uint32 concealment_flag){
	OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::sendportSettings");
	OstTraceInt3(TRACE_FLOW,"sendportSettings width=%d height=%d concealment_flag=%d",width,height,concealment_flag);
	frame_info_width = width;
	frame_info_height = height;
	config_concealmentflag = concealment_flag;
	OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::sendportSettings");
}

void METH(computeARMLoad)(t_bool disable){
	OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::computeARMLoad");
	iDdep.disableFWCodeexection(disable);
	OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::computeARMLoad");
}

void METH(memoryRequirements)(t_uint16 mChannelId,ts_ddep_buffer_descriptor paramBufferDesc,
							  ts_ddep_buffer_descriptor linkListBufferDesc,ts_ddep_buffer_descriptor debugBufferDesc,
							  t_uint32 outputPortBufferCountActual,
							  t_uint8 errorMapFlag,t_uint16 *proxyErrorMapBuffer)
{
	OstTraceInt0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::memoryRequirements");
	OMX_U8 index;
	//Store Memory Descriptors and initialize pointers
	mParamBufferDesc = paramBufferDesc;
    mLinkListBufferDesc = linkListBufferDesc;
    mDebugBufferDesc = debugBufferDesc;

	errorMapReportingFlag = errorMapFlag;
	mProxyErrorMapBuffer = proxyErrorMapBuffer;

    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++)
    {
        mHeaderDesc[index].isFree = OMX_TRUE;
        mHeaderDesc[index].isRequestedToFree = OMX_FALSE;
        mHeaderDesc[index].pHeader = (ts_ddep_vdc_mpeg2_header_desc*)(paramBufferDesc.nLogicalAddress 
                                                             + sizeof(ts_ddep_vdc_mpeg2_param_desc)
															 + index* sizeof(ts_ddep_vdc_mpeg2_header_desc));
		mHeaderDesc[index].pHeader->headerBufferMpcAddress = (OMX_U32)(mParamBufferDesc.nMpcAddress
															 + sizeof(ts_ddep_vdc_mpeg2_param_desc)/2
															 + index* sizeof(ts_ddep_vdc_mpeg2_header_desc)/2);

		mHeaderDesc[index].pHeader->physicalAddressParamIn = ENDIANESS_CONVERSION((OMX_U32)(mParamBufferDesc.nPhysicalAddress
															 + sizeof(ts_ddep_vdc_mpeg2_param_desc)
															 + index* sizeof(ts_ddep_vdc_mpeg2_header_desc)));
	}

	//Provide memories to MPC 
    iDdep.setNeeds(mChannelId,mLinkListBufferDesc,mDebugBufferDesc);
	iDdep.setParameter(mChannelId,ID_VDC_MPEG2,mParamBufferDesc);
	iDdep.setConfig(mChannelId,outputPortBufferCountActual,(t_uint8)errorMapFlag);
	OstTraceInt0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::memoryRequirements");
}


void METH(process)(){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::process");

	if(mPorts[1].queuedBufferCount()){
		OMX_BUFFERHEADERTYPE *bufOut;
		bufOut = mPorts[1].dequeueBuffer();
		OstTraceFiltInst1(TRACE_FLOW,"Image buffer available on output bufOut:%x",(OMX_U32)bufOut);
		fillthisbufferSharedBufOut.fillThisBuffer(bufOut);
	}

	processBitstreamBuffer();
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::process");
}

void mpeg2dec_arm_nmf_parser:: processBitstreamBuffer()
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::processBitstreamBuffer");
	if(mPorts[0].queuedBufferCount()){
		t_uint16 endOfBitstream =0;
		OMX_BUFFERHEADERTYPE *pBuffer = mPorts[0].dequeueBuffer();
		OstTraceFiltInst1(TRACE_FLOW,"Bitstream buffer available at input port pBuffer:%x",(OMX_U32)pBuffer);

		// add an empty byte to help SVA to detect end of NAL
		if (pBuffer->nFilledLen == 0){
			if(pBuffer->nFlags & OMX_BUFFERFLAG_EOS) {
				OstTraceFiltInst0(TRACE_FLOW,"Empty bitstream buffer received with EOS");
				if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS){
					memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 0), 0x00, 2);
					memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 2), 0x01, 1);
					memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 3), 0xB7, 1);
				}
				else{
					memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 0), 0x00, 2);
					memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 2), 0x01, 1);
					memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 3), 0xB0, 1);
				}
				
				memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 4), 0x00, 2);
				memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 6), 0x01, 1);
				memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 7), 0xB0, 1);
				OstTraceFiltInst1(TRACE_FLOW,"Abt to push input buffer to MPC at line : %d",__LINE__);
				emptythisbufferSharedBufIn.emptyThisBuffer(pBuffer);
			}
			else{
				mPorts[0].returnBuffer(pBuffer);
			}
		}
		else{
			// add a second SEQUENCE_END_CODE to the bitstream in order to prevent first
			// to be fed during decoding
			if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS){
				OstTraceFiltInst0(TRACE_FLOW,"Filled  bitstream buffer received with EOS");
				memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 0), 0x00, 2);
				memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 2), 0x01, 1);
				memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 3), 0xB7, 1);
			}
			else{
				memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 0), 0x00, 2);
				memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 2), 0x01, 1);
				memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 3), 0xB0, 1);
			}

			memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 4), 0x00, 2);
			memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 6), 0x01, 1);
			memset ( (void *)((pBuffer->pBuffer+pBuffer->nOffset+pBuffer->nFilledLen) + 7), 0xB0, 1);
			
			// create the nal, and add it to the queue of buffers that are being processed
			t_bit_buffer btstBuffer ;
            memset(&btstBuffer,0,sizeof(btstBuffer));

			btstBuffer.addr  = (OMX_U8 *)pBuffer->pBuffer + pBuffer->nOffset;
			btstBuffer.os    = 0;
			btstBuffer.start = btstBuffer.addr;
			btstBuffer.end   = (t_uint8*) ((t_uint32)(btstBuffer.addr) + pBuffer->nFilledLen + 8);
			btstBuffer.inuse = 0;
			btstBuffer.next  = 0;
                        
                        OstTraceFiltInst3(TRACE_FLOW,"NEW value of pBuffer->pBuffer 0x%x pBuffer->nOffset 0x%x btstBuffer.addr 0x%x",(OMX_U32)pBuffer->pBuffer,(OMX_U32)pBuffer->nOffset,(OMX_U32)btstBuffer.addr);
			mpeg2StreamParams.picture_coding_type = 0;
			OstTraceFiltInst0(TRACE_FLOW,"Sending buffer to parser");
			endOfBitstream = mParser.VAL_ParseMp2NextFrame(&mpeg2StreamParams, &btstBuffer);
			if(!endOfBitstream) {
				updateData = OMX_FALSE;
				// removed for removing warnings
				//t_dmpeg2ErrorCode error = fillHeader(pBuffer,&btstBuffer);
				fillHeader(pBuffer,&btstBuffer);
				detectPortSettingsAndNotify();
			}
			else{
				// If buffer contains only sequence end code and it is end of sequence
				if(!mpeg2StreamParams.picture_coding_type) 
					pBuffer->nFilledLen = 0;
				else{
					updateData = OMX_FALSE;
					//removed for removing warnings
					//t_dmpeg2ErrorCode error = fillHeader(pBuffer,&btstBuffer);
					fillHeader(pBuffer,&btstBuffer);
					detectPortSettingsAndNotify();
				}
			}
			OstTraceFiltInst1(TRACE_FLOW,"Pushing input buffer to MPC DDEP : 0x%x",(OMX_U32)pBuffer);
			if (pBuffer->nFlags & OMX_BUFFERFLAG_CODECCONFIG)
			{
				OstTraceInt0(TRACE_FLOW,"Got frame with codec configuration data, making nfilled len to ZERO");
				pBuffer->nFilledLen = 0;
			}
			emptythisbufferSharedBufIn.emptyThisBuffer(pBuffer);
		}
	}
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::processBitstreamBuffer");
}

void mpeg2dec_arm_nmf_parser::set_pJdcIn_parameters(ts_t1xhv_vdc_mpeg2_param_in *pIn_parameters, ts_mp2 *btst_mp2)
{
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::set_pJdcIn_parameters");
	memset(pIn_parameters,0x0,sizeof(ts_t1xhv_vdc_mpeg2_param_in));
    pIn_parameters->vertical_size      = btst_mp2->vertical_size;  
    pIn_parameters->mb_width           = btst_mp2->mb_width;    
    pIn_parameters->mb_height          = btst_mp2->mb_height;

    memcpy (pIn_parameters->intra_quantizer_matrix,
            btst_mp2->coded_intra_quantizer_matrix,     sizeof (t_ushort_value) * 64);

    memcpy (pIn_parameters->non_intra_quantizer_matrix,
            btst_mp2->coded_non_intra_quantizer_matrix, sizeof (t_ushort_value) * 64);

    pIn_parameters->picture_coding_type        = btst_mp2->picture_coding_type;
    pIn_parameters->full_pel_forward_vector    = btst_mp2->full_pel_forward_vector;
    pIn_parameters->forward_f_code             = btst_mp2->forward_f_code;
    pIn_parameters->full_pel_backward_vector   = btst_mp2->full_pel_backward_vector;
    pIn_parameters->backward_f_code            = btst_mp2->backward_f_code;
    pIn_parameters->f_code_0_0                 = btst_mp2->f_code[0][0];
    pIn_parameters->f_code_0_1                 = btst_mp2->f_code[0][1];
    pIn_parameters->f_code_1_0                 = btst_mp2->f_code[1][0];
    pIn_parameters->f_code_1_1                 = btst_mp2->f_code[1][1];
    pIn_parameters->intra_dc_precision         = btst_mp2->intra_dc_precision;
    pIn_parameters->picture_structure          = btst_mp2->picture_structure;
    pIn_parameters->top_field_first            = btst_mp2->top_field_first;
    pIn_parameters->frame_pred_frame_dct       = btst_mp2->frame_pred_frame_dct;
    pIn_parameters->concealment_motion_vectors = btst_mp2->concealment_motion_vectors;
    pIn_parameters->q_scale_type               = btst_mp2->q_scale_type;
    pIn_parameters->intra_vlc_format           = btst_mp2->intra_vlc_format;
    pIn_parameters->alternate_scan             = btst_mp2->alternate_scan;
    pIn_parameters->scalable_mode              = btst_mp2->scalable_mode;
    pIn_parameters->MPEG2_Flag                 = btst_mp2->MPEG2_Flag;
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::set_pJdcIn_parameters");
}


//Header buffer to be used

void mpeg2dec_arm_nmf_parser::initHeaderDescriptors(){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::initHeaderDescriptors");
	OMX_U8 index;
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
		mHeaderDesc[index].pHeader = NULL;
		mHeaderDesc[index].isFree = OMX_TRUE;
		mHeaderDesc[index].isRequestedToFree = OMX_FALSE;
    }
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::initHeaderDescriptors");
}

void mpeg2dec_arm_nmf_parser::clearAllPendingRequestsToFreeHeader(){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::clearAllPendingRequestsToFreeHeader");
    OMX_U8 index;
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
        if(mHeaderDesc[index].isRequestedToFree == OMX_TRUE){
			mHeaderDesc[index].isRequestedToFree = OMX_FALSE;
			mHeaderDesc[index].isFree = OMX_TRUE;
        }
	}
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::clearAllPendingRequestsToFreeHeader");
}

ts_ddep_vdc_mpeg2_header_desc* mpeg2dec_arm_nmf_parser::getFreeHeaderDesc(){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::getFreeHeaderDesc");
    OMX_U8 index;
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
        if(mHeaderDesc[index].isFree == OMX_TRUE){
            mHeaderDesc[index].isFree = OMX_FALSE;
			OstTraceFiltInst1(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::getFreeHeaderDesc header:%x",(OMX_U32)mHeaderDesc[index].pHeader);
            return (ts_ddep_vdc_mpeg2_header_desc*)mHeaderDesc[index].pHeader;
        }
    }
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::getFreeHeaderDesc");
    return NULL;
}

void mpeg2dec_arm_nmf_parser::freeHeaderDesc(t_uint32 pBuffer){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::freeHeaderDesc");
	OMX_U8 index;
    for(index=0;index<VFM_HEADER_FIFO_COUNT;index++){
        if(mHeaderDesc[index].pHeader->headerBufferMpcAddress == pBuffer){
			mHeaderDesc[index].isFree = OMX_TRUE;
            mHeaderDesc[index].isRequestedToFree = OMX_TRUE;

			if(errorMapReportingFlag){
               for(OMX_U8 i=0;i<225;i++)
				   *(mProxyErrorMapBuffer+i)|= mHeaderDesc[index].pHeader->errormap[i];
			}

            break;
        }
    }
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::freeHeaderDesc");
}


void mpeg2dec_arm_nmf_parser::setBitOffset(ts_ddep_vdc_mpeg2_header_desc *pHeader,t_bit_buffer *pBtstBuffer){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::setBitOffset");
	//pHeader->bitOffsetAddress = ENDIANESS_CONVERSION((t_uint32)pBtstBuffer->addr);
	pHeader->bitOffsetAddress = ENDIANESS_CONVERSION((t_uint32)(pBtstBuffer->addr-pBtstBuffer->start));
	OstTraceFiltInst3(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::setBitOffset value of bitoffset address 0x%x current 0x%x start 0x%x",(OMX_U32)pHeader->bitOffsetAddress,(OMX_U32)pBtstBuffer->addr,(OMX_U32)pBtstBuffer->start);
}


t_dmpeg2ErrorCode mpeg2dec_arm_nmf_parser::fillHeader(OMX_BUFFERHEADERTYPE *pBuffer,t_bit_buffer *pBtstBuffer){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::fillHeader");
	ts_ddep_vdc_mpeg2_header_desc *headerBuf= getFreeHeaderDesc();
	if(headerBuf){
        if (!updateData){
			SharedBuffer *sharedBuf = (SharedBuffer *) pBuffer->pInputPortPrivate;
			Buffer_p buf;
			OMX_ERRORTYPE error = OMX_ErrorNone;
            buf = (Buffer_p)sharedBuf->getMPCHeader();
			//Check Whether parsed configuration is compatible with FW
			error = mParser.check_mp2_input_parameters(&mpeg2StreamParams);
			if(error){
				freeHeaderDesc((t_uint32)headerBuf);
				proxy.eventHandler(OMX_EventError,(OMX_U32)error,0);
			}
			
			headerBuf->buffer_p=ENDIANESS_CONVERSION(buf);
			set_pJdcIn_parameters(&(headerBuf->s_in_parameters),&mpeg2StreamParams);
			setBitOffset(headerBuf,pBtstBuffer);

			// Reset error map buffer
			if(errorMapReportingFlag){
				memset(headerBuf->errormap,0,225*sizeof(OMX_U16)); //size of 720p
				//In case of I Frame we reset cummulative error map also as
				//specified in OpenMax spec
				if((headerBuf->s_in_parameters).picture_coding_type==1)
					memset(mProxyErrorMapBuffer,0,225*sizeof(OMX_U16)); //size of 720p
			}
                        OstTraceFiltInst1(TRACE_FLOW,"Pushing header buffer to MPC DDEP 0x%x",(OMX_U32)headerBuf->headerBufferMpcAddress);
			emptythisheader.emptyThisHeader((Buffer_p)(headerBuf->headerBufferMpcAddress));
			updateData = OMX_TRUE;;
        }
    }
	else
		return DMPEG2_MALLOC_ERROR;

	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::fillHeader");
	return DMPEG2_NO_ERROR;
}

void mpeg2dec_arm_nmf_parser::detectPortSettingsAndNotify(){
	OstTraceFiltInst0(TRACE_FLOW,"Enter mpeg2dec_arm_nmf_parser::detectPortSettingsAndNotify");
	OMX_BOOL has_changed = OMX_FALSE;
	OstTraceFiltInst0(TRACE_FLOW, "=> Mpeg4dec_Parser: detectPortSettingsAndNotify()");
	OstTraceFiltInst2(TRACE_FLOW, "detectPortSettingsAndNotify frame_info_height (%d) mpeg2StreamParams.vertical_size (%d)",frame_info_height,mpeg2StreamParams.vertical_size);
    mpeg2StreamParams.vertical_size = ((mpeg2StreamParams.vertical_size  + 0xF) & (~0xF));
    mpeg2StreamParams.horizontal_size = ((mpeg2StreamParams.horizontal_size  + 0xF) & (~0xF));

    if(frame_info_height != mpeg2StreamParams.vertical_size){
        frame_info_height = mpeg2StreamParams.vertical_size;
        OstTraceFiltInst0(TRACE_FLOW, "port setting change event generated::frame_info_height\n");
		has_changed = OMX_TRUE;
	}

	if(frame_info_width != mpeg2StreamParams.horizontal_size){
		frame_info_width = mpeg2StreamParams.horizontal_size;
		OstTraceFiltInst0(TRACE_FLOW, "port setting change event generated::frame_info_width\n");
        has_changed = OMX_TRUE;
	}

	if(has_changed){
        has_changed = OMX_FALSE;
		OstTraceFiltInst0(TRACE_FLOW,"Raising port settings changed event");
		iChangePortSettingsSignal.portSettings(mpeg2StreamParams.horizontal_size,mpeg2StreamParams.vertical_size);   
    }
	OstTraceFiltInst0(TRACE_FLOW,"Exit mpeg2dec_arm_nmf_parser::detectPortSettingsAndNotify");
}


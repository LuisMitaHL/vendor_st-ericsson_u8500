/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
// Include self generated NMF header file for this component
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef __MPEG4DEC_ARM_NMF
	#include <mpeg4dec/arm_nmf/ddep.nmf>
	#include <ddep.hpp>
	#include "VFM_Memory.h"

	//+ER352606
	#define DECODING_EVENT 1
	#define DEBLOCKING_EVENT 2
	//-ER352606
    	
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_mpeg4dec_arm_nmf_ddep_src_ddepTraces.h"
#endif
#else
    #include <mpeg4dec/mpc/ddep.nmf>
	#include <inc/archi-wrapper.h>
	#include <fsm/component/include/Component.inl>
    #include "vfm_vdc_mpeg4.idt"
	#include <ddep.h>

   
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_mpeg4dec_mpc_ddep_src_ddepTraces.h"
#endif

static void process(Component *this);
static void processActual(Component *this);
static void release_resources(void);
static void mp4d_fill_link_structure(Buffer_p);
static void mp4d_mark_end_link_structure();
#endif
#ifdef __MPEG4DEC_ARM_NMF
	#define BUFFERFLAG_EOS OMX_BUFFERFLAG_EOS
	#define BUFFERFLAG_ENDOFRAME OMX_BUFFERFLAG_ENDOFFRAME
	#define BUFFERFLAG_DECODEONLY OMX_BUFFERFLAG_DECODEONLY
#endif
#ifdef __MPEG4DEC_ARM_NMF
void mpeg4dec_arm_nmf_ddep::processEvent(void)
{
	OstTraceFiltInst0(TRACE_FLOW, "processEvent \n");
        Component::processEvent();
}

void mpeg4dec_arm_nmf_ddep::emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
{
	//printf("\n DDEP_ARM::emptyThisBuffer enter");
	OstTraceFiltInst0(TRACE_FLOW, "emptyThisBuffer \n");
	Component::deliverBuffer(0, pBuffer);
	//printf("\n DDEP_ARM::emptyThisBuffer exit");
}

void mpeg4dec_arm_nmf_ddep::fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
{
	//printf("\n DDEP_ARM::fillThisBuffer enter");
	OstTraceFiltInst0(TRACE_FLOW, "fillThisBuffer \n");
    Component::deliverBuffer(1, pBuffer);
	//printf("\n DDEP_ARM::fillThisBuffer exit");
}

void mpeg4dec_arm_nmf_ddep::emptyThisHeader(t_uint32 buf)
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec_Ddep: emptyThisHeader()");
	Component::deliverBuffer(2, (OMX_BUFFERHEADERTYPE_p)buf);

//+ER 354962
#ifdef __MPEG4DEC_ARM_NMF
	iCommandAck.pendingCommandAck();
#endif
//-ER 354962
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec_Ddep: emptyThisHeader()");
}

mpeg4dec_arm_nmf_ddep::mpeg4dec_arm_nmf_ddep()
{
		t_uint32 i;
		OstTraceInt0(TRACE_FLOW, "constructor called for ddep \n");
	    for(i=0;i<10;i++)
	    reference_array[i]=0; // storing dblk param buffer and reference
		for(i=0;i<4;i++)
		array_flags[i]=0;
		free_references = 0;  //required to remove  header buffer if we get flush command before end algo has been handled and also for EOS
        eos_flag_set = 0;
		defer_pause = 0;
        get_hw_resource = 0;
		header_port_enabled = 1;
		get_hw_resource_requested = 0;
		DataInBuf=0;
		MaxBufInput=0;
		NoOfCopiedBuffers=0;
        for(i=0;i<INPUT_BUFFER_COUNT;i++)
		{
			InputBufferArray[i]=0;
			bufIn[i]=0;
        }
		CopyOperationOngoing=0;
		BytesWrittenInInternalBuffer=0;
		ReadyToProgram=0;
		//+ER344943
		decoding_complete=0;
		deblocking_complete=0;
		//-ER344943
		bufOut=0;
		bufheader=0;																	//
		currentCodingType=0;
		currentVopCoded=0;
		frameWidth=0;
		frameHeight=0;
		compState = OMX_StateIdle;
		fillThisBufferCount = 0;  		//debugging purpose
		emptyThisBufferCount = 0;		//debugging purpose
		emptyThisHeaderCount = 0;		//debugging purpose
		fillThisBufferDoneCount = 0;	//debugging purpose
		emptyThisBufferDoneCount = 0;	//debugging purpose
		emptyThisHeaderDoneCount = 0; 	//debugging purpose
		Error_Reporting    = 0; //check whether needed or not
		get_vpp_resource = 0;  // to be fixed
		firstFramedecoded = 0;
			// to be reset somewhere
		WAIT_FOR_BUFFER=0;
       fakeBufIn = (OMX_BUFFERHEADERTYPE_p)&fakeBufferDesc;
	   InternalbufIn = (OMX_BUFFERHEADERTYPE_p)&InternalBufferDesc;
       bsm.index_to_next_not_required_bitstream_buffer=0;
	   bsm.current_bitstream_buffer_index=0;
	   bsm.last_index_ps_ddep_bitstream_buf_link_and_header=0;
	   bsm.physical_ps_ddep_bitstream_buf_link_and_header=0;
	   bsm.ps_ddep_bitstream_buf_link_and_header=0;
	   dblk_dpb_buffer.B_buffer_index=0;
	   dblk_dpb_buffer.buffout_reference=0;
	   dblk_dpb_buffer.bw_reference_buffer=0;
	   dblk_dpb_buffer.IP_buffer_index=0;
	   dblk_dpb_buffer.last_coding_type=0;
	   dblk_dpb_buffer.Last_vop_not_coded=0;

	   dpb_buffer.buffer_copied=0;
	   for(t_uint8 i=0;i<3;i++)
	   dpb_buffer.bw_reference_buffer[i]=0;
	   dpb_buffer.curr_bw_buffer_index=0;
	   dpb_buffer.curr_fw_buffer_index=0;
	   dpb_buffer.dpb_full=0;
	   dpb_buffer.dpb_IP_full=0;
	   for(t_uint8 i=0;i<2;i++)
	   dpb_buffer.fw_reference_buffer[i]=0;
	   dpb_buffer.last_coding_type=0;

	   //param.addr_header_buffer=0;
	   param.addr_in_bitstream_buffer=0;
	   param.addr_in_frame_buffer=0;
	   param.addr_in_frame_parameters=0;
	   param.addr_in_parameters=0;
	   param.addr_internal_buffer=0;
	   param.addr_out_bitstream_buffer=0;
	   param.addr_out_bitstream_buffer=0;
	   param.addr_out_bitstream_buffer=0;
	   param.addr_out_frame_buffer=0;
	   param.addr_out_frame_parameters=0;
	   param.addr_out_parameters=0;
	   param.algoId=(t_t1xhv_algo_id)0;
	   param.ready=0;

	   s_mpeg4d_ddep_desc.bbm_desc.nLogicalAddress=0;
	   s_mpeg4d_ddep_desc.bbm_desc.nMpcAddress=0;
	   s_mpeg4d_ddep_desc.bbm_desc.nPhysicalAddress=0;
	   s_mpeg4d_ddep_desc.bbm_desc.nSize=0;
	   s_mpeg4d_ddep_desc.s_debug_buffer_desc.nLogicalAddress=0;
	   s_mpeg4d_ddep_desc.s_debug_buffer_desc.nMpcAddress=0;
	   s_mpeg4d_ddep_desc.s_debug_buffer_desc.nPhysicalAddress=0;
	   s_mpeg4d_ddep_desc.s_debug_buffer_desc.nSize=0;
}
mpeg4dec_arm_nmf_ddep::~mpeg4dec_arm_nmf_ddep()
{
	OstTraceFiltInst0(TRACE_FLOW, "destructor called for ddep \n");
}

t_nmf_error mpeg4dec_arm_nmf_ddep::construct(void)
{
	OstTraceFiltInst0(TRACE_FLOW, "construct\n");
	//printf("\n DDEP_ARM::construct enter");
	return NMF_OK;
}

void mpeg4dec_arm_nmf_ddep::destroy()
{
	OstTraceFiltInst0(TRACE_FLOW, "destroy\n");
}

OMX_BOOL mpeg4dec_arm_nmf_ddep::buffer_available_atinput()
{
	return OMX_TRUE;
	
}


OMX_BOOL mpeg4dec_arm_nmf_ddep::buffer_available_atoutput()
{
	if((bufOut==NULL) && (mPorts[1].queuedBufferCount()))
	{
		OstTraceFiltInst0(TRACE_FLOW, "buffer_available_atoutput \n");
        bufOut = mPorts[1].dequeueBuffer();
		return OMX_TRUE;
	}
	else if(bufOut)
		return OMX_TRUE;
	else
		return OMX_FALSE;

}

OMX_BOOL mpeg4dec_arm_nmf_ddep::buffer_available_atheaderport()
{
	if((bufheader==NULL) && (mPorts[2].queuedBufferCount()))
	{
		OstTraceFiltInst0(TRACE_FLOW, "buffer_available_atheaderport \n");
        bufheader = mPorts[2].dequeueBuffer();
		return OMX_TRUE;
	}
	else if(bufheader)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

#else
PUT_PRAGMA
void METH(start)()
{
	OstTraceFiltInst0(TRACE_FLOW, "start \n");
}

PUT_PRAGMA
void METH(processEvent)()
{
	OstTraceFiltInst0(TRACE_FLOW, "processEvent \n");
    Component_processEvent(&mDataDep);
}

PUT_PRAGMA
void METH(emptyThisBuffer)(Buffer_p buf)
{
	OstTraceInt1(TRACE_FLOW, "=> MPEG4Dec Ddep: emptyThisBuffer(): buf = 0x%X", buf);
	OstTraceFiltInst1(TRACE_FLOW, "emptyThisBuffer::ALLOCLENGTH=%X \n",buf->ALLOCLENGTH);
#ifndef __MPEG4DEC_ARM_NMF
	//NmfPrint0(2,"\nETB");
#endif
	//PRINT_VAR(buf);
    Component_deliverBuffer(&mDataDep, 0, buf);
}

PUT_PRAGMA
void METH(fillThisBuffer)(Buffer_p buf)
{
	OstTraceInt2(TRACE_FLOW, "<=> MPEG4Dec Ddep: fillThisBuffer(): buf = 0x%X, buf->allocLen = %d.", buf, buf->allocLen);
	
	//printf("\n DDEP_MPC::fillThisBuffer=%X ",++fillThisBufferCount);
    Component_deliverBuffer(&mDataDep, 1, buf);
}

PUT_PRAGMA
void METH(emptyThisHeader)(Buffer_p buf)
{
#ifndef __MPEG4DEC_ARM_NMF
	//NmfPrint0(2,"\nETH");
#endif
	//printf("\n DDEP_MPC::emptyThisHeader=%X",++emptyThisHeaderCount);
	OstTraceInt1(TRACE_FLOW, "<=> MPEG4Dec Ddep: emptyThisHeader(): buf = 0x%x", buf);
    Component_deliverBuffer(&mDataDep, 2, buf);
}

t_uint16 buffer_available_atinput(Component *this)
{
    return 0;
}
t_uint16 buffer_available_atoutput(Component *this)
{
	if(bufOut==0 && Port_queuedBufferCount(&mPorts[1]))
	{
        bufOut = Port_dequeueBuffer(&this->ports[1]);
		OstTraceFiltInst1(TRACE_FLOW, "buffer_available_atoutput bufOut =%X\n",bufOut);
		return 1;
	}
	else if(bufOut)
		return 1;
	else
		return 0;

}

t_uint16 buffer_available_atheaderport(Component *this)
{
	if(bufheader==0 && Port_queuedBufferCount(&mPorts[2]))
	{
        bufheader = Port_dequeueBuffer(&this->ports[2]);
		OstTraceFiltInst1(TRACE_FLOW, "buffer_available_atheaderport bufheader =%X\n",bufheader);
		return 1;
	}
	else if(bufheader)
		return 1;
	else
		return 0;

}

PUT_PRAGMA
void METH(informResourceStatus)(t_t1xhv_resource_status status,t_t1xhv_resource resource)
{
		OstTraceFiltInst2(TRACE_FLOW, "informResourceStatus::status=%X resource=%X \n ",status,resource);
		//printf("\nSend Logevent informresource.");
		iSendLogEvent.event_informResourceStatus(status,resource);
}

PUT_PRAGMA
void METH(vppendAlgo)(t_t1xhv_status status,
		t_t1xhv_vpp_info info,
		t_uint32 durationInTicks)
{
	OstTraceFiltInst2(TRACE_FLOW, "vppendAlgo::status=%X info=%X \n ",status,info);
	//printf("\nendAlgo %d \n",status);
	iSendLogEvent.eventVPPEndAlgo(status,info,durationInTicks);
}
#endif

#ifndef __MPEG4DEC_ARM_NMF
PUT_PRAGMA
void METH(eventVPPEndAlgo)(t_t1xhv_status status,
		t_t1xhv_vpp_info info,
		t_uint32 durationInTicks)
#else
PUT_PRAGMA
void METH(eventVPPEndAlgo)(t_uint32 status,
		t_uint32 info,
		t_uint32 durationInTicks)
#endif
{
	t_uint32 i;
	OstTraceFiltInst2(TRACE_FLOW, "eventVPPEndAlgo::status=%X info=%X \n ",status,info);
    //printf("\n enter eventVPPEndAlgo from vpp");
    //PRINT_VAR(status);
	//PRINT_VAR(info);
   //README:propagate EOS on VNC
   for(i=0;i<ps_ddep_vdc_mpeg4_header_desc->InBuffCnt;i++)
   {
	   if(bufIn[i]->FLAGS & BUFFERFLAG_EOS)
	   {
		   free_references = 1;
		   eos_flag_set = 1;
		   PRINT_VAR(bufIn[i]->FLAGS);
		   OstTraceFiltInst0(TRACE_FLOW, "eventVPPEndAlgo::EOS in VPPendalgo\n");
		   //PRINT_VAR(flushPending);
	   }
   }
   //README::Forward reference copy into output buffer
   //VNC (only when no-deblocking)
   //for a third consecutive B frame
   if(((dpb_buffer.buffer_copied==OMX_FALSE) && (!reference_array[0]) && (!currentVopCoded)))
	{
	   for(i=0;i<ps_ddep_vdc_mpeg4_header_desc->InBuffCnt;i++)
		   mp4d_fill_link_structure(bufIn[i]);
       mp4d_mark_end_link_structure();
		dpb_buffer.buffer_copied = OMX_TRUE;
		//printf("\n VNC (only when no-deblocking)");
		OstTraceFiltInst0(TRACE_FLOW, "eventVPPEndAlgo::Forward reference copy into output buffer\n");
#ifndef __MPEG4DEC_ARM_NMF
       if(currentCodingType==2)
	   {
		   vpp_algo.configureAlgoFormatConv(&ivppEndAlgo,
	    						   YUV420MB_YUV420MB,
	    						   SVP_SHAREDMEM_TO16(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->ADDRESS),
	    						   SVP_SHAREDMEM_TO16(bufOut->ADDRESS),
	    						   0,
	    						   0,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height);

       }
	   else
	   {
		    vpp_algo.configureAlgoFormatConv(&ivppEndAlgo,
	    						   YUV420MB_YUV420MB,
	    						   SVP_SHAREDMEM_TO16(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index + 1)& 1]->ADDRESS),
	    						   SVP_SHAREDMEM_TO16(bufOut->ADDRESS),
	    						   0,
	    						   0,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height);
	   }
	   //printf("\nvpp_algo.controlAlgo entering");
	   vpp_algo.controlAlgo(CMD_START, 0, 1);
	   //printf("\nvpp_algo.controlAlgo exiting");
#else
	   if(currentCodingType==2)
	   {
		   memcpy((t_uint8 *)bufOut->ADDRESS,(t_uint8 *)dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->ADDRESS,  (ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width*ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height*1.5));
		   iSendLogEvent.eventVPPEndAlgo(0,0,0);

       }
	   else
	   {
		   memcpy( (t_uint8 *)bufOut->ADDRESS, (t_uint8 *)dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index + 1)& 1]->ADDRESS, (ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width*ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height*1.5));
		   iSendLogEvent.eventVPPEndAlgo(0,0,0);
	   }
#endif
	}			//README::for a third consecutive B frame
	else if((dpb_buffer.buffer_copied==OMX_FALSE) && (currentCodingType == 2) && (dpb_buffer.dpb_full == OMX_TRUE))
	{
		dpb_buffer.buffer_copied = OMX_TRUE;
		//printf("\n allocated:::RESOURCE_MMDSP");
		get_hw_resource_requested=1;
		OstTraceFiltInst0(TRACE_FLOW, "eventVPPEndAlgo::Request for RESOURCE_MMDSP\n");
		release_resources();
#ifndef __MPEG4DEC_ARM_NMF
		iResource.allocResource(RESOURCE_MMDSP, &iInformResourceStatus);
#else
		iSendLogEvent.event_informResourceStatus(0,0);
#endif
	}
	else if(!currentVopCoded)
	{
		//printf("\n VPPeventEndAlgo:::eventEndAlgo");
		OstTraceFiltInst0(TRACE_FLOW, "eventVPPEndAlgo::Call eventEndAlgo\n");
		iSendLogEvent.eventEndAlgo((t_t1xhv_status)status,(t_t1xhv_decoder_info)info,durationInTicks,0);
	}
    //printf("exit eventVPPEndAlgo vpp");
}
#ifndef __MPEG4DEC_ARM_NMF
PUT_PRAGMA
void METH(event_informResourceStatus)(t_t1xhv_resource_status status,t_t1xhv_resource resource)
#else
PUT_PRAGMA
void METH(event_informResourceStatus)(t_uint32 status,t_uint32 resource)
#endif
{
	OstTraceFiltInst0(TRACE_FLOW, "event_informResourceStatus \n");
	//printf("\n DDEP_ARM::informResourceStatus enter   status = %d",status);
	//README::Forward reference copy into internal buffer
	//1::for a third consecutive B frame
	//2::VNC (only when no-deblocking)
	if(((dpb_buffer.buffer_copied==OMX_FALSE) && (currentCodingType == 2) && (dpb_buffer.dpb_full == OMX_TRUE))
	   ||((dpb_buffer.buffer_copied==OMX_FALSE) && (!reference_array[0]) && (!currentVopCoded)))
	{
       get_vpp_resource = 1;
	   get_hw_resource_requested=0;
	   //PRINT_VAR(currentCodingType);
	   //PRINT_VAR(currentVopCoded);
	   //PRINT_VAR(dpb_buffer.buffer_copied);
	   //printf("\n COPY_FORWARD_REFERENCE");
	   OstTraceFiltInst0(TRACE_FLOW, "event_informResourceStatus::Forward reference copy into internal buffer\n");
	   //+ER348605
#ifndef __MPEG4DEC_ARM_NMF
	   if(!dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
       {
		   //PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->ADDRESS);
		   //PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
		   vpp_algo.configureAlgoFormatConv(&ivppEndAlgo,
	    						   YUV420MB_YUV420MB,
                                   SVP_SHAREDMEM_TO16(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index + 1)& 1]->ADDRESS),
                                   SVP_SHAREDMEM_TO16(reference_array[4]),
	    						   0,
	    						   0,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height);

       }
	  else
	   {
		    //PRINT_VAR(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index + 1)& 1]->ADDRESS);
		    vpp_algo.configureAlgoFormatConv(&ivppEndAlgo,
	    						   YUV420MB_YUV420MB,
	    						   SVP_SHAREDMEM_TO16(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->ADDRESS),
	    						   SVP_SHAREDMEM_TO16(reference_array[4]),
	    						   0,
	    						   0,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height);

	   }

	   //printf("\nvpp_algo.controlAlgo entering");
	   vpp_algo.controlAlgo(CMD_START, 0, 1);
	   //printf("\nvpp_algo.controlAlgo exiting");

#else
       if(!dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
	   {
           memcpy((t_uint8 *)reference_array[4],(t_uint8 *)dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index + 1)& 1]->ADDRESS,  (ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width*ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height*1.5));
           iSendLogEvent.eventVPPEndAlgo(0,0,0);
	   }
	   else
	   {   
           memcpy( (t_uint8 *)reference_array[4],(t_uint8 *)dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->ADDRESS,  (ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width*ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height*1.5));
           iSendLogEvent.eventVPPEndAlgo(0,0,0);
	   }//+ER348605
#endif
	}
	else if((dpb_buffer.buffer_copied==OMX_TRUE) && (!reference_array[0]) && (!currentVopCoded)) 
	{	
		t_uint32 i;
		OstTraceFiltInst0(TRACE_FLOW, "event_informResourceStatus::Consecutive VNC\n");
        for(i=0;i<ps_ddep_vdc_mpeg4_header_desc->InBuffCnt;i++)
			mp4d_fill_link_structure(bufIn[i]);
		mp4d_mark_end_link_structure();
		get_vpp_resource = 1;
	    get_hw_resource_requested=0;
		dpb_buffer.buffer_copied = OMX_TRUE;
		//printf("\n VNC (only when no-deblocking)");
#ifndef __MPEG4DEC_ARM_NMF
       if(currentCodingType==2)
	   {
		   vpp_algo.configureAlgoFormatConv(&ivppEndAlgo,
	    						   YUV420MB_YUV420MB,
	    						   SVP_SHAREDMEM_TO16(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->ADDRESS),
	    						   SVP_SHAREDMEM_TO16(bufOut->ADDRESS),
	    						   0,
	    						   0,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height);

       }
	   else
	   {
		    vpp_algo.configureAlgoFormatConv(&ivppEndAlgo,
	    						   YUV420MB_YUV420MB,
	    						   SVP_SHAREDMEM_TO16(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index + 1)& 1]->ADDRESS),
	    						   SVP_SHAREDMEM_TO16(bufOut->ADDRESS),
	    						   0,
	    						   0,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
	    						   ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height);
	   }
	   //printf("\nvpp_algo.controlAlgo entering");
	   vpp_algo.controlAlgo(CMD_START, 0, 1);
	   //printf("\nvpp_algo.controlAlgo exiting");
#else
	   if(currentCodingType==2)
	   {
		   memcpy((t_uint8 *)bufOut->ADDRESS,(t_uint8 *)dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->ADDRESS,  (ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width*ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height*1.5));
		   iSendLogEvent.eventVPPEndAlgo(0,0,0);

       }
	   else
	   {
		   memcpy( (t_uint8 *)bufOut->ADDRESS, (t_uint8 *)dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index + 1)& 1]->ADDRESS, (ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width*ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height*1.5));
		   iSendLogEvent.eventVPPEndAlgo(0,0,0);
	   }
#endif
	}
    else
	{
			//printf("\n DDEP_ARM::informResourceStatus exit");
			switch (status)
			{
#ifdef __MPEG4DEC_ARM_NMF
						case 0    :
#else
                        case STA_RESOURCE_FREE    :
#endif
					// A previously allocated resource is now free, so ready to be used.
					// This will call the function process if componentis still in Executing mode
					// Cannot launch directly the decoder because may be the component is no more in
					// executing
					get_hw_resource = 1;
					get_hw_resource_requested=0;
					/* +Change start for ER339609 */
					if(compState == OMX_StatePause)
					{
							if(defer_pause)
							{
								OstTraceFiltInst0(TRACE_FLOW, "defer_pause \n");
								defer_pause = 0;
#ifdef 	__MPEG4DEC_ARM_NMF
								Component::sendCommand(OMX_CommandStateSet, OMX_StatePause); 
#else
								Component_sendCommand(&mDataDep, OMX_CommandStateSet, OMX_StatePause);
#endif
							}
						release_resources();
					}
					else
					{	OstTraceFiltInst0(TRACE_FLOW, "event_informResourceStatus::Program Algo\n");
#ifdef __MPEG4DEC_ARM_NMF
						processActual();
#else
                        processActual(&mDataDep);
#endif				
					} 
					 /* -Change start for ER339609 */
					break;
#ifdef __MPEG4DEC_ARM_NMF
						case 1    :
#else
                        case STA_PARAM_ERROR      : // Input parameter error so that allocation can't be done.
#endif
				//case STA_NOT_SUPPORTED    : // Required allocation is not supported.
				//case STA_ERROR_ALLOC_FULL : // Internal allocation FIFO full. Please allocate later.
					//PRINT_VAR(status);
					break;

			default:
					break;
		  }
	}
	//printf("\n DDEP_ARM::informResourceStatus exit");
}

PUT_PRAGMA
void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
    
    //printf("\n DDEP_ARM::setTunnelStatus enter");
#ifdef __MPEG4DEC_ARM_NMF
    if (portIdx == -1)
    {
        for (int i=0; i<TOTAL_PORT_COUNT; i++)
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

#else
	int i=0;
    if (portIdx == -1)
    {
        for (i=0; i<TOTAL_PORT_COUNT; i++)
        {
            if (isTunneled & (1<<i))
            {
                Port_setTunnelStatus(&mPorts[i],true);
            }
            else
            {
                Port_setTunnelStatus(&mPorts[i],false);
            }
        }
    }
    else
    {
        if (isTunneled & (1<<portIdx))
        {
			Port_setTunnelStatus(&mPorts[portIdx],true);
        }
        else
        {
			Port_setTunnelStatus(&mPorts[portIdx],false);
        }
    }

	//printf("\n DDEP_ARM::setTunnelStatus exit");
#endif
	OstTraceFiltInst0(TRACE_FLOW, "setTunnelStatus \n");
}

#ifdef __MPEG4DEC_ARM_NMF
STATIC_FLAG void COMP(ReleaseBuffer)(t_uint32 port_idx,OMX_BUFFERHEADERTYPE_p buf)
#else
STATIC_FLAG void COMP(ReleaseBuffer)(t_uint32 port_idx,Buffer_p buf)
#endif
{
#ifndef __MPEG4DEC_ARM_NMF
		OstTraceInt2(TRACE_FLOW, "=> MPEG4Dec Ddep: ReleaseBuffer port_idx = %d, buf = 0x%x", port_idx, buf);
#endif

    if((buf->FLAGS & BUFFERFLAG_DECODEONLY)&&(!(buf->FLAGS & BUFFERFLAG_EOS)))
	{
        OstTraceInt0(TRACE_FLOW, "ReleaseBuffer::BUFFERFLAG_DECODEONLY \n");
		OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: Releasing Buffer ReleaseBuffer() line %d", __LINE__);
#ifdef __MPEG4DEC_ARM_NMF
		Component::deliverBuffer(port_idx, buf);
#else
		Component_deliverBuffer(&mDataDep,port_idx, buf);
#endif

	}
	else
	{
#ifdef 	__MPEG4DEC_ARM_NMF
		//printf("\n DDEP_ARM::ReleaseBuffer enter");
		if(isDispatching())
		{
			//printf("\n DDEP_ARM::returnBuffer enterport_idx =%d",port_idx);
			mPorts[port_idx].returnBuffer(buf);
		}
		else
		{
			//printf("\n DDEP_ARM::returnBufferAsync enter port_idx =%d",port_idx);
			returnBufferAsync(port_idx,buf);
		}
		//printf("\n DDEP_ARM::ReleaseBuffer exit");
#else
		if(doNotReleaseNextI && (port_idx == 1) /*&& (buf == immediate_i_frame_bufout)*/) //currentCodingType may not be 0 when the I frame (internal buffer in this case) is released
		{
			//Do nothing
			OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: ReleaseBuffer() line %d, not releasing any buffer", __LINE__);
			doNotReleaseNextI = 0;
			immediate_i_frame_bufOut = 0;

			//>OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Ddep: ReleaseBuffer(): buf->address + 0 = 0x%x", *((t_uint8 *)(buf->address)+ 0));
			//>OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Ddep: ReleaseBuffer(): buf->address + 1 = 0x%x", *((t_uint8 *)(buf->address)+ 1));
			//>OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Ddep: ReleaseBuffer(): buf->address + 2 = 0x%x", *((t_uint8 *)(buf->address)+ 2));
			//>OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Ddep: ReleaseBuffer(): buf->address + 3 = 0x%x", *((t_uint8 *)(buf->address)+ 3));
			//>OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Ddep: ReleaseBuffer(): buf->address + 4 = 0x%x", *((t_uint8 *)(buf->address)+ 4));

		}
		else
		{
			OmxEvent ev;
			//printf("\nDDEP_MPC::ReleaseBuffer bufferport_index=%ld",(t_uint32)port_idx);

			OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: ReleaseBuffer() Releasing Buffer line %d", __LINE__);
	
			ev.fsmEvent.signal =  OMX_RETURNBUFFER_SIG;
			ev.args.returnBuffer.portIdx = port_idx;
			ev.args.returnBuffer.buffer = buf;
			FSM_dispatch(&mDataDep,&ev);
		}
	//printf("\n exiting ReleaseBuffer");
#endif
        //if((buf->FLAGS & BUFFERFLAG_EOS) && (port_idx==1))
		//iSendLogEvent.eventEOS(buf->FLAGS);
	}

	#ifndef __MPEG4DEC_ARM_NMF
		OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Ddep: ReleaseBuffer");
	#endif
}
void METH(eventEOS)(t_uint32 FLAGS)
{
    proxy.eventHandler(OMX_EventBufferFlag, 1,FLAGS);
}
STATIC_FLAG void COMP(mp4d_init_buffer_link)(t_ahb_address physical_address, ts_ddep_buffer_descriptor *buffer_desc)
{
    OstTraceFiltInst0(TRACE_FLOW, "mp4d_init_buffer_link \n");
	//printf("\n DDEP_ARM::mp4d_init_buffer_link enter");
#ifdef __MPEG4DEC_ARM_NMF
	bsm.ps_ddep_bitstream_buf_link_and_header = (t_uint32)(ts_ddep_bitstream_buf_link_and_header SHAREDMEM*)(buffer_desc->nLogicalAddress);
	ps_ddep_bitstream_buf_link_and_header = (ts_ddep_bitstream_buf_link_and_header*)bsm.ps_ddep_bitstream_buf_link_and_header;
	//printf("\n DDEP_ARM::=%X",bsm.ps_ddep_bitstream_buf_link_and_header);
	//printf("\n DDEP_ARM::=%X",ps_ddep_bitstream_buf_link_and_header);
#else
	bsm.ps_ddep_bitstream_buf_link_and_header = (ts_ddep_bitstream_buf_link_and_header SHAREDMEM*)(buffer_desc->nMpcAddress);
	ps_ddep_bitstream_buf_link_and_header = bsm.ps_ddep_bitstream_buf_link_and_header;

#endif
    //printf("\n DDEP_ARM::mp4d_init_buffer_link enter bsm =%x ts_ddep_bitstream_buf_link_and_header =%x ts_t1xhv_bitstream_buf_link =%x t_ahb_address =%x  t_ddep_bitstream_mgmt =%x ",sizeof(bsm),sizeof(ts_ddep_bitstream_buf_link_and_header),sizeof(ts_t1xhv_bitstream_buf_link),sizeof(t_ahb_address),sizeof(t_ddep_bitstream_mgmt));
	//PRINT_VAR(sizeof(bsm));
	//PRINT_VAR(sizeof(ts_ddep_bitstream_buf_link_and_header));
	//PRINT_VAR(sizeof(ts_t1xhv_bitstream_buf_link));
	//PRINT_VAR(sizeof(t_ahb_address));
	//PRINT_VAR(sizeof(t_ddep_bitstream_mgmt));
    bsm.physical_ps_ddep_bitstream_buf_link_and_header = physical_address;
	//printf("\n DDEP_ARM::=%X",bsm.physical_ps_ddep_bitstream_buf_link_and_header);
	bsm.last_index_ps_ddep_bitstream_buf_link_and_header=(buffer_desc->nSize)/sizeof(ts_ddep_bitstream_buf_link_and_header);
	//printf("\n DDEP_ARM::bsm.last_index_ps_ddep_bitstream_buf_link_and_header=%d",bsm.last_index_ps_ddep_bitstream_buf_link_and_header);

	if(bsm.last_index_ps_ddep_bitstream_buf_link_and_header==0)
	{
		OstTraceFiltInst0(TRACE_FLOW, "last_index_ps_ddep_bitstream_buf_link=0 \n");
		//printf("last_index_ps_ddep_bitstream_buf_link=0");
	}

	bsm.index_to_next_not_required_bitstream_buffer = 0;
	//PRINT_VAR(buffer_desc->nSize);
	buffer_desc->nSize-=bsm.last_index_ps_ddep_bitstream_buf_link_and_header*sizeof(ts_ddep_bitstream_buf_link_and_header);
	//PRINT_VAR(buffer_desc->nSize);
	//printf("\n DDEP_ARM::buffer_desc->nSize=%d",buffer_desc->nSize);
	//printf("\n DDEP_ARM::mp4d_init_buffer_link exit");
}
#ifdef __MPEG4DEC_ARM_NMF
STATIC_FLAG void COMP(mp4d_fill_link_structure)(OMX_BUFFERHEADERTYPE_p pSrc)
#else
STATIC_FLAG void COMP(mp4d_fill_link_structure)(Buffer_p pSrc)
#endif
{
	t_uint16 at_index = bsm.current_bitstream_buffer_index;
	t_ahb_address curr;
	OstTraceFiltInst1(TRACE_FLOW, "mp4d_fill_link_structure alloclength=%d\n",pSrc->ALLOCLENGTH);
	
	//printf("\n DDEP_ARM::mp4d_fill_link_structure enter");
	//printf("\n DDEP_ARM::mp4d_fill_link_structure enter  one= %d two =%d",bsm.current_bitstream_buffer_index,bsm.last_index_ps_ddep_bitstream_buf_link_and_header);
    if (bsm.current_bitstream_buffer_index>=bsm.last_index_ps_ddep_bitstream_buf_link_and_header)
	{
		//printf("No More buffer link header possible");
		while(1); //FIXME send error to Proxy
	}

	curr = bsm.physical_ps_ddep_bitstream_buf_link_and_header + sizeof(ts_ddep_bitstream_buf_link_and_header)*(t_uint32)MULTIPLYING_FACTOR*((t_uint32)at_index);
	//printf("\n DDEP_ARM::mp4d_fill_link_structure curr =%x",curr);

	//PRINT_VAR(curr);
	//PRINT_VAR(at_index);

    if(pSrc == fakeBufIn || DataInBuf )
	{
		//PRINT_VAR(pSrc);
		ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16((curr+(t_uint32)sizeof(ts_ddep_bitstream_buf_link_and_header)*(t_uint32)MULTIPLYING_FACTOR)|EXT_BIT);
		ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_prev_buf_link = SVP_SHAREDMEM_TO16(at_index?((curr-(t_uint32)sizeof(ts_ddep_bitstream_buf_link_and_header)*(t_uint32)MULTIPLYING_FACTOR))|EXT_BIT:(t_uint32)0);
		if(DataInBuf)
		{
			ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_start = AHB_(SVP_SHAREDMEM_TO16(reference_array[8]));
			ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_end   = SVP_SHAREDMEM_TO16(AHB_(reference_array[8]) + MULTIPLYING_FACTOR*AHB_(pSrc->ALLOCLENGTH));
        }
		else
		{
			ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_start = AHB_(SVP_SHAREDMEM_TO16(pSrc->ADDRESS));
			ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_end   = SVP_SHAREDMEM_TO16(AHB_(pSrc->ADDRESS) + AHB_(pSrc->ALLOCLENGTH));
		}
		//printf("\n DDEP_ARM::mp4d_fill_link_structure fakeBufIn addr_next_buf_link =%x",ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link);
	    //printf("\n DDEP_ARM::mp4d_fill_link_structure fakeBufIn addr_prev_buf_link =%x",ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_prev_buf_link);
		//printf("\n DDEP_ARM::mp4d_fill_link_structure fakeBufIn addr_buffer_start =%x",ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_start);
		//printf("\n DDEP_ARM::mp4d_fill_link_structure fakeBufIn addr_buffer_end =%x",ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_end);
		//PRINT_VAR(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link);
		//PRINT_VAR(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_prev_buf_link);
		//PRINT_VAR(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_start);
		//PRINT_VAR(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_end);

	}
	else
	{
        ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16((curr+(t_uint32)sizeof(ts_ddep_bitstream_buf_link_and_header)*(t_uint32)MULTIPLYING_FACTOR)|EXT_BIT);
		ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_prev_buf_link = SVP_SHAREDMEM_TO16(at_index?((curr-(t_uint32)sizeof(ts_ddep_bitstream_buf_link_and_header)*(t_uint32)MULTIPLYING_FACTOR))|EXT_BIT:(t_uint32)0);
		ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_start = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(pSrc->ADDRESS)));
		ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_end   = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(AHB_(pSrc->ADDRESS)) + MULTIPLYING_FACTOR*AHB_(SVP_SHAREDMEM_FROM24(pSrc->ALLOCLENGTH)));
		//printf("\n DDEP_ARM::mp4d_fill_link_structure addr_next_buf_link =%x",ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link);
		//printf("\n DDEP_ARM::mp4d_fill_link_structure addr_prev_buf_link =%x",ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_prev_buf_link);
		//printf("\n DDEP_ARM::mp4d_fill_link_structure addr_buffer_start =%x",ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_start);
		//printf("\n DDEP_ARM::mp4d_fill_link_structure addr_buffer_end =%x",ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_buffer_end);
		//PRINT_VAR(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link);
		//PRINT_VAR(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_prev_buf_link);
    }


	ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_buffer_p = SVP_SHAREDMEM_TO16((t_uint32)pSrc);

	bsm.current_bitstream_buffer_index++;
	//PRINT_VAR(bsm.current_bitstream_buffer_index);
	//printf("\n DDEP_ARM::mp4d_fill_link_structure exit");
}

STATIC_FLAG void COMP(mp4d_mark_end_link_structure)()
{
	OstTraceFiltInst0(TRACE_FLOW, "mp4d_mark_end_link_structure \n");
	//printf("\n DDEP_ARM::mp4d_mark_end_link_structure enter");
	{
		//at first step add fakebuffer in the end
#ifdef __MPEG4DEC_ARM_NMF
		//OMX_BUFFERHEADERTYPE_p FakeBuffer = fakeBufIn;
#else
		//Buffer_p FakeBuffer = fakeBufIn;
#endif
		//printf(" Func::mp4d_mark_end_link_structure\n");
		//printf("fakeBufIn =0x%x",fakeBufIn);
		mp4d_fill_link_structure(fakeBufIn); //make sure that nFlag is 0xFFFFFFFFul to inform it not send this buffer back
	}

	{
		t_uint32 at_index = bsm.current_bitstream_buffer_index-1;
		ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16(0);
		ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_prev_buf_link = SVP_SHAREDMEM_TO16(0);
		//PRINT_VAR(ps_ddep_bitstream_buf_link_and_header[at_index].s_ddep_bitstream_buf_link.addr_next_buf_link);
		//PRINT_VAR(ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_prev_buf_link);
//		ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_buffer_start -= 48ul;
//#ifndef __MPEG4DEC_ARM_NMF
		//ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_buffer_start = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[0].s_ddep_bitstream_buf_link.addr_buffer_start) - 48ul);
//#endif

	}
	//printf("\n DDEP_ARM::mp4d_mark_end_link_structure exit");
}

#ifdef __MPEG4DEC_ARM_NMF
STATIC_FLAG void COMP(mp4d_get_next_not_required_bitstream_buffer)(OMX_BUFFERHEADERTYPE_p &bufferIn)
#else
STATIC_FLAG void COMP(mp4d_get_next_not_required_bitstream_buffer)(Buffer_p *bufferIn)
#endif
{
   //printf("\n DDEP_ARM::mp4d_get_next_not_required_bitstream_buffer enter index_to_next_not_required_bitstream_buffer =%d current_bitstream_buffer_index=%d \n",bsm.index_to_next_not_required_bitstream_buffer,bsm.current_bitstream_buffer_index);
#ifdef __MPEG4DEC_ARM_NMF
	bufferIn=NULL;
#else
	*bufferIn=0;
#endif
	if (bsm.current_bitstream_buffer_index==0) return;

	while (1)
	{
#ifdef __MPEG4DEC_ARM_NMF
		OMX_BUFFERHEADERTYPE_p pBuffer;
#else
		Buffer_p SHAREDMEM pBuffer;
#endif
		//PRINT_VAR(bsm.index_to_next_not_required_bitstream_buffer);
		if (bsm.index_to_next_not_required_bitstream_buffer == bsm.current_bitstream_buffer_index)
		{
			//reset indexes
			bsm.index_to_next_not_required_bitstream_buffer = 0;
			bsm.current_bitstream_buffer_index = 0;
			//PRINT_VAR(bsm.current_bitstream_buffer_index);
			//PRINT_VAR(bsm.index_to_next_not_required_bitstream_buffer);
			break;
		}

#ifdef __MPEG4DEC_ARM_NMF
		pBuffer = (OMX_BUFFERHEADERTYPE_p)(ps_ddep_bitstream_buf_link_and_header[bsm.index_to_next_not_required_bitstream_buffer].s_ddep_buffer_p);
#else
		pBuffer = SVP_SHAREDMEM_FROM16(ps_ddep_bitstream_buf_link_and_header[bsm.index_to_next_not_required_bitstream_buffer].s_ddep_buffer_p);

#endif
		bsm.index_to_next_not_required_bitstream_buffer++;

		if (pBuffer == fakeBufIn) continue;  //dont know why ::fakeBufIn

#ifdef __MPEG4DEC_ARM_NMF
		bufferIn = pBuffer;
#else
		*bufferIn = pBuffer;
#endif
		//printf("\n DDEP_ARM::break exit");
		break;
	}
	OstTraceFiltInst0(TRACE_FLOW, "mp4d_get_next_not_required_bitstream_buffer \n");
}

STATIC_FLAG void COMP(mp4d_create_fake_buffer)(t_ahb_address physical_address, ts_ddep_buffer_descriptor *fake_buffer_desc)
{
	t_uint32 tmp_var;
#ifdef __MPEG4DEC_ARM_NMF
	t_uint16 SHAREDMEM *ptrToFillFakeBuffer = (t_uint16 SHAREDMEM*)fake_buffer_desc->nLogicalAddress;
#else
	t_uint16 SHAREDMEM *ptrToFillFakeBuffer = (t_uint16 SHAREDMEM*)fake_buffer_desc->nMpcAddress;
#endif
	//printf("\n DDEP_ARM::mp4d_create_fake_buffer ptrToFillFakeBuffer =%x enter",ptrToFillFakeBuffer);
	//PRINT_VAR(fake_buffer_desc->nSize);
    if(fake_buffer_desc->nSize<128)  //why this check
	{
		//printf("insufficient memory (for fake buffer)");  ///return event to proxy
	}

	// Assuming 16 bit mode: will fail in functioning if not 16 bit
#ifndef __MPEG4DEC_ARM_NMF
#define LOW(x) (x&0xFFFFu)
#define HIGH(x) ((x>>16)&0xFFFFu)
#else
#define LOW(x) (x)
#define HIGH(x) (x)
#endif

	// to verify if LOW and HIGH should be swapped (verify from arm memory side)
	//why SH and SP has been added
	tmp_var = START_CODE_VALUE_SH;
	ptrToFillFakeBuffer[9]=LOW(tmp_var);
	ptrToFillFakeBuffer[8]=HIGH(tmp_var);
	ptrToFillFakeBuffer[11]=LOW(tmp_var);
	ptrToFillFakeBuffer[10]=HIGH(tmp_var);
	tmp_var = START_CODE_VALUE_SP;
	ptrToFillFakeBuffer[13]=LOW(tmp_var);
	ptrToFillFakeBuffer[12]=HIGH(tmp_var);
	ptrToFillFakeBuffer[15]=LOW(tmp_var);
	ptrToFillFakeBuffer[14]=HIGH(tmp_var);
#ifdef __MPEG4DEC_ARM_NMF
    fakeBufIn->ADDRESS = (t_uint8*)fake_buffer_desc->nLogicalAddress;
#else
    (fakeBufIn->ADDRESS) = (fake_buffer_desc->nPhysicalAddress);
#endif
	//PRINT_VAR(fakeBufIn);
	//PRINT_VAR(fakeBufIn->ADDRESS);
	fakeBufIn->FLAGS = 0;
	fakeBufIn->ALLOCLENGTH = 256u;
	fakeBufIn->LENGTH = 16u;

#ifndef __MPEG4DEC_ARM_NMF
	fakeBufIn->byteInLastWord = 0;
	fakeBufIn->data = 0;
#endif
	OstTraceFiltInst0(TRACE_FLOW, "mp4d_create_fake_buffer \n");
}
//called in case of EOS received with emptyInput buffer
STATIC_FLAG void COMP(do_flushAllPortsX)()
{
    OstTraceInt0(TRACE_FLOW, "=> do_flushAllPortsX()");
    //printf("\n DDEP_ARM::do_flushAllPortsX enter");
	//PRINT_VAR(bufIn);
	firstFramedecoded=0;
	ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag = 0;
	if(bufIn[0])
	{
	  		  ReleaseBuffer(0,bufIn[0]);
	  		  bufIn[0]->LENGTH = SVP_SHAREDMEM_TO24(0);
	  		  bufIn[0]=0;
  
	}
	PRINT_VAR(bufheader);
	if(bufheader)
	{
				ReleaseBuffer(2,bufheader);
				bufheader->LENGTH = SVP_SHAREDMEM_TO24(0);
				bufheader=0;

	}

	if(!reference_array[0])
	{



		PRINT_VAR(bufOut);
		if(bufOut)
		{		if((!(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2])) && (!(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])))
				{
					bufOut->FLAGS |= BUFFERFLAG_EOS;
					//proxy.eventHandler(OMX_EventBufferFlag, 1,bufOut->FLAGS);

					PRINT_VAR(bufOut->FLAGS);
				}
		}
		PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
		if(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
		{
			if(!(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]))
			{
				dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->FLAGS |= BUFFERFLAG_EOS;
				//proxy.eventHandler(OMX_EventBufferFlag, 1,dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->FLAGS);
			}
		ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
		dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = 0;
		}
		PRINT_VAR(dpb_buffer.bw_reference_buffer[0]);
		if(dpb_buffer.bw_reference_buffer[0])// for copied frame
		{
			ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[0]);
			dpb_buffer.bw_reference_buffer[0] = 0;
		}
		PRINT_VAR(dpb_buffer.bw_reference_buffer[1]);
		if(dpb_buffer.bw_reference_buffer[1])// in display order B frame
		{
			ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[1]);
			dpb_buffer.bw_reference_buffer[1] = 0;
		}
		PRINT_VAR(dpb_buffer.bw_reference_buffer[2]);
		if(dpb_buffer.bw_reference_buffer[2])// in display order b frame
		{
			ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[2]);
			dpb_buffer.bw_reference_buffer[2] = 0;
		}
		PRINT_VAR(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]);
		if(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2])
		{
			dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]->FLAGS |= BUFFERFLAG_EOS;
			//proxy.eventHandler(OMX_EventBufferFlag, 1,dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]->FLAGS);
			ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]);
			dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2] = 0;
		}
		dpb_buffer.curr_fw_buffer_index = 0;
		dpb_buffer.curr_bw_buffer_index = 1; // why??
		dpb_buffer.dpb_IP_full = OMX_FALSE;
		dpb_buffer.dpb_full = OMX_FALSE;
		dpb_buffer.buffer_copied = OMX_FALSE;

		if(bufOut)
		{       
				ReleaseBuffer(1,bufOut);
				bufOut->LENGTH = SVP_SHAREDMEM_TO24(0);
				bufOut=0;
		}

	}
	else
// deblocking enabled
	{
		//PRINT_VAR(bufOut);

		if(dblk_dpb_buffer.buffout_reference) //not in display order::to be corrected
		{
			//PRINT_VAR(dblk_dpb_buffer.buffout_reference);
			dblk_dpb_buffer.buffout_reference->LENGTH = SVP_SHAREDMEM_TO24(0);
			ReleaseBuffer(1,dblk_dpb_buffer.buffout_reference);
			dblk_dpb_buffer.buffout_reference = 0;
		}
		  if(dblk_dpb_buffer.bw_reference_buffer) // release bw_reference_buffer
		{
			//PRINT_VAR(dblk_dpb_buffer.bw_reference_buffer);
			ReleaseBuffer(1,dblk_dpb_buffer.bw_reference_buffer);
			dblk_dpb_buffer.bw_reference_buffer = 0;
		}
		if(bufOut)
		{
					bufOut->FLAGS |= BUFFERFLAG_EOS;
					//proxy.eventHandler(OMX_EventBufferFlag, 1,bufOut->FLAGS);
					//PRINT_VAR(bufOut->FLAGS);
					ReleaseBuffer(1,bufOut);
					bufOut->LENGTH = SVP_SHAREDMEM_TO24(0);
					bufOut=0;

		}
		dblk_dpb_buffer.Last_vop_not_coded=0;
		dblk_dpb_buffer.B_buffer_index=0;

		dblk_dpb_buffer.IP_buffer_index=0;

		dblk_dpb_buffer.last_coding_type=0;
	}
	//printf("\n DDEP_ARM::do_flushAllPortsX exit");
}

STATIC_FLAG void COMP(do_flushAllPorts)()
{
	OstTraceInt0(TRACE_FLOW, "=> do_flushAllPorts()");
	//printf("\n DDEP_ARM::do_flushAllPorts enter");
	firstFramedecoded=0;
	ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag=0;
	if(!reference_array[0])
	{


        PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
		if(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
		{
			if(!(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]))
			{
				if(eos_flag_set)
				{
					eos_flag_set=0;
					dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->FLAGS |= BUFFERFLAG_EOS;
					//proxy.eventHandler(OMX_EventBufferFlag, 1,dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->FLAGS);
				}
			}
			ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
			dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = 0;
		}
		PRINT_VAR(dpb_buffer.bw_reference_buffer[0]);
		if(dpb_buffer.bw_reference_buffer[0])// for copied frame
		{
			ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[0]);
			dpb_buffer.bw_reference_buffer[0] = 0;
		}
		PRINT_VAR(dpb_buffer.bw_reference_buffer[1]);
		if(dpb_buffer.bw_reference_buffer[1])// in display order B frame
		{
			ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[1]);
			dpb_buffer.bw_reference_buffer[1] = 0;
		}
		PRINT_VAR(dpb_buffer.bw_reference_buffer[2]);
		if(dpb_buffer.bw_reference_buffer[2])// in display order b frame
		{
			ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[2]);
			dpb_buffer.bw_reference_buffer[2] = 0;
		}
		PRINT_VAR(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]);
		if(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2])
		{
			if(eos_flag_set)
				{
					eos_flag_set=0;
					dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]->FLAGS |= BUFFERFLAG_EOS;
					//proxy.eventHandler(OMX_EventBufferFlag, 1,dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]->FLAGS);
				}
			ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]);
			dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2] = 0;
		}
		  dpb_buffer.curr_fw_buffer_index = 0;
		dpb_buffer.curr_bw_buffer_index = 1; // why??
		dpb_buffer.dpb_IP_full = OMX_FALSE;
		dpb_buffer.dpb_full = OMX_FALSE;
		dpb_buffer.buffer_copied = OMX_FALSE;
	}
	else
 //deblocking enabled
	{

        if(dblk_dpb_buffer.buffout_reference) //not in display order::to be corrected
		{
  if(!(dblk_dpb_buffer.bw_reference_buffer))
			{
				if(eos_flag_set)
				{
					eos_flag_set=0;
					dblk_dpb_buffer.buffout_reference->FLAGS |= BUFFERFLAG_EOS;
					//proxy.eventHandler(OMX_EventBufferFlag, 1,dblk_dpb_buffer.buffout_reference->FLAGS);
				}
			}
			//PRINT_VAR(dblk_dpb_buffer.buffout_reference);
			ReleaseBuffer(1,dblk_dpb_buffer.buffout_reference);
			dblk_dpb_buffer.buffout_reference = 0;
		}
		if(dblk_dpb_buffer.bw_reference_buffer) // release bw_reference_buffer
		{
			 if(eos_flag_set)
			 {
			 	eos_flag_set=0;
			 	dblk_dpb_buffer.bw_reference_buffer->FLAGS |= BUFFERFLAG_EOS;
			 	//proxy.eventHandler(OMX_EventBufferFlag, 1,dblk_dpb_buffer.bw_reference_buffer->FLAGS);
			 }
			//PRINT_VAR(dblk_dpb_buffer.bw_reference_buffer);
			ReleaseBuffer(1,dblk_dpb_buffer.bw_reference_buffer);
			dblk_dpb_buffer.bw_reference_buffer = 0;
		}

		dblk_dpb_buffer.Last_vop_not_coded=0;
		dblk_dpb_buffer.B_buffer_index=0;

		dblk_dpb_buffer.IP_buffer_index=0;

		dblk_dpb_buffer.last_coding_type=0;
    }
	//printf("\n DDEP_ARM::do_flushAllPorts exit");
}

STATIC_FLAG void COMP(remove_references)()
{
	OstTraceInt0(TRACE_FLOW, "remove_references \n");
	//printf("\n DDEP_ARM::remove_references enter");
	firstFramedecoded=0;
	ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag=0;
    if(!reference_array[0])
	{


		PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
		if(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
		{
            ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
			dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = 0;
		}
		PRINT_VAR(dpb_buffer.bw_reference_buffer[0]);
		if(dpb_buffer.bw_reference_buffer[0])// for copied frame
		{
			ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[0]);
			dpb_buffer.bw_reference_buffer[0] = 0;
		}
		PRINT_VAR(dpb_buffer.bw_reference_buffer[1]);
		if(dpb_buffer.bw_reference_buffer[1])// in display order B frame
		{
			ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[1]);
			dpb_buffer.bw_reference_buffer[1] = 0;
		}
		PRINT_VAR(dpb_buffer.bw_reference_buffer[2]);
		if(dpb_buffer.bw_reference_buffer[2])// in display order b frame
		{
			ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[2]);
			dpb_buffer.bw_reference_buffer[2] = 0;
		}
		PRINT_VAR(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]);
		if(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2])
		{
			ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]);
			dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2] = 0;
		}
		dpb_buffer.curr_fw_buffer_index = 0;
		dpb_buffer.curr_bw_buffer_index = 1; // why??
		dpb_buffer.dpb_IP_full = OMX_FALSE;
		dpb_buffer.dpb_full = OMX_FALSE;
		dpb_buffer.buffer_copied = OMX_FALSE;
	}
	else
//deblocked enabled
	{

        if(dblk_dpb_buffer.buffout_reference) //not in display order::to be corrected
		{
            //PRINT_VAR(dblk_dpb_buffer.buffout_reference);
			ReleaseBuffer(1,dblk_dpb_buffer.buffout_reference);
			dblk_dpb_buffer.buffout_reference = 0;
		}
		if(dblk_dpb_buffer.bw_reference_buffer) // release bw_reference_buffer
		{
			//PRINT_VAR(dblk_dpb_buffer.bw_reference_buffer);
			ReleaseBuffer(1,dblk_dpb_buffer.bw_reference_buffer);
			dblk_dpb_buffer.bw_reference_buffer = 0;
		}
		dblk_dpb_buffer.Last_vop_not_coded=0;
		dblk_dpb_buffer.B_buffer_index=0;

		dblk_dpb_buffer.IP_buffer_index=0;

		dblk_dpb_buffer.last_coding_type=0;
	}
	//printf("\n DDEP_ARM::remove_references exit");
}


/*****************************************************************************/
/**
 * \brief  setNeeds
 *
 * Set to this component memory needs (internal buffers)
  */
/*****************************************************************************/
PUT_PRAGMA
void METH(setNeeds)(t_uint16 error_reporting,t_uint8 BufCountActual, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc,t_uint16 disable)
{
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec Ddep: setNeeds()");

	OstTraceInt1(TRACE_FLOW, "setNeeds error_reporting =%d\n",error_reporting);
	//printf("\n DDEP_ARM::setNeeds enter");
	/* +Change start for ER335883 */
	Error_Reporting = Error_Reporting | error_reporting;   //why is used nad what value to be set
	/* -Change end for ER335883 */
	//PRINT_VAR(s_channelId);
    MaxBufInput=BufCountActual;
	
	//printf("setNeeds sizes bbm=%X, dbg=%X\n", bbm_desc.nSize, debugBuffer_desc.nSize);

	// assuming 16 bit memory allocation from ARM side
#ifndef __MPEG4DEC_ARM_NMF
	//PRINT_VAR(bbm_desc.nSize);
	//PRINT_VAR(debugBuffer_desc.nSize);
	bbm_desc.nSize>>=1;
    debugBuffer_desc.nSize>>=1;
	//PRINT_VAR(bbm_desc.nSize);
	//PRINT_VAR(debugBuffer_desc.nSize);
#endif
	s_mpeg4d_ddep_desc.bbm_desc = bbm_desc;
	s_mpeg4d_ddep_desc.s_debug_buffer_desc = debugBuffer_desc;
#ifdef __MPEG4DEC_ARM_NMF
		mp4d_init_buffer_link(bbm_desc.nLogicalAddress, &bbm_desc);
#else
#ifdef __DEBUG_TRACE_ENABLE
	mpc_trace_init(debugBuffer_desc.nMpcAddress, debugBuffer_desc.nSize);
#endif
// for getting traces from algo component
	//iSleep.setDebug(0, s_mpeg4d_ddep_desc.s_debug_buffer_desc.nMpcAddress, 0);
    //iResource.setDebug(DBG_MODE_NORMAL, convto16bitmode(s_mpeg4d_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
#ifdef __MPEG4DEC_ARM_NMF
	disable=0;
#endif
if(disable)
	{
		OstTraceFiltInst0(TRACE_FLOW, "ARM_LOAD_CALCULATION \n");
		iCodecAlgo.setDebug(DBG_MODE_NO_HW, convto16bitmode(s_mpeg4d_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
		vpp_algo.setDebug(DBG_MODE_NO_HW, convto16bitmode(s_mpeg4d_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
	}
	else
	{
		iCodecAlgo.setDebug(DBG_MODE_NORMAL, convto16bitmode(s_mpeg4d_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
		vpp_algo.setDebug(DBG_MODE_NORMAL, convto16bitmode(s_mpeg4d_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
	}
	mp4d_init_buffer_link(bbm_desc.nPhysicalAddress, &bbm_desc);
#endif

//+ER 354962
#ifdef __MPEG4DEC_ARM_NMF
	iCommandAck.pendingCommandAck();
#endif
//-ER 354962
	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec Ddep: setNeeds()");
}

PUT_PRAGMA
void METH(setConfig)(t_uint16 error_reporting, t_uint16 valueDecision, t_uint32 tempValue)
{
    //if valueDecision == 1 behave like normal setconfig
    if (valueDecision == 1) {
    
#ifdef __MPEG4DEC_ARM_NMF
	//printf("\n DDEP_ARM::setConfig enter");
	//printf("\n DDEP_ARM::setConfig exit");
	//currently not doing anything for arm-nmf
#else
	t_uint32 x = 0x00;
	ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flv_version = SVP_SHAREDMEM_TO16(x); //do we need this
#endif
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec Ddep: setConfig()");
	OstTraceInt1(TRACE_FLOW, "setConfig error_reporting =%d\n",error_reporting);

	/* +Change start for ER335883 */
	OstTraceInt1(TRACE_FLOW, "process::OMX_CommandPortDisable Error_Reporting =%d  \n",Error_Reporting);
	Error_Reporting=Error_Reporting | error_reporting;
	OstTraceInt1(TRACE_FLOW, "process::OMX_CommandPortDisable Error_Reporting =%d  \n",Error_Reporting);
	/* -Change end for ER335883 */

//+ER 354962
#ifdef __MPEG4DEC_ARM_NMF
	iCommandAck.pendingCommandAck();
#endif
//-ER 354962
    }

    /* valueDecision == 2 then change for immediate I frame, ER 425364*/ 
    if (valueDecision == 2) {
#ifndef __MPEG4DEC_ARM_NMF
        OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec Ddep: setBufferForI() ENTRY");//buffer_i_frame[0] (Physical) = 0x%x, buffer_i_frame[1] (Logical) = 0x%x", buffer_i_frame[0], buffer_i_frame[1]);

        buffer_i_frame_physical = tempValue;

        OstTraceInt1(TRACE_FLOW, "setBufferForI(): buffer_i_frame_physical = 0x%x", buffer_i_frame_physical);

        OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Ddep: setBufferForI()");
#endif

    }

   /* valueDecision == 3 then change for immediate I frame, ER 425364*/
    if (valueDecision == 3) {
#ifndef __MPEG4DEC_ARM_NMF
    OstTraceInt1(TRACE_FLOW, "=> MPEG4Dec Ddep: sendImmediateIFlagToMpc(): immediate_i_flag = %d", tempValue);

    immediateIFlag = tempValue;

    OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Ddep: sendImmediateIFlagToMpc()");
#endif
    }

   /* valueDecision == 9 then , release one output buffer from queue, ER451628
	bufOut is not being released */
	if(valueDecision == 9){
	#ifndef __MPEG4DEC_ARM_NMF
	if(Port_queuedBufferCount(&mPorts[1]))
        {
            ReleaseBuffer(1,Port_dequeueBuffer(&mPorts[1]));
        }
	#endif
	}

	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Ddep: setConfig()");
}

PUT_PRAGMA
void METH(setParameter)(t_uint32 deblocking_param_struct[10], t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vdc_mpeg4_param_desc,void *mpc_vfm_mem_ctxt)
{
#ifdef __MPEG4DEC_ARM_NMF
	t_uint32 physical_address_desc = ddep_vdc_mpeg4_param_desc.nLogicalAddress;
#else
	t_uint32 physical_address_desc = ddep_vdc_mpeg4_param_desc.nPhysicalAddress;
#endif
    ts_ddep_vdc_mpeg4_param_desc SHAREDMEM *null_desc = (ts_ddep_vdc_mpeg4_param_desc SHAREDMEM *)NULL;
	t_uint32 i;
	OstTraceFiltInst0(TRACE_FLOW, "=> MPEG4Dec Ddep: setParameter()");
    save_memory_context =   mpc_vfm_mem_ctxt;
	//PRINT_VAR(ddep_vdc_mpeg4_param_desc.nSize);
	//printf("\n DDEP_ARM::setParameter enter");
	for(i=0;i<10;i++)
	{
		//PRINT_VAR(SVP_SHAREDMEM_FROM24(deblocking_param_struct[i]));
		reference_array[i]=deblocking_param_struct[i];

	}
	//PRINT_VAR(SVP_SHAREDMEM_TO16(deblocking_param_struct[4]));
	//PRINT_VAR(SVP_SHAREDMEM_TO16(deblocking_param_struct[8]));
	//PRINT_VAR(SVP_SHAREDMEM_FROM16(deblocking_param_struct[4]));
	//PRINT_VAR(SVP_SHAREDMEM_FROM16(deblocking_param_struct[8]));
	//PRINT_VAR(SVP_SHAREDMEM_FROM24(deblocking_param_struct[4]));
	//PRINT_VAR(SVP_SHAREDMEM_FROM24(deblocking_param_struct[8]));
	//PRINT_VAR(deblocking_param_struct[4]);
	//PRINT_VAR(deblocking_param_struct[8]);
	//PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[4]));
	//PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[8]));
	//PRINT_VAR(reference_array[4]);
	//PRINT_VAR(reference_array[8]);
	//PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[0]));
    //PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[1]));
    //PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[2]));
    //PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[3]));
    //PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[4]));
    //PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[5]));
    //PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[6]));
    //PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[7]));
#ifdef __MPEG4DEC_ARM_NMF
	ps_ddep_vdc_mpeg4_param_desc = (ts_ddep_vdc_mpeg4_param_desc SHAREDMEM *)ddep_vdc_mpeg4_param_desc.nLogicalAddress;
#else
	//keep mpc address for dynamic configuration
	ps_ddep_vdc_mpeg4_param_desc = (ts_ddep_vdc_mpeg4_param_desc SHAREDMEM *)ddep_vdc_mpeg4_param_desc.nMpcAddress;
	ddep_vdc_mpeg4_param_desc.nSize>>=1;
	//PRINT_VAR(ddep_vdc_mpeg4_param_desc.nSize);
	//PRINT_VAR(sizeof(param));
	//PRINT_VAR(sizeof(codec_param_type));
	param.algoId = algoId;
#endif
	// now set physical addresses
	//OPTIMIZATION POINT
	param.addr_in_frame_buffer      = physical_address_desc + ((t_uint32)(&null_desc->s_in_frame_buffer         ))*(t_uint32)MULTIPLYING_FACTOR;
	param.addr_out_frame_buffer     = physical_address_desc + ((t_uint32)(&null_desc->s_out_fram_buffer         ))*(t_uint32)MULTIPLYING_FACTOR;
	param.addr_internal_buffer      = physical_address_desc + ((t_uint32)(&null_desc->s_internal_buffer         ))*(t_uint32)MULTIPLYING_FACTOR;
	param.addr_in_bitstream_buffer  = physical_address_desc + ((t_uint32)(&null_desc->s_in_out_bitstream_buffer ))*(t_uint32)MULTIPLYING_FACTOR;
	param.addr_out_bitstream_buffer = physical_address_desc + ((t_uint32)(&null_desc->s_out_bitstream_buffer 	))*(t_uint32)MULTIPLYING_FACTOR;
	param.addr_in_parameters        = physical_address_desc + ((t_uint32)(&null_desc->s_in_parameters           ))*(t_uint32)MULTIPLYING_FACTOR;
	param.addr_out_parameters       = physical_address_desc + ((t_uint32)(&null_desc->s_out_parameters          ))*(t_uint32)MULTIPLYING_FACTOR;
	param.addr_in_frame_parameters  = physical_address_desc + ((t_uint32)(&null_desc->s_in_out_frame_parameters ))*(t_uint32)MULTIPLYING_FACTOR;
	param.addr_out_frame_parameters = physical_address_desc + ((t_uint32)(&null_desc->s_out_frame_parameters	))*(t_uint32)MULTIPLYING_FACTOR;
	//PRINT_VAR(ddep_vdc_mpeg4_param_desc.nMpcAddress);
	//PRINT_VAR(ddep_vdc_mpeg4_param_desc.nPhysicalAddress);
	ddep_vdc_mpeg4_param_desc.nMpcAddress += sizeof(ts_ddep_vdc_mpeg4_param_desc);
	ddep_vdc_mpeg4_param_desc.nPhysicalAddress += sizeof(ts_ddep_vdc_mpeg4_param_desc)*(t_uint32)MULTIPLYING_FACTOR;
	//PRINT_VAR(ddep_vdc_mpeg4_param_desc.nMpcAddress);
	//PRINT_VAR(ddep_vdc_mpeg4_param_desc.nPhysicalAddress);
	ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.backward_time=0;
	ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.forward_time=0;
    ddep_vdc_mpeg4_param_desc.nSize -= sizeof(ts_ddep_vdc_mpeg4_param_desc);
	//PRINT_VAR(sizeof(ts_ddep_vdc_mpeg4_param_desc));
	//PRINT_VAR(ddep_vdc_mpeg4_param_desc.nSize);
	//Set Header Buffers memory here
	//FIXME remove this from ddep
    if((ddep_vdc_mpeg4_param_desc.nSize/NO_OF_HEADERS) < sizeof(ts_ddep_vdc_mpeg4_header_desc))
    {
        //printf("\n Header buffer memory not enough \n");   //send error event to proxy
    }
	else
	{

		ddep_vdc_mpeg4_param_desc.nSize -= NO_OF_HEADERS*sizeof(ts_ddep_vdc_mpeg4_header_desc);
		ddep_vdc_mpeg4_param_desc.nPhysicalAddress += NO_OF_HEADERS*sizeof(ts_ddep_vdc_mpeg4_header_desc)*(t_uint32)MULTIPLYING_FACTOR;
#ifdef __MPEG4DEC_ARM_NMF

		ddep_vdc_mpeg4_param_desc.nLogicalAddress += NO_OF_HEADERS*sizeof(ts_ddep_vdc_mpeg4_header_desc);
        mp4d_create_fake_buffer(ddep_vdc_mpeg4_param_desc.nLogicalAddress,&ddep_vdc_mpeg4_param_desc);
#else
        ddep_vdc_mpeg4_param_desc.nMpcAddress += NO_OF_HEADERS*sizeof(ts_ddep_vdc_mpeg4_header_desc);
        mp4d_create_fake_buffer(ddep_vdc_mpeg4_param_desc.nPhysicalAddress,&ddep_vdc_mpeg4_param_desc);
		//PRINT_VAR(sizeof(ts_ddep_vdc_mpeg4_header_desc));
		//PRINT_VAR(ddep_vdc_mpeg4_param_desc.nMpcAddress);
		//PRINT_VAR(ddep_vdc_mpeg4_param_desc.nPhysicalAddress);
#endif
	}

	ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag = 0;

	dpb_buffer.fw_reference_buffer[0]=0;
	dpb_buffer.fw_reference_buffer[1]=0;
	dpb_buffer.bw_reference_buffer[0]=0;
	dpb_buffer.bw_reference_buffer[1]=0;
	dpb_buffer.bw_reference_buffer[2]=0;
	dpb_buffer.curr_fw_buffer_index = 0;
	dpb_buffer.curr_bw_buffer_index = 1; // why??
    dpb_buffer.dpb_IP_full = OMX_FALSE;
	dpb_buffer.dpb_full = OMX_FALSE;
	dpb_buffer.buffer_copied = OMX_FALSE;

	{
	  dblk_dpb_buffer.buffout_reference=0;
	  dblk_dpb_buffer.bw_reference_buffer=0;
	  dblk_dpb_buffer.Last_vop_not_coded=0;
	  dblk_dpb_buffer.B_buffer_index=0;
	  dblk_dpb_buffer.IP_buffer_index=0;
	  dblk_dpb_buffer.last_coding_type=0;
	}
	//printf("\n DDEP_ARM::setParameter exit");

//+ER 354962
#ifdef __MPEG4DEC_ARM_NMF
	iCommandAck.pendingCommandAck();
#endif
//-ER 354962
	OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec Ddep: setParameter()");
}


STATIC_FLAG void COMP(release_resources)(void)
{
#ifdef __MPEG4DEC_ARM_NMF
    if(get_vpp_resource)
	{
        get_vpp_resource=0;
    }
	else
	{
	   if (get_hw_resource)
	   {
		   get_hw_resource=0;
	   }
	}
#else
    // the HW resource must be released to avoid dead lock
    ENTER_CRITICAL_SECTION;
	if(get_vpp_resource)
	{
		OstTraceFiltInst0(TRACE_FLOW, "release_resources VPP \n");
		//printf("\n release_resources get_vpp_resource entering");
		iResource.freeResource(RESOURCE_VPP, &iInformResourceStatus);
        get_vpp_resource=0;
    }
	else
	{
	   if (get_hw_resource)
	   {
		   OstTraceFiltInst0(TRACE_FLOW, "release_resources  \n");
		   //printf("\n release_resources get_hw_resource entering");
		   iResource.freeResource(RESOURCE_MMDSP,&iInformResourceStatus);
		   get_hw_resource=0;
	   }
	}

    EXIT_CRITICAL_SECTION;
    //printf("\n release_resources exiting");
#endif
}
//+ER344943
#ifdef __MPEG4DEC_ARM_NMF
PUT_PRAGMA
void METH(endAlgoDeblock)(t_t1xhv_status status,
        t_t1xhv_decoder_info errors,
        t_uint32 durationInTicks,
        t_uint32 bitstream_size)
{
	OstTraceInt2(TRACE_FLOW, "endAlgoDeblock status =%d errors=%d \n",status,errors);
    OstTraceInt2(TRACE_FLOW, "endAlgoDeblock durationInTicks =%X bitstream_size=%X \n",durationInTicks,bitstream_size);
    if(durationInTicks)
        return;
    OstTraceInt2(TRACE_FLOW, "endAlgoDeblock decoding_complete status =%d deblocking_complete=%d \n",decoding_complete,deblocking_complete);
    //do copy if required
    VFM_CacheInvalidate(save_memory_context,bufOut->ADDRESS,bufOut->nAllocLen);
    if(((frameWidth % 32) != 0) && ((frameHeight % 32) != 0))
    {    
         t_uint32 temp,align_temp ;
         temp=    (((frameWidth * frameHeight * 5)/4));
         align_temp = (temp + 127) & (~0x7f)  ;
         
         OstTraceInt1(TRACE_FLOW, "endAlgoDeblock COPY CR data copied =%d \n",((frameWidth * frameHeight)/4));
         
         memcpy((t_uint8 *)(reference_array[8]),(t_uint8 *)(bufOut->ADDRESS + align_temp),((frameWidth * frameHeight)/4));
         OstTraceInt2(TRACE_FLOW, "copied to  =%X copied from=%X\n",(t_uint32)reference_array[8],(t_uint32)(bufOut->ADDRESS + align_temp));
         
         memcpy((t_uint8 *)(bufOut->ADDRESS + temp),(t_uint8 *)(reference_array[8]),((frameWidth * frameHeight)/4));
         OstTraceInt2(TRACE_FLOW, "copied to  =%X copied from=%X\n",(t_uint32)(bufOut->ADDRESS + temp),(t_uint32)reference_array[8]);
    }

//+ER352606
	iSendLogEvent.registerEvents(status,errors,durationInTicks,bitstream_size,DEBLOCKING_EVENT);

}

PUT_PRAGMA
void METH(registerEvents)(t_t1xhv_status status,
        t_t1xhv_decoder_info errors,
        t_uint32 durationInTicks,
        t_uint32 bitstream_size,
		t_uint8 event_type)
{
	if(event_type==DECODING_EVENT)
	{
		decoding_complete =1;
	}

	if(event_type==DEBLOCKING_EVENT)
	{
		deblocking_complete =1;
	}

	if((decoding_complete == 1) && (deblocking_complete == 1))
	{
		decoding_complete=0;
		deblocking_complete=0;
		iSendLogEvent.eventEndAlgo(status,errors,durationInTicks,bitstream_size);
	}
}
//-ER352606
#endif
//-ER344943
PUT_PRAGMA
void METH(endAlgo)(t_t1xhv_status status,
        t_t1xhv_decoder_info errors,
        t_uint32 durationInTicks,
        t_uint32 bitstream_size)
{
#ifndef __MPEG4DEC_ARM_NMF
	 ticks_endalgo=ITC_GET_TIMER_32_MY();
	 ticks_decode=ticks_endalgo-ticks_controlalgo;
	OstTraceInt2(TRACE_FLOW, "endAlgo ticks_endalgo =%d ticks_decode=%d \n",ticks_endalgo,ticks_decode);
#endif
	OstTraceInt2(TRACE_FLOW, "=> MPEG4Dec Ddep: endAlgo(): status = %d, errors = %d", status, errors);
//+ER344943
#ifdef __MPEG4DEC_ARM_NMF
	 OstTraceInt2(TRACE_FLOW, "endAlgo decoding_complete status =%d deblocking_complete=%d \n",decoding_complete,deblocking_complete);

	//+ER352606
	iSendLogEvent.registerEvents(status,errors,durationInTicks,bitstream_size,DECODING_EVENT);
	//-ER352606
#else
	OstTraceInt1(TRACE_FLOW, "endAlgo=%d durationInticks \n",durationInTicks);
	 iSendLogEvent.eventEndAlgo(status,errors,durationInTicks,bitstream_size);
#endif
//-ER344943
}

PUT_PRAGMA
void METH(eventEndAlgo)(t_t1xhv_status status,
        t_t1xhv_decoder_info errors,
        t_uint32 durationInTicks,
        t_uint32 bitstream_size)
{
	t_uint32 i;
#ifdef __MPEG4DEC_ARM_NMF
	OMX_BUFFERHEADERTYPE_p ea_bufIn;
	//+ER352805
	OMX_BUFFERHEADERTYPE_p ea_bufIn_hold = 0;
	//-ER352805
#else
	Buffer_p ea_bufIn;
	//+ER352805
	Buffer_p ea_bufIn_hold = 0;
	//-ER352805
#endif
#ifndef __MPEG4DEC_ARM_NMF
	//NmfPrint0(2,"\nEEAE");
#endif
	OstTraceInt2(TRACE_FLOW, "=> MPEG4Dec Ddep: eventEndAlgo(): status = %d, errors = %d", status, errors);

	OstTraceInt2(TRACE_FLOW, "MPEG4Dec Ddep: eventEndAlgo(): durationInTicks = %d, bitstream_size = %d", durationInTicks, bitstream_size);

	OstTraceInt2(TRACE_FLOW, "MPEG4Dec Ddep: eventEndAlgo(): bufOut = 0x%x, bufOut->ADDRESS = 0x%x", (unsigned int)bufOut, (unsigned int)(bufOut->ADDRESS));
    //printf("\n DDEP_ARM::eventEndAlgo status =%d %d",status,errors);
	if((durationInTicks == EMPTY_BUFFER) && (bitstream_size == EMPTY_BUFFER))   //just return empty input buffers
	{
		//printf("\n DDEP_ARM::EMPTY_BUFFER");
        if(bufIn[0])
		{
			ReleaseBuffer(0,bufIn[0]);
			bufIn[0]=0;
        }
		ReleaseBuffer(2,bufheader);
		bufheader=0;
//+ER344873
		get_hw_resource_requested=0;
//-ER344873
		return ;
	}
	else if((durationInTicks == HANDLE_EOS) && (bitstream_size == HANDLE_EOS))   //case where EOS has come with filled length zero
	{
		//printf("\n DDEP_ARM::do_flushAllPortsX");
		 do_flushAllPortsX();
//+ER344873
		 get_hw_resource_requested=0;
//-ER344873
		 return ;
	}
	else if(status == STATUS_BUFFER_NEEDED)
	{
		if(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
		{
			PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
			ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
			dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = 0;
			dpb_buffer.dpb_IP_full = OMX_FALSE;
			iSendLogEvent.eventProcess();
			return;
		}
	}
    else
	{
		while(1)
		{
#ifdef __MPEG4DEC_ARM_NMF
			mp4d_get_next_not_required_bitstream_buffer(ea_bufIn);
#else
			mp4d_get_next_not_required_bitstream_buffer(&ea_bufIn);
#endif
            if (ea_bufIn==0)
			{
				//printf("\n break \n");
				break;
			}
			//PRINT_VAR(ea_bufIn);
			WAIT_FOR_BUFFER = 0;
            if (ea_bufIn && DataInBuf==0)
			{
				//FIXME for other buffer modes than Image, this should be done once
#ifdef __MPEG4DEC_ARM_NMF
				if(ea_bufIn->nTimeStamp)
					bufOut->nTimeStamp = ea_bufIn->nTimeStamp;
#else
				if(ea_bufIn->nTimeStamph || ea_bufIn->nTimeStampl)
				{
					bufOut->nTimeStamph = ea_bufIn->nTimeStamph;
					bufOut->nTimeStampl = ea_bufIn->nTimeStampl;
				}
#endif

				bufOut->FLAGS = (bufOut->FLAGS & (~BUFFERFLAG_EOS));

				//+ER352805
				OstTraceInt1(TRACE_FLOW, "eventEndAlgo::Holding release of input buffer. line = %d", __LINE__);
				ea_bufIn_hold = ea_bufIn;
				//-ER352805
				for(i=0;i<ps_ddep_vdc_mpeg4_header_desc->InBuffCnt;i++)
					bufIn[i]=0;
			   //printf("\nreturn emptyThisBuffer %X",++emptyThisBufferDoneCount);
			}
			else
			{
				//reset it once endalgo received
                DataInBuf=0; 
				BytesWrittenInInternalBuffer=0;
				NoOfCopiedBuffers=0;
			}
		}
		if(Error_Reporting & 1)  //error map
		{
			t_uint32 i;
			OstTraceInt0(TRACE_FLOW, "error map copy \n");
			//printf("\n error map copy \n");
		    ps_ddep_vdc_mpeg4_header_desc = (ts_ddep_vdc_mpeg4_header_desc SHAREDMEM *)bufheader;
		    for(i =0;i<225;i++)
		    ps_ddep_vdc_mpeg4_header_desc->errormap[i]=ps_ddep_vdc_mpeg4_param_desc->s_out_parameters.error_map[i];
		}
		if(Error_Reporting & 2)  //fast update
		{
            t_uint32 i;
			t_uint32 mb_width  =	(frameWidth + (16-(frameWidth%16))%16)/16; 
            t_uint16 first_mb, mb_num, mb_count;
			OstTraceFiltInst0(TRACE_FLOW, "fast update \n");
			ps_ddep_vdc_mpeg4_header_desc = (ts_ddep_vdc_mpeg4_header_desc SHAREDMEM *)bufheader;
			for (i=0; i<8; i++)
			{
				first_mb = ps_ddep_vdc_mpeg4_param_desc->s_out_parameters.slice_loss_first_mb[i]+1;
				mb_num = ps_ddep_vdc_mpeg4_param_desc->s_out_parameters.slice_loss_mb_num[i];
			
				if (mb_num<=0) 
				{
					continue;
				}
            
				mb_count = MINIMUM(8-(first_mb%8)+1, mb_num); // nb of mb to fill first byte
				if (mb_count < 8) 
				{ // if 8, we fill it in step 2
					mb_num -= mb_count;
				}
                if (ps_ddep_vdc_mpeg4_header_desc->mNumMBs != 0) 
				{
					t_uint32 current_first_mb = ps_ddep_vdc_mpeg4_header_desc->mFirstGOB*mb_width + ps_ddep_vdc_mpeg4_header_desc->mFirstMB;
					t_uint32 current_last_mb = current_first_mb + ps_ddep_vdc_mpeg4_header_desc->mNumMBs;
					t_uint32 last_mb = first_mb + mb_num - 1;
					if (current_first_mb<first_mb) 
					{
						first_mb = current_first_mb;
					}
					if (current_last_mb>last_mb) 
					{
						last_mb = current_last_mb;
					}
					mb_num = last_mb - first_mb + 1;
				}
				ps_ddep_vdc_mpeg4_header_desc->mFirstGOB = first_mb / mb_width;
				ps_ddep_vdc_mpeg4_header_desc->mFirstMB = first_mb % mb_width;
				ps_ddep_vdc_mpeg4_header_desc->mNumMBs = mb_num;
			}
		}
		if(Error_Reporting & 4)
		{
			if(eos_flag_set)
			{
				OstTraceInt1(TRACE_FLOW, "endAlgo=%d eos_flag_set \n",eos_flag_set);
				eos_flag_set=0;
				bufOut->FLAGS |= BUFFERFLAG_EOS;
            }
			if(bufOut)
			{
#ifndef __MPEG4DEC_ARM_NMF
					bufOut->LENGTH = (frameWidth*frameHeight*3)>>2;
#else
					bufOut->LENGTH = (frameWidth*frameHeight*3)>>1;
#endif
				ReleaseBuffer(1,bufOut);
				bufOut=0;
			}
	
		}
		else if(!reference_array[0])
		{
			//PRINT_VAR(currentVopCoded);
			if(Error_Reporting & 4)
			{
				if(bufOut)
				{
					ReleaseBuffer(1,bufOut);
					bufOut=0;
				}
			}
			if(currentVopCoded)
			{
#ifndef __MPEG4DEC_ARM_NMF
					bufOut->LENGTH = (frameWidth*frameHeight*3)>>2;
#else
					bufOut->LENGTH = (frameWidth*frameHeight*3)>>1;
#endif
				bufOut->LENGTH = SVP_SHAREDMEM_TO24(bufOut->LENGTH);
               // PRINT_VAR(currentCodingType);
				dpb_buffer.last_coding_type=currentCodingType;
				if((currentCodingType == 0)||(currentCodingType == 1)||(currentCodingType == 3))
				{
						//PRINT_VAR(dpb_buffer.dpb_IP_full);
						if(dpb_buffer.dpb_IP_full == OMX_TRUE)	// release in display order
						{
							//printf("\n RELEASE::fw_reference_buffer %d ",dpb_buffer.curr_fw_buffer_index);
							PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
							ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
							dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = 0;
							dpb_buffer.dpb_IP_full = OMX_FALSE;
						}
						//PRINT_VAR(dpb_buffer.bw_reference_buffer[1]);
						if(dpb_buffer.bw_reference_buffer[1]) // release in display order
						{
							//printf("\n RELEASE::bw_reference_buffer 1 ");
							//PRINT_VAR(dpb_buffer.bw_reference_buffer[1]);
							ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[1]);
							dpb_buffer.bw_reference_buffer[1] = 0;
						}
						//PRINT_VAR(dpb_buffer.bw_reference_buffer[2]);
						if(dpb_buffer.bw_reference_buffer[2]) // release in display order
						{
							//printf("\n RELEASE::bw_reference_buffer 2 ");
							PRINT_VAR(dpb_buffer.bw_reference_buffer[2]);
							ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[2]);
							dpb_buffer.bw_reference_buffer[2] = 0;
						}
						if((currentCodingType == 0) && (dpb_buffer.dpb_IP_full == OMX_TRUE))
						{
							//printf("\n RELEASE::fw_reference_buffer 2 %d \n",((dpb_buffer.curr_fw_buffer_index+1)%2));
							PRINT_VAR(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]);
							ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]);
							dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2] = 0;
							dpb_buffer.curr_fw_buffer_index = 0;
						}
						OstTraceInt1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);	

						#ifndef __MPEG4DEC_ARM_NMF
							OstTraceInt2(TRACE_FLOW, "MPEG4Dec DDep: eventEndAlgo(): currentCodingType = %d, immediateIFlag = %d", currentCodingType, immediateIFlag);
							//If this was an I frame and the immediateIFlag was TRUE
							if((currentCodingType == 0) && (immediateIFlag))
							{
								OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec DDep: eventEndAlgo(): Releasing bufOut and setting references for immediate I frame release");

								//Reset the flag
								immediateIFlag = 0;

								//Store the address of bufOut in the fw_reference_buffer array
								dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = bufOut;
								
								OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec DDep: eventEndAlgo(): Calling ReleaseBuffer(1, bufOut)");

								//Store the address of bufOut in some other variable too so as to check the 
								// fw_reference_buffer against this variable in processActual(). If they match, then the
								// addr_fw_ref_buffer for algo will not be this buffer but the internal buffer into which
								// it is copied.
								immediate_i_frame_bufOut = bufOut;

								//Now 'immediately' release the buffer
								ReleaseBuffer(1, bufOut);

								//Set doNotReleaseNextI so that we do not release the bufOut again later
								doNotReleaseNextI = 1;
								bufOut = 0;
							}
							else
							{
						#endif
								OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
								//We now store the bufOut as the fw_reference_buffer and discard 'bufOut'
								dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = bufOut;
								bufOut=0;
						#ifndef __MPEG4DEC_ARM_NMF
							}
						#endif

						#ifndef __MPEG4DEC_ARM_NMF
							OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec DDep: eventEndAlgo(): dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = 0x%x", dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
						#endif
						
						//PRINT_VAR(dpb_buffer.curr_fw_buffer_index);
						//PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
						dpb_buffer.curr_fw_buffer_index = (dpb_buffer.curr_fw_buffer_index + 1) % 2;

						if((dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] !=0)
						   &&(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2] !=0))
						{
							OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
							//PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
							//PRINT_VAR(dpb_buffer.fw_reference_buffer[(dpb_buffer.curr_fw_buffer_index+1)%2]);
                            dpb_buffer.dpb_IP_full = OMX_TRUE;
							//PRINT_VAR(dpb_buffer.dpb_IP_full);
						}
						else
						{
							dpb_buffer.dpb_IP_full = OMX_FALSE;
						}

						OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);

						dpb_buffer.dpb_full = OMX_FALSE;
						dpb_buffer.buffer_copied = OMX_FALSE;
						dpb_buffer.curr_bw_buffer_index = 1;
				}
				OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
				if(currentCodingType == 2)
				{
						OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
						if(dpb_buffer.curr_bw_buffer_index == 2)
						{
							dpb_buffer.dpb_full = OMX_TRUE;
							OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
						}
						if((dpb_buffer.dpb_full == OMX_TRUE)
						   &&(dpb_buffer.buffer_copied == OMX_TRUE))
						{
							OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
							PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
							if(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
							{
								OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
								ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
								dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = 0;
								dpb_buffer.dpb_IP_full = OMX_FALSE;
							}

							PRINT_VAR(dpb_buffer.bw_reference_buffer[1]);
							if(dpb_buffer.bw_reference_buffer[1])
							{
								OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
							  ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[1]);
							  dpb_buffer.bw_reference_buffer[1] = 0;

							}
							PRINT_VAR(dpb_buffer.bw_reference_buffer[2]);
							if(dpb_buffer.bw_reference_buffer[2])
							{
								OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
								ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[2]);
								dpb_buffer.bw_reference_buffer[2] = 0;
							}

							dpb_buffer.dpb_full = OMX_FALSE;
						}

						OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
						if((dpb_buffer.buffer_copied == OMX_TRUE) &&
						   (dpb_buffer.dpb_full == OMX_FALSE))
						{
							PRINT_VAR(bufOut);
							if(bufOut)
							{
								ReleaseBuffer(1,bufOut);
								bufOut=0;
							}
						}
						else
						{
							OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
							dpb_buffer.bw_reference_buffer[dpb_buffer.curr_bw_buffer_index] = bufOut;
							PRINT_VAR(dpb_buffer.bw_reference_buffer[dpb_buffer.curr_bw_buffer_index]);
							bufOut=0;
                            dpb_buffer.curr_bw_buffer_index = (dpb_buffer.curr_bw_buffer_index + 1)%3;

							if(dpb_buffer.curr_bw_buffer_index == 0)
								dpb_buffer.curr_bw_buffer_index = 1; // never access index 0
						}

				}

				OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
			}
			else
			{
				if(!currentVopCoded)
				{
					OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
					PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
					if(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
					{
						OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
						ReleaseBuffer(1,dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
						dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = 0;
						dpb_buffer.dpb_IP_full = OMX_FALSE;
					}

					OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
					PRINT_VAR(dpb_buffer.bw_reference_buffer[1]);
					if(dpb_buffer.bw_reference_buffer[1])
					{
						OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
					  ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[1]);
					  dpb_buffer.bw_reference_buffer[1] = 0;

			   				 }
					OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
					PRINT_VAR(dpb_buffer.bw_reference_buffer[2]);
					if(dpb_buffer.bw_reference_buffer[2])
					{
						OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
						ReleaseBuffer(1,dpb_buffer.bw_reference_buffer[2]);
						dpb_buffer.bw_reference_buffer[2] = 0;
					}
				}
						OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);

                //printf(" VOP_NOT_CODED");
				if(bufOut)
				{
#ifndef __MPEG4DEC_ARM_NMF
					bufOut->LENGTH = (frameWidth*frameHeight*3)>>2;
#else
					bufOut->LENGTH = (frameWidth*frameHeight*3)>>1;
#endif
				bufOut->LENGTH = SVP_SHAREDMEM_TO24(bufOut->LENGTH);
				//PRINT_VAR(SVP_SHAREDMEM_TO24(bufOut->LENGTH));
				PRINT_VAR(bufOut);
				OstTraceFiltInst1(TRACE_FLOW, "eventEndAlgo line %d", __LINE__);
					ReleaseBuffer(1,bufOut);
					bufOut=0;
				}
			}
		}
		else  //for deblocking enabled
		{
			if(currentVopCoded)
			{
#ifndef __MPEG4DEC_ARM_NMF
					bufOut->LENGTH = (frameWidth*frameHeight*3)>>2;
#else
					bufOut->LENGTH = (frameWidth*frameHeight*3)>>1;
#endif
				bufOut->LENGTH = SVP_SHAREDMEM_TO24(bufOut->LENGTH);
                if(!firstFramedecoded)
				{
					OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Ddep: eventEndAlgo(): line %d firstFramedecoded is TRUE, setting dblk_dpb_buffer.buffout_reference", __LINE__); 
					if(dblk_dpb_buffer.buffout_reference == 0) {
						dblk_dpb_buffer.buffout_reference=bufOut;
						bufOut=0;
					}
				}
                //PRINT_VAR(currentCodingType);
				if(!(currentCodingType==2)) //for I and P
				{
						//PRINT_VAR(bufOut);
						if(bufOut)	// release in display order
						{
							OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Ddep: eventEndAlgo(): line %d Releasing bufOut", __LINE__);

							//PRINT_VAR(bufOut);
							ReleaseBuffer(1,bufOut);
							bufOut=0;
						}
						//PRINT_VAR(dblk_dpb_buffer.bw_reference_buffer);
						if(dblk_dpb_buffer.bw_reference_buffer) // release bw_reference_buffer
						{
							//PRINT_VAR(dblk_dpb_buffer.bw_reference_buffer);
							ReleaseBuffer(1,dblk_dpb_buffer.bw_reference_buffer);
							dblk_dpb_buffer.bw_reference_buffer = 0;
						}
						if(free_references && dblk_dpb_buffer.buffout_reference) //not in display order::to be corrected
						{
							OstTraceFiltInst1(TRACE_FLOW, "MPEG4Dec Ddep: eventEndAlgo(): line %d Releasing dblk_dpb_buffer.buffout_reference", __LINE__); 

						#ifndef __MPEG4DEC_ARM_NMF
							if(!immediateIFlag) {
						#endif
								//If control has come here in a case other than 
								// immediateIFlag case (which means Flush/Idle/Disable/InputEOS cases),
								// then also set EOS flag while releasing the output buffer
								dblk_dpb_buffer.buffout_reference->FLAGS |= BUFFERFLAG_EOS;
								//proxy.eventHandler(OMX_EventBufferFlag, 1,dblk_dpb_buffer.buffout_reference->FLAGS);
						#ifndef __MPEG4DEC_ARM_NMF
							}
						#endif

							//PRINT_VAR(dblk_dpb_buffer.buffout_reference);
							ReleaseBuffer(1,dblk_dpb_buffer.buffout_reference);
							dblk_dpb_buffer.buffout_reference = 0;
						}
				}
				else //for B
				{
						//PRINT_VAR(dblk_dpb_buffer.last_coding_type);
						if(dblk_dpb_buffer.last_coding_type==2 )	// for consecutive B
						{

							//PRINT_VAR(bufOut);
							ReleaseBuffer(1,bufOut);
							bufOut=0;
						}
						else  // for a single B frame
						{

dblk_dpb_buffer.bw_reference_buffer=bufOut;
							 bufOut=0;
						}


				}
				dblk_dpb_buffer.last_coding_type = currentCodingType;
			}
			else
			{
				ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.decode_flag=1;
				dblk_dpb_buffer.Last_vop_not_coded=1;
				//printf(" VOP_NOT_CODED");
#ifndef __MPEG4DEC_ARM_NMF
					bufOut->LENGTH = (frameWidth*frameHeight*3)>>2;
#else
					bufOut->LENGTH = (frameWidth*frameHeight*3)>>1;
#endif
				bufOut->LENGTH = SVP_SHAREDMEM_TO24(bufOut->LENGTH);
				//dblk_dpb_buffer.last_coding_type = currentCodingType;
				//PRINT_VAR(SVP_SHAREDMEM_TO24(bufOut->LENGTH));
				//PRINT_VAR(bufOut);
				ReleaseBuffer(1,bufOut);
				bufOut=0;
			}

		}
	}
    //PRINT_VAR(bufheader);
	//PRINT_VAR(status);

	//+ER352805
	// Input buffer is held, and not yet released.
	if(ea_bufIn_hold)
	{
		ReleaseBuffer(0,ea_bufIn_hold);
		// Not required, but still:
		ea_bufIn_hold = 0;
	}
	//-ER352805

	if (bufheader && status != STATUS_BUFFER_NEEDED )
	{
		//PRINT_VAR(bufHeaderIn);
        ReleaseBuffer(2,bufheader);
        bufheader=0;
    }
    //Set firstFramedecoded to True; Atlease one frame is decoded; CAUTION: this is not for debug ;)
    if(!firstFramedecoded)
	{
        firstFramedecoded = 1;
		//PRINT_VAR(firstFramedecoded);
	}
    // Should we really released the HW on a buffer based
    release_resources();

#ifndef __MPEG4DEC_ARM_NMF
	if(free_references && immediateIFlag)
	{
		OstTraceInt0(TRACE_FLOW, "MPEG4Dec Ddep: eventEndAlgo(): Reseting free_references and immediateIFlag");
		immediateIFlag = 0;
		free_references = 0;
		lastIWasReleased = 1;
		//We set firstFramedecoded to false so that if decoding continues, i.e. a B/P frame is sent next,
		// then it will be treated as first frame in processActual(), so no deblocking will be done
		// on previously decoded frame (Which in our case was the I frame and has already been released),
		// and bufOut will be stored in buffout_reference.
		// This is useful because at the end of decoding of next frame, there will be no bufOut and hence
		// no buffer will be released. So no need to handle the re-release of the I frame buffer.
		// The only difference would be that the decoded I frame has been copied into an internal buffer
		// and this will be now used as reference for the next B/P frame.
		firstFramedecoded = 0;
	}
	else if(free_references)
    {
		OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec Ddep: eventEndAlgo(): Calling do_flushAllPorts();");
		//PRINT_VAR(free_references);
        do_flushAllPorts();
        free_references = 0;
    }
#else
	if(free_references)
    {
		OstTraceFiltInst0(TRACE_FLOW, "MPEG4Dec Ddep: eventEndAlgo(): Calling do_flushAllPorts();");
		//PRINT_VAR(free_references);
        do_flushAllPorts();
        free_references = 0;
    }
#endif

	if(defer_pause)
	{
		OstTraceInt0(TRACE_FLOW, "defer_pause \n");
		defer_pause = 0;
#ifdef 	__MPEG4DEC_ARM_NMF
	Component::sendCommand(OMX_CommandStateSet, OMX_StatePause); 
#else
	Component_sendCommand(&mDataDep, OMX_CommandStateSet, OMX_StatePause);
#endif
	}
	//if(compState==OMX_StateExecuting)
	OstTraceInt0(TRACE_FLOW, "MPEG4Dec Ddep: eventEndAlgo(): Calling eventProcess()");
	iSendLogEvent.eventProcess();
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Ddep: eventEndAlgo()");

    return;
}

#ifdef __MPEG4DEC_ARM_NMF
STATIC_FLAG void COMP(reset)()
#else
static void reset(struct Component *this)
#endif
{
		OstTraceFiltInst0(TRACE_FLOW, "reset \n");
       	//printf("\nreset");
	release_resources();
}

void COMP(disablePortIndication)(t_uint32 portIdx)
{
		OstTraceInt1(TRACE_FLOW, "disablePortIndication mpc portIdx =%d\n",portIdx);
        //printf("\n DDEP_ARM::disablePortIndication enter ");
		if(portIdx==2)
		{
		   header_port_enabled=0;
		   //PRINT_VAR(header_port_enabled);
		}
		PRINT_VAR(portIdx);
}

void COMP(enablePortIndication)(t_uint32 portIdx)
{
		OstTraceInt1(TRACE_FLOW, "enablePortIndication mpc portIdx=%d\n",portIdx);
       //printf("\n DDEP_ARM::enablePortIndication enter ");
	   if(portIdx==2)
		{
		   header_port_enabled=1;
		   PRINT_VAR(header_port_enabled);
		}
	   PRINT_VAR(portIdx);
}

void COMP(flushPortIndication)(t_uint32 portIdx)
{
	OstTraceFiltInst1(TRACE_FLOW, "flushPortIndication mpc portIdx=%d\n",portIdx);
	//printf("\n DDEP_ARM::flushPortIndication enter ");
	PRINT_VAR(portIdx);
}

// Process function is called only if component is in executing mode
// Thanks to the FSM
#ifdef __MPEG4DEC_ARM_NMF
void METH(process)()
#else
static void process(Component *this)
#endif
{
	t_uint32 i;
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec Ddep: process()");
#ifndef __MPEG4DEC_ARM_NMF
	//NmfPrint0(2,"\nPE");
#endif
	//printf("\n DDEP_ARM::process enter ");
    if (get_vpp_resource)
	{
		// README::resource already aquired and in use
		OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec Ddep: process(): get_vpp_resource");
		return;
	}
    if (CopyOperationOngoing)
	{
		// README::CopyOperationOngoing
		OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec Ddep: process(): CopyOperationOngoing");
		return;
	}
	if (get_hw_resource)
	{
		// README::resource already aquired and in use
		OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec Ddep: process(): get_hw_resource");
		return;
	}
	if (get_hw_resource_requested)
	{
		// README::resource already requested and in use
		OstTraceFiltInst0(TRACE_FLOW, "<= MPEG4Dec Ddep: process(): get_hw_resource_requested");
		return;
	}
#ifndef __MPEG4DEC_ARM_NMF
	//if((Port_queuedBufferCount(&mPorts[0])==MaxBufInput))
		//printf("\n DDEP_MPC::process MaxBufInput =%d bufcount=%d ",MaxBufInput,Port_queuedBufferCount(&mPorts[0]));
#endif
	//README::all input buffer consumed ,do the copy operation 
#ifdef __MPEG4DEC_ARM_NMF
	if(( ! buffer_available_atheaderport()) && (mPorts[0].queuedBufferCount()==MaxBufInput))
#else
	if((! buffer_available_atheaderport(&mDataDep) )&& (Port_queuedBufferCount(&mPorts[0])==MaxBufInput))
#endif
	{
//+ER344873
		return;//temporary fix::BPL wont work after this ,remove buffers
//-ER344873
/*
        OstTraceFiltInst0(TRACE_FLOW, "process::all input buffer consumed ,do the copy operation  \n");
        for(i=0;i<MaxBufInput;i++)
		{
#ifdef __MPEG4DEC_ARM_NMF
			if(mPorts[0].queuedBufferCount())
			{
                bufIn[i]=mPorts[0].dequeueBuffer();
            }
#else
			if(Port_queuedBufferCount(&mPorts[0]))
			{
                bufIn[i]=Port_dequeueBuffer(&this->ports[0]);
                //printf("\n DDEP_MPC1::remove buffer input");
            }
#endif
		}
        CopyOperationOngoing=1;
		NoOfCopiedBuffers+=MaxBufInput;
		//printf("\n DDEP_MPC::  iCopy.InternalCopy %d  %d   ",MaxBufInput,BytesWrittenInInternalBuffer);
		iCopy.InternalCopy(MaxBufInput,(BytesWrittenInInternalBuffer*MULTIPLYING_FACTOR));
		return;
*/
	}
#ifdef __MPEG4DEC_ARM_NMF
	if(buffer_available_atheaderport() && buffer_available_atoutput())
#else
    if(buffer_available_atheaderport(&mDataDep)  && buffer_available_atoutput(&mDataDep))
#endif
	{
        ps_ddep_vdc_mpeg4_header_desc = (ts_ddep_vdc_mpeg4_header_desc SHAREDMEM *)bufheader;
        OstTraceInt2(TRACE_FLOW, "process::dpb_IP_full=%d pictureCodingType =%d\n",dpb_buffer.dpb_IP_full,ps_ddep_vdc_mpeg4_header_desc->pictureCodingType);
        OstTraceInt1(TRACE_FLOW, "process:: vopCoded =%d\n",ps_ddep_vdc_mpeg4_header_desc->vopCoded);
        if((dpb_buffer.dpb_IP_full == OMX_TRUE) && (ps_ddep_vdc_mpeg4_header_desc->pictureCodingType != 2) && (ps_ddep_vdc_mpeg4_header_desc->vopCoded))
        {
              // release in display order
              //printf("\n RELEASE::fw_reference_buffer %d ",dpb_buffer.curr_fw_buffer_index);
             if(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
             {
                 iSendLogEvent.eventEndAlgo(STATUS_BUFFER_NEEDED,VDC_ERT_NONE,0,0);
                 return;
             }
        }
        //PRINT_VAR(NoOfCopiedBuffers);
		//PRINT_VAR(ps_ddep_vdc_mpeg4_header_desc->InBuffCnt);
		if(ps_ddep_vdc_mpeg4_header_desc->InBuffCnt>=NoOfCopiedBuffers)//check this
			ps_ddep_vdc_mpeg4_header_desc->InBuffCnt -= NoOfCopiedBuffers;
	//	if(NoOfCopiedBuffers)
			//printf("\n DDEP_MPC:: FrameContained in  =%d NoOfCopiedBuffers  =%d",ps_ddep_vdc_mpeg4_header_desc->InBuffCnt,NoOfCopiedBuffers);
		//ps_ddep_vdc_mpeg4_header_desc->InBuffCnt %= MaxBufInput;  //to be uncommented
#ifndef __MPEG4DEC_ARM_NMF
	OstTraceInt2(TRACE_FLOW, "InBuffCnt=%d ReadyToProgram =%d\n",ps_ddep_vdc_mpeg4_header_desc->InBuffCnt,ReadyToProgram);
#endif
		if(!ReadyToProgram)
		{
			for(i=0;i<ps_ddep_vdc_mpeg4_header_desc->InBuffCnt;i++)
			{
#ifdef __MPEG4DEC_ARM_NMF
				if(mPorts[0].queuedBufferCount())
				{
					if(bufIn[i])
					{
						OstTraceFiltInst0(TRACE_FLOW, "already dequeueInputBuffer\n");
					}
					else
					{
						bufIn[i]=mPorts[0].dequeueBuffer();
					}
                }
#else
				if(Port_queuedBufferCount(&mPorts[0]))
				{
					if(bufIn[i])
					{
						OstTraceFiltInst1(TRACE_FLOW, "already dequeueInputBuffer::buf=%X \n",bufIn[i]);
					}
					else
					{
						bufIn[i]=Port_dequeueBuffer(&this->ports[0]);
					}
					OstTraceFiltInst1(TRACE_FLOW, "dequeueInputBuffer::buf=%X \n",bufIn[i]);
					OstTraceFiltInst1(TRACE_FLOW, "dequeueInputBuffer::ALLOCLENGTH=%X \n",bufIn[i]->ALLOCLENGTH);
                    //PRINT_VAR(bufIn[i]->ADDRESS);
				}
#endif
              }
			  if(ps_ddep_vdc_mpeg4_header_desc->InBuffCnt>1 || (NoOfCopiedBuffers && (ps_ddep_vdc_mpeg4_header_desc->InBuffCnt==1)))
			    {
                  CopyOperationOngoing=1;
				  NoOfCopiedBuffers+=ps_ddep_vdc_mpeg4_header_desc->InBuffCnt;
				  OstTraceFiltInst0(TRACE_FLOW, "process::do the copy operation  \n");
				  //printf("\n DDEP_MPC::  iCopy.InternalCopy %d  %d   ",ps_ddep_vdc_mpeg4_header_desc->InBuffCnt,BytesWrittenInInternalBuffer);
                  iCopy.InternalCopy(ps_ddep_vdc_mpeg4_header_desc->InBuffCnt,(BytesWrittenInInternalBuffer*MULTIPLYING_FACTOR));
				  ReadyToProgram=1;
				  return;
			    }
		}
		ReadyToProgram=0;
		//README::Case where EOS comes in empty buffer
		OstTraceFiltInst2(TRACE_FLOW, "process::HANDLE_EOS  bufOut =%d FLAGS =%d\n",bufOut->ALLOCLENGTH,bufIn[0]->FLAGS);
		OstTraceFiltInst1(TRACE_FLOW, "HANDLE_EOS::ALLOCLENGTH=%X \n",bufIn[0]->ALLOCLENGTH);
		if((ps_ddep_vdc_mpeg4_header_desc->InBuffCnt ==1) && (SVP_SHAREDMEM_FROM24(bufIn[0]->LENGTH) == 0) && (bufIn[0]->FLAGS & BUFFERFLAG_EOS))
		{
			OstTraceFiltInst0(TRACE_FLOW, "process::HANDLE_EOS  \n");
//+ER344873
			get_hw_resource_requested=1;//printf("\n DDEP_ARM::process HANDLE_EOS");
//-ER344873
			iSendLogEvent.eventEndAlgo(STATUS_JOB_COMPLETE,VDC_ERT_NONE,HANDLE_EOS,HANDLE_EOS);
			return;
		}
		//if(ps_ddep_vdc_mpeg4_header_desc->cease)
			//printf("\n DDEP_ARM::process cease=%d",ps_ddep_vdc_mpeg4_header_desc->cease);
		//README::always return empty buffers (case of VOL header)
		OstTraceInt2(TRACE_FLOW, "process::EMPTY_BUFFER  height =%d width =%d\n",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width);
		OstTraceInt4(TRACE_FLOW, "process: ps_ddep_vdc_mpeg4_header_desc->cease = %d, ps_ddep_vdc_mpeg4_header_desc->InBuffCnt = %d, SVP_SHAREDMEM_FROM24(bufIn[0]->LENGTH) = %d, bufIn[0]->FLAGS = %d", ps_ddep_vdc_mpeg4_header_desc->cease, ps_ddep_vdc_mpeg4_header_desc->InBuffCnt, SVP_SHAREDMEM_FROM24(bufIn[0]->LENGTH), bufIn[0]->FLAGS);
		if((ps_ddep_vdc_mpeg4_header_desc->cease==0) && (ps_ddep_vdc_mpeg4_header_desc->InBuffCnt == 1) && (SVP_SHAREDMEM_FROM24(bufIn[0]->LENGTH) == 0) && (!(bufIn[0]->FLAGS & BUFFERFLAG_EOS)))
		{
			OstTraceFiltInst0(TRACE_FLOW, "process::EMPTY_BUFFER  \n");
//+ER344873
			get_hw_resource_requested=1;//printf("\n DDEP_ARM::process EMPTY_BUFFER");
//-ER344873
			iSendLogEvent.eventEndAlgo(STATUS_JOB_COMPLETE,VDC_ERT_NONE,EMPTY_BUFFER,EMPTY_BUFFER);   
			return;
		}
        //README::checking condition for cease for SH
		/* +Change start for ER335883 */
		OstTraceFiltInst2(TRACE_FLOW, "process::EMPTY_BUFFER Error_Reporting =%d  cease =%d\n",Error_Reporting,ps_ddep_vdc_mpeg4_header_desc->cease);
        if((Error_Reporting & 8) || (ps_ddep_vdc_mpeg4_header_desc->cease))
		{
		/* -Change end for ER335883 */
#ifndef __MPEG4DEC_ARM_NMF
	//NmfPrint0(2,"\nCEASE");
#endif
				OstTraceFiltInst2(TRACE_FLOW, "CEASE_AT_DDEP Error_Reporting =%d cease =%d\n",Error_Reporting,ps_ddep_vdc_mpeg4_header_desc->cease);
                PRINT_VAR(ps_ddep_vdc_mpeg4_header_desc->cease);
                return;
		}	
		//README::process input buffer with non-zero filled length
		{
				//printf("\n iResource.allocResource entering");
				currentVopCoded=(ps_ddep_vdc_mpeg4_header_desc->vopCoded);
				currentCodingType = ((ps_ddep_vdc_mpeg4_header_desc->pictureCodingType));
				OstTraceFiltInst2(TRACE_FLOW, "process::currentVopCoded=%d currentCodingType=%d",currentVopCoded,currentCodingType);
				//README::for third consecutive B frame do a copy using vpp
				if((dpb_buffer.buffer_copied==OMX_FALSE) && (currentCodingType == 2) && (dpb_buffer.dpb_full == OMX_TRUE))
				{
					OstTraceFiltInst0(TRACE_FLOW, "process::VPP for B\n");
					//PRINT_VAR(ps_ddep_vdc_mpeg4_header_desc->vopCoded);
				    //printf("\n allocated:::RESOURCE_VPP currentCodingType=%d ",currentCodingType);
					get_hw_resource_requested = 1;
#ifdef __MPEG4DEC_ARM_NMF
					iSendLogEvent.event_informResourceStatus(0,0);
#else
					iResource.allocResource(RESOURCE_VPP, &iInformResourceStatus);
#endif            				//README::allocate resource in                    
								//1:: if deblocking enabled                       
				}  				//2::vopCoded is true when deblocking is disabled 
                else if(ps_ddep_vdc_mpeg4_header_desc->vopCoded || reference_array[0])
                {
					OstTraceFiltInst0(TRACE_FLOW, "process::DSP\n");
					//printf("\n allocated:::RESOURCE_MMDSP");
					get_hw_resource_requested = 1;
#ifdef __MPEG4DEC_ARM_NMF
					iSendLogEvent.event_informResourceStatus(0,0);
#else
					iResource.allocResource(RESOURCE_MMDSP, &iInformResourceStatus);
#endif
				}
				else  //README::vop not coded in case of no-deblocking
				{
					OstTraceFiltInst0(TRACE_FLOW, "process::VPP for VNC\n");
					//printf("\n allocated:::RESOURCE_VPP vopCoded=%d ",ps_ddep_vdc_mpeg4_header_desc->vopCoded);
					//PRINT_VAR(ps_ddep_vdc_mpeg4_header_desc->vopCoded);
					get_hw_resource_requested = 1;
#ifdef __MPEG4DEC_ARM_NMF
					iSendLogEvent.event_informResourceStatus(0,0);
#else
					iResource.allocResource(RESOURCE_VPP, &iInformResourceStatus);
#endif

				}
			   //printf("\n iResource.allocResource exiting");
		}
     }
#ifndef __MPEG4DEC_ARM_NMF
	//NmfPrint0(2,"\nPX");
#endif
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Ddep: process()");
}

void METH(CopyComplete)(t_uint32 bytes_written,t_uint16 InBuffCnt)
{
	t_uint32 i=0;
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec Ddep: CopyComplete()");
	for(i=0;i<InBuffCnt;i++)
	{
        InternalbufIn->FLAGS=bufIn[i]->FLAGS;
		bufIn[i]->LENGTH=0;
		ReleaseBuffer(0,bufIn[i]);
		bufIn[i]=0;
	}
	//PRINT_VAR(InternalbufIn->FLAGS);
	BytesWrittenInInternalBuffer=bytes_written/MULTIPLYING_FACTOR;
    CopyOperationOngoing=0;
	InternalbufIn->LENGTH=BytesWrittenInInternalBuffer;
	DataInBuf=1;
	//PRINT_VAR(BytesWrittenInInternalBuffer);
	//PRINT_VAR(InternalbufIn->LENGTH);
	//PRINT_VAR(InBuffCnt);
	PRINT_VAR(bytes_written);
	iSendLogEvent.eventProcess();

//+ER 354962
#ifdef __MPEG4DEC_ARM_NMF
	iCommandAck.pendingCommandAck();
#endif
//-ER 354962
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Ddep: CopyComplete()");
}

#ifndef __MPEG4DEC_ARM_NMF
#if 0
PUT_PRAGMA
void METH(setBufferForI)(t_uint32 phyAddr)
{
	OstTraceInt0(TRACE_FLOW, "=> MPEG4Dec Ddep: setBufferForI() ENTRY");//buffer_i_frame[0] (Physical) = 0x%x, buffer_i_frame[1] (Logical) = 0x%x", buffer_i_frame[0], buffer_i_frame[1]);

	buffer_i_frame_physical = phyAddr;

	OstTraceInt1(TRACE_FLOW, "setBufferForI(): buffer_i_frame_physical = 0x%x", buffer_i_frame_physical);

	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Ddep: setBufferForI()");
}

PUT_PRAGMA
void METH(sendImmediateIFlagToMpc)(t_uint8 immediate_i_flag)
{
	OstTraceInt1(TRACE_FLOW, "=> MPEG4Dec Ddep: sendImmediateIFlagToMpc(): immediate_i_flag = %d", immediate_i_flag);

	immediateIFlag = immediate_i_flag;

	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Ddep: sendImmediateIFlagToMpc()");
}
#endif
#endif


#ifdef __MPEG4DEC_ARM_NMF
void mpeg4dec_arm_nmf_ddep::processActual()
#else
static void processActual(Component *this)
#endif
{

    t_uint32 i;
	OstTraceInt0(TRACE_FLOW, "=> Mpeg4dec ddep: processActual()");
	//printf("\n DDEP_ARM::processActual enter ");
#ifndef __MPEG4DEC_ARM_NMF
	//NmfPrint0(2,"\nPAE");
#endif
   for(i=0;i<ps_ddep_vdc_mpeg4_header_desc->InBuffCnt;i++)
   {
	   if(bufIn[i]->FLAGS & BUFFERFLAG_EOS)
	   {
		   free_references = 1;
		   eos_flag_set = 1;
		   PRINT_VAR(bufIn[i]->FLAGS);
		   PRINT_VAR(free_references);
	   }
	}
#ifndef __MPEG4DEC_ARM_NMF
	if(immediateIFlag & currentCodingType == 0)
	{
		free_references = 1;	//so that the dest_buffer for current frame is buffout_reference
		firstFramedecoded = 0; //so that this bufOut itself is the buffout_reference above
		eos_flag_set = 0; //required?
	}
#endif
   //PRINT_VAR(bufIn[0]);
   //PRINT_VAR(bufOut);
   if(DataInBuf)
   {
	   OstTraceInt0(TRACE_FLOW, "DataInBuf\n");
	   //chnage also for programming buffer
	   //printf("\nInternalbufIn");
	   //InternalbufIn->ADDRESS=(t_uint8*)reference_array[8];
	   //PRINT_VAR(InternalbufIn->ADDRESS);
	   //PRINT_VAR(InternalbufIn);
	   //PRINT_VAR(InternalbufIn->ALLOCLENGTH);
	   InternalbufIn->ALLOCLENGTH=bufOut->ALLOCLENGTH;
       mp4d_fill_link_structure(InternalbufIn);
	   mp4d_mark_end_link_structure();
   }
   else
   {
	   for(i=0;i<ps_ddep_vdc_mpeg4_header_desc->InBuffCnt;i++)
	   mp4d_fill_link_structure(bufIn[i]);
		mp4d_mark_end_link_structure();
   }
   if(Error_Reporting & 1)
   {
	   for(i =0;i<225;i++)
		   ps_ddep_vdc_mpeg4_param_desc->s_out_parameters.error_map[i]=0;
   }
   if(Error_Reporting & 2)
   {
	   for(i =0;i<8;i++)
	   {
		   ps_ddep_vdc_mpeg4_param_desc->s_out_parameters.slice_loss_first_mb[i]=0;
		   ps_ddep_vdc_mpeg4_param_desc->s_out_parameters.slice_loss_mb_num[i]=0;
	   }
   }
   if(!reference_array[0])
   {
	    PRINT_VAR(currentCodingType);
		switch(currentCodingType)
		{
			case 0:
				//+ER344944
				if(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)])
				{
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)]->ADDRESS)));
					//PRINT_VAR(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)]);
				}
				else  
				{   //internal buffer used as reference for I frame
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[4])));
					//PRINT_VAR(bufOut);
				}
				//-ER344944
				ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(0));
				break;
			case 1:
		    case 3:
				if(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)])
				{
				#ifndef __MPEG4DEC_ARM_NMF
						if(((t_uint32)(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)]) == immediate_i_frame_bufOut) && (doNotReleaseNextI))
						{
							OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, addr_fwd_ref_buffer for P frame will be the internal buffer", __LINE__);
							ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(buffer_i_frame_physical)));
						}
						else
						{
							OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, addr_fwd_ref_buffer for P frame will be output buffer", __LINE__);
				#endif
							ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)]->ADDRESS)));
				#ifndef __MPEG4DEC_ARM_NMF
						}
				#endif
					//PRINT_VAR(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)]);
				}
				else  // test this code: ask for such stream
				{  //in case first  frame is P frame
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
					//PRINT_VAR(bufOut);
				}
				ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(0));
				break;
		case 2:
				if(dpb_buffer.buffer_copied)
				{
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[4]));
				}
				else
				{
					if(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
					{
					#ifndef __MPEG4DEC_ARM_NMF
						//>OstTraceInt2(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index] = 0x%x", __LINE__, (((t_uint32)dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])&0xffff));
						//>OstTraceInt2(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]>>16 = 0x%x", __LINE__, (((t_uint32)dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]>>16)&0xffff));
					
						OstTraceFiltInst2(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, doNotReleaseNextI = %d", __LINE__, doNotReleaseNextI);
					
						//>OstTraceInt2(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, buffer_i_frame_physical = 0x%x", __LINE__, (buffer_i_frame_physical&0xffff));
						//>OstTraceInt2(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, buffer_i_frame_physical>>16 = 0x%x", __LINE__, ((buffer_i_frame_physical>>16)&0xffff));

						if(((t_uint32)(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]) == immediate_i_frame_bufOut) && (doNotReleaseNextI))
						{
							OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, addr_fwd_ref_buffer for B frame will be the internal buffer", __LINE__);
							ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(buffer_i_frame_physical)));
						}
						else
						{
							OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, addr_fwd_ref_buffer for B frame will be some output buffer", __LINE__);
					#endif
							ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->ADDRESS)));
					#ifndef __MPEG4DEC_ARM_NMF
						}
					#endif
						//PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
					}
					else if(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)])

					{
						//Condition is hit when decode order of frames is IBP, (display order: BIP), so prev frame should go as backward reference.
						//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)]->ADDRESS)));

						//PRINT_VAR(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)]);

					}
					else
					{  //in case first  frame is P frame
						ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
						//PRINT_VAR(bufOut);
					}
				}
                if(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)])
				{
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)]->ADDRESS)));
					 //PRINT_VAR(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)]);
				}
				else if(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index])
				{
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]->ADDRESS)));
					//PRINT_VAR(dpb_buffer.fw_reference_buffer[dpb_buffer.curr_fw_buffer_index]);
				}
				else
				{
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
					//PRINT_VAR(bufOut);
				}
                break;
			default:
			   proxy.eventHandler(OMX_EventError, (t_uint32)0x80001020UL,0);  //OMX_ErrorFormatNotDetected
		}
		//printf("\nDDEP_ARM::processActual  addr_fwd_ref_buffer=%x",ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer);
		//printf("\nDDEP_ARM::processActual  addr_bwd_ref_buffer  =%x",ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer);
       // PRINT_VAR((SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
		//copy input flags to output flags fo DECODE ONLY buffers
		if(DataInBuf)
			bufOut->FLAGS=InternalbufIn->FLAGS;
		else
			bufOut->FLAGS=bufIn[ps_ddep_vdc_mpeg4_header_desc->InBuffCnt-1]->FLAGS;
		ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
		//printf("\nconfigureAlgo .addr_dest_buffer=%x",ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer);
		ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(0));
		ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_motion_vector_buffer = SVP_SHAREDMEM_TO16((0));

        ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16((0));
		ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16((0));

#ifdef __MPEG4DEC_ARM_NMF
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = SVP_SHAREDMEM_TO16((s_mpeg4d_ddep_desc.bbm_desc.nLogicalAddress) | EXT_BIT);
#else
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = SVP_SHAREDMEM_TO16((s_mpeg4d_ddep_desc.bbm_desc.nPhysicalAddress) | EXT_BIT);
#endif
		if(DataInBuf)
		{
				//printf("\nInternalbufIn");
				ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_start = AHB_(SVP_SHAREDMEM_TO16(reference_array[8]));
		}
		else
			ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_start = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufIn[0]->ADDRESS)));
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.bitstream_offset = ps_ddep_vdc_mpeg4_header_desc->bitOffset;
		//printf("\DDEP_ARM::processActual  bitstream_offset       =%d",ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.bitstream_offset);
		//printf("\DDEP_ARM::processActual  addr_bitstream_start       =%x",ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_start);
        //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.bitstream_offset);
		//PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_start);
		//PRINT_VAR(bufIn->allocLen);
		//PRINT_VAR(bufIn->ADDRESS);
		//VOP parameters
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.picture_coding_type = ps_ddep_vdc_mpeg4_header_desc->pictureCodingType;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quant = ps_ddep_vdc_mpeg4_header_desc->quant;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.rounding_type = ps_ddep_vdc_mpeg4_header_desc->roundingType;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.intra_dc_vlc_thr = ps_ddep_vdc_mpeg4_header_desc->intraDcVlcThr;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_forward = ps_ddep_vdc_mpeg4_header_desc->vopFcodeForward;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.difftraj_x = ps_ddep_vdc_mpeg4_header_desc->difftraj_x;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.difftraj_y = ps_ddep_vdc_mpeg4_header_desc->difftraj_y;
		if(!ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flag_short_header)
		{
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_backward = ps_ddep_vdc_mpeg4_header_desc->vopFcodeBackward;
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modulo_time_base = ps_ddep_vdc_mpeg4_header_desc->moduloTimeBase;
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_time_increment = ps_ddep_vdc_mpeg4_header_desc->vopTimeIncrement;
		}
		frameWidth = (ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width);
		frameHeight =(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height);
		currentVopCoded = ((ps_ddep_vdc_mpeg4_header_desc->vopCoded));
	  //  PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.future_reference_vop);
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.future_reference_vop= ps_ddep_vdc_mpeg4_param_desc->s_out_frame_parameters.future_reference_vop;
       //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.future_reference_vop);
	   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.forward_time);
		//PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.backward_time);
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.forward_time= ps_ddep_vdc_mpeg4_param_desc->s_out_frame_parameters.forward_time;
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.backward_time= ps_ddep_vdc_mpeg4_param_desc->s_out_frame_parameters.backward_time;
		
		//PRINT_VAR(currentVopCoded);
        if(currentVopCoded)
		{
			OstTraceFiltInst2(TRACE_FLOW, "processActual::picture_coding_type=%X quant=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.picture_coding_type,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quant);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::quant_type=%X low_delay=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quant_type,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.low_delay);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::interlaced=%X rounding_type=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.interlaced,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.rounding_type);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::intra_dc_vlc_thr=%X vop_fcode_forward=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.intra_dc_vlc_thr,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_forward);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::vop_fcode_backward=%X frame_width=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_backward,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::frame_height=%X flag_short_header=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flag_short_header);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::modulo_time_base=%X vop_time_increment=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modulo_time_base,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_time_increment);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::vop_time_increment_resolution=%X resync_marker_disable=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_time_increment_resolution,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.resync_marker_disable);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::data_partitioned=%X reversible_vlc=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.data_partitioned,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.reversible_vlc);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::error_concealment_config=%X flag_sorenson=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.error_concealment_config,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flag_sorenson);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::source_frame_width=%X source_frame_height=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.source_frame_width,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.source_frame_height);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::output_format=%X quarter_sample=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.output_format,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quarter_sample);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::deblock_flag=%X decode_flag=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.decode_flag);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::sprite_enable=%X no_of_sprite_warping_points=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.sprite_enable,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.no_of_sprite_warping_points);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::sprite_warping_accuracy=%X sprite_brightness_change=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.sprite_warping_accuracy,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.sprite_brightness_change);
			OstTraceFiltInst2(TRACE_FLOW, "processActual::difftraj_x=%X difftraj_y=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.difftraj_x,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.difftraj_y);
		   // OstTraceInt2(TRACE_FLOW, "processActual::advanced_intra=%X deblocking_filter=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.advanced_intra,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblocking_filter);
		   // OstTraceInt2(TRACE_FLOW, "processActual::slice_structure=%X modified_quantizer=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.slice_structure,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modified_quantizer);
			iCodecAlgo.configureAlgo(ENDALGO
			   convto16bitmode(param.addr_in_frame_buffer|EXT_BIT),
			   convto16bitmode(param.addr_out_frame_buffer|EXT_BIT),
			   convto16bitmode(param.addr_internal_buffer|EXT_BIT),
			   convto16bitmode(param.addr_in_bitstream_buffer|EXT_BIT),
			   convto16bitmode(param.addr_out_bitstream_buffer|EXT_BIT),
			   convto16bitmode(param.addr_in_parameters|EXT_BIT),
			   convto16bitmode(param.addr_out_parameters|EXT_BIT),
			   convto16bitmode(param.addr_in_frame_parameters|EXT_BIT),
			   convto16bitmode(param.addr_out_frame_parameters|EXT_BIT));
#ifndef __MPEG4DEC_ARM_NMF
			   ticks_controlalgo=ITC_GET_TIMER_32_MY();
			   OstTraceInt1(TRACE_FLOW, "controlalgo ticks_controlalgo =%d  \n",ticks_controlalgo);
#else
			   decoding_complete=1;
			   deblocking_complete=1;
#endif
				OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep - processActual(): Calling iCodecAlgo.controlAlgo at line %d", __LINE__);
			   iCodecAlgo.controlAlgo(CMD_START, 0, 0 ); //decoding only  + deringing to be supported
			   OstTraceInt0(TRACE_FLOW, "controlAlgo \n ");
		}
		/*else 
		{


#ifndef __MPEG4DEC_ARM_NMF            
			    if(currentCodingType==0)  //not correct if first frame is vop not coded
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(dpb_buffer.fw_reference_buffer[((dpb_buffer.curr_fw_buffer_index+1)%2)]->address));
				get_vpp_resource = 1;
				get_hw_resource = 0;
				//PRINT_VAR((ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer));
				//PRINT_VAR((ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer));
                vpp_algo.configureAlgoFormatConv(&ivppEndAlgo,
										YUV420MB_YUV420MB,
										ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer,
										ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer,
										0,
										0,
										ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
										ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height,
										ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width,
										ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height);


				//printf("\n vpp_algo.controlAlgo entering");
				vpp_algo.controlAlgo(CMD_START, 0, 1);
				//printf("\n vpp_algo.controlAlgo exiting"); 

#else
				memcpy( (t_uint8 *)ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer,(t_uint8 *)ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer,  (ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width*ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height*1.5));

				iSendLogEvent.eventVPPEndAlgo(0,0,0);
#endif

		}*/
   }
	else  //deblocking
	{
#ifdef __MPEG4DEC_ARM_NMF
	{

		t_uint32 addr_deblocking_param_buffer_in,addr_source_buffer,addr_dest_buffer_deblock;
		OstTraceInt2(TRACE_FLOW, "deblocking software free_references =%d currentCodingType =%d \n",free_references,currentCodingType);
        switch(currentCodingType)
		{
			case 0:
                OstTraceInt1(TRACE_FLOW, " I IP_buffer_index= %d \n",dblk_dpb_buffer.IP_buffer_index);
				ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
				PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
                ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(0));
				break;
		    case 3:
			case 1:
                OstTraceInt1(TRACE_FLOW, " P IP_buffer_index= %d \n",dblk_dpb_buffer.IP_buffer_index);
                ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
				PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
               ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(0));
				break;
			case 2:
                OstTraceInt1(TRACE_FLOW, " B IP_buffer_index= %d \n",dblk_dpb_buffer.IP_buffer_index);
				ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index& 1) + 4]));
				PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
				ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
				PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
                break;
			default:
				proxy.eventHandler(OMX_EventError, (t_uint32)0x80001020UL,0);  //OMX_ErrorFormatNotDetected
		}
        if(! free_references)
		{
		   if(currentCodingType==2)
		   {
               OstTraceInt1(TRACE_FLOW, " B_buffer_index= %d \n",dblk_dpb_buffer.B_buffer_index);
			   		//copy input flags to decoded output flags
			   array_flags[(dblk_dpb_buffer.B_buffer_index & 1) + 2]=bufIn[ps_ddep_vdc_mpeg4_header_desc->InBuffCnt-1]->FLAGS;
			   ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 6]));
			   ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
			   //addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
               dblk_dpb_buffer.B_buffer_index = (dblk_dpb_buffer.B_buffer_index + 1 )& 1;
			   OstTraceInt1(TRACE_FLOW, " B_buffer_index= %d \n",dblk_dpb_buffer.B_buffer_index);
		   }
		   else
		   {
               OstTraceInt1(TRACE_FLOW, " IP_buffer_index= %d \n",dblk_dpb_buffer.IP_buffer_index);
			   //copy input flags to decoded output flags
			   array_flags[(dblk_dpb_buffer.IP_buffer_index & 1)]=bufIn[ps_ddep_vdc_mpeg4_header_desc->InBuffCnt-1]->FLAGS;
			   ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
			   ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
			   //addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
               dblk_dpb_buffer.IP_buffer_index = (dblk_dpb_buffer.IP_buffer_index + 1 )& 1;
			   OstTraceInt1(TRACE_FLOW, " IP_buffer_index= %d \n",dblk_dpb_buffer.IP_buffer_index);
		   }
	    }
		else  //to be changed
		{
			  if(currentCodingType==2)
			  {
				  OstTraceInt1(TRACE_FLOW, " B_buffer_index= %d \n",dblk_dpb_buffer.B_buffer_index);
                  ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
				  //addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
				  dblk_dpb_buffer.B_buffer_index = (dblk_dpb_buffer.B_buffer_index + 1 )& 1;
				  OstTraceInt1(TRACE_FLOW, " B_buffer_index= %d \n",dblk_dpb_buffer.B_buffer_index);
              }
			  else
			  {
				  OstTraceInt1(TRACE_FLOW, " IP_buffer_index= %d \n",dblk_dpb_buffer.IP_buffer_index);
                  //+ Er 337787
				  if(firstFramedecoded) 
				  {
					  ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
					  //addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
                  }
				  dblk_dpb_buffer.IP_buffer_index = (dblk_dpb_buffer.IP_buffer_index + 1 )& 1;
              }
			 if(!firstFramedecoded) 
			 {
				 dblk_dpb_buffer.buffout_reference=bufOut;
			 }
			 //- Er 337787
			 		//copy input flags to output flags
			 dblk_dpb_buffer.buffout_reference->FLAGS=bufIn[ps_ddep_vdc_mpeg4_header_desc->InBuffCnt-1]->FLAGS;
			 ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer=AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(dblk_dpb_buffer.buffout_reference->ADDRESS)));
		}
		OstTraceInt2(TRACE_FLOW, "addr_dest_buffer = %X addr_deblocking_param_buffer =%X\n",ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer,ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer);
        
		ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_motion_vector_buffer = SVP_SHAREDMEM_TO16((0));
		
		//if(firstFramedecoded)
			//ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag=0x1;
		OstTraceInt2(TRACE_FLOW, " deblock_flag= %d Last_vop_not_coded =%d \n",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag,dblk_dpb_buffer.Last_vop_not_coded);
		if(!firstFramedecoded)
		{
			addr_source_buffer=0;//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16((0));
			addr_deblocking_param_buffer_in=0;//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16((0));
			addr_dest_buffer_deblock=0;//ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer_deblock = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(0));
		}
		else if(dblk_dpb_buffer.Last_vop_not_coded==0)
		{
			OstTraceInt2(TRACE_FLOW, " currentCodingType= %d last_coding_type =%d \n",currentCodingType,dblk_dpb_buffer.last_coding_type);
			if(dblk_dpb_buffer.last_coding_type==2)
			{	if(currentCodingType==2)
				{
				    //copy decoded output flags to output flags
					array_flags[(dblk_dpb_buffer.B_buffer_index & 1) + 2]=bufOut->FLAGS;
					//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 6]));
					//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
					addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 6]));
					addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
                }
				else
				{
					//copy decoded output flags to output flags
					array_flags[((dblk_dpb_buffer.B_buffer_index + 1 ) & 1) + 2]=bufOut->FLAGS;
					//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.B_buffer_index + 1 ) & 1) + 6]));
					//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.B_buffer_index + 1 ) & 1) + 2]));
					addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.B_buffer_index + 1 ) & 1) + 6]));
					addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.B_buffer_index + 1 ) & 1) + 2]));
                }
			}
			else
			{
				if(currentCodingType==2)
				{
					//copy decoded output flags to output flags
					array_flags[((dblk_dpb_buffer.IP_buffer_index + 1) & 1)]=bufOut->FLAGS;
					//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1) & 1) + 4]));
					//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index + 1) & 1]));
					addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1) & 1) + 4]));
					addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index + 1) & 1]));
                }
				else
				{
					//copy decoded output flags to output flags
					array_flags[(dblk_dpb_buffer.IP_buffer_index & 1)]=bufOut->FLAGS;
					//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
					//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
                    addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
					addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
                }
			}
			//ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer_deblock = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
			addr_dest_buffer_deblock = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
        }
		else  //if previous was vop not coded
		{
			dblk_dpb_buffer.Last_vop_not_coded=0;
			//copy decoded output flags to output flags
			array_flags[(dblk_dpb_buffer.IP_buffer_index & 1)]=bufOut->FLAGS;
			//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
			//ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
            //ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer_deblock = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
			addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
			addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
            addr_dest_buffer_deblock = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
        }
		OstTraceInt2(TRACE_FLOW, "addr_source_buffer = %X addr_deblocking_param_buffer_in =%X\n",addr_source_buffer,addr_deblocking_param_buffer_in);
		OstTraceInt2(TRACE_FLOW, "Last_vop_not_coded = %d addr_dest_buffer_deblock =%X\n",dblk_dpb_buffer.Last_vop_not_coded,addr_dest_buffer_deblock);
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = SVP_SHAREDMEM_TO16((s_mpeg4d_ddep_desc.bbm_desc.nLogicalAddress) | EXT_BIT);

		if(DataInBuf)
		{
			ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_start = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[8])));
		}
		else
		{
			ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_start = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufIn[0]->ADDRESS)));
		}

		ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.bitstream_offset = ps_ddep_vdc_mpeg4_header_desc->bitOffset;
		
		OstTraceInt2(TRACE_FLOW, "addr_bitstream_start = %X bitstream_offset =%X\n",ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_start,ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.bitstream_offset);

		//VOP parameters
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.picture_coding_type = ps_ddep_vdc_mpeg4_header_desc->pictureCodingType;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quant = ps_ddep_vdc_mpeg4_header_desc->quant;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.rounding_type = ps_ddep_vdc_mpeg4_header_desc->roundingType;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.intra_dc_vlc_thr = ps_ddep_vdc_mpeg4_header_desc->intraDcVlcThr;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_forward = ps_ddep_vdc_mpeg4_header_desc->vopFcodeForward;
		
		if(!ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flag_short_header)
		{
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_backward = ps_ddep_vdc_mpeg4_header_desc->vopFcodeBackward;
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modulo_time_base = ps_ddep_vdc_mpeg4_header_desc->moduloTimeBase;
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_time_increment = ps_ddep_vdc_mpeg4_header_desc->vopTimeIncrement;
		}
		frameWidth = (ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width);
		frameHeight =(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height);
		currentVopCoded = ((ps_ddep_vdc_mpeg4_header_desc->vopCoded));
        ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.future_reference_vop= ps_ddep_vdc_mpeg4_param_desc->s_out_frame_parameters.future_reference_vop;

		ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.forward_time= ps_ddep_vdc_mpeg4_param_desc->s_out_frame_parameters.forward_time;
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.backward_time= ps_ddep_vdc_mpeg4_param_desc->s_out_frame_parameters.backward_time;
		//for h263 profile 3
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.advanced_intra = ps_ddep_vdc_mpeg4_header_desc->advanced_intra;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblocking_filter = ps_ddep_vdc_mpeg4_header_desc->deblocking_filter;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.slice_structure = ps_ddep_vdc_mpeg4_header_desc->slice_structure;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modified_quantizer = ps_ddep_vdc_mpeg4_header_desc->modified_quantizer;
		OstTraceInt1(TRACE_FLOW, " currentVopCoded= %d",currentVopCoded);
		if(currentVopCoded)
		{
		    OstTraceInt2(TRACE_FLOW, "processActual::picture_coding_type=%X quant=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.picture_coding_type,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quant);
			OstTraceInt2(TRACE_FLOW, "processActual::quant_type=%X low_delay=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quant_type,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.low_delay);
			OstTraceInt2(TRACE_FLOW, "processActual::interlaced=%X rounding_type=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.interlaced,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.rounding_type);
			OstTraceInt2(TRACE_FLOW, "processActual::intra_dc_vlc_thr=%X vop_fcode_forward=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.intra_dc_vlc_thr,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_forward);
			OstTraceInt2(TRACE_FLOW, "processActual::vop_fcode_backward=%X frame_width=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_backward,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width);
			OstTraceInt2(TRACE_FLOW, "processActual::frame_height=%X flag_short_header=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flag_short_header);
			OstTraceInt2(TRACE_FLOW, "processActual::modulo_time_base=%X vop_time_increment=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modulo_time_base,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_time_increment);
			OstTraceInt2(TRACE_FLOW, "processActual::vop_time_increment_resolution=%X resync_marker_disable=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_time_increment_resolution,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.resync_marker_disable);
			OstTraceInt2(TRACE_FLOW, "processActual::data_partitioned=%X reversible_vlc=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.data_partitioned,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.reversible_vlc);
			OstTraceInt2(TRACE_FLOW, "processActual::error_concealment_config=%X flag_sorenson=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.error_concealment_config,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flag_sorenson);
			OstTraceInt2(TRACE_FLOW, "processActual::source_frame_width=%X source_frame_height=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.source_frame_width,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.source_frame_height);
			OstTraceInt2(TRACE_FLOW, "processActual::output_format=%X quarter_sample=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.output_format,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quarter_sample);
			OstTraceInt2(TRACE_FLOW, "processActual::deblock_flag=%X decode_flag=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.decode_flag);
			OstTraceInt2(TRACE_FLOW, "processActual::sprite_enable=%X no_of_sprite_warping_points=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.sprite_enable,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.no_of_sprite_warping_points);
			OstTraceInt2(TRACE_FLOW, "processActual::sprite_warping_accuracy=%X sprite_brightness_change=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.sprite_warping_accuracy,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.sprite_brightness_change);
			OstTraceInt2(TRACE_FLOW, "processActual::difftraj_x=%X difftraj_y=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.difftraj_x,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.difftraj_y);
		    OstTraceInt2(TRACE_FLOW, "processActual::advanced_intra=%X deblocking_filter=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.advanced_intra,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblocking_filter);
		    OstTraceInt2(TRACE_FLOW, "processActual::slice_structure=%X modified_quantizer=%X \n ",ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.slice_structure,ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modified_quantizer);
		

			iCodecAlgo.configureAlgo(convto16bitmode(param.addr_in_frame_buffer|EXT_BIT),
			convto16bitmode(param.addr_out_frame_buffer|EXT_BIT),
			convto16bitmode(param.addr_internal_buffer|EXT_BIT),
			convto16bitmode(param.addr_in_bitstream_buffer|EXT_BIT),
			convto16bitmode(param.addr_out_bitstream_buffer|EXT_BIT),
			convto16bitmode(param.addr_in_parameters|EXT_BIT),
			convto16bitmode(param.addr_out_parameters|EXT_BIT),
			convto16bitmode(param.addr_in_frame_parameters|EXT_BIT),
			convto16bitmode(param.addr_out_frame_parameters|EXT_BIT));

			OstTraceInt0(TRACE_FLOW, "controlalgo iCodecAlgo\n");
			if(frameWidth > 352 || frameHeight > 288)  //recommended by firmware
			{
				iCodecAlgo.controlAlgo(CMD_START, 0,3);//iCodecAlgo.controlAlgo(CMD_START, 0,1);
			}
			else
			{
				iCodecAlgo.controlAlgo(CMD_START, 0,3);//iCodecAlgo.controlAlgo(CMD_START, 0,2);
			}
			if(firstFramedecoded) 
			{
                    //do copy if required
                    if(((frameWidth % 32) != 0) && ((frameHeight % 32) != 0))
                    {   t_uint32 temp,align_temp ;
                        temp=    (((frameWidth * frameHeight * 5)/4));
                        align_temp = (temp + 127) & (~0x7f)  ;
                        OstTraceInt2(TRACE_FLOW, "COPY Y + CB temp = %d align_temp=%d\n",temp,align_temp);
                        memcpy((t_uint8 *)reference_array[8],(t_uint8 *)(addr_source_buffer),temp);
                        OstTraceInt2(TRACE_FLOW, "start address =%X data copied=%d\n",addr_source_buffer,temp);
                        OstTraceInt1(TRACE_FLOW, "COPY CR data copied =%d \n",((frameWidth * frameHeight)/4));
                        memcpy((t_uint8 *)(reference_array[8] + align_temp),(t_uint8 *)(addr_source_buffer + temp),((frameWidth * frameHeight)/4));
                        OstTraceInt2(TRACE_FLOW, "copied to  =%X copied from=%X\n",(reference_array[8] + align_temp),(addr_source_buffer + temp));
                        addr_source_buffer =  reference_array[8];
                    }
                    {
                         VFM_CacheClean(save_memory_context,(t_uint8*)addr_deblocking_param_buffer_in,((frameWidth/16) * frameHeight));
                         VFM_CacheClean(save_memory_context,(t_uint8*)addr_source_buffer,(frameWidth * (frameHeight/2) * 2 + 256));
                    }
					iAlgoDeblock.configureAlgo(frameWidth,
						 frameHeight,
						 1,//recommended by firmware
						 ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modified_quantizer,
						 (t_uint32)VFM_GetPhysical(save_memory_context, (t_uint8*)addr_deblocking_param_buffer_in),//addr_deblocking_param_buffer_in,
						 (t_uint32)VFM_GetPhysical(save_memory_context, (t_uint8*)addr_source_buffer),//addr_source_buffer,
						 (t_uint32)VFM_GetPhysical(save_memory_context, (t_uint8*)addr_dest_buffer_deblock),//addr_dest_buffer_deblock,
						 reference_array[8]);

					OstTraceInt0(TRACE_FLOW, "controlalgo iAlgoDeblock \n");

					if(frameWidth > 352 || frameHeight > 288)  //recommended by firmware
					{
						if(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblocking_filter)
						{
							//iAlgoDeblock.controlAlgo(CMD_START,0);
                            OstTraceInt0(TRACE_FLOW, "profile 3 stream,so do copy  \n");
                            memcpy((t_uint8 *)addr_dest_buffer_deblock,(t_uint8 *)(addr_source_buffer),((frameWidth/2) * frameHeight * 3));
                            deblocking_complete=1;
						}
						else
						{   
                            OstTraceInt0(TRACE_FLOW, "iAlgoDeblock.controlAlgo  \n");
                            iAlgoDeblock.controlAlgo(CMD_START,1);
						}
						
					}
					else
					{
						if(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblocking_filter)
						{
							//iAlgoDeblock.controlAlgo(CMD_START,0);
                            OstTraceInt0(TRACE_FLOW, "profile 3 stream,so do copy  \n");
                            memcpy((t_uint8 *)addr_dest_buffer_deblock,(t_uint8 *)(addr_source_buffer),((frameWidth/2) * frameHeight * 3));
                            deblocking_complete=1;

						}
						else
						{
                            OstTraceInt0(TRACE_FLOW, "iAlgoDeblock.controlAlgo  \n");
							iAlgoDeblock.controlAlgo(CMD_START,2);
						}
                    }
			}
			else
			{
					deblocking_complete=1;
					OstTraceInt2(TRACE_FLOW, "deblocking_complete=%d firstFramedecoded =%d\n",deblocking_complete,firstFramedecoded);
			}
          }
		  else
		  {
			//do memcpy here
		    //printf("\nVOP NOT CODED, copying from previous ref frame");
		    if(currentCodingType==2)
		    {
		 	   OstTraceInt1(TRACE_FLOW, " B_buffer_index= %d",dblk_dpb_buffer.B_buffer_index);
		 	   dblk_dpb_buffer.B_buffer_index = (dblk_dpb_buffer.B_buffer_index + 1 )& 1;
		    }
		    else
		    {
		 	   OstTraceInt1(TRACE_FLOW, " IP_buffer_index= %d",dblk_dpb_buffer.IP_buffer_index);
		 	   dblk_dpb_buffer.IP_buffer_index = (dblk_dpb_buffer.IP_buffer_index + 1 )& 1;
		    }
			OstTraceInt1(TRACE_FLOW, " IP_buffer_index= %d",dblk_dpb_buffer.IP_buffer_index);
			OstTraceInt1(TRACE_FLOW, " B_buffer_index= %d",dblk_dpb_buffer.B_buffer_index);

		   /* ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.decode_flag=0;
			iCodecAlgo.configureAlgo(convto16bitmode(param.addr_in_frame_buffer|EXT_BIT),
			convto16bitmode(param.addr_out_frame_buffer|EXT_BIT),
			convto16bitmode(param.addr_internal_buffer|EXT_BIT),
			convto16bitmode(param.addr_in_bitstream_buffer|EXT_BIT),
			convto16bitmode(param.addr_out_bitstream_buffer|EXT_BIT),
			convto16bitmode(param.addr_in_parameters|EXT_BIT),
			convto16bitmode(param.addr_out_parameters|EXT_BIT),
			convto16bitmode(param.addr_in_frame_parameters|EXT_BIT),
			convto16bitmode(param.addr_out_frame_parameters|EXT_BIT));
			
			OstTraceInt0(TRACE_FLOW, "controlalgo iCodecAlgo\n");
			//deblocking and deringing
			iCodecAlgo.controlAlgo(CMD_START, 0,0);*/
			
			decoding_complete=1;
            //do copy if required
            if(((frameWidth % 32) != 0) && ((frameHeight % 32) != 0))
            {   t_uint32 temp,align_temp ;
                temp=    (((frameWidth * frameHeight * 5)/4));
                align_temp = (temp + 127) & (~0x7f)  ;
                OstTraceInt0(TRACE_FLOW, "COPY Y + CB \n");
                memcpy((t_uint8 *)reference_array[8],(t_uint8 *)(addr_source_buffer),temp);
                OstTraceInt2(TRACE_FLOW, "start address =%X data copied=%d\n",addr_source_buffer,temp);
                OstTraceInt1(TRACE_FLOW, "COPY CR data copied =%d \n",((frameWidth * frameHeight)/4));
                memcpy((t_uint8 *)(reference_array[8] + align_temp),(t_uint8 *)(addr_source_buffer + temp),((frameWidth * frameHeight)/4));
                OstTraceInt2(TRACE_FLOW, "copied to  =%X copied from=%X\n",(reference_array[8] + align_temp),(addr_source_buffer + temp));
                addr_source_buffer =  reference_array[8];
            }
            {
                VFM_CacheClean(save_memory_context,(t_uint8*)addr_deblocking_param_buffer_in,((frameWidth/16) * frameHeight));
                VFM_CacheClean(save_memory_context,(t_uint8*)addr_source_buffer,(frameWidth * (frameHeight/2) * 2 + 256));
            }
			iAlgoDeblock.configureAlgo(frameWidth,
						 frameHeight,
						 2,//recommended by firmware
						 ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modified_quantizer,
						 (t_uint32)VFM_GetPhysical(save_memory_context, (t_uint8*)addr_deblocking_param_buffer_in),//addr_deblocking_param_buffer_in,
						 (t_uint32)VFM_GetPhysical(save_memory_context, (t_uint8*)addr_source_buffer),//addr_source_buffer,
						 (t_uint32)VFM_GetPhysical(save_memory_context, (t_uint8*)addr_dest_buffer_deblock),//addr_dest_buffer_deblock,
						 reference_array[8]);

			OstTraceInt0(TRACE_FLOW, "controlalgo iAlgoDeblock \n");
			iAlgoDeblock.controlAlgo(CMD_START,0);
			if(frameWidth > 352 || frameHeight > 288)  //recommended by firmware
			{
						if(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblocking_filter)
						{
							//iAlgoDeblock.controlAlgo(CMD_START,0);
                            OstTraceInt0(TRACE_FLOW, "profile 3 stream,so do copy  \n");
                            memcpy((t_uint8 *)addr_dest_buffer_deblock,(t_uint8 *)(addr_source_buffer),((frameWidth/2) * frameHeight * 3));
                            deblocking_complete=1;
                        }
						else
						{
                            OstTraceInt0(TRACE_FLOW, "iAlgoDeblock.controlAlgo  \n");
							iAlgoDeblock.controlAlgo(CMD_START,1);
						}
						
			}
			else
			{
						if(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblocking_filter)
						{
							//iAlgoDeblock.controlAlgo(CMD_START,0);
                            OstTraceInt0(TRACE_FLOW, "profile 3 stream,so do copy  \n");
                            memcpy((t_uint8 *)addr_dest_buffer_deblock,(t_uint8 *)(addr_source_buffer),((frameWidth/2) * frameHeight * 3));
                            deblocking_complete=1;
                        }
						else
						{
                            OstTraceInt0(TRACE_FLOW, "iAlgoDeblock.controlAlgo  \n");
							iAlgoDeblock.controlAlgo(CMD_START,2);
						}
            }

		  }
	}
#else
	{
		OstTraceFiltInst0(TRACE_FLOW, "deblocking enabled hardware \n");
		//	PRINT_VAR(currentCodingType);
		switch(currentCodingType)
		{
			case 0:
				//+ER344944
				{
					PRINT_VAR(dblk_dpb_buffer.IP_buffer_index);
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
				}
				//-ER344944
				ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(0));
				break;
		    case 3:
			case 1:
				{
					PRINT_VAR(dblk_dpb_buffer.IP_buffer_index);

				#ifndef __MPEG4DEC_ARM_NMF
					if(lastIWasReleased) {
						OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, addr_fwd_ref_buffer for P frame will be the internal buffer", __LINE__);
						ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(buffer_i_frame_physical));
					}
					else {
						OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, addr_fwd_ref_buffer for P frame will be reference_array[] buffer", __LINE__);
				#endif
						ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
				#ifndef __MPEG4DEC_ARM_NMF
					}
				#endif	
					
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
				}
				ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_TO24(0));
				break;
			case 2:
				{
					PRINT_VAR(dblk_dpb_buffer.IP_buffer_index);
				#ifndef __MPEG4DEC_ARM_NMF
					if(lastIWasReleased) {
						OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, addr_fwd_ref_buffer for B frame will be the internal buffer", __LINE__);
						ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(buffer_i_frame_physical));
						lastIWasReleased = 0;
					}
					else {
						OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep: processActual(): line %d, addr_fwd_ref_buffer for B frame will be reference_array[] buffer", __LINE__);
				#endif
						ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_fwd_ref_buffer=  SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index& 1) + 4]));
				#ifndef __MPEG4DEC_ARM_NMF
					}
				#endif
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
				}
				{
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_bwd_ref_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 )& 1) + 4]));
				}
                break;
			default:
				proxy.eventHandler(OMX_EventError, (t_uint32)0x80001020UL,0);  //OMX_ErrorFormatNotDetected
		}
        if(! free_references)
		{
		   if(currentCodingType==2)
		   {
			   PRINT_VAR(dblk_dpb_buffer.B_buffer_index);
			   		//copy input flags to decoded output flags
			   array_flags[(dblk_dpb_buffer.B_buffer_index & 1) + 2]=bufIn[ps_ddep_vdc_mpeg4_header_desc->InBuffCnt-1]->FLAGS;
			   ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 6]));
			   ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
			   PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 6]));
			   PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
			   dblk_dpb_buffer.B_buffer_index = (dblk_dpb_buffer.B_buffer_index + 1 )& 1;
		   }
		   else
		   {
			   PRINT_VAR(dblk_dpb_buffer.IP_buffer_index);
			   //copy input flags to decoded output flags
			   array_flags[(dblk_dpb_buffer.IP_buffer_index & 1)]=bufIn[ps_ddep_vdc_mpeg4_header_desc->InBuffCnt-1]->FLAGS;
			   ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
			   ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
			   PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
			   PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
			   dblk_dpb_buffer.IP_buffer_index = (dblk_dpb_buffer.IP_buffer_index + 1 )& 1;
		   }
	    }
		else  //to be changed
		{
			   if(currentCodingType==2)
			  {
				  PRINT_VAR(dblk_dpb_buffer.B_buffer_index);
				  ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
				  dblk_dpb_buffer.B_buffer_index = (dblk_dpb_buffer.B_buffer_index + 1 )& 1;
              }
			  else
			  {
				  PRINT_VAR(dblk_dpb_buffer.IP_buffer_index);
				  //+ Er 337787
				  if(firstFramedecoded) 
					  ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_deblocking_param_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
                  dblk_dpb_buffer.IP_buffer_index = (dblk_dpb_buffer.IP_buffer_index + 1 )& 1;
              }
			 if(!firstFramedecoded) 
			 {
			 #ifndef __MPEG4DEC_ARM_NMF
   				OstTraceInt1(TRACE_FLOW, "JD:: firstFramedecoded is 0 and buffout_reference = 0x%x", dblk_dpb_buffer.buffout_reference);
			 #endif
				if(dblk_dpb_buffer.buffout_reference == 0) {
				 dblk_dpb_buffer.buffout_reference = bufOut;
				}
			 }
			 //- Er 337787
			 		//copy input flags to output flags
			 dblk_dpb_buffer.buffout_reference->FLAGS=bufIn[ps_ddep_vdc_mpeg4_header_desc->InBuffCnt-1]->FLAGS;
			 ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer=AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(dblk_dpb_buffer.buffout_reference->ADDRESS)));
		}
        ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_motion_vector_buffer = SVP_SHAREDMEM_TO16((0));
		if(firstFramedecoded)
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag=0x1;

		if(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag == 0)
		{
			ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16((0));
			ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16((0));
			ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer_deblock = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(0));
		}
		else if(dblk_dpb_buffer.Last_vop_not_coded==0)
		{
			if(dblk_dpb_buffer.last_coding_type==2)
			{	if(currentCodingType==2)
				{
				    //copy decoded output flags to output flags
					array_flags[(dblk_dpb_buffer.B_buffer_index & 1) + 2]=bufOut->FLAGS;
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 6]));
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 6]));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.B_buffer_index & 1) + 2]));
				}
				else
				{
					//copy decoded output flags to output flags
					array_flags[((dblk_dpb_buffer.B_buffer_index + 1 ) & 1) + 2]=bufOut->FLAGS;
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.B_buffer_index + 1 ) & 1) + 6]));
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.B_buffer_index + 1 ) & 1) + 2]));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.B_buffer_index + 1) & 1) + 6]));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.B_buffer_index + 1) & 1) + 2]));
				}
			}
			else
			{
				if(currentCodingType==2)
				{
					//copy decoded output flags to output flags
					array_flags[((dblk_dpb_buffer.IP_buffer_index + 1) & 1)]=bufOut->FLAGS;
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1) & 1) + 4]));
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index + 1) & 1]));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[((dblk_dpb_buffer.IP_buffer_index + 1 ) & 1) + 4]));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index + 1 ) & 1]));
				}
				else
				{
					//copy decoded output flags to output flags
					array_flags[(dblk_dpb_buffer.IP_buffer_index & 1)]=bufOut->FLAGS;
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
					ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
                    PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
					PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
				}
			}
			ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer_deblock = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
			PRINT_VAR(bufOut);
		}
		else  //if previous was vop not coded
		{
			dblk_dpb_buffer.Last_vop_not_coded=0;
			//copy decoded output flags to output flags
			array_flags[(dblk_dpb_buffer.IP_buffer_index & 1)]=bufOut->FLAGS;
			ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
			ps_ddep_vdc_mpeg4_param_desc->s_in_frame_buffer.addr_deblocking_param_buffer_in = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
            ps_ddep_vdc_mpeg4_param_desc->s_out_fram_buffer.addr_dest_buffer_deblock = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut->ADDRESS)));
			PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[(dblk_dpb_buffer.IP_buffer_index & 1) + 4]));
			PRINT_VAR(SVP_SHAREDMEM_FROM24(reference_array[dblk_dpb_buffer.IP_buffer_index & 1]));
			PRINT_VAR(bufOut);
		}
#ifdef __MPEG4DEC_ARM_NMF
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = SVP_SHAREDMEM_TO16((s_mpeg4d_ddep_desc.bbm_desc.nLogicalAddress) | EXT_BIT);
#else
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = SVP_SHAREDMEM_TO16((s_mpeg4d_ddep_desc.bbm_desc.nPhysicalAddress) | EXT_BIT);
#endif
		if(DataInBuf)
			ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_start = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(reference_array[8])));
		else
			ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.addr_bitstream_start = AHB_(SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufIn[0]->ADDRESS)));
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.bitstream_offset = ps_ddep_vdc_mpeg4_header_desc->bitOffset;
		//PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_out_bitstream_buffer.bitstream_offset);

		//VOP parameters
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.picture_coding_type = ps_ddep_vdc_mpeg4_header_desc->pictureCodingType;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quant = ps_ddep_vdc_mpeg4_header_desc->quant;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.rounding_type = ps_ddep_vdc_mpeg4_header_desc->roundingType;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.intra_dc_vlc_thr = ps_ddep_vdc_mpeg4_header_desc->intraDcVlcThr;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_forward = ps_ddep_vdc_mpeg4_header_desc->vopFcodeForward;
		if(!ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flag_short_header)
		{
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_backward = ps_ddep_vdc_mpeg4_header_desc->vopFcodeBackward;
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modulo_time_base = ps_ddep_vdc_mpeg4_header_desc->moduloTimeBase;
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_time_increment = ps_ddep_vdc_mpeg4_header_desc->vopTimeIncrement;
		}
		frameWidth = (ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width);
		frameHeight =(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height);
		currentVopCoded = ((ps_ddep_vdc_mpeg4_header_desc->vopCoded));
		//PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.future_reference_vop);
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.future_reference_vop= ps_ddep_vdc_mpeg4_param_desc->s_out_frame_parameters.future_reference_vop;
        PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.future_reference_vop);

		ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.forward_time= ps_ddep_vdc_mpeg4_param_desc->s_out_frame_parameters.forward_time;
		ps_ddep_vdc_mpeg4_param_desc->s_in_out_frame_parameters.backward_time= ps_ddep_vdc_mpeg4_param_desc->s_out_frame_parameters.backward_time;
		//for h263 profile 3
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.advanced_intra = ps_ddep_vdc_mpeg4_header_desc->advanced_intra;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblocking_filter = ps_ddep_vdc_mpeg4_header_desc->deblocking_filter;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.slice_structure = ps_ddep_vdc_mpeg4_header_desc->slice_structure;
		ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modified_quantizer = ps_ddep_vdc_mpeg4_header_desc->modified_quantizer;
		PRINT_VAR(currentVopCoded);
		if(currentVopCoded)
		{
		   //printf("\nconfigureAlgo");
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.picture_coding_type        );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quant                      );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quant_type                 );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.low_delay                  );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.interlaced                 );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.rounding_type              );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.intra_dc_vlc_thr           );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_forward          );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_fcode_backward         );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_width                );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.frame_height               );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flag_short_header          );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.modulo_time_base           );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_time_increment         );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.vop_time_increment_resolution);
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.resync_marker_disable      );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.data_partitioned           );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.reversible_vlc             );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.error_concealment_config   );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flag_sorenson              );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.flv_version                );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.source_frame_width          );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.source_frame_height         );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.output_format               );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.quarter_sample             );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.deblock_flag                );
		   //PRINT_VAR(ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.decode_flag                 );

			iCodecAlgo.configureAlgo(ENDALGO
			convto16bitmode(param.addr_in_frame_buffer|EXT_BIT),
			convto16bitmode(param.addr_out_frame_buffer|EXT_BIT),
			convto16bitmode(param.addr_internal_buffer|EXT_BIT),
			convto16bitmode(param.addr_in_bitstream_buffer|EXT_BIT),
			convto16bitmode(param.addr_out_bitstream_buffer|EXT_BIT),
			convto16bitmode(param.addr_in_parameters|EXT_BIT),
			convto16bitmode(param.addr_out_parameters|EXT_BIT),
			convto16bitmode(param.addr_in_frame_parameters|EXT_BIT),
			convto16bitmode(param.addr_out_frame_parameters|EXT_BIT));

#ifndef __MPEG4DEC_ARM_NMF
			ticks_controlalgo=ITC_GET_TIMER_32_MY();
			OstTraceInt1(TRACE_FLOW, "controlalgo ticks_controlalgo =%d  \n",ticks_controlalgo);
#endif
			//deblocking and deringing
			OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep - processActual(): Calling iCodecAlgo.controlAlgo at line %d", __LINE__);

		   iCodecAlgo.controlAlgo(CMD_START, 0,1);
          }
		  else
		  {
			//do memcpy here
		    //printf("\nVOP NOT CODED, copying from previous ref frame");
		    if(currentCodingType==2)
		    {
		 	   PRINT_VAR(dblk_dpb_buffer.B_buffer_index);
		 	   dblk_dpb_buffer.B_buffer_index = (dblk_dpb_buffer.B_buffer_index + 1 )& 1;
		    }
		    else
		    {
		 	   PRINT_VAR(dblk_dpb_buffer.IP_buffer_index);
		 	   dblk_dpb_buffer.IP_buffer_index = (dblk_dpb_buffer.IP_buffer_index + 1 )& 1;
		    }
			ps_ddep_vdc_mpeg4_param_desc->s_in_parameters.decode_flag=0;
			iCodecAlgo.configureAlgo(ENDALGO
			convto16bitmode(param.addr_in_frame_buffer|EXT_BIT),
			convto16bitmode(param.addr_out_frame_buffer|EXT_BIT),
			convto16bitmode(param.addr_internal_buffer|EXT_BIT),
			convto16bitmode(param.addr_in_bitstream_buffer|EXT_BIT),
			convto16bitmode(param.addr_out_bitstream_buffer|EXT_BIT),
			convto16bitmode(param.addr_in_parameters|EXT_BIT),
			convto16bitmode(param.addr_out_parameters|EXT_BIT),
			convto16bitmode(param.addr_in_frame_parameters|EXT_BIT),
			convto16bitmode(param.addr_out_frame_parameters|EXT_BIT));

#ifndef __MPEG4DEC_ARM_NMF
			ticks_controlalgo=ITC_GET_TIMER_32_MY();
			   OstTraceFiltInst1(TRACE_FLOW, "controlalgo ticks_controlalgo =%d  \n",ticks_controlalgo);
#endif
			//deblocking and deringing
			OstTraceInt1(TRACE_FLOW, "MPEG4Dec Ddep - processActual(): Calling iCodecAlgo.controlAlgo at line %d", __LINE__);
			iCodecAlgo.controlAlgo(CMD_START, 0,0);

		  }
	}
#endif

	}

#ifndef __MPEG4DEC_ARM_NMF
	//NmfPrint0(2,"\nPAX");
#endif
	OstTraceInt0(TRACE_FLOW, "<= Mpeg4dec Ddep: processActual()");
}

PUT_PRAGMA
void METH(eventProcess)()
//change this
{
    OstTraceFiltInst0(TRACE_FLOW, "=> eventProcess()");
	//printf("\neventProcess entering");
	//Component_scheduleProcessEvent(&mDataDep);
	//check whether it works with this change or not:
#ifdef 	__MPEG4DEC_ARM_NMF
	scheduleProcessEvent();
#else
	Component_scheduleProcessEvent(&mDataDep);
#endif
	//printf("\neventProcess exiting");
}


PUT_PRAGMA
void METH(fsmInit)(fsmInit_t initFSM)
{
	OstTraceInt0(TRACE_FLOW, "fsmInit \n");
	//printf("\n DDEP_ARM::fsmInit enter ");
	//PRINT_VAR(initFSM.portsDisabled);
	//PRINT_VAR(initFSM.portsTunneled);
#ifdef 	__MPEG4DEC_ARM_NMF
    if (initFSM.traceInfoAddr)
    {
        setTraceInfo(initFSM.traceInfoAddr, initFSM.id1);
	}
	mPorts[0].init(InputPort,
								 false,
								 true,
								 0,
								 0,
								 INPUT_BUFFER_COUNT,
								 &inputport,
								 0,
								 initFSM.portsDisabled&1,
								 initFSM.portsTunneled&1,
								 (Component *)this
								 );

	mPorts[1].init(OutputPort,
								 false,
								 true,
								 0,
								 0,
								 OUTPUT_BUFFER_COUNT,
								 &outputport,
								 1,
								 (initFSM.portsDisabled >> 1)&1,
								 0,
								 (Component *)this
								 );
	mPorts[2].init(InputPort,
								 false,
								 true,
								 0,
								 0,
								 INPUT_BUFFER_COUNT,
								 &headerport,
								 2,
								 0,
								 (initFSM.portsTunneled)&1,
								 (Component *)this
								 );
    init(TOTAL_PORT_COUNT, mPorts, &proxy, &me, 0);

#else
//	if (initFSM.traceInfoAddr)

//    {
//    	this->mTraceInfoPtr->dspAddr = initFSM.traceInfoAddr;
//		this->mId1 = initFSM.id1;

//	}
	FSM_traceInit(&mDataDep, initFSM.traceInfoAddr, initFSM.id1);
	this = (TRACE_t *)&mDataDep;
    // Port are non supplier , fifo size is one ,
    Port_init(&mPorts[0], InputPort, false, 1, NULL, &mFifoIn, INPUT_BUFFER_COUNT, &inputport,0,(initFSM.portsDisabled & 1),(initFSM.portsTunneled&1),&mDataDep);
    Port_init(&mPorts[1], OutputPort,false, 1, NULL, &mFifoOut, OUTPUT_BUFFER_COUNT, &outputport,1,((initFSM.portsDisabled>>1)&1),((0)&1),&mDataDep);
    /* Header port */
    Port_init(&mPorts[2], InputPort, false, 1, NULL, &mFifoHeader, INPUT_BUFFER_COUNT, &headerport,2,0,(initFSM.portsTunneled&1),&mDataDep);

    mDataDep.reset      = reset;
    mDataDep.process    = process;
    mDataDep.disablePortIndication = disablePortIndication;
	mDataDep.enablePortIndication = enablePortIndication;
	mDataDep.flushPortIndication = flushPortIndication;

    Component_init(&mDataDep, TOTAL_PORT_COUNT, mPorts, &proxy);
#endif
	//printf("\n DDEP_ARM::fsmInit exit ");
}
PUT_PRAGMA
void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param){}

PUT_PRAGMA
void METH(sendCommandX)(OMX_COMMANDTYPE cmd, t_uword param)
{
	t_uint32 i;
	OstTraceInt2(TRACE_FLOW, "=> MPEG4Dec Ddep: sendCommandX: sendCommandX() cmd = %d, param = %d", cmd, param);
#ifndef __MPEG4DEC_ARM_NMF
	//NmfPrint0(2,"\nSCE");
#endif
	//README::so that garbage value is not accessed
    //printf("\n DDEP_ARM::sendCommand  enter cmd=%d param=%d \n",cmd,param);
#ifndef __MPEG4DEC_ARM_NMF
	//NmfPrint2(2,"\nSCX %d %d",cmd,param);
#endif
	OstTraceFiltInst2(TRACE_FLOW, "sendCommandX::cmd=%d param=%d",cmd,param);
    switch(cmd)
	{
		case OMX_CommandStateSet:
		{
				switch((OMX_STATETYPE)param)
				{
				case OMX_StateIdle:
					if(get_hw_resource || get_hw_resource_requested || get_vpp_resource)
					{
						 free_references = 1;
						 PRINT_VAR(free_references);
						 //printf("\n delay sendCommand for idle");
                         OstTraceFiltInst0(TRACE_FLOW, "delay sendCommand for idle \n");
					}
					else
					{
						PRINT_VAR(bufOut);
						if(bufOut)
						{
								ReleaseBuffer(1,bufOut);
								bufOut->LENGTH = SVP_SHAREDMEM_TO24(0);
								bufOut=0;

						}
						PRINT_VAR(bufIn[0]);
						if(bufheader)
						{
							for(i=0;i<ps_ddep_vdc_mpeg4_header_desc->InBuffCnt;i++)
							{
								if(bufIn[i])
								{
										ReleaseBuffer(0,bufIn[i]);
										bufIn[i]->LENGTH = SVP_SHAREDMEM_TO24(0);
										bufIn[i]=0;
		
								}
							}
						}
						PRINT_VAR(bufheader);
						if(bufheader)
						{
								ReleaseBuffer(2,bufheader);
								bufheader->LENGTH = SVP_SHAREDMEM_TO24(0);
								bufheader=0;

						}
						remove_references();
					}
#ifndef 	__MPEG4DEC_ARM_NMF
						if (compState == OMX_StateExecuting)
						{
							iSleep.allowSleep(&iInformResourceStatus);
							//printf("\n iSleep.preventSleep(&iInformResourceStatus)");
						}
#endif
						compState = (OMX_STATETYPE)param;
                        Error_Reporting=(Error_Reporting & 7);
                        break;
				case OMX_StatePause:
#ifndef 	__MPEG4DEC_ARM_NMF
						if (compState == OMX_StateExecuting)
						{
							iSleep.allowSleep(&iInformResourceStatus);
							//printf("\n iSleep.allowSleep(&iInformResourceStatus)");
						}
#endif
						compState = (OMX_STATETYPE)param;
					if((get_hw_resource || get_hw_resource_requested || get_vpp_resource))
					{
						 defer_pause = 1;
						 OstTraceFiltInst0(TRACE_FLOW, "defer_pause \n");
						 PRINT_VAR(defer_pause);
						//+ER 354962
						#ifdef __MPEG4DEC_ARM_NMF
							iCommandAck.pendingCommandAck();
						#endif
						//-ER 354962
						OstTraceInt1(TRACE_FLOW, "<= MPEG4Dec Ddep: sendCommandX() defer_pause = %d", defer_pause);
						return;
						 //printf("\n delay sendCommand for pause");
					}
						break;
				case OMX_StateExecuting:
#ifndef 	__MPEG4DEC_ARM_NMF
						if (compState != OMX_StateExecuting)
						{
							iSleep.preventSleep(&iInformResourceStatus);
							//printf("\n iSleep.preventSleep(&iInformResourceStatus)");
						}
#endif
						compState = (OMX_STATETYPE)param;
						break;
					default:
						break;//return event error to proxy
				}
		}
		break;
		case OMX_CommandFlush:
		case OMX_CommandPortDisable:
		{
			OstTraceFiltInst1(TRACE_FLOW, "process::OMX_CommandPortDisable Error_Reporting =%d  \n",Error_Reporting);
			if(param !=0)
			{
				Error_Reporting =(Error_Reporting & 7);
			}
			OstTraceFiltInst1(TRACE_FLOW, "process::OMX_CommandPortDisable Error_Reporting =%d  \n",Error_Reporting);
            PRINT_VAR(param);
            PRINT_VAR(get_hw_resource);
			PRINT_VAR(get_hw_resource_requested);
			if((cmd ==OMX_CommandPortDisable) && header_port_enabled && param ==0) //send command for port 2
			{
#ifdef 	__MPEG4DEC_ARM_NMF
				Component::sendCommand(cmd, param+2);
#else
				Component_sendCommand(&mDataDep, cmd, param+2);
#endif
			}

#ifndef __MPEG4DEC_ARM_NMF
			doNotReleaseNextI = 0;
			lastIWasReleased = 0;
#endif
            if(get_hw_resource || get_hw_resource_requested || get_vpp_resource)
			{
               // if(param !=0) //remove references only if command for port 1 or for OMX_ALL
				free_references = 1;
				OstTraceInt0(TRACE_FLOW, "free_references \n");
				PRINT_VAR(free_references);
			}
            else
			{
				if(bufOut && (param !=0))
				{
					ReleaseBuffer(1,bufOut);
					bufOut->LENGTH = SVP_SHAREDMEM_TO24(0);
					bufOut=0;

				}
				if(param !=1)
				{
					if(bufheader)
					{
						for(i=0;i<ps_ddep_vdc_mpeg4_header_desc->InBuffCnt;i++)
						{
							if(bufIn[i])
							{
								ReleaseBuffer(0,bufIn[i]);
								bufIn[i]->LENGTH = SVP_SHAREDMEM_TO24(0);
								bufIn[i]=0;
							}
						}
					}

				}

				if(bufheader && (param !=1))
				{
					ReleaseBuffer(2,bufheader);
					bufheader->LENGTH = SVP_SHAREDMEM_TO24(0);
					bufheader=0;

				}
				/// remove all the references
				 //if(param !=0)
					 remove_references();
			}
#ifndef 	__MPEG4DEC_ARM_NMF
			while(Port_queuedBufferCount(&mPorts[2]) && (param ==0)) 
			{
				   ReleaseBuffer(2,Port_dequeueBuffer(&mPorts[2]));
			}
#else
			while((mPorts[2].queuedBufferCount()) && (param ==0)) 
			{
				   ReleaseBuffer(2,mPorts[2].dequeueBuffer());
			}
#endif
		}
		break;
		case OMX_CommandPortEnable:
		if((!header_port_enabled) && param ==0) //send command for port 2
		{
#ifdef 	__MPEG4DEC_ARM_NMF
				Component::sendCommand(cmd, param+2);
#else
				Component_sendCommand(&mDataDep, cmd, param+2);
#endif
		}
        break;
		default:
		break;//return event error to proxy
	}
#ifdef 	__MPEG4DEC_ARM_NMF
	Component::sendCommand(cmd, param);
#else
	Component_sendCommand(&mDataDep, cmd, param);
#endif

//+ER 354962
#ifdef __MPEG4DEC_ARM_NMF
	iCommandAck.pendingCommandAck();
#endif
//-ER 354962
	OstTraceInt0(TRACE_FLOW, "<= MPEG4Dec Ddep: sendCommandX()");
}

#ifdef NONE
#define SHAREDMEM
#endif



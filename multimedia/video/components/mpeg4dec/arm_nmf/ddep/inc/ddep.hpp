/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _DDEP_HPP_
#define _DDEP_HPP_
#include "vfm_vdc_mpeg4.idt"
#include <mpeg4dec_arm_mpc.h>
#include "stdio.h"
#ifndef ENDIANESS_CONVERSION
#define ENDIANESS_CONVERSION(number) ((t_uint32)(number)>>16 | ((t_uint32)(number) & 0xffff)<<16)
#endif
typedef struct
{
	OMX_BUFFERHEADERTYPE* fw_reference_buffer[2];
	OMX_BUFFERHEADERTYPE* bw_reference_buffer[3];
	t_uint8 curr_fw_buffer_index;
	t_uint8 curr_bw_buffer_index;
	t_bool dpb_IP_full;
				//	whether fw_reference_buffer[2] is full or not
	t_bool dpb_full;
   				//	flag for two consecutive B frames
	t_bool buffer_copied;
				//	do copy operation in case of more than 2 consecutive B frames, where to set it false .not yet clear??
	t_uint8 last_coding_type;			//	for storing last_coding type
}decoded_picture_buffer;

typedef struct
{
	OMX_BUFFERHEADERTYPE* buffout_reference;  		//  for storing buffout for first frame
	OMX_BUFFERHEADERTYPE* bw_reference_buffer;		//  for storing backward reference in case of b frame
	t_uint8 Last_vop_not_coded; 	//  for storing vop_not_coded
	t_uint8 last_coding_type;			//	for storing last_coding type
	t_uint8 B_buffer_index;
			//  index for storing B frames
	t_uint8 IP_buffer_index;
			//  index for storing I/P frames
}deblocked_picture_buffer;

class mpeg4dec_arm_nmf_ddep: public Component, public mpeg4dec_arm_nmf_ddepTemplate
{
private:
		 t_uint32 	 	 	reference_array[10]; // storing dblk param buffer and reference
		 OMX_U32 			array_flags[4];  //for copying flags in deblocking enabled case
		 t_uint8 			free_references;  //required to remove  header buffer if we get flush command before end algo has been handled and also for EOS
		 t_uint8 			eos_flag_set;
		 t_uint8 			defer_pause;
		 void *       	 	mFifoIn[INPUT_BUFFER_COUNT];
		 void *       		mFifoOut[OUTPUT_BUFFER_COUNT];
		 void *       		mFifoHeader[INPUT_BUFFER_COUNT];
		 Port         		mPorts[TOTAL_PORT_COUNT];
		 t_uint32			InputBufferArray[INPUT_BUFFER_COUNT];
		 t_uint32			CopyOperationOngoing;
		 t_uint32			BytesWrittenInInternalBuffer;
		 t_uint16     		get_hw_resource;
		 t_uint16     		header_port_enabled;
		 t_uint16     		get_hw_resource_requested;
		 t_uint16			MaxBufInput;
		 t_uint16			DataInBuf;
		 t_uint16			NoOfCopiedBuffers;
		 t_uint32	        ReadyToProgram;
		 t_uint8			decoding_complete;
		 t_uint8			deblocking_complete;
		 OMX_BUFFERHEADERTYPE_p		  	bufIn[INPUT_BUFFER_COUNT];
		 OMX_BUFFERHEADERTYPE_p			bufOut;
		 OMX_BUFFERHEADERTYPE_p			bufheader;																	// 
		 t_uint16     		currentCodingType;
		 t_uint16     		currentVopCoded;
		 t_uint16			frameWidth;
		 t_uint16			frameHeight;
		 OMX_STATETYPE 		compState;
		 t_uint32			fillThisBufferCount;  		//debugging purpose
		 t_uint32			emptyThisBufferCount;		//debugging purpose
		 t_uint32			emptyThisHeaderCount;		//debugging purpose
		 t_uint32			fillThisBufferDoneCount;	//debugging purpose
		 t_uint32			emptyThisBufferDoneCount;	//debugging purpose
		 t_uint32			emptyThisHeaderDoneCount; 	//debugging purpose
		 t_uint8 			Error_Reporting; //check whether needed or not
		 t_uint16     		get_vpp_resource;  // to be fixed
		 t_uint8      		firstFramedecoded;
		 t_uint16 			WAIT_FOR_BUFFER;
				// to be used with EOW 
		 Buffer_t	fakeBufferDesc;
		 Buffer_t	InternalBufferDesc;
		 OMX_BUFFERHEADERTYPE_p									  fakeBufIn;
		 OMX_BUFFERHEADERTYPE_p									  InternalbufIn;
		 ts_ddep_vdc_mpeg4_header_desc*   	ps_ddep_vdc_mpeg4_header_desc;
		 ts_ddep_vdc_mpeg4_param_desc*	  ps_ddep_vdc_mpeg4_param_desc;
		 decoded_picture_buffer 					  dpb_buffer;		
		 deblocked_picture_buffer 				      dblk_dpb_buffer;
		 ts_ddep_vdc_mpeg4_ddep_desc 	s_mpeg4d_ddep_desc;
		 codec_param_type 		param;
		 ts_ddep_bitstream_buf_link_and_header*	ps_ddep_bitstream_buf_link_and_header;  //why this is being used
		 t_ddep_bitstream_mgmt 	bsm;

public:
    //member functions
    mpeg4dec_arm_nmf_ddep();
    virtual ~mpeg4dec_arm_nmf_ddep();
	virtual void processEvent(void);
	virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);
	virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer);
	virtual void emptyThisHeader(t_uint32 pBuffer);
	virtual t_nmf_error construct(void);
	virtual void destroy(void) ;
	OMX_BOOL buffer_available_atoutput();
	OMX_BOOL buffer_available_atinput();
	OMX_BOOL buffer_available_atheaderport();
	virtual void event_informResourceStatus(t_uint32 status,t_uint32 resource); //should be removed
	virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
    void ReleaseBuffer(t_uint32 port_idx,OMX_BUFFERHEADERTYPE_p buf);
	virtual void mp4d_init_buffer_link(t_ahb_address physical_address, ts_ddep_buffer_descriptor *buffer_desc);
	virtual void mp4d_fill_link_structure(OMX_BUFFERHEADERTYPE_p pSrc);
	virtual void mp4d_mark_end_link_structure();
	virtual void mp4d_get_next_not_required_bitstream_buffer(OMX_BUFFERHEADERTYPE_p &bufferIn);
	virtual void mp4d_create_fake_buffer(t_ahb_address physical_address, ts_ddep_buffer_descriptor *fake_buffer_desc);
	virtual void do_flushAllPortsX();
	virtual void do_flushAllPorts();
	virtual void remove_references();
    virtual void setNeeds(t_uint16 Error_Reporting,t_uint8 BufCountActual,ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc,t_uint16 disable);
    //virtual void setConfig(t_uint16 Error_Reporting);
    //+ER425364
    virtual void setConfig(t_uint16 error_reporting, t_uint16 valueDecision, t_uint32 tempValue);
    //-ER425364
    virtual void CopyComplete(t_uint32 bytes_written,t_uint16 InBuffCnt);

	virtual void setParameter(t_uint32 deblocking_param_struct[10], t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vdc_mpeg4_param_desc,void *mpc_vfm_mem_ctxt);
    void release_resources(void);
	virtual void endAlgo(t_t1xhv_status status,t_t1xhv_decoder_info errors,t_uint32 durationInTicks,t_uint32 bitstream_size);
	//+ER344943
	virtual void endAlgoDeblock(t_t1xhv_status status,t_t1xhv_decoder_info errors,t_uint32 durationInTicks,t_uint32 bitstream_size);
	//-ER344943
	//+ER352606
	virtual void registerEvents(t_t1xhv_status status,t_t1xhv_decoder_info errors,t_uint32 durationInTicks,t_uint32 bitstream_size,t_uint8 event_type);
	//-ER352606
	virtual void eventEndAlgo(t_t1xhv_status status,t_t1xhv_decoder_info errors,t_uint32 durationInTicks,t_uint32 bitstream_size);
	virtual void eventVPPEndAlgo(t_uint32 status, t_uint32 info, t_uint32 durationInTicks);
	virtual void sendCommandX(OMX_COMMANDTYPE cmd, t_uword param);
	virtual void eventEOS(t_uint32 flag);
	virtual void reset();
	virtual void disablePortIndication(t_uint32 portIdx);
	virtual void enablePortIndication(t_uint32 portIdx);
	virtual void flushPortIndication(t_uint32 portIdx) ;
	virtual void process() ;
	virtual void processActual();
    virtual void eventProcess();
	virtual void fsmInit(fsmInit_t initFSM);
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
    void *save_memory_context;
    //virtual void stateChangeIndication(OMX_STATETYPE oldState, OMX_STATETYPE newState);
};

#endif /* PARSER_HPP_ */

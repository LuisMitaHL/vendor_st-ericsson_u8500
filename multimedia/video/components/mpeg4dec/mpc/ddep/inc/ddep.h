/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __MPC_DDEP_H
#define __MPC_DDEP_H

#include <mpeg4dec_arm_mpc.h>


/*===========================================================================*/
/*---------------------------------------------------------------------------*/
// for ignoring resource mgr and algo components
//#define DDEP_BYPASS_ALL_OTHER_DSP_COMPOENENTS

// for XTI traces
#include <trace/mpc_trace.h>
#include <trace/mpc_trace.c>

/*---------------------------------------------------------------------------*/
/*===========================================================================*/
typedef struct
{
	Buffer_p fw_reference_buffer[2];
	Buffer_p bw_reference_buffer[3];
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
	Buffer_p buffout_reference;  		//  for storing buffout for first frame
	Buffer_p bw_reference_buffer;		//  for storing backward reference in case of b frame
	t_uint8 Last_vop_not_coded; 	//  for storing vop_not_coded
	t_uint8 last_coding_type;			//	for storing last_coding type
	t_uint8 B_buffer_index;
			//  index for storing B frames
	t_uint8 IP_buffer_index;
			//  index for storing I/P frames
}deblocked_picture_buffer;
static t_uint32 ITC_GET_TIMER_32_MY(){
   t_uint16 lsb;
   lsb = HW_ITC_MY(CFG_TIC_L) & 0x00FFFF ;
   return (((t_uint32)HW_ITC_MY(CFG_TIC_H) << 16) | (t_uint32)lsb);
}
static int EXTMEM ticks_endalgo=0;
static int EXTMEM ticks_controlalgo=0;
static int EXTMEM ticks_decode=0;
static t_uint32 	 	EXTMEM 	reference_array[10]; // storing dblk param buffer and reference
static t_uint32 		EXTMEM 	array_flags[4];  //for copying flags in deblocking enabled case
volatile const t_uint16 EXTMEM 	avoid_infinte_loop_warning=0;
static t_uint8 			EXTMEM	free_references = 0;  //required to remove  header buffer if we get flush command before end algo has been handled and also for EOS
static t_uint8 			EXTMEM	eos_flag_set = 0;  
static t_uint8 			EXTMEM	defer_pause = 0;  
static void *       	EXTMEM 	mFifoIn[INPUT_BUFFER_COUNT];
static void *       	EXTMEM	mFifoOut[OUTPUT_BUFFER_COUNT];
static void *       	EXTMEM	mFifoHeader[INPUT_BUFFER_COUNT];
static Port         	EXTMEM	mPorts[TOTAL_PORT_COUNT];
static t_uint32			EXTMEM  InputBufferArray[INPUT_BUFFER_COUNT];
static  t_uint32		EXTMEM   CopyOperationOngoing;
static   t_uint32		EXTMEM	BytesWrittenInInternalBuffer;
static   t_uint32		EXTMEM	ReadyToProgram;
static Component    	EXTMEM	mDataDep;
static t_uint16     	EXTMEM	get_hw_resource = 0;
static t_uint16     	EXTMEM	header_port_enabled = 1;
static t_uint16     	EXTMEM	get_hw_resource_requested = 0;
static t_uint16			EXTMEM  MaxBufInput;
static t_uint16			EXTMEM	DataInBuf;
static t_uint16			EXTMEM	NoOfCopiedBuffers;
static Buffer_p			EXTMEM  bufIn[INPUT_BUFFER_COUNT];
static Buffer_p			EXTMEM	bufOut;
static Buffer_p			EXTMEM	bufheader;																	// 
static t_uint16     	EXTMEM	currentCodingType;
static t_uint16     	EXTMEM	currentVopCoded;
static t_uint16			EXTMEM	frameWidth;
static t_uint16			EXTMEM	frameHeight;
static t_uint8 			EXTMEM	immediateIFlag = 0;
static t_uint8 			EXTMEM	doNotReleaseNextI = 0;
static t_uint8			EXTMEM	lastIWasReleased = 0;
static t_uint32			EXTMEM	buffer_i_frame_physical = 0;
static t_uint32			EXTMEM	immediate_i_frame_bufOut = 0;
static OMX_STATETYPE 	EXTMEM	compState = OMX_StateIdle;
static t_uint32			EXTMEM	fillThisBufferCount = 0;  		//debugging purpose
static t_uint32			EXTMEM	emptyThisBufferCount = 0;		//debugging purpose
static t_uint32			EXTMEM	emptyThisHeaderCount = 0;		//debugging purpose
static t_uint32			EXTMEM	fillThisBufferDoneCount = 0;	//debugging purpose
static t_uint32			EXTMEM	emptyThisBufferDoneCount = 0;	//debugging purpose
static t_uint32			EXTMEM	emptyThisHeaderDoneCount = 0; 	//debugging purpose
static t_uint8 			EXTMEM	Error_Reporting    = 0; //check whether needed or not
static t_uint16     	EXTMEM	get_vpp_resource = 0;  // to be fixed
static t_uint8      	EXTMEM	firstFramedecoded = 0;
static  void            EXTMEM  *save_memory_context;
			// to be reset somewhere
static t_uint16 		EXTMEM 	WAIT_FOR_BUFFER=0;
				// to be used with EOW 
static TraceInfo_t EXTMEM traceInfo   = {0,0,0};
static TRACE_t     EXTMEM traceObject = {&traceInfo, 0};
static TRACE_t *   EXTMEM this = (TRACE_t *)&traceObject;
static Buffer_t	SHAREDMEM		 					InternalBufferDesc;
static Buffer_p									EXTMEM	InternalbufIn = (Buffer_p)&InternalBufferDesc;
static Buffer_t	SHAREDMEM		 					fakeBufferDesc;
static Buffer_p									EXTMEM	fakeBufIn = (Buffer_p)&fakeBufferDesc;
static ts_ddep_vdc_mpeg4_header_desc SHAREDMEM* EXTMEM 	ps_ddep_vdc_mpeg4_header_desc;
static ts_ddep_vdc_mpeg4_param_desc SHAREDMEM*	EXTMEM	ps_ddep_vdc_mpeg4_param_desc;
static decoded_picture_buffer 					EXTMEM	dpb_buffer;		
static deblocked_picture_buffer 				EXTMEM	dblk_dpb_buffer;

static ts_ddep_vdc_mpeg4_ddep_desc 	EXTMEM s_mpeg4d_ddep_desc;
static      codec_param_type 		EXTMEM	param;

ts_ddep_bitstream_buf_link_and_header SHAREDMEM*	EXTMEM	ps_ddep_bitstream_buf_link_and_header;  //why this is being used

static t_ddep_bitstream_mgmt EXTMEM	bsm;

#define FAIL_PRINT(x) \
{\
	FsmEvent ev;\
	static t_uint8 EXTMEM16 arr_123[]=x;\
	printf(arr_123);\
	proxy.eventHandler(OMX_EventMax, s_mpeg4d_ddep_desc.s_debug_buffer_desc.nLogicalAddress + mpc_trace_get_offset() , mpc_trace_get_log_size());\
	while(avoid_infinte_loop_warning);\
}

asm long convto16bitmode(long value) {
   mv @{value}, @{}
   L_msli @{}, #8, @{}
   asri @{}.0, #8, @{}.0
}


asm long convfrom16bitmode(long value) {
   mv @{value}, @{}
   asli @{}.0, #8, @{}.0
   L_lsri @{}, #8, @{}
}

#endif // __MPC_DDEP_H

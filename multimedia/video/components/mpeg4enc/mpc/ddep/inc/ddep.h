/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __DDEP_H_
#define __DDEP_H_

#include <trace/mpc_trace.h>
#include <trace/mpc_trace.c>

#define EXT_BIT       1



#define FAIL_PRINT(x) \
{\
	FsmEvent ev;\
	static t_uint8 EXTMEM16 arr_123[]=x;\
	printf(arr_123);\
	proxy.eventHandler(OMX_EventMax, s_mpeg4e_ddep_desc.s_debug_buffer_desc.nLogicalAddress + mpc_trace_get_offset() , mpc_trace_get_log_size());\
	while(avoid_infinte_loop_warning);\
}

volatile const t_uint16 avoid_infinte_loop_warning=1;

static t_uint32 set_flag_for_eow;
static t_uint32 handle_send_command_eow;

static void *       mFifoIn[INPUT_BUFFER_COUNT];
static void *       mFifoOut[OUTPUT_BUFFER_COUNT];

static t_uint32 mStabCropVectorX[STAB_ARRAY_SIZE];
static t_uint32 mStabCropVectorY[STAB_ARRAY_SIZE];
static t_uint32 mStabCropWindowWidth[STAB_ARRAY_SIZE];
static t_uint32 mStabCropWindowHeight[STAB_ARRAY_SIZE];

static t_uint16 stab_crop_array_start = 0;
static t_uint16 stab_crop_array_end_plus1 = 0;
static Component    mDataDep;

static t_uint32 set_flag_for_eow;
static t_uint32 handle_send_command_eow;

static t_uint8  eosReached = 0;

static t_bool EXTMEM isFWDisabled = FALSE;

static Port         mPorts[TOTAL_PORT_COUNT];

static t_uint8 control_algo_called 	= 0;
static t_uint8 pause_state_called 	= 0;

static t_uint16 	get_codec_parameter=0;
static t_uint16     get_hw_resource = 0;
static t_uint16     get_hw_resource_requested = 0;

static t_uint32 frame_number  = 0;

static Buffer_p     bufIn;
static Buffer_p     bufOut;

static t_uint32     fillThisBufferCount = 0;
static t_uint32     emptyThisBufferCount = 0;
static t_uint32     fillThisBufferDoneCount = 0;
static t_uint32     emptyThisBufferDoneCount = 0;

/* pointer to configuration structure */
ts_ddep_vec_mpeg4_param_desc SHAREDMEM *ps_ddep_vec_mpeg4_param_desc_mpc;
ts_ddep_vec_mpeg4_param_desc SHAREDMEM *ps_ddep_vec_mpeg4_param_desc_host_set;
ts_ddep_vec_mpeg4_param_desc SHAREDMEM *ps_ddep_vec_mpeg4_param_desc_tmp_set;

ts_ddep_vec_mpeg4_param_desc_dh SHAREDMEM *ps_ddep_vec_mpeg4_param_desc_dh;

static t_uint8 set_param_done;

static t_uint32 prev_bitstream_size = 0;
static t_uint16 prev_no_of_packets = 0;
static t_uint32 partial_end_algo_count=0;

static OMX_STATETYPE compState = OMX_StateIdle;

static t_uint32 param_desc_mpc_addr;

static t_uint16 last_read_conf_no =0;
static t_uint16 new_config_received =0;

static t_uint16 once = 1;

static t_sva_timestamp_value prev_pts = 0;

static t_uint16 previous_skipped_flag = 0;

static void release_resources(void);
static void ReleaseBuffer(t_uint32 port_idx,Buffer_p buf);

static void processActual(Component *this);

//> declared in common/inc
static ts_ddep_vec_mpeg4_ddep_desc s_mpeg4e_ddep_desc;

//> declared in common/inc
static      codec_param_type param;

static t_sva_ec_save current_header, previous_header;

asm long convto16bitmode(long value)
{
	mv @{value}, @{}
	L_msli @{}, #8, @{}
	asri @{}.0, #8, @{}.0
}


asm long convfrom16bitmode(long value)
{
	mv @{value}, @{}
	asli @{}.0, #8, @{}.0
	L_lsri @{}, #8, @{}
}

#endif /*_DDEP_H_*/


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_HAMAC_PIPE_GENERIC_H
#define HOST_HAMAC_PIPE_GENERIC_H

#include "host_hamac_interface.h"
#include "omx_define.h"
#include <inc/type.h>

// #define VERBOSE_HAMAC_PIPE

typedef enum {
	VOID_FRAME_NONE,
	VOID_FRAME_INPUT_FLUSH,
	VOID_FRAME_OUTPUT_FLUSH,
	VOID_FRAME_EOS_WO_FLUSH,
	VOID_FRAME_EOS_WITH_FLUSH,
	VOID_FRAME_RESET,
	VOID_FRAME_ALGO_BYPASS,
	VOID_FRAME_PIC_PARAM_CHANGE,
	VOID_FRAME_PAUSE_INFO
} t_void_frame_info;

typedef enum {
	DECODER_STATE_NONE,
	DECODER_STATE_FLUSH,
	DECODER_STATE_PICCHANGE
} t_decoder_state;

typedef struct 
{
	/* ptr to frames bumped in DPB, delayed after decoding of the frame (+1 if flush + decoding)*/	
	t_frames *bumped_frame;
} t_hamac_bumped_frames;

typedef struct 
{
	t_hamac_bumped_frames bumped_frames[2*(MAXNUMFRM+1)];
	t_uint16 bumped_frames_number; /* number of bumped frames */
	t_hamac_param param_in;
	t_bit_buffer *bit_buffer_addr;
	t_sint16 frame_pos; /* position of the frame in frame array */
	t_void_frame_info void_frame_info;
	t_hamac_pipe_specific_info hamac_pipe_specific_info;
} t_hamac_pipe_info;

typedef struct 
{
	t_hamac_bumped_frames bumped_frames[MAXNUMFRM+1];
	t_uint16 bumped_frames_number; /* number of bumped frames */
  t_void_frame_info void_frame_info;
  t_uint32          index;   // index used to guarantee that void frames are handled in the correct
                             // order w.r.t. std hamac pipe. On 8500 this is guaranteed by construction
} t_aux_hamac_pipe_info;

#ifndef AUX_HAMAC_PIPE_SIZE
#define AUX_HAMAC_PIPE_SIZE 5
#endif

typedef struct 
{
	t_hamac_pipe_info hamac_pipe_info[HAMAC_PIPE_SIZE+1];
	t_sint16 hamac_pipe_read;
	t_sint16 hamac_pipe_write;
	t_hamac_pipe_info *hamac_pipe_read_ptr;
	t_hamac_pipe_info *hamac_pipe_write_ptr;
	t_decoder_state decoder_state;
	t_uint16        pipe_is_empty;
	
	// Auxiliary Hamac Pipe used to convey void frames, in order to avoid waste of memory
	// If you don't use it (i.e. you still convey void frames through std pipe), just set AUX_HAMAC_PIPE_SIZE to 0 
	t_aux_hamac_pipe_info aux_hamac_pipe_info[AUX_HAMAC_PIPE_SIZE+1];
	t_uint16 aux_hamac_pipe_read_pos;
	t_uint16 aux_hamac_pipe_write_pos;
} t_hamac_pipe;

#endif // HOST_HAMAC_PIPE_GENERIC_H

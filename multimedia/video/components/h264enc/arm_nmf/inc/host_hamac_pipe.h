/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_HAMAC_PIPE_H
#define HOST_HAMAC_PIPE_H

/**
 * \file host_hamac_pipe.h
 * \brief hamac pipe implementation on host side
 */

#include "host_hamac_interface.h"
#include "omx_define.h"
#include "inc/type.h"        // to defnie NMF_PANIC
#ifdef __SYMBIAN32__
#define LOG_Log NMF_LOG
#else
#include <los/api/los_api.h>
#endif

// #define VERBOSE_HAMAC_PIPE

#define inline

typedef struct 
{
	/* ptr to frames bumped in DPB, delayed after decoding of the frame (+1 if flush + decoding)*/	
	t_frames *bumped_frame;
	t_uint32 nTimeStampH;
	t_uint32 nTimeStampL;
	t_uint32 nFlags;
} t_hamac_bumped_frames;

typedef struct 
{
	t_hamac_bumped_frames bumped_frames[MAXNUMFRM+1];
	t_uint16 bumped_frames_number; /* number of bumped frames */
	t_hamac_param param_in;
	t_uint16 slice_linked_list_size;
	t_bit_buffer *bit_buffer_addr;
	t_uint16 slices;
	t_block_info *b_info;
	t_uint16 *mb_slice_map;
	t_uint32 void_frame_nFlags;
} t_hamac_pipe_info;

typedef struct 
{
	t_hamac_pipe_info hamac_pipe_info[HAMAC_PIPE_SIZE+1];
	t_sint16 hamac_pipe_read;
	t_sint16 hamac_pipe_write;
	t_hamac_pipe_info *hamac_pipe_read_ptr;
	t_hamac_pipe_info *hamac_pipe_write_ptr;
} t_hamac_pipe;

#if 0
static inline void hamac_pipe_init(t_hamac_pipe *hamac_pipe) {
	hamac_pipe->hamac_pipe_read=0;
	hamac_pipe->hamac_pipe_write=0; // we start by incrementing it in reset
}

static inline t_sint32 hamac_pipe_is_full(t_hamac_pipe *hamac_pipe) {
	t_sint16 hamac_pipe_write;
	hamac_pipe_write = hamac_pipe->hamac_pipe_write+1;
	if (hamac_pipe_write == HAMAC_PIPE_SIZE+1) hamac_pipe_write=0;	
    if ((HAMAC_PIPE_SIZE==0) || 
		(hamac_pipe_write == hamac_pipe->hamac_pipe_read)) {
		return 1;
	}
	return 0;
}

static inline void hamac_pipe_next_read_pos(t_hamac_pipe *hamac_pipe) {
	t_sint16 hamac_pipe_read;
	hamac_pipe_read = hamac_pipe->hamac_pipe_read+1;
	if (hamac_pipe_read == HAMAC_PIPE_SIZE+1) hamac_pipe_read=0;
	hamac_pipe->hamac_pipe_read_ptr = &hamac_pipe->hamac_pipe_info[hamac_pipe_read];
	#ifdef VERBOSE_HAMAC_PIPE
	LOS_Log("hamac_pipe_next_read_pos %d\n", hamac_pipe_read);
	#endif
}

static inline void hamac_pipe_next_read(t_hamac_pipe *hamac_pipe) {
	hamac_pipe->hamac_pipe_read++;
	if (hamac_pipe->hamac_pipe_read == HAMAC_PIPE_SIZE+1) hamac_pipe->hamac_pipe_read=0;
	#ifdef VERBOSE_HAMAC_PIPE
	LOS_Log("hamac_pipe_next_read %d\n", hamac_pipe->hamac_pipe_read);
	#endif
}

static inline void hamac_pipe_next_write(t_hamac_pipe *hamac_pipe) {
	hamac_pipe->hamac_pipe_write++;
	if (hamac_pipe->hamac_pipe_write == HAMAC_PIPE_SIZE+1) hamac_pipe->hamac_pipe_write=0;
	hamac_pipe->hamac_pipe_write_ptr = &hamac_pipe->hamac_pipe_info[hamac_pipe->hamac_pipe_write];
	if ((HAMAC_PIPE_SIZE>0) && 
		(hamac_pipe->hamac_pipe_write == hamac_pipe->hamac_pipe_read)) {
		#ifdef VERBOSE_HAMAC_PIPE
		LOS_Log ("\nERROR: HAMAC_PIPE write full. %d\n", hamac_pipe->hamac_pipe_write);
		#endif
		NMF_PANIC("\nERROR: HAMAC_PIPE write full.\n");
	}
}

static inline void hamac_pipe_rewind_write(t_hamac_pipe *hamac_pipe) {
	hamac_pipe->hamac_pipe_write--;
	if (hamac_pipe->hamac_pipe_write == -1) hamac_pipe->hamac_pipe_write=HAMAC_PIPE_SIZE;
	#ifdef VERBOSE_HAMAC_PIPE
    LOS_Log("Rewind pipe at w%d r%d\n", hamac_pipe->hamac_pipe_write, hamac_pipe->hamac_pipe_read);
	#endif
}

static inline void hamac_pipe_reset_frame(t_hamac_pipe *hamac_pipe) {
	hamac_pipe_next_write(hamac_pipe);
	hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number = 0;
	#ifdef VERBOSE_HAMAC_PIPE
	LOS_Log("Reset pipe at w%d r%d\n", hamac_pipe->hamac_pipe_write, hamac_pipe->hamac_pipe_read);
	#endif
}

static inline void hamac_pipe_add_bumped_frame(	t_hamac_pipe *hamac_pipe, t_frames *frame, 
									t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags) {
	hamac_pipe->hamac_pipe_write_ptr->bumped_frames[
		hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number].nTimeStampH = nTimeStampH;
	hamac_pipe->hamac_pipe_write_ptr->bumped_frames[
		hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number].nTimeStampL = nTimeStampL;
	hamac_pipe->hamac_pipe_write_ptr->bumped_frames[
		hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number].nFlags = nFlags;
	hamac_pipe->hamac_pipe_write_ptr->bumped_frames[
		hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number].bumped_frame = frame;
	hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number++;
	#ifdef VERBOSE_HAMAC_PIPE
	LOS_Log("\tAdding frame %p in pipe %d\n", frame, hamac_pipe->hamac_pipe_write);
	#endif
}

static inline t_sint16 hamac_pipe_mark_eos_frame(t_hamac_pipe *hamac_pipe) {
	if (hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number)
	{
		hamac_pipe->hamac_pipe_write_ptr->bumped_frames[
			hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number-1].nFlags |= OSI_OMX_BUFFERFLAG_EOS;
		return 1;
 	}
	return 0;
}

static inline t_uint32 hamac_pipe_bumped_frames_number(t_hamac_pipe *hamac_pipe) {
	return hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number;
}

static inline t_block_info *hamac_pipe_get_write_b_info(t_hamac_pipe *hamac_pipe) {
	return hamac_pipe->hamac_pipe_write_ptr->b_info;
}

static inline t_uint16 *hamac_pipe_get_write_mb_slice_map(t_hamac_pipe *hamac_pipe) {
	return hamac_pipe->hamac_pipe_write_ptr->mb_slice_map;
}

static inline void hamac_pipe_set_write_slice_linked_list_size(t_hamac_pipe *hamac_pipe, t_uint16 slice_linked_list_size) {
	hamac_pipe->hamac_pipe_write_ptr->slice_linked_list_size = slice_linked_list_size;
}

static inline t_uint16 hamac_pipe_get_write_slice_linked_list_size(t_hamac_pipe *hamac_pipe) {
	return hamac_pipe->hamac_pipe_write_ptr->slice_linked_list_size;
}

static inline t_hamac_param *hamac_pipe_get_write_param_in(t_hamac_pipe *hamac_pipe) {
	return &hamac_pipe->hamac_pipe_write_ptr->param_in;
}

static inline t_hamac_param *hamac_pipe_get_read_param_in(t_hamac_pipe *hamac_pipe){
	return &hamac_pipe->hamac_pipe_read_ptr->param_in;
}

static inline void hamac_pipe_set_bit_buffer_addr(t_hamac_pipe *hamac_pipe, t_bit_buffer *bit_buffer_addr) {
	hamac_pipe->hamac_pipe_write_ptr->bit_buffer_addr =bit_buffer_addr ;
}

static inline t_bit_buffer *hamac_pipe_get_bit_buffer_addr(t_hamac_pipe *hamac_pipe) {
	return hamac_pipe->hamac_pipe_read_ptr->bit_buffer_addr;
}

static inline void hamac_pipe_set_slices(t_hamac_pipe *hamac_pipe, t_uint16 slices) {
	hamac_pipe->hamac_pipe_write_ptr->slices = slices;
}

static inline t_uint16 hamac_pipe_get_slices(t_hamac_pipe *hamac_pipe) {
	return hamac_pipe->hamac_pipe_read_ptr->slices;
}

static inline void hamac_pipe_set_void_frame_nFlags(t_hamac_pipe *hamac_pipe, t_uint32 void_frame_nFlags) {
	hamac_pipe->hamac_pipe_write_ptr->void_frame_nFlags = void_frame_nFlags;
}

static inline t_uint32 hamac_pipe_get_void_frame_nFlags(t_hamac_pipe *hamac_pipe) {
	return hamac_pipe->hamac_pipe_read_ptr->void_frame_nFlags;
}

// void hamac_pipe_bump_frames(t_hamac_pipe *hamac_pipe, t_input_control *input_control, t_dec_buff *p_buff, t_dec_buff *p_buff, t_seq_par *p_sp, void *ctxt);
#endif

#endif // HOST_HAMAC_PIPE_H

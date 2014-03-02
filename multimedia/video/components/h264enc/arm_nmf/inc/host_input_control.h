/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef HOST_INPUT_CONTROL_H
#define HOST_INPUT_CONTROL_H


/**
 * \file host_input_control.h
 * \brief Routines for input control. 
 * 
 */
#if VERBOSE_INPUT_CONTROL == 1

extern	t_uint32	buffct;
extern void display_list(char *s,t_bit_buffer *p);
#endif

static inline void input_control_init(t_input_control *input_control) {
#ifndef H264DEC_OPTDPB_EXTERNALLOC
	input_control->dpb_frame_available = 1; // Before Alloc, we allow nal (First decremented in Alloc)
#else
	input_control->dpb_frame_available = 0;
#endif
	// Init value is one because we want the decoder to request a NAL as son as it get  a DPB buffer
	input_control->pending_ack = 1;
	input_control->bit_buffer_list = 0;
}

static inline void input_control_inc_dpb_frame_available(t_input_control *input_control) {
	input_control->dpb_frame_available++;
#if VERBOSE_INPUT_CONTROL == 1
	LOS_Log("input_control_inc_dpb_frame_available %d\n", input_control->dpb_frame_available);
#endif
}

static inline void input_control_dec_dpb_frame_available(t_input_control *input_control) {
	if (input_control->dpb_frame_available>0) input_control->dpb_frame_available--;
#if  VERBOSE_INPUT_CONTROL == 1
	LOS_Log("input_control_dec_dpb_frame_available %d\n", input_control->dpb_frame_available);
#endif
}

#endif // HOST_INPUT_CONTROL_H



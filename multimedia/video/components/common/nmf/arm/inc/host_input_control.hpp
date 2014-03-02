/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


// This file is aimed to be included in the hpp file of the codec,
// in order to have the input_control global local to the class
public:
	t_input_control *input_control;

inline void input_control_init(t_input_control *input_control_ptr) {
	input_control = input_control_ptr;
#ifdef CODEC_INTERNALLOC
	input_control->dpb_frame_available = 1; // Before Alloc, we allow nal (First decremented in Alloc)
#else
	input_control->dpb_frame_available = 0;
#endif
	// Init value is one because we want the decoder to request a NAL as son as it get a DPB buffer
	input_control->pending_ack = 1;
	input_control->bit_buffer_list = 0;
}

inline void input_control_inc_dpb_frame_available() {
	input_control->dpb_frame_available++;
#if VERBOSE_INPUT_CONTROL == 1
	NMF_LOG("input_control_inc_dpb_frame_available %d\n", input_control->dpb_frame_available);
#endif
}

inline void input_control_dec_dpb_frame_available() {
	if (input_control->dpb_frame_available>0) input_control->dpb_frame_available--;
#if  VERBOSE_INPUT_CONTROL == 1
	NMF_LOG("input_control_dec_dpb_frame_available %d\n", input_control->dpb_frame_available);
#endif
}




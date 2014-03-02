/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_INPUT_CONTROL_H
#define HOST_INPUT_CONTROL_H

#include <arm_codec_type.idt>

typedef struct t_input_control_type
{
	t_sint16 dpb_frame_available;
	t_uint16 pending_ack;
	t_bit_buffer *bit_buffer_list;
}
t_input_control;

#endif // HOST_INPUT_CONTROL_H



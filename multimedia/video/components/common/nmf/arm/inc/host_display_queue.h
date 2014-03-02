/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef HOST_DISPLAY_QUEUE_H
#define HOST_DISPLAY_QUEUE_H


#include "types.h"
#include "host_types.h"

t_sint16 is_marked_frame(t_dec_buff *p_buff, t_dpb_info *dpb_info);
t_sint16 is_dpbused_frame(t_dec_buff *p_buff, t_frames *frame);
void display_queue_add_pics_buf_frame(t_dec_buff *p_buff, t_sint16 pics_buf_index, t_frames *frame);
void display_queue_remove_pics_buf_frame(t_dec_buff *p_buff, t_sint16 pics_buf_index);
void display_queue_remove_unreferenced_frame(t_dec_buff *p_buff, t_uint16 i);
t_sint16 display_queue_get_frame_from_free_pool(t_dec_buff *p_buff, t_uint32 size16);
void display_queue_reset_frames_array(t_dec_buff *p_buff);
t_uint16 display_queue_init_frames_array(t_dec_buff *p_buff, t_uint16 max_nb_frames, void *vfm_memory_ctxt);
void display_queue_free_frames_array(t_dec_buff *p_buff,void *vfm_memory_ctxt);

#endif

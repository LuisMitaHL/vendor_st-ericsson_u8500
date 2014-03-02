/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __fileReader_cpp
#define __fileReader_cpp
#include <t1xhv_common.idt>
#include <t1xhv_vdc.idt>
#include <vc1dec/arm_nmf/api/nmftype.idt>
#include "vc1dec/arm_nmf/inc/types.h"
#include "vc1dec/arm_nmf/inc/host_types.h"
#include "vc1dec/arm_nmf/inc/host_decoder.h"
#include "vc1dec/arm_nmf/inc/vc1d_common.h"

#include "TraceObject.h"

class vc1dec_arm_nmf_decoder: public vc1dec_arm_nmf_decoderTemplate, public TraceObject
{
public:

virtual t_nmf_error construct(void);
virtual void stop(void);
virtual void destroy(void);

virtual void init_codec(void);

// Interfaces
virtual void set_memory_ctxt(void * mem_ctxt);
virtual void set_ImmediateRelease( t_uint16 bImmediateRelease);
virtual void set_picsize(
		t_uint16 pic_width,
		t_uint16 pic_height
		);

virtual void 	frame(
	t_bit_buffer	*p_bitstream,	/**< Input buffer */
	void 		*sei, 		/**< TimeStamp */
	t_uint32 nTimeStampH, t_uint32 nTimeStampL, /**< TimeStamp */
	t_uint32 	nFlags		/**< Openmax Flags (End of Stream,...) */
	);

virtual void output_buffer(
	void	*frame, 		/**< buffer to be used as decoder output */
	t_uint32  size
	);

virtual void endCodec
	(t_t1xhv_status status, t_t1xhv_decoder_info info, t_uint32 durationInTicks);

virtual void 	input_decoder_command(t_command_type command_type);
virtual void 	output_decoder_command(t_command_type command_type);
virtual void  initTraceInfo(TraceInfo_t * ptr, unsigned int val);

// display queue
void 		display_queue_output_picture(t_frames *frame, t_picture_output picture_type, t_frameinfo *p_frameinfo);
void 		display_queue_new_output_buffer(t_dec *dec, void *ptr);
void debug_frames_info(char *s);
void 	display_queue_put(t_dec_buff *p_buff, t_dpb_info *dpb_info, t_frames *frame) ;

// DPB
t_sint16    InsertFrame (t_vc1_picture_type	picture_type_to_decode,
                          t_dec_buff *p_buff,  t_uint32 size,
                          t_sva_buffers *p_sva_buffers);
t_sint16    BumpFrame(t_vc1_picture_type	picture_type_to_decode,
                      t_dec_buff *p_buff);
void 	FlushDPB(t_dec_buff *p_buff);
void InsertDummyRefIntoDpb(t_dec_buff *p_buff,
			   t_uint16 pic_width,
			   t_uint16 pic_height);

// Hamac pipe
t_uint32 hamac_pipe_bump_frames(t_dec*dec);
t_uint32  aux_hamac_pipe_bump_frames(t_dec *dec);
void hamac_pipe_send_frame_eos(t_dec *dec);
void hamac_pipe_output_flush(t_dec *dec);
void hamac_pipe_void_frame(t_dec *dec, t_void_frame_info void_frame_info);

  void input_control_check_buffer_list(t_bit_buffer *p_b);
  void input_control_acknowledge_pending(t_bit_buffer *p_b);
  void input_control_acknowledge(t_bit_buffer *p_b, t_handshake mode);
  void input_control_free_all_buffers(t_bit_buffer *p_b);
t_uint32 pipe_is_ready();
void display_queue_remove_unreferenced_frame(t_dec_buff *p_buff, t_uint16 dpb_loc);
t_sint16 display_queue_get_frame_from_free_pool(t_dec_buff *p_buff, t_uint32 size16);
void    display_queue_reset_frames_array(t_dec_buff *p_buff);
t_uint16    display_queue_init_frames_array(t_dec_buff *p_buff, t_uint16 max_nb_frames, void *vfm_memory_ctxt);
void    display_queue_free_frames_array(t_dec_buff *p_buff,void *vfm_memory_ctxt);

	t_dec       	dec_static;
	ts_t1xhv_vdc_vc1_param_in_common * 	p_decoder_seq_param;
	void *         	vfm_memory_ctxt;

	t_uint16 picture_width;
	t_uint16 picture_height;
  	ts_sequence_parameters  	seq_params;

  	t_bool	seq_param_received;
  	t_bool	ref_frame_initialized;
  	t_bool	low_power_state;

#include "host_input_control.hpp"
#include "host_hamac_pipe.hpp"

};


#endif

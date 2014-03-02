/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef NMF_BUILD

#ifdef __H264DEC__
#include <h264dec/arm_nmf/decoder.nmf>
#endif
#ifdef __VC1DEC__
#include <vc1dec/arm_nmf/decoder.nmf>
#endif
#ifdef __MPEG2DEC__
#include <mpeg2dec/arm_nmf/decoder.nmf>
#endif

#if (!defined __H264DEC__ && !defined __VC1DEC__ && !defined __MPEG2DEC__)
#error Macro __H264DEC__ or __VC1DEC__ or __MPEG2DEC__ should be defined
#endif

#endif

#include "types.h"
#include "settings.h"
#include "host_hamac_pipe.h"
#include "host_display_queue.h"
#include "host_frameinfo.h"

// #define VERBOSE_HAMAC_PIPE

/* bumped pictures during DPB start put in hamac_pipe.bumped_frames
 * hamac_pipe_write incremented after DPB start + DPB end
 * hamac_pipe_read incremented after displaying bumped pictures
 * if ref picture used in HAMAC x is bumped in DPB start x+1 or modified in HAMAC x+1,
 * this is not a problem as HAMAC pipe is linear */
t_uint32 METH(hamac_pipe_bump_frames)(t_dec *dec) {
	int i;
	t_frames *frame;
	t_uint32 eos=0;

	t_uint16 bump_ct = hamac_pipe->hamac_pipe_read_ptr->bumped_frames_number; // nb of frames to be bumped
	#ifdef VERBOSE_HAMAC_PIPE
	NMF_LOG("Bumping frames from pipe %d\n", hamac_pipe->hamac_pipe_read);
	#endif
	for (i=0; i<bump_ct; i++) {
		frame = hamac_pipe->hamac_pipe_read_ptr->bumped_frames[i].bumped_frame;
#ifndef __VC1DEC__
		eos|= (frame->nFlags & OSI_OMX_BUFFERFLAG_EOS)?1:0;
		// Need to handshake NAL before Outputing buffers for last frame 
		if (i==bump_ct-1) { // last frame to be bumped
			if (eos)
			{	
				// set EOS flag to last frame to be bumped
				input_control->pending_ack=0;
				input_control_check_buffer_list(hamac_pipe_get_bit_buffer_addr());
				nal_ctl.nal_handshake(XOFF,hamac_pipe_get_bit_buffer_addr());
				frame->nFlags |= OSI_OMX_BUFFERFLAG_EOS;
			}
		}
		else {
			// EOS moved to last frame to be bumped
			frame->nFlags &= ~OSI_OMX_BUFFERFLAG_EOS;
		}
#endif
		display_queue_output_picture(frame, VALID_PIC, &(frame->frameinfo));
    #ifdef VERBOSE_HAMAC_PIPE
		NMF_LOG("Bumping frame 0x%x ptr 0x%x nFlags=%x\n", frame, frame->ptr[0], frame->nFlags); 
		#endif
	}
	
	return eos;
}



t_uint32 METH(aux_hamac_pipe_bump_frames)(t_dec *dec) {
	int i;
	t_frames *frame;
	t_uint32 eos=0;

	t_uint16 bump_ct = hamac_pipe->aux_hamac_pipe_info[hamac_pipe->aux_hamac_pipe_read_pos].bumped_frames_number; // nb of frames to be bumped
#ifdef VERBOSE_HAMAC_PIPE
	NMF_LOG("Bumping frames from AUX pipe %d at %d\n",bump_ct,hamac_pipe->aux_hamac_pipe_read_pos);
#endif
	for (i=0; i<bump_ct; i++) {
		frame = hamac_pipe->aux_hamac_pipe_info[hamac_pipe->aux_hamac_pipe_read_pos].bumped_frames[i].bumped_frame;
		eos|= (frame->nFlags & OSI_OMX_BUFFERFLAG_EOS)?1:0;
		// Need to handshake NAL before Outputing buffers for last frame 
		if (i==bump_ct-1) { // last frame to be bumped
			if (eos)
			{	
				// set EOS flag to last frame to be bumped
				input_control->pending_ack=0;
				input_control_check_buffer_list(0);
				nal_ctl.nal_handshake(XOFF,0);
				frame->nFlags |= OSI_OMX_BUFFERFLAG_EOS;
			}
		}
		else {
			// EOS moved to last frame to be bumped
			frame->nFlags &= ~OSI_OMX_BUFFERFLAG_EOS;
		}
		display_queue_output_picture(frame, VALID_PIC, &(frame->frameinfo));
#ifdef VERBOSE_HAMAC_PIPE
		NMF_LOG("AUX: Bumping frame 0x%x ptr 0x%x nFlags=%x\n", frame, frame->ptr[0], frame->nFlags); 
#endif
	}
	
	return eos;
}

void METH(hamac_pipe_send_frame_eos)(t_dec *dec) {
	// Need to handshake NAL before Outputing buffers for last frame     
	input_control->pending_ack=0;
    
    // Note: the old code (before fixing ER318391) was:
    //	  input_control_check_buffer_list(hamac_pipe_get_bit_buffer_addr());
	//    nal_ctl.nal_handshake(XOFF,hamac_pipe_get_bit_buffer_addr());
    // A cleaner fix could be proposed by cleaning-up hamac_pipe->hamac_pipe_read_ptr->bit_buffer_addr
    // which is returned by hamac_pipe_get_bit_buffer_addr()
	nal_ctl.nal_handshake(XOFF,0);
     
	// get a frame to signal the EOS, and add the flag EOS
	// We cannot send an EOS with a null ptr, because the proxy need to send the info outside
	// => pick a ptr, signal no data with INVALID_OUT, and send EOS
	t_frames *frame = 
		&dec->buf.frames[
			display_queue_get_frame_from_free_pool(&dec->buf, 0 /*size16*/)];
	frame->nFlags = OSI_OMX_BUFFERFLAG_EOS;
	frame->is_available = 0;
	display_queue_output_picture(frame, INVALID_PIC, &(frame->frameinfo));
}

void METH(hamac_pipe_output_flush)(t_dec *dec) {
	t_frameinfo frameinfo;
	t_sint16 frame_pos;
	// Flush command has been send to the decoder, and has travelled through the hamac pipe
	// => the hamac pipe is now empty
#ifdef VERBOSE_HAMAC_PIPE
	NMF_LOG("FLUSH command received through hamac pipe\n");
#endif

	// give back all buffers to the proxy
	while ((frame_pos = display_queue_get_frame_from_free_pool(&dec->buf, 0)) != -1) {
#ifdef VERBOSE_HAMAC_PIPE
		NMF_LOG("FLUSH : giving back invalid frame %d\n", frame_pos);
#endif
		t_frames *frame = &dec->buf.frames[frame_pos];
		frame->is_available = 0;
		// frame->nFlags = 0;
#ifdef __H264DEC__
   		set_frameinfo(&(frame->frameinfo), dec->p_active_sp, &dec->sh[0], 0, 0, 0);
#endif
		display_queue_output_picture(frame, INVALID_PIC, &(frame->frameinfo));
	}
	
	// reset the decoder : frame array (wait_to_be_free), input_control
	input_control_init(&dec->input_control);
	// InitBuffer(&dec->buf); // may be safer to do it ? then set DPB size
	display_queue_reset_frames_array(&dec->buf); // we must reset wait_to_be_free
	hamac_pipe->decoder_state = DECODER_STATE_NONE;
	
	// send flush command ack on decoder output
	// nal_ctl.nal_handshake(FLUSH_ACK, hamac_pipe_get_bit_buffer_addr());
#ifdef __VC1DEC__
	nal_ctl.nal_handshake(FLUSH_ACK, 0);
	input_control->dpb_frame_available--; /* in case of seek we need one output buffer more
	                                         in order to start */
#endif
#ifdef __H264DEC__
	nal_ctl.nal_handshake(FLUSH_ACK, 0);
	set_frameinfo(&frameinfo, dec->p_active_sp, &dec->sh[0], 0, 0, 0);
	dec->sps_active=0;
	//input_control->dpb_frame_available--; 
                               /* in case of seek we need one output buffer more   in order to start */
#endif
	display_queue_output_picture(0 /*frame*/, INVALID_PIC_FLUSH_ACK, &frameinfo);
}

void METH(hamac_pipe_void_frame)(t_dec *dec, t_void_frame_info void_frame_info) {
	switch(void_frame_info) {
		case VOID_FRAME_NONE:
			break;
		case VOID_FRAME_EOS_WITH_FLUSH:
			// EOS flag already sent with last bumped frame, 
			// thus nothing to be done
			break;
		case VOID_FRAME_EOS_WO_FLUSH:
			// we got an EOS, but we have no frame to bump anymore,
			// so we need to output a fake frame with eos flag
			hamac_pipe_send_frame_eos(dec);
			break;
		case VOID_FRAME_OUTPUT_FLUSH:
			// flush buffers on decoder output
			hamac_pipe_output_flush(dec);
			break;
		case VOID_FRAME_PAUSE_INFO:
      nal_ctl.nal_handshake(PAUSE_ACK,0);
      break;	  
		default:
			NMF_PANIC("received void frame %d, not implemented\n", void_frame_info);
			break;
	}
}


